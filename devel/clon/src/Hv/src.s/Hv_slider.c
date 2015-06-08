/**
 * <EM>Deals with slider items on <B>views.</B></EM>
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
#include "Hv_drag.h"


#define  Hv_SLIDERSIZE        28
#define  Hv_SLIDERINSIDE      8
#define  Hv_SLIDERGAP         ((Hv_SLIDERSIZE - Hv_SLIDERINSIDE)/2)
#define  Hv_THUMBLENGTH       20
#define  Hv_THUMBWIDTH        12
#define  Hv_SLIDERMINSIZE     (2*Hv_SLIDERGAP + 10)

static Hv_DragData    thedd = NULL;

/*------ local prototypes -------*/

static void Hv_SliderHandler(Hv_XEvent  *);

static void Hv_PositionThumb(Hv_Item thumb);

static void Hv_ThumbInitialize(Hv_Item  Item);

static void Hv_SliderThumbCallback(Hv_Event  hvevent);

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

/**
 * Hv_DrawSliderItem
 * @purpose   Draw the slider item.
 * @param     Item    The slider item in question.
 * @param     region  Clipping resion.
 */

void Hv_DrawSliderItem(Hv_Item    Item,
		       Hv_Region   region) {

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
    Hv_DrawVLinePair(y1, y2, y3, y4, (short)(r-1));

    if (insiderect.width > 70) {
      x14 = l + w/4;
      x34 = l + (3*w)/4;
      Hv_DrawVLinePair(y1, y2, y3, y4, x14);
      Hv_DrawVLinePair(y1, y2, y3, y4, (short)(x34-1));
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
    Hv_DrawHLinePair(x1, x2, x3, x4, (short)(b-1));

    if (insiderect.height > 70) {
      y14 = t + h/4;
      y34 = t + (3*h)/4;
      Hv_DrawHLinePair(x1, x2, x3, x4, y14);
      Hv_DrawHLinePair(x1, x2, x3, x4, y34);
    }
  }
}

/**
 * Hv_SliderInitialize
 * @purpose   Initialize the slider item.
 * @param     Item        The slider item in question.
 * @param     attributes  The attribute array.
 */

void Hv_SliderInitialize(Hv_Item      Item,
			 Hv_AttributeArray attributes) {

  Hv_View              View = Item->view;
  Hv_SliderData        *data;
  char                 maxstr[20], minstr[20], *bstr;
  Hv_CompoundString    xmaxst, xminst;
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
  xmaxst = Hv_CreateAndSizeCompoundString(maxstr, Hv_fixed2, &maxw, &h);
  xminst = Hv_CreateAndSizeCompoundString(minstr, Hv_fixed2, &minw, &h);
  Hv_CompoundStringFree(xmaxst);
  Hv_CompoundStringFree(xminst);

  if (maxw > minw)
    bstr = maxstr;
  else
    bstr = minstr;

  if (attributes[Hv_SHOWVALUE].i) {

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
  }


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

	if (Item->balloon != NULL) {
      Hv_InitCharStr(&(data->thumb->balloon), Item->balloon);
	  Hv_Free(Item->balloon);
	  Item->balloon = NULL;
	}
  }

  Hv_PositionThumb(Item);
  Hv_UpdateSlider(Item, data->current);
  Hv_DisableItem(Item);   /* so that can select thumb */  
  Hv_EnableItem(data->thumb);
}

/**
 * Hv_PositionThumb
 * @purpose   Position the thumb in the right place.
 * @param     slider    The slider item in question.
 * @local
 */

static void Hv_PositionThumb(Hv_Item slider) {
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
	       (short)(xc - (Hv_THUMBWIDTH/2)),
	       (short)(yc - (Hv_THUMBLENGTH/2)),
	       Hv_THUMBWIDTH, Hv_THUMBLENGTH);

    xc = thumb->area->left + thumb->area->width/2;
    hot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, (short)(hot - xc), 0);
  }
  else {
    Hv_SetRect(thumb->area, 
		(short)(xc - (Hv_THUMBLENGTH/2)), 
		(short)(yc - (Hv_THUMBWIDTH/2)),
		Hv_THUMBLENGTH,
		Hv_THUMBWIDTH);

    yc = (thumb->area->top + thumb->area->bottom)/2;
    hot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, 0, (short)(hot - yc));
  }
  
}

/**
 * Hv_GetSliderCurrentValue
 * @purpose   Get the current value of the slider.
 * @param     slider    The slider item in question.
 * @return    Current slider value.
 */

short  Hv_GetSliderCurrentValue(Hv_Item  slider) {
  Hv_SliderData    *data;

  if (slider == NULL)
    return -32766;

  data = (Hv_SliderData  *)(slider->data);
  return data->current;
}


/**
 * Hv_SetSliderCurrentValue
 * @purpose   Set the current value of the slider.
 * @param     slider    The slider item in question.
 * @param     val       The value to set.
 * @param     redraw    Redraw the slider item if this is True.
 */

void  Hv_SetSliderCurrentValue(Hv_Item  slider,
			       short   val,
			       Boolean  redraw) {
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

  if (vstr != NULL) {
      Hv_NumberToString(bstr, (float)(val), 0, 0.0);
      Hv_SetItemText(vstr, bstr);

      Hv_SizeTextItem(vstr, vstr->area->left, vstr->area->top);
  }

  if (data->orientation == Hv_HORIZONTAL) {
    xc = thumb->area->left + thumb->area->width/2;
    xhot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, (short)(xhot - xc), 0);
  }
  else {
    yc = (thumb->area->top + thumb->area->bottom)/2;
    yhot = Hv_SliderThumbHotSpot(slider);
    Hv_OffsetRect(thumb->area, 0, (short)(yhot - yc));
  }

  if (redraw)
    Hv_RedrawItem(slider);
}



/**
 * Hv_ThumbInitialize
 * @purpose   Get the current value of the slider.
 * @param     Item    The slider thumb  item in question.
 * @local
 */


static void Hv_ThumbInitialize(Hv_Item Item) {
  Item->standarddraw = Hv_DrawSliderThumbItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->framewidth = 2;
}


/**
 * Hv_SliderThumbCallback
 * @purpose   deal with sliding the slider.
 * @param     hvevent    The causal event.
 * @local
 */

static void Hv_SliderThumbCallback(Hv_Event hvevent) {
  
/* create the drag data */

  thedd = Hv_MallocDragData(hvevent->view,
			    hvevent->item,
			    &(hvevent->where),
			    NULL,
			    NULL,
			    Hv_NODRAGPOLICY,
			    -1,
			    NULL);

  Hv_AlternateEventHandler = Hv_SliderHandler;
}

/**
 * Hv_UpdateSlider
 * @purpose  Update the slider item.
 * @slider   The slider item in question.
 * @param    newval   The new value of the slider.
 * @local
 */

static void Hv_UpdateSlider(Hv_Item     slider,
			    short      newval) {
  Hv_View                View = slider->view;
  Hv_Region              reg1;
  Hv_Item                vstr;
  Hv_ColorScheme         *scheme;
  Hv_SliderData          *data = (Hv_SliderData *)(slider->data);
  char                   bstr[30];

  vstr = data->valstr;
  data->current = newval;

  if (vstr != NULL) {
      Hv_NumberToString(bstr, (float)(newval), 0, 0.0);
      Hv_SetItemText(vstr, bstr);
  }

  if (Hv_ViewIsVisible(View)) {
      reg1 = Hv_ClipView(View);
      if (vstr != NULL) {
	  scheme = vstr->scheme;
	  Hv_FillRect(vstr->area, scheme->fillcolor);
	  Hv_DrawTextItem(vstr, reg1);
      }
      Hv_DestroyRegion(reg1);
  }
}


/* ------------- Hv_DrawSliderThumbItem ------------*/

static void Hv_DrawSliderThumbItem(Hv_Item    Item,
				   Hv_Region   region) {
  if (!Hv_ItemIsEnabled(Item)) {
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
    Hv_DrawLine(x1, (short)(t+1),   x2, (short)(t+1),   Hv_black);
    Hv_DrawLine(x3, t,     x4, t,     Hv_white);
    Hv_DrawLine(x3, (short)(t+1),   x4, (short)(t+1),   Hv_black);
}

/*------- Hv_DrawVLinePair -------*/

static void Hv_DrawVLinePair(short  y1,
			     short  y2,
			     short  y3,
			     short  y4,
			     short  l)

{
    Hv_DrawLine(l,   y1, l,     y2, Hv_white);
    Hv_DrawLine((short)(l+1), y1, (short)(l+1),   y2, Hv_black);
    Hv_DrawLine(l,   y3, l,     y4, Hv_white);
    Hv_DrawLine((short)(l+1), y3, (short)(l+1),   y4, Hv_black);
}

/*
 * Hv_EditSlider
 * @purpose  Double click editor for the slider.
 * @param  hvevent  The causal event.
 */

static void Hv_EditSlider(Hv_Event     hvevent) {

  Hv_Item              thumb = hvevent->item;
  Hv_View              View = hvevent->view;
  Hv_Item              Item;

  static Hv_Widget     dialog = NULL;
  Hv_Widget            dummy, rowcol, rc;
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
    data->valmax  = Hv_sMax((short)(data->valmin+1), 
		(short)(Hv_GetIntText(maxval)));
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

/**
 * Hv_SliderHandler
 * @purpose   This is the alternate event handler called when sliding the
 *            slider thumb. When dragging is terminated, the event
 *            handler is restored and the callback is called.
 * @param     event    Event pointer.
 * @local
 */

static void Hv_SliderHandler(Hv_XEvent *event) {

  static Boolean  firstEntry = True; /* used for initialization */
  Boolean           done = False;
  Hv_EventData      hvevent;
  Hv_SliderData    *data;
  Hv_Item           thumb;
  Hv_Item           slider;
  static short      dh = 0;
  static short      dv = 0;
  Hv_Region         reg;
  Boolean           horizontal;
  short             newval;
  short             hot;
  static short      oldval;
  static float      scale;
  static short      base;
  static Hv_Rect    thumbrect;
  static Hv_Region  vrgn = NULL;

  if (event == NULL)
    return;

  if ((thedd == NULL) || 
      (thedd->view == NULL) || 
      (thedd->item == NULL) ||
      (thedd->item->parent == NULL)) {
    fprintf(stderr, "Serious problem in Hv_SlideHandler\n");
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    return;
  }

  thumb = thedd->item;
  slider = thumb->parent;
  data = (Hv_SliderData *)(slider->data);
  horizontal = (data->orientation == Hv_HORIZONTAL);

  if (firstEntry) {

    dh = 0;
    dv = 0;

/* do nothing if thumb is disabled */

    if (!Hv_ItemIsEnabled(thumb)) {
      Hv_FreeDragData(thedd);
      thedd = NULL;
      firstEntry = True;
      Hv_AlternateEventHandler = NULL;
      return;
    }

/* set up limit rect */

    Hv_CopyRect(&thumbrect, thumb->area);

    Hv_CopyRect(&(thedd->limitrect), slider->area);
    Hv_ShrinkRect(&(thedd->limitrect), 
		  (short)(Hv_SLIDERGAP-(thumbrect.width/2)), 
		  (short)(Hv_SLIDERGAP-(thumbrect.height/2)));
    
/*
 * first get the limit rect for the mouse. The MOUSE moving out of
 * this rectangle implies the DragRect has moved out of the LimitRect.
 */

    thedd->limitrect.left   += (thedd->startpp.x - thumbrect.left);
    thedd->limitrect.top    +=  (thedd->startpp.y - thumbrect.top);
    thedd->limitrect.width  -= thumbrect.width;
    thedd->limitrect.height -= thumbrect.height;
    Hv_FixRectRB(&(thedd->limitrect));

    Hv_CopyRect(&(thedd->finalrect), thumb->area);

    vrgn = Hv_ClipView(thedd->view);
    Hv_DrawXorRect(&(thedd->finalrect));
    
    Hv_SetPoint(&(thedd->finalpp), thedd->startpp.x, thedd->startpp.y);
    oldval = data->current;
    newval = oldval;

    if (horizontal) {
      scale = ((float)(data->valmax - data->valmin))/
	(slider->area->width - 1 - (2*Hv_SLIDERGAP));
      base = slider->area->left + Hv_SLIDERGAP;
    }
    else {
      scale = ((float)(data->valmax - data->valmin))/
	(slider->area->height - 1 - (2*Hv_SLIDERGAP));
      base = slider->area->bottom - Hv_SLIDERGAP;
    }
  
    firstEntry = False;
  }


  Hv_SetClippingRegion(vrgn);

/* handle the event */

  switch(event->type) {
  case Hv_motionNotify:

    if (horizontal) {
      thedd->finalpp.x = Hv_sMax(thedd->limitrect.left,
			 Hv_sMin(thedd->limitrect.right, 
				 Hv_GetXEventX(event)));
      dh = thedd->finalpp.x - thedd->startpp.x;
    }
    else {
      thedd->finalpp.y = Hv_sMax(thedd->limitrect.top,
			 Hv_sMin(thedd->limitrect.bottom, 
				 Hv_GetXEventY(event)));
      dv = thedd->finalpp.y - thedd->startpp.y;
    }
    
    Hv_Println("DH = %d   DV = %d ", dh, dv);

    Hv_DrawXorRect(&(thedd->finalrect));
    Hv_CopyRect(&(thedd->finalrect), &thumbrect);
    Hv_OffsetRect(&(thedd->finalrect), dh, dv);
    Hv_DrawXorRect(&(thedd->finalrect));
    

/* see if we moved enough to change current value */

    if (horizontal) {
      hot = (thedd->finalrect.left + thedd->finalrect.right)/2; 
      newval = data->valmin + (short)(Hv_nint(scale*((float)(hot - base))));
    }
    else {
      hot = (thedd->finalrect.top + thedd->finalrect.bottom)/2;
      newval = data->valmin + (short)(Hv_nint(scale*((float)(base - hot))));
    }
    
    newval = Hv_sMin(Hv_sMax(newval, data->valmin), data->valmax);

    Hv_UpdateSlider(slider, newval);

    break;
    
  case Hv_buttonRelease: case Hv_buttonPress:
    done = True;
    break;

  }





/* now update the slider */

  if (done) {

/* Final xor rect */
      
    Hv_DrawXorRect(&(thedd->finalrect));
    Hv_DestroyRegion(vrgn);

    if ((dh != 0) || (dv != 0)) {
      Hv_OffsetRect(thumb->area, dh, dv);
      reg = Hv_ClipView(thedd->view);
      Hv_DrawSliderItem(slider, reg);
      Hv_DrawSliderThumbItem(thumb, reg);
      Hv_DestroyRegion(reg);


/* call the parents callback, if any */

      hvevent.item = slider;
      hvevent.view = thedd->view;
      
      if (slider->singleclick != NULL) {
	hvevent.item = slider;
	slider->singleclick(&hvevent);
      }

/* automaticaly redraw hotrect? */

      if (Hv_CheckBit(thumb->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
	Hv_DrawViewHotRect(thedd->view);
    } /* end dh | dv != 0*/

    dh = 0;
    dv = 0;

    Hv_FreeDragData(thedd);
    thedd = NULL;
    firstEntry = True;
    Hv_AlternateEventHandler = NULL;
  }

}

#undef  Hv_SLIDERSIZE
#undef  Hv_SLIDERINSIDE
#undef  Hv_SLIDERGAP
#undef  Hv_SLIDERMINSIZE
#undef  Hv_THUMBLENGTH
#undef  Hv_THUMBWIDTH









