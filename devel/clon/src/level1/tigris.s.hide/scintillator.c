
#ifndef VXWORKS

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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"		/* contains all necessary motif include files */
#include "tigris.h"

BitsLookup bitslookup[48]; /* global */
int scintColor[6]; /* global */

extern int SDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern Scint_Package Scintillators[6]; /* defined in ced_geometry.c */
extern VisSectorDef secdef[6]; /* defined in init.c */


/* local variables */

static Hv_Widget fillcolorlabel, framecolorlabel, hitcolorlabel;
static short fillcolor, framecolor, hitcolor;

/* ------ local prototypes -------*/

static void frontBitsOff(int sc, int sd, Hv_Point  pt[4], Hv_View view);
static void frontBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view);
static void boundBitsOff(int sc, int sd, Hv_Point pt[4], Hv_View view);
static void boundBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view);
static void backBitsOff(int sc, int sd, Hv_Point pt[4], Hv_View view);
static void backBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view);
static void ScintClick(Hv_Event hevent);
static int WhichScint(Hv_View View, Hv_Point pp, int *scintNum, int *scintData,
                      Hv_Point *pt);
static void ComputeScintPoints(Hv_View View, int sect, int sc, int bit,
                               Hv_Point *pt);
static void Front16Scintillators(Hv_View View, short sect, short minid,
                                 short maxid, Hv_Point *pt1, Hv_Point *pt2,
                                 Hv_Point *pt3);
static void DrawScintItem(Hv_Item Item, Hv_Region region);
static void FixScintRegion(Hv_Item Item);
static void EditScintItem(Hv_Event hvevent);
static void EditScintColor(Widget w);


/* ------------------------------------------------------- */

void
init_bitslookup()
{
  int i, j, k ;

  for(i=0, j=0; i<15; i++, j++)
  {
    bitslookup [i].b.howmany = 0;
    bitslookup[i].b.litebitbut [0] = 0;
    bitslookup[i].b.litebitbut [1] = 0;
    bitslookup[i].howMany = 2;
    bitslookup[i].litBits[0] = j;
    bitslookup[i].litBits[1] = j+1;
  }
  bitslookup [i].b.howmany = 0;
  bitslookup[i].b.litebitbut [0] = 0;
  bitslookup[i].b.litebitbut [1] = 0;
  bitslookup[i].howMany = 2;
  bitslookup[i].litBits[0] = j;
  bitslookup[i++].litBits[1] = j++;  /* the 15 th one */

  for (k=48, j=1; i<47; i++) {
    bitslookup[i].b.howmany = j;
    if (j == 1) {
      bitslookup[i].b.litebitbut[0] = k;
      bitslookup[i].b.litebitbut[1] = 0;
    } else {
      bitslookup[i].b.litebitbut[0] = k;
      bitslookup[i].b.litebitbut[1] = k+1;
    }
    if (++j > 2) { j = 1; k += 1; }
  }
  bitslookup[i].b.howmany = 2;
  bitslookup[i].b.litebitbut[0] = 63;
  bitslookup[i].b.litebitbut[1] = 63; /* i=47 here */

  i = 16;
  bitslookup [i].howMany = 3;
  bitslookup [i].litBits [0] = 16;
  bitslookup [i].litBits [1] = 17;  /* look in boundBits to explain 17 here */
  bitslookup [i].litBits [2] = 17;  /* done with # 16: won't use 3rd bit */
  i = 17;
  bitslookup [i].howMany = 5;
  bitslookup [i].litBits [0] = 16;
  bitslookup [i].litBits [1] = 17;
  bitslookup [i].litBits [2] = 17;
  bitslookup [i].litBits [3] = 18;
  bitslookup [i].litBits [4] = 19; /* this does # 17; and back on track ... */

  for (k=17, j = 3, i=18; i<47; i++) {
    if (j==3) {
      bitslookup [i].howMany = j;
      bitslookup [i].litBits [0] = k;
      bitslookup [i].litBits [1] = k+1;
      bitslookup [i].litBits [2] = k+2; 
      j = 5;
    } else {
      bitslookup [i].howMany = j;
      bitslookup [i].litBits [0] = k;
      bitslookup [i].litBits [1] = k+1;
      bitslookup [i].litBits [2] = k+2;
      bitslookup [i].litBits [3] = k+3;
      bitslookup [i].litBits [4] = k+4;
      j = 3;
      k += 2;
    }
  }
 /* should be k=47, j=5 , i=47; we want only 3 for feedback  */
  k+=2;
  bitslookup [i].howMany = 5;
  bitslookup [i].litBits [0] = k; 
  bitslookup [i].litBits [1] = k;
  bitslookup [i].litBits [2] = k;
  bitslookup [i].litBits [3] = k;
  bitslookup [i].litBits [4] = k;
}

/* ============================================================= */

static void
frontBitsOff(int sc, int sd, Hv_Point  pt[4], Hv_View view)
{
  Hv_Region r, totR ;
  ViewData  vdata ;
  int       theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  if ( (secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] > 0) && 
      (secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] > 0) ) {
    
    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (sc+(sd*16)) ] = 0;
    
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] -= 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] -= 1 ;
    
    ComputeScintPoints (view, theSector, sc, sd+1, pt) ;
    totR = Hv_CreateRegionFromPolygon (pt, 4) ;
    
    ComputeScintPoints (view, theSector, sc+1, sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    Hv_DrawView (view, totR) ;
    
    Hv_DestroyRegion (totR) ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 0;

  } 
}

/* ============================================================= */

static void
frontBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view)
{
  Hv_Region    r, totR ;
  ViewData     vdata ;
  int          theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] += 1 ;
  secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] += 1 ;
  
  SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (sc+(sd*16)) ] = 1;
  
  ComputeScintPoints (view, theSector, sc, sd+1, pt) ;
  totR = Hv_CreateRegionFromPolygon (pt, 4) ;
  
  ComputeScintPoints (view, theSector, sc+1, sd+1, pt) ;
  r = Hv_CreateRegionFromPolygon (pt, 4) ;
  Hv_AddRegionToRegion (&totR, r) ;
  Hv_DestroyRegion (r) ;
  Hv_DrawView (view, totR) ;
  
  Hv_DestroyRegion (totR) ;
  secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 1;

}

/* ============================================================= */
/*
 * notice this tries to light 2 scintillators.
 */
/* ============================================================= */

static void
boundBitsOff(int sc, int sd, Hv_Point pt[4], Hv_View view)
{
  Hv_Region r, totR ;
  ViewData  vdata ;
  int k, theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  if (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] == 1) {
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] -= 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] -= 1 ;
    
    k = bitslookup[sc].b.litebitbut[0] ;

    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 0;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
    totR = Hv_CreateRegionFromPolygon (pt, 4) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    Hv_DrawView (view, totR) ;
    
    Hv_DestroyRegion (totR) ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 0;

  }
}

/* ============================================================= */
/*
 * notice this tries to light 2 scintillators.
 */
/* ============================================================= */

static void
boundBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view)
{
  Hv_Region r, totR ;
  ViewData  vdata ;
  int k, theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] += 1 ;
  secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] += 1 ;
  
  k = bitslookup[sc].b.litebitbut[0] ;

  SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 1;
  
  ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
  totR = Hv_CreateRegionFromPolygon (pt, 4) ;
  
  ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
  r = Hv_CreateRegionFromPolygon (pt, 4) ;
  Hv_AddRegionToRegion (&totR, r) ;
  Hv_DestroyRegion (r) ;
  
  Hv_DrawView (view, totR) ;
  
  Hv_DestroyRegion (totR) ;
  secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 1;

}

/* ============================================================= */

static void
backBitsOff(int sc, int sd, Hv_Point pt[4], Hv_View view)
{
  Hv_Region r, totR ;
  ViewData  vdata ;
  int k, theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  if (bitslookup[sc].howMany == 3) {
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] -= 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] -= 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[2]] -= 1 ;
    
    k = bitslookup[sc].b.litebitbut[0] ;

    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 0;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
    totR = Hv_CreateRegionFromPolygon (pt, 4) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[2], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    Hv_DrawView (view, totR) ;
    
    Hv_DestroyRegion (totR) ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 0;

  } else {
    if (bitslookup[sc].howMany == 5) {

      secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] = 0 ;
      secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] = 0 ;
      secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[2]] = 0 ;
      secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[3]] = 0 ;
      secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[4]] = 0 ;
      
      k = bitslookup[sc].b.litebitbut[0] ;

      SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 0;
      
      k = bitslookup[sc].b.litebitbut[1] ;

      SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 0;
      
      ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
      totR = Hv_CreateRegionFromPolygon (pt, 4) ;
      
      ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
      r = Hv_CreateRegionFromPolygon (pt, 4) ;
      Hv_AddRegionToRegion (&totR, r) ;
      Hv_DestroyRegion (r) ;
      
      ComputeScintPoints (view, theSector, bitslookup[sc].litBits[2], sd+1, pt) ;
      r = Hv_CreateRegionFromPolygon (pt, 4) ;
      Hv_AddRegionToRegion (&totR, r) ;
      Hv_DestroyRegion (r) ;
      
      ComputeScintPoints (view, theSector, bitslookup[sc].litBits[3], sd+1, pt) ;
      r = Hv_CreateRegionFromPolygon (pt, 4) ;
      Hv_AddRegionToRegion (&totR, r) ;
      Hv_DestroyRegion (r) ;
      
      ComputeScintPoints (view, theSector, bitslookup[sc].litBits[4], sd+1, pt) ;
      r = Hv_CreateRegionFromPolygon (pt, 4) ;
      Hv_AddRegionToRegion (&totR, r) ;
      Hv_DestroyRegion (r) ;
      
      Hv_DrawView (view, totR) ;
      
      Hv_DestroyRegion (totR) ;
      secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc-1] = 0;
      secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc  ] = 0;
      secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc+1] = 0;

    }
  }
}

/* ============================================================= */

static void
backBitsOn(int sc, int sd, Hv_Point pt[4], Hv_View view)
{
  Hv_Region r, totR ;
  ViewData  vdata ;
  int k, theSector;

  vdata = (ViewData) (view->data) ;

  if (vdata->sectorNumber > 2)
    theSector = vdata->sectorNumber - 3;
  else
    theSector = vdata->sectorNumber;

  if (bitslookup[sc].howMany == 3) {
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] += 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] += 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[2]] += 1 ;
    
    k = bitslookup[sc].b.litebitbut[0] ;

    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 1;
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
    totR = Hv_CreateRegionFromPolygon (pt, 4) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[2], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    Hv_DrawView (view, totR) ;
    
    Hv_DestroyRegion (totR) ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] = 1;


  } else {
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[0]] = 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[1]] = 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[2]] = 2 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[3]] = 1 ;
    secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][sd][bitslookup[sc].litBits[4]] = 1 ;
    
    k = bitslookup[sc].b.litebitbut[0] ;

    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 1;
    k = bitslookup[sc].b.litebitbut[1] ;

    SDbits4sgl[vdata->sectorNumber][vdata->sglphase][ (k+(sd*16)) ] = 1;
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[0], sd+1, pt) ;
    totR = Hv_CreateRegionFromPolygon (pt, 4) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[1], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[2], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[3], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    ComputeScintPoints (view, theSector, bitslookup[sc].litBits[4], sd+1, pt) ;
    r = Hv_CreateRegionFromPolygon (pt, 4) ;
    Hv_AddRegionToRegion (&totR, r) ;
    Hv_DestroyRegion (r) ;
    
    Hv_DrawView (view, totR) ;
    
    Hv_DestroyRegion (totR) ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc-1] = 1 ;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc  ] = 1;
    secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc+1] = 1;


  }
}

/* ----------- LightScintillators ----------------- */

void
LightScintillators(Hv_View view, int sc, int sd, Hv_Point *pt)
{
  ViewData  vdata ;
  
  vdata = (ViewData) (view->data) ;

  if (sc < 16) {
    if (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][bitslookup[sc].litBits[0]] == 0)
      frontBitsOn(sc, sd, pt, view);
    else 
      frontBitsOff(sc, sd, pt, view);
    DrawSDbits (vdata->ScintDFront16bits, NULL);
  } else 
    if (sc == 16) {
      if (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] == 0)
	boundBitsOn (sc, sd, pt, view);
      else
	boundBitsOff (sc, sd, pt, view);
    } else {
      if ( (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc-1] == 0) && 
	  (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc+1] == 0) && ((sc%2) !=0))  
	backBitsOn (sc, sd, pt, view);
      else
	if ( (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc-1] == 1) && 
	    (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc+1] == 1) && ((sc%2) !=0))
	  backBitsOff (sc, sd, pt, view);
	else {
	  if (secdef[vdata->sectorNumber].sd.state[vdata->sglphase][sd][sc] == 0) {
	    if ( sc%2 == 0)
	      backBitsOn (sc, sd, pt, view) ;
	  } else 
	    if ( sc%2 == 0)
	      backBitsOff (sc, sd, pt, view) ;
	}
      DrawSDbits (vdata->ScintDBack32bits, NULL);
    }
}

/*-------- ScintClick ---------------- */

static void
ScintClick(Hv_Event hevent)
{
  int sc, sd, scint; 
  Hv_Point pt[4] ;

  scint = WhichScint (hevent->view, hevent->where, &sc, &sd, pt) ;
  if (scint != 0)
    LightScintillators (hevent->view, sc, sd, pt);
}

/*------ WhichScint  ---------------*/
/*pp is mouse location*/
static int
WhichScint(Hv_View View, Hv_Point pp, int *scintNum, int *scintData,
           Hv_Point *pt)
{
  ViewData    viewdata;
  Hv_Item     temp;
  short       sect;
  short       plane;
  short       i, j, minid, maxid ;
  Hv_Point    poly[4];

  viewdata = GetViewData(View);

  for (j=0; j<4; j++) {
    temp = viewdata->ArrayOScintillators [j];
    if (Hv_PointInRegion(pp, temp->region)) {

/* now we know that we are in a plane. we need to determine which slab */

      plane = temp->user1;
      sect  = temp->user2;
      ScintSlabLimits(plane, &minid, &maxid);
      
      for (i = minid; i <= maxid; i++) {
	GetSlabPolygon(View, sect, i, poly);
	if (Hv_PointInPolygon(pp, poly, 4)) {
          *scintNum = i ;
          /* in a scint now compute which piece */
          ComputeScintPoints (View, temp->user2, i, 1, pt) ;
          if (Hv_PointInPolygon (pp, pt, 4)) {
            *scintData = 0 ;
            return True ;
          }
          ComputeScintPoints (View, temp->user2, i, 2, pt) ;
          if (Hv_PointInPolygon (pp, pt, 4)) {
            *scintData = 1 ;
             return True ;
          } else {
            ComputeScintPoints (View, temp->user2, i, 3, pt) ;
            *scintData = 2 ;
            return True ;
          }
	}
      }
    }
  }
  return 0;
}

/*------ MallocScintillator --------*/

Hv_Item
MallocScintillator(Hv_View View, int sect, short ID)
{
/* ID is a C INDEX [0, 3]. 
   It corresponds to the "plane"

   plane 0:   scint 0  - 22
   plane 1:   scint 23 - 33
   plane 2:   scint 34 - 41
   plane 3:   scint 42 - 47

   and is placed in user1 for rapid
   recovery during feedback.

   sect is a C Index [0..5]  */

  Hv_Item      Item;
  Hv_FPoint    *fpts;
  char         balloon[60];


  fpts = (Hv_FPoint *)Hv_Malloc(4*sizeof(Hv_FPoint));
  sprintf(balloon, "Scintillator Plane %1d", ID+1);

  Item = Hv_VaCreateItem(View,
			 Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			 Hv_TAG,          SCINTITEM,
			 Hv_NUMPOINTS,    4,
			 Hv_DATA,         fpts,
			 Hv_NUMROWS,      1,
			 Hv_NUMCOLUMNS,   1,
			 Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			 Hv_DOUBLECLICK,  EditScintItem,
			 Hv_DRAWCONTROL,  Hv_ZOOMABLE,
			 Hv_AFTEROFFSET,  Hv_PolygonAfterOffset,
			 Hv_FIXREGION,    FixScintRegion,
			 Hv_USER1,        ID,
			 Hv_USER2,        (short)sect,
			 Hv_BALLOON,      balloon,
			 NULL);
  
  
  Item->singleclick = ScintClick;
/*  Item->type = Hv_USERITEM;  */
  Item->standarddraw = DrawScintItem;
  return  Item;
}

/*-------- SetScintGeometrySector -----*/

void
SetScintGeometrySector(Hv_View View)
{
  ViewData             viewdata;
  short                minid, maxid;
  int                  vs, sect, j;
  Hv_Item              temp;
  Hv_FPoint            *fpts, *Pmin, *Pmax;
  Hv_WorldPolygonData  *wpoly;

  if (View->tag != SECTOR)
    return;

  viewdata = GetViewData(View);

/* recall: vs < 0 for mostdata; vs is NOT a C index */

  vs = abs(viewdata->visiblesectors) - 1;

  for (j=0; j<4; j++) {
    temp = viewdata->ArrayOScintillators[j] ;
/* the user2 field holds the sector. Make sure it is still consistent, since
   via the option buttons  the sectors may have changed. Note also that the
   items for the "upper" sectors [0,1,2] are easy to determine. */

    if (temp->user2 < 3)   /* upper sector */
      sect = vs;
    else
      sect = vs + 3;

    temp->user2 = (short)sect;

/* recal that the "plane" is stored in user1 */

    wpoly = (Hv_WorldPolygonData *)(temp->data);
    fpts = wpoly->fpts;

    ScintSlabLimits(temp->user1, &minid, &maxid);
    Pmin = Scintillators[sect].scints[minid].P;
    Pmax = Scintillators[sect].scints[maxid].P;
    fpts[0] = Pmin[1];
    fpts[1] = Pmin[2];
    fpts[2] = Pmax[3];
    fpts[3] = Pmax[0];
    temp->fixregion(temp);
  }
}

/* ------- ComputeScintPoints ------------- */
/*
 *  The range of sect icomes from...
 *  The top 3 scintillators (as viewed from the side)
 *  are a mirror of the bottom 3. So I gather the points
 *  to draw from the top 3 scintillators only.
 */
/* sect - range 0 - 2  - use the top scintillators */
/* sc   - range 1 - 48 */
/* bit  - range 1, 2, 3 */
/* pt   - addr of rectangular poly */

static void
ComputeScintPoints(Hv_View View, int sect, int sc, int bit, Hv_Point *pt)
{
  Hv_FPoint fp1, fp2, fp3, fp4, jp1, jp2, jp3, jp4 ;

  /*  int sect ; */
  /*  sect = Item->user2 ;  */
  /*  printf ("sector = %d, sc = %d, bit = %d:::", sect, sc, bit); */

  switch (bit) {
  case 1 : 
    { 
        fp1 = Scintillators[sect].scints[sc].P[3];
        fp2 = Scintillators[sect].scints[sc].P[0];
        fp3 = Scintillators[sect].scints[sc].P[1];
        fp4 = Scintillators[sect].scints[sc].P[2];

        jp1.h = fp2.h + (0.3)*(fp1.h - fp2.h) ;
        jp1.v = fp2.v + (0.3)*(fp1.v - fp2.v) ;

        jp2.h = fp3.h + (0.3)*(fp4.h - fp3.h) ;
        jp2.v = fp3.v + (0.3)*(fp4.v - fp3.v) ;

        Hv_WorldToLocal(View, fp2.h, fp2.v, &pt[0].x, &pt[0].y);
        Hv_WorldToLocal(View, jp1.h, jp1.v, &pt[1].x, &pt[1].y);
        Hv_WorldToLocal(View, jp2.h, jp2.v, &pt[2].x, &pt[2].y);
        Hv_WorldToLocal(View, fp3.h, fp3.v, &pt[3].x, &pt[3].y);
/*	printf ("pt[0] = (%d, %d)  -- pt[1] = (%d, %d): pt[2] = (%d, %d)  -- pt[3] = (%d, %d)\n", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y);  */
        return ;
      }
  case 2 :
    {
        fp1 = Scintillators[sect].scints[sc].P[3];
        fp2 = Scintillators[sect].scints[sc].P[0];
        fp3 = Scintillators[sect].scints[sc].P[1];
        fp4 = Scintillators[sect].scints[sc].P[2];

        jp1.h = fp2.h + (0.3)*(fp1.h - fp2.h) ;
        jp1.v = fp2.v + (0.3)*(fp1.v - fp2.v) ;

        jp2.h = fp2.h + (0.67)*(fp4.h - fp3.h) ;
        jp2.v = fp2.v + (0.67)*(fp4.v - fp3.v) ;

        jp3.h = fp3.h + (0.67)*(fp4.h - fp3.h) ;
        jp3.v = fp3.v + (0.67)*(fp4.v - fp3.v) ;

        jp4.h = fp3.h + (0.3)*(fp4.h - fp3.h) ;
        jp4.v = fp3.v + (0.3)*(fp4.v - fp3.v) ;

        Hv_WorldToLocal(View, jp1.h, jp1.v, &pt[0].x, &pt[0].y);
        Hv_WorldToLocal(View, jp2.h, jp2.v, &pt[1].x, &pt[1].y);
        Hv_WorldToLocal(View, jp3.h, jp3.v, &pt[2].x, &pt[2].y);
        Hv_WorldToLocal(View, jp4.h, jp4.v, &pt[3].x, &pt[3].y);

        return ;
      }
  case 3 :
    {
      fp1 = Scintillators[sect].scints[sc].P[3];
      fp2 = Scintillators[sect].scints[sc].P[0];
      fp3 = Scintillators[sect].scints[sc].P[1];
      fp4 = Scintillators[sect].scints[sc].P[2];

      jp1.h = fp2.h + (0.67)*(fp1.h - fp2.h) ;
      jp1.v = fp2.v + (0.67)*(fp1.v - fp2.v) ;

      jp2.h = fp3.h + (0.67)*(fp4.h - fp3.h) ;
      jp2.v = fp3.v + (0.67)*(fp4.v - fp3.v) ;

      Hv_WorldToLocal(View, jp1.h, jp1.v, &pt[0].x, &pt[0].y);
      Hv_WorldToLocal(View, fp1.h, fp1.v, &pt[1].x, &pt[1].y);
      Hv_WorldToLocal(View, fp4.h, fp4.v, &pt[2].x, &pt[2].y);
      Hv_WorldToLocal(View, jp2.h, jp2.v, &pt[3].x, &pt[3].y);

      return ;
    }
  }
}

/* --------------------------------- */

static void
Front16Scintillators(Hv_View View, short sect, short minid, short maxid,
                     Hv_Point *pt1, Hv_Point *pt2, Hv_Point *pt3)
{
  Hv_FPoint fp1, fp2, jp1, jp2, jp3, jp4;
 
  fp1 = Scintillators[sect].scints[minid].P[2];
  fp2 = Scintillators[sect].scints[minid].P[1];
  jp3.h = fp2.h + (0.3)*(fp1.h - fp2.h) ;
  jp3.v = fp2.v + (0.3)*(fp1.v - fp2.v) ;

  jp1 = Scintillators[sect].scints[maxid].P[3];
  jp2 = Scintillators[sect].scints[maxid].P[0];
  jp4.h = jp2.h + (0.3)*(jp1.h - jp2.h) ;
  jp4.v = jp2.v + (0.3)*(jp1.v - jp2.v) ;

  Hv_WorldToLocal(View, jp3.h, jp3.v, &pt1[2].x, &pt1[2].y);
  Hv_WorldToLocal(View, jp4.h, jp4.v, &pt1[3].x, &pt1[3].y);

  pt2[1].x = pt1[2].x ; pt2[1].y = pt1[2].y ;
  pt2[0].x = pt1[3].x ; pt2[0].y = pt1[3].y ;

  Hv_WorldToLocal(View, jp2.h, jp2.v, &pt1[0].x, &pt1[0].y);
  Hv_WorldToLocal(View, fp2.h, fp2.v, &pt1[1].x, &pt1[1].y);

  fp1 = Scintillators[sect].scints[minid].P[2];
  fp2 = Scintillators[sect].scints[minid].P[1];
  jp3.h = fp2.h + (0.67)*(fp1.h - fp2.h) ;
  jp3.v = fp2.v + (0.67)*(fp1.v - fp2.v) ;

  jp1 = Scintillators[sect].scints[maxid].P[3];
  jp2 = Scintillators[sect].scints[maxid].P[0];
  jp4.h = jp2.h + (0.67)*(jp1.h - jp2.h) ;
  jp4.v = jp2.v + (0.67)*(jp1.v - jp2.v) ;

  Hv_WorldToLocal(View, jp3.h, jp3.v, &pt2[2].x, &pt2[2].y);
  Hv_WorldToLocal(View, jp4.h, jp4.v, &pt2[3].x, &pt2[3].y);

  pt3[0].x = pt2[2].x ; pt3[0].y = pt2[2].y ;
  pt3[1].x = pt2[3].x ; pt3[1].y = pt2[3].y ;

  Hv_WorldToLocal(View, Scintillators[sect].scints[minid].P[2].h, 
        Scintillators[sect].scints[minid].P[2].v, 
        &pt3[3].x, &pt3[3].y);

  Hv_WorldToLocal(View, Scintillators[sect].scints[maxid].P[3].h,
        Scintillators[sect].scints[maxid].P[3].v, 
        &pt3[2].x, &pt3[2].y);

}

/*---------- DrawScintItem --------*/

static void
DrawScintItem(Hv_Item Item, Hv_Region region)
{
  Hv_View               View = Item->view;
  ViewData              vdata ;
  Hv_WorldPolygonData  *wpoly;
  short                 i, minId, maxId;
  short                 ID, sect;
  short                 j,t, color ;
  Hv_Point              pt[4], pt1[4], pt2[4], pt3[4] ;
  Hv_FPoint             fp1, fp2 ;
  
  vdata = GetViewData(View);
  
  if (!(vdata->displaySC))
    return;

  FixScintRegion(Item);

  wpoly = (Hv_WorldPolygonData *)(Item->data);
  if (wpoly->poly == NULL)
    return;

  /* now the lines marking the slabs */

  ID   = Item->user1;
  sect = Item->user2;

  ScintSlabLimits(ID, &minId, &maxId);

  if (ID == 0) {
    color = Hv_gray15 ;
    Front16Scintillators (View, sect, minId, 15, pt1, pt2, pt3) ;
    Hv_FillPolygon (pt1, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt2, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt3, 4, False, color, vdata->scintframecolor) ;

    Hv_DrawLine (pt1[2].x, pt1[2].y, pt1[3].x, pt1[3].y, vdata->scintframecolor) ;
    Hv_DrawLine (pt2[2].x, pt2[2].y, pt2[3].x, pt2[3].y, vdata->scintframecolor) ;

    Front16Scintillators (View, sect, 16, maxId, pt1, pt2, pt3) ;
    color = Hv_gray10 ;
    Hv_FillPolygon (pt1, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt2, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt3, 4, False, color, vdata->scintframecolor) ;

    Hv_DrawLine (pt1[2].x, pt1[2].y, pt1[3].x, pt1[3].y, vdata->scintframecolor) ;
    Hv_DrawLine (pt2[2].x, pt2[2].y, pt2[3].x, pt2[3].y, vdata->scintframecolor) ;

  } else {
    color = Hv_gray10 ;
    Front16Scintillators (View, sect, minId, maxId, pt1, pt2, pt3) ;
    Hv_FillPolygon (pt1, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt2, 4, False, color, vdata->scintframecolor) ;
    Hv_FillPolygon (pt3, 4, False, color, vdata->scintframecolor) ;

    Hv_DrawLine (pt1[2].x, pt1[2].y, pt1[3].x, pt1[3].y, vdata->scintframecolor) ;
    Hv_DrawLine (pt2[2].x, pt2[2].y, pt2[3].x, pt2[3].y, vdata->scintframecolor) ;

  }

  for (j=0; j<3; j++) {
    for (i=minId; i<maxId+1; i++) {
      if (secdef[vdata->sectorNumber].sd.bits[vdata->sglphase][j][i] > 0) {
        ComputeScintPoints (View, Item->user2, i, (j+1), pt) ;
        if (i >= 16)
          t = 3;
        else
          t = 0 ;
        Hv_FillPolygon (pt, 4, False, scintColor[j+t], vdata->scintframecolor) ;
      }
    }
  }

  Hv_FramePolygon(wpoly->poly, 4, vdata->scintframecolor);

  for (i = minId; i < maxId; i++) {
    fp1 = Scintillators[sect].scints[i].P[3];
    fp2 = Scintillators[sect].scints[i].P[0];

    Hv_WorldToLocal(View, fp1.h, fp1.v, &pt[1].x, &pt[1].y);
    Hv_WorldToLocal(View, fp2.h, fp2.v, &pt[2].x, &pt[2].y);
    Hv_DrawLine(pt[1].x, pt[1].y, pt[2].x, pt[2].y, vdata->scintframecolor);

  }


}

/*------- GetSlabPolygon -------*/
/* sect - C Index */
/* slab - C Index */
/* poly - previously malloced */

void
GetSlabPolygon(Hv_View View, short sect, short slab, Hv_Point *poly)
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

/*---------- FixScintRegion --------*/

static void
FixScintRegion(Hv_Item Item)
{
  Hv_View               View = Item->view;
  Hv_WorldPolygonData  *wpoly;
  Hv_FPoint             fpts[4];
  int                   i;

  Hv_DestroyRegion(Item->region);
  Item->region = NULL;
  
  wpoly = (Hv_WorldPolygonData *)(Item->data);

  if (wpoly->poly != NULL)
    Hv_Free(wpoly->poly);
    
  if (wpoly->fpts != NULL) {

    for (i = 0; i < 4; i++) {
      fpts[i].h = wpoly->fpts[i].h;
      fpts[i].v = MidplaneXtoProjectedX(View, wpoly->fpts[i].v);
    }


    wpoly->poly = (XPoint *)(Hv_Malloc(4*sizeof(XPoint)));
    Hv_WorldPolygonToLocalPolygon(View, 4, wpoly->poly, fpts);
    Item->region = Hv_CreateRegionFromPolygon(wpoly->poly, 4);
    Hv_ClipBox(Item->region, Item->area);
    Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
  }
}

/*-------- EditScintItem -------*/
/* item edit for SCINTITEMs  */
static void
EditScintItem(Hv_Event hvevent)
{
  Hv_View                 View = hvevent->view;
  static Widget           dialog = NULL;
  Widget                  dummy, rowcol, rc;
  ViewData                vdata;
  int                     answer;

  vdata = GetViewData(View);
  
  if (!dialog) {
    Hv_VaCreateDialog(&dialog, Hv_TITLE, " Scintillator Editor ", NULL);
    rowcol = Hv_StandardMainRowCol(dialog, 6);  /* rowcol to hold all other widgets */

    rc = Hv_StandardPackingRowCol(rowcol, 3, 6);
    fillcolorlabel  = Hv_SimpleColorLabel(rc, " fill color ", (Hv_FunctionPtr)EditScintColor);
    framecolorlabel = Hv_SimpleColorLabel(rc, "frame color ", (Hv_FunctionPtr)EditScintColor);
    hitcolorlabel   = Hv_SimpleColorLabel(rc, "  hit color ", (Hv_FunctionPtr)EditScintColor);
    
    dummy = Hv_StandardActionButtons(rowcol, 26, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
  }
  
/* the dialog has been created */
  
  fillcolor  = vdata->scintfillcolor;
  framecolor = vdata->scintframecolor;
  hitcolor   = vdata->scinthitcolor;
  Hv_ChangeLabelColors(fillcolorlabel,  -1, fillcolor);
  Hv_ChangeLabelColors(framecolorlabel, -1, framecolor);
  Hv_ChangeLabelColors(hitcolorlabel,   -1, hitcolor);


  while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {
    vdata->scintfillcolor = fillcolor;
    vdata->scintframecolor = framecolor;
    vdata->scinthitcolor = hitcolor;

    DrawAllScintItems(View);

    if(answer != Hv_APPLY)
      break;
  }  
}

/*------- DrawAllScintItems ----- */

void
DrawAllScintItems(Hv_View View)
{
  ViewData      vdata;
  Hv_Region     clipreg = NULL;
  Hv_Item       temp;
  int           i;

  vdata =  GetViewData(View);
  clipreg = Hv_ClipHotRect(View, 0);

  for (i=0; i<4; i++) {
    temp = vdata->ArrayOScintillators[i] ;
    printf ("temp[%p] temp->data[%p]\n", temp, temp->data); 
    DrawScintItem(temp, clipreg);
  }
/*  DrawViewEvent(View, clipreg);   */
  Hv_DestroyRegion(clipreg);
}



/* ------ EditScintColor --- */
/* widget 'w' doing callback */
static void
EditScintColor(Widget w)
{
  if (w == fillcolorlabel)
    Hv_ModifyColor(w, &fillcolor, "Fill Color", False);
  else if (w == framecolorlabel)
    Hv_ModifyColor(w, &fillcolor, "Frame Color", False);
  if (w == hitcolorlabel)
    Hv_ModifyColor(w, &fillcolor, "Hit Color", False);
}

/*------ ScintSlabLimits ------*/
/* ID is a C INDEX [0, 3]. 
   It corresponds to the "plane"

   plane 0:   scint 0  - 22
   plane 1:   scint 23 - 33
   plane 2:   scint 34 - 41
   plane 3:   scint 42 - 47
*/
void
ScintSlabLimits(short ID, short *minid, short *maxid)
{
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
    *maxid = 41;
  }
  else if (ID == 3) {
    *minid = 42;
    *maxid = 47;
  }
}

#else /* UNIX only */

void
scintillator_dummy()
{
}

#endif
