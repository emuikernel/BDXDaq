#
# Makefile for clon/prtr/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = prtr


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

# for debugging
#ifeq ("$(OSTYPE)","SunOS")
#  CFLAGS = -g -DSunOS -DOSTYPE="SunOS" -DSunOS_sun4u
#  CLON_FLAGS = 
#endif

# use C++ as linker instead of CC: we'll link main *.c program with
# SmartSocket's C++ libraries, and using CC will not resolve references
ifeq ("$(OSTYPE)","Linux")
  LDCC = $(CXX)
endif

CLON_FLAGS += -I./src.s \
			-I$(EPICS_BASE)/include \
			-I$(CODA)/common/include \
			-I$(CLON)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -I$(EPICS_BASE)/include/os/solaris
endif

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -I$(EPICS_BASE)/include/os/Linux
endif

#if want smartsockets
CLON_FLAGS += -DSSIPC -I$(RTHOME)/include

LIBNAMES += $(globallib)/libcmon.a

LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libcodatt.a
LIBNAMES += $(CODA_LIB)/libet.a

###LIBNAMES += $(CLON_LIB)/libclonutil.a

#if want epics ca
ifeq ("$(OSTYPE)","SunOS")
  LIBNAMES += -lca
endif
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += $(EPICS_BASE_LIB)/libcas.a
  LIBNAMES += $(EPICS_BASE_LIB)/libca.a
  LIBNAMES += $(EPICS_BASE_LIB)/libCom.a
  LIBNAMES += $(EPICS_BASE_LIB)/libgdd.a
endif

LIBNAMES += -lmysqlclient

#if want smartsockets
LIBNAMES += $(CLON_LIB)/libipc.a
LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50
ifeq ("$(OSTYPE)","SunOS")
  LIBNAMES += -lCrun
endif
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
  LIBNAMES += -lrt
endif

LIBNAMES += $(X11LIBS)
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += -lresolv
LIBNAMES += $(SYSLIBS)


#-------------------
# customize targets
#-------------------



#coda_pr: main/coda_prtr.c
#	$(CC) -o coda_pr.o $(CFLAGS) $(CLON_FLAGS) -DPRLIB $<



install: install_lib install_bin

#	rm -rf tmp
#	mkdir tmp
#	cp sda.inc/map_manager.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















