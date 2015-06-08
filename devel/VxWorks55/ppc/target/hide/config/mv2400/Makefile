# Makefile - makefile for bsp/config/mv2400
#
# Copyright 1984-2001 Wind River Systems, Inc.
# Copyright 1996,1997,1998 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01i,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01h,25sep01,dat  Update for T2.2
# 01g,19sep01,dat  adding sysVxALib.o, removing -DTARGET_DIR
# 01f,22jun01,pch  undo commenting of CPU type; fix bootrom build
# 01e,03apr01,pch  Comment out "CPU = PPC604" to disable from global target
#                  build.  This is a temporary expedient to remove encumbrance
#                  to Altivec work, and needs to be fixed later.
# 01d,17jun99,rhv  Incorporating WRS code review changes.
# 01c,19may99,rhv  Adjusting RAM_HIGH_ADRS to 8MB.
# 01b,27jan99,mdp  Added SENS support.
# 01a,15dec98,mdp  written, from mv2304/Makefile, ver 01e.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola MVME2400 board with a PowerPC 750 processor.
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


TARGET_DIR	= mv2400
VENDOR		= Motorola
BOARD		= MVME2400

EXTRA_DEFINE    = -DMV2400

USR_ENTRY       = usrInit

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, MakeSkel, Makefile, and Makefile.*
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= fff00100 # ROM entry address
ROM_SIZE	= 00100000 # number of bytes of ROM space

RAM_LOW_ADRS	= 00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00800000 # RAM text/data address

BOOT_EXTRA      = hawkSmc.o hawkI2c.o sysMotVpdUtil.o sysMotVpd.o sysVxALib.o

#
# to get symTbl.o, 'gmake vxWorks.st'
# to build bootrom_uncmp.bin, remove symTbl.o
#

MACH_EXTRA	= hawkSmc.o hawkI2c.o sysMotVpdUtil.o intDisconnect.o symTbl.o

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx2400
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx2400.sym

installFD:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx2400_FD
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx2400_FD.sym

