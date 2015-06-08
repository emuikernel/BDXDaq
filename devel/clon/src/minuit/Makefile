#
# Makefile for clon/minuit/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = minuit


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

## ifeq ("$(OSTYPE)","SunOS")
## # '-fast' invokes '-xvector=yes', as result undefined symbols
##   FFLAGS += -xvector=no
## endif

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin

#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp






























