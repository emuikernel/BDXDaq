/*
----------------------------------------------------
-							
-  File:    sectorhex.c				
-							
-  Summary:						
-           Sector "hexagon"  Drawing
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  2/19/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"

/* ------ local prototypes -------*/

static void      Hv_EditItemColor(Hv_Event hvevent);


#define  ANGDEL  8.0

/*------ MallocSectorHex --------*/

Hv_Item  MallocSectorHex(Hv_View View)

{
  Hv_Item      Item;
  Hv_FPoint    *fpts;
  short        npts = 12;
  Hv_FRect     world;
  float        xc, yc;
  int          i, j;
  ViewData     viewdata;

  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));

  Hv_LocalRectToWorldRect(View, View->hotrect, &world);
  xc = (world.xmin + world.xmax)/2.0;
  yc = (world.ymin + world.ymax)/2.0;

/* set the first point RELATIVE TO THE CENTER */

  fpts[0].h = 0.0;
  fpts[0].v = 0.97*(world.ymax - yc);

  for (i = 1; i < 6; i++) {
    fpts[i].h = fpts[0].h;
    fpts[i].v = fpts[0].v;
  }
    
  Hv_RotateFPoint(Hv_DEGTORAD*ANGDEL, fpts);
  Hv_RotateFPoint(Hv_DEGTORAD*(60.0-ANGDEL),  fpts+1);
  Hv_RotateFPoint(Hv_DEGTORAD*(60.0+ANGDEL),  fpts+2);
  Hv_RotateFPoint(Hv_DEGTORAD*(120.0-ANGDEL), fpts+3);
  Hv_RotateFPoint(Hv_DEGTORAD*(120.0+ANGDEL), fpts+4);
  Hv_RotateFPoint(Hv_DEGTORAD*(180.0-ANGDEL), fpts+5);

  for (i = 6; i < 12; i++) {
    j = 11 - i;
    fpts[i].h = -fpts[j].h;
    fpts[i].v = fpts[j].v;
  }

/* now add the center offset back in */


  for (i = 0; i < 12; i++) {
    fpts[i].h += xc;
    fpts[i].v += yc;
  }

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SECTORHEXITEM,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_COLOR,        Hv_cadetBlue,
			 Hv_FILLCOLOR,    -1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    Hv_FixPolygonRegion,
			 Hv_USERDRAW,     DrawSectorHexItem,
			 Hv_DOUBLECLICK,  Hv_EditItemColor,
			 NULL);


  viewdata->SectorHexItem = Item;
  Hv_SetViewBrickWallItem(Item); 

  return  Item;
}

/*---------- DrawSectorHexItem --------*/

void      DrawSectorHexItem(Hv_Item   Item,
			    Hv_Region region)

{
  Hv_View               View = Item->view;
  Hv_WorldPolygonData  *wpoly;
  Hv_Point             *poly;
  Hv_Region             childrgn;
  Hv_Region             totrgn = NULL;
  Hv_Item               child;

  if (Hv_inPostscriptMode)
    Hv_PSComment("Drawing Sector Hex");

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

  childrgn = Hv_CreateRegion();

  if (Item->children != NULL)
    for (child = Item->children->first; child != NULL; child = child->next)
      Hv_AddRegionToRegion(&childrgn, child->region);

  if (region != NULL) {
    totrgn = Hv_SubtractRegion(region, childrgn);
    Hv_SetClippingRegion(totrgn);
  }

  Hv_FillPolygon(wpoly->poly, wpoly->numpnts, True, Item->color, Hv_black); 

  Hv_DestroyRegion(childrgn);
  Hv_DestroyRegion(totrgn);
  Hv_SetClippingRegion(region);

  poly = wpoly->poly;
  Hv_DrawLines(poly,   4, Hv_white);
  Hv_DrawLines(poly+9, 3, Hv_white);
  Hv_DrawLine(poly[11].x, poly[11].y, poly[0].x,  poly[0].y,  Hv_white);

  if (Hv_inPostscriptMode)
    Hv_PSComment("Done Drawing Sector Hex");

}

/*--- Hv_EditItemColor -----*/

static void Hv_EditItemColor(Hv_Event hvevent)

/* a generic color changer for the Item->color field */

{
  Hv_Item    Item = hvevent->item;
  short      oldcolor;

  oldcolor = Item->color;

  Hv_ModifyColor(NULL, &(Item->color), "Modify Item Color", True);

  if (oldcolor != Item->color)
    Hv_StandardRedoItem(Item);
}

#undef ANGDEL
