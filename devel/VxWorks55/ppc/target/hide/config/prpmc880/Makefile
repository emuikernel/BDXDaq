# Makefile - makefile for bsp/config/prpmc880
#
# Copyright 1984-1999 Wind River Systems, Inc.
# Copyright 1996-2003 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01a,04apr03,simon Ported. from ver 01a, mcpm905/Makefile 
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola PRPMC880 board with a PowerPC 7447 processor.
#
# INCLUDES
#     makeTarget
#

CPU              = PPC604
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR	= prpmc880
VENDOR		= Motorola
BOARD		= PRPMC880

EXTRA_DEFINE    = -DPRPMC880

## The following added compilation flags must be commented out when building
## with the DIAB compiler.

#ADDED_CFLAGS    = -pedantic -Wall -W -Wmissing-declarations \
#                  -Wstrict-prototypes -Wmissing-prototypes -i./

USR_ENTRY       = usrInit

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, and Makefile.
# All definitions for these constants must be identical.
#

# Sergey: 2 following lines were modified
ROM_TEXT_ADRS = f5f00100 # ROM entry address
ROM_WARM_ADRS = f5f00104 # ROM warm entry address
###Sergey ROM_TEXT_ADRS = fff00100 # ROM entry address
###Sergey ROM_WARM_ADRS = fff00104 # ROM warm entry address


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

MACH_EXTRA      = sysMv64360Smc.o sysAUtil.o sysMv64360I2c.o sysMotI2c.o \
		  mv64360End.obj 

RELEASE         = bootrom_uncmp vxWorks vxWorks.st bootrom

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

#

install:
	cp vxWorks $(CODA)/VXWORKS_ppc/bin/vx880
	cp vxWorks.sym $(CODA)/VXWORKS_ppc/bin/vx880.sym

#sudo cp bootrom.bin /tftpboot/boot_prpmc880.bin
