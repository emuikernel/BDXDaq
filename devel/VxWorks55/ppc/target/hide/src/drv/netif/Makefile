# Makefile - makefile for src/drv/netif
#
# modification history
# --------------------
# 01s,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01r,12dec01,hbh  Added build of SM files for SimSolaris.
# 01q,09nov01,jmp  only build if_loop.o for simulators.
# 01p,25oct01,hbh  Removed if_ulip.c compilation.
# 01o,12oct01,tam  added repackaging support
# 01n,16mar99,dat  made if_cpm uniqie to CPU32 and PPC860
# 01m,10mar99,dat  made if_fn unique to SPARClite cpu, removed if_es
# 01l,02mar99,fle  fixed a problem introduced by last modif
# 01k,01mar99,dat  removed unsupported drivers for T2.0
# 01j,19aug98,fle  removed any reference to if_nicMesq.c
# 01i,01jun98,dat  fixed EXTRA_INCLUDE to use TGT_DIR
# 01h,21sep97,db   added if_nicMesq
# 01g,16jun97,map  added if_cs
# 01f,07may97,dat  added if_nicEvb.c to DOC_FILES
# 01e,30apr97,map  added if_eihk to DOC_FILES; made if_eihk, and if_eitp 
#                  CPU specific.
# 01e,24apr97,hk   added if_eidve.o to OBJS_COMMON.
# 01d,06dec96,dat  updated DOC_FILES, added if_qu, if_mbc
# 01c,25nov96,dat  updated DOC_FILES
# 01b,18nov96,dat  added if_lnPci.c to DOC_FILES, SPR #7481
# 01a,18jun96,yp   created from 02i of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#

TGT_DIR=$(WIND_BASE)/target

#
# Don't forget the special drivers, for special CPU types.
#
DOC_FILES= if_cpm.c if_ei.c \
	   if_fn.c if_ln.c if_lnPci.c if_loop.c \
	   if_sl.c if_sm.c if_sn.c smNetLib.c smNetShow.c \
	   if_elc.c if_dc.c if_ultra.c if_eex.c if_fei.c \
	   if_elt.c if_ene.c if_esmc.c \
	   if_mbc.c if_eihk.c if_nicEvb.c if_cs.c if_eidve.c 

LIB_BASE_NAME=drv

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS_COMMON = if_ei.o \
	      if_ln.o if_lnPci.o if_loop.o \
	      if_sl.o if_sm.o if_sn.o smNetLib.o smNetShow.o \
	      if_elc.o if_dc.o if_ultra.o if_eex.o if_fei.o \
	      if_elt.o if_ene.o if_esmc.o \
	      if_cs.o if_eidve.o


OBJS_MC68000 = if_mbc.o

OBJS_CPU32   = if_cpm.o
OBJS_PPC860  = if_cpm.o

OBJS_R3000   = if_eihk.o

OBJS_PPC403  = if_nicEvb.o

OBJS_SPARClite  = if_fn.o

OBJS         = $(OBJS_COMMON) $(OBJS_$(CPU))

#
# redefine OBJS for simulators since only some files need to be built
#
ifeq (SIMNT,$(CPU))
OBJS  = if_loop.o 
endif

ifeq (SIMSPARCSOLARIS,$(CPU))
OBJS = if_loop.o if_sm.o smNetLib.o smNetShow.o
endif

include $(TGT_DIR)/h/make/rules.library


CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
