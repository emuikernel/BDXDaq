#
# Makefile for clon/cmon/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = cmon


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

## # '-fast' invokes '-xvector=yes', as result undefined symbols
## ifeq ("$(OSTYPE)","SunOS")
##   FFLAGS += -xvector=no
## endif

# if debugging
#ifeq ("$(OSTYPE)","SunOS")
#  FFLAGS = -g -KPIC -mt -DSunOS -e -ftrap=no%inexact,no%underflow
#  CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
#endif

CLON_FLAGS += -I./$(MAIN).s \
			-I./sda.inc -I./sim.inc -I./recsis.inc \
			-I$(CODA)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libcodatt.a
LIBNAMES += $(globallib)/libminuit.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin

	rm -rf tmp
	mkdir tmp
	cp sda.inc/map_manager.h tmp
	cp sda.inc/prlib.h tmp
	cp sda.inc/dclib.h tmp
	cp sda.inc/sclib.h tmp
	cp sda.inc/wcs.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















