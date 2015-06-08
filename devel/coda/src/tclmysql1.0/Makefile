#
# Makefile for tclmysql1.0/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = tclmysql1.0


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include


#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \
	-DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 

ifeq ("$(OSTYPE)","SunOS")
	CLON_FLAGS += -DSOLARIS=1 -DNO_UNION_WAIT=1 -DTK_FILE_COUNT=_cnt -DNEED_MATHERR=1 -Dvfork=fork
endif

ifeq ("$(OSTYPE)","Linux")
	CLON_FLAGS += -DTK_FILE_READ_PTR=1
endif

ifeq ("$(OSTYPE)","Darwin")
        CLON_FLAGS += $(X11INCDIR)
endif

LIBNAMES += $(globallib)/libmysql4.1.20.a
LIBNAMES += $(globallib)/libtcl7.4.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib
	@for i in $(CODA)/common/lib $(CODA)/common/include ; \
    do \
    if [ ! -d $$i ] ; then \
        echo "Making directory $$i"; \
        mkdir $$i; \
        chmod 755 $$i; \
        else true; \
        fi; \
    done;
	cp $(localbin)/tclAppInit $(CODA)/$(OSTYPE_MACHINE)/bin/tclmysql1.0

	rm -rf tmp
	mkdir tmp
	cp src.s/tclSQL.h tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp

#
#-------------------------------------------------------------------------
#




















