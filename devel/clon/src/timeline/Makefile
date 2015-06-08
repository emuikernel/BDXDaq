#
# Makefile for clon/timeline/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = timeline


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I$(MAIN).s \
				-I$(EPICS_BASE)/include \
				-I$(EPICS_BASE)/include/os/solaris \
				-I$(EPICS_EXTENSIONS)/src/ezca \
			-I$(CODA)/common/include -I$(CLON)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

#for rrdtool
###CLON_FLAGS += -I/www/rrdtool1.2.25/include

LIBNAMES += $(globallib)/libHv.a

#for rrdtool
###LIBNAMES += /www/rrdtool1.2.25/lib/librrd.so

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/timeline.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp






























