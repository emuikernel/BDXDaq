h63118
s 00000/00000/00000
d R 1.2 02/08/26 22:05:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvdraw/init.c
e
s 00033/00000/00000
d D 1.1 02/08/26 22:05:06 boiarino 1 0
c date and time created 02/08/26 22:05:06 by boiarino
e
u
U
f e 0
t
T
I 1
#include "Hv.h"
#include "draw.h"

#define  SIMPLE_VIEW     1

/*----- Init -------*/

void Init(void)

{
  Hv_View     view;

  Hv_VaCreateView(&view,
		  Hv_TAG,             SIMPLE_VIEW,
		  Hv_HOTRECTTMARGIN,  38,
		  Hv_TOP,             50,
		  Hv_HOTRECTWIDTH,    650,
		  Hv_HOTRECTHEIGHT,   650,
		  Hv_XMAX,            1.0,
		  Hv_YMAX,            1.0,
		  Hv_INITIALIZE,      SetupView,
		  Hv_TITLE,           " Draw ",
		  Hv_POPUPCONTROL,    Hv_STANDARDPOPUPCONTROL - Hv_QUICKZOOM,
		  Hv_DRAWCONTROL,     Hv_STANDARDDRAWCONTROL - Hv_ZOOMONRESIZE,
		  Hv_FEEDBACK,        Feedback,
		  Hv_HOTRECTCOLOR,    Hv_lavender,
		  Hv_OPENATCREATION,  True,
		  NULL);
  
}



E 1
