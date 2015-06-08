h50992
s 00000/00000/00000
d R 1.2 02/08/26 22:05:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvdraw/feedback.c
e
s 00026/00000/00000
d D 1.1 02/08/26 22:05:06 boiarino 1 0
c date and time created 02/08/26 22:05:06 by boiarino
e
u
U
f e 0
t
T
I 1
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



E 1
