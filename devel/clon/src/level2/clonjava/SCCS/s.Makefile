h38875
s 00002/00001/00025
d D 1.9 03/06/23 14:19:55 gurjyan 10 9
c switch to java 1.4
c 
e
s 00004/00002/00022
d D 1.8 02/08/15 11:30:58 boiarino 9 8
c .SUFFIXES: .java .class to make 'gmake' happy
c 
c 
e
s 00006/00002/00018
d D 1.7 00/10/30 14:40:33 wolin 8 7
c Only compile when needed
e
s 00002/00002/00018
d D 1.6 00/10/30 11:23:33 wolin 7 6
c Minor
e
s 00001/00000/00019
d D 1.5 00/10/06 10:12:21 wolin 6 5
c Protections
e
s 00001/00000/00018
d D 1.4 00/09/07 10:41:07 wolin 5 4
c Gif files
e
s 00005/00002/00013
d D 1.3 00/09/07 10:36:12 wolin 4 3
c Now working
e
s 00001/00001/00014
d D 1.2 00/08/07 13:48:04 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 euphrates/Makefile
e
s 00015/00000/00000
d D 1.1 00/08/07 13:46:19 gurjyan 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
D 9

E 9
I 9
#
E 9
#  makefile for euphrates
D 4
#  javac needs to be added
E 4
#  vhg, 7-aug-2000
I 9
#
#Sergey: add java suffixes just in case
.SUFFIXES: .java .class
E 9

D 9

E 9
D 8
all:
I 4
	/apps/java/jdk1.2.1/bin/javac euphrates.java
E 8
I 8
.java.class:
D 10
	/apps/java/jdk1.2.1/bin/javac $<
E 10
I 10
#	/apps/java/jdk1.2.1/bin/javac $<
	javac $<
E 10


all: euphrates.class

E 8
E 4
 
clean:
I 4
D 7
	rm *.class
E 7
I 7
	rm -f *.class
E 7
E 4
 
distclean:
I 4
D 7
	rm *.class
E 7
I 7
	rm -f *.class
E 7

E 4
 
install: exports
exports:
D 3
	cp *.class $(CLON_BIN)
E 3
I 3
D 4
	cp *.gif *.class $(CLON_BIN)
E 4
I 4
	cp *.class $(CLON_BIN)
I 5
	cp *.gif $(CLON_GIF)
I 6
	-chmod ug+rw $(CLON_GIF)/*
E 6
E 5
E 4
E 3
E 1
