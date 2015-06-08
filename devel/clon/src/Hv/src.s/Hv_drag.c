/**
 * <EM>Deals with dragging and rubber-banding.
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
#include "Hv_drag.h"
#include "Hv_maps.h"

/*------  local prototypes ----------*/

static void Hv_AfterDragDraw(Hv_Item,
							 Hv_Region);


static void Hv_SimpleDragItem(Hv_Item,
		 Hv_Rect *, 
		 Hv_Point,
		 Boolean);


static void Hv_DragHandler(Hv_XEvent *);

static void Hv_RubberBandHandler(Hv_XEvent *);

static void Hv_PolySelectHandler(Hv_XEvent *);

static void Hv_SimpleDragCallback(Hv_DragData);


static 	void Hv_FillWedgePoints(Hv_View    View,
			       Hv_FPoint  *fp,
			       short      np,
			       Hv_Map     map,
			       float     *ang1,
			       float     *ang2,
			       float     *radius,
			       int       *direction);


static void     Hv_Point2Rect(Hv_Rect *,
			      Hv_Point,
			      Hv_Point);

static void     Hv_EnlargeRectOrOval(Hv_Rect *,
				     Hv_Rect,
				     Hv_Point,
				     Boolean,
				     Boolean);

static void     Hv_InitShowCorner(char *,
				  Hv_Rect *,
				  Hv_Rect *);

static void     Hv_GetCurrentPoint(Hv_Point *,
				   int,
				   int,
				   Hv_Rect *);

static void     Hv_InitLimitRect(Hv_Rect *);

static void     Hv_UpdateStr(char *,
			     char *,
			     Hv_Rect *);

static void     Hv_FancyDragItem(Hv_Item,
				 Hv_Rect *,
				 Hv_Point,
				 Hv_Point *,
				 Boolean,
				 Boolean);
static short    Hv_ssign(short);
static void     Hv_UpdateSizeRect(Hv_Rect *);
static void     Hv_SetSizeString(char *,
				 char *,
				 Hv_Rect *,
				 Boolean,
				 short,
				 short,
				 float);


/*
 * One global drag structure, which should be NULL if not dragging.
 */

static Hv_DragData    thedd = NULL;

#define   Hv_MAXPOLYPNTS    200


/**
 * Hv_LaunchSelectPolygon
 * @purpose  Select a polygon by clicking (double click to stop).
 * @param view         View in question.
 * @param item         Parent item, if needed by callback
 * @param limitRect    Limit Rect
 * @param start        Starting point
 * @param selectPolicy Selection policy.
 * @param callback     Called when selection is finished.
 */

void  Hv_LaunchSelectPolygon(Hv_View view,
			     Hv_Item parent,
			     Hv_Rect *limitRect,
			     Hv_Point *start,
			     Hv_FunctionPtr callback) {

/*
 * usual pathology check
 */
  
  if ((start == NULL) || 
	  (view == NULL) ||
      (limitRect == NULL)) {
    return;
  }

/* if global dd is not NULL, something is wrong */

  if (thedd != NULL) {
    fprintf(stderr, "Warning, already an active drag occurring.\n");
    return;
  }

/* create the drag data structure */

  thedd = Hv_MallocDragData(view,
	  parent,
	  start,
	  NULL,
	  limitRect,
	  0,
	  -1,
	  callback);

  Hv_AlternateEventHandler = Hv_PolySelectHandler;

}



/* ------------ Hv_SelectWedge -------------*/


void   Hv_SelectWedge(Hv_View    View,
		      Hv_Rect    limitRect,
		      Hv_Point   start,
		      Hv_FPoint  *basepoint,
		      float      *azim1,
		      float      *azim2,
		      float      *radius,
		      int        *direction)


/* selects a polygon around the screen, staying within limitRect.
   Uses the  button specified.

   Hv_Point   **poly;      polygon selected
   short      *npts;       points in poly
   Hv_Rect    limitRect;   Limit rect -- no part of drag rect can go out of here
   Hv_Point   start;     Starting position of mouse when rect is being dragged
*/

{
  short          npts; 
  Boolean        done;                  /* Controls when dragging is over */
  Hv_XEvent      Event;                 /* An X event */
  Hv_FPoint     *fp;
  Hv_Point      *poly = NULL;
  Hv_Point       cp, pp;
  int            nclick = 1;
  Hv_Region      hrr = NULL;

  Hv_Map         map = NULL;
  Boolean        first = True;
  Boolean        mapview;

  Hv_InitLimitRect(&limitRect);
  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  Hv_GrabPointer();


/* have to behave differently for map views */
  
  mapview = (View->mapdata != NULL); 

  if (!mapview) {
    npts = Hv_NUMWEDGEPOINTS+1;
    poly = Hv_NewPoints(npts);
    poly[0].x = -32001;
  }
  else {
    npts = 3;
    map = Hv_MallocMap(Hv_NUMWEDGEPOINTS+1);
  }

  fp = (Hv_FPoint *)(Hv_Malloc(npts*sizeof(Hv_FPoint)));

  cp = start;
  pp = start;
  done = False;

  *direction = -1;

/* first point of poly is the original click */

  Hv_LocalToWorld(View, &(fp[0].h), &(fp[0].v), cp.x, cp.y);

  if (mapview) { /* store lat long of basepoint */
      Hv_XYToLatLong(View, fp[0].h, fp[0].v, &(basepoint->h), &(basepoint->v));
  }
  else {
      Hv_SetFPoint(basepoint, fp[0].h, fp[0].v);
  }

  while (!done) {

/* if Check For Motion or Button Press */


    if (Hv_CheckMaskEvent(Hv_BUTTONPRESSMASK | Hv_BUTTONMOTIONMASK,
			  &Event)) {
	  

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case Hv_motionNotify: case Hv_buttonPress: 
	Hv_GetCurrentPoint(&cp,
			   Hv_GetXEventX(&Event),
			   Hv_GetXEventY(&Event),
			   &limitRect);
	

/* If haven't had 2nd click we are getting the
   radius else determining arc */

	if (nclick == 1) {
	  Hv_LocalToWorld(View, &(fp[1].h),
			  &(fp[1].v),
			  cp.x, cp.y);

	  Hv_DrawXorLine(&(start), &pp);
	  Hv_DrawXorLine(&(start), &cp);
	  pp = cp;
	}
	else {  /* already clicked twice */

	  Hv_LocalToWorld(View, &(fp[npts-1].h),
			  &(fp[npts-1].v),
			  cp.x, cp.y);

	  if (!mapview) {
	    Hv_FillWedgePoints(View,
			       fp, npts, map,
			       azim1, azim2,
			       radius, direction);
	  }

	  if (Hv_feedbackWhileSelecting && (Hv_hotView->feedback)) {
	    Hv_hotView->feedback(Hv_hotView, cp);
	    Hv_SetClippingRegion(hrr);
	  }


	  if (first)
	    first = False;
	  else { /* erasure */
	    if (!mapview)
	      Hv_FrameXorPolygon(poly, npts); 
	    else {
	      Hv_DrawWedgeMap(View,
			      NULL,
			      hrr,
			      map,
			      False,
			      -1,
			      -1,
			      -1);
	    }
	  }
	  

	  if (!mapview) {
	    Hv_WorldPolygonToLocalPolygon(View, npts, poly, fp);
	    if (Event.type != Hv_buttonPress)
	      Hv_FrameXorPolygon(poly, npts);
	  }
	  else {
	    if (Event.type != Hv_buttonPress) {
	      Hv_FillWedgePoints(View,
				 fp, npts, map,
				 azim1, azim2,
				 radius, direction);
	      Hv_DrawWedgeMap(View,
			      NULL,
			      hrr,
			      map,
			      False,
			      -1,
			      -1,
			      -1);
	    }
	  }
	  

	}

	if (Event.type == Hv_buttonPress) {
	  nclick++;
	  
	  if (nclick == 2) {
	    Hv_DrawXorLine(&(start), &pp);
	  }

/* third click, counting the first, terminates */

	  else if (nclick == 3) {
	    done = True;
	    Hv_FullClip();
	  }
	}

	
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */

  if (mapview)
    Hv_DestroyMap(&map);
  else
    Hv_Free(poly);

  Hv_Free(fp);
  Hv_DestroyRegion(hrr);
  Hv_UngrabPointer();

}



/*-----------------------------------------------------------
 *  Hv_FillWedge uses the 0th, 1st and last points to
 *  fill in the reset making an arch
 *-----------------------------------------------------------*/

static 	void Hv_FillWedgePoints(Hv_View    View,
				Hv_FPoint *fp,
				short      np,
				Hv_Map     map,
				float     *ang1,
				float     *ang2,
				float     *radius,
				int       *direction) {
  static float openang;

  float newang;
  float dx1, dy1;
  float dx2, dy2;
  float dx, dy;
  float dang, ang;
  float latitude, longitude;
  float lat3, long3;
  float lat1, long1;
  float crossproduct;
  float endang;

  Boolean mapview = (View->mapdata != NULL);

  float a1, a2;

  int   i;
  short npm1, npm2;

  npm1 = np-1;
  npm2 = np-2;
  

  if (*direction < 0) {
    openang  = 0.0;
  }

  dx1 = fp[1].h    - fp[0].h;
  dy1 = fp[1].v    - fp[0].v;
  dx2 = fp[npm1].h - fp[0].h;
  dy2 = fp[npm1].v - fp[0].v;


  a1 = (float)(atan2(dy1, dx1));
  a2 = (float)(atan2(dy2, dx2));
  
  newang = a2 - a1;

/* have to fix wrap problem */

  if (fabs(newang-openang) > 1.0) {
    if (newang < openang)
      newang += (float)(Hv_TWOPI);
    else
      newang -= (float)(Hv_TWOPI);
  }

  openang = newang;

  if (openang < -Hv_TWOPI)
    openang += (float)(Hv_TWOPI);
  else if (openang > Hv_TWOPI)
    openang -= (float)(Hv_TWOPI);


/* look for a change from the master direction if small opening ang */

  if (fabs(openang) < 0.5) {
    crossproduct = dx1*dy2 - dx2*dy1;
    *direction = (crossproduct < 0.0) ? Hv_CLOCKWISE : Hv_COUNTERCLOCKWISE;
  }
	

/* for map views we want the base lat long */

    if (mapview) {
      Hv_XYToLatLong(View, fp[0].h, fp[0].v, &latitude, &longitude);
      Hv_XYToLatLong(View, fp[1].h, fp[1].v, &lat1, &long1);
      Hv_XYToLatLong(View, fp[npm1].h, fp[npm1].v, &lat3, &long3);

/* note getazim always returns between 0 and 2 pi */

      *ang1 = Hv_GetAzimuth(latitude, longitude, lat1, long1);
      *ang2 = Hv_GetAzimuth(latitude, longitude, lat3, long3);
      *radius = Hv_GCDistance(latitude, longitude, lat1, long1);
    }
    else {
      dx1 = fp[1].h - fp[0].h;
      dy1 = fp[1].v - fp[0].v;
      
      dx2 = fp[npm1].h - fp[0].h;
      dy2 = fp[npm1].v - fp[0].v;
      
      *ang1 = (float)atan2(dy1, dx1);
      *ang2 = (float)atan2(dy2, dx2);

      if (*ang1 < 0.0)
	*ang1 += (float)(Hv_TWOPI);

      if (*ang2 < 0.0)
	*ang2 += (float)(Hv_TWOPI);

      *radius = (float)sqrt(dx1*dx1 + dy1*dy1);
    }

    endang = *ang2;
    
    if (*direction == Hv_COUNTERCLOCKWISE) {
      if (*ang2 > *ang1) {
	endang -= (float)Hv_TWOPI;
      }
    }
    else {
      if (*ang2 < *ang1) {
	endang += (float)Hv_TWOPI;
      }
    }

  
    if (mapview) {
      Hv_SetWedgeMapPoints(View,
			   map,
			   latitude,
			   longitude,
			   0.0,
			   *radius,
			   *ang1,
			   endang,
			   Hv_WEDGEMODE);
    }
    else {

      ang = endang - *ang1;
      dang = ang/npm2;
      for (i = 2; i < np; i++) {
	ang = *ang1 + (i-1)*dang;
	dx = (float)((*radius)*cos(ang));
	dy = (float)((*radius)*sin(ang));
	fp[i].h = fp[0].h + dx;
	fp[i].v = fp[0].v + dy;
      }
    }
 
}

/**
 * Hv_LaunchSelectOval
 * @purpose  Select an oval by rubber-banding.
 * @param view         View in question.
 * @param item         Item, if needed by callback
 * @param limitRect    Limit Rect
 * @param start        Starting point
 * @param selectPolicy Selection policy.
 * @param callback     Called when selection is finished.
 */

void   Hv_LaunchSelectOval(Hv_View view,
			   Hv_Item item,
			   Hv_Rect          *limitRect,
			   Hv_Point         *start,
			   short             selectPolicy,
			   Hv_FunctionPtr    callback) {
  
  Hv_LaunchSelectRect(view,
		      item,
		      limitRect,
		      start,
		      (short)(selectPolicy | Hv_OVALMETHOD),
		      (float)(1.0),
		      callback);
}


/**
 * Hv_LaunchSelectRect
 * @purpose  Select a rectangle by rubber-banding.
 * @param view         View in question.
 * @param item         Item, if needed by callback
 * @param limitRect    Limit Rect
 * @param start        Starting point
 * @param selectPolicy Selection policy.
 * @param asprat       Aspect ratio, used when enforcing aspect ratio.
 * @param callback     Called when selection is finished.
 */

void   Hv_LaunchSelectRect(Hv_View view,
			   Hv_Item item,
			   Hv_Rect          *limitRect,
			   Hv_Point         *start,
			   short             selectPolicy,
			   float             asprat,
			   Hv_FunctionPtr    callback) {


/*
 * usual pathology check
 */
  
  if ((start == NULL) || 
	  (view == NULL) ||
      (limitRect == NULL)) {
    return;
  }
  
/* if global dd is not NULL, something is wrong */

  if (thedd != NULL) {
    fprintf(stderr, "Warning, already an active drag occurring.\n");
    return;
  }

/* create the drag data structure */

  thedd = Hv_MallocDragData(view,
	  item,
	  start,
	  NULL,
	  limitRect,
	  selectPolicy,
	  -1,
	  callback);

  thedd->asprat = asprat;

  Hv_AlternateEventHandler = Hv_RubberBandHandler;
}

/* ------------ Hv_EnlargeLine ----------*/

void Hv_EnlargeLine(Hv_Rect   limitRect,
		    Hv_Point  start,
		    Hv_Point *fixedpnts,
		    short     nfp,
		    Hv_Point *varpnt)

/*****************************************

Permits a line to be enlarged

Hv_Rect   limitRect
Hv_Point  start   starting mouse point
Hv_Point *fixedpnts anchored points on line/
short     nfp
Hv_Point *varpnt    point on line that is changing; not exactly start because of slop
*****************************************/

/*---- private trick: if nfp < 0 call feedback while resizing */


{
  Boolean        done;                  /* Controls when dragging is over */
  Hv_XEvent      Event;                 /* An X event */
  short          dh, dv, oldx, oldy;
  int            i;
  Hv_Point       current;
  Boolean        fback = False;
  Hv_Region      hrr = NULL;
  Boolean        userok = True;    

  Hv_InitLimitRect(&limitRect);
  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  Hv_GrabPointer();
  oldx = start.x;
  oldy = start.y;

  done = False;

  if (nfp < 0) {
    fback = True;
    nfp = -nfp;
  }

  for (i = 0; i < nfp; i++)
    Hv_DrawXorLine(fixedpnts+i, varpnt);

  while (!done) {

/* if Check For Motion or Button Releases */

    if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK | Hv_BUTTONPRESSMASK | Hv_BUTTONMOTIONMASK,
			  &Event)) {
	  

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case Hv_motionNotify: 

	Hv_SetClippingRegion(hrr);
	for (i = 0; i < nfp; i++)
	  Hv_DrawXorLine(fixedpnts+i, varpnt);

	Hv_GetCurrentPoint(&current,
			   Hv_GetXEventX(&Event),
			   Hv_GetXEventY(&Event),
			   &limitRect);
	
	if (fback && (Hv_hotView->feedback))
	  Hv_hotView->feedback(Hv_hotView, current);
	
	dh = current.x - oldx;
	dv = current.y - oldy;
	
	varpnt->x += dh;
	varpnt->y += dv;
	
/* see if the user routine vetos this. A userok of
   "True" means the user is happy so could forward! */

	if (Hv_UserLineEnlargeCheck != NULL)
	  Hv_UserLineEnlargeCheck(Hv_theEnlargedPoly,
				  Hv_theEnlargedPolyNp,
				  &userok);
	else
	  userok = True;
	
	if (!userok) {
	  varpnt->x -= dh;
	  varpnt->y -= dv;
	  Hv_SysBeep();
	}
	else {
	  oldx = current.x;
	  oldy = current.y;
	}

	Hv_SetClippingRegion(hrr);
	for (i = 0; i < nfp; i++)
	  Hv_DrawXorLine(fixedpnts+i, varpnt);
	
	break;

     /* end of case Hv_motionNotify */
	      
/* if button whatButton is released, we are done */

      case Hv_buttonRelease:
	done  = True;
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor line */

  Hv_SetClippingRegion(hrr);
  for (i = 0; i < nfp; i++)
    Hv_DrawXorLine(fixedpnts+i, varpnt);

  Hv_UngrabPointer();
  Hv_DestroyRegion(hrr);
}


/* ------------ Hv_ConnectLine ----------*/

void Hv_ConnectLine(Hv_Rect   limitRect,
		    Hv_Point  start,
		    Hv_Point *stop)

/* slightly different protocol when connecting two things:
   click on one, relase, move to the other, click */

{
  Boolean        done;                  /* Controls when dragging is over */
  Hv_XEvent         Event;                 /* An X event */

  Hv_InitLimitRect(&limitRect);

  Hv_GrabPointer();
  *stop = start;
  done = False;

  Hv_DrawXorLine(&start, stop);

  while (!done ) {

/* if Check For Motion or Button Presses */

    if (Hv_CheckMaskEvent(Hv_BUTTONPRESSMASK | Hv_BUTTONMOTIONMASK,
			  &Event)) {
	  

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case Hv_motionNotify: 
	Hv_DrawXorLine(&start, stop);
	Hv_GetCurrentPoint(stop,
			   Hv_GetXEventX(&Event),
			   Hv_GetXEventY(&Event),
			   &limitRect);
	Hv_DrawXorLine(&start, stop);
	break;

     /* end of case Hv_motionNotify */
	      
/* if button whatButton is released, we are done */

      case Hv_buttonPress:
	done  = True;
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor line */

  Hv_DrawXorLine(&start, stop);
  Hv_UngrabPointer();
}


/* ------------ Hv_CreateLine -------------*/

void   Hv_CreateLine(Hv_Rect   limitRect,
		     Hv_Point  start,
		     Hv_Point *stop)

/* selects a rectangle around the screen, staying within limitRect.
   Uses the  button specified. */

{
  Hv_Point  fixedpnt;

  fixedpnt = start;
  *stop = start;

  Hv_EnlargeLine(limitRect, start, &fixedpnt, 1, stop);
  return;
}


/* ------------ Hv_EnlageRect -------------*/

void   Hv_EnlargeRect(Hv_Rect *TheGrowRect,
		      Hv_Rect  limitRect,
		      Hv_Point start,
		      Boolean  ShowSize)

/*
Hv_Rect  *TheGrowRect; Original Position of rectangle to be grown  (CHANGED!!)
Hv_Rect   limitRect;   Limit rect -- no part of drag rect can go out of here
Hv_Point  start;     Starting position of mouse when rect is being dragged
Boolean   ShowSize;    if true, show the size (W, H) of the rect as it is grown
*/

{
  Hv_EnlargeRectOrOval(TheGrowRect, limitRect, start, ShowSize, False);
}

/* ------------ Hv_EnlageOval -------------*/

void   Hv_EnlargeOval(Hv_Rect  *TheGrowRect,
		      Hv_Rect   limitRect,
		      Hv_Point  start,
		      Boolean   ShowSize)

/*
Hv_Rect   *TheGrowRect;  Original Position of rectangle to be grown  (CHANGED!!)
Hv_Rect   limitRect;     Limit rect -- no part of drag rect can go out of here
Hv_Point  start;       Starting position of mouse when rect is being dragged
Boolean   ShowSize;      if true, show the size (W, H) of the rect as it is grown
*/

{
  Hv_EnlargeRectOrOval(TheGrowRect, limitRect, start, ShowSize, True);
}


/* ------------ Hv_EnlageRectOrOval -------------*/

static void   Hv_EnlargeRectOrOval(Hv_Rect *TheGrowRect,
				   Hv_Rect  limitRect,
				   Hv_Point start,
				   Boolean  ShowSize,
				   Boolean  IsOval)

/* selects a rectangle around the screen, staying within limitRect.
   Uses the  button specified. (Replacement for Hv_GrowRect)

Hv_Rect  *TheGrowRect      Original Position of rectangle to be grown  (CHANGED!!) 
Hv_Rect  limitRect         Limit rect -- no part of drag rect can go out of here 
Hv_Point start           Starting position of mouse when rect is being dragged 
Boolean  ShowSize          if true, show the size (W, H) of the rect as it is grown 
Boolean  IsOval            if true, this is an oval

*/

{
  Boolean     done;             /* Controls when dragging is over */
  Hv_XEvent   Event;            /* An X event */
  Hv_Rect     crect;            /* current rect */
  short       dh, dv;           /* offests */
  Hv_Rect     srect;            /* for displaying the size of the growrect */

  char             *newstr = " 1234, 1234 ";
  char             *oldstr =  " XXXX, XXXX ";
  short            grabcorner;  /* which corner we are grabbing */
  Hv_Point         current;
  

  grabcorner = Hv_WhichPointOnRect(start.x, start.y, TheGrowRect);
  Hv_InitLimitRect(&limitRect);

  done = False;
  Hv_CopyRect(&crect, TheGrowRect);

  if (ShowSize) {
    Hv_InitShowCorner(newstr, &crect, &srect);
    Hv_SetSizeString(newstr, oldstr, &srect, False, crect.width, crect.height, 0.0);
  }

  if (IsOval)
    Hv_DrawXorOval(&crect);
  else
    Hv_DrawXorRect(&crect);

  while (!done) {


/* if Check For Motion or Button Releases */


    if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK | Hv_BUTTON1MOTIONMASK,
			  &Event)) {
	  
      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case Hv_motionNotify: {
	Hv_GetCurrentPoint(&current,
			   Hv_GetXEventX(&Event),
			   Hv_GetXEventY(&Event),
			   &limitRect);
	dh = current.x - start.x;
	dv = current.y - start.y;


	if (IsOval)
	  Hv_DrawXorOval(&crect);
	else
	  Hv_DrawXorRect(&crect);

	switch (grabcorner) {

	case Hv_UPPERLEFT:
	  crect.left = TheGrowRect->left + dh;
	  crect.top  = TheGrowRect->top  + dv;
	  break;

	case Hv_UPPERRIGHT:
	  crect.right = TheGrowRect->right + dh;
	  crect.top   = TheGrowRect->top   + dv;
	  break;

	case Hv_LOWERRIGHT:
	  crect.right  = TheGrowRect->right  + dh;
	  crect.bottom = TheGrowRect->bottom + dv;
	  break;

	case Hv_LOWERLEFT:
	  crect.left   = TheGrowRect->left   + dh;
	  crect.bottom = TheGrowRect->bottom + dv;
	  break;

	case Hv_MIDDLELEFT:
	  crect.left = TheGrowRect->left + dh;
	  break;

	case Hv_MIDDLETOP:
	  crect.top = TheGrowRect->top + dv;
	  break;

	case Hv_MIDDLERIGHT:
	  crect.right = TheGrowRect->right + dh;
	  break;

	case Hv_MIDDLEBOTTOM:
	  crect.bottom = TheGrowRect->bottom + dv;
	  break;

	}

	Hv_FixRectWH(&crect);


	if (IsOval)
	  Hv_DrawXorOval(&crect);
	else
	  Hv_DrawXorRect(&crect);
	
	break;
      }  /* end of case Hv_motionNotify */
	      
/* if button 1 released, we are done */

      case Hv_buttonRelease:
	if (Hv_GetXEventButton(&Event) == 1) {	      
	  done = True;
	  break;
	}
	
      default:
	break;
      } /* end of switch on event type */
    }  /* end  if if CheckMask */

    if (ShowSize)
      Hv_SetSizeString(newstr, oldstr, &srect, False, crect.width, crect.height, 0.0);
    
  }   /*end of while !done */
  
/* Final xor rect */


  if (IsOval)
    Hv_DrawXorOval(&crect);
  else
    Hv_DrawXorRect(&crect);
      
  if (ShowSize)
    Hv_UpdateSizeRect(&srect);

  Hv_CopyRect(TheGrowRect, &crect);
}


/**
 * Hv_DragItem
 * @purpose   Drag an item.
 * @param   Item        Item being dragged.
 * @param   limitRect   Limit rect, probably the hotrect.
 * @param   start       Starting point
 * @param   showCorner  If True, show corner location while dragging.
 */


void Hv_DragItem(Hv_Item  Item,
		 Hv_Rect  *limitRect, 
		 Hv_Point start,
		 Boolean  showCorner) {

  Hv_View     View = Hv_GetItemView(Item);
  Hv_Point    stop;
  short       dh, dv;
  Hv_Region   udr1, udr2, udr3;
  Boolean     rotateIt = False;
  Boolean     fancyDrag = False;
  Boolean     enlarged;
  Hv_Region   connectreg;

/* if a balloon is up, take it down */

  if (Hv_balloonUp)
    Hv_HideBalloon();

/* bring the view to the front */

  Hv_SendViewToFront(View);

/* area we actually enlarging? */

  if ((Hv_CheckBit(Item->drawcontrol, Hv_ENLARGEABLE)) && (Item->checkenlarge != NULL)) {
    Item->checkenlarge(Item, start, showCorner, &enlarged);
    if (enlarged)
      return;
  }
  
/* are we actually rotating? */

  if (Hv_CheckBit(Item->drawcontrol, Hv_ROTATABLE) && Hv_PointInRect(start, Item->rotaterect)) {
    View->lastrotateitem = Item;
    View->lastazimuth = Item->azimuth;
    rotateIt = True;
  }


  if (!rotateIt) {
    if (!Hv_CheckBit(Item->drawcontrol, Hv_DRAGGABLE))
      return;
    View->lastdragitem = Item;
  }

/* Fancy drag ? */

  fancyDrag = Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_FANCYDRAG);
  Hv_completeFeedback = False;

/* just a simple drag? */


  if ((!fancyDrag) && (!rotateIt)) {
	  Hv_SimpleDragItem(Item, limitRect, start, showCorner);
	  return;
  }


  Hv_extraFastPlease = fancyDrag || rotateIt;

  udr1 = Hv_CopyItemRegion(Item, True);    /* original region */
  connectreg = Hv_ConnectionRegion(Item);
  if (connectreg != NULL) {
    Hv_AddRegionToRegion(&udr1, connectreg);
    Hv_DestroyRegion(connectreg);
    connectreg = NULL;
  }

  Hv_ShrinkRegion(udr1, -3, -3); 

  udr2 = Hv_ClipHotRect(View, 0);

  if (rotateIt || fancyDrag) {
    Hv_FancyDragItem(Item, limitRect, start, &stop, rotateIt, showCorner);
    dh = stop.x - start.x;
    dv = stop.y - start.y;
    Hv_extraFastPlease = False;
    
    if (!rotateIt) {
      if (Item->afterdrag != NULL)
	Item->afterdrag(Item, 0, 0);
      Hv_SetPoint(&(View->lastdragdelta), (short)(-dh), (short)(-dv));
    }

  }
  Hv_DestroyRegion(udr2);
  
  Hv_completeFeedback = True;
  
  if (Item->fixregion != NULL)
    Item->fixregion(Item);  
  
  if (!Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_NOUPDATEAFTERDRAG))
    if ((dh != 0) || (dv != 0)) {
      udr2 = Hv_CopyItemRegion(Item, True);     /* new region */
      
      if (!Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_DONTUPDATECONNECTIONS)) {
	Hv_UpdateConnections(View, Item, False);
	connectreg = Hv_ConnectionRegion(Item);
	if (connectreg != NULL) {
	  Hv_AddRegionToRegion(&udr2, connectreg);
	  Hv_DestroyRegion(connectreg);
	  connectreg = NULL;
	}
      }
      
      Hv_ShrinkRegion(udr2, -3, -3); 
      
      udr3 = Hv_UnionRegion(udr2, udr1);
      Hv_extraFastPlease = False;
      Hv_DrawView(View, udr3);   
      
      Hv_DestroyRegion(udr2);
      Hv_DestroyRegion(udr3);
    } /* dh or dv nonzero */
  
  Hv_DestroyRegion(udr1); 
}


/**
 * Hv_SimpleDragItem
 * @purpose   Simple, routine drag of an item.
 * @param   item        Item being dragged.
 * @param   limitrect   Limit rect, probably the hotrect.
 * @param   start       Starting point
 * @param   show        If True, show corner location while dragging.
 * @local
 */


static void Hv_SimpleDragItem(Hv_Item  item,
		 Hv_Rect  *limitrect, 
		 Hv_Point start,
		 Boolean  show) {

	int         policy;


	if ((item == NULL) || (limitrect == NULL))
		return;

	policy = Hv_NODRAGPOLICY;

	if (show)
		policy += Hv_SHIFTPOLICY;

	  Hv_LaunchDragRect(item->view,
		  item,
		  &start,
		  item->area,
		  limitrect,
		  policy,
		  Hv_SimpleDragCallback);

}


/**
 * Hv_SimpleDragCallback
 * @purpose This is the callback for the routine drag of an item.
 * @param   dd   The drag data pointer.
 */

static void Hv_SimpleDragCallback(Hv_DragData dd) {
  
	short      dh, dv;   /* offsets */
	
	if ((dd == NULL) || (dd->view == NULL) || (dd->item == NULL)) {
		fprintf(stderr, "Serious problem in Hv_SimpleDragCallback.");
		return;
	}

/* set the offset values */
	      
	dh = dd->finalpp.x - dd->startpp.x;
	dv = dd->finalpp.y - dd->startpp.y;
  
    Hv_offsetDueToDrag = True ;
    Hv_OffsetItem(dd->item, dh, dv, True);
    Hv_offsetDueToDrag = False;
    
/* if this is a hotrect object, we have to fix world rect */

    Hv_FixItemWorldArea(dd->item, True);
    
/* specialized after drag callback? */

    if (dd->item->afterdrag != NULL)
      dd->item->afterdrag(dd->item, dh, dv);

/* set up for "undo" */


    Hv_SetPoint(&(dd->view->lastdragdelta), 
		(short)(-dh), 
		(short)(-dv));


	Hv_completeFeedback = True;
	
	if (dd->item->fixregion != NULL)
		dd->item->fixregion(dd->item);  
	
	if (!Hv_CheckBit(dd->item->drawcontrol, Hv_NOUPDATEAFTERDRAG))
		if ((dh != 0) || (dv != 0))
			Hv_AfterDragDraw(dd->item, dd->itemregion);
		
}

/**
 * Hv_AfterDragDraw
 * @purpose  Draws the view of the item with an update region set
 *           as the union of the old item region and the new item region
 *           (including connection regions). 
 * @param    item    The item in question.
 * @param    oldreg     The original region (not freed)
 */

static void Hv_AfterDragDraw(Hv_Item    item,
							 Hv_Region  oldreg) {

	Hv_Region  newreg, connectreg, totreg;

	if (item == NULL)
		return;

	newreg = Hv_CopyItemRegion(item, True);     /* new region */

/* have to update those wacky connections */

	if (!Hv_CheckBit(item->drawcontrol, Hv_DONTUPDATECONNECTIONS)) {
		Hv_UpdateConnections(item->view, item, False);
		connectreg = Hv_ConnectionRegion(item);
		if (connectreg != NULL) {
			Hv_AddRegionToRegion(&newreg, connectreg);
			Hv_DestroyRegion(connectreg);
		}
	}
				
	Hv_ShrinkRegion(newreg, -3, -3); 
				
	totreg = Hv_UnionRegion(newreg, oldreg);
	Hv_extraFastPlease = False;
	Hv_DrawView(item->view, totreg);   
				
	Hv_DestroyRegion(newreg);
	Hv_DestroyRegion(totreg);
}


/**
 * Hv_FancyDragItem
 * @purpose  Continually redraw object as it gets dragged around
 * @param   Item;
 * @param  *limitRect;   Limit rect: no part of drag rect can go out (NOT CHANGED )
 * @param   start;       Starting position of mouse when rect is being dragged
 * @param  *stop;        Final location of mouse when dragging is done 
 * @param   rotateIt;    We are rotating, not dragging
 * @param   showCorner;  If true, show top-left while dragging
 * @local
 */

static void   Hv_FancyDragItem(Hv_Item  Item,
			       Hv_Rect  *limitRect,
			       Hv_Point start,
			       Hv_Point *stop,
			       Boolean  rotateIt,
			       Boolean  showCorner) {


  Hv_Rect        dragRect;
  Hv_View        View = Hv_GetItemView(Item);
  Hv_Rect        mouseLimit;           /* Mouse must stay in here to keep dragRect in limitRect*/
  Boolean        done = False;        /* Controls when dragging is over */
  Hv_XEvent      Event;               /* An X event */
  Hv_Rect        crect;               /* current rect */
  short          dh, dv;              /* offests */
  char          *newstr;
  char          *oldstr;
  Hv_Rect        srect;               /* for displaying the Corner of the dragrect */
  short          xc, yc;
  Hv_Point       osp;
  Hv_Region      ldr;
  Hv_Point       refpp;
  float          delx, dely;
  float          ang = Item->azimuth;
  int            count = 0;
  Hv_Rect        clipRect;
  int            i;
  float          az, daz, gcd;

/* we handle the rotation of world polys */

  Hv_WorldPolygonData  *wpoly;
  float                fx, fy;

  newstr = (char *)Hv_Malloc(30);
  oldstr  = (char *)Hv_Malloc(30);
  strcpy(newstr, " 1234, 1234 ");
  strcpy(oldstr,  " 1234, 1234 ");

  Hv_CopyRect(&dragRect, Item->area);

/* If no reference point, use the center of the area */

  if (Item->rotaterp == NULL)
      Hv_GetRectCenter(Item->area, &(refpp.x), &(refpp.y));
  else
      Hv_SetPoint(&refpp, Item->rotaterp->x, Item->rotaterp->y);

	Hv_CopyRect(&clipRect, limitRect);
	Hv_ShrinkRect(&clipRect, 1, 1);
	ldr = Hv_RectRegion(&clipRect);
	Hv_SetClippingRegion(ldr);

/*  ldr = Hv_ClipHotRect(View, 1);*/

/* for right-bottom ambiguities, it is practical to shrink the limit rect a little */

  Hv_CopyRect(&mouseLimit, limitRect);
  Hv_InitLimitRect(&mouseLimit);

/* initialize */

  *stop = start;

  Hv_CopyRect(&crect, &dragRect);

  if (showCorner) {
    Hv_InitShowCorner(newstr, &crect, &srect);
    Hv_SetSizeString(newstr, oldstr, 
		     &srect, rotateIt,
		     crect.left, crect.top, 
		     Hv_RADTODEG*ang);
  }

  if (Hv_hotItem != NULL)
    Hv_GetItemCenter(Hv_hotItem, &xc, &yc);

  Hv_DrawXorItem(Item, ldr); 

  while (!done) {

/* if Check For Motion or Button Releases */


      if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK | Hv_BUTTON1MOTIONMASK,
			  &Event)) {
	  
	switch(Event.type) {

/* Motion Notify means the mouse is moving */

	case Hv_motionNotify:
	  count++;
	  if ((count % 2) == 1) {
	    
	    Hv_DrawXorItem(Item, ldr); 
	    Hv_GetCurrentPoint(stop,
			       Hv_GetXEventX(&Event),
			       Hv_GetXEventY(&Event),
			       &mouseLimit);

	    if (rotateIt) {
	      delx = (float)(stop->x - refpp.x);
	      dely = (float)(stop->y - refpp.y);
	      if ((delx != 0.0) ||  (dely != 0.0))
		ang = (float)atan2((double)delx, (double)(-dely));
	      if (ang < 0)
		ang += (float)Hv_TWOPI;

/* put in a handler for world polygons */

	      if (Item->type == Hv_WORLDPOLYGONITEM) {
		wpoly = (Hv_WorldPolygonData *)(Item->data);


		if (Hv_TreatAsMap(Item)) {


		  daz = Item->azimuth-ang; /*change in azimuth*/


/* in map views, fpts store lat longs, 1st point is base */

		  for (i = 0; i < wpoly->numpnts; i++) {
		    az = Hv_GetAzimuth(wpoly->basepoint.h,
				       wpoly->basepoint.v,
				       wpoly->fpts[i].h,
				       wpoly->fpts[i].v);



		    az -= daz;


		    gcd = Hv_GCDistance(wpoly->basepoint.h, wpoly->basepoint.v,
					wpoly->fpts[i].h, wpoly->fpts[i].v);


		    Hv_OffsetLatLong(wpoly->basepoint.h, wpoly->basepoint.v, 
				     gcd, az,
				     &(wpoly->fpts[i].h), &(wpoly->fpts[i].v));


		  }

		}
		else {
		  Hv_LocalToWorld(View, &fx, &fy, refpp.x, refpp.y);
		  Hv_RotateFPointsAboutPoint(Item->azimuth-ang, wpoly->fpts, wpoly->numpnts, fx, fy);
		  Hv_WorldPolygonToLocalPolygon(View, wpoly->numpnts, wpoly->poly, wpoly->fpts);
		}
		
	      } /* end world poly handler */

	      Item->azimuth = ang;

/* as of 1.0051, items can have individual afterrotates */

	      if (Item->afterrotate != NULL)
		  Item->afterrotate(Item);
	      
	      if (Hv_AfterItemRotate != NULL)
		Hv_AfterItemRotate(Item, ang);
	    }

	    else {   /* must be dragging */ 
	      dh = stop->x - start.x;
	      dv = stop->y - start.y;
	      Hv_offsetDueToDrag = True ;
	      Hv_OffsetItem(Item, dh, dv, True);
	      Hv_offsetDueToDrag = False;
	      
/* if this is a hotrect object, we have to fix world rect */
	  
	      Hv_FixItemWorldArea(Item, True);

	      if (Item->afterdrag != NULL)
		Item->afterdrag(Item, dh, dv);
	      
	      if (Hv_hotItem != NULL)
		if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_FEEDBACKWHENDRAGGED))
		  if (Hv_PointInRect(*stop, Hv_hotView->hotrect))
		    if (Hv_hotView->feedback != NULL) {
		      Hv_SetPoint(&osp, stop->x, stop->y); 
		      Hv_hotView->feedback(Hv_hotView, osp);
		      Hv_SetClippingRegion(ldr);
		    }
	      
	      Hv_OffsetRect(&crect, dh, dv);
	    }

	    start = *stop;
	    Hv_DrawXorItem(Item, ldr);
	  }
	  break; /* end of case Hv_motionNotify */

	      
/* if button 1 released, we are done */

	case Hv_buttonRelease:
	  if (Hv_GetXEventButton(&Event) == 1) {	      
	    done = True;
	    break;
	  }
	      
	default:
	  break;
	}

      }  /* end  if if CheckMask */


      if (showCorner)
	Hv_SetSizeString(newstr, oldstr, &srect, rotateIt,
			 crect.left, crect.top, Hv_RADTODEG*ang);
      
    }   /*end of while !done */

/* Final xor draw */
      
  Hv_DrawXorItem(Item, ldr);

  if (showCorner)
    Hv_UpdateSizeRect(&srect);

  Hv_DestroyRegion(ldr);
  Hv_Free(newstr);
  Hv_Free(oldstr);
}


/* ----- Hv_UpdateStr ---- */

static void Hv_UpdateStr(char *newstr,
			 char *oldstr,
			 Hv_Rect *r)

{
  Hv_String   *hvstr;
  
  if (strcmp(newstr, oldstr) == 0)
    return;

  hvstr = Hv_CreateString(newstr);
  
  Hv_SetBackground(Hv_colors[Hv_white]);
  Hv_useDrawImage = True;
  Hv_DrawCompoundString((short)(r->left+1), (short)(r->top+1), hvstr);
  Hv_useDrawImage = False;
  Hv_DestroyString(hvstr);
  strcpy(oldstr, newstr);
  Hv_SetBackground(Hv_colors[Hv_canvasColor]);
}


/*------- Hv_GetCurrentPoint ------*/

static void     Hv_GetCurrentPoint(Hv_Point *CPnt,
				   int x,
				   int y,
				   Hv_Rect *limitRect)

{
  CPnt->x = Hv_sMax(limitRect->left, Hv_sMin(limitRect->right,  (short)x));
  CPnt->y = Hv_sMax(limitRect->top,  Hv_sMin(limitRect->bottom, (short)y));
}


/**
 * @purpose  Local routine for initiailize a rectangle used as a drag-limit
 *           rect.
 * @param  limitRect    The Rect to initialize.
 * @local
 */

static void Hv_InitLimitRect(Hv_Rect *limitRect) {
  limitRect->width  -= 1;
  limitRect->height -= 1;
  Hv_FixRectRB(limitRect);
}

/*--------- Hv_InitShowCorner -----*/

static void     Hv_InitShowCorner(char *newstr,
				  Hv_Rect *CurRect,
				  Hv_Rect *ShowRect)

{
  Hv_String    *hvstr;
  short         w, h;

  hvstr = Hv_CreateString(newstr);
  Hv_CompoundStringDimension(hvstr, &w, &h);

  Hv_SetRect(ShowRect,
	     (short)((CurRect->left + CurRect->right - ((short)w))/2),
	     (short)((CurRect->top + CurRect->bottom - ((short)h))/2),
	     (short)(w+6), (short)(h+4)); 

  Hv_FillRect(ShowRect,  Hv_white);
  Hv_FrameRect(ShowRect, Hv_black);

  Hv_DestroyString(hvstr);  
}


/* ------- Hv_Point2Rect ---------- */

static void Hv_Point2Rect(Hv_Rect *rect,
			  Hv_Point P1,
			  Hv_Point P2)

/* converts two pixel points into a rect */

{
  rect->left  = P1.x;
  rect->top   = P1.y;
  rect->right = P2.x;
  rect->bottom = P2.y;

  Hv_GoodRect(rect);
  Hv_FixRectWH(rect);
}


/* ------------ Hv_ssign ----------------- */

static short  Hv_ssign(short s)

{
  if (s < 0)
    return -1;
  else
    return 1;
}


/*------- Hv_SetSizeString -------*/

static void Hv_SetSizeString(char    *newstr,
			     char    *oldstr,
			     Hv_Rect *srect,
			     Boolean  usefloat,
			     short    i1,
			     short    i2,
			     float    f)

{
  if (usefloat)
    sprintf(newstr, " %-4.1f deg ", f);
  else
    sprintf(newstr, " %-4d, %-4d", i1, i2);
  Hv_UpdateStr(newstr, oldstr, srect);
}


/*------ Hv_UpdateSizeRect ---------*/

static void Hv_UpdateSizeRect(Hv_Rect *srect)

{
  Hv_Region  rgn;
  Boolean    temp;

  Hv_ShrinkRect(srect, -2, -2);
  rgn = Hv_RectRegion(srect);

  temp = Hv_extraFastPlease;
  Hv_extraFastPlease = False;
  Hv_DrawView(Hv_hotView, rgn);
  Hv_extraFastPlease = temp;

  Hv_DestroyRegion(rgn);
}

/**
 * Hv_LaunchDragRect
 * @purpose  Replacement for old drag rect function. This one does not
 *           require a secondary event loop.
 * @param  view       View where dragging is taking place.
 * @param  item       Item being dragged. If NULL, we are not dragging
 *                    an item, but maybe a view or scroll bar thumb.
 * @param  startpp    Mouse point where drag initiated.
 * @param  startrect  Initial location of rectangle
 * @param  limitrect  Confining rectangle
 * @param  policy     If RECTCONFINED, the rect being dragged cannot leave
 *                    the limit rect. If MOUSECONFINED, the mouse cannot
 *                    leave, which means the rect can be dragged off the edge.
 * @param  callback   Callback when dragging is done.
 */

void Hv_LaunchDragRect(Hv_View         view,
					   Hv_Item         item,
		       Hv_Point       *startpp,
		       Hv_Rect        *startrect,
		       Hv_Rect        *limitrect,
		       int             policy,
		       Hv_FunctionPtr  callback) {

	Hv_Region  vreg, rreg, treg;

/*
 * usual pathology check
 */

  if ((view == NULL) || 
      (startpp == NULL) || 
      (startrect == NULL) ||
      (limitrect == NULL)) {
    return;
  }

/* if global dd is not NULL, something is wrong */

  if (thedd != NULL) {
    fprintf(stderr, "Warning, already an active drag occurring.\n");
    return;
  }

/* create the drag data structure */

  thedd = Hv_MallocDragData(view,
	  item,
	  startpp,
	  startrect,
	  limitrect,
	  policy,
	  -1,
	  callback);
  
  /* set the clip for item drags */
  
  if (item != NULL) {
	  vreg = Hv_GetViewRegion(view);
	  rreg = Hv_RectRegion(limitrect);
	  treg = Hv_IntersectRegion(vreg, rreg);
	  Hv_SetClippingRegion(treg);
	  Hv_DestroyRegion(vreg);
	  Hv_DestroyRegion(rreg);
	  Hv_DestroyRegion(treg);
  }
  
  
 	Hv_AlternateEventHandler = Hv_DragHandler;
}

/**
 * Hv_MallocDragData
 * @purpose  Allocate the drag data structure.
 * @param  view       View where dragging is taking place.
 * @param  item       Item being dragged. If NULL, we are not dragging
 *                    an item, but maybe a view or scroll bar thumb.
 * @param  startpp    Mouse point where drag initiated.
 * @param  startrect  Initial location of rectangle
 * @param  limitrect  Confining rectangle
 * @param  policy     If RECTCONFINED, the rect being dragged cannot leave
 *                    the limit rect. If MOUSECONFINED, the mouse cannot
 *                    leave, which means the rect can be dragged off the edge.
 * @param  vertex     Used by polygon enlarge routines
 * @param  callback   Callback when dragging is done.
 */

Hv_DragData Hv_MallocDragData(Hv_View         view,
			      Hv_Item         item,
			      Hv_Point       *startpp,
			      Hv_Rect        *startrect,
			      Hv_Rect        *limitrect,
			      int             policy,
			      int             vertex,
			      Hv_FunctionPtr  callback) {
  
  Hv_DragData dd;
  Hv_Region   connectreg;
  
  dd = (Hv_DragData)Hv_Malloc(sizeof(Hv_DragDataRec));
  
  dd->view      = view;
  dd->item      = item;
  Hv_SetPoint(&(dd->startpp), startpp->x, startpp->y);
  
  Hv_CopyRect(&(dd->startrect), startrect);
  Hv_CopyRect(&(dd->limitrect), limitrect);
  dd->policy    = policy;
  dd->vertex    = vertex;
  dd->callback  = callback;
  dd->itemregion = NULL;
  dd->tempworld = NULL;
  dd->poly = NULL;
  dd->numpts = 0;
	

/* if an item drag, copy its region, including connections */

  if (dd->item != NULL) {
    dd->itemregion = Hv_CopyItemRegion(dd->item, True);    /* original region */
    connectreg = Hv_ConnectionRegion(dd->item);
    if (connectreg != NULL) {
      Hv_AddRegionToRegion(&(dd->itemregion), connectreg);
      Hv_DestroyRegion(connectreg);
    }
    
    Hv_ShrinkRegion(dd->itemregion, -3, -3);   
  }
  
  return dd;
}

/**
 * Hv_FreeDragData
 * @purpose Free memory associated with drag data.
 */

void Hv_FreeDragData(Hv_DragData dd) {
  if (dd == NULL)
    return;
  
  Hv_Free(dd->poly);
 
  Hv_DestroyRegion(dd->itemregion);
  Hv_Free(dd);
  if (dd == thedd)
    thedd = NULL;
}


/**
 * Hv_DragHandler
 * @purpose   This is the alternate event handler called when dragging
 *            is taking place. When dragging is terminated, the event
 *            handler is restored and the callback is called.
 * @param     event    Event pointer.
 * @local
 */

static void Hv_DragHandler(Hv_XEvent *event) {

	static Boolean  firstEntry = True; /* used for initialization */
	static Hv_Rect  mouseLimit;        /* mouse confinement rect */
	static Hv_Rect  currentRect;       /* current rectangle */
	static Hv_Point stop;              /* current point */
	static short    xc, yc;            /* item center for item drag */
	short           dh, dv;            /* offsets */
	Hv_Region       hrr;               /* hot rect region */
	Hv_Region       rreg;
	Hv_Point        osp;               /* used for feedback-while-drag */

	if (event == NULL)
		return;

	if (thedd == NULL) {
		fprintf(stderr, "Serious problem in Hv_DragHandler\n");
		Hv_AlternateEventHandler = NULL;
		firstEntry = True;
		return;
	}


	if (thedd->item == NULL) {
		rreg = Hv_RectRegion(&(thedd->limitrect));
		Hv_SetClippingRegion(rreg);
		Hv_DestroyRegion(rreg);
	}
	else {
		hrr = Hv_ClipHotRect(thedd->view, 0);
		Hv_DestroyRegion(hrr);
		}

/* some first time initialization */

	if (firstEntry) {
		Hv_CopyRect(&mouseLimit, &(thedd->limitrect));
		Hv_InitLimitRect(&mouseLimit);

/* 
 * If the policy is to keep the entire dragrect confined
 * (as opposed to just the mouse) we must adjust the limit rect.
 */
		if (Hv_CheckBit(thedd->policy, Hv_RECTCONFINED)) {
			mouseLimit.left   += (thedd->startpp.x - thedd->startrect.left);
			mouseLimit.top    += (thedd->startpp.y - thedd->startrect.top);
			mouseLimit.width  -= thedd->startrect.width;
			mouseLimit.height -= thedd->startrect.height;
			Hv_FixRectRB(&mouseLimit);
		}


		Hv_SetPoint(&stop, thedd->startpp.x, thedd->startpp.y);

		Hv_CopyRect(&currentRect, &(thedd->startrect));

/* are we gonna show an update string (because shift was pressed)? */

		if (Hv_CheckBit(thedd->policy, Hv_SHIFTPOLICY)) {
			strcpy(thedd->newstr, " 1234, 1234 ");
			strcpy(thedd->oldstr,  " 1234, 1234 ");
            Hv_InitShowCorner(thedd->newstr, &currentRect, &(thedd->strrect));
            Hv_SetSizeString(thedd->newstr,
				 thedd->oldstr,
				 &(thedd->strrect),
				 False,
				 currentRect.left,
				 currentRect.top,
				 0.0);
		}

		Hv_DrawXorRect(&currentRect);

/* item drag? */

		if (thedd->item != NULL)
			Hv_GetItemCenter(thedd->item, &xc, &yc);

		firstEntry = False;
	}

	switch(event->type) {

/* Motion Notify means the mouse is moving */

	case Hv_motionNotify:
		Hv_GetCurrentPoint(&stop,
			   Hv_GetXEventX(event),
			   Hv_GetXEventY(event),
			   &mouseLimit);
	
		dh = stop.x - thedd->startpp.x;
		dv = stop.y - thedd->startpp.y;		

/* see if we have a policy for fixed x or y, used by scroll bars */

		if (Hv_CheckBit(thedd->policy, Hv_FIXEDXPOLICY))
			dh = 0;
		else if (Hv_CheckBit(thedd->policy, Hv_FIXEDYPOLICY))
			dv = 0;

/* dragging an item and providing feedback? */

		if (thedd->item != NULL)
			if (Hv_CheckBit(thedd->item->drawcontrol, Hv_FEEDBACKWHENDRAGGED))
				if (Hv_PointInRect(stop, thedd->view->hotrect))
					if (thedd->view->feedback != NULL) {
						Hv_SetPoint(&osp, stop.x, stop.y); 
						thedd->view->feedback(thedd->view, osp);
						hrr = Hv_ClipHotRect(thedd->view, 0);
						Hv_DestroyRegion(hrr);
					}


		Hv_DrawXorRect(&currentRect);
		Hv_CopyRect(&currentRect, &(thedd->startrect));
		Hv_OffsetRect(&currentRect, dh, dv);
		Hv_DrawXorRect(&currentRect);

/* user may want to do something here */

		if (Hv_DuringRectDrag != NULL)
			Hv_DuringRectDrag(dh, dv);

/* show update? */

		if (Hv_CheckBit(thedd->policy, Hv_SHIFTPOLICY))
			Hv_SetSizeString(thedd->newstr,
			     thedd->oldstr,
			     &(thedd->strrect),
				 False,
				 currentRect.left,
				 currentRect.top,
				 0.0);

		break;

	case Hv_buttonRelease: case Hv_buttonPress:

/* This terminates the drag, cleanup and call the callback */

		Hv_DrawXorRect(&currentRect);

		if (Hv_CheckBit(thedd->policy, Hv_SHIFTPOLICY))
			Hv_UpdateSizeRect(&(thedd->strrect));

		Hv_AlternateEventHandler = NULL;

/* set the final values */

		Hv_SetPoint(&(thedd->finalpp),
			Hv_GetXEventX(event),
			Hv_GetXEventY(event));

		Hv_CopyRect(&(thedd->finalrect), &currentRect);

/* call the callback */

		if (thedd->callback != NULL)
			thedd->callback(thedd);

		Hv_FreeDragData(thedd);
		thedd = NULL;
		firstEntry = True;
		break;
	}
	
}


/**
 * Hv_RubberBandHandler
 * @purpose   This is the alternate event handler called when rubber banding
 *            is taking place. When selecting is terminated, the event
 *            handler is restored and the callback is called.
 * @param     event    Event pointer.
 * @local
 */

static void Hv_RubberBandHandler(Hv_XEvent *event) {

  static Boolean  firstEntry = True; /* used for initialization */
  static Hv_Point stop;              /* current point */
  static Hv_Rect  currentRect;       /* current rectangle */
  Hv_Region   hrr = NULL;
  
  short          delx, dely, del;
  static short          l = 0;
  static short          t = 0;
  static short          r = 0;
  static short          b = 0;
  static Boolean        isoval;  

  if (event == NULL)
    return;
  
  if (thedd == NULL) {
    fprintf(stderr, "Serious problem in Hv_RubberBandHandler\n");
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    return;
  }

  hrr = Hv_ClipHotRect(thedd->view, 0);
  Hv_DestroyRegion(hrr);


/* some first time initialization */

  if (firstEntry) {

    isoval = Hv_CheckBit(thedd->policy, Hv_OVALMETHOD);

/* these policies are used by the xy ploy package */

    if (Hv_CheckBit(thedd->policy, Hv_FIXEDYPOLICY)) {
      t = thedd->limitrect.top;
      b = thedd->limitrect.bottom;
      thedd->startpp.y = t + 1;
    }

    if (Hv_CheckBit(thedd->policy, Hv_FIXEDXPOLICY)) {
      l = thedd->limitrect.left;
      r = thedd->limitrect.right;
      thedd->startpp.x = l + 1;
    }
    
    Hv_InitLimitRect(&(thedd->limitrect));
    Hv_SetPoint(&stop, thedd->startpp.x, thedd->startpp.y);
    Hv_Point2Rect(&currentRect, thedd->startpp, stop);


    if (isoval)
      Hv_DrawXorOval(&currentRect);
    else
      Hv_DrawXorRect(&currentRect);
    firstEntry = False;

  }

  switch(event->type) {

/* Motion Notify means the mouse is moving */

  case Hv_motionNotify:
    if (isoval)
      Hv_DrawXorOval(&currentRect);
    else
      Hv_DrawXorRect(&currentRect);

    Hv_GetCurrentPoint(&stop,
		       Hv_GetXEventX(event),
		       Hv_GetXEventY(event),
		       &(thedd->limitrect));
	
/* see if we have a policy for fixed x or y, used by scroll bars */

    if (Hv_CheckBit(thedd->policy, Hv_FIXEDXPOLICY)) {
      stop.x = r - 1;
    }
    else if (Hv_CheckBit(thedd->policy, Hv_FIXEDYPOLICY)) {
      stop.y = b - 1;
    }
    else if (Hv_CheckBit(thedd->policy, Hv_SQUAREPOLICY)) {
      delx = stop.x -  thedd->startpp.x;
      dely = stop.y -  thedd->startpp.y;
      
      if (thedd->asprat > 1.0) {
	del = (short) ((float) delx / thedd->asprat);
	if (Hv_ssign(delx) == Hv_ssign(dely))
	  dely = del;
	else
	  dely = -del;
	stop.y = thedd->startpp.y + dely;
      }
      else {
	del = (short) ((float) dely * thedd->asprat);
	if (Hv_ssign(delx) == Hv_ssign(dely))
	  delx = del;
	else
	  delx = -del;
	stop.x = thedd->startpp.x + delx;
      }
      
      Hv_GetCurrentPoint(&stop,
			 (int)(stop.x),
			 (int)(stop.y),
			 &(thedd->limitrect));
    }
    
    Hv_Point2Rect(&currentRect, thedd->startpp, stop);

    if (isoval)
      Hv_DrawXorOval(&currentRect);
    else
      Hv_DrawXorRect(&currentRect);

    break;

  case Hv_buttonRelease: case Hv_buttonPress:

/* Final xor rect/oval */
      
    if (isoval)
      Hv_DrawXorOval(&currentRect);
    else
      Hv_DrawXorRect(&currentRect);
		
/* for a fixed y/x policy, make sure no y/x zoom */
		
    if (Hv_CheckBit(thedd->policy, Hv_FIXEDYPOLICY)) {
      currentRect.top = t;
      currentRect.bottom = b;
      Hv_FixRectWH(&currentRect);
    }
		
    else if (Hv_CheckBit(thedd->policy, Hv_FIXEDXPOLICY)) {
      currentRect.left = l;
      currentRect.right = r;
      Hv_FixRectWH(&currentRect);
    }

    Hv_CopyRect(&(thedd->finalrect), &currentRect);

/* call the callback */

    if (thedd->callback != NULL)
      thedd->callback(thedd);
    
    Hv_FreeDragData(thedd);
    thedd = NULL;
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    break;
  }
  
}

/**
 * Hv_PolySelectHandler
 * @purpose   This is the alternate event handler called when selecting a
 *            polygom. When selecting is terminated, the event
 *            handler is restored and the callback is called.
 * @param     event    Event pointer.
 * @local
 */

static void Hv_PolySelectHandler(Hv_XEvent *event) {

  static Boolean  firstEntry = True; /* used for initialization */
  static Hv_Point current;           /* current point */
  static Hv_Point prev;              /* previous point */
  Hv_Region       hrr = NULL;        /* hot rect region */
  static short    npts;              /* number of points */
  Boolean         mapview;
  Boolean         userok;
  Boolean         done;
  int             i;

  static Hv_Point  temppoly[Hv_MAXPOLYPNTS];
  
  if (event == NULL)
    return;
  
  if (thedd == NULL) {
    fprintf(stderr, "Serious problem in Hv_PolySelectHandler\n");
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    npts = 0;
    return;
  }

  hrr = Hv_ClipHotRect(thedd->view, 0);
  mapview = Hv_IsMapView(thedd->view);
  done = False;

/* some first time initialization */

  if (firstEntry) {
    Hv_InitLimitRect(&(thedd->limitrect));
    Hv_SetPoint(&current, thedd->startpp.x, thedd->startpp.y);
    Hv_SetPoint(&prev,    thedd->startpp.x, thedd->startpp.y);

/* first point of poly is the original click */

    temppoly[0] = current;
    npts = 1;
    firstEntry = False;
  }

  switch(event->type) {

/* Motion Notify means the mouse is moving */

  case Hv_motionNotify:
    Hv_GetCurrentPoint(&current,
		       Hv_GetXEventX(event),
		       Hv_GetXEventY(event),
		       &(thedd->limitrect));

    if (mapview && (thedd->view->feedback))
      thedd->view->feedback(thedd->view, current);

/* see if there is a user check here.
   Trick it into using an extra point in the poly */
	
    if (Hv_UserLineEnlargeCheck != NULL) {
      temppoly[npts] = current;
      Hv_UserLineEnlargeCheck(temppoly,
			      npts+1,
			      &userok);
    }
    else
      userok = True;
	
    if (!userok)
      current = prev;

    Hv_SetClippingRegion(hrr);
    Hv_DrawXorLine(&(temppoly[npts-1]), &prev);
    Hv_DrawXorLine(&(temppoly[npts-1]), &current);
    prev = current;
    break;
    
  case Hv_buttonPress:
    Hv_GetCurrentPoint(&current,
		       Hv_GetXEventX(event),
		       Hv_GetXEventY(event),
		       &(thedd->limitrect));

    temppoly[npts] = current;
    npts++;
    
/* see if there is a user check here */

    if (Hv_UserLineEnlargeCheck != NULL) {
      Hv_UserLineEnlargeCheck(temppoly,
			      npts,
			      &userok);
      Hv_SetClippingRegion(hrr);
    }
    else
      userok = True;
	
    done = ((Hv_ClickType(NULL,
			  Hv_GetXEventButton(event)) == 2) ||
	    (npts >= Hv_MAXPOLYPNTS));

    if (!userok) {
      if (done)
	temppoly[npts - 1] = prev;
      else
	npts--;
    }
    
    break;
  }

  if (done) {
    Hv_SetClippingRegion(hrr);
    for (i = 1; i < npts; i++)
      Hv_DrawXorLine(&(temppoly[i-1]), &(temppoly[i]));
    
    if (npts > 1) {
      thedd->numpts = npts;
			Hv_Println("NUMBER OF PNTS: %d", thedd->numpts);
      thedd->poly = Hv_NewPoints(npts);
      for (i = 0; i < npts; i++) {
	(thedd->poly)[i].x = temppoly[i].x;
	(thedd->poly)[i].y = temppoly[i].y;
      }
    }
    else {
      thedd->numpts = 0;
      thedd->poly = NULL;
    }
    
    Hv_FullClip();
    Hv_DestroyRegion(hrr);

/* call the callback */

    if (thedd->callback != NULL)
      thedd->callback(thedd);
    
    Hv_FreeDragData(thedd);
    thedd = NULL;
 
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    npts = 0;
  } /* end (done) */
}

#undef   Hv_MAXPOLYPNTS



