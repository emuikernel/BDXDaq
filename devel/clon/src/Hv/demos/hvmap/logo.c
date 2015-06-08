/*
----------------------------------------------------
-							
-  File:    logo.c				
-							
-  Summary:						
-            draws a logo item on the Welcome View
-            and handles the tiling
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  08/24/95	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/


#include "Hv.h"
#include "maps.h"

static void CenterWrite();

/*------- CenterWrite ------*/

static void CenterWrite(xc, y, str, font, color)

short   xc;
short   *y;
char    *str;
short   font, color;

{
  Hv_String     *hvstr;
  short         w, h;

  hvstr = Hv_CreateString(str);
  hvstr->strcol = color;
  hvstr->font = font;
  Hv_CompoundStringDimension(hvstr, &w, &h);

  Hv_DrawCompoundString(xc - (w/2), *y, hvstr);
  *y += h;
  Hv_DestroyString(hvstr);
}


/*--------- DrawLogoItem --------*/

void DrawLogoItem(Item,  region)

Hv_Item      Item;
Hv_Region    region;

{
  short      t, xc;

  t = Item->area->top + 2;
  xc = (Item->area->left + Item->area->right)/2;

  Hv_Simple3DRect(Item->area, False, Hv_canvasColor);

  CenterWrite(xc, &t, "Your Company", Hv_helveticaBold14, Hv_darkBlue);
  t += 2;

  CenterWrite(xc, &t, "1234 Yomama Drive", Hv_helveticaBold11, Hv_black);
  CenterWrite(xc, &t, "Pittsburgh, PA 15212", Hv_helveticaBold11, Hv_black);
  CenterWrite(xc, &t, "support@com.com", Hv_helveticaBold11, Hv_red);
}

/*------ DrawLogo --------*/

void DrawLogo(w, h)

int   w, h;

/* the user name */

{
  short t = 4;
  CenterWrite((short)(w/2), &t, Hv_userName, Hv_helveticaBold14, Hv_skyBlue);
}


