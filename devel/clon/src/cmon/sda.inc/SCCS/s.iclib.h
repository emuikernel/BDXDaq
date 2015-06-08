h48744
s 00000/00000/00000
d R 1.2 03/12/22 17:40:12 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/iclib.h
e
s 00021/00000/00000
d D 1.1 03/12/22 17:40:11 boiarino 1 0
c date and time created 03/12/22 17:40:11 by boiarino
e
u
U
f e 0
t
T
I 1

/* iclib.h - header file for iclib.c */

#define length_ic 529
#define ehitmin   0.001
#define egrplmin  0.005
#define egrpmin   0.005
#define ihalf     23
#define MaxRaw    1000
#define MaxX      23
#define MaxY      23
#define ADCg      0.0001
#define TDCch     0.05
#define MaxHits   500
#define MaxGroup  20


/* functions */

int icinit(int runnum);
int tclib(int *jw);
E 1
