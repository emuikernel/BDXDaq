# Makefile - makefile for src/drv/timer
#
# modification history
# --------------------
# 01c,17jul02,dat  remove obsolete information
# 01b,12mar02,dat  SPR 72518 Drivers docs in separate directory
# 01a,14oct01,dat  written from VxAE 1.1
#
# DESCRIPTION
# This file contains the makefile rules for building the vx library
#
#*/

TGT_DIR=$(WIND_BASE)/target

# For VxWorks5, drivers are documented in the BSP ref. pages
DOC_FILES=

# For VxAE we document all driver components
# DOC_FILES= CIOTimer.c ambaTimer.c coldfireTimer.c dga001Timer.c \
#    ev960jxTimer.c i8253Timer.c i8254Timer.c loApicTimer.c lr33kTimer.c \
#    m5204Timer.c m5206Timer.c m68230Timer.c m68302Timer.c \
#    m68332Timer.c m68340Timer.c m68360Timer.c m68562Timer.c m68681Timer.c \
#    m68901Timer.c m8260Timer.c mb86940Timer.c mccTimer.c mipsR4kTimer.c \
#    ncr89105Timer.c nvr4101RTCTimer.c nvr4102RTCTimer.c \
#    pcc2Timer.c pccTimer.c ppc403Timer.c ppc555Timer.c ppc860Timer.c \
#    ppcDecTimer.c ppcZ8536Timer.c sa1100Timer.c sa1501Timer.c sh7032Timer.c \
#    sh7040Timer.c sh7055Timer.c sh7410Timer.c sh7604Timer.c sh7615Timer.c \
#    sh7700Timer.c simPcTimer.c sonicTimer.c sun4Timer.c sunEcTimer.c swTimer.c \
#    ultrasparcTimer.c vme2Timer.c z8036Timer.c z8536Timer.c \

LIBNAME=lib$(CPU)$(TOOL)vx.a
LIBDIRNAME=obj$(CPU)$(TOOL)vx

EXTRA_INCLUDE=-I$(TGT_DIR)/h/

OBJS=

include $(TGT_DIR)/h/make/rules.library
DOC_OPTS= -mg -fname

CC_OPTIM=$(CC_OPTIM_DRIVER)
DOC_DIR = $(DOCS_ROOT)/vxworks/ref/drv
DOC_CHAPTER=Driver_Libraries

