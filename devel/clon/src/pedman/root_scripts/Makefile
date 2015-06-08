#
	CFLAGS = -g
	LFLAGS = -g
#
#----------------------------------------------------------------------
# Use cc
#----------------------------------------------------------------------
	CC = cc
	LN = cc
#----------------------------------------------------------------------
	RM =  rm -f
	MV =  mv -f
#---------------------------------------------------------------------
# Needed to link with BOS stuff.
#---------------------------------------------------------------------

    BOSPATH = /usr/local/clas/offline/clas/builds/PRODUCTION/lib/SunOS
    BOSLIBS = -L$(BOSPATH) -lrecutl -lc_bos_io -lbosio -lclasutil -lmapmanager

#---------------------------------------------------------------------
# If Needing to link with CERN stuff
#---------------------------------------------------------------------

    CERNPATH = $(CERN_ROOT)/lib
    CERNLIBS = -L$(CERNPATH) -lpacklib

#----------------------------------------------------------------------
# Platform dependend libs...
#----------------------------------------------------------------------
#ifeq ($(OSTYPE),SunOS)
   LN = f77
   SYSTEM_LIBS= -lc -lw -lgen -lintl -lm -lnsl -lsocket -ldl
#endif

#---------------------------------------------------------------------
# Put all the libraries together with their paths
#---------------------------------------------------------------------

        LIBS = -L$(CLAS_LIB) $(BOSLIBS) $(SYSTEM_LIBS)
        LIBSC = -L$(CLAS_LIB) $(BOSLIBS) $(CERNLIBS) $(SYSTEM_LIBS) 

#---------------------------------------------------------------------
# This variable lists the subdirectories to search for include files.
#---------------------------------------------------------------------

      CLAS_PACK = /usr/local/clas/offline/clas/builds/PRODUCTION/packages
      INCLUDES = -I. -I$(CLAS_PACK) -I$(CLAS_PACK)/include 

#---------------------------------------------------------------------
# How to make a .o file from a .c file 
#---------------------------------------------------------------------

.c.o :
	$(CC) $(CFLAGS) $(C_DEFINES) $(INCLUDES) -c $<

all: pedo
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

clean:
	$(RM) *.o core

install: exports
exports:
	cp pedo $(CLON_BIN)
	cp *.C $(ROOTSYS)/macros

distclean:
	$(RM) *.o *.root pedo core
#---------------------------------------------------------------------
#additional dependencies
#---------------------------------------------------------------------

INCLUDEFILES =  $(CLAS_PACK)/include/demux_roc.h \
                $(CLAS_PACK)/include/bostypes.h   \
                $(CLAS_PACK)/include/bosddl.h

pedo.o: $(INCLUDEFILES)








