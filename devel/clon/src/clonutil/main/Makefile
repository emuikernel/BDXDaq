#
# builds test routines
#

DEBUG = -g
PROG = run_status
PROG = run_status run_config run_number


all: $(PROG)


run_status: run_status.o
	CC $(DEBUG) -o run_status run_status.o ../s/get_run_status.o \
		../s/get_run_config.o -L$(CDEVROOT)/lib -lcdev -L$(CODA_LIB) \
		-lmysql4.1.20 -lsocket -lnsl -lposix4


run_config: run_config.o
	CC $(DEBUG) -o run_config run_config.o -L../s -lutil -L$(CODA_LIB) \
		-lmysql4.1.20 -lsocket -lnsl -lposix4


run_number: run_number.o
	CC $(DEBUG) -o run_number run_number.o -L../s -lutil -L$(CODA_LIB) \
		-lmysql4.1.20 -lsocket -lnsl -lposix4


.cc.o:
	CC -c $(DEBUG) $<


clean:
	rm -f *.o

distclean:
	rm -f *.o $(PROG)


install: exports
exports:
	cp $(PROG) $(CLON_BIN)
