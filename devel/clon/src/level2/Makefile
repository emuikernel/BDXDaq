#
# Makefile for clon/level2/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = level2


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I$(MAIN).s \
			-I$(CODA)/common/include -I$(CLON)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(globallib)/libHv.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_java
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp






























