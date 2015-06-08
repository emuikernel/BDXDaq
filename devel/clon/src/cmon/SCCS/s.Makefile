h60957
s 00001/00001/00310
d D 1.13 03/12/16 22:17:34 boiarino 14 13
c .
e
s 00003/00002/00308
d D 1.12 03/06/20 18:22:08 boiarino 13 12
c *** empty log message ***
e
s 00007/00008/00303
d D 1.11 03/04/17 16:49:26 boiarino 12 11
c *** empty log message ***
e
s 00002/00002/00309
d D 1.10 02/05/09 23:12:47 boiarino 11 10
c codatt <--> bosio
c 
e
s 00003/00001/00308
d D 1.9 02/04/30 16:08:46 boiarino 10 9
c add TTLIB 
c 
e
s 00008/00001/00301
d D 1.8 02/04/30 14:27:05 boiarino 9 8
c add gensglib
c 
e
s 00008/00004/00294
d D 1.7 02/04/30 12:28:27 boiarino 8 7
c add codatt
c 
e
s 00001/00001/00297
d D 1.6 01/11/27 11:32:05 boiarino 7 6
c minor
c 
e
s 00001/00013/00297
d D 1.5 01/11/26 13:44:27 boiarino 6 5
c remove minuit and root from here, move it one level up
c 
e
s 00006/00003/00304
d D 1.4 01/11/19 23:38:31 boiarino 5 4
c add targets LIBMINUIT and LIBCMROOT
c 
e
s 00013/00002/00294
d D 1.3 01/11/19 23:13:27 boiarino 4 3
c add subdir Makes
c 
e
s 00002/00001/00294
d D 1.2 01/11/19 19:38:10 boiarino 3 1
c sone fixes
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:13:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/Makefile
e
s 00295/00000/00000
d D 1.1 01/11/19 19:13:07 boiarino 1 0
c date and time created 01/11/19 19:13:07 by boiarino
e
u
U
f e 0
t
T
I 1
#---------------------------------------------------------------------------
#
# Makefile for 'cmon' directory
#
# You may issue one of the following command:
#  > "make clean" -Clean (touch: all CMON dirs, *.F, *.inc).
#  > "make lib"   -Updates Library with modified *.F files.
#  > "make exe"   -Updates Library & makes Executable module.
#  > "make full"  -Make Clean & new Library & new Executable (new include!).
#---------------------------------------------------------------------------

SHELL = /bin/sh

MYBIN   = $(CLON_BIN)
MYLIB   = $(CLON_LIB)
MYINC   = $(CLON_INC)
OBJ      = ./$(shell uname)/obj

ifeq ("$(shell uname)","HP-UX")
  INC      = ./sda.inc -I./sim.inc
  locallib = ./HPUX/lib
  localbin = ./HPUX/bin
  BOSLIB   = ./HPUX/lib
  XLIB     = /usr/lib/X11R5
# Machine dependent compiler flags and CLAS definitions
  FC = fort77
  FFLAGS = +ppu +O2 -I$(INC) -DHPUX 
  CC = cc
  CFLAGS = -Aa -Ae
  LD = $(FC)
  SHLD = CC -z
  SHLIBS = -lm
endif

ifeq ("$(shell uname)","SunOS")
  locallib = ./SunOS/lib
  localbin = ./SunOS/bin
  BOSLIB   = ../bosio/SunOS/lib
I 10
  TTLIB   = ../codatt
E 10
I 3
  MAPLIB   = $(CLON_LIB)
E 3
  XLIB     = /usr/openwin/lib -L/usr/lib -lsocket -lnsl
D 8
  INC      = ./sda.inc -I./sim.inc -I./recsis.inc -I../bosio/bosio.s 
E 8
I 8
  INC      = ./sda.inc -I./sim.inc -I./recsis.inc -I../bosio/bosio.s \
D 14
				-I../codatt 
E 14
I 14
				-I../codatt -I$(CODA)/common/include
E 14
E 8
# Machine dependent compiler flags and CLAS definitions
# -> use ' -p' for profiling (in both compiling and linking !) !!!
# -> use ' -KPIC -mt' for multithreaded version !!!
D 12
# -> use ' -fast' for optimization !!!
E 12
I 12
# -> use ' -fast' for 32bit or '-xarch=v9' for 64bit (NOT TOGETHER !!!)
#    or '-g' for debugger
E 12
  FC = f77 
  FFLAGS = -fast -KPIC -mt -DSunOS -e -ftrap=no%inexact,no%underflow -I$(INC)
  CC = cc 
  CFLAGS = -fast -KPIC -mt -DSunOS -I$(INC)
D 12
  LD = $(FC) -z muldefs 
E 12
I 12
  LD = $(FC) -fast -z muldefs 
E 12
  SHLD = CC -G -fast
endif

ifeq ("$(shell uname)","AIX")
  INC      = ./sda.inc -I./sim.inc
  locallib = ./aix/lib
  localbin = ./aix/bin
  BOSLIB   = ./aix/lib       
  XLIB     = -L/usr/lib
# Machine dependent compiler flags and CLAS definitions
  FC = xlf
  FFLAGS = -O -qextname -WF,-I$(INC) -DAIX 
  CFLAGS = -I$(INC)
  LD = $(FC)
endif

ifeq ("$(shell uname)","Linux")
D 8
  INC      = ./sda.inc  -I./sim.inc -I../bosio/bosio.s 
E 8
I 8
  INC      = ./sda.inc  -I./sim.inc -I../bosio/bosio.s \
				-I../codatt
E 8
  locallib = ./Linux/lib
  localbin = ./Linux/bin
  BOSLIB   = ../bosio/Linux/lib
I 10
  TTLIB   = ../codatt
E 10
  XLIB     = /usr/X11R6/lib
# Machine dependent compiler flags and CLAS definitions
  FC = g77
D 8
  FFLAGS = -O2 -m486 -fno-automatic -finit-local-zero -ffixed-line-length-none \
	-fno-second-underscore -DLinux -I$(INC)
E 8
I 8
  FFLAGS = -O2 -m486 -fno-automatic -finit-local-zero \
			-ffixed-line-length-none -fno-second-underscore -DLinux -I$(INC)
E 8
  CC = gcc
  CFLAGS = -O2 -DLinux -fwritable-strings -I$(INC)
  LD = $(FC)
  SHLD = g++ -Wl,-soname,libBosio.so -shared -g
endif

#------------------------------
# Set up CERN & MATH libraries
#------------------------------

MATHLIB = /lib
CERNLIB = $(CERN_ROOT)/lib

#----------------------------------------------------------------
# List of subdirectories containing FORTRAN and/or C source code
#----------------------------------------------------------------

SOURCE_FILES = $(wildcard *.s/*.[cF])
SUB_DIRECTORIES = $(sort $(dir $(SOURCE_FILES)))

#--------------------------------------------------------------------------
# Prepare include file search path. All include files should be referenced
# with respect to.
#--------------------------------------------------------------------------

#-------------------------------------
# CMON, CLAS and CERN object libraries
#-------------------------------------

LIBNAMES   = cmon
D 11
LIBNAMES  += bosio
E 11
I 8
LIBNAMES  += codatt
I 11
LIBNAMES  += bosio
E 11
E 8
D 12
LIBNAMES  += mapmanager
E 12
LIBNAMES  += X11 m

LIBNAMESCERN   = cmon
D 11
LIBNAMESCERN  += bosio
E 11
I 8
LIBNAMESCERN  += codatt
I 11
LIBNAMESCERN  += bosio
E 11
E 8
D 12
LIBNAMESCERN  += mapmanager
E 12
D 7
LIBNAMESCERN  += min
E 7
I 7
LIBNAMESCERN  += minuit1
E 7
LIBNAMESCERN  += graflib
LIBNAMESCERN  += grafX11
LIBNAMESCERN  += packlib
LIBNAMESCERN  += X11 m

#
#===================== End of MODIFICATION Section =========================
#

#--------------------
# Set up the targets
#--------------------

LIBRARY = $(locallib)/libcmon.a
I 4
D 6
LIBMINUIT = $(locallib)/libmin.a
I 5
LIBCMROOT = root/libCMRoot.so
E 6
E 5
E 4

#-----------------------------------------
# Prepare libraries and their search path
#-----------------------------------------

LIBS = $(patsubst %,-l%,$(LIBNAMES))

LIBSCERN = $(patsubst %,-l%,$(LIBNAMESCERN))

D 3
VPATH_DIR = $(locallib):$(MATHLIB):$(BOSLIB):$(CERNLIB):$(XLIB):../Map
E 3
I 3
D 10
VPATH_DIR = $(locallib):$(MATHLIB):$(BOSLIB):$(MAPLIB):$(CERNLIB):$(XLIB)
E 10
I 10
VPATH_DIR = $(locallib):$(MATHLIB):$(TTLIB):$(BOSLIB):$(MAPLIB):$(CERNLIB):$(XLIB)
E 10
E 3

LIBS_DIR  = -L$(subst :, -L,$(VPATH_DIR))

vpath %.a $(strip $(VPATH_DIR)):/usr/local/lib:/usr/lib
VPATH =.:$(patsubst %/,:%,$(SUB_DIRECTORIES))

#----------------------------------------------
# build up the list of source and object files
#----------------------------------------------

FORTRAN_SEARCH  = $(patsubst %,%*.F, . $(SUB_DIRECTORIES))
FORTRAN_SOURCES = $(wildcard $(FORTRAN_SEARCH))
NOTDIR_F_SOURCES = $(notdir $(FORTRAN_SOURCES))

C_SEARCH  = $(patsubst %,%*.c, . $(SUB_DIRECTORIES))
C_SOURCES = $(wildcard $(C_SEARCH))

NOTDIR_C_SOURCES = $(notdir $(C_SOURCES))

OBJS  = $(NOTDIR_F_SOURCES:%.F=$(LIBRARY)(%.o)) \
	$(NOTDIR_C_SOURCES:%.c=$(LIBRARY)(%.o))

I 4
all: exe

E 4
exe: dirs lib shlib $(localbin)/cmonbat $(localbin)/cmonint $(localbin)/cmonsim \
		$(localbin)/cmonminuit $(localbin)/makedict $(localbin)/makefulldict \
D 9
		$(localbin)/mergedict $(localbin)/customdict $(localbin)/map2dat
E 9
I 9
		$(localbin)/mergedict $(localbin)/customdict $(localbin)/map2dat \
		$(localbin)/gensglib
E 9

full: clean exe

D 4
lib: $(LIBRARY)
E 4
I 4
D 5
lib: $(LIBRARY) $(LIBMINUIT) $(ROOT)
E 5
I 5
D 6
lib: $(LIBRARY) $(LIBMINUIT) $(LIBCMROOT)
E 6
I 6
lib: $(LIBRARY)
E 6
E 5
E 4

dirs:
	if (test ! -d $(shell uname)); then \
		mkdir $(shell uname); \
		mkdir $(shell uname)/lib; \
		mkdir $(shell uname)/obj; \
		mkdir $(shell uname)/bin; \
	fi

I 13
distclean: clean
E 13
clean:
D 4
	rm -f $(locallib)/libcmon.a $(locallib)/libCmon.so
E 4
I 4
	rm -f $(locallib)/*.a $(locallib)/*.so
E 4
	rm -f $(OBJ)/*.o $(locallib)/*.o
	rm -f $(localbin)/cmonint $(localbin)/cmonbat
	rm -f $(localbin)/makedict $(localbin)/makefulldict
	rm -f $(localbin)/customdict $(localbin)/mergedict
	rm -f $(localbin)/cmonminuit $(localbin)/cmonsim
	rm -f $(localbin)/map2dat
I 9
	rm -f $(localbin)/gensglib
E 9
I 4
D 6
	cd minuit; $(MAKE) clean
	cd root; $(MAKE) clean
E 6
E 4

install:
	cp $(locallib)/libcmon.a $(MYLIB)
	cp $(locallib)/libCmon.so $(MYLIB)
I 5
D 6
	cd minuit; $(MAKE) install
	cd root;   $(MAKE) install
E 6
E 5

$(LIBRARY): $(OBJS)
	@$(AR) t $(LIBRARY)

I 4
D 5
$(LIBMINUIT):
E 5
I 5
D 6
$(LINMINUIT):
E 5
	cd minuit; $(MAKE)

D 5
$(ROOT):
E 5
I 5
$(LIBCMROOT):
E 5
	cd root; $(MAKE)

E 6
E 4
shlib:
	$(SHLD) $(OBJ)/*.o $(SHLIBS) -o $(locallib)/libCmon.so

$(localbin)/cmonminuit: $(locallib)/sda_minuit.o $(patsubst %, lib%.a,$(LIBNAMESCERN))
	$(LD) $(locallib)/sda_minuit.o $(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonminuit

$(localbin)/makedict: $(locallib)/makedict.o $(locallib)/prlib_sim.o \
		$(patsubst %, lib%.a,$(LIBNAMES))
	$(LD) $(locallib)/makedict.o $(locallib)/prlib_sim.o $(LIBS_DIR) $(LIBS) \
		-o $(localbin)/makedict

$(localbin)/makefulldict: $(locallib)/makefulldict.o $(locallib)/prlib_sim.o \
		$(patsubst %, lib%.a,$(LIBNAMES))
	$(LD) $(locallib)/makefulldict.o $(locallib)/prlib_sim.o $(LIBS_DIR) $(LIBS) \
		-o $(localbin)/makefulldict

$(localbin)/mergedict: $(locallib)/mergedict.o $(locallib)/prlib_sim.o \
		$(patsubst %, lib%.a,$(LIBNAMES))
	$(LD)	$(locallib)/mergedict.o $(locallib)/prlib_sim.o $(LIBS_DIR) $(LIBS) \
		-o $(localbin)/mergedict

$(localbin)/customdict: $(locallib)/customdict.o $(locallib)/prlib_sim.o \
		$(patsubst %, lib%.a,$(LIBNAMES))
	$(LD) $(locallib)/customdict.o $(locallib)/prlib_sim.o $(LIBS_DIR) $(LIBS) \
		-o $(localbin)/customdict

$(localbin)/map2dat: main/map2dat.c
	$(CC) main/map2dat.c $(LIBS_DIR) $(LIBS) -o $(localbin)/map2dat

I 9
$(localbin)/gensglib: main/gensglib.c $(patsubst %, lib%.a,$(LIBNAMES))
	$(CC) main/gensglib.c $(LIBS_DIR) $(LIBS) -o $(localbin)/gensglib
E 9


D 12
$(localbin)/cmonbat: $(locallib)/main_bat.o $(patsubst %, libcmon.a,$(LIBNAMESCERN))
	$(LD) $(locallib)/main_bat.o $(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonbat

#$(localbin)/cmonbat: $(locallib)/main_bat.o $(patsubst %, lib%.a,$(LIBNAMESCERN))
E 12
I 12
#$(localbin)/cmonbat: $(locallib)/main_bat.o $(patsubst %, libcmon.a,$(LIBNAMESCERN))
E 12
#	$(LD) $(locallib)/main_bat.o $(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonbat

I 12
$(localbin)/cmonbat: $(locallib)/main_bat.o $(patsubst %, lib%.a,$(LIBNAMESCERN))
	$(LD) $(locallib)/main_bat.o $(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonbat
E 12

I 12

E 12

$(localbin)/cmonint: $(locallib)/main_int.o $(locallib)/traceq.o $(locallib)/sda_def.o \
		$(locallib)/utkinit.o $(patsubst %, lib%.a,$(LIBNAMESCERN))
	$(LD) $(locallib)/main_int.o $(locallib)/traceq.o $(locallib)/sda_def.o \
		$(locallib)/utkinit.o $(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonint

$(localbin)/cmonsim: $(locallib)/main_bat.o $(locallib)/prlib_sim.o $(locallib)/traceq.o \
		$(patsubst %, lib%.a,$(LIBNAMESCERN))
	$(LD) $(locallib)/main_bat.o $(locallib)/traceq.o $(locallib)/prlib_sim.o \
		$(LIBS_DIR) $(LIBSCERN) -o $(localbin)/cmonsim

# compile files from 'main' directory

$(locallib)/prlib_sim.o: prlib.s/prlib.c
	$(CC) -o $(locallib)/prlib_sim.o -c $(CFLAGS) -DSIM prlib.s/prlib.c

$(locallib)/main_bat.o: main/main_bat.F
	$(FC) -o $(locallib)/main_bat.o -c $(FFLAGS) main/main_bat.F

$(locallib)/main_int.o: main/main_int.F
	$(FC) -o $(locallib)/main_int.o -c $(FFLAGS) main/main_int.F

$(locallib)/sda_minuit.o: main/sda_minuit.F
	$(FC) -o $(locallib)/sda_minuit.o -c $(FFLAGS) main/sda_minuit.F

$(locallib)/sda_def.o: main/sda_def.F
	$(FC) -o $(locallib)/sda_def.o -c $(FFLAGS) main/sda_def.F

$(locallib)/traceq.o: main/traceq.F
	$(FC) -o $(locallib)/traceq.o -c $(FFLAGS) main/traceq.F

$(locallib)/makedict.o: main/makedict.c
	$(CC) -o $(locallib)/makedict.o -c $(CFLAGS) main/makedict.c

$(locallib)/makefulldict.o: main/makefulldict.c
	$(CC) -o $(locallib)/makefulldict.o -c $(CFLAGS) main/makefulldict.c

$(locallib)/mergedict.o: main/mergedict.c
	$(CC) -o $(locallib)/mergedict.o -c $(CFLAGS) main/mergedict.c

$(locallib)/customdict.o: main/customdict.c
	$(CC) -o $(locallib)/customdict.o -c $(CFLAGS) main/customdict.c

$(locallib)/map2dat.o: main/map2dat.c
	$(CC) -o $(locallib)/map2dat.o -c $(CFLAGS) main/map2dat.c

I 9
$(locallib)/gensglib.o: main/gensglib.c
	$(CC) -o $(locallib)/gensglib.o -c $(CFLAGS) main/gensglib.c

E 9
$(locallib)/utkinit.o: main/utkinit.c
	$(CC) -o $(locallib)/utkinit.o -c $(CFLAGS) main/utkinit.c

# create library

.F.a:
	$(FC) $(FFLAGS) -c $<
	@$(AR) r $@ $%
	mv $% $(OBJ)

.c.a:
	$(CC) $(CFLAGS) -c $<
	@$(AR) r $@ $%
	mv $% $(OBJ)

I 13
# dependencies to outside packages
#$(locallib)/sglib.o:	../codatt/sgutil.h
E 13


D 13


E 13

E 1
