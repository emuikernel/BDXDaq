#
# Makefile for rc/sace3.1/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = sace3.1

#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include

#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./include $(X11INCDIR)

LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSVR4 -DSYSV
endif

#ifeq ("$(OSTYPE)","Linux")
#  CLON_FLAGS += 
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






























