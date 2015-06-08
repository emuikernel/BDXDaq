/**
 * <EM>Various non-gateway mouse utilization routines.</EM>
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


#include <ctype.h>
#include "Hv.h"	    /* contains all necessary motif include files */
#include "Hv_drag.h"
#include "Hv_pointer.h"
#include "Hv_xyplot.h"
#include "Hv_maps.h"

#define PANLEFT   1
#define PANUP     2
#define PANDOWN   3
#define PANRIGHT  4

/* --- local prototypes --- */

static void Hv_DoneMagnify(int opt);

static void Hv_HandleContinuous(Hv_XEvent  *);

static void Hv_LaunchContinuousZoom(Hv_View,
                                    Hv_Item,
                                    Hv_Point *,
                                    Boolean);

static void Hv_HandleMagnify(Hv_XEvent  *);

static void Hv_HandlePointer(Hv_XEvent	*);

static void Hv_HandlePress(Hv_XEvent	*);

static void Hv_HandlePressWhileMagnified(Hv_XEvent	*);

static void Hv_HandleRelease(Hv_XEvent	*);

static void Hv_DragViewCallback(Hv_DragData);

static void Hv_ZoomSelectCallback(Hv_DragData);

static Hv_Item Hv_CheckForButtonGroup(Hv_View  View,
                                      Hv_Point pp);

static void Hv_PanView(Hv_View View,
                       int     direction);


static short Hv_GetSelection(Hv_View   *View,
                             Hv_Item   *Item,
                             short     x,
                             short     y,
                             Boolean   ispress);

static Hv_Item  Hv_WhichItem(Hv_ItemList  items,
                             short     x,
                             short     y,
                             short     val);

static void     Hv_LocalDragView(Hv_View  View,
                                 Hv_Point current,
                                 Boolean  shifted);

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
                            Hv_XEvent   *event,
                            Hv_Event   hvevent);

static void  Hv_GrowRect(Hv_Rect  theGrowRect,
                         Hv_Rect  limitRect,
                         Hv_Point current,
                         Hv_Point *stop,
                         Boolean  showSize);    

static int       HotArea;        /* Area of View selected by button press */
static Hv_DragData    thedd = NULL;

static Hv_Item   olditem = NULL; /* used for tracking in buttongroups */

                                 /**
                                 * Hv_HandleButtonPress
                                 * @purpose Callback routine one one of the mouse buttons is clicked.
                                 * @param    w      The widget that registered the callback
                                 * @param    dummy  Not used
                                 * @param    event  The event data
*/



void Hv_ButtonPress(Hv_Widget     w,
                    Hv_Pointer    dummy,
                    Hv_XEvent    *event) {
  
  /* if alternative handler, pass it to him and bail */
  
  if (Hv_AlternateEventHandler != NULL)
    Hv_AlternateEventHandler(event);
  else 
    Hv_HandlePress(event);
}

/**
* Hv_HandlePress
* @purpose Callback routine one one of the mouse buttons is clicked.
* @param    event  The event data
* @local
*/



void Hv_HandlePress(Hv_XEvent  *event) {
  
  Hv_Item         temp;
  Hv_Point        current;             /*Mouse position at "click"*/
  Hv_Point        stop;              /*Final mouse position after a "drag"*/
  short           dh, dv;              /*Offests*/
  Hv_Rect         limitRect;           /*Limits things like grow sizes*/
  Boolean         done;
  Boolean         shifted;
  Hv_EventData    hvevent, hvevent2;
  short           oldactivecolor;
  Hv_View         tempview;
  short           policy, dl, dt, x, y;
  float           asprat;
  Boolean         SingleClick = False;
  Boolean         DoubleClick = False;
  short           button;
  
  if((thedd != NULL) &&
    (thedd->view->magworld != NULL) &&
    (Hv_AlternateEventHandler != NULL) &&
    (thedd->view->magrect != NULL)) {
    Hv_HandlePressWhileMagnified(event);
    return;
  }
  
  
  Hv_CheckDanglingTextItem();
  
  if ((Hv_activeTextItem != NULL) && (Hv_DanglingTextItemCallback != NULL))
    Hv_DanglingTextItemCallback();
  
  /* some actions do special things if the shift key was held down */
  
  shifted = Hv_Shifted(event);
  
  /* see if button pressed occurred inside an item */
  
  Hv_hotItem = NULL;         /* set the global vars to NULL */
  Hv_hotView = NULL;          /* these used for item selection */
  HotArea = 0;
  
  /* now check where the click occured and return the
  Viewptr, itemptr, and the ID if it is in a special area */
  
  x = Hv_GetXEventX(event);
  y = Hv_GetXEventY(event);
  
  
  HotArea = Hv_GetSelection(&Hv_hotView,
    &Hv_hotItem,
    x,
    y,
    True);
  
  if (Hv_hotView != NULL) {
    current.x = x;
    current.y = y;
    stop = current;
    Hv_FullClip();  /* Always do a full clip on a button press */
    
    Hv_StuffEvent(Hv_hotView,
      Hv_hotItem,
      event,
      &hvevent);  /* convert to simpler HvEvent */
    
    button = Hv_GetXEventButton(event);
    
    /* WIN32 TRICK: to accomodate two button mouse, we intepret CTRL and mb1 as a mb2. Thus
    you can also zoom by holding control and using mb1. This should be bypassed
    if we are not in the hotrect or there is a click intercept */
    
    if ((HotArea == Hv_INHOTRECT) && (Hv_hotView != NULL) &&
      (Hv_hotItem != NULL) && 
      ((Hv_hotItem == Hv_hotView->containeritem) ||
      (Hv_hotItem->type == Hv_PLOTITEM) ||
      (Hv_hotItem->type == Hv_WORLDIMAGEITEM))) {
      if ((button == 1) && (Hv_ClickIntercept == NULL)) {
        if (Hv_ControlPressed(event))
          button = 2;
      }
    }
    
    switch (button)  {  /* check which button was clicked (int) */
      
      /* case 1 is for button 1 (the left button, normally) */
      
    case 1:
      if ((Hv_ClickIntercept != NULL) && (!Hv_ignoreClickIntercept)) {
        Hv_ClickIntercept(&hvevent);
        return;
      }
      
      
      /* look for signal for new (v62) auto zoom */
      
      
      if ((HotArea == Hv_INHOTRECT) && (Hv_hotView != NULL) &&
        (Hv_hotItem != NULL) && 
        ((Hv_hotItem == Hv_hotView->containeritem) ||
        (Hv_hotItem->type == Hv_WORLDIMAGEITEM))) {
        
        if ((Hv_hotItem != NULL) && (Hv_hotItem->type == Hv_WORLDIMAGEITEM))
          if ((Hv_hotItem->singleclick != NULL) && 
            (Hv_hotItem->singleclick != Hv_DefaultSingleClickCallback)) {
            Hv_hotItem->singleclick(&hvevent);
            return;
          }
          
          if (Hv_allowContinuousZoom && !(Hv_ViewIs3D(Hv_hotView)))
            Hv_LaunchContinuousZoom(Hv_hotView, Hv_hotItem, &current, shifted);
          
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
        } /* end DC and DC->callback */
        
        /* for hot rect items, bring'em to the front if they have a callback */
        
        else if (SingleClick && (Hv_hotItem->singleclick != NULL)) {
          if ((Hv_hotItem->domain == Hv_INSIDEHOTRECT) &&
            (Hv_hotItem->singleclick != Hv_DefaultSingleClickCallback)) {
            hvevent2 = hvevent;
            hvevent2.modifiers = 0;
          }
          
          Hv_hotItem->singleclick(&hvevent);
        }      
        
        
        else if(!SingleClick && !DoubleClick) {
          if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_DRAGGABLE) ||
            Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ENLARGEABLE) ||
            Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ROTATABLE))  {
            
            Hv_CopyRect(&limitRect, Hv_hotView->hotrect);
            
            if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_STRICTHRCONFINE)) {
              if (Hv_hotView == Hv_virtualView)
                Hv_ShrinkRect(&limitRect, (short)(-Hv_hotItem->user1), (short)(-Hv_hotItem->user1));
              
              dl = current.x - Hv_hotItem->area->left;
              dt = current.y - Hv_hotItem->area->top;
              Hv_SetRect(&limitRect, (short)(limitRect.left+dl), (short)(limitRect.top+dt),
                (short)(limitRect.width-Hv_hotItem->area->width),
                (short)(limitRect.height-Hv_hotItem->area->height));
            }
            
            if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_CONFINETOPARENT) && (Hv_hotItem->parent != NULL)) {
              Hv_CopyRect(&limitRect, Hv_hotItem->parent->area);
              dl = current.x - Hv_hotItem->area->left;
              dt = current.y - Hv_hotItem->area->top;
              Hv_SetRect(&limitRect, (short)(limitRect.left+dl), (short)(limitRect.top+dt),
                (short)(limitRect.width-Hv_hotItem->area->width),
                (short)(limitRect.height-Hv_hotItem->area->height));
            }
            
            else if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_CONFINETOGRANDPARENT) 
              && (Hv_hotItem->parent != NULL) && (Hv_hotItem->parent->parent != NULL)) {
              Hv_CopyRect(&limitRect, Hv_hotItem->parent->parent->area);
              dl = current.x - Hv_hotItem->area->left;
              dt = current.y - Hv_hotItem->area->top;
              Hv_SetRect(&limitRect,
                (short)(limitRect.left+dl),
                (short)(limitRect.top+dt),
                (short)(limitRect.width-Hv_hotItem->area->width),
                (short)(limitRect.height-Hv_hotItem->area->height));
            }
            
            
            
            Hv_DragItem(Hv_hotItem, &limitRect, current, shifted);
            
          } /* end check bit */
        } /* end !SingleClick */
        
      } /* end of have a hot item */
      
      /*are we in one of the special areas? */
      
      else
        switch (HotArea)  {     /* if it is a special area then: */
        
        
        
        /* for the arrows, we can have "continuous" scrolling */
        
        
  case Hv_INLEFTARROW:
  case Hv_INRIGHTARROW:
  case Hv_INUPARROW:
  case Hv_INDOWNARROW: 
    if (Hv_hotView->enabled) {
      Hv_SendViewToFront(Hv_hotView);
      done = False;
      
      tempview = Hv_hotView;
      oldactivecolor = Hv_scrollActiveColor;
      while (!done)  {
        Hv_scrollActiveColor = Hv_red;
        Hv_HandleScroll(Hv_hotView, HotArea, current);  /* run the scroll handling routine */
        
        if (Hv_CheckMaskEvent(Hv_BUTTONRELEASEMASK | Hv_BUTTONMOTIONMASK,
          event)) {
          
          switch (event->type) {
          case Hv_motionNotify:
            done = (HotArea != Hv_GetSelection(&Hv_hotView,
              &Hv_hotItem,
              Hv_GetXEventX(event),
              Hv_GetXEventY(event),
              True));
            break;
            
          case Hv_buttonRelease:
            done = (Hv_GetXEventButton(event) == 1);
            break;
          }
        }
      }  /*done scrolling */
      Hv_scrollActiveColor = oldactivecolor;
      Hv_DrawScrollArrows(tempview, NULL, (short)(0xFFFF));
    }
    break;
    
  case Hv_INHORIZONTALTHUMB:
  case Hv_INVERTICALTHUMB:
  case Hv_INLEFTSLIDE:
  case Hv_INRIGHTSLIDE:
  case Hv_INUPSLIDE:
  case Hv_INDOWNSLIDE:
    if (Hv_hotView->enabled) {
      Hv_SendViewToFront(Hv_hotView);
      Hv_HandleScroll(Hv_hotView, HotArea, current);  /* run the scroll handling routine */
    }
    break;
    
  case Hv_INGROWBOX:   /* if the special area was a growbox (make view larger or smaller) */
    if (Hv_hotView->enabled) {
      Hv_CopyRect(&limitRect, Hv_hotView->local);
      limitRect.left += Hv_hotView->minwidth;
      limitRect.top  += Hv_hotView->minwidth;
      limitRect.width =  (Hv_canvasRect.right  - limitRect.left) - (Hv_hotView->local->right  - current.x);
      limitRect.height = (Hv_canvasRect.bottom - limitRect.top)  - (Hv_hotView->local->bottom - current.y);
      Hv_FixRectRB(&limitRect);
      
      if ((limitRect.height > 0) && (limitRect.width > 0)) {
        Hv_GrowRect(*(Hv_hotView->local), limitRect, current, &stop, shifted);
        
        /* set the offset values */
        
        dh = stop.x - current.x;
        dv = stop.y - current.y;
        
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
    Hv_LocalDragView(Hv_hotView, current, shifted);
    break;
    
    
  case Hv_INBIGBOX: case Hv_INCLOSEBOX:  /* handled on mouse RELEASE */ 
    break;
    
    
    /* if not in a special area or on any selectable item, drag the view */
    
  default:
    Hv_LocalDragView(Hv_hotView, current, shifted);
    break;
  }
  break;
  
    case 2:   /********** The second button was clicked **********/
      
      /* check to make sure the View is ZOOMABLE and that we are starting in the hot rect */
      
      
      if ((Hv_hotView->enabled) && (HotArea == Hv_INHOTRECT) && 
        (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_ZOOMABLE))) {
        
        
        if (Hv_hotView->tag == Hv_XYPLOTVIEW) {
          Hv_ZoomPlot(Hv_hotView, current, event);
        }
        else {
          Hv_CopyRect(&limitRect, Hv_hotView->hotrect);       /*ONLY ZOOM WITHIN HOTRECT*/
          
          if (!Hv_dontZoom && (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_SQUARE))) {
            policy = Hv_SQUAREPOLICY;
            asprat = ((float)Hv_hotView->hotrect->width / (float)Hv_hotView->hotrect->height);
          }
          else {
            policy = Hv_NOSELECTPOLICY;
            asprat = (float)(1.0);
          }
          
          if (shifted)
            policy = policy | Hv_SHIFTPOLICY;
          
          Hv_LaunchSelectRect(Hv_hotView,
            NULL,
            &limitRect,
            &current,
            policy,
            asprat,
            Hv_ZoomSelectCallback);
          
        } /* end NOT an Hv_XYPLOT */
      }  /* end of zoomable */
      else 
        Hv_SysBeep();
      
      break;
    }
  }  /* end the selected view != NULL */
  else {  /* no View selsected */
    switch (Hv_GetXEventButton(event))  {  /* check which button was clicked (int) */
    case 1: case 2:
      Hv_SysBeep();
      break;
    }
  }
}

/**
* Hv_HandlePressWhileMagnified
* @purpose Callback routine one one of the mouse buttons is clicked and we are magnified.
* @param    event  The event data
* @local
*/

static void Hv_HandlePressWhileMagnified(Hv_XEvent  *event) {
  
  Hv_Point        current;             /*Mouse position at "click"*/
  Hv_Rect         limitRect;           /*Limits things like grow sizes*/
  Boolean         shifted;
  Hv_EventData    hvevent, hvevent2;
  Hv_Item         temp;
  short           x, y;
  Boolean         SingleClick = False;
  Boolean         DoubleClick = False;
  short           button;
  
  Hv_FRect        tempworld;
  Hv_Region       hrreg;
  Hv_Region       mrreg;
  Hv_Region       ireg;
  
  if ((thedd == NULL) || (thedd->view == NULL)) {
    Hv_Println("Something is wrong in Hv_HandlePressWhileMagnified (A)");
    return;
  }
  
  Hv_CheckDanglingTextItem();
  
  if ((Hv_activeTextItem != NULL) && (Hv_DanglingTextItemCallback != NULL))
    Hv_DanglingTextItemCallback();
  
  /* some actions do special things if the shift key was held down */
  
  shifted = Hv_Shifted(event);
  
  /* see if button pressed occurred inside an item */
  
  Hv_hotItem = NULL;         /* set the global vars to NULL */
  Hv_hotView = NULL;          /* these used for item selection */
  HotArea = 0;
  
  /* now check where the click occured and return the
  Viewptr, itemptr, and the ID if it is in a special area */
  
  x = Hv_GetXEventX(event);
  y = Hv_GetXEventY(event);
  
  HotArea = Hv_GetSelection(&Hv_hotView,
    &Hv_hotItem,
    x,
    y,
    True);
  
  
  /* more pathology checks */
  
  if (HotArea != Hv_INHOTRECT) {
    Hv_Println("Something is wrong in Hv_HandlePressWhileMagnified (B)");
    return;
  }
  
  if (Hv_hotView != thedd->view) {
    Hv_Println("Something is wrong in Hv_HandlePressWhileMagnified (C)");
    return;
  }
  
  if (Hv_hotItem == NULL) {
    Hv_Println("Clicked On Nothing");
    return;
  }
  
  Hv_DoneMagnify(1);
  
  current.x = x;
  current.y = y;
  
  if (!Hv_PointInRect(current, thedd->view->magrect)) {
    Hv_Println("Something is wrong in Hv_HandlePressWhileMagnified (D)");
    return;
  }
  
  hrreg = Hv_HotRectRegion(Hv_hotView);
  mrreg = Hv_RectRegion(Hv_hotView->magrect);
  
  ireg = Hv_IntersectRegion(hrreg, mrreg);
  
  Hv_SetClippingRegion(ireg);
  
  Hv_DestroyRegion(hrreg);
  Hv_DestroyRegion(mrreg);
  Hv_DestroyRegion(ireg);
  
  Hv_StuffEvent(Hv_hotView,
    Hv_hotItem,
    event,
    &hvevent);  /* convert to simpler HvEvent */
  
  button = Hv_GetXEventButton(event);
  
  if (button != 1) {
    Hv_Println("No zooming in mag rect.");
    return;
  }
  
  /* if this item has one, pause to see if this is really a double click. Also
  see if button 1 was released -- which would make it a true single click
  rather than a drag initiation */
  
  
  hvevent.clicktype = Hv_ClickType(Hv_hotItem, 1);
  DoubleClick = (hvevent.clicktype == 2);
  SingleClick = (hvevent.clicktype == 1);
  
  /* if its an item with a singleclick or doubleclick  callback, envoke it ! */
  
  if (DoubleClick &&  (Hv_hotItem->doubleclick != NULL)) {
    Hv_LockItem(Hv_hotItem);
    Hv_hotItem->singleclick(&hvevent);
    Hv_FreePixmap(thedd->view->magpixmap);
    thedd->view->magpixmap = 0;
    Hv_hotItem->doubleclick(&hvevent);
    
    
    /* must unlock the item, but first make sure it wasn't deleted
    by the double click routine */
    
    for (temp = Hv_hotView->items->first;  temp != NULL;  temp = temp->next) {
      if (temp == Hv_hotItem) {
        Hv_UnlockItem(Hv_hotItem);
        break;
      }
    }
    return;
  } /* end DC and DC->callback */
  
  /* for hot rect items, bring'em to the front if they have a callback */
  
  else if (SingleClick && (Hv_hotItem->singleclick != NULL)) {
    if ((Hv_hotItem->domain == Hv_INSIDEHOTRECT) &&
      (Hv_hotItem->singleclick != Hv_DefaultSingleClickCallback)) {
      hvevent2 = hvevent;
      hvevent2.modifiers = 0;
    }
    
    Hv_hotItem->singleclick(&hvevent);
    Hv_FreePixmap(thedd->view->magpixmap);
    thedd->view->magpixmap = 0;
    return;
  }      
  
  
  else if(!SingleClick && !DoubleClick) {
    if (Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_DRAGGABLE) ||
      Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ENLARGEABLE) ||
      Hv_CheckBit(Hv_hotItem->drawcontrol, Hv_ROTATABLE))  {
      
      Hv_CopyFRect(&tempworld, thedd->view->world);
      Hv_CopyFRect(thedd->view->world, thedd->view->magworld);
      Hv_CopyRect(&limitRect, thedd->view->magrect);
      Hv_ClipRect(thedd->view->magrect);
      /*				 Hv_RepositionHotRectItems(thedd->view->items);
      
        Hv_DestroyLocalMaps(thedd->view);
      Hv_BuildLocalMaps(thedd->view);*/
      
      Hv_Println("WW (A) %f", thedd->view->world->width);
      Hv_DragItem(Hv_hotItem, &limitRect, current, shifted);
      
      if (thedd != NULL) {
        Hv_FreePixmap(thedd->view->magpixmap);
        thedd->view->magpixmap = 0;
        Hv_CopyFRect(thedd->view->magworld, thedd->view->world);
        Hv_CopyFRect(thedd->view->world, &tempworld);
        Hv_DestroyLocalMaps(thedd->view);
        Hv_BuildLocalMaps(thedd->view);
        Hv_RepositionHotRectItems(thedd->view->items);
      }
      else {
        Hv_Println("Unexpected NULL dd");
      }
    }
  } /* end !SingleClick */
}


/**
* Hv_CheckForButtonGroup
* @purpose  Check if the mouse is in a button group.
* @param  View     The view to check
* @param  pp       The point to check
* @local
*/



static Hv_Item Hv_CheckForButtonGroup(Hv_View View,
                                      Hv_Point pp) {
  
  Hv_Item temp;
  
  for (temp = View->items->first; temp != NULL; temp = temp->next) {
    if (temp->type == Hv_BUTTONGROUPITEM) {
      if (Hv_PointInRect (pp, temp->area))
        return temp;
    }
  }
  
  return NULL;
}

/**
* Hv_PointerMotion
* @purpose Callback routine when the mouse is moved
* @param    w      The widget that registered the callback
* @param    dummy  Not used
* @param    event  The event data
*/


void Hv_PointerMotion(Hv_Widget	 w,
                      Hv_Pointer dummy,
                      Hv_XEvent	*event) {
  
  /* if alternative handler, pass it to him and bail */
  
  if (Hv_AlternateEventHandler != NULL)
    Hv_AlternateEventHandler(event);
  else
    Hv_HandlePointer(event);
  
}


/**
* Hv_HandlePointer
* @purpose Callback routine when the mouse is moved
* @param    event  The event data
* @local
*/


static void Hv_HandlePointer(Hv_XEvent	*event) {
  
  Hv_View            View;
  Hv_Point           pp;
  short              WhatArea;
  Hv_Item            TheItem = NULL;
  char              *GrowBoxBalloonText = "Use this box to change the size of this view.";
  char              *BigBoxBalloonText = "Use this box to change the size of this view to full-screen.";
  char              *CloseBoxBalloonText = "Use this box to close the view.";
  char              *DragRegionBalloonText = "Use this region to move the view.";
  
  Boolean            shifted;
  
  /* some actions do special things if the shift key was held down */
  
  shifted = Hv_Shifted(event);
  
  Hv_SetPoint(&pp,
    Hv_GetXEventX(event),
    Hv_GetXEventY(event));
  
  if (Hv_showBalloons)
    WhatArea = Hv_GetSelection(&View, &TheItem, pp.x, pp.y, False);
  else {
    WhatArea = Hv_GetSelection(&View, NULL, pp.x, pp.y, False);
    
    /* lets see if it is in a button group, since they track */
    
    if (View != NULL) {
      TheItem = Hv_CheckForButtonGroup(View, pp);
    }
  }
  
  /* see if we exited a button group */
  
  if (TheItem != olditem) {
    if ((olditem != NULL) && (olditem->type == Hv_BUTTONGROUPITEM))
      Hv_ButtonGroupExit(olditem);
    olditem = TheItem;
  }
  
  /* see if we are in a button group */
  
  
  if ((TheItem != NULL) && (TheItem->type == Hv_BUTTONGROUPITEM)) {
    Hv_ButtonGroupMouseMotion(TheItem,
      Hv_GetXEventX(event),
      Hv_GetXEventY(event));
  }
  
  
  
  if (!View) {
    Hv_SetCursor(Hv_defaultCursor);
    
    /* if a balloon is up, take it down */
    
    Hv_HideBalloon();
  }
  else
    
    switch (WhatArea) {
    
    /*
    * see if we are moving the cursor through the hotrect,
    * in which case the feedback text needs updating.
    * note that the updating is View specific.
    */
    
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
      
      if (!shifted && (View->feedbackitem != NULL)) {
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

/**
* Hv_HandleButtonRelease
* @purpose Callback routine one one of the mouse buttons is clicked.
* @param    w      The widget that registered the callback
* @param    dummy  Not used
* @param    event  The event data
*/

void Hv_ButtonRelease(Hv_Widget     w,
                      Hv_Pointer    dummy,
                      Hv_XEvent    *event) {
  
  
  /* if alternative handler, pass it to him and bail */
  
  if (Hv_AlternateEventHandler != NULL)
    Hv_AlternateEventHandler(event);
  else
    Hv_HandleRelease(event);
}


/**
* Hv_HandleRelease
* @purpose Callback routine one one of the mouse buttons is released.
* @param    event  The event data
* @local
*/


void Hv_HandleRelease(Hv_XEvent    *event) {
  Boolean  shifted;
  
  /* some actions do special things if the shift key was held down */
  
  shifted = Hv_Shifted(event);
  
  if ((Hv_hotView != NULL)  && (Hv_GetXEventButton(event) == 1))  {       /* check which button was released */
    
    /* see if we are in the help View */
    
    switch (HotArea) {      /* if it is a special area then: */
      
    case Hv_INBIGBOX:
      
      /* Toggle between other View size & location (Other Size Default: Full screen) */
      
      if (Hv_CheckBit(Hv_hotView->drawcontrol, Hv_ENLARGEABLE))
        Hv_FlipView(Hv_hotView);
      break;
      
      
    case Hv_INCLOSEBOX:  /* Close the View Box */
      if (shifted)
        Hv_CloseView(NULL); /*close ALL Views*/
      else
        Hv_CloseView(Hv_hotView);   /* a menus.c procedure, since menu item IDs are used */
      break;
      
    default:      /* if it is a general area in the View Box then */
      if(shifted) {  /* Send to Back if shift-click */
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


/**
* Hv_ContinuousZoom
* @purpose    To "continuously" zoom by dragging in an inert part of the hotrect.
* @param      View     The view to zoom
* @param      current  The starting point
* @param      shifted  If true, this will be passed to Hv_PanView
*/


void     Hv_ContinuousZoom(Hv_View  View,
                           Hv_Point current,
                           Boolean  shifted) {
  
  
  
}


/**
* Hv_PanView
* @purpose    To pan a view left/right by holding the shift and dragging an
* inert part of the hotrect.
* @param      View       The view to pan
* @param      direction  Either PANLEFT or PANRIGHT
* @local
*/


static void Hv_PanView(Hv_View View,
                       int     direction) {
  
  
  Hv_FRect  *world = View->world;
  float     del;
  
  
  if (direction == PANLEFT) {
    del = (float)(0.03*(world->width));
    if (world->xmin > View->defaultworld->xmin) {
      world->xmin -= del;
      world->xmin = Hv_fMax(View->defaultworld->xmin, world->xmin);
      world->xmax = world->xmin + world->width;
    }
    else
      return;
  }
  
  
  else if (direction == PANRIGHT) {
    del = (float)(0.03*(world->width));
    if (world->xmax < View->defaultworld->xmax) {
      world->xmax += del;
      world->xmax = Hv_fMin(View->defaultworld->xmax, world->xmax);
      world->xmin = world->xmax - world->width;
    }
    else
      return;
  }
  
  else if (direction == PANDOWN) {
    del = (float)(0.03*(world->height));
    if (world->ymax < View->defaultworld->ymax) {
      world->ymax += del;
      world->ymax = Hv_fMin(View->defaultworld->ymax, world->ymax);
      world->ymin = world->ymax - world->height;
    }
    else
      return;
  }
  
  else if (direction == PANUP) {
    del = (float)(0.03*(world->height));
    if (world->ymin > View->defaultworld->ymin) {
      world->ymin -= del;
      world->ymin = Hv_fMax(View->defaultworld->ymin, world->ymin);
      world->ymax = world->ymin + world->height;
    }
    else
      return;
  }
  
  Hv_PercentZoom(View, 1.00);
  
}


/**
* Hv_LocalDragView
* @purpose   Initiate the launch of a view drag.
* @param  view      The view in question (being dragged).
* @param  current   Current mouse point.
* @param  shifted   If true, shift was helf down.
*/


static void     Hv_LocalDragView(Hv_View  view,
                                 Hv_Point current,
                                 Boolean  shifted)  {
  
  
  if (view == NULL)
    return;
  
  Hv_SendViewToFront(view);
  Hv_InvertColorScheme(view->containeritem->scheme);
  Hv_DrawViewTitleBorder(view, True, False);
  
  Hv_LaunchDragRect(view,
    NULL,
    &current,
    view->local,
    &Hv_canvasRect,
    Hv_NODRAGPOLICY,
    Hv_DragViewCallback);
  
}

/**
* Hv_GetSelection
* @purpose  Determin what Item we clicked, or what special area on a view.
* @param  View  upon return, the view we are in (or NULL).
* @param  Item  upon return, the item we are in (or NULL).
* @param  x  Mouse x coordinate.
* @param  y  Mouse y coordinate. 
* @param  ispress   If true, coming here as the result of a press (as opposed
* to a mouse motion) 
* @return   short indicatind which special area the point is in.
* @local
*/

static short Hv_GetSelection(Hv_View   *View,
                             Hv_Item   *Item,
                             short     x,
                             short     y,
                             Boolean   ispress) {
  
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
  
  
  /* look for case of a modal view */
  
  if ((Hv_modalView != NULL)  && (*View != NULL) && (Hv_modalView != *View)) {
    *Item = NULL;
    return 0;
  }
  
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
      if (ispress) {
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
  else if (Hv_PointInRectangle(pp, (short)(View->local->left + bw + 1), yo, (short)(xo - View->local->left + bw + 1), h))
    *val = Hv_INLEFTSLIDE;
  else if (Hv_PointInRectangle(pp, (short)(xo + w), yo, (short)(View->local->right - (2*bw) - 3 - xo - w), h))
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
  else if (Hv_PointInRectangle(pp, xo, (short)(View->local->top + (2*bw) + 3), w, (short)(yo - View->local->top - ((2*bw) - 3))))
    *val = Hv_INUPSLIDE;
  else if (Hv_PointInRectangle(pp, xo, (short)(yo+h), w, (short)(View->local->bottom - (2*bw) - 3 - yo-h)))
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
                            Hv_XEvent   *event,
                            Hv_Event   hvevent)
                            
                            
{
  hvevent->view = View;
  hvevent->item = Item;
  hvevent->where.x = Hv_GetXEventX(event);
  hvevent->where.y = Hv_GetXEventY(event);
  hvevent->modifiers = 0;
  hvevent->clicktype = 1;
  hvevent->callbackdata = NULL;
  
  if (Hv_Shifted(event))
    Hv_SetBit(&(hvevent->modifiers), Hv_SHIFT);
  if (Hv_AltPressed(event))
    Hv_SetBit(&(hvevent->modifiers), Hv_MOD1);
  if (Hv_ControlPressed(event))
    Hv_SetBit(&(hvevent->modifiers), Hv_CONTROL);
}


/* ------------ Hv_GrowRect -------------*/

static void  Hv_GrowRect(Hv_Rect  theGrowRect,
                         Hv_Rect  limitRect,
                         Hv_Point current,
                         Hv_Point *stop,
                         Boolean  showSize)    
                         
                         /* selects a rectangle around the screen, staying within limitRect.
                         Uses the  button specified. (REPLACED BY Hv_EnlargeRect. USED ONLY
                         FOR GROWING VIEWS (assumes grab on bottom right))*/
                         
{
  Hv_Rect     tempRect;
  short       dh, dv;
  
  Hv_CopyRect(&tempRect, &theGrowRect);
  Hv_EnlargeRect(&tempRect, limitRect, current, showSize);
  dh = tempRect.right  - theGrowRect.right;
  dv = tempRect.bottom - theGrowRect.bottom;
  stop->x = current.x + dh;
  stop->y = current.y + dv;
}


/**
* Hv_DragViewCallback
* @purpose This is the callback that the view was dragged.
* @param   dd   The drag data pointer.
*/

static void Hv_DragViewCallback(Hv_DragData dd) {
  
  short  dh, dv;
  
  if ((dd == NULL) || (dd->view == NULL)) {
    fprintf(stderr, "Serious problem in Hv_DragViewCallback.");
    return;
  }
  
  
  Hv_InvertColorScheme(dd->view->containeritem->scheme);
  
  /* set the offset values */
  
  dh = dd->finalpp.x - dd->startpp.x;
  dv = dd->finalpp.y - dd->startpp.y;
  
  /* make sure that we moved the view enough to matter.
  If not moved at all, assume it was desired to bring it to the front */
  
  if ((abs(dh) > Hv_MINVIEWMOVE) || (abs(dv) > Hv_MINVIEWMOVE))
    Hv_MoveView(dd->view, dh, dv);
  else
    Hv_DrawViewTitleBorder(dd->view, True, False);
}


/**
* Hv_ZoomSelectCallback
* @purpose  Callback when done rubber-banding for a zoom.
* @param   dd   The drag data.
* @local
*/

static void Hv_ZoomSelectCallback(Hv_DragData dd) {
  
  if ((dd == NULL) || (dd->view == NULL)) {
    fprintf(stderr, "Serious problem in Hv_ZoomSelectCallback.");
    return;
  }
  
  /* if shifted, restore default world */
  
  if(Hv_CheckBit(dd->policy, Hv_SHIFTPOLICY)) { /* Default Zoom Area */
    Hv_RestoreDefaultWorld(dd->view);
    return;
  }
  
  /* routine zoom, if rubber band rect big enough */
  
  if((dd->finalrect.width < Hv_MINVIEWMOVE) ||
    (dd->finalrect.height < Hv_MINVIEWMOVE))
    return;
  
  if (!Hv_dontZoom)
    Hv_ZoomView(dd->view, &(dd->finalrect));
  else
    Hv_Information("Zooming is disabled when a tool is active.");
}

/**
* Hv_LaunchMagnify
* @purpose  Start the magnification process.
* @param  pp   Location when F1 was pressed.
*/

void Hv_LaunchMagnify(Hv_Point   *pp) {
  Hv_View   temp;
  
  for (temp = Hv_views.last; temp != NULL; temp = temp->prev) {
    if (Hv_ViewIsVisible(temp)) {
      if (Hv_CheckBit(temp->drawcontrol, Hv_MAGNIFY)) {
        if (Hv_PointInRect(*pp, temp->hotrect)) {
          Hv_Println("Point in %s", Hv_GetViewTitle(temp));
          
          thedd = Hv_MallocDragData(temp,
            NULL,
            pp,
            NULL,
            NULL,
            0,
            -1,
            NULL);
          
          Hv_Println("CALL Mag (A)");
          Hv_MagnifyView(temp, pp);
          Hv_AlternateEventHandler = Hv_HandleMagnify;
          return;
        }
      }
    }
  }
}


/**
* Hv_HandleMagnify
* @purpose  Alternate event handler for magnification.
* @param   event   Event pointer.
* @local
*/

static void Hv_HandleMagnify(Hv_XEvent  *event) {
  
  Hv_Point  pp;
  
  if (event == NULL)
    return;
  
  if (thedd == NULL) {
    fprintf(stderr, "Serious problem in Hv_HandleMagnify\n");
    Hv_AlternateEventHandler = NULL;
    return;
  }
  
  switch(event->type) {
  case Hv_motionNotify:
    
    Hv_SetPoint(&pp, Hv_GetXEventX(event), Hv_GetXEventY(event));
    
    if (Hv_PointInRect(pp, thedd->view->magrect)) {
      
      if ((thedd->view->tag == Hv_XYPLOTVIEW) || (thedd->view->magworld == NULL)) {
        Hv_Println("DMAG (A)");
        Hv_DoneMagnify(0);
      }
      else {
        thedd->tempworld = thedd->view->world;
        thedd->view->world = thedd->view->magworld;
        Hv_RepositionHotRectItems(thedd->view->items);
        Hv_HandlePointer(event);
        thedd->view->world = thedd->tempworld;
        thedd->tempworld = NULL;
        Hv_RepositionHotRectItems(thedd->view->items);
      }
    }
    
    else {
      Hv_Println("DMAG (B)");
      Hv_DoneMagnify(0);
    }
    
    break;
    
  case Hv_buttonPress:
    thedd->tempworld = thedd->view->world;
    thedd->view->world = thedd->view->magworld;
    Hv_RepositionHotRectItems(thedd->view->items);
    Hv_HandlePress(event);
    Hv_Println("DMAG (C)");
    Hv_DoneMagnify(0);
    break;
  }
  
  
}

/**
* Hv_DoneMagnify
* @purpose  Call when magnification is done.
* @local
*/

static void Hv_DoneMagnify(int opt) {
  
  if (thedd == NULL)
    return;
  
  if (thedd->tempworld != NULL) {
    thedd->view->magworld = thedd->view->world;
    thedd->view->world = thedd->tempworld;
    thedd->tempworld = NULL;
  }
  
  if (opt == 0) {
    thedd->view->magworld = NULL;
    Hv_Println("CALL Mag (B)");
    Hv_MagnifyView(thedd->view, NULL);
    if(Hv_IsMapView(thedd->view)) {
      /*			Hv_DestroyLocalMaps(thedd->view);*/
      Hv_BuildLocalMaps(thedd->view);
    }
    Hv_FreeDragData(thedd);
    thedd = NULL;
    Hv_AlternateEventHandler = NULL;
  }
}


/**
* Hv_HandleContinuousZoom
* @purpose  Alternate event handler for continuous zoom.
* @param   event   Event pointer.
* @local
*/

static void Hv_HandleContinuousZoom(Hv_XEvent  *event) {
  
  static Boolean firstEntry = True;
  Boolean done = False;
  short          dv, dh;
  int            skip = 6;
  static int     count;
  
  if (event == NULL)
    return;
  
  if (thedd == NULL) {
    fprintf(stderr, "Serious problem in Hv_HandleContinuousZoom\n");
    Hv_AlternateEventHandler = NULL;
    return;
  }
  
  if (firstEntry) {
    Hv_SetPoint(&(thedd->finalpp), thedd->startpp.x, thedd->startpp.y);
    count = -1;
    firstEntry = False;
    return;
  }
  
  
  switch(event->type) {
  case Hv_motionNotify:
    
    
    count = (count+1) % skip;
    
    if (count != 0)
      return;
    
    dh = Hv_GetXEventX(event) - thedd->finalpp.x;
    dv = Hv_GetXEventY(event) - thedd->finalpp.y;
    
    Hv_SetPoint(&(thedd->finalpp), Hv_GetXEventX(event), Hv_GetXEventY(event));
    
    if (!Hv_PointInRect(thedd->finalpp, thedd->view->hotrect)) {
      done = True;
      break;
    }
    
    /* not shifted, do zoom, shifted do Pan */
    
    if (!Hv_CheckBit(thedd->policy, Hv_SHIFTPOLICY)) {
      
      if (dv < -1) {
        if (Hv_CheckPercentCanZoom(thedd->view, (float)0.97))
          Hv_PercentZoom(thedd->view, (float)0.97);
        else {
          Hv_SysBeep();
          done = True;
        }
      }
      else if (dv > 1) {
        if (Hv_CheckPercentCanZoom(thedd->view, (float)1.03))
          Hv_PercentZoom(thedd->view, (float)1.03);
        else {
          Hv_SysBeep();
          done = True;
        }
      }
    } /* end ! shifting */
    else { /* Panning */
      if (abs(dv) >= abs(dh)) {
        if (dv < 1)
          Hv_PanView(thedd->view, PANUP);
        else if (dv > 1)
          Hv_PanView(thedd->view, PANDOWN);
      }
      else {
        if (dh < 1)
          Hv_PanView(thedd->view, PANRIGHT);
        else if (dh > 1)
          Hv_PanView(thedd->view, PANLEFT);
      }
      
    } /* end panning */
    
    break;
    
  case Hv_buttonPress: case Hv_buttonRelease:
    done = True;
    break;
  }
  
  if (done) {
    Hv_FreeDragData(thedd);
    thedd = NULL;
    Hv_AlternateEventHandler = NULL;
    firstEntry = True;
  }
  
}

/**
* Hv_LaunchContinuousZoom
* @purpose   Start the continuous zoom (or pan) process.
* @param    view     View that was selected.
* @param    item     Non NULL if item was selected.
* @param    pp       First point.
* @param    shifted  True if shift was selected (pan instead of zoom)
* @local
*/

static void Hv_LaunchContinuousZoom(Hv_View view,
                                    Hv_Item item,
                                    Hv_Point *pp,
                                    Boolean shifted) {
  
  short policy = 0;
  
  if (view == NULL)
    return;
  
  if (shifted)
    policy = Hv_SHIFTPOLICY;
  
  thedd = Hv_MallocDragData(view,
    item,
    pp,
    NULL,
    NULL,
    policy,
    -1,
    NULL);
  
  Hv_AlternateEventHandler = Hv_HandleContinuousZoom;
}

#undef PANLEFT
#undef PANRIGHT
#undef PANUP
#undef PANDOWN




