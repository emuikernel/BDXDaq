# Makefile - makefile for bsp/config/mv5100
#
# Copyright 1984-2001 Wind River Systems, Inc.
# Copyright 1996-2001 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01k,26jun02,kab  Add bootrom.bin
# 01j,07mar02,kab  Remove -O2 compiler option
# 01i,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01h,25sep01,dat  Update for T2.2
# 01g,19sep01,dat  adding sysVxALib.o, removing -DTARGET_DIR
# 01f,19jun01,dtr  Removing previous changes for mv5100Drv.a.
# 01f,15jun01,kab  Restored Mot SCSI drv
# 01e,07may01,mot  Remove local binary drivers
# 01d,24apr01,kab  Added LIB_EXTRA archive of 3rd party driver obj's
# 01c,28mar01,pch  Set HEX_FLAGS to empty for new GNU objcopyppc; remove
#                  sym895Lib.obj (use target/lib/objPPC604gnuvx/sym895Lib.o
#                  instead)
# 01b,27jul00,dmw  Added sym895Lib.obj and sysMaxL2Disable.o to MACH_EXTRA.
# 01a,13mar00,dmw  Written (from version 01a of mcpn765/Makefile).
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola MVME5100 board with a PowerPC 750 or 7400 processor.
#
# INCLUDES
#     makeTarget
#

CPU              = PPC604
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.


TARGET_DIR	= mv5100
VENDOR		= Motorola
BOARD		= MVME5100

LIB_EXTRA       =

EXTRA_DEFINE    = -DMV5100

USR_ENTRY       = usrInit

RELEASE		+= bootrom.bin

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, and Makefile.
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= fff00100 # ROM entry address
ROM_SIZE	= 00100000 # number of bytes of ROM space

RAM_LOW_ADRS	= 00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00800000 # RAM text/data address

BOOT_EXTRA      = hawkSmc.o hawkI2c.o sysMotVpdUtil.o sysMotVpdALib.o \
		  sysVxALib.o

#
MACH_EXTRA      = hawkSmc.o hawkI2c.o sysMotVpdUtil.o sysMotVpdALib.o \
                  sysMaxL2Disable.o intDisconnect.o symTbl.o

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx5100
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx5100.sym

installFD:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx5100_FD
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx5100_FD.sym
