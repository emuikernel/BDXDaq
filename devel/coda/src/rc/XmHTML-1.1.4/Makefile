#
# Makefile for rc/XmHTML-1.1.4
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = XmHTML-1.1.4


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

# '-xO5' does not compiles !!! use -xO2
ifeq ("$(OSTYPE)","SunOS")
  CFLAGS = -xO2 -KPIC -mt -DSunOS -DOSTYPE="SunOS"
endif

CLON_FLAGS += -I./include -I../jpeg6.1/jpeg6.1.s $(X11INCDIR) \
	-DVERSION=1103 \
	-DHAVE_MEMCPY -DHAVE_LIBJPEG -DHAVE_REGEX -DNDEBUG 

LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -Dsun -Dsparc -DSVR4 -DSYSV -DNEED_STRERROR -DNEED_STRCASECMP
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






























