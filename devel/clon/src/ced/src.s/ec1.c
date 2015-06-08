/*
----------------------------------------------------
-							
-  File:    ec1.c				
-							
-  Summary:						
-           Italian Shower Counter Drawing 
-  
-  The routines here are either GENERIC or specific
-  to the sector view.
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

static void fractPoint(float f,
		       Hv_FPoint *fp1,
		       Hv_FPoint *fp2,
		       Hv_FPoint *fp);

static void DrawEC1Item(Hv_Item  Item,
			Hv_Region region);

static void EditEC1Item(Hv_Event   hvevent);

static Boolean FrameOnly = False;

/*--------- checkEC1Data ------------*/

Boolean checkEC1Data(short sect,
		     ECDataPtr hits) {
    return True;
}

/*------ MallocEC1Shower --------*/

Hv_Item MallocEC1Shower(Hv_View  View,
			int      sect)  /*C Index*/

{
  Hv_Item    Item;
  Hv_FPoint  *fpts;
  char       balloon[60];

  fpts = (Hv_FPoint *)Hv_Malloc(6*sizeof(Hv_FPoint));
  GetSectorViewEC1FPolygon((short)sect, fpts);

  sprintf(balloon, "Italian Calorimeter");

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SECTOREC1ITEM,
			 Hv_NUMPOINTS,    6,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditEC1Item,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    FixWPolygonBasedRegion,
			 Hv_USER2,        (int)sect,
			 Hv_BALLOON,      balloon,
			 NULL);
  
  Item->standarddraw = DrawEC1Item;
  Item->type = Hv_USERITEM;
  return Item;
}

/*---------- DrawEC1Item --------*/

static void DrawEC1Item(Hv_Item  Item,
			Hv_Region region)
{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  Hv_Point              poly[6];
  short                 sect;
  int                   i, j;
  Hv_Point              poly2[4];

  viewdata = GetViewData(View);

  sect = Item->user2;

  if (sect > 1) {
      Hv_DisableItem(Item);
      return;
  }

  Hv_EnableItem(Item);

  if (!(viewdata->displayEC))
    return;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;
  
  if (FrameOnly)
    Hv_FramePolygon(wpoly->poly, 6, ec1framecolor);
  else
    Hv_FillPolygon(wpoly->poly, 6, True, ec1fillcolor[0],
		   ec1framecolor);

/* the extra, nonactive part */

  if (FrameOnly)
    Hv_FramePolygon(wpoly->poly + 1, 4, ec1framecolor);
  else
    Hv_FillPolygon(wpoly->poly + 1, 4, True, Hv_skyBlue,
		   ec1framecolor);

/* now the line marking the layer separations */
  
  EC1LayerPolygon(Item, sect, 1, poly);
  if (FrameOnly)
    Hv_FramePolygon(poly, 4, ec1framecolor);
  else
    Hv_FillPolygon(poly, 4, True, ec1fillcolor[1],
		   ec1framecolor);

  EC1LayerPolygon(Item, sect, 3, poly);
  if (FrameOnly)
    Hv_FramePolygon(poly, 4, ec1framecolor);
  else
    Hv_FillPolygon(poly, 4, True, ec1fillcolor[1],
		   ec1framecolor);


/* draw the long strip polys only*/

  for (j = 0; j < 3; j = j + 2)
      for (i = 0; i < 24; i++) {
	  EC1SectorViewLongStripPoly(Item, sect, j, i, poly);
	  Hv_FramePolygon(poly, 4, ec1framecolor);
      }

/* draw the short strip poly region */

  for (j = 1; j < 4; j = j + 2) {
      EC1SectorViewShortStripPoly(Item, sect, j, 39, poly);

      poly2[0].x = poly[0].x;      poly2[0].y = poly[0].y;
      poly2[1].x = poly[1].x;      poly2[1].y = poly[1].y;
      EC1SectorViewShortStripPoly(Item, sect, j, 0, poly);
      poly2[2].x = poly[2].x;      poly2[2].y = poly[2].y;
      poly2[3].x = poly[3].x;      poly2[3].y = poly[3].y;
      Hv_FillPolygon(poly2, 4, True, ec1fillcolor[0], ec1framecolor);
  }


/*  for (j = 1; j < 4; j = j + 2)
      for (i = 0; i < 40; i++) {
	  EC1SectorViewShortStripPoly(Item, sect, j, i, poly);
	  Hv_FramePolygon(poly, 4, ec1framecolor);
      }  */
}


/*-------- EC1LayerFPolygon -----*/

static void EC1LayerFPolygon(Hv_Item    Item,
			     int        sect,
			     int        layer,
			     Hv_FPoint  *fpoly)

{
    Hv_FPoint  *gd;
    float       f1, f2;

    gd = LargeAngleEcal[sect].PA;

    f1 = ((float)layer)/4.0;
    f2 = f1 + 0.25;

    fractPoint(f1, gd,   gd+1, fpoly);
    fractPoint(f2, gd,   gd+1, fpoly+1);
    fractPoint(f2, gd+5, gd+4, fpoly+2);
    fractPoint(f1, gd+5, gd+4, fpoly+3);

    MidplanePolyToProjectedPoly(Item->view, fpoly, 4);
}


/*-------- EC1LayerPolygon ------*/

void EC1LayerPolygon(Hv_Item   Item,
		     int       sect,
		     int       layer,
		     Hv_Point  *poly)
{
    Hv_FPoint   fpoly[4];

    EC1LayerFPolygon(Item, sect, layer, fpoly);
    Hv_WorldPolygonToLocalPolygon(Item->view, 4, poly, fpoly);

}


/*---------- EC1SectorViewShortStripPoly ----------*/

void EC1SectorViewShortStripPoly(Hv_Item   Item,
				 int       sect,
				 int       layer,
				 int       id,
				 Hv_Point *poly) {

/* these are used in layers 1 & 3 (C Index) and
   represent the short strips which are parallel
   to the page -- THUS THESE ARE NOT GEOMETRICALLY
   REALISTIC-- JUST HOT SPOTS FOR MOUSE FEEDBACK */

    Hv_FPoint   fpoly[4];
    Hv_FPoint   fpts[4];
    Hv_FPoint   fpts2[4];

    float       f1, f2;


    EC1LayerFPolygon(Item, sect, layer, fpoly);
    
    if ((id < 0) || (id > 39))
	return;


    f1 = ((float)id/40.0);
    f2 = ((float)(id+1)/40.0);


    fractPoint(f1, fpoly+2, fpoly+3, fpts);
    fractPoint(f2, fpoly+2, fpoly+3, fpts+1);
    fractPoint(f2, fpoly+1, fpoly,   fpts+2);
    fractPoint(f1, fpoly+1, fpoly,   fpts+3);

/*    fprintf(stderr, "fpts[0]  %f %f\n", fpts[0].h, fpts[0].v);
    fprintf(stderr, "fpts[1]  %f %f\n", fpts[1].h, fpts[1].v);
    fprintf(stderr, "fpts[2]  %f %f\n", fpts[2].h, fpts[2].v);
    fprintf(stderr, "fpts[3]  %f %f\n", fpts[3].h, fpts[3].v);  */

    f1 = (39.0 - (float)id)/100.0;
    f2 = 1.0 - f1;

    fractPoint(f1, fpts+1, fpts+2, fpts2);
    fractPoint(f2, fpts+1, fpts+2, fpts2+1);
    fractPoint(f2, fpts,   fpts+3,   fpts2+2);
    fractPoint(f1, fpts,   fpts+3,   fpts2+3); 

    Hv_WorldPolygonToLocalPolygon(Item->view, 4, poly, fpts2);

/* fix zero height poly in low res mode */

    if ((id < 39) && (poly[0].y == poly[3].y) && (poly[1].y == poly[2].y)) {
/*	fprintf(stderr, "fix poly for id = %d\n", id+1); */
	poly[3].y++;
	poly[2].y++;
    }

/*
    fprintf(stderr, "***id = %d\n", id+1);
    fprintf(stderr, "poly[0]  %d %d\n", poly[0].x, poly[0].y);
    fprintf(stderr, "poly[1]  %d %d\n", poly[1].x, poly[1].y);
    fprintf(stderr, "poly[2]  %d %d\n", poly[2].x, poly[2].y);
    fprintf(stderr, "poly[3]  %d %d\n", poly[3].x, poly[3].y);  */
}

/*---------- EC1SectorViewLongStripPoly ----------*/

void EC1SectorViewLongStripPoly(Hv_Item   Item,
				int       sect,
				int       layer,
				int       id,
				Hv_Point *poly) {

/* these are used in layers 0 & 2 (C Index) and
   represent the long strips which are perpendicular
   to the page */

    Hv_FPoint   fpoly[4];
    Hv_FPoint   fpts[4];

    float       f1, f2;
    
    
    if ((id < 0) || (id > 23))
	return;

    EC1LayerFPolygon(Item, sect, layer, fpoly);
    f1 = ((float)id/24.0);
    f2 = ((float)(id+1)/24.0);
    
    fractPoint(f1, fpoly+3, fpoly,   fpts);
    fractPoint(f2, fpoly+3, fpoly,   fpts+1);
    fractPoint(f2, fpoly+2, fpoly+1, fpts+2);
    fractPoint(f1, fpoly+2, fpoly+1, fpts+3);

/*    MidplanePolyToProjectedPoly(Item->view, fpoly, 4); */
    Hv_WorldPolygonToLocalPolygon(Item->view, 4, poly, fpts);
}

/*-------- fractPoint ----------*/

static void fractPoint(float f,
		       Hv_FPoint *fp1,
		       Hv_FPoint *fp2,
		       Hv_FPoint *fp)

{
    fp->h = fp1->h + f*(fp2->h - fp1->h);
    fp->v = fp1->v + f*(fp2->v - fp1->v);
}

/*-------- EditEC1Item -------*/

static void   EditEC1Item(Hv_Event hvevent)

/* item edit for EC1ITEMs (Italian Cal)  */

{
    EditDetectorColors();
}

/*-------- GetSectorViewEC1Polygon --------*/

void GetSectorViewEC1Polygon(Hv_View   View,
			    short     sect,
			    Hv_Point  *poly)

/* plane and sect are C indices */

{
    Hv_FPoint  fpts[6];
    GetSectorViewEC1FPolygon(sect, fpts);
    MidplanePolyToProjectedPoly(View, fpts, 6);
    Hv_WorldPolygonToLocalPolygon(View, 6, poly, fpts);
}

/*------ GetSectViewEC1Hit --------*/

void GetSectViewEC1Hit(short    sect,
		       short    lay,
		       short    thestrip,
		       short   *tdcL,
		       short   *adcL,
		       short   *tdcR,
		       short   *adcR) 
{
    EC1DataPtr   hits;
    unsigned char strip, layer;
    int    i;

    if (bosIndexEC1[sect] == 0)
	return;

/* now look for a matching hit */

    hits = bosEC1[sect];


    for (i = 0; i < bosNumEC1[sect]; i++) {
	BreakShort(hits->id, &strip, &layer);

    if ((short)layer == lay)
	if ((short)strip == thestrip) {
	    *tdcL = (short)(hits->tdcleft);
	    *adcL = (short)(hits->adcleft);
	    *tdcR = (short)(hits->tdcright);
	    *adcR = (short)(hits->adcright);
	    return;
	}
    hits++;
  }

}

/*------- DrawEC1Hits --------*/

void DrawEC1Hits(Hv_View   View,
		 short     sect,
		 Hv_Region region)
{
    EC1DataPtr   hits;
    unsigned char strip, layer;
    int         i;
    Hv_Item     Item;
    ViewData    viewdata;
    short       fillcolor;
    Hv_Point    poly[4];

    if (bosIndexEC1[sect] == 0)
	return;

    viewdata = GetViewData(View);
    
    if (!(viewdata->displayEC))
	return;

    fillcolor = ec1hitcolor;

    hits = bosEC1[sect];

    Item = viewdata->SectViewEC1Items[0];

    for (i = 0; i < bosNumEC1[sect]; i++) {
	BreakShort(hits->id, &strip, &layer);
	
	layer--;
	strip--;

 /* layers 0 & 2 are the long strips, which have the faithful geometry */

	if ((layer == 0) || (layer == 2)) {
	    EC1SectorViewLongStripPoly(Item, sect, (int)layer, (int)strip, poly);

	    Hv_FillPolygon(poly, 4, True, ec1hitcolor,
			   ec1framecolor);
	}

/* layers 1 & 3 are the short strips, which do NOT have a faithful geometry */

	if ((layer == 1) || (layer == 3)) {
	    EC1SectorViewShortStripPoly(Item, sect, (int)layer, (int)strip, poly);
	    Hv_FillPolygon(poly, 4, True, ec1hitcolor, ec1hitcolor);
	}



    hits++;
  }


}


/*-------- GetSectorViewEC1FPolygon --------*/

void GetSectorViewEC1FPolygon(short     sect,
			      Hv_FPoint *fpts)

/* plane and sect are C indices */


/* NOTE: THIS DOES NOT RETURN PHI CORRECTED POLY */

{
    Hv_FPoint  *gd;
    int        i;

    gd = LargeAngleEcal[sect].PA;

    for (i = 0; i < 6; i++) {
	fpts[i].h = gd[i].h;
	fpts[i].v = gd[i].v;
    }
}
