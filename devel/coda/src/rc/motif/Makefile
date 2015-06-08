#
# Makefile for rc/motif
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = motif


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

#for debugging
ifeq ("$(OSTYPE)","SunOS")
  CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
endif

CLON_FLAGS += -I./include $(X11INCDIR) \
	-DFUNCPROTO -DXTFUNCPROTO

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






























