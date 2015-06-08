h61314
s 00001/00001/00025
d D 1.9 02/08/09 16:36:16 boiarino 10 9
c minor
c 
e
s 00001/00001/00025
d D 1.8 01/04/17 10:53:06 giovanet 9 8
c  get makefile working
e
s 00002/00002/00024
d D 1.7 00/10/30 11:30:47 wolin 8 7
c Cleaning class files
e
s 00003/00001/00023
d D 1.6 00/10/06 11:57:56 clasrun 7 6
c move the .clas files to BIN_DIR
c 
e
s 00001/00001/00023
d D 1.5 00/10/06 03:40:59 clasrun 6 5
c *** empty log message ***
e
s 00001/00001/00023
d D 1.4 00/10/06 03:39:30 clasrun 5 4
c corrected an error 
e
s 00000/00005/00024
d D 1.3 00/10/06 03:38:06 clasrun 4 3
c *** empty log message ***
e
s 00002/00001/00027
d D 1.2 00/10/06 03:33:58 clasrun 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/gui/Makefile
e
s 00028/00000/00000
d D 1.1 00/10/06 02:11:41 clasrun 1 0
c 
e
u
U
f b 
f e 0
t
T
I 7
BIN_DIR    = $(CLON_BIN)
E 7
I 1

D 10
JAVA2_DIR  =  /apps/java/jdk1.2.1/bin/ 
E 10
I 10
JAVA2_DIR  =  /apps/java/jdk1.2.1/bin/
E 10
JAVA1_DIR  =  /apps/java/jdk1.2.1/bin/

all:  
D 5
	javac ss_recieve
E 5
I 5
D 6
	javac ss_recieve.java
E 6
I 6
	javac ss_receive.java
E 6
E 5
D 9
#	$(JAVA2_DIR)javac LaserMon.java 
E 9
I 9
	$(JAVA2_DIR)javac ECLaserMon.java 
E 9


exports: update
export: update
install: update 


clean:
D 8
	@echo "gui cleaning: nothing to do .class and .java part of dist"
E 8
I 8
	rm -f *.class
E 8


update:
D 3
	@echo "gui update: nothing to do java run on source area files" 
E 3
I 3
D 7
	@echo "gui update: nothing to do java runs on source area files" 
E 7
I 7
	@echo "gui update: copy xxx.class to $CLON_BIN" 
	cp *.class  $(BIN_DIR)/
E 7
E 3
D 4
	
E 4


I 3
D 4

E 4
E 3
distclean:
D 8
	@echo "gui cleaning: nothing to do .class and .java part of dist"
E 8
I 8
	rm -f *.class
E 8
D 4
	


E 4
E 1
