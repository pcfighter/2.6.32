/* arch/arm/mach-msm/board-thunderg-panel.c
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

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <mach/msm_rpcrouter.h>
#include <mach/gpio.h>
#include <mach/vreg.h>
#include <mach/board.h>
#include <mach/board_lge.h>
#include "devices.h"
#include "board-thunderg.h"

#define MSM_FB_LCDC_VREG_OP(name, op) \
do { \
	vreg = vreg_get(0, name); \
	if (vreg_##op(vreg)) \
		printk(KERN_ERR "%s: %s vreg operation failed \n", \
			(vreg_##op == vreg_enable) ? "vreg_enable" \
				: "vreg_disable", name); \
} while (0)

static char *msm_fb_vreg[] = {
	"gp1",
	"gp2",
};

static int mddi_power_save_on;
static void msm_fb_mddi_power_save(int on)
{
	struct vreg *vreg;
	int flag_on = !!on;

	if (mddi_power_save_on == flag_on)
		return;

	mddi_power_save_on = flag_on;

	if (on) {
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[0], enable);
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[1], enable);
	} else{
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[0], disable);
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[1], disable);
	}
}

static struct mddi_platform_data mddi_pdata = {
	.mddi_power_save = msm_fb_mddi_power_save,
};

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = 97,
};

static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("pmdh", &mddi_pdata);
	msm_fb_register_device("lcdc", 0);
}

static struct platform_device rt9393_bl = {
	.name = "rt9393-bl",
};
static struct msm_panel_common_pdata mddi_ss_driveric_pdata = {

};

struct platform_device mddi_ss_driveric_device = {
	.name   = "mddi_ss",
	.id     = 0,
	.dev    = {
		.platform_data = &mddi_ss_driveric_pdata,
	}
};

struct device* thunderg_backlight_dev(void)
{
	return &rt9393_bl.dev;
}

/* common functions */
void __init lge_add_lcd_devices(void)
{
	platform_device_register(&mddi_ss_driveric_device);

	msm_fb_add_devices();
	
	platform_device_register(&rt9393_bl);
}
