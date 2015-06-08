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

#define  FACTOR_H       0.0            /*in fanned view, take into account origin offset*/
#define  FACTOR_V       315.0          /*in fanned view, take into account origin offset*/
#define  HDEL1  16.0
#define  HDEL2  36.0

#define  HDELMAX 40.0
#define  HDELMIN 4.0

#define  LAECDELMAX 34.0
#define  LAECDELMIN 4.0
#define  LAECDEL1  4.0
#define  LAECDEL2  21.0

#define  LAECMARGIN 40.0

#define  LAECLEFT     0
#define  LAECRIGHT    1

/* constants used for IJ border */

#define  I_MIN           -182.246
#define  I_MAX            190.272
#define  J_MIN           -190.855
#define  J_MAX            190.855
#define  IJ_SLOPE         1.9518378
#define  IJ_INTERCEPT    -I_MIN

/* ------ local prototypes -------*/

static void DrawLAECHits(Hv_View   View,
			 short     sect,
			 Hv_Region region);

static void DrawAccumulatedLAECHits(Hv_View   View,
				    short     sect,
				    Hv_Region region);


static void DrawAccumulatedHits(Hv_View   View,
				short     sect,
				Hv_Region region);


static void FrameLAECStrips(Hv_View  View,
			   short   sect,
			   short   type,
			   short   color);


static void LocalToIJ(Hv_View View,
		      short   sect,
		      short   x,
		      short   y,
		      ijPoint *rij);

static void ActiveTrianglePoly(Hv_Item Item, Hv_Point *poly);

static void ActiveLAECPoly(Hv_Item Item, Hv_Point *poly);

static Boolean LayerMatchesPlane(short plane,
				 short layer);

static Boolean LayerMatchesLAECPlane(short plane,
				     short layer);

static void GetStripCorners(Hv_View       View,
			    short         sect,
			    short         type,
			    unsigned      short  id,
			    Hv_Point     *points);

static void GetLAECStripCorners(Hv_View       View,
				short         sect,
				short         type,
				unsigned      short  id,
				Hv_Point     *points);

static void GetStripFPoly(Hv_View       View,
			  short         sect,
			  short         type,
			  unsigned      short  id,
			  Hv_FPoint    *fpts,
			  float         del1,
			  float         del2);

static void GetLAECStripFPoly(Hv_View       View,
			      short         sect,
			      short         type,
			      unsigned      short  id,
			      Hv_FPoint    *fpts,
			      float         del1,
			      float         del2,
			      Boolean       left);

static void GetFannedShowerPoints (short   sect,
				   Hv_FPoint *fP,
				   float   ExpansionFactor);

static void GetLAECPoints (short      sect,
			   Hv_FPoint *fP,
			   float     shrink,
			   int       xstrip,
			   int       ystrip,
			   float     del1,
			   float     del2,
			   Boolean   left);

static void GetDel1Del2(Hv_View View,
			unsigned char layer,
			unsigned short adc,
			float *del1,
			float *del2);

static void GetLAECDel1Del2(Hv_View View,
			    unsigned char layer,
			    unsigned short adc,
			    float *del1,
			    float *del2);

static Hv_Region TotalHitRegion(Hv_View View,
				short   sect,
				short   plane);

static Hv_Region TotalLAECHitRegion(Hv_View View,
				    short   sect,
				    short   plane);

static short  PixelEnergyToColor(Hv_View  View,
				 float   energy);

static short  ADCToColor(Hv_View  View,
			 short    adc);

static short  ADCToLAECColor(Hv_View  View,
			     short    adc);

static float  ADCToDel(Hv_View  View,
			short    adc);


static float  ADCToLAECDel(Hv_View  View,
			   short    adc);


static void ExtendFPoint(Hv_FPoint *fp1,
			 Hv_FPoint *fp2,
			 Hv_FPoint *fp,
			 float      theta,
			 float      del);

static void DrawPixelHits(Hv_View  View,
			  short   sect,
			  Hv_Region region);

static void FillECStripHits(Hv_View    View,
			    short      sect,
			    short      plane,
			    Boolean    FrameOnly,
			    Hv_Region  region);


static void FillLAECStripHits(Hv_View    View,
			      short      sect,
			      short      plane,
			      Boolean    FrameOnly,
			      Hv_Region  region);

static void UnionStripHitRegion(Hv_View   View,
				short    sect,
				short    type,
				short    ID,
				Hv_Region  *region);

static void UnionLAECStripHitRegion(Hv_View   View,
				    short    sect,
				    short    type,
				    short    ID,
				    Hv_Region  *region);

static void DrawStripHit(Hv_View    View,
			 short      sect,
			 short      type,
			 short      ID,
			 unsigned char  layer,
			 unsigned short adc,
			 Boolean    FrameOnly,
			 Hv_Region  region);

static void DrawLAECStripHit(Hv_View    View,
			     short      sect,
			     short      type,
			     short      ID,
			     unsigned char  layer,
			     unsigned short adcl,
			     unsigned short adcr,
			     Boolean    FrameOnly,
			     Hv_Region  region);


static void DrawECTriangle(Hv_Item  Item,
			   Hv_Region region);

static void DrawLAECItem(Hv_Item  Item,
			 Hv_Region region);

static void fPointFromij(Hv_FPoint  *fp,
			 ijPoint    ij);

static void ijFromfPoint(Hv_FPoint  fp,
			 ijPoint   *ij);

static void ijktoxyz(Shower_Package *sp,
		     short      sect,
		     float      i,
		     float      j,
		     float      k,
		     Cart3D     *X);

Boolean PlaneBorderOnly = False;

/*------------ GetLAECXY ----------*/

extern void GetLAECXY(Hv_View  View,
		      Hv_Point pp,
		      short    sect,
		      short    *xstrip,
		      short    *ystrip) {

  int             i;
  Hv_Point        strippoly[4];
  ViewData        viewdata;

  viewdata = GetViewData(View);

  *xstrip = -1;
  *ystrip = -1;

  for (i = 0; i < 24; i++) {
    GetLAECStripCorners(View, sect, XSTRIP, i, strippoly);
    if (Hv_PointInPolygon(pp, strippoly, 4)) {
	*xstrip = i+1;
	break;
    }
  }
  
  for (i = 0; i < 40; i++) {
    GetLAECStripCorners(View, sect, YSTRIP, i, strippoly);
    if (Hv_PointInPolygon(pp, strippoly, 4)) {
	*ystrip = i+1;
	break;
    }
  }
  
}



/*---------- GetShowerUVW -------*/

void GetShowerUVW(Hv_View  View,
		  Hv_Point pp,
		  short   sect,
		  short   *u,
		  short   *v,
		  short   *w,
		  float   *z,
		  float   *x,
		  float   *y,
		  float   *rad,
		  float   *theta,
		  float   *phi,
		  ijPoint *rij)

{
  int             i;
  Hv_Point        poly[4];
  float           k, dk;
  ViewData        viewdata;
  float           rho;
  Cart3D          X;
  Shower_Package  *sp;

  viewdata = GetViewData(View);

  *u = -1;
  *v = -1;
  *w = -1;

  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, USTRIP, i, poly);
    if (Hv_PointInPolygon(pp, poly, 4)) {
      *u = i+1;
      break;
    }
  }

  if (*u <= 0)
    return;

  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, VSTRIP, i, poly);
    if (Hv_PointInPolygon(pp, poly, 4)) {
      *v = i+1;
      break;
    }
  }

  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, WSTRIP, i, poly);
    if (Hv_PointInPolygon(pp, poly, 4)) {
      *w = i+1;
      break;
    }
  }

/* now get the position information */


  sp = &(ShowerCounters[sect]);
  GetStripTypekValue(USTRIP, viewdata->ecplane, &k, &dk);
  *theta = (90.0 - (sect*60.0))*Hv_DEGTORAD;
  LocalToIJ(View, sect, pp.x, pp.y, rij);

  ijktoxyz(sp, sect, rij->i, rij->j, k, &X);
  *rad = Cart3DLength(&X);
  *x = X.x;
  *y = X.y;
  *z = X.z;

  rho = sqrt((X.x*X.x) + (X.y*X.y));
  *theta = Hv_RADTODEG*atan2(rho, *z);

  if ((*y == 0.0) && (*x == 0.0))
    *phi = 0.0;
  else
    *phi = Hv_RADTODEG*atan2(*y, *x);
  if (*phi < 0.0)
    *phi += 360.0;
  
}


/*-------- LocalToIJ ---------*/

static void LocalToIJ(Hv_View View,
		      short   sect,
		      short   x,
		      short   y,
		      ijPoint *rij) {

    Hv_FPoint    fp;
    float        theta;
    
    Hv_LocalToWorld(View, &(fp.h), &(fp.v), x, y);
    theta = (90.0 - (sect*60.0))*Hv_DEGTORAD;
    Hv_RotateFPoint(-theta, &fp);
    ijFromfPoint(fp, rij);    /*we now have ij coord of point in local ij system*/
}

/*-------- FrameECStrips ----------*/

void FrameECStrips(Hv_View  View,
		   short   sect,
		   short   type,
		   short   color)

/*
   sect     C Index 
   type     < 0 --> frame ALL 
*/

{
  int i;
  Hv_Point              strippoly[4];
  Hv_Region             clipreg;

  clipreg = Hv_ClipHotRect(View, 1);

  if ((type < 0) || (type == USTRIP))
  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, USTRIP, i, strippoly);
    Hv_FramePolygon(strippoly, 4, color);
  }

  if ((type < 0) || (type == VSTRIP))
  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, VSTRIP, i, strippoly);
    Hv_FramePolygon(strippoly, 4, color);
  }

  if ((type < 0) || (type == WSTRIP))
  for (i = 0; i < 36; i++) {
    GetStripCorners(View, sect, WSTRIP, i, strippoly);
    Hv_FramePolygon(strippoly, 4, color);
  }

  Hv_DestroyRegion(clipreg);
}

/*-------- FrameLAECStrips ----------*/

static void FrameLAECStrips(Hv_View  View,
			   short   sect,
			   short   type,
			   short   color)

/*
   sect     C Index 
   type     < 0 --> frame ALL 
*/

{
  int i;
  Hv_Point              strippoly[4];
  Hv_Region             clipreg;

  clipreg = Hv_ClipHotRect(View, 1);

/* x are the long strips */

  if ((type < 0) || (type == XSTRIP))
  for (i = 0; i < 24; i++) {
    GetLAECStripCorners(View, sect, XSTRIP, i, strippoly);
    Hv_FramePolygon(strippoly, 4, color);
  }

/* y are the short strips */

  if ((type < 0) || (type == YSTRIP))
  for (i = 0; i < 40; i++) {
    GetLAECStripCorners(View, sect, YSTRIP, i, strippoly);
    Hv_FramePolygon(strippoly, 4, color);
  }


  Hv_DestroyRegion(clipreg);
}

/*------- MallocForwardEC ------------*/

Hv_Item MallocForwardEC(Hv_View  View,
			short    sect, /*C Index*/
			Hv_Item  parent)

{
  Hv_Item      Item;
  Hv_FPoint    *fpts;
  short        npts = 3;

  ViewData     viewdata;
  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));
  GetFannedShowerPoints(sect, fpts, 0.3);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_PARENT,       parent,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_COLOR,        viewdata->fannedecbgcolor[0],
			 Hv_FILLCOLOR,    -1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_USERDRAW,     DrawECTriangle,
			 Hv_USER2,        (short)sect,
			 NULL);

  Item->singleclick = NULL;
  Item->doubleclick = NULL;
  return  Item;
}

/*------- MallocLAEC ------------*/

Hv_Item MallocLAEC(Hv_View  View,
		   short    sect, /*C Index*/
		   Hv_Item  parent)

{
  Hv_Item      Item = NULL;
  Hv_FPoint    *fpts;
  short        npts = 4;

  ViewData     viewdata;
  viewdata = GetViewData(View);

  fpts = (Hv_FPoint *)Hv_Malloc(npts*sizeof(Hv_FPoint));

  GetLAECPoints(sect, fpts, 0.0, -1, -1, 0.0, 0.0, False);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_PARENT,       parent,
			 Hv_NUMPOINTS,    npts,
			 Hv_DATA,         fpts,
			 Hv_COLOR,        viewdata->fannedecbgcolor[0],
			 Hv_FILLCOLOR,    -1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_USERDRAW,     DrawLAECItem,
			 Hv_USER2,        (short)sect,
			 NULL);

  Item->singleclick = NULL;
  Item->doubleclick = NULL;
  
  return  Item;
}

/*------ GetLAECHit ----------*/

extern void GetLAECHit(short  sect,
		       short  plane,
		       short  xstrip,
		       short  ystrip,
		       short  *xtdcl,
		       short  *xadcl,
		       short  *xtdcr,
		       short  *xadcr,
		       short  *ytdcl,
		       short  *yadcl,
		       short  *ytdcr,
		       short  *yadcr) {

/* note the xstrip and y strip are NOT c indices */

    short        xlayer, ylayer;
    EC1DataPtr   hits;
    Boolean      foundx, foundy;
    int          i;
    unsigned char strip, layer;

    if (bosIndexEC1[sect] == 0)
	return;

    plane  = 2*plane;
    xlayer = plane + 1;
    ylayer = plane + 2;
    
    foundx = False;
    foundy = False;
    hits = bosEC1[sect];

    for (i = 0; i < bosNumEC1[sect]; i++) {
	BreakShort(hits->id, &strip, &layer);
	
	
	if ((strip == xstrip) && (layer == xlayer)) {
	    *xtdcl = hits->tdcleft;
	    *xadcl = hits->adcleft;
	    *xtdcr = hits->tdcright;
	    *xadcr = hits->adcright;
	    foundx = True;
	}
	else if ((strip == ystrip) && (layer == ylayer)) {
	    *ytdcl = hits->tdcleft;
	    *yadcl = hits->adcleft;
	    *ytdcr = hits->tdcright;
	    *yadcr = hits->adcright;
	    foundy = True;
	}

    
	if (foundx && foundy)
	    break;
	
	hits++;
    }

}

/*------ GetShowerUVWHit --------*/

void GetShowerUVWHit(short  sect,
		     short  plane,
		     short  u,
		     short  v,
		     short  w,
		     short  *utdc,
		     short  *vtdc,
		     short  *wtdc,
		     short  *uadc,
		     short  *vadc,
		     short  *wadc)

/* 
   sect     C Index
   plane    C Index
   u, v, w  NOT C INDICES 
   *utdc, *vtdc, *wtdc, *uadc, *vadc, *wadc
*/


/*----------------------------------------
Given the sector, plane and uvw IDs, this
routine will search the BOS banks for matching
hits and return the tdcs and adcs.
-----------------------------------------*/

{
  short        ulayer, vlayer, wlayer;
  ECDataPtr    hits;
  Boolean      foundu, foundv, foundw;
  int          i;
  unsigned char strip, layer;

/* assume failure */

  *utdc = -1;
  *uadc = -1;
  *vtdc = -1;
  *vadc = -1;
  *wtdc = -1;
  *wadc = -1;

  if (bosIndexEC[sect] == 0)
    return;

  if (plane == ECBOTH)
    return;

  plane  = 3*plane;
  ulayer = plane + 1;
  vlayer = plane + 2;
  wlayer = plane + 3;

  foundu = False;
  foundv = False;
  foundw = False;

  hits = bosEC[sect];
  for (i = 0; i < bosNumEC[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);

    if ((strip == u) && (layer == ulayer)) {
      *utdc = hits->tdc;
      *uadc = hits->adc;
      foundu = True;
    }
    else if ((strip == v) && (layer == vlayer)) {
      *vtdc = hits->tdc;
      *vadc = hits->adc;
      foundv = True;
    }
    else if ((strip == w) && (layer == wlayer)) {
      *wtdc = hits->tdc;
      *wadc = hits->adc;
      foundw = True;
    }
    
    if (foundu && foundv && foundw)
      break;
    
    hits++;
  }
}

/*-------- DrawFannedShowerHits -----*/

void DrawFannedShowerHits(Hv_View   View,
			  short     sect,
			  Hv_Region region)

/*----------------------------------
On the given view and in the given sector,
draw the shower hits
------------------------------------*/

{
  ViewData     viewdata;
  short        plane;

  Hv_Region    innerreg = NULL;
  Hv_Region    outerreg = NULL;
  Hv_Region    isectreg = NULL;

  DrawLAECHits(View, sect, region);

  viewdata = GetViewData(View);

  if (viewdata->showwhat != SINGLEEVENT) {
      DrawAccumulatedHits(View, sect, region);
      return;
  }

  if (bosIndexEC[sect] == 0)
    return;

  if (View->tag != CALORIMETER)
    return;

  plane = viewdata->ecplane;
      
  if (plane != ECBOTH) {
    FillECStripHits(View, sect, plane, False, region);
    FillECStripHits(View, sect, plane, True, region);
  }
  else {  /* both */
    innerreg = TotalHitRegion(View, sect, ECINNER);
    outerreg = TotalHitRegion(View, sect, ECOUTER);
    isectreg = Hv_IntersectRegion(innerreg, outerreg);
    Hv_FillRegion(innerreg, Hv_gray7, region);
    Hv_FillRegion(outerreg, Hv_gray9, region);
    Hv_FillRegion(isectreg, Hv_skyBlue, region);
    Hv_DestroyRegion(innerreg);
    Hv_DestroyRegion(outerreg);
    Hv_DestroyRegion(isectreg);
    FillECStripHits(View, sect, plane, True, region);
  }
    
/* strips must have gotten garbled */
  
  if (viewdata->displayU)
    FrameECStrips(View, sect, USTRIP, viewdata->fannedecstripcolor);
  if (viewdata->displayV)
    FrameECStrips(View, sect, VSTRIP, viewdata->fannedecstripcolor);
  if (viewdata->displayW)
    FrameECStrips(View, sect, WSTRIP, viewdata->fannedecstripcolor);
  
  DrawPixelHits(View, sect, region);
}


/*----------- DrawAccumulatedHits ----------*/

static void DrawAccumulatedHits(Hv_View   View,
				short     sect,
				Hv_Region region) {

    ViewData         viewdata;
    int              i;
    int              val;
    short            mincolor, maxcolor, underflow, overflow, acc;
    Hv_RainbowData  *data;

    Hv_Point         poly[4];
    short            plane;

    viewdata = GetViewData(View);

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

    plane = viewdata->ecplane;


/* loop over the slabs and draw mit the correct color */

    for (i = 0; i < 36; i++) {

/* val will first be counts */

	if (plane == ECBOTH) 
	    val = accec[sect].counts[ECINNER][viewdata->ecshowwhat][i] +
		accec[sect].counts[ECOUTER][viewdata->ecshowwhat][i];
	else
	    val = accec[sect].counts[plane][viewdata->ecshowwhat][i];
	
	if (val <= data->valmin)
	    acc = underflow;
	else if (val >= data->valmax)
	    acc = overflow;
	else
	    acc = CountToColor(val, data, viewdata->algorithm);


	GetStripCorners(View, sect, viewdata->ecshowwhat, i, poly);
	Hv_FillPolygon(poly, 4, False, acc, acc);
    }


}


/*----------- DrawAccumulatedLAECHits ----------*/

static void DrawAccumulatedLAECHits(Hv_View   View,
				    short     sect,
				    Hv_Region region) {

    ViewData         viewdata;
    int              i;
    int              val;
    short            mincolor, maxcolor, underflow, overflow, acc;
    Hv_RainbowData  *data;
    int              imax;
    Hv_Point         poly[4];
    short            plane;

    if (sect > 1)
	return;

    viewdata = GetViewData(View);

/* are we showing counts or avg TDC? */

    if (viewdata->showwhat == ACCUMULATEDEVENTS)
	data = (Hv_RainbowData *)(viewdata->AccumulatedHitsItem[1]->data);
    else
	return;


/* get the min and max colors for the scale */

    mincolor  = Hv_sMin(data->mincolor, data->maxcolor);
    maxcolor  = Hv_sMax(data->mincolor, data->maxcolor);
    underflow = Hv_black;
    overflow  = Hv_gray9;

    plane = viewdata->ecplane;

    if (viewdata->ec1showwhat == XSTRIP)
	imax = 24;
    else 
	imax = 40;
	
/* loop over the slabs and draw mit the correct color */

    for (i = 0; i < imax; i++) {

/* val will first be counts */

	if (plane == ECBOTH) 
	    val = accec1[sect].counts[ECINNER][viewdata->ec1showwhat][i] +
		accec1[sect].counts[ECOUTER][viewdata->ec1showwhat][i];
	else
	    val = accec1[sect].counts[plane][viewdata->ec1showwhat][i];
	
	if (val <= data->valmin)
	    acc = underflow;
	else if (val >= data->valmax)
	    acc = overflow;
	else
	    acc = CountToColor(val, data, viewdata->algorithm);


	GetLAECStripCorners(View, sect, viewdata->ec1showwhat, i, poly);
	Hv_FillPolygon(poly, 4, False, acc, acc);
    }



}


/*-------- DrawLAECHits -----*/

static void DrawLAECHits(Hv_View   View,
			 short     sect,
			 Hv_Region region)

/*----------------------------------
On the given view and in the given sector,
draw the shower hits
------------------------------------*/

{
  ViewData     viewdata;
  short        plane;

  Hv_Region    innerreg = NULL;
  Hv_Region    outerreg = NULL;
  Hv_Region    isectreg = NULL;

  viewdata = GetViewData(View);

  if (viewdata->showwhat != SINGLEEVENT) {
      DrawAccumulatedLAECHits(View, sect, region);
      return;
  }

  if (bosIndexEC1[sect] == 0)
    return;

  if (View->tag != CALORIMETER)
    return;

  plane = viewdata->ecplane;
      
  if (plane != ECBOTH) {
      FillLAECStripHits(View, sect, plane, False,region);
      FillLAECStripHits(View, sect, plane, True, region);
  }
  else {  /* both */
      innerreg = TotalLAECHitRegion(View, sect, ECINNER);
      outerreg = TotalLAECHitRegion(View, sect, ECOUTER);
      isectreg = Hv_IntersectRegion(innerreg, outerreg);
      Hv_FillRegion(innerreg, Hv_gray7, region);
      Hv_FillRegion(outerreg, Hv_gray9, region);
      Hv_FillRegion(isectreg, Hv_skyBlue, region);
      Hv_DestroyRegion(innerreg);
      Hv_DestroyRegion(outerreg);
      Hv_DestroyRegion(isectreg);
      FillLAECStripHits(View, sect, plane, True, region);
  }
    
/* strips must have gotten garbled */
  
  if (viewdata->displayX)
    FrameLAECStrips(View, sect, XSTRIP, viewdata->fannedecstripcolor);
  if (viewdata->displayY)
    FrameLAECStrips(View, sect, YSTRIP, viewdata->fannedecstripcolor);
  
/*  DrawLAECPixelHits(View, sect, region); */
}

/*-------- GetLAECHistoHit -------------*/

extern void GetLAECHistoHit(Hv_View   View,
			    Hv_Point  pp,
			    short     sect,
			    short     *xstrip,
			    short     *ystrip,
			    short     *tdc,
			    short     *adc,
			    Boolean   *left)

{
  EC1DataPtr     hits;
  Hv_FPoint      fpts[4];
  Hv_Point       poly[4];
  short          type;
  int            i;
  float          del1, del2;
  ViewData       viewdata;
  unsigned char  strip, layer;

  *xstrip = -1;
  *ystrip = -1;
  *tdc = -1;
  *adc = -1;

  viewdata = GetViewData(View);

  hits = bosEC1[sect];

  for (i = 0; i < bosNumEC1[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);

    if (LayerMatchesLAECPlane(viewdata->ecplane, layer)) {
      type = (layer - 1) % 2;

/* at this point strip is not a C index */
      
      if ((strip > 0) && (strip <= 40)) {

/* try left first */

	  GetLAECDel1Del2(View, layer, hits->adcleft, &del1, &del2);
	  GetLAECStripFPoly(View, sect, type, strip-1, fpts, del1, del2, True);
	  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
	  
	  if (Hv_PointInPolygon(pp, poly, 4)) {
	      *left = True;
	      *tdc = (short)(hits->tdcleft);
	      *adc = (short)(hits->adcleft);
	      
	      if (type == XSTRIP) {
		  *xstrip = strip;
		  if (layer > 2)
		      *xstrip += 100;
	      }
	      
	      else if (type == YSTRIP) {
		  *ystrip = strip;
		  if (layer > 2)
		      *ystrip += 100;
	      }
	      
	      return;
	  } /* end point in poly (left) */


/* now try right */

	  GetLAECDel1Del2(View, layer, hits->adcright, &del1, &del2);
	  GetLAECStripFPoly(View, sect, type, strip-1, fpts, del1, del2, False);
	  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
	  
	  if (Hv_PointInPolygon(pp, poly, 4)) {
	      *left = False;
	      *tdc = (short)(hits->tdcright);
	      *adc = (short)(hits->adcright);
	      
	      if (type == XSTRIP) {
		  *xstrip = strip;
		  if (layer > 2)
		      *xstrip += 100;
	      }
	      
	      else if (type == YSTRIP) {
		  *ystrip = strip;
		  if (layer > 2)
		      *ystrip += 100;
	      }
	      
	      return;
	  } /* end point in poly (right) */
	  
	  
      } /* end good id */
      
    }
    hits++;
  }
}

/*----- GetShowerHistoHit ------*/

void GetShowerHistoHit(Hv_View   View,
		       Hv_Point  pp,
		       short     sect,
		       short     *u,
		       short     *v,
		       short     *w,
		       short     *tdc,
		       short     *adc)

{
  ECDataPtr     hits;
  Hv_FPoint     fpts[4];
  Hv_Point      poly[4];
  short         type;
  int           i;
  float         del1, del2;
  ViewData      viewdata;
  unsigned char strip, layer;

  *u = -1;
  *v = -1;
  *w = -1;
  *tdc = -1;
  *adc = -1;

  viewdata = GetViewData(View);
  
  hits = bosEC[sect];
  
  for (i = 0; i < bosNumEC[sect]; i++) {
      BreakShort(hits->id, &strip, &layer);
      
      if (LayerMatchesPlane(viewdata->ecplane, layer)) {
	  type = (layer - 1) % 3;
	  
/* at this point strip is not a C index */
      
	  if ((strip > 0) && (strip <= NUM_STRIP)) {
	      GetDel1Del2(View, layer, hits->adc, &del1, &del2);
	      GetStripFPoly(View, sect, type, strip-1, fpts, del1, del2);
	      
	      Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
	      
	      if (Hv_PointInPolygon(pp, poly, 4)) {
		  *tdc = (short)(hits->tdc);
		  *adc = (short)(hits->adc);
		  
		  if (type == USTRIP) {
		      *u = strip;
		      if (layer > 3)
			  *u += 100;
		  }
		  
		  else if (type == VSTRIP) {
		      *v = strip;
		      if (layer > 3)
			  *v += 100;
		  }
		  
		  else if (type == WSTRIP) {
		      *w = strip;
		      if (layer > 3)
			  *w += 100;
		  }
		  
		  return;  /* added this return 6/16/97 seems safe */
		  
	      }
	  } /* end good id */
	  
      }
      hits++;
  }
}


/*------ DrawLAECStripHit -----------*/

static void DrawLAECStripHit(Hv_View    View,
			     short      sect,   /*C INDEX*/
			     short      type,   /*XSTRIP, YSTRIP*/
			     short      ID,     /*NOT a C Index*/
			     unsigned   char layer,
			     unsigned   short adcl,
			     unsigned   short adcr,
			     Boolean    FrameOnly,
			     Hv_Region  region)
{
  ViewData      viewdata;
  short         fillcolor, framecolor;
  Hv_Point      poly[4];
  Hv_FPoint     fpts[4];
  float         del1, del2;
  short         x1, x2, y1, y2;
  short         minid, maxid;
    

  viewdata = GetViewData(View);
  ID--;  /* convert to cindex */


/* check for bad guys */

  if ((type < XSTRIP) || (type > YSTRIP) || (ID < 0) || (ID > 40)) {
    fprintf(stderr, "WARNING bad data in EC1 bank.\n");
    fprintf(stderr, "type = %d   ID = %d\n", type, ID);
    return;
  }

  fillcolor  = viewdata->fannedechitcolor;
  framecolor = viewdata->fannedecstripcolor;

  GetLAECStripCorners(View, sect, type, ID, poly);
  if (FrameOnly)
    Hv_FramePolygon(poly, 4, framecolor);
  else
    Hv_FillPolygon(poly, 4, True, fillcolor, framecolor);

/* now draw the extended color histo strip */

/* first left */

  GetLAECDel1Del2(View, layer, adcl, &del1, &del2);

  if ((del2 > 1.0) && ((del2-del1) > 0.1)) {
      fillcolor = ADCToLAECColor(View, adcl);
      GetLAECStripFPoly(View, sect, type, ID, fpts, del1, del2, True);
      Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
      Hv_FillPolygon(poly, 4, True, fillcolor, framecolor);


/* for case of both, draw a center line. Inner goes below, outer above */

      if (viewdata->ecplane == ECBOTH) {
	  del1 = LAECDEL1;
	  del2 = del1 + LAECDELMAX/2.0;
	  
	  minid = Hv_sMax(0, ID-1);
	  
	  if (type == XSTRIP)
	      maxid = Hv_sMin(24, ID+2);
	  else
	      maxid = Hv_sMin(39, ID+2);
	  
	  GetLAECStripFPoly(View, sect, type, minid, fpts, del1, del2, True);
	  Hv_WorldToLocal(View, fpts[0].h, fpts[0].v, &x1, &y1);
	  GetLAECStripFPoly(View, sect, type, maxid, fpts, del1, del2, True);
	  Hv_WorldToLocal(View, fpts[0].h, fpts[0].v, &x2, &y2);
	  Hv_DrawLine(x1, y1, x2, y2, Hv_gray14);
      }
      
  }

/* now right */

  GetLAECDel1Del2(View, layer, adcr, &del1, &del2);

  if ((del2 > 1.0) && ((del2-del1) > 0.1)) {
      fillcolor = ADCToLAECColor(View, adcr);
      GetLAECStripFPoly(View, sect, type, ID, fpts, del1, del2, False);
      Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
      Hv_FillPolygon(poly, 4, True, fillcolor, framecolor);



/* for case of both, draw a center line. Inner goes below, outer above */

      if (viewdata->ecplane == ECBOTH) {
	  del1 = LAECDEL1;
	  del2 = del1 + LAECDELMAX/2.0;
	  
	  minid = Hv_sMax(0, ID-2);
	  
	  if (type == XSTRIP)
	      maxid = Hv_sMin(24, ID+1);
	  else
	      maxid = Hv_sMin(39, ID+1);
	  
	  GetLAECStripFPoly(View, sect, type, minid, fpts, del1, del2, False);
	  Hv_WorldToLocal(View, fpts[2].h, fpts[2].v, &x1, &y1);
	  GetLAECStripFPoly(View, sect, type, maxid, fpts, del1, del2, False);
	  Hv_WorldToLocal(View, fpts[2].h, fpts[2].v, &x2, &y2);
	  Hv_DrawLine(x1, y1, x2, y2, Hv_gray14);
      }
      

  }

}

/*------ DrawStripHit -----------*/

static void DrawStripHit(Hv_View    View,
			 short      sect,   /*C INDEX*/
			 short      type,   /*USTRIP, VSTRIP, or WSTRIP*/
			 short      ID,     /*NOT a C Index*/
			 unsigned   char layer,
			 unsigned   short adc,
			 Boolean    FrameOnly,
			 Hv_Region  region)
{
  ViewData      viewdata;
  short         fillcolor, framecolor;
  Hv_Point      poly[4];
  Hv_FPoint     fpts[4];
  float         del1, del2;
  short         x1, x2, y1, y2;
  short         minid, maxid;
    

  viewdata = GetViewData(View);
  ID--;  /* convert to cindex */


/* check for bad guys */

  if ((type < USTRIP) || (type > WSTRIP) || (ID < 0) || (ID > NUM_STRIP)) {
    fprintf(stderr, "WARNING bad data in EC bank.\n");
    fprintf(stderr, "type = %d   ID = %d\n", type, ID);
    return;
  }

  fillcolor  = viewdata->fannedechitcolor;
  framecolor = viewdata->fannedecstripcolor;

  GetStripCorners(View, sect, type, ID, poly);
  if (FrameOnly)
    Hv_FramePolygon(poly, 4, framecolor);
  else
    Hv_FillPolygon(poly, 4, True, fillcolor, framecolor);

/* now draw the extended color histo strip */

  GetDel1Del2(View, layer, adc, &del1, &del2);

  if ((del2 < 1.0) || ((del2-del1) < 0.1))
    return;

  fillcolor = ADCToColor(View, adc);
  GetStripFPoly(View, sect, type, ID, fpts, del1, del2);
  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
  Hv_FillPolygon(poly, 4, True, fillcolor, framecolor);

/* for case of both, draw a center line. Inner goes below, outer above */

  if (viewdata->ecplane == ECBOTH) {
    del1 = HDEL1;
    del2 = del1 + HDELMAX/2.0;
    minid = Hv_sMax(0, ID-1);
    maxid = Hv_sMin(35, ID+2);
    GetStripFPoly(View, sect, type, minid, fpts, del1, del2);
    Hv_WorldToLocal(View, fpts[0].h, fpts[0].v, &x1, &y1);
    GetStripFPoly(View, sect, type, maxid, fpts, del1, del2);
    Hv_WorldToLocal(View, fpts[0].h, fpts[0].v, &x2, &y2);
    Hv_DrawLine(x1, y1, x2, y2, Hv_gray14);
  }
}

/*------ GetDel1Del2 -------*/

static void GetDel1Del2(Hv_View View,
			unsigned char  layer,
			unsigned short adc,
			float *del1,
			float *del2)

{
  float     del;
  ViewData  viewdata;
 
  viewdata = GetViewData(View);

  del = ADCToDel(View, adc);
  del = Hv_fMax(del, HDELMIN);

  if (viewdata->ecplane == ECBOTH) {
    del /= 2.0;
    
    if (layer > 3) {
      *del1 = HDEL2;
      *del2 = *del1 + del;
    }
    else {
      *del2 = HDEL2;
      *del1 = *del2 - del;
    }
  }
  else {
    *del1 = HDEL1;
    *del2 = *del1 + del;
  }
}

/*------ GetLAECDel1Del2 -------*/

static void GetLAECDel1Del2(Hv_View View,
			    unsigned char  layer,
			    unsigned short adc,
			    float *del1,
			    float *del2)

{
    float     del;
    ViewData  viewdata;
    
    viewdata = GetViewData(View);
    
    del = ADCToLAECDel(View, adc);
    del = Hv_fMax(del, LAECDELMIN);

    if (viewdata->ecplane == ECBOTH) {
	del /= 2.0;
	
	if (layer > 2) {
	    *del1 = LAECDEL2;
	    *del2 = *del1 + del;
	}
	else {
	    *del2 = LAECDEL2;
	    *del1 = *del2 - del;
	}
    }
    else {
	*del1 = LAECDEL1;
	*del2 = *del1 + del;
    }
}

/*------ TotalLAECHitRegion ---------*/

static Hv_Region TotalLAECHitRegion(Hv_View View,
				short   sect,
				short   plane)

{
  Hv_Region     reg = NULL;
  EC1DataPtr    hits;
  Hv_Region     xreg = NULL;
  Hv_Region     yreg = NULL;
  int           i;
  unsigned char strip, layer;
  short         type;

  if (bosIndexEC1[sect] == 0)
    return NULL;

/* add the hit regions */

  hits = bosEC1[sect];
  for (i = 0; i < bosNumEC1[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);

    type = (layer - 1) % 2;  /* 0..1  for x, y*/
    if (LayerMatchesLAECPlane(plane, layer)) {
      if (type == 0)
	UnionLAECStripHitRegion(View, sect, type, strip, &xreg);
      else if (type == 1)
	UnionLAECStripHitRegion(View, sect, type, strip, &yreg);
    }
    hits++;  
  }  /* end for i loop over hits */
  
/* get the total region of all hit strips */

  Hv_AddRegionToRegion(&reg, xreg);
  Hv_AddRegionToRegion(&reg, yreg);
  return reg;
}


/*------ TotalHitRegion ---------*/

static Hv_Region TotalHitRegion(Hv_View View,
				short   sect,
				short   plane)

{
  Hv_Region     reg = NULL;
  ECDataPtr     hits;
  Hv_Region     ureg = NULL;
  Hv_Region     vreg = NULL;
  Hv_Region     wreg = NULL;
  int           i;
  unsigned char strip, layer;
  short         type;

  if (bosIndexEC[sect] == 0)
    return NULL;

/* add the hit regions */

  hits = bosEC[sect];
  for (i = 0; i < bosNumEC[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);

    type = (layer - 1) % 3;  /* 0..2  for u, v, w*/
    if (LayerMatchesPlane(plane, layer)) {
      if (type == 0)
	UnionStripHitRegion(View, sect, type, strip, &ureg);
      else if (type == 1)
	UnionStripHitRegion(View, sect, type, strip, &vreg);
      else
	UnionStripHitRegion(View, sect, type, strip, &wreg);
    }
    hits++;  
  }  /* end for i loop over hits */
  
/* get the total region of all hit strips */

  Hv_AddRegionToRegion(&reg, ureg);
  Hv_AddRegionToRegion(&reg, vreg);
  Hv_AddRegionToRegion(&reg, wreg);
  return reg;
}


/*------ DrawPixelHits -------*/

static void DrawPixelHits(Hv_View   View,
			  short     sect,  /* C Index */
			  Hv_Region region)


{
  ViewData      viewdata;
  short         plane;
  int           num;
  PiDataPtr    *localhits;
  Hv_Item       ecitem;
  Hv_Point      poly[3];
  Hv_Region     treg = NULL;
  Hv_Region     ureg = NULL;
  Hv_Rect       rect;
  Hv_RainbowData *data;
  short         x, y;
  ijPoint       rij;
  Hv_Rect       cr;
  short         color;
  float         energy;

  if ((bosECPI == NULL) || (bosNumECPI < 1))
      return;

  viewdata = GetViewData(View);

  plane = viewdata->ecplane;

  ECPIHitsHere(sect, plane, &num, &localhits);

  if ((num == 0) || (localhits == NULL))
      return;


/* get the corresponding item and see if it is visible */


  ecitem = viewdata->ForwardECViewECItems[sect];
  ActiveTrianglePoly(ecitem, poly);

  treg = Hv_CreateRegionFromPolygon(poly, 3);
  ureg = Hv_IntersectRegion(treg, region);
  Hv_DestroyRegion(treg);
  if (XEmptyRegion(ureg)) {
      Hv_DestroyRegion(ureg);
      return;
  }

  Hv_SetClippingRegion(ureg);

/* find the enclosing rect */

  Hv_ClipBox(ureg, &rect);

/* do the raster scan */

  data = (Hv_RainbowData *) (viewdata->EnergyRainbowItem->data);

  fprintf(stderr, "DRAWIN ECAL RECON SECTOR %d...\n", sect+1);

  x = rect.left;

  while (x < rect.right) {
      y = rect.top;
      while (y < rect.bottom) {
	  LocalToIJ(View, sect, x, y, &rij);

	  energy = ijToEnergy(rij.i, rij.j, num, localhits);
	  if (energy > 0.001) {
	      color = PixelEnergyToColor(View, energy);
	      Hv_SetRect(&cr, x, y, data->dx, data->dy);
	      Hv_FillRect(&cr, color);
	  }

	  y += data->dy;
      }
      x += data->dx;
  }

  Hv_Free(localhits);
  Hv_SetClippingRegion(region);
}

/*------ ijToEnergy ----------*/

float ijToEnergy(float  i,
		 float  j,
		 int    num,
		 PiDataPtr *localhits)

{
    int       n;
    float     e;
    PiDataPtr pn;
    float     en, expi, expj;

    e = 0.0;

    for (n = 0; n < num; n++) {
	pn = localhits[n];

	expi = (i - pn->iloc)/(pn->diloc);
	expi = -expi*expi/2.0;

	expj = (j - pn->jloc)/(pn->djloc);
	expj = -expj*expj/2.0;

	en = pn->e*exp(expi)*exp(expj);

/* 12/9 add normalization effects, which means the "e"
   returned should be the continuous energy density at
   that ij location, the double integral of which equals
   the "e" of the bank  */

/*	en = en/(Hv_TWOPI*(pn->diloc)*(pn->djloc)); */

	e += en;
    }

    return e;
}


/*------ ECPIHitsHere --------*/

void ECPIHitsHere(short sect, short plane,
		  int        *num,
		  PiDataPtr  **localhits)

/* sect : 0..5 */

{
    int i, j;
    int lay;
    PiDataPtr  hits;

    sect++;

/* initialize */

    *num = 0;
    *localhits = NULL;
    if ((bosECPI == NULL) || (bosNumECPI < 1))
	return;

/* must use this weird 9,10,11 scheme in ECPI ddl */

    if (plane == ECBOTH)
	lay = 9;
    else if (plane == ECINNER)
	lay = 10;
    else if (plane == ECOUTER)
	lay = 11;
    else
	return;

/* first loop counts #hits in this sect/layer */

    for (hits = bosECPI, i = 0; i < bosNumECPI; i++) {
	if ((hits->id == sect) && (hits->layer == lay))
	    *num += 1;
	hits++;
    }

    if (*num == 0)
	return;

    *localhits = (PiDataPtr *)(Hv_Malloc((*num) * sizeof(PiDataPtr)));

/* 2nd loop assigns */

    for (hits = bosECPI, i = 0, j = 0; i < bosNumECPI; i++) {
	if ((hits->id == sect) && (hits->layer == lay)) {
	    (*localhits)[j] = hits;
	    j++;
	}
	hits++;
    }
}


/*------ DrawPixel -------*/

void DrawPixel(Hv_View   View,
	       short     sect,     /*C Index*/
	       short     u,        /*not C Index*/
	       short     v,        /*not C Index*/
	       short     w,        /*not C Index*/
	       short     color,
	       Hv_Region region)


{
  Hv_Point    upoints[4];
  Hv_Point    vpoints[4];
  Hv_Point    wpoints[4];
  Hv_Region   ureg, vreg, wreg, uvreg, uvwreg;
  ViewData    viewdata;
  short       framecolor;

  viewdata = GetViewData(View);

  if ((u <= 0) || (v <= 0) || (w <= 0))
    return;

  GetStripCorners(View, sect, USTRIP, u-1, upoints);
  GetStripCorners(View, sect, VSTRIP, v-1, vpoints);
  GetStripCorners(View, sect, WSTRIP, w-1, wpoints);

  ureg = Hv_CreateRegionFromPolygon(upoints, 4);
  vreg = Hv_CreateRegionFromPolygon(vpoints, 4);
  wreg = Hv_CreateRegionFromPolygon(wpoints, 4);

  uvreg = Hv_IntersectRegion(ureg, vreg);
  uvwreg = Hv_IntersectRegion(uvreg, wreg);
  
  Hv_FillRegion(uvwreg, color, region);

  Hv_SetClippingRegion(uvwreg);

  framecolor = viewdata->fannedecstripcolor;
  
  Hv_FramePolygon(upoints, 4, framecolor);
  Hv_FramePolygon(vpoints, 4, framecolor);
  Hv_FramePolygon(wpoints, 4, framecolor);
  Hv_SetClippingRegion(region);

  Hv_DestroyRegion(ureg);
  Hv_DestroyRegion(vreg);
  Hv_DestroyRegion(wreg);
  Hv_DestroyRegion(uvreg);
  Hv_DestroyRegion(uvwreg);
}

/*------ FillECStripHits -----------*/

static void FillECStripHits(Hv_View    View,
			    short      sect,   /* C index */
			    short      plane,  /* C Index */
			    Boolean    FrameOnly,
			    Hv_Region  region)
{
  ECDataPtr    hits;
  int          i;
  
  unsigned char layer, strip;
  short         type, fillcolor;
  Hv_Region     ureg = Hv_CreateRegion();
  Hv_Region     vreg = Hv_CreateRegion();
  Hv_Region     wreg = Hv_CreateRegion();
  Hv_Region     uvreg = Hv_CreateRegion();
  Hv_Region     uvwreg = Hv_CreateRegion();
  Hv_Region     isuvreg = Hv_CreateRegion();
  Hv_Region     isectreg = Hv_CreateRegion();

  Hv_Region     vwreg = Hv_CreateRegion();
  Hv_Region     uwreg = Hv_CreateRegion();
  Hv_Region     just2reg = Hv_CreateRegion();

  ViewData     viewdata;

  if (bosIndexEC[sect] == 0)
    return;

  viewdata = GetViewData(View);

  if (!FrameOnly) {
    fillcolor  = viewdata->fannedechitcolor;

/* first get the hit regions */

    hits = bosEC[sect];
    for (i = 0; i < bosNumEC[sect]; i++) {
      BreakShort(hits->id, &strip, &layer);
      type = (layer - 1) % 3;  /* 0..2  for u, v, w*/
      if (LayerMatchesPlane(plane, layer)) {
	if (type == 0)
	  UnionStripHitRegion(View, sect, type, strip, &ureg);
	else if (type == 1)
	  UnionStripHitRegion(View, sect, type, strip, &vreg);
	else
	  UnionStripHitRegion(View, sect, type, strip, &wreg);
      }
      hits++;  
    }  /* end for i loop over hits */
    
/* get the total region of all hit strips */

    uvwreg = Hv_UnionRegion(ureg, vreg);
    Hv_AddRegionToRegion(&uvwreg, wreg);
    
    if (ureg && vreg)
      uvreg = Hv_IntersectRegion(ureg, vreg);
    
    if (wreg && vreg)
      vwreg = Hv_IntersectRegion(wreg, vreg);
    
    if (ureg && wreg)
      uwreg = Hv_IntersectRegion(ureg, wreg);

/* get the region where at least two hits overlap */

    just2reg = Hv_UnionRegion(uvreg, vwreg);
    Hv_AddRegionToRegion(&just2reg, uwreg);

/* get the region where threee hits overlap */

    if (ureg && vreg && wreg) {
      isuvreg = Hv_IntersectRegion(ureg, vreg);
      isectreg = Hv_IntersectRegion(isuvreg, wreg);
      Hv_FillRegion(isectreg, Hv_royalBlue, region);
    }
    
    Hv_SubtractRegionFromRegion(&just2reg, isectreg);
    Hv_FillRegion(just2reg, Hv_skyBlue, region);
    Hv_SubtractRegionFromRegion(&uvwreg, just2reg);
    Hv_SubtractRegionFromRegion(&uvwreg, isectreg);
    
    Hv_FillRegion(uvwreg, fillcolor, region);

    Hv_DestroyRegion(ureg);
    Hv_DestroyRegion(vreg);
    Hv_DestroyRegion(wreg);
    Hv_DestroyRegion(uvreg);
    Hv_DestroyRegion(vwreg);
    Hv_DestroyRegion(uwreg);
    Hv_DestroyRegion(just2reg);
    Hv_DestroyRegion(uvwreg);
    Hv_DestroyRegion(isuvreg);
    Hv_DestroyRegion(isectreg);

  }  /* done with !FrameOnly */

/* now frame the hit strips */

  hits = bosEC[sect];
  for (i = 0; i < bosNumEC[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);
    type = (layer - 1) % 3;  /* 0..2  for u, v, w*/

/* note if in PS mode we want to frame AND fill */

    if (LayerMatchesPlane(plane, layer))
      DrawStripHit(View, sect, type, strip, layer, hits->adc,
		   !Hv_inPostscriptMode, region);
    hits++;  
  }  /* end for i loop over hits */
  

/* if in PS mode, frame the strips again */

  if (Hv_inPostscriptMode) {

    hits = bosEC[sect];
    for (i = 0; i < bosNumEC[sect]; i++) {
      BreakShort(hits->id, &strip, &layer);
      type = (layer - 1) % 3;  /* 0..2  for u, v, w*/

      if (LayerMatchesPlane(plane, layer))
	DrawStripHit(View, sect, type, strip, layer, hits->adc,
		     True, region);
      hits++;  
    }  /* end for i loop over hits */
  }

}


/*------ FillLAECStripHits -----------*/

static void FillLAECStripHits(Hv_View    View,
			      short      sect,   /* C index */
			      short      plane,  /* C Index */
			      Boolean    FrameOnly,
			      Hv_Region  region)
{
    EC1DataPtr    hits;
    int           i;
  
    unsigned char layer, strip;
    short         type, fillcolor;
    Hv_Region     xreg = Hv_CreateRegion();
    Hv_Region     yreg = Hv_CreateRegion();
    Hv_Region     xyreg = Hv_CreateRegion();
    Hv_Region     isectreg = Hv_CreateRegion();

    ViewData     viewdata;

    if (bosIndexEC1[sect] == 0)
	return;

    viewdata = GetViewData(View);

    if (!FrameOnly) {
	fillcolor  = viewdata->fannedechitcolor;

/* first get the hit regions */

	hits = bosEC1[sect];
	for (i = 0; i < bosNumEC1[sect]; i++) {
	    BreakShort(hits->id, &strip, &layer);
	    type = (layer - 1) % 2;  /* 0..1  for x, y*/
	    if (LayerMatchesLAECPlane(plane, layer)) {
		if (type == 0)
		    UnionLAECStripHitRegion(View, sect, type, strip, &xreg);
		else if (type == 1)
		    UnionLAECStripHitRegion(View, sect, type, strip, &yreg);
	    }
	    hits++;  
	}  /* end for i loop over hits */
    
/* get the total region of all hit strips */

	xyreg    = Hv_UnionRegion(xreg, yreg);
	isectreg = Hv_IntersectRegion(xreg, yreg);
	Hv_SubtractRegionFromRegion(&xyreg, isectreg);

 	Hv_FillRegion(isectreg, Hv_skyBlue, region);
	Hv_FillRegion(xyreg,    fillcolor, region);

	Hv_DestroyRegion(xreg);
	Hv_DestroyRegion(yreg);
	Hv_DestroyRegion(xyreg);
	Hv_DestroyRegion(isectreg);

    }  /* done with !FrameOnly */

/* now frame the hit strips */

    hits = bosEC1[sect];
    for (i = 0; i < bosNumEC1[sect]; i++) {
	BreakShort(hits->id, &strip, &layer);
	type = (layer - 1) % 2;  /* 0..1  for x, y*/

/* note if in PS mode we want to frame AND fill */

	if (LayerMatchesLAECPlane(plane, layer))
	    DrawLAECStripHit(View, sect, type, strip, layer,
			     hits->adcleft, hits->adcright,
			     !Hv_inPostscriptMode, region);
	hits++;  
  }  /* end for i loop over hits */
  

/* if in PS mode, frame the strips again */

    if (Hv_inPostscriptMode) {

	hits = bosEC1[sect];
	for (i = 0; i < bosNumEC1[sect]; i++) {
	    BreakShort(hits->id, &strip, &layer);
	    type = (layer - 1) % 2;  /* 0..1  for x, y*/
	    
	    if (LayerMatchesLAECPlane(plane, layer))
		DrawLAECStripHit(View, sect, type, strip, layer,
				 hits->adcleft, hits->adcright,
				 True, region);
	    hits++;  
	}  /* end for i loop over hits */
    }
    
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

  if (viewdata->ecplane == ECINNER)
    adcrainbow = (Hv_RainbowData *)(viewdata->InnerADCRainbowItem->data);
  else
    adcrainbow = (Hv_RainbowData *)(viewdata->OuterADCRainbowItem->data);

  vmin = adcrainbow->valmin;
  vmax = adcrainbow->valmax;

  if (adc <= vmin)
    return 0.0;

  if (adc >= vmax)
    return HDELMAX;

  valgap = vmax - vmin;

  del = (HDELMAX * ((float)(adc - vmin)))/((float)valgap);
  return del;
}

/*------- ADCToLAECDel ------- */

static float   ADCToLAECDel(Hv_View  View,
			    short    adc)
{
    ViewData          viewdata;
    Hv_RainbowData   *adcrainbow;
    short             valgap;
    float             del;
    short             vmin, vmax;
    
    viewdata = GetViewData(View);
    
    if (viewdata->ecplane == ECINNER)
	adcrainbow = (Hv_RainbowData *)(viewdata->EC1InnerADCRainbowItem->data);
    else
	adcrainbow = (Hv_RainbowData *)(viewdata->EC1OuterADCRainbowItem->data);

    vmin = adcrainbow->valmin;
    vmax = adcrainbow->valmax;
    
    if (adc <= vmin)
	return 0.0;
    
    if (adc >= vmax)
	return LAECDELMAX;
    
    valgap = vmax - vmin;
    
    del = (LAECDELMAX * ((float)(adc - vmin)))/((float)valgap);
    return del;
}

/*------- ADCToLAECColor ------- */

static short   ADCToLAECColor(Hv_View  View,
			      short    adc)
{
  ViewData          viewdata;
  Hv_RainbowData   *adcrainbow;
  short             colorgap;
  short             valgap;
  float             del;
  short             vmin, vmax, cmin, cmax;

  viewdata = GetViewData(View);

  if (viewdata->ecplane == ECINNER)
    adcrainbow = (Hv_RainbowData *)(viewdata->EC1InnerADCRainbowItem->data);
  else
    adcrainbow = (Hv_RainbowData *)(viewdata->EC1OuterADCRainbowItem->data);

  
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

  if (viewdata->ecplane == ECINNER)
    adcrainbow = (Hv_RainbowData *)(viewdata->InnerADCRainbowItem->data);
  else
    adcrainbow = (Hv_RainbowData *)(viewdata->OuterADCRainbowItem->data);

  
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

/*------- PixelEnergyToColor ------- */

static short   PixelEnergyToColor(Hv_View  View,
				  float   energy)
{
  ViewData          viewdata;
  Hv_RainbowData   *pixrainbow;
  short             colorgap;
  short             valgap;
  short             senergy;
  float             del;
  short             vmin, vmax, cmin, cmax;

  viewdata = GetViewData(View);

  pixrainbow = (Hv_RainbowData *)(viewdata->EnergyRainbowItem->data);
  energy /= pixrainbow->scalefactor;
  senergy = ((short)((int)energy));
  
  vmin = pixrainbow->valmin;
  vmax = pixrainbow->valmax;
  cmin = pixrainbow->mincolor;
  cmax = pixrainbow->maxcolor;

  if (senergy <= vmin)
    return cmin;

  if (senergy >= vmax)
    return cmax;

  valgap = vmax - vmin;
  colorgap = cmax - cmin;

  del = ((float)colorgap * ((float)(senergy - vmin)))/((float)valgap);

  return (cmin + (short)((int)del));
}


/* ---------- GetStripFPoly ----------*/

static void GetStripFPoly(Hv_View       View,
			  short         sect,
			  short         type,
			  unsigned      short  id,
			  Hv_FPoint    *fpts,
			  float         del1,
			  float         del2)

{
  ijCorners     *ij;
  short         k;
  Hv_FPoint     tpts[4];
  float         dv1, dv2, dh1, dh2, theta;

/* the ij points are stored in the local ij system --
   an upside down triangle with origin in center with
   i (yes i) vertical and j (yes j) horizontal. So to
   locate sector 1 in its usual posistion, it must be rotated
   by 90 degrees */

  if (sect < 10)
    theta = (90.0 - (sect*60.0))*Hv_DEGTORAD;
  else {
    theta = 0.0;
    sect -= 10;
  }

  if (type == USTRIP)
    ij = ShowerCounters[sect].Ustrips;
  else if (type == VSTRIP)
    ij = ShowerCounters[sect].Vstrips;
  else
    ij = ShowerCounters[sect].Wstrips;

/* get the world coordinates from the ij */

  for (k = 0; k < 4; k++)
    fPointFromij(fpts+k, ij[id].corners[k]); 

/* now rotate to take into account what sector */

  if (fabs(theta) > 0.01)
    Hv_RotateFPoints(theta, fpts, 4);

  if ((del2 < 1.0) || ((del2-del1) < 0.1))
    return;

  if (type == VSTRIP) {
    del1 += 6.0;
    del2 += 6.0;
  }

  dh1 = fpts[1].h - fpts[2].h;
  dv1 = fpts[1].v - fpts[2].v;
  
  dh2 = fpts[0].h - fpts[3].h;
  dv2 = fpts[0].v - fpts[3].v;
  
  if ((fabs(dh1) + fabs(dv1)) > (fabs(dh2) + fabs(dv2)))
    theta = atan2(dv1, dh1);
  else
    theta = atan2(dv2, dh2);
  
  ExtendFPoint(fpts+2, fpts+1, tpts+2, theta, del1);
  ExtendFPoint(fpts+2, fpts+1, tpts+1, theta, del2);
  ExtendFPoint(fpts+3, fpts,   tpts+3, theta, del1);
  ExtendFPoint(fpts+3, fpts,   tpts,   theta, del2);
  
  for (k = 0; k < 4; k++) {
    fpts[k].h = tpts[k].h;
    fpts[k].v = tpts[k].v;
  }
  
}


/* ---------- GetLAECStripFPoly ----------*/

static void GetLAECStripFPoly(Hv_View       View,
			      short         sect,
			      short         type,
			      unsigned      short  id,
			      Hv_FPoint    *fpts,
			      float         del1,
			      float         del2,
			      Boolean       left)

{

    if (type == XSTRIP) {
	GetLAECPoints(sect, fpts, LAECMARGIN, id, -1, del1, del2, left);
    }


    else if (type == YSTRIP) {
	GetLAECPoints(sect, fpts, LAECMARGIN, -1, id, del1, del2, left);
    }

}


/*------ ExtendFPoint ------*/

static void ExtendFPoint(Hv_FPoint *fp1,
			 Hv_FPoint *fp2,
			 Hv_FPoint *fp,
			 float     theta,
			 float     del) /* fixed amount in cm */

{
  fp->h = fp2->h + del*cos(theta);
  fp->v = fp2->v + del*sin(theta);
}

/* ---------- GetStripCorners ----------*/

static void GetStripCorners(Hv_View        View,
			    short           sect,
			    short           type,
			    unsigned short  id,
			    Hv_Point       *points)

/*Used by FANNED view routines*/

{
  Hv_FPoint     fpts[4];
  
  GetStripFPoly(View, sect, type, id, fpts, 0.0, 0.0);
  Hv_WorldPolygonToLocalPolygon(View, 4, points, fpts);
}

/* ---------- GetLAECStripCorners ----------*/

static void GetLAECStripCorners(Hv_View        View,
				short           sect,
				short           type,
				unsigned short  id,
				Hv_Point       *points)

/*Used by FANNED view routines*/

{
  Hv_FPoint     fpts[4];
  
  GetLAECStripFPoly(View, sect, type, id, fpts, 0.0, 0.0, True);
  Hv_WorldPolygonToLocalPolygon(View, 4, points, fpts);
}

#define EXPAND_SIN   0.585    /*sin alpha (35.08 deg) = 0.585 */  
#define EXPAND_COS   0.811    /*cos alpha (35.08 deg) = 0.811*/

/* --------- GetFannedShowerPoints ---- */

static void GetFannedShowerPoints (short   sect,
				   Hv_FPoint *fP,
				   float   ExpansionFactor)

/* gets the screen cordinates for the front plane of shower counter
   USE FRONT PLANE POINTS FOR DRAWING EITHER/BOTH PLANES */

{
  float  expdist;
  float  expv, exph;
  float  theta;
  short  k;

/* 419 cm is the nominal triangle altitude */

  expdist = 419.0*ExpansionFactor;
  expv = EXPAND_SIN*expdist;
  exph = EXPAND_COS*expdist;

  theta = (90.0 - (sect*60.0))*Hv_DEGTORAD;

  for (k = 0; k < 3; k++)
    fPointFromij(&(fP[k]), ShowerCounters[sect].cFront[k]); 

/* expand the triangles a bit */
  
  fP[0].v -= expdist;
  fP[1].h += exph;    fP[1].v += expv;
  fP[2].h -= exph;    fP[2].v += expv;

  Hv_RotateFPoints(theta, fP, 3);
}

/* --------- GetLAECPoints ---- */

static void GetLAECPoints (short     sect,
			   Hv_FPoint *fP,
			   float     shrink,
			   int       xstrip,
			   int       ystrip,
			   float     del1,
			   float     del2,
			   Boolean   left)

/* gets the screen cordinates for the front plane of LAEC
   USE FRONT PLANE POINTS FOR DRAWING EITHER/BOTH PLANES */

/* xstrip [0..23] ystrip [0..39] */

{
  float  theta, dx, dy, del;
  Boolean extend;

  float  l = -865.0 + shrink;
  float  r = -590.0 - shrink;
  float  t =  300.0 - shrink;
  float  b = -300.0 + shrink;

  del = del2 - del1;
  extend = (del > 1.0);

  if (xstrip > -1) {
      dx = (r - l)/24.0;
      l = l + (xstrip*dx);
      r = l + dx;
      if (extend) {
	  if (left) {
	      b = b - del2;
	      t = b + del;
	  }
	  else {
	      b = t + del1;
	      t = b + del;
	  }
      }

  }

  if (ystrip > -1) {
      dy = (b - t)/40.0;
      b = b - (ystrip*dy);
      t = b - dy;
      if (extend) {
	  if (left) {
	      l = l - del2;
	      r = l + del;
	  }
	  else {
	      l = r + del1;
	      r = l + del;
	  }
      }
  }


  fP[0].h = l;    fP[0].v = b;
  fP[1].h = r;    fP[1].v = b;
  fP[2].h = r;    fP[2].v = t;
  fP[3].h = l;    fP[3].v = t;

/* rotate to the proper sector */

  theta = -(sect*60.0)*Hv_DEGTORAD;
  Hv_RotateFPoints(theta, fP, 4);
}

/*---------- DrawECTriangle --------*/

static void DrawECTriangle(Hv_Item  Item,
			   Hv_Region region)

/* a FANNED VIEW routine */

{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  short                 sect;
  Hv_Point             *poly;
  Hv_Point              poly2[3];
  Hv_Region             hrreg = NULL;
  Hv_Region             sreg = NULL;
  Hv_Region             treg = NULL;
  Hv_String            *str;
  char                  text[5];
  short                 dx, dy, sw, sh;

  viewdata = GetViewData(View);
  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;


  poly = wpoly->poly;
  sect = Item->user2;

/* poly2 will hold the active polygon */

  ActiveTrianglePoly(Item, poly2);
  sreg = Hv_CreateRegionFromPolygon(poly2, 3);

  hrreg = Hv_ClipHotRect(View, 1);

  treg = Hv_SubtractRegion(hrreg, sreg);
  Hv_SetClippingRegion(treg);

  Hv_FillPolygon(poly, 3, True, Item->color, Hv_black);


/* draw sector label near center */

  sprintf(text, "%-d", sect+1);
  str = Hv_CreateString(text);
  str->font = Hv_timesBold14;
  str->strcol = Hv_white;
  Hv_CompoundStringDimension(str, &sw, &sh);


  switch (sect) {
      case 0:
	  dx = 23;
	  dy = sh/2;
	  break;

      case 1:
	  dx = 15;
	  dy = 2*sh - 4;
	  break;

      case 2:
	  dx = -12;
	  dy = 2*sh - 4;
	  break;

      case 3:
	  dx = -23 + sw;
	  dy = sh/2;
	  break;

      case 4:
	  dx = -12;
	  dy = -sh + 2;
	  break;

      default:
	  dx = 15;
	  dy = -sh + 2;
	  break;

  }


  Hv_DrawCompoundString(poly[0].x-dx, poly[0].y-dy, str);
  Hv_DestroyString(str);


  Hv_SetClippingRegion(hrreg);

/* fill in the green active  polygon? */

  if (PlaneBorderOnly)
    Hv_FramePolygon(poly2, 3, Hv_black);
  else
    Hv_FillPolygon(poly2, 3, True, viewdata->fannedeccolor, Hv_black);
  
  Hv_SetLineWidth(2);

  switch (sect) {
  case 0:
    Hv_DrawLine(poly[0].x, poly[0].y, poly[2].x,  poly[2].y,  Hv_white);
    Hv_DrawLines(poly2, 3, Hv_white);
    break;

  case 1:
    Hv_DrawLine(poly[0].x, poly[0].y, poly[1].x,  poly[1].y,  Hv_white); 
    Hv_DrawLine(poly[0].x, poly[0].y, poly[2].x,  poly[2].y,  Hv_white);
    Hv_DrawLine(poly2[1].x, poly2[1].y, poly2[2].x,  poly2[2].y,  Hv_white);
   break;

  case 2:
    Hv_DrawLine(poly[0].x, poly[0].y, poly[1].x,  poly[1].y,  Hv_white);
    Hv_DrawLine(poly2[1].x, poly2[1].y, poly2[2].x,  poly2[2].y,  Hv_white);
    Hv_DrawLine(poly2[2].x, poly2[2].y, poly2[0].x,  poly2[0].y,  Hv_white);
    break;

  case 3:
    Hv_DrawLines(poly, 3, Hv_white);
    Hv_DrawLine(poly2[0].x, poly2[0].y, poly2[2].x,  poly2[2].y,  Hv_white);
    break;

  case 4:
    Hv_DrawLine(poly[1].x, poly[1].y, poly[2].x,  poly[2].y,  Hv_white);
    Hv_DrawLine(poly2[0].x, poly2[0].y, poly2[1].x,  poly2[1].y,  Hv_white);
    Hv_DrawLine(poly2[0].x, poly2[0].y, poly2[2].x,  poly2[2].y,  Hv_white);
    break;

  case 5:
    Hv_DrawLine(poly[1].x, poly[1].y, poly[2].x,  poly[2].y,  Hv_white);
    Hv_DrawLine(poly[2].x, poly[2].y, poly[0].x,  poly[0].y,  Hv_white);
    Hv_DrawLine(poly2[0].x, poly2[0].y, poly2[1].x,  poly2[1].y,  Hv_white);
    break;
  }
  
  Hv_SetLineWidth(0);


  if (!PlaneBorderOnly)
    switch (View->tag) {
    case CALORIMETER:
      if (viewdata->displayU)
	FrameECStrips(View, sect, USTRIP, viewdata->fannedecstripcolor);
      
      if (viewdata->displayV)
	FrameECStrips(View, sect, VSTRIP, viewdata->fannedecstripcolor);
      
      if (viewdata->displayW)
	FrameECStrips(View, sect, WSTRIP, viewdata->fannedecstripcolor);
      
      break;
    }
  
  Hv_SetClippingRegion(region);
  Hv_DestroyRegion(hrreg);
  Hv_DestroyRegion(sreg);
  Hv_DestroyRegion(treg);
}

/*---------- DrawLAECItem --------*/

static void DrawLAECItem(Hv_Item  Item,
			 Hv_Region region)

/* a FANNED VIEW routine */

{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  short                 sect;
  Hv_Point             *poly;
  Hv_Point              poly2[4];
  Hv_Region             hrreg = NULL;
  Hv_Region             sreg = NULL;
  Hv_Region             treg = NULL;

  viewdata = GetViewData(View);
  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;


  poly = wpoly->poly;
  sect = Item->user2;

/* poly2 will hold the active polygon */

  ActiveLAECPoly(Item, poly2);
  sreg = Hv_CreateRegionFromPolygon(poly2, 4);
  

  hrreg = Hv_ClipHotRect(View, 1);

  treg = Hv_SubtractRegion(hrreg, sreg);
  Hv_SetClippingRegion(treg);

  Hv_FillPolygon(poly, 4, True, Item->color, Hv_black);


  Hv_SetClippingRegion(hrreg);

/* fill in the green active  polygon? */

  if (!PlaneBorderOnly) {
    Hv_FillPolygon(poly2, 4, False, viewdata->fannedeccolor, Hv_black);

    if (viewdata->displayX)
	FrameLAECStrips(View, sect, XSTRIP, viewdata->fannedecstripcolor);
    
    if (viewdata->displayY)
	FrameLAECStrips(View, sect, YSTRIP, viewdata->fannedecstripcolor);
  }
  Hv_FramePolygon(poly2, 4, Hv_black);


  Hv_SetLineWidth(2);

  Hv_DrawLine(poly[0].x,  poly[0].y,  poly[1].x,   poly[1].y,   Hv_white);
  Hv_DrawLine(poly[1].x,  poly[1].y,  poly[2].x,   poly[2].y,   Hv_white);
  Hv_DrawLine(poly2[2].x, poly2[2].y, poly2[3].x,  poly2[3].y,  Hv_white);
  Hv_DrawLine(poly2[0].x, poly2[0].y, poly2[3].x,  poly2[3].y,  Hv_white);
  
  Hv_SetLineWidth(0);

  Hv_SetClippingRegion(region);
  Hv_DestroyRegion(hrreg);
  Hv_DestroyRegion(sreg);
  Hv_DestroyRegion(treg);
}



/*------ ActiveTrianglePoly --------*/

static void ActiveTrianglePoly(Hv_Item Item, Hv_Point *poly)

{
    short     sect;
    Hv_FPoint fpts[3];

    sect = Item->user2;
    GetFannedShowerPoints(sect, fpts, 0.05);
    Hv_WorldPolygonToLocalPolygon(Item->view, 3, poly, fpts);

}


/*------ ActiveLAECPoly --------*/

static void ActiveLAECPoly(Hv_Item Item, Hv_Point *poly)

{
    short     sect;
    Hv_FPoint fpts[4];

    sect = Item->user2;
    GetLAECPoints(sect, fpts, LAECMARGIN, -1, -1, 0.0, 0.0, False);
    Hv_WorldPolygonToLocalPolygon(Item->view, 4, poly, fpts);

}


/*------ UnionLAECStripHitRegion -----*/

static void UnionLAECStripHitRegion(Hv_View     View,
				    short       sect,
				    short       type,
				    short       ID,
				    Hv_Region  *region)
/*
   sect     C INDEX
   type     XSTRIP, YSTRIP
   ID       NOT a C Index
*/

{
  Hv_Point      poly[4];

  GetLAECStripCorners(View, sect, type, ID-1, poly);
  Hv_UnionPolygonWithRegion(poly, 4, region);
}

/*------ UnionStripHitRegion -----*/

static void UnionStripHitRegion(Hv_View   View,
				short    sect,
				short    type,
				short    ID,
				Hv_Region  *region)
/*
   sect     C INDEX
   type     USTRIP, VSTRIP, or WSTRIP
   ID       NOT a C Index
*/

{
  Hv_Point      poly[4];

  GetStripCorners(View, sect, type, ID-1, poly);
  Hv_UnionPolygonWithRegion(poly, 4, region);
}


/* ---------  ijFromfPoint --------- */

static void  ijFromfPoint(Hv_FPoint  fp,
			  ijPoint    *ij)

{
  ij->i = fp.v - FACTOR_V;     /*in local ij system, i is VERTICAL*/
  ij->j = fp.h - FACTOR_H;     /*in local ij system, j is HORIZONTAL*/
}


/* ---------  fPointFromij --------- */

static void  fPointFromij(Hv_FPoint *fp,
			  ijPoint    ij)

{
  fp->v = ij.i + FACTOR_V;     /*in local ij system, i is VERTICAL*/
  fp->h = ij.j + FACTOR_H;     /*in local ij system, j is HORIZONTAL*/
}

/* ------ ijktoxyz -------- */

static void ijktoxyz(Shower_Package *sp,
		     short      sect,
		     float      i,
		     float      j,
		     float      k,
		     Cart3D     *X)

/* converts local i, j, k to a 3D Cartesian vector in the CLAS system.
  The local ij system is an upside down triangle with i vertical and j horizontal */

{
  float      xo, zo, st, ct, theta;
  Hv_FPoint  fp;

/*First convert to local xyz, with x positive in "top" sector */

  ct = sp->CosT;
  st = fabs(sp->SinT);
  zo = sp->R0.z;
  xo = fabs(sp->R0.x);

  X->x = xo + ct*i + st*k;
  X->y = j;
  X->z = zo - st*i + ct*k;

/* now rotate about z axis*/

  theta = sect*60.0*Hv_DEGTORAD;

  if (theta > 0.01) {
    fp.h = X->y;
    fp.v = X->x;
    Hv_RotateFPoint(-theta, &fp);
    X->y = fp.h;
    X->x = fp.v;
  }
  
} 


/*------ LayerMatchesPlane -----*/

static Boolean LayerMatchesPlane(short plane,
				 short layer)

{
  if ((plane == ECINNER) && (layer > 0) && (layer < 4))
    return True;
  else if ((plane == ECOUTER) && (layer > 3) && (layer < 7))
    return True;
  else if (plane == ECBOTH)
    return True;
  else
    return False;
}
/*------ LayerMatchesLAECPlane -----*/

static Boolean LayerMatchesLAECPlane(short plane,
				 short layer)

{
  if ((plane == ECINNER) && (layer > 0) && (layer < 3))
    return True;
  else if ((plane == ECOUTER) && (layer > 2) && (layer < 5))
    return True;
  else if (plane == ECBOTH)
    return True;
  else
    return False;
}

#undef  FACTOR_H
#undef  FACTOR_V
#undef  HDEL1
#undef  HDEL2
#undef  HDELMAX
#undef  HDELMIN

#undef  I_MIN
#undef  I_MAX
#undef  J_MIN
#undef  J_MAX
#undef  IJ_SLOPE
#undef  IJ_INTERCEPT

