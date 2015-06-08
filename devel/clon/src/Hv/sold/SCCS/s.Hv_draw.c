h02110
s 00000/00000/00000
d R 1.2 02/08/25 23:21:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_draw.c
e
s 01390/00000/00000
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
	File:     Hv_draw.c
	==================================================================================
	
	The Hv library was developed at CEBAF under contract to the
	Department of Energy and is the property of the U.S. Government.
	
	It may be used to develop commercial software, but the Hv
	source code and/or compiled modules/libraries may not be sold.
	
	Questions or comments should be directed to heddle@cebaf.gov
	==================================================================================
	*/

#include "Hv.h"	                 /* contains all necessary include files */
#include "Hv_maps.h"

/* ------ local prototypes --------*/

static void Hv_PSFrameWedge(short,
			    short,
			    short,
			    int,
			    int,
			    short);

/*================ LOWLEVEL ROUTINES ===============
  
  Low level routines are defined as the ones that check
  if we are in postscript mode and redirect if necessary.
  Thus routines built upon these lowelevel routines will
  automatically produce proper postscrip output */


/*---------- Hv_StringDraw ---------*/

void    Hv_StringDraw(short x,
		      short y,
		      Hv_String *str,
		      Hv_Region region)
    
{
    XmString	      xmst;			/* Motif compound string */
    short             xmw, xmh;		        /* String width  in pixels */
    unsigned long     oldfg;
    char              *fontname;
    float             fontsize, fx, fy;
    Boolean           shadow;
    float             extrah = 0.0;
    float             extraw = 0.0;
    Hv_Pixmap	      pmap1, pmap2;
    XImage           *image1, *image2;
    Hv_Rect           area, parea;
    Window            twin;
    int               i, j;
    short	      desty;		        /* destination vert pixel */
    
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
/* x and y represent the LEFT TOP of the string */
    
    if ((str == NULL) || (str->text == NULL))
	return;
    
    xmst = Hv_CreateAndSizeMotifString(str->text, str->font, &xmw, &xmh);
    
    if (Hv_inPostscriptMode) {
	fontname = (char *)Hv_Malloc(80);
	Hv_PSFont(str->font, fontname, &fontsize);
	
	shadow = False;
	
	if (str->horizontal) {
	    extrah = xmh*0.75*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY;
	    Hv_LocalToPSLocal(x, y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx, fy - extrah, fontsize, fontname, str->text,
			    Hv_PSHORIZONTAL, str->strcol, shadow, Hv_white);
	}
	else  {
	    extrah = xmh*0.25*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY;
	    extraw = xmw*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY;
	    Hv_LocalToPSLocal(x+xmh, y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx - extrah, fy - extraw, fontsize, fontname, str->text,
			    Hv_PSVERTICAL, str->strcol, False, Hv_white);
	}
	Hv_Free(fontname);
	return;
    }
    
    oldfg = Hv_SwapForeground(str->strcol);
    
    if (str->horizontal) {
	
	if (Hv_useDrawImage)
	    XmStringDrawImage(Hv_display, Hv_mainWindow, Hv_fontList, xmst, Hv_gc, x,
			      y, xmw, XmALIGNMENT_BEGINNING, XmALIGNMENT_BEGINNING, NULL);
	else
	    XmStringDraw(Hv_display, Hv_mainWindow, Hv_fontList, xmst, Hv_gc, x,
			 y, xmw, XmALIGNMENT_BEGINNING, XmALIGNMENT_BEGINNING, NULL);
	
    }
    else {  /* vertical, non postscript */
	pmap1 = Hv_CreatePixmap((int)xmw, (int)xmh);  /* horizontal */
	pmap2 = Hv_CreatePixmap((int)xmh, (int)xmw);  /* vertical */
	twin = Hv_mainWindow;
	Hv_mainWindow = pmap1;
	Hv_SetRect(&area, x, y, xmw+1, xmh+1);
	Hv_SetRect(&parea, 0, 0, xmw+1, xmh+1);
	Hv_ClipRect(&parea);
	
	if (str->fillcol >= 0)
	    Hv_FillRect(&parea, str->fillcol);
	
	str->horizontal = True;
	Hv_DrawCompoundString(1, 1, str);
	str->horizontal = False;
	Hv_mainWindow = twin;
	
	image1 = XGetImage(Hv_display,
			   (Pixmap)pmap1,
			   0,
			   0,
			   (int)xmw,
			   (int)xmh,
			   AllPlanes,
			   ZPixmap);
	
	image2 = XGetImage(Hv_display,
			   (Pixmap)pmap2,
			   0,
			   0,
			   (int)xmh,
			   (int)xmw,
			   AllPlanes,
			   ZPixmap);
	
/* now rotate text pixels from image1 onto image 2 */
	
	for (i = 1; i < (int)xmw; i++) {
	    desty = xmw-i;
	    for (j = 1; j < (int)xmh; j++)
		XPutPixel(image2, j, desty, XGetPixel(image1, i, j));
	}
	
/* Now that image2 contains the rotated text, put it onto the screen */
	
/* restore the clip region */
	
	if (region != NULL)
	    Hv_SetClippingRegion(region);
	
	if (str->fillcol >= 0)
	    Hv_FillRect(&area, str->fillcol);
	
	XPutImage(Hv_display, Hv_mainWindow, Hv_gc, image2, 1, 1,
		  area.left, area.top, xmh, xmw); 
	Hv_Flush();
	
/* free memory used by images and pixmaps */
	
	Hv_FreePixmap(pmap2);
	XDestroyImage(image2);
	XDestroyImage(image1);
	Hv_FreePixmap(pmap1);
	
    }
    
    XmStringFree(xmst);
    Hv_SetForeground(oldfg);
}

/*--------- Hv_DrawGCLine ---------*/

void Hv_DrawGCLine(Hv_View View,
		   float   lat1,
		   float   long1,
		   float   lat2,
		   float   long2,
		   int     np,
		   short   color) {

/*
 * Connects two lat long points (radians) with
 * np intermediate points along the same great
 * circle azimuth. The bigger np, the smoother the
 * line, and the better it will "wrap" near the
 * edge of a map. This routine only operates on
 * map views.
 */

    float    *latitude;
    float    *longitude;
    short     ndots;
    Hv_Point  *xp;
    int       i;

    if ((View == NULL) || !Hv_IsMapView(View) || (np < 1))
	return;
    

    latitude  = (float *)Hv_Malloc(np*sizeof(float));
    longitude = (float *)Hv_Malloc(np*sizeof(float));

    
    Hv_MakeGCLine(lat1,
		  long1,
		  lat2,
		  long2,
		  np,
		  latitude,
		  longitude);

    ndots = (short)np + 2;
    xp = Hv_NewPoints(ndots);

    Hv_LatLongToLocal(View, &(xp[0].x), &(xp[0].y), lat1, long1);
    Hv_LatLongToLocal(View, &(xp[ndots-1].x), &(xp[ndots-1].y), lat2, long2);

    for (i = 0; i <= np; i++)
	Hv_LatLongToLocal(View, &(xp[i].x), &(xp[i].y),
			  latitude[i-1],
			  longitude[i-1]);
    
    Hv_DrawLines(xp, ndots, color);

    Hv_Free(xp);
    Hv_Free(latitude);
    Hv_Free(longitude);
}

/* ------------- Hv_DrawLine ----------- */

void	Hv_DrawLine(short x1,
		    short y1,
		    short x2,
		    short y2,
		    short color)
    
/* Draw line from  x1 y1  to  x2 y2 in specified color.
   "color" is a pixel value into the color map. In our
   case it is either one of the constant colors such
   as Hv_black or Hv_gray12 or Hv_colors[index] */
    
{
    unsigned long   old_fg;
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
    
    if (Hv_inPostscriptMode) {
	Hv_LocalToPSLocal(x1, y1, &x1, &y1);
	Hv_LocalToPSLocal(x2, y2, &x2, &y2);
	Hv_PSDrawLine(x1, y1, x2, y2, color);
	return;
    }
    
    
    old_fg = Hv_SwapForeground(color);
    XDrawLine(Hv_display, Hv_mainWindow, Hv_gc, x1, y1, x2, y2);  /* draw the line */
    Hv_SetForeground(old_fg);                  /* revert the color to thd old foreground color */
}

/* ------------- Hv_FrameRect ------*/

void Hv_FrameRect (Hv_Rect *rect,
		   short    color)
    
/* Frame a rectangle in specified color */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    Hv_Rect         psr;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(rect, &psr);
	Hv_PSFrameRect(&psr, color);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XDrawRectangle(Hv_display, Hv_mainWindow, Hv_gc, rect->left, rect->top, rect->width, rect->height); /* draw the rectangle */
    Hv_SetForeground(old_fg);                  /* revert to the old foreground color */
}

/* ------------- Hv_DrawSegments ----------- */

void Hv_DrawSegments(Hv_Segment *xseg,
		     short       nseg,
		     short       color)
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
    
{
    unsigned long   old_fg;
    XSegment        *psseg;
    
    if (Hv_inPostscriptMode) {
	psseg = (XSegment *)Hv_Malloc(nseg*sizeof(XSegment));
	Hv_LocalSegmentsToPSLocalSegments(xseg, psseg, nseg);
	Hv_PSDrawSegments(psseg, nseg, color);
	Hv_Free(psseg);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XDrawSegments(Hv_display, Hv_mainWindow, Hv_gc, xseg, nseg);
    Hv_SetForeground(old_fg);
}

/* ------------- Hv_DrawLines ----------- */

void Hv_DrawLines(Hv_Point  *xp,
		  short      np,
		  short      color)
    
/*  connects the dots */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    Hv_Point       *psxp;
    
    if (Hv_inPostscriptMode) {
	psxp = Hv_NewPoints((int)np);
	Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
	Hv_PSDrawLines(psxp, np, color);
	Hv_Free(psxp);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XDrawLines(Hv_display, Hv_mainWindow, Hv_gc, xp, np, CoordModeOrigin);
    Hv_SetForeground(old_fg);
}

/* ------------- Hv_FramePolygon ----------- */

void Hv_FramePolygon(Hv_Point *xp,
		     short     np,
		     short     color)
    
/*  frames the poloygon */
    
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    Hv_Point   *psxp;
    
    if (Hv_inPostscriptMode) {
	psxp = Hv_NewPoints((int)np);
	Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
	Hv_PSFramePolygon(psxp, np, color);
	Hv_Free(psxp);
	return;
    }
    
    Hv_DrawLines(xp, np, color);
    Hv_DrawLine(xp[np-1].x, xp[np-1].y, xp[0].x, xp[0].y, color);     
}

/* ------------ Hv_FillPolygon ------------- */

void Hv_FillPolygon(Hv_Point   *xp,
		    short       np,
		    Boolean     frame,
		    short       fillcolor,
		    short       framecolor)
    
/* fills and optionally frames the poloygon
   
   NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{ 
    unsigned long   old_fg;
    Hv_Point       *psxp;
    
    
    if (Hv_inPostscriptMode) {
	psxp = Hv_NewPoints((int)np);
	Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
	
	if (fillcolor >= 0)
	    Hv_PSFillPolygon(psxp, np, fillcolor);
	
	if (frame)
	    Hv_PSFramePolygon(psxp, np, framecolor);
	Hv_Free(psxp);
	return;
    }
    
    
    old_fg = Hv_SwapForeground(fillcolor);
    
/* if its a "good" polygon, draw it using XFillPolygon. However, this seems to have
   a bug: if one or more vertices is outside the root window (a "bad" polygon) , the fill is distorted
   in which case we fill via the slower "FillRegion". When R4 is out, check this bug*/
    
    if (fillcolor >= 0)
	XFillPolygon(Hv_display, Hv_mainWindow, Hv_gc, xp, np, Nonconvex, CoordModeOrigin); 
    
    if (frame)
	Hv_FramePolygon(xp, np, framecolor);
    
    Hv_SetForeground(old_fg);
}

/* ------------- Hv_FrameWedge ----------- */

void   Hv_FrameWedge(short xc,
		     short yc,
		     short rad,
		     int   start,
		     int   del,
		     short color)
    
    
{
    short  diameter;
    unsigned long   old_fg;
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
    if (Hv_inPostscriptMode) {
	Hv_LocalToPSLocal(xc, yc, &xc, &yc);
	rad = (short)(rad*Hv_psResolutionFactor);
	Hv_PSFrameWedge(xc, yc, rad, start, del, color);
	return;
    }
    
    diameter = 2*rad;
    
    old_fg = Hv_SwapForeground(color);
    XDrawArc (Hv_display, Hv_mainWindow, Hv_gc,
	      xc-rad, yc-rad, diameter, diameter, 64*start, 64*del);
    Hv_SetForeground(old_fg);
}


/* ------------- Hv_FrameCircle ----------- */

void Hv_FrameCircle(short xc,
		    short yc,
		    short rad,
		    short color)
    
/*  frames a circle */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    short           diameter;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalToPSLocal(xc, yc, &xc, &yc);
	rad = (short)(rad*Hv_psResolutionFactor);
	Hv_PSFrameCircle(xc, yc, rad, color);
	return;
    }
    
    diameter = 2*rad;
    old_fg = Hv_SwapForeground(color);
    XDrawArc (Hv_display, Hv_mainWindow, Hv_gc, xc-rad, yc-rad, diameter, diameter, 0, 23040);
    Hv_SetForeground(old_fg);
}

/* ------------ Hv_FillCircle ------------- */

void Hv_FillCircle(short   xc,
		   short   yc,
		   short   rad,
		   Boolean frame,
		   short   fillcolor,
		   short   framecolor)
    
/* fills and optionally frames circle */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{ 
    unsigned long   old_fg;
    short           diameter;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalToPSLocal(xc, yc, &xc, &yc);
	rad = (short)(rad*Hv_psResolutionFactor);
	if (fillcolor >= 0)
	    Hv_PSFillCircle(xc, yc, rad, fillcolor);
	
	if (frame)
	    Hv_PSFrameCircle(xc, yc, rad, framecolor);
	return;
    }
    
    diameter = 2*rad;
    old_fg = Hv_SwapForeground(fillcolor);
    
    if (fillcolor >= 0)
	XFillArc (Hv_display, Hv_mainWindow, Hv_gc, xc-rad, yc-rad, diameter, diameter, 0, 23040);
    
    
    if (frame)
	Hv_FrameCircle(xc, yc, rad, framecolor);
    
    Hv_SetForeground(old_fg);
}

/*------------ Hv_FrameOval ------------*/

void            Hv_FrameOval(Hv_Rect *rect,
			     short    color)
    
/*****************************************
  Simply frames an oval defined by given
  rect in specifued color.
  
  NOTE: This is a lowest level Hv draw routine
  and thus CHECKS FOR POSTSCRIPT MODE 
  ******************************************/
    
{
    unsigned long   old_fg;
    Hv_Rect         psr;
    
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(rect, &psr);
	Hv_PSFrameOval(&psr, color);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XDrawArc (Hv_display, Hv_mainWindow, Hv_gc,
	      rect->left, rect->top, rect->width,  rect->height, 0, 23040);
    Hv_SetForeground(old_fg);
    
}

/*--------- Hv_FillOval ----------------*/


void 	       Hv_FillOval(Hv_Rect *rect,
			   Boolean  frame,
			   short    fillcolor,
			   short    framecolor)
    
/*****************************************
  Fills an optionally frames the oval
  
  NOTE: This is a lowest level Hv draw routine
  and thus CHECKS FOR POSTSCRIPT MODE 
  ******************************************/
    
{
    unsigned long   old_fg;
    Hv_Rect         psr;
    
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(rect, &psr);
	Hv_PSFillOval(&psr, fillcolor);
	
	if (frame)
	    Hv_PSFrameOval(&psr, framecolor);
	return;
    }
    
    if (fillcolor >= 0) {
	old_fg = Hv_SwapForeground(fillcolor);
	XFillArc (Hv_display, Hv_mainWindow, Hv_gc,
		  rect->left, rect->top, rect->width,
		  rect->height, 0, 23040);
	Hv_SetForeground(old_fg);
    }
    
    if (frame)
	Hv_FrameOval(rect, framecolor);
}


/* --------- Hv_FillRect ----------- */

void	Hv_FillRect(Hv_Rect *rect,
		    short    color)
    
/* paint rect in specified color*/
    
/* paint a rectangle a color specified by color ( a color index) */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    Hv_Rect         psr;
    
    if (color < 0)
	return;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalRectToPSLocalRect(rect, &psr);
	Hv_PSFillRect(&psr, color);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XFillRectangle(Hv_display, Hv_mainWindow, Hv_gc, rect->left, rect->top, rect->width, rect->height);
    Hv_SetForeground(old_fg);
}


/* --------- Hv_FillPatternRect ----------- */

void	Hv_FillPatternRect(Hv_Rect *rect,
			   short    color,
			   short    pattern)
    
{
    if ((pattern < -1) || (pattern >= Hv_HOLLOWPAT))
	return;
    
    Hv_SetPattern(pattern, color);
    Hv_FillRect(rect, color);
    Hv_SetPattern(Hv_FILLSOLIDPAT, 0);
}

/* -------- Hv_DrawWorldCircle ------------ */

void    Hv_DrawWorldCircle(Hv_View View,
			   float   fx,
			   float   fy,
			   float   frad,
			   Boolean Fill,
			   short   framecolor,
			   short   fillcolor)
    
/* draws a circle based on world information
   
   Hv_View   View         View pointer
   float     fx, fy       center
   float     frad         world radius
   Boolean   Fill         if True, then fill too
   short     framecolor   color of circle (frame)
   short     fillcolor    color of circle (fill)
   
   */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
    
{
    short         radx, rady;       /* pixel radii */
    Hv_Point      P;                /* pixel Position */
    int           angle = 23040;    /* 360*64 */
    int           rad2x, rad2y;             /* twice the pixel rad */
    unsigned long old_fg;
    
    if (Hv_inPostscriptMode) {
	Hv_PSDrawWorldCircle(View, fx, fy, frad, Fill, framecolor, fillcolor);
	return;
    }
    
    Hv_WorldToLocal(View, fx, fy, &P.x, &P.y);
    
    radx = View->local->width*frad/View->world->width;
    rady = View->local->height*frad/View->world->height;
    
    radx = abs(radx);
    rady = abs(rady);
    
    rad2x = 2*radx;
    rad2y = 2*rady;
    
    if (Fill && (fillcolor >= 0)) {
	old_fg = Hv_SwapForeground(fillcolor);    /* swap & store the old foregound color */
	XFillArc (Hv_display, Hv_mainWindow, Hv_gc, (P.x-radx), (P.y-rady), rad2x, rad2y, 0, angle);
	Hv_SetForeground(old_fg);         /* resets the forground to the original value */
    }
    
    old_fg = Hv_SwapForeground(framecolor);    /* swap & store the old foregound color */
    XDrawArc (Hv_display, Hv_mainWindow, Hv_gc, (P.x-radx), (P.y-rady), rad2x, rad2y, 0, angle);
    Hv_SetForeground(old_fg);         /* resets the forground to the original value */
}

/* -------- Hv_DrawPoint ------------------ */

void Hv_DrawPoint(short x,
		  short y,
		  short color)
    
/* ----- Draws a single point. */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long old_fg;
    
    if (Hv_inPostscriptMode) {
	Hv_LocalToPSLocal(x, y, &x, &y);
	Hv_PSDrawPoint(x, y, color);
	return;
    }
    
/* draws one pixel point on the screen of a given color at (x,y) */
    
    old_fg = Hv_SwapForeground(color);    /* swap & store the old foregound color */
    XDrawPoint(Hv_display, Hv_mainWindow, Hv_gc, x, y);   /* draws the specified point */
    Hv_SetForeground(old_fg);         /* resets the forground to the original value */
}

/* -------- Hv_DrawPoints ------------------ */

void Hv_DrawPoints(Hv_Point *xp,
		   int       np,
		   short     color)
    
{
    unsigned long  old_fg;
    Hv_Point      *psxp;
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
    if (Hv_inPostscriptMode) {
	psxp = Hv_NewPoints(np);
	Hv_LocalPolygonToPSLocalPolygon(xp, psxp, np);
	Hv_PSDrawPoints(psxp, np, color);
	Hv_Free(psxp);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);    /* swap & store the old foregound color */
    
    XDrawPoints(Hv_display,
		Hv_mainWindow,
		Hv_gc,
		xp,
		np,
		CoordModeOrigin);
    
    Hv_SetForeground(old_fg);         /* resets the forground to the original value */
}

/*------ Hv_DrawSymbolsOnPoints ------*/

void  Hv_DrawSymbolsOnPoints(Hv_Point  *xp,
			     int       np,
			     short     size,
			     short     color,
			     short     symbol)
    
{
    int               i;
    short             size2;
    Hv_Rect           rect;
    Hv_Point          pts[7];
    XRectangle       *rects;
    unsigned long     oldfg;
    
    
    if (symbol == Hv_NOSYMBOL)
	return;
    
    if (symbol == Hv_BIGPOINTSYMBOL) {
	Hv_DrawSymbolsOnPoints(xp, np, 3, color, Hv_RECTSYMBOL);
	return;
    }
    
/* see if it is a user draw - signified
   by a symbol > 999 and a non null
   Hv_UserDrawSymbolsOnPoints */
    
    if ((symbol > 999) && (Hv_UserDrawSymbolsOnPoints != NULL)) {
	Hv_UserDrawSymbolsOnPoints(xp, np, size, color, symbol);
	return;
    }
    
    if ((size < 2) || (symbol == Hv_POINTSYMBOL)) {
	Hv_DrawPoints(xp, np, color);
	return;
    }
    else if (size < 4) {
	if (Hv_inPostscriptMode) {
	    for (i = 0; i < np; i++) {
		Hv_SetRect(&rect, xp[i].x - 1, xp[i].y - 1, 2, 2);
		Hv_FillRect(&rect, color);
	    }
	}
	else {
	    oldfg = Hv_SwapForeground(color);
	    rects = (XRectangle *)Hv_Malloc(np*sizeof(XRectangle));
	    for (i = 0; i < np; i++) 
		Hv_SetXRectangle(rects+i, xp[i].x - 1, xp[i].y - 1, 3, 3);
	    
	    XFillRectangles(Hv_display, Hv_mainWindow, Hv_gc, rects, np);
	    Hv_Free(rects);
	    Hv_SetForeground(oldfg);
	}
	return;
    }
    
/* enforce a minimum symbolsize */
    
    size = Hv_sMax(size, 4);
    
    size2 = size/2;
    
    if ((!Hv_simpleSymbols) && (symbol == Hv_CROSSSYMBOL))
	Hv_SetLineWidth(2);
    
    for (i = 0; i < np; i++) {
	
	if (symbol == Hv_RECTSYMBOL) {  /* rects */
	    Hv_SetRect(&rect, xp[i].x - size2, xp[i].y - size2, size, size);
	    
	    if (Hv_simpleSymbols) {
		if (color >= 0)
		    Hv_FillRect(&rect, color);
		Hv_FrameRect(&rect, Hv_black);
	    }
	    else
		Hv_Simple3DRect(&rect, False, color);
	}
	
	else if (symbol == Hv_CIRCLESYMBOL)  /* circles */
	    Hv_FillCircle(xp[i].x, xp[i].y, size2, True, color, Hv_black);
	
	else if (symbol == Hv_XSYMBOL) {  /* an "x" */
	    Hv_DrawLine(xp[i].x-size2, xp[i].y-size2, xp[i].x+size2, xp[i].y+size2, color);
	    Hv_DrawLine(xp[i].x-size2, xp[i].y+size2, xp[i].x+size2, xp[i].y-size2, color);
	}

	else if (symbol == Hv_DIAMONDSYMBOL)  /* diamonds*/
	    Hv_Simple3DDiamond(xp[i].x, xp[i].y, size, color, False);
	
	else if (symbol == Hv_TRIANGLEUPSYMBOL) {  /* traiangle with point up */
	    Hv_SetPoint(pts,   xp[i].x - size2, xp[i].y + size2);
	    Hv_SetPoint(pts+1, xp[i].x,      xp[i].y - size2);
	    Hv_SetPoint(pts+2, xp[i].x + size2 + 1, xp[i].y + size2);
	    Hv_FillPolygon(pts, 3, True, color, Hv_black);
	    if (!Hv_simpleSymbols)
		Hv_DrawLine(pts[2].x, pts[2].y, pts[0].x, pts[0].y, Hv_fancySymbolLite);
	}
	
	else if (symbol == Hv_TRIANGLEDOWNSYMBOL) {  /* traiangle with point down */
	    Hv_SetPoint(pts,   xp[i].x - size2, xp[i].y - size2);
	    Hv_SetPoint(pts+1, xp[i].x,      xp[i].y + size2);
	    Hv_SetPoint(pts+2, xp[i].x + size2 + 1, xp[i].y - size2);
	    if (Hv_simpleSymbols)
		Hv_FillPolygon(pts, 3, True, color, Hv_black);
	    else {
		Hv_FillPolygon(pts, 3, True, color, Hv_fancySymbolLite);
		Hv_DrawLine(pts[2].x, pts[2].y, pts[0].x, pts[0].y, Hv_black);
	    }
	}
	
	else if (symbol == Hv_BLOCKISYMBOL)
	    Hv_BlockISymbol(xp[i].x, xp[i].y, size, color, Hv_black);
	
	else if (symbol == Hv_BLOCKHSYMBOL)
	    Hv_BlockHSymbol(xp[i].x, xp[i].y, size, color, Hv_black);
	
	else if (symbol == Hv_UPARROWSYMBOL) {
	    Hv_SetPoint(pts,   xp[i].x-2, xp[i].y+4);
	    Hv_SetPoint(pts+1, xp[i].x-2, xp[i].y);
	    Hv_SetPoint(pts+2, xp[i].x-5, xp[i].y);
	    Hv_SetPoint(pts+3, xp[i].x,   xp[i].y-5);
	    Hv_SetPoint(pts+4, xp[i].x+5, xp[i].y);
	    Hv_SetPoint(pts+5, xp[i].x+2, xp[i].y);
	    Hv_SetPoint(pts+6, xp[i].x+2, xp[i].y+4);
	    Hv_FillPolygon(pts, 7, True, color, Hv_orange);
	}
	
	else if (symbol == Hv_DOWNARROWSYMBOL) {
	    Hv_SetPoint(pts,   xp[i].x-2, xp[i].y-4);
	    Hv_SetPoint(pts+1, xp[i].x-2, xp[i].y);
	    Hv_SetPoint(pts+2, xp[i].x-5, xp[i].y);
	    Hv_SetPoint(pts+3, xp[i].x,   xp[i].y+5);
	    Hv_SetPoint(pts+4, xp[i].x+5, xp[i].y);
	    Hv_SetPoint(pts+5, xp[i].x+2, xp[i].y);
	    Hv_SetPoint(pts+6, xp[i].x+2, xp[i].y-4);
	    Hv_FillPolygon(pts, 7, True, color, Hv_orange);
	}
	
	else if (symbol == Hv_LEFTARROWSYMBOL) {
	    Hv_SetPoint(pts,   xp[i].x+4, xp[i].y+2);
	    Hv_SetPoint(pts+1, xp[i].x,   xp[i].y+2);
	    Hv_SetPoint(pts+2, xp[i].x,   xp[i].y+5);
	    Hv_SetPoint(pts+3, xp[i].x-5, xp[i].y);
	    Hv_SetPoint(pts+4, xp[i].x,   xp[i].y-5);
	    Hv_SetPoint(pts+5, xp[i].x,   xp[i].y-2);
	    Hv_SetPoint(pts+6, xp[i].x+4, xp[i].y-2);
	    Hv_FillPolygon(pts, 7, True, color, Hv_orange);
	}
	
	else if (symbol == Hv_RIGHTARROWSYMBOL) {
	    Hv_SetPoint(pts,   xp[i].x-4, xp[i].y+2);
	    Hv_SetPoint(pts+1, xp[i].x,   xp[i].y+2);
	    Hv_SetPoint(pts+2, xp[i].x,   xp[i].y+5);
	    Hv_SetPoint(pts+3, xp[i].x+5, xp[i].y);
	    Hv_SetPoint(pts+4, xp[i].x,   xp[i].y-5);
	    Hv_SetPoint(pts+5, xp[i].x,   xp[i].y-2);
	    Hv_SetPoint(pts+6, xp[i].x-4, xp[i].y-2);
	    Hv_FillPolygon(pts, 7, True, color, Hv_orange);
	}
	
	else {  /* crosses */
	    if (Hv_simpleSymbols) {
		Hv_DrawLine(xp[i].x-size2, xp[i].y, xp[i].x+size2, xp[i].y, color);
		Hv_DrawLine(xp[i].x, xp[i].y-size2, xp[i].x, xp[i].y+size2, color);
	    }
	    else {
		Hv_SetRect(&rect, xp[i].x - 1, xp[i].y - size2, 2, size);
		Hv_FrameRect(&rect, Hv_black);
		Hv_SetRect(&rect, xp[i].x-size2, xp[i].y - 1, size, 2);
		Hv_FrameRect(&rect, Hv_black);
		if (color >= 0) {
		    Hv_SetRect(&rect, xp[i].x - size2, xp[i].y - size2, size, size);
		    Hv_DrawLine(rect.left, xp[i].y, rect.right, xp[i].y, color);
		    Hv_DrawLine(xp[i].x, rect.top, xp[i].x, rect.bottom, color);
		}
	    }
	}
	
    }
    
    Hv_SetLineWidth(0);
}


/*------ Hv_BlockISymbol ----------*/ 

void  Hv_BlockISymbol(short     x,
		      short     y,
		      short     size,
		      short     fillcolor,
		      short     framecolor)
{
    Hv_Point     pts[12];
    short        size2;
    
    size2 = size/2;
    
    Hv_SetPoint(pts,    x-size2,   y-size2);
    Hv_SetPoint(pts+1,  x+size2,   y-size2);
    Hv_SetPoint(pts+2,  x+size2,   y-2);
    Hv_SetPoint(pts+3,  x+2,       y-2);
    Hv_SetPoint(pts+4,  x+2,       y+2);
    Hv_SetPoint(pts+5,  x+size2,   y+2);
    Hv_SetPoint(pts+6,  x+size2,   y+size2);
    Hv_SetPoint(pts+7,  x-size2,   y+size2);
    Hv_SetPoint(pts+8,  x-size2,   y+2);
    Hv_SetPoint(pts+9,  x-2,       y+2);
    Hv_SetPoint(pts+10, x-2,       y-2);
    Hv_SetPoint(pts+11, x-size2,   y-2);
    Hv_FillPolygon(pts, 12, True, fillcolor, framecolor);
}


/*------ Hv_BlockHSymbol ----------*/

void  Hv_BlockHSymbol(short     x,
		      short     y,
		      short     size,
		      short     fillcolor,
		      short     framecolor)
{
    Hv_Point     pts[12];
    short        size2;
    
    size2 = size/2;
    
    Hv_SetPoint(pts,    x-size2,   y-size2);
    Hv_SetPoint(pts+1,  x-2,       y-size2);
    Hv_SetPoint(pts+2,  x-2,       y-2);
    Hv_SetPoint(pts+3,  x+2,       y-2);
    Hv_SetPoint(pts+4,  x+2,       y-size2);
    Hv_SetPoint(pts+5,  x+size2,   y-size2);
    Hv_SetPoint(pts+6,  x+size2,   y+size2);
    Hv_SetPoint(pts+7,  x+2,       y+size2);
    Hv_SetPoint(pts+8,  x+2,       y+2);
    Hv_SetPoint(pts+9,  x-2,       y+2);
    Hv_SetPoint(pts+10, x-2,       y+size2);
    Hv_SetPoint(pts+11, x-size2,   y+size2);
    Hv_FillPolygon(pts, 12, True, fillcolor, framecolor);
}

/*------ Hv_Simple3DDiamond ----------*/

void Hv_Simple3DDiamond(short     x,
			short     y,
			short     size,
			short     fillcolor,
			Boolean   out)
{
    short    size2;
    Hv_Rect  rect;
    Hv_Point pts[4];
    short    tc, bc;
    
    size2 = size/2;
    size = 2*size2;
    
    if (out) {
	tc =  Hv_fancySymbolLite;
	bc = Hv_black;
    }
    else {
	tc = Hv_black;
	bc =  Hv_fancySymbolLite;
    }
    
    Hv_SetRect(&rect, x - size2, y - size2, size, size);
    Hv_SetPoint(&pts[0], rect.left,  y);
    Hv_SetPoint(&pts[1], x,          rect.top);
    Hv_SetPoint(&pts[2], rect.right, y);
    Hv_SetPoint(&pts[3], x,          rect.bottom);
    
    if (Hv_simpleSymbols)
	Hv_FillPolygon(pts, 4, True, fillcolor, Hv_black);
    else {
	Hv_FillPolygon(pts, 4, True, fillcolor, fillcolor);
	Hv_DrawLine(pts[0].x,   pts[0].y-1, pts[1].x,   pts[1].y-1, tc);
	Hv_DrawLine(pts[1].x,   pts[1].y-1, pts[2].x,   pts[2].y-1, tc);
	Hv_DrawLine(pts[0].x,   pts[0].y,   pts[3].x,   pts[3].y,   bc);
	Hv_DrawLine(pts[3].x,   pts[3].y,   pts[2].x+1, pts[2].y-1, bc);
    }
}

/*------ Hv_Simple3DCircle ----------*/

void Hv_Simple3DCircle(short     xc,
		       short     yc,
		       short     rad,
		       short     fillcolor,
		       Boolean   out)
{
    short    tc, bc;
    short    d = rad*2;
    
    if (out) {
	tc = Hv_gray14;
	bc = Hv_black;
    }
    else {
	tc = Hv_black;
	bc = Hv_gray14;
    }
    

    if (out)
	Hv_FillCircle(xc, yc, rad, False, fillcolor, fillcolor);
    else {
	Hv_FillCircle(xc, yc, rad, True, Hv_gray10, Hv_gray10);
	Hv_FillCircle(xc, yc, rad-2, False, fillcolor, fillcolor);
    }
    Hv_FrameArc(xc-rad, yc-rad, d, d, 2880,  11520, tc);
    Hv_FrameArc(xc-rad, yc-rad, d, d, 2880, -11520, bc);
}

/*========= higher level routines ========*/


/* --------- Hv_FrameArc ------------*/

void	Hv_FrameArc(short x,
		    short y,
		    short width,
		    short height,
		    int   ang1,
		    int   ang2,
		    short color)
    
/* frame arc in specified color
   
   short     x			left pixel coordinate of bounding rect
   short     y		        top  pixel coordinate of bounding rect
   short     width			pixel width  of bounding rect
   short     height		pixel height of boundingrect
   int       ang1                  start angle (in 64ths of a deg)
   int       ang2                  delta angle
   short     color			color index of frame color
   
   */
    
/* frame an arc  specified by color ( a color index) */
    
{
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XDrawArc(Hv_display, Hv_mainWindow, Hv_gc, x, y, width, height, ang1, ang2);
    Hv_SetForeground(old_fg);
}

/* --------- Hv_FillArc ------------*/

void	Hv_FillArc(short x,
		   short y,
		   short width,
		   short height,
		   int   ang1,
		   int   ang2,
		   Boolean frame,
		   short color,
		   short framec)
    
/* fill arc in specified color
   
   short     x			left pixel coordinate of bounding rect
   short     y		        top  pixel coordinate of bounding rect
   short     width			pixel width  of bounding rect
   short     height		pixel height of boundingrect
   int       ang1                  start angle (in 64ths of a deg)
   int       ang2                  delta angle
   short     color			color index of frame color
   
   */
    
/* frame an arc  specified by color ( a color index) */
    
{
    unsigned long   old_fg;
    
    old_fg = Hv_SwapForeground(color);
    XFillArc(Hv_display, Hv_mainWindow, Hv_gc, x, y, width, height, ang1, ang2);
    Hv_SetForeground(old_fg);

    if (frame)
	Hv_FrameArc(x, y, width, height, ang1, ang2, framec);
}

/* --------- Hv_EraseRect ----------- */

void	Hv_EraseRect(Hv_Rect *rect)
    
/* erase specified rect on specified window */
/* by filling the rect with the background color */
    
{
    Hv_EraseRectangle(rect->left, rect->top, rect->width, rect->height);
}

/* --------- Hv_Draw3DCircle -------*/

void Hv_Draw3DCircle(short           xc,
		     short           yc,
		     int             radius,
		     Hv_ColorScheme *scheme)
    
{
    int    i, del, del2;
    int    ang0;
    int    angdel;
    short  mincol, maxcol, col;
    
    Hv_FillCircle(xc, yc, radius, False, scheme->fillcolor, Hv_black);
    
    if (scheme->darktrimcolor > scheme->lighttrimcolor)
	ang0 = 300;
    else
	ang0 = 120;
    
    mincol = Hv_sMin(scheme->darktrimcolor, scheme->lighttrimcolor);
    maxcol = Hv_sMax(scheme->darktrimcolor, scheme->lighttrimcolor);
    
    del = maxcol - mincol;
    if (del == 0)
	return;
    
    del2 = 2*del;
    angdel = 1 + (int)(360.0/del2);
    
    for (i = 0; i < del2; i++) {
	if (i > del)
	    col = mincol + del2 -i;
	else
	    col = mincol + i;
	
	if (col == scheme->fillcolor)
	    if (i > del)
		col++;
	    else
		col--;
	
	Hv_FrameWedge(xc, yc, radius, (ang0 + i*angdel) % 360, angdel, col);
    }
    
}


/* --------- Hv_Draw3DRect ---------*/

void  Hv_Draw3DRect(Hv_Rect        *rect,
		    Hv_ColorScheme *scheme)
    
{
    Hv_Draw3DRectangle(rect->left,
		       rect->top,
		       rect->width,
		       rect->height,
		       scheme);
}

/* -------- Hv_DrawItem --------------- */

void	Hv_DrawItem(Hv_Item   Item,
		    Hv_Region region)
    
/* draws the Item on the View in the region, using the proper color */
    
{
    Hv_Region   drawregion = NULL;
    Hv_Region   diffregion = NULL;
    Hv_Region   newregion = NULL;
    Hv_View     View = Item->view;
    Hv_Region   clipregion = NULL;
    Hv_Rect     carea, psrect;
    Hv_Region   parentregion = NULL;
    
/* many things could cause an immediate return */
    
    if (!Hv_CheckItemToDraw(Item))
	return;
    
    if (Hv_handlingExposure && (Item == Hv_excludeFromExposure))
	return;
    
    if (!Hv_ViewIsVisible(View))
	return;
    
/* if postscript and contents only, only draw hotrect items */
    
    if (Hv_inPostscriptMode && !Hv_psEntireView && (Item->domain == Hv_OUTSIDEHOTRECT))
	return;
    
    if ((Item->domain == Hv_INSIDEHOTRECT) && !Hv_drawingOn)
	return;
    
    if (Hv_drawingOffScreen)
	drawregion = region;
    else {  /* draw on screen */
	
	if (Hv_inPostscriptMode) {
	    if (Item->domain == Hv_OUTSIDEHOTRECT) {
		if (Item == View->containeritem)
		    Hv_CopyRect(&carea, Item->area);
		else
		    Hv_InsideContainerArea(View, &carea);
	    }
	    else
		Hv_CopyRect(&carea, View->hotrect);
	    Hv_LocalRectToPSLocalRect(&carea, &psrect);
	    Hv_PSClipRect(&psrect);
	    
	}  /* end ps mode */
	else {
	    if (region != NULL)
		drawregion = region;
	    else {
		if (Item->domain == Hv_OUTSIDEHOTRECT)
		    drawregion = Hv_GetViewRegion(View);
		else
		    drawregion = Hv_ClipHotRect(View, 1);
	    }  /* end not NULL */
	} /* end !ps mode */
    } /* not offscreen */
    
/* if balloon is up, subtact its region */
    
    if (Hv_balloonUp && (Hv_theBalloon.region != NULL) && !Hv_drawingOffScreen) {
	diffregion = Hv_SubtractRegion(drawregion, Hv_theBalloon.region);
	
	if (drawregion != region)
	    Hv_DestroyRegion(drawregion);
	drawregion = diffregion;
    }
    
/* clip parent ? */
    
    if ((Hv_CheckItemDrawControlBit(Item, Hv_CLIPPARENT) && (Item->parent != NULL))) {
	parentregion = Hv_CopyItemRegion(Item->parent, False);
	Hv_ShrinkRegion(parentregion, 4, 4);
	
	newregion = Hv_IntersectRegion(drawregion, parentregion);
	
	if (drawregion != region)
	    Hv_DestroyRegion(drawregion);
	drawregion = newregion;
	
	Hv_DestroyRegion(parentregion);
    }
    
    Hv_SetClippingRegion(drawregion);
    
/* see if standard draw */
    
    if (Item->standarddraw)
	Item->standarddraw(Item, drawregion);
    
    Hv_SetClippingRegion(drawregion);
    
/* see if user draw */
    
    if (Item->userdraw)
	Item->userdraw(Item, drawregion);
    
/* see if highlight draw (i.e. when editing) */
    
    if ((Hv_HighlightDraw != NULL) && Hv_CheckBit(Item->drawcontrol, Hv_HIGHLIGHTITEM))
	Hv_HighlightDraw(Item, drawregion);
    
/* set foreground color and clip region back */
    
    Hv_DestroyRegion(clipregion);
    
    if (drawregion != region)
	Hv_DestroyRegion(drawregion);
    
    Hv_SetClippingRegion(region);
    Hv_SetForeground(Hv_mainFg);
    
/*  if (Hv_inPostscriptMode)
    Hv_PSRestore();  */
    
    if (Hv_inPostscriptMode)
	Hv_PSFullClip();
}

/* ------ Hv_PSFrameWedge -------*/

static void Hv_PSFrameWedge(short xc,
			    short yc,
			    short rad,
			    int   start,
			    int   del,
			    short color)
    
/*  frames a circle. xc, yc and rad in PS hi res pixels. */
    
{
    float  psx, psy, psrad, fdum, psxo;
    short  sdum = 0;
    
    Hv_PSSetColor(color);
    Hv_PSLocalToPS(xc, yc, &psx, &psy);
    
    Hv_PSLocalToPS(0,   sdum, &psxo,  &fdum);
    Hv_PSLocalToPS(rad, sdum, &psrad, &fdum);
    psrad = fabs(psrad - psxo);
    
    fprintf(Hv_psfp, "newpath %9.3f %9.3f %9.3f %d %d arc stroke\n", psx, psy, psrad, start, start+del);
}


/* ------- Hv_Simple3DRect --------*/

void Hv_Simple3DRect(Hv_Rect *r,
		     Boolean out,
		     short   fc)
    
{
    Hv_Simple3DRectangle(r->left, r->top, r->width, r->height, out, fc);
} 





E 1
