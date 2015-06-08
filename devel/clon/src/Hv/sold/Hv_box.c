/*
-----------------------------------------------------------------------
File:     Hv_box.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"


/* --------- Hv_UpdateBoxItem ------------- */

void    Hv_UpdateBoxItem(Hv_Item Item,
			 char    *str)

{
  Hv_View     View = Item->view;
  short       fw;
  Hv_Region   crgn = NULL;
  short       xc, yc, w, h;

  if (!Hv_ViewIsVisible(View))
    return;

  if (Item == NULL)
    return;

  if ((Item->str) && (str != NULL)) {
    if ((Item->str->text != NULL) && (strcmp(Item->str->text, str) == 0))
      return;
    
    Hv_Free(Item->str->text);
    Hv_InitCharStr(&(Item->str->text), str);
  }

     
  if ((Item->str) && (Item->str->text)) {
    fw = Item->scheme->framewidth;
    crgn = Hv_GetItemDrawingRegion(Item, fw, fw);
    Hv_SetClippingRegion(crgn);
    Hv_DestroyRegion(crgn);

    Hv_FillRect(Item->area, Item->scheme->fillcolor);
    Hv_SetBackground(Hv_colors[Item->scheme->fillcolor]);
    Hv_useDrawImage = True;
    Hv_GetItemCenter(Item, &xc, &yc);
    Hv_CompoundStringDimension(Item->str, &w, &h);
    Hv_DrawCompoundString(xc - w/2, yc - h/2, Item->str);
    Hv_useDrawImage = False;
    Hv_SetBackground(Hv_colors[Hv_canvasColor]);
  }
}

/* --------- Hv_DrawBoxItem ------------- */

void    Hv_DrawBoxItem(Hv_Item   Item,
		       Hv_Region region)

{     
  Hv_Rect          *area;
  Hv_ColorScheme   *scheme;

  area   = Item->area;
  scheme = Item->scheme;

  if ((area->width < 1) || (area->height < 1))
    return;


/* lightweight? */

  if (Hv_CheckItemDrawControlBit(Item, Hv_LIGHTWEIGHT))
      Hv_Simple3DRect(area, False, scheme->fillcolor);
  else {
      Hv_FillRect(area, scheme->fillcolor); 
      Hv_Draw3DRect(area, scheme); 
      Hv_FrameRect(area, Hv_black); 
  }

  Hv_UpdateBoxItem(Item, NULL);

}

/*-------- Hv_BoxInitialize ------*/

void Hv_BoxInitialize(Hv_Item Item,
		      Hv_AttributeArray attributes)

{
  Item->standarddraw = Hv_DrawBoxItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/* if there is a string in the attributes, create it here. This
   will be the case for an "update"style box (details of string will
   be filled by Hv_VaCreateItem) */

  if (attributes[Hv_TEXT].v != NULL)
    Item->str = Hv_CreateString(NULL);
}

