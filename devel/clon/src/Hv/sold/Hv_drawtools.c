/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"

#define   Hv_DTWIDTH     25
#define   Hv_DTHEIGHT    24

/*-------- local prototypes -------*/

static short Hv_GetMBOffset(Hv_Item Item,
			 short choice);


static Hv_Item Hv_AddDTMB(Hv_View View,
			  short nr,
			  short nc,
			  short choice,
			  char *btext,
			  short mbw,
			  short mbh,
			  short icw,
			  short ich,
			  short placement,
			  short fcolor,
			  short rspolicy,
			  Boolean useactivearea,
			  Boolean tandem,
			  Hv_FunctionPtr IconDraw,
			  Hv_FunctionPtr CB,
			  Hv_FunctionPtr MenuCB);

static void Hv_FontCB(Hv_Event hvevent);

static void Hv_FontDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect);

static void Hv_ShapeCB(Hv_Event hvevent);

static void Hv_ShapeDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect);

static void Hv_FillDraw(Hv_Item Item,
			short   choice,
			Hv_Rect *rect);

static void Hv_BorderDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect);

static void Hv_LineStyleDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect);

static void Hv_LineWidthDraw(Hv_Item Item,
			     short   choice,
			     Hv_Rect *rect);

static void Hv_ArrowStyleDraw(Hv_Item Item,
			      short   choice,
			      Hv_Rect *rect);


static void Hv_PatternDraw(Hv_Item Item,
			   short   choice,
			   Hv_Rect *rect);

static void Hv_FontMenuCB(Hv_Item  Item,
			  short    choice);

static void Hv_ShapeMenuCB(Hv_Item  Item,
			   short    choice);

static void Hv_FillMenuCB(Hv_Item  Item,
			  short    choice);

static void Hv_BorderMenuCB(Hv_Item  Item,
			    short    choice);

static void Hv_LineStyleMenuCB(Hv_Item  Item,
			       short    choice);

static void Hv_LineWidthMenuCB(Hv_Item  Item,
			       short    choice);

static void Hv_ArrowStyleMenuCB(Hv_Item  Item,
				short    choice);

static void Hv_PatternMenuCB(Hv_Item  Item,
			     short    choice);

/*--------- Hv_AddDrawingTools ---------*/

void   Hv_AddDrawingTools(Hv_View  View,
			  short    left,
			  short    top,
			  short    orientation,
			  short    resizepolicy,
			  short    fillcolor,
			  Boolean  cameratoo,
			  Hv_Item *first,
			  Hv_Item *last)

/******************************************
  Upgrade of Hv_StandardTools that creates the 
  menubutton based drawing tools. If cameratoo
  is True, the camera button is added first.
******************************************/

{
  short       placement;
  Hv_Item     Item = NULL;
  short       cw, ch;

/* if fill color is default, meake it the standard gray */

  if (fillcolor < Hv_red)
    fillcolor = Hv_gray11;

  if (orientation == Hv_VERTICAL) {
    placement = Hv_POSITIONBELOW;
    cw = Hv_DTWIDTH  + 6;
    ch = Hv_DTHEIGHT + 4;
  }
  else {
    placement = Hv_POSITIONRIGHT;
    cw = Hv_DTWIDTH  + 6;
    ch = Hv_DTHEIGHT + 4;
  }
  
  *first = NULL;

/* if a camera is requested, add it first */

  if (cameratoo) {
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,         Hv_BUTTONITEM,
			   Hv_SINGLECLICK,  Hv_CameraCallback,
			   Hv_USERDRAW,     Hv_DrawCamera,
			   Hv_LEFT,         left,
			   Hv_TOP,          top,
			   Hv_WIDTH,        cw,
			   Hv_HEIGHT,       ch,
			   Hv_RESIZEPOLICY, resizepolicy,
			   Hv_FILLCOLOR,    fillcolor,
			   Hv_BALLOON,      "Use this \"camera\" to print this view.",
			   NULL);
  
    *first = Item;
  }

/* now add the drawing tools as menu buttons */

  Item = Hv_AddDTMB(View, 8, 5, View->hotfont - Hv_fixed,
		    "Use this put text on the HotRect and to set the default font.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH, Hv_DTHEIGHT, placement, fillcolor, resizepolicy, True, True,
		    (Hv_FunctionPtr)Hv_FontDraw,
		    (Hv_FunctionPtr)Hv_FontCB,
		    (Hv_FunctionPtr)Hv_FontMenuCB);  /* font button */

  Item = Hv_AddDTMB(View, 1, 5, View->hotshape - Hv_LINESHAPE,
		    "Use this put a shape on the HotRect and to set the default shape.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH, Hv_DTHEIGHT, placement, fillcolor, resizepolicy, True, True,
		    (Hv_FunctionPtr)Hv_ShapeDraw,
		    (Hv_FunctionPtr)Hv_ShapeCB,
		    (Hv_FunctionPtr)Hv_ShapeMenuCB);  /* shape button */

  Item = Hv_AddDTMB(View, 9, 9, View->hotfillcolor - Hv_red,
		    "Use this set the default fill color.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    16, 16, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_FillDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_FillMenuCB);  /* fill button */

  Item = Hv_AddDTMB(View, 9, 9, View->hotbordercolor - Hv_red,
		    "Use this set the default border color.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    16, 16, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_BorderDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_BorderMenuCB);  /* border button */


  Item = Hv_AddDTMB(View, 2, 6, View->hotpattern - Hv_FILLSOLIDPAT,
		    "Use this set the default Pattern color.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH, Hv_DTHEIGHT, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_PatternDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_PatternMenuCB);  /* pattern button */


  Item = Hv_AddDTMB(View, 1, 4, 0,
		    "Use this set the line style.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH+12, Hv_DTHEIGHT-6, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_LineStyleDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_LineStyleMenuCB);  /* line style button */

  Item = Hv_AddDTMB(View, 1, 6, 0,
		    "Use this set the line width.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH+12, Hv_DTHEIGHT-6, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_LineWidthDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_LineWidthMenuCB);  /* line width button */


  Item = Hv_AddDTMB(View, 1, 4, 0,
		    "Use this set the arrow style.",
		    Hv_DTWIDTH, Hv_DTHEIGHT,
		    Hv_DTWIDTH+12, Hv_DTHEIGHT-6, placement, fillcolor, resizepolicy, False, False,
		    (Hv_FunctionPtr)Hv_ArrowStyleDraw,
		    NULL,
		    (Hv_FunctionPtr)Hv_ArrowStyleMenuCB);  /* arrow style button */



  *last = Item;
}


/*---------- Hv_AddDTMB -----------*/

static Hv_Item Hv_AddDTMB(Hv_View View,
			  short nr,
			  short nc,
			  short choice,
			  char *btext,
			  short mbw,
			  short mbh,
			  short icw,
			  short ich,
			  short placement,
			  short fcolor,
			  short rsp,
			  Boolean useactivearea,
			  Boolean tandem,
			  Hv_FunctionPtr IconDraw,
			  Hv_FunctionPtr CB,
			  Hv_FunctionPtr MenuCB)

{
  Hv_Item   Item;
  short     row, col;
  Boolean   spec_case = 0;

  if (mbh < 0) {
    spec_case = 1;
    mbh = -mbh;
  }
  if (mbw < 0) {
    spec_case = 2;
    mbw = -mbw;
  }


/* need to convert choice */

  if (choice < 0)
    choice = nr*nc - 1;
  
  row = choice / nc;
  col = choice % nc;
  choice = (row + 1)*256 + col + 1; 
  

  Item = Hv_VaCreateItem(View,
			 Hv_MBTANDEM,           tandem,
			 Hv_MBNOACTIVEAREA,     !useactivearea,
			 Hv_TYPE,               Hv_MENUBUTTONITEM,
			 Hv_FILLCOLOR,          fcolor,
			 Hv_MENUBUTTONSTYLE,    Hv_ICONBASED,
			 Hv_NUMROWS,            nr,
			 Hv_NUMCOLUMNS,         nc,
			 Hv_MBCOLOR,            Hv_aliceBlue,
			 Hv_ARMCOLOR,           Hv_aliceBlue,
			 Hv_ICONDRAW,           IconDraw,
			 Hv_CALLBACK,           CB,
			 Hv_MENUCHOICECB,       MenuCB,
			 Hv_CURRENTCHOICE,      choice,
			 Hv_RELATIVEPLACEMENT,  placement,
			 Hv_MBICONWIDTH,        mbw,
			 Hv_MBICONHEIGHT,       mbh,
			 Hv_ICONWIDTH,          icw,
			 Hv_ICONHEIGHT,         ich,
			 Hv_BALLOON,            btext,
			 Hv_RESIZEPOLICY,       rsp,
			 NULL);

/* check for the special cases */

  if (spec_case == 1) {
    if (placement == Hv_POSITIONBELOW)
      Item->area->bottom -= (Item->area->height/2);
    else
      Item->area->top += (Item->area->height/2);
    Hv_FixRectWH(Item->area);
  }

  else if (spec_case == 2) {
    if (placement == Hv_POSITIONBELOW)
      Item->area->bottom -= (Item->area->height/2);
    else {
      Item->area->top = Item->prev->prev->area->top;
      Item->area->left = Item->prev->area->left;
      Item->area->right = Item->prev->area->right;
      Item->area->bottom = Item->prev->area->top;
    }
    Hv_FixRectWH(Item->area);
  }

  return Item;
}

/*--------- Hv_ShapeCB --------*/

static void Hv_ShapeCB(Hv_Event hvevent)
     
{

/* just redirect to one of the original
drawing tools callbacks */

  Hv_usedNewDrawTools = True;
  switch (hvevent->view->hotshape) {

  case Hv_RECTSHAPE:
    Hv_WorldRectButtonCallback(hvevent);
    break;

  case Hv_LINESHAPE:
    Hv_WorldLineButtonCallback(hvevent);
    break;

  case Hv_CLOSEDPOLYSHAPE: case Hv_OPENPOLYSHAPE:
    Hv_WorldPolygonButtonCallback(hvevent);
    break;

  case Hv_OVALSHAPE:
    Hv_WorldOvalButtonCallback(hvevent);
    break;

  }

}


/*------- Hv_ShapeDraw ---------*/

static void Hv_ShapeDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect)

{
  short              offset, shape;
  short              xc, yc;
  Hv_Point           poly[9];

  offset = Hv_GetMBOffset(Item, choice);
  shape = Hv_LINESHAPE + offset;
  Hv_ShrinkRect(rect, 1, 1);
  Hv_GetRectCenter(rect, &xc, &yc);

  switch (shape) {
  case Hv_LINESHAPE:
    Hv_SetLineWidth(2);
    Hv_DrawLine(rect->left, yc, rect->right, yc, Hv_black);
    Hv_SetLineWidth(0);
    break;

  case Hv_RECTSHAPE:
    Hv_ShrinkRect(rect, 2, 2);
    Hv_OffsetRect(rect, -1, 0);
    Hv_FillRect(rect, Hv_tan);
    Hv_FrameRect(rect, Hv_black);
    break;

  case Hv_OVALSHAPE:
    Hv_ShrinkRect(rect, 2, 2);
    Hv_OffsetRect(rect, -1, 0);
    Hv_FillOval(rect, True, Hv_tan, Hv_black);
    break;

  case Hv_OPENPOLYSHAPE: case Hv_CLOSEDPOLYSHAPE:
    Hv_SetPoint(&(poly[0]), rect->left+1, rect->bottom-2);
    Hv_SetPoint(&(poly[1]), poly[0].x,    poly[0].y-9);
    Hv_SetPoint(&(poly[2]), poly[1].x+4,  poly[1].y-6);
    Hv_SetPoint(&(poly[3]), poly[2].x+8, poly[2].y);
    Hv_SetPoint(&(poly[4]), poly[3].x+6,  poly[3].y+8);
    Hv_SetPoint(&(poly[5]), poly[4].x-5,  poly[4].y+7);
    Hv_SetPoint(&(poly[6]), poly[5].x-7,  poly[5].y);
    Hv_SetPoint(&(poly[7]), poly[6].x,    poly[6].y-6);
    Hv_SetPoint(&(poly[8]), poly[7].x-4,  poly[7].y+6);

    if (shape == Hv_OPENPOLYSHAPE) {
      Hv_SetLineWidth(2);
      Hv_DrawLines(poly+1, 7, Hv_black);
      Hv_SetLineWidth(0);
    }
    else
      Hv_FillPolygon(poly, 9, True, Hv_tan, Hv_black);
    break;


  }

}

/*--------- Hv_FontCB --------*/

static void Hv_FontCB(Hv_Event hvevent)
     
{
  Hv_usedNewDrawTools = True;
  Hv_TextButtonCallback(hvevent);
}


/*------- Hv_FontDraw ---------*/

static void Hv_FontDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect)

{
  short              offset, font;
  Hv_String         *hvstr;
  short              w, h, xc, yc;

  offset = Hv_GetMBOffset(Item, choice);

  hvstr = Hv_CreateString("S");

/* a bit confusing since we use fixed2 several times */

  if (offset == 0)
    font = Hv_fixed;
  else if (offset > 4)
    font = Hv_fixed + offset - 3;
  else
    font = Hv_fixed2;
  
  hvstr->font = font;

  Hv_CompoundStringDimension(hvstr, &w, &h);
  Hv_GetRectCenter(rect, &xc, &yc);

  Hv_DrawCompoundString(xc - w/2, yc - h/2 + 3, hvstr);

  Hv_DestroyString(hvstr);
}


/*------- Hv_FillDraw ---------*/

static void Hv_FillDraw(Hv_Item Item,
			short   choice,
			Hv_Rect *rect)

{
  short  offset;

  offset = Hv_GetMBOffset(Item, choice);

  if (offset < Hv_numColors)
    Hv_FillRect(rect, Hv_red + offset);
  else {
    rect->height -= 1;
    rect->width -= 1;
    Hv_FixRectRB(rect);
    /*    Hv_FillRect(rect, Hv_aliceBlue); */
    Hv_FrameRect(rect, Hv_black);
    Hv_DrawLine(rect->left, rect->top,    rect->right, rect->bottom, Hv_black);
    Hv_DrawLine(rect->left, rect->bottom, rect->right, rect->top,    Hv_black);
  }
}


/*------- Hv_PatternDraw ---------*/

static void Hv_PatternDraw(Hv_Item Item,
			   short   choice,
			   Hv_Rect *rect)

{
  short offset, pattern, i;

  offset = Hv_GetMBOffset(Item, choice);
  pattern = Hv_FILLSOLIDPAT + offset;


  if (pattern != Hv_HOLLOWPAT) {
    /*    Hv_FillRect(rect, Hv_aliceBlue); */
    Hv_FillPatternRect(rect, Hv_black, pattern);
  }
  else {
    Hv_FillRect(rect, Hv_white);
    rect->height -= 1;
    rect->width -= 1;
    Hv_FixRectRB(rect);

    for (i = 0; i < 10; i++) {
      Hv_FrameRect(rect, Hv_gray6 - i);
      Hv_ShrinkRect(rect, 1, 1);
      if ((rect->width == 1) || (rect->height == 1))
	break;
    }
  }
  
}


/*------- Hv_LineStyleDraw ---------*/

static void Hv_LineStyleDraw(Hv_Item Item,
			     short   choice,
			     Hv_Rect *rect)

{
  short offset, xc, yc;

  offset = Hv_GetMBOffset(Item, choice);

  rect->width  -= 1;
  rect->height  -= 1;
  Hv_FixRectRB(rect);
  Hv_ShrinkRect(rect, 1, 1);
  Hv_GetRectCenter(rect, &xc, &yc);
  yc++;

  Hv_SetLineStyle(0, Hv_SOLIDLINE + offset);
  Hv_DrawLine(rect->left, yc, rect->right, yc, Hv_black);
  Hv_SetLineStyle(0, Hv_SOLIDLINE);
}


/*------- Hv_LineWidthDraw ---------*/

static void Hv_LineWidthDraw(Hv_Item Item,
			     short   choice,
			     Hv_Rect *rect)

{
  short offset, width, xc, yc;

  offset = Hv_GetMBOffset(Item, choice);
  if (offset == 0)
    width = 0;
  else
    width = offset + 1;


  rect->width  -= 1;
  rect->height  -= 1;
  Hv_FixRectRB(rect);
  Hv_ShrinkRect(rect, 1, 1);
  Hv_GetRectCenter(rect, &xc, &yc);
  yc++;

  Hv_SetLineWidth(width);
  Hv_DrawLine(rect->left, yc, rect->right, yc, Hv_black);
  Hv_SetLineWidth(0);
}


/*------- Hv_ArrowStyleDraw ---------*/

static void Hv_ArrowStyleDraw(Hv_Item Item,
			      short   choice,
			      Hv_Rect *rect)

{
  short              offset;
  short              xc, yc;
  Hv_Point           poly[3];

  offset = Hv_GetMBOffset(Item, choice);

  rect->width  -= 1;
  rect->height  -= 1;
  Hv_FixRectRB(rect);
  Hv_ShrinkRect(rect, 1, 1);
  Hv_GetRectCenter(rect, &xc, &yc);
  yc++;



  Hv_DrawLine(rect->left, yc, rect->right, yc, Hv_black);
  if ((offset == 1) || (offset == 3)) {
    Hv_SetPoint(poly, rect->right, yc);
    Hv_SetPoint(poly+1, rect->right-8, yc-5);
    Hv_SetPoint(poly+2, rect->right-8, yc+5);
    Hv_FillPolygon(poly, 3, False, Hv_black, -1);
  }
  if ((offset == 2) || (offset == 3)) {
    Hv_SetPoint(poly, rect->left, yc);
    Hv_SetPoint(poly+1, rect->left+8, yc-5);
    Hv_SetPoint(poly+2, rect->left+8, yc+5);
    Hv_FillPolygon(poly, 3, False, Hv_black, -1);
  }
}


/*------- Hv_BorderDraw ---------*/

static void Hv_BorderDraw(Hv_Item Item,
			 short   choice,
			 Hv_Rect *rect)

{
  short offset, color;

  rect->width  -= 1;
  rect->height  -= 1;
  Hv_FixRectRB(rect);
  Hv_ShrinkRect(rect, 1, 1);

  offset = Hv_GetMBOffset(Item, choice);
  color = Hv_red + offset;

  if (offset < Hv_numColors) {
    if ((color > (Hv_white-3)) && (color < (Hv_white+6)))
      Hv_FillRect(rect, Hv_black);
    else if ((color >= Hv_aliceBlue) && (color < Hv_royalBlue))
      Hv_FillRect(rect, Hv_black);
    /*    else
      Hv_FillRect(rect, Hv_aliceBlue); */

    Hv_SetLineWidth(3);
    Hv_FrameRect(rect, color);
    Hv_SetLineWidth(0);
  }
  else {
    /*    Hv_FillRect(rect, Hv_aliceBlue); */
    Hv_SetLineWidth(3);
    Hv_FrameRect(rect, Hv_black);
    Hv_SetLineWidth(0);
    Hv_DrawLine(rect->left, rect->top,    rect->right, rect->bottom, Hv_black);
    Hv_DrawLine(rect->left, rect->bottom, rect->right, rect->top,    Hv_black);
  }
}


/*-------- Hv_FontMenuCB ------------*/

static void Hv_FontMenuCB(Hv_Item  Item,
		   short    choice)

/***********************************
  callback routine for menu buttons
***********************************/

{
  short offset, font;

  offset = Hv_GetMBOffset(Item, choice);

  if (offset == 0)
    font = Hv_fixed;
  else if (offset > 4)
    font = Hv_fixed + offset - 3;
  else
    font = Hv_fixed2;

  Item->view->hotfont = font;

}


/*-------- Hv_ShapeMenuCB ------------*/

static void Hv_ShapeMenuCB(Hv_Item  Item,
		   short    choice)

/***********************************
  callback routine for shape selection
  menu button
***********************************/

{
  short offset;

  offset = Hv_GetMBOffset(Item, choice);

  Item->view->hotshape = Hv_LINESHAPE + offset;
}


/*-------- Hv_FillMenuCB ------------*/

static void Hv_FillMenuCB(Hv_Item  Item,
		   short    choice)

/***********************************
  callback routine for fill color
  select menu button
***********************************/

{
  short offset;

  offset = Hv_GetMBOffset(Item, choice);
  
  if (offset >= Hv_numColors)
    Item->view->hotfillcolor = -1;
  else
    Item->view->hotfillcolor = Hv_red+offset;

}



/*-------- Hv_LineStyleMenuCB ------------*/

static void Hv_LineStyleMenuCB(Hv_Item  Item,
			       short    choice)

/***********************************
  callback routine for border line
  style menu button
***********************************/

{
  short              offset;
  Hv_View            View = Item->view;

  offset = Hv_GetMBOffset(Item, choice);
  
  View->hotlinestyle = Hv_SOLIDLINE + offset;
}


/*-------- Hv_LineWidthMenuCB ------------*/

static void Hv_LineWidthMenuCB(Hv_Item  Item,
			       short    choice)

/***********************************
  callback routine for border line
  size menu button
***********************************/

{
  short              offset;
  Hv_View            View = Item->view;

  offset = Hv_GetMBOffset(Item, choice);
  
  if (offset == 0)
    View->hotlinewidth = 0;
  else
    View->hotlinewidth = offset + 1;
}


/*-------- Hv_ArrowStyleMenuCB ------------*/

static void Hv_ArrowStyleMenuCB(Hv_Item  Item,
			       short    choice)

/***********************************
  callback routine for arrow
  style menu button
***********************************/

{
  short              offset;
  Hv_View            View = Item->view;

  offset = Hv_GetMBOffset(Item, choice);

  if (offset == 0)
    View->hotarrowstyle = Hv_NOARROW;
  else
    View->hotarrowstyle = Hv_ARROWATEND + offset - 1;
}


/*-------- Hv_BorderMenuCB ------------*/

static void Hv_BorderMenuCB(Hv_Item  Item,
			    short    choice)

/***********************************
  callback routine for border color
  select menu button
***********************************/

{
  short offset;

  offset = Hv_GetMBOffset(Item, choice);

  if (offset >= Hv_numColors)
    Item->view->hotbordercolor = -1;
  else
    Item->view->hotbordercolor = Hv_red+offset;
}

/*-------- Hv_PatternMenuCB ------------*/

static void Hv_PatternMenuCB(Hv_Item  Item,
			    short    choice)

/***********************************
  callback routine for pattern select 
  menu button
***********************************/

{
  short offset;

  offset = Hv_GetMBOffset(Item, choice);
  Item->view->hotpattern = Hv_FILLSOLIDPAT + offset;
}


/*-------- Hv_GetMBOffset -------*/

static short Hv_GetMBOffset(Hv_Item Item,
			 short choice)

/************************************
  Convenience routine for iconbased
  buttons that returns "offset" which
  starts at 0 and increases along the
  rows. 
**********************************/

{
  Hv_MenuButtonData  mbd;
  short   row, col;

  mbd = Hv_GetMenuButtonData(Item);

  row = (choice / 256) - 1;
  col = (choice % 256) - 1;
  
  return  row*(mbd->numcol) + col;

}  

#undef   Hv_DTWIDTH
#undef   Hv_DTHEIGHT
