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



