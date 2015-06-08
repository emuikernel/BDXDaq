#
# Makefile for clon/ced/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = ced


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./src.s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(CLAS_PACK)/include
###                -DWITH_OFFLINE


LIBNAMES += $(globallib)/libHv.a
###LIBNAMES += $(CLAS_LIB) -ldc -lmapmanager -lc_bos_io -lclasutil -lrecutl

# for noise reduction etc
LIBNAMES += $(CLON)/src/cmon/$(OSTYPE_MACHINE)/lib/libcmon.a
LIBNAMES += $(CODA)/src/codatt/$(OSTYPE_MACHINE)/lib/libcodatt.a

LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libet.a

#for old solaris only ???
LIBNAMES += -lresolv

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif


#-------------------
# customize targets
#-------------------
install: install_lib install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















