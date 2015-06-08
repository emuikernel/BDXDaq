#
# Makefile for clon/runlog/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = runlog


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include

# if want debugging mode, uncomment following lines
ifeq ("$(OSTYPE)","SunOS")
    CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
    LDFLAGS = -g -KPIC -mt
endif
ifeq ("$(OSTYPE_MACHINE)","SunOS_i86pc")
    CFLAGS += -DSunOS_i86pc
endif
ifeq ("$(OSTYPE_MACHINE)","SunOS_sun4u")
    CFLAGS += -DSunOS_sun4u
endif
ifdef RTPC_RUN_LOG
    CFLAGS += -D_RTPC_RUN_LOG_
endif

#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(RTHOME)/include

LIBNAMES = $(globallib)/libclonutil.a
LIBNAMES += $(globallib)/libipc.a

LIBNAMES += $(globallib)/libepicsutil.a

LIBNAMES += $(CODA_LIB)/libtcl7.4.a

###LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a
LIBNAMES += $(CODA)/src/mysql4.1.20/$(OSTYPE_MACHINE)/lib/libmysql4.1.20.a

LIBNAMES += $(CODA_LIB)/libbosio.a

# EPICS libraries
#LIBNAMES += $(EPICS_BASE_LIB)/libcas.a
LIBNAMES += $(EPICS_BASE_LIB)/libca.a
LIBNAMES += $(EPICS_BASE_LIB)/libCom.a
#LIBNAMES += $(EPICS_BASE_LIB)/libgdd.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















