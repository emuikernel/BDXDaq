# Makefile - makefile for bsp/config/prpmc800
#
# Copyright 1984-2001 Wind River Systems, Inc.
# Copyright 1996-2001 Motorola, Inc. All Rights Reserved
#
# modification history
# --------------------
# 01i,16jul02,dtr  Adding bootrom.bin to RELEASE build.
# 01h,25mar02,dtr  Removing HEX_FLAGS.
# 01g,08dec01,dtr  Driver in this release so patch prpmc800Drv.a removed. 
# 01f,29nov01,dtr  Adding fei driver object via prpmc800Drv.a.
# 01e,27nov01,dtr  Removed gei/fei/dec driver objects.
# 01d,15aug01,srr  Included Gigabit Ethernet driver object.
# 01c,11oct00,dmw  Added i8250Sio.o to get latest WRS version.
# 01b,07sep00,dmw  Changed harrierI2c.o to sysMotI2c.o.
# 01a,31jul00,dmw  Written (from version 01b of mcpn765/Makefile).
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# Motorola PrPMC800 board with a PowerPC 750 or 7400 processor.
#
# INCLUDES
#     makeTarget
#

CPU              = PPC604
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target







#################################################
#################################################
#################################################

###include $(TGT_DIR)/h/make/defs.bsp





# map TOOL to TOOL_FAMILY, if not already done in BSP Makefile

ifeq ($(TOOL_FAMILY),)
ifeq  ($(findstring gnu,$(TOOL)),gnu)
TOOL_FAMILY	= gnu
else
ifeq  ($(findstring diab,$(TOOL)),diab)
TOOL_FAMILY	= diab
endif
endif
endif

.SUFFIXES:  .cpp .out .cxx

## exe target is the cannonical starting point.

default : exe

## most targets will make vxWorks by default

DEFAULT_RULE	= vxWorks

## TARGET_DIR still used for backward compatibility

BSP_NAME	= $(TARGET_DIR)

## default flags

CFLAGS		= $(CC_ARCH_SPEC) $(CC_COMPILER) $(CC_OPTIM) $(CC_WARNINGS) \
		  $(CC_INCLUDE) $(CC_DEFINES) $(ADDED_CFLAGS)  \
		  $(CC_SOFT_FLOAT)

C++FLAGS	= $(CC_ARCH_SPEC) $(C++_COMPILER) $(CC_OPTIM) $(C++_WARNINGS) \
		  $(CC_INCLUDE) $(CC_DEFINES) $(ADDED_C++FLAGS) \
		  $(CC_SOFT_FLOAT)

CASFLAGS	= $(CC_ARCH_SPEC) $(OPTION_PP) $(OPTION_LANG_ASM) $(CC_INCLUDE) \
	          $(CC_DEFINES)

CFLAGS_AS	= $(CC_ARCH_SPEC) $(CC_COMPILER) $(CC_OPTIM) $(CC_INCLUDE) \
		  $(CC_DEFINES) $(ADDED_CFLAGS) $(OPTION_PP_AS) \
		  $(OPTION_LANG_ASM)

# Special flags for Position Independent Code (bootInit.o)
CFLAGS_PIC	= $(CC_ARCH_SPEC) $(CC_COMPILER) $(CC_OPTIM) $(CC_WARNINGS) \
		  $(CC_INCLUDE) $(CC_DEFINES) $(ADDED_CFLAGS) $(CC_PIC) \
		  $(CC_SOFT_FLOAT)

CC_WARNINGS	= $(CC_WARNINGS_ALL)

CC_OPTIM	= $(CC_OPTIM_TARGET)

CC_INCLUDE	= $(OPTION_INCLUDE_DIR)$(UP)/h $(INCLUDE_CC) $(EXTRA_INCLUDE) \
		  $(OPTION_INCLUDE_DIR). $(OPTION_INCLUDE_DIR)$(CONFIG_ALL) \
		  $(OPTION_INCLUDE_DIR)$(TGT_DIR)/h \
		  $(OPTION_INCLUDE_DIR)$(TGT_DIR)/src/config \
		  $(OPTION_INCLUDE_DIR)$(TGT_DIR)/src/drv

CC_DEFINES	= $(OPTION_DEFINE_MACRO)CPU=$(CPU) \
		  $(OPTION_DEFINE_MACRO)TOOL_FAMILY=$(TOOL_FAMILY) \
		  $(OPTION_DEFINE_MACRO)TOOL=$(TOOL) \
		  $(DEFINE_CC) $(EXTRA_DEFINE)

COMPILE_SYMTBL	= $(CC) $(OPTION_OBJECT_ONLY) $(OPTION_DOLLAR_SYMBOLS) $(CFLAGS)

## compiler and assembler flags for the project facility

CFLAGS_PROJECT_REMOVE = $(CC_PROJECT_OPTIMIZE_FLAGS) \
						$(PROJECT_BSP_FLAGS_EXTRA)
CFLAGS_PROJECT_STRIPPED = $(filter-out $(CFLAGS_PROJECT_REMOVE),$(C++FLAGS))
CFLAGS_PROJECT = -g $(CFLAGS_PROJECT_STRIPPED)

CFLAGS_AS_PROJECT_STRIPPED = $(filter-out $(CFLAGS_PROJECT_REMOVE),$(CFLAGS_AS))
CFLAGS_AS_PROJECT = -g $(CFLAGS_AS_PROJECT_STRIPPED)

## set default values for ROM_LINK_ADRS, TGT_DIR, for old Makefiles

ifeq ($(ROM_LINK_ADRS),)
ROM_LINK_ADRS = $(ROM_TEXT_ADRS)
endif

ifeq ($(TGT_DIR),)
TGT_DIR	= $(WIND_BASE)/target
endif

## standard list of BSP modules

###Sergey MACH_DEP	= sysALib.o sysLib.o $(MACH_EXTRA) $(ADDED_MODULES)
MACH_DEP	= $(MACH_EXTRA) $(ADDED_MODULES)


## linker script definitions

include $(TGT_DIR)/h/make/defs.link

## documentation files

DOC_FILES	= sysLib tyCoDrv sysTffs mkboot
DOC_FLAGS	= $(OPTION_DEFINE_MACRO)DOC $(OPTION_DEFINE_MACRO)INCLUDE_SCSI \
		  $(OPTION_DEFINE_MACRO)INCLUDE_SHOW_ROUTINES $(EXTRA_DOC_FLAGS)

# Now include the CPU/TOOL make file and any host specific definitions

include $(TGT_DIR)/h/tool/$(TOOL_FAMILY)/make.$(CPU)$(TOOL)
include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)


## Release macros are for WRS internal release process only.

RELEASE		= $(RELEASE_PRE) $(RELEASE_CMD) $(RELEASE_PRJ) $(RELEASE_POST)
RELEASE_PRE	=
RELEASE_POST	=

## Default project builds, arch sensitive

# ARM, MIPS, PPC, SH
# build 2 projects, gnu project has 4 build specs, diab project also has 4
# (These should be in the arch specific tool files, not here)

ifeq ($(VX_CPU_FAMILY),arm)
    RELEASE_PRJ	= prj_default prj_diab
endif

ifeq ($(VX_CPU_FAMILY),mips)
    RELEASE_PRJ	= prj_default prj_diab
endif

ifeq ($(VX_CPU_FAMILY),ppc)
    RELEASE_PRJ	= prj_default prj_diab
endif

ifeq ($(VX_CPU_FAMILY),sh)
    RELEASE_PRJ	= prj_default prj_diab
endif

# Simulators: one project with just single build spec, 

ifeq ($(VX_CPU_FAMILY),simpc)
    RELEASE_PRJ	= prj_default_one
endif

ifeq ($(VX_CPU_FAMILY),simso)
    RELEASE_PRJ	= prj_default_one
endif

# all else defaults to a single project with 4 build specs

ifeq ($(RELEASE_PRJ),)
    RELEASE_PRJ = prj_default
endif

# default command line builds

ifeq ($(RELEASE_CMD),)
    RELEASE_CMD = vxWorks vxWorks.st bootrom.hex
endif







#################################################
#################################################
#################################################



## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.


TARGET_DIR	= prpmc800
VENDOR		= Motorola
BOARD		= PRPMC800

EXTRA_DEFINE    = -DPRPMC800

USR_ENTRY       = usrInit

RELEASE	       += bootrom.bin

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, and Makefile.
# All definitions for these constants must be identical.
#

ROM_TEXT_ADRS	= fff00100 # ROM entry address
ROM_SIZE	= 00100000 # number of bytes of ROM space

RAM_LOW_ADRS	= 00100000 # RAM text/data address
RAM_HIGH_ADRS	= 00800000 # RAM text/data address

BOOT_EXTRA      = harrierSmc.o sysMotI2c.o sysMotVpdUtil.o sysMotVpdALib.o
#romInit.o romStart.o


#
# to get symTbl.o, 'gmake vxWorks.st'
# to build bootrom_uncmp.bin, remove symTbl.o
#
MACH_EXTRA      = 	sysALib.o sysLib.o harrierSmc.o sysMotI2c.o sysMotVpdUtil.o \
					sysMotVpdALib.o sysMaxL2Disable.o \
				intDisconnect.o
#					symTbl.o

# mvme5500:
# PRJ_OBJS       = sysALib.o sysLib.o sysMotI2c.o sysMv64260I2c.o \
#					sysMv64260Smc.o sysAUtil.o usrAppInit.o prjConfig.o \
#					linkSyms.o intDisconnect.o
                 

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)


#

install:
	cp vxWorks $(CLON_ROOT)/share/vxworks/kernel/vx800
	cp vxWorks.sym $(CLON_ROOT)/share/vxworks/kernel/vx800.sym

install5500:
	cp vxWorks $(CLON_ROOT)/share/vxworks/kernel/vx800mv5500
	cp vxWorks.sym $(CLON_ROOT)/share/vxworks/kernel/vx800mv5500.sym

#