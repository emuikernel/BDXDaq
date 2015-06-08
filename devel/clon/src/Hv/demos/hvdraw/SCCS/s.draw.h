h37480
s 00000/00000/00000
d R 1.2 02/08/26 22:05:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvdraw/draw.h
e
s 00019/00000/00000
d D 1.1 02/08/26 22:05:06 boiarino 1 0
c date and time created 02/08/26 22:05:06 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef   __DRAWH__
#define   __DRAWH__

/* define feedback tags */

#define LOCALXY        0

extern   void   Init(void);
extern   void   SetupView(Hv_View);
extern   void   Feedback(Hv_View,
			 Hv_Point);

#endif






E 1
