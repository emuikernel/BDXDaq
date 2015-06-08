# Makefile - makefile for bsp/src/drv/hdisk
#
# modification history
# --------------------
# 01f,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01e,15jan02,jkf  SPR#72617, removing 01d kludge.
# 01d,12oct01,tam  added repackaging support
# 01c,21jun00,rsh  upgrade to dosFs 2.0
# 01d,26oct99,jkf  DOC_OPTS = $(DOC_OPTS_MAN) -category DOSFS2
# 01c,08oct99,jkf  added a kludge to get this into the T2 component
#                  documentation directory "dosfs2".  This change should be
#                  be removed for T3, since dosfs2 will no longer be a
#                  component release.
# 01b,01jun98,dat  fixed EXTRA_INCLUDE to use TGT_DIR
# 01a,18jun96,yp   created from 01c of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=	ataDrv.c ataShow.c

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS=	ideDrv.o ataDrv.o ataShow.o

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
