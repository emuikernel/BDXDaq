# Makefile - makefile for bsp/src/drv/parallel
#
# modification history
# --------------------
# 01c,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01b,12oct01,tam  added repackaging support
# 01a,03may00,ms   created from 01b of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=smcFdc37b78x.c

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS=

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries

