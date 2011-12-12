/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without swiftn the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <mach/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <mach/board.h>
#include <mach/board_lge.h>
#include "board-swift.h"

#define GI2C_E(fmt, args...)  printk(KERN_INFO "(E)gpio-i2c[%-18s:%5d]" fmt, __func__, __LINE__, ## args)
#define GI2C_D(fmt, args...)  printk(KERN_INFO "(I)gpio-i2c[%-18s:%5d]" fmt, __func__, __LINE__, ## args)


/* I2C Bus Num */
#define I2C_BUS_NUM_ACCEL		20
#define I2C_BUS_NUM_COMPASS 	21


static struct i2c_gpio_platform_data accel_i2c_pdata = {
	.sda_pin = ACCEL_GPIO_I2C_SDA,
	.sda_is_open_drain = 0,
	.scl_pin = ACCEL_GPIO_I2C_SCL,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device swift_accel_i2c_device = {
	.name = "i2c-gpio",
	.id = I2C_BUS_NUM_ACCEL,
	.dev.platform_data = &accel_i2c_pdata,
};

static struct i2c_board_info accel_i2c_bdinfo = {
	I2C_BOARD_INFO("bma150",/* "accel_bma150",*/ ACCEL_I2C_ADDRESS),/*SMB380 ,0x38), MMA7660 0x4C original */
	.type = "bma150",/* "accel_bma150",*/
	.irq = MSM_GPIO_TO_INT(ACCEL_GPIO_INT),
};

void __init swift_init_accel(void) {
	int rc = 0;

	gpio_configure(ACCEL_GPIO_I2C_SDA, GPIOF_DRIVE_OUTPUT);
	//gpio_set_value(ACCEL_GPIO_I2C_SDA, 1);
	gpio_configure(ACCEL_GPIO_I2C_SCL, GPIOF_DRIVE_OUTPUT);
	//gpio_set_value(ACCEL_GPIO_I2C_SCL, 1);

	rc = gpio_request(ACCEL_GPIO_INT, "motion_int");
	if (rc < 0) {
		GI2C_E("150 failed to gpio %d request(ret=%d)\n", ACCEL_GPIO_INT, rc);
		return;
	}

	gpio_direction_input(ACCEL_GPIO_INT);

	
	/*temp3*/printk("diyu1: Function : %s \n", __FUNCTION__);
	rc = i2c_register_board_info(I2C_BUS_NUM_ACCEL, &accel_i2c_bdinfo, 1);
	if (rc < 0) {
		GI2C_E("150: failed to i2c register board info(busnum=%d, ret=%d)\n", I2C_BUS_NUM_ACCEL, rc);
		return;
	}
	
	rc = platform_device_register(&swift_accel_i2c_device);
	if (rc != 0) {
		GI2C_E("150: failed to register motion platform device(ret=%d)\n", rc);
	}
}

static struct i2c_gpio_platform_data compass_i2c_pdata = {
        .sda_pin = ECOM_GPIO_I2C_SDA,
        .sda_is_open_drain = 0,
        .scl_pin =      ECOM_GPIO_I2C_SCL,
        .scl_is_open_drain = 0,
        .udelay = 2
};

static struct platform_device swift_compass_i2c_device = {
        .name = "i2c-gpio",
        .id = I2C_BUS_NUM_COMPASS,
        .dev.platform_data = &compass_i2c_pdata,
};

static struct i2c_board_info compass_i2c_bdinfo = {
        I2C_BOARD_INFO("akm8973", ECOM_I2C_ADDRESS),
        .irq = MSM_GPIO_TO_INT(ECOM_GPIO_INT),
};

void __init swift_init_i2c_compass(void) {
		int rc = 0;
        /*temp3*/printk("Function : %s start.\n", __FUNCTION__);

        gpio_configure(ECOM_GPIO_I2C_SCL, GPIOF_DRIVE_OUTPUT);
        //gpio_set_value(ECOM_GPIO_I2C_SCL, 1);
        gpio_configure(ECOM_GPIO_I2C_SDA, GPIOF_DRIVE_OUTPUT);
        //gpio_set_value(ECOM_GPIO_I2C_SDA, 1);
        //gpio_configure(ECOM_GPIO_INT, GPIOF_DRIVE_OUTPUT);
        //gpio_set_value(ECOM_GPIO_INT, 1);

		rc = gpio_request(ECOM_GPIO_INT, "compass_int");
	
		if (rc < 0) {
			GI2C_E("8973: failed to gpio %d request(ret=%d)\n", ECOM_GPIO_INT, rc);
			return;
		}
		
		gpio_direction_input(ECOM_GPIO_INT);
		
        //gpio_configure(ECOM_GPIO_RST, GPIOF_DRIVE_OUTPUT);
        //gpio_set_value(ECOM_GPIO_RST, 1);

		
        /*temp3*/printk("Function : %s processing.\n", __FUNCTION__);

        rc = i2c_register_board_info(I2C_BUS_NUM_COMPASS, &compass_i2c_bdinfo, 1);
		if (rc < 0) {
			GI2C_E("8973: failed to i2c register board info(busnum=%d, ret=%d)\n", I2C_BUS_NUM_COMPASS, rc);
			return;
		}
		
		
        rc = platform_device_register(&swift_compass_i2c_device);
		if (rc != 0) {
			GI2C_E("8973: failed to register motion platform device(ret=%d)\n", rc);
		}
}