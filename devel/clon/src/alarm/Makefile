#
# Makefile for clon/alarm/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = alarm


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(RTHOME)/include

JFLAGS = -classpath ./:$(CLON)/common/jar/clonutil.jar:$(CLAS)/jar/klg.jar:$(CLAS)/jar/symclass.zip:$(CLAS)/jar/symbeans.jar:$(CLAS)/jar/ss.jar


LIBNAMES = $(globallib)/libclonutil.a
LIBNAMES += $(globallib)/libipc.a
LIBNAMES += $(CODA_LIB)/libtcl7.4.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif

LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin install_java
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















