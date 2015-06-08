#
# Makefile for clon/pedman/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = pedman


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(RTHOME)/include

LIBNAMES += $(globallib)/libipc.a

LIBNAMES += $(CODA_LIB)/libtcl7.4.a
LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a
LIBNAMES += $(CODA_LIB)/libbosio.a

###LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

#LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin
	rm -rf tmp
	mkdir tmp
	cp root_scripts/*.C tmp/
	chmod 664 tmp/*
	cp tmp/* $(ROOTSYS)/macros
	rm -rf tmp





















