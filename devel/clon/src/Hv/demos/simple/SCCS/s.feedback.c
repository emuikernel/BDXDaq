h63147
s 00000/00000/00000
d R 1.2 02/08/26 22:35:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/simple/feedback.c
e
s 00033/00000/00000
d D 1.1 02/08/26 22:35:25 boiarino 1 0
c date and time created 02/08/26 22:35:25 by boiarino
e
u
U
f e 0
t
T
I 1
#include  "Hv.h"
#include  "simple.h"


/* ------ simpleFeedback ---------- */

void simpleFeedback(Hv_View View,
		    Hv_Point pp)

{
  float            worldx, worldy;
  char             text[20];

  Hv_LocalToWorld(View, &worldx, &worldy, pp.x, pp.y);

  sprintf(text, "%-d", pp.x);
  Hv_ChangeFeedback(View, LOCALX, text);

  sprintf(text, "%-d", pp.y);
  Hv_ChangeFeedback(View, LOCALY, text);

  sprintf(text, "%-f", worldx);
  Hv_ChangeFeedback(View, WORLDX, text);

  sprintf(text, "%-f", worldy);
  Hv_ChangeFeedback(View, WORLDY, text);
}






E 1
