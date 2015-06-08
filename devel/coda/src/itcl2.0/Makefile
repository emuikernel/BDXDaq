#
# Makefile for itcl2.0/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = itcl2.0


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include


#-----------------------
# customize definitions
#-----------------------

# Directory from which applications will reference the library of itcl
# scripts (note: you can set the ITCL_LIBRARY environment variable at
# run-time to override this value (Sergey: you can not !) ):
ITCL_LIBRARY =	$(CODA)/common/lib/$(MAIN)

CLON_FLAGS += -I./$(MAIN).s \
	-I$(CODA)/src/tcl7.4/tcl7.4.s \
	-DITCL_LIBRARY=\"${ITCL_LIBRARY}\" -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1

ifeq ("$(OSTYPE)","SunOS")
	CLON_FLAGS += -DSOLARIS=1 -DNO_UNION_WAIT=1 -DTK_FILE_COUNT=_cnt -DNEED_MATHERR=1 -Dvfork=fork 
endif

ifeq ("$(OSTYPE)","Linux")
	CLON_FLAGS +=  -DTK_FILE_READ_PTR=1 -DTK_FILE_READ_PTR=1
endif

LIBNAMES += $(globallib)/libtcl7.4.a

LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib
	@for i in $(CODA)/common/lib $(CODA)/common/include $(CODA)/common/lib/itcl2.0 ; \
    do \
    if [ ! -d $$i ] ; then \
        echo "Making directory $$i"; \
        mkdir $$i; \
        chmod 755 $$i; \
        else true; \
        fi; \
    done;
##	cp $(localbin)/tclAppInit $(CODA)/$(OSTYPE_MACHINE)/bin/itclsh2.0

	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/itcl.h tmp/itcl_v2.h
#	cp $(MAIN).s/itclInt.h tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp

	rm -rf tmp
	mkdir tmp
	cp library/*.itcl tmp/
#	cp main/tclAppInit.c tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/lib/itcl2.0/
	rm -rf tmp

#
#-------------------------------------------------------------------------
#




















