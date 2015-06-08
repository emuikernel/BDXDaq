# Makefile - makefile for bsp/config/mv2304
#
# Copyright 1984-2002 Wind River Systems, Inc.
# Copyright 1996 Motorola, Inc.
#
# modification history
# --------------------
# 01k,30apr02,sbs  Update for Tornado 2.2
# 01j,16jan02,yvp  Increase RAM_HIGH_ADRS to 0x00300000 - prevent build errs.
# 01i,14nov01,dat  removed RELEASE
# 01h,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01g,25sep01,dat  Update for T2.2
# 01f,28mar01,pch  Set HEX_FLAGS to empty for new GNU objcopyppc
# 01f,04jun01,sn   Removed obsolete TARGET_DIR preprocessor macro
# 01e,18may98,ms_  change ROM_SIZE to 1 Meg (0x00100000) (SPR 21240)
# 01d,15apr98,ms_  release target is bootrom_uncmp, vxWorks, vxWorks.st
# 01c,08apr98,dat  removed dec21140.obj
# 01b,15aug97,mas  changed bootrom_uncmp.hex to bootrom.hex for standard RELEASE
# 01a,02jun97,dat  written, from mv2604/Makefile, ver 01f.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola MVME2300 board with a PowerPC 604 processor.
#
# INCLUDES
#     makeTarget
#*/

CPU              = PPC604
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.


TARGET_DIR	= mv2304
VENDOR		= Motorola
BOARD		= MVME2300

EXTRA_DEFINE    = -DMV2300

USR_ENTRY       = usrInit

RELEASE		+= bootrom.bin

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, MakeSkel, Makefile, and Makefile.*
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= fff00100 # ROM entry address
ROM_SIZE	= 0x00100000 # number of bytes of ROM space

RAM_LOW_ADRS	= 00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00300000 # RAM text/data address

MACH_EXTRA = intDisconnect.o symTbl.o

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx2300
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx2300.sym

installFD:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx2300_FD
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx2300_FD.sym




