# Makefile - makefile for bsp/src/drv/parallel
#
# modification history
# --------------------
# 01d,12mar02,dat  SPR 72518 Drivers docs in separate directory, added
#		   supported pci libraries to DOC_FILES
# 01c,16nov01,dgp  Use iPIIX4.c instead of iPiix4Pci.c.
# 01b,12oct01,tam  added repackaging support
# 01a,03may00,ms   created from 01b of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=iPIIX4.c pciAutoConfigLib.c pciConfigLib.c pciConfigShow.c \
	pciIntLib.c

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS=

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries

