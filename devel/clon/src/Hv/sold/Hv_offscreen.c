/*	
-----------------------------------------------------------------------
File:     Hv_offscreen.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"	                 /* contains all necessary include files */

extern Window  Hv_trueMainWindow;  /* unchanging (even while off screen draws) */


/*------ local prototypes ---------*/

static void Hv_DrawBackgroundItems(Hv_ItemList  items,
				   Hv_Region    region);

static void   Hv_SetViewClean(Hv_View  View);


/*--------- Hv_SaveUnder --------*/

extern Hv_Pixmap       Hv_SaveUnder(short x,
				    short y,
				    unsigned int  w,
				    unsigned int  h)

/***********************************************
  Saves the indicated area into a pixmap (which
  it allocates). This is used, for example, by
  the menu routines which must save what is
  underneath before popping up.
************************************************/

{
  Hv_Pixmap    pmap;

  pmap = Hv_CreatePixmap(w+2, h+2);

  XCopyArea(Hv_display, Hv_trueMainWindow, pmap, Hv_gc, x-1, y-1,
	    w+2, h+2, 0, 0);

  return pmap;
}

/*------- Hv_RestoreSaveUnder -----------*/

extern void            Hv_RestoreSaveUnder(Hv_Pixmap pmap,
					   short x,
					   short y,
					   unsigned int  w,
					   unsigned int  h)

/***********************************************
  Restore the indicated area from a pixmap, which
  was presumably created by a previous call to
  Hv_SaveUnder. This is used, for example, by
  the menu routines which must restore what was
  underneath after popping down.
************************************************/

{
  XCopyArea(Hv_display, pmap, Hv_trueMainWindow, Hv_gc, 0, 0,
	    w+2, h+2, x-1, y-1);

}


/*------ Hv_DrawItemOnPixmap ------*/

Hv_Pixmap  Hv_DrawItemOnPixmap(Hv_Item   Item,
			       Hv_Region region)

/***********************************
  Malloc a pixmap an draws an item.
  onto it. Does NOT draw children.
***********************************/

{
  Hv_Pixmap        pmap;
  Hv_Rect         *area;
  Window           tempwind;
  Hv_Rect          prect;
  short            dh, dv;

  if (Item == NULL)
    return 0;

  area = Item->area;
  pmap = Hv_CreatePixmap(area->width+1, area->height+1);

/* store the mainwindow */

  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, area->width+1, area->height+1);
  Hv_ClipRect(&prect);
  
  dh = area->left;
  dv = area->top;
  Hv_OffsetItem(Item, -dh, -dv, True);
  Hv_drawingOffScreen = True;
  Hv_DrawItem(Item, NULL);
  Hv_mainWindow = tempwind;
  Hv_drawingOffScreen = False;
  Hv_OffsetItem(Item, dh, dv, True);
  return pmap;
}


/*------ Hv_UpdateItemOffScreen ------*/

void  Hv_UpdateItemOffScreen(Hv_Item   Item,
			     Hv_Region region)

/***********************************
  Updates an item offscreeen. Does
  NOT update children.
***********************************/

{
  Hv_Pixmap        pmap;
  Hv_Rect         *area;
  Window           tempwind;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        rgn = NULL;

  if (Item == NULL)
    return;

  if (!Hv_ViewIsVisible(Item->view))
    return;

  area = Item->area;
  pmap = Hv_CreatePixmap(area->width+1, area->height+1);

/* store the mainwindow */

  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, area->width+1, area->height+1);
  Hv_ClipRect(&prect);

  dh = area->left;
  dv = area->top;
  Hv_OffsetItem(Item, -dh, -dv, True);
  Hv_drawingOffScreen = True;
  Hv_DrawItem(Item, NULL);

  
  Hv_mainWindow = tempwind;
  Hv_drawingOffScreen = False;
  Hv_OffsetItem(Item, dh, dv, True);

/* now copy back on screen */

  rgn = Hv_GetMinimumItemRegion(Item);
  Hv_SetClippingRegion(rgn);

  XCopyArea(Hv_display, pmap, Hv_trueMainWindow, Hv_gc, 0, 0,
	    area->width, area->height, area->left, area->top);
  Hv_Flush();

  Hv_DestroyRegion(rgn);
  
  if (region != NULL)
    Hv_RestoreClipRegion(region);
  
  Hv_FreePixmap(pmap);
}


/*----- Hv_SaveViewBackground ------*/

void Hv_SaveViewBackground(Hv_View   View,
			   Hv_Region  region)
{
  Window           tempwind;
  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;

/* first free the old pixmap and get a new one of the proper size */

  Hv_FreePixmap(View->background);

  hr = View->hotrectborderitem->area;
  View->background = Hv_CreatePixmap(hr->width+1, hr->height+1);

  if (View->background == 0)
    return;

/* store the mainwindow */

  tempwind = Hv_mainWindow;
  Hv_mainWindow = View->background;

/* set the clip to the entire pixmap and draw onto it */

  Hv_SetRect(&prect, 0, 0, hr->width+1, hr->height+1);
  Hv_ClipRect(&prect);
  
  dh = hr->left;
  dv = hr->top;
  Hv_OffsetView(View, -dh, -dv);
  Hv_DrawBackgroundItems(View->items, NULL);
  Hv_OffsetView(View, dh, dv);

/* restore the main window and clip region */

  Hv_SetViewClean(View);
  Hv_mainWindow = tempwind;

  Hv_RestoreClipRegion(region);
}


/*------ Hv_OffscreenViewControlUpdate --------*/

void Hv_OffScreenViewControlUpdate(Hv_View    View,
			    Hv_Region  region)

/******************************************
  This is the offscreen equivalent of
  Hv_DrawView that updates a portion
  of a view's offscreen buffer.

  If in postscript mode, this does nothing

  DRAWS ONLY CONTROL ITEMS

  OF WHETHER OR NOT THE VIEW SAVES THE BACKGROUND
  AND INDEPENDENTLY OF WHETHER THE ITEMS HAVE
  THEIR "IN BACKGROUND" BIT SET
*****************************************/

{
  Window           tempwind;
  Hv_Rect          *local;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        drawrgn = NULL;
  Hv_Region        rgn = NULL;
  Hv_Pixmap        pmap;
  Hv_Item          temp;
  
/* check for pathologies */

  if ((Hv_inPostscriptMode) || (Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
    return;

  Hv_SetViewDirty(View);

/* create pixmap, store the mainwindow */

  local = View->local;
  pmap = Hv_CreatePixmap(local->width, local->height);
  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;
  Hv_CopyRegion(&drawrgn, region);

/* now make preparations for offscreen drawing */

  Hv_SetRect(&prect, 0, 0, local->width, local->height);
  Hv_ClipRect(&prect);
  
  dh = local->left;
  dv = local->top;

  Hv_OffsetView(View, -dh, -dv);
  XOffsetRegion((Region)drawrgn, -dh, -dv);

/* now ready to draw */

  Hv_drawingOffScreen = True;


  for (temp = View->items->first;  temp != NULL;  temp = temp->next) 
    if (temp->domain != Hv_INSIDEHOTRECT) {
      Hv_MaybeDrawItem(temp, drawrgn);
	  
/* sublist? */
      
    if (temp->children != NULL)
      Hv_DrawItemList(temp->children, drawrgn);
  }

  Hv_drawingOffScreen = False;
  
  Hv_OffsetView(View, dh, dv);

  Hv_mainWindow = tempwind;

  Hv_DestroyRegion(drawrgn);
  rgn = Hv_RectRegion(local);
  drawrgn = Hv_IntersectRegion(rgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

  XCopyArea(Hv_display, pmap, Hv_trueMainWindow, Hv_gc, 0, 0,
	    local->width, local->height, local->left, local->top);
  
  XFlush(Hv_display);
  
  Hv_DestroyRegion(rgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}


/*------ Hv_OffscreenViewUpdate --------*/

void Hv_OffScreenViewUpdate(Hv_View    View,
			    Hv_Region  region)

/******************************************
  This is the offscreen equivalent of
  Hv_DrawView that updates a portion
  of a view's offscreen buffer.

  ONLY HOTRECT ITEMS CAN POSSIBLY BE DRAWN

  If in postscript mode, this does nothing

  OF WHETHER OR NOT THE VIEW SAVES THE BACKGROUND
  AND INDEPENDENTLY OF WHETHER THE ITEMS HAVE
  THEIR "IN BACKGROUND" BIT SET  *******


*****************************************/

{
  Window           tempwind;
  Hv_Rect          *hr;
  Hv_Rect          prect;
  short            dh, dv;
  Hv_Region        drawrgn = NULL;
  Hv_Region        hrrgn = NULL;
  Hv_Pixmap        pmap;
  Hv_Item          hrb, temp;
  
/* check for pathologies */

  if ((Hv_inPostscriptMode) || (Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
    return;

  Hv_SetViewDirty(View);

/* create pixmap, store the mainwindow */

  hrb = View->hotrectborderitem;
  hr = View->hotrect;
  pmap = Hv_CreatePixmap(hr->width, hr->height);
  tempwind = Hv_mainWindow;
  Hv_mainWindow = pmap;
  Hv_CopyRegion(&drawrgn, region);

/* now make preparations for offscreen drawing */

  Hv_SetRect(&prect, 0, 0, hr->width, hr->height);
  Hv_ClipRect(&prect);
  
  dh = hr->left;
  dv = hr->top;

  Hv_OffsetView(View, -dh, -dv);
  XOffsetRegion((Region)drawrgn, -dh, -dv);

/* now ready to draw */

  Hv_drawingOffScreen = True;

  if (hrb != NULL) {
    if (XRectInRegion(drawrgn, hr->left, hr->top,
		      hr->width, hr->height) != RectangleOut)
      Hv_DrawItem(hrb, drawrgn);
  }

  for (temp = View->items->first;  temp != NULL;  temp = temp->next)
    if (temp->domain == Hv_INSIDEHOTRECT) {
      Hv_MaybeDrawItem(temp, drawrgn);
	  
/* sublist? */
      
      if (temp->children != NULL)
	Hv_DrawItemList(temp->children, drawrgn);
    }

  Hv_drawingOffScreen = False;
  
  Hv_OffsetView(View, dh, dv);

  Hv_mainWindow = tempwind;

  Hv_DestroyRegion(drawrgn);
  hrrgn = Hv_HotRectRegion(View);
  drawrgn = Hv_IntersectRegion(hrrgn, region);

  Hv_SetClippingRegion(drawrgn);

/* now copy the pixmap on screen */

  XCopyArea(Hv_display, pmap, Hv_trueMainWindow, Hv_gc, 0, 0,
	    hr->width, hr->height, hr->left, hr->top);
  
  XFlush(Hv_display);
  
  Hv_DestroyRegion(hrrgn);
  Hv_DestroyRegion(drawrgn);
  Hv_RestoreClipRegion(region);
  Hv_FreePixmap(pmap);

}


/*------- Hv_RestoreViewBackground ---------*/

void  Hv_RestoreViewBackground(Hv_View View)

{
  Hv_Rect       *hr;

  if (View->background == 0) {
    Hv_SetViewDirty(View);
    return;
  }

  if (!Hv_ViewIsVisible(View))
    return;

  hr = View->hotrectborderitem->area;

  XCopyArea(Hv_display, View->background, Hv_trueMainWindow, Hv_gc, 0, 0,
	    hr->width+1, hr->height+1, hr->left, hr->top);
  XFlush(Hv_display);
}

/*------- Hv_ViewSavesBackground -------*/

Boolean    Hv_ViewSavesBackground(Hv_View View)

{
  return Hv_CheckBit(View->drawcontrol, Hv_SAVEBACKGROUND);
}

/*------- Hv_IsViewDirty ---------*/

Boolean   Hv_IsViewDirty(Hv_View View)

{
  return Hv_CheckBit(View->drawcontrol, Hv_DIRTY);
}

/*------ Hv_SetViewDirty --------*/


void      Hv_SetViewDirty(Hv_View View)

/* dirty bit only relevant for views that save bg */

{
  if (Hv_ViewSavesBackground(View))
    Hv_SetBit(&(View->drawcontrol), Hv_DIRTY);
}


/*----------- Hv_FreePixmap ------------*/

void   Hv_FreePixmap(Hv_Pixmap pmap)

{
  if (pmap == 0)
    return;

  XFreePixmap(Hv_display, (Pixmap)pmap);
}



/*----------- Hv_CreatePixmap ----------*/

Hv_Pixmap  Hv_CreatePixmap(unsigned int  w,
			   unsigned int  h)

/****************************************
Note: the extra care taken below with
Hv_trueMainWindow is because the "usual"
Hv_trueMainWindow may have been temporarily
reset due to offscreen drawing.
******************************************/

{

  Pixmap              pmap;
  unsigned int        depth;
  Window              twin;
  Hv_Rect             rect;

  if (Hv_inPostscriptMode)
    return 0;

  if (Hv_trueMainWindow == 0) {
    fprintf(stderr, "in Hv_CreatePixmap with Hv_trueMainWindow = 0\n");
    return 0;
  }

  if (Hv_display == NULL)
    return 0;

  depth = DefaultDepthOfScreen(DefaultScreenOfDisplay(Hv_display));

  pmap = XCreatePixmap(Hv_display, Hv_trueMainWindow,
		       w, h, depth);

  twin = Hv_mainWindow;
  Hv_mainWindow = pmap;

  XSetFunction(Hv_display, Hv_gc, GXcopy);
  
  Hv_SetRect(&rect, 0, 0, (short)w, (short)h);
  Hv_ClipRect(&rect);
  Hv_FillRect(&rect, Hv_white);
  Hv_mainWindow = twin;
  return  (Hv_Pixmap)pmap;
}

/*------ Hv_DrawBackgroundItems -------------*/

static void Hv_DrawBackgroundItems(Hv_ItemList items,
				   Hv_Region region)

{
  Hv_View     View;
  Hv_Item     temp;
  Hv_Region   hrrgn = NULL;
  Hv_Region   hrbrgn = NULL;
  Hv_Rect     hrect;

  if ((items == NULL) || (items->first == NULL))
    return;

  View = items->first->view;
  Hv_CopyRect(&hrect, View->hotrect);
  Hv_ShrinkRect(&hrect, 1, 1);
  
  hrrgn  =  Hv_RectRegion(&hrect);
  hrbrgn =  Hv_RectRegion(View->hotrectborderitem->area);
  Hv_drawingOffScreen = True;

/* if there really was a region, don't draw outside of it */

  if (region != NULL) {
    Hv_IntersectRegionWithRegion(&hrrgn, region);
    Hv_IntersectRegionWithRegion(&hrbrgn, region);
  }
  
  for (temp = items->first; temp != NULL; temp = temp->next)

    if (Hv_CheckItemDrawControlBit(temp, Hv_INBACKGROUND))
      if ((temp->domain == Hv_INSIDEHOTRECT) || (temp == View->hotrectborderitem)) {
	
	if (region == NULL) {
	  
	  if (temp == View->hotrectborderitem)
	    Hv_DrawItem(temp, hrbrgn); 
	  else
	    Hv_DrawItem(temp, hrrgn);
	}
	else {
	  if (temp == View->hotrectborderitem)
	    Hv_MaybeDrawItem(temp, hrbrgn);
	  else
	    Hv_MaybeDrawItem(temp, hrrgn);
	}
	
	if (temp->children != NULL)
	  Hv_DrawBackgroundItems(temp->children, region);
      }

/* off screen user drawing? */

  if (View->offscreenuserdraw != NULL)
    View->offscreenuserdraw(View, hrrgn);

  Hv_drawingOffScreen = False;
  Hv_DestroyRegion(hrrgn);
  Hv_DestroyRegion(hrbrgn);
}


/*------ Hv_SetViewClean --------*/


static void      Hv_SetViewClean(Hv_View View)

/* dirty bit only relevant for views that save bg */

{
  Hv_ClearBit(&(View->drawcontrol), Hv_DIRTY);
}


