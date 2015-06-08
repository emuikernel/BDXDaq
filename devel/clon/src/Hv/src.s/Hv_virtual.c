/*
==================================================================================
Hv.virtual.c contains the virtual desktop routines

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of they U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include   "Hv.h"

#define VIEWFBTAG      1001
#define VVVIEWREPTAG  -8372

typedef struct vvdata *Hv_VVData;

typedef struct vvdata

{
  Hv_Item         viewport;    /* item serving as viewport */
  short           vw, vh;      /* requested virtual screen size */
} Hv_VVDataRec;

/*------ local prototypes ------*/

static void  Hv_DestroyViewRepItem(Hv_Item Item);

static Hv_Item Hv_PointInViewRep(Hv_ItemList  items,
				 Hv_Point     pp);
     
static void    Hv_ViewRepDoubleClick(Hv_Event hvevent);

static Hv_Item Hv_GetViewRepItem(Hv_View View);

static void Hv_TurnViewRepsOff(Hv_View View);

static void Hv_TurnViewRepsOn(Hv_View View);

static Hv_View Hv_GetViewRepView(Hv_Item Item);

static char   *Hv_GetViewRepViewTitle(Hv_Item Item);

static void Hv_LocalToVV(Hv_View View,
			 short *vx,
			 short *vy,
			 short lx,
			 short ly);

static void Hv_LocalRectToVVRect(Hv_View View,
				 Hv_Rect *lrect,
				 Hv_Rect *vvrect);

static void Hv_ViewPortAfterDrag(Hv_Item Item,
				 short dh,
				 short dv);

static void Hv_ViewRepAfterDrag(Hv_Item Item,
				 short dh,
				 short dv);

static void Hv_SetViewPortArea(Hv_Item Item);

static void Hv_DrawViewPortItem(Hv_Item Item,
				Hv_Region region);

static void Hv_DrawViewRepItem(Hv_Item Item,
			       Hv_Region region);

static void Hv_VVSetup(Hv_View View);

static void Hv_VVFeedback(Hv_View View,
			  Hv_Point pp);

static void Hv_MallocVVData(Hv_View View,
			    short w,
			    short h);

static Hv_VVData Hv_GetVVData(Hv_View View);


static short vvw, vvh;
static short fbfont, fbfill, fbdatacols, fbtextcolor;

/*-------- Hv_CenterVVAroundView -------------*/

void   Hv_CenterVVAroundView(Hv_View View)

/* this is called when the user has selected
   the view from the views menu. If the view
   is off the virtual view, we adjust the
   view so that it is centered */

{
  Hv_Item       vrep;
  Hv_EventData  hverec; 

/* if the view is visible, return */

  if (Hv_ViewIsVisible(View))
    return;

  if (Hv_virtualView == NULL)
    return;

  vrep = Hv_GetViewRepItem(View);

  hverec.view = vrep->view;
  hverec.item = vrep;

  Hv_SendViewToFront(Hv_virtualView);
  Hv_ViewRepDoubleClick(&hverec);
}



/*------ Hv_UpdateVirtualView -------*/

void  Hv_UpdateVirtualView(void)

/* This routine should be called by anything
   that can effect the appearance of the virtual
   desktop */


{
  Hv_VVData  vv;

  if (Hv_virtualView == NULL)
    return;

  Hv_drawingOn = False;
  Hv_TurnViewRepsOff(Hv_virtualView);
  Hv_TurnViewRepsOn(Hv_virtualView);
  vv = Hv_GetVVData(Hv_virtualView);


/* always put the viewport on top */

  Hv_SendItemToFront(vv->viewport);
  Hv_drawingOn = True;

  Hv_DrawViewHotRect(Hv_virtualView);
}

/**
 * Hv_DeleteVVRep
 * @purpose   Called when a View id deleted, so that its
 * corresponding virtual view rep is also deleted.
 * @param  View    View that was deleted.
 */

void Hv_DeleteVVRep(Hv_View View) {
  Hv_Item  vrep;

  if (Hv_virtualView == NULL)
    return;

  vrep = Hv_GetViewRepItem(View);
  if (vrep == NULL)
    return;

  Hv_DeleteItem(vrep);
}


/*------ Hv_SendVVRepToFront -------*/

void Hv_SendVVRepToFront(Hv_View View)

{
  Hv_Item  vrep;

  if (Hv_virtualView == NULL)
    return;

  vrep = Hv_GetViewRepItem(View);
  if (vrep == NULL)
    return;

  Hv_SendItemToFront(vrep);
  Hv_UpdateVirtualView();
}


/*------ Hv_SendVVRepToBack -------*/

void Hv_SendVVRepToBack(Hv_View View)

{
  Hv_Item  vrep;

  if (Hv_virtualView == NULL)
    return;

  vrep = Hv_GetViewRepItem(View);
  if (vrep == NULL)
    return;

  Hv_SendItemToBack(vrep);
  Hv_UpdateVirtualView();
}

/*------ Hv_TurnViewRepsOn ---------*/

static void Hv_TurnViewRepsOn(Hv_View View)

{
  Hv_View    temp;
  Hv_Item    Item;
  Hv_Rect    *hotrect;

  if (View == NULL)
    return;

  hotrect = View->hotrect;

  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) 
    if (temp != Hv_virtualView)
      if (Hv_ViewIsActive(temp)) {

	Item = Hv_GetViewRepItem(temp);

	if (Item != NULL)
	  Hv_ClearItemDrawControlBit(Item, Hv_DONTDRAW);
	else { /* first time for this view */
	  Item = Hv_VaCreateItem(View,
				 Hv_TYPE,        Hv_USERITEM,
				 Hv_DOUBLECLICK, Hv_ViewRepDoubleClick,
				 Hv_TAG,         VVVIEWREPTAG,
				 Hv_DATA,        (void *)temp,
				 Hv_DRAWCONTROL, Hv_DRAGABLE+Hv_STRICTHRCONFINE,
				 Hv_AFTERDRAG,   Hv_ViewRepAfterDrag,
				 Hv_LEFT,        hotrect->left,
				 Hv_TOP,         hotrect->top,
				 Hv_WIDTH,       100,
				 Hv_HEIGHT,      100,
				 Hv_USERDRAW,    Hv_DrawViewRepItem,
				 Hv_DESTROY,     Hv_DestroyViewRepItem,
				 NULL);
	

	  Item->singleclick = NULL;
	}
      }
  
}

/*------- Hv_CreateVirtualView ------*/

void  Hv_CreateVirtualView(Hv_AttributeArray attributes)

/* This creates the "virtual desktop" viee that allows
   you to have an effectively larger screen

   w and h are the size your want for your virtual desktop
   (in pixels)

 */

{
  short      l, t, hrw, hrh, hrfill;   
  float      fw, fh;

  vvw = Hv_sMax(1000, Hv_sMin(10000, attributes[Hv_VIRTUALWIDTH].s));
  vvh = Hv_sMax(1000, Hv_sMin(10000, attributes[Hv_VIRTUALHEIGHT].s));

  l = 10;
  t = 40;

/* the hot rect height is fixed, hrh to give aspect
   ratio of requested virtual desk top*/

  hrw    = attributes[Hv_VIRTUALVIEWHRW].s;
  fbfont = attributes[Hv_VIRTUALVIEWFBFONT].s;
  fbdatacols = attributes[Hv_VIRTUALVIEWFBDATACOLS].s;

  fbfill = attributes[Hv_VIRTUALVIEWFBFILL].s;
  if (fbfill < 0)
    fbfill = Hv_white-6;

  fbtextcolor = attributes[Hv_VIRTUALVIEWFBTEXTCOLOR].s;
  if (fbtextcolor < 0)
    fbtextcolor = Hv_black;

/* hrfill = attributes[Hv_VIRTUALVIEWHRFILL].s; */
	hrfill = Hv_white-9;


  fw = (float)vvw;
  fh = (float)vvh;

  hrh = (int)((fh/fw)*hrw);

  Hv_VaCreateView(&Hv_virtualView,
		  Hv_LEFT,            l,
		  Hv_TOP,             t,
		  Hv_HOTRECTWIDTH,    hrw,
		  Hv_HOTRECTHEIGHT,   hrh,
		  Hv_HOTRECTTMARGIN,  attributes[Hv_VIRTUALVIEWTMARGIN].s,
		  Hv_HOTRECTLMARGIN,  attributes[Hv_VIRTUALVIEWLMARGIN].s,
		  Hv_HOTRECTRMARGIN,  attributes[Hv_VIRTUALVIEWRMARGIN].s,
		  Hv_HOTRECTBMARGIN,  attributes[Hv_VIRTUALVIEWBMARGIN].s,
		  Hv_TAG,             Hv_VIRTUALVIEW,
		  Hv_TITLE,           "Virtual Desktop",
		  Hv_INITIALIZE,      Hv_VVSetup,
		  Hv_POPUPCONTROL,    Hv_MINPOPUPCONTROL - Hv_UNDOVIEW,
		  Hv_OPENATCREATION,  True,
		  Hv_FEEDBACK,        Hv_VVFeedback,
		  Hv_HOTRECTCOLOR,    hrfill,
		  NULL);

}


/*-------- Hv_VVFeedback -------*/

static void Hv_VVFeedback(Hv_View View,
			  Hv_Point pp)

{
  char *text;

  Hv_Item     Item;

  Item = Hv_PointInViewRep(View->items,
			     pp);

  if (Item == NULL)
    Hv_ChangeFeedback(View, VIEWFBTAG, "  (none)");
  else {
    text = Hv_GetViewRepViewTitle(Item);
    Hv_ChangeFeedback(View, VIEWFBTAG, text);
    Hv_Free(text);
  }
}


/*------ Hv_PointInViewRep ------*/

static Hv_Item Hv_PointInViewRep(Hv_ItemList  items,
				 Hv_Point     pp)
     

/* returns the first (backwards) match. */

{
  Hv_Item     temp;
  Hv_View     View;

  if (items == NULL)
    return NULL;

  for (temp = items->last; temp != NULL; temp = temp->prev)
    if (temp->domain == Hv_INSIDEHOTRECT)
      if (temp->type == Hv_USERITEM)
	if (temp->tag == VVVIEWREPTAG) 
	  if (Hv_PointInItem(pp, temp)) {
	    View = (Hv_View)(temp->data);
	    if (Hv_ViewIsActive(View))
	      return temp;
	  }

  return NULL;
}

/*-------- Hv_VVSetup --------*/

static void Hv_VVSetup(Hv_View View)

{
  Hv_VVData vv;
  Hv_Item   Item;
  Hv_Rect   *hotrect = View->hotrect;

  Hv_MallocVVData(View, vvw, vvh);
  vv = Hv_GetVVData(View);

/* add the simple feedback item */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,              Hv_FEEDBACKITEM,
			 Hv_LEFT,              hotrect->left-3,
			 Hv_TOP,               hotrect->top-30,
			 Hv_FEEDBACKDATACOLS,  fbdatacols,
			 Hv_FEEDBACKENTRY,     VIEWFBTAG, fbfont, fbtextcolor, " ",
			 Hv_RESIZEPOLICY,      Hv_DONOTHINGONRESIZE,
			 Hv_FILLCOLOR,         fbfill,
			 NULL);

  Item->area->right = hotrect->right+3;
  Hv_FixRectWH(Item->area);

/* add the viewport */

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,              Hv_USERITEM,
			 Hv_DRAWCONTROL,       Hv_DRAGABLE + Hv_STRICTHRCONFINE,
			 Hv_AFTERDRAG,         Hv_ViewPortAfterDrag,
			 Hv_LEFT,              hotrect->left,
			 Hv_TOP,               hotrect->top,
			 Hv_WIDTH,             100,
			 Hv_HEIGHT,            100,
			 Hv_USERDRAW,          Hv_DrawViewPortItem,
			 Hv_USER1,             3,  /* for drag slop */
			 NULL);
  vv->viewport = Item;
  Hv_UpdateVirtualView();
}

/*---------- Hv_MallocVVData ------*/

static void Hv_MallocVVData(Hv_View View,
			    short w,
			    short h)
     
{
  Hv_VVData   vv;

  vv = (Hv_VVData)Hv_Malloc(sizeof(Hv_VVDataRec));
  vv->viewport = NULL;
  vv->vw = w;
  vv->vh = h;
  View->data = (void *)vv;
}

/*----------- Hv_GetVVData -----*/

static Hv_VVData Hv_GetVVData(Hv_View View)

{
  return (Hv_VVData)(View->data);
}



/*------ Hv_DrawViewRepItem -----*/

static void Hv_DrawViewRepItem(Hv_Item Item,
			       Hv_Region region)

/* draw the small rect that represents the view */

{
  Hv_View   VRView;   /* the view this item represents */
  Hv_Rect   tinyhr;

  VRView = Hv_GetViewRepView(Item);

  Hv_LocalRectToVVRect(Item->view,
		       VRView->local,
		       Item->area);

  Hv_Simple3DRect(Item->area, True, Hv_gray11);


  if (VRView->hotrect == NULL)
    return;

  Hv_LocalRectToVVRect(Item->view,
		       VRView->hotrect,
		       &tinyhr);

  tinyhr.left   = Hv_sMax((short)(Item->area->left+2),   tinyhr.left);
  tinyhr.top    = Hv_sMax((short)(Item->area->top+2),    tinyhr.top);
  tinyhr.right  = Hv_sMin((short)(Item->area->right-2),  tinyhr.right);
  tinyhr.bottom = Hv_sMin((short)(Item->area->bottom-2), tinyhr.bottom);

  Hv_FixRectWH(&tinyhr);

  if((tinyhr.width > 1) && (tinyhr.height > 1))
    Hv_Simple3DRect(&tinyhr, False, VRView->hotrectborderitem->color);
}

/*------ Hv_DrawViewPortItem -----*/

static void Hv_DrawViewPortItem(Hv_Item Item,
				Hv_Region region)

{
  Hv_Rect     area;
  Hv_Region   tvreprgn = NULL;
  Hv_Region   vrgn = NULL;
  Hv_Region   rrgn = NULL;
  Hv_Region   drgn = NULL;
  Hv_Item     temp;

  Hv_SetViewPortArea(Item);
  Hv_CopyRect(&area, Item->area);


  Hv_FrameRect(&area, Hv_black);
  Hv_ShrinkRect(&area, 1, 1);
  Hv_FrameRect(&area, Hv_lawnGreen);
  Hv_ShrinkRect(&area, 1, 1);
  Hv_FrameRect(&area, Hv_aquaMarine);
  Hv_ShrinkRect(&area, 1, 1);
  Hv_FrameRect(&area, Hv_black); 

/* The view ports region is a little complicated.
   We want the viewport on top, but if a
   view rep is underneath we want to be able to
   drag it without first moving the viewport. We do this
   by a) start with a min internal region
      b) remove all overlap with view reps
      c) intersect remaining region with items area
*/

  Hv_KillRegion(&(Item->region));
  rrgn = Hv_RectRegion(Item->area);

  if (Hv_EmptyRegion(rrgn)) {
    Hv_DestroyRegion(rrgn);
    Hv_KillRegion(&(Item->region));
    return;
  }

  Hv_ShrinkRect(&area, 2, 2);
  drgn = Hv_RectRegion(&area);
  Hv_SubtractRegionFromRegion(&rrgn, drgn);  /* rrgn is now hollow */

  if (!Hv_EmptyRegion(drgn)) {

/* get the total visible view rep region */

    for (temp = Item->view->items->first; temp != NULL; temp = temp->next)
      if (temp->tag == VVVIEWREPTAG)
	if (!Hv_CheckItemDrawControlBit(temp, Hv_DONTDRAW)) {
	  vrgn = Hv_RectRegion(temp->area);
	  Hv_AddRegionToRegion(&tvreprgn, vrgn);
	  Hv_DestroyRegion(vrgn);
	} 
    
    Hv_SubtractRegionFromRegion(&drgn, tvreprgn);
    Item->region = Hv_UnionRegion(rrgn, drgn);
  }

  Hv_DestroyRegion(tvreprgn);
  Hv_DestroyRegion(rrgn);
  Hv_DestroyRegion(drgn);
}

/*----- Hv_SetViewPortArea ------*/

static void Hv_SetViewPortArea(Hv_Item Item)

{
  Hv_View   View;
  Hv_VVData vv;
  Hv_Rect   *hr;
  float     fcw, fch;
  short     w, h;

  if (Item == NULL)
    return;

  View = Item->view;

  vv = Hv_GetVVData(View);
  hr = View->hotrect;

  fcw = (float)(Hv_canvasRect.width);
  fch = (float)(Hv_canvasRect.height);

  w = 8 + (short)((fcw/vv->vw)*hr->width);
  h = 8 + (short)((fch/vv->vh)*hr->height);

  Item->area->width  = Hv_sMax(10, w);
  Item->area->height = Hv_sMax(10, h);

  Hv_FixRectRB(Item->area);
}

/*------- Hv_ViewPortAfterDrag  ---------*/

static void Hv_ViewPortAfterDrag(Hv_Item Item,
				 short dh,
				 short dv)

{
  float     rw, rh;
  Hv_VVData vv;
  Hv_Rect   *hr;
  Hv_View   temp;
  short     vdh, vdv;
  Hv_Region updrgn = NULL;

/* covert dh and dv to their equivalent in
   real local coordinates */


  vv = Hv_GetVVData(Hv_virtualView);
  hr = Hv_virtualView->hotrect;

  rw = ((float)vv->vw)/hr->width;
  rh = ((float)vv->vh)/hr->height;

  vdh  = -(int)(rw*dh);
  vdv  = -(int)(rh*dv);

/* now move all views (except virtual view)*/

  for (temp = Hv_views.last; temp != NULL; temp = temp->prev)
    if (temp != Hv_virtualView)
      Hv_OffsetView(temp, vdh, vdv);

/* now draw all views */

  updrgn = Hv_RectRegion(&Hv_canvasRect);

  if (Hv_ViewIsActive(Hv_virtualView))
    Hv_RemoveRectFromRegion(&updrgn, Hv_virtualView->local);
  
  if (updrgn != NULL) {
    Hv_EraseRegion(updrgn);
    Hv_UpdateViews(updrgn);
    Hv_DestroyRegion(updrgn);
  }

  Hv_DrawControlItems(Hv_virtualView, NULL);
  Hv_UpdateVirtualView();
}

/*------- Hv_ViewRepAfterDrag  ---------*/

static void Hv_ViewRepAfterDrag(Hv_Item Item,
				 short dh,
				 short dv)

{
  float     rw, rh;
  Hv_VVData vv;
  Hv_Rect   *hr;
  Hv_View   View;
  short     vdh, vdv;

/* covert dh and dv to their equivalent in
   real local coordinates */

  vv = Hv_GetVVData(Hv_virtualView);
  hr = Hv_virtualView->hotrect;

  rw = ((float)vv->vw)/hr->width;
  rh = ((float)vv->vh)/hr->height;

  vdh  = (int)(rw*dh);
  vdv  = (int)(rh*dv);

  View = (Hv_View)(Item->data);

/* now move the view */

  Hv_MoveView(View, vdh, vdv);
}

/*-------- Hv_VirtualRectToLocalRect --------*/

static void Hv_LocalRectToVVRect(Hv_View View,
				 Hv_Rect *lrect,
				 Hv_Rect *vvrect)

/* converts a rectangle in actual local rect
   (i.e. in screen coords; the "true" local system)
   into the equivalent, smaller virtual view rect */

{
  short l, t, r, b;


  Hv_GetRectLTRB(lrect, &l, &t, &r, &b);
  Hv_LocalToVV(View, &(vvrect->left),  &(vvrect->top),    l, t);
  Hv_LocalToVV(View, &(vvrect->right), &(vvrect->bottom), r, b);
  Hv_FixRectWH(vvrect);
}

/*-------- Hv_LocalToVV --------*/

static void Hv_LocalToVV(Hv_View View,
			 short *vx,
			 short *vy,
			 short lx,
			 short ly)

/* converts actual local coordinates
   (i.e. screen coords; the "true" local system)
   into the equivalent  virtual view coords */

{
  float       hrw, hrh, rx, ry;
  Hv_Rect     *hr;
  Hv_Rect     *area;
  Hv_VVData   vv;

  hr = View->hotrect;
  vv = Hv_GetVVData(View);

  area = vv->viewport->area;

  hrw = (float)(hr->width);
  hrh = (float)(hr->height);

  rx = hrw/vv->vw;
  ry = hrh/vv->vh;

  *vx = area->left + 4 + (int)(rx*lx);
  *vy = area->top + 4  + (int)(ry*ly);
}

/*------ Hv_GetViewRepView ------*/

static Hv_View Hv_GetViewRepView(Hv_Item Item)

{
  return (Hv_View)(Item->data);
}


/*------ Hv_GetViewRepViewTitle ------*/

static char   *Hv_GetViewRepViewTitle(Hv_Item Item)

/* returns a copy of the title of the view
   represented by the ViewRepIte. The
   copy should be freed */

{
  Hv_View View;
  char   *title;

  View = Hv_GetViewRepView(Item);
  title = Hv_GetViewTitle(View);
  return title;
}

/*-------- Hv_GetViewRepItem ------*/

static Hv_Item Hv_GetViewRepItem(Hv_View View)

{
  
  Hv_Item    temp;
  
  for (temp = Hv_virtualView->items->first; temp != NULL; temp = temp->next)
    if (temp->tag == VVVIEWREPTAG)
      if (temp->data == (void *)View)
	return temp;

  return NULL;
}


/*-------- Hv_TurnViewRepsOff --------*/

static void  Hv_TurnViewRepsOff(Hv_View View)

{
  Hv_Item    temp;

  if (View == NULL)
    return;
  
  for (temp = View->items->first; temp != NULL; temp = temp->next)
    if (temp->tag == VVVIEWREPTAG)
      Hv_SetItemDrawControlBit(temp, Hv_DONTDRAW);
}

/*------- Hv_ViewRepDoubleClick ---------*/

static void    Hv_ViewRepDoubleClick(Hv_Event hvevent)

{
  Hv_Item     vrep = hvevent->item;
  Hv_View     View = hvevent->view;
  Hv_Item     vport;
  short       vpx, vpy, vrx, vry, dh, dv;

  Hv_VVData   vv;

  vv = Hv_GetVVData(View);

  vport = vv->viewport;

  Hv_GetItemCenter(vport, &vpx, &vpy);
  Hv_GetItemCenter(vrep,  &vrx, &vry);

  dh = vrx - vpx;
  dv = vry - vpy;
  Hv_OffsetItem(vport, dh, dv, True);
  vport->afterdrag(vport, dh, dv);
}

/**
 *  Hv_DestroyViewRepItem
 * @purpose   This is a bit of a hack, since the ViewRep item
 * holds the "real" view in the data field we have to set the
 * data field to NULL to prevent the real view from being improperly
 * destroyed.
 * @param   Item   The ViewRepItem being destroyed.
 * @local
 */

static void  Hv_DestroyViewRepItem(Hv_Item Item) {
	Item->data = NULL;
}


#undef VIEWFBTAG
#undef VVVIEWREPTAG

