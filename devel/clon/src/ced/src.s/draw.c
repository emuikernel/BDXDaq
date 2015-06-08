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


