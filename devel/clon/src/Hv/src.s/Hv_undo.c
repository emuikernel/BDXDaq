/*	
-----------------------------------------------------------------------
File:     Hv_undo.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"
#include "Hv_undo.h"

/* ----- Hv_DoUndo ----- */

void	Hv_DoUndo(Hv_Widget	w,
		  Hv_Pointer	client_data,
		  Hv_Pointer	call_data)

{
  int        action;   /* ID of item selected */
  float      tempazimuth;
  Hv_Item    Item;
  Hv_View    View;
  Hv_Region  oldrgn = NULL;
  Hv_Region  newrgn = NULL;
  Hv_Region  unrgn = NULL;
  short      dh, dv;

  action = (int)client_data;    /* this will be the item selected */

  View = Hv_hotView;

  switch (action) {
    
  case Hv_LASTITEMDRAG:
    Item = View->lastdragitem;
    oldrgn = Hv_CopyItemRegion(Item, True);

    dh = View->lastdragdelta.x;
    dv = View->lastdragdelta.y;

    Hv_offsetDueToDrag = True ;
    Hv_OffsetItem(Item, dh, dv, True);
    Hv_offsetDueToDrag = False;

/* if this is a hotrect object, we have to fix world rect */

    Hv_FixItemWorldArea(Item, True);
    
/* specialized after drag callback? */

    if (Item->afterdrag != NULL)
      Item->afterdrag(Item, dh, dv);

/* make next undo a "redo " */

    Hv_SetPoint(&(View->lastdragdelta), (short)(-dh), (short)(-dv));
    Hv_UpdateConnections(View, Item, True);
    break;
    
  case Hv_LASTITEMROTATE:
    Item = View->lastrotateitem;
    oldrgn = Hv_CopyItemRegion(Item, True);
    
    tempazimuth = View->lastazimuth;
    if (Item->fixregion != NULL)
      Item->fixregion(Item);

    View->lastazimuth = Item->azimuth;
    Item->azimuth = tempazimuth;

    break;
  }

  if (Item == NULL) {
    Hv_DestroyRegion(oldrgn);
    return;
  }

  if (Item->fixregion != NULL)
    Item->fixregion(Item);
  
  newrgn = Hv_CopyItemRegion(Item, True);
  unrgn = Hv_UnionRegion(oldrgn, newrgn);
  Hv_ShrinkRegion(unrgn, -2, -2);

  Hv_DrawView(View, unrgn);
  Hv_DestroyRegion(oldrgn);
  Hv_DestroyRegion(newrgn);
  Hv_DestroyRegion(unrgn);

}

/*-------- Hv_CheckUndoMenu ---------*/

void Hv_CheckUndoMenu(Hv_View View)

{
  Boolean     useundomenu;
  Boolean     dirtydrag;
  Boolean     dirtyrotate;
  Boolean     anythingdirty;

  useundomenu =  Hv_CheckBit(View->popupcontrol, Hv_UNDOVIEW);

  if (!useundomenu)
    return;

  dirtydrag   = (View->lastdragitem != NULL);
  dirtyrotate = (View->lastrotateitem != NULL);

  anythingdirty = dirtydrag | dirtyrotate;

  Hv_SetSensitivity(Hv_popupItems[Hv_FirstSetBit(Hv_UNDOVIEW)], anythingdirty);
  Hv_SetSensitivity(Hv_lastItems[Hv_LASTITEMDRAG], dirtydrag);
  Hv_SetSensitivity(Hv_lastItems[Hv_LASTITEMROTATE], dirtyrotate);

}
