h45446
s 00001/00001/00077
d D 1.11 02/09/09 11:44:40 boiarino 12 11
c use libHvold
e
s 00003/00005/00075
d D 1.10 02/09/09 11:25:54 boiarino 11 10
c use old HV, new one is incompartible yet
c some cleanup
e
s 00002/00002/00078
d D 1.9 02/08/25 23:38:05 boiarino 10 9
c take Hv headers and lib from $(HVPATH)/s
e
s 00002/00002/00078
d D 1.8 02/08/23 19:57:50 boiarino 9 8
c minor
e
s 00001/00001/00079
d D 1.7 02/08/22 22:03:15 boiarino 8 7
c use our Hv
e
s 00001/00001/00079
d D 1.6 02/08/22 20:38:58 boiarino 7 6
c rm tigrisLite.o in 'clean'
e
s 00021/00008/00059
d D 1.5 02/07/06 23:43:39 boiarino 6 5
c add *_OLD programs
c 
e
s 00001/00001/00066
d D 1.4 02/06/30 23:23:46 boiarino 5 4
c use local ../libvme_download.a
c 
e
s 00003/00002/00064
d D 1.3 02/06/30 12:02:21 boiarino 4 3
c add -I../
c 
e
s 00001/00000/00065
d D 1.2 02/03/29 12:22:26 boiarino 3 1
c 	cp ascii.geo $(CLON_PARMS)/trigger
c 
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/Makefile
e
s 00065/00000/00000
d D 1.1 02/03/29 12:12:59 boiarino 1 0
c date and time created 02/03/29 12:12:59 by boiarino
e
u
U
f e 0
t
T
I 1
######################################################################
#
# makefile for the 'tigris' directory: Tigris_PPC, TigrisLite_PPC etc
#
######################################################################

CC = cc

D 8
HVPATH = /home/heddle/Hv
E 8
I 8
HVPATH = ../../../Hv
E 8
D 10
HVINC  = $(HVPATH)/src
HVLIB  = $(HVPATH)/SunOS
E 10
I 10
D 11
HVINC  = $(HVPATH)/s
HVLIB  = $(HVPATH)/s
E 11
I 11
HVINC  = $(HVPATH)/sold
HVLIB  = $(HVPATH)/sold
E 11
E 10
D 5
vme_stuff = $(CLON_LIB)/libvme_download.a 
E 5
I 5
vme_stuff = ../libvme_download.a 
E 5

MOTIFLIBPATH = /usr/dt/lib
XLIBPATH   = /usr/openwin/lib
D 12
XLIBS = -L$(HVLIB) -lHv -L$(MOTIFLIBPATH) -lXm -L$(XLIBPATH) \
E 12
I 12
XLIBS = -L$(HVLIB) -lHvold -L$(MOTIFLIBPATH) -lXm -L$(XLIBPATH) \
E 12
		-lXt -lX11 -lnsl -lsocket
LIBS = $(vme_stuff) $(XLIBS) -lm -lc

SRCS = main.c init.c setup.c feedback.c ced_geometry.c scintillator.c \
D 6
		textitem.c evbutton.c sectorhex.c sectbuttons.c fileio.c scintbuts.c \
E 6
I 6
		textitem.c evbutton.c sectorhex.c sectbuttons.c scintbuts.c \
E 6
		cradle.c menus.c setupview.c
OBJS = ${SRCS:.c=.o}

CFLAGS = -g $(PLATFORMFLAGS) -c
LDFLAGS = -g

INCLUDEDIR = /usr/include/X11
HV_XINCLUDEDIR = /usr/openwin/include
D 11
HV_XMINCLUDEDIR = /opt/SUNWspro/Motif_Solaris24/dt/include
E 11
HV_XLIBDIR = /usr/openwin/lib
HV_XMLIBDIR = /usr/dt/lib
HVINCLUDEDIR = $(HVINC)
D 4
INCLUDES =  -I$(INCLUDEDIR) -I$(HVINCLUDEDIR) -I$(HV_XMINCLUDEDIR) \
E 4
I 4
D 11
INCLUDES =   -I../ -I$(INCLUDEDIR) -I$(HVINCLUDEDIR) -I$(HV_XMINCLUDEDIR) \
E 4
			-I$(HV_XINCLUDEDIR)
E 11
I 11
INCLUDES =   -I../ -I$(INCLUDEDIR) -I$(HVINCLUDEDIR) -I$(HV_XINCLUDEDIR)
E 11

#####################################################

D 6
all: Tigris_PPC TigrisLite_PPC
E 6
I 6
all: Tigris_PPC Tigris_OLD TigrisLite_PPC TigrisLite_OLD
E 6

D 6
Tigris_PPC: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
E 6
I 6
Tigris_PPC: $(OBJS) fileio.o
	$(CC) -o $@ fileio.o $(LDFLAGS) $(OBJS) $(LIBS)
E 6

D 6
TigrisLite_PPC: $(OBJS)
D 4
	${CC} -o TigrisLite_PPC tigrisLite.c -I$(HVINC) $(LIBS) -lnsl -lsocket
E 4
I 4
	${CC} -o TigrisLite_PPC tigrisLite.c  -I../ -I$(HVINC) $(LIBS) -lnsl \
				-lsocket
E 6
I 6
Tigris_OLD: $(OBJS) fileio_old.o
	$(CC) -o $@ fileio_old.o $(LDFLAGS) $(OBJS) $(LIBS)
E 6
E 4

I 6
D 9
TigrisLite_PPC: $(OBJS) fileio.o
E 9
I 9
TigrisLite_PPC: $(OBJS) tigrisLite.c fileio.o
E 9
	${CC} -o TigrisLite_PPC tigrisLite.c -I../ -I$(HVINC) fileio.o \
				$(LIBS) -lnsl -lsocket

D 9
TigrisLite_OLD: $(OBJS) fileio_old.o
E 9
I 9
TigrisLite_OLD: $(OBJS) tigrisLite.c fileio_old.o
E 9
	${CC} -o TigrisLite_OLD -DOLDLOAD tigrisLite.c -I../ \
				-I$(HVINC) fileio_old.o $(LIBS) -lnsl -lsocket

fileio_old.o: fileio.c
	$(CC) $(INCLUDES) $(CFLAGS) -DOLDLOAD -o fileio_old.o fileio.c

E 6
.c.o :
	$(CC) $(INCLUDES) $(CFLAGS) $<

.PHONY : clean

clean: distclean
distclean:
D 6
	rm -f $(OBJS) TigrisLite_PPC Tigris_PPC
E 6
I 6
D 7
	rm -f $(OBJS) fileio.o fileio_old.o
E 7
I 7
	rm -f $(OBJS) fileio.o fileio_old.o tigrisLite.o
E 7
	rm -f TigrisLite_PPC Tigris_PPC TigrisLite_OLD Tigris_OLD
E 6

install: exports
exports:
	cp TigrisLite_PPC $(CLON_BIN)
	cp Tigris_PPC $(CLON_BIN)
I 6
	cp TigrisLite_OLD $(CLON_BIN)
	cp Tigris_OLD $(CLON_BIN)
E 6
I 3
	cp ascii.geo $(CLON_PARMS)/trigger
E 3






E 1
