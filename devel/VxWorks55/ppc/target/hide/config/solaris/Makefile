# Makefile - makefile for bsp/config/solaris
#
# modification history
# --------------------
# 01j,08may02,sn   SPR 76627 - moved LD definitions to make.SIMSPARCSOLARISgnu
# 01i,18apr02,jmp  added unixSio.c to DOC_FILES (SPR #75746).
# 02i,15nov01,jmp  removed tyCoDrv.o from MACH_EXTRA. added bsp specific
#                  driver rule to LIB_EXTRA.
# 02h,14nov01,dat  RELEASE update
# 02g,07nov01,tam  updated for repackaging
# 02f,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 02e,25sep01,dat  Update for T2.2
# 02d,29jun01,mem  Increase HIGH_ADRS
# 02c,22may96,dat  SPR 6141, use of TGT_DIR
# 02b,22may96,ism  changed LD_PARTIAL_FLAGS for vxWorks.st fix
# 02a,18dec95,ism  Changed for SIMSPARCSOLARIS
# 01d,28jun95,jcf  added example RAM_ defines.
# 01c,14jun95,wrs  updated to 5.3 with bspTo5.3
# 01b,22may95,jcf  wdb integration.
# 01a,23jan93,gae  written.
#
# DESCRIPTION
# This file contains rules for building VxWorks for
# sunos.
#
# NOTE
# Only vxWorks and vxWorks.st images supported.
#
# INCLUDES
#     makeTarget
#*/

CPU              = SIMSPARCSOLARIS
TOOL             = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

DOC_FILES	 = sysLib unixSio

TARGET_DIR       = solaris
VENDOR           = Sun
BOARD            = SunOS5_x

ROM_TEXT_ADRS    =
ROM_SIZE         =

RAM_LOW_ADRS     = 00020000 # example adrs of RAM start of code
RAM_HIGH_ADRS    = 0f000000 # example RAM entry address for ROM startup

MACH_EXTRA       =

LD_LOW_FLAGS     = -static

LIB_EXTRA = $(TGT_DIR)/config/solaris/solarisDrv.a	# Bsp specific drvs.

$(TGT_DIR)/config/solaris/solarisDrv.a : $(TGT_DIR)/config/solaris/unixSio.c $(TGT_DIR)/config/solaris/unixSio.h
	$(RM) $(TGT_DIR)/config/solaris/solarisDrv.a $(TGT_DIR)/config/solaris/unixSio.o
	$(MAKE) "CC_OPTIM=$(CC_OPTIM_DRIVER)" $(TGT_DIR)/config/solaris/unixSio.o
	$(AR) cru $(TGT_DIR)/config/solaris/solarisDrv.a $(TGT_DIR)/config/solaris/unixSio.o
	$(RANLIB) $(TGT_DIR)/config/solaris/solarisDrv.a

RELEASE = vxWorks vxWorks.st bsp2prj

## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)
