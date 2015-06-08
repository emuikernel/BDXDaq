# Makefile - makefile for bsp/src/drv/sound
#
# Copyright 1984-1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01e,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01d,25oct01,tam  added repackaging support
# 01c,11oct01,dat  SPR 20403, CC_OPTIM_DRIVER
# 01b,28apr01,mem  Disabled docs build.
# 01a,18jun96,jk   created.
#
# DESCRIPTION
#

TGT_DIR = $(WIND_BASE)/target

#DOC_FILES = sb16drv.c

LIB_BASE_NAME	= drv

EXTRA_INCLUDE = -I$(TGT_DIR)/src/drv/sound

OBJS = sb16drv.o

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM = $(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
