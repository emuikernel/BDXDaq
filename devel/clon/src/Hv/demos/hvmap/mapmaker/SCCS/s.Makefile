h53686
s 00000/00000/00000
d R 1.2 02/08/26 22:16:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvmap/mapmaker/Makefile
e
s 00035/00000/00000
d D 1.1 02/08/26 22:16:05 boiarino 1 0
c date and time created 02/08/26 22:16:05 by boiarino
e
u
U
f e 0
t
T
I 1
#
# Makefile for the Hv/demos/hvmap/mapmaker directory
#

CC = cc
RM = rm -f
SYSLIBS = -lm
CFLAGS = -O
LDFLAGS = 
LOCAL_LIBRARIES = 


SRCS=	MapReader.c

OBJS = ${SRCS:.c=.o}

PROGRAM = mapreader

.c.o:
	$(RM) $@
	$(CC) -c  $(CFLAGS) $*.c

$(PROGRAM): $(OBJS) 
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(SYSLIBS) $(LOCAL_LIBRARIES)

MapReader.o: maps.h MapReader.c
	$(RM) MapReader.o
	$(CC) -c  $(CFLAGS) MapReader.c

distclean: clean
clean:
	$(RM) $(PROGRAM) *.o core


E 1
