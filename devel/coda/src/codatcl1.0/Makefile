#
# Makefile for codatcl1.0/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = codatcl1.0


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include


#-----------------------
# customize definitions
#-----------------------

# Directory from which applications will reference the library of Dp
# scripts (note: you can set the DP_LIBRARY environment variable at
# run-time to override this value (Sergey: you can not !) ):
DP_LIBRARY = $(CODA)/common/lib/$(MAIN)

#sergey: we need tkInt.h, and it is not installed to /usr/include, so we have to
#  access tcl/tk source area for header files
CLON_FLAGS += -I./$(MAIN).s $(TCLINCDIR) \
	-I$(CODA)/common/include \
	-DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 -DHAVE_STRDUP=1 \
	-DHAVE_STRERROR=1  -DHAVE_STRCASECMP=1 -DHAVE_UNISTD_H=1 -DHAVE_ERRNO_H=1 \
	-DHAVE_FCNTL_H=1 -DNO_STREAM_H=1 -DNO_UN_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1  \
	-DHAVE_TOLOWER=1 -DSTDC_HEADERS=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_FLOAT_H=1 \
	-DHAVE_MALLOC_H=1 -DHAVE_ERRNO_H=1 -DHAVE_UNISTD_H=1 -DHAVE_MEMORY_H=1 -DHAVE_LIMITS_H=1 \
	-DHAVE_SYS_TIME_H=1 -DHAVE_SYS_WAIT_H=1 -DTIME_WITH_SYS_TIME=1 -DNO_UNION_WAIT=1 \
	-DNEED_MATHERR=1 -DSVIPC_NO_UNION_SEMUN=1 -DTK_FILE_COUNT=_cnt 

###	-DDP_LIBRARY=\"${DP_LIBRARY}\" \

CLON_FLAGS += -DUSE_TK

ifeq ("$(OSTYPE)","SunOS")
	CLON_FLAGS += -DSOLARIS=1 -DNO_UNION_WAIT=1 -DTK_FILE_COUNT=_cnt -DNEED_MATHERR=1 -Dvfork=fork \
		-D_REENTRANT -D__EXTENSIONS__
endif

ifeq ("$(OSTYPE)","Linux")
	CLON_FLAGS += -DTK_FILE_READ_PTR=1
endif

ifeq ("$(OSTYPE)","Darwin")
        CLON_FLAGS += $(X11INCDIR)
endif


#-------------------
# customize targets
#-------------------

install:
	@for i in $(CODA)/$(OSTYPE_MACHINE)/lib/codatcl ; \
    do \
    if [ ! -d $$i ] ; then \
        echo "Making directory $$i"; \
        mkdir $$i; \
        chmod 755 $$i; \
        else true; \
        fi; \
    done;
	cp ./$(OSTYPE_MACHINE)/lib/*.so $(CODA)/$(OSTYPE_MACHINE)/lib/codatcl/

#copy package index file: must be in the same place as shared library
	rm -rf tmp
	mkdir tmp
	cp scripts/pkgIndex.tcl tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/$(OSTYPE_MACHINE)/lib/codatcl/
	rm -rf tmp

#
#-------------------------------------------------------------------------
#




















