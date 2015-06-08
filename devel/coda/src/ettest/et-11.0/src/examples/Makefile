#
# ET examples makefile
#

ifndef BMS_HOME
  $(error "Need to define BMS_HOME")
endif

ifeq ($(BMS_OS), vxworks)
  $(error "This makefile is for unix, use Makefile.vxworks-ppc")
endif

# Setting TOPLEVEL tells Makefile.common where to find Makefile.local
# and to include it if make is run from this dir or the top-level package
# directory.
TOPLEVEL = ../..

PACKAGES = et
MODULE_NAME = et

# include files are in et/src directory
CFLAGS    += -I../libsrc
CXXFLAGS  += -I../libsrc

# Things common to all builds live in Makefile.common
include $(BMS_HOME)/Makefile.common

# list your main files here if you don't want all files with main compiled
# MAIN_FILES =

# Things for building executables
include $(BMS_HOME)/Makefile.bin

