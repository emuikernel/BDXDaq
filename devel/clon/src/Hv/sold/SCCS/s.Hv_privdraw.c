h16272
s 00000/00000/00000
d R 1.2 02/08/25 23:21:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_privdraw.c
e
s 00709/00000/00000
d D 1.1 02/08/25 23:21:19 boiarino 1 0
c date and time created 02/08/25 23:21:19 by boiarino
e
u
U
f e 0
t
T
I 1
/*	
	-----------------------------------------------------------------------
	File:     Hv_privdraw.c
	==================================================================================
	
	The Hv library was developed at CEBAF under contract to the
	Department of Energy and is the property of the U.S. Government.
	
	It may be used to develop commercial software, but the Hv
	source code and/or compiled modules/libraries may not be sold.
	
	Questions or comments should be directed to heddle@cebaf.gov
	==================================================================================
	*/

#include "Hv.h"


/*------- Hv_DrawString ------*/

void    Hv_DrawString(short x,
		      short y,
		      Hv_String *str)
{
    XmString	    xmst;			/* Motif compound string */
    short             xmw, xmh;		        /* String width  in pixels */
    unsigned long     oldfg;
    char              *fontname;
    float             fontsize, fx, fy;
    Boolean           shadow;
    float             extrah = 0.0;
    float             extraw = 0.0;
    
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
	else  {  /* vertical string */
	    extrah = xmh*0.25*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY;
	    extraw = xmw*Hv_psResolutionFactor*72.0/Hv_PSPIXELDENSITY;
	    Hv_LocalToPSLocal(x+xmh, y, &x, &y);
	    Hv_PSLocalToPS(x, y, &fx, &fy);
	    Hv_PSDrawString(fx - extrah, fy - extraw, fontsize, fontname, str->text,
			    Hv_PSVERTICAL, str->strcol, False, Hv_white);
/*	    Hv_PSDrawString(fx - extrah, fy, fontsize, fontname, str->text,
			    Hv_PSVERTICAL, str->strcol, False, Hv_white);*/
	}
	Hv_Free(fontname);
	return;
    } /* end ps mode */
    
    oldfg = Hv_SwapForeground(str->strcol);
    
    if (Hv_useDrawImage)
	XmStringDrawImage(Hv_display, Hv_mainWindow, Hv_fontList, xmst, Hv_gc, x,
			  y, xmw, XmALIGNMENT_BEGINNING, XmALIGNMENT_BEGINNING, NULL);
    else
	XmStringDraw(Hv_display, Hv_mainWindow, Hv_fontList, xmst, Hv_gc, x,
		     y, xmw, XmALIGNMENT_BEGINNING, XmALIGNMENT_BEGINNING, NULL);
    
    XmStringFree(xmst);
    Hv_SetForeground(oldfg);
}

/* --------- Hv_CheckForExposures --------- */

void Hv_CheckForExposures(int whattype)
    
/* int    whattype  (either Expose or DrawExpose) */
    
{
    XEvent     event;
    Hv_Region  updregion = NULL;
    
    XSync(Hv_display, False); 
    
    while(XCheckTypedWindowEvent(Hv_display, Hv_mainWindow, whattype, &event)) {
	if (!updregion)
	    updregion = Hv_CreateRegion();
	XtAddExposureToRegion(&event, (Region)updregion);
	XSync(Hv_display, False);
    }
    
    if (updregion) {	
	Hv_handlingExposure = True;
	Hv_UpdateViews(updregion);
	Hv_excludeFromExposure = NULL;
	Hv_handlingExposure = False;
	Hv_KillRegion(&updregion);
    }
}

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


/*-------- Hv_DrawWorldGrid --------*/

void Hv_DrawWorldGrid(Hv_View   View,
		      Hv_FRect *warea,
		      short     nrow,
		      short     ncol,
		      short     color,
		      short     shrink)
    
{
    Hv_Rect     area;
    float       dx, dy, xt, yt;
    short       nseg, xs, ys;
    XSegment    *xseg;
    XSegment    *tseg;
    int         i;
    
    if ((nrow <= 0) || (ncol <= 0))
	return;
    
    nseg = (nrow - 1) + (ncol - 1);
    if (nseg < 1)
	return;
    
    xseg = (XSegment *)Hv_Malloc(nseg * sizeof(XSegment));
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

void    Hv_PolygonEnclosingRectangle(XRectangle *xrect,
				     Hv_Point   *xp,
				     int        npoly)
    
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
    
    Hv_SetXRectangle(xrect, left, top, right-left, bottom-top);
}


/* --------- Hv_EraseRectangle ----------- */

void	Hv_EraseRectangle(short left,
			  short top,
			  short width,
			  short height)
    
/* erase specified rect on specified window
   by filling the rect with the background color */
    
{
    unsigned long   old_fg;
    
/*  XClearArea(Hv_display, Hv_mainWindow, left, top, width, height, False); */
    
    old_fg = Hv_SwapForeground(Hv_canvasColor);
    
    if (Hv_TileDraw)
	Hv_SetFillStyle(FillTiled);
    
    XFillRectangle(Hv_display, Hv_mainWindow, Hv_gc, left, top, width, height);
    Hv_SetFillStyle(FillSolid);
    Hv_SetForeground(old_fg); 
}

/* ------- Hv_Simple3DRectangle --------*/

void Hv_Simple3DRectangle(short   l,
			  short   t,
			  short   w,
			  short   h,
			  Boolean out,
			  short   fc)
{
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

/* --------- Hv_FillRectangle ----------- */

void	Hv_FillRectangle(short left,
			 short top,
			 short width,
			 short height,
			 short color)
    
/* paint rect in specified color*/
    
/* paint a rectangle a color specified by color ( a color index) */
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    Hv_Rect         r;
    
    if (Hv_inPostscriptMode) {
	Hv_SetRect(&r, left, top, width, height);
	Hv_LocalRectToPSLocalRect(&r, &r);
	if (color >= 0)
	    Hv_PSFillRect(&r, color);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XFillRectangle(Hv_display, Hv_mainWindow, Hv_gc, left, top, width, height);
    Hv_SetForeground(old_fg);
}

/* ------------- Hv_FrameRectangle ------*/

void Hv_FrameRectangle (short x,
			short y,
			short w,
			short h,
			short color)
    
/* Frame a rectangle in specified color */
    
    
/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */
    
{
    unsigned long   old_fg;
    Hv_Rect         r;
    
    
    if (Hv_inPostscriptMode) {
	Hv_SetRect(&r, x, y, w, h);
	Hv_LocalRectToPSLocalRect(&r, &r);
	Hv_PSFrameRect(&r, color);
	return;
    }
    
    old_fg = Hv_SwapForeground(color);
    XDrawRectangle(Hv_display, Hv_mainWindow, Hv_gc, x, y, w, h); /* draw the rectangle */
    Hv_SetForeground(old_fg);                  /* revert to the old foreground color */
}

/* --------- Hv_Draw3DRectangle ---------*/

void  Hv_Draw3DRectangle(short l,
			 short t,
			 short w,
			 short h,
			 Hv_ColorScheme *scheme)
    
{
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
    
    if (scheme->framewidth > 2)
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

/* ------- Hv_DrawXorLine ------------ */

void Hv_DrawXorLine(Hv_Point *p1,
		    Hv_Point *p2)
    
{
    int     m_gc;          /* backup of original function */
    
    
    XGetGCValues(Hv_display,
		 Hv_gc,
		 GCFunction,
		 (XGCValues *)(&m_gc));   /* copies the current gc funtion value */
    
    XSetFunction(Hv_display, Hv_gc, GXxor);
    
    Hv_SetLineWidth(3);
    Hv_DrawLine(p1->x, p1->y, p2->x, p2->y, Hv_yellow);  /* draw the line */
    Hv_SetLineWidth(0);
    XSetFunction (Hv_display, Hv_gc, m_gc);             /* resets the gc drawing function to the original value */
}


/* ------- Hv_DrawXorLines ------------ */

void Hv_DrawXorLines(Hv_Point *xp,
		     short np)
    
{
    int     m_gc;          /* backup of original function */
    
    
    XGetGCValues(Hv_display,
		 Hv_gc,
		 GCFunction,
		 (XGCValues *)(&m_gc));   /* copies the current gc funtion value */
    
    XSetFunction(Hv_display, Hv_gc, GXxor);
    
    Hv_SetLineWidth(2);

    Hv_DrawLines(xp, np, Hv_yellow);
    Hv_SetLineWidth(0);
    XSetFunction (Hv_display, Hv_gc, m_gc);             /* resets the gc drawing function to the original value */
}

/* ------- Hv_FrameXorPolygon ------------ */

void Hv_FrameXorPolygon(Hv_Point *xp,
			short     np)
    
{
    int     m_gc;          /* backup of original function */
    
    
    XGetGCValues(Hv_display,
		 Hv_gc,
		 GCFunction,
		 (XGCValues *)(&m_gc));   /* copies the current gc funtion value */
    
    XSetFunction(Hv_display, Hv_gc, GXxor);
    
    Hv_SetLineWidth(3);
    Hv_FramePolygon(xp, np, Hv_yellow);  /* draw the poly */
    Hv_SetLineWidth(0);
    XSetFunction (Hv_display, Hv_gc, m_gc);    /* resets the gc drawing function to the original value */
}

/* ------- Hv_DrawXorRect ------------ */

void Hv_DrawXorRect(Hv_Rect *r)
    
{
    int             m_gc;         /* backup of original function */
    Hv_Rect         rect;
    
    
    Hv_CopyRect(&rect, r);
    Hv_GoodRect (&rect);  /* returns a proper set of box coords */
    
    XGetGCValues(Hv_display,
		 Hv_gc,
		 GCFunction,
		 (XGCValues *)(&m_gc));       /* copies the current gc funtion value */
    XSetFunction (Hv_display, Hv_gc, GXxor);  /* xor's the GC's drawing function */
    
    Hv_SetLineWidth(0);
    
/* draw mutilple single line width lines instead of changing
   the width --- this is faster */
    
    Hv_FrameRect(&rect, Hv_blue);
    Hv_ShrinkRect(&rect, -1, -1);
    Hv_FrameRect(&rect, Hv_green);
    
    XSetFunction (Hv_display, Hv_gc, m_gc);             /* resets the gc drawing function to the original value */
}

/* ------- Hv_DrawXorOval ------------ */

void Hv_DrawXorOval(Hv_Rect *r)
    
{
    Hv_Rect         rect;
    int             m_gc;         /* backup of original function */
    int             i;
    unsigned long   old_fg;         /* storage variable for the old forground color */
    
    old_fg = Hv_SwapForeground(Hv_blue); 
    
    Hv_CopyRect(&rect, r);
    Hv_GoodRect (&rect);  /* returns a proper set of box coords */
    
    XGetGCValues(Hv_display, Hv_gc, GCFunction, (XGCValues *)(&m_gc));   
    XSetFunction (Hv_display, Hv_gc, GXxor);
    
    Hv_SetLineWidth(0);
    
/* draw mutilple single line width lines instead of changing
   the width --- this is faster */
    
    for (i = 0; i < 2; i++) {
	XDrawArc(Hv_display, Hv_mainWindow, Hv_gc, 
		 rect.left, rect.top, rect.width, rect.height, 0, 23040); 
	Hv_ShrinkRect(&rect, -1, -1);
    }
    
    XSetFunction (Hv_display, Hv_gc, m_gc); 
    Hv_SetForeground(old_fg);
}

/* ------- Hv_DrawXorItem ------------ */

void Hv_DrawXorItem(Hv_Item   Item,
		    Hv_Region region)
    
{
    int             m_gc;           /* backup of original function */
    
    
    XGetGCValues(Hv_display, Hv_gc, GCFunction, (XGCValues *)(&m_gc)); 
    XSetFunction(Hv_display, Hv_gc, GXxor);
    
    Hv_DrawItem(Item, region);
    XSetFunction (Hv_display, Hv_gc, m_gc); 
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
    
    Hv_DrawCompoundString(area->left + extra, area->top + extra, str);
    Hv_SetForeground(oldfg);
}


/* --------- Hv_DrawHotRectBorderItem ------------- */

void    Hv_DrawHotRectBorderItem(Hv_Item   Item,
				 Hv_Region region)
    
{
    if (!Hv_CheckBit(Item->view->drawcontrol, Hv_NOHOTRECTFILL))
	Hv_FillRect(Item->area, Item->scheme->fillcolor);
    
    Hv_Draw3DRect(Item->area, Item->scheme);
}


/*------- Hv_DrawCamera -------*/

void Hv_DrawCamera(Hv_Item   Item,
		   Hv_Region region)
    
/* draws the icon for camera buttons -- used to
   create postscript pictures or to print */     
    
{
    Hv_Rect          area, box;
    Hv_ColorScheme   *scheme;
    short            margin, rad, yt1, yt2, xt1, xt2, xc, yc;
    Hv_Point         xp[4];
    short            DarkColor, LightColor;
    
    Hv_CopyRect(&area, Item->area);
    
    scheme = Item->scheme;
    margin = scheme->framewidth + scheme->borderwidth;
    Hv_ShrinkRect(&area, margin +1, margin);
    
    yt1 = area.top + 3;
    yt2 = area.bottom;
    xt1 = area.left;
    xt2 = area.right;
    xc = (xt1 + xt2)/2;
    yc = (yt1 + yt2)/2;
    rad = 3;
    
/* flash */
    
    Hv_SetPoint(&(xp[0]), xc-rad-2, yt1);
    Hv_SetPoint(&(xp[1]), xc-1,     area.top-1);
    Hv_SetPoint(&(xp[2]), xc+1,     area.top-1); 
    Hv_SetPoint(&(xp[3]), xc+rad+2, yt1); 
    
    if (Item->state == Hv_OFF) {
	DarkColor = Hv_black;
	LightColor = Hv_white;
	Hv_FillPolygon(xp, 4, True, DarkColor, LightColor);
	Hv_DrawLine(xc-rad+1, yt1-2, xc+rad-1, yt1-2, Hv_aliceBlue);
    }
    else {
	DarkColor = Hv_black;
	LightColor = Hv_yellow;
	Hv_FillPolygon(xp, 4, True, Hv_yellow, LightColor);
    }
    
    Hv_SetRect(&box, xt1, yt1, xt2-xt1, yt2-yt1);
    Hv_FillRect(&box, DarkColor);
    
/* main horiz lines */
    
    Hv_DrawLine(xt1,   yt1,     xt2,     yt1,     LightColor);
    Hv_DrawLine(xt1+1, yt1+1,   xt2,     yt1+1,   Hv_gray13);
    Hv_DrawLine(xt1,   yt2,     xt2,     yt2,     LightColor);
    Hv_DrawLine(xt1+1, yt2+1,   xt2+1,   yt2+1,   Hv_gray7);
    
/* main vertical lines */
    
    Hv_DrawLine(xt1,      yt1+1, xt1,       yt2, LightColor);
    Hv_DrawLine(xt1+1,    yt1+2, xt1+1,     yt2, Hv_gray9);
    Hv_DrawLine(xt2-1,    yt1+2, xt2-1,     yt2, Hv_gray6);
    Hv_DrawLine(xc-rad-1, yt1+2, xc-rad-1,  yt2, LightColor);
    Hv_DrawLine(xc+rad+1, yt1+2, xc+rad+1,  yt2, LightColor);
    
/* shutter button on left */
    
    Hv_DrawLine(xt1+1, yt1-1, xt1+4, yt1-1, LightColor);
    Hv_DrawLine(xt1+1, yt1-2, xt1+4, yt1-1, LightColor);
    
/* lens */
    
    Hv_FrameCircle(xc, yc+1, rad+1, LightColor);
    Hv_DrawLine(xc-1, yc+1, xc-1, yc+3, LightColor);
    Hv_DrawLine(xc-1, yc,   xc+2, yc,   LightColor);
}
E 1
