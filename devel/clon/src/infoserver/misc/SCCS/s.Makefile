h03690
s 00005/00002/00020
d D 1.3 00/10/30 11:46:19 wolin 4 3
c Fixed distclean
e
s 00005/00000/00017
d D 1.2 00/09/06 10:45:44 wolin 3 1
c Added targets
e
s 00000/00000/00000
d R 1.2 98/01/22 12:45:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/misc/Makefile
c Name history : 1 0 misc/Makefile
e
s 00017/00000/00000
d D 1.1 98/01/22 12:45:04 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
#
# Makefile for scaler info-server
#
INC = -I../includes

CFLAGS = $(INC)

all: killer sender

killer: killer.c ../includes/servmsg.h ../includes/argproc.h
	rtlink $(CFLAGS) -o killer killer.c

sender: evtsender.c ../includes/servmsg.h ../includes/argproc.h
	rtlink $(CFLAGS) -o sender evtsender.c 

I 3
D 4
distclean: clean
E 4
I 4

distclean:
	rm -f *.o killer sender

E 4
E 3
clean:
D 4
	rm *.o
E 4
I 4
	rm -f *.o
E 4
I 3

install: exports
exports:
	echo "Nothing to do for install in misc"
E 3
E 1
