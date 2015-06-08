#
# Makefile for clon/ipc/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = ipc


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

# use C++ as linker instead of CC: we'll link main *.c program with
# SmartSocket's C++ libraries, and using CC will not resolve references
ifeq ("$(OSTYPE)","Linux")
  LDCC = $(CXX)
endif

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(RTHOME)/include

LIBNAMES += $(globallib)/libclonutil.a
LIBNAMES += $(CODA_LIB)/libet.a
LIBNAMES += $(CODA_LIB)/libtcl7.4.a
LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50


JFLAGS = -classpath $(CLON)/common/jar/clonutil.jar:$(CLAS)/jar/ss.jar



#for 'inet_aton'
LIBNAMES += -lresolv

ifeq ("$(OSTYPE)","SunOS")
  LIBNAMES += -lCrun
endif

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif


LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin install_java
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















