/*	
-----------------------------------------------------------------------
File:     Hv_pointer.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include <ctype.h>
#include "Hv.h"	    /* contains all necessary motif include files */
#include "Hv_xyplot.h"

/* --- local prototypes --- */

static short Hv_GetSelection(Hv_View   *View,
			     Hv_Item   *Item,
			     short     x,
			     short     y);

static Hv_Item  Hv_WhichItem(Hv_ItemList  items,
			     short     x,
			     short     y,
			     short     val);

static void     Hv_LocalDragView(Hv_View  View,
				 Hv_Point StartPP,
				 Boolean  Shifted);

static Boolean Hv_InGrowBox(Hv_Item   Item,
			    Hv_Point   pp);

static Boolean Hv_InCloseBox(Hv_Item  Item,
			     Hv_Point  pp);

static Boolean Hv_InEnlargeBox(Hv_Item  Item,
			       Hv_Point  pp);

static Boolean Hv_InLeftArrow(Hv_Item  Item,
			      Hv_Point  pp);

static Boolean Hv_InRightArrow(Hv_Item   Item,
			       Hv_Point   pp);

static Boolean Hv_InUpArrow(Hv_Item   Item,
			    Hv_Point   pp);

static Boolean Hv_InDownArrow(Hv_Item  Item,
			      Hv_Point pp);

static void Hv_InHorizBar(Hv_View   View,
			  Hv_Point   pp,
			  short    *val);

static void Hv_InVertBar(Hv_View    View,
			 Hv_Point    pp,
			 short     *val);

static Boolean Hv_InDragRegion(Hv_Item  Item,
			       Hv_Point  pp);

static void   Hv_StuffEvent(Hv_View   View,
			    Hv_Item   Item,
			    XEvent   *event,
			    Hv_Event   hvevent);

static void  Hv_GrowRect(Hv_Rect  TheGrowRect,
			 Hv_Rect  LimitRect,
			 Hv_Point StartPP,
			 Hv_Point *StopPP,
			 Boolean  ShowSize);    

static int       HotArea;        /* Area of View selected by button press */

/*-------- Hv_ButtonPress --------- */

void Hv_ButtonPress(Widget    w,
		    XtPointer client_data,
		    XEvent    *event)

{
  Hv_Item         temp;
  Hv_Point        StartPP;             /*Mouse position at "click"*/
  Hv_Point        StopPP;              /*Final mouse position after a "drag"*/
  short           dh, dv;              /*Offests*/
  Hv_Rect         LimitRect;           /*Limits things like grow sizes*/
  Hv_Rect         TheSelectedRect;     /*Selected rect for zooming*/
  Boolean         done;
  Boolean         Shifted;
  Hv_EventData    hvevent, hvevent2;
  short           oldactivecolor;
  Hv_View         tempview;
  short           dl, dt;

  Boolean         SingleClick = False;
  Boolean         DoubleClick = False;

  short           button;

  Hv_CheckDanglingTextItem();


  if ((Hv_activeTextItem != NULL) && (Hv_DanglingTextItemCallback != NULL))
    Hv_DanglingTextItemCallback();
  
/* some actions do special things if the shift key was held down */

  Shifted = ((event->xbutton.state & ShiftMask) == ShiftMask);

/* see if button pressed occurred inside an item */

  Hv_hotItem = NULL;         /* set the global vars to NULL */
  Hv_hotView = NULL;          /* these used for item selection */
  HotArea = 0;

/* now check where the click occured and return the
   Viewptr, itemptr, and the ID if it is in a special area */

  HotArea = Hv_GetSelection(&Hv_hotView,
			    &Hv_hotItem,
			    event->xbutton.x,
			    event->xbutton.y);

  if (Hv_hotView != NULL) {
    StartPP.x = event->xbutton.x;
    StartPP.y = event->xbutton.y;
    StopPP = StartPP;
    Hv_FullClip();                /* Always do a full clip on a button press */

    Hv_StuffEvent(Hv_hotView, Hv_hotItem, event, &hvevent);  /* convert to simpler HvEvent */

    button = event->xbutton.button;

/* TRICK: to accomodate two button mouse, we intepret CTRL and mb1 as a mb2. Thus
   you can also zoom by holding control and using mb1. This should be bypassed
   if we are not in the hotrect or ther is a click intercept */

/* 6/7/96: apply trick to linux only */

#ifdef __linux__
    if ((button == 1) && (HotArea == Hv_INHOTRECT) &&
	Hv_CheckBit(hvevent.modifiers, Hv_CONTROL) &&
	(Hv_ClickIntercept == NULL))

      button = 2;
#endif
      
    switch (button)  {  /* check which button was clicked (int) */

/* case 1 is for button 1 (the left button, normally) */

    case 1:
      if (Hv_ClickIntercept != NULL) {
	Hv_ClickIntercept(&hvevent);
	return;
      }

/* if this item has one, pause to see if this is really a double click. Also
   see if button 1 was released -- which would make it a true single click
   rather than a drag initiation */

      if (Hv_hotItem != NULL) {
	hvevent.clicktype = Hv_ClickType(Hv_hotItem, 1);
	DoubleClick = (hvevent.clicktype == 2);
	SingleClick = (hvevent.clicktype == 1);
      }

/* if its an item with a singleclick or doubleclick  callback, envoke it ! */

      if (Hv_hotView->enabled && Hv_hotItem) {
	  if (DoubleClick &&  (Hv_hotItem->doubleclick != NULL)) {
	      Hv_LockItem(Hv_hotItem);
	      Hv_hotItem->doubleclick(&hvevent);

/* must unlock the item, but first make sure it wasn't deleted
   by the double click routine */

	      for (temp = Hv_hotView->items->first;  temp != NULL;  temp = temp->next) {
		  if (temp == Hv_hotItem) {
		      Hv_UnlockItem(Hv_hotItem);
		      break;
		  }
	      }
	  }

/* for hot rect items, bring'em to the front if they have a callback */
      
	else if (SingleClick && (Hv_hotItem->singleclick != NULL)) {
	  if ((Hv_hotItem->domain == Hv_INSIDEHOTRECT) &&
	      (Hv_hotItem->singleclick != Hv_DefaultSingleClickCallback)) {
	    hvevent2 = hvevent;
	    hvevent2.modifiers = 0;
	  }
	  Hv_hotItem->singleclick(&hvevent);
	}      


	else if(!SingleClick) {
	  if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_DRAGGABLE) ||
	      Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ENLARGEABLE) ||
	      Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ROTATABLE))  {
	    
	    Hv_CopyRect(&LimitRect, Hv_hotView->hotrect);

	    if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_STRICTHRCONFINE)) {
	      if (Hv_hotView == Hv_virtualView)
		Hv_ShrinkRect(&LimitRect, -Hv_hotItem->user1, -Hv_hotItem->user1);

	      dl = StartPP.x - Hv_hotItem->area->left;
	      dt = StartPP.y - Hv_hotItem->area->top;
	      Hv_SetRect(&LimitRect, LimitRect.left+dl, LimitRect.top+dt,
			 LimitRect.width-Hv_hotItem->area->width,
			 LimitRect.height-Hv_hotItem->area->height);
	    }
	    
	    if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_CONFINETOPARENT) && (Hv_hotItem->parent != NULL)) {
	      Hv_CopyRect(&LimitRect, Hv_hotItem->parent->area);
	      dl = StartPP.x - Hv_hotItem->area->left;
	      dt = StartPP.y - Hv_hotItem->area->top;
	      Hv_SetRect(&LimitRect, LimitRect.left+dl, LimitRect.top+dt,
			 LimitRect.width-Hv_hotItem->area->width,
			 LimitRect.height-Hv_hotItem->area->height);
	    }
	    
	    else if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_CONFINETOGRANDPARENT) 
		     && (Hv_hotItem->parent != NULL) && (Hv_hotItem->parent->parent != NULL)) {
	      Hv_CopyRect(&LimitRect, Hv_hotItem->parent->parent->area);
	      dl = StartPP.x - Hv_hotItem->area->left;
	      dt = StartPP.y - Hv_hotItem->area->top;
	      Hv_SetRect(&LimitRect, LimitRect.left+dl, LimitRect.top+dt,
			 LimitRect.width-Hv_hotItem->area->width, LimitRect.height-Hv_hotItem->area->height);
	    }
	    
	    Hv_DragItem(Hv_hotItem, &LimitRect, StartPP, Shifted);
	  }
	}
	
      }
      
/*are we in one of the special areas? */

      else
	switch (HotArea)  {     /* if it is a special area then: */

/* for the arrows, we can have "continuous" scrolling */


	case Hv_INLEFTARROW:  case Hv_INRIGHTARROW: case Hv_INUPARROW:  case Hv_INDOWNARROW: 
	  if (Hv_hotView->enabled) {
	    Hv_SendViewToFront(Hv_hotView);
	    done = False;
	    
	    tempview = Hv_hotView;
	    oldactivecolor = Hv_scrollActiveColor;
	    while (!done)  {
	      Hv_scrollActiveColor = Hv_red;
	      Hv_HandleScroll(Hv_hotView, HotArea, StartPP);  /* run the scroll handling routine */
	      if (XCheckMaskEvent(Hv_display, ButtonReleaseMask | ButtonMotionMask, event)) {
		switch (event->type) {
		case MotionNotify:
		  done = (HotArea != Hv_GetSelection(&Hv_hotView,
						     &Hv_hotItem,
						     event->xbutton.x,
						     event->xbutton.y));
		  break;
		  
		case ButtonRelease:
		  done = (event->xbutton.button == 1);
		  break;
		}
	      }
	    }  /*done scrolling */
	    Hv_scrollActiveColor = oldactivecolor;
	    Hv_DrawScrollArrows(tempview, NULL, 0xFFFF);
	  }
	  break;
	  
	case Hv_INHORIZONTALTHUMB: case Hv_INVERTICALTHUMB: case Hv_INLEFTSLIDE:
	case Hv_INRIGHTSLIDE: case Hv_INUPSLIDE: case Hv_INDOWNSLIDE:
          if (Hv_hotView->enabled) {
	    Hv_SendViewToFront(Hv_hotView);
	    Hv_HandleScroll(Hv_hotView, HotArea, StartPP);  /* run the scroll handling routine */
          }
	  break;
	  
	case Hv_INGROWBOX:   /* if the special area was a growbox (make view larger or smaller) */
          if (Hv_hotView->enabled) {
	    Hv_CopyRect(&LimitRect, Hv_hotView->local);
	    LimitRect.left += Hv_hotView->minwidth;
	    LimitRect.top  += Hv_hotView->minwidth;
	    LimitRect.width =  (Hv_canvasRect.right  - LimitRect.left) - (Hv_hotView->local->right  - StartPP.x);
	    LimitRect.height = (Hv_canvasRect.bottom - LimitRect.top)  - (Hv_hotView->local->bottom - StartPP.y);
	    Hv_FixRectRB(&LimitRect);

	    if ((LimitRect.height > 0) && (LimitRect.width > 0)) {
	      Hv_GrowRect(*(Hv_hotView->local), LimitRect, StartPP, &StopPP, Shifted);

/* set the offset values */
	      
	      dh = StopPP.x - StartPP.x;
	      dv = StopPP.y - StartPP.y;
	    
	      if (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_ENLARGEABLE)) {
			
/* If not resized at all, assume it was desired to bring it to the front */

	        if ((dh == 0) && (dv == 0)) 
		  Hv_SendViewToFront(Hv_hotView);
	        else
		  Hv_GrowView(Hv_hotView, dh, dv);
              }
	    }
	  }

	  break;
	      
	case Hv_INDRAGREGION: /* if the special area was a dragregion (move the view) */
	  Hv_LocalDragView(Hv_hotView, StartPP, Shifted);
	  break;


	case Hv_INBIGBOX: case Hv_INCLOSEBOX:  /* handled on mouse RELEASE */ 
	  break;

/* if not in a special area or on any selectable item, drag the view */

	default:
	  Hv_LocalDragView(Hv_hotView, StartPP, Shifted);
	  break;
	}
      break;
      
    case 2:   /********** The second button was clicked **********/
	  
/* check to make sure the View is ZOOMABLE and that we are starting in the hot rect */


      if ((Hv_hotView->enabled) && (HotArea == Hv_INHOTRECT) && 
           (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_ZOOMABLE))) {


	if (Hv_hotView->tag == Hv_XYPLOTVIEW) {
	  Hv_ZoomPlot(Hv_hotView, StartPP, event);
	}
	else {
	  Hv_CopyRect(&LimitRect, Hv_hotView->hotrect);       /*ONLY ZOOM WITHIN HOTRECT*/
	  
	  if (!Hv_dontZoom && (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_SQUARE)))
	    Hv_SelectRect(&TheSelectedRect, LimitRect, StartPP, event->xbutton.button, Hv_SQUAREPOLICY,
			  ((float)Hv_hotView->hotrect->width/ (float)Hv_hotView->hotrect->height));
	  else
	    Hv_SelectRect(&TheSelectedRect, LimitRect, StartPP, event->xbutton.button, Hv_NOSELECTPOLICY, 0.0);
	  
	  if((event->xbutton.state & ShiftMask) == ShiftMask) /* Default Zoom Area */
	    Hv_RestoreDefaultWorld(Hv_hotView);

/* routine zoom*/

	  else  if((TheSelectedRect.width > Hv_MINVIEWMOVE) && (TheSelectedRect.height > Hv_MINVIEWMOVE)) {
	    if (!Hv_dontZoom)
	      Hv_ZoomView(Hv_hotView, &TheSelectedRect);      /* zoom the box */
	    else
	      Hv_Information("Zooming is disabled when a tool is active.");
	  } /* end of routine zoom */
	} /* end NOT an Hv_XYPLOT */

      }  /* end of zoomable */
      else 
	Hv_SysBeep();

      break;
      
    }
  }  /* end the selected lo != NULL */
  else  /* no View selsected */
    switch (event->xbutton.button)  {  /* check which button was clicked (int) */
      case 1: case 2:
        Hv_SysBeep();
	break;
    }
}

/*-------- Hv_PointerMotion --------- */

void Hv_PointerMotion(Widget	w,
		      XtPointer client_data,
		      XEvent	*event)

{
  Hv_View            View;
  Hv_Point           pp;
  short              WhatArea;
  Hv_Item            TheItem = NULL;
  char              *GrowBoxBalloonText = "Use this box to change the size of this view.";
  char              *BigBoxBalloonText = "Use this box to change the size of this view to full-screen.";
  char              *CloseBoxBalloonText = "Use this box to close the view.";
  char              *DragRegionBalloonText = "Use this region to move the view.";

  Boolean            Shifted;

/* some actions do special things if the shift key was held down */

  Shifted = ((event->xbutton.state & ShiftMask) == ShiftMask);

  Hv_SetPoint(&pp, event->xmotion.x, event->xmotion.y);

  if (Hv_showBalloons)
    WhatArea = Hv_GetSelection(&View, &TheItem, pp.x, pp.y);
  else
    WhatArea = Hv_GetSelection(&View, NULL, pp.x, pp.y);

  if (!View) {
    Hv_SetCursor(Hv_defaultCursor);

/* if a balloon is up, take it down */
    
    Hv_HideBalloon();
  }
  else

    switch (WhatArea) {
       
/*see if we are moving the cursor through the hotrect, in which case the feedback text needs updating.
  note that the updating is View specific for each View */

    case Hv_INHOTRECT:
      if (TheItem) {
	if (Hv_showBalloons)
	  Hv_PrepareBalloon(View, TheItem, -1, TheItem->balloon);
      }
      else
	Hv_HideBalloon();

      Hv_SetCursor(Hv_crossCursor);

/* don't do feedback if shift is pressed. this gives the user a chance
   to "lock" the feedback */

      if (!Shifted && (View->feedbackitem != NULL)) {
	
	if (View->feedback)
	  View->feedback(View, pp);
	Hv_FullClip();
      }
      break;

    case Hv_INCLOSEBOX:
      Hv_SetCursor(Hv_defaultCursor);
      if (Hv_showBalloons) 
	Hv_PrepareBalloon(View, NULL, Hv_INCLOSEBOX, CloseBoxBalloonText);
      break;
	
    case Hv_INDRAGREGION:
      if (Hv_showBalloons)
	Hv_PrepareBalloon(View, NULL, Hv_INDRAGREGION, DragRegionBalloonText);
      Hv_SetCursor(Hv_dragCursor);
      break;
      
    case Hv_INBIGBOX:
      Hv_SetCursor(Hv_defaultCursor);
      if (Hv_showBalloons)
	Hv_PrepareBalloon(View, NULL, Hv_INBIGBOX, BigBoxBalloonText);
      break;
      
      
    case Hv_INGROWBOX:
      if (Hv_showBalloons)
	Hv_PrepareBalloon(View, NULL, Hv_INGROWBOX, GrowBoxBalloonText);
      Hv_SetCursor(Hv_bottomRightCursor);
      break;
      
    default:
      Hv_SetCursor(Hv_defaultCursor);
      
/* examine the possibility of showing a balloon */
      
      if (Hv_showBalloons) {
	if (TheItem)   /* on an item */
	  Hv_PrepareBalloon(View, TheItem, -1, TheItem->balloon);
	else  /* not on ANY item, in which case if balloon is up it should be removed */
	  Hv_HideBalloon();
	
      }
      break;
    }
  
}

/*-------- Hv_ButtonRelease --------- */

void Hv_ButtonRelease(Widget    w,
		      XtPointer client_data,
		      XEvent    *event)

{
  Boolean  Shifted;

/* some actions do special things if the shift key was held down */

  Shifted = ((event->xbutton.state & ShiftMask) == ShiftMask);

   if ((Hv_hotView != NULL)  && (event->xbutton.button == 1))  {       /* check which button was released */

/* see if we are in the help View */

     switch (HotArea) {      /* if it is a special area then: */

     case Hv_INBIGBOX:

/* Toggle between other View size & location (Other Size Default: Full screen) */

       if (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_ENLARGEABLE))
	 Hv_FlipView(Hv_hotView);
       break;

		  
     case Hv_INCLOSEBOX:  /* Close the View Box */
       if (Shifted)
	 Hv_CloseView(NULL); /*close ALL Views*/
       else
	 Hv_CloseView(Hv_hotView);   /* a menus.c procedure, since menu item IDs are used */
       break;
       
     default:      /* if it is a general area in the View Box then */
       if(Shifted) {  /* Send to Back if shift-click */
	 if (!Hv_dontSendBack)
	   Hv_SendViewToBack(Hv_hotView); 
	 else
	   Hv_dontSendBack = False;
       }
       else  /* Bring to Front if just click */
	 Hv_SendViewToFront(Hv_hotView);
       break;
     }
   }
}


/*-------- Hv_LocalDragView ----------*/

static void     Hv_LocalDragView(Hv_View  View,
				 Hv_Point StartPP,
				 Boolean  Shifted)

{
  short     dh, dv;
  Hv_Point  StopPP;

  if (View == NULL)
    return;

  Hv_SendViewToFront(View);
  Hv_InvertColorScheme(View->containeritem->scheme);
  Hv_DrawViewTitleBorder(View, True, False);
  Hv_DragRect(View->local, &Hv_canvasRect, StartPP, &StopPP, Hv_NODRAGPOLICY, Shifted, False);
  Hv_InvertColorScheme(View->containeritem->scheme);
  
/* set the offset values */
	      
  dh = StopPP.x - StartPP.x;
  dv = StopPP.y - StartPP.y;
  
/* make sure that we moved the view enough to matter.
   If not moved at all, assume it was desired to bring it to the front */

  if ((abs(dh) > Hv_MINVIEWMOVE) || (abs(dv) > Hv_MINVIEWMOVE))
    Hv_MoveView(View, dh, dv);
  else
    Hv_DrawViewTitleBorder(View, True, False);
  
}

/* -------- Hv_GetSelection -------- */

static short Hv_GetSelection(Hv_View   *View,
			     Hv_Item   *Item,
			     short     x,
			     short     y)

{
/* this routine takes the x and y coordinates of the
   mouse position and determines what View,
   what item and if it is in a special area of the
   container item, returning this information
   to the calling routine */

  Hv_Point      pp;              /* Point corresponding to mouse selection*/
  short         val;             /* returns 0 normally */
  Hv_Rect       *rect;

  Hv_SetPoint(&pp, x, y);

  *View = NULL;

  if (Item != NULL)
    *Item = NULL; 
  val = 0;

  Hv_WhichView(View, pp);

/* now that we found what view it is in, lets
   determine the item. First check whether it is
   in the grow box, then check the drag region, finally
   check if it is in an item  */

  if (*View != NULL) {

    rect = (*View)->hotrect;
      
    if (Hv_PointInRect (pp, rect))
      val = Hv_INHOTRECT;
    
    else if (Hv_InGrowBox((*View)->containeritem, pp) &&
	     (Hv_CheckBit((*View)->drawcontrol, Hv_ENLARGEABLE)))
      val = Hv_INGROWBOX;

    else if (Hv_InCloseBox((*View)->containeritem, pp))
      val = Hv_INCLOSEBOX;

    else if (Hv_InEnlargeBox((*View)->containeritem, pp) &&
	     (Hv_CheckBit((*View)->drawcontrol, Hv_ENLARGEABLE)))
      val = Hv_INBIGBOX;

    else if (Hv_InDragRegion((*View)->containeritem, pp))
      val = Hv_INDRAGREGION;

/* check and see if we are scrolling and if so
   which scroll button is pressed */

    else {
      if (Hv_CheckBit((*View)->drawcontrol, Hv_SCROLLABLE)) {
	if ((*View)->hscroll->active) {
	  if (Hv_InLeftArrow((*View)->containeritem, pp))
	    val = Hv_INLEFTARROW;
	  else if (Hv_InRightArrow((*View)->containeritem, pp))
	    val = Hv_INRIGHTARROW;
	  else 
	    Hv_InHorizBar((*View), pp, &val);
	}
	
	if ((*View)->vscroll->active) {
	  if (Hv_InUpArrow((*View)->containeritem, pp))
	    val = Hv_INUPARROW;
	  else if (Hv_InDownArrow((*View)->containeritem, pp))
	    val = Hv_INDOWNARROW;
	  else 
	    Hv_InVertBar((*View), pp, &val);
	}
      }
    }

/* if none of the special areas except
   hotrect apply return the item we are in */

    if (((val == 0) || (val == Hv_INHOTRECT)) && (Item != NULL))
      *Item = Hv_WhichItem((*View)->items, x, y, val);

/* however, dont return the container -- this will allow me
   to drag via any inert part of the view */

    if ((Item != NULL) && (*Item != NULL) && 
	(val != Hv_INHOTRECT) &&
	(*Item == ((*View)->containeritem)))
      *Item = NULL;

  }

  return val;
}



/*---------- Hv_WhichItem --------------- */

static Hv_Item  Hv_WhichItem(Hv_ItemList  items,
			     short     x,
			     short     y,
			     short     val)

{
  Hv_Item       theItem;        /* returned */
  Hv_Item       temp;           /* for searching */
  Hv_Point      pp;
  Boolean       inItem;
  Hv_Item       citem;
  Hv_ConnectionDataPtr  data;

  Hv_SetPoint(&pp, x, y);
  theItem = NULL;

  temp = items->last;

  while ((theItem == NULL) && (temp != NULL)) {
    
    if (!Hv_CheckItemDrawControlBit(temp, Hv_DONTDRAW) &&
	(((val == Hv_INHOTRECT) && (temp->domain == Hv_INSIDEHOTRECT)) ||
	((val != Hv_INSIDEHOTRECT) && (temp->domain == Hv_OUTSIDEHOTRECT)))) {

/* first check children, which may be on top */

      if (temp->children != NULL)
	theItem = Hv_WhichItem(temp->children, x, y, val);
      
      if (theItem == NULL) {

/* check the region if there is on, otherwise check the area */

	if ((temp->region == NULL) ||
	    Hv_CheckBit(temp->drawcontrol, Hv_CHECKAREAONLY))
	  inItem = Hv_PointInRect(pp, temp->area);
	else
	  inItem = Hv_PointInRegion(pp, temp->region);
	  
	if ((inItem ||
	     (Hv_CheckBit(temp->drawcontrol, Hv_ROTATABLE) &&
	      Hv_PointInRect(pp, temp->rotaterect)))
	    && Hv_ItemIsEnabled(temp))
	  theItem = temp;


/* if in a connection item, see if we should
   really be in one of the connectors */

	if ((theItem != NULL) && (theItem->type == Hv_CONNECTIONITEM)) {
	  data = (Hv_ConnectionDataPtr)(theItem->data);
	  citem = data->item1;
	  if (Hv_PointInRect(pp, citem->area) && (Hv_ItemIsEnabled(citem)))
	    theItem = citem;
	  else {
	    citem = data->item2;
	    if (Hv_PointInRect(pp, citem->area) && (Hv_ItemIsEnabled(citem)))
	      theItem = citem;
	  }
	}
	
	
      }
      
    }  /* end of if for items' domain */
    temp = temp->prev;
  } /* end while */

  return theItem;
}


/* -------- Hv_InGrowBox -----------*/

static Boolean Hv_InGrowBox(Hv_Item   Item,
			    Hv_Point   pp)

{
/* tests to see if the pixel point is in the grow box of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->right  - 1 - bw;
   yo = Item->area->bottom - 1 - bw;

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InCloseBox -----------*/

static Boolean  Hv_InCloseBox(Hv_Item  Item,
			      Hv_Point  pp)
{
/* tests to see if the pixel point is in the close box of the container item */

   short    xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->left;
   yo = Item->area->top;

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InEnlargeBox -----------*/

static Boolean  Hv_InEnlargeBox(Hv_Item  Item,
				Hv_Point  pp)

{  
/* tests to see if the pixel point is in the enlarge box of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->right - bw;
   yo = Item->area->top;

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InLeftArrow -----------*/

static Boolean  Hv_InLeftArrow(Hv_Item  Item,
			       Hv_Point  pp)

{
/* tests to see if the pixel point is in the left arrow box of the container item */

   short     xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->left;
   yo = Item->area->bottom - bw;

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InRightArrow -----------*/

static Boolean  Hv_InRightArrow(Hv_Item  Item,
				Hv_Point  pp)
{
/* tests to see if the pixel point is in the right arrow box of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->right  - (bw * 2);
   yo = Item->area->bottom - bw;

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InUpArrow -----------*/

static Boolean  Hv_InUpArrow(Hv_Item  Item,
			     Hv_Point  pp)

{
/* tests to see if the pixel point is in the up arrow box of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->right  - bw;
   yo = Item->area->top    + bw;

   return  Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InDownArrow -----------*/

static Boolean  Hv_InDownArrow(Hv_Item  Item,
			       Hv_Point  pp)

{
/* tests to see if the pixel point is in the down arrow box of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->right  - bw;
   yo = Item->area->bottom - (bw * 2);

   return Hv_PointInRectangle(pp, xo, yo, bw, bw);
}

/* -------- Hv_InHorizBar -----------*/

static void Hv_InHorizBar(Hv_View   View,
			  Hv_Point   pp,
			  short    *val)


{
  /* tests to see if the pixel point is in the horiz bar box of the container item */
  /* returning vals representing specific areas of the bar */

  Hv_Item       Item;
  Boolean       INArea = False;
  short         xo, yo;
  short         w, h, dummy, bw;

  if ((View == NULL) || (View->containeritem == NULL))
    return;
 
  Item = View->containeritem;
  bw = Item->scheme->borderwidth;
	  
  Hv_HorizThumbInfo (View, &xo, &w, &dummy);
  yo = View->local->bottom - bw;
  h = bw- 1;
  
  INArea = Hv_PointInRectangle(pp, xo, yo, w, h);
  
  if (INArea)
    *val = Hv_INHORIZONTALTHUMB;
  else if (Hv_PointInRectangle(pp, View->local->left + bw + 1, yo, xo - View->local->left + bw + 1, h))
    *val = Hv_INLEFTSLIDE;
  else if (Hv_PointInRectangle(pp, xo + w, yo, View->local->right - (2*bw) - 3 - xo - w, h))
    *val = Hv_INRIGHTSLIDE;
}

/* -------- Hv_InVertBar -----------*/

static void Hv_InVertBar(Hv_View    View,
			 Hv_Point    pp,
			 short     *val)


{
  /* tests to see if the pixel point is in the vert bar box of the container item */
  /* returning vals representing specific areas of the bar */

  Hv_Item       Item;
  Boolean       INArea = False;
  short         xo, yo;
  short         w, h, dummy, bw;

  if ((View == NULL) || (View->containeritem == NULL))
    return;
  
  Item = View->containeritem;
  bw = Item->scheme->borderwidth;
  
  Hv_VertThumbInfo (View, &yo, &h, &dummy);
  xo = View->local->right - bw;
  w = bw- 1;
      
  INArea = Hv_PointInRectangle(pp, xo, yo, w, h);
  
  if (INArea)
    *val = Hv_INVERTICALTHUMB;
  else if (Hv_PointInRectangle(pp, xo, View->local->top + (2*bw) + 3, w, yo - View->local->top - ((2*bw) - 3)))
    *val = Hv_INUPSLIDE;
  else if (Hv_PointInRectangle(pp, xo, yo+h, w, View->local->bottom - (2*bw) - 3 - yo-h))
    *val = Hv_INDOWNSLIDE;
}

/* -------- Hv_InDragRegion -----------*/

static Boolean Hv_InDragRegion(Hv_Item  Item,
			       Hv_Point  pp)

{
/* determines whether the pixel point is the the draw region of the container item */

   short          xo, yo;
   short          bw;

   bw = Item->scheme->borderwidth;

   xo = Item->area->left;
   yo = Item->area->top;

   return Hv_PointInRectangle(pp, xo, yo, Item->area->width, bw);
}


/*------ Hv_StuffEvent-------*/

static void   Hv_StuffEvent(Hv_View   View,
			    Hv_Item   Item,
			    XEvent   *event,
			    Hv_Event   hvevent)


{
  hvevent->view = View;
  hvevent->item = Item;
  hvevent->where.x = (short)(event->xbutton.x);
  hvevent->where.y = (short)(event->xbutton.y);
  hvevent->modifiers = 0;
  hvevent->clicktype = 1;

  if ((event->xbutton.state & ShiftMask)   == ShiftMask)
    Hv_SetBit(&(hvevent->modifiers), Hv_SHIFT);
  if ((event->xbutton.state & Mod1Mask)    == Mod1Mask)
    Hv_SetBit(&(hvevent->modifiers), Hv_MOD1);
  if ((event->xbutton.state & ControlMask) == ControlMask)
    Hv_SetBit(&(hvevent->modifiers), Hv_CONTROL);
}


/* ------------ Hv_GrowRect -------------*/

static void  Hv_GrowRect(Hv_Rect  TheGrowRect,
			 Hv_Rect  LimitRect,
			 Hv_Point StartPP,
			 Hv_Point *StopPP,
			 Boolean  ShowSize)    

/* selects a rectangle around the screen, staying within LimitRect.
   Uses the  button specified. (REPLACED BY Hv_EnlargeRect. USED ONLY
   FOR GROWING VIEWS (assumes grab on bottom right))*/

{
  Hv_Rect     TempRect;
  short       dh, dv;

  Hv_CopyRect(&TempRect, &TheGrowRect);
  Hv_EnlargeRect(&TempRect, LimitRect, StartPP, ShowSize);
  dh = TempRect.right  - TheGrowRect.right;
  dv = TempRect.bottom - TheGrowRect.bottom;
  StopPP->x = StartPP.x + dh;
  StopPP->y = StartPP.y + dv;
}
