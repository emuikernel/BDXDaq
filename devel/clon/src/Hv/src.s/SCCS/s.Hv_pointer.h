h47206
s 00000/00000/00000
d R 1.2 02/09/09 11:30:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_pointer.h
e
s 00020/00000/00000
d D 1.1 02/09/09 11:30:44 boiarino 1 0
c date and time created 02/09/09 11:30:44 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef __HVPOINTERH__
#define __HVPOINTERH__

void Hv_LaunchMagnify(Hv_Point *);

extern void            Hv_ButtonPress(Hv_Widget,
				      Hv_Pointer,
				      Hv_XEvent *);

extern void            Hv_PointerMotion(Hv_Widget,
					Hv_Pointer,
					Hv_XEvent *);

extern void            Hv_ButtonRelease(Hv_Widget,
					Hv_Pointer,
					Hv_XEvent   *);


#endif

E 1
