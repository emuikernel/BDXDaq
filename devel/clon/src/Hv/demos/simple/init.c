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




