h20044
s 00001/00002/00062
d D 1.8 02/08/09 10:23:52 boiarino 9 8
c minor
c 
e
s 00005/00002/00059
d D 1.7 02/07/30 18:08:50 boiarino 8 7
c remove -lca
c 
e
s 00001/00001/00060
d D 1.6 02/07/30 17:39:24 boiarino 7 6
c remove executbles in 'clean'
c 
e
s 00002/00001/00059
d D 1.5 00/09/07 11:20:51 wolin 6 5
c New CC
e
s 00003/00003/00057
d D 1.4 00/08/18 13:11:43 gurjyan 5 4
c 
e
s 00002/00002/00058
d D 1.3 00/08/07 16:14:34 gurjyan 4 3
c *** empty log message ***
e
s 00001/00001/00059
d D 1.2 00/08/07 15:57:41 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/spardb/unix/Makefile
e
s 00060/00000/00000
d D 1.1 00/08/07 15:57:06 gurjyan 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
D 8
#  makefile by ejw,( modified by vhg ) sb 12-96 (first one)
E 8
I 8
#
# Makefile for pedman/spardb/unix
#
E 8

CC = rtlink -g
CXX = rtlink -cxx -g

D 5
CMLOG = /usr/local/cmlog
CDEV  = /usr/local/cdev
E 5
I 5
CMLOG = $(CMLOGROOT)
CDEV  = $(CDEVROOT)
E 5

INC_CLAS    = -I$(CLON_INC)
LINK_CLAS   = -L$(CLON_LIB) -lipc -lutil 

INC_CDEV  = -I$(CDEV)/include
LINK_CDEV = -L$(CDEV)/lib/solaris -lcdev

INC_CMLOG  = -I$(CMLOG)/include
LINK_CMLOG = -L$(CMLOG)/lib/solaris -lcmlog

INC_CODA    = -I$(CODA)/common/include 
D 5
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldpS -L/usr/local/coda/2.0b1/SunOS/lib -lca 
E 5
I 5
D 8
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldpS -L$(CODA)/SunOS/lib -lca 
E 8
I 8
D 9
#LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldpS -L$(CODA)/SunOS/lib -lca 
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldpS -L$(CODA)/SunOS/lib
E 9
I 9
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldpS -L$(CODA)/SunOS/lib -lca 
E 9
E 8
E 5


INC_TK    = -I$(TCLINCLUDE_DIR) -I/usr/openwin/include  
LINK_TK   = -L$(CLON_LIB) -lipc -L/usr/openwin/lib/ -L$(CODA)/SunOS/lib \
  -lX11 -ltk4.0 -ltcl7.4 -lSQL -lmsql -ldplite 



all : pedman_init pedman_read pedman_write pedman_dump

pedman_init: pedman_init.c 
	$(CC) pedman_init.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) -lC  -o pedman_init

pedman_read: pedman_read.c 
	$(CC) pedman_read.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) -lC  -o pedman_read

pedman_write: pedman_write.c 
	$(CC) pedman_write.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) -lC  -o pedman_write


pedman_dump: pedman_dump.c 
D 6
	$(CC) pedman_dump.c $(INC_CLAS) $(LINK_CLAS) $(INC_CODA) $(LINK_CODA) -lC  -o pedman_dump
E 6
I 6
	$(CC) -c pedman_dump.c $(INC_CLAS) $(INC_CODA)
	$(CXX) pedman_dump.o $(LINK_CLAS) $(LINK_CODA) -lC  -o pedman_dump
E 6

clean :
D 4
	rm *.o 
E 4
I 4
D 7
	rm -f *.o 
E 7
I 7
	rm -f *.o pedman_init pedman_read pedman_write pedman_dump
E 7
E 4

distclean :
D 4
	rm *.o pedman_init pedman_read pedman_write pedman_dump
E 4
I 4
	rm -f *.o pedman_init pedman_read pedman_write pedman_dump
E 4

exports :
D 3
	cp pedman_init pedman_read pedman_write pedman_dump /usr/local/clas/bin/.
E 3
I 3
	cp pedman_init pedman_read pedman_write pedman_dump $(CLON_BIN)
E 3










E 1
