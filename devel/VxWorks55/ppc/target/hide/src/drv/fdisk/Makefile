# Makefile - makefile for bsp/src/drv/hdisk
#
# modification history
# --------------------
# 01d,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01c,12oct01,tam  added repackaging support
# 01b,01jun98,dat  fixed EXTRA_INCLUDE to use TGT_DIR
# 01a,18jun96,yp   created from 01b of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=	nec765Fd.c

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS=	nec765Fd.o

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
