#
# Makefile for clon/epicsutil/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = epicsutil


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./$(MAIN).s -I$(EPICS_BASE)/include

ifeq ("$(OSTYPE)","SunOS")
  CFLAGS = -g -DSunOS -DOSTYPE="SunOS" -DSunOS_sun4u
  CLON_FLAGS += -I$(EPICS_BASE)/include/os/solaris
endif
ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -I$(EPICS_BASE)/include/os/Linux
endif

CLON_FLAGS +=	-I$(CODA)/common/include \
				-I$(CLON)/common/include

CLON_FLAGS += -I$(RTHOME)/include


LIBNAMES += $(globallib)/libipc.a
LIBNAMES += $(globallib)/libclonutil.a
LIBNAMES += $(CODA_LIB)/libtcl7.4.a
LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a

# EPICS libraries
LIBNAMES += $(EPICS_BASE_LIB)/libcas.a
LIBNAMES += $(EPICS_BASE_LIB)/libca.a
LIBNAMES += $(EPICS_BASE_LIB)/libCom.a
LIBNAMES += $(EPICS_BASE_LIB)/libgdd.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
  LIBNAMES += -lrt
endif

##LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
##LIBNAMES += -lCrun
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/clas_epics_server.h tmp
	cp $(MAIN).s/epics_server.h tmp
	cp $(MAIN).s/epicsutil.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















