h07072
s 00001/00003/00025
d D 1.21 04/03/11 14:51:58 boiarino 22 21
c use default 'javac'
e
s 00001/00001/00027
d D 1.20 04/03/11 09:57:14 gurjyan 21 20
c new java
e
s 00002/00002/00026
d D 1.19 04/03/11 09:52:39 gurjyan 20 19
c *** empty log message ***
e
s 00002/00002/00026
d D 1.18 04/03/10 17:07:40 gurjyan 19 18
c *** empty log message ***
e
s 00002/00001/00026
d D 1.17 03/06/23 14:18:42 gurjyan 18 17
c switch to java 1.4
c 
e
s 00001/00001/00026
d D 1.16 01/10/23 12:34:45 gurjyan 17 16
c *** empty log message ***
e
s 00001/00001/00026
d D 1.15 01/10/19 13:25:13 gurjyan 16 15
c test
e
s 00001/00001/00026
d D 1.14 01/10/19 07:46:46 gurjyan 15 14
c added active_man
c 
e
s 00001/00001/00026
d D 1.13 01/02/05 17:09:24 gurjyan 14 13
c java version 1.1.7
c 
e
s 00001/00001/00026
d D 1.12 00/11/27 12:01:14 wolin 13 12
c Using java 1.2.1
e
s 00011/00003/00016
d D 1.11 00/10/30 16:00:51 wolin 12 11
c Only compile if needed
e
s 00001/00001/00018
d D 1.10 00/10/30 11:59:31 wolin 11 10
c Fixed distclean
e
s 00002/00002/00017
d D 1.9 00/10/30 11:22:49 wolin 10 9
c Minor
e
s 00002/00000/00017
d D 1.8 00/10/05 14:25:30 wolin 9 8
c Protections
e
s 00005/00001/00012
d D 1.7 00/09/07 09:39:32 wolin 8 7
c New CC
e
s 00001/00000/00012
d D 1.6 00/09/06 16:09:34 wolin 7 6
c Added chmod
e
s 00001/00001/00011
d D 1.5 00/08/04 17:28:14 gurjyan 6 5
c 
e
s 00001/00001/00011
d D 1.4 00/08/04 17:23:06 gurjyan 5 4
c 
e
s 00001/00000/00011
d D 1.3 00/08/04 17:08:02 gurjyan 4 3
c 
e
s 00008/00000/00003
d D 1.2 00/08/04 17:06:47 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Makefile
e
s 00003/00000/00000
d D 1.1 00/07/27 16:14:26 wolin 1 0
c date and time created 00/07/27 16:14:26 by wolin
e
u
U
f e 0
t
T
I 1
# makefile for daqtlc: DAQ expert system for CLAS
# vhg, 24-jul-00

I 12

.java.class:
D 13
	javac $<
E 13
I 13
D 14
	/apps/java/jdk1.2.1/bin/javac $<
E 14
I 14
D 18
	/apps/java/jdk1.1.7/bin/javac $<
E 18
I 18
D 19
#	/apps/java/jdk1.1.7/bin/javac $<
	javac $<
E 19
I 19
D 20
	/apps/java/jdk1.1.7/bin/javac $<
E 20
I 20
D 22
#	/apps/java/jdk1.1.7/bin/javac $<
E 20
#	javac $<
I 20
D 21
        /u/apps/java/j2sdk1.4.2_03/bin/javac $<
E 21
I 21
	/u/apps/java/j2sdk1.4.2_03/bin/javac $<
E 22
I 22
	javac $<
E 22
E 21
E 20
E 19
E 18
E 14
E 13

D 20

E 20
E 12
I 3
all:
I 7
D 12
	-chmod +x jmake
E 7
	jmake 
E 12
I 12
	/bin/csh -s < jmake 
	jar cf tlc.jar *.class

E 12
I 8

E 8
clean:
D 10
	rm *.class
E 10
I 10
D 12
	rm -f *.class
E 12
I 12
	rm -f *.class	

E 12
E 10
I 8

E 8
distclean:
I 4
D 10
	rm *.class
E 10
I 10
D 11
	rm -f *.class
E 11
I 11
	rm -f *.class *.jar
I 12

E 12
E 11
E 10
I 8

E 8
E 4
install: exports
exports:
D 5
	cp *.jar $CLON_JAR; cp daqtlc $CLON_BIN
E 5
I 5
D 6
	cp tlc.jar $CLON_JAR; cp daqtlc $CLON_BIN
E 6
I 6
D 8
	cp tlc.jar $(CLON_JAR); cp daqtlc $(CLON_BIN)
E 8
I 8
	cp tlc.jar $(CLON_JAR)
D 15
	cp daqtlc $(CLON_BIN)
E 15
I 15
D 16
	cp daqtlc active_man $(CLON_BIN)
E 16
I 16
D 17
	cp daqtlc active_man boy.gif $(CLON_BIN)
E 17
I 17
	cp daqtlc infodaqtlc boy.gif $(CLON_BIN)
E 17
E 16
E 15
I 9
	-chmod ug+rwx $(CLON_BIN)/daqtlc
	-chmod a+x $(CLON_BIN)/daqtlc
E 9
E 8
E 6
E 5
E 3
E 1
