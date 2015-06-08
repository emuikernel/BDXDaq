#
# Makefile for clon/src/Hv/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = Hv


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./src.s -DHv_NOJPEG

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib

	rm -rf tmp
	mkdir tmp
	cp src.s/Hv.h tmp
	cp src.s/Hv_*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















