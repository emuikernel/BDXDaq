h06029
s 00001/00001/00031
d D 1.8 03/12/22 17:45:58 boiarino 9 8
c add  -I../tty6016
e
s 00005/00011/00027
d D 1.7 03/11/27 21:22:02 boiarino 8 7
c .
e
s 00013/00014/00025
d D 1.6 03/03/03 14:02:26 boiarino 7 6
c first version for 2306 CPU
e
s 00001/00003/00038
d D 1.5 02/03/27 17:45:24 boiarino 6 5
c minor
c 
e
s 00001/00001/00040
d D 1.4 00/09/07 11:49:21 wolin 5 4
c No ipGS at all
e
s 00002/00000/00039
d D 1.3 00/09/06 11:05:18 wolin 4 3
c Added targets
e
s 00008/00003/00031
d D 1.2 00/08/10 16:40:10 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/Makefile
e
s 00034/00000/00000
d D 1.1 00/08/03 10:53:14 gurjyan 1 0
c date and time created 00/08/03 10:53:14 by gurjyan
e
u
U
f e 0
t
T
I 1
#
# Description:
D 7
#   	Makefile for MV code
E 7
I 7
#   	Makefile for PPC code
E 7
#
D 8
#
E 8
I 8
include $(CLON_SOURCE)/vxworks/Makefile.include
E 8
D 3
#LIBDIR = /apps/tornado/68k/target/lib
INCDIR = /apps/tornado/68k/target/h
E 3
I 3

D 6
WIND_TARGET_TYPE=68k
E 6
D 7
GCC_EXEC_PREFIX=/apps/tornado/68k/host/gnu/sun4-solaris2/lib/gcc-lib/
D 6

#LIBDIR = /site/vxworks/5.3/68k/target/lib
E 6
INCDIR = /site/vxworks/5.3/68k/target/h
E 7
I 7
D 8
INCDIR = $(WIND_BASE)/target/h
E 8
I 8
D 9
INCDIR = -I. -I$(INCS) -I$(CODA)/source/camac
E 9
I 9
INCDIR = -I. -I$(INCS) -I$(CODA)/source/camac -I../tty6016
E 9
E 8
E 7
I 6

E 6
E 3
D 8
CAMAC = $(CODA)/source/camac
D 7
CC = cc68k
DEFS = -DCPU=MC68020 -DVXWORKS
INCS = -Wall -mc68020 -fvolatile -fstrength-reduce -nostdinc -I. -I$(INCDIR) -I$(CAMAC)
CFLAGS = -O $(DEFS)
E 7
I 7
CC = ccppc
DEFS = -DCPU=PPC604 -DVXWORKS -DVXWORKSPPC -D_GNU_TOOL
INCS = -I. -I$(INCDIR) -I$(CAMAC)
CFLAGS = -O -fno-builtin -fno-for-scope -nostdinc $(DEFS)
E 7

E 8
D 3

E 3
D 7
OBJS = calsys_commands.o laser.o motor.o
E 7
I 7
OBJS = calsys_commands_ppc.o laser_ppc.o motor_ppc.o
E 7

all: $(OBJS)

D 7
calsys_commands.o : calsys_commands.c
	$(CC) -c $(CFLAGS) calsys_commands.c $(INCS) -o calsys_commands.o
E 7
I 7
calsys_commands_ppc.o : calsys_commands.c
D 8
	$(CC) -c $(CFLAGS) calsys_commands.c $(INCS) -o calsys_commands_ppc.o
E 8
I 8
	$(CC) -c $(CFLAGS) calsys_commands.c $(INCDIR) -o calsys_commands_ppc.o
E 8
E 7

D 7
laser.o : laser.c
	$(CC) -c $(CFLAGS) laser.c $(INCS) -o laser.o
E 7
I 7
laser_ppc.o : laser.c
D 8
	$(CC) -c $(CFLAGS) laser.c $(INCS) -o laser_ppc.o
E 8
I 8
	$(CC) -c $(CFLAGS) laser.c $(INCDIR) -o laser_ppc.o
E 8
E 7

D 7
motor.o : motor.c
	$(CC) -c $(CFLAGS) motor.c $(INCS) -o motor.o
E 7
I 7
motor_ppc.o : motor.c
D 8
	$(CC) -c $(CFLAGS) motor.c $(INCS) -o motor_ppc.o
E 8
I 8
	$(CC) -c $(CFLAGS) motor.c $(INCDIR) -o motor_ppc.o
E 8
E 7

I 4
distclean: clean
E 4
clean:
	rm -f $(OBJS)


I 4
install: exports
E 4
exports:
D 5
	cp $(OBJS) ipGS.o ipGS_klg.o  $(CLON_VXWORKS)/code
E 5
I 5
	cp $(OBJS) $(CLON_VXWORKS)/code
E 5
I 3


E 3

E 1
