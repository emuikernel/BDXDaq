/**
 * <EM>Deals with creating, drawing,
 * etc for Hv's button items.</EM>
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

#define  Hv_DEFAULTBUTTONWIDTH       30
#define  Hv_DEFAULTBUTTONHEIGHT      24
#define  Hv_DEFAULTBUTTONRADIUS      15

/* ------ local prototypes --------*/

static void       Hv_DrawButtonFrame(Hv_Item,
				     Hv_Region);

static void       Hv_AutoSizeButton(Hv_Item);

static void       Hv_FillButton(Hv_Item);


/* -------- Hv_DrawButtonItem --------------- */

void Hv_DrawButtonItem(Hv_Item Item,
		       Hv_Region region)

/*draws the specified Item Hv_BUTTONITEM*/

{
  Hv_DrawButtonFrame(Item, region);
  Hv_DrawButtonText(Item, region);
}


/*------ Hv_ButtonDrawingArea -----*/

void Hv_ButtonDrawingArea(Hv_Item  Item,
	      	          Hv_Rect  *area)

/* mallocs and returns in "area" a rectangle
   which is the button's area minus the frame
   overhead -- which is the part of the button
   safe to draw on */

{
   Hv_ColorScheme   *scheme;

  scheme = Item->scheme;

  Hv_CopyRect(area, Item->area);
  Hv_ShrinkRect(area, 1, 1);
}

/**
 * Hv_FillButton
 * @purpose Fill the background   
 * @param  Item   The button item.
 * @local
 */

static void Hv_FillButton(Hv_Item Item) {
  short             xc, yc;
  Hv_Rect           *area;
  Hv_ColorScheme    *scheme;
  short             margin;
  Hv_Rect           rect;
  Boolean           circular, selectable;
  short             radius;
  short             fcolor;

/* do not fill bg on timed redraw */

  if (Hv_timedRedraw)
    return;

  area   = Hv_GetItemArea(Item);


  scheme = Item->scheme;

  margin = 1; 

/* can the button be selected, or is it desenitized? */

  selectable = Hv_ItemIsEnabled(Item);

/* round or rectangular ? */

  circular = Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED);

/* fill color depends on if button is on or off
   and if it is selecatble  */

  if (!selectable)
    fcolor = Hv_gray12;
  else {
    if (Item->state == Hv_OFF)
      fcolor = Item->scheme->fillcolor;
    else
      fcolor = Item->scheme->hotcolor;
  }

  Hv_CopyRect(&rect, area);
  Hv_ShrinkRect(&rect, margin, margin);
  
  if (circular) {
    radius = rect.width/2;
    radius -= 4;
    Hv_GetItemCenter(Item, &xc, &yc);
  }

/* fill the background */

  if (circular)
    Hv_FillCircle(xc, yc, radius, False, fcolor, Hv_black);
  else
    Hv_FillRect(&rect, fcolor);
}


/* -------- Hv_DrawButtonText --------*/

void Hv_DrawButtonText(Hv_Item Item,
		       Hv_Region region)

{
  short             xmw, xmh;		        /* String width  in pixels */
  short             xc, yoff;
  Hv_Rect           *area;
  Hv_ColorScheme    *scheme;
  Hv_String         *str;
  short             margin;
  short             oldcolor;
  Boolean           selectable;
  short             tcolor;

  Hv_FillButton(Item);

  area   = Hv_GetItemArea(Item);
  scheme = Item->scheme;
  margin = scheme->framewidth; 

/* if no text to write, we are done */

  str = Hv_GetItemString(Item);
  if ((str == NULL) || (str->text == NULL))
    return;

/* can the button be selected, or is it desenitized? */

  selectable = Hv_ItemIsEnabled(Item);
  tcolor = Item->str->strcol;

/* set up the motif string and get some characteristics */

  Hv_CompoundStringDimension(str, &xmw, &xmh);
    
  if (str->horizontal) {
    xc = area->left + (area->width - (short)xmw)/2 - 1;
    yoff = 3 + ((area->height - (short)xmh)/2) - margin;
    
    oldcolor = Item->str->strcol;
    str->strcol = tcolor;

    if (selectable)
	Hv_DrawCompoundString((short)(xc + 2), (short)(area->top + yoff), str);
    else
	Hv_DrawGhostedText((short)(xc + 2), (short)(area->top + yoff), str, Hv_white, Hv_gray7);

    str->strcol = oldcolor;
  }
  
  
}




/*------- Hv_ButtonItemInitialize ----------*/

void  Hv_ButtonItemInitialize(Hv_Item Item,
			      Hv_AttributeArray attributes)

{
  Boolean     circular;
  short       radius;

  Item->standarddraw = Hv_DrawButtonItem;
  Hv_DefaultSchemeOut(&(Item->scheme));

  circular = Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED);

  if (circular) {
    radius = attributes[Hv_RADIUS].s;
    if (radius < 2)
      radius = Hv_DEFAULTBUTTONRADIUS;
    Hv_SetRect(Item->area, Item->area->left, Item->area->top,
	       (short)(radius*2), (short)(radius*2));

    Item->scheme->lighttrimcolor = Hv_white;
    Item->scheme->darktrimcolor = Hv_gray1;
    Item->scheme->framewidth = 1;
  }
  else {
    if ((Item->area->width < 2) || (Item->area->height < 2))
      Hv_SetRect(Item->area, Item->area->left, Item->area->top,
		 Hv_DEFAULTBUTTONWIDTH, Hv_DEFAULTBUTTONHEIGHT);
  }


  if (attributes[Hv_FILLCOLOR].s == Hv_DEFAULT)
    Item->scheme->fillcolor -= 1;
  else
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;
    
  Item->scheme->topcolor -= 1;
  Item->scheme->framewidth  = attributes[Hv_FRAMEWIDTH].s;

/* give two chances to pass the label */

  if (attributes[Hv_TEXT].v != NULL)
    Item->str = Hv_CreateString((char *)(attributes[Hv_TEXT].v));
  else if (attributes[Hv_LABEL].v != NULL)
    Item->str = Hv_CreateString((char *)(attributes[Hv_LABEL].v));

  if (Item->str != NULL)
    Item->str->font = Hv_sMax(0, attributes[Hv_FONT].s);
  
  if (attributes[Hv_TEXTCOLOR].s >= 0)
    if (Item->str != NULL)
      Item->str->strcol = attributes[Hv_TEXTCOLOR].s;
  
  if (attributes[Hv_HOTCOLOR].s != Hv_DEFAULT)
    Item->scheme->hotcolor = attributes[Hv_HOTCOLOR].s;

/* if no single click provided, give the default */

  if (attributes[Hv_SINGLECLICK].v == NULL)
    Item->singleclick = Hv_ButtonSingleClick;

  if (attributes[Hv_AUTOSIZE].s != 0)
    Hv_AutoSizeButton(Item);
}

/*-------- Hv_ToggleButton -------------- */

void Hv_ToggleButton(Hv_Item Item)

{
  if (Item->type != Hv_BUTTONITEM)
    return;

  Hv_ToggleItemState(Item);
}

/*------- Hv_ButtonSingleClick ----------*/

void  Hv_ButtonSingleClick(Hv_Event hvevent)

{
  Hv_ToggleButton(hvevent->item);
}

/*------- Hv_GetButtonState ----------*/

short  Hv_GetButtonState(Hv_Item Item)

{
  if (Item == NULL)
    return -1;

  return Item->state;
}

/*------- Hv_SetButtonState ----------*/

void  Hv_SetButtonState(Hv_Item Item,
			short state)

{
  if (Item == NULL)
    return;

  if (Item->state != state)
    Hv_ToggleButton(Item);
}


/**
 *  Hv_DrawButtonFrame
 * @purpose   Draw the button frame.
 * @param  Item    The button item.
 * @param  region  The clipping region.
 * @local
 */

static void Hv_DrawButtonFrame(Hv_Item Item,
			       Hv_Region region) {

/* draws a sculptured frame around the button depending on its state*/

 short      xc, yc, radius;

  Hv_ColorScheme   *scheme;

  scheme = Item->scheme;

/* is it a round or rectangular button? */

  if (Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED)) {
    radius = Item->area->width/2;
    Hv_GetItemCenter(Item, &xc, &yc);


    if (Item->state == Hv_OFF)
      Hv_InvertColorScheme(scheme);
    
    Hv_SetLineWidth(3);
    Hv_Draw3DCircle(xc, yc, radius-2, scheme);
    Hv_SetLineWidth(0);
    Hv_FrameCircle(xc, yc, radius,   Hv_black);

    if (Item->state == Hv_OFF)
      Hv_FrameCircle(xc, yc, (short)(radius-4), (short)(scheme->fillcolor-1));
    else
      Hv_FrameCircle(xc, yc, (short)(radius-4), (short)(scheme->fillcolor+1));

    if (Item->state == Hv_OFF)
      Hv_InvertColorScheme(scheme);
    
    return;
  }

    if (Hv_timedRedraw)
      Hv_Simple3DRect(Item->area,
		      Item->state == Hv_OFF,
		      -1);
    else
      Hv_Simple3DRect(Item->area,
		      Item->state == Hv_OFF,
		      (short)(Item->state == Hv_OFF ? scheme->fillcolor : scheme->hotcolor)); 

}


/*-------- Hv_AutoSizeButton -------*/

static void       Hv_AutoSizeButton(Hv_Item Item)

{
  short             w, h, pw;
  Hv_ColorScheme    *scheme;

  if ((Item == NULL) || (Item->str == NULL) || (Item->str->text == NULL))
    return;

  scheme = Item->scheme;
  if (scheme == NULL)
    pw = 2;
  else
    pw = scheme->framewidth + scheme->borderwidth; 

  pw *= 2;

  Hv_CompoundStringDimension(Item->str, &w, &h);
  Item->area->width  = Hv_sMax((short)(w + pw), Item->area->width);
  Item->area->height = Hv_sMax((short)(h + pw), Item->area->height);
  Hv_FixRectRB(Item->area);
}


#undef  Hv_DEFAULTBUTTONWIDTH
#undef  Hv_DEFAULTBUTTONHEIGHT
#undef  Hv_DEFAULTBUTTONRADIUS



