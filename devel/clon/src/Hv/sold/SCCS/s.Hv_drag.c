h29612
s 00000/00000/00000
d R 1.2 02/08/25 23:21:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_drag.c
e
s 01915/00000/00000
d D 1.1 02/08/25 23:21:08 boiarino 1 0
c date and time created 02/08/25 23:21:08 by boiarino
e
u
U
f e 0
t
T
I 1
/*
-----------------------------------------------------------------------
File:     Hv_drag.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains various functions related to dragging
   items  */

#include "Hv.h"	  /* contains all necessary include files */
#include "Hv_maps.h"

/*------  local prototypes ----------*/


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


/* ------------ Hv_DragRect -------------*/

void   Hv_DragRect(Hv_Rect  *TheDragRect,
		   Hv_Rect  *LimRect,
		   Hv_Point  StartPP,
		   Hv_Point *StopPP,
		   short     DragPolicy,
		   Boolean   ShowCorner,
		   Boolean   Confine)

/* drag a rectangle around the screen, staying within LimitRect.
   Uses the first mouse button.

Hv_Rect  *TheDragRect  Original Position of rectangle to be dragged  (NOT CHANGED)
Hv_Rect  *LimRect      Limit rect: no part of dragrect can go out of here (NOT CHANGED)
Hv_Point  StartPP      Starting position of mouse when rect is being dragged
Hv_Point *StopPP       Final location of mouse when dragging is done
short     DragPolicy   One of the recognized drag policies
Boolean   ShowCorner   If true, show top-left while dragging
Boolean   Confine      if true, keep entire rect within limitrect, else just mouse

*/

{
  Hv_Rect        LimitRect;     /* Mouse must stay in here to keep TheDragRect in LimitRect*/
  Boolean        done;          /* Controls when dragging is over */
  XEvent         Event;         /* An X event */
  Hv_Rect        crect;         /* current rect */
  short          dh, dv;        /* offests */
  char          *newstr;
  char          *oldstr;
  unsigned long  oldfg, oldbg;
  Hv_Rect        srect;         /* for displaying the Corner of the dragrect */
  short          xc, yc;
  Hv_Point         osp;
  Hv_Region      hrr;

 /* for right-bottom ambiguities, it is practical to shrink the limit rect a little */

  newstr = (char *)Hv_Malloc(30);
  oldstr  = (char *)Hv_Malloc(30);
  strcpy(newstr, " 1234, 1234 ");
  strcpy(oldstr,  " 1234, 1234 ");

  Hv_CopyRect(&LimitRect, LimRect);
  Hv_InitLimitRect(&LimitRect);

/* first get the limit rect for the mouse. The MOUSE moving out of this rectangle implies
   the DragRect has moved out of the LimitRect.*/

  if (Confine) {
    LimitRect.left   += (StartPP.x - TheDragRect->left);
    LimitRect.top    +=  (StartPP.y - TheDragRect->top);
    LimitRect.width  -= TheDragRect->width;
    LimitRect.height -= TheDragRect->height;
    Hv_FixRectRB(&LimitRect);
  }

/* initialize */

  oldfg = Hv_SwapForeground(Hv_black);    /* swap & store the old foregound color */
  oldbg = Hv_SwapBackground(Hv_white);    /* swap & store the old backgound color */

  *StopPP = StartPP;
  done = False;
  Hv_CopyRect(&crect, TheDragRect);

  if (ShowCorner) {
    Hv_InitShowCorner(newstr, &crect, &srect);
    Hv_SetSizeString(newstr, oldstr, &srect, False, crect.left, crect.top, 0.0);
  }

  if (Hv_hotItem != NULL)
    Hv_GetItemCenter(Hv_hotItem, &xc, &yc);

  Hv_DrawXorRect(&crect);

  while (!done) {

/* if Check For Motion or Button Releases */

      if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | Button1MotionMask, &Event)) {

	switch(Event.type) {

/* Motion Notify means the mouse is moving */

	case MotionNotify: {

	  Hv_GetCurrentPoint(StopPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	  dh = StopPP->x - StartPP.x;
	  dv = StopPP->y - StartPP.y;

	  switch (DragPolicy) {
	    
	  case Hv_NODRAGPOLICY: 
	    break;

	  case Hv_FIXEDXPOLICY: 
	    dh = 0;
	    break;
	    
	  case Hv_FIXEDYPOLICY: 
	    dv = 0;
	    break;
	    
	  default:
	    dh = 0;
	    dv = 0;
	    break;
	  }

	  if (Hv_hotItem != NULL)
	    if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_FEEDBACKWHENDRAGGED))
	      if (Hv_PointInRect(*StopPP, Hv_hotView->hotrect))
		if (Hv_hotView->feedback != NULL) {
		  Hv_SetPoint(&osp, StopPP->x, StopPP->y); 
		  Hv_hotView->feedback(Hv_hotView, osp);
		  hrr = Hv_ClipHotRect(Hv_hotView, 0);
		  Hv_DestroyRegion(hrr);
		}

	  Hv_DrawXorRect(&crect);
	  Hv_CopyRect(&crect, TheDragRect);
	  Hv_OffsetRect(&crect, dh, dv);
	  Hv_DrawXorRect(&crect);

/* user may want to do something here */

	  if (Hv_DuringRectDrag != NULL)
	    Hv_DuringRectDrag(dh, dv);

	  break;
	}  /* end of case MotionNotify */
	      
/* if button 1 released, we are done */

	case ButtonRelease:
	  if (Event.xbutton.button == 1) {	      
	    done = True;
	    break;
	  }
	      
	default:
	  break;
	}

      }  /* end  if if CheckMask */


      if (ShowCorner)
	Hv_SetSizeString(newstr, oldstr, &srect, False, crect.left, crect.top, 0.0);
    }   /*end of while !done */

/* Final xor rect */
      
  Hv_DrawXorRect(&crect);

  if (ShowCorner)
    Hv_UpdateSizeRect(&srect);

  Hv_SetForeground(oldfg);         /* resets the foreground to the original value */
  Hv_SetBackground(oldbg);         /* resets the background to the original value */

  Hv_Free(newstr);
  Hv_Free(oldstr);
}

/* ------------ Hv_SelectPolygon -------------*/

#define   Hv_MAXPOLYPNTS    100

void   Hv_SelectPolygon(Hv_Point **poly,
			short     *npts,
			Hv_Rect    LimitRect,
			Hv_Point   StartPP)

/* selects a polygon around the screen, staying within LimitRect.
   Uses the  button specified.

   Hv_Point   **poly;      polygon selected
   short      *npts;       points in poly
   Hv_Rect    LimitRect;   Limit rect -- no part of drag rect can go out of here
   Hv_Point   StartPP;     Starting position of mouse when rect is being dragged
*/

{
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */
  Hv_Point       CurrentPP;             /* Current mouse position*/
  Hv_Point      *temppoly;
  int            i;
  Hv_Point       PrevPP;
  Hv_Region      hrr = NULL;
  Boolean        userok = True;    

  Hv_InitLimitRect(&LimitRect);
  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  XGrabPointer(Hv_display,
	       Hv_mainWindow,
	       True,
	       0,
	       GrabModeAsync,
	       GrabModeAsync,
	       None,
	       None,
	       CurrentTime);

  temppoly = Hv_NewPoints(Hv_MAXPOLYPNTS);
  done = False;
  CurrentPP = StartPP;
  PrevPP = StartPP;

/* first point of poly is the original click */

  *npts = 0;
  temppoly[*npts] = CurrentPP;
  (*npts)++;

  while (!done) {

/* if Check For Motion or Button Press */

    if (XCheckMaskEvent(Hv_display, ButtonPressMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 
	Hv_GetCurrentPoint(&(CurrentPP), Event.xbutton.x, Event.xbutton.y, &LimitRect);

	if (Hv_feedbackWhileSelecting && (Hv_hotView->feedback))
	  Hv_hotView->feedback(Hv_hotView, CurrentPP);

/* see if there is a user check here.
   Trick it into using an extra point in the poly */
	
	if (Hv_UserLineEnlargeCheck != NULL) {
	  temppoly[*npts] = CurrentPP;
	  Hv_UserLineEnlargeCheck(temppoly,
				  *npts+1,
				  &userok);
	}
	else
	  userok = True;
	
	if (!userok)
	  CurrentPP = PrevPP;

	Hv_SetClippingRegion(hrr);
	Hv_DrawXorLine(&(temppoly[(*npts)-1]), &PrevPP);
	Hv_DrawXorLine(&(temppoly[(*npts)-1]), &CurrentPP);
	PrevPP = CurrentPP;
	break;


      case ButtonPress:
	Hv_GetCurrentPoint(&CurrentPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);

	temppoly[*npts] = CurrentPP;
	(*npts)++;

/* see if there is a user check here */

	if (Hv_UserLineEnlargeCheck != NULL) {
	  Hv_UserLineEnlargeCheck(temppoly,
				  *npts,
				  &userok);
	  Hv_SetClippingRegion(hrr);
	}
	else
	  userok = True;
	
	done = ((Hv_ClickType(NULL, Event.xbutton.button) == 2) ||
		((*npts) >= Hv_MAXPOLYPNTS));

	if (!userok) {
	  if (done)
	    temppoly[(*npts) - 1] = PrevPP;
	  else
	    (*npts)--;
	}
	
	if (done)
	  Hv_FullClip();
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


  Hv_SetClippingRegion(hrr);
  for (i = 1; i < *npts; i++)
    Hv_DrawXorLine(&(temppoly[i-1]), &(temppoly[i]));

  if (*npts > 1) {
    *poly = Hv_NewPoints(*npts);
    for (i = 0; i < *npts; i++) {
      (*poly)[i].x = temppoly[i].x;
      (*poly)[i].y = temppoly[i].y;
    }
  }
  else {
    *npts = 0;
    *poly = NULL;
  }

  Hv_Free(temppoly);

  Hv_DestroyRegion(hrr);
  XUngrabPointer(Hv_display, CurrentTime);

}

#undef   Hv_MAXPOLYPNTS



/* ------------ Hv_SelectWedge -------------*/


void   Hv_SelectWedge(Hv_View    View,
		      Hv_Rect    LimitRect,
		      Hv_Point   StartPP,
		      Hv_FPoint  *basepoint,
		      float      *azim1,
		      float      *azim2,
		      float      *radius,
		      int        *direction)


/* selects a polygon around the screen, staying within LimitRect.
   Uses the  button specified.

   Hv_Point   **poly;      polygon selected
   short      *npts;       points in poly
   Hv_Rect    LimitRect;   Limit rect -- no part of drag rect can go out of here
   Hv_Point   StartPP;     Starting position of mouse when rect is being dragged
*/

{
  short          npts; 
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */
  Hv_FPoint     *fp;
  Hv_Point      *poly = NULL;
  Hv_Point       cp, pp;
  int            nclick = 1;
  Hv_Region      hrr = NULL;

  Hv_Map         map = NULL;
  Boolean        first = True;
  Boolean        mapview;

  Hv_InitLimitRect(&LimitRect);
  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  XGrabPointer(Hv_display,
	       Hv_mainWindow,
	       True,
	       0,
	       GrabModeAsync,
	       GrabModeAsync,
	       None,
	       None,
	       CurrentTime);


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

  cp = StartPP;
  pp = StartPP;
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

    if (XCheckMaskEvent(Hv_display, ButtonPressMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: case ButtonPress: 
	Hv_GetCurrentPoint(&cp, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	

/* If haven't had 2nd click we are getting the
   radius else determining arc */

	if (nclick == 1) {
	  Hv_LocalToWorld(View, &(fp[1].h),
			  &(fp[1].v),
			  cp.x, cp.y);

	  Hv_DrawXorLine(&(StartPP), &pp);
	  Hv_DrawXorLine(&(StartPP), &cp);
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
	    if (Event.type != ButtonPress)
	      Hv_FrameXorPolygon(poly, npts);
	  }
	  else {
	    if (Event.type != ButtonPress) {
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

	if (Event.type == ButtonPress) {
	  nclick++;
	  
	  if (nclick == 2) {
	    Hv_DrawXorLine(&(StartPP), &pp);
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
  XUngrabPointer(Hv_display, CurrentTime);

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


  a1 = atan2(dy1, dx1);
  a2 = atan2(dy2, dx2);
  
  newang = a2 - a1;

/* have to fix wrap problem */

  if (fabs(newang-openang) > 1.0) {
    if (newang < openang)
      newang += Hv_TWOPI;
    else
      newang -= Hv_TWOPI;
  }

  openang = newang;

  if (openang < -Hv_TWOPI)
    openang += Hv_TWOPI;
  else if (openang > Hv_TWOPI)
    openang -= Hv_TWOPI;


/* look for a change from the master direction if small opening ang */

  if (fabs(openang) < 0.5) {
    crossproduct = dx1*dy2 - dx2*dy1;
    *direction = (crossproduct < 0.0) ? Hv_CLOCKWISE : Hv_COUNTERCLOCKWISE;
/*    fprintf(stderr, " MASTER DIRECTION: %d   CP = %f\n", *direction, crossproduct);*/
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
      
      *ang1 = atan2(dy1, dx1);
      *ang2 = atan2(dy2, dx2);

      if (*ang1 < 0.0)
	*ang1 += Hv_TWOPI;

      if (*ang2 < 0.0)
	*ang2 += Hv_TWOPI;

      *radius = sqrt(dx1*dx1 + dy1*dy1);
    }

    endang = *ang2;
    
    if (*direction == Hv_COUNTERCLOCKWISE) {
      if (*ang2 > *ang1) {
	endang -= Hv_TWOPI;
      }
    }
    else {
      if (*ang2 < *ang1) {
	endang += Hv_TWOPI;
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
	dx = (*radius)*cos(ang);
	dy = (*radius)*sin(ang);
	fp[i].h = fp[0].h + dx;
	fp[i].v = fp[0].v + dy;
      }
    }
 
}



/* ------------ Hv_SelectRect -------------*/

void   Hv_SelectRect(Hv_Rect  *TheSelectedRect,
		     Hv_Rect   LimitRect,
		     Hv_Point  StartPP,
		     short     WhatButton,
		     short     SelectPolicy,
		     float     asprat)

/* selects a rectangle around the screen, staying within LimitRect.
   Uses the  button specified.

Hv_Rect  *TheSelectedRect Rectangle selected
Hv_Rect   LimitRect       Limit rect -- no part of drag rect can go out of here
Hv_Point  StartPP         Starting position of mouse when rect is being dragged
short     WhatButton      Looks for a release of this button to terminate selection
short     SelectPolicy    determines what policy is used for selecting
float     asprat          hotrect aspect ratio (w/h) needed for SQUARE selections

*/

{
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */
  Hv_Point       CurrentPP;             /* Current mouse position*/
  short          delx, dely, del;
  short          l = 0;
  short          t = 0;
  short          r = 0;
  short          b = 0;

/* for a fixed y/x policy, we need to set the starting
   point's y/x to be at the top/left of the plot */

  if (SelectPolicy == Hv_FIXEDYPOLICY) {
    t = LimitRect.top;
    b = LimitRect.bottom;
    StartPP.y = t + 1;
  }

  else if (SelectPolicy == Hv_FIXEDXPOLICY) {
    l = LimitRect.left;
    r = LimitRect.right;
    StartPP.x = l + 1;
  }

  Hv_InitLimitRect(&LimitRect);

  done = False;
  CurrentPP = StartPP;

  Hv_Point2Rect(TheSelectedRect, StartPP, CurrentPP);

  Hv_DrawXorRect(TheSelectedRect);

  while (!done) {

/* if Check For Motion or Button Releases */

    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 
	Hv_DrawXorRect(TheSelectedRect);
	Hv_GetCurrentPoint(&CurrentPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);

	switch (SelectPolicy) {
	case Hv_SQUAREPOLICY:
	  delx = CurrentPP.x -  StartPP.x;
	  dely = CurrentPP.y -  StartPP.y;

	  if (asprat > 1.0) {
	    del = (short) ((float) delx / asprat);
	    if (Hv_ssign(delx) == Hv_ssign(dely))
	      dely = del;
	    else
	      dely = -del;
	    CurrentPP.y = StartPP.y + dely;
	  }
	  else {
	    del = (short) ((float) dely * asprat);
	    if (Hv_ssign(delx) == Hv_ssign(dely))
	      delx = del;
	    else
	      delx = -del;
	    CurrentPP.x = StartPP.x + delx;
	  }

	  Hv_GetCurrentPoint(&CurrentPP,
			     (int)(CurrentPP.x),
			     (int)(CurrentPP.y),
			     &LimitRect);
	  break;

	case Hv_FIXEDYPOLICY:
	  CurrentPP.y = b - 1;
	  break;

	case Hv_FIXEDXPOLICY:
	  CurrentPP.x = r - 1;
	  break;

	default:
	  break;
	}  /* end switch on selectpolicy */

	Hv_Point2Rect(TheSelectedRect, StartPP, CurrentPP);
	Hv_DrawXorRect(TheSelectedRect);
	break;

     /* end of case MotionNotify */
	      
/* if button WhatButton is released, we are done */

      case ButtonRelease:
	if (Event.xbutton.button == WhatButton) {	      
	  done = True;
	  break;
	}
	      
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor rect */
      
  Hv_DrawXorRect(TheSelectedRect);


/* for a fixed y/x policy, make sure no y/x zoom */

  if (SelectPolicy == Hv_FIXEDYPOLICY) {
    TheSelectedRect->top = t;
    TheSelectedRect->bottom = b;
    Hv_FixRectWH(TheSelectedRect);
  }

  if (SelectPolicy == Hv_FIXEDXPOLICY) {
    TheSelectedRect->left = l;
    TheSelectedRect->right = r;
    Hv_FixRectWH(TheSelectedRect);
  }

}

/* ------------ Hv_SelectOval -------------*/

void   Hv_SelectOval(Hv_Rect   *TheSelectedRect,
		     Hv_Rect    LimitRect,
		     Hv_Point   StartPP,
		     short      WhatButton)

/* selects a rectangle around the screen, staying within LimitRect.
   Uses the  button specified.

Hv_Rect *TheSelectedRect;  Rectangle selected
Hv_Rect  LimitRect;        Limit rect -- no part of drag rect can go out of here
Hv_Point StartPP;          Starting position of mouse when rect is being dragged
short    WhatButton;       Looks for a release of this button to terminate selection

*/

{
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */
  Hv_Point       CurrentPP;             /* Current mouse position*/

  Hv_InitLimitRect(&LimitRect);

  done = False;
  CurrentPP = StartPP;
  Hv_Point2Rect(TheSelectedRect, StartPP, CurrentPP);

  Hv_DrawXorOval(TheSelectedRect);

  while (!done) {

/* if Check For Motion or Button Releases */

    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 
	Hv_DrawXorOval(TheSelectedRect);
	Hv_GetCurrentPoint(&CurrentPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	Hv_Point2Rect(TheSelectedRect, StartPP, CurrentPP);
	Hv_DrawXorOval(TheSelectedRect);
	break;

     /* end of case MotionNotify */
	      
/* if button WhatButton is released, we are done */

      case ButtonRelease:
	if (Event.xbutton.button == WhatButton) {	      
	  done = True;
	  break;
	}
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor oval */
      
  Hv_DrawXorOval(TheSelectedRect);

}

/* ------------ Hv_EnlargeLine ----------*/

void Hv_EnlargeLine(Hv_Rect   LimitRect,
		    Hv_Point  StartPP,
		    Hv_Point *fixedpnts,
		    short     nfp,
		    Hv_Point *varpnt)

/*****************************************

Permits a line to be enlarged

Hv_Rect   LimitRect
Hv_Point  StartPP   starting mouse point
Hv_Point *fixedpnts anchored points on line/
short     nfp
Hv_Point *varpnt    point on line that is changing; not exactly StartPP because of slop
*****************************************/

/*---- private trick: if nfp < 0 call feedback while resizing */


{
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */
  short          dh, dv, oldx, oldy;
  int            i;
  Hv_Point       CurrentPP;
  Boolean        fback = False;
  Hv_Region      hrr = NULL;
  Boolean        userok = True;    

  Hv_InitLimitRect(&LimitRect);
  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  XGrabPointer(Hv_display, Hv_mainWindow, True, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  oldx = StartPP.x;
  oldy = StartPP.y;

  done = False;

  if (nfp < 0) {
    fback = True;
    nfp = -nfp;
  }

  for (i = 0; i < nfp; i++)
    Hv_DrawXorLine(fixedpnts+i, varpnt);

  while (!done) {

/* if Check For Motion or Button Releases */

    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonPressMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 

	Hv_SetClippingRegion(hrr);
	for (i = 0; i < nfp; i++)
	  Hv_DrawXorLine(fixedpnts+i, varpnt);

	Hv_GetCurrentPoint(&CurrentPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	
	if (fback && (Hv_hotView->feedback))
	  Hv_hotView->feedback(Hv_hotView, CurrentPP);
	
	dh = CurrentPP.x - oldx;
	dv = CurrentPP.y - oldy;
	
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
	  oldx = CurrentPP.x;
	  oldy = CurrentPP.y;
	}

	Hv_SetClippingRegion(hrr);
	for (i = 0; i < nfp; i++)
	  Hv_DrawXorLine(fixedpnts+i, varpnt);
	
	break;

     /* end of case MotionNotify */
	      
/* if button WhatButton is released, we are done */

      case ButtonRelease:
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

  XUngrabPointer(Hv_display, CurrentTime);
  Hv_DestroyRegion(hrr);
}


/* ------------ Hv_ConnectLine ----------*/

void Hv_ConnectLine(Hv_Rect   LimitRect,
		    Hv_Point  StartPP,
		    Hv_Point *StopPP)

/* slightly different protocol when connecting two things:
   click on one, relase, move to the other, click */

{
  Boolean        done;                  /* Controls when dragging is over */
  XEvent         Event;                 /* An X event */

  Hv_InitLimitRect(&LimitRect);

  XGrabPointer(Hv_display, Hv_mainWindow, True, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  *StopPP = StartPP;
  done = False;

  Hv_DrawXorLine(&StartPP, StopPP);

  while (!done) {

/* if Check For Motion or Button Presses */

    if (XCheckMaskEvent(Hv_display, ButtonPressMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 
	Hv_DrawXorLine(&StartPP, StopPP);
	Hv_GetCurrentPoint(StopPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	Hv_DrawXorLine(&StartPP, StopPP);
	break;

     /* end of case MotionNotify */
	      
/* if button WhatButton is released, we are done */

      case ButtonPress:
	done  = True;
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor line */

  Hv_DrawXorLine(&StartPP, StopPP);
  XUngrabPointer(Hv_display, CurrentTime);
}


/* ------------ Hv_CreateLine -------------*/

void   Hv_CreateLine(Hv_Rect   LimitRect,
		     Hv_Point  StartPP,
		     Hv_Point *StopPP)

/* selects a rectangle around the screen, staying within LimitRect.
   Uses the  button specified. */

{
  Hv_Point  fixedpnt;

  fixedpnt = StartPP;
  *StopPP = StartPP;

  Hv_EnlargeLine(LimitRect, StartPP, &fixedpnt, 1, StopPP);
  return;
}


/* ------------ Hv_EnlageRect -------------*/

void   Hv_EnlargeRect(Hv_Rect *TheGrowRect,
		      Hv_Rect  LimitRect,
		      Hv_Point StartPP,
		      Boolean  ShowSize)

/*
Hv_Rect  *TheGrowRect; Original Position of rectangle to be grown  (CHANGED!!)
Hv_Rect   LimitRect;   Limit rect -- no part of drag rect can go out of here
Hv_Point  StartPP;     Starting position of mouse when rect is being dragged
Boolean   ShowSize;    if true, show the size (W, H) of the rect as it is grown
*/

{
  Hv_EnlargeRectOrOval(TheGrowRect, LimitRect, StartPP, ShowSize, False);
}

/* ------------ Hv_EnlageOval -------------*/

void   Hv_EnlargeOval(Hv_Rect  *TheGrowRect,
		      Hv_Rect   LimitRect,
		      Hv_Point  StartPP,
		      Boolean   ShowSize)

/*
Hv_Rect   *TheGrowRect;  Original Position of rectangle to be grown  (CHANGED!!)
Hv_Rect   LimitRect;     Limit rect -- no part of drag rect can go out of here
Hv_Point  StartPP;       Starting position of mouse when rect is being dragged
Boolean   ShowSize;      if true, show the size (W, H) of the rect as it is grown
*/

{
  Hv_EnlargeRectOrOval(TheGrowRect, LimitRect, StartPP, ShowSize, True);
}


/* ------------ Hv_EnlageRectOrOval -------------*/

static void   Hv_EnlargeRectOrOval(Hv_Rect *TheGrowRect,
				   Hv_Rect  LimitRect,
				   Hv_Point StartPP,
				   Boolean  ShowSize,
				   Boolean  IsOval)

/* selects a rectangle around the screen, staying within LimitRect.
   Uses the  button specified. (Replacement for Hv_GrowRect)

Hv_Rect  *TheGrowRect      Original Position of rectangle to be grown  (CHANGED!!) 
Hv_Rect  LimitRect         Limit rect -- no part of drag rect can go out of here 
Hv_Point StartPP           Starting position of mouse when rect is being dragged 
Boolean  ShowSize          if true, show the size (W, H) of the rect as it is grown 
Boolean  IsOval            if true, this is an oval

*/

{
  Boolean     done;             /* Controls when dragging is over */
  XEvent      Event;            /* An X event */
  Hv_Rect     crect;            /* current rect */
  short       dh, dv;           /* offests */
  Hv_Rect     srect;            /* for displaying the size of the growrect */

  char             *newstr = " 1234, 1234 ";
  char             *oldstr =  " XXXX, XXXX ";
  short            grabcorner;  /* which corner we are grabbing */
  Hv_Point         CurrentPP;
  

  grabcorner = Hv_WhichPointOnRect(StartPP.x, StartPP.y, TheGrowRect);
  Hv_InitLimitRect(&LimitRect);

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

    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | Button1MotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: {
	Hv_GetCurrentPoint(&CurrentPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);
	dh = CurrentPP.x - StartPP.x;
	dv = CurrentPP.y - StartPP.y;


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
      }  /* end of case MotionNotify */
	      
/* if button 1 released, we are done */

      case ButtonRelease:
	if (Event.xbutton.button == 1) {	      
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



/*---------- Hv_DragItem -------*/

void Hv_DragItem(Hv_Item  Item,
		 Hv_Rect  *LimitRect, 
		 Hv_Point StartPP,
		 Boolean  ShowCorner)

{
  Hv_View     View = Hv_GetItemView(Item);
  Hv_Point    StopPP;
  short       dh, dv;
  Hv_Region   udr1, udr2, udr3;
  Boolean     RotateIt = False;
  Boolean     FancyDrag = False;
  Boolean     enlarged;
  Hv_Region   connectreg;

/* if a balloon is up, take it down */

  if (Hv_balloonUp)
    Hv_HideBalloon();

/* bring the view to the front */

  Hv_SendViewToFront(View);

/* area we actually enlarging? */

  if ((Hv_CheckBit(Item->drawcontrol, Hv_ENLARGEABLE)) && (Item->checkenlarge != NULL)) {
    Item->checkenlarge(Item, StartPP, ShowCorner, &enlarged);
    if (enlarged)
      return;
  }
  
/* are we actually rotating? */

  if (Hv_CheckBit(Item->drawcontrol, Hv_ROTATABLE) && Hv_PointInRect(StartPP, Item->rotaterect)) {
    View->lastrotateitem = Item;
    View->lastazimuth = Item->azimuth;
    RotateIt = True;
  }


  if (!RotateIt) {
    if (!Hv_CheckBit(Item->drawcontrol, Hv_DRAGGABLE))
      return;
    View->lastdragitem = Item;
  }

/* Fancy drag ? */

  FancyDrag = Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_FANCYDRAG);

  Hv_completeFeedback = False;
  Hv_extraFastPlease = FancyDrag || RotateIt;

  udr1 = Hv_CopyItemRegion(Item, True);    /* original region */
  connectreg = Hv_ConnectionRegion(Item);
  if (connectreg != NULL) {
    Hv_AddRegionToRegion(&udr1, connectreg);
    Hv_DestroyRegion(connectreg);
    connectreg = NULL;
  }

  Hv_ShrinkRegion(udr1, -2, -2); 

  udr2 = Hv_ClipHotRect(View, 0);

  if (RotateIt || FancyDrag) {
    Hv_FancyDragItem(Item, LimitRect, StartPP, &StopPP, RotateIt, ShowCorner);
    dh = StopPP.x - StartPP.x;
    dv = StopPP.y - StartPP.y;
    Hv_extraFastPlease = False;
    
    if (!RotateIt) {
      if (Item->afterdrag != NULL)
	Item->afterdrag(Item, 0, 0);
      Hv_SetPoint(&(View->lastdragdelta), -dh, -dv);
    }

  }
  else /* normal drag */ {
    Hv_DragRect(Item->area, LimitRect, StartPP, &StopPP, Hv_NODRAGPOLICY, ShowCorner, False);

/* set the offset values */
	      
    dh = StopPP.x - StartPP.x;
    dv = StopPP.y - StartPP.y;
    
    Hv_offsetDueToDrag = True ;
    Hv_OffsetItem(Item, dh, dv, True);
    Hv_offsetDueToDrag = False;
    
/* if this is a hotrect object, we have to fix world rect */

    Hv_FixItemWorldArea(Item, True);
    
/* specialized after drag callback? */

    if (Item->afterdrag != NULL)
      Item->afterdrag(Item, dh, dv);

    Hv_SetPoint(&(View->lastdragdelta), -dh, -dv);

  }  /* end normal drag */

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
      
      Hv_ShrinkRegion(udr2, -2, -2); 
      
      udr3 = Hv_UnionRegion(udr2, udr1);
      Hv_extraFastPlease = False;
      Hv_DrawView(View, udr3);   
      
      Hv_DestroyRegion(udr2);
      Hv_DestroyRegion(udr3);
    } /* dh or dv nonzero */
  
  Hv_DestroyRegion(udr1); 
}


/* ------------ Hv_FancyDragItem -------------*/

static void   Hv_FancyDragItem(Hv_Item  Item,
			       Hv_Rect  *LimRect,
			       Hv_Point StartPP,
			       Hv_Point *StopPP,
			       Boolean  RotateIt,
			       Boolean  ShowCorner)

/* drag an item around the screen, staying within LimitRect.
   Uses the first mouse button.

Hv_Item   Item;
Hv_Rect  *LimRect;       Limit rect: no part of drag rect can go out (NOT CHANGED )
Hv_Point  StartPP;       Starting position of mouse when rect is being dragged
Hv_Point *StopPP;        Final location of mouse when dragging is done 
Boolean   RotateIt;      we are rotating, not dragging
Boolean   ShowCorner;    If true, show top-left while dragging

*/

{
  Hv_Rect        TheDragRect;
  Hv_View        View = Hv_GetItemView(Item);
  Hv_Rect        LimitRect;           /* Mouse must stay in here to keep TheDragRect in LimitRect*/
  Boolean        done = False;        /* Controls when dragging is over */
  XEvent         Event;               /* An X event */
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

  int            i;
  float          az, daz, gcd;

/* we handle the rotation of world polys */

  Hv_WorldPolygonData  *wpoly;
  float                fx, fy;

  newstr = (char *)Hv_Malloc(30);
  oldstr  = (char *)Hv_Malloc(30);
  strcpy(newstr, " 1234, 1234 ");
  strcpy(oldstr,  " 1234, 1234 ");

  Hv_CopyRect(&TheDragRect, Item->area);

/* If no reference point, use the center of the area */

  if (Item->rotaterp == NULL)
      Hv_GetRectCenter(Item->area, &(refpp.x), &(refpp.y));
  else
      Hv_SetPoint(&refpp, Item->rotaterp->x, Item->rotaterp->y);

  ldr = Hv_ClipView(View);

/* for right-bottom ambiguities, it is practical to shrink the limit rect a little */

  Hv_CopyRect(&LimitRect, LimRect);
  Hv_InitLimitRect(&LimitRect);

/* initialize */

  *StopPP = StartPP;

  Hv_CopyRect(&crect, &TheDragRect);

  if (ShowCorner) {
    Hv_InitShowCorner(newstr, &crect, &srect);
    Hv_SetSizeString(newstr, oldstr, &srect, RotateIt,
		     crect.left, crect.top, Hv_RADTODEG*ang);
  }

  if (Hv_hotItem != NULL)
    Hv_GetItemCenter(Hv_hotItem, &xc, &yc);

  Hv_DrawXorItem(Item, ldr); 

  while (!done) {

/* if Check For Motion or Button Releases */

      if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | Button1MotionMask, &Event)) {

	switch(Event.type) {

/* Motion Notify means the mouse is moving */

	case MotionNotify:
	  count++;
	  if ((count % 2) == 1) {
	    
	    Hv_DrawXorItem(Item, ldr); 
	    Hv_GetCurrentPoint(StopPP, Event.xbutton.x, Event.xbutton.y, &LimitRect);

	    if (RotateIt) {
	      delx = (float)(StopPP->x - refpp.x);
	      dely = (float)(StopPP->y - refpp.y);
	      if ((delx != 0.0) ||  (dely != 0.0))
		ang = atan2((double)delx, (double)(-dely));
	      if (ang < 0)
		ang += Hv_TWOPI;

/* put in a handler for world polygons */

	      if (Item->type == Hv_WORLDPOLYGONITEM) {
		wpoly = (Hv_WorldPolygonData *)(Item->data);


		if (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG)) {


/*		  fprintf(stderr, "item az = %f\n", Hv_RADTODEG*(Item->azimuth));
		  fprintf(stderr, "current ang = %f\n", Hv_RADTODEG*ang); */

		  daz = Item->azimuth-ang; /*change in azimuth*/


/* in map views, fpts store lat longs, 1st point is base */

		  for (i = 0; i < wpoly->numpnts; i++) {
		    az = Hv_GetAzimuth(wpoly->basepoint.h,
				       wpoly->basepoint.v,
				       wpoly->fpts[i].h,
				       wpoly->fpts[i].v);


/*		    if (i == 2) {
		      fprintf(stderr, "old az = %f\n", Hv_RADTODEG*az);
		    } */


		    az -= daz;


/*		    if (i == 2) {
		      fprintf(stderr, "new az = %f\n", Hv_RADTODEG*az);
		    } */

		    gcd = Hv_GCDistance(wpoly->basepoint.h, wpoly->basepoint.v,
					wpoly->fpts[i].h, wpoly->fpts[i].v);


/*		    if (i == 2) {
		      fprintf(stderr, "GCD = %f\n", gcd);
		      fprintf(stderr, "ol = %f  ol = %f\n", Hv_RADTODEG*wpoly->fpts[i].h,
			      Hv_RADTODEG*wpoly->fpts[i].v);
		    }*/


		    Hv_OffsetLatLong(wpoly->basepoint.h, wpoly->basepoint.v, 
				     gcd, az,
				     &(wpoly->fpts[i].h), &(wpoly->fpts[i].v));


/*		    if (i == 2) {
		      fprintf(stderr, "nl = %f  nl = %f\n", Hv_RADTODEG*wpoly->fpts[i].h,
			      Hv_RADTODEG*wpoly->fpts[i].v);
		    }*/
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
	      dh = StopPP->x - StartPP.x;
	      dv = StopPP->y - StartPP.y;
	      Hv_offsetDueToDrag = True ;
	      Hv_OffsetItem(Item, dh, dv, True);
	      Hv_offsetDueToDrag = False;
	      
/* if this is a hotrect object, we have to fix world rect */
	  
	      Hv_FixItemWorldArea(Item, True);

	      if (Item->afterdrag != NULL)
		Item->afterdrag(Item, dh, dv);
	      
	      if (Hv_hotItem != NULL)
		if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_FEEDBACKWHENDRAGGED))
		  if (Hv_PointInRect(*StopPP, Hv_hotView->hotrect))
		    if (Hv_hotView->feedback != NULL) {
		      Hv_SetPoint(&osp, StopPP->x, StopPP->y); 
		      Hv_hotView->feedback(Hv_hotView, osp);
		      Hv_SetClippingRegion(ldr);
		    }
	      
	      Hv_OffsetRect(&crect, dh, dv);
	    }

	    StartPP = *StopPP;
	    Hv_DrawXorItem(Item, ldr);
	  }
	  break; /* end of case MotionNotify */

	      
/* if button 1 released, we are done */

	case ButtonRelease:
	  if (Event.xbutton.button == 1) {	      
	    done = True;
	    break;
	  }
	      
	default:
	  break;
	}

      }  /* end  if if CheckMask */


      if (ShowCorner)
	Hv_SetSizeString(newstr, oldstr, &srect, RotateIt,
			 crect.left, crect.top, Hv_RADTODEG*ang);
      
    }   /*end of while !done */

/* Final xor draw */
      
  Hv_DrawXorItem(Item, ldr);

  if (ShowCorner)
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
  Hv_DrawCompoundString(r->left+1, r->top+1, hvstr);
  Hv_useDrawImage = False;
  Hv_DestroyString(hvstr);
  strcpy(oldstr, newstr);
  Hv_SetBackground(Hv_colors[Hv_canvasColor]);
}


/*------- Hv_GetCurrentPoint ------*/

static void     Hv_GetCurrentPoint(Hv_Point *CPnt,
				   int x,
				   int y,
				   Hv_Rect *LimitRect)

{
  CPnt->x = Hv_sMax(LimitRect->left, Hv_sMin(LimitRect->right,  (short)x));
  CPnt->y = Hv_sMax(LimitRect->top,  Hv_sMin(LimitRect->bottom, (short)y));
}


/*---------- Hv_InitLimitRect -------*/

static void Hv_InitLimitRect(Hv_Rect *LimitRect)

{
  LimitRect->width  -= 1;
  LimitRect->height -= 1;
  Hv_FixRectRB(LimitRect);
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
	     (CurRect->left + CurRect->right - ((short)w))/2,
	     (CurRect->top + CurRect->bottom - ((short)h))/2,
	     w+6, h+4); 

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
E 1
