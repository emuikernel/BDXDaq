# Makefile - makefile for bsp/config/mv5500
#
# Copyright 1984-1999 Wind River Systems, Inc.
# Copyright 1996-2003 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01i,25jul03,scb Updated copyright
# 01h,25jul03,scb Added DOC_FILES: sysMotVpdShow 
#		      sysMv64260SmcShow ns8730xSuperIo sysIbc
# 01g,02dec02,scb Added more modules to DOC_FILES line.
# 01f,02dec02,scb Added "sysMv64260Smc.o" to MACH_EXTRA.
# 01e,02nov02,scb Removed temporary ADDED_CFLAGS.
# 01d,29oct02,scb Make i8250Sio inclusion from ".obj" file.
# 01c,20oct02,scb "EXTRA_DEFINE    = -DMV5500" (only).
# 01b,14oct02,yyz Added DOC_FILES and EXTRA_DOC_FLAGS with sysRtc,
#                 sysFailsafe, and m48t37.
# 01a,01oct02,scb Adapted from hxeb100 base (ver 01d).
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola MVME5500 board with a PowerPC 7455 processor.
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
		  sysMotVpdShow sysMv64260SmcShow ns8730xSuperIo sysIbc

#
# EXTRA_DOC_FLAGS is included so that all real-time clock, alarm
# clock, and failsafe timer routines will be included in the html
# documentation (make man), regardless of whether INCLUDE_RTC or
# INCLUDE_FAILSAFE are defined.
#

EXTRA_DOC_FLAGS = -DINCLUDE_RTC -DINCLUDE_FAILSAFE

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR	= mv5500
VENDOR		= Motorola
BOARD		= MVME5500

EXTRA_DEFINE    = -DMV5500

USR_ENTRY       = usrInit

#
# The constants ROM_TEXT_ADRS, ROM_WARM_ADRS,  RAM_LOW_ADRS and
# RAM_HIGH_ADRS are defined in config.h, and Makefile.
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
# If INCLUDE_NETWORK, INCLUDE_END, and INCLUDE_WANCOM_END are all
# #define'd then "wancomEnd.obj" must be included as part of the
# MACH_EXTRA list in the Makefile (remove the "#" in front of
# "wancomEnd.obj").  If any of the above are not #define'd, then
# "wancomEnd.obj" must be removed from the MACH_EXTRA list.  Failure
# to follow this will result in build errors.

BOOT_EXTRA      = sysAUtil.o sysMv64260I2c.o sysMotI2c.o sysMv64260Smc.o

# was:
#MACH_EXTRA      = sysAUtil.o sysMv64260I2c.o sysMotI2c.o sysMv64260Smc.o \
#		  i8250Sio.obj # wancomEnd.obj
                  

#
# to get symTbl.o, 'gmake vxWorks.st'
# to build bootrom_uncmp.bin, remove symTbl.o
#
MACH_EXTRA   =sysAUtil.o sysMv64260I2c.o sysMotI2c.o sysMv64260Smc.o \
		  i8250Sio.obj intDisconnect.o symTbl.o

# Sergey
sysLib.o: sysDma.c
sysLib.o: sysMv64260Dma.c

RELEASE         = bootrom_uncmp vxWorks vxWorks.st bootrom

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx5500
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx5500.sym











