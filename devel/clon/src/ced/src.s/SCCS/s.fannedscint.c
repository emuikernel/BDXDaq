h45081
s 00035/00012/00775
d D 1.3 07/11/12 16:41:05 heddle 4 3
c new start counter
e
s 00003/00003/00784
d D 1.2 07/10/26 10:02:44 heddle 3 1
c added more scints
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/fannedscint.c
e
s 00787/00000/00000
d D 1.1 02/09/09 16:27:08 boiarino 1 0
c date and time created 02/09/09 16:27:08 by boiarino
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
-  File:    fannedshower.c				
-							
-  Summary:						
-           Shower Counter Drawing 
-  
-  The routines here are specific the "fanned"
-  shower view.
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

#define  SCINTOFFSET 225.0
#define  LENSCALE    1.55

#define  NSCINTP    16   /*4 * num_scint */

#define  SCINTMARGIN   100.0
#define  MARGINMIN     10.0
#define  PEAKMARGIN    60.0

/* ------ local prototypes -------*/


static void DrawAccumulatedHits(Hv_View   View,
				short     sect,
				Hv_Region region);

static short  ADCToColor(Hv_View  View,
			 short    adc);

static float  ADCToDel(Hv_View  View,
			short    adc);


static void GetDel1Del2(Hv_View View,
			unsigned short adc,
			float *del1,
			float *del2);


static void SpecialFrame(short sect,
			 Hv_Point  *poly,
			 short     fcolor,
			 short     hcolor,
			 Boolean   hiliteonly);

static void GetScintFPoly(short         sect,
			  unsigned      short  id,
			  Hv_FPoint    *fp,
			  float         margin,
			  float         del1,
			  float         del2,
			  Boolean       left);

static void GetScintCorners(Hv_View       View,
			    short         sect,
			    unsigned      short  id,
			    Hv_Point     *points);


static void ActiveScintPoly(Hv_Item Item, Hv_Point *poly);

static void DrawFannedScintItem(Hv_Item Item,
				Hv_Region region);

static void GetFannedScintPoints(short       sect,
				 Hv_FPoint  *fp,
				 float       margin);


static void FrameScints(Hv_View  View,
			short   sect,
			short   color);


Boolean BordersOnly = False;


/*------- GetScintViewHit ---------*/

void GetScintViewHit(short sect, 
		     short scint,
		     short *tdcl,
		     short *adcl, 
		     short *tdcr,
		     short *adcr) {

/*
 *  scint is NOT a c index
 */

    int          i;
    SCDataPtr    hits;
    short        slab;

    *tdcl = -1;
    *adcl = -1;
    *tdcr = -1;
    *adcr = -1;

    if (bosIndexSC[sect] == 0)
	return;

    hits = bosSC[sect];
    for (i = 0; i < bosNumSC[sect]; i++) {
	slab = hits->id; 


	if (slab == scint) {
	    *tdcl = hits->tdcleft;
	    *adcl = hits->adcleft;
	    *tdcr = hits->tdcright;
	    *adcr = hits->adcright;
	    return;
	}
	hits++;
    }
}


/* -------- DrawStartHits ----------*/

void DrawStartHits(Hv_View View,
		   Hv_Region region) {

D 4
    STDataPtr  hits;
E 4
I 4
    STN0DataPtr  hitsN0;
    STN1DataPtr  hitsN1;
E 4

    Hv_Point   poly1[3];  /* inner (TDC) */
    Hv_Point   poly2[4];  /* outer (ADC) */
I 4
    Hv_Point subpoly1[4][3];
    Hv_Point subpoly2[4][4];

E 4
    int        i;
D 4
    short      sect;
E 4
I 4
    short      sect, sc_id;
E 4
    ViewData   viewdata = GetViewData(View);

    if (viewdata->showwhat != SINGLEEVENT) {
	return;
    }

D 4
    if (bosIndexST == 0)
E 4
I 4
    if ((bosIndexSTN0 == 0) && (bosIndexSTN1 == 0)) {
E 4
	return;
I 4
    }
E 4

D 4
    hits = bosST;
    for (i = 0; i < bosNumST; i++) {
	sect = (short)(hits->id - 1);
	StartCounterSubPoly(View, sect, poly1, poly2);
	if (hits->tdc > 0)
	    Hv_FillPolygon(poly1, 3, True, Hv_red, Hv_black);
	if (hits->adc > 0)
	    Hv_FillPolygon(poly2, 4, True, Hv_red, Hv_black);
	hits++;
E 4
I 4
    hitsN0 = bosSTN0;
    hitsN1 = bosSTN1;

    for (i = 0; i < bosNumSTN0; i++) {
      sc_id = (short)(hitsN0->id-1);
      sect = sc_id/4;
      StartCounterSubPoly(View, sect, poly1, poly2);
      splitStartCounterPoly1(poly1, subpoly1);
      
      if (hitsN0->tdc > 0) {
	Hv_FillPolygon(subpoly1[sc_id % 4], 3, True, Hv_red, Hv_black);
      }
      hitsN0++;
E 4
    }

I 4

    for (i = 0; i < bosNumSTN1; i++) {
      sc_id = (short)(hitsN1->id-1);
      sect = sc_id/4;
      StartCounterSubPoly(View, sect, poly1, poly2);
      splitStartCounterPoly2(poly2, subpoly2);
      
      if (hitsN1->adc > 0) {
	Hv_FillPolygon(subpoly2[sc_id % 4], 4, True, Hv_red, Hv_black);
      }
      hitsN1++;
    }


E 4
}


/*--------- DrawScintViewHits ----------*/

void DrawScintViewHits(Hv_View   View,
		       short     sect,
		       Hv_Region region) {

  ViewData     viewdata;
  short        fillcolor, adccolor;
  int          i;
  SCDataPtr    hits;
  short        slab;
  Hv_FPoint    fpoly[4];
  Hv_Point     poly[4];
  float        del1, del2;
  short        adcl, adcr;


  viewdata = GetViewData(View);

/* see if we are actually drawing the accumulated hits */

    if (viewdata->showwhat != SINGLEEVENT) {
	DrawAccumulatedHits(View, sect, region);
	return;
    }

    if (bosIndexSC[sect] == 0)
	return;

    fillcolor = viewdata->fannedscinthitcolor;

    hits = bosSC[sect];
    for (i = 0; i < bosNumSC[sect]; i++) {
      slab = hits->id - 1; /* convert to C Index */

/* check on validity of slab */

      if ((slab < NUM_SCINT) && (slab >= 0)) {
	GetScintCorners(View, sect, slab, poly);
	Hv_FillPolygon(poly, 4, True, fillcolor, viewdata->fannedscintstripcolor);

	adcl = hits->adcleft;
	adcr = hits->adcright;

/* now draw the extended color histo hit */

/* first left */

	GetDel1Del2(View, adcl, &del1, &del2);

	if ((del2 - del1) > 1.0) {
	    adccolor = ADCToColor(View, adcl);
	    GetScintFPoly(sect, slab, fpoly, 0.0, del1, del2, True);
	    Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpoly);
	    Hv_FillPolygon(poly, 4, True, adccolor, Hv_gray8);
	}

/* then right */	

	GetDel1Del2(View, adcr, &del1, &del2);

	if ((del2 - del1) > 1.0) {
	    adccolor = ADCToColor(View, adcr);
	    GetScintFPoly(sect, slab, fpoly, 0.0, del1, del2, False);
	    Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpoly);
	    Hv_FillPolygon(poly, 4, True, adccolor, Hv_gray8);
	}

      }
      hits++;
    }

/* reframe the scints */

}


/*----------- DrawAccumulatedHits ----------*/

static void DrawAccumulatedHits(Hv_View   View,
				short     sect,
				Hv_Region region) {



    int              i;
    int              val;
    short            mincolor, maxcolor, underflow, overflow, acc;
    Hv_RainbowData  *data;
    ViewData         viewdata = GetViewData(View);
    Hv_Point         poly[4];

/* are we showing counts or avg TDC? */

    if (viewdata->showwhat == ACCUMULATEDEVENTS)
	data = (Hv_RainbowData *)(viewdata->AccumulatedHitsItem[0]->data);
    else
	return;


/* get the min and max colors for the scale */

    mincolor  = Hv_sMin(data->mincolor, data->maxcolor);
    maxcolor  = Hv_sMax(data->mincolor, data->maxcolor);
    underflow = Hv_black;
    overflow  = Hv_gray9;

/* loop over the slabs and draw mit the correct color */

D 3
    for (i = 0; i < 48; i++) {
E 3
I 3
    for (i = 0; i < NUM_SCINT; i++) {
E 3

/* val will first be counts */
	
	val = accsc[sect].counts[i];

	if (val <= data->valmin)
	    acc = underflow;
	else if (val >= data->valmax)
	    acc = overflow;
	else
	    acc = CountToColor(val, data, viewdata->algorithm);

	GetScintCorners(View, sect, i, poly);
	Hv_FillPolygon(poly, 4, False, acc, viewdata->fannedscintstripcolor);


/*	fprintf(stderr, "sect: %d  val:  %d   color: %d\n", sect+1, val, acc); */

    }


}



/*------------ GetScintCoordinates ----------*/

void GetScintCoordinates(Hv_View  View,
			 Hv_Point pp,
			 short    sect,
			 short    *id,
			 float    *x,
			 float    *y,
			 float    *z,
			 float    *r,
			 float    *theta,
			 float    *phi) {

  int             i;
  Hv_Point        poly[4];
  ViewData        viewdata;

  float          fx, fy, tt;
  Hv_FPoint      xy, fp, fpoly[4], *sfp;
  Scint_Package   sp;

  viewdata = GetViewData(View);

  *id = -1;

  for (i = 0; i < NUM_SCINT; i++) {
      GetScintCorners(View, sect, i, poly);
      if (Hv_PointInPolygon(pp, poly, 4)) {
	  *id = i+1;

/* get world coordinates of pp and rotate onto sector 1 */


	  Hv_LocalToWorld(View, &(fp.h), &(fp.v), pp.x, pp.y);

	  if (sect != 0) {
	      tt = (sect*60.0)*Hv_DEGTORAD;
	      Hv_RotateFPoint(tt, &fp);
	  }


/* get "fractions" along slab */

	  GetScintFPoly(0, i, fpoly, 0.0, 0.0, 0.0, True);
	  fx = (fpoly[0].h - fp.h)/(fpoly[0].h - fpoly[1].h);
	  fy = (fpoly[0].v - fp.v)/(fpoly[0].v - fpoly[3].v);

	  fx = Hv_fMax(0.0, Hv_fMin(1.0, fx));
	  fy = Hv_fMax(0.0, Hv_fMin(1.0, fy));

/*	  fprintf(stderr, "fp = (%f, %f)  fx = %f  fy = %f\n", fp.h, fp.v, fx, fy); */


	  sp  = Scintillators[0];
	  sfp = sp.scints[i].P;

	  *y = (0.5 - fy)*sp.scints[i].length;
	  *z = sfp[1].h + fx*(sfp[0].h - sfp[1].h);
	  *x = sfp[1].v + fx*(sfp[0].v - sfp[1].v);

/* rotate y and x back */

	  if (sect != 0) {
	      tt = (sect*60.0)*Hv_DEGTORAD;
	      xy.h = *x;
	      xy.v = *y;
	      Hv_RotateFPoint(tt, &xy);
	      *x = xy.h;
	      *y = xy.v;
	  }
	  
	  
	  *r = sqrt((*x * *x) + (*y * *y) + (*z * *z));
	  
	  *phi = Hv_RADTODEG*atan2(*y, *x);
	  if (*phi < 0.0)
	      *phi += 360.0;
	  

	  if (fabs(*z) < 0.1)
	      *theta = 0.0;
	  else
	      *theta = Hv_RADTODEG*(acos(((*z)/(*r))));


	  break;

      }
  }
  
}


/*------- MallocFannedScint ------------*/

Hv_Item MallocFannedScint(Hv_View  View,
			  short    sect, /*C Index*/
			  Hv_Item  parent)

{
  Hv_Item      Item;
  Hv_FPoint    *fpts;
  short        npts = NSCINTP;

  ViewData     viewdata;
  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));
  GetFannedScintPoints(sect, fpts, SCINTMARGIN);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_PARENT,       parent,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_COLOR,        viewdata->fannedscintbgcolor,
			 Hv_FILLCOLOR,    -1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_USERDRAW,     DrawFannedScintItem,
			 Hv_USER2,        (short)sect,
			 NULL);

  Item->singleclick = NULL;
  Item->doubleclick = NULL;
  return  Item;
}


/*---------- DrawFannedScintItem -------*/

static void DrawFannedScintItem(Hv_Item Item,
				Hv_Region region) {

    Hv_View               View = Item->view;
    ViewData              viewdata;
    Hv_WorldPolygonData  *wpoly;
    short                 sect;
    Hv_Point             *poly;
    Hv_Point              poly2[NSCINTP];
    Hv_Region             hrreg = NULL;
    Hv_Region             sreg = NULL;

    viewdata = GetViewData(View);

    wpoly = (Hv_WorldPolygonData *)(Item->data);
    if (wpoly->poly == NULL)
	return;

    sect = Item->user2;
    poly = wpoly->poly;

    Hv_FillPolygon(poly, NSCINTP, False, viewdata->fannedscintbgcolor, Hv_black);


/* poly2 will hold the active polygon */

    ActiveScintPoly(Item, poly2);
    sreg = Hv_CreateRegionFromPolygon(poly2, NSCINTP);
    
    hrreg = Hv_ClipHotRect(View, 1);

    if (!BordersOnly)
	Hv_FillPolygon(poly2, NSCINTP, True, viewdata->fannedscintcolor, Hv_gray12);
  

    SpecialFrame(sect, poly,  Hv_black,  Hv_gray12, False);
    SpecialFrame(sect, poly2, Hv_gray12, Hv_black,  False);

/* frame the scints ? */

    if (viewdata->displaySC) {
	FrameScints(View, sect, viewdata->fannedscintstripcolor);
	SpecialFrame(sect, poly2, Hv_gray12, Hv_black,  False);
    }
}

/*--------- SpecialFrame --------*/

static void SpecialFrame(short sect,
			 Hv_Point  *poly,
			 short     fcolor,
			 short     hcolor,
			 Boolean   hiliteonly) {

    if (!hiliteonly)
	Hv_FramePolygon(poly, NSCINTP, fcolor);

    switch (sect) {
    case 0:
	Hv_DrawLines(poly+1,  8, hcolor);
	break;

    case 1:
	Hv_DrawLines(poly+7,  7, hcolor);
	break;

    case 2:
	Hv_DrawLines(poly+8,  8, hcolor);
	break;

    case 3:
	Hv_DrawLines(poly+11,  5, hcolor);
	break;

    case 4:
	Hv_DrawLines(poly,  3, hcolor);
	break;

    case 5:
	Hv_DrawLines(poly,  8, hcolor);
	break;
    }
}



/*-------- GetFannedScintPoints ----------*/

static void GetFannedScintPoints(short       sect,
				 Hv_FPoint  *fp,
				 float       margin) {

/* note margin = 0 corresponds to the active area */

    float           xexp;
    float           theta;
    Hv_FPoint       sfp[4];
    
    xexp = 0.07*margin;

    GetScintFPoly(0, 0,  sfp, margin, 0.0, 0.0, True);
    fp[0].h    = sfp[0].h;  fp[0].v   = sfp[0].v;
    fp[15].h   = sfp[3].h;  fp[15].v  = sfp[3].v;


    GetScintFPoly(0, 22, sfp, margin, 0.0, 0.0, True);
    fp[1].h    = sfp[0].h;  fp[1].v   = sfp[0].v;
    fp[14].h   = sfp[3].h;  fp[14].v  = sfp[3].v;

    GetScintFPoly(0, 34, sfp, margin, 0.0, 0.0, True);
    fp[2].h    = sfp[0].h;  fp[2].v   = sfp[0].v;
    fp[13].h   = sfp[3].h;  fp[13].v  = sfp[3].v;
    fp[3].h    = sfp[1].h;  fp[3].v   = sfp[1].v;
    fp[12].h   = sfp[2].h;  fp[12].v  = sfp[2].v;

    GetScintFPoly(0, 38, sfp, margin, 0.0, 0.0, True);
    fp[4].h    = sfp[1].h;  fp[4].v   = sfp[1].v;
    fp[11].h   = sfp[2].h;  fp[11].v  = sfp[2].v;

    GetScintFPoly(0, 39, sfp, margin, 0.0, 0.0, True);
    fp[5].h    = sfp[1].h;  fp[5].v   = sfp[1].v;
    fp[10].h   = sfp[2].h;  fp[10].v  = sfp[2].v;

D 3
    GetScintFPoly(0, 42, sfp, margin, 0.0, 0.0, True);
E 3
I 3
    GetScintFPoly(0, 45, sfp, margin, 0.0, 0.0, True);
E 3
    fp[6].h    = sfp[1].h;  fp[6].v   = sfp[1].v;
    fp[9].h    = sfp[2].h;  fp[9].v   = sfp[2].v;

D 3
    GetScintFPoly(0, 47, sfp, margin, 0.0, 0.0, True);
E 3
I 3
    GetScintFPoly(0, 56, sfp, margin, 0.0, 0.0, True);
E 3
    fp[7].h    = sfp[1].h;  fp[7].v   = sfp[1].v;
    fp[8].h    = sfp[2].h;  fp[8].v   = sfp[2].v;


    fp[0].h   += xexp;
    fp[7].h   -= xexp;
    fp[8].h   -= xexp;
    fp[15].h  += xexp;


/* rotate to the proper sector */

    if (sect != 0) {
	theta = -(sect*60.0)*Hv_DEGTORAD;
	Hv_RotateFPoints(theta, fp, NSCINTP);
    }
    
}



/*------ ActiveScintPoly --------*/

static void ActiveScintPoly(Hv_Item Item, Hv_Point *poly)

{
    short     sect;
    Hv_FPoint fpts[NSCINTP];

    sect = Item->user2;
    GetFannedScintPoints(sect, fpts, 0.0);
    Hv_WorldPolygonToLocalPolygon(Item->view, NSCINTP, poly, fpts);

}


/*-------- FrameScints ----------*/

static void FrameScints(Hv_View  View,
			short   sect,
			short   color) {
    int i;
    Hv_Point              strippoly[4];
    Hv_Region             clipreg;
    
    clipreg = Hv_ClipHotRect(View, 1);
    
    for (i = 0; i < NUM_SCINT; i++) {
	GetScintCorners(View, sect, i, strippoly);
	Hv_FramePolygon(strippoly, 4, color);
    }
    
    Hv_DestroyRegion(clipreg);
}

/* ---------- GetScintCorners ----------*/

static void GetScintCorners(Hv_View        View,
			    short           sect,
			    unsigned short  id,
			    Hv_Point       *points)

/*Used by FANNED view routines*/

{
  Hv_FPoint     fpts[4];
  
  GetScintFPoly(sect, id, fpts, 0.0, 0.0, 0.0, True);
  Hv_WorldPolygonToLocalPolygon(View, 4, points, fpts);
}



/* --------- GetScintFPoly ---- */

static void GetScintFPoly (short      sect,
			   unsigned   short  id,
			   Hv_FPoint *fp,
			   float      margin,
			   float      del1,
			   float      del2,
			   Boolean    left)

/* gets the screen cordinates for a scint slab */


{

    float           theta, len;
    Scint_Package   sp;
    int             i;
    float           x1, x2, del;
    Boolean         extend;
    
    sp = Scintillators[sect];

/* use strips 0, 34, and 47 (c indices) */

    x1 = SCINTOFFSET;

    len = (sp.scints[id].length + margin)/LENSCALE;

    for (i = 0; i < id; i++)
	x1 += sp.scints[i].width;

    x2 = x1 + sp.scints[id].width;

    del = del2 - del1;
    extend = (del > 1.0);

    fp[0].h = -x1;      fp[0].v =  len; 
    fp[1].h = -x2;      fp[1].v =  len;
    fp[2].h = -x2;      fp[2].v = -len; 
    fp[3].h = -x1;      fp[3].v = -len;

    if (extend) {
	if (left) {
	    fp[2].v -= del2;
	    fp[0].v = fp[2].v + del;
	}

	else {
	    fp[0].v += del2;
	    fp[2].v = fp[0].v - del;
	}

	fp[1].v = fp[0].v;
	fp[3].v = fp[2].v;

    }


/* rotate to the proper sector */

    if (sect != 0) {
	theta = -(sect*60.0)*Hv_DEGTORAD;
	Hv_RotateFPoints(theta, fp, 4);
    }
}


/*------ GetDel1Del2 -------*/

static void GetDel1Del2(Hv_View View,
			unsigned short adc,
			float *del1,
			float *del2)

{
  float     del;
  ViewData  viewdata;
 
  viewdata = GetViewData(View);

  del = ADCToDel(View, adc);

  *del1 = MARGINMIN;
  *del2 = *del1 + del;
}


/*------- ADCToColor ------- */

static short   ADCToColor(Hv_View  View,
			  short    adc)
{
  ViewData          viewdata;
  Hv_RainbowData   *adcrainbow;
  short             colorgap;
  short             valgap;
  float             del;
  short             vmin, vmax, cmin, cmax;

  viewdata = GetViewData(View);

  adcrainbow = (Hv_RainbowData *)(viewdata->ADCRainbowItem->data);

  
  vmin = adcrainbow->valmin;
  vmax = adcrainbow->valmax;
  cmin = adcrainbow->mincolor;
  cmax = adcrainbow->maxcolor;

  if (adc <= vmin)
    return cmin;

  if (adc >= vmax)
    return cmax;

  valgap = vmax - vmin;
  colorgap = cmax - cmin;

  del = ((float)colorgap * ((float)(adc - vmin)))/((float)valgap);
  return (cmin + (short)((int)del));
}


/*------- ADCToDel ------- */

static float   ADCToDel(Hv_View  View,
			  short    adc)
{
  ViewData          viewdata;
  Hv_RainbowData   *adcrainbow;
  short             valgap;
  float             del;
  short             vmin, vmax;

  viewdata = GetViewData(View);

  adcrainbow = (Hv_RainbowData *)(viewdata->ADCRainbowItem->data);

  vmin = adcrainbow->valmin;
  vmax = adcrainbow->valmax;

  if (adc <= vmin)
    return 0.0;

  if (adc >= vmax)
    return PEAKMARGIN;

  valgap = vmax - vmin;

  del = (PEAKMARGIN * ((float)(adc - vmin)))/((float)valgap);
  return del;
}
E 1
