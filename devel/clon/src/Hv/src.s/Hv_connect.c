/*
-----------------------------------------------------------------------
File:     Hv_connect.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with connecting items */

#include "Hv.h"	  /* contains all necessary include files */

/*---- local prototypes ------*/

static void    Hv_DrawConnection(Hv_Item,
				 Hv_Region);

static void    Hv_ResetConnection(Hv_ConnectionNodePtr);

static void    Hv_FixConnectionRegion(Hv_Item);

 
/*-------- Hv_ToggleConnection ----*/

void   Hv_ToggleConnection(Hv_Item Item)

{
  Hv_ConnectionDataPtr data;

  data = (Hv_ConnectionDataPtr)(Item->data);
  data->active = !(data->active);
  
  Item->fixregion(Item);
}

/*-------- Hv_DisableConnection ----*/

void   Hv_DisableConnection(Hv_Item Item)

{
  Hv_ConnectionDataPtr data;

  data = (Hv_ConnectionDataPtr)(Item->data);
  data->active = False;
  
  Item->fixregion(Item);
}

/*-------- Hv_EnableConnection ------*/

void   Hv_EnableConnection(Hv_Item Item)

{
  Hv_ConnectionDataPtr data;

  data = (Hv_ConnectionDataPtr)(Item->data);
  data->active = True;
  
  Item->fixregion(Item);
}


/*-------- Hv_MultiplexConnection ------*/

Hv_Item   Hv_MultiplexConnection(Hv_Item Item1,
				 Hv_Item Item2,
				 short color,
				 short style,
				 short thickness,
				 short multiplex,
				 short arrowcolor,
				 short arrowstyle,
				 short user1,
				 Boolean active,
				 Hv_CheckProc CP)


{
  Hv_Item               CItem;
  Hv_ConnectionDataPtr  data;

  if ((multiplex < 1) || (multiplex > 4))
    return NULL;

/* check to see if the link already exists.. that is the whole
  purpose of multiplexing */
  
  if ((CItem = Hv_DuplicateConnection(Item1, Item2, user1)) == NULL) {

    CItem = Hv_AddConnection(Item1, Item2, color, style, thickness,
			     arrowcolor, arrowstyle, user1, active, CP, False);
  
  }
  
  data = (Hv_ConnectionDataPtr)(CItem->data);
  
  data->multiplexed = True;
  data->multiplexcolors[multiplex] = arrowcolor;
  
  Hv_SetBitPosition(&(data->multiplexbits), multiplex);
  
/*  Hv_DrawItem(CItem, NULL); */

  return CItem;
}

/*-------- Hv_AddConnection ------*/

Hv_Item    Hv_AddConnection(Hv_Item       Item1,
			    Hv_Item       Item2,
			    short         color,
			    short         style,
			    short         thickness,
			    short         arrowcolor,
			    short         arrowstyle,
			    short         user1,
			    Boolean       active,
			    Hv_CheckProc  CP,
			    Boolean       draw)

{
  Hv_View               View = Item1->view;
  Hv_ConnectionNodePtr  temp;
  Hv_ConnectionDataPtr  data;
  float                 xstart, ystart, xstop, ystop;
  short                 x1, x2, y1, y2;

  if ((Item1 == NULL) || (Item2 == NULL)) {
    Hv_Println("WARNING: attempt to make connection with a NULL item.");
    return NULL;
  }

  if (Item1 == Item2) {
    Hv_Println("WARNING: attempt to make connect an item with itself.");
    return NULL;
  }

/* only hotrect items on the same view can be connected */

  if ((Item1->view) != (Item2->view)) {
    Hv_Println("WARNING: attempt to make connection between items on different views.");
    return NULL;
  }

  if ((Item1->domain != Hv_INSIDEHOTRECT) || (Item2->domain != Hv_INSIDEHOTRECT)) {
    Hv_Println("WARNING: attempt to make connection between non hotrect items.");
    return NULL;
  }


/* create a new entry into the View's connection list */
    
  if (View->connections == NULL) {
    View->connections = (Hv_ConnectionNodePtr)Hv_Malloc(sizeof(Hv_ConnectionNode));
    temp = View->connections;
  }
  
  else {
    for (temp = View->connections; temp->next != NULL; temp = temp->next)
      ;
    temp->next = (Hv_ConnectionNodePtr)Hv_Malloc(sizeof(Hv_ConnectionNode));
    temp = temp->next;
  }
  
  temp->next = NULL;

/* now set up the connection item */

  data = (Hv_ConnectionDataPtr)Hv_Malloc(sizeof(Hv_ConnectionData));
  data->style = style;
  data->thickness = thickness;
  data->multiplexbits = 0;
  data->multiplexed = False;
  data->arrowstyle = arrowstyle;
  data->checkproc = CP;
  data->item1 = Item1;
  data->item2 = Item2;
  data->active = active;
  data->arrowcolor = arrowcolor;
  
  Hv_GetItemCenter(Item1, &x1, &y1);
  Hv_GetItemCenter(Item2, &x2, &y2);
  Hv_LocalToWorld(View, &xstart, &ystart, x1,  y1);
  Hv_LocalToWorld(View, &xstop,  &ystop,  x2,  y2);

/* connections will share some world line routines */

  temp->item = Hv_VaCreateItem(View,
			       Hv_TYPE,         Hv_CONNECTIONITEM,
			       Hv_DATA,         (void *)data,
			       Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			       Hv_FIXREGION,    Hv_FixConnectionRegion,
			       Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_NOPOINTERCHECK,
			       Hv_STARTX,       xstart,
			       Hv_STOPX,        xstop,
			       Hv_STARTY,       ystart,
			       Hv_STOPY,        ystop,
			       Hv_COLOR,        color,
			       Hv_USER1,        user1,
			       NULL);

  if (draw)
    Hv_DrawItem(temp->item, NULL);
  return temp->item;
}

/*------- Hv_FixConnectionRegion -------*/

static void Hv_FixConnectionRegion(Hv_Item Item)

{
  Hv_ConnectionDataPtr data;
/*
  Hv_FPoint            fp1, fp2;

  data = (Hv_ConnectionDataPtr)(Item->data);

  Hv_GetFLineCorner(wline->start, Item->worldarea, &fp1);
  Hv_GetFLineCorner(wline->stop,  Item->worldarea, &fp2);
  Hv_WorldPoint2ToLocalRect(View, &fp1, &fp2, Item->area, 0);
*/
  data = (Hv_ConnectionDataPtr)(Item->data);

  if (data->active)
    Hv_FixLineRegion(Item);
  else {
    Hv_DestroyRegion(Item->region);
    Item->region = NULL;
  }
}

/*---- Hv_DuplicateConnection ------*/

Hv_Item Hv_DuplicateConnection(Hv_Item Item1,
			       Hv_Item Item2,
			       short   user1)

/* note: only considered duplicate if 1 = 1 and 2 = 2.

   AND same user1

   not considered duplicate if 1 = 2 and 2 = 1 */

{
  Hv_View                 View = Item1->view;
  Hv_ConnectionNodePtr    temp;
  Hv_ConnectionDataPtr    data;
  
  for (temp = View->connections; temp != NULL; temp = temp->next) {
    data = (Hv_ConnectionDataPtr)(temp->item->data);
    if ((data->item1 == Item1) && (data->item2 == Item2) && (temp->item->user1 == user1))
      return temp->item;
  }

  return NULL;
}

/*---------- Hv_ChangeConnectionStyles  -------*/

void   Hv_ChangeConnectionStyles(Hv_View View,
				 Hv_Item Item,
				 short   user1,
				 short   color, 
				 short   thickness,
				 short   style,
				 short   arrowstyle,
				 short   arrowcolor)

/* applies to connection containing Item or all if item = NULL.
   send -1 to anything (color, thick, etc that you DONT want changed */

{
  Hv_ConnectionNodePtr  temp;
  Hv_ConnectionDataPtr  data;

  for (temp = View->connections; temp != NULL; temp = temp->next) {
    data = (Hv_ConnectionDataPtr)(temp->item->data);
    if ((Item == NULL) || (data->item1 == Item) || (data->item2 == Item)) {
      if ((user1 < 0) || (temp->item->user1 == user1)) {
        if (color >= 0)
          temp->item->color = color;
        if (thickness >= 0)
          data->thickness = thickness;
        if (style >= 0)
          data->style = style;
        if (arrowstyle >= 0)
          data->arrowstyle = arrowstyle;
        if (arrowcolor >= 0)
          data->arrowcolor = arrowcolor;
      }
    }
  }
}


/*------ Hv_UpdateConnections  ---------*/

void    Hv_UpdateConnections(Hv_View View,
			     Hv_Item Item,
			     Boolean Redraw)


/* update connection containing a specific item, or
   ALL the view's connections if Item = NULL */

{
  Hv_ConnectionNodePtr  temp;
  Hv_ConnectionDataPtr  data;
  Hv_Region             totreg = NULL;
  Hv_Item               CItem;

  for (temp = View->connections; temp != NULL; temp = temp->next) {
    CItem = temp->item;
    data = (Hv_ConnectionDataPtr)(CItem->data);
    if ((Item == NULL) || (Item == data->item1) || (Item == data->item2)) {
      if (Redraw && View->drawconnections && data->active)
	Hv_AddRegionToRegion(&totreg, CItem->region);
      Hv_ResetConnection(temp);
      if (Redraw && View->drawconnections && data->active)
	Hv_AddRegionToRegion(&totreg, CItem->region);
    }
  }
  
  if (View->drawconnections && Redraw && (totreg != NULL))
    Hv_DrawView(View, totreg);
  Hv_DestroyRegion(totreg);

}

/*------ Hv_ConnectionRegion -----*/

Hv_Region   Hv_ConnectionRegion(Hv_Item Item)

/* returns the total of all regions of connections with this item */

{
  Hv_ConnectionNodePtr  temp;
  Hv_ConnectionDataPtr  data;
  Hv_Region             totreg = NULL;
  Hv_Item               CItem;
  Hv_View               View;

  View = Item->view;

  if ((Item == NULL) ||  !(View->drawconnections))
    return NULL;


  for (temp = View->connections; temp != NULL; temp = temp->next) {
    CItem = temp->item;
    data = (Hv_ConnectionDataPtr)(CItem->data);
    if ((data->active) && ((Item == data->item1) || (Item == data->item2)))
      Hv_AddRegionToRegion(&totreg, CItem->region);
  }

  return totreg;
}


/*------ Hv_ResetConnection -------*/

static void Hv_ResetConnection(Hv_ConnectionNodePtr connect)

{
  float                 xstart, ystart, xstop, ystop;
  short                 x1, x2, y1, y2;
  Hv_Item               Item;
  Hv_View               View;
  Hv_ConnectionDataPtr  data;

  Item = connect->item;
  View = Item->view;
  data = (Hv_ConnectionDataPtr)(Item->data);

  Hv_GetItemCenter(data->item1, &x1, &y1);
  Hv_GetItemCenter(data->item2, &x2, &y2);
  Hv_LocalToWorld(View, &xstart, &ystart, x1,  y1);
  Hv_LocalToWorld(View, &xstop,  &ystop,  x2,  y2);
  Hv_SetWorldLineBasedOnEndPoints(Item, xstart, ystart, xstop, ystop);
}


/*------ Hv_RemoveViewConnections  ---------*/

void    Hv_RemoveViewConnections(Hv_View View,
				 Boolean Redraw,
				 short   user1)
     
/* remove ALL connections on the view if user1 < 0, else
  just those that match user1  */ 

{
  Hv_ConnectionNodePtr   temp, prev, next;
  Hv_Region              totregion = NULL;
  Hv_Region              hrr = NULL;
  Hv_Region              clipreg = NULL;

  prev = NULL;
  for (temp = View->connections; temp != NULL; temp = next) {
    next = temp->next;

    if ((user1 < 0) || (temp->item->user1 == user1)) {
    
/* prev will be NULL for the first connection */

      if (prev != NULL)
	prev->next = next;
      else /* deleting the first */
	View->connections = next;
      

/* first delete the actual connection item */

      Hv_AddRegionToRegion(&totregion, temp->item->region);
      Hv_DestroyItem(temp->item);
      temp->item = NULL;

/* now delete the connection node */

      Hv_Free(temp);

    }  /* end of "match" */
    else
      prev = temp;
    
  }  /* end of for (temp) */  

  if (totregion != NULL) {
    hrr = Hv_HotRectRegion(View);
    clipreg = Hv_IntersectRegion(hrr, totregion);
    Hv_DrawView(View, clipreg);
    Hv_DestroyRegion(totregion);
    Hv_DestroyRegion(clipreg);
    Hv_DestroyRegion(hrr);
  }

}

/*------ Hv_RemoveConnections  ---------*/

void    Hv_RemoveConnections(Hv_Item Item,
			     short   user1,
			     Boolean Redraw1,
			     Boolean Redraw2,
			     Boolean Massive)
     
/* remove any connections on the view containing the 
   given item if user1 < 0, else only those with matching user1*/

{
  Hv_View                View = Item->view;
  Hv_ConnectionNodePtr   temp, prev, next;
  Hv_ConnectionDataPtr   data;

  Hv_Region              totregion = NULL;
  Hv_Region              hrr = NULL;
  Hv_Region              clipreg = NULL;


  prev = NULL;
  for (temp = View->connections; temp != NULL; temp = next) {
    next = temp->next;
    data = (Hv_ConnectionDataPtr)(temp->item->data);

    if (((data->item1 == Item) || (data->item2 == Item)) && 
        ((user1 < 0) || (temp->item->user1 == user1))) {
    
/* prev will be NULL for the first connection */

      if (prev != NULL)
	prev->next = next;
      else /* deleting the first */
	View->connections = next;
      
/* first delete the actual connection item */


      if (!Massive){
	Hv_AddRegionToRegion(&totregion, temp->item->region);
	if (Redraw1)
	  Hv_AddRegionToRegion(&totregion, data->item1->region);
	if (Redraw2)
	  Hv_AddRegionToRegion(&totregion, data->item2->region);
      }
      

      Hv_DestroyItem(temp->item);
      temp->item = NULL;

/* now delete the connection node */

      Hv_Free(temp);

    }  /* end of "match" */
    else
      prev = temp;
    
  }  /* end of for (temp) */  

  if (!Massive)
    if (totregion != NULL) {
      hrr = Hv_HotRectRegion(View);
      clipreg = Hv_IntersectRegion(hrr, totregion);
      Hv_DrawView(View, clipreg);
      Hv_DestroyRegion(totregion);
      Hv_DestroyRegion(clipreg);
      Hv_DestroyRegion(hrr);
    }

  if (Massive)
    Hv_DrawViewHotRect(View);

}

/*------ Hv_RemoveConnection  ---------*/

void    Hv_RemoveConnection(Hv_Item Item1,
			    Hv_Item Item2,
			    short   user1,
			    Boolean Redraw)
     
/* remove any connections on the view containing the 
   two items if user1 < 0, else only those with matching user1*/

{
  Hv_View                View = Item1->view;
  Hv_ConnectionNodePtr   temp, prev, next;
  Hv_ConnectionDataPtr   data;

  Hv_Region              totregion = NULL;
  Hv_Region              hrr = NULL;
  Hv_Region              clipreg = NULL;

  prev = NULL;
  for (temp = View->connections; temp != NULL; temp = next) {
    next = temp->next;
    data = (Hv_ConnectionDataPtr)(temp->item->data);

    if (((data->item1 == Item1) && (data->item2 == Item2)) ||
	((data->item1 == Item2) && (data->item2 == Item1))) {

      if ((user1 < 0) || (temp->item->user1 == user1)) {
    
/* prev will be NULL for the first connection */

	if (prev != NULL)
	  prev->next = next;
	else /* deleting the first */
	  View->connections = next;


	if (Redraw){
	  Hv_AddRegionToRegion(&totregion, temp->item->region);
	  Hv_AddRegionToRegion(&totregion, data->item1->region);
	  Hv_AddRegionToRegion(&totregion, data->item2->region);
	}
      
      
/* first delete the actual connection item */

	Hv_DestroyItem(temp->item);
	temp->item = NULL;

/* now delete the connection node */

	Hv_Free(temp);
      }
    }  /* end of "match" */
    else
      prev = temp;
    
  }  /* end of for (temp) */  
  

  if (Redraw && (totregion != NULL)) {
    hrr = Hv_HotRectRegion(View);
    clipreg = Hv_IntersectRegion(hrr, totregion);
    Hv_DrawView(View, clipreg);
    Hv_DestroyRegion(totregion);
    Hv_DestroyRegion(clipreg);
    Hv_DestroyRegion(hrr);
  }
}

/*------ Hv_DrawConnection  ---------*/

static void    Hv_DrawConnection(Hv_Item Item,
				 Hv_Region region)

{
  Hv_View              View = Item->view;
  Hv_ConnectionDataPtr data;
  Hv_Item              item1, item2;
  
  if (!(View->drawconnections))
    return;
  
  data = (Hv_ConnectionDataPtr)(Item->data);

  item1 = data->item1;
  item2 = data->item2;

/* do not draw if not connected to two items
   or either item has do not draw me bit set */

  if ((item1 == NULL) || (item2 == NULL))
    return;

  if (Hv_CheckItemDrawControlBit(item1, Hv_DONTDRAW) ||
      Hv_CheckItemDrawControlBit(item2, Hv_DONTDRAW))
    return;

  if (!(data->active))
    return;

  if (data->checkproc != NULL)
    if (!(data->checkproc(Item)))
      return;

/* if it is to be drawn, draw it like a world line */

  Hv_DrawWorldLineItem(Item, region);
}

/*------- Hv_ConnectionInitialize ------*/

void  Hv_ConnectionInitialize(Hv_Item           Item,
			      Hv_AttributeArray attributes)

{
  Item->standarddraw = Hv_DrawConnection;
  Item->data = (void *)(attributes[Hv_DATA].v);
  Hv_WorldLineInitialize(Item, attributes);
}






