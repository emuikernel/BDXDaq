/**
 * <EM>Deals with nonpublic (internal) drawing primitives.</EM>
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


#include "Hv.h"


/*-------- Hv_GetTemporaryWorld ------------ */

void  Hv_GetTemporaryWorld(Hv_FRect *world,
			   Hv_Rect  *local,
			   Hv_FRect *temp_world,
			   Hv_Rect  *temp_local)
    
/*
  Hv_FRect  *world         NOT MODIFIED 
  Hv_Rect   *local         NOT MODIFIED 
  Hv_FRect  *temp_world
  Hv_Rect   *temp_local    NOT MODIFIED 
  
  Given an overall world and local system,  and a temporary
  local system (e.g. a small rectangle within local), this
  returns the corresponding temporary world, consistent with
  the overall world & local */
    
{
    float     rx,  ry;
    
    rx = world->width/((float)local->width);
    ry = world->height/((float)local->height);
    temp_world->width = rx*temp_local->width;
    temp_world->height = ry*temp_local->height;
    rx = temp_world->width/((float)temp_local->width);
    ry = temp_world->height/((float)temp_local->height);
    
    temp_world->xmin = world->xmin - rx*(local->left - temp_local->left);
    temp_world->ymax = world->ymax - ry*(temp_local->top - local->top);
    temp_world->xmax = temp_world->xmin + temp_world->width;
    temp_world->ymin = temp_world->ymax - temp_world->height;
}


/**
 * Hv_DrawWorldGrid
 * @purpose   Draw a grid (enclosed by a polygon or rect, probably)
 * @param View   The View being drawn on
 * @param warea  A world enclosing rect
 * @param nrow   Number of rows
 * @param ncol   Number of columns
 * @param shrink shrink in pixels
 */

void Hv_DrawWorldGrid(Hv_View   View,
		      Hv_FRect *warea,
		      short     nrow,
		      short     ncol,
		      short     color,
			  short     shrink) {
    
    Hv_Rect     area;
    float       dx, dy, xt, yt;
    short       nseg, xs, ys;
    Hv_Segment *xseg;
    Hv_Segment *tseg;
    int         i;
    
    if ((nrow <= 0) || (ncol <= 0))
	return;
    
    nseg = (nrow - 1) + (ncol - 1);
    if (nseg < 1)
	return;
    
    xseg = (Hv_Segment *)Hv_Malloc(nseg * sizeof(Hv_Segment));
    tseg = xseg;
    
    Hv_WorldRectToLocalRect(View, &area, warea);
    Hv_ShrinkRect(&area, shrink, shrink);
    
    dx = (warea->width)/ncol;
    dy = (warea->height)/nrow;
    
    xt = warea->xmin;
    for (i = 1; i < ncol; i++) {
	xt += dx; 
	Hv_WorldToLocal(View, xt, warea->ymin, &xs, &ys);
	Hv_SetSegment((Hv_Segment *)tseg, xs, area.top, xs, area.bottom);
	tseg++;
    }
    
    yt = warea->ymin;
    for (i = 1; i < nrow; i++) {
	yt += dy; 
	Hv_WorldToLocal(View, warea->xmin, yt, &xs, &ys);
	Hv_SetSegment((Hv_Segment *)tseg, area.left, ys, area.right, ys);
	tseg++;
    }
    
    Hv_DrawSegments(xseg, nseg, color);
    Hv_Free(xseg);
}


/*----------  Hv_PolygonEnclosingRectangle ---------*/

void    Hv_PolygonEnclosingRectangle(Hv_Rectangle *xrect,
				     Hv_Point     *xp,
				     int          npoly)
    
/* returns the rectangle that encloses the polygon */
    
{
    short  left, top, right, bottom;
    int i;
    
    left = xp[0].x;     right = left;
    top  = xp[0].y;     bottom = top;
    
    for (i = 1; i < npoly; i++) {
	left   = Hv_sMin(left,   xp[i].x);
	right  = Hv_sMax(right,  xp[i].x);
	top    = Hv_sMin(top,    xp[i].y);
	bottom = Hv_sMax(bottom, xp[i].y);
    }
    
    Hv_SetXRectangle(xrect, left, top, (short)(right-left), (short)(bottom-top));
}


/* ------- Hv_Simple3DRectangle --------*/


/**
 * Hv_Simple3DRectangle    Draws a simple 3D rectangle.
 * @param      l       left of the rectangle
 * @param      t       top of the rectangle
 * @param      w       witdh of the rectangle
 * @param      h       height of the rectangle
 * @param      out     True, rectangle appears "out". False -> "in".
 * @param      fc      if >= 0, fill using this color index
 */

void Hv_Simple3DRectangle(short   l,
			  short   t,
			  short   w,
			  short   h,
			  Boolean out,
			  short   fc) {
  short  tc, bc;
  short  r, b;
  
  r = l + w;
  b = t + h;
  
  if (out) {
    tc =  Hv_fancySymbolLite;
    bc = Hv_black;
  }
  else {
    tc = Hv_black;
    bc = Hv_fancySymbolLite;
  }
  
  if (fc >= 0)
    Hv_FillRectangle(l, t, w, h, fc);
  
  Hv_DrawLine(l, b, l, t, tc);
  Hv_DrawLine(l, t, r, t, tc);
  Hv_DrawLine(r, t, r, b, bc);
  Hv_DrawLine(r, b, l, b, bc);
}



/**
 * Hv_Draw3DRectangle
 * @purpose  Draw a rectangle with a 3D effect
 * @param    l      Left
 * @param    t      Top
 * @param    r      Right
 * @param    b      Bottom
 * @param    scheme The color scheme
 */

void  Hv_Draw3DRectangle(short l,
			 short t,
			 short w,
			 short h,
			 Hv_ColorScheme *scheme) {

  short         np = 6;
  short         r, b;
  Hv_Point	*xp;		/* array of points */
  short         pixwid, lpw, tpw, rmw, bmw;
  
  r = l + w;
  b = t + h;
  
  if (scheme->framewidth == 1) {
    l--;
    t--;
    xp = Hv_NewPoints(3);
    xp [0].x = l;        xp[0].y = b-1;
    xp [1].x = l;        xp[1].y = t;
    xp [2].x = r;        xp[2].y = t;
    Hv_DrawLines(xp, 3,  scheme->topcolor);
    
    xp [0].x = r;        xp[0].y = t+1;
    xp [1].x = r;        xp[1].y = b;
    xp [2].x = l;        xp[2].y = b;
    
    Hv_DrawLines(xp, 3,  scheme->bottomcolor);
    Hv_Free(xp);
    return;
  }
  
  xp = Hv_NewPoints((int)np);
  pixwid = scheme->framewidth;
  
  lpw = l + pixwid;
  tpw = t + pixwid;
  rmw = r - pixwid;
  bmw = b - pixwid;
  
/* draw the top half */
    
  xp [0].x = l;        xp[0].y = b;
  xp [1].x = lpw;      xp[1].y = bmw;
  xp [2].x = lpw;      xp[2].y = tpw;
  xp [3].x = rmw;      xp[3].y = tpw;
  xp [4].x = r;        xp[4].y = t;  
  xp [5].x = l;        xp[5].y = t;
  
  Hv_FillPolygon(xp, np, FALSE, scheme->topcolor, Hv_black);
    
/* the LightTrim */
    
  if (scheme->framewidth > 1)
    Hv_DrawLines(xp, 5, scheme->lighttrimcolor);
    
/* draw the bottom half -- only 3rd and 6th points change */
    
  xp [2].x = rmw;      xp[2].y = bmw;
  xp [5].x = r;        xp[5].y = b;  
  
  Hv_FillPolygon(xp, np, FALSE, scheme->bottomcolor, Hv_black);
    
/* now the dark trim */
    
  if (scheme->framewidth > 2)
    Hv_DrawLines(xp + 1, 3, scheme->darktrimcolor);
  Hv_Free(xp);
}

/**
 * Hv_DrawXorLine
 * @purpose  Draw a line in XOR mode.
 * @param   p1   The Hv_Point of one end.
 * @param   p2   The Hv_Point of the other end.
 */

void Hv_DrawXorLine(Hv_Point *p1,
		    Hv_Point *p2) {
  int     m_gc;          /* backup of original function */
    
    
  Hv_GetGCValues(&m_gc);  
  Hv_SetFunction(Hv_GXXOR);
  
  Hv_SetLineWidth(2);
  Hv_DrawLine(p1->x, p1->y, p2->x, p2->y, Hv_yellow);  /* draw the line */
  Hv_SetLineWidth(0);
  Hv_SetFunction(m_gc);             /* resets the gc drawing function to the original value */
}


/**
 * Hv_DrawXorLines
 * @purpose  Draw a set of lines in XOR mode.
 * @param   xp   The array of Hv_Points holding the lines.
 * @param   np   The number of points.
 */

void Hv_DrawXorLines(Hv_Point *xp,
		     short np) {
    
  int     m_gc;          /* backup of original function */
    
    
  Hv_GetGCValues(&m_gc);  
  Hv_SetFunction(Hv_GXXOR);
  
  Hv_SetLineWidth(2);
  Hv_DrawLines(xp, np, Hv_yellow);
  Hv_SetLineWidth(0);
  Hv_SetFunction(m_gc);             /* resets the gc drawing function to the original value */
}

/**
 * Hv_FrameXorPolygon
 * @purpose  Draw a polygon in XOR mode.
 * @param   xp   The array of Hv_Points holding the polygon.
 * @param   np   The number of points.
 */

void Hv_FrameXorPolygon(Hv_Point *xp,
			short     np) {
    int     m_gc;          /* backup of original function */
    

    if ((xp == NULL) || (np < 3))
      return;
    
    Hv_GetGCValues(&m_gc);  
    Hv_SetFunction(Hv_GXXOR);
    
    Hv_SetLineWidth(2);
    Hv_FramePolygon(xp, np, Hv_yellow);  /* draw the poly */
    Hv_SetLineWidth(0);
    Hv_SetFunction(m_gc);    /* resets the gc drawing function to the original value */
}

/**
 * Hv_DrawXorRect
 * @purpose  Draw a rectangle in XOR mode.
 * @param   r   The Hv_Rect to draw.
 */

void Hv_DrawXorRect(Hv_Rect *r) {
    
    int             m_gc;         /* backup of original function */
    Hv_Rect         rect;
    
    
    Hv_CopyRect(&rect, r);
    Hv_GoodRect (&rect);  /* returns a proper set of box coords */
    
    Hv_GetGCValues(&m_gc); 
    Hv_SetFunction(Hv_GXXOR);
     
    Hv_SetLineWidth(0);
    
/* draw mutilple single line width lines instead of changing
   the width --- this is faster */
    
    Hv_FrameRect(&rect, Hv_blue);
    Hv_ShrinkRect(&rect, -1, -1);
    Hv_FrameRect(&rect, Hv_green);
    
    Hv_SetFunction(m_gc);             /* resets the gc drawing function to the original value */
}

/**
 * Hv_DrawXorOval
 * @purpose  Draw a rectangle in XOR mode.
 * @param   r   The Hv_Rect enclosing the oval.
 */

void Hv_DrawXorOval(Hv_Rect *r) {
    Hv_Rect         rect;
    int             m_gc;         /* backup of original function */
    int             i;
    
    Hv_CopyRect(&rect, r);
    Hv_GoodRect (&rect);  /* returns a proper set of box coords */
    
    Hv_GetGCValues(&m_gc);  
    Hv_SetFunction(Hv_GXXOR);
    
    Hv_SetLineWidth(0);
    
/* draw mutilple single line width lines instead of changing
   the width --- this is faster */
    
    for (i = 0; i < 2; i++) {
	Hv_FrameArc(rect.left,  rect.top,
		    rect.width, rect.height,
		    0,  23040, Hv_blue); 
	Hv_ShrinkRect(&rect, -1, -1);
    }
    
    Hv_SetFunction(m_gc); 
}

/**
 * Hv_DrawXorItem
 * @purpose  Draw an Item in XOR mode, probably because it is
 * being dragged.
 * @param   Item    The item in question.
 * @param   Region  Clip Region.
 */

void Hv_DrawXorItem(Hv_Item   Item,
					Hv_Region region) {
    
    int             m_gc;           /* backup of original function */
    
    Hv_GetGCValues(&m_gc);  
    Hv_SetFunction(Hv_GXXOR);
    
    Hv_DrawItem(Item, region);
    Hv_SetFunction(m_gc); 
}


/* -------- Hv_DrawClockItem --------------- */

void Hv_DrawClockItem(Hv_Item   Item,
		      Hv_Region region)
    
/*draws the specified Item Hv_CLOCKITEM*/
    
{
    short            xmw, xmh;
    unsigned long    oldfg;
    Hv_Rect          *area;
    Hv_ColorScheme   *scheme;
    Hv_String        *str;
    char             *hvdate;
    
    short            extra, extra2;
    
    area = Item->area;
    scheme = Item->scheme;
    str = Item->str;
    
    hvdate = Hv_GetHvTime();
    
    Hv_FillRect(area, scheme->fillcolor);
    Hv_SetItemText(Item, hvdate);
    Hv_Free(hvdate);
    
/* set up the motif string and get some characteristics */
    
    Hv_CompoundStringDimension(str, &xmw, &xmh);
    
    extra = 2;
    extra2 = 2*extra;
    
    area->width  = xmw + extra2;
    area->height = xmh + extra2;
    Hv_FixRectRB(area);
    
    if (Hv_CheckBit(Item->drawcontrol, Hv_FRAMEAREA))
	Hv_Simple3DRect(area, False, -1);
    
    oldfg = Hv_SwapForeground(str->strcol);                                  /* get the old forground color */
    
/* set some size information then draw the horizontal string */
    
    Hv_DrawCompoundString((short)(area->left + extra), (short)(area->top + extra), str);
    Hv_SetForeground(oldfg);
}


/**
 *  Hv_DrawHotRectBorderItem 
 * @purpose   Draw the hotrect border.
 * @param  Item   The HotRectBorder Item.
 * @param  region Clipping region.
 */

void    Hv_DrawHotRectBorderItem(Hv_Item   Item,
				 Hv_Region region) {
    
  Hv_Rect   area;
  
  Hv_CopyRect(&area, Item->area);

  if (!Hv_CheckBit(Item->view->drawcontrol, Hv_NOHOTRECTFILL))
    Hv_FillRect(&area, Item->color);

  area.left -= 1;
  area.top -= 1;
  area.width += 1;
  area.height += 1;
  Hv_FixRectRB(&area);
  
  Hv_Simple3DRect(&area, False, -1);
}


/*------- Hv_DrawCamera -------*/

void Hv_DrawCamera(Hv_Item   Item,
		   Hv_Region region)
    
/* draws the icon for camera buttons -- used to
   create postscript pictures or to print */     
    
{
    Hv_Rect          area, box;
    short            rad, yt1, yt2, xt1, xt2, xc, yc;
    Hv_Point         xp[4];
    short            DarkColor, LightColor;
    
    Hv_CopyRect(&area, Item->area);
    
    Hv_ShrinkRect(&area, 3, 3);
    
    yt1 = area.top + 3;
    yt2 = area.bottom;
    xt1 = area.left;
    xt2 = area.right;
    xc = (xt1 + xt2)/2;
    yc = (yt1 + yt2)/2;
    rad = 3;
    
/* flash */
    
    Hv_SetPoint(&(xp[0]), (short)(xc-rad-2), yt1);
    Hv_SetPoint(&(xp[1]), (short)(xc-1),     (short)(area.top-1));
    Hv_SetPoint(&(xp[2]), (short)(xc+1),     (short)(area.top-1)); 
    Hv_SetPoint(&(xp[3]), (short)(xc+rad+2), yt1); 
    
    if (Item->state == Hv_OFF) {
	DarkColor = Hv_black;
	LightColor = Hv_white;
	Hv_FillPolygon(xp, 4, True, DarkColor, LightColor);
	Hv_DrawLine((short)(xc-rad+1), (short)(yt1-2), (short)(xc+rad-1), (short)(yt1-2), Hv_aliceBlue);
    }
    else {
	DarkColor = Hv_black;
	LightColor = Hv_yellow;
	Hv_FillPolygon(xp, 4, True, Hv_yellow, LightColor);
    }
    
    Hv_SetRect(&box, xt1, yt1, (short)(xt2-xt1), (short)(yt2-yt1));
    Hv_FillRect(&box, DarkColor);
    
/* main horiz lines */
    
    Hv_DrawLine(xt1,   yt1,     xt2,     yt1,     LightColor);
    Hv_DrawLine((short)(xt1+1), (short)(yt1+1),   xt2,     (short)(yt1+1),   Hv_gray13);
    Hv_DrawLine(xt1,   yt2,     xt2,     yt2,     LightColor);
    Hv_DrawLine((short)(xt1+1), (short)(yt2+1),   (short)(xt2+1),   (short)(yt2+1),   Hv_gray7);
    
/* main vertical lines */
    
    Hv_DrawLine(xt1,  (short)(yt1+1), xt1,       yt2, LightColor);
    Hv_DrawLine((short)(xt1+1),    (short)(yt1+2), (short)(xt1+1),     yt2, Hv_gray9);
    Hv_DrawLine((short)(xt2-1),    (short)(yt1+2), (short)(xt2-1),     yt2, Hv_gray6);
    Hv_DrawLine((short)(xc-rad-1), (short)(yt1+2), (short)(xc-rad-1),  yt2, LightColor);
    Hv_DrawLine((short)(xc+rad+1), (short)(yt1+2), (short)(xc+rad+1),  yt2, LightColor);
    
/* shutter button on left */
    
    Hv_DrawLine((short)(xt1+1), (short)(yt1-1), (short)(xt1+4), (short)(yt1-1), LightColor);
    Hv_DrawLine((short)(xt1+1), (short)(yt1-2), (short)(xt1+4), (short)(yt1-1), LightColor);
    
/* lens */
    
    Hv_FrameCircle(xc, (short)(yc+1), (short)(rad+1), LightColor);
    Hv_DrawLine((short)(xc-1), (short)(yc+1), (short)(xc-1), (short)(yc+3), LightColor);
    Hv_DrawLine((short)(xc-1), yc,   (short)(xc+2), yc,   LightColor);
}
