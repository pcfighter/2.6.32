cmd_drivers/input/misc/gpio_axis.o := /home/mur4ik/android/cm/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,drivers/input/misc/.gpio_axis.o.d  -nostdinc -isystem /home/mur4ik/android/cm/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/mur4ik/android/kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=6 -march=armv6k -mtune=arm1136j-s -msoft-float -Uarm -Wframe-larger-than=2112 -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(gpio_axis)"  -D"KBUILD_MODNAME=KBUILD_STR(gpio_axis)" -D"DEBUG_HASH=33" -D"DEBUG_HASH2=6" -c -o drivers/input/misc/gpio_axis.o drivers/input/misc/gpio_axis.c

deps_drivers/input/misc/gpio_axis.o := \
  drivers/input/misc/gpio_axis.c \

drivers/input/misc/gpio_axis.o: $(deps_drivers/input/misc/gpio_axis.o)

$(deps_drivers/input/misc/gpio_axis.o):
