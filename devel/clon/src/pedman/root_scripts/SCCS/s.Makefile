h54868
s 00004/00002/00114
d D 1.8 02/08/28 21:38:32 boiarino 9 8
c export *.C to the $ROOTSYS/macros
e
s 00011/00011/00105
d D 1.7 02/08/09 17:48:22 boiarino 8 7
c use CERN_ROOT, cc, bosio,
c /usr/local/clas/offline/clas/builds/PRODUCTION
c 
e
s 00001/00001/00115
d D 1.6 00/09/07 11:33:11 wolin 7 6
c Using pedo instead of pedo_old.exec
e
s 00001/00001/00115
d D 1.5 00/08/30 08:44:12 gurjyan 6 5
c 
e
s 00000/00000/00116
d D 1.4 00/08/07 16:10:41 gurjyan 5 4
c *** empty log message ***
e
s 00001/00001/00115
d D 1.3 00/08/07 16:09:49 gurjyan 4 3
c *** empty log message ***
e
s 00002/00000/00114
d D 1.2 00/08/07 16:05:20 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/Makefile
e
s 00114/00000/00000
d D 1.1 00/08/07 15:52:23 gurjyan 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
#
	CFLAGS = -g
	LFLAGS = -g
#
#----------------------------------------------------------------------
# Use gcc as standard, more consistend, besides, cc !@#$%!
#----------------------------------------------------------------------
D 8
        CC = gcc
	LN = gcc
E 8
I 8
#	CC = gcc
#	LN = gcc
	CC = cc
	LN = cc
E 8
#----------------------------------------------------------------------
D 8
        RM =  rm -f
        MV =  mv -f
E 8
I 8
	RM =  rm -f
	MV =  mv -f
E 8

#---------------------------------------------------------------
#INC_CLAS    = -I$(CLON_INC)
#LINK_CLAS   = -L$(CLON_LIB) -lipc -lutil 
#---------------------------------------------------------------

#---------------------------------------------------------------------
# Needed to link with BOS stuff.
#---------------------------------------------------------------------

D 8
    BOSPATH = /group/clas/builds/release-2-3/lib/SunOS
    BOSLIBS = -L$(BOSPATH) -lc_bos_io -lrecutl -lfputil -lbos \
	      -lfpack -lc_bos_io -lclasutil -lmapmanager
E 8
I 8
    BOSPATH = /usr/local/clas/offline/clas/builds/PRODUCTION/lib/SunOS
    BOSLIBS = -L$(BOSPATH) -lrecutl -lc_bos_io -lbosio -lclasutil -lmapmanager
E 8

#---------------------------------------------------------------------
# If Needing to link with CERN stuff
#---------------------------------------------------------------------

D 8
    CERNPATH = /net/fs1/site/cernlib/sun4_solaris2/97a/lib
E 8
I 8
    CERNPATH = $(CERN_ROOT)/lib
E 8
    CERNLIBS = -L$(CERNPATH) -lpacklib

#----------------------------------------------------------------------
# Platform dependend libs...
#----------------------------------------------------------------------
#ifeq ($(OSTYPE),SunOS)
   LN = f77
   SYSTEM_LIBS= -lc -lw -lgen -lintl -lm -lnsl -lsocket -ldl
#endif
#ifeq ($(OSTYPE),HP)
#   LN = echo
#   SYSTEM_LIBS= -lf77
#endif
#ifeq ($(OSTYPE),AIX)
#   LN = echo
#   SYSTEM_LIBS= -lf77
#endif
#ifeq ($(OSTYPE),Linux)
#   SYSTEM_LIBS= -lf2c -lm
#endif
#ifeq ($(OSTYPE),IRIX)
#   SYSTEM_LIBS= -lF77 -lI77 -lftn -lm
#endif

#---------------------------------------------------------------------
# Put all the libraries together with their paths
#---------------------------------------------------------------------

        LIBS = -L$(CLAS_LIB) $(BOSLIBS) $(SYSTEM_LIBS)
        LIBSC = -L$(CLAS_LIB) $(BOSLIBS) $(CERNLIBS) $(SYSTEM_LIBS) 

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

D 8
      CLAS_PACK = /group/clas/builds/PRODUCTION/packages
      INCLUDES = -I. \
                 -I$(CLAS_PACK)  -I$(CLAS_PACK)/include 
E 8
I 8
      CLAS_PACK = /usr/local/clas/offline/clas/builds/PRODUCTION/packages
      INCLUDES = -I. -I$(CLAS_PACK) -I$(CLAS_PACK)/include 
E 8

#---------------------------------------------------------------------
# How to make a .o file from a .c file 
#---------------------------------------------------------------------

.c.o :
	$(CC) $(CFLAGS) $(C_DEFINES) $(INCLUDES) -c $<

D 4

E 4
I 4
all: pedo
E 4
pedo: pedo.o 
	$(LN) $(LFLAGS) -o $@ pedo.o $(LIBSC)

warn: 
	@echo "To compile stuff, make sure you also have"
	@echo "../../include and ../../c_bos_io checked out."
	@echo "============================================="
	@echo

#---------------------------------------------------------------------
# This rule cleans up the build directory (type "make clean").
#---------------------------------------------------------------------

D 9
     clean:
E 9
I 9
clean:
E 9
	$(RM) *.o core

I 9
install: exports
E 9
exports:
D 6
	cp pedo $(CLON_BIN)
E 6
I 6
D 7
	cp pedo_old.exec $(CLON_BIN)
E 7
I 7
	cp pedo $(CLON_BIN)
I 9
	cp *.C $(ROOTSYS)/macros
E 9
E 7
E 6

I 3
distclean:
D 9
	$(RM) *.o pedo core
E 9
I 9
	$(RM) *.o *.root pedo core
E 9
E 3
#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

INCLUDEFILES =  $(CLAS_PACK)/include/demux_roc.h \
                $(CLAS_PACK)/include/bostypes.h   \
                $(CLAS_PACK)/include/bosddl.h

pedo.o: $(INCLUDEFILES)








E 1
