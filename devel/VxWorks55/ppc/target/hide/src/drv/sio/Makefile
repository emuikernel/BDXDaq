# Makefile - makefile for bsp/src/drv/sio
#
# Copyright 1984-2002, Wind River Systems, Inc. Alameda, CA
#
# modification history
# --------------------
# 01q,16jul02,dat  remove obsolete information
# 01p,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01o,19nov01,pch  Move ppc860Sio.o from OBJS_COMMON to OBJS_PPC860
# 01n,16nov01,dgp  remove unixSio.c and winSio.c from doc build
# 01m,13nov01,jmp  removed unixSio (moved to solaris bsp). removed winSio
#                  (moved to simpc bsp).
# 01l,25oct01,tam  added repackaging support
# 01k,11oct01,dat  SPR 20403, using CC_OPTIM_DRIVER for all builds
# 01j,30aug01,pch  Move ppc403Sio.o from OBJS_COMMON to OBJS_PPC403
# 01i,07mar00,hk   added shSciSio.
# 01h,23mar99,zl   added ppc555SciSio.*
# 01g,19jan99,dra  added drivers from T2.* ES ports.
# 01f,16jul98,cym  added winSio.*.
# 01e,08may98,dbt  added unixSio.* removed during ARM merge.
# 01d,20jan98,cdp  added sa1100Sio.*.
# 01c,19aug97,jpd  added ambaSio.*.
# 01b,02dec96,cdp  added st16552Sio.*
# 01a,18jun96,yp   created from 01j of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES= z8530Sio.c i8250Sio.c m68360Sio.c cd2400Sio.c m68562Sio.c \
	   m68332Sio.c mb86940Sio.c ns16550Sio.c \
	   nvr4101DSIUSio.c nvr4101SIUSio.c nvr4102DSIUSio.c \
	   ppc403Sio.c ppc555SciSio.c ppc860Sio.c \
	   evbNs16550Sio.c m68681Sio.c m68302Sio.c m68901Sio.c \
	   sab82532Sio.c st16552Sio.c ambaSio.c sa1100Sio.c \
	   coldfireSio.c shSciSio.c shScifSio.c

LIB_BASE_NAME	= drv

OBJS_COMMON =  z8530Sio.o i8250Sio.o m68360Sio.o cd2400Sio.o m68562Sio.o \
        m68332Sio.o mb86940Sio.o ns16550Sio.o \
        evbNs16550Sio.o m68681Sio.o m68302Sio.o m68901Sio.o \
        sab82532Sio.o st16552Sio.o ambaSio.o sa1100Sio.o unixSio.o winSio.o \
	coldfireSio.o shSciSio.o shScifSio.o

OBJS_MIPS32 = nvr4101DSIUSio.o nvr4101SIUSio.o nvr4102DSIUSio.o
OBJS_MCF5200 = m5206Sio.o m5204Sio.o
OBJS_PPC555 = ppc555SciSio.o
OBJS_PPC403 = ppc403Sio.o
OBJS_PPC860 = ppc860Sio.o

OBJS = $(OBJS_COMMON) $(OBJS_$(CPU))

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries
