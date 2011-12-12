/* arch/arm/mach-msm/board-thunderg-input.c
 * Copyright (C) 2009 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/types.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/gpio_event.h>
#include <linux/keyreset.h>
#include <mach/gpio.h>
#include <mach/vreg.h>
#include <mach/board.h>
#include <mach/board_lge.h>
#include <mach/rpc_server_handset.h>

#include "board-thunderg.h"
static int prox_power_set(unsigned char onoff);

/* LGE_S [ynj.kim@lge.com] 2010-05-15 : atcmd virtual device */
static unsigned short atcmd_virtual_keycode[ATCMD_VIRTUAL_KEYPAD_ROW][ATCMD_VIRTUAL_KEYPAD_COL] = {
	{KEY_1, 	KEY_8, 			KEY_Q,  	 KEY_I,		KEY_D,      	KEY_HOME,	KEY_B,          	KEY_UP},
	{KEY_2, 	KEY_9, 			KEY_W,		 KEY_O,		KEY_F,		KEY_RIGHTSHIFT, KEY_N,			KEY_DOWN},
	{KEY_3, 	KEY_0, 			KEY_E,		 KEY_P,		KEY_G,      	KEY_Z,        	KEY_M, 			KEY_UNKNOWN},
	{KEY_4, 	KEY_BACK,  		KEY_R,		 KEY_SEARCH,	KEY_H,		KEY_X,    	KEY_LEFTSHIFT,		KEY_UNKNOWN},
	{KEY_5, 	KEY_BACKSPACE, 		KEY_T,		 KEY_LEFTALT,	KEY_J,      	KEY_C,     	KEY_REPLY,		KEY_CAMERA},
	{KEY_6, 	KEY_ENTER,  		KEY_Y,  	 KEY_A,		KEY_K,		KEY_V,  	KEY_RIGHT,     		KEY_CAMERAFOCUS},
	{KEY_7, 	KEY_MENU,		KEY_U,  	 KEY_S,		KEY_L,	    	KEY_SPACE,      KEY_LEFT,     		KEY_SEND},
	{KEY_POWER, 	KEY_END,  		KEY_UNKNOWN, KEY_UNKNOWN,	KEY_UNKNOWN,	KEY_UNKNOWN,    KEY_FOLDER_MENU,	KEY_FOLDER_HOME},

};

static struct atcmd_virtual_platform_data atcmd_virtual_pdata = {
	.keypad_row = ATCMD_VIRTUAL_KEYPAD_ROW,
	.keypad_col = ATCMD_VIRTUAL_KEYPAD_COL,
	.keycode = (unsigned char *)atcmd_virtual_keycode,
};

static struct platform_device atcmd_virtual_device = {
	.name = "atcmd_virtual_kbd",
	.id = -1,
	.dev = {
		.platform_data = &atcmd_virtual_pdata,
	},
};
/* LGE_E [ynj.kim@lge.com] 2010-05-15 : atcmd virtual device */

/* head set device */
static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_device = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};

static unsigned int keypad_col_gpios[] = { 34, 35, 36 };
static unsigned int keypad_row_gpios[] = { 37, 38, 39 };

#define KEYMAP_INDEX(col, row) ((col)*ARRAY_SIZE(keypad_col_gpios) + (row))

#define KEY_FOCUS 242

static const unsigned short keypad_keymap_thunder[ARRAY_SIZE(keypad_col_gpios) * ARRAY_SIZE(keypad_row_gpios)] = {
	[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP, 
	[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN, 
	[KEYMAP_INDEX(1, 0)] = KEY_FOCUS, 
	[KEYMAP_INDEX(1, 1)] = KEY_CAMERA, 
	[KEYMAP_INDEX(1, 2)] = KEY_SEARCH,
	[KEYMAP_INDEX(2, 0)] = KEY_SEND,
	[KEYMAP_INDEX(2, 1)] = KEY_HOME,
};

int thunderg_matrix_info_wrapper(struct input_dev *input_dev,struct gpio_event_info *info, void **data, int func)
{
        int ret ;
		if (func == GPIO_EVENT_FUNC_RESUME) {
			gpio_tlmm_config(GPIO_CFG(keypad_col_gpios[0], 0,
						GPIO_INPUT, GPIO_PULL_UP,GPIO_2MA), GPIO_ENABLE);
			gpio_tlmm_config(GPIO_CFG(keypad_col_gpios[1], 0,
						GPIO_INPUT, GPIO_PULL_UP,GPIO_2MA), GPIO_ENABLE);
		}

		ret = gpio_event_matrix_func(input_dev,info, data,func);
        return ret ;
}

static int thunderg_gpio_matrix_power(
                const struct gpio_event_platform_data *pdata, bool on)
{
	/* this is dummy function to make gpio_event driver register suspend function
	 * 2010-01-29, cleaneye.kim@lge.com
	 * copy from ALOHA code
	 * 2010-04-22 younchan.kim@lge.com
	 */

	return 0;
}

static struct gpio_event_matrix_info thunder_keypad_matrix_info = {
	.info.func	= thunderg_matrix_info_wrapper,
	.keymap		= NULL,
	.output_gpios	= keypad_row_gpios,
	.input_gpios	= keypad_col_gpios,
	.noutputs	= ARRAY_SIZE(keypad_row_gpios),
	.ninputs	= ARRAY_SIZE(keypad_col_gpios),
	.settle_time.tv.nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE | GPIOKPF_PRINT_UNMAPPED_KEYS
	//.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE | GPIOKPF_PRINT_MAPPED_KEYS
};

static void __init thunder_select_keymap(void)
{
	thunder_keypad_matrix_info.keymap = keypad_keymap_thunder;

	return;
}

static struct gpio_event_info *thunder_keypad_info[] = {
	&thunder_keypad_matrix_info.info
};

static struct gpio_event_platform_data thunder_keypad_data = {
	.name		= "swift_keypad", //"thunder_keypad",
	.info		= thunder_keypad_info,
	.info_count	= ARRAY_SIZE(thunder_keypad_info),
	.power          = thunderg_gpio_matrix_power,
};

struct platform_device keypad_device_thunder= {
	.name	= GPIO_EVENT_DEV_NAME,
	.id	= -1, //0
	.dev	= {
		.platform_data	= &thunder_keypad_data,
	},
};

static struct input_dev *keypad_dev;

static int keypad_gpio_event_matrix_func(struct input_dev *input_dev, struct gpio_event_info *info, void **data, int func)
{
	int err;
	int i;

	err = gpio_event_matrix_func(input_dev, info, data, func);

	if (func == GPIO_EVENT_FUNC_INIT && !err) {
		keypad_dev = input_dev;
		for (i = 0; i < ARRAY_SIZE(keypad_virtual_keys); i++)
			set_bit(keypad_virtual_keys[i] & KEY_MAX,
				input_dev->keybit);
	} else if (func == GPIO_EVENT_FUNC_UNINIT) {
		keypad_dev = NULL;
	}

	return err;
}

struct input_dev *msm_keypad_get_input_dev(void)
{
	return keypad_dev;
}

/* keyreset platform device */
static int thunderg_reset_keys_up[] = {
	KEY_HOME,
	0
};

static struct keyreset_platform_data thunderg_reset_keys_pdata = {
	.keys_up = thunderg_reset_keys_up,
	.keys_down = {
		//KEY_BACK,
		KEY_VOLUMEDOWN,
		KEY_SEARCH,
		0
	},
};

struct platform_device thunderg_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &thunderg_reset_keys_pdata,
};

/* input platform device */
static struct platform_device *thunderg_input_devices[] __initdata = {
	&hs_device,
	&keypad_device_thunder,
	//&thunderg_reset_keys_device,
	&atcmd_virtual_device,
};

/* MCS6000 Touch */
/*static struct gpio_i2c_pin ts_i2c_pin[] = {
	[0] = {
		.sda_pin	= TS_GPIO_I2C_SDA,
		.scl_pin	= TS_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= TS_GPIO_IRQ,
	},
};

static struct i2c_gpio_platform_data ts_i2c_pdata = {
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay				= 2,
};

static struct platform_device ts_i2c_device = {
	.name	= "i2c-gpio",
	.dev.platform_data = &ts_i2c_pdata,
};

static int ts_set_vreg(unsigned char onoff)
{
	struct vreg *vreg_touch;
	int rc;

	printk("[Touch] %s() onoff:%d\n",__FUNCTION__, onoff);

	vreg_touch = vreg_get(0, "synt");

	if(IS_ERR(vreg_touch)) {
		printk("[Touch] vreg_get fail : touch\n");
		return -1;
	}

	if (onoff) {
		rc = vreg_set_level(vreg_touch, 3050);
		if (rc != 0) {
			printk("[Touch] vreg_set_level failed\n");
			return -1;
		}
		vreg_enable(vreg_touch);
	} else
		vreg_disable(vreg_touch);

	return 0;
}

static struct touch_platform_data ts_pdata = {
	.ts_x_min = TS_X_MIN,
	.ts_x_max = TS_X_MAX,
	.ts_y_min = TS_Y_MIN,
	.ts_y_max = TS_Y_MAX,
	.power 	  = ts_set_vreg,
	.irq 	  = TS_GPIO_IRQ,
	.scl      = TS_GPIO_I2C_SCL,
	.sda      = TS_GPIO_I2C_SDA,
};

static struct i2c_board_info ts_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("touch_mcs6000", TS_I2C_SLAVE_ADDR),
		.type = "touch_mcs6000",
		.platform_data = &ts_pdata,
	},
};

static void __init thunderg_init_i2c_touch(int bus_num)
{
	ts_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&ts_i2c_pdata, ts_i2c_pin[0],	&ts_i2c_bdinfo[0]);
	i2c_register_board_info(bus_num, &ts_i2c_bdinfo[0], 1);
	platform_device_register(&ts_i2c_device);
}*/

/* acceleration */
static struct gpio_i2c_pin accel_i2c_pin[] = {
	[0] = {
		.sda_pin	= ACCEL_GPIO_I2C_SDA,
		.scl_pin	= ACCEL_GPIO_I2C_SCL,
		.reset_pin	= 0,		
		.irq_pin	= ACCEL_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data accel_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device accel_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &accel_i2c_pdata,
};

static int accel_power_set(unsigned char onoff)
{
	int ret = 0;
	return ret;
}
	
static struct acceleration_platform_data accel_pdata = {
	.power		= accel_power_set,
};

static struct i2c_board_info accel_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("acceleration", ACCEL_I2C_ADDRESS),
		.type = "acceleration",
		.platform_data = &accel_pdata,
	}
};

static void __init thunderg_init_i2c_acceleration(int bus_num)
{
	accel_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&accel_i2c_pdata, accel_i2c_pin[0], &accel_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &accel_i2c_bdinfo[0], 1);
	platform_device_register(&accel_i2c_device);
}

/* ecompass */
static struct gpio_i2c_pin ecom_i2c_pin[] = {
	[0] = {
		.sda_pin	= ECOM_GPIO_I2C_SDA,
		.scl_pin	= ECOM_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ECOM_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data ecom_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device ecom_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &ecom_i2c_pdata,
};

static int ecom_power_set(unsigned char onoff)
{
	int ret = 0;
	return ret;
}

static struct ecom_platform_data ecom_pdata = {
	.pin_int        = ECOM_GPIO_INT,
	.pin_rst	= 0,
	.power          = ecom_power_set,
};

static struct i2c_board_info ecom_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("akm8973", ECOM_I2C_ADDRESS),
		.type = "akm8973",
		.platform_data = &ecom_pdata,
	}
};

static void __init thunderg_init_i2c_ecompass(int bus_num)
{
	ecom_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&ecom_i2c_pdata, ecom_i2c_pin[0], &ecom_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &ecom_i2c_bdinfo[0], 1);
	platform_device_register(&ecom_i2c_device);
}

/* common function */
void __init lge_add_input_devices(void)
{
	thunder_select_keymap();
	platform_add_devices(thunderg_input_devices, ARRAY_SIZE(thunderg_input_devices));

	//lge_add_gpio_i2c_device(thunderg_init_i2c_touch);
	lge_add_gpio_i2c_device(thunderg_init_i2c_acceleration);
	lge_add_gpio_i2c_device(thunderg_init_i2c_ecompass);
}

