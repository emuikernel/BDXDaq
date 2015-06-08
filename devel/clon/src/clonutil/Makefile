#
# Makefile for clon/clonutil/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = clonutil


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./$(MAIN).s -I$(EPICS_BASE)/include \
				-I$(EPICS_BASE)/include/os/solaris \
				-I$(EPICS_EXTENSIONS)/src/ezca \
				-I$(RTHOME)/include \
				-I$(CODA)/common/include -I$(CLON)/common/include

JFLAGS = -classpath $(CLAS)/jar/ss.jar

LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a

#for old solaris only ???
LIBNAMES += -lresolv

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin install_java
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















