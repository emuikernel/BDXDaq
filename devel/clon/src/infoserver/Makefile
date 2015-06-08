#
# Makefile for clon/infoserver/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = infoserver


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./$(MAIN).s \
				-I$(EPICS_BASE)/include \
				-I$(EPICS_EXTENSIONS)/src/ezca \
				-I$(CDEV)/include \
				-I$(RTHOME)/include \
				-I$(CODA)/common/include -I$(CLON)/common/include

###				-I$(EPICS_BASE)/include/os/solaris \



LIBNAMES += $(globallib)/libclonutil.a
LIBNAMES += $(globallib)/libipc.a

LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a
LIBNAMES += $(CODA_LIB)/libtcl7.4.a
LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libet.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

LIBNAMES += -lresolv
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif


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





















