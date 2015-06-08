h55647
s 00070/00000/00000
d D 1.1 06/06/27 15:23:42 boiarino 1 0
c date and time created 06/06/27 15:23:42 by boiarino
e
u
U
f e 0
t
T
I 1
#
# Makefile for clas/minuit/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = minuit


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \
			-D_POSIX_PTHREAD_SEMANTICS

#LIBNAMES += -lresolv
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp






























E 1
