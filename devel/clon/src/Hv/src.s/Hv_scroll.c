/**
 * <EM>Deals with scrolling a view.</EM>
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
#include "Hv_drag.h"

extern Hv_Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */

/*----- local prototypes --------*/

static void Hv_DragScrollCallback(Hv_DragData);

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







int   Hv_sbredrawcontrol = 0;

/**
 * Hv_SetScrollActivity
 * @purpose  Set the scroll activity based on comparison of present world
             system to default world system.
 * @param    View    The View in question
 */

void Hv_SetScrollActivity(Hv_View View) {

/* Set the scroll activity based on comparison of present world
   system to default world system */

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


/**
 *  Hv_VertThumbInfo
 * @purpose returns the relative position and the size of the
 *          vertical thumb for the given View.
 * @param    View    The View in question
 * @param    t       upon return, top of thumb
 * @param    h       upon return, height of thumb
 * @param    eff_height upon return, effective height of scroll area
 */

void Hv_VertThumbInfo(Hv_View  View,
		      short    *t,
		      short    *h,
		      short    *eff_height) {

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

    *h = (short)(slider_height * Hv_fMin((float)(0.999), 
		(tempWorld.height / View->defaultworld->height)));      /* compute the height of the slider */


    *h = Hv_sMax(*h, Hv_MINTHUMBWIDTH);      /* restrict the height o the slider */


/* compute the relative position of the top of the thumb */

    *t = (short)(slide_bottom - *h - (tempWorld.ymin - View->defaultworld->ymin)/(View->defaultworld->height) * slider_height);
    *t = Hv_sMax(*t, slide_top);  /* make sure we dont go off the top */
    *t = Hv_sMin(*t, (short)(slide_bottom - *h));  /* make sure we dont go off the bottom */
    
    *eff_height = slider_height - (*h);
  }
}

/**
 *  Hv_HorizThumbInfo
 * @purpose returns the relative position and the size of the
 *          horizontal thumb for the given View.
 * @param    View    The View in question
 * @param    l       upon return, length of thumb
 * @param    w       upon return, width of thumb
 * @param    eff_width upon return, effective width of scroll area
 */

void Hv_HorizThumbInfo(Hv_View   View,
		       short     *l,
		       short     *w,
			   short     *eff_width) {


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

    *w = (int) (slider_width * Hv_fMin((float)(0.999), 
		(tempWorld.width / View->defaultworld->width)));  /* compute the width of the slider */
    *w = Hv_sMax(*w, Hv_MINTHUMBWIDTH);  /* restrict the width of the slider (thumb) */

/* compute the left side of the thumb */

    *l = (short)(slide_left + (tempWorld.xmin - View->defaultworld->xmin)/(View->defaultworld->width) * slider_width); 

    *l = Hv_sMax(*l, slide_left);          /* make sure we dont go off the top */
    *l = Hv_sMin(*l, (short)(slide_right - *w));     /* make sure we dont go off the bottom */
    
    *eff_width = slider_width - *w;
  }
}

/**
 * Hv_DrawScrollBars 
 * @purpose    Draw the scroll bars
 * @param  View      The view in question
 * @param  region    The clipping region
 */

void  Hv_DrawScrollBars(Hv_View     View,
						Hv_Region      region) {

  short   bw;        /*border width*/
  short   l, t, r, b;
  short   rm2, rmbw, rmbwm2, bmbwm2, bmbwm1, rm2bwm2;
  short   bm2bwm2, tp2bwp2, tpbwp2;
  short   lp1, lpbw, bm2, bmbw;


  Hv_Rect          Hslide, Vslide, Hthumb, Vthumb;
  Hv_ColorScheme   *scheme, thumbscheme;

  scheme = View->containeritem->scheme;
  Hv_GetViewScrollRects(View, &Hthumb, &Vthumb, &Hslide, &Vslide);
  
  Hv_SetForeground(Hv_colors[Hv_black]);

  thumbscheme = *scheme;  

  thumbscheme.bottomcolor += 1;
  thumbscheme.framewidth = 1;

  bw = scheme->borderwidth;
  Hv_GetRectLTRB(View->local, &l, &t, &r, &b);

  Hv_SetClippingRegion(region); 

/* ------- first the vertical scroll bar --------- */

  if (Hv_sbredrawcontrol != 1) {
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


    bm2bwm2 = (short)(b-2*bw-2);
    tp2bwp2 = (short)(t+2*bw+2);
    tpbwp2  = (short)(t+bw+2);
    
    rm2     = (short)(r-2);
    rmbw    = (short)(r-bw);
    rmbwm2  = (short)(r-bw-2);
    bmbwm2  = (short)(b-bw-2);
    bmbwm1  = (short)(b-bw-1);
    rm2bwm2 = (short)(r-2*bw-2);
    
    lp1  = (short)(l+1);
    lpbw = (short)(l+bw);
    bm2  = (short)(b-2);
    bmbw = (short)(b-bw);

    Hv_DrawLine(rmbw, bmbwm2,   rmbw, bm2bwm2,  Hv_gray14);  /*left line*/
    Hv_DrawLine(rmbw, bm2bwm2,  rm2,  bm2bwm2,  Hv_gray14);  /*top line*/
    Hv_DrawLine(rm2,  bm2bwm2,  rm2,  bmbwm2,   Hv_gray6);   /*right line*/
    Hv_DrawLine(rm2,  bmbwm2,   rmbw, bmbwm2,   Hv_gray6);   /*bottom line*/
    
/* up arrow */

    Hv_DrawLine(rmbw, tp2bwp2,  rmbw,  tpbwp2,   Hv_gray14);  /*left line*/
    Hv_DrawLine(rmbw, tpbwp2,   rm2,   tpbwp2,   Hv_gray14);  /*top line*/
    Hv_DrawLine(rm2,  tpbwp2,   rm2,   tp2bwp2,  Hv_gray6);   /*right line*/
    Hv_DrawLine(rm2,  tp2bwp2,  rmbw,  tp2bwp2,  Hv_gray6);   /*bottom line*/

    Hv_DrawScrollArrows(View, region, Hv_UPARROW | Hv_DOWNARROW);
  }
  
/* ------- now the horizontal scroll bar --------- */

  if (Hv_sbredrawcontrol < 2) {
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

    Hv_DrawLine(lp1,  bm2,  lp1,  bmbw, Hv_gray14);    /*left line*/
    Hv_DrawLine(lp1,  bmbw, lpbw, bmbw, Hv_gray14);    /*top line*/
    Hv_DrawLine(lpbw, bm2,  lpbw, bmbwm1, Hv_gray6);   /*right line*/
    Hv_DrawLine(lp1,  bm2,  lpbw, bm2, Hv_gray6);      /*bottom line*/
    
/* right arrow */

    Hv_DrawLine(rm2bwm2, bm2,   rm2bwm2, bmbw,    Hv_gray14);  /*left line*/
    Hv_DrawLine(rm2bwm2, bmbw,  rmbwm2,  bmbw,    Hv_gray14);  /*top line*/
    Hv_DrawLine(rmbwm2,   bm2,  rmbwm2,  bmbwm1,  Hv_gray6);   /*right line*/
    Hv_DrawLine(rmbwm2,   bm2,  rm2bwm2, bm2,     Hv_gray6);   /*bottom line*/

    Hv_DrawScrollArrows(View, region, Hv_LEFTARROW | Hv_RIGHTARROW);

  }

  Hv_SetClippingRegion(region);
}


/**
 * Hv_HandleScroll
 * @purpose Event handling for scroll routines
 * View         What View is being scrolled
 * ScrollPart   What View part (e.g. up-arrow) initiated scroll
 * StartPP      Mouse Position when scrolling was initiated
 */

void  Hv_HandleScroll(Hv_View  View,
		      int    ScrollPart,
			  Hv_Point  StartPP) {


  Hv_Region         region; 
  Hv_Point          StopPP;
  unsigned long     oldbg;
  Hv_Rect           Hthumb, Vthumb, Hslide, Vslide;

  Hv_GetViewScrollRects(View, &Hthumb, &Vthumb, &Hslide, &Vslide);


  switch(ScrollPart) {
  case Hv_INHORIZONTALTHUMB:
	  Hv_LaunchDragRect(View,
		  NULL,
		  &StartPP,
		  &Hthumb,
		  &Hslide,
		  Hv_RECTCONFINED | Hv_FIXEDYPOLICY,
		  Hv_DragScrollCallback);
   break;
    
  case Hv_INVERTICALTHUMB:
 	  Hv_LaunchDragRect(View,
		  NULL,
		  &StartPP,
		  &Vthumb,
		  &Vslide,
		  Hv_RECTCONFINED | Hv_FIXEDXPOLICY,
		  Hv_DragScrollCallback);
    break;
    
  default:
	  StopPP = StartPP;
	  region = Hv_GetViewRegion(View);          /* get min region the View needs */
	  oldbg = Hv_SwapBackground(View->containeritem->scheme->fillcolor);
	  Hv_ScrollView(View, ScrollPart, region, StartPP, StopPP); 
	  Hv_SetBackground(oldbg);
	  Hv_DestroyRegion(region);
	  break;
  }

	  

}

/**
 * Hv_DragScrollCallback
 * @purpose This is the callback that the view's scroll thumbs were dragged.
 * @param   dd   The drag data pointer.
 */

static void Hv_DragScrollCallback(Hv_DragData dd) {
  
	int  scrollpart;
	Hv_Region region = NULL;
	
	if ((dd == NULL) || (dd->view == NULL)) {
		fprintf(stderr, "Serious problem in Hv_DragScrollCallback.");
		return;
	}

/* the policy will tell us which thumb we were in */

	if (Hv_CheckBit(dd->policy, Hv_FIXEDYPOLICY))
		scrollpart = Hv_INHORIZONTALTHUMB;
	else
		scrollpart = Hv_INVERTICALTHUMB;

  region = Hv_GetViewRegion(dd->view); /* get min region the View needs */
  Hv_ScrollView(dd->view, scrollpart, region, dd->startpp, dd->finalpp); 
  Hv_DestroyRegion(region);
}



/**
 * Hv_DrawScrollArrows
 * @purpose Draw Scroll Arrows
 * @param View      View in question
 * @param region    Clipping region
 * @param control   Bitwise control of which arrows
 */

void  Hv_DrawScrollArrows(Hv_View   View,
			  Hv_Region   region,
			  short    control) {


  Hv_Region       viewrgn = NULL;
  Hv_Point        *points;       /* holds points for each arrow */
  short           npoints = 3;   /* number of points needed for each arrow */
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



/**
 * Hv_GetRightArrowPoints 
 * @purpose  Get the polygon for the "down" scroll arrow.
 * @param  View   The relevant view.
 * @param  points The polygon
 * @local
 */

static void  Hv_GetDownArrowPoints(Hv_View    View,
				   Hv_Point   *points) {

/* get points used to draw "down" arrow in scroll bars */

  short       r, bmbw;

  r =    Hv_RectRight(View->local);
  bmbw = Hv_RectBottom(View->local) - 1 -
    View->containeritem->scheme->borderwidth;

  points[0].x = r-8;        points[0].y  = bmbw-6;
  points[1].x = r-4;        points[1].y = bmbw-12;
  points[2].x = r-12;       points[2].y = bmbw-12;

}

/**
 * Hv_GetUpArrowPoints 
 * @purpose  Get the polygon for the "up" scroll arrow.
 * @param  View   The relevant view.
 * @param  points The polygon
 * @local
 */

static void  Hv_GetUpArrowPoints(Hv_View    View,
				 Hv_Point   *points) {

/* get points used to draw "up" arrow in scroll bars */

  short       r, tpbw;

  r =    Hv_RectRight(View->local);
  tpbw = View->local->top+ 1 + View->containeritem->scheme->borderwidth;

  points[0].x = r-8;        points[0].y  = tpbw+4;
  points[1].x = r-4;        points[1].y = tpbw+10;
  points[2].x = r-12;       points[2].y = tpbw+10;

}

/**
 * Hv_GetRightArrowPoints 
 * @purpose  Get the polygon for the "right" scroll arrow.
 * @param  View   The relevant view.
 * @param  points The polygon
 * @local
 */

static void  Hv_GetRightArrowPoints(Hv_View    View,
				    Hv_Point   *points) {

/* get points used to draw "right" arrow in scroll bars */

  short       rmbw, b;

  rmbw = Hv_RectRight(View->local) - View->containeritem->scheme->borderwidth;
  b =    Hv_RectBottom(View->local);

  points[0].x = rmbw - 6;        points[0].y  = b-8;
  points[1].x = rmbw - 12;       points[1].y = b-4;
  points[2].x = rmbw - 12;       points[2].y = b-12;

}

/**
 * Hv_GetRightArrowPoints 
 * @purpose  Get the polygon for the "left" scroll arrow.
 * @param  View   The relevant view.
 * @param  points The polygon
 * @local
 */

static void  Hv_GetLeftArrowPoints(Hv_View    View,
				   Hv_Point   *points) {

/* get points used to draw "left" arrow in scroll bars */

  short       l, b;

  l = View->local->left;
  b = Hv_RectBottom(View->local);

  points[0].x = l+4;        points[0].y  = b-8;
  points[1].x = l+10;       points[1].y = b-4;
  points[2].x = l+10;       points[2].y = b-12;

}



/* ---- Hv_GetViewScrollRects ---  */

static void Hv_GetViewScrollRects(Hv_View        View,
				  Hv_Rect       *Hthumb,
				  Hv_Rect       *Vthumb,
				  Hv_Rect       *Hslide,
				  Hv_Rect       *Vslide) {
/*
  Hv_Rect     *Hthumb;   rect corresponding to the horizontal thumb
  Hv_Rect     *Vthumb;   rect corresponding to the vertical   thumb
  Hv_Rect     *Hslide;   rect corresponding to the horizontal slide area
  Hv_Rect     *Vslide;   rect corresponding to the vertical   slide area
*/

  short           l, t, r, b, bw;
  short           vtht, vthh, hthl, hthw, rmbw, bmbw;
  short           tbwp3, bwm1;
  short           dummy;

  bw = View->containeritem->scheme->borderwidth;    /*border width for the View*/
  Hv_GetRectLTRB(View->local, &l, &t, &r, &b);

  Hv_VertThumbInfo (View, &vtht, &vthh, &dummy);
  Hv_HorizThumbInfo(View, &hthl, &hthw, &dummy);

  tbwp3 = (2 * bw) + 3;
  bwm1 = bw - 1;
  rmbw = r - bw;
  bmbw = b - bw;

  Hv_SetRect(Vthumb, rmbw, vtht, bw, vthh);
  Hv_SetRect(Hthumb, hthl, bmbw, hthw, bw);


  Hv_SetRect(Vslide,
	     rmbw, 
	     (short)(t + tbwp3),
	     bwm1,
	     (short)(b - 2*tbwp3 - t + 1));

  Hv_SetRect(Hslide,
	     (short)(l + bw + 1),
	     bmbw,
	     (short)(r - tbwp3 - l - bw),
	     bwm1);

  Hv_ShrinkRect(Hthumb, 1, 2);
  Hv_ShrinkRect(Vthumb, 2, 1);

}


/**
 * Hv_UpdateThumbs
 * @purpose     Update the scroll bar thumbs
 * @param     HorizontalScroll    True for horizontal thumb, False for vertical
 * @param     OldTLen   Old thumb length
 * @param     OldTWid   Old thumb width
 */

void Hv_UpdateThumbs(Hv_View    View,
		     Boolean    HorizontalScroll,
		     short     OldTLen,
		     short     OldTWid) {

/* HorizontalScroll:  if true, this was an H scroll else it was a V scroll*/

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
      paintW = Hv_sMin(OldTWid, (short)(ThumbLen - OldTLen));
    }
    else {
      paintL = Hv_sMax(OldTLen, (short)(ThumbLen + ThumbWid));
      paintW = OldTLen + OldTWid - paintL;
    }
    
    Hv_SetRect(&rect,
	       paintL,
	       (short)(Hv_RectBottom(View->local)-bw),
	       (short)(paintW+1),
	       (short)(bw-1));
    
    Hv_FillRect(&rect, View->containeritem->scheme->fillcolor);
    Hv_FillPatternRect(&rect, Hv_scrollBarColor, Hv_SCROLLPAT);
  }
  else {

    Hv_VertThumbInfo(View, &ThumbLen, &ThumbWid, &dummy);
    if (ThumbLen > OldTLen) {
      paintT = OldTLen;
      paintH = Hv_sMin(OldTWid, (short)(ThumbLen - OldTLen));
    }
    else {
      paintT = Hv_sMax(OldTLen, (short)(ThumbLen + ThumbWid));
      paintH = OldTLen + OldTWid - paintT;
    }
    
    
    Hv_SetRect(&rect,
	       (short)(Hv_RectRight(View->local)-bw),
	       paintT,
	       (short)(bw-1),
	       (short)(paintH+1));

    Hv_FillRect(&rect, View->containeritem->scheme->fillcolor);
    Hv_FillPatternRect(&rect, Hv_scrollBarColor, Hv_SCROLLPAT);
  }
  
}

/** 
 * Hv_CheckHShift
 * @purpose Make sure we dont shift beyond the default world
 * @param  dh   upon return, horizontal pixel shift
 * @param  fdh  world horizontal shift
 */

void Hv_CheckHShift(Hv_View  View,
			   short    *dh,
			   float    *fdh) {


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
    *dh = (short)(View->local->width*(*fdh)/View->world->width);
}

/** 
 * Hv_CheckVShift
 * @purpose Make sure we dont shift beyond the default world
 * @param  dv   upon return, vertical pixel shift
 * @param  fdv  world vertical shift
 */

void Hv_CheckVShift(Hv_View  View,
			   short    *dv,
			   float    *fdv) {


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
    *dv = (short)(-View->local->height*(*fdv)/View->world->height);
}

/**
 *  Hv_GetHScrollStep 
 * @purpose Get the horizontal pixel step given which part
 *          of the scroll bar we used.
 * @param  View        View being scrolled
 * @param  ScrollPart  Which part of scroll bar was used
 * @param  region      Clipping region
 * @param  StartPP     Starting point
 * @param  StopPP      Stopping point
 * @param  efflen      Effective length
 * @return  The horizontal pixel step
 */

short Hv_GetHScrollStep(Hv_View  View,
			       int     ScrollPart,
			       Hv_Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
				   short    efflen) {

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
    dh = Hv_sMin((short)(-View->hscroll->slide_step), 
		(short)(-3*View->hotrect->width/4));
    break;
    
  case Hv_INRIGHTSLIDE:           /* move picture left by slide step pixels */
    dh = Hv_sMax(View->hscroll->slide_step, 
		(short)(3*View->hotrect->width/4));
    break;
    
  case Hv_INHORIZONTALTHUMB:     /* move picture horizontally by the amount the thumb was dragged */
    dh = StopPP.x - StartPP.x;
    fdh = (dh*View->defaultworld->width)/efflen;
    dh = (short)((View->local->width*fdh)/View->world->width);
    break;
    
  default:
    break;
  }
  
  return(dh);
}


/**
 *  Hv_GetVScrollStep 
 * @purpose Get the vertical pixel step given which part
 *          of the scroll bar we used.
 * @param  View        View being scrolled
 * @param  ScrollPart  Which part of scroll bar was used
 * @param  region      Clipping region
 * @param  StartPP     Starting point
 * @param  StopPP      Stopping point
 * @param  efflen      Effective length
 * @return  The vertical pixel step
 */

short Hv_GetVScrollStep(Hv_View   View,
			       int     ScrollPart,
			       Hv_Region    region,
			       Hv_Point   StartPP,
			       Hv_Point   StopPP,
				   short    efflen) {

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
    dv = Hv_sMin((short)(-View->vscroll->slide_step), 
		(short)(-3*View->hotrect->height/4));
    break;
    
  case Hv_INDOWNSLIDE:           /* move picture up by slide step pixels */
    dv = Hv_sMax(View->vscroll->slide_step, 
		(short)(3*View->hotrect->height/4));
    break;
    
    
  case Hv_INVERTICALTHUMB:       /* move picture vertically by the amount the thumb was dragged */
    dv = StopPP.y  - StartPP.y;
    fdv = (dv*View->defaultworld->height)/efflen;
    dv = (short)((View->local->height*fdv)/View->world->height);
    break;
    
  default:
    break;
  }
  
  return dv;
}





