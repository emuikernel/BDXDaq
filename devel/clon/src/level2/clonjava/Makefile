#
#  makefile for euphrates
#  vhg, 7-aug-2000
#
#Sergey: add java suffixes just in case
#Vardan: add JAVAC

.SUFFIXES: .java .class

JAVAC = $(JAVA_HOME)/bin/javac -classpath $(CLON)/share/jar/ss.jar:$(CLON)/share/jar/symbeans.jar

.java.class:
#	/apps/java/jdk1.2.1/bin/javac $<
	$(JAVAC) $<


all: euphrates.class

 
clean:
	rm -f *.class
 
distclean:
	rm -f *.class

 
install: exports
exports:
	cp *.class $(CLON_BIN)
	cp *.gif $(CLON_GIF)
	-chmod ug+rw $(CLON_GIF)/*
