.PHONY : all source examples java clean
.PHONY : install install-lib install-inc install-bin install-jar

all: source examples java

source:
	cd ./src; make;

examples:
	cd ./examples; make;

java:
	cd ./java/org/jlab/coda/et; make;
	cd ./java; make;
	cd ./java; make doc;

install: install-lib install-inc install-bin install-jar

install-lib:
	cd ./src; make install-lib;
	cd ./examples; make install-lib;

install-inc:
	cd ./src; make install-inc;

install-bin:
	cd ./src; make install-bin;
	cd ./examples; make install-bin;

install-jar:
	cd ./java/org/jlab/coda/et; make install-jar;

clean:
	cd ./src; make clean;
	cd ./examples; make clean;
	cd ./java/org/jlab/coda/et; make clean;
	cd ./java; make clean;
