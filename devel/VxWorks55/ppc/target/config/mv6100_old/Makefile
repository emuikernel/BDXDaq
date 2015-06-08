# Makefile - makefile for bsp/config/mv6100
#
# Copyright 1984-1999 Wind River Systems, Inc.
# Copyright 1996-2004 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01l,12may04,cak  Edited the release macro
# 01k,25feb04,scb  Add some new show routined file names to DOC_FILES
# 01j,03nov03,cak  BSP update.
# 01i,24jan03,cak  Modified EXTRA_DEFINE and added sysMv64360Smc.o to
#                  MACH_EXTRA.
# 01h,10jan03,efb  Added MV64360 ethernet driver file to MACH_EXTRA.
# 01g,18oct02,cak  RTC and Failsafe Timer support.
# 01f,11sep02,cak  removed sysMv64360Smc.o from MACH_EXTRA 
# 01e,26aug02,cak  Modifications necessary for Tornado 2.1 to 2.2 conversion
# 01d,31may02,cak  Added sysMotI2c.o and sysMv64360I2c.o to BOOT_EXTRA and 
#		   MACH_EXTRA and added sysMv64360Smc.o to MACH_EXTRA
# 01c,07may02,cak  Added sysMv64360Smc.o to BOOT_EXTRA, added sysAUtil.o to
#		   BOOT_EXTRA and MACH_EXTRA
# 01b,25apr02,cak  Added cacheArchLib.o
# 01a,06mar02,cak  Written. 
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola MVME6100 board with a PowerPC 7457 processor.
#
# INCLUDES
#     makeTarget
#

CPU              = PPC604
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp

#
# DOC_FILES is defined in h/make/defs.bsp.  It is being redefined
# here so that the appropriate html documentation is created when
# a make man command is executed.
#

DOC_FILES       = sysLib tyCoDrv mkboot sysRtc sysFailsafe m48t37 \
		  sysMotVpdShow sysMv64260SmcShow ns8730xSuperIo sysIbc \
		  sysTempeShow mv64360DumpRegs mv64360DumpRegs \
		  mv64360CPUWindows pcicfgdump 

#
# EXTRA_DOC_FLAGS is included so that all real-time clock, alarm
# clock, and failsafe timer routines will be included in the html
# documentation (make man), regardless of whether INCLUDE_RTC or
# INCLUDE_FAILSAFE are defined.
#

EXTRA_DOC_FLAGS = -DINCLUDE_RTC -DINCLUDE_FAILSAFE

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR	= mv6100
VENDOR		= Motorola
BOARD		= MVME6100

EXTRA_DEFINE    = -DMV6100

USR_ENTRY       = usrInit

#
# The constants ROM_TEXT_ADRS, ROM_WARM_ADRS, ROM_SIZE, RAM_LOW_ADRS,
# and RAM_HIGH_ADRS are defined in config.h, and Makefile.
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= fff00100 # ROM entry address
ROM_WARM_ADRS   = fff00104 # ROM warm entry address
ROM_SIZE	= 00100000 # number of bytes of ROM space

RAM_LOW_ADRS	= 00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00800000 # RAM text/data address

# BOOT_EXTRA and MACH_EXTRA
# 
# romInit.s should not call out to subroutines declared in other modules.
# If an outside module is necessary, it can be linked into the system by
# adding the module to BOOT_EXTRA.  If the same module is referenced by
# other BSP code, then the module must be added to MACH_EXTRA as well.
# This is done to prevent the module from being compressed, since it is
# needed before the uncompression has taken place.
#
# The BSP Developer's Kit defines BOOT_EXTRA as "Additional modules to be
# linked with compressed ROM images.  These modules are not linked with
# uncompressed or ROM-resident images, just compressed images.  Adding a
# module to MACH_EXTRA makes it available to uncompressed or ROM-resident
# images.  MACH_EXTRA is used to "add an object module to VxWorks...This 
# macro causes the linker to link it into the output object."  It is used
# to include object modules that are not "included" via sysLib.c. 
#
# NOTE: If INCLUDE_GEISC_END, in config.h, is undefined then mv64360End.obj
#       on the MACH_EXTRA line must either be removed or commented out.
#       This is to prevent compiler errors.

BOOT_EXTRA      = sysMv64360Smc.o sysAUtil.o sysMv64360I2c.o sysMotI2c.o





#
# to get symTbl.o, 'gmake vxWorks.st'
# to build bootrom_uncmp.bin, remove symTbl.o
#
MACH_EXTRA      = sysMv64360Smc.o sysAUtil.o sysMv64360I2c.o sysMotI2c.o \
                  mv64360End.obj intDisconnect.o #symTbl.o



                  
RELEASE        += vxWorks vxWorks.st bootrom.bin vxWorks_rom.bin \
		  vxWorks.st_rom.bin bootrom_uncmp.bin 

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx6100
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx6100.sym


