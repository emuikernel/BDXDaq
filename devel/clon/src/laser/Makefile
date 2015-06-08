#
# Makefile for clon/laser/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = laser


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./sc.s -I./tty6016/vxworks.s \
			-I$(CODA)/common/include -I$(CLON)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp






























