#
# Makefile for the Linux kernel device drivers.
#
# 2006-04-04, designed by huawei corp.
# written as internal module's Makefile.
#
#KERNEL_MAKE	:= -C /home/shared/snapshoot/Hi3511v100_FPGA_OSDrv-LastRelease/sdk/pub/kbuild/ 
KERNEL_MAKE := -C ../../../pub/kbuild-FULL_DBG

obj-m += hirtc.o
hirtc-y := hi_rtc.o


default:
	make $(KERNEL_MAKE) M=$(PWD) modules 
clean: 
	make $(KERNEL_MAKE) M=$(PWD) clean 
