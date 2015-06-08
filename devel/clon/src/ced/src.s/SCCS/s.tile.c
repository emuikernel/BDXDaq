h15123
s 00005/00003/00094
d D 1.2 07/11/12 16:41:07 heddle 3 1
c new start counter
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/tile.c
e
s 00097/00000/00000
d D 1.1 02/09/09 16:27:14 boiarino 1 0
c date and time created 02/09/09 16:27:14 by boiarino
e
u
U
f e 0
t
T
I 1
/*
----------------------------------------------------
-							
-  File:    tile.c				
-							
-  Summary:						
-            draws the window tile
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

/*------ local prototypes -------*/


static void CenterWrite(char      *str,
			short     xc,
			short     *y,
			short     font,
			short     lightcolor,
			short     darkcolor);

/*------ DrawTile --------*/

void DrawTile(void)

{
  short      xc, y;
  short      darkcolor;
  short      lightcolor;
  Hv_Rect    rect;
  Hv_Region  region;
  char       text[80];

  Hv_SetRect(&rect, 0, 0, (short)Hv_tileX, (short)Hv_tileY);
  region = Hv_RectRegion(&rect);
  Hv_SetClippingRegion(region);

D 3
  darkcolor = Hv_blue+4;
  lightcolor = Hv_gray10;
E 3
I 3
  /*  darkcolor = Hv_blue+4;
      lightcolor = Hv_gray10; */
  darkcolor = Hv_black;
  lightcolor = Hv_white;
E 3

  Hv_FillRect(&rect, Hv_canvasColor);
  Hv_DrawLine(rect.left,  rect.bottom, rect.left,  rect.top,    lightcolor);
  Hv_DrawLine(rect.left,  rect.top,    rect.right, rect.top,    lightcolor);
  Hv_DrawLine(rect.right-1, rect.top,    rect.right-1, rect.bottom-1, darkcolor);
  Hv_DrawLine(rect.right-1, rect.bottom-1, rect.left,  rect.bottom-1, darkcolor);

  xc = ((short)Hv_tileX)/2;
  y = 30;

  if (Hv_hostName == NULL)
      sprintf(text, "version %-5.3f", cedversion);
  else
      sprintf(text, "version %-5.3f on host %s", cedversion, Hv_hostName);

  CenterWrite("CHRISTOPHER NEWPORT UNIVERSITY", xc, &y, Hv_times14, lightcolor, darkcolor);
  y += 27;
D 3
  CenterWrite("TJNAF CLAS COLLABORATION", xc, &y, Hv_times14, lightcolor, darkcolor);
E 3
I 3
  CenterWrite("Nuclear Physics Group", xc, &y, Hv_times14, lightcolor, darkcolor);
E 3
  y += 27;
  CenterWrite(text, xc, &y, Hv_times14, lightcolor, darkcolor);

  Hv_DestroyRegion(region);

}

/*------- CenterWrite ---------*/

static void CenterWrite(char      *str,
			short     xc,
			short     *y,
			short     font,
			short     lightcolor,
			short     darkcolor)

{
  Hv_String      *hvstr;
  short          w, h;

  hvstr = Hv_CreateString(str);

  hvstr->font = font;
  hvstr->strcol = darkcolor; 

  Hv_CompoundStringDimension(hvstr, &w, &h);
  Hv_DrawGhostedText(xc - w/2, *y, hvstr, lightcolor, darkcolor);

  *y += h;
  Hv_DestroyString(hvstr);
}
E 1
