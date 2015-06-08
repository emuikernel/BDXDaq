#
# Makefile for rc/libhtmlw-2.7
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = libhtmlw-2.7


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./include $(X11INCDIR) \
	-DMOTIF -DMOTIF2_1 -DDISABLE_TRACE

LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSYSV -DSVR4
endif

#ifeq ("$(OSTYPE)","Linux")
#  CLON_FLAGS += -DDEBUG -DNeedFunctionProtoTypes
#endif

#-------------------
# customize targets
#-------------------
#install: install_lib
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CODA)/common/include
#	rm -rf tmp






























