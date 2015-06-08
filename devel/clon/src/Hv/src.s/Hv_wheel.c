/**
 * <EM>Routines that deal with the Hv_WHEELITEM.</EM>
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

#include "Hv.h"	  /* contains all necessary include files */
#include "Hv_drag.h"


#define  Hv_MINWHEELLENGTH     78
#define  Hv_WHEELWIDTH      16

static Hv_DragData    thedd = NULL;

/*------ local prototypes -------*/

static void Hv_WheelHandler(Hv_XEvent  *);


static void  Hv_ChangeWheelItemText(Hv_Item  Item,
				    char    *str);

static void   Hv_DrawOneSymbol(Hv_Point  *hvp,
			       short     sym);

static void Hv_UpdateWheel(Hv_Item     wheel,
			   short      newval);


static void Hv_WheelCallback(Hv_Event  hvevent);


static void Hv_SetWheelRects(Hv_Item  Item,
			     Hv_Rect  *rect,
			     Hv_Rect  *increct,
			     Hv_Rect  *decrect);

static Boolean Hv_CheckForDone(int   pause,
			       Hv_Rect *rect,
			       Hv_Point *pp);

static void EditWheelColor(Hv_Widget  w);

static Boolean UpdateOnly = False;

static Hv_Widget  bgcolor;
static short      newbgcolor;

/**
 * Hv_DrawWheelItem
 * @purpose  To draw the wheel item
 * @param    Item     The item to draw
 * @param    region   The clipping region
 */

void Hv_DrawWheelItem(Hv_Item     Item,
		      Hv_Region   region) {

  Hv_WheelData    *wheel;
  short           l, t, r, b, w, h;
  short           del;
  short           marks[9];
  short           gaps[9];
  short           y0[9];
  int             bigdim, wd, wc;

  Hv_Rect         area, rect, increct, decrect;
  int             i;
  Hv_Region       reg1, reg2, reg3;
  Hv_View         View = Item->view;
  Hv_Point        hvp;

  wheel = (Hv_WheelData *)(Item->data);
  if (wheel->orientation == Hv_HORIZONTAL)
    bigdim = Item->area->width;
  else
    bigdim = Item->area->height;
  
  wd = (bigdim - 1)/9;

/* the offsets should add to -(2* numlimes) e.g. -2*9 = -18 */

  gaps[0] = wd-6;
  gaps[1] = wd-5;
  gaps[2] = wd;
  gaps[3] = wd+1;
  gaps[4] = wd+2;
  gaps[5] = wd+1;
  gaps[6] = wd;
  gaps[7] = wd-5;
  gaps[8] = wd-6;
  
  y0[0] = -gaps[0];
  for (i = 1; i < 9; i++)
    y0[i] = y0[i-1] + gaps[i-1];
  
  
  del = (wheel->current - wheel->valmin) % gaps[0];

  Hv_SetWheelRects(Item, &area, &increct, &decrect);

/* area is the part that the wheel is in */

  Hv_GetRectLTRB(&area, &l, &t, &r, &b);
  w = area.width;
  h = area.height;

  if (!UpdateOnly) {
    Hv_Simple3DRect(Item->area, False, Item->color);
    if (wheel->orientation == Hv_HORIZONTAL) {
      Hv_SetPoint(&hvp, (short)(r+6), (short)((t+b)/2));
      Hv_DrawOneSymbol(&hvp, Hv_RIGHTARROWSYMBOL);
      Hv_SetPoint(&hvp, (short)(l-6), (short)((t+b)/2));
      Hv_DrawOneSymbol(&hvp, Hv_LEFTARROWSYMBOL);
    }
    else {
      Hv_SetPoint(&hvp, (short)((l+r)/2), (short)(t-6));
      Hv_DrawOneSymbol(&hvp, Hv_UPARROWSYMBOL);
      Hv_SetPoint(&hvp, (short)((l+r)/2), (short)(b+6));
      Hv_DrawOneSymbol(&hvp, Hv_DOWNARROWSYMBOL);
    }
  }

  
  reg1 = Hv_RectRegion(&area);
  reg2 = Hv_GetViewRegion(View);
  reg3 = Hv_IntersectRegion(reg1, reg2);
  Hv_SetClippingRegion(reg3);
  Hv_DestroyRegion(reg1);
  Hv_DestroyRegion(reg2);
  Hv_DestroyRegion(reg3);

/* now the shading */

  if (wheel->orientation == Hv_HORIZONTAL)
    wc = (area.width/10) + 1;
  else
    wc = (area.height/10) + 1;

  for (i = 0; i < 10; i++) {
    if (wheel->orientation == Hv_HORIZONTAL)
      Hv_SetRect(&rect, (short)(l+wc*i), t, (short)wc, h);
    else
      Hv_SetRect(&rect, l, (short)(t+wc*i), w, (short)wc);
    Hv_FillRect(&rect, (short)(Hv_gray15+i));
  }
  
/* now the lines */

  for (i = 0; i < 9; i++)
    if (wheel->orientation == Hv_HORIZONTAL)
      marks[i] = l + y0[i] + (gaps[i]*del)/gaps[0];
    else
      marks[i] = t + gaps[0] - 1 + y0[i] - (gaps[i]*del)/gaps[0];
  
  if (wheel->orientation == Hv_HORIZONTAL)
    for (i = 0; i < 9; i++) {
      Hv_DrawLine(marks[i],   (short)(t-1), marks[i],   b, (short)(Hv_gray16 + (i%3)));
      Hv_DrawLine((short)(marks[i]+1), (short)(t-1), (short)(marks[i]+1), b, Hv_black);
    }
  else
    for (i = 0; i < 9; i++) {
      Hv_DrawLine((short)(l-1), marks[i],   r, marks[i],   (short)(Hv_gray16 + (i%3)));
      Hv_DrawLine((short)(l-1), (short)(marks[i]+1), r, (short)(marks[i]+1), Hv_black);
    }
  
  
  if (region != NULL)
    Hv_SetClippingRegion(region);

  Hv_Flush();
}

/**
 * Hv_WheelInitialize
 * @purpose  Initialization of the wheel item
 * @param     Item        The wheel item
 * @param     attributes  The attribute array
 * @note Specific attributes (in addition to those common to all items)
 * @table
 * |R Attribute              | Type    | Default            | Comment |r
 * |R Hv_BACKGROUND          | short   | Hv_navyBlue        | Color under the wheel |r
 * |R Hv_CURRENTVALUE        | int     | 50                 | Current wheel value |r
 * |R Hv_DOUBLECLICK         | Hv_FunctioPtr    | Hv_EditWheelValues | Wheel editor |r
 * |R Hv_FONT                | short   | Hv_fixed2          | Font for all text |r
 * |R Hv_GRANDCHILDPLACEMENT | int     | None               | For placing within a box |r
 * |R Hv_MINVALUE            | int     | 0                  | Minimum value of wheel |r
 * |R Hv_MAXVALUE            | int     | 100                | Maximum value of wheel |r
 * |R Hv_ORIENTATION         | short   | Hv_HORIZONTAL      | Other choice: Hv_VERTICAL |r
 * |R Hv_TEXTCOLOR           | short   | Hv_black           | Text color |r
 * |R Hv_TITLE               | char *  | NULL               | Optional title |r
 * |R Hv_SHOWVALUE           | Boolean | True               | If True, current value is displayed |r
 * @endtable
 */

void Hv_WheelInitialize(Hv_Item           Item,
			Hv_AttributeArray attributes) {

  Hv_View              View = Item->view;
  Hv_WheelData        *data;
  short                xc, yc;
  Hv_Item              parent;
  Boolean              inbox;
  short                boxfill = 0;
  int                  placement;
  int                  wd;
  char                 bstr[30];
  short                gap;

  Item->standarddraw = Hv_DrawWheelItem;

/* redirect the users single click so that Hv's can be called first! */

  if (Item->singleclick != NULL)
    Item->checkenlarge = Item->singleclick;

  if (attributes[Hv_DOUBLECLICK].v == NULL)
    Item->doubleclick = Hv_EditWheelValues;

  if (attributes[Hv_BACKGROUND].s != Hv_DEFAULT)
    Item->color = attributes[Hv_BACKGROUND].s;
  else
    Item->color = Hv_navyBlue;
      

  Item->singleclick = Hv_WheelCallback;
      
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;
  Item->data = (void *) Hv_Malloc(sizeof(Hv_WheelData));

  parent = Item->parent;

/* see if this wheel is the child of a BOX parent */

  inbox = ((parent != NULL) &&
	   ((parent->type == Hv_BOXITEM) || (parent->type == Hv_RADIOCONTAINERITEM)));

  if (inbox)
    boxfill = parent->scheme->fillcolor;
    
  data = (Hv_WheelData *)(Item->data);
  data->orientation = attributes[Hv_ORIENTATION].s;
  data->valmin = (short)attributes[Hv_MINVALUE].i;
  data->valmax = (short)attributes[Hv_MAXVALUE].i;
  data->current =(short)attributes[Hv_CURRENTVALUE].i;
  placement = attributes[Hv_GRANDCHILDPLACEMENT].i;

  if (data->orientation == Hv_HORIZONTAL) {
    gap = 3;
    Item->area->height = Hv_WHEELWIDTH;

/* only certain lengths are permitted so that the shading
   works out */

    wd = (Item->area->width - 1)/9;
    Item->area->width = 9*wd + 1;

    Item->area->width = Hv_sMax(Item->area->width, Hv_MINWHEELLENGTH);
    if (placement == Hv_DEFAULT)
      placement = Hv_POSITIONRIGHT;
  }
  else {
    gap = 0;

/* only certain heights are permitted so that the shading
   works out */

    wd = (Item->area->height - 1)/9;
    Item->area->height = 9*wd + 1;

    Item->area->height = Hv_sMax(Item->area->height, Hv_MINWHEELLENGTH);

    Item->area->width = Hv_WHEELWIDTH;
    if (placement == Hv_DEFAULT)
      placement = Hv_POSITIONBELOW;
  }

  Hv_FixRectRB(Item->area);
  Hv_GetItemCenter(Item, &xc, &yc);


/* place the title string above regardless of whether it is horiz or vert */

  if (attributes[Hv_TITLE].v != NULL) {
    data->titlestr = Hv_VaCreateItem(View,
				     Hv_TYPE,            Hv_TEXTITEM,
				     Hv_PARENT,          Item,
				     Hv_FONT,            attributes[Hv_FONT].s,
				     Hv_TEXT,            (char *)(attributes[Hv_TITLE].v),
				     Hv_CHILDPLACEMENT,  Hv_POSITIONABOVE,
				     Hv_PLACEMENTGAP,    0,
				     NULL);


/* if wheel is in a box, need the proper background */

    if (inbox) { 
      data->titlestr->scheme->fillcolor = boxfill;
      data->titlestr->str->fillcol = boxfill;
    }
  }
  else
    data->titlestr = NULL;

  Hv_NumberToString(bstr, (float)(data->current), 0, 0.0);

  if (attributes[Hv_SHOWVALUE].i) {
      data->valstr = Hv_VaCreateItem(View,
				     Hv_TYPE,              Hv_TEXTITEM,
				     Hv_PARENT,            Item,
				     Hv_TEXT,              bstr,
				     Hv_FONT,              attributes[Hv_FONT].s,
				     Hv_CHILDPLACEMENT,    placement,
				     Hv_PLACEMENTGAP,      gap,
				     Hv_TEXTCOLOR,         attributes[Hv_TEXTCOLOR].s,
				     NULL);

/* if wheel is in a box, need the proper background */

      if (inbox) {
	  data->valstr->scheme->fillcolor = boxfill;
	  data->valstr->str->fillcol = boxfill;
	}
  }
  else
      data->valstr = NULL;

}

/**
 * Hv_EditWheelValues
 * @purpose To edit the wheel parameters
 * @param  hvevent     The event that initiated the edit
 */


void     Hv_EditWheelValues(Hv_Event hvevent) {

  Hv_Item           Item = hvevent->item;
  Hv_WheelData      *wheel;
  static Hv_Widget  dialog = NULL;
  Hv_Widget         rowcol, rc;
  static Hv_Widget  minval, maxval, cval;
  int               newval, answer;
  short             oldcval;


  if (Item == NULL)
    return;

  wheel = (Hv_WheelData *)(Item->data);

  if (wheel == NULL)
    return;

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Wheel Editor ", NULL);

    rowcol = Hv_DialogColumn(dialog, 6);
    rc = Hv_DialogTable(rowcol, 4, 6);

    minval = Hv_SimpleTextEdit(rc, "Minimum", NULL, 4);
    maxval = Hv_SimpleTextEdit(rc, "Maximum", NULL, 4);
    cval   = Hv_SimpleTextEdit(rc, "Current", NULL, 4);

    bgcolor  = Hv_SimpleColorLabel(rc, " background", (Hv_FunctionPtr)EditWheelColor);
    rc = Hv_StandardActionButtons(rowcol, 
				  20, 
				  Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
    
  }

/* the dialog has been created */

  newbgcolor  = Item->color;
  Hv_ChangeLabelColors(bgcolor,  -1, Item->color);

  Hv_SetIntText(minval, wheel->valmin);
  Hv_SetIntText(maxval, wheel->valmax);
  Hv_SetIntText(cval,   wheel->current);

  oldcval = wheel->current;

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    Item->color   = newbgcolor;

    wheel->valmin  = Hv_GetIntText(minval);
    wheel->valmax  = Hv_GetIntText(maxval);
    newval = Hv_GetIntText(cval);

    wheel->valmax = Hv_sMax((short)(wheel->valmin+1), (short)(wheel->valmax));
    newval = Hv_sMax(wheel->valmin, Hv_sMin(wheel->valmax, (short)newval));


  
    if (oldcval != newval) {
      oldcval = newval;
      Hv_UpdateWheel(Item, (short)newval);

      if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
	Hv_DrawViewHotRect(Item->view);
    }

    if (answer != Hv_APPLY)
      break;
  }

}

/**
 * Hv_GetWheelValues
 * @purpose To extract the wheel's "value" data
 * @param wheel    The wheel item
 * @param minval   Will contain the minimum value
 * @param maxval   Will contain the maximum value
 * @param val      Will contain the current value
 */

void   Hv_GetWheelValues(Hv_Item   wheel,
			 short    *minval,
			 short    *maxval,
			 short    *val) {
  Hv_WheelData    *data;

  if (wheel == NULL)
    return;

  data = (Hv_WheelData  *)(wheel->data);

  *val = data->current;
  *minval = data->valmin;
  *maxval = data->valmax;
}

/**
 * Hv_GetWheelCurrentValue
 * @purpose To extract the wheel's current value
 * @param wheel    The wheel item
 * @return  The current value of the wheel
 */

short      Hv_GetWheelCurrentValue(Hv_Item wheel) {

  Hv_WheelData    *data;

  if (wheel == NULL)
    return 0;

  data = (Hv_WheelData  *)(wheel->data);
  return data->current;
}


/**
 * Hv_SetWheelValues
 * @purpose To set the wheel's "value" data
 * @param wheel    The wheel item
 * @param vmin     The new minimum value
 * @param vmax     The new maximum value
 * @param val      The new current value
 * @param redraw   If true, the wheel is redrawn
 */

void Hv_SetWheelValues(Hv_Item  wheel,
		       short    vmin,
		       short    vmax,
		       short    val,
		       Boolean  redraw) {

  Hv_WheelData    *data;
  Hv_Item          vstr;
  char             bstr[30];

  if (wheel == NULL)
    return;

  val = Hv_sMin(vmax, Hv_sMax(vmin, val));

  data = (Hv_WheelData  *)(wheel->data);

  data->valmin = vmin;
  data->valmax = vmax;

  if (data->current != val) {
    vstr = data->valstr;

/* bug fix from aclark of new zealand */

    data->current = val;

    if (vstr != NULL) {
	Hv_NumberToString(bstr, (float)(val), 0, 0.0);
	Hv_SetItemText(vstr, bstr);
    }
  }
  if (redraw)
    Hv_RedrawItem(wheel);
}

/**
 * Hv_SetWheelCurrentValue
 * @purpose To set the wheel's current value
 * @param wheel    The wheel item
 * @param val      The new value of the wheel
 * @param redraw   If true, the wheel is redrawn
 */

void  Hv_SetWheelCurrentValue(Hv_Item  wheel,
			      short    val,
			      Boolean  redraw) {
  Hv_WheelData    *data;
  Hv_Item          vstr;
  char             bstr[30];

  if (wheel == NULL)
    return;

  data = (Hv_WheelData  *)(wheel->data);
  vstr = data->valstr;

  data->current = val;

  if (vstr != NULL) {
      Hv_NumberToString(bstr, (float)(val), 0, 0.0);
      Hv_SetItemText(vstr, bstr);
      Hv_SizeTextItem(vstr, vstr->area->left, vstr->area->top);
  }

  if (redraw)
    Hv_RedrawItem(wheel);
}


/*-------- Hv_DrawOneSymbol -------*/

static void   Hv_DrawOneSymbol(Hv_Point  *hvp,
			       short     sym)

{
  Hv_DrawSymbolsOnPoints(hvp,
			 1,
			 10,
			 Hv_yellow,
			 sym);
}

/* ------------- Hv_UpdateWheel ------------*/

static void Hv_UpdateWheel(Hv_Item     wheel,
			   short      newval)

{
  Hv_View                View = wheel->view;
  Hv_Region              reg1;
  Hv_Item                vstr;
  Hv_ColorScheme         *scheme;
  Hv_WheelData           *data = (Hv_WheelData *)(wheel->data);
  char                   bstr[30];

  vstr = data->valstr;

  newval = Hv_sMin(data->valmax, Hv_sMax(data->valmin, newval));

  if (data->current == newval)
      return;

  data->current = newval;

  if (vstr != NULL) {
      Hv_NumberToString(bstr, (float)(newval), 0, 0.0);
      Hv_ChangeWheelItemText(vstr, bstr);

      if (Hv_ViewIsVisible(View)) {
	  scheme = vstr->scheme;
	  reg1 = Hv_ClipView(View);
	  Hv_FillRect(vstr->area, scheme->fillcolor);
	  Hv_DrawTextItem(vstr, reg1);
	  Hv_DestroyRegion(reg1);
      }
  }


  UpdateOnly = True;
  Hv_DrawWheelItem(wheel, NULL);
  if (wheel->userdraw != NULL)
    wheel->userdraw(wheel, NULL);

  UpdateOnly = False;
}

/**
 * Hv_WheelCallback
 * @purpose   Callback for the wheel
 * @param  hvevent   Causal event.
 * @local
 */

static void Hv_WheelCallback(Hv_Event hvevent) {

  Hv_Item         Item = hvevent->item;
  Hv_View         View = hvevent->view;
  int             policy = 0;


/* if control or alt is also pressed, shift
   by a larger amount */

  if (hvevent->modifiers != 0)
    policy = Hv_SHIFTPOLICY;
  
/* create the drag data */

  thedd = Hv_MallocDragData(View,
			    Item,
			    &(hvevent->where),
			    NULL,
			    NULL,
			    policy,
			    -1,
			    NULL);

  Hv_AlternateEventHandler = Hv_WheelHandler;
}

/*-------- Hv_SetWheelRects --------*/

static void Hv_SetWheelRects(Hv_Item  Item,
			     Hv_Rect  *rect,
			     Hv_Rect  *increct,
			     Hv_Rect  *decrect)

{
  Hv_Rect         *area;
  Hv_WheelData    *wheel;
  short           al, at, ar, ab;
  short           l, t, w, h;

  wheel = (Hv_WheelData *)(Item->data);
  area = Item->area;

  Hv_GetRectLTRB(area, &al, &at, &ar, &ab);

  if (wheel->orientation == Hv_HORIZONTAL) {
    l = area->left+14;
    t = area->top+2;
    w = area->width-28;
    h = area->height-4;
    Hv_SetRect(decrect, al,    at, 14, area->height);
    Hv_SetRect(increct, (short)(ar-14), at, 14, area->height);
  }
  else {
    l = area->left+2;
    t = area->top+14;
    w = area->width-4;
    h = area->height-28;
    Hv_SetRect(increct, al, at,    area->width, 14);
    Hv_SetRect(decrect, al, (short)(ab-14), area->width, 14);
  }

  Hv_SetRect(rect, l, t, w, h);
  
}

/*------- Hv_CheckForDone ------*/

static Boolean Hv_CheckForDone(int   pause,
			       Hv_Rect *rect,
			       Hv_Point *pp) {

  Hv_XEvent    event;
  Boolean   done = False;


  
  if (Hv_CheckMaskEvent(Hv_BUTTONPRESSMASK | Hv_BUTTONRELEASEMASK | Hv_BUTTONMOTIONMASK,
			&event)) {

    Hv_SetPoint(pp,
		Hv_GetXEventX(&event),
		Hv_GetXEventY(&event));

    switch (event.type) {
    case Hv_motionNotify:
/*      done = !Hv_PointInRect(*pp, rect); */
      break;
      
    case Hv_buttonPress:
    case Hv_buttonRelease:
     done = True;
      break;
    }
  }

  if (pause > 0)
    Hv_Pause(pause, NULL);

  return done;
}


/* ------ EditWheelColor --- */

static void EditWheelColor(Hv_Widget w)

{
  if (w == bgcolor)
    Hv_ModifyColor(w, &newbgcolor, "Edit background color", False);
}


/*--------- Hv_ChangeWheelItemText -------------*/

static void  Hv_ChangeWheelItemText(Hv_Item  Item,
				    char    *str) {

    Hv_Region  rgn = NULL;
    short      x, y, extra;
    

    if ((Item == NULL) || (Item->type != Hv_TEXTITEM) || (Item->str == NULL))
	return;

    extra = Hv_STRINGEXTRA;

    rgn = Hv_GetViewRegion(Item->view);
    Hv_SetClippingRegion(rgn);
    Hv_FillRect(Item->area, Item->str->fillcol);
    
/* first get the desired x & y from the "old" area */
    
    Hv_CompoundStringXYFromArea(Item->str, &x, &y, Item->area, extra);


    Hv_SetItemText(Item, str);
    Hv_SizeTextItem(Item, x, y);
    
    Hv_DrawItem(Item, rgn);
    Hv_DestroyRegion(rgn);
}



/**
 * Hv_WheelHandler
 * @purpose   This is the alternate event handler called when changing the
 *            wheel is taking place. When dragging is terminated, the event
 *            handler is restored and the callback is called.
 * @param     event    Event pointer.
 * @local
 */

static void Hv_WheelHandler(Hv_XEvent *event) {

  static Boolean    firstEntry = True; /* used for initialization */
  Boolean           done = False;
  Hv_EventData      hvevent;
  static Hv_Rect    rect, increct, decrect;
  static Boolean    inincrease = False;
  static Boolean    indecrease = False;
  static Boolean    inwheel = False;
  short             dval = 0;
  static short      del = 1;
  Hv_WheelData      *wheel;
  short             currentval;
  static Hv_Region  vrgn = NULL;

  if (event == NULL)
    return;

  if ((thedd == NULL) || (thedd->view == NULL) || (thedd->item == NULL)) {
    fprintf(stderr, "Serious problem in Hv_WheelHandler\n");
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
    return;
  }

  wheel = (Hv_WheelData *)(thedd->item->data);

  if (firstEntry) {

    Hv_SetWheelRects(thedd->item, &rect, &increct, &decrect);

    inincrease = Hv_PointInRect(thedd->startpp, &increct);
    indecrease =  Hv_PointInRect(thedd->startpp, &decrect);
    inwheel = Hv_PointInRect(thedd->startpp, &rect);

    if (!inwheel && !inincrease && !indecrease) {
      Hv_Println("In NOTHING");
      Hv_FreeDragData(thedd);
      thedd = NULL;
      firstEntry = True;
      Hv_AlternateEventHandler = NULL;
      return;
    }

    Hv_SetPoint(&(thedd->finalpp),
		thedd->startpp.x,
		thedd->startpp.y);

    if (Hv_CheckBit(thedd->policy, Hv_SHIFTPOLICY)) {
      del = Hv_sMax(2, (short)((wheel->valmax - wheel->valmin)/20));
      Hv_dontSendBack = True;
    }

    vrgn = Hv_ClipView(thedd->view);
    firstEntry = False;
  }

  Hv_SetClippingRegion(vrgn);

/* handle the event */

  switch(event->type) {
  case Hv_motionNotify:
    
    if (inwheel) {

      Hv_SetPoint(&(thedd->startpp),
		  thedd->finalpp.x,
		  thedd->finalpp.y);
      
      
      Hv_SetPoint(&(thedd->finalpp),
		  Hv_GetXEventX(event),
		  Hv_GetXEventY(event));
      
      if (wheel->orientation == Hv_HORIZONTAL)
	dval = del*(thedd->finalpp.x - thedd->startpp.x);
      else
	dval = del*(thedd->startpp.y - thedd->finalpp.y);
    }

    break;
    
  case Hv_buttonRelease: case Hv_buttonPress:
    Hv_Println("BUTTON");

    dval = 0;

    Hv_SetPoint(&(thedd->finalpp),
		Hv_GetXEventX(event),
		Hv_GetXEventY(event));

    inincrease = inincrease && Hv_PointInRect(thedd->finalpp, &increct);
    indecrease = indecrease && Hv_PointInRect(thedd->finalpp, &decrect);

    if (inincrease) {
      if (wheel->current < wheel->valmax)
	dval = del;
    }
    
    else if (indecrease) {
      if (wheel->current > wheel->valmin)
	dval = -del;
    }

    done = True;
    break;

  default:
    Hv_Println("DEFAULT");

  }

/* now update the wheel */


/* done if out of bounds */

    currentval = Hv_sMax(wheel->valmin, 
			 Hv_sMin(wheel->valmax, 
				 (short)(wheel->current + dval)));
    
    done = done || ((currentval >= wheel->valmax) ||
		    (currentval <= wheel->valmin));
    
    if (dval != 0)
      Hv_UpdateWheel(thedd->item, currentval);

  if (done) {
    Hv_DestroyRegion(vrgn);

/* automaticaly redraw hotrect? */

    if (Hv_CheckBit(thedd->item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
      Hv_DrawViewHotRect(thedd->view);

/*
 * dirty trick: the user's callback was placed in in the checkenlarge field!
 */

    hvevent.item = thedd->item;
    hvevent.view = thedd->view;
    if (thedd->item->checkenlarge != NULL)
      thedd->item->checkenlarge(&hvevent);

    Hv_FreeDragData(thedd);
    thedd = NULL;
    firstEntry = True;
    inincrease = False;
    inincrease = False;
    indecrease = False;
    inwheel    = False;
    del = 1;
    Hv_AlternateEventHandler = NULL;
  }

}


#undef  Hv_MINWHEELLENGTH
#undef  Hv_WHEELWIDTH









