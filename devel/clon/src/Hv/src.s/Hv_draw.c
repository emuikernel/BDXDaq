/**
 * <EM>Holds many of the platform independent
 * drawing primitives.</EM>
 * <p>
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
#include "Hv_maps.h"

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


/*--------------------------------------
 *
 * Hv_FramePolygon
 *
 * A routine for framing a
 * polygon in a specified color.
 *
 *
 * NOTE this is NOT a gateway function
 * (It only calls Hv functions)
 *
 *--------------------------------------*/


void Hv_FramePolygon(Hv_Point *xp,
		     short     np,
		     short     color) {
    
/*  frames the poloygon */
    
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


/* --------- Hv_FillPatternRect ----------- */

void	Hv_FillPatternRect(Hv_Rect *rect,
			   short    color,
			   short    pattern) {

  if ((pattern < -1) || (pattern >= Hv_HOLLOWPAT))
    return;
  
  Hv_SetPattern(pattern, color);
  Hv_FillRect(rect, color);
  Hv_SetPattern(Hv_FILLSOLIDPAT, 0);
}

/**
 * Hv_DrawWorldCircle  draws a circle based on world coordinates
 * @param  View         the Hv View
 * @param  fx           horizontal center
 * @param  fy           vertical   center
 * @param  frad         radius
 * @param  Fill         if True, then fill
 * @param  framecolor   frame color
 * @param  fillcolor    if filled, the fill color
 */

void    Hv_DrawWorldCircle(Hv_View View,
			   float   fx,
			   float   fy,
			   float   frad,
			   Boolean Fill,
			   short   framecolor,
			   short   fillcolor) {
    
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    

  short         radx, rady;       /* pixel radii */
  Hv_Point      P;                /* pixel Position */
  int           angle = 23040;    /* 360*64 */
  short         rad2x, rad2y;             /* twice the pixel rad */
  short         x, y;
    
  if (Hv_inPostscriptMode) {
    Hv_PSDrawWorldCircle(View, fx, fy, frad, Fill, framecolor, fillcolor);
    return;
  }
    
  Hv_WorldToLocal(View, fx, fy, &P.x, &P.y);
    
  radx = (short)(View->local->width*frad/View->world->width);
  rady = (short)(View->local->height*frad/View->world->height);
  
  radx = abs(radx);
  rady = abs(rady);
  
  rad2x = 2*radx;
  rad2y = 2*rady;

  x = P.x-radx;
  y = P.y-rady;
       
  if (Fill && (fillcolor >= 0))
    Hv_FillArc (x,        y,
		rad2x,    rad2y,
		0,        angle,
		False,    fillcolor, 
		-1);

  Hv_FrameArc(x,        y,
	      rad2x,    rad2y,
	      0, angle, framecolor);
}


/**
 * Hv_FrameWedge
 * @param       xc         the horizontal center
 * @param       yc         the vertical center
 * @param       rad        the pixel radius
 * @param       start      the starting angle in degrees
 * @param       del        the angle subtended in degrees
 * @param       color      the color index for the frame
 */

void   Hv_FrameWedge(short xc,
		     short yc,
		     short rad,
		     int   start,
		     int   del,
		     short color) {

  short  diameter = 2*rad;

  if (Hv_inPostscriptMode) {
    Hv_LocalToPSLocal(xc, yc, &xc, &yc);
    rad = (short)(rad*Hv_psResolutionFactor);
    Hv_PSFrameWedge(xc, yc, rad, start, del, color);
    return;
  }
    
  Hv_FrameArc((short)(xc-rad),
	      (short)(yc-rad),
	      diameter,
	      diameter,
	      (short)(64*start),
	      (short)(64*del),
	      color);
}


/**
 *
 * Hv_FrameCircle
 * @param       xc         the horizontal center
 * @param       yc         the vertical center
 * @param       rad        the pixel radius
 * @param       color      the color index for the frame
 *
 */

void Hv_FrameCircle(short xc,
		    short yc,
		    short rad,
		    short color) {
  
  
  short  diameter = 2*rad;
  
  if (Hv_inPostscriptMode) {
    Hv_LocalToPSLocal(xc, yc, &xc, &yc);
    rad = (short)(rad*Hv_psResolutionFactor);
    Hv_PSFrameCircle(xc, yc, rad, color);
    return;
  }


  Hv_FrameArc((short)(xc-rad),
	      (short)(yc-rad),
	      diameter,
	      diameter,
	      0,
	      23040,
	      color);
}

/**
 *
 * Hv_FillCircle
 * @param       xc         the horizontal center
 * @param       yc         the vertical center
 * @param       rad        the pixel radius
 * @param       frame      if true, then frame
 * @param       fillcolor  the color index for the fill
 * @param       framecolor the color index for the frame
 *
 */

void Hv_FillCircle(short   xc,
		   short   yc,
		   short   rad,
		   Boolean frame,
		   short   fillcolor,
		   short   framecolor) {
  
  short  diameter = 2*rad;

    
  if (Hv_inPostscriptMode) {
    Hv_LocalToPSLocal(xc, yc, &xc, &yc);
    rad = (short)(rad*Hv_psResolutionFactor);
    if (fillcolor >= 0)
      Hv_PSFillCircle(xc, yc, rad, fillcolor);
    
    if (frame)
      Hv_PSFrameCircle(xc, yc, rad, framecolor);
    return;
  }

    
  if (fillcolor >= 0)
    Hv_FillArc((short)(xc-rad),
	       (short)(yc-rad),
	       diameter,
	       diameter,
	       0,
	       23040,
	       False,
	       fillcolor,
	       -1);
    
    
  if (frame)
    Hv_FrameCircle(xc, yc, rad, framecolor);
  
}

/**
 *
 * Hv_FrameOval
 * @param       rect       the bounding rect
 * @param       color the color index for the frame
 *
 */

void Hv_FrameOval(Hv_Rect *rect,
		  short    color) {

    
  Hv_Rect         psr;
    
  if (Hv_inPostscriptMode) {
    Hv_LocalRectToPSLocalRect(rect, &psr);
    Hv_PSFrameOval(&psr, color);
    return;
  }

  Hv_FrameArc(rect->left,
	      rect->top,
	      rect->width,
	      rect->height,
	      0,
	      23040,
	      color);
}

/**
 * Hv_FillOval
 * @param       rect       the bounding rect
 * @param       frame      if true, then frame
 * @param       fillcolor  the color index for the fill
 * @param       framecolor the color index for the frame
 */

void Hv_FillOval(Hv_Rect *rect,
		 Boolean  frame,
		 short    fillcolor,
		 short    framecolor) {

  Hv_Rect         psr;
    
    
  if (Hv_inPostscriptMode) {
    Hv_LocalRectToPSLocalRect(rect, &psr);
    Hv_PSFillOval(&psr, fillcolor);
    
    if (frame)
      Hv_PSFrameOval(&psr, framecolor);
    return;
  }
  
  
  if (fillcolor >= 0) {
    Hv_FillArc(rect->left,
	       rect->top,
	       rect->width,
	       rect->height,
	       0,
	       23040,
	       False,
	       fillcolor,
	       -1);
  }
  
  if (frame)
    Hv_FrameOval(rect, framecolor);
}

/**
 * Hv_DrawSymbolsOnPoints
 * @param      xp
 * @param      np
 * @param      size
 * @param      color
 * @param      symbol
 */

void  Hv_DrawSymbolsOnPoints(Hv_Point  *xp,
			     int       np,
			     short     size,
			     short     color,
			     short     symbol) {
  
  int               i;
  short             size2;
  Hv_Rect           rect;
  Hv_Point          pts[7];
  Hv_Rectangle     *rects;
    
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
	Hv_SetRect(&rect, (short)(xp[i].x - 1), (short)(xp[i].y - 1), 2, 2);
	Hv_FillRect(&rect, color);
      }
    }
    else {
      rects = (Hv_Rectangle *)Hv_Malloc(np*sizeof(Hv_Rectangle));
      for (i = 0; i < np; i++) 
	Hv_SetXRectangle(rects+i, (short)(xp[i].x - 1), (short)(xp[i].y - 1), 3, 3);
      
      Hv_FillRectangles(rects, np, color);
      Hv_Free(rects);
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
      Hv_SetRect(&rect, (short)(xp[i].x - size2), (short)(xp[i].y - size2), size, size);
      
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
      Hv_DrawLine((short)(xp[i].x-size2), (short)(xp[i].y-size2), (short)(xp[i].x+size2), (short)(xp[i].y+size2), color);
      Hv_DrawLine((short)(xp[i].x-size2), (short)(xp[i].y+size2), (short)(xp[i].x+size2), (short)(xp[i].y-size2), color);
    }
    
    else if (symbol == Hv_DIAMONDSYMBOL)  /* diamonds*/
      Hv_Simple3DDiamond(xp[i].x, xp[i].y, size, color, False);
    
    else if (symbol == Hv_TRIANGLEUPSYMBOL) {  /* traiangle with point up */
      Hv_SetPoint(pts,   (short)(xp[i].x - size2), (short)(xp[i].y + size2));
      Hv_SetPoint(pts+1, (short)(xp[i].x),      (short)(xp[i].y - size2));
      Hv_SetPoint(pts+2, (short)(xp[i].x + size2 + 1), (short)(xp[i].y + size2));
      Hv_FillPolygon(pts, 3, True, color, Hv_black);
      if (!Hv_simpleSymbols)
	Hv_DrawLine(pts[2].x, pts[2].y, pts[0].x, pts[0].y, Hv_fancySymbolLite);
    }
    
    else if (symbol == Hv_TRIANGLEDOWNSYMBOL) {  /* traiangle with point down */
      Hv_SetPoint(pts,   (short)(xp[i].x - size2), (short)(xp[i].y - size2));
      Hv_SetPoint(pts+1, (short)(xp[i].x),      (short)(xp[i].y + size2));
      Hv_SetPoint(pts+2, (short)(xp[i].x + size2 + 1), (short)(xp[i].y - size2));
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
      Hv_SetPoint(pts,   (short)(xp[i].x-2), (short)(xp[i].y+4));
      Hv_SetPoint(pts+1, (short)(xp[i].x-2), (short)(xp[i].y));
      Hv_SetPoint(pts+2, (short)(xp[i].x-5), (short)(xp[i].y));
      Hv_SetPoint(pts+3, (short)(xp[i].x),   (short)(xp[i].y-5));
      Hv_SetPoint(pts+4, (short)(xp[i].x+5), (short)(xp[i].y));
      Hv_SetPoint(pts+5, (short)(xp[i].x+2), (short)(xp[i].y));
      Hv_SetPoint(pts+6, (short)(xp[i].x+2), (short)(xp[i].y+4));
      Hv_FillPolygon(pts, 7, True, color, Hv_orange);
    }
    
    else if (symbol == Hv_DOWNARROWSYMBOL) {
      Hv_SetPoint(pts,   (short)(xp[i].x-2), (short)(xp[i].y-4));
      Hv_SetPoint(pts+1, (short)(xp[i].x-2), (short)(xp[i].y));
      Hv_SetPoint(pts+2, (short)(xp[i].x-5), (short)(xp[i].y));
      Hv_SetPoint(pts+3, (short)(xp[i].x),   (short)(xp[i].y+5));
      Hv_SetPoint(pts+4, (short)(xp[i].x+5), (short)(xp[i].y));
      Hv_SetPoint(pts+5, (short)(xp[i].x+2), (short)(xp[i].y));
      Hv_SetPoint(pts+6, (short)(xp[i].x+2), (short)(xp[i].y-4));
      Hv_FillPolygon(pts, 7, True, color, Hv_orange);
    }
    
    else if (symbol == Hv_LEFTARROWSYMBOL) {
      Hv_SetPoint(pts,   (short)(xp[i].x+4), (short)(xp[i].y+2));
      Hv_SetPoint(pts+1, (short)(xp[i].x),   (short)(xp[i].y+2));
      Hv_SetPoint(pts+2, (short)(xp[i].x),   (short)(xp[i].y+5));
      Hv_SetPoint(pts+3, (short)(xp[i].x-5), (short)(xp[i].y));
      Hv_SetPoint(pts+4, (short)(xp[i].x),   (short)(xp[i].y-5));
      Hv_SetPoint(pts+5, (short)(xp[i].x),   (short)(xp[i].y-2));
      Hv_SetPoint(pts+6, (short)(xp[i].x+4), (short)(xp[i].y-2));
      Hv_FillPolygon(pts, 7, True, color, Hv_orange);
    }
	
    else if (symbol == Hv_RIGHTARROWSYMBOL) {
      Hv_SetPoint(pts,   (short)(xp[i].x-4), (short)(xp[i].y+2));
      Hv_SetPoint(pts+1, (short)(xp[i].x),   (short)(xp[i].y+2));
      Hv_SetPoint(pts+2, (short)(xp[i].x),   (short)(xp[i].y+5));
      Hv_SetPoint(pts+3, (short)(xp[i].x+5), (short)(xp[i].y));
      Hv_SetPoint(pts+4, (short)(xp[i].x),   (short)(xp[i].y-5));
      Hv_SetPoint(pts+5, (short)(xp[i].x),   (short)(xp[i].y-2));
      Hv_SetPoint(pts+6, (short)(xp[i].x-4), (short)(xp[i].y-2));
      Hv_FillPolygon(pts, 7, True, color, Hv_orange);
    }
    
    else {  /* crosses */
      if (Hv_simpleSymbols) {
	Hv_DrawLine((short)(xp[i].x-size2), xp[i].y, (short)(xp[i].x+size2), xp[i].y, color);
	Hv_DrawLine(xp[i].x, (short)(xp[i].y-size2), xp[i].x, (short)(xp[i].y+size2), color);
      }
      else {
	Hv_SetRect(&rect, (short)(xp[i].x - 1), (short)(xp[i].y - size2), 2, size);
	Hv_FrameRect(&rect, Hv_black);
	Hv_SetRect(&rect, (short)(xp[i].x-size2), (short)(xp[i].y - 1), size, 2);
	Hv_FrameRect(&rect, Hv_black);
	if (color >= 0) {
	  Hv_SetRect(&rect, (short)(xp[i].x - size2), (short)(xp[i].y - size2), size, size);
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
    
    Hv_SetPoint(pts,    (short)(x-size2),   (short)(y-size2));
    Hv_SetPoint(pts+1,  (short)(x+size2),   (short)(y-size2));
    Hv_SetPoint(pts+2,  (short)(x+size2),   (short)(y-2));
    Hv_SetPoint(pts+3,  (short)(x+2),       (short)(y-2));
    Hv_SetPoint(pts+4,  (short)(x+2),       (short)(y+2));
    Hv_SetPoint(pts+5,  (short)(x+size2),   (short)(y+2));
    Hv_SetPoint(pts+6,  (short)(x+size2),   (short)(y+size2));
    Hv_SetPoint(pts+7,  (short)(x-size2),   (short)(y+size2));
    Hv_SetPoint(pts+8,  (short)(x-size2),   (short)(y+2));
    Hv_SetPoint(pts+9,  (short)(x-2),       (short)(y+2));
    Hv_SetPoint(pts+10, (short)(x-2),       (short)(y-2));
    Hv_SetPoint(pts+11, (short)(x-size2),   (short)(y-2));
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
    
    Hv_SetPoint(pts,    (short)(x-size2),   (short)(y-size2));
    Hv_SetPoint(pts+1,  (short)(x-2),       (short)(y-size2));
    Hv_SetPoint(pts+2,  (short)(x-2),       (short)(y-2));
    Hv_SetPoint(pts+3,  (short)(x+2),       (short)(y-2));
    Hv_SetPoint(pts+4,  (short)(x+2),       (short)(y-size2));
    Hv_SetPoint(pts+5,  (short)(x+size2),   (short)(y-size2));
    Hv_SetPoint(pts+6,  (short)(x+size2),   (short)(y+size2));
    Hv_SetPoint(pts+7,  (short)(x+2),       (short)(y+size2));
    Hv_SetPoint(pts+8,  (short)(x+2),       (short)(y+2));
    Hv_SetPoint(pts+9,  (short)(x-2),       (short)(y+2));
    Hv_SetPoint(pts+10, (short)(x-2),       (short)(y+size2));
    Hv_SetPoint(pts+11, (short)(x-size2),   (short)(y+size2));
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
    
    Hv_SetRect(&rect, (short)(x - size2), (short)(y - size2), size, size);
    Hv_SetPoint(&pts[0], rect.left,  y);
    Hv_SetPoint(&pts[1], x,          rect.top);
    Hv_SetPoint(&pts[2], rect.right, y);
    Hv_SetPoint(&pts[3], x,          rect.bottom);
    
    if (Hv_simpleSymbols)
	Hv_FillPolygon(pts, 4, True, fillcolor, Hv_black);
    else {
	Hv_FillPolygon(pts, 4, True, fillcolor, fillcolor);
	Hv_DrawLine(pts[0].x,   (short)(pts[0].y-1), pts[1].x,   (short)(pts[1].y-1), tc);
	Hv_DrawLine(pts[1].x,   (short)(pts[1].y-1), pts[2].x,   (short)(pts[2].y-1), tc);
	Hv_DrawLine(pts[0].x,   pts[0].y,   pts[3].x,   pts[3].y,   bc);
	Hv_DrawLine(pts[3].x,   pts[3].y,   (short)(pts[2].x+1), (short)(pts[2].y-1), bc);
    }
}

/**
 * Draw a 3D circle.
 * @param xc         Horizontal pixel center
 * @param yc         Vertical pixel center
 * @param rad        Radius in pixels
 * @param fillcolor  The fillcolor (index).
 * @param out        If true, drawn as an "outsie"
 */

void Hv_Simple3DCircle(short     xc,
		       short     yc,
		       short     rad,
		       short     fillcolor,
		       Boolean   out)
 {
    short    tc, bc;
    short    d;
    int      radx;

	d = rad*2;
	radx = rad-4;

    if (out) {
	tc = Hv_white;
	bc = Hv_black;
    }
    else {
	tc = Hv_black;
	bc = Hv_white;
    }
    
    if (out)
	Hv_FillCircle(xc, yc, rad, False, fillcolor, fillcolor);
    else {
      if (fillcolor >= 0) {
	Hv_FillCircle(xc, yc, rad, True, Hv_gray12, Hv_gray12);
	Hv_FillCircle(xc, yc, (short)radx, True, fillcolor, fillcolor);
      }
    }
    Hv_FrameArc((short)(xc-rad), (short)(yc-rad), d, d, 2880,  11520, tc);
    Hv_FrameArc((short)(xc-rad), (short)(yc-rad), d, d, 2880, -11520, bc);

}

/*========= higher level routines ========*/


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
    
    Hv_FillCircle(xc, yc, (short)radius, False, scheme->fillcolor, Hv_black);
    
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
	
	Hv_FrameWedge(xc, yc, (short)radius, (ang0 + i*angdel) % 360, angdel, col);
    }
    
}


/* --------- Hv_DawGhostedText ---------*/

void Hv_DrawGhostedText(int x,
			int y,
			Hv_String *str,
			short     lightcolor,
			short     darkcolor) {

    short   oldcolor;

    if (str == NULL)
	return;
    oldcolor = str->strcol;

    str->strcol = lightcolor;
    Hv_DrawCompoundString((short)x, (short)(y+1), str);


    str->strcol = darkcolor;
    Hv_DrawCompoundString((short)x, (short)y, str);
    str->strcol = oldcolor;
    
}


/**
 * Hv_Draw3DRect
 * @purpose  Draw a rectangle with a 3D effect
 * @param    rect   The rectangle
 * @param    scheme The color scheme
 */

void  Hv_Draw3DRect(Hv_Rect        *rect,
		    Hv_ColorScheme *scheme) {
    
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
    
    if (Hv_balloonUp && 
	(Hv_theBalloon.region != NULL) && 
	!Hv_drawingOffScreen) {
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

/* ------- Hv_Simple3DRect --------*/

void Hv_Simple3DRect(Hv_Rect *r,
		     Boolean out,
		     short   fc)
    
{
    Hv_Simple3DRectangle(r->left, r->top, r->width, r->height, out, fc);
} 





