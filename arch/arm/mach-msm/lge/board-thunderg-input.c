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
	{KEY_1, 		KEY_8, 				KEY_Q,  	 KEY_I,          KEY_D,      	KEY_HOME,	KEY_B,          KEY_UP},
	{KEY_2, 		KEY_9, 		  		KEY_W,		 KEY_O,       	 KEY_F,		 	KEY_RIGHTSHIFT, 	KEY_N,			KEY_DOWN},
	{KEY_3, 		KEY_0, 		  		KEY_E,		 KEY_P,          KEY_G,      	KEY_Z,        	KEY_M, 			KEY_UNKNOWN},
	{KEY_4, 		KEY_BACK,  			KEY_R,		 KEY_SEARCH,     KEY_H,			KEY_X,    		KEY_LEFTSHIFT,	KEY_UNKNOWN},
	{KEY_5, 		KEY_BACKSPACE, 		KEY_T,		 KEY_LEFTALT,    KEY_J,      	KEY_C,     		KEY_REPLY,    KEY_CAMERA},
	{KEY_6, 		KEY_ENTER,  		KEY_Y,  	 KEY_A,		     KEY_K,			KEY_V,  	    KEY_RIGHT,     	KEY_CAMERAFOCUS},
	{KEY_7, 		KEY_MENU,	KEY_U,  	 KEY_S,    		 KEY_L, 	    KEY_SPACE,      KEY_LEFT,     	KEY_SEND},
	{KEY_UNKNOWN, 	KEY_UNKNOWN,  		KEY_UNKNOWN, KEY_UNKNOWN, 	 KEY_UNKNOWN,	KEY_UNKNOWN,    KEY_FOLDER_MENU,      	KEY_FOLDER_HOME},

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

static unsigned int keypad_row_gpios[] = { 34, 35, 36 };
static unsigned int keypad_col_gpios[] = { 37, 38, 39 };

#define KEYMAP_INDEX(row, col) ((row)*ARRAY_SIZE(keypad_col_gpios) + (col))

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
	.keymap		= keypad_keymap_thunder,
	.output_gpios	= keypad_row_gpios,
	.input_gpios	= keypad_col_gpios,
	.noutputs	= ARRAY_SIZE(keypad_row_gpios),
	.ninputs	= ARRAY_SIZE(keypad_col_gpios),
	.settle_time.tv.nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv.nsec = 20 * NSEC_PER_MSEC,
	.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_PRINT_UNMAPPED_KEYS | GPIOKPF_DRIVE_INACTIVE
};

static struct gpio_event_info *thunder_keypad_info[] = {
	&thunder_keypad_matrix_info.info
};

static struct gpio_event_platform_data thunder_keypad_data = {
	.name		= "thunder_keypad",
	.info		= thunder_keypad_info,
	.info_count	= ARRAY_SIZE(thunder_keypad_info),
	.power          = thunderg_gpio_matrix_power,
};

struct platform_device keypad_device_thunder= {
	.name	= GPIO_EVENT_DEV_NAME,
	.id	= -1,
	.dev	= {
		.platform_data	= &thunder_keypad_data,
	},
};

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

/* common function */
void __init lge_add_input_devices(void)
{
	platform_add_devices(thunderg_input_devices, ARRAY_SIZE(thunderg_input_devices));
}

