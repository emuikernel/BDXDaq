h56077
s 00000/00000/00000
d R 1.2 02/08/25 23:21:22 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_scroll.c
e
s 01039/00000/00000
d D 1.1 02/08/25 23:21:21 boiarino 1 0
c date and time created 02/08/25 23:21:21 by boiarino
e
u
U
f e 0
t
T
I 1
/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"	                 /* contains all necessary include files */

extern Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */

/*----- local prototypes --------*/

static void Hv_StuffVArrowPoints(Hv_Point   *points,
				 short    x1,
				 short    x2,
				 short    x3,
				 short    x4,
				 short    x5,
				 short    y1,
				 short    y2,
				 short    y3);

static void Hv_StuffHArrowPoints(Hv_Point   *points,
				 short    x1,
				 short    x2,
				 short    x3,
				 short    y1,
				 short    y2,
				 short    y3,
				 short    y4,
				 short    y5);

static void Hv_GetDownArrowPoints(Hv_View    View,
				  Hv_Point   *points);

static void Hv_GetUpArrowPoints(Hv_View    View,
				Hv_Point   *points);

static void Hv_GetRightArrowPoints(Hv_View    View,
				   Hv_Point   *points);

static void Hv_GetLeftArrowPoints(Hv_View    View,
				  Hv_Point   *points);

static void Hv_GetViewScrollRects(Hv_View        View,
				  Hv_Rect       *Hthumb,
				  Hv_Rect       *Vthumb,
				  Hv_Rect       *Hslide,
				  Hv_Rect       *Vslide);

static void Hv_UpdateThumbs(Hv_View    View,
			    Boolean    HorizontalScroll,
			    short     OldTLen,
			    short     OldTWid);

static void Hv_CheckHShift(Hv_View  View,
			   short    *dh,
			   float    *fdh);

static void Hv_CheckVShift(Hv_View  View,
			   short    *dv,
			   float    *fdv       
			   );

static short Hv_GetHScrollStep(Hv_View  View,
			       int     ScrollPart,
			       Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen);

static short Hv_GetVScrollStep(Hv_View   View,
			       int     ScrollPart,
			       Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen);

static void Hv_ScrollView(Hv_View   View,
			  int     ScrollPart,
			  Region    region,
			  Hv_Point   StartPP,
			  Hv_Point   StopPP);


static int   redrawcontrol = 0;

/* ----------- Hv_SetScrollActivity -------- */

void Hv_SetScrollActivity(Hv_View View)

/* Set the scroll activity based on comparison of present world
   system to default world system */

{
  Hv_FRect   tempWorld;                /* temp world of hot rect */
  
  if (View != NULL)
    if ((View->hscroll != NULL) && (View->vscroll != NULL)) {      /* check that this View has scrolls */

/* get the world system for the hot rect */

      Hv_GetTemporaryWorld(View->defaultworld, View->hotrect, &tempWorld, View->local);

/* if the world system is greater than the default we need to activate scrolling.
   Do this check for both horiz and vert */

      View->hscroll->active = ((View->world->xmin > tempWorld.xmin) || (View->world->xmax < tempWorld.xmax));
      View->vscroll->active = ((View->world->ymin > tempWorld.ymin) || (View->world->ymax < tempWorld.ymax));
    }
}


/* ---------- Hv_VertThumbInfo ------------- */

void Hv_VertThumbInfo(Hv_View  View,
		      short    *t,
		      short    *h,
		      short    *eff_height)

/*
  short    *t            position of top of thumb
  short    *h            height of thumb
  short    *eff_height   effective height of scroll area

  returns the relative position and the size of the vertical thumb for the given View.
  the vertical thumb controls vertical scrolling
*/

{
  short          L, T, R, B;
  Hv_FRect       tempWorld;         /* temporary world coord system */
  short          slide_top;         /* top of slider */
  short          slide_bottom;      /* bottom of slider */
  short          slider_height;     /* height of the sider area */
  Hv_ColorScheme *scheme;

  if (View != NULL) {
    scheme = View->containeritem->scheme;
    Hv_LocalRectToWorldRect(View, View->hotrect, &tempWorld);

/* compute the top, bottom, and height of the slider area */

    Hv_GetRectLTRB(View->local, &L, &T, &R, &B);

    slide_top    =   T + ((2*scheme->borderwidth) + 2); 
    slide_bottom =   B - ((2*scheme->borderwidth) + 2);
    slider_height =  slide_bottom - slide_top;

/* the "height" of the thumb depends on ratio of "present" world sys to default world sys */

    *h = slider_height * (tempWorld.height / View->defaultworld->height);      /* compute the height of the slider */

    *h = Hv_sMax(*h, Hv_MINTHUMBWIDTH);      /* restrict the height o the slider */

/* compute the relative position of the top of the thumb */

    *t = slide_bottom - *h - (tempWorld.ymin - View->defaultworld->ymin)/(View->defaultworld->height) * slider_height;
    *t = Hv_sMax(*t, slide_top);  /* make sure we dont go off the top */
    *t = Hv_sMin(*t, (slide_bottom - *h));  /* make sure we dont go off the bottom */
    
    *eff_height = slider_height - (*h);
  }
}

/* ---------- Hv_HorizThumbInfo ------------- */

void Hv_HorizThumbInfo(Hv_View   View,
		       short     *l,
		       short     *w,
		       short     *eff_width)

/*
  short    *l           position of left side of thumb
  short    *w           width of thumb
  short    *eff_width   effective width of scroll slider

  returns the relative position and the size of the horizontal thumb for the given View.
  the vertical thumb controls horizontal scrolling
*/

{
  Hv_FRect       tempWorld;         /* temporary world coord system */
  short          slide_left;        /* left of slider */
  short          slide_right;       /* right of slider */
  short          slider_width;      /* width of the sider area */
  Hv_ColorScheme *scheme;
  short          L, T, R, B;
  
  if (View != NULL) {
    scheme = View->containeritem->scheme;
    Hv_LocalRectToWorldRect(View, View->hotrect, &tempWorld);

/* compute the left, right, and width of the slider area (this is the width of the entire shaded
   area, not just the thumb)*/

    Hv_GetRectLTRB(View->local, &L, &T, &R, &B);

    slide_left  =  L  + ((scheme->borderwidth) + 2);
    slide_right =  R - ((2*scheme->borderwidth) + 2);
    slider_width = slide_right - slide_left;

/* the "width" of the thumb depends on ratio of "present" world sys to default world sys */

    *w = (int) (slider_width * (tempWorld.width / View->defaultworld->width));  /* compute the width of the slider */
    *w = Hv_sMax(*w, Hv_MINTHUMBWIDTH);  /* restrict the width of the slider (thumb) */

/* compute the left side of the thumb */

    *l = slide_left + (tempWorld.xmin - View->defaultworld->xmin)/(View->defaultworld->width) * slider_width; 

    *l = Hv_sMax(*l, slide_left);          /* make sure we dont go off the top */
    *l = Hv_sMin(*l, (slide_right - *w));     /* make sure we dont go off the bottom */
    
    *eff_width = slider_width - *w;
  }
}

/* ----------  Hv_DrawScrollBars --------- */

void  Hv_DrawScrollBars(Hv_View     View,
			Region      region)

/* draws the scroll bars on a View */

{
  short            bw;        /*border width*/
  short            l, t, r, b;

  Hv_Rect          Hslide, Vslide, Hthumb, Vthumb;
  Hv_ColorScheme   *scheme, thumbscheme;

  scheme = View->containeritem->scheme;
  Hv_GetViewScrollRects(View, &Hthumb, &Vthumb, &Hslide, &Vslide);
  
  Hv_SetForeground(Hv_colors[Hv_black]);

  thumbscheme = *scheme;  
/*  thumbscheme.fillcolor += 1; */
  thumbscheme.bottomcolor += 1;
  thumbscheme.framewidth = 1;

  bw = scheme->borderwidth;
  Hv_GetRectLTRB(View->local, &l, &t, &r, &b);

  Hv_SetClippingRegion(region); 

/* ------- first the vertical scroll bar --------- */

  if (redrawcontrol != 1) {
    if (View->vscroll->active) {
      Hv_FillRect(&Vslide, Hv_scrollBarColor);
      Hv_FrameRect(&Vslide, Hv_black);
      Hv_Simple3DRect(&Vthumb, True, thumbscheme.fillcolor);
    }
    else {
      Hv_FillRect(&Vslide, scheme->fillcolor);
      Hv_FrameRect(&Vslide, Hv_black);
    }

/* down arrow */

    Hv_DrawLine(r - bw, b-(bw+2),   r - bw, b-(2*bw+2), Hv_gray14);  /*left line*/
    Hv_DrawLine(r - bw, b-(2*bw+2), r - 2,  b-(2*bw+2), Hv_gray14);  /*top line*/
    Hv_DrawLine(r - 2,  b-(2*bw+2), r - 2,  b-(bw+2),   Hv_gray6);   /*right line*/
    Hv_DrawLine(r - 2,  b-(bw+2),   r - bw, b-(bw+2),   Hv_gray6);   /*bottom line*/
    
/* up arrow */

    Hv_DrawLine(r - bw, t+(2*bw+2),  r - bw,  (t+bw+2),    Hv_gray14);  /*left line*/
    Hv_DrawLine(r - bw, t+(bw+2),    r - 2,   (t+bw+2),    Hv_gray14);  /*top line*/
    Hv_DrawLine(r - 2,  t+(bw+2),    r - 2,   (t+2*bw+2),  Hv_gray6);   /*right line*/
    Hv_DrawLine(r - 2,  t+(2*bw+2),  r - bw,  (t+2*bw+2),  Hv_gray6);   /*bottom line*/

    Hv_DrawScrollArrows(View, region, Hv_UPARROW | Hv_DOWNARROW);
  }
  
/* ------- now the horizontal scroll bar --------- */

  if (redrawcontrol < 2) {
    if (View->hscroll->active) {
      Hv_FillRect(&Hslide, Hv_scrollBarColor);
      Hv_FrameRect(&Hslide, Hv_black);
      Hv_Simple3DRect(&Hthumb, True, thumbscheme.fillcolor);
    }
    else {
      Hv_FillRect(&Hslide, scheme->fillcolor);
      Hv_FrameRect(&Hslide, Hv_black);
    }

/* left arrow */

    Hv_DrawLine(l+1, b-2, l+1, b-bw, Hv_gray14);      /*left line*/
    Hv_DrawLine(l+1, b-bw, l+bw, b-bw, Hv_gray14);    /*top line*/
    Hv_DrawLine(l+bw, b-2, l+bw, b-bw-1, Hv_gray6);   /*right line*/
    Hv_DrawLine(l+1, b-2, l+bw, b-2, Hv_gray6);       /*bottom line*/
    
/* right arrow */

    Hv_DrawLine(r - (2*bw+2), b-2,  r - (2*bw+2), b-bw,    Hv_gray14);  /*left line*/
    Hv_DrawLine(r - (2*bw+2), b-bw, r - (bw+2),   b-bw,    Hv_gray14);  /*top line*/
    Hv_DrawLine(r - (bw+2),   b-2,  r - (bw+2),   b-bw-1,  Hv_gray6);   /*right line*/
    Hv_DrawLine(r - (bw+2),   b-2,  r - (2*bw+2), b-2,     Hv_gray6);   /*bottom line*/

    Hv_DrawScrollArrows(View, region, Hv_LEFTARROW | Hv_RIGHTARROW);

  }

  Hv_SetClippingRegion(region);
}


/* ------------ Hv_HandleScroll ----------*/

void  Hv_HandleScroll(Hv_View  View,
		      int    ScrollPart,
		      Hv_Point  StartPP)

/*
  Hv_View    View         What View is being scrolled
  int        ScrollPart   What View part (e.g. up-arrow) initiated scroll
  Hv_Point   StartPP      Mouse Position when scrolling was initiated
*/

{
  /* does event handling for scroll routines */

  Region            region; 
  Hv_Point          StopPP;
  short             oldbg;
  Hv_Rect           Hthumb, Vthumb, Hslide, Vslide;

  Hv_GetViewScrollRects(View, &Hthumb, &Vthumb, &Hslide, &Vslide);

  StopPP = StartPP;
  region = Hv_GetViewRegion(View);          /* get min region the View needs */

  switch(ScrollPart) {
  case Hv_INHORIZONTALTHUMB:
    Hv_DragRect(&Hthumb, &Hslide, StartPP, &StopPP, Hv_FIXEDYPOLICY, False, True);
    break;
    
  case Hv_INVERTICALTHUMB:
    Hv_DragRect(&Vthumb, &Vslide, StartPP, &StopPP, Hv_FIXEDXPOLICY, False, True);
    break;
    
  default:
    break;
  }

	  
  oldbg = Hv_SwapBackground(View->containeritem->scheme->fillcolor);
  Hv_ScrollView(View, ScrollPart, region, StartPP, StopPP); 
  Hv_SetBackground(oldbg);
  Hv_DestroyRegion(region);
}

/* --------- Hv_DrawScrollArrows -----------*/

void  Hv_DrawScrollArrows(Hv_View   View,
			  Region   region,
			  short    control)

/* draws the scroll arrows on a View
   region:  clipping region */

{
  Hv_Region       viewrgn = NULL;
  Hv_Point        *points;       /* holds points for each arrow */
  short           npoints = 7;   /* number of points needed for each arrow */
  short           cHFill, cVFill;

  points = Hv_NewPoints((int)npoints);

  if (region == NULL) {
    viewrgn = Hv_GetViewRegion(View);
    Hv_SetClippingRegion(viewrgn);
    Hv_DestroyRegion(viewrgn);
  }

/* get the fillcors */

  if (View->vscroll->active)
    cVFill = Hv_scrollActiveColor;
  else
    cVFill = Hv_scrollInactiveColor;

  if (View->hscroll->active)
    cHFill = Hv_scrollActiveColor;
  else
    cHFill = Hv_scrollInactiveColor;

  if (Hv_CheckBit(control, Hv_UPARROW)) {
    Hv_GetUpArrowPoints(View, points);
    Hv_FillPolygon(points, npoints, True, cVFill, Hv_black);
  }
  
  if (Hv_CheckBit(control, Hv_DOWNARROW)) {
    Hv_GetDownArrowPoints(View, points);
    Hv_FillPolygon(points, npoints, True, cVFill, Hv_black);
  }
  
  if (Hv_CheckBit(control, Hv_LEFTARROW)) {
    Hv_GetLeftArrowPoints(View, points);
    Hv_FillPolygon(points, npoints, True, cHFill, Hv_black);
  }
  
  if (Hv_CheckBit(control, Hv_RIGHTARROW)) {
    Hv_GetRightArrowPoints(View, points);
    Hv_FillPolygon(points, npoints, True, cHFill, Hv_black);
  }
  Hv_Free(points);
}


/*========= local statics =========*/


/* -------- Hv_StuffVArrowPoints ----------*/

static void Hv_StuffVArrowPoints(Hv_Point   *points,
				 short    x1,
				 short    x2,
				 short    x3,
				 short    x4,
				 short    x5,
				 short    y1,
				 short    y2,
				 short    y3)

/* stuffs the points into a points array -- appropriate only for vertical scroll arrows*/

{
  points[0].y =  y1;   points[0].x = x1;
  points[1].y =  y2;   points[1].x = x2;
  points[2].y =  y2;   points[2].x = x3;
  points[3].y =  y3;   points[3].x = x3;
  points[4].y =  y3;   points[4].x = x4;
  points[5].y =  y2;   points[5].x = x4;
  points[6].y =  y2;   points[6].x = x5;
}

/* -------- Hv_StuffHArrowPoints ----------*/

static void Hv_StuffHArrowPoints(Hv_Point   *points,
				 short    x1,
				 short    x2,
				 short    x3,
				 short    y1,
				 short    y2,
				 short    y3,
				 short    y4,
				 short    y5)

/* stuffs the points into a points array -- appropriate only for vertical scroll arrows*/

{
  points[0].x =  x1;   points[0].y = y1;
  points[1].x =  x2;   points[1].y = y2;
  points[2].x =  x2;   points[2].y = y3;
  points[3].x =  x3;   points[3].y = y3;
  points[4].x =  x3;   points[4].y = y4;
  points[5].x =  x2;   points[5].y = y4;
  points[6].x =  x2;   points[6].y = y5;
}

/* ----------- Hv_GetDownArrowPoints -----------*/

static void  Hv_GetDownArrowPoints(Hv_View    View,
				   Hv_Point   *points)

/* get points used to draw "down" arrow in scroll bars */

{
  short       r, bmbw;

  r =    Hv_RectRight(View->local);
  bmbw = Hv_RectBottom(View->local) - 1 -
    View->containeritem->scheme->borderwidth;

  Hv_StuffVArrowPoints(points,
		       r-8,
		       r-13,
		       r-10,
		       r-6,
		       r-3,
		       bmbw-4,
		       bmbw-9,
		       bmbw-13);
}

/* ----------- Hv_GetUpArrowPoints -----------*/

static void  Hv_GetUpArrowPoints(Hv_View    View,
				 Hv_Point   *points)

/* get points used to draw "up" arrow in scroll bars */

{
  short       r, tpbw;

  r =    Hv_RectRight(View->local);
  tpbw = View->local->top+ 1 + View->containeritem->scheme->borderwidth;

  Hv_StuffVArrowPoints(points,
		       r-8,
		       r-13,
		       r-10,
		       r-6,
		       r-3,
		       tpbw+4,
		       tpbw+9,
		       tpbw+13);
}

/* ----------- Hv_GetRightArrowPoints -----------*/

static void  Hv_GetRightArrowPoints(Hv_View    View,
				    Hv_Point   *points)

/* get points used to draw "right" arrow in scroll bars */

{
  short       rmbw, b;

  rmbw = Hv_RectRight(View->local) - View->containeritem->scheme->borderwidth;
  b =    Hv_RectBottom(View->local);

  Hv_StuffHArrowPoints(points,
		       rmbw-5,
		       rmbw-10,
		       rmbw-14,
		       b-8,
		       b-13,
		       b-10,
		       b-6,
		       b-3);
}

/* ----------- Hv_GetLeftArrowPoints -----------*/

static void  Hv_GetLeftArrowPoints(Hv_View    View,
				   Hv_Point   *points)

/* get points used to draw "left" arrow in scroll bars */

{
  short       l, b;

  l = View->local->left;
  b = Hv_RectBottom(View->local);

  Hv_StuffHArrowPoints(points,
		       l+3,
		       l+8,
		       l+12,
		       b-8,
		       b-13,
		       b-10,
		       b-6,
		       b-3);

}



/* ---- Hv_GetViewScrollRects ---  */

static void Hv_GetViewScrollRects(Hv_View        View,
				  Hv_Rect       *Hthumb,
				  Hv_Rect       *Vthumb,
				  Hv_Rect       *Hslide,
				  Hv_Rect       *Vslide)
/*
  Hv_Rect     *Hthumb;   rect corresponding to the horizontal thumb
  Hv_Rect     *Vthumb;   rect corresponding to the vertical   thumb
  Hv_Rect     *Hslide;   rect corresponding to the horizontal slide area
  Hv_Rect     *Vslide;   rect corresponding to the vertical   slide area
*/

{
  short           l, t, r, b, bw;
  short           vtht, vthh, hthl, hthw;
  short           tbwp3, bwm1;
  short           dummy;

  bw = View->containeritem->scheme->borderwidth;    /*border width for the View*/
  Hv_GetRectLTRB(View->local, &l, &t, &r, &b);

  Hv_VertThumbInfo (View, &vtht, &vthh, &dummy);
  Hv_HorizThumbInfo(View, &hthl, &hthw, &dummy);

  tbwp3 = (2 * bw) + 3;
  bwm1 = bw - 1;

  Hv_SetRect(Vthumb, r - bw, vtht, bw, vthh);
  Hv_SetRect(Hthumb, hthl, b - bw, hthw, bw);
  Hv_SetRect(Vslide, r - bw, t + tbwp3, bwm1, b - 2*tbwp3 - t + 1);
  Hv_SetRect(Hslide, l + bw + 1, b - bw, r - tbwp3 - l - bw, bwm1);

  Hv_ShrinkRect(Hthumb, 1, 2);
  Hv_ShrinkRect(Vthumb, 2, 1);

}


/* ------------ Hv_UpdateThumbs --------- */

static void Hv_UpdateThumbs(Hv_View    View,
			    Boolean    HorizontalScroll,
			    short     OldTLen,
			    short     OldTWid)

/* HorizontalScroll:  if true, this was an H scroll else it was a V scroll*/

{
  short         ThumbLen, ThumbWid, dummy;
  short         paintL, paintT, paintW, paintH;
  short         bw;
  Hv_Rect       rect; 

  bw = View->containeritem->scheme->borderwidth;

/* erase "bad part" of old thumb*/

  if (HorizontalScroll) {
    Hv_HorizThumbInfo(View, &ThumbLen, &ThumbWid, &dummy);
    if (ThumbLen > OldTLen) {
      paintL = OldTLen;
      paintW = Hv_sMin(OldTWid, ThumbLen - OldTLen);
    }
    else {
      paintL = Hv_sMax(OldTLen, ThumbLen + ThumbWid);
      paintW = OldTLen + OldTWid - paintL;
    }
    
    Hv_SetRect(&rect,
	       paintL,
	       Hv_RectBottom(View->local)-bw,
	       paintW+1,
	       bw-1);

    Hv_FillRect(&rect, View->containeritem->scheme->fillcolor);
    Hv_FillPatternRect(&rect, Hv_scrollBarColor, Hv_SCROLLPAT);
  }
  else {
    Hv_VertThumbInfo(View, &ThumbLen, &ThumbWid, &dummy);
    if (ThumbLen > OldTLen) {
      paintT = OldTLen;
      paintH = Hv_sMin(OldTWid, ThumbLen - OldTLen);
    }
    else {
      paintT = Hv_sMax(OldTLen, ThumbLen + ThumbWid);
      paintH = OldTLen + OldTWid - paintT;
    }
    
    
    Hv_SetRect(&rect,
	       Hv_RectRight(View->local)-bw,
	       paintT,
	       bw-1,
	       paintH+1);

    Hv_FillRect(&rect, View->containeritem->scheme->fillcolor);
    Hv_FillPatternRect(&rect, Hv_scrollBarColor, Hv_SCROLLPAT);
  }
  
}

/* -------- Hv_CheckHShift ---------- */

static void Hv_CheckHShift(Hv_View  View,
			   short    *dh,
			   float    *fdh)

/* make sure we dont shift beyond the default world
   float  *fdh      WORLD horizontal shift */

{
  Hv_FRect           defW;
  Boolean            Changed = False;

/* get the world coordinates for the hotrect in the present world system */

    Hv_LocalRectToWorldRect(View, View->hotrect, &defW);

/* make sure defaultworld will not be exceeded by the shift */

  if (*fdh < 0.0) {
    if ((defW.xmin + *fdh) < View->defaultworld->xmin) {
      Changed = True;
      *fdh = View->defaultworld->xmin - defW.xmin;
    }
  }
  else {
    if ((defW.xmax + *fdh) > View->defaultworld->xmax) {
      Changed = True;
      *fdh = View->defaultworld->xmax - defW.xmax;
    }
  }
  
  if (Changed)
    *dh = View->local->width*(*fdh)/View->world->width;
}

/* -------- Hv_CheckVShift ---------- */

static void Hv_CheckVShift(Hv_View  View,
			   short    *dv,
			   float    *fdv       
			   )

/* make sure we dont shift beyond the default world
   float  *fdv      WORLD vert shift */

{
  Hv_FRect             defW;
  Boolean              Changed = False;

/* get the world coordinates for the hotrect in the present world system */

  Hv_LocalRectToWorldRect(View, View->hotrect, &defW);

/* make sure defaultworld will not be exceeded by the shift */

  if (*fdv < 0.0) {
    if ((defW.ymin + *fdv) < View->defaultworld->ymin) {
      Changed = True;
      *fdv = View->defaultworld->ymin - defW.ymin;
    }
  }
  else {
    if ((defW.ymax + *fdv) > View->defaultworld->ymax) {
      Changed = True;
      *fdv = View->defaultworld->ymax - defW.ymax;
    }
  }
  
  if (Changed)
    *dv = -View->local->height*(*fdv)/View->world->height;
}

/* ----------- Hv_GetHScrollStep ----------*/

static short Hv_GetHScrollStep(Hv_View  View,
			       int     ScrollPart,
			       Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen)
{
  short   dh = 0;
  float   fdh = 0.0;

  switch (ScrollPart) {  
  case Hv_INLEFTARROW:            /*move picture right by arrow_step pixels*/
    dh = -View->hscroll->arrow_step;
    break;
    
  case Hv_INRIGHTARROW:           /* move picture left by arrow_step pixels */
    dh = View->hscroll->arrow_step;
    break;
    
  case Hv_INLEFTSLIDE:            /* move picture right by slide step pixels */
    dh = -View->hscroll->slide_step;
    break;
    
  case Hv_INRIGHTSLIDE:           /* move picture left by slide step pixels */
    dh = View->hscroll->slide_step;
    break;
    
  case Hv_INHORIZONTALTHUMB:     /* move picture horizontally by the amount the thumb was dragged */
    dh = StopPP.x - StartPP.x;
    fdh = (dh*View->defaultworld->width)/efflen;
    dh = (View->local->width*fdh)/View->world->width;
    break;
    
  default:
    break;
  }
  
  return(dh);
}


/* ----------- Hv_GetVScrollStep ----------*/

static short Hv_GetVScrollStep(Hv_View   View,
			       int     ScrollPart,
			       Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
			       short    efflen)
{
  short   dv = 0;
  float   fdv = 0.0;

  switch (ScrollPart) {  
      
  case Hv_INUPARROW:              /* move picture down by arrow_step pixels */
    dv = -View->vscroll->arrow_step;
    break;
    
  case Hv_INDOWNARROW:            /* move picture up by arrow step pixels */
    dv = View->vscroll->arrow_step;
    break;
    
    
  case Hv_INUPSLIDE:             /* move picture down by slide step pixels */
    dv = -View->vscroll->slide_step;
    break;
    
  case Hv_INDOWNSLIDE:           /* move picture up by slide step pixels */
    dv = View->vscroll->slide_step;
    break;
    
    
  case Hv_INVERTICALTHUMB:       /* move picture vertically by the amount the thumb was dragged */
    dv = StopPP.y  - StartPP.y;
    fdv = (dv*View->defaultworld->height)/efflen;
    dv = (View->local->height*fdv)/View->world->height;
    break;
    
  default:
    break;
  }
  
  return dv;
}


/* ------------- Hv_ScrollView ----------*/

static void Hv_ScrollView(Hv_View   View,
			  int     ScrollPart,
			  Region    region,
			  Hv_Point   StartPP,
			  Hv_Point   StopPP)

/* comes here for actual scrolling based on part selected */


{
  short             tl, tw, left, top;

  short             dh;    /*horizontal shift in pixel coordinates due to scrolling*/
  short             dv;    /*vertical   shift in pixel coordinates due to scrolling*/

  float             fdh;   /*horizontal shift in world coordinates due to scrolling*/
  float             fdv;   /*vertical   shift in world coordinates due to scrolling*/

  int               src_x, src_y;     /*top left corner of rect used as source in XCopyArea*/
  int               dest_x, dest_y;   /*top left corner of rect used as destination in XCopyArea*/
  unsigned int      width, height;
  short             updwidth, updheight;
  Region            temp;

  Hv_Point          destP, srcP;
  Boolean           HorizontalScroll;
  short             efflen;
  Hv_Rect           rect;

/* set defaults */

  Hv_SetClippingRegion(region);     /* set region to clipping region*/
  
  Hv_CheckForExposures(Expose); 

  dh = 0;
  dv = 0;
  fdh = 0.0;
  fdv = 0.0;
  src_x = View->hotrect->left+1;
  dest_x = src_x;
  left = src_x;
  src_y = View->hotrect->top+1;
  dest_y = src_y;
  top = src_y;

/* is this a horizontal or vertical scroll?*/

  HorizontalScroll = ((ScrollPart == Hv_INHORIZONTALTHUMB) ||
		      (ScrollPart == Hv_INRIGHTARROW) || 
		      (ScrollPart == Hv_INLEFTARROW) ||
		      (ScrollPart == Hv_INLEFTSLIDE) ||
		      (ScrollPart == Hv_INRIGHTSLIDE));

/* get geometric info abouth the thumbs */


  if (HorizontalScroll) {
    Hv_HorizThumbInfo(View, &tl, &tw, &efflen);
    if (efflen > 0) {
      dh = Hv_GetHScrollStep(View, ScrollPart, region, StartPP, StopPP, efflen);
      fdh = (dh*View->world->width)/View->local->width;
      Hv_CheckHShift(View, &dh, &fdh);
    }
  }

  else {
    Hv_VertThumbInfo(View, &tl, &tw, &efflen);
    if (efflen > 0) {
      dv = Hv_GetVScrollStep(View, ScrollPart, region, StartPP, StopPP, efflen);
      fdv = -(dv*View->world->height)/View->local->height;
      Hv_CheckVShift(View, &dv, &fdv);
    }
  }
  
/* adjust the world */

  if (dh != 0) {
    View->world->xmin += fdh;
    View->world->xmax += fdh;
  }
  
  if (dv != 0) {
    View->world->ymin += fdv;
    View->world->ymax += fdv;
  }

  Hv_FixFRectWH(View->world);
  Hv_UpdateThumbs(View, HorizontalScroll, tl, tw);       /* update the slider thumbs */

  if (HorizontalScroll)
    redrawcontrol = 1;
  else
    redrawcontrol = 2;

  Hv_DrawScrollBars(View, region); 
  redrawcontrol = 0;

/* do the actual adjustments to the offset amounts */

  if (dh < 0) {
    src_x = View->hotrect->left+1;
    dest_x = src_x - dh;
    updwidth = -dh;
    updheight = View->hotrect->height - 1;
    left = src_x;
  }
  else if (dh > 0) {
    dest_x = View->hotrect->left+1;
    src_x = dest_x + dh;
    updwidth = dh;
    updheight = View->hotrect->height - 1;
    left = Hv_RectRight(View->hotrect)-dh;
  }
  
  if (dv < 0) {
    src_y = View->hotrect->top+1;
    dest_y = src_y - dv;
    updwidth = View->hotrect->width - 1;
    updheight = -dv;
    top = src_y;
  }
  else if (dv > 0) {
    dest_y = View->hotrect->top+1;
    src_y = dest_y + dv;
    updwidth = View->hotrect->width - 1;
    updheight = dv;
    top = Hv_RectBottom(View->hotrect)-dv;
  }


/* 10/11/95 change -1 to -2 in next two lines */
  
  height = View->hotrect->height - abs(dv) - 2;
  if (height > 32000)
    height = 0;
  
  width =  View->hotrect->width -  abs(dh) - 2;
  if (width > 32000)
    width = 0;


/****************************************
Note: the extra care taken below with
Hv_trueMainWindo is beacuse the "usual"
Hv_trueMainWindow may have been temporarily
reset due to offscreen drawing.
******************************************/
  
  Hv_SetPoint(&destP, dest_x, dest_y);
  Hv_SetPoint(&srcP, src_x, src_y);
  if ((width > 0) && (height > 0) && (Hv_PointInRect(destP, View->hotrect))
      && (Hv_PointInRect(srcP, View->hotrect)))

    XCopyArea(Hv_display, Hv_trueMainWindow, Hv_trueMainWindow, Hv_gc,
	      src_x, src_y, width, height, dest_x, dest_y);

/* what ever was not handled by CopyArea will be handled by an explicit
   redraw. Make sure we only redraw inside the hotrect */

  left = Hv_sMax(View->hotrect->left, Hv_sMin(left,View->hotrect->right));
  top =  Hv_sMax(View->hotrect->top,  Hv_sMin(top, View->hotrect->bottom));
  updwidth =  Hv_sMax(0, Hv_sMin(updwidth,  (View->hotrect->right - left)));
  updheight = Hv_sMax(0, Hv_sMin(updheight, (View->hotrect->bottom - top)));

  Hv_scrollDH = -dh;
  Hv_scrollDV = -dv;

/* reposition hotrect items */

  Hv_offsetDueToScroll = True;
  Hv_OffsetHotRectItems(View->items, -dh, -dv);
  Hv_offsetDueToScroll = False;

/* now redraw affected part */
  
  Hv_SetRect(&rect, left, top, updwidth, updheight);
  temp  = Hv_RectRegion(&rect);     /* malloc the region */

  XShrinkRegion(temp, -1, -1);

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);  

  Hv_DrawView(View, temp);
  Hv_scrollDH = 0;
  Hv_scrollDV = 0;
  Hv_DestroyRegion(temp);

  Hv_SetClippingRegion(region);               /* set region to clipping region*/
  XFlush(Hv_display);
  Hv_CheckForExposures(GraphicsExpose);
  Hv_CheckForExposures(NoExpose);
}





E 1
