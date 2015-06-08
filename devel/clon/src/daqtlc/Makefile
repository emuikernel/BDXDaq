# makefile for daqtlc: DAQ expert system for CLAS
# vhg, 24-jul-00


.java.class:
	javac $<

all:
	/bin/csh -s < jmake 
	jar cf tlc.jar *.class


clean:
	rm -f *.class	


distclean:
	rm -f *.class *.jar


install: exports
exports:
	cp tlc.jar $(CLON_JAR)
	cp daqtlc infodaqtlc boy.gif $(CLON_BIN)
	-chmod ug+rwx $(CLON_BIN)/daqtlc
	-chmod a+x $(CLON_BIN)/daqtlc
