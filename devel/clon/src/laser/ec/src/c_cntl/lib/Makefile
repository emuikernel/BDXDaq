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
	 
