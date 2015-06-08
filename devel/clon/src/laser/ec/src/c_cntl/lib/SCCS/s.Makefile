h58092
s 00000/00000/00000
d R 1.2 00/09/21 14:12:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/Makefile
e
s 00025/00000/00000
d D 1.1 00/09/21 14:12:44 wolin 1 0
c date and time created 00/09/21 14:12:44 by wolin
e
u
U
f e 0
t
T
I 1
#  makefile by ejw,( modified by vhg ) sb 12-96 (first one)

CC =  cc -c
AR = ar -r liblaser.a

INC_ECLASER   = -I../include
LINK_ECLASER  = -L. -llaser
INC_CLAS    = -I$(CLON_INC)
LINK_CLAS   = -L$(CLON_LIB) -lipc -lutil 
INC_CODA    = -I$(CODA)/common/include
LINK_CODA  = -L$(CODA)/SunOS/lib -lmsql -ldd -ldpS -L/usr/local/coda/2.0b1/SunOS/lib -lca 

all: parse

parse:parse.c
	$(CC) parse.c $(INC_CLAS) $(INC_CODA) $(INC_ECLASER)  
	$(AR) parse.o

check:
	ar -t liblaser.a	
	nm liblaser.a	

clean:
	rm -f *.o 
	 
E 1
