h39952
s 00000/00000/00000
d R 1.2 02/09/09 11:30:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_opengl.h
e
s 00016/00000/00000
d D 1.1 02/09/09 11:30:41 boiarino 1 0
c date and time created 02/09/09 11:30:41 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _HV_OPENGL_H_
#define _HV_OPENGL_H_

#ifndef EVARS
#define EVARS extern 
#endif

void Setup_GLWidget(Hv_View View);

#ifndef WIN32
EVARS XVisualInfo  *Hv_3Dvi;
EVARS Widget          Hv_mainw;              /* main window */
#endif
#undef EVARS

#endif
E 1
