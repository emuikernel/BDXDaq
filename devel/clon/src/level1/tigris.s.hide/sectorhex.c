
#ifndef VXWORKS

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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

#define  ANGDEL  8.0

/* ------ local prototypes -------*/

static void DrawSectorHexItem(Hv_Item Item, Hv_Region region);
static void InvertSectorHex(Hv_Item Item, Hv_Region region);

/*------ MallocSectorHex --------*/

Hv_Item
MallocSectorHex(Hv_View View, int num)
{
  Hv_Item      Item ;
  Hv_FPoint    *fpts;
  short        npts = 12;
  Hv_FRect     world;
  Hv_Rect      hr ;
  float        xc, yc, fx, fy ;
  int          i, j;

  ViewData     viewdata;

  Hv_CopyRect (&hr, View->hotrect) ;
  hr.height /= 2 ;

  Hv_FixRectRB (&hr) ;

  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));

  Hv_LocalRectToWorldRect(View, &hr, &world);

  xc = (world.xmin + world.xmax)/2.0;
  yc = (world.ymin + world.ymax)/2.0;

/* set the first point RELATIVE TO THE CENTER */

  fpts[0].h = 0.0;
  fpts[0].v = 0.95*(world.ymax - yc);

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

  switch (num) {
  case 1 : { fx = 0.0 ; fy =  0.0 ; break ; }
  case 3 : { fx = 0.0 ; fy = -1.0 ; break ; }
  }
  for (i = 0; i < 12; i++) {
    fpts[i].h += xc + fx;
    fpts[i].v += yc + fy;
  }

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SECTORHEXITEM,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_COLOR,        Hv_black,
			 Hv_FILLCOLOR,    Hv_lightSeaGreen,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    Hv_FixPolygonRegion,
			 Hv_USERDRAW,     DrawSectorHexItem,
			 NULL);


  Item->doubleclick = NULL;
  Item->singleclick = NULL ;

  viewdata->SectorHexItem = Item;
  return Item;
}

/*---------- DrawSectorHexItem --------*/

static void
DrawSectorHexItem(Hv_Item Item, Hv_Region region)
{
  Hv_WorldPolygonData  *wpoly;
  Hv_Point             *poly ;

  short                 topColor = Hv_white ;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

  poly = wpoly->poly;
  Hv_DrawLines(poly,   4, topColor);
  Hv_DrawLines(poly+9, 3, topColor);
  Hv_DrawLine(poly[11].x, poly[11].y, poly[0].x,  poly[0].y,  topColor);

  Hv_SetLineWidth (3) ;
  Hv_DrawLine (
    (poly[1].x+poly[2].x)/2, 
    (poly[1].y+poly[2].y)/2, 
    (poly[8].x+poly[7].x)/2, 
    (poly[8].y+poly[7].y)/2, Hv_white) ;
  Hv_DrawLine (
    (poly[3].x+poly[4].x)/2, 
    (poly[3].y+poly[4].y)/2, 
    (poly[10].x+poly[9].x)/2, 
    (poly[10].y+poly[9].y)/2, Hv_white) ;
  Hv_DrawLine (
    (poly[5].x+poly[6].x)/2, 
    (poly[5].y+poly[6].y)/2, 
    (poly[0].x+poly[11].x)/2, 
    (poly[0].y+poly[11].y)/2, Hv_white) ;

  Hv_SetLineWidth (0) ;


}

/* -------------- InvertSectorHex -------------- */

static void
InvertSectorHex(Hv_Item Item, Hv_Region region)
{
  Hv_WorldPolygonData  *wpoly;
  Hv_Point             *poly;
  short                 topColor = Hv_white, botColor=Hv_black ;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

  poly = wpoly->poly;
  Hv_DrawLines(poly,   4, botColor);
  Hv_DrawLines(poly+9, 3, botColor);
  Hv_DrawLine(poly[11].x, poly[11].y, poly[0].x,  poly[0].y,  botColor);

  Hv_DrawLines(poly+3, 7, topColor);

}
#undef ANGDEL

#else /* UNIX only */

void
sectorhex_dummy()
{
}

#endif
