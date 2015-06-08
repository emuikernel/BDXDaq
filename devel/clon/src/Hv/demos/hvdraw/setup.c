#include "Hv.h"
#include "draw.h"

/* --------- SetupView -------- */

void SetupView(Hv_View View)

/* setup the View */

{
  Hv_Item   first, last, Feedback;
  short     l, t, r, b;
  Hv_Rect   *area;

  Hv_GetRectLTRB(Hv_GetViewHotRect(View), &l, &t, &r, &b);

/* create a camera button for printing 
   and the standard drawing tools */

  Hv_AddDrawingTools(View,
		     l,
		     t - 36,
		     Hv_HORIZONTAL,
		     Hv_DONOTHINGONRESIZE,
		     Hv_lightSeaGreen,
		     True,
		     &first,
		     &last);

/* create a feedback item */

  area = Hv_GetItemArea(last);
  Feedback = Hv_VaCreateItem(View,
			     Hv_TYPE,              Hv_FEEDBACKITEM,
			     Hv_LEFT,              area->right+4,
			     Hv_TOP,               area->top+1,
			     Hv_RESIZEPOLICY,      Hv_DONOTHINGONRESIZE,
			     Hv_FEEDBACKDATACOLS,  11,
			     Hv_FEEDBACKENTRY,     LOCALXY,
			                           Hv_helveticaBold14,
			                           Hv_black,
			                         " [X, Y]: ",
			     NULL);


  /*
  
Hv_VaGetItemAttributes(Feedback,
		       Hv_LEFT,     &l,
		       Hv_TOP,     &t,
		       Hv_WIDTH,     &r,
		       Hv_HEIGHT,     &b,
		       NULL);

fprintf(stderr, "[L, T, W, H] = [%d, %d, %d, %d]\n", l, t, r, b);
*/
}






