#
# Makefile for clon/laser/tty6016
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = tty6016


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./vxworks.s \
			-I$(CODA)/common/include -I$(CLON)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp






























