/*
-----------------------------------------------------------------------
File:     Hv_callback.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"	/* contains all necessary include files */


/* ------- Hv_CamaraCallback -------*/

void Hv_CameraCallback(Hv_Event hvevent)

{
  Hv_Item   Item = hvevent->item;
  Hv_View   View = hvevent->view;

  Hv_ToggleButton(Item);
  Hv_SendViewToFront(View);

  Hv_psWholeScreen = False;
  Hv_PrinterSetup(View);
  Hv_ToggleButton(Item);
}


/*------ Hv_DefaultSingleClickCallback ----*/

void   Hv_DefaultSingleClickCallback(Hv_Event hvevent)

/* NO SHIFT: puts the object at the end of the list and redraws it
   so that it appears to be on top

   SHIFT: puts object (near) top of list and redraws so that
   if appears to be at bottom */

{
  Hv_Item       Item = hvevent->item;
  Hv_View       View = hvevent->view;
  Hv_Region     minregion;
  Boolean       shifted;
  shifted = Hv_CheckBit(hvevent->modifiers, Hv_SHIFT);

  if (Item == View->brickwallitem)
    return;

  if (shifted) {
    minregion = Hv_GetMinimumItemRegion(Item);
    Hv_SendItemToBack(Item);
  }
  else {
    Hv_SendItemToFront(Item);
    minregion = Hv_GetMinimumItemRegion(Item);
  }

  Hv_DrawView(View, minregion);
  Hv_DestroyRegion(minregion);
}

/*-------- Hv_StandardFirstEntry --------*/

void Hv_StandardFirstEntry(Hv_Event hvevent,
			   Hv_FunctionPtr callback,
			   Hv_View *activeview,
			   Hv_Item *activeitem,
			   Hv_Cursor *tempcursor)

{
  Hv_Item              Item = hvevent->item;
  Hv_View              View = hvevent->view;
  

  View->modifiers = hvevent->modifiers;
  Hv_dontZoom = True;       /*disable zooming*/

  Hv_ToggleItemState(Item);

  *tempcursor = Hv_crossCursor;
  Hv_crossCursor = Hv_dotboxCursor;
  Hv_SendViewToFront(View);
  Hv_ClickIntercept = callback;
  *activeview = View;
  *activeitem = Item;
}


/* ------ Hv_ResetActiveView  -------*/

void Hv_ResetActiveView(Hv_Event hvevent,
			Hv_View *activeView,
			Hv_Item *activeItem,
			Hv_Cursor tempcursor)

{


/* if just one modifier bit set, just return. This allows us to keep
   selecting */

  if ((hvevent != NULL) && (Hv_CountBits(hvevent->modifiers) == 1))
    return;
  
  Hv_dontZoom = False;       /*enable zooming*/
  Hv_ToggleItemState(*activeItem);
  Hv_crossCursor = tempcursor;

  (*activeView)->modifiers = 0;
  *activeView = NULL;
  *activeItem = NULL;
  Hv_ClickIntercept = NULL;
}









