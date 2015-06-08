/*
----------------------------------------------------
-							
-  File:    scintillator.c				
-							
-  Summary:						
-           Scintillator Drawing
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

/* ------ local prototypes -------*/


static void DrawScintItem(Hv_Item  Item,
			  Hv_Region region);

static void EditScintItem(Hv_Event   hvevent);

static void FrameSectorScints(Hv_View  View,
			      short   sect ,
			      Hv_Region region);

static void GetSlabFPolygon(short   sect,
			    short   plane,
			    Hv_FPoint *fpts);


static Boolean   FrameOnly = False;


Boolean checkSCData(short sect,
		    SCDataPtr hits) {
    return True;
}

/*----------- ReadDeadTOFFile  ---------------*/

void    ReadDeadTOFFile() {
}

/*-------- DrawScintHits -----*/

void DrawScintHits(Hv_View    View,
                    short     sect,    /* C Index */
                    Hv_Region region)

{
  ViewData     viewdata;
  short        fillcolor;
  int          i;
  SCDataPtr    hits;
  short        slab;
  Hv_Point     poly[4];
  Hv_Item      Item;
  Hv_Rect      srect;

  if (bosIndexSC[sect] == 0)
    return;

  viewdata = GetViewData(View);

  if (!(viewdata->displaySC))
    return;

  fillcolor = scinthitcolor;
  
  switch (View->tag) {
  case SECTOR:
    hits = bosSC[sect];
    for (i = 0; i < bosNumSC[sect]; i++) {
      slab = hits->id - 1; /* convert to C Index */

/* check on validity of slab */

      if ((slab < NUM_SCINT) && (slab >= 0)) {
	GetSlabPolygon(View, sect, slab, poly);

	if ((viewdata->suppressTDC) && ((hits->tdcleft == 0) && (hits->tdcright == 0)))
	    Hv_FillPolygon(poly, 4, False, Hv_gray13, 0);
	else
	    Hv_FillPolygon(poly, 4, False, fillcolor, 0);
      }
      hits++;
    }

/* reframe the scints */

    FrameSectorScints(View, sect, region);
    break;

  case ALLDC:
    Item = viewdata->ScintPlaneItems[sect];
    hits = bosSC[sect];

    /*fprintf(stderr, "sect = %d\n", sect);
fprintf(stderr, "hits = %p\n", hits);
fprintf(stderr, "numhits = %d\n", bosNumSC[sect]);*/

    for (i = 0; i < bosNumSC[sect]; i++) {
      slab = hits->id - 1; /* convert to C Index */

/* check on validity of slab */

      if ((slab < NUM_SCINT) && (slab >= 0)) {
	AllDCScintSlabRect(View, sect, slab, Item->area, &srect);
	Hv_FillRect(&srect, fillcolor);
	Hv_DrawLine(srect.left, srect.top, srect.left, srect.bottom, Hv_white);
	Hv_DrawLine(srect.left+1, srect.top, srect.left+1, srect.bottom, Hv_black);
      }
      hits++;
    }

    Hv_Simple3DRect(Item->area, False, -1);
    break;

  } /* end switch on tag */
}

/*------ MallocScintillator --------*/

Hv_Item MallocScintillator(Hv_View  View,
			   int      sect,
			   short    plane)


/* plane is a C INDEX [0, 3]. 
   It corresponds to the "plane"

   plane 0:   scint 0  - 22
   plane 1:   scint 23 - 33
   plane 2:   scint 34 - 44
   plane 3:   scint 45 - 56

   and is placed in user1 for rapid
   recovery during feedback.

   sect is a C Index [0..5]  */

{
  Hv_Item      Item;
  Hv_FPoint    *fpts;
  char         balloon[60];

  fpts = (Hv_FPoint *)Hv_Malloc(4*sizeof(Hv_FPoint));
  GetSlabFPolygon(sect, plane, fpts);
  
  sprintf(balloon, "Scintillator Plane %1d", plane+1);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SCINTITEM,
			 Hv_NUMPOINTS,    4,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditScintItem,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    FixWPolygonBasedRegion,
			 Hv_USER1,        (int)plane,
			 Hv_USER2,        (int)sect,
			 Hv_BALLOON,      balloon,
			 NULL);
  
  
  Item->type = Hv_USERITEM;
  Item->standarddraw = DrawScintItem;
  return  Item;
}

/*-------- SetScintGeometrySector -----*/

void SetScintGeometrySector(Hv_View View)

{
  short                sect;
  Hv_Item              temp;
  Hv_FPoint            *fpts;
  Hv_WorldPolygonData  *wpoly;
  ViewData             viewdata;
  int                  i;

  if (View->tag != SECTOR)
    return;

  viewdata = GetViewData(View);

  for (i = 0; i < NUM_SCINTPLANE2; i++)
    if ((temp = viewdata->ScintPlaneItems[i]) != NULL) {
      sect = SetItemSector(temp);

/* recal that the "plane" is stored in user1 */

      wpoly = (Hv_WorldPolygonData *)(temp->data);
      fpts = wpoly->fpts;
      
      GetSlabFPolygon(sect, (short)(temp->user1), fpts);
      temp->fixregion(temp);
    }
}



/*------- GetSlabPolygon -------*/

void GetSlabPolygon(Hv_View  View,
		    short     sect, /*C Index*/
		    short     slab, /*C Index*/
		    Hv_Point *poly)

{
  Hv_FPoint    fpts[4];
  int          j;

  for (j = 0; j < 4; j++) {
    fpts[j].h = Scintillators[sect].scints[slab].P[j].h;
    fpts[j].v = Scintillators[sect].scints[slab].P[j].v;
    fpts[j].v = MidplaneXtoProjectedX(View, fpts[j].v);
  }

  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);

}


/*------ GetScintHit -------*/

void GetScintHit(short   sect,   /* C Index */
		 short   scint,  /* C Index */
		 short   *tdcl,
		 short   *adcl,
		 short   *tdcr,
		 short   *adcr)

{
  SCDataPtr    hits;
  int          i;
  short        slab;

  *tdcl = -1;
  *adcl = -1;
  *tdcr = -1;
  *adcr = -1;

  if (bosIndexSC[sect] == 0)
    return;

  hits = bosSC[sect];
  for (i = 0; i < bosNumSC[sect]; i++) {
      slab = hits->id - 1; /* convert to C Index */
      if (slab == scint) {
	*tdcl = hits->tdcleft;
	*adcl = hits->adcleft;
	*tdcr = hits->tdcright;
	*adcr = hits->adcright;
	break;
      }
      hits++;
    }

}


/*------ ScintSlabLimits ------*/

void ScintSlabLimits(short  ID,
		     short  sect,
		     short  *minid,
		     short  *maxid) {

/* ID is a C INDEX [0, 3]. 
   It corresponds to the "plane"

   plane 0:   scint 0  - 22
   plane 1:   scint 23 - 33
   plane 2:   scint 34 - 44
   plane 3:   scint 45 - 56
*/


    if (ID == 0) {
      *minid = 0;
      *maxid = 22;
    }
    else if (ID == 1) {
      *minid = 23;
      *maxid = 33;
    }
    else if (ID == 2) {
      *minid = 34;
      *maxid = 44;
    }
    else if (ID == 3) {
      *minid = 45;
      *maxid = 56;
    }

}

/*----- PanelFromID -------*/


int  PanelFromID(int ID)

/*  ID: NOT a C Index*/

{
  if (ID < 24)
    return 1;

  if (ID < 35)
    return 2;

  if (ID < 46)
    return 3;

  if (ID < 58)
    return 4;

  return -1;
}


/*---------- DrawScintItem --------*/

static void      DrawScintItem(Hv_Item   Item,
			       Hv_Region region)

{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  short                 i, minid, maxid;
  Hv_FPoint             fp1, fp2;
  short                 ID, sect;
  short                 x1, x2, y1, y2;

  viewdata = GetViewData(View);
  
  if (!(viewdata->displaySC))
    return;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL) {
    return;
  }
  
  if (FrameOnly) {
    Hv_FramePolygon(wpoly->poly, 4, scintframecolor);
  }
  else {
    Hv_FillPolygon(wpoly->poly, 4, True, scintfillcolor,
		   scintframecolor);
  }

/* now the lines marking the slabs */

  ID   = (short)(Item->user1); //the plane index
  sect = (short)(Item->user2);

  ScintSlabLimits(ID, sect, &minid, &maxid);
  
  for (i = minid; i < maxid; i++) {
    fp1 = Scintillators[sect].scints[i].P[3];
    fp2 = Scintillators[sect].scints[i].P[0];

    fp1.v = MidplaneXtoProjectedX(View, fp1.v);
    fp2.v = MidplaneXtoProjectedX(View, fp2.v);

    Hv_WorldToLocal(View, fp1.h, fp1.v, &x1, &y1);
    Hv_WorldToLocal(View, fp2.h, fp2.v, &x2, &y2);
    Hv_DrawLine(x1, y1, x2, y2, scintframecolor);
  }

}

/*-------- EditScintItem -------*/

static void   EditScintItem(Hv_Event hvevent)

/* item edit for SCINTITEMs  */

{
    EditDetectorColors();
}


/*------- FrameSectorScints ----- */

static void FrameSectorScints(Hv_View  View,
			      short   sect ,    /* C Index */
			      Hv_Region region)
{
  ViewData      viewdata;
  Hv_Item       temp;
  int           i;
  
  viewdata =  GetViewData(View);

  FrameOnly = True;
  
  for (i = 0; i < NUM_SCINTPLANE2; i++)
    if ((temp = viewdata->ScintPlaneItems[i]) != NULL)
      if (temp->user2 == sect)
	DrawScintItem(temp, region);

  FrameOnly = False;
}


/*------------ GetSlabFPolygon --------*/

static void   GetSlabFPolygon(short   sect,
			      short   plane,
			      Hv_FPoint *fpts)
{
  Hv_FPoint      *Pmin, *Pmax;
  short          minid, maxid;

  ScintSlabLimits(plane, sect, &minid, &maxid);
  Pmin = Scintillators[sect].scints[minid].P;
  Pmax = Scintillators[sect].scints[maxid].P;
  fpts[0] = Pmin[1];
  fpts[1] = Pmin[2];
  fpts[2] = Pmax[3];
  fpts[3] = Pmax[0];
}
