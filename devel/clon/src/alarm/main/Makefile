
#  makefile for online alarm system tilities

#  ejw, 15-jan-97

###DEBUG = -g

PROG = alarm_handler alarm_server alarm_mgr.class


all: $(PROG)
 

alarm_server: alarm_server.scc
	esqlcc alarm_server.scc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC)  -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -ltcl -L$(II_SYSTEM)/ingres/lib/ -lingres -lposix4 -lelf


alarm_handler: alarm_handler.o
	rtlink -cxx $(DEBUG) -o $@ $@.o -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -ltcl -lposix4

 
alarm_mgr.class: alarm_mgr.java
	javac alarm_mgr.java


.cc.o:
	CC -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include $<
 
 
clean:
	rm -f *.o core alarm_server.cc $(PROG)


distclean:
	rm -f *.o core alarm_server.cc $(PROG)


install: exports
exports:
	cp $(PROG) $(CLON_BIN)

