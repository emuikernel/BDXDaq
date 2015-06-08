h20149
s 00000/00000/00000
d R 1.2 02/08/26 22:35:27 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/simple/init.c
e
s 00048/00000/00000
d D 1.1 02/08/26 22:35:26 boiarino 1 0
c date and time created 02/08/26 22:35:26 by boiarino
e
u
U
f e 0
t
T
I 1
#include "Hv.h"
#include "Hv_xyplot.h"
#include "simple.h"

#define  SIMPLE_VIEW     1

/*----- simpleInit -------*/

void simpleInit(void)

{
  Hv_View     view;
  int         i;
  short       left, top;
  char        text[30];

/* create two views, one way off screen to
   demo the virtual desktop */

  for (i = 0; i < 2; i++) {
    sprintf(text, "Simple %d", i+1);

    left = 75 + 3000*i;
    top  = 55  + 2000*i;

    Hv_VaCreateView(&view,
		    Hv_TAG,             SIMPLE_VIEW,
		    Hv_LEFT,            left,
		    Hv_TOP,             top,
		    Hv_HOTRECTHMARGIN,  53,
		    Hv_HOTRECTWIDTH,    400,
		    Hv_HOTRECTHEIGHT,   724,
		    Hv_XMAX,            0.4,
		    Hv_YMAX,            0.66,
		    Hv_INITIALIZE,      SetupSimpleView,
		    Hv_TITLE,           text,
		    Hv_POPUPCONTROL,    Hv_STANDARDPOPUPCONTROL - Hv_QUICKZOOM,
		    Hv_DRAWCONTROL,     Hv_STANDARDDRAWCONTROL - Hv_ZOOMONRESIZE,
		    Hv_FEEDBACK,        simpleFeedback,
		    Hv_OPENATCREATION,  True,
		    NULL);
  }

}




E 1
