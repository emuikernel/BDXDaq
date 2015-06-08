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


