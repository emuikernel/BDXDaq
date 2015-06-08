h00038
s 00001/00001/00260
d D 1.5 03/12/16 22:15:32 boiarino 6 5
c .
e
s 00000/00002/00261
d D 1.4 03/04/17 16:43:34 boiarino 5 4
c *** empty log message ***
e
s 00001/00001/00262
d D 1.3 02/09/09 16:39:38 boiarino 4 3
c small fix
e
s 00001/00001/00262
d D 1.2 02/09/09 16:33:33 boiarino 3 1
c bos.h -> bosced.h
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/Makefile
e
s 00263/00000/00000
d D 1.1 02/09/09 16:27:30 boiarino 1 0
c date and time created 02/09/09 16:27:30 by boiarino
e
u
U
f e 0
t
T
I 1
#
# Makefile for 'ced' directory
#
#---------------------------------------------------------------------
# Define the OS Name
#---------------------------------------------------------------------

ifndef OSNAME
  OSNAME = $(shell uname)
endif


HVLIBDIR = ../Hv/s
HV_LOCATION = ../Hv/s
HELPDIR = ./help


#---------------------------------------------------------------------
# Define the C compiler and C Flags
#---------------------------------------------------------------------

CC = cc
D 6
CFLAGS = -O -c
E 6
I 6
CFLAGS = -fast -KPIC -mt -c
E 6

ifeq ($(OSNAME),Linux)
  CC = gcc 
  CFLAGS += -Wall -m486 -ansi
endif

#---------------------------------------------------------------------
# Define the necessary X libraries and paths
# First the default settings, and then architectural
# dependent mods -- YOU MAY HAVE TO HACK THESE PATHS
#---------------------------------------------------------------------


XLIBS         = -lXt -lX11
XMLIBS        = -lXm
XINCLUDEDIR   = /usr/include/X11
XMINCLUDEDIR  = /usr/include/X11 
XLIBDIR       = /usr/lib 
XMLIBDIR      = /usr/lib
HVINCLUDEDIR  = $(HV_LOCATION)

ifeq ($(OSNAME),Linux)
  XLIBS        = -lXt -lXpm -lXext -lX11 -lXp
  XINCLUDEDIR  = /usr/X11R6/include
  XMINCLUDEDIR = /usr/X11R6/include
  XLIBDIR      = /usr/X11R6/lib
  XMLIBDIR     = /usr/lib/Motif1.2
endif

ifeq ($(OSNAME),SunOS)
  XINCLUDEDIR  = /usr/openwin/include
  XLIBDIR      = /usr/openwin/lib
  XMLIBDIR     = /usr/dt/lib
endif

#---------------------------------------------------------------------
# Define rm& mv  so as not to return errors
#---------------------------------------------------------------------

RM =  rm -f
MV =  mv -f

#---------------------------------------------------------------------
# Define (relative to the source dir) the object dir
#---------------------------------------------------------------------

VIEWPATH = ../..

#---------------------------------------------------------------------
# This variable lists bos path and libraries
#---------------------------------------------------------------------

BOSPATH = $(CLAS_LIB)
BOSLIBS =  -L$(BOSPATH) \
               -ldc -lmapmanager -lc_bos_io -lclasutil -lrecutl -lbosio

#---------------------------------------------------------------------
# This variable lists cern path.
#---------------------------------------------------------------------

CERNLIBS = -L$(CERN_ROOT)/lib -lpacklib

#---------------------------------------------------------------------
# Put all the libraries together with their paths
#---------------------------------------------------------------------

HVLIB = $(HVLIBDIR)/libHv.a
LIBS = $(HVLIB) -L$(XMLIBDIR) $(XMLIBS) \
		$(BOSLIBS) \
		$(CODA)/SunOS/lib/libet.a \
		-L$(XLIBDIR) $(XLIBS) \
		$(CERNLIBS) $(STANDARDLIBS)

SOCKETLIBS = -lnsl -lsocket -lthread -lposix4 -lresolv -ldl
ifeq ($(OSNAME),Linux)
  SOCKETLIBS = 
endif

#---------------------------------------------------
# Program and library names
#---------------------------------------------------

PROGRAM = ced

#---------------------------------------------------------------------
#LIBFLAGS are used when building the ced library
#---------------------------------------------------------------------

LIBFLAGS = -DMLIB

#---------------------------------------------------------------------
# The source files that make up the application
#---------------------------------------------------------------------

      SRCS = \
             alldc.c\
             alldcview.c\
             anchor.c\
             beamline.c\
             bitops.c\
             calview.c\
             cerenkov.c\
             dc.c\
             dlogs.c\
             draw.c\
             ec1.c\
			 et.c\
             evdump.c\
             event.c\
             fannedscint.c\
             fannedshower.c\
             feedback.c\
             field.c\
             geometry.c\
             init.c\
             main.c\
             math.c\
             menus.c\
             message.c\
D 5
             removenoise.c\
E 5
             scintillator.c\
             scintview.c\
             sectorhex.c\
             sectorview.c\
             setup.c\
             shower.c\
             tagger.c\
             tile.c\
             track.c

#---------------------------------------------------------------------
# The object files (via macro substitution)
#---------------------------------------------------------------------

OBJS = ${SRCS:.c=.o}

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

      INCLUDES = -I$(HVINCLUDEDIR) \
        -I$(XINCLUDEDIR) \
        -I$(XMINCLUDEDIR) \
		-I../clas/bosio/bosio.s \
		-I$(CODA)/common/include \
        -I$(CLAS_PACK)/include

#---------------------------------------------------------------------
# How to make a .o file from a .c file 
#---------------------------------------------------------------------


%.o : %.c
	$(CC) $(CFLAGS) -D_LANGUAGE_C $(INCLUDES) $< -o $@


#--------------------------------------------------------
# Default target
#--------------------------------------------------------

exe: $(OBJS)
	f77 -o $(PROGRAM) $(OBJS) $(LIBS) $(SOCKETLIBS)
	@echo "executable placed in $(PROGRAM)"

#---------------------------------------------------------------------
# This rule creates the library libced.a
#---------------------------------------------------------------------

lib: 
	@echo "ced only builds as an executable."

#---------------------------------------------------------------------
#this rule makes the help file
#---------------------------------------------------------------------

help:
	$(RM) $(HELPDIR)/$(PROGRAM).help
	nroff $(HELPDIR)/beforenroff.help >> $(HELPDIR)/$(PROGRAM).help
	$(RM) $(CLAS_PARMS)/$(PROGRAM).help
	cp $(HELPDIR)/$(PROGRAM).help $(CLAS_PARMS)
	cat $(HELPDIR)/$(PROGRAM).help | more

#---------------------------------------------------------------------
# This rule cleans up the build directory (type "make clean").
#---------------------------------------------------------------------

distclean: clean
clean:
	$(RM) *.o ced core

#---------------------------------------------------------------------
# Installation
#---------------------------------------------------------------------

exports: install
install:
D 4
	cp bin/$(OSNAME)/ced $(CLON_BIN)
E 4
I 4
	cp ced $(CLON_BIN)
E 4

#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

D 3
INCLUDEFILES =  ced.h data.h bos.h geometry.h macro.h proto.h
E 3
I 3
INCLUDEFILES =  ced.h data.h bosced.h geometry.h macro.h proto.h
E 3
$(PROGRAM):     $(HVLIB)

alldc.o:        $(INCLUDEFILES)
alldcview.o:    $(INCLUDEFILES) feedback.h
anchor.o:       $(INCLUDEFILES)
beamline.o:     $(INCLUDEFILES)
bitops.o:       $(INCLUDEFILES)
calview.o:      $(INCLUDEFILES) feedback.h
cerenkov.o:     $(INCLUDEFILES)
dc.o:           $(INCLUDEFILES)
dd.o:           $(INCLUDEFILES)
dlogs.o:        $(INCLUDEFILES)
draw.o:         $(INCLUDEFILES)
ec1.o:          $(INCLUDEFILES)
evdump.o:       $(INCLUDEFILES)
event.o:        $(INCLUDEFILES)
fannedscint.o:  $(INCLUDEFILES)
fannedshower.o: $(INCLUDEFILES)
feedback.o:     $(INCLUDEFILES) feedback.h
field.o:        $(INCLUDEFILES)
geometry.o:     $(INCLUDEFILES)
init.o:         $(INCLUDEFILES)
main.o:         $(INCLUDEFILES)
math.o:         $(INCLUDEFILES)
menus.o:        $(INCLUDEFILES)
message.o:      $(INCLUDEFILES)
D 5
removenoise.o:  $(INCLUDEFILES)
E 5
scintillator.o: $(INCLUDEFILES)
scintview.o:    $(INCLUDEFILES) feedback.h
sectorhex.o:    $(INCLUDEFILES)
sectorview.o:   $(INCLUDEFILES) feedback.h
setup.o:        $(INCLUDEFILES) feedback.h
shower.o:       $(INCLUDEFILES)
tagger.o:       $(INCLUDEFILES)
tile.o:         $(INCLUDEFILES)
track.o:        $(INCLUDEFILES)

E 1
