#
#  steering for event_monitor build
#
#  ejw, 6-may-98
#

PROG = event_monitor


all: $(PROG)

$(PROG):
	$(MAKE) -f Makefile.prog $(PROG)

clean:
	rm -f core *.o $(PROG) 

distclean:
	rm -f core *.o $(PROG) 


install: exports
exports:
	cp $(PROG) $(CLON_BIN)
