#
# Makefile for rc/cg/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = cg


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./$(MAIN).s $(X11INCDIR) \
	-DMOTIF -DCACHE_BITMAPS -DCACHE_SIZE_LIMIT=300 -DCACHE_FID

LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DDEBUGGING
endif

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -DDEBUG -DNeedFunctionProtoTypes
endif

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






























