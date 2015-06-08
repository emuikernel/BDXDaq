/*
-----------------------------------------------------------------------
File:     Hv_slider.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with slider items */

#include "Hv.h"	  /* contains all necessary include files */


#define  Hv_SLIDERSIZE        28
#define  Hv_SLIDERINSIDE      8
#define  Hv_SLIDERGAP         ((Hv_SLIDERSIZE - Hv_SLIDERINSIDE)/2)
#define  Hv_THUMBLENGTH       20
#define  Hv_THUMBWIDTH        12
#define  Hv_SLIDERMINSIZE     (2*Hv_SLIDERGAP + 10)

/*------ local prototypes -------*/

static void Hv_PositionThumb(Hv_Item thumb);

static void Hv_ThumbInitialize(Hv_Item  Item);

static void Hv_SliderThumbCallback(Hv_Event  hvevent);

static short  Hv_DragSliderThumb(Hv_Item    slider,
				 Hv_Point   StartPP);

static void Hv_UpdateSlider(Hv_Item     slider,
			    short      newval);

static void Hv_DrawSliderThumbItem(Hv_Item    Item,
				   Hv_Region   region);

static short Hv_SliderThumbHotSpot(Hv_Item slider);

static void Hv_DrawHLinePair(short  x1,
			     short  x2,
			     short  x3,
			     short  x4,
			     short  t);

static void Hv_DrawVLinePair(short  y1,
			     short  y2,
			     short  y3,
			     short  y4,
			     short  l);


static void Hv_EditSlider(Hv_Event     hvevent);

/* ------------- Hv_DrawSliderItem ------------*/

void Hv_DrawSliderItem(Hv_Item    Item,
		       Hv_Region   region)

{
  Hv_SliderData    *slider;
  Hv_Rect          insiderect;
  short            x1, x2, x3, x4, xc, x14, x34;
  short            y1, y2, y3, y4, yc, y14, y34;
  short            l, t, r, b, w, h;

  slider = (Hv_SliderData *)(Item->data);

  Hv_CopyRect(&insiderect, Item->area);

  Hv_ShrinkRect(&insiderect, Hv_SLIDERGAP, Hv_SLIDERGAP);

  l = insiderect.left;
  t = insiderect.top;
  r = insiderect.right;
  b = insiderect.bottom;
  w = insiderect.width;
  h = insiderect.height;


  if (Hv_CheckBit(Item->drawcontrol, Hv_FRAMEAREA))
    Hv_Simple3DRect(Item->area, False, Item->scheme->fillcolor);
  else
    Hv_FillRect(Item->area, Item->scheme->fillcolor);
  
  Hv_Simple3DRect(&insiderect, False, Hv_navyBlue);

/* now the "etches" */

  if (slider->orientation == Hv_HORIZONTAL) {
    y1 = Item->area->top + 1;
    y2 = t - 2;
    y3 = b + 3;
    y4 = Item->area->bottom;
    xc = (l + r)/2;

    Hv_DrawVLinePair(y1, y2, y3, y4, l);
    Hv_DrawVLinePair(y1, y2, y3, y4, xc);
    Hv_DrawVLinePair(y1, y2, y3, y4, r-1);

    if (insiderect.width > 70) {
      x14 = l + w/4;
      x34 = l + (3*w)/4;
      Hv_DrawVLinePair(y1, y2, y3, y4, x14);
      Hv_DrawVLinePair(y1, y2, y3, y4, x34-1);
    }
  }
  else {
    x1 = Item->area->left + 1;
    x2 = l - 2;
    x3 = r + 3;
    x4 = Item->area->left + Item->area->width;
    yc = (t + b)/2;

    Hv_DrawHLinePair(x1, x2, x3, x4, t);
    Hv_DrawHLinePair(x1, x2, x3, x4, yc);
    Hv_DrawHLinePair(x1, x2, x3, x4, b-1);

    if (insiderect.height > 70) {
      y14 = t + h/4;
      y34 = t + (3*h)/4;
      Hv_DrawHLinePair(x1, x2, x3, x4, y14);
      Hv_DrawHLinePair(x1, x2, x3, x4, y34);
    }
  }
}

/*-------- Hv_SliderInitialize --------*/

void Hv_SliderInitialize(Hv_Item      Item,
			 Hv_AttributeArray attributes)

{
  Hv_View              View = Item->view;
  Hv_SliderData        *data;
  char                 maxstr[20], minstr[20], *bstr;
  XmString             xmaxst, xminst;
  short                minw, maxw, h;
  Hv_Item              parent;
  Boolean              inbox;
  short                boxfill = 0;
  short                placement;

  Item->standarddraw = Hv_DrawSliderItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;
  Item->data = (void *) Hv_Malloc(sizeof(Hv_SliderData));

  parent = Item->parent;

/* see if this slider is the child of a BOX parent */

  inbox = ((parent != NULL) && (parent->type == Hv_BOXITEM));
  if (inbox)
    boxfill = parent->scheme->fillcolor;
    
  data = (Hv_SliderData *)(Item->data);
  data->orientation = attributes[Hv_ORIENTATION].s;
  data->valmin = (short)attributes[Hv_MINVALUE].i;
  data->valmax = (short)attributes[Hv_MAXVALUE].i;
  data->current =(short)attributes[Hv_CURRENTVALUE].i;
  placement = attributes[Hv_GRANDCHILDPLACEMENT].s;

  if (data->orientation == Hv_HORIZONTAL) {
    Item->area->height = Hv_SLIDERSIZE;
    Item->area->width = Hv_sMax(Hv_SLIDERMINSIZE, Item->area->width);
    if (placement == Hv_DEFAULT)
      placement = Hv_POSITIONRIGHT;
  }
  else {
    Item->area->width = Hv_SLIDERSIZE;
    Item->area->height = Hv_sMax(Hv_SLIDERMINSIZE, Item->area->height);
    if (placement == Hv_DEFAULT)
      placement = Hv_POSITIONBELOW;
  }

  Hv_FixRectRB(Item->area);


/* place the title string above regardless of whether it is horiz or vert */

  data->titlestr = Hv_VaCreateItem(View,
				     Hv_TYPE,            Hv_TEXTITEM,
				     Hv_PARENT,          Item,
				     Hv_FONT,            Hv_fixed2,
				     Hv_TEXT,            (char *)(attributes[Hv_TITLE].v),
				     Hv_CHILDPLACEMENT,  Hv_POSITIONABOVE,
				     NULL);


/* if slider is in a box, need the proper background */

  if (inbox) {
    data->titlestr->scheme->fillcolor = boxfill;
    if (attributes[Hv_BACKGROUND].s == Hv_DEFAULT)
      Item->scheme->fillcolor = boxfill;
  }


/* see how big to make the value box */

  Hv_NumberToString(maxstr, (float)(data->valmax), 0, 0.0);
  Hv_NumberToString(minstr, (float)(data->valmin), 0, 0.0);
  xmaxst = Hv_CreateAndSizeMotifString(maxstr, Hv_fixed2, &maxw, &h);
  xminst = Hv_CreateAndSizeMotifString(minstr, Hv_fixed2, &minw, &h);
  XmStringFree(xmaxst);
  XmStringFree(xminst);

  if (maxw > minw)
    bstr = maxstr;
  else
    bstr = minstr;

  data->valstr = Hv_VaCreateItem(View,
				   Hv_TYPE,              Hv_TEXTITEM,
				   Hv_PARENT,            Item,
				   Hv_TEXT,              bstr,
				   Hv_FONT,              Hv_fixed2,
				   Hv_CHILDPLACEMENT,    placement,
				   Hv_PLACEMENTGAP,      1,
				   Hv_TEXTCOLOR,         Hv_red,
				   NULL);
/* if slider is in a box, need the proper background */

  if (inbox)
    data->valstr->scheme->fillcolor = boxfill;

  data->thumb = Hv_VaCreateItem(View,
				Hv_TYPE,            Hv_SLIDERTHUMBITEM,
				Hv_INITIALIZE,      Hv_ThumbInitialize,
				Hv_PARENT,          Item,
				Hv_SINGLECLICK,     Hv_SliderThumbCallback,
				Hv_DOUBLECLICK,     Hv_EditSlider,
				NULL);


/* see if the redraw hotrect was set for the parent */

  if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
    Hv_SetBit(&(data->thumb->drawcontrol), Hv_REDRAWHOTRECTWHENCHANGED);

/* let the thumb handle the balloon for the slider, since the slider
   is inactive */

  if (Hv_CheckBit(Item->drawcontrol, Hv_HASBALLOON)) {
    Hv_SetBit(&(data->thumb->drawcontrol), Hv_HASBALLOON);
    data->thumb->balloon = Item->balloon;
  }

  Hv_PositionThumb(Item);
  Hv_UpdateSlider(Item, data->current);
  Hv_DisableItem(Item);   /* so that can select thumb */  
  Hv_EnableItem(data->thumb);
}

/*------ Hv_PositionThumb -------*/

static void Hv_PositionThumb(Hv_Item slider)

{
  short                xc, yc;
  Hv_Item              thumb;
  Hv_SliderData       *data;
  short                hot;

  if (slider == NULL)
    return;

  data = (Hv_SliderData  *)(slider->data);
  thumb = data->thumb;
  
  Hv_GetItemCenter(slider, &xc, &yc);

  if (data->orientation == Hv_HORIZONTAL) {
    Hv_SetRect(thumb->area,
	       xc - (Hv_THUMBWIDTH/2),
	       yc - (Hv_THUMBLENGTH/2),
	       Hv_THUMBWIDTH, Hv_THUMBLENGTH);

    xc = thumb->area->left + thumb->area->width/2;
    hot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, hot - xc, 0);
  }
  else {
    Hv_SetRect(thumb->area, xc - (Hv_THUMBLENGTH/2), yc - (Hv_THUMBWIDTH/2),
	       Hv_THUMBLENGTH, Hv_THUMBWIDTH);
    yc = (thumb->area->top + thumb->area->bottom)/2;
    hot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, 0, hot - yc);
  }
  
}


/*------ Hv_GetSliderCurrentValue -----------*/

short  Hv_GetSliderCurrentValue(Hv_Item  slider)

{
  Hv_SliderData    *data;

  if (slider == NULL)
    return -32766;

  data = (Hv_SliderData  *)(slider->data);
  return data->current;
}



/*-------- Hv_SetSliderCurrentValue --------*/

void  Hv_SetSliderCurrentValue(Hv_Item  slider,
			       short   val,
			       Boolean  redraw)

{
  Hv_SliderData    *data;
  short            xc, xhot, yc, yhot;
  Hv_Item          vstr, thumb;
  char             bstr[30];

  if (slider == NULL)
    return;

  data = (Hv_SliderData  *)(slider->data);
  thumb = data->thumb;
  vstr = data->valstr;

  data->current = val;
  Hv_NumberToString(bstr, (float)(val), 0, 0.0);
  Hv_SetItemText(vstr, bstr);

  Hv_SizeTextItem(vstr, vstr->area->left, vstr->area->top);

  if (data->orientation == Hv_HORIZONTAL) {
    xc = thumb->area->left + thumb->area->width/2;
    xhot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, xhot - xc, 0);
  }
  else {
    yc = (thumb->area->top + thumb->area->bottom)/2;
    yhot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, 0, yhot - yc);
  }

  if (redraw)
    Hv_RedrawItem(slider);
}


/*-------- Hv_ThumbInitialize --------*/

static void Hv_ThumbInitialize(Hv_Item Item)

{
  Item->standarddraw = Hv_DrawSliderThumbItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->framewidth = 2;
}

/* ------- Hv_SliderThumbCallback -------*/

static void Hv_SliderThumbCallback(Hv_Event hvevent)

{
  Hv_Item        thumb = hvevent->item;
  Hv_View        View  = hvevent->view;

  Hv_Point       startPP;
  short          del;
  Region         reg;
  Hv_Item        slider = thumb->parent;


/* do nothing if parent is disabled */

  if (!Hv_ItemIsEnabled(thumb))
    return;

/* drag the thumb, if we are in it */

  Hv_SetPoint(&startPP, hvevent->where.x, hvevent->where.y); 

  del = Hv_DragSliderThumb(slider, startPP);

/* if del != 0, the thumb was indeed dragged */

  if (del != 0) {
    reg = Hv_ClipView(View);
    Hv_DrawSliderItem(slider, reg);
    Hv_DrawSliderThumbItem(thumb, reg);
    Hv_DestroyRegion(reg);

/* call the parents callback, if any */

    if (slider->singleclick != NULL) {
      hvevent->item = slider;
      slider->singleclick(hvevent);
    }

/* automaticaly redraw hotrect? */

    if (Hv_CheckBit(thumb->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
      Hv_DrawViewHotRect(View);
  }
  else
    Hv_SysBeep();
}


/*------ Hv_DragSliderThumb -------*/

static short  Hv_DragSliderThumb(Hv_Item    slider,
				 Hv_Point   StartPP)

/* drag a thumb around the slider, staying within SliderRect.
   Uses the first mouse button. Similar to DragRect*/

{
  Hv_Rect         ThumbRect;             /* Original Position of rectangle to be dragged  (NOT CHANGED) */
  Hv_Rect         MouseLimitRect;        /* Mouse must stay in here to keep ThumbRect in SliderRect*/
  Boolean         done;                  /* Controls when dragging is over */
  XEvent          Event;                 /* An X event */
  Hv_Rect         CurrentRect;           /* current rect */
  short           dh, dv;                /* offests */
  short           oldval, newval, hot, base;
  float           scale;
  Hv_Point        StopPP;                /* Final location of mouse when dragging is done */
  Hv_Item         thumb;
  Hv_SliderData   *data = (Hv_SliderData *)(slider->data);
  Boolean         horizontal;

  horizontal = (data->orientation == Hv_HORIZONTAL);
  

/* get the thumb */

  oldval = data->current;
  thumb = data->thumb;

  Hv_CopyRect(&ThumbRect, thumb->area);

  Hv_CopyRect(&MouseLimitRect, slider->area);
  Hv_ShrinkRect(&MouseLimitRect, Hv_SLIDERGAP-(ThumbRect.width/2), Hv_SLIDERGAP-(ThumbRect.height/2));

/* first get the limit rect for the mouse. The MOUSE moving out of this rectangle implies
   the DragRect has moved out of the LimitRect.*/

  MouseLimitRect.left   += (StartPP.x - ThumbRect.left);
  MouseLimitRect.top    +=  (StartPP.y - ThumbRect.top);
  MouseLimitRect.width  -= ThumbRect.width;
  MouseLimitRect.height -= ThumbRect.height;
  Hv_FixRectRB(&MouseLimitRect);

/* initialize */

  StopPP = StartPP;
  oldval = data->current;
  newval = oldval;

  if (horizontal) {
    scale = ((float)(data->valmax - data->valmin))/(slider->area->width - 1 - (2*Hv_SLIDERGAP));
    base = slider->area->left + Hv_SLIDERGAP;
  }
  else {
    scale = ((float)(data->valmax - data->valmin))/(slider->area->height - 1 - (2*Hv_SLIDERGAP));
    base = slider->area->bottom - Hv_SLIDERGAP;
  }

  done = False;

  Hv_CopyRect(&CurrentRect, &ThumbRect);
  Hv_DrawXorRect(&CurrentRect);

  dv = 0;
  dh = 0;
  while (!done) {

/* if Check For Motion or Button Releases */

      if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | Button1MotionMask, &Event)) {

	switch(Event.type) {

/* Motion Notify means the mouse is moving */

	case MotionNotify: {


	  if (horizontal) {
	    StopPP.x = Hv_sMax(MouseLimitRect.left, Hv_sMin(MouseLimitRect.right,  Event.xbutton.x));
	    dh = StopPP.x - StartPP.x;
	  }
	  else {
	    StopPP.y = Hv_sMax(MouseLimitRect.top, Hv_sMin(MouseLimitRect.bottom,  Event.xbutton.y));
	    dv = StopPP.y - StartPP.y;
	  }

	  Hv_DrawXorRect(&CurrentRect);
	  Hv_CopyRect(&CurrentRect, &ThumbRect);
	  Hv_OffsetRect(&CurrentRect, dh, dv);
	  Hv_DrawXorRect(&CurrentRect);


/* see if we moved enough to change current value */

	  if (horizontal) {
	    hot = (CurrentRect.left + CurrentRect.right)/2; 
	    newval = data->valmin + (short)(Hv_nint(scale*((float)(hot - base))));
	  }
	  else {
	    hot = (CurrentRect.top + CurrentRect.bottom)/2;
	    newval = data->valmin + (short)(Hv_nint(scale*((float)(base - hot))));
	  }

	  newval = Hv_sMin(Hv_sMax(newval, data->valmin), data->valmax);
	  Hv_UpdateSlider(slider, newval);
	  break;
	}  /* end of case MotionNotify */
	      
/* if button 1 released, we are done */

	case ButtonRelease:
	  
	  if (Event.xbutton.button == 1) 
	    done = True;
	  break;
	      
	default:
	  break;
	}

      }  /* end  if if CheckMask */
    }   /*end of while !done */

/* Final xor rect */
      
  Hv_DrawXorRect(&CurrentRect);

  if (horizontal) {
    if (dh != 0)
      Hv_OffsetRect(thumb->area, dh, 0);
    if (newval != oldval)
      return dh;
    else
      return 0;
  }
  else {
    if (dv != 0)
      Hv_OffsetRect(thumb->area, 0, dv);
    if (newval != oldval)
      return dv;
    else
      return 0;
  }

}


/* ------------- Hv_UpdateSlider ------------*/

static void Hv_UpdateSlider(Hv_Item     slider,
			    short      newval)

{
  Hv_View                View = slider->view;
  Hv_Region              reg1;
  Hv_Item                vstr;
  Hv_ColorScheme         *scheme;
  Hv_SliderData          *data = (Hv_SliderData *)(slider->data);
  char                   bstr[30];

  vstr = data->valstr;
  data->current = newval;
  Hv_NumberToString(bstr, (float)(newval), 0, 0.0);
  Hv_SetItemText(vstr, bstr);

  if (Hv_ViewIsVisible(View)) {
    scheme = vstr->scheme;
    reg1 = Hv_ClipView(View);
    Hv_FillRect(vstr->area, scheme->fillcolor);
    Hv_DrawTextItem(vstr, reg1);
    Hv_DestroyRegion(reg1);
  }
}


/* ------------- Hv_DrawSliderThumbItem ------------*/

static void Hv_DrawSliderThumbItem(Hv_Item    Item,
				   Hv_Region   region)

{
  if (!Hv_ItemIsEnabled(Item)) {
fprintf(stderr, "thumb not enabled\n");
    Hv_FillRect(Item->area, Hv_gray12);
    Hv_FrameRect(Item->area, Hv_gray8);
    return;
  }

  Hv_FillRect(Item->area, Item->scheme->fillcolor);
  Hv_Draw3DRect(Item->area, Item->scheme);
  Hv_FrameRect(Item->area, Hv_black);
}


/* ------- Hv_SliderThumbHotSpot -------*/

static short Hv_SliderThumbHotSpot(Hv_Item slider)

/* returns the horiz position (in pixels) where
   the center of the thumb should be */

{
  Hv_SliderData   *data;
  float           scale;
  short           hot, base, val;

  data = (Hv_SliderData *)(slider->data);
  val  = data->current;


  if (data->orientation == Hv_HORIZONTAL) {
    scale = ((float)(data->valmax - data->valmin))/(slider->area->width - 1 - (2*Hv_SLIDERGAP));
    base = slider->area->left + Hv_SLIDERGAP;
    hot = base + (short)(Hv_nint(((float)(val - data->valmin))/scale));
  }
  else {
    scale = ((float)(data->valmax - data->valmin))/(slider->area->height - 1 - (2*Hv_SLIDERGAP));
    base = slider->area->bottom - Hv_SLIDERGAP;
    hot = base - (short)(Hv_nint(((float)(val - data->valmin))/scale));
  }


  return hot;
}


/*------- Hv_DrawHLinePair -------*/

static void Hv_DrawHLinePair(short  x1,
			     short  x2,
			     short  x3,
			     short  x4,
			     short  t)

{
    Hv_DrawLine(x1, t,     x2, t,     Hv_white);
    Hv_DrawLine(x1, t+1,   x2, t+1,   Hv_black);
    Hv_DrawLine(x3, t,     x4, t,     Hv_white);
    Hv_DrawLine(x3, t+1,   x4, t+1,   Hv_black);
}

/*------- Hv_DrawVLinePair -------*/

static void Hv_DrawVLinePair(short  y1,
			     short  y2,
			     short  y3,
			     short  y4,
			     short  l)

{
    Hv_DrawLine(l,   y1, l,     y2, Hv_white);
    Hv_DrawLine(l+1, y1, l+1,   y2, Hv_black);
    Hv_DrawLine(l,   y3, l,     y4, Hv_white);
    Hv_DrawLine(l+1, y3, l+1,   y4, Hv_black);
}

/*------ Hv_EditSlider --------*/

static void Hv_EditSlider(Hv_Event     hvevent)

/* actually attached to the thumb!! */

{
  Hv_Item              thumb = hvevent->item;
  Hv_View              View = hvevent->view;
  Hv_Item              Item;

  static Widget        dialog = NULL;
  Widget               dummy, rowcol, rc;
  Hv_SliderData       *data;

  static Hv_Widget     minval, maxval, cval;
  int                  answer;
  short                oldcval, oldmin, oldmax;

  if ((thumb == NULL) || (thumb->parent == NULL))
    return;

  Item = thumb->parent;  /* the slider */

  data = (Hv_SliderData *) (Item->data);

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Slider Editor ", NULL);

    rowcol = Hv_DialogColumn(dialog, 6);
    rc = Hv_DialogTable(rowcol, 4, 6);

    minval = Hv_SimpleTextEdit(rc, "Minimum", NULL, 4);
    maxval = Hv_SimpleTextEdit(rc, "Maximum", NULL, 4);
    cval   = Hv_SimpleTextEdit(rc, "Current", NULL, 4);

    dummy = Hv_StandardActionButtons(rowcol, 26, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }

/* the dialog has been created */

  Hv_SetIntText(minval, data->valmin);
  Hv_SetIntText(maxval, data->valmax);
  Hv_SetIntText(cval,   data->current);

  oldcval = data->current;
  oldmin  = data->valmin;
  oldmax  = data->valmax;

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    data->valmin  = (short)(Hv_GetIntText(minval));
    data->valmax  = Hv_sMax(data->valmin+1, (short)(Hv_GetIntText(maxval)));
    data->current = Hv_sMax(data->valmin, Hv_sMin(data->valmax, (short)(Hv_GetIntText(cval))));

    
    if ((oldcval != data->current) || (oldmin != data->valmin) || (oldmax != data->valmax)) {
      oldcval = data->current;
      oldmin  = data->valmin;
      oldmax  = data->valmax;

      Hv_PositionThumb(Item);
      Hv_UpdateSlider(Item, data->current);
      
      Hv_DrawItem(Item, NULL);
      Hv_DrawItem(thumb, NULL);

/* automaticaly redraw hotrect? */

      if (Hv_CheckBit(thumb->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
	Hv_DrawViewHotRect(View);

    }

    if (answer != Hv_APPLY)
      break;
  }

}


#undef  Hv_SLIDERSIZE
#undef  Hv_SLIDERINSIDE
#undef  Hv_SLIDERGAP
#undef  Hv_SLIDERMINSIZE
#undef  Hv_THUMBLENGTH
#undef  Hv_THUMBWIDTH









