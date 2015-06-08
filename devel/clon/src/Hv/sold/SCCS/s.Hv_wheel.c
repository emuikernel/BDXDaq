h03251
s 00000/00000/00000
d R 1.2 02/08/25 23:21:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_wheel.c
e
s 00701/00000/00000
d D 1.1 02/08/25 23:21:25 boiarino 1 0
c date and time created 02/08/25 23:21:25 by boiarino
e
u
U
f e 0
t
T
I 1
/*
-----------------------------------------------------------------------
File:     Hv_wheel.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with wheel items */

#include "Hv.h"	  /* contains all necessary include files */


#define  Hv_MINWHEELLENGTH     78
#define  Hv_WHEELWIDTH      16

/*------ local prototypes -------*/

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

static void EditWheelColor(Widget  w);

static Boolean UpdateOnly = False;

static Hv_Widget  bgcolor;
static short      newbgcolor;

/* ------------- Hv_DrawWheelItem ------------*/

void Hv_DrawWheelItem(Hv_Item    Item,
		      Hv_Region   region)

{
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
      Hv_SetPoint(&hvp, r+6, (t+b)/2);
      Hv_DrawOneSymbol(&hvp, Hv_RIGHTARROWSYMBOL);
      Hv_SetPoint(&hvp, l-6, (t+b)/2);
      Hv_DrawOneSymbol(&hvp, Hv_LEFTARROWSYMBOL);
    }
    else {
      Hv_SetPoint(&hvp, (l+r)/2, t-6);
      Hv_DrawOneSymbol(&hvp, Hv_UPARROWSYMBOL);
      Hv_SetPoint(&hvp, (l+r)/2, b+6);
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
      Hv_SetRect(&rect, l+wc*i, t, wc, h);
    else
      Hv_SetRect(&rect, l, t+wc*i, w, wc);
    Hv_FillRect(&rect, Hv_gray15+i);
  }
  
/*  Hv_SetRect(&area, l, t, w, h); */

/* now the lines */

  for (i = 0; i < 9; i++)
    if (wheel->orientation == Hv_HORIZONTAL)
      marks[i] = l + y0[i] + (gaps[i]*del)/gaps[0];
    else
      marks[i] = t + gaps[0] - 1 + y0[i] - (gaps[i]*del)/gaps[0];
  
  if (wheel->orientation == Hv_HORIZONTAL)
    for (i = 0; i < 9; i++) {
      Hv_DrawLine(marks[i],   t-1, marks[i],   b, Hv_gray16 + (i%3));
      Hv_DrawLine(marks[i]+1, t-1, marks[i]+1, b, Hv_black);
    }
  else
    for (i = 0; i < 9; i++) {
      Hv_DrawLine(l-1, marks[i],   r, marks[i],   Hv_gray16 + (i%3));
      Hv_DrawLine(l-1, marks[i]+1, r, marks[i]+1, Hv_black);
    }
  
  
  if (region != NULL)
    Hv_SetClippingRegion(region);
  XFlush(Hv_display);
}

/*-------- Hv_WheelInitialize --------*/

void Hv_WheelInitialize(Hv_Item      Item,
			Hv_AttributeArray attributes)

{
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

  if (inbox)
    data->titlestr->scheme->fillcolor = boxfill;
  }
  else
    data->titlestr = NULL;

  Hv_NumberToString(bstr, (float)(data->current), 0, 0.0);

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

  if (inbox)
    data->valstr->scheme->fillcolor = boxfill;

}

/*---------- Hv_EditWheelValues -------*/

void     Hv_EditWheelValues(Hv_Event hvevent)

{
  Hv_Item           Item = hvevent->item;
  Hv_WheelData      *wheel;
  static Hv_Widget  dialog = NULL;
  Hv_Widget         rowcol, rc;
  static Hv_Widget  minval, maxval, cval;
  int               answer;
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
    rc = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
    
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
    wheel->current = Hv_GetIntText(cval);

    wheel->valmax = Hv_sMax(wheel->valmin+1, wheel->valmax);
    wheel->current = Hv_sMax(wheel->valmin, Hv_sMin(wheel->valmax, wheel->current));


    Hv_DrawItem(Item, NULL);

    if (oldcval != wheel->current) {
      oldcval = wheel->current;
      Hv_UpdateWheel(Item, wheel->current);

      if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
	Hv_DrawViewHotRect(Item->view);
    }

    if (answer != Hv_APPLY)
      break;
  }

}

/*---------- Hv_GetWheelValues -------*/

void   Hv_GetWheelValues(Hv_Item   wheel,
			 short    *minval,
			 short    *maxval,
			 short    *val)
{
  Hv_WheelData    *data;

  if (wheel == NULL)
    return;

  data = (Hv_WheelData  *)(wheel->data);

  *val = data->current;
  *minval = data->valmin;
  *maxval = data->valmax;
}


/*--------- Hv_GetWheelCurrentValue --------*/

short      Hv_GetWheelCurrentValue(Hv_Item wheel)

{
  Hv_WheelData    *data;

  if (wheel == NULL)
    return 0;

  data = (Hv_WheelData  *)(wheel->data);
  return data->current;
}

/*-------  Hv_SetWheelValues ---------*/

void Hv_SetWheelValues(Hv_Item  wheel,
		       short   vmin,
		       short   vmax,
		       short   val,
		       Boolean  redraw)
{
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
    Hv_NumberToString(bstr, (float)(val), 0, 0.0);
    Hv_SetItemText(vstr, bstr);
/*    Hv_SizeTextItem(vstr, vstr->area->left, vstr->area->top); */
  }
  if (redraw)
    Hv_RedrawItem(wheel);
}

/*-------- Hv_SetWheelCurrentValue --------*/

void  Hv_SetWheelCurrentValue(Hv_Item  wheel,
			      short   val,
			      Boolean  redraw)

{
  Hv_WheelData    *data;
  Hv_Item          vstr;
  char             bstr[30];

  if (wheel == NULL)
    return;

  data = (Hv_WheelData  *)(wheel->data);
  vstr = data->valstr;

  data->current = val;
  Hv_NumberToString(bstr, (float)(val), 0, 0.0);
  Hv_SetItemText(vstr, bstr);
  Hv_SizeTextItem(vstr, vstr->area->left, vstr->area->top);

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
  Hv_NumberToString(bstr, (float)(newval), 0, 0.0);
  Hv_ChangeTextItemText(vstr, bstr, True);

  if (Hv_ViewIsVisible(View)) {
    scheme = vstr->scheme;
    reg1 = Hv_ClipView(View);
    Hv_FillRect(vstr->area, scheme->fillcolor);
    Hv_DrawTextItem(vstr, reg1);
    Hv_DestroyRegion(reg1);
  }


  UpdateOnly = True;
  Hv_DrawWheelItem(wheel, NULL);
  UpdateOnly = False;
}

/*--------- Hv_WheelCallback --------*/

static void Hv_WheelCallback(Hv_Event hvevent)

{
  Hv_Item         Item = hvevent->item;
  Hv_View         View = hvevent->view;
  Hv_WheelData    *wheel;
  Hv_Rect         rect, increct, decrect;
  Boolean         inincrease = False;
  Boolean         indecrease = False;
  Boolean         inwheel = True;
  short           dval;
  Boolean         mods;
  short           del = 1;
  Boolean         done = False;
  Hv_Point        oldpp, newpp;

  wheel = (Hv_WheelData *)(Item->data);

/* if control or alt is also pressed, shift
   by a larger amount */

  mods = (hvevent->modifiers != 0);
  
  if (mods) {
    del = Hv_sMax(2, (wheel->valmax - wheel->valmin)/20);
    Hv_dontSendBack = True;
  }
  
  newpp = hvevent->where;

  Hv_SetWheelRects(Item, &rect, &increct, &decrect);
  if (Hv_PointInRect(newpp, &rect))
    inwheel = True;
  else if (Hv_PointInRect(newpp, &increct))
    inincrease = True;
  else if (Hv_PointInRect(newpp, &decrect))
    indecrease = True;
  dval = 0;

  if (!inwheel && !inincrease && !indecrease)
    return;

  while (!done) {
    if (inincrease) {
      done = Hv_CheckForDone(1000, &increct, &newpp);
      
      if (wheel->current < wheel->valmax)
	dval = del;
    }
    else if (indecrease) {
      done = Hv_CheckForDone(1000, &decrect, &newpp);

      if (wheel->current > wheel->valmin)
	dval = -del;
    }
    
    else if (inwheel) {
      oldpp = newpp;
      done = Hv_CheckForDone(0, &rect, &newpp);
      if (wheel->orientation == Hv_HORIZONTAL)
	dval = del*(newpp.x - oldpp.x);
      else
	dval = del*(oldpp.y - newpp.y);
    }

/* done if out of bounds */

/*    done = (done || ((wheel->current + dval) >= wheel->valmax) ||
	    ((wheel->current + dval) <= wheel->valmin));
*/

    if (dval != 0)
      Hv_UpdateWheel(Item, wheel->current + dval);
  }


/* automaticaly redraw hotrect? */

  if (Hv_CheckBit(Item->drawcontrol, Hv_REDRAWHOTRECTWHENCHANGED))
    Hv_DrawViewHotRect(View);

/* now the trick -- the user's callback was placed in in the checkenlarge field! */

  if (Item->checkenlarge != NULL)
    Item->checkenlarge(hvevent);
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
    Hv_SetRect(increct, ar-14, at, 14, area->height);
  }
  else {
    l = area->left+2;
    t = area->top+14;
    w = area->width-4;
    h = area->height-28;
    Hv_SetRect(increct, al, at,    area->width, 14);
    Hv_SetRect(decrect, al, ab-14, area->width, 14);
  }

  Hv_SetRect(rect, l, t, w, h);
  
}

/*------- Hv_CheckForDone ------*/

static Boolean Hv_CheckForDone(int   pause,
			       Hv_Rect *rect,
			       Hv_Point *pp)

{
  XEvent    event;
  Boolean   done = False;

  if (pause > 0)
    Hv_Pause(pause, NULL);
  
  if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonMotionMask, &event)) {
    Hv_SetPoint(pp, event.xbutton.x, event.xbutton.y);

    switch (event.type) {
    case MotionNotify:
/*      done = !Hv_PointInRect(*pp, rect); */
      break;
      
    case ButtonRelease:
      done = True;
      break;
    }
  }
  
  return done;
}


/* ------ EditWheelColor --- */

static void EditWheelColor(Widget w)

{
  if (w == bgcolor)
    Hv_ModifyColor(w, &newbgcolor, "Edit background color", False);
}

#undef  Hv_MINWHEELLENGTH
#undef  Hv_WHEELWIDTH









E 1
