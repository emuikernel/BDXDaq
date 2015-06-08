h18135
s 00000/00000/00000
d R 1.2 02/08/26 22:35:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/simple/Makefile
e
s 00106/00000/00000
d D 1.1 02/08/26 22:35:25 boiarino 1 0
c date and time created 02/08/26 22:35:25 by boiarino
e
u
U
f e 0
t
T
I 1
#
# Makefile for Hv/s directory
#
SHELL = /bin/sh

include $(CLON_SOURCE)/Hv/flags.mk

#---------------------------------------------------------------------
# Define the C compiler
#---------------------------------------------------------------------

        CC = $(HV_CC)

#---------------------------------------------------------------------
# Define rm  & mv so as not to return errors
#---------------------------------------------------------------------

        RM =  rm -f
        MV =  mv -f

#---------------------------------------------------------------------
# Put all the libraries together with their paths
#---------------------------------------------------------------------

        HVLIB = $(HV_LIBDIR)/libHv.a
        LIBS = $(HVLIB) -L$(HV_XMLIBDIR) $(HV_XMLIBS)\
	   -L$(HV_XLIBDIR) $(HV_XLIBS)\
	   $(HV_STANDARDLIBS)

#---------------------------------------------------
# Program and library names
#---------------------------------------------------

        PROGRAM = simple

#---------------------------------------------------------------------
# The source files that make up the application
#---------------------------------------------------------------------

        SRCS = main.c init.c feedback.c setup.c

#---------------------------------------------------------------------
# The object files (via macro substitution)
#---------------------------------------------------------------------

        OBJS = ${SRCS:.c=.o}

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

      INCLUDES = -I../../../src -I$(HV_INCLUDEDIR)\
        -I$(HV_XINCLUDEDIR)\
        -I$(HV_XMINCLUDEDIR)

#---------------------------------------------------------------------
# How to make a .o file from a .c file 
#---------------------------------------------------------------------


.c.o :
	$(CC) $(HV_CFLAGS) $(EXTRAFLAGS) $(INCLUDES) $<

#--------------------------------------------------------
# Default target

#the tmpfle business is just so mv does not
#give an error if no files are there to move

#--------------------------------------------------------

all: $(PROGRAM)

#---------------------------------------------------------------------
# This rule generates the executable using the object files and libraries.
#---------------------------------------------------------------------

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

#---------------------------------------------------------------------
# This rule cleans up the build directory (type "make clean").
#---------------------------------------------------------------------

distclean: clean
clean:
	$(RM) $(PROGRAM) *.o core

#---------------------------------------------------------------------
# This rule makes the help file.
#---------------------------------------------------------------------

help:
	$(RM) $(PROGRAM).help
	nroff beforenroff.help > $(PROGRAM).help

#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

$(PROGRAM): $(HVLIB)

main.o:     simple.h
init.o:     simple.h
feedback.o: simple.h
setup.o:    simple.h
E 1
