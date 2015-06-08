/**
 * <EM>Deals with creating, drawing, etc. for the Hv_Item.</EM>
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
#include "Hv_xyplot.h"
#include "Hv_maps.h"


/*--------  local prototypes ---------*/

static void    Hv_AddRectToRect(Hv_Rect   *dr,
				Hv_Rect   *sr);

static Hv_Item Hv_CreateItem(Hv_View           View,
			     Hv_AttributeArray attributes);

static Boolean Hv_CheckItemOverlap(Hv_Item   Item,
				   Hv_Region  region);

static void    Hv_KillItem(Hv_Item  Item);

static void    Hv_TimerDrawItem(Hv_Pointer      xtptr,
				Hv_IntervalId  *id);

static void    Hv_PositionItemRelative(Hv_Item   Item,
				       Hv_Item   relitem,
				       int       placement,
				       short     gap,
				       Boolean   center);

static Hv_FunctionPtr   Hv_DefaultInitialize(short type);

static Hv_FunctionPtr   Hv_DefaultDestroy(short type);

static short   Hv_DefaultDomain(short type);

static short   Hv_DefaultResizePolicy(short domain);

static void    Hv_FixItemListWorldArea(Hv_ItemList   items);

static void    Hv_DrawItemsAfterThisOne(Hv_Item 	Item,
					Hv_Region	region);

static void    Hv_PositionChildItem(Hv_Item   Item,
				    int        policy,
				    short      gap);




/**
 * Hv_LockItem
 * @purpose Lock an item so it cannot be deleyed
 * @param  Item   The item to be locked.
 */


void  Hv_LockItem(Hv_Item Item) {
    if (Item != NULL)
	Hv_SetItemDrawControlBit(Item, Hv_LOCKED);
}


/**
 * Hv_UnlockItem
 * @purpose Unlock an item so it can be deleyed
 * @param  Item   The item to be unlocked.
 */


void   Hv_UnlockItem(Hv_Item Item) {
    if (Item != NULL)
	Hv_ClearItemDrawControlBit(Item, Hv_LOCKED);
}


/**
 * Hv_IsItemLocked
 * @purpose Test if an item is locked.
 * @param  Item   The item to be tested.
 * @return True if item is locked
 */

Boolean   Hv_IsItemLocked(Hv_Item Item) {
    if (Item != NULL)
	return Hv_CheckItemDrawControlBit(Item, Hv_LOCKED);
    else
	return False;
}

/*----------- Hv_ToggleItemState ---------*/

void   Hv_ToggleItemState(Hv_Item Item) {
  Hv_Region   region;
  Hv_View     View;
  
  if (Item == NULL)
    return;
  
  View = Item->view;
  
  if (Item->state == Hv_OFF)
    Item->state = Hv_ON;
  else
    Item->state = Hv_OFF;
  
  if (Hv_ViewIsVisible(View)) {
    region = Hv_ClipView(View);
    
    if (Item->standarddraw)
      Item->standarddraw(Item, region);
    
    /* see if there is special drawing routine */
    
    if (Item->userdraw)
      Item->userdraw(Item, region);
    
    Hv_DestroyRegion(region);
  }
}


/*------- Hv_SetItemToDraw -------*/

void     Hv_SetItemToDraw(Hv_Item Item) {
    Hv_ClearItemDrawControlBit(Item, Hv_DONTDRAW);
}

/*------- Hv_SetItemNotToDraw -------*/

void     Hv_SetItemNotToDraw(Hv_Item Item) {
    Hv_SetItemDrawControlBit(Item, Hv_DONTDRAW);
}

/*------- Hv_CheckItemToDraw -------*/

Boolean  Hv_CheckItemToDraw(Hv_Item Item) {
    return   !Hv_CheckItemDrawControlBit(Item, Hv_DONTDRAW);
}


/*------- Hv_SetItemState -------*/

void Hv_SetItemState(Hv_Item   Item,
		     short     state) {
    Item->state = state;
}


/*------- Hv_GetItemState --------*/

short Hv_GetItemState(Hv_Item Item) {
    return Item->state;
}


/* ---------- Hv_SetItemDrawControlBit ---------*/

void      Hv_SetItemDrawControlBit(Hv_Item  Item,
                                   int      bit) {
    Hv_SetBit(&(Item->drawcontrol), bit);
}

/* ---------- Hv_CheckItemDrawControlBit ---------*/

Boolean  Hv_CheckItemDrawControlBit(Hv_Item  Item,
                                    int      bit) {
    return Hv_CheckBit(Item->drawcontrol, bit);
}

/* ---------- Hv_ClearItemDrawControlBit ---------*/

void      Hv_ClearItemDrawControlBit(Hv_Item  Item,
                                     int      bit) {
    Hv_ClearBit(&(Item->drawcontrol), bit);
}


/* ------- Hv_HotRectItemsBoundary ------*/

void  Hv_HotRectItemsBoundary(Hv_ItemList  items,
                              Hv_Rect      *br) {
  Hv_View    View;
  Hv_Rect    cbr, *hr;
  Hv_Item    temp;
    
  if ((items == NULL) || (items->first == NULL) || (br == NULL))
    return;
    
  View = items->first->view;
    
  br->left   = 32000;
  br->top    = 32000;
  br->right  = -32000;
  br->bottom = -32000;
    
  for (temp = items->first; temp != NULL; temp = temp->next)
    if ((temp->domain == Hv_INSIDEHOTRECT) && 
	(temp->type != Hv_HOTRECTBORDERITEM)) 
	    
      if ((View->tag != Hv_XYPLOTVIEW)
	  || (Hv_printThisPlotOnly == NULL)
	  || (temp == Hv_printThisPlotOnly)
	  || (temp->parent == Hv_printThisPlotOnly))  {
		
	Hv_AddRectToRect(br, temp->area);
		
	if (temp->children != NULL) {
	  Hv_HotRectItemsBoundary(temp->children, &cbr);
	  Hv_AddRectToRect(br, &cbr);
	}
      }
    
/* however, don't let it be bigger than the hotrect! */
    
  hr = items->first->view->hotrect;
  br->left   = Hv_sMax(br->left, hr->left);
  br->top    = Hv_sMax(br->top, hr->top);
  br->right  = Hv_sMin(br->right, hr->right);
  br->bottom = Hv_sMin(br->bottom, hr->bottom);
    
    
  Hv_FixRectWH(br);
}


/*-------- Hv_AddRectToRect -------*/

static void    Hv_AddRectToRect(Hv_Rect   *dr,
				Hv_Rect   *sr) {
    
    if ((dr == NULL) || (sr == NULL))
	return;
    
    dr->left   = Hv_sMin(dr->left, sr->left);
    dr->top    = Hv_sMin(dr->top, sr->top);
    dr->right  = Hv_sMax(dr->right, sr->right);
    dr->bottom = Hv_sMax(dr->bottom, sr->bottom);
    
    Hv_FixRectWH(dr);
}

/*------ Hv_ItemChangedRedraw -----*/

void Hv_ItemChangedRedraw(Hv_Item     Item,
			  Hv_Region   oldrgn) {
    Hv_Region  newrgn = NULL;
    Hv_Region  totrgn = NULL;
    
    newrgn = Hv_CopyItemRegion(Item, True);
    Hv_ShrinkRegion(newrgn, -2, -2);
    totrgn = Hv_UnionRegion(oldrgn, newrgn);
    Hv_DrawView(Item->view, totrgn);
    Hv_DestroyRegion(totrgn);
    Hv_DestroyRegion(newrgn); 
}


/*------ Hv_ItemMatch ------*/

Hv_Item Hv_ItemMatch(Hv_ItemList  items,
		     short        type,
		     short        tag,
		     short        domain) {
    
    
/* returns the first (backwards) match. Also searches child lists
   as encountered. The tag check is ONLY RELEVANT for
   user items (type = Hv_USERITEM) */
    
    Hv_Item     temp;
    
    if (items == NULL)
	return NULL;
    
    for (temp = items->last; temp != NULL; temp = temp->prev)
	if (temp->domain == domain) {
	    
	    if (temp->type == type)
		if ((type != Hv_USERITEM) || (temp->tag == tag)) 
		    return temp;
	    
	    if (temp->children != NULL) {
		temp = Hv_ItemMatch(temp->children, type, tag, domain);
		if (temp != NULL)
		    return temp;
	    }
	}
    
    return NULL;
}

/*------ Hv_PointInItemMatch ------*/

Hv_Item Hv_PointInItemMatch(Hv_ItemList  items,
			    short        type,
			    short        tag,
			    short        domain,
			    Hv_Point     pp) {
    
    
/* returns the first (backwards) match. Also searches child lists
   as encountered. The tag check is ONLY RELEVANT for
   user items (type = Hv_USERITEM) */
    
    Hv_Item     temp, temp2;
    
    if (items == NULL)
	return NULL;
    
    for (temp = items->last; temp != NULL; temp = temp->prev)
	if (temp->domain == domain) {
	    
	    if (temp->type == type)
		if ((type != Hv_USERITEM) || (temp->tag == tag)) 
		    if (Hv_PointInItem(pp, temp))
			return temp;
	    
	    if (temp->children != NULL) {
		temp2 = Hv_PointInItemMatch(temp->children, type, tag, domain, pp);
		if (temp2 != NULL)
		    return temp2;
	    }
	}
    
    return NULL;
}

/*------ Hv_PointInItem ------*/

Boolean Hv_PointInItem(Hv_Point  pp,
		       Hv_Item   Item) {
    
    if (Item == NULL)
	return False;
    
    if ((Item->region != NULL) && 
	!(Hv_CheckBit(Item->drawcontrol, Hv_CHECKAREAONLY)))
	return Hv_PointInRegion(pp, Item->region);
    else
	return Hv_PointInRect(pp, Item->area);
}

/*------- Hv_RedrawItem --------*/

void  Hv_RedrawItem(Hv_Item Item) {
    Hv_Region      rgn;
    
    if (Item == NULL)
	return;
    
    if (Hv_ViewIsVisible(Item->view)) {
	rgn = Hv_CopyItemRegion(Item, True);
	Hv_DrawView(Item->view, rgn);
	Hv_KillRegion(&rgn);
    }
}

/*-------- Hv_GetItemText -------*/

char   *Hv_GetItemText(Hv_Item Item) {
    if ((Item == NULL) || (Item->str == NULL))
	return NULL;
    
    return Item->str->text;
}


/*-------- Hv_SetItemText -------*/

void  Hv_SetItemText(Hv_Item   Item,
		     char    *text) {
    Hv_String    *str;
    
    if ((Item == NULL) || ((str = Item->str) == NULL))
	return;
    
    Hv_Free(str->text);
    Hv_InitCharStr(&(str->text), text);
    str->offset = 0;
}



/* -------- Hv_SetItemTimedRedraw -------*/

void Hv_SetItemTimedRedraw(Hv_Item        Item,
			   unsigned long  interval) {
    
/* starts a timer. When the timer expires, the procedure Hv_TimerDraw
   will be called with the given item. This can be used, for example, to
   animate buttons */
    
/* all items with timed redraw get their MINCLIPREGION bit set */
    
    if ((Item == NULL) || (interval == 0))
	return;
    
/* if drawinterval was never created, do so now */
    
    if (Item->drawinterval == NULL) {
	Item->drawinterval = (Hv_Interval *)Hv_Malloc(sizeof(Hv_Interval));
	Item->drawinterval->interval = 0;
	Item->drawinterval->intervalid = 0;
	Item->drawinterval->timedredrawcount = 0;
    }
    
    Item->drawinterval->interval = interval;
    Item->drawinterval->intervalid = Hv_AddTimeOut(interval,
						   (Hv_TimerCallbackProc)Hv_TimerDrawItem,
						   (Hv_Pointer)Item); 
    Hv_Flush();
}


/*------- Hv_RemoveItemTimedRedraw --------*/

void Hv_RemoveItemTimedRedraw(Hv_Item Item) {
    
/* this removes a pending timeout redraw */
    
    if ((Item == NULL) ||
	(Item->drawinterval == NULL) ||
	(Item->drawinterval->intervalid == 0))
	return;
    
    Hv_RemoveTimeOut(Item->drawinterval->intervalid);
    Hv_Flush();
    Item->drawinterval->interval = 0;
    Item->drawinterval->intervalid = 0;
    Item->drawinterval->timedredrawcount = 0;
}

/*------- Hv_GetItemDrawingRegion ------*/

Hv_Region    Hv_GetItemDrawingRegion(Hv_Item    Item,
				     short      dh,
				     short      dv) {
    
    
/*-----------------------------------------------------
  Mallocs and returns (does not "set") the intersection
  of the items region and the views drawing region. Also
  subtracts out the baloon region, if necessary.
  
  Includes Children
  -----------------------------------------------------*/
    
  Hv_Region     vrgn = NULL;
  Hv_Region     irgn = NULL;
  Hv_Region     crgn = NULL;
  Hv_Region     drgn = NULL;
  Hv_View       View;
    
  if (Item == NULL)
    return NULL;
  
  if ((View = Item->view) == NULL)
    return NULL;

  
  irgn = Hv_CopyItemRegion(Item, False);
  if (irgn == NULL)
    return NULL;
  
  
  if ((dh != 0) || (dv != 0))
    Hv_ShrinkRegion(irgn, dh, dv); 

  if (Item->domain == Hv_INSIDEHOTRECT)
    vrgn = Hv_ClipHotRect(Item->view, 1);
  else {
    if (Item != View->containeritem) {
      vrgn = Hv_GetInsideContainerRegion(View);
      if ((vrgn != NULL) && 
	  (Item != View->feedbackitem) && 
	  (View->feedbackitem != NULL)) {
	Hv_RemoveRectFromRegion(&vrgn, View->feedbackitem->area);
      }
    }
    else
      vrgn = Hv_GetViewRegion(Item->view);
  }
    
/* first cut: drawing region is intersection of view and item region */

  crgn = Hv_IntersectRegion(vrgn, irgn);
  
  Hv_KillRegion(&vrgn);
  Hv_KillRegion(&irgn);
    
/* if balloon is up, subtact its region */
    
  if (Hv_balloonUp && (Hv_theBalloon.region != NULL)) {
    drgn = Hv_SubtractRegion(crgn, Hv_theBalloon.region);
    Hv_KillRegion(&crgn);
    return drgn;
  }
  
  return crgn;
}


/* ---------  Hv_GetMinimumItemRegion ---------*/

Hv_Region Hv_GetMinimumItemRegion(Hv_Item Item) {
    
/* Mallocs and returns the "clipping" region for drawing this item.
   This is the items's region minus the regions of any items on
   top of this one. USE SPARINGLY -- POTENTIALLY SLOW. If the extra
   subtraction of "on-top" items is not needed, use the faster
   Hv_GetItemDrawingRegion */
    
    Hv_Item          temp;
    Hv_Region        tempregion;
    Hv_Region        region, diffregion;
    
/* start with the item's own drawing region */
    
    if (Item == NULL)
	return NULL;
    
    region = Hv_GetItemDrawingRegion(Item, 0, 0);
    
/* now go on to the remainder of the list (items drawn on top of this one  */
    
    for (temp = Item->next; temp != NULL; temp = temp->next) 
	if (temp->domain == Item->domain) {
	    tempregion = Hv_CopyItemRegion(temp, True);
	    diffregion = Hv_SubtractRegion(region, tempregion);
	    Hv_KillRegion(&tempregion);
	    Hv_KillRegion(&region);
	    region = diffregion;
	}
    
    return region;
}

/*------- Hv_SetItemArea ------*/

void  Hv_SetItemArea(Hv_Item  Item,
		     short    left,
		     short    top,
		     short    width,
		     short    height) {
    
    if (Item == NULL)
	return;
    
    Hv_SetRect(Item->area, left, top, width, height);
}

/*------- Hv_GetItemArea ------*/

Hv_Rect   *Hv_GetItemArea(Hv_Item Item) {

    if (Item == NULL)
	return NULL;
    
    return Item->area;
}

/*------- Hv_GetTotalItemArea ------*/

void   Hv_GetTotalItemArea(Hv_Item Item,
			   Hv_Rect *rect) {
    
/* like Hv_GetItemArea, but includes children */
    
  Hv_Rect   crect;
  Hv_Item   temp;
  
  if (Item == NULL) {
    Hv_SetRect(rect, 0, 0, 0, 0);
    return;
  }
  
  Hv_CopyRect(rect, Item->area);
  
  if (Item->children == NULL)
    return;
  
  for (temp = Item->children->first; temp != NULL; temp = temp->next) {
    Hv_GetTotalItemArea(temp, &crect);
    Hv_AddRectToRect(rect, &crect);
  }
  
}


/*------- Hv_SetItemRegion ------*/

void     Hv_SetItemRegion(Hv_Item Item,
			  Hv_Region region) {

    if (Item == NULL)
	return;
    
    Hv_KillRegion(&Item->region);
    Item->region = NULL;
    if (region != NULL)
	Hv_CopyRegion(&(Item->region), region); 
}

/*------- Hv_GetItemRegion ------*/

Hv_Region  Hv_GetItemRegion(Hv_Item Item) {

    if (Item == NULL)
	return NULL;
    return Item->region;
}


/*------- Hv_SetItemString ------*/

void   Hv_SetItemString(Hv_Item   Item,
			Hv_String *str) {

    if (Item == NULL)
	return;
    
    Hv_DestroyString(Item->str);
    Item->str = NULL;
    if (str != NULL)
	Hv_CopyString(&(Item->str), str); 
}

/*------- Hv_GetItemString ------*/

Hv_String  *Hv_GetItemString(Hv_Item Item) {

    if (Item == NULL)
	return NULL;
    return Item->str;
}


/*------- Hv_SetItemData ------*/

void Hv_SetItemData(Hv_Item  Item,
		    void    *data) {
    
/* simple information hiding routine that sets the item's data */
    
    if (Item == NULL)
	return;
    
    Item->data = data;
}


/*------- Hv_GetItemData ------*/

void *Hv_GetItemData(Hv_Item Item) {
    
/* simple information hiding routine that returns the item's color */
    
    if (Item == NULL)
	return NULL;
    else
	return Item->data;
}

/*------- Hv_SetItemColor ------*/

void Hv_SetItemColor(Hv_Item Item,
		     short   color) {
    
/* simple information hiding routine that sets the item's color */
    
    if (Item == NULL)
	return;
    
    Item->color = color;
}


/*------- Hv_GetItemColor ------*/

short Hv_GetItemColor(Hv_Item Item) {
    
/* simple information hiding routine that returns the item's color */
    
    if (Item == NULL)
	return Hv_black;
    else
	return Item->color;
}

/*------- Hv_SetItemTag ------*/

void Hv_SetItemTag(Hv_Item Item,
		   short   tag) {
    
/* simple information hiding routine that sets the item's tag */
    
    if (Item == NULL)
	return;
    
    Item->tag = tag;
}

/*------- Hv_GetItemTag ------*/

short Hv_GetItemTag(Hv_Item Item) {
    
/* simple information hiding routine that returns the item's tag */
    
    if (Item == NULL)
	return -1;
    else
	return Item->tag;
}

/*------- Hv_GetItemView ------*/

Hv_View   Hv_GetItemView(Hv_Item Item) {
    
/* simple information hiding routine that return's the item's view */
    
    if (Item == NULL)
	return NULL;
    else
	return Item->view;
}


/**
 * Hv_SetupHotRectBorderItem
 * @purpose Sets up the hot rect border.
 * @param    View   Thew owner view.
 * @return   The HotRect border item.
 */

Hv_Item  Hv_SetupHotRectBorderItem(Hv_View  View) {
    
/* sets up a hotrect border based on the Views hotrect*/
    
    Hv_Item     HRBPtr;
    Hv_Rect     *hr;
    
    if (View == NULL)
	return NULL;
    
    HRBPtr = Hv_VaCreateItem(View,
			     Hv_TYPE,         Hv_HOTRECTBORDERITEM,
			     Hv_DOMAIN,       Hv_OUTSIDEHOTRECT,
			     Hv_RESIZEPOLICY, Hv_RESIZEONRESIZE,
					 Hv_COLOR,        Hv_white,
			     Hv_DRAWCONTROL,  Hv_NOPOINTERCHECK + Hv_INBACKGROUND,
			     NULL);
    
    hr = View->hotrect;
    Hv_CopyRect(HRBPtr->area, hr);
    
    return HRBPtr;
}


/*------- Hv_MoveItemBehindItem ------*/

void  Hv_MoveItemBehindItem(Hv_Item moveItem,
			    Hv_Item stayItem) {

    Hv_View      View = Hv_GetItemView(moveItem);
    Hv_Item      mprev, mnext, snext;
    
    if ((moveItem == NULL) || (stayItem == NULL))
	return;
    
    if(moveItem->prev == stayItem)
	return;
    
    mprev = moveItem->prev;
    mnext = moveItem->next;
    snext = stayItem->next;
    
    if (View->items->first == moveItem)
	View->items->first = mnext;
    
    if (View->items->last == moveItem)
	View->items->last = mprev;
    else if (View->items->last == stayItem)
	View->items->last = moveItem;
    
    if (mnext != NULL)  /*i.e, not last */
	mnext->prev = mprev;
    
    if (mprev != NULL)  /*i.e, not first */
	mprev->next = mnext;
    
    if (snext != NULL)
	snext->prev = moveItem;
    
    moveItem->next = stayItem->next;
    moveItem->prev = stayItem;
    stayItem->next = moveItem; 
}


/*------- Hv_SendItemToBack ------*/

void Hv_SendItemToBack(Hv_Item Item) {
    
/* move an item to the start of the list (so it is drawn first
   and appears to be in the back */
    
/* NOTE: if this item is a child, send it to the very front of the list.
   If is an orphan send it behind its views's brickwallitem,  hotrectborder or container */
    
    Hv_ItemList   items;
    Hv_View       View;
    Hv_Item       prev, next;
    
    if (Item == NULL)
	return;
    
    View = Item->view;
    
    if (Item == View->brickwallitem)
	return;
    
/* is this a child item? */
    
    if (Item->parent == NULL) {
	if (Item->view->brickwallitem != NULL)
	    Hv_MoveItemBehindItem(Item, View->brickwallitem);
	else if (Item->view->hotrectborderitem != NULL)
	    Hv_MoveItemBehindItem(Item, View->hotrectborderitem);
	else
	    Hv_MoveItemBehindItem(Item, View->containeritem);
	return;
    }
    
    items = Item->parent->children;
    
    if (Item == items->first)
	return;
    else {
	
/* no chance of prev being NULL */
	
	prev = Item->prev;
	next = Item->next;
	
	prev->next = next;
	if (next == NULL)
	    items->last = prev;
	else
	    next->prev = prev;
	
	Item->next = items->first;
	items->first->prev = Item;
	items->first = Item;
	Item->prev = NULL;
    }
}

/*------- Hv_SendItemToFront ------*/

void Hv_SendItemToFront(Hv_Item Item) {
    
/* move an item to the end of the list (so it is drawn last
   and appears to be in the front */
    
    Hv_ItemList   items;
    
    if (Item == NULL)
	return;
    
/* is this a child item? */
    
    if (Item->parent == NULL)
	items = Item->view->items;
    else
	items = Item->parent->children;
    
    if (Item == items->first)
	return;
    
/* normal case */
    
    Item->prev->next = Item->next;     /* connect one before to one after */
    
    if (Item->next == NULL)            /* Item was last if this is true */
	items->last = Item->prev;
    else
	Item->next->prev = Item->prev;   /* connect one after to one before */
    
    items->first->prev = Item;         /* Item now in front of he who used to be first */
    Item->next = items->first;         /* next is he who used to be first */
    items->first = Item;               /* since it is the first! */
    Item->prev  = NULL;                /* since there is no one before me! */
    
/* normal case */
    
    Item->next->prev = Item->prev;     /* connect one after to one before */
    
    if (Item->prev == NULL)            /* Item was first if this is true */
	items->first = Item->next;
    else
	Item->prev->next = Item->next;   /* connect one before to one after */
    
    items->last->next = Item;          /* Item now follows he who used to be last */
    Item->prev  = items->last;         /* previous is he who used to be last */
    items->last = Item;                /* since it is the last! */
    Item->next  = NULL;                /* since there is no one after me! */
}

/*----- Hv_DumpItemList ------ */

void Hv_DumpItemList(Hv_ItemList  items,
		     FILE        *fp,
		     int          nspace) {
    Hv_Item       temp;
    Hv_Rect       *r;
    char          *spacer;
    char          itname[12];
    int           i;
    
    if ((items == NULL) || (fp == NULL))
	return;
    
    spacer = (char *)Hv_Malloc(nspace+1);
    for (i = 0; i < nspace; i++)
	spacer[i] = ' ';
    spacer[nspace] = '\0';
    
    fprintf(fp, "%s   type   \t tag\tdraw?\tdomain\tregion\t    AREA [L,T,W,H]\n", spacer);
    for (temp = items->first;  temp != NULL;	temp = temp->next) {
	r  = temp->area;
	Hv_ItemName(temp, itname);
	fprintf(fp, "%s%-10s\t%4d\t%4d\t",
		spacer, itname, temp->tag, Hv_CheckItemToDraw(temp));
	
	if (temp->domain == Hv_INSIDEHOTRECT)
	    fprintf(fp, "IN_HR \t");
	else
	    fprintf(fp, "OUT_HR\t");
	
	
	fprintf(fp, "%p\t[%4d,%4d,%4d,%4d]  ", temp->region,
		r->left, r->top, r->width, r->height); 
	
	if ((temp->str != NULL) && (temp->str->text != NULL))
	    fprintf(fp, " \"%s\"\n", temp->str->text);
	else
	    fprintf(fp, "\n");
	
/* sublist ? */
	
	if (temp->children != NULL) {
	    fprintf(fp, "*****  CHILDREN OF ABOVE ITEM *****\n");
	    Hv_DumpItemList(temp->children, fp, nspace+2);
	    fprintf(fp, "**********\n");
	}
	
    }
    
    Hv_Free(spacer);
}

/**
 * Hv_DeleteItem
 * @purpose   Call to delete an item.
 * @param  Item     The Item to delete.
 */

void  Hv_DeleteItem(Hv_Item Item) {

    Hv_View    View;
    Hv_Region  region;

/*
 * mod in 1.0052, check for locked item
 */
    
    if ((Item == NULL) || Hv_IsItemLocked(Item))
	return;
    
    Hv_Flush();
    
    View = Item->view;

/*
 * mod in 1.0052, check for view visibility
 */
    
    if (Hv_ViewIsVisible(View)) {
	region = Hv_CopyItemRegion(Item, True);
	Hv_ShrinkRegion(region, -2, -2);
	Hv_DestroyItem(Item);
	Hv_DrawView(View, region);
	Hv_KillRegion(&region);
    }
    else {
	Hv_DestroyItem(Item);
    }
}


/*------- Hv_DestroyItem ------*/

void Hv_DestroyItem(Hv_Item Item) {
    
/* does NO redrawing */
/* ASSUMES mydata field deleted ELSEWHERE */
    

    Hv_ItemList     items;
    if (Item == NULL)
	return;
    
    if (Item->parent != NULL)
	items = Item->parent->children;
    else
	items = Item->view->items;
    
    if (Item->children != NULL) {
	Hv_KillItemList(Item->children);
	Hv_Free(Item->children);
	Item->children = NULL;
    }
    
/* remove this item from any connection lists */
    
    if ((Item->domain == Hv_INSIDEHOTRECT) && (Item->type != Hv_CONNECTIONITEM))
	Hv_RemoveConnections(Item, -1, False, False, True);
    
    
    if (Item->prev != NULL)  /* if there is a previous item to reset */
	Item->prev->next = Item->next;
    
    if (Item->next != NULL)  /* if there is a next item to reset */
	Item->next->prev = Item->prev;
    
    if (items->first == Item)   /* if it is the first item in the list */
	items->first = Item->next;
    
    if (items->last == Item)    /* if it is the last item in the list */
	items->last = Item->prev;
    
    Hv_KillItem(Item);
}

/*-------- Hv_EnableItem --------------*/

void    Hv_EnableItem(Hv_Item Item)
    
{
    Hv_Item temp;

    if (Item == NULL)
	return;
    
    Hv_ClearBit(&(Item->drawcontrol), Hv_NOPOINTERCHECK);

/* children too */

  if (Item->children == NULL)
    return;
    
  for (temp = Item->children->first; temp != NULL; temp = temp->next)
    Hv_EnableItem(temp);

}

/*-------- Hv_DisableItem --------------*/

void     Hv_DisableItem(Hv_Item Item)
    
{
    Hv_Item temp;

    if (Item == NULL)
	return;
    
    Hv_SetBit(&(Item->drawcontrol), Hv_NOPOINTERCHECK);

/* children too */

  if (Item->children == NULL)
    return;
    
  for (temp = Item->children->first; temp != NULL; temp = temp->next)
    Hv_DisableItem(temp);
}

/*----------- Hv_EnableAllItems --------*/

void    Hv_EnableAllItems(Hv_View View)
    
{
    Hv_Item   temp;
    
    if (View == NULL)
	return;
    
    
    for (temp = View->items->first; temp != NULL; temp = temp->next)
	if ((temp->type != Hv_CONTAINERITEM) &&
	    (temp->type != Hv_HOTRECTBORDERITEM) &&
	    (temp->type != Hv_BOXITEM) &&
	    (temp->type != Hv_MAPITEM) &&
	    (temp->type != Hv_FEEDBACKITEM) &&
	    (temp->type != Hv_RADIOCONTAINERITEM)) {
	    
	    Hv_EnableItem(temp);
	    
	}
    
}

/*----------- Hv_DisableAllItems --------*/

void     Hv_DisableAllItems(Hv_View View)
    
{
    Hv_Item   temp;
    
    if (View == NULL)
	return;
    
    for (temp = View->items->first; temp != NULL; temp = temp->next)
	if ((temp->type != Hv_CONTAINERITEM) &&
	    (temp->type != Hv_HOTRECTBORDERITEM) &&
	    (temp->type != Hv_BOXITEM) &&
	    (temp->type != Hv_MAPITEM) &&
	    (temp->type != Hv_FEEDBACKITEM) &&
	    (temp->type != Hv_RADIOCONTAINERITEM)) {
	    
	    Hv_DisableItem(temp);
	    
	}
    
}


/*-------- Hv_SetItemSensitivity --------------*/

void Hv_SetItemSensitivity(Hv_Item  Item,
			   Boolean  val,
			   Boolean  Redraw)
    
/* a newer and more consistent version of the
   enable/disable functions. If Redraw is true,
   the item will be redrawn */
    
{
    if (Item == NULL)
	return;
    
    if (val)
	Hv_EnableItem(Item);
    else
	Hv_DisableItem(Item);
    
    if (Redraw)
	Hv_RedrawItem(Item);
}


/*-------- Hv_ItemIsEnabled --------------*/

Boolean  Hv_ItemIsEnabled(Hv_Item Item)
    
{
    if (Item == NULL)
	return False;
    
    return !Hv_CheckBit(Item->drawcontrol, Hv_NOPOINTERCHECK);
}

/**
 * Hv_KillItemList
 * @purpose  Delete all the Hv_Items on an Hv_ItemList.
 * @param   The item list to delete
 */

void      Hv_KillItemList(Hv_ItemList items) {
    
    Hv_Item	temp, tempnext;
    
    if (items == NULL)
	return;
    
    temp = items->first;
    
    if (temp != NULL) {
	do {
	    tempnext = temp->next;
	    
	    if (temp->children != NULL)
		Hv_KillItemList(temp->children);
	    
/* free space allocated to auxillary pointers, if any */
	    
	    Hv_KillItem(temp);
	    temp = tempnext;
	}
	while (temp != NULL);
    }
    
/* list head should now reflect an empty list */
    
    items->first = NULL;
    items->last = NULL;
}

/* ---------- Hv_OffsetItem ------*/

void Hv_OffsetItem(Hv_Item    Item,
		   short      dh,
		   short      dv,
		   Boolean    children)
    
/* offset a drawn item by dh and dv */
    
{
    
/* offset the area values */
    
    if (Item == NULL)
	return;
    
    Hv_OffsetRect(Item->area,  dh, dv);
    Hv_FixRectWH(Item->area);  /*should be unnecessary*/
    
/* offset region, if present */
    
    if (Item->region != NULL)
	Hv_OffsetRegion(Item->region, dh, dv);
    
/* specialized offset ? */
    
    if (Item->afteroffset != NULL)
	Item->afteroffset(Item, dh, dv);
    
    if (children)
	if (Item->children != NULL)
	    Hv_OffsetItemList(Item->children, dh, dv);
    
}


/* ---------- Hv_OffsetItemList ------*/

void Hv_OffsetItemList(Hv_ItemList   items,
		       short         dh,
		       short         dv)
    
/* Offset all the item's in the list items by dh right & dv down */
    
{
    Hv_Item       temp;  /* pointer to the item in the list */
    
    if (items == NULL)
	return;
    
/* go through the list offsetting the item */
    
    for (temp = items->first; temp != NULL; temp = temp->next)
	Hv_OffsetItem (temp, dh, dv, True);
}

/* ---------- Hv_OffsetHotRectItems ------*/

void Hv_OffsetHotRectItems(Hv_ItemList   items,
			   short        dh,
			   short        dv) {
    
/* Offset all the hotrect item's in the list items by dh right & dv down */
    
  Hv_Item       temp;  /* pointer to the item in the list */
    
  if (items == NULL)
    return;
    
/* go through the list offsetting the item */
  
  for (temp = items->first; temp != NULL; temp = temp->next)
    if (temp->domain == Hv_INSIDEHOTRECT)
      Hv_OffsetItem (temp, dh, dv, True);
}

/* ---------- Hv_ResizeItem ------*/

void Hv_ResizeItem (Hv_Item     Item,
		    short       dh,
		    short       dv,
		    Boolean     children) {
    
/*Resize an item, basing the resizing algorithm on its drawcontrol*/
    
  Hv_View View = Hv_GetItemView(Item);
  short  cx1, cx2;
  
  if (Item == NULL)
    return;
  
  if (Item->resizepolicy == Hv_OFFSETONRESIZE)
    Hv_OffsetItem(Item, dh, dv, False);  /* offset the proper width and height */
  
  else if (Item->resizepolicy == Hv_SHIFTXONRESIZE)
    Hv_OffsetItem(Item, dh, 0, False);   /* offset the width but not the height */
  
  else if (Item->resizepolicy == Hv_GROWXONRESIZE)
    Hv_ResizeRect(Item->area, dh, 0);
  
  else if (Item->resizepolicy == Hv_GROWYONRESIZE)
    Hv_ResizeRect(Item->area, 0, dv);
  
  else if (Item->resizepolicy == Hv_SHIFTYONRESIZE)
    Hv_OffsetItem(Item, 0, dv, False);   /* offset the height but not the width */
  
  else if (Item->resizepolicy == Hv_SIZEXOFFYONRESIZE) {
	
/* offset the height and resize the width */
	
    Hv_OffsetItem(Item, 0, dv, False);  
    Hv_ResizeRect(Item->area, dh, 0);
  }
    
  else if (Item->resizepolicy == Hv_RESIZEONRESIZE) /* resize the width and height */
    Hv_ResizeRect (Item->area, dh, dv);
  
  else if (Item->resizepolicy == Hv_RECENTERONRESIZE) {
    cx1 = View->hotrect->left+(short)((View->hotrect->width)/2);
    cx2 = Item->area->left+(short)((Item->area->width)/2);
    Hv_OffsetItem (Item, (short)(cx1 - cx2), dv, False);
  }
    
/* resize children ? */
    
  if (children)
    if (Item->children != NULL)
      Hv_ResizeItemList(Item->children, dh, dv);
}


void Hv_ResizeItemList(Hv_ItemList   items,
		       short         dh,
		       short         dv) {

  Hv_Item       temp;   /* temp item pointer */
    
  if (items == NULL)
    return;
    
/* go through the list offsetting the resizing the item */
    
  for (temp = items->first; temp != NULL; temp = temp->next)
    Hv_ResizeItem(temp, dh, dv, True);
}


/* ------- Hv_RepositionHotRectItems -------*/

void  Hv_RepositionHotRectItems(Hv_ItemList   items) {
    
/* repositions the hotrect items under the assumption that their
   WORLDAREA is ok but their PIXEL AREA is not. This, for example,
   is what happens as the result of a resize or a zoom */
    
  Hv_Item       temp;   /* temp drawn item pointer */
  float         fxc, fyc;
  short         xc, yc;
  short         newxc, newyc, dh, dv;
  Hv_View       View = NULL;
  
  Boolean     drawingchildren;
    
  if (items == NULL)
    return;
    
  if (items->first == NULL)
    return;
    
  View = items->first->view;
    
  drawingchildren = (View->items != items);

  for (temp = items->first; temp != NULL; temp = temp->next) {
    if (temp->domain == Hv_INSIDEHOTRECT) {
	    
/* zoomable items refers to those that change size as the result a zoom --
   most of the hot rect ites will be of this type */


      if (Hv_CheckBit(temp->drawcontrol, Hv_ZOOMABLE)) {
	  
	Hv_WorldRectToLocalRect(temp->view, temp->area, temp->worldarea);
	  
/* specialized region fix? */
		
	if (temp->fixregion != NULL)
	  temp->fixregion(temp);
      }
      else {  /* non zoomable */
	if (temp->fixregion != NULL)
	  temp->fixregion(temp);
	else {
	  fxc = (float)((temp->worldarea->xmin + temp->worldarea->xmax)/2.0);
	  fyc = (float)((temp->worldarea->ymin + temp->worldarea->ymax)/2.0);
	  Hv_WorldToLocal(temp->view, fxc, fyc, &newxc, &newyc);
	    
	  Hv_GetRectCenter(temp->area, &xc, &yc);
	    
	  dh = newxc - xc;
	  dv = newyc - yc;
	  Hv_OffsetItem(temp, dh, dv, False);
	  Hv_FixItemWorldArea(temp, False); 
	}
      }  /* end non-zoomable */
	    
/* go through any sublist repositioning the items */
	    
      if (temp->children != NULL)
	Hv_RepositionHotRectItems(temp->children);
    }
  }
   
  if (!drawingchildren)
    Hv_UpdateConnections(View, NULL, False); 
}

/*---- Hv_GetItemCenter ---------*/

void Hv_GetItemCenter(Hv_Item   Item,
		      short    *xc,
		      short    *yc) {
    Hv_Rect   r;
    
    if (Item == NULL)
	return;
    
    if ((Item->region != NULL) && !(Hv_CheckBit(Item->drawcontrol, Hv_CHECKAREAONLY))) {
	Hv_ClipBox(Item->region, &r);
	Hv_GetRectCenter(&r, xc, yc);
    }
    else
	Hv_GetRectCenter(Item->area, xc, yc);
}

/*---------- Hv_DrawItemList --------------- */

void	Hv_DrawItemList(Hv_ItemList	items,
			Hv_Region	region) {
  Hv_Item	temp;
  
  if (items == NULL)
    return;
    
  for (temp = items->first;  temp != NULL;  temp = temp->next) {
    if (!Hv_inPostscriptMode || 
	(temp->view->tag != Hv_XYPLOTVIEW) ||
	(Hv_printThisPlotOnly == NULL) ||
	(temp == Hv_printThisPlotOnly) ||
	(temp->parent == Hv_printThisPlotOnly)) {
	    
	    
      Hv_MaybeDrawItem(temp, region);
      
/* sublist? */
	    
      if (temp->children != NULL)
	Hv_DrawItemList(temp->children, region);
    }
  }
}

/* ------------ Hv_UpdateItem ---------- */

void  Hv_UpdateItem(Hv_Item Item)
    
/* draw the visible area of the item given */
    
{
    Hv_View         View = Item->view;
    Hv_Region       region;
    
    if (Hv_ViewIsVisible(View)) {
	region = Hv_GetMinimumItemRegion(Item);
	if (!Hv_EmptyRegion(region)) {
	    Hv_DrawView(View, region);
	    Hv_FullClip();
	}
	
	Hv_KillRegion(&region);
    }
}


/*---------- Hv_GetItemFromTag --------------- */

Hv_Item Hv_GetItemFromTag(Hv_ItemList  items,
			  short        tag)
    
{
    Hv_Item    theItem;              /* returned */
    Hv_Item    temp;                /* for searching */
    
    theItem = NULL;
    
    if (items == NULL)
	return NULL;
    
    temp = items->first;
    
    while ((theItem == NULL) && (temp != NULL)) {
	
	if (Hv_GetItemTag(temp) == tag)
	    theItem = temp;
	
/* check sublist, if any */
	
	if ((theItem == NULL) && (temp->children != NULL))
	    theItem = Hv_GetItemFromTag(temp->children, tag);
	
	temp = temp->next;
    }
    return theItem;
}


/* ---------- Hv_VaCreateItem ----------- */


Hv_Item  Hv_VaCreateItem(Hv_View View, ...) {
    
    
    va_list           ap;            /* traverses the list */
    Hv_Item           Item = NULL;
    Hv_Item           parent;
    
    int               tags[Hv_MAXAUTOFBENTRIES];
    int               fonts[Hv_MAXAUTOFBENTRIES];
    int               colors[Hv_MAXAUTOFBENTRIES];
    char              *optlist[Hv_MAXCHOICE];
    int               i;

    Boolean           wasplaced = False;
    

    Hv_AttributeArray attributes;
    
    va_start(ap, View);
    
/* now get the variable arguments */
    
    Hv_GetAttributes(ap, attributes, optlist, tags, fonts, colors, &Hv_vaNumOpt);
    
/* set the arrays to global variables so that all initialization routines
   have access */
    
    
    if (Hv_vaNumOpt > 0) {
	Hv_vaText = optlist;
	Hv_vaTags = tags;
	Hv_vaFonts = fonts;
	Hv_vaColors = colors;
    }
    
/* overwrite some defaults */
    
    if (attributes[Hv_DRAWCONTROL].i < 0)
	attributes[Hv_DRAWCONTROL].i = 0;
    
    if (attributes[Hv_DOMAIN].s < 0)
	attributes[Hv_DOMAIN].s = Hv_DefaultDomain(attributes[Hv_TYPE].s);
    
    if (attributes[Hv_INITIALIZE].fp == NULL)
	attributes[Hv_INITIALIZE].fp = Hv_DefaultInitialize(attributes[Hv_TYPE].s);
    
    if (attributes[Hv_DESTROY].fp == NULL)
	attributes[Hv_DESTROY].fp = Hv_DefaultDestroy(attributes[Hv_TYPE].s);
    
/* ALL HOTRECT ITEMS USE DEFAULT RESIZE POLICY */
    
    if ((attributes[Hv_DOMAIN].s == Hv_INSIDEHOTRECT) || (attributes[Hv_RESIZEPOLICY].s < 0))
	attributes[Hv_RESIZEPOLICY].s = Hv_DefaultResizePolicy(attributes[Hv_DOMAIN].s);
    
    parent = (Hv_Item)(attributes[Hv_PARENT].v);
    Item = Hv_CreateItem(View, attributes);
    
/* see if any user init */
    
    if (Item->initialize != NULL)
	Item->initialize(Item, attributes);
    
/* user int might have set data ptr */
    
    if (Item->data == NULL)
	Item->data = (void *)(attributes[Hv_DATA].v);
    
    
/* if an object is going to live in the hotrect and has not been given a singleclick
   callback, give it the standard bring-to-front callback */
    
    if ((Item->singleclick == NULL) && (Item->domain ==  Hv_INSIDEHOTRECT))
	Item->singleclick = Hv_DefaultSingleClickCallback;
    
/* some things are only relevant if auxillary space was created */
    
    Hv_ItemStringAttributes(Item, attributes);
    
/* redraw timer ?  */
    
    if (attributes[Hv_REDRAWTIME].l > 0)
	Hv_SetItemTimedRedraw(Item, attributes[Hv_REDRAWTIME].l);
    
/* child placement ? (CHILDDH and CHILDV have lower precedence) */
    
    if ((Item->parent != NULL) && (attributes[Hv_CHILDDH].s > -32000)) {
	Item->area->left = Item->parent->area->left + attributes[Hv_CHILDDH].s;
	Hv_FixRectRB(Item->area);
        wasplaced = True;
    }  
    
    if ((Item->parent != NULL) && (attributes[Hv_CHILDDV].s > -32000)) {
	Item->area->top = Item->parent->area->top + attributes[Hv_CHILDDV].s;
	Hv_FixRectRB(Item->area);
        wasplaced = True;
    }
    
    if ((parent != NULL) && (attributes[Hv_CHILDPLACEMENT].i >= 0)) {
        Hv_PositionChildItem(Item, attributes[Hv_CHILDPLACEMENT].i,
	  		   attributes[Hv_PLACEMENTGAP].s);
        wasplaced = True;
    }
    
/* relative placement? (relative to relItem, or prev if null) */
    
    if (attributes[Hv_RELATIVEPLACEMENT].i >= 0) {
        Hv_PositionItemRelative(Item, (Hv_Item)(attributes[Hv_PLACEMENTITEM].v),
			      attributes[Hv_RELATIVEPLACEMENT].i,
			      attributes[Hv_PLACEMENTGAP].s, False);
        wasplaced = True;
    }
    
/* some final touches:
   
   1) if Hv_FANCYDRAG is set, then Hv_DRAGGABLE is forced on as well
   2) if Hv_FEEDBACKWHENDRAGGED is set, then Hv_DRAGGABLE is forced on as well
   3) certain items MUST have Hv_NOPOINTERCHECK bit set
   4) if w or h was not provided for button items (but text was), then auto size it
   5) check that button is big enough for its text
   
   */
    
    if (Hv_CheckBit(Item->drawcontrol, Hv_FANCYDRAG) ||
	Hv_CheckBit(Item->drawcontrol, Hv_FEEDBACKWHENDRAGGED))
	Hv_SetBit(&(Item->drawcontrol), Hv_DRAGGABLE);
    
    if ((Item->type == Hv_FEEDBACKITEM)
	|| (Item->type == Hv_HOTRECTBORDERITEM)
	|| (Item->type == Hv_BOXITEM))
	Hv_SetBit(&(Item->drawcontrol), Hv_NOPOINTERCHECK);
    
    if ((Item->type == Hv_CLOCKITEM) && (Item->str->text != NULL))
	if ((Item->area->width < 2) || (Item->area->height < 2)) 
	    Hv_SizeTextItem(Item, Item->area->left, Item->area->top);

/* size the text item unless it has been placed already */

    if (!wasplaced && (Item->type == Hv_TEXTITEM))
      Hv_SizeTextItem(Item, Item->area->left, Item->area->top);
    
/* all hot rect items should have their world area set properly */
    
    if (Item->domain == Hv_INSIDEHOTRECT)
	Hv_FixItemWorldArea(Item, False);
    
    
/* resize based on feedback item? */
    
    if (Item->type == Hv_FEEDBACKITEM) {
	if ((attributes[Hv_FEEDBACKMARGIN].s >= 0)
	    && (attributes[Hv_FEEDBACKMARGIN].s < 1000))
	    Hv_FitFeedback(View, attributes[Hv_FEEDBACKMARGIN].s); 
    }
    
/* if any option strings, delete them */
    
    for (i = 0; i < Hv_vaNumOpt; i++)
	Hv_Free(Hv_vaText[i]);
    
/* draw at creation? */
    
    if (attributes[Hv_DRAWATCREATION].s) 
	Hv_RedrawItem(Item);
    
    return Item;
}


/* ---------- Hv_ItemStringAttributes ----------- */

void Hv_ItemStringAttributes(Hv_Item          Item,
			     Hv_AttributeArray attributes) {
    
/* space must already be set */
    
    if ((Item == NULL) || (Item->str == NULL) ||
	(Item->type == Hv_OPTIONBUTTONITEM))
	return;
    
/* if text is not NULL, then assume initialization routine
   has already handled this */
    
    if (Item->str->text != NULL)
	return;
    
    Item->str->font = Hv_sMax(0, attributes[Hv_FONT].s);
    
    if (attributes[Hv_TEXTCOLOR].s >= 0)
	Item->str->strcol = attributes[Hv_TEXTCOLOR].s;
    
    Item->str->fillcol = attributes[Hv_FILLCOLOR].s;
    if (attributes[Hv_TEXT].v != NULL)
	Hv_InitCharStr(&(Item->str->text), (char *)(attributes[Hv_TEXT].v));
}

/* ---------- Hv_CreateItem ----------- */

static Hv_Item Hv_CreateItem(Hv_View           View,
			     Hv_AttributeArray attributes) {

    Hv_Item	       Item;		/* returned Item pointer */
    
/* create a new Item and add it to the end of the list*/
    
    Item = (Hv_Item) Hv_Malloc(sizeof(Hv_ItemData));	/* beg for the memory*/
    
    Item->view = View;
    Item->standarddraw = NULL;
    Item->parent   = (Hv_Item)(attributes[Hv_PARENT].v);
    Item->userptr = NULL;
    Item->userptr2 = NULL;

    
    Item->type = attributes[Hv_TYPE].s;	    /* set type to requested type*/
    Item->drawcontrol = attributes[Hv_DRAWCONTROL].i;          /* set to requested drawcontrol*/
    
    Item->singleclick = (Hv_FunctionPtr)(attributes[Hv_SINGLECLICK].fp);
    Item->doubleclick = (Hv_FunctionPtr)(attributes[Hv_DOUBLECLICK].fp);
    Item->destroy     = (Hv_FunctionPtr)(attributes[Hv_DESTROY].fp);
    Item->userdraw    = (Hv_FunctionPtr)(attributes[Hv_USERDRAW].fp);
    Item->initialize  = (Hv_FunctionPtr)(attributes[Hv_INITIALIZE].fp);
    Item->afterdrag   = (Hv_FunctionPtr)(attributes[Hv_AFTERDRAG].fp);
    Item->afterrotate = (Hv_FunctionPtr)(attributes[Hv_AFTERROTATE].fp);
    Item->afteroffset = (Hv_FunctionPtr)(attributes[Hv_AFTEROFFSET].fp);
    Item->fixregion   = (Hv_FunctionPtr)(attributes[Hv_FIXREGION].fp);
    Item->checkenlarge = (Hv_FunctionPtr)(attributes[Hv_CHECKENLARGE].fp);
    
    Item->pattern = attributes[Hv_PATTERN].s;
    Item->color   = attributes[Hv_COLOR].s;
    Item->state   = attributes[Hv_STATE].s;
    Item->tag     = attributes[Hv_TAG].s;
    
    Item->user1 = attributes[Hv_USER1].i;
    Item->user2 = attributes[Hv_USER2].i;
    Item->userptr = attributes[Hv_USERPTR].v;
    
/* children inherit domain and resize policy */
    
    if (Item->parent == NULL) {
	Item->resizepolicy = attributes[Hv_RESIZEPOLICY].s;        /* as requested */
	Item->domain = attributes[Hv_DOMAIN].s;                    /* as requested */
    }
    else {
	Item->resizepolicy = Item->parent->resizepolicy;
	Item->domain = Item->parent->domain;
    }

    Item->rotaterp = NULL;
    Item->worldarea = NULL;                   /* default no world area */
    Item->next = NULL;			    /* default next field points nowhere */
    Item->prev = NULL;			    /* default prev field points nowhere */
    Item->str = NULL;			    /* default no text data */
    Item->data = NULL;		            /* default no auxillary data */
    Item->drawinterval = NULL;                /* default no timed redraw */
    Item->children = NULL;                    /* children must be added later */
    
    Item->azimuth = 0.0;
    Item->region = NULL;                      /*default no drawing region*/
    Item->scheme = NULL;                      /*default no color scheme*/
    
/* default to a small area */
    
    Item->area = Hv_NewRect();
    
/* create a balloon? */
    
    if (attributes[Hv_BALLOON].v != NULL)
	Hv_CreateBalloon(Item, (char *)(attributes[Hv_BALLOON].v));
    else
	Item->balloon = NULL;
    
/* change the area ? */
    
    if (attributes[Hv_WIDTH].s > 0)
	Item->area->width = attributes[Hv_WIDTH].s;
    if (attributes[Hv_HEIGHT].s > 0)
	Item->area->height = attributes[Hv_HEIGHT].s;
    
    if (attributes[Hv_LEFT].s > -32000)
	Item->area->left = attributes[Hv_LEFT].s;
    if (attributes[Hv_TOP].s  > -32000)
	Item->area->top = attributes[Hv_TOP].s;
    
    Hv_FixRectRB(Item->area);
    
/* rotatable ? */
    
    if (Hv_CheckBit(Item->drawcontrol, Hv_ROTATABLE))
	Item->rotaterect = Hv_NewRect();
    else
	Item->rotaterect = NULL;


    Item->base = NULL;

    
/* now do some Item specific allocation */
    
    switch (Item->type) {

    case	Hv_WORLDIMAGEITEM:
      Item->str = Hv_CreateString(NULL);
	
    case	Hv_CLOCKITEM:
	Item->standarddraw = Hv_DrawClockItem;
	Item->str = Hv_CreateString(NULL);
	Hv_DefaultSchemeOut(&(Item->scheme));
	Item->scheme->framewidth  = 2;        /* in this case it is the width of the 3d frame */
	Item->scheme->borderwidth = 2;        /* in this case it is the spacer between text and frame */
	break;
	
    case    Hv_HOTRECTBORDERITEM:
	Item->standarddraw = Hv_DrawHotRectBorderItem;
	if (View->hotrectborderitem != NULL)
	    Hv_Warning("Presently only one hotrect border  per view is supported.");
	else
	    View->hotrectborderitem = Item;
	break;
	
    case    Hv_CONTAINERITEM:
	Item->standarddraw = Hv_DrawContainerItem;
	Hv_DefaultSchemeOut(&(Item->scheme));
	Item->scheme->borderwidth = 15;      /* width of the border for special buttons i.e. drag,close,scroll,etc. */
	Item->scheme->framewidth  = 3;       /* width of the 3d frame */ 
	
/* container also has text for its title */
	
	Item->str = Hv_CreateString(NULL);
	break;
	
    default:
	break;
    }
    
/* is this a stand alone item or a child? */
    
    if (Item->parent) {
	if (Item->parent->children == NULL) {
	    Item->parent->children = (Hv_ItemList)Hv_Malloc(sizeof(Hv_ItemListData));
	    Item->parent->children->first = NULL;
	    Item->parent->children->last = NULL;
	}
	Hv_AddItemToList(Item->parent->children, Item);      /*Item to end of parent's list */
    }
    else
	Hv_AddItemToList(View->items, Item);	       /*Item to end of view's list */
    
    return Item;
}



/*------- Hv_CopyItemRegion ------*/

Hv_Region Hv_CopyItemRegion(Hv_Item  Item,
			    Boolean  IncludeChildren) {
    
/* mallocs and returns a copy of the item's region */
    
  Hv_Region    region, hrr;
  Hv_Rect      area;
  Hv_Item      child;
  Hv_Region    diffregion, childregion, sumregion;
  
  if (Item == NULL)
    return NULL;
  
  if (!Hv_ViewIsVisible(Item->view))
    return NULL;
    
/* if the item is using its region field, copy that. 
   Otherwise create a region from the area */
    
  if (Item->region == NULL) {
    Hv_CopyRect(&area, Item->area);
    Hv_ResizeRect(&area, 1, 1);
    region = Hv_RectRegion(&area);
  }
  else {
    Hv_CopyRegion(&region, Item->region);
	
/* this hack helps for rare items where the region and
   and area are kept separate */
	
    if (Hv_CheckItemDrawControlBit(Item, Hv_CHECKAREAOL1ST))
      Hv_UnionRectWithRegion(Item->area, region);
  }
    
/* include child regions? */
    
  if (IncludeChildren && (Item->children != NULL))
    for (child = Item->children->first; child != NULL; child = child->next) {
      childregion = Hv_CopyItemRegion(child, True);
      sumregion = Hv_UnionRegion(region, childregion);
      Hv_KillRegion(&childregion);
      Hv_KillRegion(&region);
      region = sumregion;
    }
    
/* of a hot rect item, intersect with hot rect */
    
  if (Item->domain == Hv_INSIDEHOTRECT) {
    hrr = Hv_HotRectRegion(Item->view);
    diffregion = Hv_IntersectRegion(hrr, region);
    Hv_KillRegion(&hrr);
    Hv_KillRegion(&region);
    region = diffregion;
  }
    
  return  region;
}

/* -------- Hv_AddItemToList -------------*/

void Hv_AddItemToList(Hv_ItemList	items,
		      Hv_Item    	Item) {
    
/* add an Item to the end of the list. Note: when allocated,
   the Items next and prev field are set to NULL --- see CreateItem. */
    
    if (!items->last)	/*i.e 1st member of list */
	items->first = Item;
    else	{  /* add to end and make links */
	items->last->next = Item;
	Item->prev = items->last;
    }
    
/* in any event, listhead.last should point to newly added guy */
    
    items->last = Item;
}

/*------- Hv_CheckItemOverlap ------*/

static Boolean Hv_CheckItemOverlap(Hv_Item   Item,
				   Hv_Region  region) {
    
/* checks if the item's drawing region overlaps
   "region", which is presumably an update region.
   If the item's region is NULL (which it will
   be for simple rectangular items, it checks
   the items area instead if its region */
    
    Hv_Region     testreg;
    Boolean       result = False;
    
    if ((Hv_CheckBit(Item->drawcontrol, Hv_ALWAYSREDRAW))
	&& (Item->domain == Hv_INSIDEHOTRECT))
	return Hv_RectInRegion(Item->view->hotrect, region);
    
/*******************************
  
  Algorithm modified 8/31/96:
  
  1) check area first
  2) if fails, and region exists, check region
  
  *************************************/
    
    if ((Item->region == NULL) || Hv_CheckItemDrawControlBit(Item, Hv_CHECKAREAOL1ST))
	result = Hv_RectInRegion(Item->area, region);
    
    if (!result && Item->region) {
	testreg = Hv_IntersectRegion(region, Item->region);
	result = !Hv_EmptyRegion(testreg);
	Hv_DestroyRegion(testreg);
    }
    
    return result;
    
}

/**
 * Hv_DefaultSchemeOut
 * @purpose   Allocates an initializes a color scheme
 * whose 3D effect will be to stick "out".
 * @param    scheme    Upon return, points to newly allocated scheme.
 */

void Hv_DefaultSchemeOut(Hv_ColorScheme  **scheme) {
    
/* sets scheme to the default color scheme for "outsies" */
    
    short Top_Color       = Hv_gray13;
    short Bottom_Color    = Hv_gray6;
    short Fill_Color      = Hv_gray10;
    short LightTrim_Color = Hv_white;
    short DarkTrim_Color  = Hv_black; 
    short Hot_Color       = Hv_gray10;
    
    *scheme = (Hv_ColorScheme *)Hv_Malloc(sizeof(Hv_ColorScheme));
    
    (*scheme)->topcolor = Top_Color;
    (*scheme)->bottomcolor = Bottom_Color;
    (*scheme)->fillcolor = Fill_Color;
    (*scheme)->lighttrimcolor = LightTrim_Color;
    (*scheme)->darktrimcolor = DarkTrim_Color;
    (*scheme)->hotcolor = Hot_Color;
    (*scheme)->framewidth  = 2;          /* in this case it is the width of the 3d frame */
    (*scheme)->borderwidth = 2;          /* in this case it is the spacer between text and frame */
}

/**
 * Hv_DefaultSchemeIn
 * @purpose   Alocates an initializes a color scheme
 * whose 3D effect will be to be pushed "in".
 * @param    scheme    Upon return, points to newly allocated scheme.
 */

void Hv_DefaultSchemeIn(Hv_ColorScheme  **scheme) {
    
/* sets scheme to the default color scheme for "insies" */
    
    Hv_DefaultSchemeOut(scheme);
    Hv_InvertColorScheme(*scheme);
}


/*-------- Hv_MaybeDrawItem ------------*/

void Hv_MaybeDrawItem(Hv_Item 	Item,
		      Hv_Region	region) {
    
/* Draws the Item only if necessary -- based on supplied clip region */
    
    if (Item == NULL)
	return;
    
/* the "this plot only" for plot views is a special case */
    
/*  if (Hv_inPostscriptMode && (Hv_printThisPlotOnly && 
    ((Hv_printThisPlotOnly == Item)
    || (Hv_printThisPlotOnly == Item->parent))))
    Hv_DrawItem(Item, region);
    
    else */

    
    if (Hv_CheckItemOverlap(Item, region)) {
	Hv_DrawItem(Item, region);
    }

}

/*------ Hv_ItemName -----*/

void  Hv_ItemName(Hv_Item Item,
		  char   *name) {
    
/* purely a diagnostic procedure used by DumpList */
    
    if (Item == NULL) {
	strcpy(name, "NULLITEM");
	return;
    }
    
    switch (Item->type) {
    case Hv_USERITEM:
	if (Hv_userItemName != NULL)
	    Hv_userItemName(Item, name);
	else
	    strcpy(name, "USER");
	break;
	
    case Hv_RAINBOWITEM:
	strcpy(name, "RAINBOW");
	break;
	
    case Hv_SMALLICONITEM:
	strcpy(name, "SMALLICON");
	break;
	
    case Hv_COLORBUTTONITEM:
	strcpy(name, "COLORBTN");
	break;
	
    case Hv_CONTAINERITEM:
	strcpy(name, "CONTAINER");
	break;
	
    case Hv_FEEDBACKITEM:
	strcpy(name, "FEEDBACK");
	break;
	
    case Hv_BUTTONITEM:
	strcpy(name, "BUTTON");
	break;
	
    case Hv_HOTRECTBORDERITEM:
	strcpy(name, "HRBORDER");
	break;
	
    case Hv_RADIOBUTTONITEM:
	strcpy(name, "RADIOBTN");
	break;
	
    case Hv_OPTIONBUTTONITEM:
	strcpy(name, "OPTIONBTN");
	break;
	
    case Hv_RADIOCONTAINERITEM:
	strcpy(name, "RADIOCNTR");
	break;
	
    case Hv_SLIDERITEM:
	strcpy(name, "SLIDER");
	break;
	
    case Hv_WHEELITEM:
	strcpy(name, "WHEEL");
	break;
	
    case Hv_SEPARATORITEM:
	strcpy(name, "SEPRATR");
	break;
	
    case Hv_CONNECTIONITEM:
	strcpy(name, "CONNECT");
	break;
	
    case Hv_SLIDERTHUMBITEM:
	strcpy(name, "SLIDTHUMB");
	break;
	
    case Hv_MAPITEM:
	strcpy(name, "MAP");
	break;
	
    case Hv_PLOTITEM:
	strcpy(name, "PLOT");
	break;
	
    case Hv_LEDBARITEM:
	strcpy(name, "LEDBAR");
	break;
	
    case Hv_WORLDLINEITEM:
	strcpy(name, "WRLDLINE");
	break;
	
    case Hv_WORLDPOLYGONITEM:
	strcpy(name, "WRLDPOLY");
	break;

    case Hv_WORLDIMAGEITEM:
	strcpy(name, "WRLDIMAGE");
	break;
	
    case Hv_BUTTONGROUPITEM:
	strcpy(name, "BUTTONGRP");
	break;
	
    case Hv_WORLDWEDGEITEM:
	strcpy(name, "WRLDWDGE");
	break;
	
    case Hv_WORLDOVALITEM:
	strcpy(name, "WRLDOVAL");
	break;
	
    case Hv_TEXTITEM:
	strcpy(name, "TEXT");
	break;
	
    case Hv_TEXTENTRYITEM:
	strcpy(name, "TEXTENTRY");
	break;
	
    case Hv_FRACTIONDONEITEM:
	strcpy(name, "FRACTDONE");
	break;
	
    case Hv_CLOCKITEM:
	strcpy(name, "CLOCK");
	break;
	
    case Hv_BOXITEM:
	strcpy(name, "BOX");
	break;
	
    case Hv_WORLDRECTITEM:
	strcpy(name, "WORLDRECT");
	break;
	
    default:
	strcpy(name, "UNKNWN");
    }
}



/*-------- Hv_KillItem --------------*/

static void Hv_KillItem(Hv_Item Item) {
    
/* note: does NOT kill the items pointed to by next and prev */
    
    Hv_FunctionPtr    tempdestroy;
    
    if(Item->destroy != NULL) {
	tempdestroy = Item->destroy;
	Item->destroy = NULL;
	tempdestroy(Item);
    }
    
    Hv_DestroyString(Item->str);
    
    Hv_Free(Item->balloon);
    Hv_Free(Item->scheme);
    Hv_Free(Item->area);
    Hv_Free(Item->base);
    Hv_Free(Item->rotaterect);
    Hv_Free(Item->rotaterp);
    
    Hv_RemoveItemTimedRedraw(Item);
    Hv_Free(Item->drawinterval);
    
/* hack below is for caps which uses the data field of
   text items */
    
    if ((Item->type != Hv_TEXTITEM) && (Item->type != Hv_TEXTENTRYITEM))
	Hv_Free(Item->data);
    Hv_Free(Item->worldarea);
    Hv_KillRegion(&Item->region);
    Hv_Free(Item->children);
    
    Hv_Free(Item);
    Hv_Flush();
}

/*-------- Hv_TimeDrawItem -------*/

static void    Hv_TimerDrawItem(Hv_Pointer      xtptr,
				Hv_IntervalId  *id) {
    Hv_Item    Item;
    Hv_Region  region = NULL;
    Hv_View    View;


    Item = (Hv_Item)xtptr;

    if (Item == NULL)
      return;


    View = Item->view;

 
/*
 * An insidious problem is that the view and/or
 * may have been deleted from under us.
*/


     if (!Hv_ItemExistsInView(View, Item)) {
	fprintf(stderr, "Exiting TimeDraw prematurely.\n");
	return; 
      }

	if (Hv_ViewIsVisible(View)) {

      region = Hv_GetItemDrawingRegion(Item, 0, 0);

/* keep a count of how many times this object was
   redrawn because of a timed redraw */
    
      Item->drawinterval->timedredrawcount++;

      Hv_timedRedraw = True;
 
      if (Item->domain == Hv_OUTSIDEHOTRECT) {

	Hv_DrawItem(Item, region);
      }
      else {
	if (Hv_CheckItemDrawControlBit(Item, Hv_JUSTMEONTIMER)) {
	  Hv_SendItemToFront(Item);
	  Hv_DrawItem(Item, NULL);
	}
	else 
	  Hv_DrawItemsAfterThisOne(Item, region);
      }
    
      Hv_timedRedraw = False;
    }

/* set a new timer */
    
    Hv_SetItemTimedRedraw(Item, Item->drawinterval->interval);
    Hv_KillRegion(&region);
}

/*-------- Hv_PositionItemRelative -------*/

static void    Hv_PositionItemRelative(Hv_Item   Item,
				       Hv_Item   relitem,
				       int       placement,
				       short     gap,
				       Boolean   center) {
    
/* positions an item relative to relitem */
    
    short        pleft, ptop, pright, pbottom, left, top;
    short        w, h, xc, yc;
    short        xo, yo, dh, dv;
    Boolean      roundbutton;
    
    if (Item == NULL)
	return;
    
    roundbutton = ((Item->type == Hv_BUTTONITEM) &&
		   Hv_CheckItemDrawControlBit(Item, Hv_ROUNDED));
    
    center = center || roundbutton;
    
    xo = Item->area->left;
    yo = Item->area->top;
    
/* if no placement item was provided, assume previous */
    
    if (relitem == NULL)
	relitem = Item->prev;
    
    if (relitem == NULL)
	return;
    
    Hv_GetRectLTRB(relitem->area, &pleft, &ptop, &pright, &pbottom);
    
    if (center || (placement == Hv_POSITIONCENTER))
	Hv_GetItemCenter(relitem, &xc, &yc);
    
    if ((Item->type == Hv_TEXTITEM) && !(Hv_CheckBit(Item->drawcontrol, Hv_DONTAUTOSIZE))) {
	Hv_CompoundStringDimension(Item->str, &w, &h);
	w += 2*Hv_STRINGEXTRA;
	h += 2*Hv_STRINGEXTRA;
      }
    else {
	w = Item->area->width;
	h = Item->area->height;
    }

    
    switch(placement) {
    case Hv_POSITIONCENTER:
	left = xc - w/2;
	top = yc - h/2;
	break;
	
    case Hv_POSITIONBELOW:
	if (center)
	    left = xc - w/2;
	else
	    left = pleft;
	top  = pbottom + gap;
	break;
	
    case Hv_POSITIONABOVE:
	if (center)
	    left = xc - w/2;
	else
	    left = pleft;

	top  = ptop - gap - h;
	break;
	
    case Hv_POSITIONLEFT:
	left = pleft - gap - w;
	if (center)
	    top = yc - h/2;
	else
	    top = ptop;
	break;
	
    case Hv_POSITIONRIGHT:
	left = pright + gap;
	if (center)
	    top = yc - h/2;
	else
	    top = ptop;
	break;
	
    default:
	left = xc;
	top  = yc;
	break;
    }
    
    
    if ((Item->type == Hv_TEXTITEM) && !(Hv_CheckBit(Item->drawcontrol, Hv_DONTAUTOSIZE)))
	Hv_SizeTextItem(Item, (short)(left+Hv_STRINGEXTRA), (short)(top+2*Hv_STRINGEXTRA));
    else
	Hv_SetRect(Item->area, left, top, w, h);
    
    if (Item->children != NULL) {
	dh = Item->area->left - xo;
	dv = Item->area->top  - yo;
	Hv_OffsetItemList(Item->children, dh, dv);
    }
}

/*------- Hv_DefaultInitialize -------*/

static Hv_FunctionPtr Hv_DefaultInitialize(short type) {
    
    
/************************************
  For many standard items, this sets
  the initialization routine so that
  it doesn't have to be provided by
  each and every Hv_VaCreateItem.
  ************************************/
    
    switch (type) {
	
    case Hv_MENUBUTTONITEM:
	return (Hv_FunctionPtr)Hv_MenuButtonInitialize;
	
    case Hv_BUTTONITEM:
	return (Hv_FunctionPtr)Hv_ButtonItemInitialize;
	
    case Hv_COLORBUTTONITEM:
	return (Hv_FunctionPtr)Hv_ColorButtonInitialize;
	
    case Hv_SLIDERITEM:
	return (Hv_FunctionPtr)Hv_SliderInitialize;
	
    case Hv_FEEDBACKITEM:
	return (Hv_FunctionPtr)Hv_FeedbackInitialize;
	
    case Hv_BOXITEM:
	return (Hv_FunctionPtr)Hv_BoxInitialize;
	
    case Hv_WHEELITEM:
	return (Hv_FunctionPtr)Hv_WheelInitialize;
	
    case Hv_SEPARATORITEM:
	return (Hv_FunctionPtr)Hv_SeparatorInitialize;
	
    case Hv_CONNECTIONITEM:
	return (Hv_FunctionPtr)Hv_ConnectionInitialize;
	
    case Hv_RAINBOWITEM:
	return (Hv_FunctionPtr)Hv_RainbowInitialize;
	
    case Hv_OPTIONBUTTONITEM:
	return (Hv_FunctionPtr)Hv_OptionButtonInitialize;
	
    case Hv_RADIOCONTAINERITEM:
	return (Hv_FunctionPtr)Hv_RadioContainerInitialize;
	
    case Hv_RADIOBUTTONITEM:
	return (Hv_FunctionPtr)Hv_RadioButtonInitialize;
	
    case Hv_TEXTITEM:
	return (Hv_FunctionPtr)Hv_TextItemInitialize;
	
    case Hv_PLOTITEM:
	return (Hv_FunctionPtr)Hv_PlotItemInitialize;
	
    case Hv_TEXTENTRYITEM:
	return (Hv_FunctionPtr)Hv_TextEntryItemInitialize;
	
    case Hv_FRACTIONDONEITEM:
	return (Hv_FunctionPtr)Hv_FractionDoneInitialize;
	
    case Hv_BUTTONGROUPITEM:
	return (Hv_FunctionPtr)Hv_ButtonGroupInitialize;
	
    case Hv_LEDBARITEM:
	return (Hv_FunctionPtr)Hv_LEDInitialize;
	
    case Hv_WORLDRECTITEM:
	return (Hv_FunctionPtr)Hv_WorldRectInitialize;
	
    case Hv_WORLDPOLYGONITEM:
	return (Hv_FunctionPtr)Hv_WorldPolygonInitialize;
	
    case Hv_WORLDIMAGEITEM:
	return (Hv_FunctionPtr)Hv_WorldImageInitialize;
	
    case Hv_WORLDWEDGEITEM:
	return (Hv_FunctionPtr)Hv_WorldWedgeInitialize;
	
/* world oval uses world rect's init */
	
    case Hv_WORLDOVALITEM:
	return (Hv_FunctionPtr)Hv_WorldRectInitialize;
	
    case Hv_WORLDLINEITEM:
	return (Hv_FunctionPtr)Hv_WorldLineInitialize;
	
    case Hv_MAPITEM:
	return (Hv_FunctionPtr)Hv_InitializeMap;
	
    default:
	return NULL;
    }
    
}

/*------- Hv_DefaultDestroy -------*/

static Hv_FunctionPtr  Hv_DefaultDestroy(short type) {
    
    
/************************************
  For many standard items, this sets
  the destroy routine so that
  it doesn't have to be provided by
  each and every Hv_VaCreateItem.
  ************************************/
    
    switch (type) {
	
    case Hv_MENUBUTTONITEM:
	return (Hv_FunctionPtr)Hv_DestroyMenuButtonItem;
	
    case Hv_BUTTONGROUPITEM:
	return (Hv_FunctionPtr)Hv_DestroyButtonGroup;
	
    case Hv_LEDBARITEM:
	return (Hv_FunctionPtr)Hv_DestroyLEDItem;
	
    case Hv_WORLDPOLYGONITEM:
	return (Hv_FunctionPtr)Hv_DestroyWorldPolygon;
	
    case Hv_WORLDIMAGEITEM:
	return (Hv_FunctionPtr)Hv_DestroyWorldImage;
	
    case Hv_WORLDWEDGEITEM:
	return (Hv_FunctionPtr)Hv_DestroyWorldWedge;
	
    case Hv_FEEDBACKITEM:
	return (Hv_FunctionPtr)Hv_DestroyFeedbackItem;
	
    case Hv_PLOTITEM:
	return (Hv_FunctionPtr)Hv_DestroyPlotItem;
	
    default:
	return NULL;
    }
    
}


/*------- Hv_DefaultDomain -------*/

static short    Hv_DefaultDomain(short type) {

    switch (type) {
    case Hv_USERITEM:
    case Hv_MAPITEM:
    case Hv_PLOTITEM:
	return Hv_INSIDEHOTRECT;
	
    default:
	return Hv_OUTSIDEHOTRECT;
    }
}


/*------- Hv_DefaultResizePolicy -------*/

static short    Hv_DefaultResizePolicy(short domain) {

    switch (domain) {
    case Hv_INSIDEHOTRECT:
	return Hv_DONOTHINGONRESIZE;
	
    default:
	return Hv_SHIFTXONRESIZE;
    }
}

/*--------- Hv_FixItemWorldArea ----------*/

void Hv_FixItemWorldArea(Hv_Item  Item,
			 Boolean  children) {
    
/* sets the item's world area to correspond to its pixel area */
    
    if (Item->domain != Hv_INSIDEHOTRECT)
	return;
    
    if (Item->worldarea == NULL) 
	Item->worldarea = Hv_NewFRect();
    
    Hv_LocalRectToWorldRect(Item->view, Item->area, Item->worldarea);
    
    if (children)
	if (Item->children != NULL)
	    Hv_FixItemListWorldArea(Item->children);
}

/*--------- Hv_FixItemListWorldArea ----------*/

static void Hv_FixItemListWorldArea(Hv_ItemList items) {
    
  Hv_Item       temp;  /* pointer to the item in the list */
  
  for (temp = items->first; temp != NULL; temp = temp->next) 
    Hv_FixItemWorldArea(temp, True);
}

/*---------- Hv_DrawItemsAfterThisOne --------------- */

static void    Hv_DrawItemsAfterThisOne(Hv_Item 	Item,
					Hv_Region	region) {
    
    Hv_Item	temp;
    
    if (region != NULL)
	Hv_SetClippingRegion(region);         /* set region to clipping region*/
    
    for (temp = Item;  temp != NULL;  temp = temp->next)
	if (temp->domain == Item->domain) {
	    Hv_MaybeDrawItem(temp, region);
	    
/* sublist? */
	    
	    if (temp->children != NULL)
		Hv_DrawItemList(temp->children, region);
	}
    
    
}


/*-------- Hv_PositionChildItem ------ */

static void    Hv_PositionChildItem(Hv_Item   Item,
				    int        policy,
				    short      gap) {
    
    if ((Item == NULL) || (Item->parent == NULL))
	return;
    
    if (Item->type == Hv_TEXTITEM) {
	if (Item->str == NULL)
	    return;
	gap += 2; 
    }
    
    Hv_PositionItemRelative(Item, Item->parent, policy, gap, True);
}


