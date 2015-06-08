h17588
s 00000/00000/00000
d R 1.2 02/08/26 22:12:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvmap/Makefile
e
s 00111/00000/00000
d D 1.1 02/08/26 22:12:15 boiarino 1 0
c date and time created 02/08/26 22:12:15 by boiarino
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
# Define rm so as not to return errors
#---------------------------------------------------------------------

     	RM =  rm -f

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

        PROGRAM = hvmap

#---------------------------------------------------------------------
# The source files that make up the application
#---------------------------------------------------------------------


      SRCS = \
             init.c\
             dlogs.c\
             feedback.c\
             logo.c\
             menus.c\
             main.c\
             overlay.c\
             setup.c

#---------------------------------------------------------------------
# The object files (via macro substitution)
#---------------------------------------------------------------------

        OBJS = ${SRCS:.c=.o}

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

      INCLUDES = -I$(HV_INCLUDEDIR)\
        -I$(HV_XINCLUDEDIR)\
        -I$(HV_XMINCLUDEDIR)

#---------------------------------------------------------------------
# How to make a .o file from a .c file 
#---------------------------------------------------------------------


.c.o :
	$(CC) $(HV_CFLAGS) $(INCLUDES) $<

#---------------------------------------------------------------------
# This rule generates the executable using the object files and libraries.
#---------------------------------------------------------------------

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

#---------------------------------------------------------------------
#this is for the object center code system
#--------------------------------------------------------------------

     ocent_src: $(SRCS)
	#load $(HV_CFLAGS) $(HV_PLATFORMFLAGS) $(INCLUDES) -C $(SRCS)

     ocent_obj: $(OBJS)
	#load $(OBJS) $(LIBS)
	#link

#---------------------------------------------------------------------
# This rule cleans up the build directory (type "make clean").
#---------------------------------------------------------------------

distclean: clean
clean:
	$(RM) $(PROGRAM) *.o core
	
#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

$(PROGRAM):      $(HVLIB)

feedback.o:       maps.h
main.o:           maps.h
setup.o:          maps.h
init.o:           maps.h 
dlogs.o:          maps.h
menus.o:          maps.h 
overlay.o:        maps.h 
logo.o:           maps.h 
E 1
