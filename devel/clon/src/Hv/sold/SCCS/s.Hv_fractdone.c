h28379
s 00000/00000/00000
d R 1.2 02/08/25 23:21:12 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_fractdone.c
e
s 00171/00000/00000
d D 1.1 02/08/25 23:21:11 boiarino 1 0
c date and time created 02/08/25 23:21:11 by boiarino
e
u
U
f e 0
t
T
I 1
/*
-----------------------------------------------------------------------
File:     Hv_fractdone.c
==================================================================================

The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

/* This file contains functions dealing with slider items */

#include "Hv.h"	  /* contains all necessary include files */

#define Hv_FDMINWIDTH  10

/*--------- Hv_FractionDoneUpdate --------- */

extern void  Hv_FractionDoneUpdate(Hv_Item Item,
				   float   fraction)

{
  Hv_FractionDoneData   *fdone;
  Hv_Rect               area;
  Hv_Region             rrgn = NULL;

  if (Item == NULL)
    return;

  fdone = (Hv_FractionDoneData *)(Item->data);
  fdone->fraction = fraction;
/*  Hv_DrawItem(Item, NULL); */

  Hv_CopyRect(&area, Item->area);
  Hv_ShrinkRect(&area, 1, 1);

  rrgn = Hv_RectRegion(&area);
  Hv_DrawItem(Item, rrgn);
  Hv_DestroyRegion(rrgn);

/* also check for exposures, since it may have been a long
   time since last update */

  Hv_CheckForExposures(Expose);
}


/* ------------- Hv_DrawFractionDoneItem ------------*/

void Hv_DrawFractionDoneItem(Hv_Item   Item,
			     Hv_Region region)

{
  Hv_FractionDoneData   *fdone;
  Hv_Rect               area, trect;
  short                 l, t, r, b, yc, yt;
  short                 edge;
  float                 fraction;

  fdone = (Hv_FractionDoneData *)(Item->data);
  fraction = Hv_fMax(0.0, Hv_fMin(1.0, fdone->fraction));

  Hv_CopyRect(&area, Item->area);
  Hv_GetRectLTRB(&area, &l, &t, &r, &b);

  yc = (t + b)/2;

  Hv_DrawLine(l, b, l, t, fdone->topcolor);
  Hv_DrawLine(l, t, r, t, fdone->topcolor);
  Hv_DrawLine(r, t, r, b, fdone->bottomcolor);
  Hv_DrawLine(r, b, l, b, fdone->bottomcolor);


/* now the bar across the middle */

  l++;
  r--;

  edge = l + (short)(fraction*(r-l)); 
  edge = Hv_sMax(l, Hv_sMin(r, edge));  

/* now the "done" part */

  if (edge > l) {
    for (yt = t+1; yt < (yc-6); yt++)
      Hv_DrawLine(l, yt, edge, yt, Hv_navyBlue);

    Hv_DrawLine(l, yc-6, edge, yc-6, Hv_slateBlue);
    Hv_DrawLine(l, yc-5, edge, yc-5, Hv_purple);
    Hv_DrawLine(l, yc-4, edge, yc-4, Hv_white-6);
    Hv_DrawLine(l, yc-3, edge, yc-3, Hv_white-5);
    Hv_DrawLine(l, yc-2, edge, yc-2, Hv_white-4);
    Hv_DrawLine(l, yc-1, edge, yc-1, Hv_white);
    Hv_DrawLine(l, yc,   edge, yc,   Hv_white);
    Hv_DrawLine(l, yc+1, edge, yc+1, Hv_white-1);
    Hv_DrawLine(l, yc+2, edge, yc+2, Hv_white-5);
    Hv_DrawLine(l, yc+3, edge, yc+3, Hv_white-5);
    Hv_DrawLine(l, yc+4, edge, yc+4, Hv_white-7);
    Hv_DrawLine(l, yc+5, edge, yc+5, Hv_purple);
    Hv_DrawLine(l, yc+6, edge, yc+6, Hv_slateBlue);

    for (yt = yc+7; yt < b; yt++)
      Hv_DrawLine(l, yt, edge, yt, Hv_navyBlue);

   } 

/* now the "not done" part */

  if (edge < r) {
    Hv_SetRect(&trect,
	       edge,
	       area.top + 1,
	       Hv_RectRight(&area) - edge,
	       yc - area.top -1);
/*
    Hv_CopyRect(&trecgt, &area);
    trect.top += 1;
    trect.left = edge;
    trect.bottom = yc-1;
    Hv_FixRectWH(&trect);

*/

    Hv_FillRect(&trect, fdone->fillcolor);
    
    Hv_CopyRect(&trect, &area);
    trect.left = edge;
    trect.top = yc+2;
    Hv_FixRectWH(&trect);
    Hv_FillRect(&trect, fdone->fillcolor);
    
    Hv_DrawLine(edge, yc-1, r, yc-1, fdone->topcolor);
    Hv_DrawLine(edge, yc,   r, yc,   fdone->bottomcolor);
    Hv_DrawLine(edge, yc+1, r, yc+1, fdone->topcolor);
  }
}


/*-------- Hv_FractionDoneInitialize --------*/

void Hv_FractionDoneInitialize(Hv_Item Item,
			       Hv_AttributeArray attributes)

{
  Hv_FractionDoneData  *fdone;

/* use a custom color scheme */

  fdone = (Hv_FractionDoneData *)Hv_Malloc(sizeof(Hv_FractionDoneData));
  fdone->fraction = 0.0;
  fdone->topcolor = Hv_gray7;
  fdone->bottomcolor = Hv_gray14;
  fdone->fillcolor = Hv_black;

  Item->data = (void *)fdone;

/* see if user provided width and height */

  Item->area->height = 18;
  Item->area->width = Hv_sMax(Hv_FDMINWIDTH, Item->area->width);
  Hv_FixRectRB(Item->area);

  Item->standarddraw = Hv_DrawFractionDoneItem;
}

#undef Hv_FDMINWIDTH
E 1
