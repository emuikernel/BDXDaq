/*
-----------------------------------------------------------------------
File:     Hv_colorbutton.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with colorbutton items */

#include "Hv.h"	  /* contains all necessary include files */

#define  Hv_CBSIZE 11

/*------- Hv_DrawColorButtonItem ---------*/

void     Hv_DrawColorButtonItem(Hv_Item   Item,
				Hv_Region region)

{
  Hv_Rect              *area;
  Hv_ColorScheme       *scheme;
  Hv_ColorButtonData   *data;
  short                fcolor, realcolor, lcolor;
  Hv_Rect              rect;
  Boolean              selectable;

/* can the button be selected, or is it desenitized? */

  selectable = Hv_ItemIsEnabled(Item);

  area   = Item->area;
  scheme = Item->scheme;
  data   = (Hv_ColorButtonData *)(Item->data);

  fcolor = data->fillcolor;

  if (!selectable) 
    Hv_SetPattern(1, Hv_black);

  
  if (fcolor >= 0)
    Hv_Simple3DRect(area, Item->state == Hv_OFF, fcolor);
  else {
    realcolor = fcolor + 256;
    if ((realcolor >= Hv_red) && (realcolor < (Hv_red + Hv_numColors))) {
      Hv_Simple3DRect(area, False, realcolor);
      Hv_CopyRect(&rect, area);
      Hv_ShrinkRect(&rect, 2, 2);
      
      if ((realcolor == Hv_navyBlue) ||
	  ((realcolor > Hv_gray6) && (realcolor <= Hv_black)))
	lcolor = Hv_white;
      else
	lcolor = Hv_black;

      Hv_DrawLine(rect.left,  rect.top, rect.right, rect.bottom, lcolor);
      Hv_DrawLine(rect.right, rect.top, rect.left,  rect.bottom, lcolor);
    }
    else
      Hv_Simple3DRect(area, Item->state == Hv_OFF, scheme->fillcolor);
  }

  if (!selectable)
    Hv_SetPattern(Hv_FILLSOLIDPAT, Hv_black);

}

/*-------- Hv_ColorButtonInitialize --------*/

void Hv_ColorButtonInitialize(Hv_Item           Item,
			      Hv_AttributeArray attributes)

{
  Hv_View              View = Item->view;
  Hv_ColorButtonData   *data;
  Boolean              inbox;
  short                boxfill = 0;
  Hv_Item              parent;

  Item->standarddraw = Hv_DrawColorButtonItem;
  Hv_DefaultSchemeOut(&(Item->scheme));
  Item->scheme->framewidth  = 2;      /* in this case it is the width of the 3d frame */

  if (attributes[Hv_WIDTH].s < 2)
    Item->area->width = Hv_CBSIZE;

  if (attributes[Hv_HEIGHT].s < 2)
    Item->area->height = Hv_CBSIZE;

  Hv_FixRectRB(Item->area);

  
  if (Item->singleclick == NULL)
    Item->singleclick = Hv_EditColorButtonItem;

  if (Item->doubleclick == NULL)
    Item->doubleclick = Hv_EditColorButtonItem;

  parent = Item->parent;
  inbox = ((parent != NULL) && ((parent->type == Hv_BOXITEM)
				|| (parent->type == Hv_RADIOCONTAINERITEM)));

  if (inbox)
    boxfill = parent->scheme->fillcolor;
    

  Item->data = (void *) Hv_Malloc(sizeof(Hv_ColorButtonData));
  data = (Hv_ColorButtonData *)(Item->data);

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    data->fillcolor = attributes[Hv_FILLCOLOR].s;
  else
    data->fillcolor = Item->scheme->fillcolor;

  data->allownocolor = (attributes[Hv_ALLOWNOCOLOR].s != 0);

  data->text = Hv_VaCreateItem(View,
			       Hv_TYPE,            Hv_TEXTITEM,
			       Hv_PARENT,          Item,
			       Hv_FONT,            attributes[Hv_FONT].s,
			       Hv_TEXT,            (char *)(attributes[Hv_TEXT].v),
			       Hv_CHILDPLACEMENT,  Hv_POSITIONRIGHT,
			       Hv_PLACEMENTGAP,    5,
			       NULL);

/* if button is in a box, need the proper background */

  if (inbox)
    data->text->scheme->fillcolor = boxfill;

}


/*--------  Hv_GetColorButtonColor --------*/

short    Hv_GetColorButtonColor(Hv_Item Item)

{
  Hv_ColorButtonData   *data;

  data = (Hv_ColorButtonData *)(Item->data);
  return    data->fillcolor;
}


/*--------  Hv_ChangeColorButtonText --------*/

void Hv_ChangeColorButtonText(Hv_Item Item,
			      char    *str,
			      Boolean Redraw)
     
{
  Hv_ColorButtonData   *data;

  data = (Hv_ColorButtonData *)(Item->data);

  if ((Item == NULL) || (data->text == NULL))
    return;
  Hv_ChangeTextItemText(data->text, str, Redraw);
}

/*--------- Hv_EditColorButtonItem ---------*/

void    Hv_EditColorButtonItem(Hv_Event  hvevent)

{
  Hv_Item                Item = hvevent->item;
  Hv_ColorButtonData     *data;

  data = (Hv_ColorButtonData *)(Item->data);

  if (data->fillcolor < -(Hv_numColors+1)) {
    Hv_SysBeep();
    return;
  }

  Item->state = Hv_ON;
  Hv_RedrawItem(Item);

  Hv_ModifyColor(NULL, &(data->fillcolor), "Edit  Color", data->allownocolor);
  Item->state = Hv_OFF;
  Hv_RedrawItem(Item);
}

/*--------- Hv_ToggleColorButtonItem ---------*/

void    Hv_ToggleColorButtonItem(Hv_Event  hvevent)


/* does this weird 256 offset than can be used to
   disable drawing of items tied to this color button */

{
  Hv_Item                Item = hvevent->item;
  Hv_ColorButtonData     *data;

  data = (Hv_ColorButtonData *)(Item->data);
  
  if (data->fillcolor < -(Hv_numColors+1))
    data->fillcolor += 256;
  else
    data->fillcolor -= 256;

  Hv_RedrawItem(Item);
}


#undef Hv_CBSIZE

