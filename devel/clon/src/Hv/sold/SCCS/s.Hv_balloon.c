h16616
s 00000/00000/00000
d R 1.2 02/08/25 23:21:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/sold/Hv_balloon.c
e
s 00365/00000/00000
d D 1.1 02/08/25 23:21:03 boiarino 1 0
c date and time created 02/08/25 23:21:03 by boiarino
e
u
U
f e 0
t
T
I 1
/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/

#include "Hv.h"	        /* contains all necessary include files */

/*------ local prototypes ---------*/

static int Hv_StuffBalloonStrs(char *);

/********* Hv_CreateBalloon **********/

void   Hv_CreateBalloon(Hv_Item Item,
			char *text)

/* simply mallocs the balloon string for the
   specified item and sets it to the passed
   string */

{
  Hv_InitCharStr(&(Item->balloon), text);
  Hv_SetBit(&(Item->drawcontrol), Hv_HASBALLOON);
}

/********* Hv_DestroyBalloon **********/

void   Hv_DestroyBalloon(Hv_Item Item)

/* frees the balloon string for the specified item */

{
  Hv_Free(Item->balloon);
  Hv_ClearBit(&(Item->drawcontrol), Hv_HASBALLOON);
}


/********* Hv_ShowBalloon ********/

void Hv_ShowBalloon(void)

/* Displays a previously "stuffed" balloon */

{
  int           i;
  short         Margin = 10;
  short         Margin2, W, Width;
  short         LineHeight, TotalHeight; 
  Hv_Point      BalloonPoints[5];
  short         whatcase;
  short         xo, yo;
  unsigned long old_fg;



  Width = 0;
  Margin2 = 2*Margin;

/* now get max width of the  Xmsts for this balloon */

  for (i = 0; i < Hv_theBalloon.numline; i++) {
    W = (short)XmStringWidth(Hv_fontList, Hv_balloonXmst[i]);
    if (W > Width)
      Width = W;
  }

/* get the limiting size of the balloon */

  Width = Width + Margin2;
  LineHeight = (short)XmStringHeight(Hv_fontList, Hv_balloonXmst[0]);
  TotalHeight = 2*Margin2 + (Hv_theBalloon.numline*LineHeight);
  
/* now decide where the balloon is to be drawn */

  BalloonPoints[0] = Hv_theBalloon.pp;   /*the anchor point from which the balloon emerges */


  if (((Hv_theBalloon.pp.x + Width) < Hv_canvasRect.right) && ((Hv_theBalloon.pp.y - TotalHeight) > Hv_canvasRect.top))
    whatcase = 1;  /* above, right */
  else if (((Hv_theBalloon.pp.x + Width) < Hv_canvasRect.right) && ((Hv_theBalloon.pp.y + TotalHeight) < Hv_canvasRect.bottom))
    whatcase = 2;  /* below, right */
  else if (((Hv_theBalloon.pp.x - Width) > Hv_canvasRect.left) && ((Hv_theBalloon.pp.y - TotalHeight) > Hv_canvasRect.top))
    whatcase = 3;  /* above, left */
  else whatcase = 4; /* below, left */


/* set up the other four points */

  switch  (whatcase) {
  case 1:
    xo = Hv_theBalloon.pp.x + Margin;
    yo = Hv_theBalloon.pp.y - TotalHeight + Margin;
    Hv_SetPoint(&(BalloonPoints[1]), Hv_theBalloon.pp.x, Hv_theBalloon.pp.y - TotalHeight);
    Hv_SetPoint(&(BalloonPoints[2]), Hv_theBalloon.pp.x + Width, Hv_theBalloon.pp.y - TotalHeight);
    Hv_SetPoint(&(BalloonPoints[3]), Hv_theBalloon.pp.x + Width, Hv_theBalloon.pp.y - Margin2);
    Hv_SetPoint(&(BalloonPoints[4]), Hv_theBalloon.pp.x + Margin, Hv_theBalloon.pp.y - Margin2);
    break;

  case 2:
    xo = Hv_theBalloon.pp.x + Margin;
    yo = Hv_theBalloon.pp.y + Margin2 + Margin;
    Hv_SetPoint(&(BalloonPoints[1]), Hv_theBalloon.pp.x, Hv_theBalloon.pp.y + TotalHeight);
    Hv_SetPoint(&(BalloonPoints[2]), Hv_theBalloon.pp.x + Width, Hv_theBalloon.pp.y + TotalHeight);
    Hv_SetPoint(&(BalloonPoints[3]), Hv_theBalloon.pp.x + Width, Hv_theBalloon.pp.y + Margin2);
    Hv_SetPoint(&(BalloonPoints[4]), Hv_theBalloon.pp.x + Margin, Hv_theBalloon.pp.y + Margin2);
    break;
	      
  case 3: default:
    xo = Hv_theBalloon.pp.x - Width + Margin;
    yo = Hv_theBalloon.pp.y - TotalHeight + LineHeight;
    Hv_SetPoint(&(BalloonPoints[1]), Hv_theBalloon.pp.x, Hv_theBalloon.pp.y - TotalHeight);
    Hv_SetPoint(&(BalloonPoints[2]), Hv_theBalloon.pp.x - Width, Hv_theBalloon.pp.y - TotalHeight);
    Hv_SetPoint(&(BalloonPoints[3]), Hv_theBalloon.pp.x - Width, Hv_theBalloon.pp.y - Margin2);
    Hv_SetPoint(&(BalloonPoints[4]), Hv_theBalloon.pp.x - Margin, Hv_theBalloon.pp.y - Margin2);
    break;

  case 4:
    xo = Hv_theBalloon.pp.x -Width + Margin;
    yo = Hv_theBalloon.pp.y + Margin2 + Margin;
    Hv_SetPoint(&(BalloonPoints[1]), Hv_theBalloon.pp.x, Hv_theBalloon.pp.y + TotalHeight);
    Hv_SetPoint(&(BalloonPoints[2]), Hv_theBalloon.pp.x - Width, Hv_theBalloon.pp.y + TotalHeight);
    Hv_SetPoint(&(BalloonPoints[3]), Hv_theBalloon.pp.x - Width, Hv_theBalloon.pp.y + Margin2);
    Hv_SetPoint(&(BalloonPoints[4]), Hv_theBalloon.pp.x - Margin, Hv_theBalloon.pp.y + Margin2);
    break;
  }

  Hv_theBalloon.region = Hv_CreateRegionFromPolygon(BalloonPoints, 5);
  XShrinkRegion((Region)(Hv_theBalloon.region), -1, -1);
  Hv_SetClippingRegion(Hv_theBalloon.region);

/* start drawing the balloon */

  Hv_FramePolygon(BalloonPoints, 5, Hv_darkBlue);
  Hv_FillPolygon(BalloonPoints, 5, True, Hv_honeydew, Hv_darkBlue);

  
  old_fg = Hv_SwapForeground(Hv_darkBlue);    /* swap & store the old foregound color */

  for (i = 0; i < Hv_theBalloon.numline; i++)
    XmStringDraw(Hv_display, Hv_mainWindow, Hv_fontList, Hv_balloonXmst[i], Hv_gc, xo,
		 yo+i*LineHeight, XmStringWidth(Hv_fontList, Hv_balloonXmst[i]), XmALIGNMENT_BEGINNING, XmALIGNMENT_BEGINNING, NULL);
    
  Hv_balloonUp = True;
  Hv_SetForeground(old_fg);         /* resets the forground to the original value */
}

/********* Hv_StuffBalloon **********/

void   Hv_StuffBalloon(Hv_Point MousePoint,
		       char *Text)

/* this routine will be used to stuff a new balloon. It also
   calls ShowBalloon, to display the newly stuffed balloon */

{
  int     i;

/* free any XmStrings from previos balloons */

  for (i = 0; i < Hv_MAXBALLOONSTR; i++) {
    if (Hv_balloonXmst[i]) {
      XmStringFree(Hv_balloonXmst[i]);
      Hv_balloonXmst[i] = NULL;
    }
  }

/* stuff some parameters of the global balloon data */

  Hv_theBalloon.pp = MousePoint;
  Hv_theBalloon.text = Text;
  Hv_theBalloon.numline = Hv_StuffBalloonStrs(Hv_theBalloon.text);

/* now get the new Xmsts for this balloon */

  for (i = 0; i < Hv_theBalloon.numline; i++) 
    Hv_balloonXmst[i] = Hv_CreateMotifString(Hv_balloonStrs[i], Hv_fixed2);

/* now show the balloon */

  Hv_ShowBalloon();
}

/*------- Hv_PrepareBalloon ------- */

void Hv_PrepareBalloon(Hv_View View,
		       Hv_Item Item,
		       short   WhatArea,
		       char    *text)

{
  Hv_Point pp;
  Boolean  DoIt = True;

/* if no balloon up, then stuuf the balloon. If same balloon
is up, do nothing. if other balloon is up, remove it and stuff
a new balloon */

  if (Item != NULL) {
    DoIt = Hv_CheckBit(Item->drawcontrol, Hv_HASBALLOON);
    if (!DoIt) {
      if ((Item->parent != NULL) && (Item->parent->type == Hv_BOXITEM)) {
	Item = Item->parent;
	DoIt = Hv_CheckBit(Item->drawcontrol, Hv_HASBALLOON);
      }
    }
  }

  if (DoIt) {
    Hv_BalloonPoint(View, Item, &pp, WhatArea);
    
    if (!Hv_balloonUp)
      Hv_StuffBalloon(pp, text);
    else {
      if (strcmp(Hv_theBalloon.text, text) != 0) {
	Hv_HideBalloon();
	Hv_StuffBalloon(pp, text);
      }
    }
  }
  else
    Hv_HideBalloon();
}

/*------- Hv_InitBalloons ------*/

void Hv_InitBalloons(void)

{
  int   i;

  Hv_showBalloons = False;                 /* default: no balloon help */
  Hv_balloonUp = False;                    /* default: no balloon visible */
  Hv_theBalloon.text = NULL;               /* global balloon next defaults to NULL */
  Hv_theBalloon.region = NULL;

  for (i = 0; i < Hv_MAXBALLOONSTR; i++) {
    Hv_balloonStrs[i] = (char *)Hv_Malloc(Hv_MAXBALLOONSTRLEN + 1);
    Hv_balloonXmst[i] = NULL;
  }

}

/*------- Hv_BallonPoint ------- */

void Hv_BalloonPoint(Hv_View  View,
		     Hv_Item  Item,
		     Hv_Point *pp,
		     short    WhatArea)

{
  Hv_Rect         *area;
  Hv_ColorScheme  *scheme;
  short           bw2;
  short           l, t, r, b;

/* if this is based on a drawn item, use its area. If
   its bases on a window component, do something else */

  if (Item != NULL) {
    pp->x = Item->area->left + (Item->area->width/2);
    pp->y = Item->area->top + (Item->area->height/3);
  }
  else {
    scheme = View->containeritem->scheme;
    area = View->containeritem->area;

    Hv_GetRectLTRB(area, &l, &t, &r, &b);
    bw2 = (scheme->borderwidth)/2;

    if (Hv_showBalloons)
      switch (WhatArea) {
	
      case Hv_INCLOSEBOX:
	pp->x = l + bw2;
	pp->y = t + bw2;
	break;
	
      case Hv_INDRAGREGION:
	pp->x = l + (area->width/2);
	pp->y = t + bw2;
	break;
	
      case Hv_INBIGBOX:
	pp->x = r - bw2;
	pp->y = t + bw2;
	break;
	
	
      case Hv_INGROWBOX:
	pp->x = r - bw2;
	pp->y = b - bw2;
	
      default:
	break;
      }
  }
}

/********* Hv_HideBalloon **********/

void   Hv_HideBalloon(void)

{
  Hv_Rect    brect;


  if (Hv_balloonUp) {
    Hv_theBalloon.text = NULL;
    if (Hv_theBalloon.region) {
      Hv_ClipBox(Hv_theBalloon.region, &brect);
      Hv_SetClippingRegion(Hv_theBalloon.region);
      XClearArea(Hv_display, Hv_mainWindow, brect.left, brect.top, brect.width, brect.height, True);
    }
    Hv_theBalloon.region = NULL;
    Hv_balloonUp = False;
  }
}


/*======== local static functions =======*/

/******* Hv_StuffBalloonStrs ********/

static int Hv_StuffBalloonStrs(char *str)

/* takes the single string str, and breaks it
(at spaces) into smaller strings, which are
then copied into the global BalloonStrs
structure */

{
  int   numline = 0;
  char  *t1, *t2, *tstop;

  tstop = str + strlen(str);
  t1 = str;

  while ((numline < Hv_MAXBALLOONSTR) && (t1 < tstop)) {
    t2 = t1 + Hv_MAXBALLOONSTRLEN - 1;

    if (t2 > tstop)
      t2 = tstop;

    while ((*t2 != '\0') && (*t2 != ' '))
      t2--;

    strncpy(Hv_balloonStrs[numline], t1, t2-t1);
    Hv_balloonStrs[numline][t2-t1] = '\0'; 
    numline++;
    t1 = t2 + 1;
  }

  return numline;
}




E 1
