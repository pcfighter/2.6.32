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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/bootmem.h>
#include <linux/usb/mass_storage_function.h>
#include <linux/power_supply.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>
#include <asm/setup.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif /* CONFIG_CACHE_L2X0 */ 

#include <asm/mach/mmc.h>
#include <mach/vreg.h>
#include <mach/mpp.h>
#include <mach/gpio.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_rpcrouter.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/msm_serial_hs.h>
#include <mach/memory.h>
#include <mach/msm_battery.h>

#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/i2c.h>
#include <linux/android_pmem.h>
#include <mach/camera.h>


#ifdef CONFIG_BATTERY_MSM
#include <linux/power_supply.h>
#include <mach/msm_battery.h>
#endif /*CONFIG_BATTERY_MSM*/

#include <mach/board_swift.h>

#include "../devices.h"
#include "../socinfo.h"
#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android.h>
#endif /*CONFIG_USB_ANDROID*/
#include "../pm.h"
#include <linux/msm_kgsl.h>

uint32_t g_fb_addr=0;
// MOD : [KERNEL] 5340 KERNEL PATCH
#ifdef CONFIG_ARCH_MSM7X27
#define MSM_PMEM_MDP_SIZE	0x1700000
#define MSM_PMEM_ADSP_SIZE	0xAE4000
#define MSM_PMEM_AUDIO_SIZE	0x5B000
#define MSM_FB_SIZE		0x177000
#define MSM_GPU_PHYS_SIZE	SZ_2M
#define PMEM_KERNEL_EBI1_SIZE	0x1C000
#else /*CONFIG_ARCH_MSM7X27*/ 

#define MSM_FB_SIZE		0x200000
#define MSM_GPU_PHYS_SIZE	SZ_2M
#define MSM_PMEM_AUDIO_SIZE    0x121000 
#define PMEM_KERNEL_EBI1_SIZE	0x80000
#endif
/* for MMC detect */
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION

#define GPIO_MMC_CD_N 49
#if defined(CONFIG_MACH_MSM7X27_SWIFT_REV_1)
#define GPIO_MMC_CD_COVER 19
#endif

#endif

//todo add headers 
void map_zero_page_strongly_ordered(void);
void __init swift_init_timed_vibrator(void);

#ifdef CONFIG_USB_FUNCTION
static struct usb_mass_storage_platform_data usb_mass_storage_pdata = {
	.nluns		= 0x01,
	.buf_size       = 16384,
	.vendor         = "LGE",
	.product        = "Android Platform",
	.release        = 0xffff,

};

static struct platform_device mass_storage_device = {
	.name           = "usb_mass_storage",
	.id             = -1,
	.dev            = {
		.platform_data          = &usb_mass_storage_pdata,
	},
};
#endif
#ifdef CONFIG_USB_ANDROID
/* dynamic composition */
static struct usb_composition usb_func_composition[] = {
	{
		.product_id         = 0x9015,
		/* MSC + ADB */
		.functions	    = 0x12 /* 10010 */
	},
	{
		.product_id         = 0xF000,
		/* MSC */
		.functions	    = 0x02, /* 0010 */
	},
	{
		.product_id         = 0xF005,
		/* MODEM ONLY */
		.functions	    = 0x03,
	},

	{
		.product_id         = 0x8080,
		/* DIAG + MODEM */
		.functions	    = 0x34,
	},
	{
		.product_id         = 0x8082,
		/* DIAG + ADB + MODEM */
		.functions	    = 0x0314,
	},
	{
		.product_id         = 0x8085,
		/* DIAG + ADB + MODEM + NMEA + MSC*/
		.functions	    = 0x25314,
	},
	{
		.product_id         = 0x9016,
		/* DIAG + GENERIC MODEM + GENERIC NMEA*/
		.functions	    = 0x764,
	},
	{
		.product_id         = 0x9017,
		/* DIAG + GENERIC MODEM + GENERIC NMEA + MSC*/
		.functions	    = 0x2764,
	},
	{
		.product_id         = 0x9018,
		/* DIAG + ADB + GENERIC MODEM + GENERIC NMEA + MSC*/
		.functions	    = 0x27614,
	},
	{
		.product_id         = 0xF009,
		/* CDC-ECM*/
		.functions	    = 0x08,
	}
};
static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id	= 0x05C6,
	.product_id	= 0x9018,
	.functions	= 0x27614,
	.version	= 0x0100,
	.compositions   = usb_func_composition,
	.num_compositions = ARRAY_SIZE(usb_func_composition),
	.product_name	= "Qualcomm HSUSB Device",
	.manufacturer_name = "Qualcomm Incorporated",
	.nluns = 1,
};
static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};
#endif

#ifdef CONFIG_USB_FUNCTION
static struct usb_function_map usb_functions_map[] = {
	{"modem", 0},
	{"diag", 1},
	{"nmea", 2},
	{"mass_storage", 3},
	{"adb", 4},
	{"ethernet", 5},
};

/* 2010-02-15, For NEW LGE host(PC) USB driver:
 *
 * Defined NEW USB connection modes for Android(by LGE)
 *
 * Normal mode(Full) : MODEM + DIAG + NMEA + UMS + ADB  (pid : 0x618E)
 * Normal mode(Light): MODEM + DIAG + NMEA + UMS        (pid : 0x618E)
 * Manufacturing mode: MODEM + DIAG                     (pid : 0x6000)
 * Mass Storage Only : UMS                              (pid : 0x61B4)
 *
 * As Full and Light modes have shared pid, we must use temporary swtiching.
 * For this, Light mode has temporary pid 0x6003. This pid is not used out
 * of phone and is in only HSUSB driver.
 */
/* dynamic composition */
static struct usb_composition usb_func_composition[] = {
    { 
        .product_id     = 0x61B4, 
        .functions      = 0x8,  /* 001000 UMS only */  }, 
	{
		.product_id     = 0x6000,
		.functions      = 0x3,  /* 000011 MDM+DIAG */  },
	{
		.product_id     = 0x6003,
		.functions      = 0x0F, /* 001111 MDM+DIAG+GPS+UMS */   },
	{
		.product_id     = 0x618E,
		.functions      = 0x1F, /* 011111 MDM+DIAG+GPS+UMS+ADB */   },
};
#endif

static struct msm_hsusb_platform_data msm_hsusb_pdata = {
#ifdef CONFIG_USB_FUNCTION
	.version		= 0x0100,
	.phy_info		= (USB_PHY_INTEGRATED | USB_PHY_MODEL_65NM),
	.vendor_id          = 0x1004,
	.product_name       = "LG Mobile USB Modem",
	.serial_number      = "LGE_ANDROID_GT540",
	.manufacturer_name  = "LG Electronics Inc.",
	.compositions	= usb_func_composition,
	.num_compositions = ARRAY_SIZE(usb_func_composition),
	.function_map   = usb_functions_map,
	.num_functions	= ARRAY_SIZE(usb_functions_map),
	.config_gpio    = NULL,
#endif
};

static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}

static int hsusb_chg_init(int connect)
{
	if (connect)
		return msm_chg_rpc_connect();
	else
		return msm_chg_rpc_close();
}

void hsusb_chg_vbus_draw(unsigned mA)
{
	if (mA)
		msm_chg_usb_i_is_available(mA);
	else
		msm_chg_usb_i_is_not_available();
}

void hsusb_chg_connected(enum chg_type chgtype)
{
	switch (chgtype) {
	case CHG_TYPE_HOSTPC:
		pr_debug("Charger Type: HOST PC\n");
		msm_chg_usb_charger_connected(0);
		msm_chg_usb_i_is_available(100);
		break;
	case CHG_TYPE_WALL_CHARGER:
		pr_debug("Charger Type: WALL CHARGER\n");
		msm_chg_usb_charger_connected(2);
		msm_chg_usb_i_is_available(1500);
		break;
	case CHG_TYPE_INVALID:
		pr_debug("Charger Type: DISCONNECTED\n");
		msm_chg_usb_i_is_not_available();
		msm_chg_usb_charger_disconnected();
		break;
	}
}

static struct msm_otg_platform_data msm_otg_pdata = {
	.rpc_connect	= hsusb_rpc_connect,
	.phy_reset	= msm_hsusb_phy_reset,
};

static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	/* charging apis */
	.chg_init = hsusb_chg_init,
	.chg_connected = hsusb_chg_connected,
	.chg_vbus_draw = hsusb_chg_vbus_draw,
};

#define SND(desc, num) { .name = #desc, .id = num }
static struct snd_endpoint snd_endpoints_list[] = {
       SND(HANDSET, 0),
       SND(HEADSET, 2), 
       SND(CURRENT, 35),
       SND(SPEAKER, 5),
       SND(FM_RADIO_HEADSET, 9),
       SND(FM_RADIO_SPEAKER, 10), 
       SND(BT, 12),
       SND(IN_S_SADC_OUT_HANDSET, 3),
       SND(IN_S_SADC_OUT_SPEAKER_PHONE, 6),
       SND(HEADSET_AND_SPEAKER, 7),
};
#undef SND

static struct msm_snd_endpoints swift_device_snd_endpoints = {
	.endpoints = snd_endpoints_list,
	.num = sizeof(snd_endpoints_list) / sizeof(struct snd_endpoint)
};

static struct platform_device swift_device_snd = {
	.name = "msm_snd",
	.id = -1,
	.dev    = {
		.platform_data = &swift_device_snd_endpoints
	},
};

#define DEC0_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))

#define DEC1_FORMAT ((1<<MSM_ADSP_CODEC_WAV)|(1<<MSM_ADSP_CODEC_ADPCM)| \
	(1<<MSM_ADSP_CODEC_YADPCM)|(1<<MSM_ADSP_CODEC_QCELP))

#define DEC2_FORMAT ((1<<MSM_ADSP_CODEC_WAV)|(1<<MSM_ADSP_CODEC_ADPCM)| \
	(1<<MSM_ADSP_CODEC_YADPCM)|(1<<MSM_ADSP_CODEC_QCELP))

#define DEC3_FORMAT ((1<<MSM_ADSP_CODEC_WAV)|(1<<MSM_ADSP_CODEC_ADPCM)| \
	(1<<MSM_ADSP_CODEC_YADPCM)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC4_FORMAT (1<<MSM_ADSP_CODEC_MIDI)

static unsigned int dec_concurrency_table[] = {
	/* Audio LP */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DMA)), 0,
	0, 0, 0,

	/* Concurrency 1 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	 /* Concurrency 2 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 3 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 4 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 5 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 6 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),
};

#define DEC_INFO(name, queueid, decid, nr_codec) { .module_name = name, \
	.module_queueid = queueid, .module_decid = decid, \
	.nr_codec_support = nr_codec}

static struct msm_adspdec_info dec_info_list[] = {
	DEC_INFO("AUDPLAY0TASK", 13, 0, 11), /* AudPlay0BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY1TASK", 14, 1, 4),  /* AudPlay1BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY2TASK", 15, 2, 4),  /* AudPlay2BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY3TASK", 16, 3, 4),  /* AudPlay3BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY4TASK", 17, 4, 1),  /* AudPlay4BitStreamCtrlQueue */
};

static struct msm_adspdec_database msm_device_adspdec_database = {
	.num_dec = ARRAY_SIZE(dec_info_list),
	.num_concurrency_support = (ARRAY_SIZE(dec_concurrency_table) / \
					ARRAY_SIZE(dec_info_list)),
	.dec_concurrency_table = dec_concurrency_table,
	.dec_info_list = dec_info_list,
};

static struct platform_device msm_device_adspdec = {
	.name = "msm_adspdec",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_adspdec_database
	},
};

static struct android_pmem_platform_data android_pmem_kernel_ebi1_pdata = {
	.name = PMEM_KERNEL_EBI1_DATA_NAME,
	/* if no allocator_type, defaults to PMEM_ALLOCATORTYPE_BITMAP,
	 * the only valid choice at this time. The board structure is
	 * set to all zeros by the C runtime initialization and that is now
	 * the enum value of PMEM_ALLOCATORTYPE_BITMAP, now forced to 0 in
	 * include/linux/android_pmem.h.
	 */
	.cached = 0,
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

// Description: Unable to play MP3 files, Create PMEM audio device
static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};

static struct platform_device android_pmem_kernel_ebi1_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_kernel_ebi1_pdata },
};

static struct platform_device swift_keyled = {
	.name = "swift-led",
};

#ifdef CONFIG_SWIFT_BATTERY_STUB
static struct platform_device lge_battery = {
	.name = "lge-battery",
};
#endif
#ifdef CONFIG_BATTERY_MSM
static u32 swift_calculate_batt_capacity(u32 current_voltage);

static struct msm_psy_batt_pdata swift_psy_batt_data = {
	.voltage_min_design 	= 3200,
	.voltage_max_design	= 4200,
	.avail_chg_sources   	= AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
	.calculate_capacity	= &swift_calculate_batt_capacity,
};

static  u32 swift_calculate_batt_capacity(u32 current_voltage)
{
	u32 low_voltage   = swift_psy_batt_data.voltage_min_design;
	u32 high_voltage  = swift_psy_batt_data.voltage_max_design;
	u32 cap = 0 ;
	printk(KERN_INFO "Current Battary Voltage  = %d\n ",current_voltage);
	if (current_voltage >= 4190) 
	  return 100;
	else if (current_voltage <= 3200)
	  return 0;
	else 
	  { 
	    cap =  (current_voltage - low_voltage) * 100 / (high_voltage - low_voltage);
	    cap = cap +  (5 - cap % 5 ); 
	    return  cap;
	  }
}
static struct platform_device swift_batt_device = {
	.name 		    = "msm-battery",
	.id		    = -1,
	.dev.platform_data  = &swift_psy_batt_data,
};
#endif
static struct platform_device hs_device = {
   .name = "msm-handset",
   .id   = -1,
   .dev  = {
      .platform_data = "7k_handset",
   },
};

static struct resource swift_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

static int swift_fb_detect_panel(const char *name)
{
	int ret = -EPERM;

		if (!strcmp(name, "lcdc_sharp_wvga"))
			ret = 0;
		else
			ret = -ENODEV;
	return ret;
}

static struct msm_fb_platform_data swift_fb_pdata = {
	.detect_client = swift_fb_detect_panel,
	.mddi_prescan = 1,
};

static struct platform_device swift_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources  = ARRAY_SIZE(swift_fb_resources),
	.resource       = swift_fb_resources,
	.dev    = {
		.platform_data = &swift_fb_pdata,
	}
};


static struct resource kgsl_resources[] = {
	{
		.name = "kgsl_reg_memory",
		.start = 0xA0000000,
		.end = 0xA001ffff,
		.flags = IORESOURCE_MEM,
	},
	{
		.name   = "kgsl_phys_memory",
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM,
	},
	{
		.name = "kgsl_yamato_irq",
		.start = INT_GRAPHICS,
		.end = INT_GRAPHICS,
		.flags = IORESOURCE_IRQ,
	},

};

static struct kgsl_platform_data kgsl_pdata;

static struct platform_device swift_device_kgsl = {
	.name = "kgsl",
	.id = -1,
	.num_resources = ARRAY_SIZE(kgsl_resources),
	.resource = kgsl_resources,
	.dev = {
		.platform_data = &kgsl_pdata,
	},
};

#ifdef CONFIG_ANDROID_RAM_CONSOLE
#define MSM7X27_EBI1_CS0_BASE      0x00200000
#define SWIFT_RAM_CONSOLE_BASE (MSM7X27_EBI1_CS0_BASE + 214 * SZ_1M)
#define SWIFT_RAM_CONSOLE_SIZE (128 * SZ_1K)
#endif

#ifdef CONFIG_ANDROID_RAM_CONSOLE
static struct resource ram_console_resource[] = {
   {
      .name = "ram_console",
      .start = SWIFT_RAM_CONSOLE_BASE,
      .end = SWIFT_RAM_CONSOLE_BASE + SWIFT_RAM_CONSOLE_SIZE - 1,
      .flags = IORESOURCE_MEM,
   }
};

static struct platform_device ram_console_device = {
   .name = "ram_console",
   .id = -1,
   .num_resources = ARRAY_SIZE(ram_console_resource),
   .resource = ram_console_resource,
};
#endif

static struct i2c_board_info swift_i2c_devices[] = {
#ifdef CONFIG_ISX005
	{
		I2C_BOARD_INFO("isx005", 0x1A),
	},
#endif	
};

static uint32_t camera_off_gpio_table[] = {
	/* parallel CAMERA interfaces */
	GPIO_CFG(4,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT0 */
	GPIO_CFG(5,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT1 */
	GPIO_CFG(6,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT2 */
	GPIO_CFG(7,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT3 */
	GPIO_CFG(8,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT4 */
	GPIO_CFG(9,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT5 */
	GPIO_CFG(10, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT6 */
	GPIO_CFG(11, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT7 */
	GPIO_CFG(12, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* PCLK */
	GPIO_CFG(13, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* HSYNC_IN */
	GPIO_CFG(14, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VSYNC_IN */
	GPIO_CFG(15, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* MCLK */
};

static uint32_t camera_on_gpio_table[] = {
   /* parallel CAMERA interfaces */
   GPIO_CFG(4,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT0 */
   GPIO_CFG(5,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT1 */
   GPIO_CFG(6,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT2 */
   GPIO_CFG(7,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT3 */
   GPIO_CFG(8,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT4 */
   GPIO_CFG(9,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT5 */
   GPIO_CFG(10, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT6 */
   GPIO_CFG(11, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT7 */
   GPIO_CFG(12, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_16MA), /* PCLK */
   GPIO_CFG(13, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* HSYNC_IN */
   GPIO_CFG(14, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VSYNC_IN */
   GPIO_CFG(15, 1, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_16MA), /* MCLK */

};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static void config_camera_on_gpios(void)
{
	config_gpio_table(camera_on_gpio_table,
		ARRAY_SIZE(camera_on_gpio_table));
}

static void config_camera_off_gpios(void)
{
	config_gpio_table(camera_off_gpio_table,
		ARRAY_SIZE(camera_off_gpio_table));
}

static struct msm_camera_device_platform_data swift_camera_device_data = {
	.camera_gpio_on  = config_camera_on_gpios,
	.camera_gpio_off = config_camera_off_gpios,
	.ioext.mdcphy = MSM_MDC_PHYS,
	.ioext.mdcsz  = MSM_MDC_SIZE,
	.ioext.appphy = MSM_CLK_CTL_PHYS,
	.ioext.appsz  = MSM_CLK_CTL_SIZE,
};

#ifdef CONFIG_ISX005

static struct msm_camera_sensor_flash_src swift_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_PMIC,
};

static struct msm_camera_sensor_flash_data flash_isx005 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src  = &swift_flash_src
};

static struct msm_camera_sensor_info swift_camera_sensor_isx005_data = {
	.sensor_name    = "isx005",
	.sensor_reset   = 0,
	.sensor_pwd     = 1,
//	.vcm_pwd        = 0,
	.pdata          = &swift_camera_device_data,
	.flash_data     = &flash_isx005
};

static struct platform_device swift_camera_sensor_isx005 = {
	.name      = "msm_camera_isx005",
	.dev       = {
		.platform_data = &swift_camera_sensor_isx005_data,
	},
};
#endif


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

static struct platform_device *devices[] __initdata = {
#if !defined(CONFIG_MSM_SERIAL_DEBUGGER)
	&msm_device_uart3,
#endif
	&msm_device_smd,
	&msm_device_dmov,
	&msm_device_nand,
	
	&msm_device_otg,
	&msm_device_hsusb_otg,
	&msm_device_hsusb_host,
	&msm_device_hsusb_peripheral,
	&msm_device_gadget_peripheral,
#ifdef CONFIG_USB_FUNCTION
	&mass_storage_device,
#endif
#ifdef CONFIG_USB_ANDROID
	&android_usb_device,
#endif
	&msm_device_i2c,
	&msm_device_tssc,
	&android_pmem_kernel_ebi1_device,
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device, 

	&swift_fb_device,
	&msm_device_uart_dm1,
        &swift_device_snd,
	&msm_device_adspdec,
	&swift_device_kgsl,
/* msm-handsetn platform deivce reigistration, kenobi */
   &hs_device,
    /* add swift specific devices at following lines*/
	&swift_keyled,

#ifdef CONFIG_SWIFT_BATTERY_STUB
	&lge_battery,
#endif

#ifdef CONFIG_BATTERY_MSM
	&swift_batt_device,
#endif
#ifdef CONFIG_ANDROID_RAM_CONSOLE
   &ram_console_device,
#endif

#ifdef CONFIG_ISX005
	&swift_camera_sensor_isx005,
#endif
	&rt9393_bl,
	&mddi_ss_driveric_device,		
};

static struct mddi_platform_data mddi_pdata = {
};

static void __init swift_add_fb_devices(void)
{
        msm_fb_register_device("mdp", 0);
        msm_fb_register_device("pmdh", &mddi_pdata);
}

extern struct sys_timer msm_timer;

static void __init swift_init_irq(void)
{
	msm_init_irq();
}

static struct msm_acpu_clock_platform_data swift_clock_data = {
	.acpu_switch_time_us = 50,
	.max_speed_delta_khz = 256000,
	.vdd_switch_time_us = 62,
	.power_collapse_khz = 19200000,
	.wait_for_irq_khz = 128000000,
	.max_axi_khz = 200000,
};

void msm_serial_debug_init(unsigned int base, int irq,
			   struct device *clk_device, int signal_irq);

static void swift_sdcc_gpio_init(void)
{
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	int rc = 0;
	if (gpio_request(GPIO_MMC_CD_N, "sdc2_status_irq"))
		pr_err("failed to request gpio sdc2_status_irq\n");


		rc = gpio_tlmm_config(GPIO_CFG(GPIO_MMC_CD_N, 0, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_2MA), GPIO_ENABLE);

	if (rc)
		printk(KERN_ERR "%s: Failed to configure GPIO %d\n",
				__func__, rc);
#if defined(CONFIG_MACH_MSM7X27_SWIFT_REV_1)
	
	if (gpio_request(GPIO_MMC_CD_COVER, "sdc2_status_irq"))
		pr_err("failed to request gpio sdc2_status_irq\n");
	rc = gpio_tlmm_config(GPIO_CFG(GPIO_MMC_CD_COVER, 0, GPIO_INPUT, GPIO_NO_PULL,GPIO_2MA), GPIO_ENABLE);

	if (rc)
		printk(KERN_ERR "%s: Failed to configure GPIO %d\n",__func__, rc);
#endif
												
#endif

	/* SDC1 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	if (gpio_request(51, "sdc1_data_3"))
		pr_err("failed to request gpio sdc1_data_3\n");
	if (gpio_request(52, "sdc1_data_2"))
		pr_err("failed to request gpio sdc1_data_2\n");
	if (gpio_request(53, "sdc1_data_1"))
		pr_err("failed to request gpio sdc1_data_1\n");
	if (gpio_request(54, "sdc1_data_0"))
		pr_err("failed to request gpio sdc1_data_0\n");
	if (gpio_request(55, "sdc1_cmd"))
		pr_err("failed to request gpio sdc1_cmd\n");
	if (gpio_request(56, "sdc1_clk"))
		pr_err("failed to request gpio sdc1_clk\n");
#endif

	/* SDC2 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT
	if (gpio_request(62, "sdc2_clk"))
		pr_err("failed to request gpio sdc2_clk\n");
	if (gpio_request(63, "sdc2_cmd"))
		pr_err("failed to request gpio sdc2_cmd\n");
	if (gpio_request(64, "sdc2_data_3"))
		pr_err("failed to request gpio sdc2_data_3\n");
	if (gpio_request(65, "sdc2_data_2"))
		pr_err("failed to request gpio sdc2_data_2\n");
	if (gpio_request(66, "sdc2_data_1"))
		pr_err("failed to request gpio sdc2_data_1\n");
	if (gpio_request(67, "sdc2_data_0"))
		pr_err("failed to request gpio sdc2_data_0\n");
#endif

	/* SDC3 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	if (gpio_request(88, "sdc3_clk"))
		pr_err("failed to request gpio sdc3_clk\n");
	if (gpio_request(89, "sdc3_cmd"))
		pr_err("failed to request gpio sdc3_cmd\n");
	if (gpio_request(90, "sdc3_data_3"))
		pr_err("failed to request gpio sdc3_data_3\n");
	if (gpio_request(91, "sdc3_data_2"))
		pr_err("failed to request gpio sdc3_data_2\n");
	if (gpio_request(92, "sdc3_data_1"))
		pr_err("failed to request gpio sdc3_data_1\n");
	if (gpio_request(93, "sdc3_data_0"))
		pr_err("failed to request gpio sdc3_data_0\n");
#endif

	/* SDC4 GPIOs */
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	if (gpio_request(19, "sdc4_data_3"))
		pr_err("failed to request gpio sdc4_data_3\n");
	if (gpio_request(20, "sdc4_data_2"))
		pr_err("failed to request gpio sdc4_data_2\n");
	if (gpio_request(21, "sdc4_data_1"))
		pr_err("failed to request gpio sdc4_data_1\n");
	if (gpio_request(107, "sdc4_cmd"))
		pr_err("failed to request gpio sdc4_cmd\n");
	if (gpio_request(108, "sdc4_data_0"))
		pr_err("failed to request gpio sdc4_data_0\n");
	if (gpio_request(109, "sdc4_clk"))
		pr_err("failed to request gpio sdc4_clk\n");
#endif
}

static unsigned sdcc_cfg_data[][6] = {
	/* SDC1 configs */
	{
	GPIO_CFG(51, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(52, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(53, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(54, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(55, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(56, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),
	},
	/* SDC2 configs */
	{
	GPIO_CFG(62, 2, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),
	GPIO_CFG(63, 2, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(64, 2, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(65, 2, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(66, 2, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(67, 2, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	},
	/* SDC3 configs */
	{
	GPIO_CFG(88, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),
	GPIO_CFG(89, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(90, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(91, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(92, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(93, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	},
	/* SDC4 configs */
	{
	GPIO_CFG(19, 3, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(20, 3, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(21, 4, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(107, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(108, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),
	GPIO_CFG(109, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),
	}
};

static unsigned long vreg_sts, gpio_sts;
static struct vreg *vreg_mmc;

static void swift_sdcc_setup_gpio(int dev_id, unsigned int enable)
{
	int i, rc;

	if (!(test_bit(dev_id, &gpio_sts)^enable))
		return;

	if (enable)
		set_bit(dev_id, &gpio_sts);
	else
		clear_bit(dev_id, &gpio_sts);

	for (i = 0; i < ARRAY_SIZE(sdcc_cfg_data[dev_id - 1]); i++) {
		rc = gpio_tlmm_config(sdcc_cfg_data[dev_id - 1][i],
			enable ? GPIO_ENABLE : GPIO_DISABLE);
		if (rc)
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, sdcc_cfg_data[dev_id - 1][i], rc);
	}
}

struct mmc_vdd_xlat {
	int mask;
	int level;
};


static uint32_t swift_sdcc_setup_power(struct device *dv, unsigned int vdd)
{
	int rc = 0;
	struct platform_device *pdev;
	int i;
	unsigned cnt = 0;

	pdev = container_of(dv, struct platform_device, dev);
	swift_sdcc_setup_gpio(pdev->id, !!vdd);

	printk(KERN_ERR "(%s)vdd :%x\n", __func__, vdd);

	if (vdd == 0) {
		if (!vreg_sts)
			return 0;

		clear_bit(pdev->id, &vreg_sts);

		if (!vreg_sts) {
				rc = vreg_set_level(vreg_mmc, 0);
				cnt = vreg_get_refcnt(vreg_mmc);
				for(i = 0; i < cnt; i++)
					rc = vreg_disable(vreg_mmc);
				printk(KERN_DEBUG "%s: Refcnt %d\n", 
						__func__, vreg_get_refcnt(vreg_mmc));		
			if (rc)
				printk(KERN_ERR "%s: return val: %d \n",
					__func__, rc);
		}
		return 0;
	}
	if (!vreg_sts) {
			rc = vreg_set_level(vreg_mmc, 2650);
			if (!rc) {
				rc = vreg_enable(vreg_mmc);
			}
		if (rc)
			printk(KERN_ERR "%s: return val: %d \n",
					__func__, rc);
	}
	set_bit(pdev->id, &vreg_sts);
	return 0;
}

#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
static unsigned int swift_sdcc_slot_status(struct device *dev)
{
#if defined(CONFIG_MACH_MSM7X27_SWIFT_REV_1)
	return !(gpio_get_value(GPIO_MMC_CD_N) || gpio_get_value(GPIO_MMC_CD_COVER));
#else
	return !gpio_get_value(GPIO_MMC_CD_N);
#endif
}
#endif

#ifdef CONFIG_LGE_BCM432X_PATCH
static unsigned int bcm432x_sdcc_wlan_slot_status(struct device *dev)
{
	return gpio_get_value(CONFIG_BCM4325_GPIO_WL_RESET);
}


static struct mmc_platform_data bcm432x_sdcc_wlan_data = {
    .ocr_mask   	= MMC_VDD_30_31,
	.translate_vdd	= swift_sdcc_setup_power,
    .status     	= bcm432x_sdcc_wlan_slot_status,
	.status_irq 	= MSM_GPIO_TO_INT(CONFIG_BCM4325_GPIO_WL_RESET),


    .irq_flags      = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
    .mmc_bus_width  = MMC_CAP_4_BIT_DATA,
};
#endif /* CONFIG_LGE_BCM432X_PATCH*/

#define SWIFT_MMC_VDD (MMC_VDD_165_195 | MMC_VDD_20_21 | MMC_VDD_21_22 \
			| MMC_VDD_22_23 | MMC_VDD_23_24 | MMC_VDD_24_25 \
			| MMC_VDD_25_26 | MMC_VDD_26_27 | MMC_VDD_27_28 \
			| MMC_VDD_28_29 | MMC_VDD_29_30)

static struct mmc_platform_data msm7x2x_sdcc_data = {
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	.ocr_mask		= SWIFT_MMC_VDD,
	.translate_vdd	= swift_sdcc_setup_power,
	.status         = swift_sdcc_slot_status, 
	.status_irq 	= MSM_GPIO_TO_INT(GPIO_MMC_CD_COVER),	

	.irq_flags	=  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.mmc_bus_width	= MMC_CAP_4_BIT_DATA,
#else
	.ocr_mask	= MMC_VDD_28_29,
	.translate_vdd	= swift_sdcc_setup_power,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif	
};



static void __init swift_init_mmc(void)
{
        vreg_mmc = vreg_get(NULL, "mmc");
	if (IS_ERR(vreg_mmc)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		       __func__, PTR_ERR(vreg_mmc));
		return;
	}
	swift_sdcc_gpio_init();

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	msm_add_sdcc(1, &msm7x2x_sdcc_data);
#endif /* CONFIG_MMC_MSM_SDC1_SUPPORT */	

#ifdef CONFIG_MMC_MSM_SDC2_SUPPORT

#if defined(CONFIG_LGE_BCM432X_PATCH)
	gpio_request(CONFIG_BCM4325_GPIO_WL_RESET, "wlan_cd");

	gpio_tlmm_config(GPIO_CFG(CONFIG_BCM4325_GPIO_WL_HOSTWAKEUP, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_2MA), GPIO_ENABLE);
	gpio_tlmm_config(GPIO_CFG(CONFIG_BCM4325_GPIO_WL_REGON, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), GPIO_ENABLE);
	gpio_configure(CONFIG_BCM4325_GPIO_WL_REGON, GPIOF_DRIVE_OUTPUT);
	gpio_tlmm_config(GPIO_CFG(CONFIG_BCM4325_GPIO_WL_RESET, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), GPIO_ENABLE);
	gpio_configure(CONFIG_BCM4325_GPIO_WL_RESET, GPIOF_DRIVE_OUTPUT);	
#endif /*CONFIG_LGE_BCM432X_PATCH*/
         msm_add_sdcc(2, &bcm432x_sdcc_wlan_data);
#else /* qualcomm or google */
         msm_add_sdcc(2, &msm7x2x_sdcc_data);
#endif  /* CONFIG_MMC_MSM_SDC2_SUPPORT */


#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	msm_add_sdcc(3, &msm7x2x_sdcc_data);
#endif  /* CONFIG_MMC_MSM_SDC3_SUPPORT */

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	msm_add_sdcc(4, &msm7x2x_sdcc_data);
#endif   /* CONFIG_MMC_MSM_SDC4_SUPPORT */


}


static struct msm_pm_platform_data swift_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 16000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].residency = 20000,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 12000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].residency = 20000,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].suspend_enabled
		= 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 2000,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].residency = 0,
};

static void swift_i2c_gpio_config(int iface, int config_type)
{
	int gpio_scl;
	int gpio_sda;
	if (iface) {
		gpio_scl = 95;
		gpio_sda = 96;
	} else {
		gpio_scl = 60;
		gpio_sda = 61;
	}
	if (config_type) {
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 1, GPIO_INPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 1, GPIO_INPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
	} else {
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 0, GPIO_OUTPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 0, GPIO_OUTPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
	}
}

static struct msm_i2c_platform_data swift_i2c_pdata = {
	.clk_freq = 400000,
	.rmutex = 0,
	.pri_clk = 60,
	.pri_dat = 61,
	.aux_clk = 95,
	.aux_dat = 96,
	.msm_i2c_config_gpio = swift_i2c_gpio_config,
};

static void __init swift_init_i2c(void)
{
	if (gpio_request(60, "i2c_pri_clk"))
		pr_err("failed to request gpio i2c_pri_clk\n");
	if (gpio_request(61, "i2c_pri_dat"))
		pr_err("failed to request gpio i2c_pri_dat\n");
	if (gpio_request(95, "i2c_sec_clk"))
		pr_err("failed to request gpio i2c_sec_clk\n");
	if (gpio_request(96, "i2c_sec_dat"))
		pr_err("failed to request gpio i2c_sec_dat\n");
	swift_i2c_pdata.pm_lat = swift_pm_data[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency;
	msm_device_i2c.dev.platform_data = &swift_i2c_pdata;
}

static void swift_init_pmic(void) 
{
	static struct vreg *vreg_aux2;
	int rc = 0;

	vreg_aux2 = vreg_get(NULL, "gp5");
	if (IS_ERR(vreg_aux2)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
				__func__, PTR_ERR(vreg_aux2));
		return;
	}

	rc = vreg_set_level(vreg_aux2, 0);
	if (!rc)
		rc = vreg_enable(vreg_mmc);
	if (rc)
		printk(KERN_ERR "%s: return val: %d \n",
				__func__, rc);

	return;
}

static void __init swift_init(void)
{
	if (socinfo_init() < 0)
		BUG();

	printk("LG GT540 Swift (Optimus) Init\n");

	msm_acpu_clock_init(&swift_clock_data);

	/* Initialize the zero page for barriers and cache ops */
	map_zero_page_strongly_ordered();

	//msm_hsusb_pdata.max_axi_khz = msm7x27_clock_data.max_axi_khz;

	/* This value has been set to 160000 for power savings. */
	/* OEMs may modify the value at their discretion for performance */
	/* The appropriate maximum replacement for 160000 is: */
	/* clk_get_max_axi_khz() */
	kgsl_pdata.max_axi_freq = 160000;

	msm_hsusb_pdata.swfi_latency = swift_pm_data[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	msm_device_hsusb_peripheral.dev.platform_data = &msm_hsusb_pdata;
	msm_device_otg.dev.platform_data = &msm_otg_pdata;
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
	msm_device_hsusb_host.dev.platform_data = &msm_hsusb_pdata;
	platform_add_devices(devices, ARRAY_SIZE(devices));
//	msm_camera_add_device();
	swift_init_i2c();
	i2c_register_board_info(0, swift_i2c_devices, ARRAY_SIZE(swift_i2c_devices));


	platform_device_register(&keypad_device_swift);
	
	swift_add_fb_devices();
	swift_init_mmc();

	swift_init_pmic();

	msm_pm_set_platform_data(swift_pm_data);

	swift_add_btpower_devices();

	/* initialize swift specific devices */
	swift_init_gpio_i2c_devices();

	/* initialize timed_output vibrator */
	swift_init_timed_vibrator();
}

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static void __init pmem_kernel_ebi1_size_setup(char **p)
{
	pmem_kernel_ebi1_size = memparse(*p, p);
}
__early_param("pmem_kernel_ebi1_size=", pmem_kernel_ebi1_size_setup);

static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static void __init pmem_mdp_size_setup(char **p)
{
	pmem_mdp_size = memparse(*p, p);
}
__early_param("pmem_mdp_size=", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static void __init pmem_adsp_size_setup(char **p)
{
	pmem_adsp_size = memparse(*p, p);
}
__early_param("pmem_adsp_size=", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;
static void __init pmem_audio_size_setup(char **p)
{
	pmem_audio_size = memparse(*p, p);
}
__early_param("pmem_audio_size=", pmem_audio_size_setup);


static unsigned fb_size = MSM_FB_SIZE;
static void __init fb_size_setup(char **p)
{
	fb_size = memparse(*p, p);
}
__early_param("fb_size=", fb_size_setup);

static unsigned gpu_phys_size = MSM_GPU_PHYS_SIZE;
static void __init gpu_phys_size_setup(char **p)
{
	gpu_phys_size = memparse(*p, p);
}
__early_param("gpu_phys_size=", gpu_phys_size_setup);

static void __init swift_allocate_memory_regions(void)
{
	void *addr;
	unsigned long size;

	size = pmem_kernel_ebi1_size;
	if (size) {
		addr = alloc_bootmem_aligned(size, 0x100000);
		android_pmem_kernel_ebi1_pdata.start = __pa(addr);
		android_pmem_kernel_ebi1_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for kernel"
			" ebi1 pmem arena\n", size, addr, __pa(addr));
	}

	size = pmem_mdp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_pdata.start = __pa(addr);
		android_pmem_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for mdp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = pmem_adsp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_adsp_pdata.start = __pa(addr);
		android_pmem_adsp_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for adsp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = pmem_audio_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_audio_pdata.start = __pa(addr);
		android_pmem_audio_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for audio "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = fb_size ? : MSM_FB_SIZE;
	addr = alloc_bootmem(size);
	swift_fb_resources[0].start = __pa(addr);
	swift_fb_resources[0].end = swift_fb_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
		size, addr, __pa(addr));
	
	g_fb_addr = __pa(addr);
	size = gpu_phys_size ? : MSM_GPU_PHYS_SIZE;
	addr = alloc_bootmem(size);
	kgsl_resources[1].start = __pa(addr);
	kgsl_resources[1].end = kgsl_resources[1].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for KGSL\n",
		size, addr, __pa(addr));
}

static void __init swift_map_io(void)
{
	msm_map_common_io();
	msm_map_swift_io();
	msm_clock_init(msm_clocks_7x27, msm_num_clocks_7x27);
	swift_allocate_memory_regions();
#ifdef CONFIG_CACHE_L2X0
	l2x0_init(MSM_L2CC_BASE, 0x00068012, 0xfe000000);
#endif
}

static void __init swift_fixup(struct machine_desc *desc, struct tag *tags,
		char **cmdline, struct meminfo *mi)
{
	mi->nr_banks = 1;
	mi->bank[0].start = PHYS_OFFSET;
	mi->bank[0].node = PHYS_TO_NID(PHYS_OFFSET);
#ifdef CONFIG_ANDROID_RAM_CONSOLE
	mi->bank[0].size = (214*1024*1024);
#else
	mi->bank[0].size = (215*1024*1024);
#endif
}

//MACHINE_START(MSM7X27_SWIFT, "Swift board(LGE GT540)")
MACHINE_START(MSM7X27_THUNDERG, "Swift board(LGE GT540)")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params    = 0x00200100,
	.fixup      	= swift_fixup,
	.map_io         = swift_map_io,
	.init_irq       = swift_init_irq,
	.init_machine   = swift_init,
	.timer          = &msm_timer,
MACHINE_END
