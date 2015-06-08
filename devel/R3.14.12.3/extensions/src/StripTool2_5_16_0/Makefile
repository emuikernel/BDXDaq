#/*************************************************************************
# Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 1997-2003 Southeastern Universities Research Association,
# as Operator of Thomas Jefferson National Accelerator Facility.
# Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
# Gemelnschaft (DESY).
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#**************************************************************************

#******* ADD for Archive record support Albert ****************
# STRIP_HISTORY is StripHistoryAR+ArR.c StripHistoryLANL.cc StripHistoryNULL.c 
# ARCHIVER_CALL (CAR,AAPI,NONE)
# USE_ARCHIVE_RECORD
#       if IOCs support Archive record (History cache at IOC)
#	more recent archive data coming from IOC more older data
#	comming from Unix Archiver
# 
#first you have to choose STRIP_HISTORY:
#StripHistoryNULL.c ---- no history at all.
#StripHistoryLANL.cc --- Kay Kazemir (summer 2000) (not so strong) codes
#StripHistoryAR+ArR.c -- More robust :) (current version from DESY) 
#
#Second 2 variables working only with StripHistoryAR+ArR.c
# ARCHIVER_CALL 2 nontrivial situations:
# a) CAR -- local request for all history data without any reductions
#           good enough but please note that data for one hour can have 1000000
#           points!
# b) AAPI -- network request with data reductions (average, sharp,raw,splines) 
#         -- better than local, but need to start AAPI-server. 
# c) NONE no history if noUSE_ARCHIVE_RECORD , short history otherwise
#
# USE_ARCHIVE_RECORD
#     if you choose YES more recent archive data will be coming from 
#     IOC-cache.
# 

TOP=../..
include $(TOP)/configure/CONFIG

# ==========================================================================
# Options
# ==========================================================================

USE_CLUES	?= YES
USE_SDDS	?= NO

STRIP_HISTORY      ?= StripHistoryAR+ArR.c
ARCHIVER_CALL      ?= NONE
USE_ARCHIVE_RECORD ?= NO

ifdef WIN32
HAVE_XPM        = NO
else
HAVE_XPM	?= YES
endif
HAVE_XMU	?= YES

# these libraries don't reside in the X11 directories in HP-UX, so they are
# only set here if not defined via an included file.
XMU_INC		?= $(X11_INC)
XMU_LIB		?= $(X11_LIB)
XPM_INC		?= $(X11_INC)
XPM_LIB		?= $(X11_LIB)

# Purify
ifeq ($(OS_CLASS),solaris)
#PURIFY=YES
#PURIFY_FLAGS = -first-only -chain-length=50
#PURIFY_FLAGS = -first-only -chain-length=26 -max_threads=256
#PURIFY_FLAGS = -first-only -chain-length=26 -always-use-cache-dir -cache-dir=/tmp/purifycache
#PURIFY_FLAGS = -first-only -chain-length=26 -enable-new-cache-scheme
#PURIFY_FLAGS = -first-only -chain-length=26 -enable-new-cache-scheme -always-use-cache-dir -cache-dir=/tmp/purifycache
# Put the cache files in the appropriate bin directory
#PURIFY_FLAGS += -always-use-cache-dir -cache-dir=$(shell $(PERL) $(EPICS_BASE_BIN)/fullPathName.pl .)

# Don't use SDDS when debugging with purify
#USE_SDDS=NO
# Uncomment following 2 lines to link with purify on solaris build
#DEBUGCMD = purify $(PURIFY_FLAGS)
#HOST_OPT=NO
endif

# libXm.so.4 Purify debugging for Sun
# Change directory before enabling
#ifeq ($(PURIFY),YES)
#USR_LDFLAGS += -R/home/phoebus/EVANS/scratch/MotifMLKProblem080603
#endif

CMPLR 		= STRICT

# When committed, HOST_OPT should be undefined.  Use for debugging.
ifdef WIN32
# HOST_OPT = NO
else
# HOST_OPT = NO
endif

# Use source browser on Solaris
#ifeq ($(HOST_OPT),NO)
#  ifeq ($(ANSI),ACC)
#    ifeq ($(OS_CLASS),solaris)
#      USR_CFLAGS += -xsb
#    endif
#  endif
#endif

#Temporary
#ifdef WIN32
#USR_CFLAGS += -P
#endif

STRIP_DAQ	= StripCA.c
#STRIP_HISTORY	= StripHistoryNULL.c

ifeq ($(STRIP_DAQ),StripCDEV.cc)
  USE_CDEV	= YES
endif

# Note that StripTool will ding without explanation on startup if
# STRIP_CONFIGFILE_DIR is invalid.  If STRIP_CONFIGFILE_DIR is not
# defined here, then the value in StripDefines.h will be used.
#STRIP_CONFIGFILE_DIR=\"/applic/stripTool\"

# WIN32
WIN32_RUNTIME=MD
USR_CFLAGS_WIN32 += -DWIN32 -D_WINDOWS
USR_LDFLAGS_WIN32 += /SUBSYSTEM:WINDOWS
USR_LIBS_WIN32 += $(EXCEED_XLIBS)
USR_CFLAGS_WIN32 += $(EXCEED_CFLAGS)

# ==========================================================================
# Site-specific defines
# ==========================================================================
#USR_CPPFLAGS += -DSTRIP_PRINTER_NAME=\"mcc104c\"
#USR_CPPFLAGS += -DSTRIP_PRINTER_DEVICE=\"pjetx\"
#USR_CPPFLAGS += -DSTRIP_SITE_DEFAULTS_FILE=\"/cs/op/lib/stripTool/.StripToolrc\"

# ==========================================================================
# Sources
# ==========================================================================
SRCS		+= $(STRIP_HISTORY)
SRCS		+= StripConfig.c
SRCS		+= StripCurve.c
SRCS		+= Strip.c
SRCS		+= StripDialog.c
SRCS		+= StripDataSource.c
SRCS		+= StripGraph.c
SRCS		+= StripMisc.c
SRCS		+= cColorManager.c
SRCS		+= ColorDialog.c
SRCS		+= StripTool.c
SRCS		+= $(STRIP_DAQ)
SRCS		+= jlAxis.c
SRCS		+= jlLegend.c
SRCS		+= ComboBox.c
SRCS		+= FastLabel.c
SRCS		+= Tabs.c
SRCS		+= x_rotate.c
SRCS		+= rotated.c
SRCS		+= browserHelp.c
SRCS		+= Annotation.c

ifeq ($(USE_CLUES), YES)
  SRCS		+= LiteClue.c
endif

# if any of the source modules are C++, then we need the
# C++ version of main
ifneq ($(findstring .cc, $(SRCS)),)
  SRCS		+= main_+.cc
else
  SRCS		+= main_c.c
endif
SRCS		+= $(MAIN)

ifeq ($(STRIP_HISTORY), StripHistoryAR+ArR.c)
  SRCS		+= getHistory.c
  ifeq ($(USE_ARCHIVE_RECORD), YES)
	USR_CFLAGS	+= -DSTRIP_HISTORY	
	SRCS		+= getArchiveRecord.c
        USR_CFLAGS      += -DUSE_ARCHIVE_RECORD
  endif	
  ifeq ($(ARCHIVER_CALL), CAR)
	USR_CFLAGS	+= -DUSE_CAR
	HISTORY_API	= CAR
        USR_CFLAGS	+= -DSTRIP_HISTORY
	SRCS		+= StripHistoryLANLmodif.cc
  endif	
  ifeq ($(ARCHIVER_CALL), AAPI)
	SRCS		+= get_AAPI_data.c
	USR_CFLAGS	+= -DSTRIP_HISTORY
	USR_CFLAGS	+= -DUSE_AAPI
        USR_LDLIBS      +=  -lAAPI_client  -lAAPI_common
  endif
  ifeq ($(ARCHIVER_CALL), NONE)
	USR_CFLAGS	+= -DNO_ARCHIVER_CALL
  endif		
endif

USR_INCLUDES = -I$(MOTIF_INC) -I$(X11_INC) -I$(XMU_INC) -I$(XPM_INC)

# ==========================================================================
# Preprocessor Flags
# ==========================================================================
ifneq ($(STRIP_CONFIGFILE_DIR),)
  USR_CPPFLAGS 	+= -DSTRIP_CONFIGFILE_DIR=$(STRIP_CONFIGFILE_DIR)
endif

USR_CPPFLAGS 	+= -DUSE_MOTIF
#USR_CPPFLAGS	+= -DDESY_PRINT # use xwpick instead of xwd 

ifeq ($(USE_CLUES), YES)
  USR_CPPFLAGS	+= -DUSE_CLUES
endif

ifeq ($(HAVE_XPM), YES)
  USR_CPPFLAGS	+= -DUSE_XPM
endif

ifeq ($(HAVE_XMU), YES)
  USR_CPPFLAGS	+= -DUSE_XMU
endif

ifeq ($(HISTORY_API), CAR)
#  USR_INCLUDES		+= -I$(CAR_DIR)/include
USR_INCLUDES		+= -I$(EPICS)/extensions/include 
USR_INCLUDES		+= -I$(EPICS)/base/src/ca/
  ifeq ($(CCC), aCC)
    USR_INCLUDES	+= -I$(CAR_DIR)/include/hp
  endif
endif

ifeq ($(THREADED), YES)
  ifeq ($(CCC), aCC)
    USR_CFLAGS		+= $(cc_THREAD_FLAGS)
    USR_CXXFLAGS	+= $(aCC_THREAD_FLAGS)
    THREAD_LIB		= $(hp10_THREAD_LIB)
  endif
endif

ifeq ($(USE_CDEV), YES)
  USR_INCLUDES		+= -I$(CDEV_INC)
  USR_CPPFLAGS		+= -DUSE_CDEV
endif

ifeq ($(USE_SDDS), YES)
  USR_CPPFLAGS		+= -DUSE_SDDS
endif

# ==========================================================================
# Libraries
# ==========================================================================
ifeq ($(HISTORY_API), CAR)
#  PROD_LIBS += ChanArchIO Tools
  USR_LIBS_DEFAULT += ChanArchIO Tools ca Com pthread posix4
#  USR_LDFLAGS += -L$(EPICS)/extensions/lib/$(T_A)
endif

# Note: order is important

# Default Motif library location
USR_LIBS_DEFAULT += Xm
Xm_DIR = $(MOTIF_LIB)
ifeq ($(MOTIF_VERSION),2)
  USR_LIBS += Xext Xp
  Xp_DIR = $(X11_LIB)
  Xext_DIR = $(X11_LIB)
endif

# Default Xt library locations
USR_LIBS_DEFAULT += Xt
Xt_DIR = $(X11_LIB)

ifeq ($(HAVE_XMU), YES)
#  PROD_LIBS += Xmu
  USR_LIBS_DEFAULT += Xmu
#  USR_LDFLAGS += -L$(XMU_LIB)
  Xmu_DIR = $(XMU_LIB)
endif

ifeq ($(HAVE_XPM), YES)
#  PROD_LIBS += Xpm
  USR_LIBS_DEFAULT += Xpm
#  USR_LDFLAGS += -L$(XPM_LIB)
  Xpm_DIR = $(XPM_LIB)
endif

# Default X library location
USR_LIBS_DEFAULT += X11
X11_DIR = $(X11_LIB)

ifeq ($(USE_CDEV), YES)
  USR_LIBS += cdev
endif

ifeq ($(USE_SDDS), YES)
  SYS_PROD_LIBS_solaris += sunmath
  USR_LIBS += SDDS1 lzma mdblib $(ZLIB_PROD_LIB)
ifeq ($(ZLIB_PROD_LIB),)
  PROD_SYS_LIBS = z
endif
  SDDS1_DIR = $(EPICS_EXTENSIONS_LIB)
  lzma_DIR = $(EPICS_EXTENSIONS_LIB)
  mdblib_DIR = $(EPICS_EXTENSIONS_LIB)
endif

#USR_LIBS += ca Com
#USR_LIBS += $(THREAD_LIB)

USR_LIBS += ca Com
USR_LIBS += $(THREAD_LIB)

# ==========================================================================
# WIN32 Resources
# ==========================================================================
RCS_WIN32 += StripTool.rc

# ==========================================================================
# Product
# ==========================================================================
PROD_HOST := StripTool

# ==========================================================================
# Rules
# ==========================================================================
include $(TOP)/configure/RULES


../main_+.cc: main_c.c
	$(CP) $< $@

StripTool.res:../StripTool.ico

# ==========================================================================
# Debugging (Do gnumake xxxx in O.whatever directory)
# ==========================================================================
xxxx:
	@echo USR_LDFLAGS: $(USR_LDFLAGS)
	@echo LDFLAGS: $(LDFLAGS)
	@echo STRIP_CONFIGFILE_DIR: $(STRIP_CONFIGFILE_DIR)
	@echo EPICS_EXTENSIONS_LIB: $(EPICS_EXTENSIONS_LIB)
	@echo EPICS_EXTENSIONS_INCLUDE: $(EPICS_EXTENSIONS_INCLUDE)
	@echo XPM_LIB: $(XPM_LIB)
	@echo HAVE_XPM: $(HAVE_XPM)
	@echo z_DIR: $(z_DIR)
	@echo ZLIB_FOUND: $(ZLIB_FOUND)
	@echo RUNTIME_LDFLAGS: $(RUNTIME_LDFLAGS)
	@echo BASE_3_14: $(BASE_3_14)
	@echo FULL_PATH_INSTALL_LIB: $(FULL_PATH_INSTALL_LIB)

# **************************** Emacs Editing Sequences *****************
# Local Variables:
# mode: makefile
# End:
