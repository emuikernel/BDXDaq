/*
----------------------------------------------------
-							
-  File:    feedback.c	
-							
-  Summary:						
-           sets up feedback
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  08/24/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "Hv.h"
#include "Hv_maps.h"
#include "maps.h"

static void     LatLongFeedback();


/* ------ ViewFeedback ---------- */

void ViewFeedback(View, pp)

Hv_View             View;
Hv_Point            pp;        /*mouse location*/

{
  LatLongFeedback(View, pp);
}

/* ------ LatLongFeedback ---------- */

static void LatLongFeedback(View, pp)

Hv_View             View;
Hv_Point            pp;        /*mouse location*/

{
  char             text[40];
  char             longtext[80];
  float            latitude, longitude;
  Boolean          north, east;
  Hv_ViewMapData   mdata;
  Hv_Item          globe;

  mdata = Hv_GetViewMapData(View);
  if (mdata == NULL)
    return;

/* if not mercator, make sure we are in the globe */

  if (mdata->projection != Hv_MERCATOR) {
    globe = mdata->GlobeItem;
    if ((globe == NULL) || (globe->region == NULL) || !Hv_PointInRegion(pp, globe->region)) {
      Hv_ChangeFeedback(View, LATLONG, " ");
      return;
    }
  }


/* get lat long of mouse position */

  Hv_LocalToLatLong(View, pp.x, pp.y, &latitude, &longitude);

/* clean up lat & long for display */

  latitude  *= Hv_RADTODEG;
  longitude *= Hv_RADTODEG;

  if (longitude > 180.0)
    longitude -= 360.0;

  if (longitude < -180.0)
    longitude += 360.0;

/* append a direction, as appropriate */

  east =  longitude > 0.0;
  north = latitude  > 0.0;
  latitude  = fabs(latitude);
  longitude = fabs(longitude);

/* display the lat and long of the mouse point */

  Hv_NumberToString(text, latitude, 2, 1.e-5);
  if (north)
    strcat(text, "N");
  else
    strcat(text, "S");
  sprintf(longtext, "%s  ", text);


  Hv_NumberToString(text, longitude, 2, 1.e-5);
  if (east)
    strcat(text, "E");
  else
    strcat(text, "W");

  strcat(longtext, text);
  Hv_ChangeFeedback(View, LATLONG, longtext);
}
