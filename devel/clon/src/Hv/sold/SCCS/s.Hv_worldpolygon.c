h59971
s 00000/00000/00000
d R 1.2 02/08/25 23:21:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_worldpolygon.c
e
s 02126/00000/00000
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
------------------------------------------------------------------
File:     Hv_worldpolygon.c
==================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================
*/

/* This file contains functions dealing with world polygons */

#include "Hv.h"	  /* contains all necessary include files */
#include "Hv_xyplot.h"
#include "Hv_maps.h"

#define  LEGPOINTS  20

/*------ local prototypes ------*/

static Hv_Item Hv_CopyPolygonItem(Hv_View View,
				  Hv_Item Item);

static void CopyPolygonsViewToView(Hv_View dv,
				   Hv_View sv);

static void Hv_PolygonItemSingleClick(Hv_Event hvevent);

static float Hv_InteriorAngle(Hv_Item Item,
			      int     index);

static void Hv_GetRRP(Hv_Item Item);

static float Hv_FarthestPoint(Hv_Item Item);

static void  Hv_FixPolygonMapRegion(Hv_Item Item);

static void Hv_Special4ptEnlarge(Hv_Rect  LimitRect,
				 Hv_Point StartPP,
				 int      vertex,
				 Hv_Item  Item);

static Boolean Hv_PolyIsRotatable(Hv_Item);

static void  Hv_GetPolygonArrowPolygon(Hv_Item   Item,
				       Hv_Point  *poly,
				       short     style);

static void       Hv_EditWorldPolygonColor(Widget);

static void       Hv_EditWorldPolygonPattern(Widget);

static  void Hv_DrawMapPolygon(Hv_Item   Item,
			       Hv_Region region);

/*------ variables global for this file --------*/

static   Boolean    yesdraw = True;

static   short      newpattern;
static   Widget     patternlabel;

static   short      newlinecolor,   newfillcolor, newsymbcolor;
static   Widget     linecolorlabel, fillcolorlabel, symbcolorlabel;


/*-------- Hv_EditWorldPolygonItem -------*/

void   Hv_EditWorldPolygonItem(Hv_Event hvevent)

/* item edit for Hv_WORLDPOLYGONITEMs  */

{
  Hv_Item              Item = hvevent->item;
  static Widget        dialog = NULL;
  static Widget        thickness, closed, showlines, locked;
  static Widget        symbolbutton, stylebutton, arrowbutton;

  static Widget        baselat;
  static Widget        baselong;

  Widget               dummy, rowcol, rc;
  Hv_WorldPolygonData  *wp;
  int                  answer;
  Boolean              oldclosed;
  Hv_Map               map;


/* these are used in "menu history" */

  static int         num_buttons = -1;
  static WidgetList  buttons;

  static int         num_abuttons = -1;
  static WidgetList  abuttons;

  static int         num_sbuttons = -1;
  static WidgetList  sbuttons;

  Boolean            mapview;

  if (Item == NULL)
    return;


  mapview = (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG));

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Polygon Editor ", NULL);
    rowcol = Hv_DialogColumn(dialog, 6);
    rc = Hv_DialogTable(rowcol, 2, 5);
    linecolorlabel = Hv_SimpleColorLabel(rc, "   line ", Hv_EditWorldPolygonColor);
    fillcolorlabel = Hv_SimpleColorLabel(rc, "   fill ", Hv_EditWorldPolygonColor);
    symbcolorlabel = Hv_SimpleColorLabel(rc, " symbol ", Hv_EditWorldPolygonColor);

    patternlabel   = Hv_SimplePatternLabel(rc, "pattern",
					   Hv_EditWorldPolygonPattern);

    dummy = Hv_SimpleDialogSeparator(rowcol);

    rc = Hv_DialogTable(rowcol, 3, 8);
    thickness = Hv_SimpleTextEdit(rc, " Line Width ", NULL, 4);
    baselat  = Hv_SimpleTextEdit(rc, " Basepoint Lat (deg) ", NULL, 4);
    baselong = Hv_SimpleTextEdit(rc, " Basepoint Long (deg) ", NULL, 4);

    dummy = Hv_SimpleDialogSeparator(rowcol);

/* closed ?  showlines? show points (symbols)?*/

    closed     = Hv_SimpleToggleButton(rowcol, "Auto close");
    showlines  = Hv_SimpleToggleButton(rowcol, "Frame");
    locked     = Hv_SimpleToggleButton(rowcol, "Vertices locked");

    symbolbutton = Hv_CreateSymbolPopup(rowcol, Hv_fixed2);       /* symbol menu */
    stylebutton  = Hv_CreateLineStylePopup(rowcol, Hv_fixed2);    /* line style menu */
    arrowbutton  = Hv_CreateArrowStylePopup(rowcol, Hv_fixed2);   /* arrow style menu */

    dummy = Hv_StandardActionButtons(rowcol, 20, Hv_ALLACTIONS);

/* for use in history */

    Hv_GetOptionMenuButtons(symbolbutton, &buttons,  &num_buttons);
    Hv_GetOptionMenuButtons(stylebutton,  &sbuttons, &num_sbuttons);
    Hv_GetOptionMenuButtons(arrowbutton,  &abuttons, &num_abuttons);
  }

/* the dialog has been created */


/* the the current symbol in the option menu */

  wp = (Hv_WorldPolygonData *)(Item->data);

  Hv_hotSymbol     = wp->symbol;
  Hv_hotStyle      = wp->style;
  Hv_hotArrowStyle = wp->arrowstyle;

  Hv_SetOptionMenu(symbolbutton, buttons,  (int)Hv_hotSymbol);
  Hv_SetOptionMenu(arrowbutton,  abuttons, (int)Hv_hotArrowStyle);
  Hv_SetOptionMenu(stylebutton,  sbuttons, (int)Hv_hotStyle);

  Hv_SetDeleteItem(dialog, Item);

  newpattern = Item->pattern;
  newlinecolor = Item->color;
  newfillcolor = wp->fillcolor;
  newsymbcolor = wp->symbolcolor;

  Hv_SetIntText(thickness, wp->thickness);

  if (mapview) {
    Hv_SetFloatText(baselat,  Hv_RADTODEG*wp->basepoint.h, 4);
    Hv_SetFloatText(baselong, Hv_RADTODEG*wp->basepoint.v, 4);
  }
  else {
    Hv_SetString(baselat,  "N/A");
    Hv_SetString(baselong, "N/A");
  }


  Hv_ChangeLabelPattern(patternlabel, Item->pattern);
  Hv_ChangeLabelColors(linecolorlabel,  -1, Item->color);
  Hv_ChangeLabelColors(fillcolorlabel,  -1, wp->fillcolor);
  Hv_ChangeLabelColors(symbcolorlabel,  -1, wp->symbolcolor);

  Hv_SetToggleButton(showlines,  wp->showlines);
  Hv_SetToggleButton(closed,     wp->closed);
  Hv_SetToggleButton(locked,     !Hv_CheckItemDrawControlBit(Item, Hv_ENLARGABLE));
  oldclosed = wp->closed;


/* ghost a bunch of stuff for the map based polys */

  Hv_SetSensitivity(showlines,      !mapview);
  Hv_SetSensitivity(symbolbutton,   !mapview);
  Hv_SetSensitivity(symbcolorlabel, !mapview);
  Hv_SetSensitivity(arrowbutton,    !mapview);
  Hv_SetSensitivity(baselat,         mapview);
  Hv_SetSensitivity(baselong,        mapview);


  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    Item->pattern = newpattern;
    wp->symbol = Hv_hotSymbol;
    wp->arrowstyle = Hv_hotArrowStyle;
    wp->style = Hv_hotStyle;

    Item->color  = newlinecolor;
    wp->fillcolor = newfillcolor;
    wp->symbolcolor = newsymbcolor;

    wp->showlines  = Hv_GetToggleButton(showlines);
    wp->closed     = Hv_GetToggleButton(closed);

    if (wp->closed != oldclosed) {
	if (mapview) {
	    map = (Hv_Map)(wp->map);
	    Hv_DestroyMap(&map);
	    wp->map = NULL;
	}
    }


/* locked the vertices ? */

    if (Hv_GetToggleButton(locked))
	Hv_ClearItemDrawControlBit(Item, Hv_ENLARGABLE);
    else
	Hv_SetItemDrawControlBit(Item, Hv_ENLARGABLE);

    wp->thickness = Hv_sMax(1, Hv_sMin(6, (short)Hv_GetIntText(thickness)));

    wp->basepoint.h = Hv_DEGTORAD*Hv_GetFloatText(baselat);
    wp->basepoint.v = Hv_DEGTORAD*Hv_GetFloatText(baselong);

/* now handle the redraw -- including the fact the the size of the item probably changed */

    Hv_StandardRedoItem(Item); 

    if (answer != Hv_APPLY)
      break;
  }
}


/* -------- Hv_PolygonAfterDrag -----*/

void Hv_PolygonAfterDrag(Hv_Item  Item,
			 short   dh,
			 short   dv)
{
  Hv_WorldPolygonData  *wp;
  short                n;

  if (Item == NULL)
    return;

  if ((dh == 0) && (dv == 0))
    return;

  if (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG))
    return;

  wp = (Hv_WorldPolygonData *)(Item->data);
  n = wp->numpnts;

  if ((n == 0) || (wp->fpts == NULL))
    return;

  Hv_LocalPolygonToWorldPolygon(Item->view, n, wp->poly, wp->fpts);
}


/* -------- Hv_PolygonAfterOffset -----*/

void Hv_PolygonAfterOffset(Hv_Item  Item,
			   short    dh,
			   short    dv)

{
  Hv_WorldPolygonData  *wp;
  short                 n;
  short                 x, y;
  float                 lat1, long1, gcd, az;
  int                   i;

  if (Item == NULL)
    return;

  if ((dh == 0) && (dv == 0))
    return;

  wp = (Hv_WorldPolygonData *)(Item->data);
  n = wp->numpnts;

  if ((n == 0) || (wp->fpts == NULL))
    return;

  if (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG)) {
    if (Hv_offsetDueToDrag) { 

      lat1  = wp->basepoint.h;
      long1 = wp->basepoint.v;

      Hv_LatLongToLocal(Item->view,
			&x,  &y,
			wp->basepoint.h,
			wp->basepoint.v);

/* offset the basepoint */

      Hv_LocalToLatLong(Item->view,
			x+dh, y+dv,
			&(wp->basepoint.h), &(wp->basepoint.v));

      for (i =0; i < wp->numpnts; i++) {
	az = Hv_GetAzimuth(lat1,
			   long1,
			   wp->fpts[i].h,
			   wp->fpts[i].v);

	
	gcd = Hv_GCDistance(lat1, long1,
			    wp->fpts[i].h, wp->fpts[i].v);
	
	Hv_OffsetLatLong(wp->basepoint.h, wp->basepoint.v, 
			 gcd, az,
			 &(wp->fpts[i].h), &(wp->fpts[i].v));
      }

    }


    Hv_FixPolygonRegion(Item);
  }
  else {

    Hv_OffsetPolygon(wp->poly, (int)n, dh, dv);
    
    if (Hv_offsetDueToDrag)
      Hv_LocalPolygonToWorldPolygon(Item->view, n, wp->poly, wp->fpts);
  }

}

/*------- Hv_OffsetWorldPolygon ---------*/

void  Hv_OffsetWorldPolygon(Hv_Item   Item,
			    float    dx,
			    float    dy)

{
  Hv_WorldPolygonData  *wp;
  short                n, i;

  if (Item == NULL)
    return;

  if ((dx == 0.0) && (dy == 0.0))
    return;

  wp = (Hv_WorldPolygonData *)(Item->data);
  n = wp->numpnts;

  if ((n == 0) || (wp->fpts == NULL))
    return;

  for (i = 0; i < n; i++) {
    wp->fpts[i].h += dx;
    wp->fpts[i].v += dy;
  }


  Item->fixregion(Item);

}


/*-------- Hv_WorldPolygonToLocalPolygon ------------ */

void  Hv_WorldPolygonToLocalPolygon(Hv_View   View,
				    short    npts,
				    Hv_Point  *poly,
				    Hv_FPoint *wp)

{
  int         i;
  short       xoff, yoff;
  float       xx, yy, rx, ry;
  Hv_FRect    *world;
  Hv_Rect     *local;


/* do it manually rather than multiple calls to Hv_WorldToLocal 
   to avoid repeated evaluations of rx and ry */

  local = View->local;
  world = View->world;

  xoff = local->left;
  yoff = local->top;

  rx = ((float)local->width)/world->width;
  ry = ((float)local->height)/world->height;

  for (i = 0; i < npts; i++) {
    xx = xoff + rx*(wp[i].h - world->xmin);
    yy = yoff + ry*(world->ymax - wp[i].v);
    poly[i].x = Hv_InRange(xx);
    poly[i].y = Hv_InRange(yy);

  }
}

/*-------- Hv_LocalPolygonToWorldPolygon ------------ */

void  Hv_LocalPolygonToWorldPolygon(Hv_View   View,
				    short    npts,
				    Hv_Point  *poly,
				    Hv_FPoint *wp)
{
  int   i;
  short       xoff, yoff;
  float       rx, ry;
  Hv_FRect    *world;
  Hv_Rect     *local;

/* do it manually rather than multiple calls to Hv_WorldToLocal 
   to avoid repeated evaluations of rx and ry */

  local = View->local;
  world = View->world;

  xoff = local->left;
  yoff = local->top;

  rx = world->width/((float)local->width);
  ry = world->height/((float)local->height);

  for (i = 0; i < npts; i++) {
    wp[i].h = world->xmin + rx*(poly[i].x - xoff);
    wp[i].v = world->ymax + ry*(yoff - poly[i].y);
  }
}



/*-------- Hv_LatLongPolygonToLocalPolygon ------------ */

void  Hv_LatLongPolygonToLocalPolygon(Hv_View   View,
				      short    npts,
				      Hv_Point  *poly,
				      Hv_FPoint *wp)

{
  int         i;

/* note: h->latitude, v->longitude */

  for (i = 0; i < npts; i++) {
    Hv_LatLongToLocal(View, &(poly[i].x), &(poly[i].y), 
		      wp[i].h, wp[i].v);
  }
}


/*------------ Hv_WorldPolygonArea -------*/


float Hv_WorldPolygonArea(Hv_Item Item) {

  int    np, i;
  float  anglesum;

  Hv_WorldPolygonData *wp;

  if (Item == NULL)
    return 0.0;

  wp = (Hv_WorldPolygonData *)(Item->data);
  np = wp->numpnts;
  
  anglesum = 0.0;

  for (i = 0; i < np; i++) {
    anglesum += Hv_InteriorAngle(Item, i);
  }

  return (anglesum*(np-2) - Hv_PI)*Hv_RADEARTH*Hv_RADEARTH;
}


/*-------- Hv_LocalPolygonToLatLongPolygon ------------ */

void  Hv_LocalPolygonToLatLongPolygon(Hv_View   View,
				      short    npts,
				      Hv_Point  *poly,
				      Hv_FPoint *wp)
{
  int         i;

/* note: h->latitude, v->longitude */

  for (i = 0; i < npts; i++) {
      Hv_LocalToLatLong(View, poly[i].x, poly[i].y, 
			&(wp[i].h), &(wp[i].v));
    
  }


}


/*------- Hv_DrawWorldPolygonButton -------*/

void Hv_DrawWorldPolygonButton(Hv_Item    Item,
			       Hv_Region   region)

/* draws the icon for world line tool */
     
{
  Hv_Rect          area;
  Hv_Point         poly[9];

  Hv_ButtonDrawingArea(Item, &area);

  Hv_SetPoint(&(poly[0]), area.left+1, area.bottom-4);
  Hv_SetPoint(&(poly[1]), poly[0].x,    poly[0].y-9);
  Hv_SetPoint(&(poly[2]), poly[1].x+6,  poly[1].y-6);
  Hv_SetPoint(&(poly[3]), poly[2].x+10, poly[2].y);
  Hv_SetPoint(&(poly[4]), poly[3].x+8,  poly[3].y+8);
  Hv_SetPoint(&(poly[5]), poly[4].x-7,  poly[4].y+7);
  Hv_SetPoint(&(poly[6]), poly[5].x-9,  poly[5].y);
  Hv_SetPoint(&(poly[7]), poly[6].x,    poly[6].y-6);
  Hv_SetPoint(&(poly[8]), poly[7].x-6,  poly[7].y+6);

  Hv_FillPolygon(poly, 9, True, Hv_cornFlowerBlue, Hv_black);
  Hv_DrawLine(poly[0].x, poly[0].y, poly[1].x, poly[1].y, Hv_white);
  Hv_DrawLine(poly[1].x-1, poly[1].y, poly[2].x, poly[2].y, Hv_white);
  Hv_DrawLine(poly[2].x, poly[2].y, poly[3].x, poly[3].y, Hv_white);
  Hv_DrawLine(poly[6].x, poly[6].y, poly[7].x, poly[7].y, Hv_white);
}


/*------- CopyPolygonsViewToView ----------*/

static void CopyPolygonsViewToView(Hv_View dv,
				   Hv_View sv) {

    Hv_Item       temp;
    Hv_Item       dummy;
    Boolean       copied = False;

/*
 * loop over the source view items, copying each drawing tool
 * tagged polygon
 */


    for (temp = sv->items->first;  temp != NULL;  temp = temp->next) {
	if (temp->type == Hv_WORLDPOLYGONITEM) {
	    if (temp->tag == Hv_DRAWINGTOOLTAG) {
		dummy = Hv_CopyPolygonItem(dv, temp);


/*===
 * see if the user wants to be notified
 *===*/

		if (Hv_CreateDrawingItemCB != NULL)
		    Hv_CreateDrawingItemCB(dummy);

		copied = True;
	    }
	}
    }
    

    if (copied)
	Hv_DrawViewHotRect(dv);
    
}


/* ------- Hv_WorldPolygonButtonCallback -------*/

void Hv_WorldPolygonButtonCallback(Hv_Event hvevent)

{
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  Hv_Item              newItem;
  static Cursor        tempcursor;
  Hv_Point             StartPP;
  Hv_Rect              limitrect;
  Hv_Point            *poly;
  Hv_FPoint           *fpts = NULL;
  short                npts;
  Hv_Item              parent = NULL;
  Hv_WorldPolygonData *wp;
  Boolean              mapview;
  int                  dc;

  char                *qstr;


  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent, Hv_WorldPolygonButtonCallback,
			  &activeView, &activeItem, &tempcursor);
    return;
  }
  
  if (View != activeView) {

      qstr = (char *)Hv_Malloc(100 + strlen(activeView->containeritem->str->text)
			       + strlen(View->containeritem->str->text));

    sprintf(qstr, "Copy polygons from %s to %s?",
	    activeView->containeritem->str->text,
	    View->containeritem->str->text);
    
    if (Hv_Question(qstr)) {
      CopyPolygonsViewToView(View, activeView);      
      Hv_ResetActiveView(NULL, &activeView, &activeItem, tempcursor);
    }


    return;
  }

  dc = Hv_ENLARGEABLE + Hv_ZOOMABLE + Hv_FANCYDRAG;
  mapview = Hv_IsMapView(View);

  if (mapview)
      dc = Hv_ROTATABLE + Hv_ZOOMABLE + Hv_FANCYDRAG;
  else
      dc = Hv_ENLARGEABLE + Hv_ZOOMABLE + Hv_FANCYDRAG;

/* see if click occured inside hotrect */

  Hv_SetPoint(&StartPP, hvevent->where.x, hvevent->where.y); 
  if (Hv_PointInRect(StartPP, View->hotrect)) {


/* for certain items, we assume the desire is to contain the
   new item */

    parent = Hv_PointInPlotItem(View, StartPP);
    
    if (parent == NULL)
      Hv_CopyRect(&limitrect, View->hotrect);
    else
      Hv_CopyRect(&limitrect, parent->area);

    Hv_SelectPolygon(&poly, &npts, limitrect, StartPP);

    if (poly != NULL) {

      fpts = (Hv_FPoint *)(Hv_Malloc(npts*sizeof(Hv_FPoint)));

/*
 * In 1.0053, handle map views differently
 */

      if (mapview) 
	Hv_LocalPolygonToLatLongPolygon(View, npts, poly, fpts);
      else
	Hv_LocalPolygonToWorldPolygon(View, npts, poly, fpts);
      
      newItem = Hv_VaCreateItem(View,
				Hv_PARENT,       parent,
				Hv_TAG,          Hv_DRAWINGTOOLTAG,
				Hv_TYPE,         Hv_WORLDPOLYGONITEM,
				Hv_NUMPOINTS,    npts,
				Hv_DATA,         fpts,
				Hv_NUMROWS,      9,
				Hv_NUMCOLUMNS,   9,
				Hv_DOMAIN,       Hv_INSIDEHOTRECT,
				Hv_DRAWCONTROL,  dc,
				Hv_AFTERDRAG,    Hv_PolygonAfterDrag,
				Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
				Hv_FIXREGION,    Hv_FixPolygonRegion,
				Hv_CHECKENLARGE, Hv_CheckWorldPolygonEnlarge,
				NULL);

/* now set the properties based on the view's defaults */
      
      if (Hv_usedNewDrawTools) {
	wp = (Hv_WorldPolygonData *)(newItem->data);
        wp->fillcolor = View->hotfillcolor;
	wp->closed = (View->hotshape == Hv_CLOSEDPOLYSHAPE);
	newItem->pattern = View->hotpattern;
	newItem->color = View->hotbordercolor;
	wp->thickness = View->hotlinewidth;
	wp->arrowstyle = View->hotarrowstyle;
	wp->style = View->hotlinestyle;
	Hv_usedNewDrawTools = False;
      }


/*===
 * see if the user wants to be notified
 *===*/

      if (Hv_CreateDrawingItemCB != NULL)
	Hv_CreateDrawingItemCB(newItem);
      
      Hv_DrawItem(newItem, NULL); 
      Hv_Free(poly);
    }
  

  } /* end in hotrect */
  
  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

}

/*--- Hv_PolyIsRotatable -----*/

static Boolean Hv_PolyIsRotatable(Hv_Item Item) {

    Hv_WorldPolygonData  *wp;
    wp = (Hv_WorldPolygonData *)(Item->data);
    
    return ((wp->closed) && Hv_CheckItemDrawControlBit(Item, Hv_ROTATABLE));
    
}


/* -------- Hv_DrawMapPolygon --------------- */

static  void Hv_DrawMapPolygon(Hv_Item   Item,
			       Hv_Region region) {

  Hv_WorldPolygonData  *wp;
  short                x, y;
  XRectangle           xr;
  short                x2, y2;
  Hv_Region            lineregion = NULL;
  Hv_Region            treg = NULL;
  Hv_Rect              rect;

  wp = (Hv_WorldPolygonData *)(Item->data);

  Hv_GetPolygonMap(Item, wp);
  if (wp->map == NULL) {
    fprintf(stderr, "Null polygon map.\n");
    return;
  }

  Hv_SetLineStyle(wp->thickness, wp->style);
    
  if (Item->region != NULL)
    Hv_DestroyRegion(Item->region);

  Item->region = Hv_CreateRegion();

  Hv_DrawPolygonMap(Item,
		    region,
		    (Hv_Map)(wp->map),
		    True,
		    Item->color,
		    wp->fillcolor,
		    Item->pattern);
  

/* require rotatble bit AND closed */

  if (Hv_PolyIsRotatable(Item)) {
    Hv_GetRRP(Item);
    x = Item->rotaterp->x;
    y = Item->rotaterp->y;

    Hv_GetRectCenter(Item->rotaterect, &x2, &y2);

    Hv_SetRect(&rect, x-3, y-3, 7, 7);
    Hv_FillRect(&rect,  Hv_blue);
    Hv_FrameRect(&rect, Hv_orange);
    Hv_UnionRectWithRegion(&rect, Item->region);

    Hv_DrawLine(x, y, x2, y2, Hv_gray8);

    lineregion =  Hv_CreateRegionFromLine(x, y, x2, y2, 2);

    Hv_FillRect(Item->rotaterect, Hv_orange);
    Hv_FrameRect(Item->rotaterect, Hv_blue);



    treg = Item->region;
    Item->region = Hv_UnionRegion(lineregion, treg);
    Hv_UnionRectWithRegion(Item->rotaterect, Item->region);

    Hv_DestroyRegion(treg);
    Hv_DestroyRegion(lineregion);
  }
  else { /* not rotatable */
      if (Item->rotaterect != NULL)
	  Hv_SetRect(Item->rotaterect, -10000, -10000, 0, 0);
  }


  Hv_SetPattern(-1, Hv_black);
  Hv_SetLineStyle(0, Hv_SOLIDLINE);

  XClipBox(Item->region, &xr);
  Hv_SetRect(Item->area, xr.x, xr.y, xr.width, xr.height);
  
  Hv_LocalRectToWorldRect(Item->view, Item->area, Item->worldarea);
}


/* -------- Hv_DrawWorldPolygonItem --------------- */

void Hv_DrawWorldPolygonItem(Hv_Item    Item,
			     Hv_Region   region)

/*draws the specified Item Hv_WORLDPOLYGONITEM*/

{
  Hv_View               View = Item->view;
  Hv_WorldPolygonData  *wp;
  Hv_Region             visrgn = NULL;
  Hv_Region             trgn;
  Hv_Point             *pspoly = NULL;
  short                 np;
  Hv_Point              arrowpoly[4];


  if (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG)) {
    Hv_DrawMapPolygon(Item, region);
    Hv_RestoreClipRegion(region);
    return;
  }


  wp = (Hv_WorldPolygonData *)(Item->data);

  if (wp->poly == NULL) {
    fprintf(stderr, "Hv warning: attempt to draw world polygon with NULL poly\n");
    return;
  }


  np = wp->numpnts;

  if (Hv_extraFastPlease) {
    Hv_FramePolygon(wp->poly, np, Item->color);
    return;
  }

/* standard draw */


  if (np > 2)

/* fill if color >= 0  */

    if (wp->fillcolor >= 0) {
      if (Item->pattern >= 0) 
	Hv_SetPattern(Item->pattern, wp->fillcolor);

      if ((Item->pattern != Hv_HOLLOWPAT) && wp->closed)
	Hv_FillPolygon(wp->poly, np, False, wp->fillcolor, 0);
      if (Item->pattern >= 0)
	Hv_SetPattern(-1, Hv_black);
    }

  if (wp->showlines) {
    Hv_SetLineStyle(wp->thickness, wp->style);
    if (wp->closed)
      Hv_FramePolygon(wp->poly, np, Item->color);
    else
      Hv_DrawLines(wp->poly, np, Item->color);
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
  }

/* arrow ? */
  
  if ((wp->arrowstyle == Hv_ARROWATEND) || (wp->arrowstyle == Hv_ARROWATBOTH)) {
    Hv_GetPolygonArrowPolygon(Item, arrowpoly, Hv_ARROWATEND);
    Hv_FillPolygon(arrowpoly, 4, False, Item->color, Hv_black);
  }
  if ((wp->arrowstyle == Hv_ARROWATSTART) || (wp->arrowstyle == Hv_ARROWATBOTH)) {
    Hv_GetPolygonArrowPolygon(Item, arrowpoly, Hv_ARROWATSTART);
    Hv_FillPolygon(arrowpoly, 4, False, Item->color, Hv_black);
  }

    
/* grid ? */

  if (wp->grid) {
    if (Hv_inPostscriptMode) {
      pspoly = Hv_NewPoints((int)np);
      Hv_LocalPolygonToPSLocalPolygon(wp->poly, pspoly, np);
      Hv_PSClipPolygon(pspoly, np);
    }

    if (wp->showpoints) {
      trgn = Hv_CreateRegionFromPolygon(wp->poly, np);
      visrgn = Hv_IntersectRegion(trgn, region);
      Hv_DestroyRegion(trgn);
    }
    else
      visrgn = Hv_IntersectRegion(Item->region, region);

    Hv_SetClippingRegion(visrgn);

    Hv_DrawWorldGrid(View, Item->worldarea, wp->nrows, wp->ncols, Item->color, 0);
    Hv_DestroyRegion(visrgn);
    if (Hv_inPostscriptMode) {
      Hv_PSRestore();
      Hv_Free(pspoly);
    }
  }   /* end grid */

/* points ? */

  if (wp->showpoints) {
    
/* if drew grid, clip region must be adjusted */

    if (wp->grid){ 
      visrgn = Hv_IntersectRegion(Item->region, region);
      Hv_SetClippingRegion(visrgn);
    }

    Hv_DrawSymbolsOnPoints(wp->poly,
			   (int)(np),
			   wp->symbolsize,
			   wp->symbolcolor,
			   wp->symbol);

    Hv_DestroyRegion(visrgn);
    
  }
}



/*------- Hv_FixPolygonMapRegion -------*/

static void  Hv_FixPolygonMapRegion(Hv_Item Item)

{
  Hv_View              View = Item->view;
  Hv_WorldPolygonData  *wp;
  XRectangle           xr;
  short                x, y, x2, y2;
  Hv_Region            treg = NULL;
  Hv_Region            lineregion = NULL;
  Hv_Rect              rect;

  if (Item->region != NULL)
    Hv_DestroyRegion(Item->region);
  Item->region = NULL;

  wp = (Hv_WorldPolygonData *)(Item->data);

  Item->region = Hv_CreateRegion();

  Hv_GetPolygonMap(Item, wp);


  yesdraw = False;

  Hv_DrawPolygonMap(Item,
		    NULL,
		    (Hv_Map)(wp->map),
		    True,
		    0,
		    -1,
		    -1);
  
  yesdraw = True;
  
  if (Hv_PolyIsRotatable(Item)) {
    Hv_GetRRP(Item);
    x = Item->rotaterp->x;
    y = Item->rotaterp->y;


    Hv_GetRectCenter(Item->rotaterect, &x2, &y2);

    Hv_SetRect(&rect, x-3, y-3, 7, 7);
    Hv_UnionRectWithRegion(&rect, Item->region);

    lineregion =  Hv_CreateRegionFromLine(x, y, x2, y2, 2);
    treg = Item->region;
    Item->region = Hv_UnionRegion(lineregion, treg);
    Hv_UnionRectWithRegion(Item->rotaterect, Item->region);
    Hv_DestroyRegion(treg);
    Hv_DestroyRegion(lineregion);
  }
  else { /* not rotatable */
      if (Item->rotaterect != NULL)
	  Hv_SetRect(Item->rotaterect, -10000, -10000, 0, 0);
  }
  

  XClipBox(Item->region, &xr);
  Hv_SetRect(Item->area, xr.x, xr.y, xr.width, xr.height);
  Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);

}


/*------- Hv_FixPolygonRegion -------*/

void  Hv_FixPolygonRegion(Hv_Item Item)

{
  Hv_View              View = Item->view;
  Hv_WorldPolygonData  *wp;
  XRectangle           xr;
  short                ssize;
  Hv_Region            polyreg = NULL;
  Hv_Point             poly[4];



  if (Hv_IsMapView(View) && (Item->tag == Hv_DRAWINGTOOLTAG)) {
    Hv_FixPolygonMapRegion(Item);
    return;
  }

  wp = (Hv_WorldPolygonData *)(Item->data);

  Hv_DestroyRegion(Item->region);
  Item->region = NULL;

  ssize = wp->symbolsize;

  if (wp->poly != NULL)
    Hv_Free(wp->poly);

  if ((wp->fpts != NULL) && (wp->numpnts > 1)) {

    wp->poly = Hv_NewPoints((int)wp->numpnts);
    Hv_WorldPolygonToLocalPolygon(View, wp->numpnts, wp->poly, wp->fpts);

    if (wp->closed)
      Item->region = Hv_CreateRegionFromPolygon(wp->poly, wp->numpnts);
    else
      Item->region = Hv_CreateRegionFromLines(wp->poly, wp->numpnts-1, 2);


/* if just two identical points, the region will be NULL ! */

    if (Item->region == NULL) 
      Hv_SetRect(Item->area, wp->poly[0].x - ssize/2, wp->poly[0].y - ssize/2, ssize, ssize);
    else {
      ssize = ssize+2;
      if ((wp->showpoints) && (wp->symbol != Hv_NOSYMBOL))
	XShrinkRegion(Item->region, -ssize, -ssize);
      XClipBox(Item->region, &xr);
      Hv_SetRect(Item->area, xr.x, xr.y, xr.width, xr.height);
    }

    Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
  }

/* arrow region to be added? */

  if ((wp->arrowstyle == Hv_ARROWATEND) || (wp->arrowstyle == Hv_ARROWATBOTH)) {
    Hv_GetPolygonArrowPolygon(Item, poly, Hv_ARROWATEND);
    polyreg = Hv_CreateRegionFromPolygon(poly, 4);
    Hv_AddRegionToRegion(&(Item->region), polyreg);
    Hv_DestroyRegion(polyreg);
  }

  if ((wp->arrowstyle == Hv_ARROWATSTART) || (wp->arrowstyle == Hv_ARROWATBOTH)) {
    Hv_GetPolygonArrowPolygon(Item, poly, Hv_ARROWATSTART);
    polyreg = Hv_CreateRegionFromPolygon(poly, 4);
    Hv_AddRegionToRegion(&(Item->region), polyreg);
    Hv_DestroyRegion(polyreg);
  }
}


/* -------  Hv_WorldPolygonInitialize --------*/

void Hv_WorldPolygonInitialize(Hv_Item      Item,
			       Hv_AttributeArray attributes)

{
  Hv_WorldPolygonData  *wp;

  Item->standarddraw = Hv_DrawWorldPolygonItem;
  Item->worldarea = Hv_NewFRect();
  Item->rotaterp = (Hv_Point *)Hv_Malloc(sizeof(Hv_Point));
  Item->azimuth = 0.0;

  wp = (Hv_WorldPolygonData *)Hv_Malloc(sizeof(Hv_WorldPolygonData));
  Item->data = (void *)wp;

  wp->map = NULL;
  wp->thickness = 1;
  wp->arrowstyle = Hv_NOARROW;
  wp->closed = True;
  wp->style = Hv_SOLIDLINE;
  wp->special4pt = False;

/* show points is archaic but preserve it for backward
   compatibility */

  wp->showpoints = True;
  wp->grid = False;
  wp->showlines = True;
  wp->symbol = Hv_NOSYMBOL;
  wp->symbolcolor = Hv_skyBlue;
  wp->symbolsize = 8;
  wp->numpnts = attributes[Hv_NUMPOINTS].s;
  wp->nrows = attributes[Hv_NUMROWS].s;
  wp->ncols = attributes[Hv_NUMCOLUMNS].s;
  wp->fillcolor = attributes[Hv_FILLCOLOR].s;
  wp->fpts = (Hv_FPoint *)(attributes[Hv_DATA].v);

/* default the base point to the first point */

  if (Hv_IsMapView(Item->view) && (Item->tag == Hv_DRAWINGTOOLTAG)) {
    Hv_SetFPoint(&(wp->basepoint), wp->fpts[0].h, wp->fpts[0].v);
  }

  wp->user1 = attributes[Hv_USER1].s;
  wp->poly = NULL;
  wp->userdata = attributes[Hv_USERDATA].v;

  if (attributes[Hv_FIXREGION].v == NULL)
    Item->fixregion = Hv_FixPolygonRegion;

/* for hotrect text items, use a special single click callback that
   will allow me to duplicate */
    
    if ((Item->domain == Hv_INSIDEHOTRECT) && (Item->singleclick == NULL))
	Item->singleclick = Hv_PolygonItemSingleClick;


  if (Item->doubleclick == NULL)
    Item->doubleclick = Hv_EditWorldPolygonItem;  /* default doubleclick is the world rect item editor */
  
  if (Item->fixregion != NULL)
    Item->fixregion(Item);

/* set a special bit indicating that this item is based on
   a world polygon */

  Hv_SetItemDrawControlBit(Item, Hv_WPOLYBASED);

}


/*------- Hv_DestroyWorldPolygon ---------*/

void Hv_DestroyWorldPolygon(Hv_Item Item)

{
  Hv_WorldPolygonData        *wp;
  Hv_Map                     map;

  wp = (Hv_WorldPolygonData *)(Item->data);

  if (wp->map != NULL) {
    map = (Hv_Map)(wp->map);
    Hv_DestroyMap(&map);
  }

  Hv_Free(wp->fpts);
  Hv_Free(wp->poly);
  Hv_Free(wp);
  Item->data = NULL;
}


/*------- Hv_CheckWorldPolygonEnlarge ---------*/

void Hv_CheckWorldPolygonEnlarge(Hv_Item    Item,
				 Hv_Point   StartPP,
				 Boolean    Shifted,
				 Boolean   *enlarged)

/* this is the routine that permits
   polygon vertices to be moved around */

{
  Hv_Region             udr1, udr2;
  Hv_View               View = Item->view;
  Hv_Point              fixedpts[2];
  int                   i;
  Hv_Rect               checkrect;
  short                 slop = 17;
  short                 slop2 = 7;
  Hv_WorldPolygonData   *wp;
  Boolean               inrect = False;
  Hv_Point              *poly;
  short                 n;
  Hv_Rect               limitrect;
  short                 nfp = 2;

/* see if we are in any point */

  wp = (Hv_WorldPolygonData *)(Item->data);
  n = wp->numpnts;

  poly = Hv_NewPoints((int)n);

  if (Hv_IsMapView(View) && (Item->tag == Hv_DRAWINGTOOLTAG))
    Hv_LatLongPolygonToLocalPolygon(View, n, poly, wp->fpts);
  else
    Hv_WorldPolygonToLocalPolygon(View, n, poly, wp->fpts);

  i = 0;
  Hv_lastDraggedVertex = -1;

  while (!inrect && (i < n)) {
    Hv_SetRect(&checkrect, poly[i].x-slop2, poly[i].y-slop2, slop, slop);

    if (Hv_PointInRect(StartPP, &checkrect)) {
      inrect = True;
      Hv_lastDraggedVertex = i;
      
      if ( i > 0)
	fixedpts[0] = poly[i-1];
      else {  /* i == 0 */
	if (wp->closed)
	  fixedpts[0] = poly[n-1];
	else
	  nfp = 1;
      }

      if (i < (n-1))
	fixedpts[nfp-1] = poly[i+1];
      else { /* on last point */
	if (wp->closed)
	  fixedpts[nfp-1] = poly[0];
	else
	  nfp = 1;
      }
    }
    else
      i++;
  }

  if (!inrect) {
    Hv_Free(poly);
    *enlarged = False;
    return;
  }

  udr1 = Hv_CopyItemRegion(Item, False);
  XShrinkRegion((Region)udr1, -2, -2);

  Hv_CopyRect(&limitrect, View->hotrect);
  udr2 = Hv_ClipHotRect(View, 0);


  if (wp->special4pt)
    Hv_Special4ptEnlarge(limitrect, StartPP, i, Item);
  else {
    Hv_theEnlargedPolyNp = n; 
    Hv_theEnlargedPoly = poly;

    if (Hv_CheckItemDrawControlBit(Item, Hv_FEEDBACKWHENDRAGGED))
      Hv_EnlargeLine(limitrect, StartPP, fixedpts, -nfp, poly+i);
    else
      Hv_EnlargeLine(limitrect, StartPP, fixedpts, nfp, poly+i);

    Hv_theEnlargedPoly = NULL;
    Hv_theEnlargedPolyNp = 0;

    if (Hv_IsMapView(View) && (Item->tag == Hv_DRAWINGTOOLTAG))
      Hv_LocalToLatLong(View,
			poly[i].x,
			poly[i].y,
			&(wp->fpts[i].h),
			&(wp->fpts[i].v));

    else
      Hv_LocalToWorld(View,
		      &(wp->fpts[i].h),
		      &(wp->fpts[i].v),
		      poly[i].x,
		      poly[i].y);
  }


  Hv_DestroyRegion(udr2);
  Hv_Free(poly);

/* specialized region fix? */

  if (Item->fixregion != NULL)
    Item->fixregion(Item);

  Hv_UpdateConnections(View, Item, True);
  Hv_ItemChangedRedraw(Item, udr1);
  Hv_DestroyRegion(udr1); 
  
  *enlarged = True;
}

/* ------ Hv_Special4PtEnlarge --------*/

static void Hv_Special4ptEnlarge(Hv_Rect  LimitRect,
				 Hv_Point StartPP,
				 int      vertex,
				 Hv_Item  Item)

/**********************************************
  special routine to enlarge 4pt polys that
  you want to keep rectangular
*********************************************/

{
  Hv_View              View = Item->view;
  Boolean              done = False;
  Hv_WorldPolygonData  *wp;
  Boolean              fback;
  Hv_Region            hrr = NULL;
  XEvent               Event;                 /* An X event */
  short                dh, dv, oldx, oldy;
  short                newx, newy;

  Hv_Point             *poly;
  Hv_FPoint            *fpts;
  float                fx, fy;
  short                xc, yc;

  Hv_FPoint            df;

  wp = (Hv_WorldPolygonData *)(Item->data);

  if ((wp->numpnts != 4) || (vertex > 3))
    return;

  poly = wp->poly;
  fpts = wp->fpts;

  LimitRect.width  -= 1;
  LimitRect.height -= 1;
  Hv_FixRectRB(&LimitRect);

  fback = Hv_CheckItemDrawControlBit(Item, Hv_FEEDBACKWHENDRAGGED);

  hrr = Hv_ClipHotRect(Hv_hotView, 0);

  XGrabPointer(Hv_display, Hv_mainWindow, True, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  oldx = StartPP.x;
  oldy = StartPP.y;

  Hv_GetRectCenter(Item->area, &xc, &yc);
  Hv_LocalToWorld(View, &fx, &fy, xc, yc);

  Hv_FrameXorPolygon(poly, 4);

  while (!done) {

/* if Check For Motion or Button Releases */

    if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonPressMask | ButtonMotionMask, &Event)) {

      switch(Event.type) {

/* Motion Notify means the mouse is moving */

      case MotionNotify: 

	Hv_SetClippingRegion(hrr);

	Hv_FrameXorPolygon(poly, 4);

	newx = Hv_sMax(LimitRect.left, Hv_sMin(LimitRect.right,  (short)Event.xbutton.x));
	newy = Hv_sMax(LimitRect.top,  Hv_sMin(LimitRect.bottom, (short)Event.xbutton.y));

	if (fback && (Hv_hotView->feedback))
	  Hv_hotView->feedback(Hv_hotView, poly[vertex]);
	
	dh = newx - oldx;
	dv = newy - oldy;

	df.h = (float)dh;
	df.v = (float)dv;

	Hv_RotateFPoint(-(Item->azimuth), &df);
	dh = (short)df.h;
	dv = (short)df.v;

	Hv_RotateFPointsAboutPoint(Item->azimuth, fpts, 4, fx, fy);

	Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);

/* poly is now "unrotated" */

	poly[vertex].x  += dh;
	poly[vertex].y  += dv;

/* now the part that keeps it rectangular */

	switch(vertex) {
	case 0:
	  poly[1].x -= dh;   poly[1].y += dv;
	  poly[2].x -= dh;   poly[2].y -= dv;
	  poly[3].x += dh;   poly[3].y -= dv;
	  break;

	case 1:
	  poly[2].x += dh;   poly[2].y -= dv;
	  poly[3].x -= dh;   poly[3].y -= dv;
	  poly[0].x -= dh;   poly[0].y += dv;
	  break;

	case 2:
	  poly[3].x -= dh;   poly[3].y += dv;
	  poly[0].x -= dh;   poly[0].y -= dv;
	  poly[1].x += dh;   poly[1].y -= dv;
	  break;

	case 3:
	  poly[0].x += dh;   poly[0].y -= dv;
	  poly[1].x -= dh;   poly[1].y -= dv;
	  poly[2].x -= dh;   poly[2].y += dv;
	  break;

	}

	oldx = newx;
	oldy = newy;

	Hv_SetClippingRegion(hrr);

	Hv_LocalPolygonToWorldPolygon(View, 4, poly, fpts);
	Hv_RotateFPointsAboutPoint(-(Item->azimuth), fpts, 4, fx, fy);
	Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
	Hv_FrameXorPolygon(poly, 4);
	break;

     /* end of case MotionNotify */
	      
/* if button WhatButton is released, we are done */

      case ButtonRelease:
	done  = True;
	break;
	
      default:
	break;
      }
    }  /* end  if if CheckMask */
  }   /*end of while !done */


/* Final xor poly */

  Hv_SetClippingRegion(hrr);
  Hv_FrameXorPolygon(poly, 4);

  XUngrabPointer(Hv_display, CurrentTime);
  Hv_DestroyRegion(hrr);

}


/* ------ Hv_EditWorldPolygonColor --- */

static void Hv_EditWorldPolygonColor (Widget w)

{
  if (w == linecolorlabel)
    Hv_ModifyColor(w, &newlinecolor, "Edit Line Color", False);
  else if (w == symbcolorlabel)
    Hv_ModifyColor(w, &newsymbcolor, "Edit Symbol Color", False);
  else if (w == fillcolorlabel)
    Hv_ModifyColor(w, &newfillcolor, "Edit Fill Color", True);
}

/* ------ Hv_EditWorldPolygonPattern --- */

static void Hv_EditWorldPolygonPattern(Widget w)

{
  if (w == patternlabel)
    Hv_ModifyPattern(w, &newpattern, "Fill Pattern");

}


/*------- Hv_ArrowPolygonFromWorldLine -------*/

void Hv_ArrowPolygonFromWorldLine(Hv_View    View,
				  Hv_FPoint  *fp1,
				  Hv_FPoint  *fp2,
				  short      thickness,
				  Hv_Point   *poly)

{
  Hv_Point               pc;
  Hv_FPoint              R, fpts[8];
  float                  theta;
  int                    i;
  short                  ygap, xgap, w2, flare;
  int                    np = 4;

/* r is the "vector" corresponding to the line segment */

  R.h = fp2->h - fp1->h;
  R.v = fp2->v - fp1->v;

/* pc is utilimately the "tot point" of our arrow poly */

  Hv_WorldToLocal(View, fp2->h, fp2->v, &(pc.x),  &(pc.y));

/* theta is the angle with respect to the horizontal */
  
  if ((fabs(R.h) < 1.0e-20) && (fabs(R.v) < 1.0e-20))
    theta = 0.0;
  else
    theta = atan2(R.v, R.h);
  
  
  if (thickness < 2) {
    ygap = 6;
    xgap = 10;
  }
  else {
    ygap = 5 + thickness;
    xgap = 9 + thickness;
  }

  w2 = thickness / 2;
  flare = 2 + thickness / 3;

  Hv_SetPoint(poly,   pc.x, pc.y);
  Hv_SetPoint(poly+1, pc.x-flare, pc.y-ygap);
  Hv_SetPoint(poly+2, pc.x+xgap, pc.y);
  Hv_SetPoint(poly+3, pc.x-flare, pc.y+ygap);
  
  Hv_LocalPolygonToWorldPolygon(View, np, poly, fpts);

  for (i = 1; i < np; i++) {
    fpts[i].h -= fp2->h;
    fpts[i].v -= fp2->v;
  }

  Hv_RotateFPoints(theta, fpts+1, np-1);
  
  for (i = 1; i < np; i++) {
    fpts[i].h += fpts[0].h;
    fpts[i].v += fpts[0].v;
  }

  Hv_WorldPolygonToLocalPolygon(View, np, poly, fpts); 
}


/*-----------------------------------------------------------
 *  Hv_PolygonAfterRotate
 *-----------------------------------------------------------*/

void Hv_PolygonAfterRotate(Hv_Item Item) {
}

/*--------- Hv_GetPolygonArrowPloygon -----*/

static void  Hv_GetPolygonArrowPolygon(Hv_Item   Item,
				       Hv_Point  *poly,
				       short     style)
     
/*****************************************************
  Gets the arrow polygon at one end of a world poly
******************************************************/

{
  Hv_View                View = Item->view;
  Hv_FPoint             *fp1, *fp2;
  Hv_WorldPolygonData   *wp;

  wp = (Hv_WorldPolygonData *)(Item->data);
  
  if (style < Hv_ARROWATEND)
    return;

  if (style == Hv_ARROWATEND) {
    fp1 = (wp->fpts) + wp->numpnts - 2;
    fp2 = (wp->fpts) + wp->numpnts - 1;
  }
  else { /* arrow at start */
    fp1 = (wp->fpts) + 1;
    fp2 = wp->fpts;
  }

  Hv_ArrowPolygonFromWorldLine(View, fp1, fp2, wp->thickness, poly);
}


/*--------- Hv_GetPolygonMap ----------*/

void Hv_GetPolygonMap(Hv_Item              Item,
		      Hv_WorldPolygonData *wp) {
     

  int       i, np;
  float     latarg;
  Hv_Map    map;
  int       j, k, m;
  float     gcd, r, delr, az, lat2, long2;
  int       lpp1;

  if (Item == NULL){
    fprintf(stderr, "Bad Item in Hv_GetPolygonMap\n");
    return;
  }

  if (wp == NULL){
    fprintf(stderr, "Bad WP in Hv_GetPolygonMap\n");
    return;
  }

  lpp1 = LEGPOINTS+1;

  if (wp->map == NULL) {

/* must include space for the legpoints, how
   many depends on whether we are closed */

      if (wp->closed)
	  np = (wp->numpnts)*lpp1 + 1;
      else
	  np = (wp->numpnts) + (wp->numpnts-1)*LEGPOINTS;

    wp->map = (void *)Hv_MallocMap(np);

  }

  map = (Hv_Map)(wp->map);
  np = map->numpoints;

/* loop over the "vertices" stored as lat longs in fpts */

  j = 0;
  for (i = 0; i < wp->numpnts; i++) {

/* first get the vertex point itself */

    map->longitude[j]  = wp->fpts[i].v;
    latarg = tan(0.5*(Hv_PIOVER2 + wp->fpts[i].h));
    if (latarg < 1.0e-20)
      latarg = 1.0e-20;

    map->latitude[j] = log(latarg);  /* convert the latitude to a y value */
    j++;

/* now add the leg points, just skipping the
 * last leg for open polys. The leag will connect
 * point "i", to i+1, mod numpnts
 */

    if (wp->closed || (i < (wp->numpnts-1))) {
	m = i+1;
	if (m >= wp->numpnts)
	    m = 0;

	gcd = Hv_GCDistance(wp->fpts[i].h, wp->fpts[i].v,
			    wp->fpts[m].h, wp->fpts[m].v);
    
	az = Hv_GetAzimuth(wp->fpts[i].h,
			   wp->fpts[i].v,
			   wp->fpts[m].h,
			   wp->fpts[m].v);
	
	delr = gcd/lpp1;
    
	for (k = 0; k < LEGPOINTS; k++) {
	    r = (k+1)*delr;
	    Hv_OffsetLatLong(wp->fpts[i].h, wp->fpts[i].v, 
		       r, az,
		       &lat2, &long2);
	    map->longitude[j]  = long2;
	    
	    latarg = tan(0.5*(Hv_PIOVER2 + lat2));
	    if (latarg < 1.0e-20)
		latarg = 1.0e-20;
	    
	    map->latitude[j] = log(latarg);  /* convert the latitude to a y value */

	    j++;
	} /* end of k loop */
    }
  }

/* finally, map the last to the first if closed,
   last to next to last if open */

  if (wp->closed) {
      map->latitude[np-1]  = map->latitude[0];
      map->longitude[np-1] = map->longitude[0];
  }
}


/*--------------------------------------
 * Hv_DrawPolygonMap used for wedges placed
 * on map views
 *--------------------------------------*/


void Hv_DrawPolygonMap(Hv_Item    Item,
		       Hv_Region  region,
		       Hv_Map     map,
		       Boolean    dounion,
		       short      color,
		       short      fillcolor,
		       short      pattern) {
  
/* if color < 0, uses xor mode */

  Hv_Point        *xp;
  int             npoly;
  Hv_Region       preg = NULL;
  Hv_Region       hrr = NULL;
  Hv_Region       cliprgn = NULL;
  Boolean         changepat;
  Hv_View         View = Item->view;


  Hv_Point             *pp = NULL;
  Hv_Region            treg = NULL;
  Hv_Region            totreg = NULL;
  Hv_WorldPolygonData  *wp;

  if (map == NULL) {
    fprintf(stderr, "NULL map in DrawPolygonMap\n");
    return;
  }

/* first the fill part */

  changepat = ((!Hv_extraFastPlease) && (fillcolor >= 0) && (pattern >= 0));

  if (changepat) {
    Hv_SetPattern(pattern, fillcolor);
  }


  if (yesdraw) {
      hrr = Hv_RectRegion(View->hotrect);
      cliprgn = Hv_IntersectRegion(region, hrr);
      Hv_DestroyRegion(hrr);
      Hv_SetClippingRegion(cliprgn);
  }

  while ((xp = Hv_MapPointsToPoly(View, map, &npoly, False)) != NULL) {
    if (npoly > 2) {

      if ((!Hv_extraFastPlease) && dounion && (Item != NULL)) {
	Hv_UnionPolygonWithRegion(xp, npoly, &(Item->region));
      }

      if (yesdraw) {

	if (!Hv_extraFastPlease && (color >= 0)) {
	  preg = Hv_CreateRegionFromPolygon(xp, npoly);
	  Hv_FillRegion(preg, fillcolor, cliprgn);
	  Hv_DestroyRegion(preg);
	}
	
	Hv_SetPattern(-1, Hv_black);

	if (color < 0)
	  Hv_DrawXorLines(xp, (short)npoly-1);  
	else {
	  Hv_DrawLines(xp, (short)npoly, color); 
	}

	if (changepat) 
	  Hv_SetPattern(pattern, fillcolor);
	
      }
    }  /* end npoly */
  
    Hv_Free(xp);
  }


  wp = (Hv_WorldPolygonData *)(Item->data);

  if (wp != NULL) {
	
      pp = Hv_NewPoints((int)wp->numpnts);
      Hv_LatLongPolygonToLocalPolygon(View, wp->numpnts, pp, wp->fpts);

      treg = Hv_CreateRegionFromPolygon(pp, wp->numpnts);
      
      totreg = Hv_UnionRegion(Item->region, treg);
      Hv_DestroyRegion(Item->region);
      Item->region = totreg;
      
      Hv_DestroyRegion(treg);
      if(Item->region != NULL)
	  Hv_ShrinkRegion(Item->region, -(wp->thickness+2), -(wp->thickness+2));
      Hv_Free(pp);
  }

  Hv_SetPattern(-1, Hv_black);
  Hv_DestroyRegion(cliprgn);
}


/*---------- Hv_InteriorAngle ----------*/

static float Hv_InteriorAngle(Hv_Item Item,
			      int     index) {

  float                az1, az2, openang;
  Hv_FPoint           *base;
  Hv_FPoint           *p0, *p1, *p2;
  Hv_WorldPolygonData *wp;
  int                 np;

/*
 * works for map views ONLY
 */

  if (!Hv_IsMapView(Item->view) || (Item->tag != Hv_DRAWINGTOOLTAG)) {
    return 0.0;
  }

  wp = (Hv_WorldPolygonData *)(Item->data);
      
  if ((index < 0) || (index >= wp->numpnts)) {
    fprintf(stderr, "Bad index in Hv_InteriorAngle: %d\n", index);
    return 0.0;
  }

  np = wp->numpnts;
  base = wp->fpts;

  p0 = wp->fpts+index;

  if (index == 0)
    p1 = wp->fpts+(np-1);
  else
    p1 = wp->fpts+(index-1);

  if (index == (np-1))
    p2 = wp->fpts;
  else
    p2 = wp->fpts+(index+1);


/* NOTE for map views fpts are ALREADY in lat (h) and long (v) */


  az1 = Hv_GetAzimuth(p0->h,
		      p0->v,
		      p1->h,
		      p1->v);

  az2 = Hv_GetAzimuth(p0->h,
		      p0->v,
		      p2->h,
		      p2->v);

  openang = fabs(az2 - az1);

  if (openang > Hv_PI)
    openang -= Hv_PI;

fprintf(stderr, "opening ang = %f\n", Hv_RADTODEG*openang);

  return openang;
}


/*---------- Hv_GetRRP --------*/

static void Hv_GetRRP(Hv_Item Item) {

/*
 * Compute the rotation reference point (RRP)
 * for this item.
 */

  Hv_View               View;
  Hv_WorldPolygonData  *wp;
  float                 latitude, longitude;
  short                 x, y;

  if (Item == NULL)
    return;

  View = Item->view;

  if (!Hv_IsMapView(View) || (Item->tag != Hv_DRAWINGTOOLTAG))
    return;

  wp = (Hv_WorldPolygonData *)(Item->data);

  Hv_LatLongToLocal(Item->view,
		    &(Item->rotaterp->x), &(Item->rotaterp->y),
		    wp->basepoint.h, wp->basepoint.v);

/* the rotate rect should be placed at the proper azimuth, at a
   distance based on the max distance from the base point */

  wp->basepointrad = 0.5*Hv_FarthestPoint(Item);
  
  Hv_OffsetLatLong(wp->basepoint.h, wp->basepoint.v, 
		   wp->basepointrad,
		   Item->azimuth,
		   &latitude, &longitude);

  Hv_LatLongToLocal(Item->view, &x, &y,
		    latitude, longitude);

  Hv_SetRect(Item->rotaterect, x-3, y-3, 7, 7);

}



/*----------- Hv_FarthestPoint -------*/

static float Hv_FarthestPoint(Hv_Item Item) {

  Hv_View View;
  int     i, np;
  Hv_WorldPolygonData  *wp;
  float   d = 0.0;
  Hv_FPoint  *fp;

  if (Item == NULL)
    return 0.0;

  View = Item->view;

  if (!Hv_IsMapView(View) || (Item->tag != Hv_DRAWINGTOOLTAG))
    return 0.0;

  wp = (Hv_WorldPolygonData *)(Item->data);
  np = wp->numpnts;
  fp = wp->fpts;

/* recall that the 1st point is the base */

  for (i = 0; i < np; i++) {
    d = Hv_fMax(d, Hv_GCDistance(wp->basepoint.h, wp->basepoint.v,
				 fp[i].h, fp[i].v));
  }

  
  
  return d;
}
 
/*------- Hv_CopyPolygonItem -------*/

static Hv_Item Hv_CopyPolygonItem(Hv_View View,
				  Hv_Item Item) {
    
    Hv_Item               newItem;
    Hv_FPoint            *fp;
    Hv_WorldPolygonData  *wp;
    Hv_WorldPolygonData  *newwp;
    int                   i;
    Hv_Item               parent;
    Hv_Map                map;

/*
  note, View is the destination view, which need not
  be the same as Item->view (Item is the item being copied)
*/


    wp = (Hv_WorldPolygonData *)(Item->data);


    if (View == Item->view)
	parent = Item->parent;
    else
	parent = NULL;

    fp = Hv_NewFPoints(wp->numpnts);

    for (i = 0; i < wp->numpnts; i++) {
	fp[i].h = wp->fpts[i].h;
	fp[i].v = wp->fpts[i].v;
    }


    newItem = Hv_VaCreateItem(View,
			      Hv_PARENT,       parent,
			      Hv_TAG,          Item->tag,
			      Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			      Hv_NUMPOINTS,    wp->numpnts,
			      Hv_DATA,         fp,
			      Hv_NUMROWS,      wp->nrows,
			      Hv_NUMCOLUMNS,   wp->ncols,
			      Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			      Hv_DRAWCONTROL,  Item->drawcontrol,
			      Hv_AFTERDRAG,    Hv_PolygonAfterDrag,
			      Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			      Hv_FIXREGION,    Hv_FixPolygonRegion,
			      Hv_CHECKENLARGE, Hv_CheckWorldPolygonEnlarge,
			      NULL);

    newwp = (Hv_WorldPolygonData *)(newItem->data);
	
    newwp->thickness   = wp->thickness;
    newwp->style       = wp->style;
    newwp->arrowstyle  = wp->arrowstyle;
    newwp->fillcolor   = wp->fillcolor;
    newwp->symbolcolor = wp->symbolcolor;
    newwp->symbol      = wp->symbol;
    newwp->symbolsize  = wp->symbolsize;
    newwp->grid        = wp->grid;
    newwp->closed      = wp->closed;
    newwp->showpoints  = wp->showpoints;
    newwp->showlines   = wp->showlines;

    newItem->azimuth = Item->azimuth;

    if (Hv_IsMapView(View) && !(newwp->closed)) {
	map = (Hv_Map)(newwp->map);
	Hv_DestroyMap(&map);
	newwp->map = NULL;
    }

    newItem->fixregion(newItem);

    return newItem;
}



#define DEFCOPYOFFSET 10

/*-------- Hv_PolygonItemSingleClick --------------*/


static void Hv_PolygonItemSingleClick(Hv_Event hvevent)
    
/************************************************
  Mostly this just calls the default single
  click CM, UNLESS the user has pressed SHIFT
  AND andother mod key, in which case this
  duplicates the polygon item.
  ************************************************/
    
{
    Boolean               shift;
    Boolean               ctrl;
    Boolean               alt;
    Hv_Item               Item = hvevent->item;
    Hv_Item               newItem;
    short                 xoffset, yoffset;
    static Hv_Item        lastItem = NULL;
    static int            count = 0;
    int                   xcnt;
    Hv_WorldPolygonData  *wp;

    shift = Hv_CheckBit(hvevent->modifiers, Hv_SHIFT);
    ctrl  = Hv_CheckBit(hvevent->modifiers, Hv_CONTROL);
    alt   = Hv_CheckBit(hvevent->modifiers, Hv_MOD1);

    
    if ((Item->tag == Hv_DRAWINGTOOLTAG) && shift && (ctrl || alt)) {

	wp = (Hv_WorldPolygonData *)(Item->data);

	newItem = Hv_CopyPolygonItem(Item->view, Item);

/* used lastItem to cause repeated duplicates to be offset by greater amounts */
	
	if (lastItem != Item)
	    count = 0;
	else
	    count++;
	
	if (count >= 200)
	    count = 0;
	
	xcnt = count;
	if (xcnt > 99)
	    xcnt -= 100;
	
	xoffset = ((xcnt % 10) + 1 )*DEFCOPYOFFSET;
	xoffset += 4*(xcnt/10)*DEFCOPYOFFSET;
	
	yoffset = ((count % 10) + 1 )*DEFCOPYOFFSET;
	
	if (count > 99)
	    yoffset = -yoffset;

	Hv_offsetDueToDrag = True;
	Hv_OffsetItem(newItem, xoffset, yoffset, True);
	Hv_offsetDueToDrag = False;
	

/*===
 * see if the user wants to be notified
 *===*/

	if ((Item->tag == Hv_DRAWINGTOOLTAG) && (Hv_CreateDrawingItemCB != NULL))
	  Hv_CreateDrawingItemCB(newItem);

	Hv_DrawItem(newItem, NULL);
	lastItem = Item;

    }
    else
	Hv_DefaultSingleClickCallback(hvevent);
    
}

#undef DEFCOPYOFFSET




E 1
