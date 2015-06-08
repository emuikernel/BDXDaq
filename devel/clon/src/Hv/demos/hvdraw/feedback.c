#include  "Hv.h"
#include  "draw.h"


/* ------ Feedback ---------- */

void Feedback(Hv_View View,
	      Hv_Point pp)

{
  char             text[40];
  short            l, t, r, b;

  Hv_GetRectLTRB(Hv_GetViewHotRect(View), &l, &t, &r, &b);

/* make the feedback relative to the hotrect corner */

  pp.x -= l;
  pp.y -= t;

  sprintf(text, "[%-d, %-d]", pp.x, pp.y);
  Hv_ChangeFeedback(View, LOCALXY, text);
}



