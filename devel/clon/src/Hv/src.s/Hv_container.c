/**
 * <EM>Various routines for the Hv_CONTAINER item There is one of these
 * per View, and it is essentially the border of the View.</EM>
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

/*--------- local prototypes -------*/

static void   DrawCloseBox(Hv_View);

static void   DrawGrowBox(Hv_View);

static void   DrawEnlargeBox(Hv_View);

static void   Hv_Small3DRectangle(Hv_ColorScheme *,
				  short,
				  short,
				  short,
				  short);


/**
 * Hv_InsideContainerArea
 * @purpose   Returns the part of the container that should
 *  be clipped when printing.
 * @param   View      The View in question.
 * @param   carea     Returns the "active" rectangular area.
 */

void Hv_InsideContainerArea(Hv_View   View,
							Hv_Rect   *carea) {


  short           fw, fwpbw;
  Hv_Item         cptr;
  Hv_ColorScheme  *scheme;

  if ((View == NULL) || (carea == NULL))
    return;

  cptr = View->containeritem;

  scheme = cptr->scheme;
  fw = scheme->framewidth;
  fwpbw = scheme->framewidth + scheme->borderwidth;
  
  Hv_CopyRect(carea, cptr->area);

  if (Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE + Hv_ENLARGEABLE)) {
    carea->right -= fwpbw;
    carea->bottom -= fwpbw;
  }
  else {
    carea->right -= fw;
    carea->bottom -= fw;
  }

  Hv_FixRectWH(carea);

}


/**
 * Hv_DrawContainerItem
 * @purpose Drawing the Hv_CONTAINER item.
 * @param    Item     The container item.
 * @param    region   The clipping region.
 */

void Hv_DrawContainerItem (Hv_Item   Item,
						   Hv_Region region) {
     

  Hv_View         View = Item->view;
  short           l, t, r, b;
  Hv_Rect         *area;
  Hv_ColorScheme  *scheme;

  Hv_Region       cliprgn = NULL;


  if (Item == NULL)
	  return;

/* NOTE: The area is the rectangle around EVERYTHING including
   scroll (if present)  and title areas */

  area   = Item->area;
  scheme = Item->scheme;

/* we will get l, t, r, b representing the part of the conatiner
   that is a 3D rect. This is the "whole" view minus the title area
   and minus the scroll bars */

  Hv_GetRectLTRB(area, &l, &t, &r, &b);
  
  t += scheme->borderwidth + 1;
  
  if (!Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE + Hv_ENLARGEABLE)) {
    b -= 1;
    r -= 1;
  }
  else {
    b = b - 1 - scheme->borderwidth;
    r = r - 1 - scheme->borderwidth;
  }
  
/* subtract out the hotrect */

  if ((region != NULL) && (View->hotrectborderitem != NULL)) {
    Hv_CopyRegion(&cliprgn, region);
    Hv_RemoveRectFromRegion(&cliprgn, View->hotrectborderitem->area);
    Hv_SetClippingRegion(cliprgn);
    Hv_DestroyRegion(cliprgn);
  }

  Hv_FillRect(area, scheme->fillcolor);
    
/* draw the frame boxes */

  Hv_Draw3DRectangle(l,  t, (short)(r-l),   (short)(b-t), scheme);

  if (Hv_inPostscriptMode)
    Hv_PSComment("Framing a container");

  Hv_FrameRectangle((short)(l-1), t, (short)(r-l+1), (short)(b-t), Hv_black);
  
/* now the title */

  Hv_DrawViewTitle(View);
  
/* now the grow box and the close box */

  if (Hv_CheckBit(View->drawcontrol, Hv_ENLARGEABLE)) {
    DrawGrowBox(View);   /* Origin of GrowBox & width */
    DrawEnlargeBox(View);
  }
  
/* Scroll bars? */

  if (Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE))
    Hv_DrawScrollBars(View, region);

/* all Views have close boxes */

  Hv_FrameRectangle(Item->area->left,
	                Item->area->top,
		    (short)(Item->area->width - 1),
			(short)(Item->area->height - 1),
			Hv_black);


  DrawCloseBox(View);
}

/**
 * Hv_DrawViewTitle
 * @purpose    Draw the View's title.
 * @param  View  The View in question.
 */

void   Hv_DrawViewTitle(Hv_View View) {

/* this routine draws the View title on the View making sure to add the lines to each side */

  Hv_String       *str;
  short           xmw, xmh;		        /* String width  in pixels */
  short           xc;
  Hv_Rect        *area;
  unsigned long   old_fg;

  if(View == NULL)
	  return;

  str = View->containeritem->str;

  area = View->containeritem->area;

  Hv_DrawViewTitleBorder(View, False, True);

  if (str->text != NULL) {   /* if the string exists, write it in the center */
    old_fg = Hv_SwapForeground(Hv_black);
    Hv_CompoundStringDimension(str, &xmw, &xmh);
    
    xc = area->left + (area->width - (short)xmw)/2;
    
    Hv_DrawCompoundString(xc, (short)(area->top + 2), str);
    Hv_SetForeground(old_fg);                  /* revert the color to thd old foreground color */
  }
}


/**
 * Hv_DrawGrowBox
 * @purpose    Draw the View's grow box.
 * @param  View  The View in question.
 */

static void   DrawGrowBox(Hv_View View) {

/* draw the box that allows the use to resize the View */

  short              bw, w3, l, t, w23; 
  Hv_ColorScheme     *scheme;
  Hv_Rect            *area;

  area   = View->containeritem->area;
  scheme = View->containeritem->scheme;

  bw = scheme->borderwidth;
  w3 = bw/3;
  w23 = 1 + bw/2;

  l = area->left + area->width  - bw - 1;
  t = area->top  + area->height - bw - 1;

  if (Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE)) {
    Hv_Simple3DRectangle(l, t, (short)(bw-1), (short)(bw-1), True, -1);
    Hv_Simple3DRectangle((short)(l+3), (short)(t+3), w23, w23, False, Hv_canvasColor);
    Hv_Simple3DRectangle((short)(l+3), (short)(t+3), w3, w3, True, scheme->fillcolor);
  }
  else {
    Hv_Simple3DRectangle((short)(l+1), (short)(t+1), (short)(bw-2), (short)(bw-2), False, Hv_canvasColor);
  }
}

/**
 * Hv_DrawCloseBox
 * @purpose    Draw the View's close box.
 * @param  View  The View in question.
 */

static void DrawCloseBox(Hv_View View) {

  short           bw, w; 
  Hv_ColorScheme  *scheme;
  Hv_Rect         *area;

  area   = View->containeritem->area;
  scheme = View->containeritem->scheme;
  bw = scheme->borderwidth - 1;
  w = bw-6;

  Hv_Simple3DRectangle((short)(area->left+1), (short)(area->top+1), bw, bw, True, -1);
  Hv_Simple3DRectangle((short)(area->left+4), (short)(area->top+4), w, w, False, Hv_canvasColor);
}

/**
 * Hv_DrawEnlargeBox
 * @purpose    Draw the View's enlarge box.
 * @param  View  The View in question.
 */

static void DrawEnlargeBox(Hv_View View) {
  short           bw, bwm1, w, l, t, lm1, tpbw; 
  Hv_ColorScheme  *scheme;
  Hv_Rect         *area;

  area   = View->containeritem->area;
  scheme = View->containeritem->scheme;

  bw = scheme->borderwidth - 1;
  bwm1 = bw - 1;
  w = bwm1 - 5;
  l = area->left + area->width - bw - 1;
  t = area->top + 1;
  lm1 = l - 1;
  tpbw = t + bw;

  Hv_Small3DRectangle(scheme, l, t, bwm1, bwm1);

  Hv_Simple3DRectangle((short)(l+3), (short)(area->top+3), w, w, True, -1);
  Hv_DrawLine(lm1, t, lm1, tpbw, Hv_black);
  Hv_DrawLine(lm1, tpbw, (short)(l+bw), tpbw, Hv_black);
}


/**
 * Hv_DrawViewTitleBorder
 * @purpose    Draw the border around the View's title.
 * @param  View  The View in question.
 * @param  MinClip  If true, sets a min clipping region and then when done does a Hv_FullClip.
 * @param  Erase    If true, first does an erasure.
 */

void Hv_DrawViewTitleBorder(Hv_View View,
			    Boolean MinClip,
				Boolean Erase) {

  short            l, t, w, bw;
  Hv_ColorScheme   *scheme;
  Hv_Rect          *area;
  Hv_Region        reg1, reg2, reg3;

  area = View->containeritem->area;
  scheme = View->containeritem->scheme;
  bw = scheme->borderwidth;        /* store the borderwidth so we dont have to keep doing it the long way */

  w = area->width - bw - 2;
  if (Hv_CheckBit(View->drawcontrol, Hv_ENLARGEABLE))
    w -= bw;

  l = area->left+bw+1;
  t = area->top+1;


  if (MinClip) {
    reg1 = Hv_GetViewRegion(View);
    reg2 = Hv_RectangleRegion(l, t, w, bw);
    reg3 = Hv_IntersectRegion(reg1, reg2);
    Hv_SetClippingRegion(reg3);
    Hv_DestroyRegion(reg1);
    Hv_DestroyRegion(reg2);
    Hv_DestroyRegion(reg3);
  }

  if (Erase)
    Hv_FillRectangle(l, t, w, (short)(bw-1), scheme->fillcolor);
  
  Hv_Small3DRectangle(scheme, l, t, w, (short)(bw-1));

  if (MinClip)
    Hv_FullClip();
}

/* ------- Hv_Small3DRectangle --------*/

static void Hv_Small3DRectangle(Hv_ColorScheme *scheme,
				short l,
				short t,
				short w,
				short h)

{
  short  r, b;

  r = l + w;
  b = t + h;

  Hv_DrawLine(l, b, l, t, scheme->lighttrimcolor);
  Hv_DrawLine(l, t, r, t, scheme->lighttrimcolor);
  Hv_DrawLine(r, t, r, b, scheme->darktrimcolor);
  Hv_DrawLine(r, b, l, b, scheme->darktrimcolor);
} 

