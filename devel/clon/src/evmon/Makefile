#
# Makefile for clon/evmon/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = evmon


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

ifeq ("$(OSTYPE)","Linux")
  LDCC = $(CXX)
endif

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include

# if use smartsockets
CLON_FLAGS += -I$(CLON)/common/include -I$(RTHOME)/include -DSSIPC

#LIBNAMES += $(CLAS_LIB)/libmapmanager.a
LIBNAMES += $(CLON_LIB)/libcmon.a

LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libet.a

# if use smartsockets
LIBNAMES += $(CLON_LIB)/libclonutil.a
LIBNAMES += $(CLON_LIB)/libipc.a
LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif

LIBNAMES += -lresolv
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)

#ifeq ("$(OSTYPE)","Linux")
#  LIBNAMES += -lc
#endif



#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp ./dab/monb tmp
	chmod 775 tmp/*
	cp tmp/* $(CLON)/common/scripts
	rm -rf tmp



























