#  makes dbrouter

TCLINC  = $(CODA)/common/include
TCLLIB  = $(TCL_LIBRARY)

all: dbrouter

dbrouter: dbrouter.o dbrsql.o
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -pthread \
		-lipc -lutil \
		-L$(CODA_LIB) -ltcl7.4 -L$(II_SYSTEM)/ingres/lib -lingres -lposix4 \
		-lelf


dbrouter.o: dbrouter.h dbrouter.cc
	CC -c -g -I. -I$(CLON_INC) -I$(TCLINC) -I$(RTHOME)/include dbrouter.cc


dbrsql.o: dbrouter.h dbrsql.scc
	esqlcc dbrsql.scc
	CC -c -g -I. -I$(TCLINC) -I$(RTHOME)/include dbrsql.cc

clean:
	rm -f *.o dbrsql.cc dbrouter

distclean:
	rm -f *.o dbrsql.cc dbrouter

install: exports
exports:
	cp dbrouter $(CLON_BIN)
