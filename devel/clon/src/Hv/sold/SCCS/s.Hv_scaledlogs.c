h27648
s 00000/00000/00000
d R 1.2 02/08/25 23:21:22 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_scaledlogs.c
e
s 00049/00000/00000
d D 1.1 02/08/25 23:21:21 boiarino 1 0
c date and time created 02/08/25 23:21:21 by boiarino
e
u
U
f e 0
t
T
I 1
/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"



/*------- Hv_GetScaleValue ---------*/

int  Hv_GetScaleValue(Hv_Widget w)

{
  int   val;

  XmScaleGetValue(w,  &val);
  return val;
}

/*------- Hv_SetScaleValue ---------*/

void  Hv_SetScaleValue(Hv_Widget w,
		       int       val)

{
  XmScaleSetValue(w, val);
}

/*------- Hv_SetScaleMinMax ---------*/

void  Hv_SetScaleMinMax(Hv_Widget w,
			int min,
			int max)

{
  XtVaSetValues(w,
		XmNminimum, min,
		XmNmaximum, max,
		NULL);
}

E 1
