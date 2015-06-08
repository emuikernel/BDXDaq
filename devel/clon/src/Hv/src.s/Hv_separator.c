/*
-----------------------------------------------------------------------
File:     Hv_separator.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with separator items */

#include "Hv.h"	  /* contains all necessary include files */

/*---- local prototypes ------*/

static void Hv_DrawSeparatorItem(Hv_Item    Item,
				 Hv_Region   region);

/*----- Hv_CenterSeparator -----*/

void   Hv_CenterSeparator(Hv_Item Item)

{
  Hv_SeparatorData    *separator;
  Hv_Item             parent;
  short               cent;

  parent = Item->parent;

  if (parent == NULL)
    return;

  separator = (Hv_SeparatorData *)(Item->data);

  if (separator->orientation == Hv_HORIZONTAL) { 
    cent = parent->area->top + parent->area->height/2;
    Item->area->top = cent-1;
  }
  else {
    cent = parent->area->left + parent->area->width/2;
    Item->area->left = cent-1;
  }
  Hv_FixRectRB(Item->area);
}


/*-------- Hv_SeparatorInitialize --------*/

void Hv_SeparatorInitialize(Hv_Item      Item,
			    Hv_AttributeArray attributes)
{
  Hv_SeparatorData    *data;

  Item->standarddraw = Hv_DrawSeparatorItem;
  data = (Hv_SeparatorData *)Hv_Malloc(sizeof(Hv_SeparatorData));
  Item->data = (void *)data;
  

  data->orientation = attributes[Hv_ORIENTATION].s;
  data->gap = attributes[Hv_MARGIN].s;
  data->topcolor = Hv_white;
  data->bottomcolor = Hv_black;

  if (data->orientation == Hv_HORIZONTAL)
    Item->area->height = 2;
  else
    Item->area->width = 2;
}

/*------ Hv_SeparatorResizeToParent ----------*/

void  Hv_SeparatorResizeToParent(Hv_Item Item)

{
  Hv_SeparatorData    *separator;
  Hv_Item             parent;

  if (Item == NULL)
    return;

  parent = Item->parent;

  if (parent == NULL)
    return;

  separator = (Hv_SeparatorData *)(Item->data);

  if (separator->orientation == Hv_HORIZONTAL) {
    Item->area->left  = parent->area->left  + separator->gap;
    Item->area->right = parent->area->right - separator->gap;
  }
  else {
    Item->area->top    = parent->area->top    + separator->gap;
    Item->area->bottom = parent->area->bottom - separator->gap;
  }
  Hv_FixRectWH(Item->area);
}


/* ------------- Hv_DrawSeparatorItem ------------*/

static void Hv_DrawSeparatorItem(Hv_Item    Item,
				 Hv_Region   region)
{
  Hv_SeparatorData    *separator;
  short               l, t, r, b;


  separator = (Hv_SeparatorData *)(Item->data);

  l = Item->area->left;
  t = Item->area->top;
  r = Item->area->right-1;
  b = Item->area->bottom-1;

  if (separator->orientation == Hv_HORIZONTAL) {
    Hv_DrawLine(l, t, r, t, separator->topcolor);
    Hv_DrawLine(l, (short)(t+1), r, (short)(t+1), separator->bottomcolor);
  }
  else {
    Hv_DrawLine(l, t, l, b, separator->topcolor);
    Hv_DrawLine((short)(l+1), t, (short)(l+1), b, separator->bottomcolor);
  }

}









