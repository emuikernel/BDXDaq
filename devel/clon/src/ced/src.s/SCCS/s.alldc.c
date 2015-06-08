h05206
s 00000/00000/01474
d D 1.4 07/11/12 16:41:04 heddle 5 4
c new start counter
e
s 00037/00033/01437
d D 1.3 04/02/06 17:24:00 boiarino 4 3
c fix memory leak
e
s 00002/00001/01468
d D 1.2 03/04/17 16:40:54 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/alldc.c
e
s 01469/00000/00000
d D 1.1 02/09/09 16:27:03 boiarino 1 0
c date and time created 02/09/09 16:27:03 by boiarino
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
-  File:    alldc.c	
-							
-  Summary:						
-           controls "all dc" view
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

#define    WIRESEARCHLEN          20
#define    ALLDC_MAXTHETA         151.0
#define    ALLDC_BIGG             0.6
#define    ALLDC_SMALLG           0.2
#define    ALLDC_S                0.45

#define MAXPNTSINREQ   1000   /*max number of points allowed to accumulate into a request */

/*------ local prototypes ------*/

static void FixLines(Hv_View  View,
		     short    sect,
		     Boolean *dirty);

static void DrawAllDC1TrackHits(Hv_View     View,
				short        sect,
				int          numhits,
				int          numtracks,
				DC1DataPtr   hits,
				HBLADataPtr  hbla,
				Hv_Region    region);

static void DrawAllDCDeadSuperLayerWires(Hv_View   View,
					 short     sect,
					 short     rgn,     /*0..2*/
					 short     supl,    /*0..1*/
					 Hv_Region region);

static void EditAllDCItem(Hv_Event hvevent);

static void DrawAccumulatedHits(Hv_View     View,
			  short       sect,
			  int         numhits,
			  char       *chits,
			  size_t      size,
			  Hv_Region   region);

static void AllDCSuperLayerYLimits(Hv_Rect *sr,
				   short   sect,
				   short   rgn,
				   short   supl,
				   short   numlay,
				   short   *ymin,
				   short   *ymax);


static void  AllDCSuperLayerFRect(Hv_View View,
				 short    sect,
				 short    rgn,
				 short    supl,
				 Hv_FRect *suplrect);


static void DrawAllDCScint(Hv_Item   Item,
			  Hv_Region region);

static void AllDCScintRect(Hv_View   View,
			   short     sect,
			   Hv_Rect   *scintrect);

static void AllDCScintFRect(Hv_View   View,
			    short     sect,
			    Hv_FRect *scintrect);

static float  ScintSlabTheta(short sect,
			     short scint);

static void ScintThetaSlabLimits(short sect,
				 short scint,
				 float *thetamin,
				 float *thetamax);

static void ScintThetaLimits(short sect,
			     float *thetamin,
			     float *thetamax);

static float  AllDCXtoTheta(short    x,
			    Hv_Rect *sectrect);

static void DrawLabel(short x,
		      short y,
		      char *str,
		      Hv_Region region);

static void      DrawSectorGridItem(Hv_Item   Item,
				    Hv_Region region);


Boolean skipDead = False;

/*----- GetAllDCCoordinates ----------*/

void  GetAllDCCoordinates(Hv_View   View,
			  Hv_Point  pp, 
			  short    *sect, 
			  float    *r,
			  float    *theta,
			  float    *z,
			  float    *x,
			  short    *supl,
			  short    *lay,
			  short    *wire,
			  short    *scint)

{
  int          i, j, k, w;
  Hv_Rect      sectrect, rect, srect, lrect;
  Hv_FPoint    *fp;
  float        t, u;
  float        f[4];
  ViewData     viewdata;

  short        rgn, isupl;
  Hv_Item      theItem;
  DC_Super_Layer   *supptr;
  DC_Layer         *layptr;

  viewdata = GetViewData(View);

  *supl = -1;
  *lay = -1;
  *wire = -1;

  *sect = -1;
  *r = 1.0e32;
  *theta = 1.0e32;
  *z = 1.0e32;
  *x = 1.0e32;
  *scint = -1;

  for (i = 0; i < 6; i++) {
    SectorGridSectorRect(View, (short)i, &sectrect);
    if (Hv_PointInRect(pp, &sectrect)) {
      *sect = i;
      break;
    }
  }

/* if not in any sect then just cave */

  if (*sect < 0)
    return;

/* now find the other coordinates */

  *theta = Hv_RADTODEG*AllDCXtoTheta(pp.x, &sectrect);

/* see if we are in a scint */

/*  AllDCScintRect(View, *sect, &rect); */

  Hv_CopyRect(&rect, viewdata->ScintPlaneItems[*sect]->area);

  if (Hv_PointInRect(pp, &rect)) {
    for (i = 0; i < NUM_SCINT; i++) {
      AllDCScintSlabRect(View, *sect, i, &rect, &srect);
      Hv_ShrinkRect(&srect, -1, -1);
      if (Hv_PointInRect(pp, &srect)) {
	*scint = i;
	
	fp = Scintillators[*sect].scints[i].P;

/* see what "fraction" of the rect we are pointing at */

	t = ((float)(pp.x - srect.left))/srect.width;

	if (*sect < 3) /* top */
	    u = ((float)(srect.bottom - pp.y))/srect.height;
	else  /* bottom */
	    u = ((float)(pp.y - srect.top))/srect.height;


/* bilinear interpolation */

	f[0] = (1.0 - t)*(1.0 - u);
	f[1] = t*(1.0 - u);
	f[2] = t*u;
	f[3] = (1.0 - t)*u;

	*z = 0.0;
	*x = 0.0;
	for (j = 0; j < 4; j++) {
	    *z += f[j]*fp[j].h;
	    *x += f[j]*fp[j].v;
	}


/*	*z = 0.5*(Scintillators[*sect].scints[*scint].P[0].h
		  + Scintillators[*sect].scints[*scint].P[1].h);
	*x = 0.5*(Scintillators[*sect].scints[*scint].P[1].v +
		  Scintillators[*sect].scints[*scint].P[2].v);
		  */

	*r = sqrt((*z)*(*z) + (*x)*(*x));
	break;
      }
      
    }
    return;
  }  /* in a scint */

/* see if we are on wire */
  

  for (j = 0; j < NUM_SUPERLAYER; j++) {
      theItem = viewdata->AllDCItems[*sect][j];
      Hv_CopyRect(&rect, theItem->area);
      if (Hv_PointInRect(pp, &rect)) {
	  *supl = j;

/* get info from the items user field */

	  rgn  = theItem->user2/2;
	  isupl = theItem->user2%2;  /* the 0-1 variety */
	  supptr = &(DriftChambers[*sect].regions[rgn].superlayers[isupl]);

	  for (k = 0; k < supptr->numlayers; k++) {
	      AllDCLayerRect(*sect, rgn, isupl, k, &rect, &lrect);
	      if (Hv_PointInRect(pp, &lrect)) {
		  *lay = k;
		  
		  layptr = supptr->layers + k;
		  for (w = layptr->first_wire; w <= layptr->last_wire; w++) {
		      AllDCWireRect(*sect, rgn, isupl, *lay, w, &sectrect, &lrect, &srect);
		      if (Hv_PointInRect(pp, &srect)) {
			  *wire = w;
			  return;
		      }
		  }

	      }
	  }


	  return;
      }
  }

}

/*------- DrawAllDCDeadWires ------------*/

void DrawAllDCDeadWires(Hv_View   View,
			short     sect,
			Hv_Region region) {

    short    rgn, supl;
    
    for (rgn = 0; rgn < 3; rgn++)
	for (supl = 0; supl < 2; supl++)
	    DrawAllDCDeadSuperLayerWires(View, sect, rgn, supl, region);
}


/*-------- DrawAllDCDeadSuperLayerWires ---------*/

static void DrawAllDCDeadSuperLayerWires(Hv_View   View,
					 short     sect,
					 short     rgn,     /*0..2*/
					 short     supl,    /*0..1*/
					 Hv_Region region)

{ 
    
    int              i;
    unsigned char    layer, wire;
    short            lay;
    ViewData         viewdata;
    short            fill, frame;
    Hv_Rect          *suplayrect;
    Hv_Rect          sr, layerrect, wirerect;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    Hv_Item          Item;


/*fprintf(stderr, "drawing dead wires for %d %d %d\n", sect+1, rgn+1, supl+1); */

/* first check, are there ANY in this supl */

    if (dcstatus[sect][rgn][supl] == NULL)
	return;

    viewdata = GetViewData(View);
    
    if (!(viewdata->displayDead))
	return;
    
    frame = dcframecolor;
    fill = dcdeadwirecolor;
    SectorGridSectorRect(View, sect, &sr);

    i = 0;

    if (Hv_RectIntersectsRect(&sr, View->hotrect))
	while(dcstatus[sect][rgn][supl][i].status != -99) {
    
	    BreakShort(dcstatus[sect][rgn][supl][i].id, &wire, &layer);
	
/* layer is 1..36 and must be converted */
	
	    wire--;
	    layer--;
	    
	    lay = layer % 6;  /* 0..5 variety */
	    
	    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
	    layerPtr = &(supptr->layers[lay]);
	    
	    if ((wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
		Item = viewdata->AllDCItems[sect][rgn*2 + supl];
		suplayrect = Item->area;
		AllDCLayerRect(sect, rgn, supl, lay, suplayrect, &layerrect);
		AllDCWireRect(sect, rgn, supl, lay, wire, &sr, &layerrect, &wirerect);
		Hv_FillRect(&wirerect, fill);
	    }
	    
	    
	    i++;
	}  /* end main while */

/* fix up the lines */

    skipDead = True;
    DrawAllDC(viewdata->AllDCItems[sect][2*rgn + supl], NULL);
    skipDead = False;
	

}


/*------- FixLines -------*/

static void FixLines(Hv_View  View,
		     short    sect,
		     Boolean *dirty) {

    int i;
    ViewData viewdata = GetViewData(View);

	    
    for (i = 0; i < 6; i++)
	if (dirty[i])
	    DrawAllDC(viewdata->AllDCItems[sect][i], NULL);

}

/*------- DrawAllDC1Hits ---------*/

void DrawAllDC1Hits(Hv_View     View,
		    short       sect,
		    int         numhits,
		    DC1DataPtr  hits,
		    Hv_Region   region) {

    int              i;
    unsigned char    layer, wire;
    short            rgn, lay, supl, superlayer;
    ViewData         viewdata;
    short            fill, frame;
    Boolean          dirty[6];
    Hv_Rect          *suplayrect;
    Hv_Rect          sr, layerrect, wirerect;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    Hv_Item          Item;


    viewdata = GetViewData(View);
    
    if (((hits == NULL) || (numhits < 1)) && (viewdata->showwhat == SINGLEEVENT))
	return;
    
    if (!(viewdata->displayDC))
	return;

    if ((viewdata->showwhat == ACCUMULATEDEVENTS) || (viewdata->showwhat == AVERAGETDC))
	return;


    if (viewdata->dc1control == JUSTTRACKS) {
	DrawAllDC1TrackHits(View, sect,
			    bosNumDC1[sect],
			    bosNumHBLA[sect],
			    hits, bosHBLA[sect], region);
	return;
    }

    
    frame = dcframecolor;
    fill = dcfromdc1color;
    
    SectorGridSectorRect(View, sect, &sr);
	
    if (Hv_RectIntersectsRect(&sr, View->hotrect)) 
	if ((region == NULL) || (Hv_RectInRegion(&sr, region))) {
	    
/* set all superlayers to clean (single event drawing) */
	    
	    for (i = 0; i < 6; i++)
		dirty[i] = False;
	    
	    for (i = 0; i < numhits; i++) {
		
		if (checkDC1Data(sect, hits)) {
		    BreakShort((short)(hits->id), &wire, &layer);
		    
/* layer is 1..36 and must be converted */
		    
		    wire--;
		    layer--;
		    rgn = layer / 12;
		    superlayer = layer / 6;   /* the 0..5 variety */
		    dirty[superlayer] = True;
		    
		    supl = (layer % 12) / 6;  /* the 0..1 variety */
		    lay = layer % 6;
		    
		    
		    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
		    layerPtr = &(supptr->layers[lay]);
		    
		    if ((wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
			Item = viewdata->AllDCItems[sect][rgn*2 + supl];
			suplayrect = Item->area;
			AllDCLayerRect(sect, rgn, supl, lay, suplayrect, &layerrect);
			AllDCWireRect(sect, rgn, supl, lay, wire, &sr, &layerrect, &wirerect);
			Hv_FillRect(&wirerect, fill);
		    }
		}
		hits++;
	    }
	    
	    FixLines(View, sect, dirty);
	    
	}
    
}


/*------- DrawAllDC1TrackHits ---------*/

static void DrawAllDC1TrackHits(Hv_View     View,
				short       sect,
				int         numhits,
				int         numtracks,
				DC1DataPtr  hits,
				HBLADataPtr  hbla,
				Hv_Region   region) {


    DC1DataPtr hit;
    int i, j;
    int dc1tag;
    unsigned char    layer, wire;
    short            rgn, lay, supl, superlayer;
    ViewData         viewdata;
    short            fill, frame;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    Hv_Rect          *suplayrect;
    Hv_Rect          sr, layerrect, wirerect;
    Boolean          dirty[6];
    Hv_Item          Item;


    if ((numhits == 0) || (hits == NULL) || (numtracks == 0) || (hbla == NULL))
	return;

    viewdata = GetViewData(View);

    if (!(viewdata->displayDC1))
	return;
    
    frame = dcframecolor;
    
/* regular sector view */
    
    fill = dcfromdc1color;

    frame = dcframecolor;
    
    SectorGridSectorRect(View, sect, &sr);
	
    if (Hv_RectIntersectsRect(&sr, View->hotrect)) 
	if ((region == NULL) || (Hv_RectInRegion(&sr, region))) {
	    
/* set all superlayers to clean (single event drawing) */
	    
	    for (i = 0; i < 6; i++)
		dirty[i] = False;


	    for (j = 0; j < numtracks; j++) {

		dc1tag = hbla->dc1;
		
		if ((dc1tag > 0) && (dc1tag <= numhits)) {
		    hit = hits + (dc1tag-1); /* careful, tag is not zero based */
		    
		    if (checkDC1Data(sect, hit)) {
			BreakShort((short)(hit->id), &wire, &layer);
	
/* layer is 1..36 and must be converted */
	
			wire--;
			layer--;
			
			rgn = layer / 12;
			superlayer = layer / 6;   /* the 0..5 variety */
			dirty[superlayer] = True;
			
			supl = (layer % 12) / 6;  /* the 0..1 variety */
			lay = layer % 6;
			
			
			supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
			layerPtr = &(supptr->layers[lay]);
			
			if ((wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
			    Item = viewdata->AllDCItems[sect][rgn*2 + supl];
			    suplayrect = Item->area;
			    AllDCLayerRect(sect, rgn, supl, lay, suplayrect, &layerrect);
			    AllDCWireRect(sect, rgn, supl, lay, wire, &sr, &layerrect, &wirerect);


		    Hv_FillRect(&wirerect, fill);
			}
			
			
		    }
		    
		    
		}
		
		else {
	    if (dc1tag != 0)
		fprintf(stderr, "Bad HBLA DC1 tag: %d legal range is [1, %d]\n", dc1tag, numhits);
		}

		hbla++;
	    }
	}
    

    FixLines(View, sect, dirty);
}


/*------- DrawAllDCHits ---------*/

void DrawAllDCHits(Hv_View     View,
		   short       sect,
		   int         numhits,
		   char       *chits,
		   size_t      size,
		   Hv_Region   region) {
    
/* works for both DC0 (the new banks) and DC (the old) */

    DC0DataPtr       hits;
    int              i;
    unsigned char    layer, wire;
    short            rgn, lay, supl, superlayer;
    ViewData         viewdata;
    short            fill, frame;
    Boolean          dirty[6];
    Hv_Rect          *suplayrect;
    Hv_Rect          sr, layerrect, wirerect;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    Hv_Item          Item;


    viewdata = GetViewData(View);
    
    if (((chits == NULL) || (numhits < 1)) && (viewdata->showwhat == SINGLEEVENT))
	return;
    
    if (!(viewdata->displayDC))
	return;

    if ((viewdata->showwhat == ACCUMULATEDEVENTS) || (viewdata->showwhat == AVERAGETDC)) {
	DrawAccumulatedHits(View,
			    sect,
			    numhits,
			    chits,
			    size,
			    region);
	return;
    }
    
    frame = dcframecolor;
    
    SectorGridSectorRect(View, sect, &sr);
	
    if (Hv_RectIntersectsRect(&sr, View->hotrect)) 
	if ((region == NULL) || (Hv_RectInRegion(&sr, region))) {
	    
/* set all superlayers to clean (single event drawing) */
	    
	    for (i = 0; i < 6; i++)
		dirty[i] = False;
	    
	    for (i = 0; i < numhits; i++) {
		hits = (DC0DataPtr)chits;
		
		if (hits->tdc > 32000)
		    fill  = dcaddedbytrigcolor;
		else 
		    fill  = dchitcolor;
		
		if (checkDCData(sect, hits)) {
		    BreakShort(hits->id, &wire, &layer);
		    
/* layer is 1..36 and must be converted */
		    
		    wire--;
		    layer--;
		    rgn = layer / 12;
		    superlayer = layer / 6;   /* the 0..5 variety */
		    dirty[superlayer] = True;
		    
		    supl = (layer % 12) / 6;  /* the 0..1 variety */
		    lay = layer % 6;
		    
/* see if a noise hit */
		    
I 3
			/*Sergey: temporary
E 3
		    if (reducenoise) {
			if (!CheckBitHugeWord(cleandcbitdata[sect][rgn][supl].data[lay], wire))
			    if (viewdata->suppressNoise)
				fill = -1;
			    else
				fill = dcnoisecolor;
		    }
I 3
		    */
E 3
		    
D 3
		    
E 3
/* see if out of time */
		    
		    if (!GoodDCTDCValue(rgn, hits->tdc))
			fill = dcbadtimecolor;
		    
		    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
		    layerPtr = &(supptr->layers[lay]);
		    
		    if ((wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
			Item = viewdata->AllDCItems[sect][rgn*2 + supl];
			suplayrect = Item->area;
			AllDCLayerRect(sect, rgn, supl, lay, suplayrect, &layerrect);
			AllDCWireRect(sect, rgn, supl, lay, wire, &sr, &layerrect, &wirerect);
			Hv_FillRect(&wirerect, fill);
		    }
		}
		chits += size;
	    }
	    
/* fix up the lines */

	    FixLines(View, sect, dirty);
	    
	}
    
}

/*------- DrawAccumulatedDCHits ---------*/

D 4
static void DrawAccumulatedHits(Hv_View     View,
				short       sect,
				int         numhits,
				char       *chits,
				size_t      size,
				Hv_Region   region) {
E 4
I 4
static void
DrawAccumulatedHits(Hv_View     View,
                    short       sect,
                    int         numhits,
                    char       *chits,
                    size_t      size,
                    Hv_Region   region)
{
E 4
    
/* works for both DC0 (the new banks) and DC (the old) */

    int              i;
    unsigned char    layer, wire;
    short            rgn, lay, supl;
    ViewData         viewdata;
    short            acc, frame;
    Hv_Rect          *suplayrect;
    Hv_Rect          sr, layerrect, wirerect;
    int              layer0, numwire, numlay;
    Hv_Item          Item;
    int              val;
    short            mincolor, maxcolor, underflow, overflow;

    Hv_RainbowData *data;
    short                  occurs[Hv_COLORS];    /*how many times each color occurs*/
    XRectangle             *rects[Hv_COLORS];    /*color point for each color (for each color)*/
D 4
    
E 4

    underflow = Hv_black;
    overflow = Hv_gray9;

    
    viewdata = GetViewData(View);
    
    if (((chits == NULL) || (numhits < 1)) && (viewdata->showwhat == SINGLEEVENT))
	return;
    
    if (!(viewdata->displayDC))
	return;
    
    frame = dcframecolor;
    
    SectorGridSectorRect(View, sect, &sr);
	
    if (!(Hv_RectIntersectsRect(&sr, View->hotrect)))
	return;
	    
/* accumulated hits or avg tdc display */
	    
    for (rgn = 0; rgn < 3; rgn++) {
	
	if (viewdata->showwhat == ACCUMULATEDEVENTS)
	    data = (Hv_RainbowData *)(viewdata->AccumulatedHitsItem[0]->data);
	else
	    data = (Hv_RainbowData *)(viewdata->AverageTDCItem[rgn]->data);

/* get the min and max colors for the scale */

	mincolor = Hv_sMin(data->mincolor, data->maxcolor);
	maxcolor = Hv_sMax(data->mincolor, data->maxcolor);

/* init occurs array, which will keep track of how many times each color appears. */

D 4
	for (i = mincolor; i <= maxcolor; i++) {
	    occurs[i] = 0;
	    rects[i] = (XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle)); 
	}
/*	rects[underflow] = (XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle)); */
	rects[overflow]  = (XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle)); 
E 4
I 4
  for(i=mincolor; i<=maxcolor; i++)
  {
    occurs[i] = 0;
    rects[i] = (XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle)); 
  }
E 4

D 4
/*	occurs[underflow] = 0; */   /* underflow */
	occurs[overflow] = 0;     /* overflow */
E 4
I 4
/*rects[underflow] =(XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle));*/ 
  rects[overflow]  = (XRectangle *)Hv_Malloc(MAXPNTSINREQ*sizeof(XRectangle)); 

/*occurs[underflow] = 0; */   /* underflow */
  occurs[overflow] = 0;     /* overflow */
E 4
	
D 4
	for (supl = 0; supl < 2; supl++) {
	    
	    Item = viewdata->AllDCItems[sect][rgn*2 + supl];
E 4
I 4
  for(supl=0; supl<2; supl++)
  {    
    Item = viewdata->AllDCItems[sect][rgn*2 + supl];
E 4
	    suplayrect = Item->area;
	    
	    if (!Hv_RectIntersectsRect(suplayrect, View->hotrect)) {
		break;
	    }

/* twist in 20024, fill with underflow & do not accumulate them */

	    Hv_FillRect(suplayrect, underflow);

	    layer0 = 6*supl;
		    
	    numlay = DCNumLayers(sect, rgn, supl);
	    for (layer = 0; layer < numlay; layer++) {
		lay  = layer0 + layer;
		AllDCLayerRect(sect, rgn, supl, layer, suplayrect, &layerrect);
		numwire = DCNumWires(sect, rgn, supl, layer);
		for (wire = 0; wire < numwire; wire++) {

/* val will first be counts */

		    val = accdc[sect][rgn].counts[lay][wire];

/* calculate avg tdc from sum, if theats the mode we are in */
		    
		    if (viewdata->showwhat == AVERAGETDC) {
			if (val > 0)
			    val = (int)(accdc[sect][rgn].tdcsum[lay][wire]/val);
			
		    }
			    
			    
		    if (val <= data->valmin)
			acc = underflow;
		    else if (val >= data->valmax)
			acc = overflow;
		    else
			acc = CountToColor(val, data, viewdata->algorithm);
		    
		    AllDCWireRect(sect, rgn, supl, layer, wire,
				  &sr, &layerrect, &wirerect);

		    if (acc != underflow) {
			Hv_SetXRectangle(rects[acc] + occurs[acc],
					 wirerect.left,
					 wirerect.top,
					 wirerect.width,
					 wirerect.height);
			
			occurs[acc]++;
		    }

/*		    Hv_FillRect(&wirerect, acc); */

/* dump rects   if we've reached the limit */
	
		    if (occurs[acc] == MAXPNTSINREQ)
			DumpCRectangles(acc, occurs, rects[acc]);


		} /* end wire loop */
	    } /* end layer loop */
	} /* end supl loop */

      
/* now a final dump to get any stragglers */


D 4
	for(i = mincolor; i <= maxcolor; i++)
	    if (occurs[i] > 0)
		DumpCRectangles(i, occurs, rects[i]); 
/*	if (occurs[underflow] > 0)
	    DumpCRectangles(underflow, occurs, rects[underflow]);  */
	if (occurs[overflow] > 0)
	    DumpCRectangles(overflow,  occurs, rects[overflow]);
E 4
I 4
	for(i=mincolor; i<=maxcolor; i++)
      if(occurs[i] > 0)
        DumpCRectangles(i, occurs, rects[i]); 
    /*if(occurs[underflow] > 0)
      DumpCRectangles(underflow, occurs, rects[underflow]);  */
    if(occurs[overflow] > 0)
      DumpCRectangles(overflow,  occurs, rects[overflow]);
E 4

D 4
/* free the rect arrays */
	     
	for(i = mincolor+1; i <= maxcolor; i++)
	    Hv_Free(rects[i]);
/*	Hv_Free(rects[underflow]);*/
	Hv_Free(rects[overflow]);
	
    } /* end rgn loop */
E 4
I 4
    /* free the rect arrays */

/* Sergey: memory leak was here (+1) !!!??? */
    for(i=mincolor/*+1*/; i<=maxcolor; i++) Hv_Free(rects[i]);

    /*Hv_Free(rects[underflow]);*/
    Hv_Free(rects[overflow]);

  } /* end rgn loop */
E 4
}

/*-------- MallocSectorGrid -----------*/

Hv_Item MallocSectorGrid(Hv_View View)

{
  Hv_Item  Item;

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_USERITEM,
			 Hv_TAG,          SECTORGRIDITEM,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND + Hv_ALWAYSREDRAW,
			 Hv_USERDRAW,     DrawSectorGridItem,
			 NULL);

  return Item;
}

/*--------- DrawSectorGridItem -------------*/

static void DrawSectorGridItem(Hv_Item Item,
			       Hv_Region region)

{
  Hv_View     View  = Item->view;
  int         sect;
  Hv_Rect     rect;
  Hv_String  *hvstr;
  short       sw, sh;
  char        tstr[10];

/* make a test string */

  hvstr = Hv_CreateString("Sector X");
  hvstr->font = Hv_helveticaBold11;
  Hv_StringDimension(hvstr, &sw, &sh);
  Hv_DestroyString(hvstr);

  for (sect = 0; sect < 6; sect++) {
    SectorGridSectorRect(View, sect, &rect);
    Hv_ShrinkRect(&rect, 1, 1);
    Hv_Simple3DRect(&rect, False, -1);

/* label */

    sprintf(tstr, "Sector %-1d", sect+1);
    if (sect < 3)
      DrawLabel(rect.left+8, rect.top+2,        tstr, region);
    else
      DrawLabel(rect.left+8, rect.bottom-3-sh,  tstr, region);
      

  }

}


/*------ DrawLabel --------*/

static void DrawLabel(short x,
		      short y,
		      char *str,
		      Hv_Region region)

{
  Hv_String          *hvstr;
  short               w, h;
  Hv_Rect             rect;

  hvstr = Hv_CreateString(str);
  hvstr->font = Hv_helveticaBold11;

  Hv_StringDimension(hvstr, &w, &h);
  Hv_SetRect(&rect, x-5, y-1, w+10, h+2);

  Hv_Simple3DRect(&rect, True, Hv_gray12);

  Hv_StringDraw(x, y, hvstr, region);
  Hv_DestroyString(hvstr);
}

/*------ MallocAllDC ---------*/

Hv_Item MallocAllDC(Hv_View View,
		    short   sect,
		    short   superlayer)

{
  Hv_Item             Item = NULL;
  Hv_FRect            frect;
  Hv_WorldRectData   *wrect;

  AllDCSuperLayerFRect(View, sect, superlayer/2, superlayer%2, &frect);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDRECTITEM,
			 Hv_USER1,        sect,
			 Hv_USER2,        superlayer,
			 Hv_XMIN,         frect.xmin,
			 Hv_XMAX,         frect.xmax,
			 Hv_YMIN,         frect.ymin,
			 Hv_YMAX,         frect.ymax,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditAllDCItem,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_USERDRAW,     DrawAllDC,
			 NULL);
  
/*  Item->doubleclick = NULL;*/
  wrect = (Hv_WorldRectData *)(Item->data);
  wrect->grid = False;
  wrect->frame = False;
  wrect->fillcolor = Hv_white;

  return Item;
}



/*---------- EditAllDCItem -----------*/

static void  EditAllDCItem(Hv_Event hvevent)
    
{
    EditDCData();
}


/*-------- MallocAllDCScint ---------*/

Hv_Item MallocAllDCScint(Hv_View View,
			 short   sect)
     
{
  Hv_Item             Item = NULL;
  Hv_FRect            frect;
  Hv_WorldRectData   *wrect;

  AllDCScintFRect(View, sect, &frect);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDRECTITEM,
			 Hv_USER1,        sect,
			 Hv_XMIN,         frect.xmin,
			 Hv_XMAX,         frect.xmax,
			 Hv_YMIN,         frect.ymin,
			 Hv_YMAX,         frect.ymax,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_USERDRAW,     DrawAllDCScint,
			 NULL);
  
  Item->doubleclick = NULL;
  wrect = (Hv_WorldRectData *)(Item->data);
  wrect->grid = False;
  wrect->frame = False;
  wrect->fillcolor = Hv_honeydew;

  return Item;
}

/*-------- DrawAllDCScint -------*/

static void DrawAllDCScint(Hv_Item   Item,
			  Hv_Region region)

{
  Hv_View   View = Item->view;
  int       i;
  Hv_Rect   rect;

  for (i = 0; i < (NUM_SCINT-1); i++) {
    AllDCScintSlabRect(View, Item->user1, (short)i, Item->area, &rect);
/*    Hv_DrawLine(rect.left, rect.top, rect.left, rect.bottom, Hv_white);*/
    Hv_DrawLine(rect.left+1, rect.top, rect.left+1, rect.bottom, Hv_black);
  }

  Hv_Simple3DRect(Item->area, False, -1);
}


/*-------- DrawAllDC -------*/

void DrawAllDC(Hv_Item   Item,
		      Hv_Region region)

{
  Hv_View           View = Item->view;
  int               i;
  Hv_Rect           rect;
  short             rgn, supl, sect;
  DC_Super_Layer   *supptr;

  sect = Item->user1;
  rgn  = Item->user2/2;
  supl = Item->user2%2;

  supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);

  if (sect < 3)
    for (i = 0; i < (supptr->numlayers-1); i++) {
      AllDCLayerRect(sect, rgn, supl, i, Item->area, &rect);
/*      Hv_DrawLine(rect.left, rect.top, rect.right, rect.top, Hv_white); */
      Hv_DrawLine(rect.left, rect.top-1, rect.right, rect.top-1, Hv_black);
    }
  else
    for (i = 1; i < supptr->numlayers; i++) {
      AllDCLayerRect(sect, rgn, supl, i, Item->area, &rect);
/*      Hv_DrawLine(rect.left, rect.top, rect.right, rect.top, Hv_black);
      Hv_DrawLine(rect.left, rect.top+1, rect.right, rect.top+1, Hv_white);*/
/*      Hv_DrawLine(rect.left, rect.top, rect.right, rect.top, Hv_white); */
      Hv_DrawLine(rect.left, rect.top+1, rect.right, rect.top+1, Hv_black);
    }


  if (!skipDead)
      DrawAllDCDeadSuperLayerWires(View, sect, rgn, supl, region);

  Hv_FrameRect(Item->area, Hv_black);
  Hv_CopyRect(&rect, Item->area);
  Hv_ShrinkRect(&rect, -1, -1);
  Hv_Simple3DRect(&rect, False, -1);
}


/* -------- AllDCXtoTheta ----------*/

static float  AllDCXtoTheta(short    x,
			    Hv_Rect *sectrect)


{
  return Hv_DEGTORAD*((ALLDC_MAXTHETA*(sectrect->right - x))/sectrect->width);
}

/* -------- AllDCThetatoX ----------*/

static short  AllDCThetatoX(float    theta,
			    Hv_Rect *sectrect)


{
  return ((short)(sectrect->right - (sectrect->width * (Hv_RADTODEG*theta) / ALLDC_MAXTHETA)));
}


/* ------- ScintThetaLimits ----- */

static void ScintThetaLimits(short sect,
			     float *thetamin,
			     float *thetamax)

/* sect is a C index */

/* returns the theta limits of the scints
   in a given sector */

{
  float  x1, z1, x2, z2;

  z1 = Scintillators[sect].scints[0].P[1].h;
  x1 = Scintillators[sect].scints[0].P[1].v;
  z2 = Scintillators[sect].scints[NUM_SCINT-1].P[0].h;
  x2 = Scintillators[sect].scints[NUM_SCINT-1].P[0].v;
  *thetamin = fabs(atan2(x1, z1));
  *thetamax = fabs(atan2(x2, z2));
/*  printf("[thetamin, thetamax] = [%f, %f]\n",
    Hv_RADTODEG*(*thetamin), Hv_RADTODEG*(*thetamax)); */
}


/* ------- ScintSlabTheta ---------*/

static float  ScintSlabTheta(short sect,
			     short scint)


/* sect, scint are C Indices. This returns
   the theta corresponding to the middle of
   the slab in RADIANS (midplane) */

{
  float  x1, z1, x2, z2;
  float  thetap, thetam;

  z1 = Scintillators[sect].scints[scint].P[1].h;
  x1 = Scintillators[sect].scints[scint].P[1].v;
  z2 = Scintillators[sect].scints[scint].P[0].h;
  x2 = Scintillators[sect].scints[scint].P[0].v;
  thetam = fabs(atan2(x1, z1));
  thetap = fabs(atan2(x2, z2));
  return (0.5*(thetam + thetap));
}

/* ------- ScintThetaSlabLimits ----- */

static void ScintThetaSlabLimits(short sect,
				 short scint,
				 float *thetamin,
				 float *thetamax)

{
  float  theta, thetap, thetam, dummy;

  theta = ScintSlabTheta(sect, scint);

  if (scint == 0) {
    ScintThetaLimits(sect, &thetam, &dummy);
    thetap = ScintSlabTheta(sect, 1);
    *thetamin = thetam;
    *thetamax = 0.5*(theta + thetap);
  }
  else if (scint == (NUM_SCINT - 1)) {
    thetam = ScintSlabTheta(sect, scint - 1);
    ScintThetaLimits(sect, &dummy, &thetap);
    *thetamin = 0.5*(theta + thetam);
    *thetamax = thetap;
  }
  else {
    thetam = ScintSlabTheta(sect, scint - 1);
    thetap = ScintSlabTheta(sect, scint + 1);
    *thetamin = 0.5*(theta + thetam);
    *thetamax = 0.5*(theta + thetap);
  }

/*  printf("[thetamin, thetamax] = [%f, %f]\n",
    Hv_RADTODEG*(*thetamin), Hv_RADTODEG*(*thetamax)); */
}


/* -------- AllDCScintRect ---------*/

static void AllDCScintRect(Hv_View   View,
			   short     sect,
			   Hv_Rect   *scintrect)

{
  float      thetamin, thetamax;
  short      w, xmin, xmax, ymin, ymax;
  Hv_Rect    sr;

  SectorGridSectorRect(View, sect, &sr);
  w = (short)((float)(sr.height)/(6.0 + ALLDC_S + 4.0*ALLDC_SMALLG + 3.0*ALLDC_BIGG));

  ScintThetaLimits(sect, &thetamin, &thetamax);
  xmax = AllDCThetatoX(thetamin, &sr);
  xmin = AllDCThetatoX(thetamax, &sr);

  if (sect < 3) {
    ymin = sr.top + w*(ALLDC_BIGG+ALLDC_SMALLG);
    ymax = ymin + w*ALLDC_S;
  }
  else {
    ymax = sr.bottom - w*(ALLDC_BIGG+ALLDC_SMALLG);
    ymin = ymax - w*ALLDC_S;
  }

  Hv_SetRect(scintrect, xmin, ymin, xmax-xmin, ymax-ymin);
  
}

/*------- AllDCScintFRect --------*/

static void AllDCScintFRect(Hv_View   View,
			    short     sect,
			    Hv_FRect *scintrect)

{
  Hv_Rect   rect;
  
  AllDCScintRect(View, sect, &rect);
  Hv_LocalRectToWorldRect(View, &rect, scintrect);

}


/* ------- AllDCScintSlabRect -------*/

void  AllDCScintSlabRect(Hv_View View,
			 short   sect,
			 short   scint,
			 Hv_Rect *scintrect,
			 Hv_Rect *slabrect)

{
  Hv_Rect    sr;
  float      thetamin, thetamax;
  short      xmin, xmax;

  SectorGridSectorRect(View, sect, &sr);
  ScintThetaSlabLimits(sect, scint, &thetamin, &thetamax);

  xmax = AllDCThetatoX(thetamin, &sr);
  xmin = AllDCThetatoX(thetamax, &sr);

  Hv_SetRect(slabrect, xmin, scintrect->top, xmax-xmin, scintrect->height);
}


/* ------- SectorGridSectorRect ------- */

void SectorGridSectorRect(Hv_View View,
			  short   sect,
			  Hv_Rect *r)

/* for given view and sect (C index), obtain corresponding
   sector rect for all dc view */

{
  short        row, col;
  float        w, h, xmin, ymin;
  Hv_FRect     fr;


/* get row and col (C indices) for this sector */

  row = sect / 3;
  col = sect % 3;

  w = 2.0*ALLDCWWIDTH2/3.0;
  h = ALLDCWHEIGHT2;

  xmin = -ALLDCWWIDTH2 + col*w;
  ymin = ALLDCWHEIGHT2 - (row+1)*h;
  
  Hv_SetFRect(&fr, xmin, xmin+w, ymin, ymin+h);
  Hv_WorldRectToLocalRect(View, r, &fr);
}


/* -------- AllDCSuperLayerRect -------*/

void  AllDCSuperLayerRect(Hv_View View,
			  short   sect,
			  short   rgn,
			  short   supl,
			  Hv_Rect *suplrect)

/* all c indices with supl = 0 or 1 */

{
  short            xmin, xmax, ymin, ymax;
  float            ThetaMin, ThetaMax, ThetaMin2, ThetaMax2, dtheta;
  short            numlay, firstwire, lastwire;
  DC_Super_Layer   *supptr;
  DC_Layer         *layptrinner, *layptrouter;

  float            fx, fz;
  Hv_Rect          sr;

  supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
  numlay = supptr->numlayers - 1;

  layptrinner = supptr->layers;
  layptrouter = supptr->layers + numlay;

  dtheta  = 2.0*fabs(supptr->dtheta);

  lastwire  = layptrouter->last_wire;
  firstwire = layptrouter->first_wire;

  SenseWirePosition(0.0, sect, rgn, supl, numlay, firstwire, &fz, &fx);
  ThetaMin = fabs(atan2(fx, fz)) - dtheta;
  SenseWirePosition(0.0, sect, rgn, supl, numlay, lastwire, &fz, &fx);
  ThetaMax = fabs(atan2(fx, fz)) + dtheta;

  lastwire = layptrinner->last_wire;
  firstwire = layptrinner->first_wire;

  SenseWirePosition(0.0, sect, rgn, supl, 0, firstwire, &fz, &fx);
  ThetaMin2 = fabs(atan2(fx, fz)) - dtheta;
  SenseWirePosition(0.0, sect, rgn, supl, 0, lastwire, &fz, &fx);
  ThetaMax2 = fabs(atan2(fx, fz)) + dtheta;

  ThetaMin = Hv_fMin(ThetaMin, ThetaMin2);
  ThetaMax = Hv_fMax(ThetaMax, ThetaMax2);

  SectorGridSectorRect(View, sect, &sr);

  xmin =  AllDCThetatoX(ThetaMax, &sr);
  xmax =  AllDCThetatoX(ThetaMin, &sr);

  AllDCSuperLayerYLimits(&sr, sect, rgn, supl, numlay, &ymin, &ymax);

  Hv_SetRect(suplrect, xmin, ymin, xmax-xmin, ymax-ymin);
}

/*---------- AllDCSuperLayerFRect -----------*/

static void  AllDCSuperLayerFRect(Hv_View View,
				 short    sect,
				 short    rgn,
				 short    supl,
				 Hv_FRect *suplrect)

{
  Hv_Rect     rect;

  AllDCSuperLayerRect(View, sect, rgn, supl, &rect);
  Hv_LocalRectToWorldRect(View, &rect, suplrect);
}



/* ------- AllDCSuperLayerYLimits ----- */

static void AllDCSuperLayerYLimits(Hv_Rect *sr,
				   short   sect,
				   short   rgn,
				   short   supl,
				   short   numlay,
				   short   *ymin,
				   short   *ymax)

{
  short w;

  w = (short)((float)(sr->height)/(6.0 + ALLDC_S + 4.0*ALLDC_SMALLG + 3.0*ALLDC_BIGG));

  if (sect < 3) {
    *ymax = sr->bottom - (short)((float)w*(ALLDC_SMALLG +
					   rgn*ALLDC_BIGG + supl*ALLDC_SMALLG + (2.0*rgn + supl)));
    *ymin = *ymax - w;
    
    if (numlay < 5)
      *ymax = *ymin + ((numlay+1)*w)/6;
  }
  else {
    *ymin = sr->top + (short)((float)w*(ALLDC_SMALLG +
					   rgn*ALLDC_BIGG + supl*ALLDC_SMALLG + (2.0*rgn + supl)));
    *ymax = *ymin + w;
    
    if (numlay < 5)
      *ymin = *ymax - ((numlay+1)*w)/6;
  }
}


/* -------- AllDCLayerRect -------*/

void  AllDCLayerRect(short   sect,
			    short   rgn,
			    short   supl,
			    short   layer,
			    Hv_Rect *suplrect,
			    Hv_Rect *layerrect)

{
  short            numlay;
  DC_Super_Layer   *supptr;
  short            dw, yo, ymin, ymax;

  supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
  numlay = supptr->numlayers;

/* start with the superlayer rect */

  dw = suplrect->height - 1;

  if (sect < 3) {
    yo = suplrect->bottom-1;  /*bottom of superlayer rect */
    ymax = yo - (layer*dw)/numlay;
    ymin = yo - ((layer+1)*dw)/numlay;
  }
  else {
    yo = suplrect->top;  /*top of superlayer rect */
    ymin = yo + (layer*dw)/numlay;
    ymax = yo + ((layer+1)*dw)/numlay;
  }

  Hv_SetRect(layerrect, suplrect->left + 1, ymin, suplrect->width - 2, ymax-ymin);
}


/* -------- AllDCWireRect -------*/

void  AllDCWireRect(short   sect,
		    short   rgn,
		    short   supl,
		    short   layer,
		    short   wire,
		    Hv_Rect *sr,
		    Hv_Rect *layerrect,
		    Hv_Rect *wirerect)

/* sr is the sector rect not the superlayer rect */

{
  DC_Super_Layer   *supptr;
  DC_Layer         *layptr;
  short            numwires;
  short            dh;
  float            fx, fz, theta;
  short            xmin, xmax;

  supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
  layptr = supptr->layers + layer;
  numwires = layptr->num_wires;

/* start with the layer rect */

  dh = ((short)(layerrect->width))/numwires;

  SenseWirePosition(0.0, sect, rgn, supl, layer, wire, &fz, &fx);
  theta = fabs(atan2(fx, fz));

  xmin = AllDCThetatoX(theta, sr) - (dh/2);
  xmax = xmin + dh + 1;

  wirerect->left = xmin;
  wirerect->width = xmax - xmin;
  Hv_SetRect(wirerect, xmin, layerrect->top, xmax-xmin+1, layerrect->height);
/*  Hv_SetRect(wirerect, xmin, layerrect->top, xmax-xmin, layerrect->height-1);*/
}


#undef    WIRESEARCHLEN
#undef    ALLDC_MAXTHETA
#undef    ALLDC_BIGG
#undef    ALLDC_SMALLG
#undef    ALLDC_S

#undef MAXPNTSINREQ
E 1
