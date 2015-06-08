/*
----------------------------------------------------
-							
-  File:    shower.c				
-							
-  Summary:						
-           Shower Counter Drawing 
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

#define  MIN_T_INTERCEPT  0.001
#define  MAX_T_INTERCEPT  0.999



/* ------ local prototypes -------*/

static void UVWSeparationPolygon(Hv_Item     Item,
				 short       type,
				 Hv_Point    *poly);

static void DrawStripIntersections(Hv_View  View,
				   short     sect,
				   Hv_Region region);

static Boolean StripIntersectionPoly(Hv_View  View,
				     short    sect,
				     short    plane,
				     short    type,
				     short    ID,
				     Hv_Point *poly);

static Boolean ijkInterceptParameter(Shower_Package  *sp,
				     ijPoint        *P0,
				     ijPoint        *P1,
				     float          tanphi,
				     float          k,
				     float          *t,
				     ijPoint        *Pint);

static Boolean  StripPlaneIntersection(Hv_View View,
				       Shower_Package *sp,
				       short sect,
				       float tanphi,
				       float k,
				       float dk,
				       short type,
				       short id,
				       Hv_Point *points);

static void GetSectorViewECFPolygon(short     sect,
				    short     plane,
				    Hv_FPoint *fpts);
 

static Hv_Item MallocSectorViewEC(Hv_View  View,
				  int      sect,
				  short    plane);

static void DrawShowerItem(Hv_Item  Item,
			   Hv_Region region);

static void EditShowerItem(Hv_Event   hvevent);

static Boolean   FrameOnly = False;

Boolean checkECData(short sect,
		    ECDataPtr hits) {
    return True;
}



/*------ GetSectViewShowerHit ------------*/

extern void GetSectViewShowerHit(Hv_Item  Item,
                                 Hv_Point pp,
				 short    sect,
				 short    plane,
				 short   *type,
				 short   *ID,
				 short   *tdc,
				 short   *adc)

{
  short         i;
  Hv_Point      poly[4];
  ECDataPtr     hits;
  unsigned char strip, layer;

/* set up for failure */

  *type = -1;
  *ID = -1;
  *tdc = -1;
  *adc = -1;

/* we know what sect and plane, lets see if we can 
   find the type (u, v, w)  */


  for (i = USTRIP; i <= WSTRIP; i++) {
    UVWSeparationPolygon(Item, i, poly);
    if (Hv_PointInPolygon(pp, poly, 4)) {
      *type = i;
      break;
    }
  }

  if (*type < 0) {
    return;
  }

/* if here we have a valid type; try to get and ID.
   recall that ID is not a C Index */

  for (i = 1; i <= NUM_STRIP; i++)
    if (StripIntersectionPoly(Item->view, sect, plane, *type, i, poly))
      if (Hv_PointInPolygon(pp, poly, 4)) {
	*ID = i;
	break;
      }
  
  if (*ID < 0)
    return;

/* now look for a matching hit */

  hits = bosEC[sect];

  for (i = 0; i < bosNumEC[sect]; i++) {
    BreakShort(hits->id, &strip, &layer);

/* fix bug: layer must be appropriate for plane */

    if (((plane == ECINNER) && (layer < 4)) || ((plane == ECOUTER) && (layer > 3))) {
	if (*type == ((layer - 1) % 3))
	    if (*ID == strip) {
		*tdc = (short)(hits->tdc);
		*adc = (short)(hits->adc);
		return;
	    }
    }
    hits++;
  }
}


/*-------- DrawShowerHits -----*/

void DrawShowerHits(Hv_View   View,
		    short     sect,
		    Hv_Region region)

/*----------------------------------
On the given view and in the given sector,
this procedure will draw the hits 
------------------------------------*/

{
  ViewData     viewdata;

  Hv_RainbowData *data;

  unsigned char strip, layer;
  short         type, plane;
  int           i;
  Hv_Point      poly[4];
  ECDataPtr     hits;
  short         underflow, overflow, fillcolor;
  int           val;

  if (bosIndexEC[sect] == 0)
    return;

  if (View->tag != SECTOR)
      return;

  viewdata = GetViewData(View);

  data = (Hv_RainbowData *) (viewdata->EnergyRainbowItem->data);

  underflow = Hv_white;
  overflow = Hv_red;

  hits = bosEC[sect];

  for (i = 0; i < bosNumEC[sect]; i++) {
      BreakShort(hits->id, &strip, &layer);
      layer--;  /* 0..5*/
      plane = layer/3; /*0..1*/
      type = layer % 3;  /* 0..2  for u, v, w*/

      val = (int)(hits->adc);

      if (val <= data->valmin)
	  fillcolor = underflow;
      else if (val >= data->valmax)
	  fillcolor = overflow;
      else
	  fillcolor = CountToColor(val, data, viewdata->algorithm);


/* look for bad IDS (note strip is 1..36) */

      if ((strip < 1) || (strip > 36) || (layer > 5))
	  fprintf(stderr, "BAD ID in shower counter sect = %d layer = %d  strip = %d\n",
		  sect+1, layer+1, strip);
      else {
	  if (StripIntersectionPoly(View, sect, plane, type, strip, poly))
	      Hv_FillPolygon(poly, 4, True, fillcolor, Hv_black);
      }

      hits++;
  }
}


/*------ MallocShower --------*/

Hv_Item MallocShower(Hv_View  View,
		     int      sect,  /*C Index*/
		     short    ID,
		     Hv_Item  parent)


/* ID is a

   sect is a C Index [0..5]  */

{
  Hv_Item      Item;

  switch (View->tag){
  case SECTOR:
    Item = MallocSectorViewEC(View, sect, ID);
    break;

  }
  return  Item;
}


/*-------- SetShowerGeometrySector -----*/

void SetShowerGeometrySector(Hv_View View)

{
  short                sect;
  Hv_Item              temp;
  Hv_WorldPolygonData  *wpoly;
  ViewData             viewdata;
  int                  i;

  if (View->tag != SECTOR)
    return;

  viewdata = GetViewData(View);

  for (i = 0; i < NUM_SHOWER2; i++)
    if ((temp = viewdata->ForwardECSectViewItems[i]) != NULL) {
      sect = SetItemSector(temp);

/* recall that the "plane" is stored in user1 */

      wpoly = (Hv_WorldPolygonData *)(temp->data);
      GetSectorViewECFPolygon(sect, (short)(temp->user1), wpoly->fpts);
      temp->fixregion(temp);
    }

    if ((temp = viewdata->SectViewEC1Items[0]) != NULL) {
	sect = SetItemSector(temp);

/* recall that the "plane" is stored in user1 */

	wpoly = (Hv_WorldPolygonData *)(temp->data);
	GetSectorViewEC1FPolygon(sect, wpoly->fpts);
	temp->fixregion(temp);
    }
}

/*--------- TotalECADCSum --------*/

int TotalECADCSum(short sect) {

/* sect is a C index */

    ECDataPtr     hits;

    int   sum = 0;
    int   i;

    hits = bosEC[sect];

    for (i = 0; i < bosNumEC[sect]; i++) {
	sum  += (int)(hits->adc);
	hits++;
    }

    return sum;
}



/*------- PixelFromUVW ------*/

short  PixelFromUVW(short u,
		    short v,
		    short w)

{
  return(u*(u-1) + v - w + 1);
}

/*------- UVWFromPixel ---------*/


void  UVWFromPixel(short *u,
		   short *v,
		   short *w,
		   short pixel)

/* *u, *v, *w:  [returns [1..36] */

{
  unsigned short i, r;
  float root;
  
  root = sqrt((double)pixel - 1.0);


  r = (unsigned short) (Hv_nint((float)((int)root)));
  i = pixel - (r*r);

  *w = ((short)(NUM_STRIP + 1  - (i + 1))) / 2;
  *v = *w + i - r - 1;
  *u = r + 1;
}

/*------ MallocSectorViewEC -------*/

static Hv_Item MallocSectorViewEC(Hv_View  View,
				  int      sect,
				  short    plane)

{
  Hv_Item    Item;
  Hv_FPoint  *fpts;
  char       balloon[60];

  fpts = (Hv_FPoint *)Hv_Malloc(4*sizeof(Hv_FPoint));
  GetSectorViewECFPolygon((short)sect, plane, fpts);

  if (plane == ECINNER)
    sprintf(balloon, "Inner Forward Shower Counter");
  else
    sprintf(balloon, "Outer Forward Shower Counter");

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SECTORSHOWERITEM,
			 Hv_NUMPOINTS,    4,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditShowerItem,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    FixWPolygonBasedRegion,
			 Hv_USER1,        (int)plane,
			 Hv_USER2,        (int)sect,
			 Hv_BALLOON,      balloon,
			 NULL);
  
  Item->standarddraw = DrawShowerItem;
  Item->type = Hv_USERITEM;
  return Item;
}


/*---------- DrawShowerItem --------*/

static void DrawShowerItem(Hv_Item  Item,
			   Hv_Region region)
{
  Hv_View               View = Item->view;
  ViewData              viewdata;
  Hv_WorldPolygonData  *wpoly;
  int                   plane;
  Hv_Point              poly[4];

  viewdata = GetViewData(View);

  if (!(viewdata->displayEC))
    return;

  plane = Item->user1;  /* stored here as a C index */

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;
  
  if (FrameOnly)
    Hv_FramePolygon(wpoly->poly, 4, ecframecolor[plane]);
  else
    Hv_FillPolygon(wpoly->poly, 4, True, ecfillcolor[plane],
		   ecframecolor[plane]);

/* now the lines marking the uvw separation */
  
  UVWSeparationPolygon(Item, VSTRIP, poly);
  Hv_FramePolygon(poly, 4, ecframecolor[plane]);

/*  DrawStripIntersections(Item->view, (short)(Item->user2), region); */
}


/*-------- UVWSeparationPolygon -------*/

static void UVWSeparationPolygon(Hv_Item     Item,
				 short       type,
				 Hv_Point    *poly)

/* given the sector view shower item "Item",
   this will return the "internal" screen
   poly corresponding to the u, v or
   w region as specified in type */


{
  Hv_FPoint            *fpts;
  Hv_FPoint             fpoly[4];
  Hv_WorldPolygonData  *wpoly;
  float                 dh3b, dv3b;
  float                 dh3a, dv3a;
  float                 delhb1, delvb1;
  float                 delha1, delva1;
  float                 delhb2, delvb2;
  float                 delha2, delva2;

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

  fpts = wpoly->fpts;

  dh3a = (fpts[1].h - fpts[0].h)/3.0;
  dv3a = (fpts[1].v - fpts[0].v)/3.0;

  dh3b = (fpts[2].h - fpts[3].h)/3.0;
  dv3b = (fpts[2].v - fpts[3].v)/3.0;

  switch (type) {
  case USTRIP:
    delha1 = 0.0;       delha2 = 2.0*dh3a;
    delva1 = 0.0;       delva2 = 2.0*dv3a;
    delhb1 = 0.0;       delhb2 = 2.0*dh3b;
    delvb1 = 0.0;       delvb2 = 2.0*dv3b;
    break;

  case VSTRIP:
    delha1 = dh3a;       delha2 = dh3a;
    delva1 = dv3a;       delva2 = dv3a;
    delhb1 = dh3b;       delhb2 = dh3b;
    delvb1 = dv3b;       delvb2 = dv3b;
    break;

  case WSTRIP:
    delha1 = 2.0*dh3a;   delha2 = 0.0;
    delva1 = 2.0*dv3a;   delva2 = 0.0;
    delhb1 = 2.0*dh3b;   delhb2 = 0.0;
    delvb1 = 2.0*dv3b;   delvb2 = 0.0;
    break;

  default:
    fprintf(stderr, "error in uvw separation poly\n");
    return;

  }

  fpoly[0].h = fpts[0].h + delha1;
  fpoly[0].v = fpts[0].v + delva1;
  fpoly[1].h = fpts[1].h - delha2;
  fpoly[1].v = fpts[1].v - delva2;

  fpoly[3].h = fpts[3].h + delhb1;
  fpoly[3].v = fpts[3].v + delvb1;
  fpoly[2].h = fpts[2].h - delhb2;
  fpoly[2].v = fpts[2].v - delvb2;

  MidplanePolyToProjectedPoly(Item->view, fpoly, 4);
  Hv_WorldPolygonToLocalPolygon(Item->view, 4, poly, fpoly);
}



/*-------- EditShowerItem -------*/

static void   EditShowerItem(Hv_Event hvevent)

/* item edit for ShowerITEMs  */

{
    EditDetectorColors();
}

/* --------- GetStripTypekValue ------ */

void GetStripTypekValue(short      type,
			short      plane,
			float      *k,
			float      *dk)

/* Used (mostly) for SECTOR view hit display. Determines an effective k coordinate
   for a given strip (which depends only on the type (u, v, w) and the
   plane (front, rear). Also returns a dk, to give "thickness"
   to the displayed strip 

   type:  U=0, V=1, W=2  */

{
  float  DeltaK;
  float  thickness;

  thickness = ShowerCounters[0].showers[plane].thickness;

  *dk = thickness/3.4; 
  
  if (plane == ECINNER)
    DeltaK = 0.0;
  else   /* assume rear plane */
    DeltaK = ShowerCounters[0].DeltaK;

/* next statement relies on ECStripType being an
   enumerated type */

  *k = 1.05*DeltaK + type*thickness/3.2;
}

/*-------- GetSectorViewECPolygon --------*/

void GetSectorViewECPolygon(Hv_View   View,
			    short     sect,
			    short     plane,
			    Hv_Point  *poly)

/* plane and sect are C indices */

{
  Hv_FPoint  fpts[4];
  GetSectorViewECFPolygon(sect, plane, fpts);
  MidplanePolyToProjectedPoly(View, fpts, 4);
  Hv_WorldPolygonToLocalPolygon(View, 4, poly, fpts);
}

/*-------- GetSectorViewECFPolygon --------*/

static void GetSectorViewECFPolygon(short     sect,
				    short     plane,
				    Hv_FPoint *fpts)

/* plane and sect are C indices */


/* NOTE: THIS DOES NOT RETURN PHI CORRECTED POLY */

{
  Shower   sh;
  int      i;

  sh = ShowerCounters[sect].showers[plane];

  for (i = 0; i < 4; i++) {
    fpts[i].h = sh.P[i].h;
    fpts[i].v = sh.P[i].v;
  }
}

/*------- StripIntersectionPoly -------*/

static Boolean StripIntersectionPoly(Hv_View  View,
				     short    sect,
				     short    plane,
				     short    type,  /* u v w */
				     short    ID,
				     Hv_Point *poly)

/* returns the intersection of the strip with the viewing
   plane as specified by the view's phi. For some combinations
   the strip wil NOT intersect the plane, in which case this
   routine returns False.

   This is no more than a simpler interface to the routine
   StripPlaneIntersection

   sect  and plane are C Indices
   ID is NOT a C Index */


{
  ViewData        viewdata = GetViewData(View);
  float           tanphi;
  float           k, dk;
  Shower_Package *sp;

  ID--;   /* now it's a C Index */

  tanphi = tan(Hv_DEGTORAD*viewdata->dphi);

  GetStripTypekValue(type, plane, &k, &dk);
  sp = &(ShowerCounters[sect]);
  
  return StripPlaneIntersection(View, sp, sect, tanphi, k, dk, type, ID, poly);
}


/*--------- StripPlaneIntersection --------- */

static Boolean  StripPlaneIntersection(Hv_View View,
				       Shower_Package *sp,
				       short sect,
				       float tanphi,
				       float k,
				       float dk,
				       short type,
				       short id,
				       Hv_Point *points)

/* Obtains the intersection of the strip with the plane
   specified by tan(phi) = tanphi = constant. One segment
   should come from corners 0 and 3 (t0), the other from corners
   1 and 2 (t1). The parameterization
   is:
          i = io + t(i1 - i0)
          j = jo + t(j1 - j0)
   
    thus if t < 0 or t > 1 the intersection occurs beyond the segment

Three posibilities:

   1) t0, t1 in-range  return (True)

   2) t0, t1 out-of-range  return (False)

   3) One in-range, one out-of-range, in which case:
       a) return True
       b) use the one good line
       c) if the bad t < 0, use segment 0-1 instead of the bad segment
       d) if the bad t > 0, use segment 2-3 instead of the bad segment

  If an intersection is found, the four points making the polygon corresponding
  to the intersection are returned in "points"

     sect     C index
     tanphi   defines the plane to intesect with
     k        k value for strip
     dk       dk value for strip
     type     Ustrip=0, Vstrip=1 or Wstrip=2
     id       which strip (C INDEX)
     *points  screen coordinates of intersection polygon

*/

{
  ijCorners    *ij;
  Boolean      Intersects;  /*returned*/
  float        t0, t1;
  Boolean      t0ok, t1ok;
  ijPoint      *P;        
  ijPoint      Pint03, Pint12;
  Hv_FPoint    fp[4];

  if (type == U_PLANE)
    ij = &(sp->Ustrips[id]);
  else if (type == V_PLANE)
    ij = &(sp->Vstrips[id]);
  else
    ij = &(sp->Wstrips[id]);

  P = &(ij->corners[0]);
  
  t0ok = ijkInterceptParameter(sp, P,     P + 3, tanphi, k, &t0, &Pint03);
  t1ok = ijkInterceptParameter(sp, P + 1, P + 2, tanphi, k, &t1, &Pint12);
  Intersects = t0ok || t1ok;     /*as long as one is ok*/
  
  if (Intersects) {
    if (!t0ok)    /* 0-3 did NOT intesect */
      if (t0 < MIN_T_INTERCEPT)
	t0ok = ijkInterceptParameter(sp, P,     P + 1, tanphi, k, &t0, &Pint03);
      else
	t0ok = ijkInterceptParameter(sp, P + 2, P + 3, tanphi, k, &t0, &Pint03);

    if (!t1ok)    /* 1-2 did NOT intesect */
      if (t1 < MIN_T_INTERCEPT)
	t1ok = ijkInterceptParameter(sp, P,     P + 1, tanphi, k,  &t1, &Pint12);
      else
	t1ok = ijkInterceptParameter(sp, P + 2, P + 3, tanphi, k, &t1, &Pint12);

    Intersects = (t0ok && t1ok);

/* now get the four corresponding fpoints */


    if (Intersects) {
      ijktofp(sp, sect, Pint03.i, Pint03.j, k, &fp[0]);
      ijktofp(sp, sect, Pint12.i, Pint12.j, k, &fp[1]);
      ijktofp(sp, sect, Pint12.i, Pint12.j, k+dk, &fp[2]);
      ijktofp(sp, sect, Pint03.i, Pint03.j, k+dk, &fp[3]);

      MidplanePolyToProjectedPoly(View, fp, 4);
      Hv_WorldPolygonToLocalPolygon(View, 4, points, fp);
      
    }
  }

  return Intersects;
}


/* ------ ijktofp -------- */

void  ijktofp(Shower_Package *sp,
	      short         sect,
	      float         i,
	      float         j,
	      float         k,
	      Hv_FPoint     *fp)
     
/* converts local i, j, k to  CLAS system.
  The local ij system is an upside down triangle with
  i vertical and j horizontal */

{
  float   xo, zo, st, ct;

/*First convert  in "top" sector */

  ct = sp->CosT;
  st = fabs(sp->SinT);
  zo = sp->R0.z;
  xo = fabs(sp->R0.x);

  fp->v = xo + ct*i + st*k;
  fp->h = zo - st*i + ct*k;
  
  if (sect > 2)
    fp->v = -(fp->v);
  
}


/* ------ ijkInterceptParameter ----- */

static Boolean ijkInterceptParameter(Shower_Package  *sp,
				     ijPoint        *P0,
				     ijPoint        *P1,
				     float          tanphi,
				     float          k,
				     float          *t,
				     ijPoint        *Pint)

/* calculates the value of the parameter t for which the
   line connecting (P0->i, P0->j, k) to (P1->i, P1->j, k) intersect
   the plane tan(phi) = tanphi = constant. The parameterization
   is:
          i = io + t(i1 - i0)
          j = jo + t(j1 - j0)
   
    thus if t < 0 or t > 1 the intersection occurs beyond the segment

RETURNS:  True if t in (0, 1), False otherwise 

    k;        common k value
    *t;       intersect parameter
    *Pint;    intersection point, calculated only if t in range

*/

{
  float    denom, deli, delj, st, xo;
  Boolean  intersects;

  deli = P1->i - P0->i;
  delj = P1->j - P0->j;

  st = fabs(sp->SinT);
  xo = fabs(sp->R0.x);

  denom = deli*sp->CosT*tanphi - delj;

  
  if (fabs(denom) < 1.0e-10) {
/*    printf("denom = %10e   delj = %10e  P0j = %10e   P1j = %10e\n", denom, delj, P0->j, P1->j); */
    *t = MAX_T_INTERCEPT + 1.0;
  }
  else
    *t =  (P0->j - tanphi*(xo + P0->i*sp->CosT + k*st))/denom;
  
  intersects = !((*t < MIN_T_INTERCEPT) || (*t > MAX_T_INTERCEPT));

/* calculate the intersection if the parameter is in range */

  if (intersects) {
    Pint->i = P0->i + (*t)*deli;
    Pint->j = P0->j + (*t)*delj;
  }

  return intersects;
}



/* ---------- DrawStripIntersections -------*/

static void DrawStripIntersections(Hv_View  View,
				   short     sect,  /*C Index*/
				   Hv_Region region)

/* DIAGNOSTIC PROCEDURE to see if strip intersections 
  seem to be working*/

{
  ViewData         viewdata = GetViewData(View);
  short            plane;  /*used to loop over inner & outer planes*/
  short            type;   /*used to loop over UVW */
  float            tanphi;
  Shower_Package   *sp;
  short            strip;
  Hv_Region        temp = NULL;
  float            c_phi;
  Hv_Point         poly[4];
  short            color;

  c_phi = cos(Hv_DEGTORAD*viewdata->dphi);
  tanphi = tan(Hv_DEGTORAD*viewdata->dphi);
  sp = &(ShowerCounters[sect]);

  for (plane = ECINNER; plane <= ECOUTER; plane++) {

/* get the polygon for the plane */

    GetSectorViewECPolygon(View, sect, plane, poly);
    temp = Hv_CreateRegionFromPolygon(poly, 4);
    Hv_SetClippingRegion(temp);

    for (type = USTRIP; type <= WSTRIP; type++) {

      if (type == USTRIP)
	color = Hv_red;
      else if (type == VSTRIP)
	color = Hv_aquaMarine;
      else
	color = Hv_yellow;

      for (strip = 1; strip <= NUM_STRIP; strip++)
	if (StripIntersectionPoly(View, sect, plane, type, strip, poly)) {
	  Hv_FillPolygon(poly, 4, True, color, Hv_black);
	}
    }
  }
  
  Hv_DestroyRegion(temp);
  Hv_SetClippingRegion(region);   /*restore*/
}


#undef  EXPAND_SIN
#undef  EXPAND_COS
#undef  MIN_T_INTERCEPT
#undef  MAX_T_INTERCEPT
