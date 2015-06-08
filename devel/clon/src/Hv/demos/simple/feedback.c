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






