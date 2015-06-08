#  makes online jar files

#  ejw, 15-aug-2000


PACKAGES = clon.ipcutil clon.util
JAVAC = /apps/java/jdk1.4.0/bin/javac

.java.class:
	$(JAVAC) $<


all: $(PACKAGES) dplite. jar


clon.ipcutil: clon/ipcutil/*.java
	cd clon/ipcutil; $(JAVAC) *.java; cd ../..;


clon.util: clon/util/*.java
	cd clon/util; $(JAVAC) *.java; cd ../..;


dplite.: dplite/*.java
	cd dplite; $(JAVAC) *.java; cd ..;


jar:
	jar cf clon.jar   `find ./clon/   -print | grep class`
	cd dplite; jar cf dplite.jar `find ./ -print | grep class`; mv dplite.jar ..; cd ..;


clean:
	rm -f clon/ipcutil/*.class
	rm -f dplite/*.class
	rm -f clon/util/*.class


distclean:
	rm -f clon/ipcutil/*.class
	rm -f clon/util/*.class
	rm -f dplite/*.class
	rm -f clon.jar dplite.jar


install: exports
exports:
	cp dplite.jar       $(CLON_JAR)
	cp clon.jar         $(CLON_JAR)
	/apps/java/jdk1.2/bin/javadoc -version -author -public -d $(CLON_HTML)/javadoc $(PACKAGES)


headers:
