h41331
s 00000/00000/00050
d D 1.2 07/11/12 16:41:05 heddle 3 1
c new start counter
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/draw.c
e
s 00050/00000/00000
d D 1.1 02/09/09 16:27:06 boiarino 1 0
c date and time created 02/09/09 16:27:06 by boiarino
e
u
U
f e 0
t
T
I 1
#include "ced.h"

/*----- DumpCRects --------*/

void  DumpCRects(short       color,
		 Hv_Point    *xp,
		 short       occurs[],
		 XRectangle  *rect)
     
{
  short   i;

  Hv_SetForeground(Hv_colors[color]);
  
  for (i = 0; i < occurs[color]; i++) {
    rect[i].x = xp[i].x;
    rect[i].y = xp[i].y;
  }

  if (Hv_inPostscriptMode) {
      for (i = 0; i < occurs[color]; i++) 
	  Hv_FillRectangle(rect[i].x, rect[i].y,
			   rect[i].width, rect[i].height, color);
  }
  else
      XFillRectangles(Hv_display, Hv_mainWindow, Hv_gc, rect, occurs[color]);
  occurs[color] = 0;   /*reset*/
}


/*----- DumpCRectangles --------*/

void  DumpCRectangles(short       color,
		      short       occurs[],
		      XRectangle  *rect)
     
{
  short   i;

  Hv_SetForeground(Hv_colors[color]);
  
  if (Hv_inPostscriptMode)
    for (i = 0; i < occurs[color]; i++) 
      Hv_FillRectangle(rect[i].x, rect[i].y, rect[i].width, rect[i].height, color);
  else
    XFillRectangles(Hv_display, Hv_mainWindow, Hv_gc, rect, occurs[color]);
  occurs[color] = 0;   /*reset*/
}


E 1
