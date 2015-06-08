#
# Makefile for tcl7.4/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = tcl7.4


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include


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



#-----------------------
# customize definitions
#-----------------------

# Directory from which applications will reference the library of Tcl
# scripts (note: you can set the TCL_LIBRARY environment variable at
# run-time to override this value (Sergey: you can not !) ):
TCL_LIBRARY =	$(CODA)/common/lib/$(MAIN)

CLON_FLAGS += -I./$(MAIN).s -DTCL_LIBRARY=\"${TCL_LIBRARY}\" \
	

ifeq ("$(OSTYPE)","SunOS")
	CLON_FLAGS += -DSOLARIS=1 -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 \
		-DHAVE_LIMITS_H=1 -DNO_UNION_WAIT=1 -DTK_FILE_COUNT=_cnt \
		-DNEED_MATHERR=1 -Dvfork=fork 
endif

ifeq ("$(OSTYPE)","Linux")
	CLON_FLAGS += -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 \
		-DTK_FILE_READ_PTR=1
endif

ifeq ("$(OSTYPE)","Darwin")
	CLON_FLAGS += -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 \
		-DTK_FILE_READ_PTR=1
endif

ifeq ("$(OSTYPE)","VXWORKS")
  CLON_FLAGS += -DNEED_ACCESS -DNO_GETTOD -DNO_VALUES_H \
		-DNO_SYS_ERRLIST -D__PROTOTYPE_5_0 -DTCL_VW
endif

LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib
	@for i in $(CODA)/common/lib $(CODA)/common/include $(CODA)/common/lib/tcl7.4 ; \
	do \
	if [ ! -d $$i ] ; then \
		echo "Making directory $$i"; \
		mkdir $$i; \
		chmod 755 $$i; \
		else true; \
	fi; \
	done;
##	cp $(localbin)/tclAppInit $(CODA)/$(OSTYPE_MACHINE)/bin/tclsh7.4
	cp library/tcl2c $(CODA)/common/scripts/

#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/tcl.h tmp/
#	cp $(MAIN).s/tclInt.h tmp/
#	cp $(MAIN).s/tclPort.h tmp/
#	cp $(MAIN).s/tclRegexp.h tmp/
#	cp $(MAIN).s/tclToCProto.h tmp/
#	cp $(MAIN).s/tclToC.h tmp/
#	chmod 664 tmp/*
#	cp tmp/* $(CODA)/common/include
#	rm -rf tmp

#	rm -rf tmp
#	mkdir tmp
#	cp library/*.tcl tmp/
#	cp library/tclIndex tmp/
#	cp main/tclAppInit.c tmp/
#	chmod 664 tmp/*
#	cp tmp/* $(CODA)/common/lib/tcl7.4/
#	rm -rf tmp

#
#-------------------------------------------------------------------------
#




















