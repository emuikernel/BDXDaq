/**
 * <EM>Deals with world coordinate based lines. </EM>
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
#include "Hv_xyplot.h"


/*------ local prototypes ------*/

static void  Hv_GetRegularArrowPolygon(Hv_Item   Item,
				       Hv_Point  *poly,
				       int       *np,
				       short      arrowstyle);


static void Hv_EditWorldLineColor (Hv_Widget  w);

static void  Hv_GetArrowPolygon(Hv_Item   Item,
				Hv_Point  *poly,
				int       *np,
				int       multiplex);

static Boolean  Hv_AnyActiveMUX(Hv_Item  Item);

/*------ variables global for this file --------*/

static   short           newlinecolor;
static   Hv_Widget       linecolorlabel;


/**
 * Hv_EditWorldLineItem
 * @purpose Edit the visual parameters of the line
 * @param   hvevent   The initiating event, probably a double click on the rectangle.
 */

void   Hv_EditWorldLineItem(Hv_Event hvevent) {

  Hv_Item                 Item = hvevent->item;
  static Hv_Widget        dialog = NULL;
  static Hv_Widget        thickness;
  Hv_Widget               rowcol, rc, dummy;
  Hv_WorldLineData       *wline;

  static Hv_Widget        arrowbutton;
  static Hv_Widget        stylebutton;

  int                     answer;

  static int              num_abuttons = -1;
  static Hv_WidgetList    abuttons;

  static int              num_sbuttons = -1;
  static Hv_WidgetList    sbuttons;

  if ((Item == NULL) || (Item->type != Hv_WORLDLINEITEM))
    return;

  wline = (Hv_WorldLineData *)(Item->data);

  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Line Editor ", NULL);
    rowcol = Hv_DialogColumn(dialog, 6);
    rc = Hv_TightDialogRow(rowcol, 6);

    linecolorlabel = Hv_SimpleColorLabel(rc,
					 "Line color",
					 (Hv_FunctionPtr)Hv_EditWorldLineColor);

    thickness = Hv_SimpleTextEdit(rc, "Line width", NULL, 4);

    dummy = Hv_SimpleDialogSeparator(rowcol);

    stylebutton = Hv_CreateLineStylePopup(rowcol, Hv_fixed2);    /* line style menu */
    arrowbutton = Hv_CreateArrowStylePopup(rowcol, Hv_fixed2);   /* arrow style menu */

    dummy = Hv_StandardActionButtons(rowcol, 10, Hv_ALLACTIONS);

/* for use in history */

    Hv_GetOptionMenuButtons(stylebutton,  &sbuttons, &num_sbuttons);
    Hv_GetOptionMenuButtons(arrowbutton,  &abuttons, &num_abuttons);
  }

/* the dialog has been created */

  Hv_SetDeleteItem(dialog, Item);

  Hv_hotArrowStyle = wline->arrowstyle;
  Hv_hotStyle  = wline->style;

  Hv_SetOptionMenu(stylebutton,  sbuttons, (int)Hv_hotStyle);
  Hv_SetOptionMenu(arrowbutton,  abuttons, (int)Hv_hotArrowStyle);

  newlinecolor = Item->color;
  Hv_SetIntText(thickness, wline->thickness);
  Hv_ChangeLabelColors(linecolorlabel, -1, Item->color);

  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {

    wline->arrowstyle = Hv_hotArrowStyle;
    wline->style = Hv_hotStyle;
    
    Item->color  = newlinecolor;
    wline->thickness = Hv_sMax(1, Hv_sMin(6, (short)Hv_GetIntText(thickness)));
    
    Hv_StandardRedoItem(Item);

    if (answer != Hv_APPLY)
      break;
  }
}


/* ------- Hv_WorldLineButtonCallback -------*/

void Hv_WorldLineButtonCallback(Hv_Event hvevent)

{
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  Hv_Item              newItem;
  static Hv_Cursor        tempcursor;
  Hv_Point             StartPP, StopPP;
  Hv_Rect              limitrect;
  float                xstart, ystart, xstop, ystop;
  Hv_Item              parent = NULL;
  Hv_WorldLineData    *wline;

  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent, Hv_WorldLineButtonCallback, &activeView, &activeItem, &tempcursor);
    return;
  }
  
  if (View != activeView) {
    Hv_Warning("World Line selector active on another view!");
    return;
  }

  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

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
      
      Hv_CreateLine(limitrect, StartPP, &StopPP);
      if ((abs(StopPP.x-StartPP.x) > 1) || (abs(StopPP.y-StartPP.y) > 1)) {
	Hv_LocalToWorld(View, &xstart, &ystart, StartPP.x,  StartPP.y);
	Hv_LocalToWorld(View, &xstop,  &ystop,  StopPP.x,   StopPP.y);
	newItem = Hv_VaCreateItem(View,
				  Hv_PARENT,       parent,
				  Hv_TAG,          Hv_DRAWINGTOOLTAG,
				  Hv_TYPE,         Hv_WORLDLINEITEM,
				  Hv_STARTX,       xstart,
				  Hv_STOPX,        xstop,
				  Hv_STARTY,       ystart,
				  Hv_STOPY,        ystop,
				  Hv_DOMAIN,       Hv_INSIDEHOTRECT,
				  Hv_DRAWCONTROL,  Hv_ENLARGEABLE + Hv_ZOOMABLE + Hv_FANCYDRAG,
				  Hv_FIXREGION,    Hv_FixLineRegion,
				  Hv_CHECKENLARGE, Hv_CheckWorldLineEnlarge,
				  NULL);

	if (Hv_usedNewDrawTools) {
	  wline = (Hv_WorldLineData *)(newItem->data);
	  wline->style = View->hotlinestyle;
	  wline->arrowstyle = View->hotarrowstyle;
	  newItem->color = View->hotbordercolor;
	  wline->thickness = View->hotlinewidth;
	  Hv_usedNewDrawTools = False;
	}

	hvevent->item = newItem;


/*===
 * see if the user wants to be notified
 *===*/

	if (Hv_CreateDrawingItemCB != NULL)
	  Hv_CreateDrawingItemCB(newItem);

	Hv_DrawItem(newItem, NULL); 
      }
    }
}


/*------- Hv_DrawWorldLineButton -------*/

void Hv_DrawWorldLineButton(Hv_Item    Item,
			    Hv_Region   region)

/* draws the icon for world line tool */

{
  Hv_Rect          area;
  short            l, r, yc;

  Hv_ButtonDrawingArea(Item, &area);
  yc = (area.top + area.bottom)/2;
  l = area.left+2;
  r = area.right-2;

  Hv_DrawLine(l, (short)(yc-1), r, (short)(yc-1), Hv_black);
  Hv_DrawLine(l, yc,   r, yc,   Hv_lavender);
  Hv_DrawLine(l, (short)(yc+1), r, (short)(yc+1), Hv_white);
  Hv_DrawLine(l, (short)(yc+2), r, (short)(yc+2), Hv_black);
}


/* -------- Hv_DrawWorldLineItem --------------- */

void Hv_DrawWorldLineItem(Hv_Item       Item,
			  Hv_Region     region)

/*draws the specified Item Hv_WORLDLINEITEM*/

{
  Hv_View           View = Item->view;
  Hv_Point          p1, p2;
  Hv_WorldLineData  *wline;
  Hv_Point          poly[8];
  int               np;
  int               i;
  Boolean           omp;


  wline = (Hv_WorldLineData *)(Item->data);
  
  if (Item->fixregion != NULL)
    Item->fixregion(Item);
  
  if (!Hv_extraFastPlease)
    Hv_SetLineStyle(wline->thickness, wline->style);
  
  Hv_WorldRectToLocalRect(View, Item->area, Item->worldarea);
  
  Hv_GetLineCorner(wline->start, Item->area, &p1);
  Hv_GetLineCorner(wline->stop,  Item->area, &p2);

/* for some lines, we don't want to draw them if EITHER point is out of
   the hotrect */

  
  if (Hv_worldLinePolicy == Hv_ONEPOINTINHOTRECT) {
    if (!Hv_PointInRect(p1, View->hotrect) && !Hv_PointInRect(p2, View->hotrect))
      return;
  }
  else if (Hv_worldLinePolicy == Hv_BOTHPOINTSINHOTRECT) {
    if (!Hv_PointInRect(p1, View->hotrect) || !Hv_PointInRect(p2, View->hotrect))
      return;
  } 

  if (!Hv_extraFastPlease) {
    if (View->drawarrowheads && (wline->arrowstyle != Hv_NOARROW)) {

/* the mutilplexing is used for multiple connections
    between items */

      if (wline->multiplexed) {
	omp = False;

	for (i = 1; i <= 4; i++) { 
	  if ((wline->multiplexcolors[i] > -1) && (Hv_CheckBitPosition(wline->multiplexbits, i))) {
	    omp = True;
	    Hv_GetArrowPolygon(Item, poly, &np, i);
	    if (np > 2)
	      Hv_FillPolygon(poly, (short)np, False, wline->multiplexcolors[i], Hv_black);
	  }
	} 
      }  /* end multiplexed */
      else {

/* this is a little ugly here because FIRST the multiplexing
   was added (for connections) and then the normal arrows. The
   normal arrows have styles starting at 100 */

	if (wline->arrowstyle >= Hv_ARROWATEND) {

	  if (wline->arrowstyle == Hv_ARROWATBOTH) {
	    Hv_GetRegularArrowPolygon(Item, poly, &np, Hv_ARROWATEND);
	    Hv_FillPolygon(poly, (short)np, False, Item->color, Hv_black);
	    Hv_GetRegularArrowPolygon(Item, poly, &np, Hv_ARROWATSTART);
	    Hv_FillPolygon(poly, (short)np, False, Item->color, Hv_black);
	  }
	  else {
	    Hv_GetRegularArrowPolygon(Item, poly, &np, wline->arrowstyle);
	    Hv_FillPolygon(poly, (short)np, False, Item->color, Hv_black);
	  }
	}
	else {
	  Hv_GetArrowPolygon(Item, poly, &np, 0);
	  if (np > 2)
	    Hv_FillPolygon(poly, (short)np, False, wline->arrowcolor, Hv_black);
	}
      }
    }
/* if multiplexed but no pos colors dont draw */

    if (!(wline->multiplexed) || omp) 
      Hv_DrawLine(p1.x, p1.y, p2.x, p2.y, Item->color);
  }
  
  else
    Hv_DrawLine(p1.x, p1.y, p2.x, p2.y, Hv_yellow);
  
  if (!Hv_extraFastPlease)
    Hv_SetLineStyle(0, Hv_SOLIDLINE);
}


/*------- Hv_FixLineRegion -------*/

void       Hv_FixLineRegion(Hv_Item Item)

{
  Hv_View              View = Item->view;
  Hv_WorldLineData    *wline;
  Hv_FPoint            fp1, fp2;
  short                x1, x2, y1, y2;
  Hv_Point             poly[8];
  Hv_Region            polyreg = NULL;
  int                  np, i;

  Hv_Region            hrr = NULL;
  Hv_Region            treg = NULL;

  Hv_DestroyRegion(Item->region);
  Item->region = NULL;


  wline = (Hv_WorldLineData *)(Item->data);

  if ((wline->multiplexed) && !(Hv_AnyActiveMUX(Item)))
    return;

  Hv_GetFLineCorner(wline->start, Item->worldarea, &fp1);
  Hv_GetFLineCorner(wline->stop,  Item->worldarea, &fp2);

  Hv_WorldToLocal(View, fp1.h, fp1.v, &x1, &y1);
  Hv_WorldToLocal(View, fp2.h, fp2.v, &x2, &y2);
  
  Item->region =  Hv_CreateRegionFromLine(x1, y1, x2, y2, (short)(wline->thickness+1));

  if (wline->arrowstyle > 0) {

    if (wline->multiplexed) {
      for (i = 1; i <= 4; i++) 
	if ((wline->multiplexcolors[i] > -1) && (Hv_CheckBitPosition(wline->multiplexbits, i))) {
	  Hv_GetArrowPolygon(Item, poly, &np, i);
	  if (np > 2) {
	    polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
	    Hv_AddRegionToRegion(&(Item->region), polyreg);
	    Hv_DestroyRegion(polyreg);
	  }
	} 
    } /* end multiplexed */
    else {
      if (wline->arrowstyle >= Hv_ARROWATEND) {
	if (wline->arrowstyle == Hv_ARROWATBOTH) {
	  Hv_GetRegularArrowPolygon(Item, poly, &np, Hv_ARROWATEND);
	  polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
	  Hv_AddRegionToRegion(&(Item->region), polyreg);
	  Hv_DestroyRegion(polyreg);
	  Hv_GetRegularArrowPolygon(Item, poly, &np, Hv_ARROWATSTART);
	  polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
	  Hv_AddRegionToRegion(&(Item->region), polyreg);
	  Hv_DestroyRegion(polyreg);
	}
	else {
	  Hv_GetRegularArrowPolygon(Item, poly, &np, wline->arrowstyle);
	  polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
	  Hv_AddRegionToRegion(&(Item->region), polyreg);
	  Hv_DestroyRegion(polyreg);
	}
      }
      else {
	Hv_GetArrowPolygon(Item, poly, &np, 0);
	if (np > 2) {
	  polyreg = Hv_CreateRegionFromPolygon(poly, (short)np);
	  Hv_AddRegionToRegion(&(Item->region), polyreg);
	  Hv_DestroyRegion(polyreg);
	}
      }


    }
  }
  
  if (Item->region != NULL) {
    hrr = Hv_HotRectRegion(View);
    treg = Hv_IntersectRegion(hrr, Item->region);
    Hv_DestroyRegion(hrr);
    Hv_DestroyRegion(Item->region);
    Item->region = treg;
  }
  
}


/* -------  Hv_WorldLineInitialize --------*/

void Hv_WorldLineInitialize(Hv_Item      Item,
			    Hv_AttributeArray attributes)
{
  Hv_WorldLineData     *wline;
  float                startx, stopx, starty, stopy;
  int                  i;
  
/* since this routine is also used for CONNECTIONITEM, must be carefule */

  if (Item->type == Hv_WORLDLINEITEM) {
    Item->standarddraw = Hv_DrawWorldLineItem;
    wline = (Hv_WorldLineData *)Hv_Malloc(sizeof(Hv_WorldLineData));
    Item->data = (void *)wline;
    wline->thickness = 2;
    wline->arrowstyle = Hv_NOARROW;
    wline->multiplexed = False;
    wline->multiplexbits = 0;
    for (i = 0; i < 5; i++)
      wline->multiplexcolors[i] = Hv_black;

/* added in 10041 -- use standard enlarge default if none provided */
    
    if (attributes[Hv_CHECKENLARGE].v == NULL)
      Item->checkenlarge = (Hv_FunctionPtr)Hv_CheckWorldLineEnlarge;
    
    wline->style = Hv_SOLIDLINE;
    if (Item->doubleclick == NULL)
      Item->doubleclick = Hv_EditWorldLineItem;  /* default doubleclick is the world rect item editor */
  }

  startx = (float)(attributes[Hv_STARTX].f);
  starty = (float)(attributes[Hv_STARTY].f);
  stopx  = (float)(attributes[Hv_STOPX].f);
  stopy  = (float)(attributes[Hv_STOPY].f);
  
  Hv_SetWorldLineBasedOnEndPoints(Item, startx, starty, stopx, stopy);
  
}


/*------- Hv_SetWorldLineBasedOnEndPoints -------*/

void Hv_SetWorldLineBasedOnEndPoints(Hv_Item   Item,
				     float    startx,
				     float    starty,
				     float    stopx,
				     float    stopy)

{
  float              xmin, xmax, ymin, ymax;
  Hv_WorldLineData   *wline;

  wline = (Hv_WorldLineData *)(Item->data);

  xmin = Hv_fMin(startx, stopx);
  xmax = Hv_fMax(startx, stopx);
  ymin = Hv_fMin(starty, stopy);
  ymax = Hv_fMax(starty, stopy);
  
  if (Item->worldarea == NULL)
    Item->worldarea = Hv_NewFRect();
  
  Hv_SetFRect(Item->worldarea, xmin, xmax, ymin, ymax);
  
  wline->start = Hv_WhichPointOnFLine(startx, starty, Item->worldarea);
  wline->stop  = Hv_WhichPointOnFLine(stopx,  stopy,  Item->worldarea);

  Hv_WorldRectToLocalRect(Item->view, Item->area, Item->worldarea);

  if (Item->fixregion != NULL)
    Item->fixregion(Item);
}


/*------- Hv_WhichPointOnFLine ----*/

short Hv_WhichPointOnFLine(float   x,
			   float   y,
			   Hv_FRect  *wr)
{

  if (fabs(x - wr->xmin) < fabs(x - wr->xmax)) { /* closer to xmin, either 0 or 3 */
    if (fabs(y - wr->ymin) < fabs(y - wr->ymax))
      return Hv_LOWERLEFT;
    return Hv_UPPERLEFT;
  }
  else {  /* closer to xmax, either 1 or 2 */
    if (fabs(y - wr->ymin) < fabs(y - wr->ymax))
      return Hv_LOWERRIGHT;
    return Hv_UPPERRIGHT;
  }
  
}

/*------ Hv_GetFLineCorner ---------*/

void Hv_GetFLineCorner(short   corner,
		       Hv_FRect *wr,
		       Hv_FPoint *fp)

{
  if ((corner ==  Hv_UPPERRIGHT) || (corner == Hv_LOWERRIGHT))
    fp->h = wr->xmax;
  else
    fp->h = wr->xmin;

  if ((corner ==  Hv_UPPERLEFT) || (corner == Hv_UPPERRIGHT))
    fp->v = wr->ymax;
  else
    fp->v = wr->ymin;

}


/*------- Hv_WhichPointOnLine ----*/

short Hv_WhichPointOnLine(short   x,
			  short   y,
			  Hv_Rect  *r)
{

/* returns closest point on a rect surrounding a line  */

  if (abs(x - r->left) < abs(x - r->right)) { /* closer to left, either 0 or 3 */
    if (abs(y - r->bottom) < abs(y - r->top))
      return Hv_LOWERLEFT;
    return Hv_UPPERLEFT;
  }
  else {  /* closer to right, either 1 or 2 */
    if (abs(y - r->bottom) < abs(y - r->top))
      return Hv_LOWERRIGHT;
    return Hv_UPPERRIGHT;
  }
  
}


/*------ Hv_GetLineCorner ---------*/

void Hv_GetLineCorner(short   corner,
		      Hv_Rect  *r,
		      Hv_Point *p)

/* returns the corner of the rect which is an
   endpoint of a line */

{

  if ((corner ==  Hv_UPPERRIGHT) || (corner == Hv_LOWERRIGHT))
    p->x = r->right;
  else
    p->x = r->left;

  if ((corner ==  Hv_UPPERLEFT) || (corner == Hv_UPPERRIGHT))
    p->y = r->top;
  else
    p->y = r->bottom;
}


/* ------ Hv_EditWorldLineColor --- */

static void Hv_EditWorldLineColor (Hv_Widget w)

{
  Hv_ModifyColor(w, &newlinecolor, "Edit Line Color", False);
}



/*------- Hv_CheckWorldLineEnlarge -----------*/

void Hv_CheckWorldLineEnlarge(Hv_Item  Item,
			      Hv_Point  pp,
			      Boolean  Shifted,
			      Boolean  *enlarged)
{
  Hv_View           View = Item->view;
  Hv_Point          p1, p2;
  Hv_WorldLineData  *wline;
  Hv_Rect           limitrect, tempr1, tempr2;
  short             slop, slop2;
  Hv_Region         udr1, hrr;
  float             newx, newy;
  short             fixedcorner;
  Hv_FPoint         fixedfp;

  *enlarged = False;

  if (Item->type != Hv_WORLDLINEITEM)
    return;

  if (!Hv_CheckBit(Item->drawcontrol, Hv_ENLARGEABLE))
    return;

  wline = (Hv_WorldLineData *)(Item->data);
  slop2 = wline->thickness + 2;
  slop = 2*slop2;

  Hv_GetLineCorner(wline->start, Item->area, &p1);
  Hv_GetLineCorner(wline->stop,  Item->area, &p2);
  Hv_SetRect(&tempr1, (short)(p1.x-slop2), (short)(p1.y-slop2), slop, slop);
  Hv_SetRect(&tempr2, (short)(p2.x-slop2), (short)(p2.y-slop2), slop, slop);
  Hv_CopyRect(&limitrect, View->hotrect);


  if (Hv_PointInRect(pp, &tempr1)) {
    Hv_ShrinkRect(&tempr1, wline->thickness, wline->thickness);
    Hv_FillRect(&tempr1, Hv_peachPuff);
    Hv_FrameRect(&tempr1, Hv_brown);
    fixedcorner = wline->stop;
    hrr = Hv_ClipHotRect(View, 0);
    Hv_EnlargeLine(limitrect, pp, &p2, 1, &p1);
    Hv_LocalToWorld(View, &newx, &newy, p1.x, p1.y);
  }
  else if (Hv_PointInRect(pp, &tempr2)) {
    Hv_ShrinkRect(&tempr2, wline->thickness, wline->thickness);
    Hv_FillRect(&tempr2, Hv_peachPuff);
    Hv_FrameRect(&tempr2, Hv_brown);
    fixedcorner = wline->start;
    hrr = Hv_ClipHotRect(View, 0);
    Hv_EnlargeLine(limitrect, pp, &p1, 1, &p2);
    Hv_LocalToWorld(View, &newx, &newy, p2.x, p2.y);
  }
  else 
    return;

  Hv_DestroyRegion(hrr);


/* if made it this far, must actually modify the line ! */

  udr1 = Hv_CopyItemRegion(Item, False);    /* original region */
  Hv_ShrinkRegion(udr1, -2, -2);

  Hv_GetFLineCorner(fixedcorner, Item->worldarea, &fixedfp);

  
  if (fixedcorner == wline->start)
    Hv_SetWorldLineBasedOnEndPoints(Item, fixedfp.h, fixedfp.v, newx, newy);
  else
    Hv_SetWorldLineBasedOnEndPoints(Item, newx, newy, fixedfp.h, fixedfp.v);
  

  Hv_ItemChangedRedraw(Item, udr1);
  Hv_DestroyRegion(udr1); 

  *enlarged = True;
}

/*--------- Hv_GetArrowPloygon -----*/

static void  Hv_GetArrowPolygon(Hv_Item  Item,
				Hv_Point  *poly,
				int    *np,
				int    multiplex)
{
  Hv_WorldLineData   *wline;
  Hv_Point           p1, p2, pc;
  Hv_FPoint          fp1, fp2, R, fpts[8];
  float              theta;
  Hv_View            View = Item->view;
  float              fract = 0.5;
  int                i;

  wline = (Hv_WorldLineData *)(Item->data);
  *np = 0;
  
  if (wline->arrowstyle < 1)
    return;
  
  if (wline->multiplexed) {
    if ((multiplex < 1) || (multiplex > 4))
      return;
    if (!Hv_CheckBitPosition(wline->multiplexbits, multiplex))
      return;
  }
  else {
    if (multiplex != 0)
      return;
  }
  
  if (multiplex == 0)
    fract = (float)(0.99);
  else if (multiplex == 1)
    fract = (float)(0.2);
  else if (multiplex == 2)
    fract = (float)(0.8);
  else if (multiplex == 4)
    fract = (float)(0.65);
  else
    fract = (float)(0.49);
  
  Hv_GetLineCorner(wline->start, Item->area, &p1);
  Hv_GetLineCorner(wline->stop,  Item->area, &p2);
    

/* fp1 is the "start" point */
    
  Hv_LocalToWorld(View, &(fp1.h), &(fp1.v), p1.x,  p1.y);
  Hv_LocalToWorld(View, &(fp2.h), &(fp2.v), p2.x,  p2.y);


  R.h = fp2.h - fp1.h;
  R.v = fp2.v - fp1.v;

  Hv_WorldToLocal(View, fp1.h + fract*R.h, fp1.v + fract*R.v, &(pc.x),  &(pc.y));
  
  if ((fabs(R.h) < 1.0e-20) && (fabs(R.v) < 1.0e-20))
    theta = 0.0;
  else
    theta = (float)(atan2(R.v, R.h));
  
  *np = 3;
  Hv_SetPoint(poly,   pc.x, pc.y);
  Hv_SetPoint(poly+1, (short)(pc.x-10), (short)(pc.y-6));
  Hv_SetPoint(poly+2, (short)(pc.x-10), pc.y);

  
  Hv_LocalPolygonToWorldPolygon(View, (short)(*np), poly, fpts);
    

  for (i = 1; i < *np; i++) {
    fpts[i].h -= fpts[0].h;
    fpts[i].v -= fpts[0].v;
  }

  Hv_RotateFPoints(theta, fpts+1, (short)(*np-1));
  
  for (i = 1; i < *np; i++) {
    fpts[i].h += fpts[0].h;
    fpts[i].v += fpts[0].v;
  }

    
  Hv_WorldPolygonToLocalPolygon(View, (short)(*np), poly, fpts); 
}


/*--------- Hv_GetRegularArrowPloygon -----*/

static void  Hv_GetRegularArrowPolygon(Hv_Item   Item,
				       Hv_Point  *poly,
				       int       *np,
				       short     style)     
/*****************************************************
  This routine is used for normal |drawing tool" lines.
  Its cousin Hv_GetArrowPolygon is for connections.
******************************************************/

{
  Hv_WorldLineData   *wline;
  Hv_FPoint          fp1, fp2;
  Hv_View            View = Item->view;

  wline = (Hv_WorldLineData *)(Item->data);
  *np = 0;
  
  if (style < Hv_ARROWATEND)
    return;

  *np = 4;

  if (style == Hv_ARROWATEND) {
    Hv_GetFLineCorner(wline->start, Item->worldarea, &fp1);
    Hv_GetFLineCorner(wline->stop,  Item->worldarea, &fp2);
  }
  else { /* arrow at start */
    Hv_GetFLineCorner(wline->stop,   Item->worldarea, &fp1);
    Hv_GetFLineCorner(wline->start,  Item->worldarea, &fp2);
  }


  Hv_ArrowPolygonFromWorldLine(View, &fp1, &fp2, wline->thickness, poly);
}


/*------ Hv_AnyActiveMUX ------*/

static Boolean    Hv_AnyActiveMUX(Hv_Item Item)

{
  Hv_WorldLineData  *wline;
  int               i;

  wline = (Hv_WorldLineData *)(Item->data);

  for (i = 1; i <= 4; i++) 
    if ((wline->multiplexcolors[i] > -1) && (Hv_CheckBitPosition(wline->multiplexbits, i)))
      return True;
  
  
  return False;
}






