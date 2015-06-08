#-----------------------------------------------------------------------------
# title: VMEBus Library for Universe Driver
# version: Linux 1.1
# date: April 1999
# designer: Michael Wyrick                                                    
# programmer: Michael Wyrick                                                  
# company: 
# platform: Linux 2.2.x, 2.4.x                                    
# language: GCC 2.95, 3.0
# module: 
#-----------------------------------------------------------------------------
#  Purpose: Makefile for libvme
#  Docs:                                                                       
#-----------------------------------------------------------------------------
all: libvme.a

CC = gcc
CFLAGS = -Wall

all: libvme.a libvme.so

libvme.a: libvme.o libvme_int.o
	ar rv libvme.a libvme.o libvme_int.o

libvme.so: libvme.o libvme_int.o
	${CC} ${CFLAGS} -shared -Wl,-soname,libvme.so -olibvme.so libvme.o libvme_int.o -lc

libvme.o: libvme.cpp libvme.h Makefile
	${CC} ${CFLAGS} -fPIC -c libvme.cpp

libvme_int.o: libvme_int.cc libvme_int.h Makefile
	${CC} ${CFLAGS} -fPIC -c libvme_int.cc

clean:
	rm -f libvme.o libvme_int.o libvme.so libvme.a
	
install:
	cp libvme.h $(CODA)/common/include
	cp libvme_int.h $(CODA)/common/include
	cp libvme.a $(CODA)/$(OSTYPE_MACHINE)/lib
	cp libvme.so $(CODA)/$(OSTYPE_MACHINE)/lib


