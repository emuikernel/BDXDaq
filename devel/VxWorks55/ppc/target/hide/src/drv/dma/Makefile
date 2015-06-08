# Makefile - makefile for bsp/src/drv/dma
#
# modification history
# --------------------
# 01c,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01b,12oct01,tam  added repackaging support
# 01a,18jun96,yp   created from 01d of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(WIND_BASE)/target/h/

OBJS=	l64853Dma.o i8237Dma.o


include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
