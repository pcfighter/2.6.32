EXTRA_CFLAGS += -Iarch/arm/mach-msm
obj-$(CONFIG_LGE_HEADSET_2GPIO) += lge_gpio_h2w.o
obj-$(CONFIG_LGE_SWITCH_GPIO) += lge_gpio_switch.o
obj-$(CONFIG_LGE_HANDLE_PANIC) += lge_handle_panic.o
obj-$(CONFIG_LGE_PROC_COMM) += lge_proc_comm.o
obj-$(CONFIG_LGE_SUPPORT_ERS) += lge_ers.o
obj-$(CONFIG_LGE_SUPPORT_RAPI) += LG_rapi_client.o
obj-$(CONFIG_LGE_MTD_DIRECT_ACCESS) += lge_mtd_direct_access.o
#obj-$(CONFIG_LGE_DIAGTEST) += lg_diag_testmode.o
#obj-$(CONFIG_LGE_DIAGTEST) += lg_fw_diag_class.o
#obj-$(CONFIG_LGE_DIAGTEST) += lg_fw_diag_communication.o
# add lg_diag_event_log.c for key logging in diag testmode test 2010-07-10
#obj-$(CONFIG_LGE_DIAGTEST) += lg_diag_event_log.o
# add lg_diag_keypress.c for UTS test by younchan.kim 2010-06-11
#obj-$(CONFIG_LGE_DIAGTEST) += lg_diag_keypress.o 
# add lg_diag_screen_shot.c for UTS test by minjong.gong 2010-06-13
#obj-$(CONFIG_LGE_DIAGTEST) += lg_diag_screen_shot.o
#obj-$(CONFIG_LGE_DIAGTEST) += lg_fw_diag_mtc.o
#obj-$(CONFIG_LGE_DIAGTEST) += lg_diag_wifi.o

obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-ec-ak8973.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-msensor-bma150.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-sensors.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-mmc.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-snd.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-panel.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-bt.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-camera.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-input.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-misc.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-pm.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-bl-rt9393_mc.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-vibrator.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-leds.o
#obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-ss-driveric-panel.o
#obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += mddi_ss_driveric_innotek.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += board-swift-msm-touch.o
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += lge_errorhandler.o #LGE_CHANGE [blue.park@lge.com] 2009-04-04 <For Error Handler>

obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += lge_temp.o # LGE_CHANGE neo.kang@lge.com just for Line PV. It will be removed
obj-$(CONFIG_MACH_MSM7X27_THUNDERG) += lge_mtd_direct_access.o # LGE_CHANGE neo.kang@lge.com just for Line PV. It will be removed.

obj-$(CONFIG_MACH_LGE) += devices_lge.o

obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_cmd.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_eta.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_mmc.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_event_log.o
obj-$(CONFIG_LGE_ATS_UEVENT_DEVICE) += lge_ats_uevent.o
obj-$(CONFIG_LGE_ATS_INPUT_DEVICE) += lge_ats_input.o
obj-$(CONFIG_LGE_DIAG_WMC) += lg_diag_wmc.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_flex.o # LGE_CHANGE [irene.park@lge.com] 2010-06-04 - to get flex value from ARM9 
obj-$(CONFIG_LGE_PWRSINK) += lge_pwrsink.o

#
# Makefile for the swift specific files
#

#obj-y += board-swift.o
#obj-y += board-swift-keypad.o
#obj-y += board-swift-gpio-i2c.o
#obj-y += board-swift-msensor-bma150.o
#obj-y += board-swift-ec-ak8973.o
#obj-y += board-swift-headset.o
#obj-y+= board-swift-bt.o
#obj-y += cal_algorithm.o
