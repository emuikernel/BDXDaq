/**
 * <EM>Various support routines for rectangles.</EM>
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


#include "Hv.h"	                 /* contains all necessary include files */

/**
 * Hv_PrintRect 
 * @purpose Diagnostic tool for printing an Hv_Rect
 * @param     file    The output file (e.g. stderr)
 * @param     rect    The rect to print
 * @param     message A message also printed
 */

void            Hv_PrintRect(FILE *file,
			     Hv_Rect *rect,
			     char * message) {

  if (message != NULL)
    fprintf(file, "%s ", message);


  fprintf(file, "[%d, %d, %d, %d] [%d, %d]\n",
	  rect->left,
	  rect->top,
	  rect->right,
	  rect->bottom,
	  rect->width,
	  rect->height);

}

/*
 * Hv_RectRight
 * @purpose Get the pixel value of the right edge
 * @param  r   The Hv_Rect
 * @return The rect's right edge pixel value
 */

short Hv_RectRight(Hv_Rect *r) {
  return  r->left + r->width;
}

/*
 * Hv_RectBottom
 * @purpose Get the pixel value of the bottom edge
 * @param  r   The Hv_Rect
 * @return The rect's bottom edge pixel value
 */

short Hv_RectBottom(Hv_Rect *r) {
  return  r->top + r->height;
}


/**
 * Hv_KeepRectInsideRect
 * @purpose Shrinks, if necessary, theR to keep it inside testR.
 * @param   theR   The rect that (might) be adjusted
 * @param   testR  The boundary rect
 */

void Hv_KeepRectInsideRect(Hv_Rect  *theR,
			   Hv_Rect  *testR) {

  theR->left   = Hv_sMax(theR->left,   testR->left);
  theR->top    = Hv_sMax(theR->top,    testR->top);
  theR->right  = Hv_sMin(theR->right,  testR->right);
  theR->bottom = Hv_sMin(theR->bottom, testR->bottom);
  Hv_FixRectWH(theR);
}


/*----------- Hv_GetRectCenter --------*/

void Hv_GetRectCenter(Hv_Rect  *r,
		      short   *xc,
		      short   *yc)

/*--------------------------------------
  returns the pixel center of the rect.
-----------------------------------------*/

{
  *xc = r->left + ((r->width)/2);
  *yc = r->top  + ((r->height)/2);
}

/*--------- Hv_GetFRectCenter -------*/

void Hv_GetFRectCenter(Hv_FRect  *r,
		       float    *xc,
		       float    *yc)

/*--------------------------------------
  returns the float center of the rect.
-----------------------------------------*/

{
  *xc = r->xmin + ((r->width)/2);
  *yc = r->ymin + ((r->height)/2);
}

/* ----------- Hv_FixRectWH ------------*/

void Hv_FixRectWH(Hv_Rect *theSR)

/*Given that the rects l, t, r, and b are ok, this fixes w & h*/

{
 theSR->width = theSR->right - theSR->left;
 theSR->height = theSR->bottom - theSR->top;
}

/* ----------- Hv_FixRectRB ------------*/

void Hv_FixRectRB(Hv_Rect *theSR)

/*Given that the rects l, t, w, and h are ok, this fixes r & b*/

{
 theSR->right = theSR->left + theSR->width;
 theSR->bottom = theSR->top + theSR->height;
}

/* ----------- Hv_FixRectWH ------------*/

void Hv_FixFRectWH(Hv_FRect *thefSR)

/*Given that the rects xmin, xmax, ymin, and ymax are ok, this fixes w & h*/

{
 thefSR->width =  thefSR->xmax - thefSR->xmin;
 thefSR->height = thefSR->ymax - thefSR->ymin;
}


/* ------------ Hv_InitRect ------------ */

void Hv_InitRect(Hv_Rect *theSR)

/* initialize Hv_Rect to all zeroes*/

{
  Hv_SetRect(theSR, 0, 0, 0, 0);
}


/**
 * Hv_CopyRect
 * @purpose Copies a Retangle. Undefined (but safe) if source or dest is NULL.
 * @param  dest   Destination rectangle.
 * @param src    Source rectangle.
 */

void Hv_CopyRect (Hv_Rect *dest,
		  Hv_Rect *src) {


  if ((dest == NULL) || (src == NULL))
	  return;

  dest->left   = src->left;
  dest->top    = src->top;
  dest->right  = src->right;
  dest->bottom = src->bottom;
  dest->width  = src->width;
  dest->height = src->height;
}

/**
 * Hv_CopyFRect
 * @purpose Copies a (floating point) Rectangle. Undefined (but safe)
 *         if source or dest is NULL.
 * @param  dest   Destination rectangle.
 * @param src    Source rectangle.
 */


void Hv_CopyFRect (Hv_FRect  *dest,
		   Hv_FRect  *src) {

  if ((dest == NULL) || (src == NULL))
	  return;

  dest->xmin   = src->xmin;
  dest->ymin   = src->ymin;
  dest->xmax   = src->xmax;
  dest->ymax   = src->ymax;
  dest->width  = src->width;
  dest->height = src->height;
}

/*------ Hv_GetRectLTRB ---------*/

void  Hv_GetRectLTRB(Hv_Rect  *rect,
			    short   *l,
			    short   *t,
			    short   *r,
			    short   *b)

/*--------------------------------------------
  Convenience routine for returning the pixel vals
  of the left, top, right and bottom of the rect.
----------------------------------------------*/

{
  *l = rect->left;
  *t = rect->top;
  *r = rect->right;
  *b = rect->bottom;
}


/* -------- Hv_SetRect ----------------*/

void Hv_SetRect(Hv_Rect    *srect,
		short      L,
		short      T,
		short      W,
		short      H)  

/*stuff the Rect with passed arguments*/

{
  srect->left = L;
  srect->top = T;
  srect->width = W;
  srect->height = H;
  Hv_FixRectRB(srect);
}

/* -------- Hv_SetFRect ----------------*/

void Hv_SetFRect(Hv_FRect	*frect,
		 float      xmin,
		 float      xmax,
		 float      ymin,
		 float      ymax)

/*stuff the Rect with passed arguments*/

{
  frect->xmin = xmin;
  frect->xmax = xmax;
  frect->ymin = ymin;
  frect->ymax = ymax;
  Hv_FixFRectWH(frect);
}

/*------- Hv_WhichPointOnRect ----*/

short Hv_WhichPointOnRect(short   x,
			  short   y,
			  Hv_Rect  *r)

/*--------------------------------------------
  returns an integer indicating to which part we are
  closest: 0-> UPPERLEFT, 1->MIDDLETOP, 2->UPPERRIGHT, 
  3->MIDDLELEFT, 4->MIDDLE, 5->MIDDLERIGHT,
  6->LOWERLEFT, 7->MIDDLEBOTTOM, 8->LOWERRIGHT
----------------------------------------------*/

{
  Hv_Rect   corner;

  int      i, j, k;
  short    xt, yt, w, h;
  Hv_Point pp;

  w = r->width/3;
  h = r->height/3;

  Hv_SetPoint(&pp, x, y);

  k = 0;
  for (i = 0; i < 3; i++) {
    yt = r->top + i*h;
    for (j = 0; j < 3; j++) {
      xt = r->left + j*w;
      Hv_SetRect(&corner, xt, yt, w, h);
      if (Hv_PointInRect(pp, &corner))
	    return k;
      k++;
    }
  }
  
  return -1;
}

/*------ Hv_GetRectCorner ---------*/

/* corner [0,1,2,3] => p is  [upperleft, upperright, lowerright, lowerleft] */

void Hv_GetRectCorner(short   corner,
		      Hv_Rect  *area,
		      Hv_Point *p)

{
  short  l, t, r, b, xc, yc;

  Hv_GetRectLTRB(area, &l, &t, &r, &b);
  Hv_GetRectCenter(area, &xc, &yc);

  switch (corner) {
  case Hv_UPPERLEFT:
    Hv_SetPoint(p, l, t);
    break;

  case Hv_MIDDLETOP:
    Hv_SetPoint(p, xc, t);
    break;

  case Hv_UPPERRIGHT:
    Hv_SetPoint(p, r, t);
    break;

  case Hv_MIDDLELEFT:
    Hv_SetPoint(p, l, yc);
    break;

  case Hv_MIDDLE:
    Hv_SetPoint(p, xc, yc);
    break;

  case Hv_MIDDLERIGHT:
    Hv_SetPoint(p, r, yc);
    break;

  case Hv_LOWERLEFT:
    Hv_SetPoint(p, l, b);
    break;

  case Hv_MIDDLEBOTTOM:
    Hv_SetPoint(p, xc, b);
    break;

  case Hv_LOWERRIGHT:
    Hv_SetPoint(p, r, b);
    break;

  }

}


/* ---------- Hv_ResizeRect ------- */

void Hv_ResizeRect (Hv_Rect     *rect,
		    short      dh,
		    short      dv)

/* grow the rectangle width by dh and height by dv.
   Prevents height and/or width from becoming negative */

{
  rect->width  = Hv_sMax((short)(rect->width  + dh), 0);
  rect->height = Hv_sMax((short)(rect->height + dv), 0);
  Hv_FixRectRB(rect);
}

/* ---------- Hv_OffsetRect ------*/

void Hv_OffsetRect (Hv_Rect  *rect,
		    short   dh,
		    short   dv)

/* Move the rectangle dh to the right and down dv */

{
  rect->left += dh;
  rect->right += dh;
  rect->top += dv;
  rect->bottom += dv;
}


/* ------- Hv_GoodRect --------------- */

void Hv_GoodRect(Hv_Rect *rect)

{
  short temp;  /* temp var for exchange */

/* takes the Left, top, right, and Bottom coordinates of a box
   and confirms that the right > left and the top > bottom     */

  if (rect->left > rect->right) {
    temp = rect->left;
    rect->left = rect->right;
    rect->right = temp;
    rect->width = rect->right - rect->left;
  }

  if (rect->top > rect->bottom) {
    temp = rect->top;
    rect->top = rect->bottom;
    rect->bottom = temp;
    rect->height = rect->bottom - rect->top;
  }
}

/**
 * Hv_PointInFRect
 * @purpose Check if a point is inside a rect
 * @param  pp The local point to check
 * @param  rect The local rect to check
 * @return True if point is inside the rect
 */

Boolean Hv_PointInRect(Hv_Point   pp,
		       Hv_Rect   *rect) {
  if (rect == NULL)
    return False;

  return (((pp.x > rect->left) && (pp.x < rect->right)) 
	  && ((pp.y > rect->top) && (pp.y < rect->bottom)));
}

/**
 * Hv_FPointInFRect
 * @purpose Check if a point is inside a rect
 * @param  pp The world point to check
 * @param  rect The world rect to check
 * @return True if point is inside the rect
 */

Boolean Hv_FPointInFRect(Hv_FPoint	pp,
			 Hv_FRect    *rect) {
  return (((pp.h > rect->xmin) && (pp.h < rect->xmax)) && ((pp.v > rect->ymin) && (pp.v < rect->ymax)));
}

/**
 * Hv_ShrinkRect
 * @purpose Shrinks the rect as if the amounts specified are 
 *  removed FROME EACH SIDE of the rect, so the effect may be twice
 *  what you expect. For example, the left will be moved (right) by an amount
 *  dh, and the right will be moved (left). Thus the width will be reduced by
 *  2*dh. Both dh and dv can be negative (to expand the rect).
 * @param r   The rect to shrink 
 * @param dh  The horizontal change (pixels)
 * @param dv  The vertical change (pixels)
 */

void  Hv_ShrinkRect(Hv_Rect    *r,
		    short       dh,
		    short        dv) {

  if (dh != 0) {
    r->left   += dh;
    r->right  -= dh;

	if (r->right < r->left)
		r->right = r->left;
  }

  if (dv != 0) {
    r->top    += dv;
    r->bottom -= dv;
	if (r->bottom < r->top)
		r->bottom = r->top;
   }

  if ((dh != 0) || (dv != 0))
    Hv_FixRectWH(r);
}

/**
 * Hv_LeftTopResizeRect
 * @purpose Resizes as if grabbed from upper left corner. Note
 *   that right and bottom are unchanged
 * @param r   The rect to resize 
 * @param dh  The horizontal change (pixels)
 * @param dv  The vertical change (pixels)
 */

void  Hv_LeftTopResizeRect(Hv_Rect    *r,
			   short     dh,
			   short     dv) {

  r->left   += dh;
  r->top    += dh;
  r->width  -= dh;
  r->height -= dv;
}

/* ----------  ----------- */

/**
 * Hv_SetXRectangle
 * @purpose Set the rectangle using the given parameters
 * @param r       The rectangle to set
 * @param left    Left edge (pixels)
 * @param top     Top edge (pixels)
 * @param right   Right edge (pixels)
 * @param bottom  Bottom edge (pixels)
 */

void	Hv_SetXRectangle(Hv_Rectangle	*r,
			 short	         left,
			 short	         top,
			 short	         width,		
			 short           height) {

/*  */

  r->x = left;
  r->y = top;
  r->width = width;
  r->height = height;
}

/**
 * Hv_PointInRectangle
 * @purpose Check if the given pixel point lies
 *  within the local rectangle 
 * @param pp point to check
 * @param x  left of rect
 * @param y  top of rect
 * @param w  width of rect
 * @param h  height of rect
 */

Boolean Hv_PointInRectangle(Hv_Point 	pp,
			    short	x,
			    short	y,
			    short       w,
			    short       h) {
  return (((pp.x > x) && (pp.x < (x + w))) && ((pp.y > y) && (pp.y < (y + h))));
}

/**
 * Hv_FPointInRectangle
 * @purpose Check if the given world point lies
 *  within the real rectangle 
 * @param pp point to check
 * @param x  left of rect
 * @param y  top of rect
 * @param w  width of rect
 * @param h  height of rect
 */

Boolean Hv_FPointInRectangle(Hv_FPoint	pp,
			     float	x,
			     float	y,
			     float      w,
			     float      h) {


  return (((pp.h > x) && (pp.h < (x + w))) && ((pp.v > y) && (pp.v < (y + h))));
}
