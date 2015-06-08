/*	
-----------------------------------------------------------------------
File:     Hv_views.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"		/* contains all necessary motif include files */
#include "Hv_xyplot.h"
#include "Hv_maps.h"
#include "Hv_undo.h"

#define Hv_MINCHANGE           0.0001

/* local prototypes */

static void Hv_SetupUndoZoom(Hv_View View);

static Hv_View Hv_MallocView(void);

static void Hv_FinalTouches(Hv_View View);

static void  Hv_StuffViewTitle(Hv_View   View,
			       char    *title);

static void Hv_SendViewToEndOfList(Hv_View  View);

static void Hv_SendViewToTopOfList(Hv_View    View);

static void  Hv_DoViewMenu(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data);

static void  Hv_QuickZoomResize(Hv_View  View,
				float    xmin,
				float    xmax,
				float    ymin,
				float    ymax);

static void   Hv_ViewBoundingRect(Hv_View View,
				  XRectangle *rect);

static void  Hv_ToggleView(Hv_View   View);

static void Hv_HideView(Hv_View   View);

static void Hv_SquareTheView(Hv_View   View);

static void Hv_AfterZoomDraw(Hv_View  View);

static void Hv_SetViewSize(Hv_View  View,
			   short   left,
			   short   top,
			   short   width,
			   short   height,
			   short   ml,
			   short   mt,
			   short   mr,
			   short   mb,
			   short   hrwidth,
			   short   hrheight);

static void Hv_SetViewWorld(Hv_View View,
			    float   xmin,
			    float   xmax,
			    float   ymin,
			    float   ymax);

static void Hv_PrepareForViewChange(Hv_View  View,
				    Hv_Region *oldtot,
				    Hv_Region *oldrgn);

static void Hv_FinishViewChange(Hv_View  View,
				Hv_Region oldtot,
				Hv_Region oldrgn);

static void Hv_SimulationTimeOut(XtPointer    xtptr,
                                  XtIntervalId  *id);

static void Hv_FeedbackTimeOut(XtPointer    xtptr,
			       XtIntervalId  *id);

static Hv_ScrollInfoPtr   MallocScrollBar(void);

/*------- Hv_SetViewPopupControl --------*/

void   Hv_SetViewPopupControl(View, bits)

Hv_View      View;
int          bits;

{
  if (View == NULL)
    return;

  View->popupcontrol = bits;
}


/*------ Hv_SetViewDrawControl --------*/

void  Hv_SetViewDrawControl(Hv_View  View,
			     int     drawcontrol)

{
  if (View == NULL)
    return;

  View->drawcontrol = drawcontrol;
}

/* ------ Hv_SetViewMinimumSize -------*/

void Hv_SetViewMinimumSize(Hv_View  View,
			   short   minw,
			   short   minh)

{
  if ((View == NULL) || (minw < 5) || (minh < 5))
    return;

  View->minwidth = minw;
  View->minheight = minh; 
}

/*------- Hv_StopSimulation -------*/

void      Hv_StopSimulation(Hv_View View)

/*********************************************
  Stops and removes a simulation from a 
  view.

  Modified: 6/10/96: also sets siminterval to
  zero -- fix provided by Anton Mezger at PSI.
*******************************************/

{
  if (View == NULL)
    return;

  if (View->simintervalid != 0) {
    XtRemoveTimeOut(View->simintervalid);
    View->siminterval = 0;
    View->simintervalid = 0;
  }

}

/*------- Hv_ChangeSimulationInterval ----*/

void Hv_ChangeSimulationInterval(Hv_View View,
				 unsigned long interval)

{
  Hv_StopSimulation(View);
  Hv_StartSimulation(View, View->simproc, interval);
}

/*------- Hv_StartSimulation -------*/

void   Hv_StartSimulation(Hv_View     View,
			  Hv_FunctionPtr  proc,
			  unsigned long  interval)

{
  if ((View == NULL) || (proc == NULL) || (interval < 1))
    return;

  View->siminterval = interval;
  View->simproc = proc;
  View->simintervalid = XtAppAddTimeOut(Hv_appContext, View->siminterval,
					(XtTimerCallbackProc)Hv_SimulationTimeOut,
					(XtPointer)View); 

}

/* ------ Hv_GetViewHotRect ------*/

Hv_Rect  *Hv_GetViewHotRect(Hv_View View)

/* trivial information hiding */

{
  return View->hotrect;
}

/* ------ Hv_GetViewData ------*/

void  *Hv_GetViewData(Hv_View View)

/* trivial information hiding */

{
  return View->data;
}

/* ------ Hv_GetViewTag --------*/

short   Hv_GetViewTag(Hv_View View)

/* simple information hiding procedure */

{
  if (View == NULL)
    return -1;

  return View->tag;
}

/* ------ Hv_GetViewItemList --------*/

Hv_ItemList   Hv_GetViewItemList(Hv_View View)

/* simple information hiding procedure */

{
  if (View == NULL)
    return NULL;
  
  return View->items;
}

/*----- Hv_ViewIsActive -------*/

Boolean  Hv_ViewIsActive(Hv_View  View)

{
  if (View == NULL)
    return False;
  else 
    return Hv_CheckBit(View->drawcontrol, Hv_ACTIVE);
}


/*----- Hv_ViewIsVisible ------*/

Boolean Hv_ViewIsVisible(Hv_View View)

{
  if (!Hv_ViewIsActive(View))
    return False;

  return Hv_RectIntersectsRect(&Hv_canvasRect, View->local);
}

/*-------- Hv_SetViewBrickWallItem --------*/

void  Hv_SetViewBrickWallItem(Hv_Item Item)

/* sets an item to be the first HotRect item
   no other item will be placed above it. Thus if this
   is a map, and an item drawn on top of the map is shift
   clicked, it will not end up under the map
   
   ONLY RELEVANT FOR HOTRECT ITEMS */

{
  if (Item->domain != Hv_INSIDEHOTRECT)
    return;

/* cannot set child item to be the brick wall (but NULL is ok) */

  if ((Item != NULL) && (Item->parent != NULL))
    return;

  Item->view->brickwallitem = Item;
}

/* -------- Hv_MoveView ----- */

void Hv_MoveView(Hv_View  View,
		 short    dh,
		 short    dv)

{
/* offsets a View by the specified amount, offsetting all child items. */


/* ERASES:  diff of OLD total region and NEW total region */
/* REDRAWS: union of newregion and (intresect(oldregion, oldtotal) */

  Hv_Region   oldtot;
  Hv_Region   oldrgn;

  if (View == NULL)
    return;

  Hv_PrepareForViewChange(View, &oldtot, &oldrgn);
  Hv_OffsetView(View, dh, dv);             /* offset the position values of the View and items */

/* see if user set up additional action */

  if (Hv_UserMove != NULL)
    Hv_UserMove(View, dh, dv);

  Hv_FinishViewChange(View, oldtot, oldrgn);

/* free the region memory no longer being used */

  Hv_DestroyRegion(oldtot);            
  Hv_DestroyRegion(oldrgn); 
  Hv_UpdateVirtualView();
}

/* -------- Hv_GrowView ----- */

void Hv_GrowView(Hv_View   View,
		 short     dh,
		 short     dv)

/* grows a View by the specified amount, offsetting all child items.
   Also: ERASES the old View and DRAWS the new View */

{
  Hv_Region   oldtot;
  Hv_Region   oldrgn;

  if (View == NULL)
    return;

  Hv_PrepareForViewChange(View, &oldtot, &oldrgn);
  Hv_ResizeView(View, dh, dv);          /* resize it */
  Hv_FinishViewChange(View, oldtot, oldrgn);
      
/* free the region memory no longer being used */
      
  Hv_DestroyRegion(oldtot);            
  Hv_DestroyRegion(oldrgn); 
  Hv_UpdateVirtualView();
}

/* -------- Hv_FlipView -------- */

void Hv_FlipView(Hv_View View)

/* This is called when someone hits the "explode" box */

{

  Hv_Rect        templocal, *prevlocal;
  Hv_FRect       tempworld;
  Hv_FRect      *prevworld = NULL;
  Hv_Region      oldtot;
  Hv_Region      oldrgn;
  int            slop = 2;

  short          ll, lt, lr, lb;
  short          cl, ct, cr, cb;

  Boolean        tofullscreen = False;

/* Flip (toggle)  a View between its current local; and its previous local */

  if (View == NULL)
    return;

  Hv_PrepareForViewChange(View, &oldtot, &oldrgn);

/*--------- MOD 6.15.95  if present size smaller (a little slop)
 than canvas, reset prevlocal to size of canvas. This allows
 a) toggle to full screen, resize canvas, toggle to full screen
 again. */ 
 

  Hv_GetRectLTRB(View->local,    &ll, &lt, &lr, &lb);
  Hv_GetRectLTRB(&Hv_canvasRect, &cl, &ct, &cr, &cb);
 
/* see if we are expanding to full screen */

  if ((ll > (cl + slop)) 
      || (lt > (ct + slop))
      || (lr < (cr - slop))
      || (lb < (cb - slop))) {
    Hv_SetRect(View->previouslocal, 0, 0, 32000, 32000);
    tofullscreen = True;
  }

    
  Hv_CopyRect(&templocal, View->local);   /* store the current local system */
  prevlocal = View->previouslocal;	      	   
   
  if (!Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD)) {
    Hv_CopyFRect(&tempworld, View->world);  /* store the current world system */
    prevworld = View->previousworld;	      	   
  }

/* alter the position accordingly, after making sure that previouslocal
   still fits  */

  Hv_KeepRectInsideRect(prevlocal, &Hv_canvasRect);

  Hv_OffsetView(View, prevlocal->left - templocal.left,
		 prevlocal->top - templocal.top);
		      
/* alter the size accordingly */

  Hv_ResizeView(View, prevlocal->width - templocal.width,
		 prevlocal->height - templocal.height);

/* If not expanding to full screen try as best as possible to
   restore the previous world system */
 
  if (!tofullscreen)
    if (!Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD)) {

      Hv_CopyFRect(View->world, prevworld);
      if (View->local->width > View->local->height)
	View->world->height = (View->world->width * View->local->height)/((float)(View->local->width));
      else
	View->world->width = (View->world->height * View->local->width)/((float)(View->local->height));
      View->world->xmax = View->world->xmin + View->world->width;
      View->world->ymax = View->world->ymin + View->world->height;
    }

  Hv_FinishViewChange(View, oldtot, oldrgn);

/* store the old local system as the previous local system */
	      
  *prevlocal = templocal;

/* store the old world system as the previous world system */
	      
  if (!Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    *prevworld = tempworld;

  Hv_DestroyRegion(oldtot);            
  Hv_DestroyRegion(oldrgn); 
  Hv_UpdateVirtualView();
}


/* ---------  Hv_GetViewRegion ---------*/

Hv_Region Hv_GetViewRegion(Hv_View View)

/* Mallocs and returns Gets the "clipping" region for drawing this view.
   This is the container frame minus overlap with later active Views. */

{
  Hv_View          temp;
  XRectangle       rect;
  Hv_Region        tempregion, sregion2;
  Hv_Region        region;

  if (!Hv_ViewIsVisible(View))
    return NULL;
  
  Hv_ViewBoundingRect(View, &rect);
  region = Hv_RectangleRegion(rect.x, rect.y, rect.width, rect.height);         /*start with this Views container */

  if (Hv_drawingOffScreen)
    return region;

/* now subtract out other View rects */

  for (temp = View->next;  temp != NULL; temp = temp->next)
    if (Hv_ViewIsVisible(temp)) {
      Hv_ViewBoundingRect(temp, &rect);
      if (XRectInRegion(region, rect.x, rect.y, rect.width, rect.height) != RectangleOut) {
	Hv_CopyRegion(&sregion2, region);            /* copy overall region to sregion2 */
	Hv_DestroyRegion(region);


	tempregion = Hv_RectangleRegion(rect.x, rect.y, rect.width, rect.height);         /*start with this Views container */
	region = Hv_SubtractRegion(sregion2, tempregion);
	
	Hv_DestroyRegion(tempregion);
	Hv_DestroyRegion(sregion2);
      }
    }

  return region;
}

/*---------- Hv_GetInsideContainerRegion --------*/

Hv_Region    Hv_GetInsideContainerRegion(Hv_View View)

/* very similar to Hv_GetViewRegion except it
also excludes the scroll areas */

{
  Hv_Rect         area;

  Hv_Region       vrgn = NULL;
  Hv_Region       rrgn = NULL;
  Hv_Region       irgn = NULL;

  if (View == NULL)
    return NULL;

  Hv_InsideContainerArea(View, &area);

  rrgn = Hv_RectRegion(&area);
  vrgn = Hv_GetViewRegion(View);
  irgn = Hv_IntersectRegion(rrgn, vrgn);

  Hv_DestroyRegion(rrgn);
  Hv_DestroyRegion(vrgn);
  return irgn;
}

/* ---------  Hv_DrawView ---------*/

void  Hv_DrawView(Hv_View   View,
		  Hv_Region region)


/*  Draw the item list for a View. If region is NULL, will
   use the min region obtained fro Hv_GetViewRegion */
{
  if ((Hv_mainWindow == 0) || !Hv_ViewIsVisible(View))
    return;
  
  Hv_SquareTheView(View);

  Hv_DrawControlItems(View, region);
  Hv_DrawHotRectItems(View, region);
  
/* specialize user supplied drawing */

  if (View->userdraw != NULL)
    View->userdraw(View, region);

  Hv_FullClip(); 
}

/*-------- Hv_IsMapView ------------*/

Boolean         Hv_IsMapView(Hv_View View) {

    return (View->mapdata != NULL);
}



/*---------- Hv_DrawControlItems --------------- */

void	Hv_DrawControlItems(Hv_View View,
			    Hv_Region region)


{
  Hv_Item	temp;
  Hv_Region     vrgn = NULL;
  Hv_Region     crgn = NULL;
  Hv_Region     drgn1 = NULL;
  Hv_Region     drgn2 = NULL;
  Hv_Region     drawregion;

  if (View == NULL)
    return;

  vrgn = Hv_GetViewRegion(View);
  crgn = Hv_GetInsideContainerRegion(View);

  if (region != NULL) {
    drgn1 = Hv_IntersectRegion(region, vrgn);
    drgn2 = Hv_IntersectRegion(region, crgn);
    Hv_DestroyRegion(vrgn);
    Hv_DestroyRegion(crgn);
  }
  else {
    drgn1 = vrgn;
    drgn2 = crgn;
  }

  if (Hv_inPostscriptMode)
    Hv_PSComment("Drawing Control Items");

  for (temp = View->items->first;  temp != NULL;  temp = temp->next) {
    if (temp->domain == Hv_OUTSIDEHOTRECT) {
      if (temp != View->containeritem)
	drawregion = drgn2;
      else
	drawregion = drgn1;
      
      Hv_MaybeDrawItem(temp, drawregion);
      
/* sublist? */
      
      if (temp->children != NULL)
	Hv_DrawItemList(temp->children, drawregion);
      
/* after drawing the feedback item, subtract its area */

      if (temp == View->feedbackitem)
	Hv_RemoveRectFromRegion(&drgn2, temp->area);
    }
  }
  
  Hv_DestroyRegion(drgn1);
  Hv_DestroyRegion(drgn2);
  
  if (Hv_inPostscriptMode)
    Hv_PSComment("Done drawing Control Items");

}

/*------- Hv_DrawViewHotRect -----------*/

void  Hv_DrawViewHotRect(Hv_View View)

/*  Draw the item list for a View, but
 only the part of the View that is visible
 and in the hotrect */

{
  Hv_Region   region;
  Hv_Rect     rect;

  if (!Hv_drawingOn)
    return;

  Hv_CopyRect(&rect, View->hotrect);
  Hv_ShrinkRect(&rect, -1, -1);
  region = Hv_RectRegion(&rect);

  Hv_DrawView(View, region);
  Hv_DestroyRegion(region);
}

/*------- Hv_RedrawViewHotRect -----------*/

void Hv_RedrawViewHotRect(Hv_View   View,
			  short     type,
			  Hv_Region cregion)

/*  Draw the item list for a View, but
 only the part of the View that is visible
 and in the hotrect Unlike Hv_DrawViewHotRect,
 it excludes the hotrect item itself, which
 reduces the "flashes" effect */

/* type != 0 -> only use items of that type */

{
  Hv_Item      temp;
  Hv_Region    region = NULL;

  if (cregion == NULL)
    region = Hv_ClipHotRect(View, 0);
  else  {
    Hv_CopyRegion(&region, cregion);
    Hv_SetClippingRegion(region);
  }
  
  for (temp = View->items->first; temp != NULL; temp = temp->next)
    if ((temp->domain == Hv_INSIDEHOTRECT) && 
	(temp->type != Hv_HOTRECTBORDERITEM) &&
	((type == 0) || (temp->type == type)))
      
      Hv_MaybeDrawItem(temp, region);

  Hv_DestroyRegion(region);
}


/* ------- Hv_HotRectRegion ------- */

Hv_Region      Hv_HotRectRegion(Hv_View View)

/* MALLOCS, AND RETURNS (does not set) the clip region
   to the INTERSECTION of the Views 
   drawing (i.e. visible) region and its hotrect. */

{
  Hv_Region    vis_rgn, hr_rgn, tmp_rgn;
  Hv_Rect rect;
  
  Hv_CopyRect(&rect, View->hotrect);
  hr_rgn =  Hv_RectRegion(&rect);
  
  vis_rgn = Hv_GetViewRegion(View);
  tmp_rgn = Hv_IntersectRegion(hr_rgn, vis_rgn);
  
  Hv_DestroyRegion(hr_rgn);
  Hv_DestroyRegion(vis_rgn);
  return tmp_rgn;
}

/* ------- Hv_ClipHotRect ------- */

Hv_Region      Hv_ClipHotRect(Hv_View View,
			      short   shrink)

/* MALLOCS, SETS AND RETURNS the clip region to the INTERSECTION of the Views 
   drawing (i.e. visible) region and its hotrect. */

/* NOTE: This is a lowest level Hv draw routine
   and thus CHECKS FOR POSTSCRIPT MODE */

{
  Hv_Region    vis_rgn = NULL;
  Hv_Region    hr_rgn = NULL;
  Hv_Region    tmp_rgn = NULL;
  Hv_Rect      rect;

  if (Hv_inPostscriptMode)
      Hv_PSClipHotRect(View);
  else
      vis_rgn = Hv_GetViewRegion(View);

  Hv_CopyRect(&rect, View->hotrect);
  
  if (shrink != 0)
    Hv_ShrinkRect(&rect, shrink, shrink);

  hr_rgn =  Hv_RectRegion(&rect);
  
  tmp_rgn = Hv_IntersectRegion(hr_rgn, vis_rgn);
  Hv_SetClippingRegion(tmp_rgn); 
  
  
  Hv_DestroyRegion(hr_rgn);
  Hv_DestroyRegion(vis_rgn);
  return tmp_rgn;
}

/*--------- Hv_ClipView ------*/

Hv_Region      Hv_ClipView(Hv_View View)

/* Malloc, set and return the View's present
   minimum drawing region */

{
  Hv_Region      reg;

  reg = Hv_GetViewRegion(View);
  Hv_SetClippingRegion(reg);

  return      reg;
}

/* ------- Hv_SendViewToFront ------------ */

void Hv_SendViewToFront(Hv_View View)

/* make the view appear on top, but putting in at the end of the view linked list */

{
  Hv_Region     rgn1, rgn2, rgn3;  /* regions, used to compute the minumun redraw area on the screen */
  XRectangle    rect;              /* rectangle used to set a region to the size of a View */

  if (!Hv_ViewIsVisible(View))
    return;

/* If this View is already in the front, we should do nothing */

  if (Hv_views.last != View) {
    rgn1 = Hv_GetViewRegion(View);
    
    Hv_SendViewToEndOfList(View);
    Hv_ViewBoundingRect(View, &rect);
    rgn2 = Hv_RectangleRegion(rect.x,
			      rect.y,
			      rect.width,
			      rect.height);
    rgn3 = Hv_SubtractRegion (rgn2, rgn1);
    Hv_DrawView(View, rgn3);
    Hv_DestroyRegion(rgn1);
    Hv_DestroyRegion(rgn2);
    Hv_DestroyRegion(rgn3);
  }
  Hv_SendVVRepToFront(View);
}

/* ------- Hv_SendViewToBack ------------ */

void Hv_SendViewToBack(Hv_View View)

/* make the view appear on bottom, but putting in
   at the top of the view linked list */


{
  Hv_Region     rgn1, rgn2, rgn3;  /* regions, used to compute the minimum redraw area on the screen */

  if (!Hv_ViewIsVisible(View))
    return;

  rgn1 = Hv_GetViewRegion(View);
  Hv_SendViewToTopOfList(View);
  rgn2 = Hv_GetViewRegion(View);

  rgn3 = Hv_SubtractRegion(rgn1, rgn2);
  Hv_UpdateViews(rgn3);
  Hv_DestroyRegion(rgn1);
  Hv_DestroyRegion(rgn2);
  Hv_DestroyRegion(rgn3);

  Hv_SendVVRepToBack(View);
}

/*------ Hv_SetupUndoZoom ------------*/

static void Hv_SetupUndoZoom(Hv_View View)

{
  Hv_ViewMapData vmdata;

  if (View == NULL)
    return;

  if (View->lastworld == NULL)
    View->lastworld = (Hv_FRect *)Hv_Malloc(sizeof(Hv_FRect));

/* assume existance of mapdata implies this is a map view.
   Store latmin/max in xmin/max and long min.max in y min/max
   for mercator projections */

  vmdata = Hv_GetViewMapData(View);

  if ((View->mapdata != NULL) && (vmdata != NULL) && (vmdata->projection == Hv_MERCATOR)) {
    Hv_LocalToLatLong(View, View->hotrect->left, View->hotrect->bottom,
		      &(View->lastworld->xmin), &(View->lastworld->ymin));
    Hv_LocalToLatLong(View, View->hotrect->right, View->hotrect->top,
		      &(View->lastworld->xmax), &(View->lastworld->ymax));
      
    View->lastworld->xmin *= Hv_RADTODEG;
    View->lastworld->xmax *= Hv_RADTODEG;
    View->lastworld->ymin *= Hv_RADTODEG;
    View->lastworld->ymax *= Hv_RADTODEG;
  }
  else
    Hv_LocalRectToWorldRect(View, View->hotrect, View->lastworld);

}


/* ------ Hv_RestoreDefaultWorld -------*/

void  Hv_RestoreDefaultWorld(Hv_View View)

/* restore the Views default WORLD system and redraw. The
  local system (i.e. the size of the View is NOT restored */

{
/* no need to adjust anything if world is fixed */

  if (Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    return;

  Hv_SetViewDirty(View);

  Hv_SetupUndoZoom(View);    /* set up for undo */

/* Specialized default world? */
  
    if (Hv_UserDefaultWorld != NULL)
      Hv_UserDefaultWorld(View);
    else
      Hv_GetTemporaryWorld(View->defaultworld, View->hotrect, View->world, View->local);
  
    Hv_SquareTheView(View);

/* reposition hotrect items */

  Hv_RepositionHotRectItems(View->items);

  if (Hv_UserZoom != NULL)
    Hv_UserZoom(View);

  Hv_AfterZoomDraw(View);
}

/* ---------  Hv_AutosizeView ---------*/

void     Hv_AutosizeView(Hv_View View)

/*  Adjust the View container so that it just fits
  (respecting offsets) around its item list*/

{
  short    dL, dT, dR, dB;

  Hv_Item   item;
  Hv_Item   temp;
  Hv_Rect   *area;
  Hv_Rect   *rect;

   if (View != NULL) {
     item = View->containeritem;
     area = item->area;

     dL = 15;
     dT = 30;
     dR = 30;
     dB = 15;

     area->right  = area->left + 10;
     area->bottom = area->top + 10;

     for (temp = item->next; temp != NULL; temp = temp->next) {
       rect = temp->area;
       area->right  = Hv_sMax(area->right,  rect->right);
       area->bottom = Hv_sMax(area->bottom, rect->bottom);
     }

     area->right  += dR;
     area->bottom += dB;
     
     Hv_FixRectWH(area);
     Hv_CopyRect(View->local, area);
   }
}


/* ---------  Hv_UpdateViews ---------*/

void   Hv_UpdateViews(Hv_Region region)

/*  Updates the entire View list after an exposure
  region is the exposure region */

{
  Hv_View       temp;
  XRectangle    rect;

  XSetFunction(Hv_display, Hv_gc, GXcopy);
  Hv_SetClippingRegion(region);

  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
    Hv_ViewBoundingRect(temp, &rect);
    
    if (region)
      if (Hv_ViewIsVisible(temp) &&
	  (XRectInRegion(region,
			 rect.x,
			 rect.y,
			 rect.width,
			 rect.height) != RectangleOut))
	Hv_DrawView(temp, region);
  }

}


/* ---------  Hv_FreeView  ---------*/

void   Hv_FreeView(Hv_View View)

/*  Frees all memory used by the View. The View is gone!*/

{
  Hv_View           temp;
  Hv_FunctionPtr    tempdestroy;

  if (View == NULL)
    return;

  if (Hv_ViewIsVisible(View))
    Hv_ToggleView(View);

  XtUnmanageChild(View->menu);  /* take the View from the pulldown menu */

  temp = View;

/* call the private destruction function first -- which should free
   any memory allocated by the user via the data field */

  if(temp->destroy != NULL) {
    tempdestroy = temp->destroy;
    temp->destroy = NULL;
    tempdestroy(temp);
  }


  if (View->prev != NULL)  /* if there is a previous item to reset */
    View->prev->next = View->next;
  
  if (View->next != NULL)  /* if there is a next item to reset */
    View->next->prev = View->prev;
  
  if (Hv_views.first == View)   /* if it is the first item in the list */
    Hv_views.first = View->next;
  
  if (Hv_views.last == View)    /* if it is the last item in the list */
    Hv_views.last = View->prev;
  
  temp->next = NULL;  /* to guarantee that we dont bleed over */
  temp->prev = NULL;  
  
/* free space allocated to item pointers*/
      
  if (temp->items->first != NULL)
    Hv_KillItemList(temp->items);
  Hv_Free(temp->items);

/* did the view have a map? */

  Hv_DestroyViewMapData(temp);

/* remove timeouts */

  if (temp->simintervalid != 0)
    XtRemoveTimeOut(temp->simintervalid);

  if (temp->motionlessfbintervalid != 0)
    XtRemoveTimeOut(temp->motionlessfbintervalid);


/* free various rects */
  
  Hv_Free(temp->local);
  Hv_Free(temp->hotrect);
  Hv_Free(temp->previouslocal);
  Hv_Free(temp->previousworld);
  Hv_Free(temp->defaultworld);
  Hv_Free(temp->world);
  Hv_Free(temp->data);
  
  Hv_Free(temp->hscroll);
  Hv_Free(temp->vscroll);
  
  XtDestroyWidget(temp->menu);
  
  Hv_Free(temp);
}


/* -------- Hv_ZoomView ----------------- */

void Hv_ZoomView(Hv_View   View,
		 Hv_Rect   *rect)

/* zoom the View, (ID) to the box given by rect */
     
{
  float      new_width, new_height; /* new world width and height */
  float      new_hrwidth, new_hrheight; /* new horect "world"  width and height */

  Hv_Rect    *local;
  Hv_FRect   *world;
  Hv_Rect    *hotrect;

  local   = View->local;
  world   = View->world;
  hotrect = View->hotrect;

/* if world is fixed (very rare), no point in zooming */

  if (Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    return;

  Hv_SetupUndoZoom(View);    /* set up for undo */

/* compute the new width and height being sure to limit it to a size worth using */

  new_width = (world->width * rect->width) / hotrect->width;
  new_height = (world->height * rect->height) / hotrect->height;

  new_hrwidth = (new_width * hotrect->width) / local->width;
  new_hrheight = (new_height * hotrect->height) / local->height;

  if (new_hrwidth < View->minzoomh) {
    Hv_Warning("Width too small! Cannot zoom in that far.");
    return;
  }

  if (new_hrheight < View->minzoomh) {
    Hv_Warning("Height too small! Cannot zoom in that far.");
    return;
  }

  if (new_hrwidth > View->maxzoomw) {
    Hv_Warning("Width too big! Cannot zoom out that far.");
    return;
  }

  if (new_hrheight > View->maxzoomh) {
    Hv_Warning("Height too big! Cannot zoom out that far.");
    return;
  }

/* adjust the world to reflect the desired zoom */

  world->xmin += world->width * 
    (rect->left - local->left - ((hotrect->left - local->left) * 
				 rect->width)/hotrect->width) / local->width;
  
  world->width = new_width;
  world->xmax = View->world->xmin + new_width;
  
  world->ymin += View->world->height * 
    (local->bottom - rect->bottom + 
     ((hotrect->bottom - local->bottom) * 
      rect->height)/hotrect->height) / local->height;
  
  world->height = new_height;
  world->ymax = world->ymin + new_height;
  
  Hv_SquareTheView(View);

/* reposition hotrect items */

  Hv_RepositionHotRectItems(View->items);

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);

/* see if user set up additional action */

  if (Hv_UserZoom != NULL)
    Hv_UserZoom(View);

/* Hv_AfterZoomDraw handles scroll bars, too */
  
  Hv_AfterZoomDraw(View);
  Hv_UpdateVirtualView();
}

/* -------- Hv_PercentZoom ----------------- */

void Hv_PercentZoom (Hv_View   View,
		     float     amount)

{
/* zooms the world coordinate system of the given View
   by  the specified amount (percent) */

  Hv_Rect  rect;
  Hv_Rect  *area;

  if (View == NULL)
    return;

/* plot views handled differently */

  if (View->tag == Hv_XYPLOTVIEW) {
    if (Hv_hotPlot == NULL)
      return;
    else
      area = Hv_hotPlot->area;
  }
  else
    area = View->hotrect;

/* set the new width & height to the hot rect * zoom factor. if 100% zoom (amount = 1)
   no change will be made */

  rect.width  = (short)(area->width  * amount);  
  rect.height = (short)(area->height * amount);
      
/* set the new upperleft corner */

  rect.left = area->left + (area->width  - rect.width)/2;
  rect.top = area->top  + (area->height - rect.height)/2;
  Hv_FixRectRB(&rect);
  
/* make sure we have a good box (ltwh) then zoom */

  Hv_GoodRect(&rect);

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);
  
  if (View->tag == Hv_XYPLOTVIEW)
    Hv_ZoomPlotToRect(Hv_hotPlot, &rect);
  else
    Hv_ZoomView(View, &rect);
}

/*----------- Hv_QuickZoomView -------*/

void Hv_QuickZoomView(Hv_View View,
		      float   xmin,
		      float   xmax,
		      float   ymin,
		      float   ymax)

/* xmin, xmax etc -> new world coords of hotrect*/

{

/* no point to zoom if world is fixed */

  if (Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    return;

/* skip for map quick zooms */

  if (View->mapdata == NULL) 
    Hv_SetupUndoZoom(View);    /* set up for undo */

  Hv_CheckForExposures(Expose);
  Hv_QuickZoomResize(View, xmin, xmax, ymin, ymax);
  Hv_SetScrollActivity(View);

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);

/* see if user set up additional action */

  if (Hv_UserZoom != NULL)
    Hv_UserZoom(View);


  Hv_AfterZoomDraw(View);                 /* update the region affected by the quickzoom */
}


/*------ Hv_TotalViewRegion --------*/

Hv_Region  Hv_TotalViewRegion(void)

/* mallocs and returns the combined drawing regions of all
   active Views */


{
  Hv_Region      rgn;
  XRectangle     rect;
  Hv_View        view;

  rgn = Hv_CreateRegion();

  for (view = Hv_views.first;  view != NULL;  view = view->next)
    
    if (Hv_ViewIsVisible(view)) {
      Hv_ViewBoundingRect(view, &rect);
      Hv_UnionRectangleWithRegion(&rect, rgn);
  }
  
  
  return rgn;
}

/* ------------ Hv_VaCreateView ---------- */

void Hv_VaCreateView(Hv_View *View,
			...)

{

  static short     cnt = 0;   /* for staggering default placement of new views */

  va_list           ap;             /* traverses the list */
  Hv_AttributeArray attributes;
  Hv_Item           Item;
  short             ml, mt, mr, mb;   /* margins */

  Hv_FunctionPtr    finalinit;

  cnt ++;

  *View = NULL;

  va_start(ap, View);

/* Get the variable arguments */

  Hv_GetAttributes(ap, attributes, NULL, NULL, NULL, NULL, NULL);

/* overwrite some defaults */

  if (attributes[Hv_DRAWCONTROL].i < 0)
    attributes[Hv_DRAWCONTROL].i = Hv_STANDARDDRAWCONTROL;
  if (attributes[Hv_LEFT].s < -32000)
    attributes[Hv_LEFT].s = cnt*30;
  if (attributes[Hv_TOP].s < -32000)
    attributes[Hv_TOP].s = cnt*30;

  *View = Hv_MallocView();

/* data ? */

  if (attributes[Hv_DATA].v != NULL)
    (*View)->data = (void *)attributes[Hv_DATA].v;

/* set the defaults for the drawing tools */

  (*View)->hotfont = Hv_helveticaBold14;
  (*View)->hotfontfamily = Hv_HELVETICAFAMILY;
  (*View)->hotfontsize   = Hv_FONTSIZE14;
  (*View)->hotfontstyle  = Hv_BOLDFACE;

  (*View)->hotfillcolor = -1;
  (*View)->hotbordercolor = Hv_black;
  (*View)->hotshape = Hv_RECTSHAPE;
  (*View)->hotpattern = Hv_FILLSOLIDPAT;
  (*View)->hotlinewidth = 0;
  (*View)->hotlinestyle = Hv_SOLIDLINE;
  (*View)->hotarrowstyle = Hv_NOARROW;


  (*View)->animationproc = NULL;
  (*View)->mapdata = NULL;
  (*View)->simproc = NULL;
  (*View)->simintervalid = 0;
  (*View)->motionlessfbintervalid = 0;
  (*View)->motionlessfbinterval = 0;
  (*View)->tag             = attributes[Hv_TAG].s;
  (*View)->drawcontrol     = attributes[Hv_DRAWCONTROL].i;
  (*View)->userdrawcontrol = attributes[Hv_USERDRAWCONTROL].i;
  (*View)->userdraw        = (Hv_FunctionPtr)(attributes[Hv_USERDRAW].fp);
  (*View)->offscreenuserdraw  = (Hv_FunctionPtr)(attributes[Hv_OFFSCREENUSERDRAW].fp);
  (*View)->feedback        = (Hv_FunctionPtr)(attributes[Hv_FEEDBACK].fp);
  (*View)->customize       = (Hv_FunctionPtr)(attributes[Hv_CUSTOMIZE].fp);
  (*View)->destroy         = (Hv_FunctionPtr)(attributes[Hv_DESTROY].fp);
  (*View)->initialize      = (Hv_FunctionPtr)(attributes[Hv_INITIALIZE].fp);
  (*View)->popupcontrol    = attributes[Hv_POPUPCONTROL].i;
  (*View)->minwidth        = attributes[Hv_MINWIDTH].s;
  (*View)->minheight       = attributes[Hv_MINHEIGHT].s; 

  (*View)->lastworld = NULL;  /* used for zoom undos */

/* size the view according to the appropriate attributes */
/* the H and V margins are for backward compatibility */


  if (attributes[Hv_HOTRECTHMARGIN].s > 0) {
    ml = attributes[Hv_HOTRECTHMARGIN].s;
    mr = ml;
  }
  else {
    ml = attributes[Hv_HOTRECTLMARGIN].s;
    mr = attributes[Hv_HOTRECTRMARGIN].s;;
  }

  if (attributes[Hv_HOTRECTVMARGIN].s > 0) {
    mt = attributes[Hv_HOTRECTVMARGIN].s;
    mb = mt;
  }
  else {
    mt = attributes[Hv_HOTRECTTMARGIN].s;
    mb = attributes[Hv_HOTRECTBMARGIN].s;;
  }


  Hv_SetViewSize(*View,
		 attributes[Hv_LEFT].s,  attributes[Hv_TOP].s,
		 attributes[Hv_WIDTH].s, attributes[Hv_HEIGHT].s,
		 ml, mt, mr, mb,
		 attributes[Hv_HOTRECTWIDTH].s,   attributes[Hv_HOTRECTHEIGHT].s);

/* set the world system as requested */

  Hv_SetViewWorld(*View, attributes[Hv_XMIN].f, attributes[Hv_XMAX].f,
		  attributes[Hv_YMIN].f, attributes[Hv_YMAX].f);

/* add the requested view title */

  if (attributes[Hv_TITLE].v != NULL)
    Hv_StuffViewTitle(*View, (char *)(attributes[Hv_TITLE].v));
  else  
   Hv_StuffViewTitle(*View, " ");

/* set limits on zooming */
 
  if (attributes[Hv_MINZOOMWIDTH].f > 0.0)
    (*View)->minzoomw = attributes[Hv_MINZOOMWIDTH].f;
  if (attributes[Hv_MAXZOOMWIDTH].f > 0.0)
    (*View)->maxzoomw = attributes[Hv_MAXZOOMWIDTH].f;
  if (attributes[Hv_MINZOOMHEIGHT].f > 0.0)
    (*View)->minzoomh = attributes[Hv_MINZOOMHEIGHT].f; 
  if (attributes[Hv_MAXZOOMHEIGHT].f > 0.0)
    (*View)->maxzoomh = attributes[Hv_MAXZOOMHEIGHT].f; 

/* start the silumation? */

  Hv_StartSimulation(*View,
		     (Hv_FunctionPtr)(attributes[Hv_SIMPROC].fp),
		     (unsigned long)(attributes[Hv_SIMINTERVAL].i));

  Hv_FinalTouches(*View);  


/* hot rect ? */

  if (attributes[Hv_ADDHOTRECT].i) {
    Item = Hv_SetupHotRectBorderItem(*View);  

/* if the hotrectcolor attribute is not its default,
   set up a hotrect and change its color */

    if (attributes[Hv_HOTRECTCOLOR].s != Hv_DEFAULT)
      Item->scheme->fillcolor = attributes[Hv_HOTRECTCOLOR].s;

/* motionless feedback ? */

    if (attributes[Hv_MOTIONLESSFB].i &&
	(attributes[Hv_MOTIONLESSFBINTERVAL].i > 0)) {

      (*View)->motionlessfbinterval = attributes[Hv_MOTIONLESSFBINTERVAL].i;
      (*View)->motionlessfbintervalid = XtAppAddTimeOut(Hv_appContext, (*View)->motionlessfbinterval,
						     (XtTimerCallbackProc)Hv_FeedbackTimeOut,
						     (XtPointer)(*View)); 
    }

  }


/* check out square aspect */

  Hv_SquareTheView(*View);
  Hv_LocalRectToWorldRect(*View, (*View)->hotrect, (*View)->defaultworld);

/* call the setup routine */

  if ((*View)->initialize)
    (*View)->initialize(*View);
  else
    fprintf(stderr, "Hv warning: view created without initialization function attribute\n");


/* set the "previous" world */

  Hv_CopyFRect((*View)->previousworld, (*View)->world);

/* all new views are dirty */

  Hv_SetViewDirty(*View);


/* see if configuration file has data for this view */

  Hv_ApplyConfiguration(*View);

/* final init? */

  finalinit = (Hv_FunctionPtr)(attributes[Hv_VIEWFINALINIT].fp);
  if (finalinit != NULL)
      finalinit(*View);
  
/* open at creation ? */


  if (attributes[Hv_OPENATCREATION].s != 0)
    Hv_OpenView(*View);

  Hv_UpdateVirtualView();
}


/* ----------- Hv_CloseView -------- */

void Hv_CloseView(Hv_View View)

/* handles the closing of a View. */

{
  Hv_View      temp;

/* set the proper toggle bit off */


  if (View) { 
    XtVaSetValues(View->menu, XmNset, False, NULL);
    Hv_ToggleView(View); /* run the toggle procedure which handles erasing and redrawing */
  }
  else {   /*close all open views*/
    for (temp = Hv_views.first;  temp != NULL;  temp = temp->next)
      if (Hv_ViewIsActive(temp)){  /*so we do not open closed views*/
	XtVaSetValues(temp->menu, XmNset, False, NULL);
	Hv_ToggleView(temp); /* run the toggle procedure which handles erasing and redrawing */
      }
  }

  Hv_UpdateVirtualView();
}

/* ----------- Hv_OpenView -------- */

void Hv_OpenView(Hv_View View)

/* handles the closing of a View. */

{

/* set the proper toggle bit on */

  if ((View == NULL) || (Hv_ViewIsActive(View)))
    return;

  XtVaSetValues(View->menu, XmNset, True, NULL);

/* run the toggle procedure which handles
   erasing and redrawing */

  Hv_ToggleView(View); 
  Hv_UpdateVirtualView();

}

/* ---------  Hv_EnableView  ---------*/

void  Hv_EnableView(Hv_View View)

{
  View->enabled = True;
}

/* ---------  Hv_DisableView  ---------*/

void  Hv_DisableView(Hv_View View)

{
  View->enabled = False;
}


/*------- Hv_DumpView ---------*/

void Hv_DumpView(Hv_View View,
		 FILE    *fp)

{
  fprintf(fp, "\n------- View: %s    Tag = %4d --------\n", View->containeritem->str->text, View->tag);
  Hv_DumpItemList(View->items, fp, 2);
}

/*------- Hv_DumpViewConnections ---------*/

void Hv_DumpViewConnections(Hv_View View,
			    FILE    *fp)

{
  char                   name1[12], name2[12];
  Hv_ConnectionNodePtr   connect;
  Hv_Item                Item, item1, item2;
  Hv_ConnectionDataPtr   data;
  
  fprintf(fp, "\n------- View: %s    Tag = %4d --------\n", View->containeritem->str->text, View->tag);

  for (connect = View->connections; connect != NULL; connect = connect->next) {
    Item = connect->item;
    data = (Hv_ConnectionDataPtr)(Item->data);
    item1 = data->item1;
    item2 = data->item2;
    Hv_ItemName(item1, name1);
    Hv_ItemName(item2, name2);
    fprintf(fp, "\ttag: %d\treg: %p\tmb: %d\tuser1: %d\titem1: [%p, %d, %s]\titem2: [%p, %d, %s]\n",
	    Item->tag, Item->region, data->multiplexbits, Item->user1, item1, item1->tag, name1, item2, item2->tag, name2);
  }
}

/*-------- Hv_GetViewTitle -------*/

char  *Hv_GetViewTitle(Hv_View View)

/* returns a copy that should be freed */

{
  char *title = NULL;
  
  if (View->containeritem->str->text != NULL)
    Hv_InitCharStr(&(title), View->containeritem->str->text);
  
  return title;
}


/*--------- Hv_SetViewTitle --------*/

void Hv_SetViewTitle(Hv_View   View,
		     char     *title)
{
  Hv_Region    rgn = NULL;

  Hv_StuffViewTitle(View, title);
  rgn = Hv_GetViewRegion(View);
  Hv_SetClippingRegion(rgn);

  if (Hv_ViewIsVisible(View))
      Hv_DrawViewTitle(View);
  Hv_DestroyRegion(rgn);
}


/*-------- Hv_FitFeedback ---------*/

void Hv_FitFeedback(Hv_View  View,
		    short   margin)

/* place the right and or bottom of the local to be sure as
   to include all of the feedback*/

{
  
  Hv_Item    container = View->containeritem;
  Hv_Item    fb = View->feedbackitem;
  short      minR, minB, gap;
  Hv_Rect    newlocal;

  gap =  container->scheme->borderwidth +
    container->scheme->framewidth + margin;
  
  minR = fb->area->right  + gap;
  minB = fb->area->bottom + gap;

  Hv_CopyRect(&newlocal, View->local);
  newlocal.right  = Hv_sMax(minR, newlocal.right);
  newlocal.bottom = Hv_sMax(minB, newlocal.bottom);
  Hv_FixRectWH(&newlocal);
  Hv_ChangeViewLocal(View, &newlocal);
}

/*-------- Hv_ChangeViewLocal -------*/

void Hv_ChangeViewLocal(Hv_View  View,
			Hv_Rect  *newlocal)

{
  Hv_CopyRect(View->local, newlocal);
  Hv_GetTemporaryWorld(View->defaultworld, View->hotrect, View->world, View->local);
  Hv_CopyRect(View->containeritem->area, View->local);
}


/* -------- Hv_WhichView ---------------- */

void Hv_WhichView(Hv_View  *View,
		  Hv_Point   pp)

/* returns topmost view containing the point (or NULL)*/

{
  Hv_View     temp;            /* for scanning View list */

  *View = NULL;

  temp = Hv_views.last;

  while ((*View == NULL) && (temp != NULL)) {
    if (Hv_ViewIsVisible(temp) && (Hv_PointInRect(pp, temp->local)))
      *View = temp;
    temp = temp->prev;
  }
}

/* -------- Hv_SetupWelcomeView -------*/

void Hv_SetupWelcomeView(Hv_View View)

{
  Hv_Item                 progname;
  Hv_Item		  Item;
  Hv_Item                 version;     /*version number*/
  Hv_Item                 placementitem;
  short                   position, gap, h1, h2;
  Hv_Rect                 *crect;  /* container area */

  char                    *tstr;
  char                    *hvdate;

  crect = Hv_welcomeView->containeritem->area;

  Hv_SetViewWorld(View, 0.0, 1.0, 0.0, 1.0);

/* create a TEXTITEM for the program name string */

  progname = Hv_VaCreateItem(View,
			     Hv_TYPE,         Hv_TEXTITEM,
			     Hv_TEXT,         Hv_programName,
			     Hv_LEFT,         crect->left + 18,
			     Hv_TOP,          crect->top + 30,
			     Hv_FONT,         Hv_timesBold14,
			     NULL);

  progname->doubleclick = NULL;  /* disable editor */
  h1 = progname->area->height;

/* now the date and time str */

  hvdate = Hv_GetHvTime();

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,               Hv_CLOCKITEM,
			 Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
			 Hv_BALLOON,            (char *)"An ordinary clock!",
			 Hv_PLACEMENTGAP,       15,
			 Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT,
			 Hv_TEXT,               hvdate,
			 Hv_FONT,               Hv_timesBold14,
			 Hv_REDRAWTIME,         60000,
			 NULL);

  Hv_Free(hvdate);
  h2 = Item->area->height;
  
/* align the progname and the clock */

  progname->area->top = Item->area->top + (h2 - h1)/2;
  Hv_FixRectRB(progname->area);

/* allocate a RAINBOW item for diagnostics */

  if (Hv_addWelcomeRainbow) {
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,              Hv_RAINBOWITEM,
			   Hv_TAG,               Hv_WELCOMERAINBOWTAG,
			   Hv_PLACEMENTITEM,     progname,
			   Hv_PLACEMENTGAP,      8,
			   Hv_EDITABLE,          False,
			   Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW,
			   Hv_COLORWIDTH,        3,
			   Hv_HEIGHT,            15,
			   Hv_NUMLABELS,         0,
			   Hv_BALLOON,           "Should be a rainbow, if the color allocation worked.",
			   NULL);
    placementitem = Item;
    position = Hv_POSITIONRIGHT;
    gap = 15;
  }
  else {
    placementitem = progname;
    position = Hv_POSITIONBELOW;
    gap = 8;
  }


/* now the version string */


  if (Hv_versionString != NULL) {
    tstr = (char *)Hv_Malloc(50 + strlen(Hv_programName));
    strcpy(tstr, (char *)"Specifies what version of ");
    strcat(tstr, Hv_programName);
    strcat(tstr, (char *)" you are running.");
    
    
    version = Hv_VaCreateItem(View,
			      Hv_DRAWCONTROL,        Hv_FRAMEAREA,
			      Hv_TYPE,               Hv_TEXTITEM,
			      Hv_TEXT,               Hv_versionString,
			      Hv_BALLOON,            tstr,
			      Hv_FILLCOLOR,          Hv_canvasColor,
			      Hv_PLACEMENTGAP,       gap,
			      Hv_PLACEMENTITEM,      placementitem,
			      Hv_RELATIVEPLACEMENT,  position,
			      Hv_TAG,                Hv_VERSIONSTRINGTAG,
			      NULL);

    if (Hv_canvasColor == Hv_black)
      version->str->strcol = Hv_white;

    version->doubleclick = NULL;  /* disable editor */
    Hv_Free(tstr);


/* a lttle more aligning */

    if (placementitem == progname) {
      h1 = progname->area->width;
      h2 = version->area->width;
      version->area->left = progname->area->left + (h1-h2)/2;
      Hv_FixRectRB(version->area);
    }

  }
    
  Hv_AutosizeView(View);                /*Make it just fit around its items*/
}

/*---------- Hv_DrawNonBackgroundItems --------------- */

void	Hv_DrawNonBackgroundItems(Hv_View     View,
				  Hv_Region	region)

/* Draws NON BACKGROUND hotrect items */

/* NEVER called in postscript mode */


{
  Hv_Item	temp;

  if (View == NULL)
    return;

  for (temp = View->items->first;  temp != NULL;  temp = temp->next) {
    if ((temp->domain == Hv_INSIDEHOTRECT)
	&& !Hv_CheckItemDrawControlBit(temp, Hv_INBACKGROUND)) {

      
      if (!Hv_inPostscriptMode || 
	  (temp->view->tag != Hv_XYPLOTVIEW) ||
	  (Hv_printThisPlotOnly == NULL) ||
	  (temp == Hv_printThisPlotOnly) ||
	  (temp->parent == Hv_printThisPlotOnly)) 
	Hv_MaybeDrawItem(temp, region);
	
/* sublist? */
      
      if (temp->children != NULL)
	Hv_DrawItemList(temp->children, region);
    }
    
  }
}

/*---------- Hv_DrawHotRectItems --------------- */

void	Hv_DrawHotRectItems(Hv_View     View,
			    Hv_Region	region)

{
  Hv_Item	temp;
  Hv_Region     hrrgn = NULL;
  Hv_Region     drawregion = NULL;
  Boolean       normaldraw = True;

  if (View == NULL)
    return;

  if (Hv_inPostscriptMode) {
    Hv_PSComment("Drawing HotRect Items");
    normaldraw = True;
  }

  hrrgn = Hv_ClipHotRect(View, 1);

  if (region != NULL) {
    drawregion = Hv_IntersectRegion(hrrgn, region);
    Hv_DestroyRegion(hrrgn);
  }
  else
    drawregion = hrrgn;
  
/* must check whether this view saves the background */
    
  if (Hv_ViewSavesBackground(View) && !Hv_inPostscriptMode) {
    if (Hv_IsViewDirty(View))
      Hv_SaveViewBackground(View, drawregion);
    
/* if STILL dirty, could not do the off-screen drawing */
      
    normaldraw = Hv_IsViewDirty(View);
      
  }

/* if not normal draw, then we must restore the saved background */
    
  if (!normaldraw) {
    Hv_SetClippingRegion(drawregion);
    Hv_RestoreViewBackground(View);
    
/* if reset to DIRTY, then restore failed */

    if (Hv_IsViewDirty(View))
      normaldraw = True;
    else  /* successfule restore */
      Hv_DrawNonBackgroundItems(View, drawregion);
  }

  if (normaldraw) { /* PS drawing ALWAYS comes here */

      for (temp = View->items->first;  temp != NULL;  temp = temp->next) {
	  if (temp->domain == Hv_INSIDEHOTRECT) {

	      if (!Hv_inPostscriptMode || 
		  (temp->view->tag != Hv_XYPLOTVIEW) ||
		  (Hv_printThisPlotOnly == NULL) ||
		  (temp == Hv_printThisPlotOnly) ||
		  (temp->parent == Hv_printThisPlotOnly)) {
		  
		  Hv_MaybeDrawItem(temp, drawregion);
	      }
	  
/* sublist? */
      
	      if (temp->children != NULL)
		  Hv_DrawItemList(temp->children, drawregion);
	  }
      }

/* offscreen userdraw? (normally handled in off screen drawing, but
   we may be here because a) offscrren drawing failed or b) in PS
   mode so there was no off screen drawing */

      if (View->offscreenuserdraw != NULL)
	  View->offscreenuserdraw(View, hrrgn);
      
  }
  
  Hv_DestroyRegion(drawregion);

  if (Hv_inPostscriptMode)
    Hv_PSComment("Done Drawing HotRect Items");
}


/* ---------  Hv_MallocView  ---------*/

static Hv_View Hv_MallocView(void)

/*  Mallocs a new View setting its id field to ID and its type to lotype.
    The View is added at the end of the global list Hv_views */
{
  Hv_View       View;

  View = (Hv_View) Hv_Malloc(sizeof(ViewRecord));  /* beg for the memory */
  View->enabled = True;
  View->menu = NULL;
  View->data = NULL;

/* malloc various rectangles */

  View->local         = Hv_NewRect();
  View->hotrect       = Hv_NewRect();
  View->previouslocal = Hv_NewRect();
  View->previousworld = Hv_NewFRect();
  View->defaultworld  = Hv_NewFRect();
  View->world         = Hv_NewFRect();

  View->modifiers = 0;
  View->background = 0;

/* make sure the item listhead is initialized */

  View->items = (Hv_ItemList)Hv_Malloc(sizeof(Hv_ItemListData));
  View->items->first = NULL;
  View->items->last = NULL;

  View->feedbackitem       = NULL;  /* set the feedback pointer to null for now */
  View->hotrectborderitem  = NULL;  /* set the hotrectborder pointer to null for now */
  View->brickwallitem      = NULL;  /* set the brickwall pointer to null for now */

/* undo related */

  View->lastdragitem = NULL;
  View->lastrotateitem = NULL;

/* every View has a container as its first Item */

  View->containeritem = Hv_VaCreateItem(View,
					Hv_TYPE,         Hv_CONTAINERITEM,
					Hv_RESIZEPOLICY, Hv_RESIZEONRESIZE,
					NULL);

/* set the previous and next View pointers to null, will be reset when inserted in list */

  View->next = NULL;
  View->prev = NULL;
  return View;
}


/*--------- Hv_FinalTouches --------*/

static void Hv_FinalTouches(Hv_View View)

{
  static unsigned short  mcount = 0;
  char                   accchr = ' ';

/*see if scroll bars are requested */

  if (Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE)) {
    View->hscroll = MallocScrollBar();
    View->vscroll = MallocScrollBar();
  }
  else {
    View->hscroll = NULL;
    View->vscroll = NULL;
  }

/* now place the View in last position in global View list */

  if (!Hv_views.last)	                    /*i.e 1st member of View list */
    Hv_views.first = View;
  else  {	                                  /* add to end and make links */
    Hv_views.last->next = View;
    View->prev = Hv_views.last;   
  }
  
/* in any event, listhead.last should point to newly added guy */
  
  Hv_views.last = View;

/* in the user's init, the world may have been changed such
   that scrolling is possible even on startup */

  Hv_SetScrollActivity(View);

/* now add this view to the views menu. If mcount < 10, use its
   value as an accelerator */

  if (mcount < 10)
    accchr = '0' + (mcount)++;
  
  View->menu = Hv_VaCreateMenuItem(Hv_viewMenu,
				Hv_LABEL,           View->containeritem->str->text,
				Hv_ID,              (XtPointer)View,
				Hv_CALLBACK,        Hv_DoViewMenu,
				Hv_ACCELERATORTYPE, Hv_ALT,
				Hv_ACCELERATORCHAR, accchr,
				NULL);
/* connections */

  View->editconnections = False;
  View->drawconnections = True;
  View->drawarrowheads  = True;
  View->connections = NULL;
}

/*-------- Hv_StuffViewTitle -------*/

static void  Hv_StuffViewTitle(Hv_View   View,
			       char    *title)

{
  if (View->containeritem->str->text != NULL)
    Hv_Free(View->containeritem->str->text);
  
  Hv_InitCharStr(&(View->containeritem->str->text), title);

  if (View->menu != NULL)
    Hv_SetMenuItemString(View->menu, title, Hv_DEFAULT);
}

/* ---------   Hv_SendViewToEndOfList ---------*/

static void  Hv_SendViewToEndOfList(Hv_View View)

/*  Makes the given View appear on top of all others by */
/*  switching it to LAST of the View list*/

{

  if ((View != NULL) && (Hv_views.last != NULL) && (Hv_views.last != View)) {
    View->next->prev = View->prev;      /* connect one after to one before */

    if (View->prev == NULL)             /* View was first if this is true */
      Hv_views.first = View->next;
    else
      View->prev->next = View->next;    /* connect one before to one after */
    
    Hv_views.last->next = View;         /* View now follows he who used to be last */
    View->prev = Hv_views.last;         /* previous is he who used to be last */
    Hv_views.last = View;               /* since it is the last! */
    View->next = NULL;                  /* since there is no one after me! */
  }
}

/* --------- Hv_SendViewToTopOfList  ---------*/

static void  Hv_SendViewToTopOfList(Hv_View View)

/*  Makes the given View appear at bottom of all others by
    switching it to FIRST in the View list. NOTE: this
    doesn't do the redrawing*/

{

  if ((View != NULL) && (Hv_views.first != NULL) && (Hv_views.first != View)) {
    View->prev->next = View->next;   /* connect one after to one before */

    if (View->next == NULL)            /* View was last if this is true */
      Hv_views.last = View->prev;
    else
      View->next->prev = View->prev;  /* connect one before to one after */
    
    View->next = Hv_views.first;        /* previous is he who used to be first */
    Hv_views.first->prev = View;       
    Hv_views.first = View;              /* since it is the first! */
    View->prev = NULL;                   /* since there is no one before me! */
  }
}


/* ----- Hv_DoViewMenu ----- */

static void  Hv_DoViewMenu(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data)

{
  Hv_View     View;

  Hv_CheckDanglingTextItem();
  Hv_HideBalloon();

  View = (Hv_View)client_data;

/* if view is already active, bring it to the top. If
   not visible, open it */

  if (Hv_ViewIsActive(View))
    Hv_SendViewToFront(View);
  else
    Hv_OpenView(View);

/* center virtual screen around view, if applicable */

  Hv_CenterVVAroundView(View);
  
}


/*------ Hv_QuickZoomResize -----*/

static void  Hv_QuickZoomResize(Hv_View  View,
				float    xmin,
				float    xmax,
				float    ymin,
				float    ymax)

{
  Hv_FRect   DesiredWorld;

  if (Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    return;

  Hv_SetFRect(&DesiredWorld, xmin, xmax, ymin, ymax);
  Hv_GetTemporaryWorld(&DesiredWorld, View->hotrect, View->world, View->local);
  Hv_SquareTheView(View);

/* reposition hotrect items */

  Hv_RepositionHotRectItems(View->items);
}


/* --------  Hv_ViewBoundingRect -------*/

static void   Hv_ViewBoundingRect(Hv_View View,
				  XRectangle *rect)

/* converts the Views container local system to a rectangle */

{
  Hv_Rect    *area;

  area = View->containeritem->area;
  Hv_SetXRectangle(rect, area->left, area->top, area->width, area->height);
}


/* ---------  Hv_ToggleView  ---------*/

static void   Hv_ToggleView(Hv_View View)

/* Toggle the Hv_ACTIVE bit in the View's draw control*/

{
  Hv_Region   viewrgn;
  Hv_Rect     area;

  if (View == NULL)
    return;
    
  if (Hv_ViewIsActive(View)) {
    viewrgn = Hv_GetViewRegion(View);
    Hv_CopyRect(&area, View->containeritem->area);
    Hv_HideView(View);
    Hv_SetClippingRegion(viewrgn);         /* set region to clipping region*/
    Hv_DestroyRegion(viewrgn);
  }
  else {
    Hv_SetBit(&(View->drawcontrol), Hv_ACTIVE);
    if ((Hv_mainWindow != 0) && (!Hv_firstExposure)) {
      Hv_DrawView(View, NULL);
      Hv_SendViewToFront(View);
    }
  }
}



/* -------- Hv_OffsetView ----- */

void Hv_OffsetView (Hv_View  View,
		    short    dh,
		    short    dv)

{
/* offsets a View by the specified amount, offsetting all child items
   DOES NOT IN AND OF ITSELF REDRAW! */

  if (View != NULL) {
    Hv_OffsetRect(View->local, dh, dv);              /* offset the local area */
    Hv_OffsetRect(View->hotrect, dh, dv);            /* offset the hot-rect */
    Hv_OffsetItemList(View->items, dh, dv);   /* offset all children */
  }
}


/* -------- Hv_ResizeView ----- */


void Hv_ResizeView(Hv_View  View,
		   short    dh,
		   short    dv)

{

/* resizes a View by the specified amount, resizing all child items
   DOES NOT DO ANY REDRAWING */

  Hv_FRect      tempWorld;              /*temporary world system*/
  Hv_FRect      oldworld;
  Hv_FRect      *world;
  Hv_Rect       oldlocal;
  Hv_Rect       *local, *hotrect;
  short         xoff, yoff;
  float         rx, ry;
  short         l, t, r, b;
  Hv_Item       temp;

  if (View == NULL)
    return;

  Hv_HideBalloon();

  local = View->local;
  world = View->world;
  hotrect = View->hotrect;
  
  Hv_CopyRect(&oldlocal,  local);
  Hv_CopyFRect(&oldworld, world);
  
  Hv_LocalRectToWorldRect(View, hotrect, &tempWorld);
  
  Hv_ResizeRect(local, dh, dv);              /* resize the local area */
  Hv_ResizeRect(hotrect, dh, dv);            /* resize the hot-rect */
  
/* if we are supposed to zoom on resize, then adjust the world system */
  
  if (Hv_CheckBit(View->drawcontrol, Hv_ZOOMONRESIZE)) {
    if (!Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD)) {
      Hv_GetTemporaryWorld(&(tempWorld), hotrect, world, local);
      Hv_SquareTheView(View);
    }
    Hv_RepositionHotRectItems(View->items);
  }
  else if (!Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD)) {
    
    xoff = oldlocal.left;
    yoff = oldlocal.top;
    
    rx = oldworld.width/((float)oldlocal.width);
    ry = oldworld.height/((float)oldlocal.height);
    
    Hv_GetRectLTRB(local, &l, &t, &r, &b);
    
    world->xmax = oldworld.xmin + rx*(r - xoff);
    world->ymin = oldworld.ymax + ry*(yoff - b);
      Hv_FixFRectWH(world);
    Hv_SquareTheView(View);
  }
  
  Hv_ResizeItemList(View->items, dh, dv);             /* resize all children */

/* now the view is dirty (in case background save is used ) */

  Hv_SetViewDirty(View);


/* plot views need a little special treatment */

  if (View->tag == Hv_XYPLOTVIEW)
    for (temp = View->items->first;  temp != NULL;  temp = temp->next)
      if (temp->type == Hv_PLOTITEM)
	Hv_CheckPlotAttachments(temp);
  
/* see if user set up additional action */

  if (Hv_UserResize != NULL)
    Hv_UserResize(View, dh, dv);
}

/* ---------  Hv_HideView ---------*/

static void  Hv_HideView(Hv_View View)


/*  Hides the View with given ID in global list Hv_views
  The View is not freed, it is "erased" */

{
  Hv_Region        region;

  if (View != NULL) {
    region = Hv_GetViewRegion(View);         /* get min region the View needs */
    Hv_ClearBit(&(View->drawcontrol), Hv_ACTIVE);
    Hv_EraseRegion(region);
    Hv_UpdateViews(region);
    Hv_DestroyRegion(region);
  }
  Hv_HideBalloon();
}



/* -------- Hv_SquareTheView ------ */

static void Hv_SquareTheView(Hv_View View)

/* for a square aspect ratio */

{
  float         wp, wp2, hp, hp2, del, alocal, aworld;

  if (!Hv_CheckBit(View->drawcontrol, Hv_SQUARE))
    return;

  if (Hv_CheckBit(View->drawcontrol, Hv_FIXEDWORLD))
    return;

  alocal = ((float)View->local->width)/((float) View->local->height);
  aworld = View->world->width/View->world->height;

/*change by increasing the world system*/

  if (aworld > alocal) {
    hp = View->world->width/alocal;
    del = fabs(hp - View->world->height)/View->world->height;
    if (del > Hv_MINCHANGE) {
      hp2 = (hp-View->world->height)/2.0;
      View->world->ymin = View->world->ymin-hp2;
      View->world->height = hp;
      View->world->ymax = View->world->ymin+hp;
    }
  }
  else {
    wp = View->world->height*alocal;
    del = fabs(wp - View->world->width)/View->world->width;
    if (del > Hv_MINCHANGE) {
      wp2 = (wp-View->world->width)/2.0;
      View->world->xmin = View->world->xmin-wp2;
      View->world->width = wp;
      View->world->xmax = View->world->xmin+wp;
    }
  }
  
}

/*------- Hv_AfterZoomDraw -----------*/

static void  Hv_AfterZoomDraw(Hv_View View)

/*  Call whenever the world coordinate system
    has changes, typically after a zoom or shrink.
    it a) redraws the hotrect
       b) redraws the scroll bars
    but it leaves the rest of the view untouched */

{
  Hv_Region    region;

  Hv_SetScrollActivity(View);

/* make undo zoom selectable */

  Hv_SetBit(&(View->popupcontrol), Hv_UNDOZOOM);
  XtSetSensitive(Hv_popupItems[Hv_FirstSetBit(Hv_UNDOZOOM)], True);
  
  if (Hv_ViewIsVisible(View)) {
    region = Hv_GetViewRegion(View);
    Hv_DrawScrollBars(View, region);
    Hv_DestroyRegion(region);
    Hv_DrawViewHotRect(View);
  }
}

#undef    Hv_MINCHANGE


/*---------Hv_SetViewSize ------------ */

static void Hv_SetViewSize(Hv_View  View,
			   short   left,
			   short   top,
			   short   width,
			   short   height,
			   short   ml,
			   short   mt,
			   short   mr,
			   short   mb,
			   short   hrwidth,
			   short   hrheight)

/* note: top is relative to the canvas super rect -- thus this routine
   will add the canvas Rect's top -- this basically takes the
   menu bar into account */

/* also: containers framewidth and borderwidth will be
   added to the hotrect margins */


{
  Hv_Item     container;
  short       bw, fw, extra;

  if (View == NULL)
    return;

  container = View->containeritem;
  fw = container->scheme->framewidth + 2;
  bw = container->scheme->borderwidth;
  extra = fw + bw + 2;

  top += Hv_canvasRect.top;

/* any autosizing? (meaning only the hotect width/height
   was provided, not the view's. */

  if (width <= 0)
    width = hrwidth + ml + mr + extra + fw;

  if (height <= 0)
    height = hrheight + 2*extra + mt + mb;

/* if no scroll bars, we made the widht and height to big */

  if (!Hv_CheckBit(View->drawcontrol, Hv_SCROLLABLE)) {
    height -= bw;
    width -= bw;
  }


/* now can set the local rect, which defines the view's
   overall size */
  
  Hv_SetRect(View->local, left, top, width, height);

/* default the previous local system to the entire window! */
    
  Hv_SetRect(View->previouslocal, 0, 0, 32000, 32000);
  Hv_CopyRect(View->containeritem->area, View->local);

/* now can size the all important "hotrect" */

  Hv_SetRect(View->hotrect,
	     left + ml + fw,
	     top + mt + extra,
	     hrwidth, hrheight);
}

/*------- Hv_SetViewWorld ----------*/

static void Hv_SetViewWorld(Hv_View View,
			    float   xmin,
			    float   xmax,
			    float   ymin,
			    float   ymax)

{
  Hv_SetFRect(View->defaultworld, xmin, xmax, ymin, ymax);

  View->minzoomw = View->defaultworld->width/100.0;
  View->minzoomh = View->defaultworld->height/100.0;

  View->maxzoomw = 2.0*View->defaultworld->width;
  View->maxzoomh = 2.0*View->defaultworld->height;

/* now we have the world that we want to correspond to the
   hotrect -- we must make the actual world correspond
   accordingly with the actual local*/

  Hv_GetTemporaryWorld(View->defaultworld, View->hotrect, View->world, View->local);
}


/* --------- Hv_PrepareForViewChange -----*/

static void Hv_PrepareForViewChange(Hv_View  View,
				    Hv_Region *oldtot,
				    Hv_Region *oldrgn)

{
  Hv_HideBalloon();
  *oldtot = Hv_TotalViewRegion();
  *oldrgn = Hv_GetViewRegion(View); 
}

/* --------- Hv_FinishViewChange -----*/

static void Hv_FinishViewChange(Hv_View  View,
				Hv_Region oldtot,
				Hv_Region oldrgn)
{
  Hv_Region newrgn   = NULL;
  Hv_Region diffrgn  = NULL;
  Hv_Region newtot   = NULL;
  Hv_Region isectrgn = NULL;

  newrgn = Hv_GetViewRegion(View);
  newtot = Hv_TotalViewRegion();

/* erase the difference between the old tot and the new tot, since
   that region is now "bare" */

  diffrgn = Hv_SubtractRegion(oldtot, newtot);
  Hv_EraseRegion(diffrgn);
  Hv_DestroyRegion(diffrgn);

  diffrgn = Hv_SubtractRegion(oldrgn, newrgn);

  if (diffrgn != NULL) {
    isectrgn = Hv_IntersectRegion(diffrgn, newtot);
    Hv_FillRegion(isectrgn, View->containeritem->scheme->fillcolor, isectrgn);
  }

  Hv_DrawView(View, newrgn);
  Hv_UpdateViews(isectrgn);

  Hv_DestroyRegion(diffrgn);
  Hv_DestroyRegion(newrgn);
  Hv_DestroyRegion(newtot);
  Hv_DestroyRegion(isectrgn);
}


/*------ Hv_FeedbackTimeOut ------*/

static void Hv_FeedbackTimeOut(XtPointer    xtptr,
			       XtIntervalId  *id)

/**************************************
  This routine is called for views that
  have "motionless" feedback
***************************************/
{
  Hv_View     View;
  Hv_Point    pp;
  
  View = (Hv_View)xtptr;

  Hv_GetCurrentPointerLocation(&(pp.x), &(pp.y));

  if (Hv_ViewIsVisible(View) && 
      (pp.x > 0) && (pp.y > 0) &&
      (View->feedback != NULL) &&
      Hv_PointInRect(pp, View->hotrect))
    View->feedback(View, pp);

/* restart the timer */

  View->motionlessfbintervalid = XtAppAddTimeOut(Hv_appContext, View->motionlessfbinterval,
						 (XtTimerCallbackProc)Hv_FeedbackTimeOut,
						 xtptr); 
  
}

/*------ Hv_SimulationTimeOut ------*/

static void Hv_SimulationTimeOut(XtPointer    xtptr,
                                  XtIntervalId  *id)
{
  Hv_View    View;
  
  View = (Hv_View)xtptr;
  
  if (View->simproc == NULL) {
    View->simintervalid = 0;
    return;
  }

  if (View->siminterval < 1)
    return;

/* call the sim proc and then reset the simulatio with the same interval */


/*6/28   TEST: changed order so that callback is set first,
  the user proc called */

  View->simintervalid = XtAppAddTimeOut(Hv_appContext, View->siminterval,
					(XtTimerCallbackProc)Hv_SimulationTimeOut, xtptr); 
  View->simproc(View);
}

/* ------ MallocScrollBar ------------ */

static Hv_ScrollInfoPtr MallocScrollBar(void)

{

/* beg for the memory necessary for a scroll bar, setting proper defaults */

  Hv_ScrollInfoPtr  TheSI;
  TheSI = (Hv_ScrollInfoPtr) Hv_Malloc(sizeof(Hv_ScrollInfo));
  TheSI->minval = 0.0;
  TheSI->maxval = 0.0;
  TheSI->val = 0.0;
  TheSI->active = False;
  TheSI->arrow_step = 6;
  TheSI->slide_step = 80;
  return TheSI;
}
