/*	
-----------------------------------------------------------------------
File:     Hv_drawsupport.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"	                 /* contains all necessary include files */

/*----- local prototypes --------*/

static void  Hv_PointToFPoint(Hv_Point,
			      Hv_FPoint *);

static Boolean   Hv_LinesIntersect(Hv_Point,
				   Hv_Point,
				   Hv_Point,
				   Hv_Point);

static void      Hv_FPointToPoint(Hv_Point *,
				  Hv_FPoint *);

/*-------- Hv_SetLineStyle ---------*/

void Hv_SetLineStyle(int w,
		     int style)

/*
int     w       width 
int     style   solid or dashed 
*/

{
  static int oldw = -99;
  static int oldstyle = -99;
  int        offset = 0;
  int        dashlen = 2;
  int        dotdashlen = 4;

  static char dashed[2] = {4, 4};
  static char longdashed[2] = {8, 8};
  static char dotdash[4] = {8, 4, 3, 4};

/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */

  if (Hv_inPostscriptMode) {
    if (w < 1)
      w = 1;
    w *= 3;

    Hv_PSSetLineStyle(w, style);
    return;
  }

  if (w < 2)
    w = 0;

  if ((w != oldw) || (style != oldstyle)) {
    if (style <= Hv_SOLIDLINE)
      XSetLineAttributes(Hv_display, Hv_gc, w, LineSolid, CapButt, JoinMiter);
    else {
      XSetLineAttributes(Hv_display, Hv_gc, w, LineOnOffDash, CapButt, JoinMiter); 
      if (style == Hv_DASHED)
	XSetDashes(Hv_display, Hv_gc, offset, dashed, dashlen);
      else if (style == Hv_LONGDASHED)
	XSetDashes(Hv_display, Hv_gc, offset, longdashed, dashlen);
      else if (style == Hv_DOTDASH)
	XSetDashes(Hv_display, Hv_gc, offset, dotdash, dotdashlen);
    }


    oldw = w;
    oldstyle = style;
  }
}

/*-------- Hv_SetPattern ---------*/

void Hv_SetPattern(int pattern,
		   short color)

{

/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */


  if (Hv_inPostscriptMode) {
    Hv_PSSetPattern(pattern, color);
    return;
  }


  if ((pattern <= Hv_FILLSOLIDPAT) || (pattern >= Hv_NUMPATTERNS))
     Hv_SetFillStyle(FillSolid);
  else {
   XSetStipple(Hv_display, Hv_gc, Hv_patterns[pattern]);
   Hv_SetFillStyle(FillStippled);
  }
}


/*---------- Hv_ReTileMainWindow ------*/

void Hv_ReTileMainWindow(void)

{
  Window      twind;
  Hv_Pixmap   pmap;

  if ((Hv_TileDraw != NULL) &&(Hv_mainWindow != 0)) {
    pmap = Hv_CreatePixmap(Hv_tileX, Hv_tileY);
    twind = Hv_mainWindow;
    Hv_mainWindow = pmap;
    Hv_TileDraw();
    Hv_mainWindow = twind;
    Hv_TileMainWindow(pmap);
    Hv_FreePixmap(pmap);
  }
}

/*--------- Hv_TileMainWindow -------*/

void   Hv_TileMainWindow(Hv_Pixmap pmap)

{
  if ((pmap == 0) || (Hv_display == NULL) || (Hv_mainWindow == 0))
    return;

  XSetWindowBackgroundPixmap(Hv_display, Hv_mainWindow, (Pixmap)pmap);
  XSetTile(Hv_display, Hv_gc, (Pixmap)pmap);
  XClearArea(Hv_display, Hv_mainWindow, 0, 0, 0, 0, True);
}


/*-------- Hv_SetBackground -------*/ 

void  Hv_SetBackground(unsigned long bg)

{
  XSetBackground(Hv_display, Hv_gc, bg);
}

/*-------- Hv_SetForeground -------*/ 

void  Hv_SetForeground(unsigned long fg)

{
  XSetForeground(Hv_display, Hv_gc, fg);
}

/*-------- Hv_SetFillStyle -------*/ 

void  Hv_SetFillStyle(int fillstyle)

{
  XSetFillStyle(Hv_display, Hv_gc, fillstyle);
}

/*------- Hv_RectWithinContainer -------*/

void   Hv_RectWithinContainer(Hv_View   View,
			      Hv_Rect  *rect)

/*---------------------------------
 Make sure the rect is within the confines
 of the view's container, taking into account
 possibility of scroll bars 
----------------------------------*/

{
  short       bw, fw;
  Hv_Item     cptr;

  if ((View == NULL) || (rect == NULL))
    return;

  cptr = View->containeritem;


  fw = cptr->scheme->framewidth + 1;
  bw = fw;

  if (Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE))
    bw += cptr->scheme->borderwidth;

  rect->left   = Hv_sMax(rect->left,   cptr->area->left   + fw);
  rect->top    = Hv_sMax(rect->top,    cptr->area->top    + fw);
  rect->right  = Hv_sMin(rect->right,  cptr->area->right  - bw);
  rect->bottom = Hv_sMin(rect->bottom, cptr->area->bottom - bw);
  Hv_FixRectWH(rect);
}



/* -------- Hv_GetForeground --------------*/

unsigned long  Hv_GetForeground(void)

/* returns the present foreground color pixel value from the Hv_gc */

{
  XGCValues  values;
  
  XGetGCValues (Hv_display, Hv_gc, GCForeground, &values);
  return(values.foreground);
}

/* -------- Hv_GetBackground --------------*/

unsigned long  Hv_GetBackground(void)

/* returns the present background color pixel value from the Hv_gc */

{
  XGCValues  values;
  
  XGetGCValues (Hv_display, Hv_gc, GCBackground, &values);
  return(values.background);
}


/*---------- Hv_SwapForeground ------- */

unsigned long Hv_SwapForeground(short color)

/*changes the foreground of the global display and gc
  to the given value. Returns the old one in case you want
  to save it */

{
  unsigned long old_fg;
  static Boolean HighlightMode = False;

  if (HighlightMode) {
    XSetFunction(Hv_display, Hv_gc, GXcopy);     /* xor's the GC's drawing function */
    HighlightMode = False;
 }

  old_fg = Hv_GetForeground();      /* store the old foregound color */

  if (color >= 0) {
    if (color == Hv_highlight) {
      XSetFunction(Hv_display, Hv_gc, GXxor);     /* xor's the GC's drawing function */
      HighlightMode = True;
      Hv_SetForeground(1); 
     }
    else
      Hv_SetForeground(Hv_colors[color]);         /* sets the foreground to color specified */
  }
  
  return old_fg;
}


/*---------- Hv_SwapBackground ------- */

unsigned long Hv_SwapBackground(short color)


/*changes the foreground of the global display and gc
  to the given value. Returns the old one in case you want
  to save it */

{
  unsigned long old_bg;

  old_bg = Hv_GetBackground();      /* store the old backgound color */

  if (color >= 0)
    Hv_SetBackground(Hv_colors[color]);         /* sets the background to color specified */
  return  old_bg;
}


/*--------- Hv_SetSegment ---------*/

void Hv_SetSegment(Hv_Segment *seg,
		   short       x1,
		   short       y1,
		   short       x2,
		   short       y2)

{
  seg->x1 = x1;
  seg->y1 = y1;
  seg->x2 = x2;
  seg->y2 = y2;
}

/*------ Hv_SetLineWidth ------*/

void Hv_SetLineWidth(int w)

/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */

{
  if (Hv_inPostscriptMode) {
    if (w < 1)
      w = 1;
    w *= 3;
    Hv_PSSetLineWidth(w);    
    return;
  }

  if (w < 2)
    w = 0;
  XSetLineAttributes(Hv_display, Hv_gc, w, LineSolid, CapButt, JoinMiter); 

}



/*-------- Hv_WorldPointToLocalPoint -------*/

void  Hv_WorldPointToLocalPoint(Hv_View    View,
				Hv_FPoint *wpnt,
				Hv_Point  *lpnt)

{
  Hv_WorldToLocal(View, wpnt->h, wpnt->v, &(lpnt->x), &(lpnt->y));
}

/*-------- Hv_WorldPoint2ToLocalRect -------*/

void  Hv_WorldPoint2ToLocalRect(Hv_View View,
				Hv_FPoint *wpnt1,
				Hv_FPoint *wpnt2,
				Hv_Rect   *area,
				short     slop)

{
  short   x1, y1, x2, y2;

  Hv_WorldToLocal(View, wpnt1->h, wpnt1->v, &x1, &y1);
  Hv_WorldToLocal(View, wpnt2->h, wpnt2->v, &x2, &y2);

  area->left   = Hv_sMin(x1, x2);
  area->right  = Hv_sMax(x1, x2);
  area->top    = Hv_sMin(y1, y2);
  area->bottom = Hv_sMax(y1, y2);
  Hv_FixRectWH(area);
  Hv_ShrinkRect(area, -slop, -slop);
}

/* ---------- Hv_SetPoint ---------*/

void	Hv_SetPoint(Hv_Point *pp,
		    short     x,
		    short     y)

/*Stuff the requested x and y coordinates into the Hv_Point data structure*/

{
  pp->x = x;
  pp->y = y;
}

/* ---------- Hv_SetFPoint ---------*/

void	Hv_SetFPoint(Hv_FPoint *fp,
		    float     x,
		    float     y)

/*Stuff the requested x and y coordinates into the Hv_FPoint data structure*/

{
  fp->h = x;
  fp->v = y;
}

/* ------ Hv_ItemDrawingRegion -------*/

Region Hv_ItemDrawingRegion(Hv_Item Item,
			    short   dh,
			    short   dv)

/* 
Hv_Item   Item
short     dh, dv     (shrinkage)

   mallocs and returns the intersection of the Views drawing
   region and the region made from the item's area */

/* also subtracts out the balloon region, if it is up */

{
  Hv_View     View = Item->view;

  Hv_Region   drawreg, areareg, thereg;
  Hv_Rect     area;

  Hv_CopyRect(&area, Item->area);
  Hv_ShrinkRect(&area, dh, dv);

/* for all items except containers, don't incude container border */

  if (Item->type != Hv_CONTAINERITEM)
    Hv_RectWithinContainer(View, &area);


  areareg = Hv_RectRegion(&area);
  drawreg = Hv_GetViewRegion(View);
  thereg  = Hv_IntersectRegion(areareg, drawreg);

  Hv_DestroyRegion(drawreg);
  Hv_DestroyRegion(areareg);

  if (Hv_showBalloons && Hv_balloonUp) {
    Hv_CopyRegion(&drawreg, thereg);
    Hv_DestroyRegion(thereg);
    thereg = Hv_SubtractRegion(drawreg, Hv_theBalloon.region);
    Hv_DestroyRegion(drawreg);
  }

  return thereg;
}


/* ------- Hv_ClipItem ------- */

void  Hv_ClipItem(Hv_Item Item,
		  short   dh,
		  short   dv)

/* MALLOCS, and SETS the clip region to the INTERSECTION of the Views 
   drawing (i.e. visible) region and the passed item's area, and
   then draws the View with said clip. dh and dv are "shinkage" */

{
  Hv_View        View = Hv_GetItemView(Item);
  Hv_Region      rgn;
  Hv_Item        temp;
  Hv_Rect        r;

  Hv_CopyRect(&r, Item->area);
  Hv_ShrinkRect(&r, dh, dv);

  rgn =  Hv_RectRegion(&r);

  Hv_DrawView(View, rgn);  /*Hv_DrawView will intersect this rgn with view vis rgn*/

/* go through any sub-list of items */

  for (temp = Item->children->first; temp != NULL; temp = temp->next)
      Hv_ClipItem(temp, 0, 0);

  
  Hv_DestroyRegion(rgn);
}

/*----------- Hv_ClipBox  ------*/

void     Hv_ClipBox(Hv_Region  region,
		    Hv_Rect    *r)

/* smallest rect around region */

{
   XRectangle    xrect;
   XClipBox((Region)region, &xrect);
   Hv_SetRect(r, xrect.x, xrect.y, xrect.width, xrect.height);
}


/* ------------- Hv_FullClip -------------*/

void Hv_FullClip(void)

/* Reset the clip region to the entire canvas */


/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */

{
  if (Hv_inPostscriptMode) {
    Hv_PSFullClip();
    return;
  }

  Hv_ClipRect(&Hv_canvasRect);
}



/* ------------ Hv_CreatePolygonFromRect ----------- */

void  Hv_CreatePolygonFromRect(Hv_Point  *xp,
			       Hv_Rect   *rect)

/*
Hv_Point        *xp    pointer to points
Hv_Rect         *rect  super rect to set points to

 set the points to 4pt poly equivalent to the rect */

{
  short  l, t, r, b;

  Hv_GetRectLTRB(rect, &l, &t, &r, &b);

  xp[0].x = l;   xp[0].y = b; 
  xp[1].x = r;   xp[1].y = b;
  xp[2].x = r;   xp[2].y = t;
  xp[3].x = l;   xp[3].y = t;
}

/*----------  Hv_PolygonEnclosingRect ---------*/

void    Hv_PolygonEnclosingRect(Hv_Rect  *rect,
				Hv_Point *xp,
				int      npoly)

/* returns the Rect that encloses the polygon */

{
  XRectangle   xrect;
  Hv_PolygonEnclosingRectangle(&xrect, xp, npoly);
  Hv_SetRect(rect, xrect.x, xrect.y, xrect.width, xrect.height);
}

/*----------- Hv_OffsetPolygon -------*/

void   Hv_OffsetPolygon(Hv_Point *xp,
			int       np,
			short     dh,
			short     dv)

{
  int i;

  for (i = 0; i < np; i++) {
    xp[i].x += dh;
    xp[i].y += dv;
  }
}

/* ---------- Hv_PointInRegion -------*/

Boolean Hv_PointInRegion(Hv_Point  pp,
			 Hv_Region region)

{
  if (region == NULL)
    return False;
  else
    return  XPointInRegion(region, pp.x, pp.y);
}

/* ---------- Hv_RectInRegion -------*/

Boolean  Hv_RectInRegion(Hv_Rect   *r,
			 Hv_Region region)

{
  if ((region == NULL) || (r == NULL))
    return False;

  return (XRectInRegion((Region)region, r->left, r->top,
			 r->width+1, r->height+1) != RectangleOut);
}


/* ---------- Hv_PointInPolygon -------*/

Boolean Hv_PointInPolygon(Hv_Point  pp,
			  Hv_Point *poly,
			  int       npoly)
{
  Boolean inside;
  Hv_Region  polyreg;

  polyreg = Hv_CreateRegionFromPolygon(poly, (short)npoly);
  inside = Hv_PointInRegion(pp, polyreg);

  Hv_DestroyRegion(polyreg);
  return  inside;
}


/* ----------- ValidPointOnLine ------------ */

Boolean ValidPointOnLine(ThePoint, P1, P2)

/* The name of this procedure is a little misleading: given that ThePoint
   IS KNOWN TO BE on the infinite line passing through P1 and P2, this
   returns true if in fact it ThePoint is on the segment of the line
   connecting P1 and P2 */

Hv_Point   ThePoint, P1, P2;

{
  short           xmin, ymin, xmax, ymax;

  if (P1.x == P2.x) {          /* vertical */
    if (P1.y < P2.y) {
      ymin = P1.y;
      ymax = P2.y;
    }
    else {
      ymin = P2.y;
      ymax = P1.y;
    }

    return ((ThePoint.y >= ymin) && (ThePoint.y <= ymax));
  }
  
/* horizontal or generic */

  if (P1.x < P2.x) {
    xmin = P1.x;
    xmax = P2.x;
  }
  else {
    xmin = P2.x;
    xmax = P1.x;
  }
  return ((ThePoint.x >= xmin) && (ThePoint.x <= xmax));
}

/* ----------- Hv_PolygonIntersectsPolygon ---------*/

Boolean Hv_PolygonIntersectsPolygon(Hv_Point *xp1,
				    Hv_Point *xp2,
				    short     np1,
				    short     np2)

/* sees whether the two polys intersect based on the
   algorithm that at least one vertex from one poly
   must be inside the other*/

{
  short       i, j, ii, jj;
  Boolean     isects;

  i = 0;
  isects = False;

/*first two checks are to see if any vertex from one polygon
  is inside the other. This is not a fool-proof check -- for
  example think of "star of David" made from two triangles --
  this would fail */

  while ((!isects) && (i < np2)) {
    isects = Hv_PointInPolygon(xp2[i], xp1, np1);
    i++;
  }

  i = 0;

  while ((!isects) && (i < np1)) {
    isects = Hv_PointInPolygon(xp1[i], xp2, np2);
    i++;
  }

/* if not intersecting based on thes test, one additional test is required.
   If any line from first polygon intersects with any from second, then
   polygons intersect*/
  
  if (!isects) {
    i = 0;
    while((!isects) && (i < np1)) {
      ii = (i+1) % np1;
      j = 0;
      while((!isects) && (j < np2)) {
	jj = (j+1) % np2;
	isects = Hv_LinesIntersect(xp1[i], xp1[ii], xp2[j], xp2[jj]);
	j++;
      }
      i++;
    }
  }
  
  return isects;
}

/* ------------ Hv_PolygonIntersectsRect ------- */

Boolean Hv_PolygonIntersectsRect(Hv_Point *xp2,
				 short     np2,
				 Hv_Rect  *rect)

/* determines if the polygon intersect the rectangle */

{
  Boolean     isects;
  short       np1 = 4;
  Hv_Point    *xp1;

  xp1 = Hv_NewPoints((int)np1);
  Hv_CreatePolygonFromRect(xp1, rect);
  isects = Hv_PolygonIntersectsPolygon(xp1, xp2, np1, np2);
  Hv_Free(xp1);
  return isects;
}

/* ------------ Hv_RectIntersectsRect ------- */

Boolean         Hv_RectIntersectsRect(Hv_Rect *rect1,
				      Hv_Rect *rect2)
{
  Hv_Point pp;
  short    l, t, r, b;

  Hv_GetRectLTRB(rect1, &l, &t, &r, &b);

  Hv_SetPoint(&pp, l, t);
  if (Hv_PointInRect(pp, rect2)) return True;

  Hv_SetPoint(&pp, l, b);
  if (Hv_PointInRect(pp, rect2)) return True;

  Hv_SetPoint(&pp, r, t);
  if (Hv_PointInRect(pp, rect2)) return True;

  Hv_SetPoint(&pp, r, b);
  if (Hv_PointInRect(pp, rect2)) return True;

/* reverse roles */

  Hv_GetRectLTRB(rect2, &l, &t, &r, &b);

  Hv_SetPoint(&pp, l, t);
  if (Hv_PointInRect(pp, rect1)) return True;

  Hv_SetPoint(&pp, l, b);
  if (Hv_PointInRect(pp, rect1)) return True;

  Hv_SetPoint(&pp, r, t);
  if (Hv_PointInRect(pp, rect1)) return True;

  Hv_SetPoint(&pp, r, b);
  if (Hv_PointInRect(pp, rect1)) return True;

/* center check */

  Hv_GetRectCenter(rect1, &(pp.x), &(pp.y));
  if (Hv_PointInRect(pp, rect2)) return True;

  Hv_GetRectCenter(rect2, &(pp.x), &(pp.y));
  if (Hv_PointInRect(pp, rect1)) return True;

  return False;
}


/* -------- Hv_InvertColorScheme ---------*/

void   Hv_InvertColorScheme(Hv_ColorScheme  *scheme)

/* inverts the colors on a color scheme -- useful
   for changing 3D in to 3D out and vice versa */

{
  short topcolor, lighttrimcolor;
  topcolor = scheme->topcolor;
  lighttrimcolor = scheme->lighttrimcolor;

  scheme->topcolor = scheme->bottomcolor;
  scheme->lighttrimcolor = scheme->darktrimcolor;
  scheme->bottomcolor = topcolor;
  scheme->darktrimcolor = lighttrimcolor;
}


/* --------- Hv_RotateFPoint -------- */

void  Hv_RotateFPoint(float      theta,
		      Hv_FPoint *P)

/* Rotates P (not coord sys) ccw by theta 

float          theta   angle to rotate by, in radians
Hv_FPoint      *P      point to be rotated

*/

{
  float c_theta, s_theta, x, y;

  c_theta = cos(theta);
  s_theta = sin(theta);

  x = P->h;
  y = P->v;

  P->h = x*c_theta - y*s_theta;
  P->v = y*c_theta + x*s_theta;
}

/* --------- Hv_RotateFPoints -------- */

void  Hv_RotateFPoints(float      theta,
		       Hv_FPoint *P,
		       short      num)

/* Rotates P (not coord sys) ccw by theta

float       theta     angle to rotate by, in radians
Hv_FPoint   *P        point to be rotated
short       num       number of points to be rotated

*/

{
  float c_theta, s_theta, x, y;
  short i;

  c_theta = cos(theta);
  s_theta = sin(theta);

  for (i = 0; i < num; i++) {
    x = P[i].h;
    y = P[i].v;
    
    P[i].h = x*c_theta - y*s_theta;
    P[i].v = y*c_theta + x*s_theta;
  }
}


/* --------- Hv_RotateFPointsAboutPoint -------- */

void  Hv_RotateFPointsAboutPoint(float      theta,
		       Hv_FPoint *P,
		       short      num,
		       float      fx,
		       float      fy)
{
  int  i;
  
  for (i= 0; i < num; i++) {
    P[i].h -= fx;
    P[i].v -= fy;
  }

  Hv_RotateFPoints(theta, P, num);

  for (i= 0; i < num; i++) {
    P[i].h += fx;
    P[i].v += fy;
  }
}

/*-------- Hv_SetClippingRegion -------*/ 

void  Hv_SetClippingRegion(Hv_Region region)

{
  if (region != NULL)
    XSetRegion(Hv_display, Hv_gc, (Region)region);   /* set the clipping area to the region */
  
  XSync(Hv_display, False);
}

/*------ Hv_RestoreClipRegion ------*/

void    Hv_RestoreClipRegion(Hv_Region region)

/***********************************************
  NOTE: Calls PSRestore, So should be matched
  at all times to a PSSave call
*************************************************/


{
  if (Hv_inPostscriptMode)
    Hv_PSRestore();
  else
    Hv_SetClippingRegion(region);
}


/* ------------- Hv_ClipRect -------------*/

void Hv_ClipRect(Hv_Rect *rect)

/* sets the clipping region of the Hv_gc to be the specified rectangle */

/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */

{
  Hv_Rect         psr;
  Hv_Region       region;

  if (Hv_inPostscriptMode) {
    Hv_LocalRectToPSLocalRect(rect, &psr);
    Hv_PSClipRect(&psr);
    return;
  }


  region = Hv_RectRegion(rect);       /* set a region to the rect */
  Hv_SetClippingRegion(region);            /* set the clipping area to the region */
  Hv_DestroyRegion(region);                  /* free the memory used by the region structure */
}

/* ----------- Hv_LinesIntersect ------------*/

static Boolean   Hv_LinesIntersect(Hv_Point PL1,
				   Hv_Point PR1,
				   Hv_Point PL2,
				   Hv_Point PR2)

{

/* returns true if the lines intersect */

  Hv_FPoint     L1, R1, L2, R2;
  Boolean       isects;
  short         DeltaX1, DeltaX2;
  short         DeltaY1, DeltaY2;
  float         DelZ1, DelX1;
  float         DelZ2, DelX2;
  float         tiny = 0.00001;
  Hv_FPoint     Pint;
  float         m1, m2;
  Hv_Point      ThePoint;

  Hv_PointToFPoint(PL1, &L1);
  Hv_PointToFPoint(PR1, &R1);
  Hv_PointToFPoint(PL2, &L2);
  Hv_PointToFPoint(PR2, &R2);

  DeltaX1 = PR1.y - PL1.y;
  DeltaX2 = PR2.y - PL2.y;

  isects = True;

  if ((DeltaX1 == 0) && (DeltaX2 == 0))   /*both vertical*/
    isects = False;

/* not both vertical block */

  else {
    if (DeltaX1 == 0) {   /*line1 is vertical */ 
      DeltaY2 = PR2.y - PL2.y;

      DelZ2 = (float)DeltaX2;
      DelX2 = (float)DeltaY2;
      m2 = DelX2/DelZ2;

      Pint.h = L1.h;
      Pint.v = m2*(Pint.h - L2.h) + L2.v;
    }
      
    else if (DeltaX2 == 0) {  /*line2 is vertical*/
      DeltaY1 = PR1.y - PL1.y;

      DelZ1 = (float)DeltaX1;
      DelX1 = (float)DeltaY1;
      m1 = DelX1/DelZ1;
      
      Pint.h = L2.h;
      Pint.v = m1*(Pint.h - L1.h) + L1.v;
    }
      
    else {   /*neither is vertical*/
      DeltaY1 = PR1.y - PL1.y;
      DeltaY2 = PR2.y - PL2.y;
      
      DelZ1 = (float)DeltaX1;
      DelX1 = (float)DeltaY1;
      m1 = DelX1/DelZ1;
      
      DelZ2 = (float)DeltaX2;
      DelX2 = (float)DeltaY2;
      m2 = DelX2/DelZ2;
      

/*parallel?*/

      if (fabs(m1 - m2) < tiny)
	isects = False;
      else {
	Pint.h = ((L2.v - L1.v) + m1*L1.h - m2*L2.h)/(m1 - m2);
	Pint.v = m1*(Pint.h - L1.h) + L1.v;
      }
    }

/*if passed all tests and we have an intersection point P1, see if it is valid*/

      if (isects) {
	Hv_FPointToPoint(&ThePoint, &Pint);
	isects = ValidPointOnLine(ThePoint, PL1, PR1);
	if (isects)
	  isects = ValidPointOnLine(ThePoint, PL2, PR2);
      }
  }
  return isects;
}


/*-------------- Hv_FPointToPoint ---------*/

static void   Hv_FPointToPoint(Hv_Point  *P,
			       Hv_FPoint *fP)

/* do a typecast conversion of the point */

{
  P->x = (short)(fP->h); 
  P->y = (short)(fP->v);
}

/*-------------- Hv_PointToFPoint ---------*/

static void  Hv_PointToFPoint(Hv_Point P,
			      Hv_FPoint *fP)

/* do a typecast conversion of the point */

{
  fP->h = (float)(P.x); 
  fP->v = (float)(P.y);
}
