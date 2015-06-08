/**
 * <EM>Deals with creating, drawing, etc. for the Hv_BoxItem.</EM>
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

#define  COLLAPSESIZE 10

static void Hv_StandardBoxCB(Hv_Event hvevent);

static void Hv_GetCollapseRect(Hv_Item  Item,
			       Hv_Rect  *crect);


static void Hv_SetChildDrawing(Hv_ItemList items,
			       Boolean drawme);

static void Hv_ResizeHotRectItems(Hv_ItemList items,
				  short dh,
				  short dv);

static Boolean ignoreClip = False;
static Hv_Region Hv_GetNonOverlapRegion(Hv_Item Item,
					Hv_Region region);

/** 
 * Hv_UpdateBoxItem
 * @purpose  Update The text in a BoxItem.
 * @param    Item   The BoxItem
 * @param    str    The new string.
 */

void    Hv_UpdateBoxItem(Hv_Item Item,
			 char    *str) {

  Hv_View     View = Item->view;
  short       fw;
  Hv_Region   crgn = NULL;
  Hv_Region   nolrgn = NULL;
  short       xc, yc, w, h;

  if (Item == NULL)
	return;

  if ((Item->str) && (str != NULL)) {
	if ((Item->str->text != NULL) && (strcmp(Item->str->text, str) == 0))
	  return;

	Hv_Free(Item->str->text);
	Hv_InitCharStr(&(Item->str->text), str);
  }


  if (!Hv_ViewIsVisible(View))
	return;

 
  if ((Item->str) && (Item->str->text)) {
/*	fw = Item->scheme->framewidth;*/
		fw = 1;  /* since move to "lightweight" frames */

        crgn = Hv_GetItemDrawingRegion(Item, fw, fw);
	nolrgn = Hv_GetNonOverlapRegion(Item, crgn);
	Hv_SetClippingRegion(nolrgn);

	if (Hv_CheckItemDrawControlBit(Item, Hv_FRAMEAREA)) {
		Hv_Simple3DRect(Item->area, False, Item->scheme->fillcolor);
	}
	else
	  Hv_FillRect(Item->area, Item->scheme->fillcolor);
	
	Hv_SetBackground(Hv_colors[Item->scheme->fillcolor]);
	Hv_useDrawImage = True;
	Hv_GetItemCenter(Item, &xc, &yc);
	Hv_CompoundStringDimension(Item->str, &w, &h);

	if (Hv_CheckItemDrawControlBit(Item, Hv_ALIGNLEFT))
	Hv_DrawCompoundString((short)(Item->area->left + 6), (short)(yc - h/2), Item->str);
	else
	Hv_DrawCompoundString((short)(xc - w/2), (short)(yc - h/2), Item->str);

 		Hv_DestroyRegion(crgn);
		Hv_DestroyRegion(nolrgn);

	Hv_useDrawImage = False;
	Hv_SetBackground(Hv_colors[Hv_canvasColor]);
  }
}

/** 
 * Hv_DrawBoxItem
 * @purpose  Draw the BoxItem.
 * @param    Item   The BoxItem
 * @param    region The clipping region.
 */

void    Hv_DrawBoxItem(Hv_Item   Item,
		       Hv_Region region) {

  
  Hv_Rect          *area;
  Hv_Rect          crect;
  Hv_ColorScheme   *scheme;

  Hv_Region   crgn = NULL;
  Hv_Region   nolrgn = NULL;

  area   = Item->area;
  scheme = Item->scheme;

  if ((area->width < 1) || (area->height < 1))
    return;

  crgn = Hv_GetItemDrawingRegion(Item, 0, 0);
  nolrgn = Hv_GetNonOverlapRegion(Item, crgn);
  nolrgn = Hv_IntersectRegion(nolrgn, region);

  Hv_SetClippingRegion(nolrgn);


  if (Hv_CheckItemDrawControlBit(Item, Hv_FRAMEAREA)) {
    Hv_Simple3DRect(area, False, scheme->fillcolor);
  }
  else
    Hv_FillRect(Item->area, Item->scheme->fillcolor);

/*
 * collapsable? Only if the bit is set AND this item is oustide
 * the hotrect A
 */

  if (Hv_CheckItemDrawControlBit(Item, Hv_COLLAPSABLE) &&
      (Item->domain == Hv_OUTSIDEHOTRECT)) {

/*    if (Item->singleclick == NULL) */
      Item->singleclick = Hv_StandardBoxCB;
      Hv_ClearBit(&(Item->drawcontrol), Hv_NOPOINTERCHECK);

    Hv_GetCollapseRect(Item, &crect);
/*    Hv_Simple3DRect(&crect, Item->state == Hv_OFF,  Hv_gray7);*/

    Hv_ShrinkRect(&crect, 1, 1);
    Hv_Simple3DRect(&crect, Item->state != Hv_ON, scheme->fillcolor); 

  }


  ignoreClip = True;
  Hv_UpdateBoxItem(Item, NULL);
  ignoreClip = False;

  Hv_SetClippingRegion(region);
  Hv_DestroyRegion(crgn);
  Hv_DestroyRegion(nolrgn);


}

/** 
 * Hv_BoxInitialize
 * @purpose  Initialize the BoxItem.
 * @param    Item   The BoxItem
 * @param    attributes The attribute array.
 */

void Hv_BoxInitialize(Hv_Item Item,
		      Hv_AttributeArray attributes) {


  Item->standarddraw = Hv_DrawBoxItem;
  Hv_DefaultSchemeIn(&(Item->scheme));
  Item->scheme->fillcolor -= 1;

  Hv_SetItemDrawControlBit(Item, Hv_FRAMEAREA);

  if (attributes[Hv_FILLCOLOR].s != Hv_DEFAULT)
    Item->scheme->fillcolor = attributes[Hv_FILLCOLOR].s;

/* if there is a string in the attributes, create it here. This
   will be the case for an "update"style box (details of string will
   be filled by Hv_VaCreateItem) */

  if (attributes[Hv_TEXT].v != NULL)
    Item->str = Hv_CreateString(NULL);

  if (Hv_CheckItemDrawControlBit(Item, Hv_COLLAPSABLE))
    Item->user2 = attributes[Hv_VGAP].s;
}


static Hv_Region Hv_GetNonOverlapRegion(Hv_Item Item,
										Hv_Region region) {
	Hv_Region rgn;
	Hv_Item   temp;

	Hv_CopyRegion(&rgn, region);

	if (Item != NULL) {
		for (temp = Item->view->items->first; (temp != NULL) && (temp != Item); temp = temp->next) {
			if (temp->domain == Hv_OUTSIDEHOTRECT) {
				if ((temp->type == Hv_CHOICESETITEM) || (temp->type == Hv_FEEDBACKITEM)) {
					if (Item->str != NULL)
					rgn = Hv_SubtractRectFromRegion(rgn, temp->area);
				}
			}
		}

	}

	return rgn;
}

/**
 * Hv_GetCollapseRect
 * @purpose   Return the collapse rect for this box.
 * @param    item    The box item.
 * @param    crect   Collapse rect (upon return);
 * @return   The collapse rect, or NULL.
 * @local
 */

static void Hv_GetCollapseRect(Hv_Item  Item,
			       Hv_Rect  *crect) {

  int  extra = 0;

  if (Hv_CheckItemDrawControlBit(Item, Hv_COLLAPSABLE) &&
      (Item->domain == Hv_OUTSIDEHOTRECT)) {

    if (Item->state == Hv_OFF)
      extra = Item->user2;

    Hv_SetRect(crect,
	       (short)(Item->area->right - COLLAPSESIZE - 2),
	       (short)(Item->area->top + 3 + extra),
	       COLLAPSESIZE,
	       COLLAPSESIZE);

  }

}

/**
 * @purpose   Look for clicks in collapse box
 * @param     Event that generated the callback
 * @local
 */

static void Hv_StandardBoxCB(Hv_Event hvevent) {

  short   del;
  Hv_Rect crect;
  short   width, height;

  Hv_Item item = hvevent->item;
  Hv_View view = hvevent->view;
  Hv_Item hrb;
  Hv_Region vrgn;


  Hv_GetCollapseRect(item, &crect);

  if (Hv_PointInRect(hvevent->where, &crect)) {

/**
 * ALERT ALERT for experimental version assume box contains
 * ALL CONTROLS to the right of the hotrect so collapse the
 * box and expand hot rect right this will have to be generalized.
 *
 * ALSO using user1 and user2 for storage which is a no no long term
 */      

    hrb = view->hotrectborderitem;
    vrgn = Hv_GetViewRegion(view);

    if (item->state == Hv_ON) {
      fprintf(stderr, "expanding\n");

      width  = (short)(item->user1 & 0xFFFF);
      height = (short)(item->user1 >> 16);

      del = width - COLLAPSESIZE - 4;

      item->area->left -= del;
      item->area->bottom = item->area->top + height;

      hrb->area->right -= (del-2);

      Hv_ResizeHotRectItems(view->items, (short)(-(del-2)), 0);
      /* Hv_ShrinkRect(view->hotrect, (del-2), 0); */

	  view->hotrect->right -= (del-2);

      item->state = Hv_OFF;
      Hv_SetChildDrawing(item->children, True);
    }
    else {
      fprintf(stderr, "collapsing\n");

/* store size */

      item->user1 = item->area->width + ((int)(item->area->height) << 16);

      del = item->area->width - COLLAPSESIZE - 4;
      item->area->left += del;
      item->area->bottom = item->area->top + COLLAPSESIZE + 4;

      hrb->area->right += (del-2);

/*      Hv_ShrinkRect(view->hotrect, -(del-2), 0); */
      view->hotrect->right += (del-2);

      Hv_ResizeHotRectItems(view->items, (short)(del-2), 0);

      item->state = Hv_ON;
      Hv_SetChildDrawing(item->children, False);
    }
    
    Hv_FixRectWH(item->area);
    Hv_FixRectWH(hrb->area);
    Hv_FixRectWH(view->hotrect);
    Hv_SetViewDirty(view);
    Hv_DrawView(view, vrgn);
    Hv_DestroyRegion(vrgn);

  }

}


/**
 * Hv_SetChildDrawing
 * @param  items   The item list.
 * @param  drawme  Draw control flag.
 * @local
 */

static void Hv_SetChildDrawing(Hv_ItemList items,
			       Boolean drawme) {
  
  Hv_Item    temp;
  
  if ((items == NULL) || (items->first == NULL))
    return;
  
  
  for (temp = items->first; temp != NULL; temp = temp->next) {
    
    if (drawme)
      Hv_SetItemToDraw(temp);
    else
      Hv_SetItemNotToDraw(temp);
    
    if (temp->children != NULL)
      Hv_SetChildDrawing(temp->children, drawme);
  } 
}


/**
 * Hv_ResizeHotRectItems
 * @param  items   The item list.
 * @param  dh    Horizontal pixel resize
 * @param  dv    Vertical   pixel resize
 * @local
 */

static void Hv_ResizeHotRectItems(Hv_ItemList items,
				  short dh,
				  short dv) {
  
  Hv_Item    temp;
  
  if ((items == NULL) || (items->first == NULL))
    return;
  
  
  for (temp = items->first; temp != NULL; temp = temp->next) {
    
    if (temp->domain == Hv_INSIDEHOTRECT) {
      if (temp->type == Hv_MAPITEM) {
	fprintf(stderr, "RESIZING map item\n");
      }

      Hv_ResizeItem(temp, dh, dv, True);
    }

  } 
}

#undef  COLLAPSESIZE

