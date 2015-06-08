h02485
s 00002/00002/00051
d D 1.18 03/09/15 23:09:28 boiarino 19 18
c rename target 'dplite' to 'dplite.' to make 'gmake' happy
e
s 00008/00009/00045
d D 1.17 03/09/15 23:07:16 boiarino 18 17
c old good version
e
s 00009/00008/00045
d D 1.16 03/09/15 22:43:01 boiarino 17 16
c minor
e
s 00001/00001/00052
d D 1.15 03/06/16 15:22:13 wolin 16 15
c jdk 1.4.0
e
s 00001/00001/00052
d D 1.14 02/08/16 09:31:40 wolin 15 14
c Using CLON_HTML
e
s 00001/00001/00052
d D 1.13 02/04/18 16:36:55 wolin 14 13
c Back to 1.1.7 until we upgrade the entire online
e
s 00005/00005/00048
d D 1.12 02/04/18 16:30:32 wolin 13 12
c Switched to java 1.2.1
e
s 00004/00004/00049
d D 1.11 02/04/18 15:53:38 wolin 12 11
c Need to specify compiler version
e
s 00002/00000/00051
d D 1.10 02/04/05 16:16:41 wolin 11 10
c Added headers
e
s 00003/00003/00048
d D 1.9 01/05/14 15:30:14 wolin 10 9
c Using javac directly
e
s 00009/00006/00042
d D 1.8 00/10/30 14:34:50 wolin 9 8
c Only compile if needed
e
s 00013/00003/00035
d D 1.7 00/08/15 14:53:37 wolin 8 7
c Updated for release
e
s 00001/00001/00037
d D 1.6 00/08/15 11:44:30 wolin 7 6
c Added dplite.jar
e
s 00002/00000/00036
d D 1.5 00/07/25 15:00:15 wolin 6 5
c For release
e
s 00002/00002/00034
d D 1.4 00/07/25 12:44:16 wolin 5 4
c Do not need CLON_DEVxxx any more
e
s 00002/00002/00034
d D 1.3 00/01/05 16:42:37 wolin 4 3
c Release to devel area
e
s 00011/00006/00025
d D 1.2 99/11/23 11:23:00 wolin 3 1
c Added dplite
e
s 00000/00000/00000
d R 1.2 99/09/24 15:08:53 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 jar/Makefile
e
s 00031/00000/00000
d D 1.1 99/09/24 15:08:52 wolin 1 0
c date and time created 99/09/24 15:08:52 by wolin
e
u
U
f e 0
t
T
I 1
#  makes online jar files

I 8
#  ejw, 15-aug-2000


E 8
PACKAGES = clon.ipcutil clon.util
D 13

E 13
I 13
D 14
JAVAC = /apps/java/jdk1.2.1/bin/javac
E 14
I 14
D 16
JAVAC = /apps/java/jdk1.1.7/bin/javac
E 16
I 16
JAVAC = /apps/java/jdk1.4.0/bin/javac
E 16
E 14
E 13

D 3
all: $(PACKAGES) jar
E 3
I 3
D 9
all: $(PACKAGES) dplite jar
E 9
I 9
.java.class:
D 12
	javac $<
E 12
I 12
D 13
	/apps/java/jdk1.1.7/bin/javac $<
E 13
I 13
	$(JAVAC) $<
E 13
E 12
E 9
E 3

I 8

E 8
D 3
clon.ipcutil:
	cd clon/ipcutil; /apps/java/jdk1.1.6/bin/javac ipc_init.java; cd ../../
E 3
I 3
D 9
dplite: dplite/*.java
D 8
	cd dplite; /apps/java/jdk1.1.6/bin/javac *.java; cd ..;
E 8
I 8
	cd dplite; javac *.java; cd ..;
E 9
I 9
D 17
all: $(PACKAGES) dplite jar
E 17
I 17
D 18
all: $(PACKAGES) dplite. jar
E 18
I 18
D 19
all: $(PACKAGES) dplite jar
E 19
I 19
all: $(PACKAGES) dplite. jar
E 19
E 18
E 17
E 9

E 8
E 3

D 3
clon.util:
	cd clon/util; /apps/java/jdk1.1.6/bin/javac Format.java; cd ../../
E 3
I 3
clon.ipcutil: clon/ipcutil/*.java
D 8
	cd clon/ipcutil; /apps/java/jdk1.1.6/bin/javac ipc_init.java; cd ../..;
E 8
I 8
D 9
	cd clon/ipcutil; javac ipc_init.java; cd ../..;
E 9
I 9
D 10
	cd clon/ipcutil; make *.java; cd ../..;
E 10
I 10
D 12
	cd clon/ipcutil; javac *.java; cd ../..;
E 12
I 12
D 13
	cd clon/ipcutil; /apps/java/jdk1.1.7/bin/javac *.java; cd ../..;
E 13
I 13
	cd clon/ipcutil; $(JAVAC) *.java; cd ../..;
E 13
E 12
E 10
E 9

E 8

clon.util: clon/util/*.java
D 8
	cd clon/util; /apps/java/jdk1.1.6/bin/javac Format.java; cd ../..;
E 8
I 8
D 9
	cd clon/util; javac Format.java; cd ../..;
E 9
I 9
D 10
	cd clon/util; make *.java; cd ../..;
E 10
I 10
D 12
	cd clon/util; javac *.java; cd ../..;
E 12
I 12
D 13
	cd clon/util; /apps/java/jdk1.1.7/bin/javac *.java; cd ../..;
E 13
I 13
	cd clon/util; $(JAVAC) *.java; cd ../..;
E 13
E 12
E 10


D 17
dplite: dplite/*.java
E 17
I 17
D 18
dplite.: dplite/*.java
E 18
I 18
D 19
dplite: dplite/*.java
E 19
I 19
dplite.: dplite/*.java
E 19
E 18
E 17
D 10
	cd dplite; make *.java; cd ..;
E 10
I 10
D 12
	cd dplite; javac *.java; cd ..;
E 12
I 12
D 13
	cd dplite; /apps/java/jdk1.1.7/bin/javac *.java; cd ..;
E 13
I 13
	cd dplite; $(JAVAC) *.java; cd ..;
E 13
E 12
E 10
E 9

E 8
E 3

jar:
D 3
	jar cf clon.jar `find ./ -print | grep class`
E 3
I 3
D 17
	jar cf clon.jar   `find ./clon/   -print | grep class`
	cd dplite; jar cf dplite.jar `find ./ -print | grep class`; mv dplite.jar ..; cd ..;
E 17
I 17
D 18
	jar cf clon.jar   `find ./clon/   -print | grep class`; cd ..;
	jar cf dplite.jar `find ./dplite/ -print | grep class`; cd ..;
E 18
I 18
	jar cf clon.jar   `find ./clon/   -print | grep class`
	cd dplite; jar cf dplite.jar `find ./ -print | grep class`; mv dplite.jar ..; cd ..;
E 18
E 17
E 3


clean:
	rm -f clon/ipcutil/*.class
I 8
	rm -f dplite/*.class
E 8
	rm -f clon/util/*.class

I 8

E 8
distclean:
D 17
	rm -f clon/ipcutil/*.class
	rm -f clon/util/*.class
I 8
	rm -f dplite/*.class
E 17
I 17
D 18
	rm -f clon/ipcutil/*.class clon/ipcutil/*.jar
	rm -f clon/util/*.class clon/util/*.jar
	rm -f dplite/*.class dplite/*.jar
E 18
I 18
	rm -f clon/ipcutil/*.class
	rm -f clon/util/*.class
	rm -f dplite/*.class
E 18
E 17
E 8
D 7
	rm -f clon.jar
E 7
I 7
	rm -f clon.jar dplite.jar
E 7
D 9
####	rm -fr /clasweb/javadoc/*
E 9

I 6

install: exports
E 6
exports:
I 3
D 4
	cp dplite.jar       $(CLON_JAR)
E 3
	cp clon.jar         $(CLON_JAR)
E 4
I 4
D 5
	cp dplite.jar       $(CLON_DEVJAR)
	cp clon.jar         $(CLON_DEVJAR)
E 5
I 5
	cp dplite.jar       $(CLON_JAR)
	cp clon.jar         $(CLON_JAR)
E 5
E 4
D 15
	/apps/java/jdk1.2/bin/javadoc -version -author -public -d /clasweb/javadoc $(PACKAGES)
E 15
I 15
D 17
	/apps/java/jdk1.2/bin/javadoc -version -author -public -d $(CLON_HTML)/javadoc $(PACKAGES)
E 17
I 17
D 18
	/apps/java/jdk1.2/bin/javadoc -version -author -public \
		-d $(CLON_HTML)/javadoc $(PACKAGES)
E 18
I 18
	/apps/java/jdk1.2/bin/javadoc -version -author -public -d $(CLON_HTML)/javadoc $(PACKAGES)
E 18
E 17
E 15

I 11

headers:
E 11
E 1
