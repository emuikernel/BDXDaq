# Generated automatically from Makefile.in by configure.
#
CC  = gcc
CXX = g++
LD  = ld
AR  = ar
RANLIB = ranlib
#CODA_BIN = ${prefix}/bin
#CODA_LIB = ${prefix}/lib
#CODA_INC = /usr/local/include
#CODA_JAR = /usr/local/lib/java

# find location of "java" and back up 1 dir to find java's home dir
ifeq ($(JAVAHOME),)
JAVAHOME = $(shell /bin/csh -c "set j = `which java`; set k = \$$j:h; echo \$$k:h")
endif
JAVA_INC = $(JAVAHOME)/include
JAVA_INC_LOCAL = $(JAVA_INC)/linux
INSTALL  = ../install-sh -c

AC_FLAGS  = -D_REENTRANT
CFLAGS    = -O2 -fPIC -I../src -I$(JAVA_INC) -I$(JAVA_INC_LOCAL) $(AC_FLAGS)
CXXFLAGS  = $(CFLAGS)
SHLIB_LD  = ld -G
LIBS      = -lieee -lm -lpthread  -lnsl -lresolv -ldl

JAVAC = $(JAVAHOME)/bin/javac
JAVAH = $(JAVAHOME)/bin/javah
JAVAD = $(JAVAHOME)/bin/javadoc
JAR   = $(JAVAHOME)/bin/jar
JAVAC_FLAGS = -classpath .:$(CLASSPATH)

JAVA_OBJS = \
	Consumer.class \
	StartEt.class \
	Producer.class

all: $(JAVA_OBJS)

doc:
	$(JAVAD) -package org.jlab.coda.et

clean: 
	rm -f core *~ *.o *.so *.class

# SUFFIX RULES

.SUFFIXES: .java .class

.java.class:
	$(JAVAC) $< $(JAVAC_FLAGS)
.c:
	$(CC) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.cc:
	$(CXX) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.c.o:
	$(CC) -c $(CFLAGS) $<
