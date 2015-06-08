h44047
s 00000/00000/00000
d R 1.2 02/08/26 22:35:28 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/simple/simple.h
e
s 00018/00000/00000
d D 1.1 02/08/26 22:35:27 boiarino 1 0
c date and time created 02/08/26 22:35:27 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef   __SIMPLEH
#define   __SIMPLEH

/* define feedback tags */

#define LOCALX        0
#define LOCALY        1
#define WORLDX        2
#define WORLDY        3

extern   void   simpleInit(void);

extern   void   SetupSimpleView(Hv_View);

extern   void   simpleFeedback(Hv_View,
			       Hv_Point);

#endif
E 1
