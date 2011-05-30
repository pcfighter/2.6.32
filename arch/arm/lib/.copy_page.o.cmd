cmd_arch/arm/lib/copy_page.o := /home/mur4ik/android/cm/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,arch/arm/lib/.copy_page.o.d  -nostdinc -isystem /home/mur4ik/android/cm/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -Iinclude  -I/home/mur4ik/android/kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=6 -march=armv6k -mtune=arm1136j-s -include asm/unified.h -msoft-float -gdwarf-2       -c -o arch/arm/lib/copy_page.o arch/arm/lib/copy_page.S

deps_arch/arm/lib/copy_page.o := \
  arch/arm/lib/copy_page.S \
  /home/mur4ik/android/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/mur4ik/android/kernel/arch/arm/include/asm/linkage.h \
  /home/mur4ik/android/kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/mur4ik/android/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/mur4ik/android/kernel/arch/arm/include/asm/hwcap.h \
  include/asm/asm-offsets.h \
  /home/mur4ik/android/kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \

arch/arm/lib/copy_page.o: $(deps_arch/arm/lib/copy_page.o)

$(deps_arch/arm/lib/copy_page.o):
