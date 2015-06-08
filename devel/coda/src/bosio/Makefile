#
# Makefile for bosio/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = bosio


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

# if debugging
#FFLAGS = -g -KPIC -mt -DSunOS -e -ftrap=no%inexact,no%underflow
#CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(globallib)/libet.a
LIBNAMES += -lpthread
ifneq ("$(OSTYPE)","Darwin")
  LIBNAMES += -lrt
endif
LIBNAMES += -lresolv
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lcrypt
endif


#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp






























