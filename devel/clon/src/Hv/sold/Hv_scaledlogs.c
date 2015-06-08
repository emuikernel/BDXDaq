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

