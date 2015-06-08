 /**
 * <EM>Various mathematical routines dealing with map projections.</EM>
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


#include "Hv.h"
#include "Hv_maps.h"

#define MAPTINY  1.0e-6
#define EPSILON  1.0e-9
#define ALMOST1  0.9999999
#define ALMOST180 179.99

 Boolean   Hv_mapPointIsGood;
 float     Hv_RADEARTH = (float)6371.0087714;


 static void SaveHotRectItems(Hv_ItemList   items);

 static void RestoreHotRectItems(Hv_ItemList   items);

 static Boolean ExcludeFromRestore(Hv_Item Item);

/**
 * Hv_GetAzimuthAndGCD
 * @param lat1 Latitude of first point. (decimal radians).
 * @param long1 Longitude of first point. (decimal radians).
 * @param lat2 Latitude of second point. (decimal radians).
 * @param long2 Longitude of second point. (decimal radians).
 * @param azimuth Azimuth (0-2Pi radians, 0: North, 90: East).
 * @param gcd  Returns Great Circle Distance (km).
 */
 
 void Hv_GetAzimuthAndGCD(float lat1,
			   float long1,
			   float lat2,
			   float long2,
			   float *azimuth,
			   float *gcd) {

 /* returns azimuth, between 0 and 2pi */

   double  cr, r, az, del, del2;

   del = long2-long1;

   if (fabs(del) < EPSILON) {
	   del2 = lat2 - lat1;
	   if (fabs(lat2) < EPSILON) {
		   *azimuth = 0.0;
		   *gcd = 0.0;
		   return;
	   }
   }

   *gcd = Hv_GCDistance(lat1, long1, lat2, long2);
 
   if (*gcd < EPSILON) {
		   *azimuth = 0.0;
		   return;
   }

   r = *gcd/Hv_RADEARTH;
   cr = cos(lat1)*cos(lat2)*cos(del) + sin(lat2)*sin(lat1);
   az = (sin(lat2) - sin(lat1)*cr)/(cos(lat1)*sin(r));

   if (fabs(az) < 1.0)
     az = acos(az);
   else {
     if (az > 0.0)
       az = 0.0;
     else
       az = Hv_PI;
   }

   if ((del > Hv_PI) || ((del < 0.0) && (del > -Hv_PI)))
     az = Hv_TWOPI - az;

   *azimuth = (float)az;
 }


 /**
 * Hv_GetAzimuth
 * @param lat1 Latitude of first point. (decimal radians).
 * @param long1 Longitude of first point. (decimal radians).
 * @param lat2 Latitude of second point. (decimal radians).
 * @param long2 Longitude of second point. (decimal radians).
 * @return azimuth (0-2Pi radians, 0: North, 90: East).
 */

 float Hv_GetAzimuth(float lat1,
		     float long1,
		     float lat2,
		     float long2) {

 /* returns azimuth, between 0 and 2pi */

	 float  az, r;
	 Hv_GetAzimuthAndGCD(lat1, long1, lat2, long2, &az, &r);
	 return az;
 }



 /**
  * Hv_OffsetLatLong
  * @purpose Given a ltitude and and a longitude, and a range an azimuth,
  * calculate the lat-long of the end point.
  * @param Start-point latitude (decimal radians).
  * @param Start-point latitude (decimal radians).
  * @param range   Range (km).
  * @param azimuth Azimuth (0-2Pi radians, 0: North, 90: East).
  * @param Returns endpoint latitude (decimal radians).
  * @param Returns endpoint longitude (decimal radians).
  */

 void Hv_OffsetLatLong(float latitude,
		     float longitude,  /*original location in radians */
		     float range,
		     float azimuth,        /*azimuthal heading (rads)*/
		     float *newlat,
		     float *newlong) {

   double  c;
   double  cosphi1, sinphi1;
   double  cosc, sinc;
   double  cosaz, sinaz;
   double  arg;

   cosphi1 = cos((double)latitude);
   sinphi1 = sin((double)latitude);

   cosaz = cos((double)azimuth);
   sinaz = sin((double)azimuth);

   c = (double)(range)/Hv_RADEARTH;
   cosc = cos(c);
   sinc = sin(c);

   arg = (sinphi1*cosc) + (cosphi1*sinc*cosaz);

   if (arg > 1.0)
     arg = 1.0;
   else if (arg < -1.0)
     arg = -1.0;

   *newlat  = (float)(asin(arg));
 /*  *newlong = (float)(longitude + atan((sinc*sinaz)/((cosphi1*cosc) - (sinphi1*sinc*cosaz)))); */
   *newlong = (float)(longitude + atan2(sinc*sinaz, (cosphi1*cosc) - (sinphi1*sinc*cosaz)));

   if(*newlong > Hv_PI)
     *newlong -= (float)Hv_TWOPI;
   else if(*newlong < -Hv_PI)
     *newlong += (float)Hv_TWOPI;
 }


 /**
  * Hv_GetGoodMapLimits
  * @purpose  Mostly for Quickzooms. Converts a desired range
  * specified by lat-long limits to a corresponding World coordinate
  * range.
  * @param View   The View in question.
  * @param  latmin Minimum latitude (decimal DEGREES).
  * @param  latmax Maximum latitude (decimal DEGREES).
  * @param  longmin Minimum longitude (decimal DEGREES).
  * @param  longmax Maximum longitude (decimal DEGREES).
  * @param  Returns minimum X of the World limits.
  * @param   Returns maximum X of the World limits.
  * @param   Returns minimum Y of the World limits.
  * @param   Returns maximum of the World limits.
  * @param qz   If True, View is "Quick-Zoomed".
  */

 void Hv_GetGoodMapLimits(Hv_View View,
			  float  latmin,
			  float  latmax,
			  float  longmin,
			  float  longmax,
			  float  *xmin,
			  float  *xmax,
			  float  *ymin,
			  float  *ymax,
			  Boolean qz) {

 
   Hv_ViewMapData  mdata;
   double           lamdeg;
   double           oldlam0;
   double           dellong, dellat;
   double           longcent;
   double           ar;
   double           dx;


   if (longmax > ALMOST180) {
       dx = (longmax - ALMOST180);
       longmax = (float)ALMOST180;
       longmin = Hv_fMax((float)(-ALMOST180), (float)(longmin-dx));
   }

   if (longmin < -ALMOST180) {
       dx = (-ALMOST180 - longmin);
       longmin = (float)(-ALMOST180);
       longmax = Hv_fMin((float)(ALMOST180), (float)(longmax+dx));
   }


   mdata = Hv_GetViewMapData(View);

 /* setup for undo zoom */

   if (View->lastworld == NULL)
     View->lastworld = (Hv_FRect *)Hv_Malloc(sizeof(Hv_FRect));

   if ((View->mapdata != NULL) && (mdata != NULL) && (mdata->projection == Hv_MERCATOR)) {
     Hv_LocalToLatLong(View, View->hotrect->left, View->hotrect->bottom,
		       &(View->lastworld->xmin), &(View->lastworld->ymin));
     Hv_LocalToLatLong(View, View->hotrect->right, View->hotrect->top,
		       &(View->lastworld->xmax), &(View->lastworld->ymax));

     View->lastworld->xmin *= Hv_RADTODEG;
     View->lastworld->xmax *= Hv_RADTODEG;
     View->lastworld->ymin *= Hv_RADTODEG;
     View->lastworld->ymax *= Hv_RADTODEG;
   }
   else
     Hv_LocalRectToWorldRect(View, View->hotrect, View->lastworld);

 /* begin arduous process of zooming */

   dellong = (longmax - longmin);
   longcent = (longmax + longmin)/2.0;

   dellat = latmax - latmin;
   ar = dellat/dellong;

   latmax = Hv_fMin((float)89.9, latmax);
   latmin = Hv_fMax((float)(-89.9), latmin);

   dellat = latmax - latmin;
   dellong = dellat/ar;
   dellong /= 2.0;
   longmin = (float)(longcent - dellong);
   longmax = (float)(longcent + dellong);

   latmin *= Hv_DEGTORAD;
   latmax *= Hv_DEGTORAD;
   longmin *= Hv_DEGTORAD;
   longmax *= Hv_DEGTORAD;

   Hv_DestroyLocalMaps(View);

 /* for othographic projection, simply rotate to
    center the requested points */

   if (mdata->projection != Hv_MERCATOR) {
     Hv_ChangeMapCenter(View,
			(float)(0.5*(latmin + latmax)),
			(float)(0.5*(longmin + longmax)));
 /*    Hv_DestroyLocalMaps(View); */
     Hv_SetViewDirty(View);
     RestoreHotRectItems(View->items);
     Hv_DrawViewHotRect(View);
     return;
   }

   SaveHotRectItems(View->items);

   Hv_LatLongToXY(View, xmin, ymin, latmin, longmin);
   Hv_LatLongToXY(View, xmax, ymax, latmax, longmax);


   if (*xmax < *xmin)  { 

 /* if the requested zoom crosses the edge
    we must adjust the central longitude */

     oldlam0 = mdata->lambda0;


     lamdeg = Hv_RADTODEG*(oldlam0) + 1.0;

     while ((*xmax < *xmin) && (lamdeg < 178.01)) {
       mdata->lambda0 = (float)(lamdeg*Hv_DEGTORAD);
       Hv_LatLongToXY(View, xmin, ymin, latmin, longmin);
       Hv_LatLongToXY(View, xmax, ymax, latmax, longmax);
       lamdeg += 1.0;
     }

 /* still not ok? */

     if (*xmax < *xmin) {

       lamdeg = Hv_RADTODEG*(oldlam0) - 1.0;
       while ((*xmax < *xmin) && (lamdeg > -ALMOST180)) { 
	   mdata->lambda0 = (float)(lamdeg*Hv_DEGTORAD); 
	   Hv_LatLongToXY(View, xmin, ymin, latmin, longmin);
	   Hv_LatLongToXY(View, xmax, ymax, latmax, longmax);
	   lamdeg -= 1.0;
       }
     }
   }

   RestoreHotRectItems(View->items);

/* Quick zoom the view? */

   if (qz)
     Hv_QuickZoomView(View, *xmin, *xmax, *ymin, *ymax);
 }


 /*------- Hv_LatLongToLocal -------*/

 void Hv_LatLongToLocal(Hv_View  View,
			short    *x,
			short    *y,
			float    latitude,
			float    longitude)

 /*----------------------------------------------------------
    converts lat and long in radians to screen coordinates.

    note: this is a generic routine that doesn't need to
    determine projection.
 ------------------------------------------------------------*/

 {
   float           fx, fy;

   Hv_LatLongToXY(View, &fx, &fy, latitude, longitude);
   Hv_WorldToLocal(View, fx, fy, x, y);
 }

 /*------- Hv_LocalToLatLong -------*/

 void Hv_LocalToLatLong(Hv_View  View,
			short    x,
			short    y,
			float    *latitude,
			float    *longitude)

 /*-------------------------------------------------------------
    converts sceen coordinates x y to lat and long in radians.

    note: this is a generic routine that doesn't need to
    determine projection.
 --------------------------------------------------------------*/

 {
   float           fx, fy;

   Hv_LocalToWorld(View, &fx, &fy, x, y);
   Hv_XYToLatLong(View, fx, fy, latitude, longitude);
 }

 /*------- Hv_WorldToLatLong -------*/

 void Hv_WorldToLatLong(Hv_View View,
			float  x,
			float  y,
			float  *latitude,
			float  *longitude) {

 /*
  * Same as Hv_XYToLatLong, just added for
  * name consistency
  */

     Hv_XYToLatLong(View, x, y, latitude, longitude);

 }


 /*------- Hv_MakeGCLine -----*/

 void Hv_MakeGCLine(float lat1,
		    float long1,
		    float lat2,
		    float long2,
		    int   np,
		    float *latitude,
		    float *longitude) {

 /*
  * Connects two lat long points (radians) with
  * np intermediate points along the same great
  * circle azimuth. Puts the results in the latitude
  * and longitude arrays (allocated by the caller).
  */


     float  az, gcd, delr, r;
     int   k;

     if ((np < 1) || (latitude == NULL) || (longitude == NULL)) {
	 Hv_Println("Bad arguments in Hv_MakeGCLine.");
	 return;
     }


 /* get the azimuth from start point to end point */

     az = Hv_GetAzimuth(lat1,
			long1,
			lat2,
			long2);

 /* get the great circle distance from start to end */

     gcd = Hv_GCDistance(lat1, long1,
			 lat2, long2);

     delr = gcd/(np+1);

 /* fill in the intermediate points */

     for (k = 0; k < np; k++) {
	 r = (k+1)*delr;
	 Hv_OffsetLatLong(lat1, long1, 
			  r, az,
			  &(latitude[k]), &(longitude[k]));
     }

 }


/**
 * Hv_XYToLatLong
 * @purpose   Convert World coordinates to Lat-Long.
 * @param  View    The View being used.
 * @param  x           World x coordinate.
 * @param  y           World y coordinate.
 * @param  latitude    Returns the corresponding latitude (decimal radians)
 * @param  longitude   Returns the corresponding longitude (decimal radians)
 */

 void Hv_XYToLatLong(Hv_View View,
		     float  x,
		     float  y,
		     float  *latitude,
		     float  *longitude) {


   Hv_ViewMapData  mdata = Hv_GetViewMapData(View);
   double           rho, sinc, cosc;


   switch (mdata->projection) {
   case Hv_MERCATOR:
     *longitude = x + mdata->lambda0;
     *latitude = (float)(Hv_PIOVER2 - 2*atan(exp(-y)));
     break;

   case Hv_ORTHOGRAPHIC:
     rho = sqrt(x*x + y*y);
     if (rho < MAPTINY) {
       *latitude = mdata->phi1;
       *longitude = mdata->lambda0;
     }
     else {

       sinc = rho/mdata->radius;

       if (sinc > ALMOST1) {
	      sinc = ALMOST1;
	      cosc = 0.0;
       }
       else
	 cosc = (float)sqrt(1.0 - sinc*sinc);

 /*      *latitude = asin(cosc*mdata->sinphi1 + (y*sinc*mdata->cosphi1/rho));*/
       *latitude = (float)(asin(cosc*mdata->sinphi1 + (y*mdata->cosphi1/mdata->radius)));

       if (fabs(mdata->phi1 - Hv_PIOVER2) < MAPTINY)
	 *longitude = (float)(mdata->lambda0 + atan2(x, -y));
       else if (fabs(mdata->phi1 + Hv_PIOVER2) < MAPTINY)
	 *longitude = (float)(mdata->lambda0 + atan2(x, y));
       else
	 *longitude = (float)(mdata->lambda0 + atan2(x*sinc, rho*mdata->cosphi1*cosc - y*mdata->sinphi1*sinc));

     }
     break;
   }
 }


/**
 * Hv_LatLongToWorld
 * @purpose   Convert Lat-Long coordinates to World.
 * Same as Hv_LatLongToXY, just added for name consistency.
 * @param  View    The View being used.
 * @param  x           Returns World x coordinate.
 * @param  y           Returns World y coordinate.
 * @param  latitude    The corresponding latitude (decimal radians)
 * @param  longitude   The corresponding longitude (decimal radians)
 */

 void Hv_LatLongToWorld(Hv_View View,
			float  *x,
			float  *y,
			float  latitude,
			float  longitude) {

     Hv_LatLongToXY(View, x, y, latitude, longitude);
 }

/**
 * Hv_LatLongToXY
 * @purpose   Convert Lat-Long coordinates to World.
 * @param  View    The View being used.
 * @param  x           Returns World x coordinate.
 * @param  y           Returns World y coordinate.
 * @param  latitude    The corresponding latitude (decimal radians)
 * @param  longitude   The corresponding longitude (decimal radians)
 */

 void Hv_LatLongToXY(Hv_View View,
		     float  *x,
		     float  *y,
		     float  latitude,
			 float  longitude) {


   Hv_ViewMapData    mdata;
   double  lml0, cosphi, sinphi, cosc, coslml0, cc;

   if (View == NULL) {
     *x = Hv_MercatorLongToX(0.0, longitude);
     *y = Hv_MercatorLatToY(latitude);
     return;
   }

   mdata = Hv_GetViewMapData(View);

   switch (mdata->projection) {

   case Hv_MERCATOR:
     *x = Hv_MercatorLongToX(mdata->lambda0, longitude);
     *y = Hv_MercatorLatToY(latitude);
     break;

   case Hv_ORTHOGRAPHIC:
     lml0 = longitude - mdata->lambda0;
     cosphi = cos(latitude);
     sinphi = sin(latitude);
     coslml0 = cos(lml0);
     cc = cosphi*coslml0;

     *x = (float)(mdata->radius*cosphi*sin(lml0));
     *y = (float)(mdata->rcosphi1*sinphi - mdata->rsinphi1*cc);

 /* see if it is a good point */

     cosc = mdata->sinphi1*sinphi + mdata->cosphi1*cc;

     Hv_mapPointIsGood = (cosc > 0.0001);
     break;
   }

 }



 /**
  * Hv_LatLongIsVisible
  * @param View The View in question.
  * @param latitude Latitude to check (decimal radians).
  * @param longitude Longitude to check (decimal radians).
  */

 Boolean Hv_LatLongIsVisible(Hv_View View,
			     float  latitude,
			     float  longitude) {
   Hv_ViewMapData    mdata;
   double  lml0, cosphi, sinphi, cosc, coslml0, cc;

   if (View == NULL)
     return False;

   mdata = Hv_GetViewMapData(View);

   switch (mdata->projection) {

   case Hv_MERCATOR:
       return True;
       break;

   case Hv_ORTHOGRAPHIC:
       lml0 = longitude - mdata->lambda0;
       cosphi = cos(latitude);
       sinphi = sin(latitude);
       coslml0 = cos(lml0);
       cc = cosphi*coslml0;

 /* see if it is a good point */

       cosc = mdata->sinphi1*sinphi + mdata->cosphi1*cc;

       return (cosc > 0.0001);
       break;
   }

   return False;
 }


 /*------ Hv_ChangeMapRadius -----*/

 void Hv_ChangeMapRadius(Hv_View  View,
			 float    radius)


 {
   Hv_ViewMapData    mdata;

   if (View == NULL)
     return;

   mdata = Hv_GetViewMapData(View);
   if (mdata == NULL)
     return;

   mdata->radius = radius;

   Hv_FixMapParameters(View);
 }

 /*------ Hv_ChangeMapCenter -----*/

 void Hv_ChangeMapCenter(Hv_View  View,
			 float    phi1,
			 float    lambda0)


 {
   Hv_ViewMapData    mdata;

   if (View == NULL)
     return;

   mdata = Hv_GetViewMapData(View);
   if (mdata == NULL)
     return;

   mdata->phi1 = phi1;        /* latitude */
   mdata->lambda0 = lambda0;  /* longitude */

   Hv_FixMapParameters(View);
 }


 /*------ Hv_FixMapParameters -----*/

 void Hv_FixMapParameters(Hv_View View)

 {
   Hv_ViewMapData    mdata;

   if (View == NULL)
     return;

   mdata = Hv_GetViewMapData(View);
   if (mdata == NULL)
     return;

   mdata->coslam0 = (float)cos(mdata->lambda0);
   mdata->sinlam0 = (float)sin(mdata->lambda0);
   mdata->cosphi1 = (float)cos(mdata->phi1);
   mdata->sinphi1 = (float)sin(mdata->phi1);
   mdata->rsinphi1 = mdata->radius * mdata->sinphi1;
   mdata->rcosphi1 = mdata->radius * mdata->cosphi1;
 }


 /*------ Hv_MercatorYToLat -------*/

 float Hv_MercatorYToLat(float y)

 {
 /*  return Hv_PIOVER2 - 2*atan(exp(-y));*/
   return (float)atan(sinh(y));
 }

 /*------- Hv_MercatorLatToY ------*/

 float Hv_MercatorLatToY(float lat)

 /* relevant for MERCATOR projection */

 {
   return (float)log(tan(0.5*(Hv_PIOVER2 + lat)));
 }

 /*-------- Hv_MercatorXToLong --------*/

 float Hv_MercatorXToLong(float  lambda0,
			  float  x)

 /* relevant for MERCATOR projection */

 {
   return (x + lambda0);
 }

 /*-------- Hv_MercatorLongToX --------*/

 float Hv_MercatorLongToX(float  lambda0,
			  float  longitude)

 /* relevant for MERCATOR projection */

 {
   float  x;

   x = longitude - lambda0;

   if (x > Hv_PI)
     x -= (float)Hv_TWOPI;

   if (x < -Hv_PI)
     x += (float)Hv_TWOPI;

   return x;
 }


 /*-------- Hv_GCDistance -------*/

 float Hv_GCDistance(float lat1,
		     float long1,
		     float lat2,
		     float long2)

 {
   double cosc, sinc2, c;
   double s1, s2;

   cosc = (sin(lat1)*sin(lat2)) + (cos(lat1)*cos(lat2)*cos(long1-long2));

   if (fabs(cosc) < 1.0)
     c = acos(cosc);
   else
     c = 0.0;

   if (fabs(cosc) < 1.0e-4) {

     s1 = sin((lat1 - lat2)/2.0);
     s2 = sin((long1 - long2)/2.0);

     sinc2 = sqrt((s1*s1) + (cos(lat1)*cos(lat2)*s2*s2));
     c = 2.0*asin(sinc2);
   }

   return  (float)(Hv_RADEARTH*c);
 }


 /*-------- SaveHotRectItems ------------*/

 static void SaveHotRectItems(Hv_ItemList items)

 /***************************************************
   Puts the lat long into the world area, replacing
   the x's and y's. This is because a change of lambda0
   in a Mercator projection renders the old xs and
   ys inaccurate. This must be restored with a call
   to RestoreHotRectItems. MAPS SHOULD BE EXCLUDED.

   (Actually, anything that is lat-long based can be excluded)
 ********************************************************/

 {
   Hv_Item         temp;
   Hv_View         View = NULL;
   Hv_FRect        *warea;
   short           i;
   Hv_WorldPolygonData  *wpoly;
   Hv_FPoint      *fpts;
   Hv_ViewMapData  mdata;

   if (items == NULL)
     return;

   if (items->first == NULL)
     return;

   View = items->first->view;
   mdata = Hv_GetViewMapData(View);

   for (temp = items->first; temp != NULL; temp = temp->next)
     if (temp->domain == Hv_INSIDEHOTRECT) {
	 if (!ExcludeFromRestore(temp)) {
	     if (Hv_CheckBit(temp->drawcontrol, Hv_ZOOMABLE)) {
		 warea = temp->worldarea;

		 warea->xmin = Hv_MercatorXToLong(mdata->lambda0, warea->xmin);
		 warea->xmax = Hv_MercatorXToLong(mdata->lambda0, warea->xmax);

		 if (Hv_CheckItemDrawControlBit(temp, Hv_WPOLYBASED) &&
		     !(temp->tag == Hv_DRAWINGTOOLTAG)) {

		     wpoly = (Hv_WorldPolygonData *)(temp->data);
		     if (wpoly != NULL) {
			 fpts = wpoly->fpts;
			 for (i = 0; i < wpoly->numpnts; i++)
			     fpts[i].h = Hv_MercatorXToLong(mdata->lambda0, fpts[i].h);
		     }
		 }
	     } /* zoomable */	  
	 }

	 if (temp->children != NULL)
	     SaveHotRectItems(temp->children);

     }  /* end inside hr */
 }


 /*------- ExcludeFromRestore --------*/

 static Boolean ExcludeFromRestore(Hv_Item Item) {


     if (Item->type == Hv_MAPITEM)
	 return True;
     
     if (Item->type == Hv_TEXTITEM)
	 return True;
     
     return False;

}


/*----------- RestoreHotRectItems --------*/

static void RestoreHotRectItems(Hv_ItemList items)

/******************************************
   Undoes the SaveHotRectItems routine --
   should only be called after the Save, 
   never "stand alone ".
******************************************/

{
  Hv_Item         temp;
  Hv_View         View = NULL;
  Hv_FRect        *warea;
  short           i;
  Hv_WorldPolygonData  *wpoly;
  Hv_FPoint       *fpts;
  Hv_ViewMapData  mdata;

  if (items == NULL)
    return;

  if (items->first == NULL)
    return;

  View = items->first->view;
  mdata = Hv_GetViewMapData(View);

  for (temp = items->first; temp != NULL; temp = temp->next)
    if (temp->domain == Hv_INSIDEHOTRECT) {
      if (temp->type != Hv_MAPITEM) 
	if (Hv_CheckBit(temp->drawcontrol, Hv_ZOOMABLE)) {
	  warea = temp->worldarea;

	  warea->xmin = Hv_MercatorLongToX(mdata->lambda0, warea->xmin);
	  warea->xmax = Hv_MercatorLongToX(mdata->lambda0, warea->xmax);

/* polygon based items have to save all points */

	  if (Hv_CheckItemDrawControlBit(temp, Hv_WPOLYBASED) &&
	      !(temp->tag == Hv_DRAWINGTOOLTAG)) {
	      
	    wpoly = (Hv_WorldPolygonData *)(temp->data);
	    if (wpoly != NULL) {
	      fpts = wpoly->fpts;
	      for (i = 0; i < wpoly->numpnts; i++)
		fpts[i].h = Hv_MercatorLongToX(mdata->lambda0, fpts[i].h);
	    }
	  }

	} /* zoomable */

      if (temp->children != NULL)
	RestoreHotRectItems(temp->children);
      
    }  /* end inside hr */
}



#undef MAPTINY
#undef EPSILON

#undef ALMOST1
#undef ALMOST180







