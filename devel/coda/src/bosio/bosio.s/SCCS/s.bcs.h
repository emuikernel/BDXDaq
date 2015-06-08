h38620
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bcs.h
e
s 00017/00000/00000
d D 1.1 00/08/10 11:09:55 boiarino 1 0
c date and time created 00/08/10 11:09:55 by boiarino
e
u
U
f e 0
t
T
I 1
 
/* bcs.h */

/* #define NBCS 700000 - have to be outside */
 
/* define a BOScommon data type */
 
typedef struct boscommon {
      int junk[5];
      int iw[NBCS];
	} BOScommon;
 
/* declare a global variable */
 
BOScommon	bcs_;
 
 
E 1
