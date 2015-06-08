/*
----------------------------------------------------
-							
-  File:    beamline.c	
-							
-  Summary:						
-           controls beamline drawing
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
----------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"

/*-------- local prototypes --------*/

static void DrawBeamlineItem(Hv_Item   Item,
			     Hv_Region  region);

static void FixBeamlineArea(Hv_Item   Item);

/*--------- MallocBeamlineItem ----------*/

Hv_Item MallocBeamlineItem(Hv_View View)

{
  Hv_Item         Item;

  Item = Hv_VaCreateItem(View,
			 Hv_TAG,         BEAMLINE,
			 Hv_DRAWCONTROL, Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_USERDRAW,    DrawBeamlineItem,
			 Hv_FIXREGION,   FixBeamlineArea,
			 NULL);

  FixBeamlineArea(Item);

  return Item;
}


/*----------- DrawBeamlineItem -----------*/

static void DrawBeamlineItem(Hv_Item   Item,
			     Hv_Region region)

{
  Hv_View         View = Item->view;
  short           xc, yc;
  Hv_Rect        *hr;
  Hv_Point        points[8];
  ViewData        viewdata;

  viewdata = GetViewData(View);

  if (!(viewdata->displayBeamLine))
    return;

  FixBeamlineArea(Item);
  hr = View->hotrect;

  Hv_WorldToLocal(View, 0.0, 0.0, &xc, &yc);

  if ((yc > (hr->top - 10)) && (yc < (hr->bottom + 10))) {
    Hv_DrawLine(hr->left, yc-2, hr->right, yc-2, Hv_black);
    Hv_DrawLine(hr->left, yc-1, hr->right, yc-1, Hv_lavender);
    Hv_DrawLine(hr->left, yc,   hr->right, yc,   Hv_gray11);
    Hv_DrawLine(hr->left, yc+1, hr->right, yc+1, Hv_purple);
    Hv_DrawLine(hr->left, yc+2, hr->right, yc+2, Hv_black);
  }

/* now the nominal target indicator */

    points [0].x = xc-2;        points[0].y = yc-5;
    points [1].x = xc+2;        points[1].y = yc-5;
    points [2].x = xc+5;        points[2].y = yc-2;
    points [3].x = xc+5;        points[3].y = yc+2;
    points [4].x = xc+2;        points[4].y = yc+5;
    points [5].x = xc-2;        points[5].y = yc+5;  
    points [6].x = xc-5;        points[6].y = yc+2;
    points [7].x = xc-5;        points[7].y = yc-2;  
      
    Hv_FillPolygon(points, 8, True, Hv_purple, Hv_black);
    
    Hv_DrawLine(xc, yc+4, xc, yc-4, Hv_white);
    Hv_DrawLine(xc-4, yc, xc+4, yc, Hv_white);

}

/*--------- FixBeamlineArea -----------*/

static void FixBeamlineArea(Hv_Item Item)

{
  Hv_View         View = Item->view;
  short           xc, yc;
  Hv_Rect        *hr;


  hr = View->hotrect;
  Hv_WorldToLocal(View, 0.0, 0.0, &xc, &yc);
  Hv_SetRect(Item->area, hr->left, yc - 6, hr->width, 12);
  Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
}







