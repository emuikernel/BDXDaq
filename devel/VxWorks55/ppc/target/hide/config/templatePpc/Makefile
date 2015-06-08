# Makefile - makefile for template
#
# Copyright 1984-2002 Wind River Systems, Inc.
#
# TODO - Remove the template modification history and begin a new history
#        starting with version 01a and growing the history upward with
#	 each revision.
#
# modification history
# --------------------
# 01g,29may02,dat  fixing NETWORK dependencies, 77135
# 01f,08dec01,dat  Adding templateEnd.obj to MACH_EXTRA
# 01e,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01d,11oct01,dat  Update for T2.2
# 01c,28mar01,pch  Set HEX_FLAGS to empty for new GNU objcopyppc
# 01b,07nov98,dat  chg ROM_WARM_ADRS offset to 0x8, not 0x4.
# 01a,07jul97,dat  written (from template68k/Makefile,ver01b)
#
# TODO - Update documentation.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the template.
#
# INCLUDES
#     makeTarget

CPU              = PPC603
TOOL             = gnu

TGT_DIR = $(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

# TODO - default files are sysLib.c, sysTffs.c and tycoDrv.c. Specify here the
# files to be documented bye make man. If the default files are OK, remove the
# DOC_FILES variable below

DOC_FILES	=

# TODO - Replace these examples with appropriate values.

TARGET_DIR	= templatePpc
VENDOR		= Generic
BOARD		= Model-1

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h and Makefile.
# All definitions for these constants must be identical.
# Specify values in hexadecimal, without a leading 0x.
#

# TODO - Setup the appropriate values here and in config.h

ROM_TEXT_ADRS	= fff00100 # ROM cold entry address
ROM_WARM_ADRS	= fff00108 # ROM warm entry address
ROM_SIZE	= 00080000 # number of bytes of ROM space
RAM_LOW_ADRS	= 00030000 # RAM text/data address for vxWorks
RAM_HIGH_ADRS	= 00200000 # RAM text/data address for bootrom

USR_ENTRY	= usrInit

# TODO - Remove the following line, it is for internal WRS use only.
RELEASE_PRE	= mk.templateDrvrs.a

# TODO - The following is only for the template BSP, it will usually be
# removed.
# For template BSPs, the templateEnd driver module is in a BSP specific
# archive, not the normal archive.
LIB_EXTRA	= templateDrvrs.a

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

# TODO - Add any additional make targets or rules below this point.

# TODO - Remove this special make target.
# Rather than put the template driver in the real archive, we put it here in
# the template BSP.  This is not the usual way we link to a network driver.

mk.templateDrvrs.a: $(TGT_DIR)/src/drv/end/templateEnd.c
	$(CC) $(CFLAGS) -o templateEnd.o $(OPTION_OBJECT_ONLY) \
		$(TGT_DIR)/src/drv/end/templateEnd.c
	$(AR) crus templateDrvrs.a templateEnd.o
	$(RM) templateEnd.o
