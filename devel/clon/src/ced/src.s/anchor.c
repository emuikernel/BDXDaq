/*
----------------------------------------------------
-							
-  File:    anchor.c				
-							
-  Summary:						
-            routines for the anchor item
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"
 
/*-------- local prototypes --------*/

static void  DrawAnchorItem(Hv_Item   Item,
			    Hv_Region  region);

static void DeleteAnchorItem(Hv_Event   hvevent);


/* ------- AnchorButtonCallback -------*/

void AnchorButtonCallback(Hv_Event  hvevent)

{
  Hv_View              View = hvevent->view;
  static Hv_View       activeView = NULL;
  static Hv_Item       activeItem = NULL;
  static Cursor        tempcursor;
  Hv_Point             pp;
  short                size, size2;
  ViewData             viewdata;

  size = 11;
  size2 = size/2;

  viewdata = GetViewData(View);  

  if (activeView == NULL) {
    Hv_StandardFirstEntry(hvevent, AnchorButtonCallback, &activeView, &activeItem, &tempcursor);
    return;
  }

  if (View != activeView) {
    Hv_Warning("Anchor selector active on another view!");
    return;
  }

  Hv_ResetActiveView(hvevent, &activeView, &activeItem, tempcursor);

/* see if click occured inside hotrect */

  Hv_SetPoint(&pp, hvevent->where.x, hvevent->where.y); 
  if (Hv_PointInRect(pp, View->hotrect)) {
    if (viewdata->AnchorItem != NULL)
      Hv_DeleteItem(viewdata->AnchorItem);

    viewdata->AnchorItem = Hv_VaCreateItem(View,
					   Hv_DRAWCONTROL, Hv_FEEDBACKWHENDRAGGED,
					   Hv_USERDRAW,    DrawAnchorItem,
					   Hv_BALLOON,     (char *)"Distances reported in the feedback area are measured relative to this \"anchor\".",
					   Hv_LEFT,        pp.x - size2,
					   Hv_TOP,         pp.y - size2,
					   Hv_WIDTH,       size,
					   Hv_HEIGHT,      size,
					   Hv_DOUBLECLICK, DeleteAnchorItem,
					   NULL);

    Hv_DrawItem(viewdata->AnchorItem, NULL);
  }
}

/*------ DrawAnchorButton -------*/

void DrawAnchorButton(Hv_Item    Item,
		      Hv_Region   region)

{
  Hv_Rect          area;
  Hv_Point         pts[20];
  short            x, y;
  short            fill, light, dark;

  dark = Hv_purple;
  light = Hv_aliceBlue;
  fill = Hv_skyBlue;
  
  Hv_StandardToolBackground(Item, &area);

  x = (area.left + area.right)/2;
  y = area.bottom-3;

  Hv_SetPoint(pts,      x - 7, y); 
  Hv_SetPoint(pts + 1,  x - 12, y - 5); 
  Hv_SetPoint(pts + 2,  x - 8, y - 5); 
  Hv_SetPoint(pts + 3,  x - 6, y - 3); 
  Hv_SetPoint(pts + 4,  x - 2, y - 3); 
  Hv_SetPoint(pts + 5,  x - 2, y - 12); 
  Hv_SetPoint(pts + 6,  x - 5, y - 12); 
  Hv_SetPoint(pts + 7,  x - 5, y - 14); 
  Hv_SetPoint(pts + 8,  x - 2, y - 14); 
  Hv_SetPoint(pts + 9,  x - 2, y - 18); 

  Hv_SetPoint(pts + 10, x + 2, y - 18); 
  Hv_SetPoint(pts + 11, x + 2, y - 14); 
  Hv_SetPoint(pts + 12, x + 5, y - 14); 
  Hv_SetPoint(pts + 13, x + 5, y - 12); 
  Hv_SetPoint(pts + 14, x + 2, y - 12); 
  Hv_SetPoint(pts + 15, x + 2, y - 3); 
  Hv_SetPoint(pts + 16, x + 6, y - 3); 
  Hv_SetPoint(pts + 17, x + 8, y - 5); 
  Hv_SetPoint(pts + 18, x + 12, y - 5); 
  Hv_SetPoint(pts + 19, x + 7, y); 

  Hv_FillPolygon(pts, 20, True, fill, dark);

  Hv_DrawLine(pts[1].x, pts[1].y, pts[2].x, pts[2].y, light);
  Hv_DrawLine(pts[2].x, pts[2].y, pts[3].x, pts[3].y, light);
  Hv_DrawLine(pts[3].x, pts[3].y, pts[4].x, pts[4].y, light);
  Hv_DrawLine(pts[4].x, pts[4].y, pts[5].x, pts[5].y, light);
  Hv_DrawLine(pts[6].x, pts[6].y, pts[7].x, pts[7].y, light);
  Hv_DrawLine(pts[7].x, pts[7].y, pts[8].x, pts[8].y, light);
  Hv_DrawLine(pts[8].x, pts[8].y, pts[9].x, pts[9].y, light);
  Hv_DrawLine(pts[9].x, pts[9].y, pts[10].x, pts[10].y, light);
}


/*---------- DrawAnchorItem -------*/

static void  DrawAnchorItem(Hv_Item   Item,
			    Hv_Region  region)

{
  short   xc, yc, tc, bc;
  short   l, t, r, b;

  if (Hv_extraFastPlease) {
    Hv_FrameRect(Item->area, Hv_black);
    return;
  }

  Hv_GetItemCenter(Item, &xc, &yc);

  l = Item->area->left+1;
  t = Item->area->top+1;
  r = Item->area->right;
  b = Item->area->bottom;

  Hv_Simple3DRect(Item->area, False, Hv_highlight);
  bc = Hv_yellow;
  tc = Hv_black;

  Hv_DrawLine(l,     yc+1, xc,    yc+1, bc);
  Hv_DrawLine(l,     yc,   xc,    yc,   tc);
  Hv_DrawLine(xc+2,  yc+1, r,       yc+1, bc);
  Hv_DrawLine(xc+2,  yc,   r,       yc,   tc);

  Hv_DrawLine(xc+1, t,    xc+1, yc,    bc);
  Hv_DrawLine(xc,   t,    xc,   yc,    tc);
  Hv_DrawLine(xc+1, yc+2,    xc+1, b,    bc);
  Hv_DrawLine(xc,   yc+2,    xc,   b,    tc);
}

/*------ DeleteAnchorItem ------*/

static void DeleteAnchorItem(Hv_Event   hvevent)

{
  Hv_Item              Item = hvevent->item;
  ViewData             viewdata;
  
  if (Hv_Question("Delete the anchor?")) {
    viewdata = GetViewData(Item->view);  
    Hv_DeleteItem(Item);
    viewdata->AnchorItem = NULL;
  }
}

