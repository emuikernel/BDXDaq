#
# Makefile for rc/runControl/rcServer
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = rcServer


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

#for debugging
ifeq ("$(OSTYPE)","SunOS")
  CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
endif

CLON_FLAGS += -I./src.s -I./include -I../Variables -I../Common \
	-I./pixmaps -I$(CODA)/common/include -I../../cg/cg.s -I../../motif/include \
	-I../../sace3.1/include -I../../XmHTML-1.1.4/include $(X11INCDIR)

LIBNAMES += ../../sace3.1/$(OSTYPE_MACHINE)/lib/libsace3.1.a

LIBNAMES += $(globallib)/libmysql4.1.20.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSYSV -DSVR4 -D_CODA_2_0_T \
	-D_CODA_USE_THREADS -DDEBUG -Dsolaris  \
	-DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -DHAVE_MACROS_H=1 -DSTDC_HEADERS=1 \
	-DTK_FILE_COUNT=_cnt -Dstrtod=fixstrtod -DNEED_MATHERR=1
endif

ifeq ("$(OSTYPE)","Linux")
#  CLON_FLAGS += -D_CODA_2_0_T -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -DHAVE_MACROS_H=1 -DSTDC_HEADERS=1 \
#	-DTK_FILE_COUNT=_cnt -Dstrtod=fixstrtod -DNEED_MATHERR=1
#  CLON_FLAGS += -D_CODA_2_0_T -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -DHAVE_MACROS_H=1 -DSTDC_HEADERS=1 \
#	-DTK_FILE_COUNT=_cnt -DNEED_MATHERR=1
  CLON_FLAGS += -D_CODA_2_0_T -D_CODA_DALOG
endif

ifeq ("$(OSTYPE)","Darwin")
  CLON_FLAGS += -D_CODA_2_0_T -D_CODA_DALOG
endif

#sergey: remove remanents of tcl/tk
#############CLON_FLAGS += -DUSE_TK

#-------------------
# customize targets
#-------------------
install:
	cp $(localbin)/rcServer $(CODA)/$(OSTYPE_MACHINE)/bin/





























