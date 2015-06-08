/**
 * <EM>Deals with world coordinate based rectangles. </EM>
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
#include "Hv_xyplot.h"
#include "Hv_maps.h"
#include "Hv_drag.h"

/*------ local prototypes ------*/

static void       Hv_SelectCallback(Hv_DragData);

static void       Hv_EditWorldRectColor(Hv_Widget);

static void       Hv_EditWorldRectPattern(Hv_Widget);

static void       Hv_SetRectOvalData(Hv_Item  Item);

static Hv_Item    Hv_NewRectOvalItem(Hv_View    View,
				     Hv_Item    parent,
				     Hv_Rect   *therect,
				     short      type);

/*------ variables global for this file --------*/

static   short      newpattern;

static   short      newlinecolor,   newrectcolor;
static   Hv_Widget     linecolorlabel, rectcolorlabel, patternlabel;
Boolean             Hv_updateAfterEnlarge = True;


/**
 * Hv_EditWorldRectItem
 * @purpose Edit the visual parameters of the rectangle
 * @note This same routine is also used by world ovals.
 * @param   hvevent   The initiating event, probably a double click on the rectangle.
 */

void   Hv_EditWorldRectItem(Hv_Event hvevent) {

/* item edit for Hv_WORLDRECTITEMs AND Hv_WORLDOVALITEMS  */

  Hv_Item              Item = hvevent->item;
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        thickness;
  static Hv_Widget        frame;
  Hv_Widget               dummy, rowcol, rc;
  Hv_WorldRectData     *wrect;
  int                  answer;

  static Hv_Widget        stylebutton;
  static int              num_sbuttons = -1;
  static Hv_WidgetList    sbuttons;


  if (Item == NULL)
    return;

  wrect = (Hv_WorldRectData *)(Item->data);

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " World Rectangle/Oval Editor ", NULL);
    rowcol = Hv_DialogColumn(dialog, 8);
    rc = Hv_TightDialogRow(rowcol, 8);

    linecolorlabel = Hv_SimpleColorLabel(rc, " line ", Hv_EditWorldRectColor);
    rectcolorlabel = Hv_SimpleColorLabel(rc, " fill ", Hv_EditWorldRectColor);

    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_TightDialogRow(rowcol, 8);

    patternlabel   = Hv_SimplePatternLabel(rc, "pattern",
					   Hv_EditWorldRectPattern);

    thickness = Hv_SimpleTextEdit(rc, "Line Width", NULL, 4);

    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_TightDialogRow(rowcol, 8);

/* 3D frame? */

    frame = Hv_SimpleToggleButton(rc, "3D frame");

    stylebutton = Hv_CreateLineStylePopup(rc, Hv_fixed2);    /* line style menu */

    dummy = Hv_StandardActionButtons(rowcol, 22, Hv_ALLACTIONS);
    Hv_GetOptionMenuButtons(stylebutton,  &sbuttons, &num_sbuttons);
  }

/* the dialog has been created */

/* 3d frame is NOT selectable for ovals */

  Hv_SetSensitivity(frame, Item->type != Hv_WORLDOVALITEM);

  Hv_SetDeleteItem(dialog, Item);

  Hv_hotStyle  = wrect->style;

  Hv_SetOptionMenu(stylebutton,  sbuttons, (int)Hv_hotStyle);

  newpattern = Item->pattern;
  newlinecolor = Item->color;
  newrectcolor = wrect->fillcolor;

  Hv_ChangeLabelPattern(patternlabel, Item->pattern);
  Hv_ChangeLabelColors(linecolorlabel, -1, Item->color);
  Hv_ChangeLabelColors(rectcolorlabel, -1, wrect->fillcolor);


  Hv_SetToggleButton(frame, wrect->frame);

  Hv_SetIntText(thickness, wrect->thickness);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    wrect->style = Hv_hotStyle;

    Item->pattern = newpattern;
    Item->color  = newlinecolor;
    wrect->fillcolor = newrectcolor;
    wrect->frame = Hv_GetToggleButton(frame);
    wrect->thickness = Hv_sMax(1, Hv_sMin(6, (short)Hv_GetIntText(thickness)));

    Hv_StandardRedoItem(Item);

    if (answer != Hv_APPLY)
      break;
  }
}


/* ------- Hv_WorldOvalButtonCallback -------*/

void Hv_WorldOvalButtonCallback(Hv_Event hvevent)

{
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  static Hv_Cursor        tempcursor;
  Hv_Point             pp;
  Hv_Rect              limitrect;
  Hv_Item              parent = NULL;

  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent,
			  Hv_WorldOvalButtonCallback,
			  &activeView,
			  &activeItem,
			  &tempcursor);
    return;
  }

  if (View != activeView) {
    Hv_Warning("World Oval selector active on another view!");
    return;
  }

  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

/* see if click occured inside hotrect */

  Hv_SetPoint(&pp, hvevent->where.x, hvevent->where.y); 
  if (Hv_PointInRect(pp, View->hotrect)) {

/* for certain items, we assume the desire is to contain the
   new item */

    parent = Hv_PointInPlotItem(View, pp);

    if (parent == NULL)
      Hv_CopyRect(&limitrect, View->hotrect);
    else
      Hv_CopyRect(&limitrect, parent->area);

    Hv_LaunchSelectOval(View,
			NULL,
			&limitrect,
			&pp,
			Hv_NOSELECTPOLICY,
			Hv_SelectCallback);
  }

}

/* ------- Hv_WorldRectButtonCallback -------*/

void Hv_WorldRectButtonCallback(Hv_Event hvevent)

{
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  static Hv_Cursor        tempcursor;
  Hv_Point             pp;
  Hv_Rect              limitrect;
  Hv_Item              parent = NULL;

  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent,
			  Hv_WorldRectButtonCallback,
			  &activeView,
			  &activeItem,
			  &tempcursor);
    return;
  }

  if (View != activeView) {
    Hv_Warning("World Rect selector active on another view!");
    return;
  }

  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

/* see if click occured inside hotrect */

  Hv_SetPoint(&pp, hvevent->where.x, hvevent->where.y); 
  if (Hv_PointInRect(pp, View->hotrect)) {

/* for certain items, we assume the desire is to contain the
   new item */

    parent = Hv_PointInPlotItem(View, pp);

    if (parent == NULL)
      Hv_CopyRect(&limitrect, View->hotrect);
    else
      Hv_CopyRect(&limitrect, parent->area);

	Hv_LaunchSelectRect(View,
		NULL,
		&limitrect,
		&pp,
		Hv_NOSELECTPOLICY,
		1.0,
		Hv_SelectCallback);

  }
  
}

/*--------- Hv_NewRectOvalItem -------*/

static Hv_Item Hv_NewRectOvalItem(Hv_View  View,
				  Hv_Item  parent,
				  Hv_Rect *therect,
				  short    type)

{
  float  xmin, xmax, ymin, ymax;

  Hv_LocalToWorld(View, &xmin, &ymax, therect->left,  therect->top);
  Hv_LocalToWorld(View, &xmax, &ymin, therect->right, therect->bottom);

   return Hv_VaCreateItem(View,
			  Hv_TYPE,         type,
			  Hv_TAG,          Hv_DRAWINGTOOLTAG,
			  Hv_XMIN,         xmin,
			  Hv_XMAX,         xmax,
			  Hv_PARENT,       parent,
			  Hv_YMIN,         ymin,
			  Hv_YMAX,         ymax,
			  Hv_NUMROWS,      9,
			  Hv_NUMCOLUMNS,   9,
			  Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			  Hv_DRAWCONTROL,  Hv_ENLARGEABLE+ Hv_ZOOMABLE + Hv_FANCYDRAG,
			  Hv_CHECKENLARGE, Hv_CheckWorldRectEnlarge,
			  NULL);
}


/*-------- Hv_SetRectOvalData ------*/

static void Hv_SetRectOvalData(Hv_Item  Item)

{
  Hv_WorldOvalData    *woval;
  Hv_View             View = Item->view;

  woval = (Hv_WorldOvalData *)(Item->data);
  woval->thickness = View->hotlinewidth;
  woval->fillcolor = View->hotfillcolor;
  woval->style = View->hotlinestyle;
  Item->pattern = View->hotpattern;
  Item->color = View->hotbordercolor;
  Hv_usedNewDrawTools = False;
}

/*------- Hv_DrawWorldOvalButton -------*/

void Hv_DrawWorldOvalButton(Hv_Item    Item,
			    Hv_Region   region)

/* draws the icon for world line tool */
     
{
  Hv_Rect          area;
  short            xc, yc, rad;

  Hv_ButtonDrawingArea(Item, &area);
  Hv_ShrinkRect(&area, 2, 2);

  xc = (area.left + area.right)/2;
  yc = (area.top  + area.bottom)/2;

  rad = (area.bottom - area.top)/2;
  Hv_Simple3DCircle(xc, yc, rad, Hv_cornFlowerBlue, True);
}

/*------- Hv_DrawWorldRectButton -------*/

void Hv_DrawWorldRectButton(Hv_Item    Item,
			    Hv_Region   region)

/* draws the icon for world rect tool */
     
{
  Hv_Rect          area;

  Hv_ButtonDrawingArea(Item, &area);
  Hv_ShrinkRect(&area, 3, 3);
  Hv_Simple3DRect(&area, True, Hv_cornFlowerBlue);
}


/* -------- Hv_DrawWorldRectItem --------------- */

void Hv_DrawWorldRectItem(Hv_Item    Item,
			  Hv_Region   region)

/*draws the specified Item Hv_WORLDRECTITEM*/

{
  Hv_View            View = Item->view;
  Hv_WorldRectData   *wrect;
  Hv_ColorScheme     *scheme;
  short              extra = 0;
  Hv_Rect            area;
  Hv_FRect           warea;

  wrect = (Hv_WorldRectData *)(Item->data);

  Hv_WorldRectToLocalRect(View, Item->area, Item->worldarea);

  Hv_CopyRect(&area, Item->area);


  if (Hv_extraFastPlease) {
      if (Item->type == Hv_WORLDOVALITEM)
	  Hv_FrameOval(&area, Hv_yellow);
      else
	  Hv_FrameRect(&area, Hv_yellow);
      return;
  }

/* ovals never have 3d frame */

  if ((wrect->frame) && (Item->type != Hv_WORLDOVALITEM)) {
    scheme = Item->scheme;
    extra = scheme->framewidth;
    Hv_Draw3DRect(&area, scheme);
    Hv_ShrinkRect(&area, extra, extra);
  }

  Hv_LocalRectToWorldRect(View, &area, &warea);


  if ((wrect->fillcolor >= 0) && (Item->pattern != Hv_HOLLOWPAT)) {
    Hv_SetPattern(Item->pattern, wrect->fillcolor);
    
    if (Item->type == Hv_WORLDOVALITEM)
      Hv_FillOval(&area, False, wrect->fillcolor, Hv_black);
    else
      Hv_FillRect(&area, wrect->fillcolor);
    
    Hv_SetPattern(Hv_FILLSOLIDPAT, Hv_black);
  }


  if ((Item->type != Hv_WORLDOVALITEM) && (wrect->grid))
    Hv_DrawWorldGrid(View, Item->worldarea, wrect->nrows, wrect->ncols, Item->color, extra);

  if (!Hv_extraFastPlease)
    Hv_SetLineStyle(wrect->thickness, wrect->style);

  if (Item->type == Hv_WORLDOVALITEM)
    Hv_FrameOval(&area, Item->color);
  else
    Hv_FrameRect(&area, Item->color);

  Hv_SetLineStyle(0, Hv_SOLIDLINE);

}

/*-------- Hv_WorldRectToLocalRect ------------ */

void   Hv_WorldRectToLocalRect(Hv_View   View,
			       Hv_Rect   *area,
			       Hv_FRect  *wr)

/* Given a View (which has a world & local), this adjusts the rectangle passed in area
   to correspond to the Frect passed in wr*/

{
  Hv_WorldToLocal(View, wr->xmin, wr->ymin, &(area->left),  &(area->bottom));
  Hv_WorldToLocal(View, wr->xmax, wr->ymax, &(area->right), &(area->top));
  Hv_FixRectWH(area);
}

/*-------- Hv_LocalRectToWorldRect ------------ */

void   Hv_LocalRectToWorldRect(Hv_View   View,
			       Hv_Rect   *area,
			       Hv_FRect  *wr)


/* Given a View (which has a world & local), this adjusts
   the F rectangle passed in wr to correspond to the world
   points passed in area */

{
  Hv_LocalToWorld(View, &(wr->xmin), &(wr->ymin), area->left,  area->bottom);
  Hv_LocalToWorld(View, &(wr->xmax), &(wr->ymax), area->right, area->top);
  Hv_FixFRectWH(wr);
}



/*------- Hv_CheckWorldRectEnlarge ---------*/

void Hv_CheckWorldRectEnlarge(Hv_Item    Item,
			      Hv_Point   StartPP,
			      Boolean    ShowCorner,
			      Boolean   *enlarged)


/********************************
  Used for BOTH world rect items and 
  world oval items
********************************/

{
  Hv_View      View = Item->view;
  Hv_Region    udr1 = NULL;
  Hv_Region    udr2 = NULL;
  Hv_Rect      growlim;
  short        nearestcorner;
  short        slop=8;
  Hv_Rect      checkrect;
  Hv_Rect      *area;

  short        l, t, r, b, xc, yc;
  short        slop2;
  short        dl, dt;

  slop2 = slop/2;

  area = Item->area;
  nearestcorner = Hv_WhichPointOnRect(StartPP.x, StartPP.y, area);

  Hv_GetRectLTRB(area, &l, &t, &r, &b);
  Hv_GetRectCenter(area, &xc, &yc);

  *enlarged = False;

  if ((Item->parent != NULL) && (Hv_CheckBit(Item->drawcontrol, Hv_CONFINETOPARENT))) {
    Hv_CopyRect(&growlim, Item->parent->area);
    dl = StartPP.x - Item->area->left;
    dt = StartPP.y - Item->area->top;
    Hv_SetRect(&growlim,
		(short)(growlim.left+dl),
		(short)(growlim.top+dt),
		(short)(growlim.width-Item->area->width),
		(short)(growlim.height-Item->area->height));
    
  }
  else
    Hv_CopyRect(&growlim, View->hotrect);

  switch (nearestcorner) {
    

  case Hv_UPPERLEFT:
    Hv_SetRect(&checkrect, l, t, slop, slop);
    growlim.right  = r - slop;
    growlim.bottom = b - slop;
    break;

  case Hv_UPPERRIGHT:
    Hv_SetRect(&checkrect, (short)(r-slop), t, slop, slop);
    growlim.left   = l + slop;
    growlim.bottom = b - slop;
    break;

  case Hv_LOWERRIGHT:
    Hv_SetRect(&checkrect, (short)(r-slop), (short)(b-slop), slop, slop);
    growlim.left   = l + slop;
    growlim.top    = t + slop;
    break;

  case Hv_LOWERLEFT:
    Hv_SetRect(&checkrect, l, (short)(b-slop), slop, slop);
    growlim.top    = t + slop;
    growlim.right  = r - slop;
    break;

  case Hv_MIDDLELEFT:
    Hv_SetRect(&checkrect, l, (short)(yc-slop2), slop, slop);
    growlim.right  = r-slop;
    break;

  case Hv_MIDDLETOP:
    Hv_SetRect(&checkrect, (short)(xc-slop2), t, slop, slop);
    growlim.bottom = b-slop;
    break;

  case Hv_MIDDLERIGHT:
    Hv_SetRect(&checkrect, (short)(r-slop), (short)(yc-slop2), slop, slop);
    growlim.left  = l+slop;
    break;

  case Hv_MIDDLEBOTTOM:
    Hv_SetRect(&checkrect, (short)(xc-slop2), (short)(b-slop), slop, slop);
    growlim.top  = t + slop;
    break;

  default:
    return;
  }

  Hv_FixRectWH(&growlim);

  if (!Hv_PointInRect(StartPP, &checkrect))
    return;

  Hv_FillRect(&checkrect, Hv_peachPuff);
  Hv_FrameRect(&checkrect, Hv_brown);

  if (Hv_updateAfterEnlarge) {
    udr1 = Hv_CopyItemRegion(Item, True);
    Hv_ShrinkRegion(udr1, -2, -2);
  }

  udr2 = Hv_ClipHotRect(View, 0);

  /* here we must check if this is really an oval */

  if (Item->type == Hv_WORLDOVALITEM)
    Hv_EnlargeOval(Item->area, growlim, StartPP, ShowCorner);
  else
    Hv_EnlargeRect(Item->area, growlim, StartPP, ShowCorner);

  Hv_DestroyRegion(udr2);
  Hv_FixItemWorldArea(Item, False);

  if (Hv_updateAfterEnlarge) {
    Hv_ItemChangedRedraw(Item, udr1);
    Hv_DestroyRegion(udr1); 
  }

  Hv_UpdateConnections(View, Item, True);
  *enlarged = True;
}


/* -------  Hv_WorldRectInitialize --------*/

void Hv_WorldRectInitialize(Hv_Item      Item,
			    Hv_AttributeArray attributes)

/**********************************
  note: this is used by world OVALS too!
  (they have the same data structure)
*****************************************/

{
  Hv_WorldRectData     *wrect;
  Hv_View              View = Item->view;

  Item->standarddraw = Hv_DrawWorldRectItem;

  Item->worldarea = Hv_NewFRect();
  Hv_DefaultSchemeIn(&(Item->scheme));
  wrect = (Hv_WorldRectData *)Hv_Malloc(sizeof(Hv_WorldRectData));
  Item->data = (void *)wrect;
  wrect->frame = False;
  wrect->grid = False;
  wrect->thickness = 1;
  wrect->nrows = attributes[Hv_NUMROWS].s;
  wrect->ncols = attributes[Hv_NUMCOLUMNS].s;
  wrect->fillcolor = attributes[Hv_FILLCOLOR].s;
  wrect->style = Hv_SOLIDLINE;

  Hv_SetFRect(Item->worldarea,
	  (float)(attributes[Hv_XMIN].f),
	  (float)(attributes[Hv_XMAX].f),
	  (float)(attributes[Hv_YMIN].f),
	  (float)(attributes[Hv_YMAX].f));
  
  if (Item->doubleclick == NULL)
    Item->doubleclick = Hv_EditWorldRectItem;

/* set the area to correspond to the world rect */ 

  Hv_WorldRectToLocalRect(View, Item->area, Item->worldarea);

}

/* ------ Hv_EditWorldRectColor --- */

static void Hv_EditWorldRectColor (Hv_Widget w)

{
  if (w == linecolorlabel)
    Hv_ModifyColor(w, &newlinecolor, "Edit Line Color", False);
  else if (w == rectcolorlabel)
    Hv_ModifyColor(w, &newrectcolor, "Edit Fill Color", True);
}

/* ------ Hv_EditWorldRectPattern --- */

static void Hv_EditWorldRectPattern(Hv_Widget w)

{
  if (w == patternlabel)
    Hv_ModifyPattern(w, &newpattern, "Fill Pattern");

}


/**
 * Hv_SelectCallback
 * @purpose  Callback when rubberbanding is complete.
 * @param  dd    The drag data pointer.
 * @local
 */

static void       Hv_SelectCallback(Hv_DragData dd) {

  Hv_Item    parent = NULL;
  Hv_Item    item;
  
  
  if (dd == NULL)
    return;
  
  if ((dd->finalrect.width < 1) || (dd->finalrect.height < 1))
    return;

  parent = Hv_PointInPlotItem(dd->view, dd->startpp);

/* was I selecting an oval or a rect? */

  if (Hv_CheckBit(dd->policy, Hv_OVALMETHOD))
    item = Hv_NewRectOvalItem(dd->view, parent, &(dd->finalrect), Hv_WORLDOVALITEM);
  else
    item = Hv_NewRectOvalItem(dd->view, parent, &(dd->finalrect), Hv_WORLDRECTITEM);

  if (Hv_usedNewDrawTools) 
    Hv_SetRectOvalData(item);
 
/* see if the user wants to be notified */

  if (Hv_CreateDrawingItemCB != NULL)
    Hv_CreateDrawingItemCB(item);

  Hv_DrawItem(item, NULL);

}

