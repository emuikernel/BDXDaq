/*
  ----------------------------------------------------
  -							
  -  File:    dc.c	
  -							
  -  Summary:						
  -           controls drift chambers
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

#define    SMALLANG         0.01 

/*------ local prototypes ---------*/

void DatabaseFieldWirePosition(float      phi,
			       short      sect,
			       short      rgn,
			       short      suplay,
			       short      layer,
			       short      sensewire,
			       Hv_FPoint *fpts,
			       short      control);

void BOSFieldWirePosition(float      phi,
			  short      sect,
			  short      rgn,
			  short      suplay,
			  short      layer,
			  short      sensewire,
			  Hv_FPoint *fpts,
			  short      control);

static void DatabaseSenseWirePosition(float   phi,
				      short   sect,
				      short   rgn,
				      short   suplay,
				      short   layer,
				      short   wire,
				      float   *fz,
				      float   *fx);

static void BOSSenseWirePosition(float   phi,
				 short   sect,
				 short   rgn,
				 short   suplay,
				 short   layer,
				 short   wire,
				 float   *fz,
				 float   *fx);

static void DCAfterOffset(Hv_Item Item,
			  short   dh,
			  short   dv);

static void DrawDeadSuperLayerWires(Hv_View   View,
				    short     sect,
				    short     rgn,     /*0..2*/
				    short     supl,    /*0..1*/
				    Hv_Region region);

static Boolean  GoodDCIds(int sector,
			  int layer,
			  int wire);

static void EditDCItem(Hv_Event hvevent);

static Boolean CheckDCHit(short          sect,
			  unsigned char  layer,
			  unsigned char  wire);

static void GenericDrawDC1Hits(Hv_View     View,
			       short        sect,
			       int          numhits,
			       DC1DataPtr   hits,
			       Hv_Region    region);

static void DrawDC1TrackHits(Hv_View     View,
			     short        sect,
			     int          numhits,
			     int          numtracks,
			     DC1DataPtr   hits,
			     HBLADataPtr  hbla,
			     Hv_Region    region);


static void GenericDrawDC0Hits(Hv_View     View,
			      short       sect,
			      int         numhits,
			      char       *hits,
			      size_t      size,
			      Hv_Region   region);


static void  DCPolarCoordinates(float phi,
				short sect,
				short rgn,
				short sup,
				float fz,
				float fx,
				float *r,
				float *theta);

static Boolean WhatDCLayer(short sect,
			   short rgn,
			   short supl,
			   short *lay,
			   float r,
			   float theta);

static void WhatWire(Hv_View  View,
		     float    fz,
		     float    fx,
		     Hv_Point pp,
		     short    sect,
		     short    rgn,
		     short    sup,
		     short    *lay,
		     short    *wire);

static void DrawDCWires (Hv_View View,
			 short   sect,
			 short   rgn,
			 short   supl,
			 short   ni,
			 short   nf);

static int PixDens(Hv_View View);

static void FixDCSuperLayerRegion(Hv_Item Item);

static void CopyFPoint(Hv_FPoint *dfp,
                       Hv_FPoint *sfp);

static void  DrawDCItem(Hv_Item   Item,
			Hv_Region region);

static void EditDCColor(Hv_Widget  w);

static void SuperLayerFPolygon(Hv_View   View,
			       short     sect,
			       short     superlayer,
			       int       *np,
			       Hv_FPoint **fpts);

static Boolean DCLayerVisibleWireLimits(Hv_View View,
					short   sect,
					short   rgn,
					short   suplay,
					short   lay,
					short   *ni,
					short   *nf);

static Boolean DCSuperLayerVisibleWireLimits(Hv_View View,
					     short   sect,
					     short   rgn,
					     short   supl,
					     short   *ni,
					     short   *nf);


static Hv_Widget   badtimecolorlab;    
static Hv_Widget   deadwirecolorlab;    
static Hv_Widget   fillcolorlab;    
static Hv_Widget   framecolorlab;   
static Hv_Widget   hitcolorlab;     
static Hv_Widget   noisecolorlab;     
static Hv_Widget   dcacolorlab;     
static Hv_Widget   trigsegcolorlab; 
static Hv_Widget   trigaddcolorlab; 

static short   fillcolor;    
static short   badtimecolor;    
static short   deadwirecolor;    
static short   framecolor;   
static short   hitcolor;     
static short   noisecolor;     
static short   dcacolor;     
static short   trigsegcolor; 
static short   trigaddcolor; 

static Boolean FrameOnly = False;


/*------- IsDead ---------*/

Boolean  IsDead(short sect,
		short rgn,
		short supl,
		short lay,
		short wire) {

/* returns true if the wire is in the dead wire table

   note all parameters are C indices,

   sect  [0..5]
   rgn   [0..2]
   supl  [0..1]
   lay   [0..5]
   wire  [0..191]  */

    unsigned char   twire, tlayer;
    int             i;
    short           layer;



/* first check, are there ANY in this layer */

    if (dcstatus[sect][rgn][supl] == NULL)
	return False;

    layer = 12*rgn + 6*supl + lay;  /* 0.. 35 */

    i = 0;

    while(dcstatus[sect][rgn][supl][i].status != -99) {

	BreakShort(dcstatus[sect][rgn][supl][i].id, &twire, &tlayer);
	twire--;
	tlayer--;

	if (((short)twire == wire) && ((short)tlayer == layer))
	    return True;
	
	i++;
    }

    return False;

}


/*-------- GoodDCIds -------*/

static Boolean  GoodDCIds(int sector,
			  int layer,
			  int wire)

/*  C indices */

{
    return ( ((sector > -1) && (sector < 6)) &&
	     ((layer > -1) && (layer < 36)) &&
	     ((wire > -1) && (wire < 192)) );


}


/*----------- ReadDeadWireFile  ---------------*/

void    ReadDeadWireFile() 
{
#ifdef WITH_OFFLINE
  /* read in a simple ascii table of dead wires
     in the four int format

     sector   rgn layer(1..36)   wire


     NOTE: the rgn parameter is redundant

     */

  /* Now let's try and read in a map */


  char       *fname = NULL;
  int        i, j, k;
  int        counts[6][3][2];
  Boolean    badread = False;
  FILE      *fp;
  char      *line;
  int        sect, rgn, layer, wire, supl;
  unsigned   char  ww, ll;
  Hv_View    temp;
  ViewData   viewdata;
  char *dir,def_map[100],sector[100];
#define CHANNELS 6912
#define WIRES_PER_LAYER 192
#define BAD_STATUS 1
#define GOOD_STATUS 888
#define NSECTOR 6
  int values[CHANNELS];
  int nbad;
  int RunNo=1;
  int firsttime;
  int ij, nij;
    static  Widget   dialog = NULL;
    static  Widget   gtevnum;
    Widget           rowcol, rc, dummy;
    int              num;      


    if (!dialog) {
	Hv_VaCreateDialog(&dialog, Hv_TITLE, "Get Run", NULL);
	
	rowcol = Hv_DialogColumn(dialog, 8);
	rc = Hv_DialogRow(rowcol, 10);
	gtevnum = Hv_SimpleTextEdit(rc, "Run Number ", NULL, 4);
	
	dummy = Hv_StandardActionButtons(rowcol, 20, Hv_OKBUTTON + Hv_CANCELBUTTON);
    }

    
/* the dialog has been created */
    
    Hv_SetIntText(gtevnum, RunNo);
    
    if (Hv_DoDialog(dialog, NULL) == Hv_OK) {
	RunNo = Hv_GetIntText(gtevnum);
	}

  dir = getenv("CLAS_PARMS");
  sprintf(def_map,"%s/Maps/DC_STATUS.map",dir);

   for (i = 0; i < 6; i++)
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++)
	       counts[i][j][k] = 0;

   /* ----------------------------------------------------------- */

  for (i = 0; i  < NSECTOR; ++i) {
    sprintf(sector,"sector%1d",i  + 1);
    map_get_int(def_map,sector,"status", CHANNELS, values, RunNo, &firsttime);
    for (ij = 0; ij < CHANNELS; ++ij) {
      /*      if (values[iw] == BAD_STATUS) { */
      if (values[ij] != GOOD_STATUS) {
	sect=i;
	/*	nij=ij-(sect)*CHANNELS; */
	nij=ij;
	layer=nij/WIRES_PER_LAYER;
	wire=nij%WIRES_PER_LAYER;
/*
	layer=ij/256;
	wire=(ij-1)%256;	
*/
	if (GoodDCIds(sect, layer, wire)) {
	  supl = (layer / 6) % 2;
	  rgn=layer/12;
	  counts[sect][rgn][supl]++;	
	}
	else{
	  printf("Things messed up, BAD ID\n");
	}
      }
    }
  }


   /* ----------------------------------------------------------- */



   for (i = 0; i < 6; i++) 
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++) {

	       Hv_Free(dcstatus[i][j][k]);
	       dcstatus[i][j][k] = NULL;
	       if (counts[i][j][k] > 0) {
		   dcstatus[i][j][k] = (ComponentStatusPtr)Hv_Malloc((counts[i][j][k]+1)*sizeof(ComponentStatus));

/* mark the last one */

		   dcstatus[i][j][k][counts[i][j][k]].id = 0;
		   dcstatus[i][j][k][counts[i][j][k]].status = -99;
	       }
  }


   for (i = 0; i < 6; i++)
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++)
	       counts[i][j][k] = 0;



  for (i = 0; i  < NSECTOR; ++i) {
    sprintf(sector,"sector%1d",i  + 1);
    map_get_int(def_map,sector,"status", CHANNELS, values, RunNo, &firsttime);
    for (ij = 0; ij < CHANNELS; ++ij) {
      /*      if (values[iw] == BAD_STATUS) { */
      if (values[ij] != GOOD_STATUS) {
	sect=i;
	/*	nij=ij-(sect)*CHANNELS; */
	nij=ij;
	layer=nij/WIRES_PER_LAYER;
	wire=nij%WIRES_PER_LAYER;
/*
	layer=ij/256;
	wire=(ij-1)%256;	
*/
	if (GoodDCIds(sect, layer, wire)) {
	  supl = (layer / 6) % 2;
	  rgn=layer/12;

	  /* as is customary with our data, stuff as non C indices */

	  ll = (unsigned char)(layer + 1);
	  ww = (unsigned char)(wire + 1);

	  dcstatus[sect][rgn][supl][counts[sect][rgn][supl]].id = CompositeShort(ll, ww);  
	  dcstatus[sect][rgn][supl][counts[sect][rgn][supl]].status  = -1; /* dead wire status */
	  counts[sect][rgn][supl]++;
	}
	else{
	  printf("Things messed up, BAD ID PART II\n");
	}
      }
    }
  }



  /* redraw views that might show dead wires */


  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) 
    if (Hv_ViewIsVisible(temp)) {
      viewdata = GetViewData(temp);
      if (viewdata->displayDead) {
	Hv_SetViewDirty(temp);
	Hv_DrawViewHotRect(temp);
      }
    }
#endif 
}


/*----------- oldReadDeadWireFile  ---------------*/

void    oldReadDeadWireFile() {

/* read in a simple ascii table of dead wires
   in the four int format

   sector   rgn layer(1..36)   wire


   NOTE: the rgn parameter is redundant

*/

  char       *fname = NULL;
  int        i, j, k;
  int        counts[6][3][2];
  Boolean    badread = False;
  FILE      *fp;
  char      *line;
  int        sect, rgn, layer, wire, supl;
  unsigned   char  ww, ll;
  Hv_View    temp;
  ViewData   viewdata;

  fname = Hv_FileSelect("Select a Dead Wire File", "*.dw", " ");

  if (fname == NULL)
      badread = True;

/* see if it can be opened */

   if (!badread) {
       fp = fopen(fname, "r");
       badread = (fp == NULL);
   }
   

/* give up? */

   if (badread) {
       Hv_Information("No dead-wire data file was read.");
       return;
   }


  line = (char *) Hv_Malloc(Hv_LINESIZE);

/* two pass approach, 1st pass count the number in each sect */

   for (i = 0; i < 6; i++)
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++)
	       counts[i][j][k] = 0;

   while ((Hv_GetNextNonComment(fp, line)) != NULL) {
       sscanf(line, "%d %d %d %d", &sect, &rgn, &layer, &wire);

       sect--;
       layer--;
       wire--;


       if (GoodDCIds(sect, layer, wire)) {
	   rgn = layer / 12;        /* 0.. 2 */
	   supl = (layer / 6) % 2;  /* 0.. 1 */
	   counts[sect][rgn][supl] += 1;  
       }
       else
	   fprintf(stderr,
		   "Bad ID  (sect = %d, lay = %d, wire = %d)  in bad-wire file.\n",
		   sect+1, layer+1, wire+1);
   }



/* delete previous table, allocate new table. Note
   that one extra record is allocated, the extra one
   is used to mark the end of the array by giving it
   an id of 0 and  a status of -99; */


   for (i = 0; i < 6; i++) 
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++) {

	       Hv_Free(dcstatus[i][j][k]);
	       dcstatus[i][j][k] = NULL;
	       if (counts[i][j][k] > 0) {
		   dcstatus[i][j][k] = (ComponentStatusPtr)Hv_Malloc((counts[i][j][k]+1)*sizeof(ComponentStatus));

/* mark the last one */

		   dcstatus[i][j][k][counts[i][j][k]].id = 0;
		   dcstatus[i][j][k][counts[i][j][k]].status = -99;
	       }
  }

/* second pass, stuff the wires */

  rewind(fp);

   for (i = 0; i < 6; i++)
       for (j = 0; j < 3; j++) 
	   for (k = 0; k < 2; k++)
	       counts[i][j][k] = 0;

  while ((Hv_GetNextNonComment(fp, line)) != NULL) {
      sscanf(line, "%d %d %d %d", &sect, &rgn, &layer, &wire);

      sect--;
      rgn--;
      layer--;
      wire--;

      if (GoodDCIds(sect, layer, wire)) {
	  supl = (layer / 6) % 2;

/* as is customary with our data, stuff as non C indices */

	  ll = (unsigned char)(layer + 1);
	  ww = (unsigned char)(wire + 1);

	  dcstatus[sect][rgn][supl][counts[sect][rgn][supl]].id = CompositeShort(ll, ww);  
	  dcstatus[sect][rgn][supl][counts[sect][rgn][supl]].status  = -1; /* dead wire status */
	  counts[sect][rgn][supl] += 1;
      }
  }

  Hv_Free(line);


/* redraw views that might show dead wires */


  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) 
      if (Hv_ViewIsVisible(temp)) {
	  viewdata = GetViewData(temp);
	  if (viewdata->displayDead) {
	      Hv_SetViewDirty(temp);
	      Hv_DrawViewHotRect(temp);
	  }
  }
 
}

/*----------- InitDCData --------*/

void InitDCData() {
    dcframecolor = Hv_black;
    dcfillcolor = Hv_white;
    dchitcolor = Hv_red;
    dcdcacolor = Hv_gray12; 
    dctrigsegcolor = Hv_wheat;
    dcaddedbytrigcolor = Hv_aquaMarine;
    dcnoisecolor = Hv_gray2;
    dcfromdc1color  = Hv_darkGreen;
    dcdeadwirecolor = Hv_gray8;

/* related to out-of-time */

    dcbadtimecolor = Hv_dodgerBlue;

/* min max times on a per region basis */

    dcmintime[0] = 1400;
    dcmaxtime[0] = 2400;
    dcmintime[1] = 3000;
    dcmaxtime[1] = 5400;
    dcmintime[2] = 3000;
    dcmaxtime[2] = 7400;

/* pixel density (pixels/cm) cutoffs (for each region)  */
    
    dcbighitwires[0] = 100;
    dcnormalwire[0] = 250;
    dchitcircle[0]  = 100;
    dchexagon[0]    = 350;
    
    dcbighitwires[1] = 50;
    dcnormalwire[1] = 200;
    dchitcircle[1]  = 50;
    dchexagon[1]     = 260;
    
    dcbighitwires[2] = 50;
    dcnormalwire[2] = 150;
    dchitcircle[2]  = 50;
    dchexagon[2]    = 200;

/* noise reduction parameters */

    reducenoise = True;
    missinglayers[0][0] = 2;
    missinglayers[0][1] = 2;
    missinglayers[1][0] = 2;
    missinglayers[1][1] = 2;
    missinglayers[2][0] = 2;
    missinglayers[2][1] = 2;

    layershifts[0] = 0;
    layershifts[1] = 2;
    layershifts[2] = 3;
    layershifts[3] = 3;
    layershifts[4] = 4;
    layershifts[5] = 4;

}

/*----------- checkDCData ---------*/

Boolean checkDCData(short sect,
		    DC0DataPtr hits) 
{
    unsigned char    layer, wire;
    short            rgn, supl, lay;
    DC_Super_Layer   *supPtr;
    DC_Layer         *layerPtr;


    if (hits == NULL) {
	fprintf(stderr, "null hits\n");
	return True;
    }

    BreakShort(hits->id, &wire, &layer);

/* convert to C indices */

    wire--;
    layer--;

/* check out of range layer */

    if (layer > 35) {
/*	fprintf(stderr, "bad layer %d\n", layer+1); */
	SetEventBad(BADDCLAYER, layer+1);
	return False;
    }
    
/* check for nonexistant layer */

    if ((layer == 4) || (layer == 5)) {
/*	fprintf(stderr, "bad layer %d (there is no such layer) \n", layer+1); */
	SetEventBad(BADDCLAYER, layer+1);
	return False;
    }
    
    rgn = layer / 12;         /* 0..2 */
    supl = (layer % 12) / 6;  /* the 0..1 variety */
    lay = layer % 6;


/* use the first_wire and last_wire c indices as a test */
   
    supPtr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    layerPtr = &(supPtr->layers[lay]);

    if ((wire < layerPtr->first_wire) || (wire > layerPtr->last_wire)) {
/*	fprintf(stderr, "bad wire %d in layer %d Wires in this layer should be between %d and %d\n",
		wire+1, layer+1,
		layerPtr->first_wire+1, layerPtr->last_wire+1); */
	SetEventBad(BADDCWIRE, wire+1);
	return False;
    }
    
    return True;
}


/*----------- checkDC1Data ---------*/

Boolean checkDC1Data(short sect,
		    DC1DataPtr hits) 
{
    unsigned char    layer, wire;
    short            rgn, supl, lay;
    DC_Super_Layer   *supPtr;
    DC_Layer         *layerPtr;


    if (hits == NULL) {
	fprintf(stderr, "null hits\n");
	return True;
    }

    BreakShort((short)(hits->id), &wire, &layer);

/* convert to C indices */

    wire--;
    layer--;

/* check out of range layer */

    if (layer > 35) {
	fprintf(stderr, "bad DC1 layer %d\n", layer+1);
	SetEventBad(BADDCLAYER, layer+1);
	return False;
    }
    
/* check for nonexistant layer */

    if ((layer == 4) || (layer == 5)) {
	fprintf(stderr, "bad DC1 layer %d (there is no such layer) \n", layer+1);
	SetEventBad(BADDCLAYER, layer+1);
	return False;
    }
    
    rgn = layer / 12;         /* 0..2 */
    supl = (layer % 12) / 6;  /* the 0..1 variety */
    lay = layer % 6;


/* use the first_wire and last_wire c indices as a test */
   
    supPtr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    layerPtr = &(supPtr->layers[lay]);

    if ((wire < layerPtr->first_wire) || (wire > layerPtr->last_wire)) {
/*	fprintf(stderr, "bad wire %d in layer %d Wires in this layer should be between %d and %d\n",
		wire+1, layer+1,
		layerPtr->first_wire+1, layerPtr->last_wire+1);*/
	SetEventBad(BADDCWIRE, wire+1);
	return False;
    }
    
    return True;
}


/* ----- SenseWirePosition ------*/

void SenseWirePosition(float   phi,
		       short   sect,
		       short   rgn,
		       short   suplay,
		       short   layer,
		       short   wire,
		       float   *fz,
		       float   *fx) {

    if (DCGeomBanks)
	BOSSenseWirePosition(phi, sect, rgn, suplay, layer, wire, fz, fx);
    else
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer, wire, fz, fx);

}

/* ----- BOSSenseWirePosition ------*/

static void BOSSenseWirePosition(float   phi,
				 short   sect,
				 short   rgn,
				 short   suplay,
				 short   layer,
				 short   wire,
				 float   *fz,
				 float   *fx) {

    int          boslayer, boswire;
    DCGMDataPtr  dcgm;
    DCGWDataPtr  dcgw;
    float        xo, zo, alpha;
    float        c_phi, s_phi, c_alpha, s_alpha, c_theta, s_theta, wx, wy, wz, denom;

    if ((dcgm = bosDCGM[sect]) == NULL) {
	fprintf(stderr, "Unrecoverable Geometry Error (A).\n");
	exit(1);
    }

    if ((dcgw = bosDCGW[sect]) == NULL) {
	fprintf(stderr, "Unrecoverable Geometry Error (B).\n");
	exit(1);
    }

/* get the boslayer in 1..36 mode */

    boslayer = (rgn * 12) + (suplay * 6) + layer + 1;

/* DCGW bank just assumes 192 wires in each layer */

    boswire = (192 * (boslayer-1)) + wire + 1;

    dcgm += (boslayer - 1);
    dcgw += (boswire - 1);

    xo = dcgw->x_mid;

    if (sect > 2)
	xo = -xo;

    zo = dcgw->z_mid;

    alpha = Hv_DEGTORAD*dcgm->stereo; 

/* phi and stereo angle correction */
    
    if (fabs(phi) < SMALLANG) {  /*if true, then midplane*/
	*fz = zo;
	*fx = xo;
    }
    else  {
	if (fabs(alpha) < SMALLANG) {  /*if true, then axial*/
	    *fz = zo;
	    *fx  =xo/cos(phi);
	}
	else {  /*generic case of stereo wire at nonzero phi*/

/* still an open question whether we all agree on what is "positive"
   alpha. Thus the sign in front of sin could change */
	    
	    c_alpha = cos(alpha);
	    s_alpha = -sin(alpha);
	    c_phi =   cos(phi);
	    s_phi =   sin(phi);
	    
/*w's are direction cosines of the stereo wire*/
	    
	    wx = -s_alpha * c_theta;
	    wy = c_alpha;
	    wz = s_alpha * s_theta;
	    
	    denom = wy*c_phi + wx*s_phi;
	    *fz = zo - (xo*wz*s_phi/denom);
	    *fx = xo*wy/denom;
	    
	}
    }

/*    DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer, wire, fz, fx); */

/*    if ((sect == 0) && (rgn == 0) && (suplay == 1))
	fprintf(stderr, "(%f, 0.0, %f)   (%f, %f, %f)\n", 
	*fx, *fz, dcgw->x_mid, dcgw->y_mid, dcgw->z_mid); */


}

/* ----- DatabaseSenseWirePosition ------*/

static void DatabaseSenseWirePosition(float   phi,
				      short   sect,
				      short   rgn,
				      short   suplay,
				      short   layer,
				      short   wire,
				      float   *fz,
				      float   *fx) {


/*
 *
 *  USES THE ced geometry database, NOT the BOS geometry banks
 *
 * 1) returns the world coordinates of the wire requested
 * in viewplane specified by phi.
 * phi is  the azimuthal angle of projection in radians.
 * 0 -> midplane, Pi/6 -> coil plane
 *
 * THUS IN MOST CASES phi is really Hv_DEGTORAD*viewdata->dphi
 * (note: dphi) where viewdata is for the view in question
 *
 * 2) HANDLES BOTH NON ZERO Phi AND STEREO WIRE EFFECT
 *
 * 3) NO FURTHER MidplaneXToProjectedX should be applied
 *
 * 4) ALL indices (sect, rgn, suplay and wire) are C INDICES
 */
  
    
    float            radius, theta, xc, zc, xo, zo, alpha, theta_min, dtheta;
    float            dlen, c_phi, s_phi, c_alpha, s_alpha, c_theta, s_theta, wx, wy, wz, denom;
    DC_Layer         *LayerPtr;
    short            lastcirc, firstwire, lastwire;
    DC_Super_Layer   *supptr;
    
/*check that we haven't made a request for an out-of-bounds wire.
  BE EVER MINDFUL OF "Cs" INDICES STARTING AT ZERO*/
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[suplay]);
    LayerPtr = &(supptr->layers[layer]);
    
    dlen =   supptr->dlen;
    dtheta = supptr->dtheta;
    zc =     supptr->zc;
    xc =     supptr->xc;
    alpha =  supptr->stereo_angle;
    
    lastcirc  = LayerPtr->lastcircwire;
    firstwire = LayerPtr->first_wire;
    lastwire  = LayerPtr->last_wire;
    
    if ((wire < firstwire) || (wire > lastwire)) {
/*    fprintf(stdout, "Warning: wire out of range in SenseWirePosition [%d] [%d, %d]\n",
      wire, firstwire, lastwire); */
    }
    
    wire = Hv_sMax(wire, firstwire);
    wire = Hv_sMin(wire, lastwire);
    
    radius =    LayerPtr->radius;
    theta_min = LayerPtr->theta_min;
    
/* Get the generic midplane position. Must consider the
   possibility of being in straight part of region one*/
    
    if ((rgn == 0) && (wire > lastcirc)) {
	theta = theta_min + (lastcirc-firstwire)*dtheta;
	c_theta = cos(theta);
	s_theta = sin(theta);
	zo  =  zc + radius*c_theta - (wire - lastcirc)*dlen;
	xo  =  xc + radius*s_theta;
    }
    else {
	theta = theta_min + (wire-firstwire)*dtheta;
	c_theta = cos(theta);
	s_theta = sin(theta);
	zo  =  zc + radius*c_theta;
	xo  =  xc + radius*s_theta;
    }
    
    if (fabs(phi) < SMALLANG) {  /*if true, then midplane*/
	*fz = zo;
	*fx = xo;
    }
    else  {
	if (fabs(alpha) < SMALLANG) {  /*if true, then axial*/
	    *fz = zo;
	    *fx  =xo/cos(phi);
	}
	else {  /*generic case of stereo wire at nonzero phi*/
	    
/* still an open question whether we all agree on what is "positive"
   alpha. Thus the sign in front of sin could change */
	    
	    c_alpha = cos(alpha);
	    s_alpha = -sin(alpha);
	    c_phi =   cos(phi);
	    s_phi =   sin(phi);
	    
/*w's are direction cosines of the stereo wire*/
	    
	    wx = -s_alpha * c_theta;
	    wy = c_alpha;
	    wz = s_alpha * s_theta;
	    
	    denom = wy*c_phi + wx*s_phi;
	    *fz = zo - (xo*wz*s_phi/denom);
	    *fx = xo*wy/denom;
	    
	}
    }
}


/* ----- FieldWirePosition -------------*/

void FieldWirePosition(float      phi,
		       short      sect,
		       short      rgn,
		       short      suplay,
		       short      layer,
		       short      sensewire,
		       Hv_FPoint *fpts,
		       short      control) {
    if (DCGeomBanks)
	BOSFieldWirePosition(phi, sect, rgn, suplay, layer, sensewire, fpts, control);
    else
	DatabaseFieldWirePosition(phi, sect, rgn, suplay, layer, sensewire, fpts, control);
}

/* ----- BOSFieldWirePosition -------------*/

void BOSFieldWirePosition(float      phi,
			  short      sect,
			  short      rgn,
			  short      suplay,
			  short      layer,
			  short      sensewire,
			  Hv_FPoint *fpts,
			  short      control) {


    float  dbx,  dbz;    /* positions from database */
    float  bosx, bosz;   /* positions from BOS bank */

    DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer, sensewire, &dbz, &dbx);
    DatabaseFieldWirePosition(phi, sect, rgn, suplay, layer, sensewire, fpts, control);

    BOSSenseWirePosition(phi,sect, rgn, suplay, layer, sensewire, &bosz, &bosx);

    if (Hv_CheckBit(control, POINT1)) {
	fpts[0].h = bosz + (fpts[0].h - dbz); 
	fpts[0].v = bosx + (fpts[0].v - dbx); 
    }

    if (Hv_CheckBit(control, POINT2)) {
	fpts[1].h = bosz + (fpts[1].h - dbz); 
	fpts[1].v = bosx + (fpts[1].v - dbx); 
    }

    if (Hv_CheckBit(control, POINT3)) {
	fpts[2].h = bosz + (fpts[2].h - dbz); 
	fpts[2].v = bosx + (fpts[2].v - dbx); 
    }

    if (Hv_CheckBit(control, POINT4)) {
	fpts[3].h = bosz + (fpts[3].h - dbz); 
	fpts[3].v = bosx + (fpts[3].v - dbx); 
    }

    if (Hv_CheckBit(control, POINT5)) {
	fpts[4].h = bosz + (fpts[4].h - dbz); 
	fpts[4].v = bosx + (fpts[4].v - dbx); 
    }

    if (Hv_CheckBit(control, POINT6)) {
	fpts[5].h = bosz + (fpts[5].h - dbz); 
	fpts[5].v = bosx + (fpts[5].v - dbx); 
    }
	
}



/* ----- DatabaseFieldWirePosition -------------*/

void DatabaseFieldWirePosition(float      phi,
			       short      sect,
			       short      rgn,
			       short      suplay,
			       short      layer,
			       short      sensewire,
			       Hv_FPoint *fpts,
			       short      control)
    
/* returns the world coordinates of the FIELD wires
   requested in viewplane specified by phi (usually a
   views dphi*Hv_DEGTORAD.
   
   The field wires are the vertices of the cell surrounding
   the sense wire specified by "sensewire".
   
   "control", controls in a bitwise fashion, which field wires are
   determined. Assumes fpts alloced in calling program*/
    
{
    
    float            oldradius;
    float            oldthetamin;
    float            dtheta;
    float            dr1, dr3, dr;
    DC_Layer         *LayerPtr;
    DC_Super_Layer   *supptr;
    
/* will obtain field wire position by "trick" -- parameters will be adjusted so that a sense
   wire will fall at the position of the desired field wire. Then the sense wire function
   is called and finally the orginal parameters are restored */
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[suplay]);
    LayerPtr = &(supptr->layers[layer]);
    
    oldradius =   LayerPtr->radius;
    oldthetamin = LayerPtr->theta_min;
    dtheta =      supptr->dtheta/2.0;
    
    dr = supptr->dR;       /*mid plane radia gap for this superlayer*/
    dr3 = dr/3.0;
    dr1 = 2.0*dr3;
    
/* the points refer to one of the six vertices of the hexagon.
   Being selective allows us to optimize when we are drawing
   many hexagons that share vertices */
    
    if (Hv_CheckBit(control, POINT1)) {
	LayerPtr->radius = oldradius-dr3;
	LayerPtr->theta_min = oldthetamin+dtheta;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[0].h),&(fpts[0].v));
    }
    
    if (Hv_CheckBit(control, POINT2)) {
	LayerPtr->radius = oldradius-dr1;
	LayerPtr->theta_min = oldthetamin;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[1].h),&(fpts[1].v));
    }
    
    if (Hv_CheckBit(control, POINT3)) {
	LayerPtr->radius = oldradius-dr3;
	LayerPtr->theta_min = oldthetamin-dtheta;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[2].h),&(fpts[2].v));
    }
    
    if (Hv_CheckBit(control, POINT4)) {
	LayerPtr->radius = oldradius+dr3;
	LayerPtr->theta_min = oldthetamin-dtheta;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[3].h),&(fpts[3].v));
    }
    
    if (Hv_CheckBit(control, POINT5)) {
	LayerPtr->radius = oldradius+dr1;
	LayerPtr->theta_min = oldthetamin;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[4].h),&(fpts[4].v));
    }
    
    if (Hv_CheckBit(control, POINT6)) {
	LayerPtr->radius = oldradius+dr3;
	LayerPtr->theta_min = oldthetamin+dtheta;
	DatabaseSenseWirePosition(phi, sect, rgn, suplay, layer,
				  sensewire, &(fpts[5].h),&(fpts[5].v));
    }
    
/*restore parameters*/
    
    LayerPtr->radius = oldradius;
    LayerPtr->theta_min = oldthetamin;
}


/* ---- GetDCCellPolygon -------- */

void GetDCCellPolygon(Hv_View    View,
		      short      sect,
		      short      rgn,
		      short      suplay,
		      short      layer,
		      short      wire,
		      Hv_Point  *pp,
		      Hv_FPoint *fpts)
    
/* puts the hexagon shape of the cell into the
   pp "array". Assumes that the calling program has
   alloced the memory for pp and fpts
   
   ALL INDICES ARE C INDICES  */
    
{
    ViewData   viewdata = GetViewData(View);
    short      i;
    
    FieldWirePosition(Hv_DEGTORAD*viewdata->dphi, sect, rgn,
		      suplay, layer, wire, fpts, ALLPOINTS);
    
/* now convert to local pixel coordinates*/
    
    for (i = 0; i < 6; i++)
	Hv_WorldToLocal(View, fpts[i].h, fpts[i].v, &(pp[i].x), &(pp[i].y));
    
}

/*------ DrawSEGHits ------*/

void DrawSEGHits(Hv_View   View,
		 short      sect,
		 Hv_Region  region)
    
/* draws the level 2 trigger segments */
    
/* sect is a C index [0..5] */
    
    
{
    DC0DataPtr     dc0hits = NULL;
    DC0DataPtr     tdchits = NULL;
    int            numdc0hits;
    int            i, j;
    SEGDataPtr     hits;
    short          layer, wire;
    short          savecolor;
    
    if (bosIndexSEG[sect] == 0)
	return;
    
/* try to trick it by converting to a DC0 bank */
    
/* each segment hit corresponds to at most 12  normal dc hits */
    
    numdc0hits = 12*bosNumSEG[sect];
    
    dc0hits = (DC0DataPtr)Hv_Malloc(numdc0hits*sizeof(DC0Data));
    tdchits = dc0hits;
    
/* pack the seg hits into dc0 hits */
    
    hits = bosSEG[sect];
    for (j = 0; j < bosNumSEG[sect]; j++) {
	
	
/* NOTE: superlayer IDs in seg banks start at
   1, segids start at 0, when converting
   segids to wires keep in mind the
   first wire problem */
	
	for (i = 0; i < 6; i++) {
	    layer = 6*(hits->superlayer-1) + i + 1;
	    
	    wire = 2*(hits->segid) + 1;
	    
/* skip if the wire is not physical, as in the
   forward part of region 1 */
	    
	    
	    tdchits->id = layer*256 + wire;
	    
	    if (CheckDCHit(sect, (unsigned char)layer, (unsigned char)wire))
		tdchits->tdc = 0;
	    else
		tdchits->tdc = 32001;
	    
	    
	    tdchits++;
	    wire++;
	    
	    tdchits->id = layer*256 + wire;
	    
	    if (CheckDCHit(sect, (unsigned char)layer, (unsigned char)wire))
		tdchits->tdc = 0;
	    else
		tdchits->tdc = 32001;
	    
	    
	    tdchits++;
	}
	
	hits++;
    }  /* end seg bank loop */
    
    
    savecolor = dchitcolor;
    dchitcolor = dctrigsegcolor;
    GenericDrawDC0Hits(View, sect, numdc0hits,
		      (char *)dc0hits, sizeof(DC0Data), region);
    dchitcolor = savecolor;
    
    
    Hv_Free(dc0hits);
}

/*------ DrawDC0Hits ------*/

void DrawDC0Hits(Hv_View   View,
		 short     sect,
		 Hv_Region region)
    
/* draw the new DC0 bank */
    
{
    DC0DataPtr     hits;
    ViewData viewdata = GetViewData(View);

    if (!(viewdata->displayDC0)) {
	return;
    }
	

    hits = bosDC0[sect];
    
    if  ((View->tag == SECTOR) || (View->tag == ALLDC))
	GenericDrawDC0Hits(View, sect, bosNumDC0[sect],
			  (char *)hits, sizeof(DC0Data), region);
}


/*------ DrawDC1Hits ------*/
/* draw the new DC1 bank */

void
DrawDC1Hits(Hv_View   View,
		    short     sect,
		    Hv_Region region)
{
    DC1DataPtr     hits;
    HBLADataPtr    hbla;
    int i, j, banksize;

    ViewData viewdata = GetViewData(View);


	/*sergey: reget DC1 bank
    for(i = 0; i < 6; i++)
    {
	  j = i + 1;
	  bosIndexDC1[i] = mlink_(bcs_.iw,bosBankNameDC1, &j, 4);
	  if (bosIndexDC1[i] != 0)
      {
        printf("DC1 in sec %d\n",j);
		banksize = bosBankSize(bosIndexDC1[i]);
		bosNumDC1[i] = banksize/sizeof(DC1Data);
		bosDC1[i]    = (DC1DataPtr)(&(bcs_.iw[bosIndexDC1[i]]));
		bosSectorDataSize[i] += banksize;
	  }
	}
	*/




    hits = bosDC1[sect];
	/*
	printf("DrawDC1Hits 1: %d\n",hits);
	*/
    if (!(viewdata->displayDC1))
    {
	  return;
    }
	/*
	printf("DrawDC1Hits 2\n");
	*/
    if (View->tag == ALLDC) 
    {
	  DrawAllDC1Hits(View, sect, bosNumDC1[sect], hits, region);
	  return;
    }
	

    hbla = bosHBLA[sect];

    if  (View->tag == SECTOR) {
	if (viewdata->dc1control == ALLDC1BANKS)
	    GenericDrawDC1Hits(View, sect, bosNumDC1[sect],
			       hits, region);
	else
	    DrawDC1TrackHits(View, sect,
			     bosNumDC1[sect],
			     bosNumHBLA[sect],
			     hits, hbla, region);
    }

}



/*------ DrawDeadWires------*/

void DrawDeadWires(Hv_View   View,
		   short     sect,
		   Hv_Region region)
    
/* draw dead dc wires */
    
{
    short    rgn, supl;

    for (rgn = 0; rgn < 3; rgn++)
	for (supl = 0; supl < 2; supl++)
	    DrawDeadSuperLayerWires(View, sect, rgn, supl, region);
}



/*------ DrawDeadSuperLayerWires------*/

static void DrawDeadSuperLayerWires(Hv_View   View,
				    short     sect,
				    short     rgn,     /*0..2*/
				    short     supl,    /*0..1*/
				    Hv_Region region)
    
/* draw dead dc wires */
    
{
    
    int              i;
    unsigned char    layer, wire;
    short            lay;
    ViewData         viewdata;
    Hv_Point         poly[6];
    Hv_FPoint        fpoly[6];
    short            fill, frame;
    Boolean          shoulddrawhex, drawhexagons;
    Hv_Point         pp;
    float            phi, fx, fz;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    int              pixdens;
    short            rectw;
    Hv_Rect          rect;


/*fprintf(stderr, "drawing dead wires for %d %d %d\n", sect+1, rgn+1, supl+1); */

/* first check, are there ANY in this supl */

    if (dcstatus[sect][rgn][supl] == NULL)
	return;

    if (View->tag == ALLDC)
    	return;
    
    viewdata = GetViewData(View);
    
    if (!(viewdata->displayDead))
	return;
    
    frame = dcframecolor;
    
/* regular sector view */
    
    pixdens = PixDens(View);
    phi = Hv_DEGTORAD*viewdata->dphi;
    fill = dcdeadwirecolor;
    rectw = 2 + rgn;

    i = 0;
    while(dcstatus[sect][rgn][supl][i].status != -99) {
    
	BreakShort(dcstatus[sect][rgn][supl][i].id, &wire, &layer);
	
/* layer is 1..36 and must be converted */
	
	wire--;
	layer--;

	lay = layer % 6;  /* 0..5 variety */

	supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
	layerPtr = &(supptr->layers[lay]);
	    
	if ((wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
	    SenseWirePosition(phi, sect, rgn, supl, lay, wire, &fz, &fx);
	    Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	    
/* hexagon or just rectangles ? */
	    
	    shoulddrawhex = (pixdens > dchexagon[rgn]);
	    drawhexagons = shoulddrawhex && viewdata->displayHex;

	    if (drawhexagons) {
		GetDCCellPolygon(View, sect, rgn, supl, lay, (short)wire, poly, fpoly);
		Hv_FillPolygon(poly, 6, True, fill, frame);
	    }
	    
	    else {
		Hv_SetRect(&rect, pp.x - rectw/2, pp.y - rectw/2, rectw, rectw);
		Hv_FillRect(&rect, fill);
	    }

	} /* end wire in range */
	
	i++;
    }  /* end main while */
}


/* -------- GoodDCTDCValue -------------*/

Boolean GoodDCTDCValue(short rgn,         /*0..2*/
		       short tdc) {

    return ((tdc > dcmintime[rgn]) && (tdc < dcmaxtime[rgn]));

}


/*------- DrawDC1TrackHits ---------*/

static void DrawDC1TrackHits(Hv_View     View,
			     short       sect,
			     int         numhits,
			     int         numtracks,
			     DC1DataPtr  hits,
			     HBLADataPtr  hbla,
			     Hv_Region   region) {

    DC1DataPtr hit;
    int j;
    int dc1tag;
    unsigned char    layer, wire;
    short            rgn, lay, supl;
    ViewData         viewdata;
    Hv_Point         poly[6];
    Hv_FPoint        fpoly[6];
    short            fill, frame;
    Boolean          shoulddrawhex, drawhexagons;

    Hv_Point         pp;
    float            phi, fx, fz;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    int              pixdens;

    if ((numhits == 0) || (hits == NULL) || (numtracks == 0) || (hbla == NULL))
	return;

    viewdata = GetViewData(View);

    if (!(viewdata->displayDC))
	return;
    
    frame = dcframecolor;
    
/* regular sector view */
    
    pixdens = PixDens(View);
    phi = Hv_DEGTORAD*viewdata->dphi;
    fill = dcfromdc1color;


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
		supl = (layer % 12) / 6;
		lay = layer % 6;

	
		supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
		layerPtr = &(supptr->layers[lay]);
	    
		if ((fill > -1) && (wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
		    SenseWirePosition(phi, sect, rgn, supl, lay, wire, &fz, &fx);
		    Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	    
/* hexagon or just rectangles ? */
	    
		    shoulddrawhex = (pixdens > dchexagon[rgn]);
		    drawhexagons = shoulddrawhex && viewdata->displayHex;
		    
		    if (drawhexagons) {
			GetDCCellPolygon(View, sect, rgn, supl, lay, (short)wire, poly, fpoly);
			Hv_FillPolygon(poly, 6, True, fill, frame);
		    }

	    
		    else {
			if (pixdens < dcbighitwires[rgn])
			    Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
			else {
			    if (shoulddrawhex)
				Hv_DrawSymbolsOnPoints(&pp, 1, 3, fill, Hv_RECTSYMBOL);
			    else
				Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
			}
		    }
		} /* end wire in range */
	    }


	}

	else {
	    if (dc1tag != 0)
		fprintf(stderr, "Bad HBLA DC1 tag: %d legal range is [1, %d]\n", dc1tag, numhits);
	}

	hbla++;
    }

}

/*------- GenericDrawDC1Hits ---------*/

static void GenericDrawDC1Hits(Hv_View     View,
			       short       sect,
			       int         numhits,
			       DC1DataPtr  hits,
			       Hv_Region   region) {
    
    int              i;
    unsigned char    layer, wire;
    short            rgn, lay, supl;
    ViewData         viewdata;
    Hv_Point         poly[6];
    Hv_FPoint        fpoly[6];
    short            fill, frame;
    Boolean          shoulddrawhex, drawhexagons;
    Hv_Point         pp;
    float            phi, fx, fz;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    int              pixdens;



/* ship to a special handler if the all dc view */
    
    if (View->tag == ALLDC) {
	DrawAllDC1Hits(View,
		      sect,
		      numhits,
		      hits,
		      region);
    	return;
    }

    viewdata = GetViewData(View);

    
    if (((hits == NULL) || (numhits < 1)) && (viewdata->showwhat == SINGLEEVENT))
	return;
    
    if (!(viewdata->displayDC))
	return;
    
    frame = dcframecolor;
    
/* regular sector view */
    
    pixdens = PixDens(View);
    phi = Hv_DEGTORAD*viewdata->dphi;
    fill = dcfromdc1color;

    for (i = 0; i < numhits; i++) {
	
	if (checkDC1Data(sect, hits)) {
	    BreakShort((short)(hits->id), &wire, &layer);
	
/* layer is 1..36 and must be converted */
	
	    wire--;
	    layer--;

	    rgn = layer / 12;
	    supl = (layer % 12) / 6;
	    lay = layer % 6;

	
	    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
	    layerPtr = &(supptr->layers[lay]);
	    
	    if ((fill > -1) && (wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
		SenseWirePosition(phi, sect, rgn, supl, lay, wire, &fz, &fx);
		Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	    
/* hexagon or just rectangles ? */
	    
		shoulddrawhex = (pixdens > dchexagon[rgn]);
		drawhexagons = shoulddrawhex && viewdata->displayHex;

		if (drawhexagons) {
		    GetDCCellPolygon(View, sect, rgn, supl, lay, (short)wire, poly, fpoly);
		    Hv_FillPolygon(poly, 6, True, fill, frame);
		}

		else {
		    if (pixdens < dcbighitwires[rgn])
			Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
		    else {
			if (shoulddrawhex)
			    Hv_DrawSymbolsOnPoints(&pp, 1, 3, fill, Hv_RECTSYMBOL);
			else
			    Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
		    }
		}
	    } /* end wire in range */
	}

	hits++;;
	
    }  /* end for numhits */

}



/*------- GenericDrawDC0Hits ---------*/

static void
GenericDrawDC0Hits(Hv_View     View,
			       short       sect,
			       int         numhits,
			       char       *chits,
			       size_t      size,
			       Hv_Region   region)
{
    
/* works for both DC0 (the new banks) and DC (the old) */

  DC0DataPtr       hits;
  int              i;
  unsigned char    layer, wire;
  short            rgn, lay, supl;
  ViewData         viewdata;
  Hv_Point         poly[6];
  Hv_FPoint        fpoly[6];
  short            fill, frame;
  Boolean          shoulddrawhex, drawhexagons;
  Hv_Point         pp;
  float            phi, fx, fz;
  DC_Layer         *layerPtr;
  DC_Super_Layer   *supptr;
  int              pixdens;

/* ship to a special handler if the all dc view */
    
  if(View->tag == ALLDC)
  {
	DrawAllDCHits(View, sect, numhits, chits, size, region);
    return;
  }

    
  viewdata = GetViewData(View);
    
  if(((chits == NULL) || (numhits < 1)) && (viewdata->showwhat == SINGLEEVENT)) return;
    
  if(!(viewdata->displayDC)) return;
    
  frame = dcframecolor;
    
/* regular sector view */
    
  pixdens = PixDens(View);
  phi = Hv_DEGTORAD*viewdata->dphi;
    
  for (i = 0; i < numhits; i++)
  {
	hits = (DC0DataPtr)chits;
	
	if ((bosIndexSEG[sect] > 0) && (hits->tdc > 32000))
    {
/*	    fprintf(stderr, "huge TDC: %d\n", hits->tdc);*/
	    fill  = dcaddedbytrigcolor;
	}
	else
	    fill  = dchitcolor;
	
	if (checkDCData(sect, hits))
    {
	    BreakShort(hits->id, &wire, &layer);
	
/* layer is 1..36 and must be converted */
	
	    wire--;
	    layer--;

	    rgn = layer / 12;
	    supl = (layer % 12) / 6;
	    lay = layer % 6;

/* see if a noise hit */
	    if(reducenoise)
        {
          /*Sergey: viewdata->suppressNoise set by button on 'Sector' view,
            it does not effect 'Drift Chamber' view*/
		  /*printf("SUUUUUUPRESS!!!!!!!!!!!! %d\n",viewdata->suppressNoise);*/

		  /*
		  if (!CheckBitHugeWord(cleandcbitdata[sect][rgn][supl].data[lay], wire))
		    if (viewdata->suppressNoise)
			  fill = -1;
		    else
			  fill = dcnoisecolor;
		  */
	    }

/* see if out of time */

	    if ((fill == dchitcolor) && (!GoodDCTDCValue(rgn, hits->tdc)))
		fill = dcbadtimecolor;

	
	    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
	    layerPtr = &(supptr->layers[lay]);
	    
	    if ((fill > -1) && (wire >= layerPtr->first_wire) && (wire <= layerPtr->last_wire)) {
		SenseWirePosition(phi, sect, rgn, supl, lay, wire, &fz, &fx);
		Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	    
/* hexagon or just rectangles ? */
	    
		shoulddrawhex = (pixdens > dchexagon[rgn]);
		drawhexagons = shoulddrawhex && viewdata->displayHex;

		if (drawhexagons) {
		    GetDCCellPolygon(View, sect, rgn, supl, lay, (short)wire, poly, fpoly);
		    Hv_FillPolygon(poly, 6, True, fill, frame);
		}
	    
		else {
		    if (pixdens < dcbighitwires[rgn])
			Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
		    else {
			if (shoulddrawhex)
			    Hv_DrawSymbolsOnPoints(&pp, 1, 3, fill, Hv_RECTSYMBOL);
			else
			    Hv_DrawSymbolsOnPoints(&pp, 1, 2, fill, Hv_RECTSYMBOL);
		    }
		}
	    } /* end wire in range */
	}

	chits += size;
	
    }  /* end for numhits */
}

/*------ MallocDC ------*/

Hv_Item MallocDC(Hv_View  View,
		 int      sect,
		 short    superlayer)
    
/* sect and superlayer are C indices 
   here suplay is 0..5, i.e. it is
   NOT the reduced 0..1 superlayer */
    
{
    char       text[40];
    Hv_FPoint  *fpts;
    Hv_Item    Item;
    int        np;
    
    sprintf(text, "DC Superlayer %d", superlayer+1);
    
    SuperLayerFPolygon(View, sect, superlayer, &np, &fpts);
    
    Item = Hv_VaCreateItem(View,
			   Hv_TYPE,         Hv_WORLDPOLYGONITEM,
			   Hv_TAG,          DCITEM,
			   Hv_NUMPOINTS,    np,
			   Hv_DATA,         fpts,
			   Hv_NUMROWS,      1,
			   Hv_NUMCOLUMNS,   1,
			   Hv_DOMAIN,       Hv_INSIDEHOTRECT,
			   Hv_DOUBLECLICK,  EditDCItem,
			   Hv_DRAWCONTROL,  Hv_ZOOMABLE + Hv_INBACKGROUND,
			   Hv_AFTEROFFSET,  DCAfterOffset,
			   Hv_USER1,        (int)superlayer,
			   Hv_USER2,        (int)sect,
			   Hv_BALLOON,      (void *)text,
			   Hv_FIXREGION,    FixDCSuperLayerRegion,
			   NULL);
    
    Item->type = Hv_USERITEM;
    Item->standarddraw = DrawDCItem;
    return  Item;
}

/*------ SetDCGeometrySector ------*/

void SetDCGeometrySector(Hv_View  View)
    
{
    int                  i;
    Hv_Item              temp;
    Hv_FPoint            *fpts;
    Hv_WorldPolygonData  *wpoly;
    ViewData             viewdata;
    short                sect;
    
    if (View->tag != SECTOR)
	return;
    
    viewdata = GetViewData(View);
    
    for (i = 0; i < NUM_SUPERLAYER2; i++)
	if ((temp = viewdata->DCItems[i]) != NULL) {
	    sect = SetItemSector(temp);
	    
	    wpoly = (Hv_WorldPolygonData *)(temp->data);
	    fpts = wpoly->fpts;
	    temp->fixregion(temp);
	}
    
}

/*------ GetDCHit ------*/

void GetDCHit(Hv_Item   Item,
	      Hv_Point  pp,
	      short     sect,         /* 0..5 */
	      short     superlayer,   /* 0..5 */
	      float    *occupancy,
	      int      *nhit,
	      short    *layer,
	      short    *wire,
	      short    *tdc)
    
{
    short    supl, rgn;
    float    fz, fx;
    Hv_View  View = Item->view;
    
    DC_Layer         *LayerPtr;
    DC_Super_Layer   *supptr;
    
/* assume failure */
    
    *layer = -1;
    *wire = -1;
    *occupancy = 0.0;
    *tdc = -1;
    *nhit = 0;
    
/* covert to the C indices */
    
    rgn = superlayer / 2;
    supl  = superlayer % 2;    /* reduced */
    
    
    Hv_LocalToWorld(View, &fz, &fx, pp.x, pp.y);
    WhatWire(View, fz, fx, pp, sect, rgn, supl, layer, wire);
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    LayerPtr = &(supptr->layers[*layer]);
    
    if (*wire < 0)
	return;
    
    GetDCTDCValue(sect, rgn, supl, *layer, *wire, occupancy, nhit, tdc);

}

/* ------- GetDCAccumValue --------*/

void GetDCAccumValue(short sect,        /* 0..5 */
		     short rgn,         /*0..2*/
		     short supl,        /* 0..1 */
		     short layer,       /* 0..5 */
		     short wire,        /* 0..191*/
		     int  *acccnt,
		     int  *avgtdc,
		     float *avgocc)

{
    int lay;

/* get the avg occupancy */

    if (accumEventCount > 0)
	*avgocc =  accdc[sect][rgn].occsum[supl]/accumEventCount;
    else
	*avgocc = 0.0;

/* get the avg tdc for that wire */

    lay = supl*6 + layer;  /* 0..11*/
    *acccnt =  accdc[sect][rgn].counts[lay][wire];

    if (*acccnt > 0)
	*avgtdc =  (int)(accdc[sect][rgn].tdcsum[lay][wire]/(*acccnt));
    else
	*avgtdc = -1;

}



/*---- GetDCTDCValue --------*/


void GetDCTDCValue(short sect,        /* 0..5 */
		   short rgn,         /*0..2*/
		   short supl,        /* 0..1 */
		   short layer,       /* 0..5 */
		   short wire,        /* 0..191*/
		   float *occupancy,
		   int   *nhit,
		   short *tdc)

{
    int           i, tsupl;
    unsigned char hlayer, hwire,tlayer, twire;
    DC0DataPtr    hits;    /* new hits*/


    *occupancy = -1.0;
    *tdc = -1;
    *nhit = 0;

/* at this point, "wire" will not be used except by
   feedback. It is  safe to add the first wire offset */
    
    tsupl = 2*rgn + supl;  /* 0..5 variety */
    tlayer = (unsigned char)(6*tsupl + layer + 1);
    twire = (unsigned char)(wire + 1);

/*  *wire += LayerPtr->first_wire;  */
    
/* now get the hit */
    
    if (bosIndexDC0[sect] == 0)
	return;
    
    *occupancy = SuperlayerOccupancy(sect, rgn, supl, nhit);
    
/* first try the new banks */
    
    hits = bosDC0[sect];
    for (i = 0; i < bosNumDC0[sect]; i++) {
	BreakShort(hits->id, &hwire, &hlayer);
	if ((hwire == twire) && (hlayer == tlayer)) {
	    *tdc = hits->tdc;
	    return;
	}
	hits++;
    }
    
}

/*----- CheckDCHit --------*/

static Boolean CheckDCHit(short         sect,
			  unsigned char layer,
			  unsigned char wire)
    
/* returns true if matching hit */
    
/* wire is NOT a c index */
    
{
    DC0DataPtr      hits;    /* new hits*/
    unsigned char   hwire, hlayer;
    int             i;
    
/* try the new banks */
    
    hits = bosDC0[sect];
    for (i = 0; i < bosNumDC0[sect]; i++) {
	BreakShort(hits->id, &hwire, &hlayer);
	if ((hwire == wire) && (hlayer == layer))
	    return True;
	hits++;
    }
    
    return False;
}

/*----- SuperlayerOccupancy -----*/

float SuperlayerOccupancy(short sect,
			  short rgn,
			  short supl,
			  int  *nhit)

/*
     sect: 0..5
     rgn:  0..2
     supl  0..1
*/
    
{
    DC_Super_Layer   *supptr;
    DC_Layer         *LayerPtr;
    int              nw = 0;
    int              i;
    unsigned char    wire, layer, minlay, maxlay;
    DC0DataPtr       hits;
    
    if (bosNumDC0[sect] < 1)
	return 0.0;
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    
    
    for (layer = 0; layer < supptr->numlayers; layer++) {
	LayerPtr = &(supptr->layers[layer]);
	nw += LayerPtr->num_wires;
    }
    
/* count the hits */
    
    minlay = 12*rgn + 6*supl + 1;
    maxlay = minlay+6;
    
    *nhit = 0;
    hits = bosDC0[sect];
    for (i = 0; i < bosNumDC0[sect]; i++) {
	BreakShort(hits->id, &wire, &layer);
	if ((layer >= minlay) && (layer < maxlay))
	    (*nhit)++;
	hits++;
    }
    
    return (100.0*(float)(*nhit))/nw;
}


/*------ DrawDCItem ----------*/

static void  DrawDCItem(Hv_Item   Item,
			Hv_Region region)
    
{
    Hv_View               View = Item->view;
    ViewData              viewdata;
    Hv_WorldPolygonData  *wpoly;
    short                 sect, superlayer;
    short                 supl, dcrgn;
    short                 ni, nf;
    
    viewdata = GetViewData(View);
    
    if (!(viewdata->displayDC))
	return;
    
    wpoly = (Hv_WorldPolygonData *)(Item->data);
    if (wpoly->poly == NULL)
	return;
    
    
    if (FrameOnly)
	Hv_FramePolygon(wpoly->poly, wpoly->numpnts, dcframecolor);
    else
	Hv_FillPolygon(wpoly->poly, wpoly->numpnts, True, dcfillcolor,
		       dcframecolor);
    
    
/* draw the wires */
    
    superlayer = (short)Item->user1;  /* 0..5 */
    dcrgn = superlayer / 2;    /* 0..2 */
    supl  = superlayer % 2;    /* reduced [0.1]*/
    sect =  (short)Item->user2;
    
    if (DCSuperLayerVisibleWireLimits(View, sect, dcrgn, supl, &ni, &nf)) {
	DrawDCWires(View, sect, dcrgn, supl, ni, nf);
	DrawDeadSuperLayerWires(View, sect, dcrgn, supl, region);
    }
   
}

/*------- PixDens ----------*/

static int PixDens(Hv_View View)
    
{
    float  xpix, ypix;
    
    xpix = View->local->width  / (1 + View->world->width);
    ypix = View->local->height / (1 + View->world->height);
    
    xpix *= 100.0;  /* convert to cm */
    ypix *= 100.0;  /* convert to cm */
    
    return (int)Hv_fMin(xpix, ypix);
}


/*---------- EditDCItem -----------*/

static void EditDCItem(Hv_Event hvevent) {
    EditDCData();
}


/*---------- EditDCData -----------*/

void EditDCData()
    
{
    static Widget           dialog = NULL;
    Widget                  dummy, rowcol, rcAB, rca, rcb, rc, rc1, rcc[3];
    int                     answer, rgn, supl, i, j;
    char                    dtext[40];

    static Hv_Widget        wdcnormalwire[MAX_REGIONS];
    static Hv_Widget        wdchitcircle[MAX_REGIONS];
    static Hv_Widget        wdchexagon[MAX_REGIONS];
    static Hv_Widget        wdcmintime[MAX_REGIONS];
    static Hv_Widget        wdcmaxtime[MAX_REGIONS];
    static Hv_Widget        wmissinglayers[3][2], wlayershifts[6];
    static Hv_Widget        shownoise;

    char                    text[60];
    Hv_View                 temp;
    ViewData                viewdata;
    
    if (!dialog) {
	Hv_VaCreateDialog(&dialog, Hv_TITLE, " Drift Chamber Editor ", NULL);
	rowcol = Hv_DialogColumn(dialog, 0);
	dummy = Hv_StandardLabel(rowcol, "NOTE: These changes are applied to ALL Views!", 0);
	dummy = Hv_StandardLabel(rowcol, " ", 0);
	
	rc = Hv_DialogTable(rowcol, 3, 3);


	fillcolorlab     = Hv_SimpleColorLabel(rc, "   fill color", (Hv_FunctionPtr)EditDCColor);
	framecolorlab    = Hv_SimpleColorLabel(rc, "  frame color", (Hv_FunctionPtr)EditDCColor);
	hitcolorlab      = Hv_SimpleColorLabel(rc, "    hit color", (Hv_FunctionPtr)EditDCColor);
	noisecolorlab    = Hv_SimpleColorLabel(rc, "    noise hit", (Hv_FunctionPtr)EditDCColor);
	trigsegcolorlab  = Hv_SimpleColorLabel(rc, "trig segments", (Hv_FunctionPtr)EditDCColor);
	trigaddcolorlab  = Hv_SimpleColorLabel(rc, "  trig \"adds\"", (Hv_FunctionPtr)EditDCColor);
	dcacolorlab      = Hv_SimpleColorLabel(rc, "   DOCA color", (Hv_FunctionPtr)EditDCColor);
        badtimecolorlab  = Hv_SimpleColorLabel(rc, "      bad TDC", (Hv_FunctionPtr)EditDCColor);
        deadwirecolorlab = Hv_SimpleColorLabel(rc, "    dead wire", (Hv_FunctionPtr)EditDCColor);

	
/* now the pixel denities */
	
	dummy = Hv_SimpleDialogSeparator(rowcol);
	dummy = Hv_StandardLabel(rowcol, "Rows 1-3 are pixel densities (pix/cm) controlling when various features will", 0);
	dummy = Hv_StandardLabel(rowcol, "be drawn based on how far in you have \"zoomed\". Smaller entries cause the", 0);
	dummy = Hv_StandardLabel(rowcol, "feature to draw sooner. Rows 4-5, Tmin and Tmax, delimit the \"good\" TDCs.", 0);

	dummy = Hv_StandardLabel(rowcol, " ", 0);
	rc = Hv_DialogRow(rowcol, 4);
	rc1 = Hv_DialogColumn(rc, 9);

	Hv_StandardLabel(rc1, " ", 0);
	Hv_StandardLabel(rc1, "wires", 0);
	Hv_StandardLabel(rc1, "hit circle", 0);
	Hv_StandardLabel(rc1, "hexagons", 0);
	Hv_StandardLabel(rc1, "Tmin", -2);
	Hv_StandardLabel(rc1, "Tmax", -2);

	for (j = 0; j < 3; j++) {
	    rcc[j] = Hv_DialogColumn(rc, 4);

	    sprintf(dtext, "Region %d", j+1);
	
	    Hv_StandardLabel(rcc[j], dtext, -2);
	    wdcnormalwire[j] = Hv_StandardTextField(rcc[j]);
	    wdchitcircle[j]  = Hv_StandardTextField(rcc[j]);
	    wdchexagon[j]    = Hv_StandardTextField(rcc[j]);
	    wdcmintime[j]    = Hv_StandardTextField(rcc[j]);
	    wdcmaxtime[j]    = Hv_StandardTextField(rcc[j]);
	}

	dummy = Hv_SimpleDialogSeparator(rowcol);
	dummy = Hv_StandardLabel(rowcol, "Parameters controlling the  dc noise reduction algorithm.", 0);
	dummy = Hv_SimpleDialogSeparator(rowcol);

	rcAB = Hv_DialogRow(rowcol, 30); 
	rca  = Hv_DialogColumn(rcAB, 10); 
	rcb  = Hv_DialogColumn(rcAB, 10); 
	

	shownoise = Hv_SimpleToggleButton(rca, "Show DC Noise");

	Hv_StandardLabel(rca, "Max Missing Layers by (Reg, Suplay)", -1);
	rc = Hv_DialogTable(rca, 3, 6); 
	for (rgn = 0; rgn < 3; rgn++)
	    for (supl = 0; supl < 2; supl++) {
		sprintf(text, "(%d, %d)", rgn+1, supl+1);
		wmissinglayers[rgn][supl] = Hv_SimpleTextEdit(rc, text, NULL, 6);
	    }


/* now the layer shifts */

	rc = Hv_DialogTable(rcb, 6, 2); 
	for (i = 0; i < 6; i++) {
	    sprintf(text, "shift layer %1d", i+1);
	    wlayershifts[i] = Hv_SimpleTextEdit(rc, text, NULL, 6);
	}

	dummy = Hv_StandardActionButtons(rowcol, 120, Hv_OKBUTTON + Hv_CANCELBUTTON + Hv_APPLYBUTTON);
    }
    
/* the dialog has been created */
    
    Hv_SetToggleButton(shownoise, reducenoise);

    fillcolor     = dcfillcolor;
    badtimecolor  = dcbadtimecolor;
    deadwirecolor = dcdeadwirecolor;
    framecolor    = dcframecolor;
    hitcolor      = dchitcolor;
    noisecolor    = dcnoisecolor;
    dcacolor      = dcdcacolor;
    trigsegcolor  = dctrigsegcolor;
    trigaddcolor  = dcaddedbytrigcolor;
    
    Hv_ChangeLabelColors(deadwirecolorlab, -1, deadwirecolor);
    Hv_ChangeLabelColors(badtimecolorlab,  -1, badtimecolor);
    Hv_ChangeLabelColors(fillcolorlab,     -1, fillcolor);
    Hv_ChangeLabelColors(framecolorlab,    -1, framecolor);
    Hv_ChangeLabelColors(hitcolorlab,      -1, hitcolor);
    Hv_ChangeLabelColors(noisecolorlab,    -1, noisecolor);
    Hv_ChangeLabelColors(trigsegcolorlab,  -1, trigsegcolor);
    Hv_ChangeLabelColors(trigaddcolorlab,  -1, trigaddcolor);
    Hv_ChangeLabelColors(dcacolorlab,      -1, dcacolor);
    
    for (rgn = 0; rgn < 3; rgn++)
	for (supl = 0; supl < 2; supl++) 
	    Hv_SetIntText(wmissinglayers[rgn][supl], missinglayers[rgn][supl]);

    for (i = 0; i < 6; i++)
	Hv_SetIntText(wlayershifts[i], layershifts[i]);

    for (i = 0; i < MAX_REGIONS; i++) {
	Hv_SetIntText(wdcnormalwire[i], dcnormalwire[i]);
	Hv_SetIntText(wdchitcircle[i],  dchitcircle[i]);
	Hv_SetIntText(wdchexagon[i],    dchexagon[i]);
	Hv_SetIntText(wdcmintime[i],    dcmintime[i]);
	Hv_SetIntText(wdcmaxtime[i],    dcmaxtime[i]);

    }
    
    
    while ((answer = Hv_DoDialog(dialog, NULL)) > 0) {

	reducenoise = Hv_GetToggleButton(shownoise);

	dcfillcolor        = fillcolor;
	dcbadtimecolor     = badtimecolor;
	dcdeadwirecolor    = deadwirecolor;
	dcframecolor       = framecolor;
	dchitcolor         = hitcolor;
	dcnoisecolor       = noisecolor;
	dcdcacolor         = dcacolor;
	dctrigsegcolor     = trigsegcolor;
	dcaddedbytrigcolor = trigaddcolor;

	for (rgn = 0; rgn < 3; rgn++)
	    for (supl = 0; supl < 2; supl++) 
		missinglayers[rgn][supl] = Hv_GetIntText(wmissinglayers[rgn][supl]);

	for (i = 0; i < 6; i++)
	    layershifts[i] = Hv_GetIntText(wlayershifts[i]);
	
	for (i = 0; i < MAX_REGIONS; i++) {
	    dcnormalwire[i] = Hv_GetIntText(wdcnormalwire[i]);
	    dchitcircle[i]  = Hv_GetIntText(wdchitcircle[i]);
	    dchexagon[i]    = Hv_GetIntText(wdchexagon[i]);
	    dcmintime[i]    = Hv_GetIntText(wdcmintime[i]);
	    dcmaxtime[i]    = Hv_GetIntText(wdcmaxtime[i]);
	}
	

/* redraw all views with visible dcs */

       for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
	   viewdata = GetViewData(temp);
	   if ((viewdata->DCItems[0] || viewdata->AllDCItems[0][0]) && viewdata->displayDC) {
	       Hv_SetViewDirty(temp);
	       Hv_DrawViewHotRect(temp); 
	   }
       }

	if (answer != Hv_APPLY)
	    break;
    }
    
}


/* ------ EditDCColor --- */

static void EditDCColor(Hv_Widget w)
    
{
    if (w == fillcolorlab)
	Hv_ModifyColor(w, &fillcolor, "Fill Color", False);
    else if (w == badtimecolorlab)
	Hv_ModifyColor(w, &badtimecolor, "Bad Time Color", False);
    else if (w == deadwirecolorlab)
	Hv_ModifyColor(w, &deadwirecolor, "Dead Wire Color", False);
    else if (w == framecolorlab)
	Hv_ModifyColor(w, &framecolor, "Frame Color", False);
    else if (w == hitcolorlab)
	Hv_ModifyColor(w, &hitcolor, "Hit Color", False);
    else if (w == noisecolorlab)
	Hv_ModifyColor(w, &noisecolor, "Noise Color", False);
    else if (w == dcacolorlab)
	Hv_ModifyColor(w, &dcacolor, "DCA Color", False);
    else if (w == trigsegcolorlab)
	Hv_ModifyColor(w, &trigsegcolor, "Trigger Segment Color", False);
    else if (w == trigaddcolorlab)
	Hv_ModifyColor(w, &trigaddcolor, "Trigger Added Color", False);
}


/* --------- SuperLayerFPolygon --------- */

static void SuperLayerFPolygon(Hv_View   View,
			       short     sect,
			       short     superlayer,
			       int       *np,
			       Hv_FPoint **fpts)
    
    
/*  
    sect and superlayer are C indices, both 0..5.
    superlayer will be converted to a rgn 0..2
    and a "reduced" superlayer 0..1
    */
    
    
{
    float             phi;
    short             rgn, supl;
    short             ni, nf, j;
    int               count;
    short             numlay;
    short             whatlay;
    DC_Super_Layer   *supptr;
    short             lastwire, firstwire;
    ViewData          viewdata = GetViewData(View);
    Hv_FPoint         fp_buffer[6];
    Hv_FPoint         *fp_max;
    size_t            size;
    
/* covert the grand superlayer to the archaic rgn and supl */
    
    rgn = superlayer / 2;
    supl = superlayer % 2;
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    numlay = DriftChambers[sect].regions[rgn].superlayers[supl].numlayers;
    
    phi = Hv_DEGTORAD*viewdata->dphi;
    
/* allocate a maximal buffer */
    
    fp_max = (Hv_FPoint *)Hv_Malloc(423*sizeof(Hv_FPoint));
    
/* start on the top layer, getting ALL THE VISIBLE WIRES along the top layer */
    
    whatlay = numlay-1;
    DCSuperLayerVisibleWireLimits(View, sect, rgn, supl, &ni, &nf);
    
    count = 0;
    
    for (j = ni; j <= nf; j++) {
	
/*  need kluge for region 1 axial, and we must now
    take pains to ensure that are wire numbers are
    within proper limits, thanks to Bogdans first
    wire offsets */
	
	if ((rgn == 0) && (supl == 1)) {
	    if (j <= supptr->layers[5].last_wire)
		whatlay = 5;
	    else if (j <= supptr->layers[3].last_wire)
		whatlay = 3;
	    else
		whatlay = 2;
	}
	
	lastwire  = supptr->layers[whatlay].last_wire;
	firstwire = supptr->layers[whatlay].first_wire;
	
	if (j == lastwire) {
	    FieldWirePosition(phi, sect, rgn, supl, whatlay, j, fp_buffer, POINT1+POINT5+POINT6);
	    CopyFPoint(fp_max+count, fp_buffer+4); count++;
	    CopyFPoint(fp_max+count, fp_buffer+5); count++;
	    CopyFPoint(fp_max+count, fp_buffer);   count++;
	    
/*further region 1 axial kluge */
	    
	    if ((whatlay == 5) && (rgn == 0) && (supl == 1)) {
		j = supptr->layers[4].last_wire;
		FieldWirePosition(phi, sect, rgn, supl, 4, j, fp_buffer, POINT1+POINT6);
		CopyFPoint(fp_max+count, fp_buffer+5); count++;
		CopyFPoint(fp_max+count, fp_buffer);   count++;
	    }
	    
	}
	else if ((j >= firstwire) && (j <= lastwire)) {
	    FieldWirePosition(phi, sect, rgn, supl, whatlay, j, fp_buffer, POINT5);
	    CopyFPoint(fp_max+count, fp_buffer+4); count++;
	}
    }  /* end j loop over wires */
    
/*  Now come DOWN the layers on the left getting the last visible
    wire. Need kluge for region 1 & 2 axial. Check for lastwire */
    
    if ((rgn == 0) && (supl == 1))
	whatlay = 2;
    else
	whatlay = numlay-1;
    
    for (j = whatlay; j >= 0; j--) {
	if (nf >= supptr->layers[j].first_wire) {
	    lastwire = supptr->layers[j].last_wire;
	    lastwire = Hv_sMin(nf, lastwire);


/* hack for change in # wires region 2 */

	    if ((rgn == 1) && (supl == 0) && (j == 3) && (lastwire == 190)) {
		FieldWirePosition(phi, sect, rgn, supl, j, lastwire, fp_buffer, POINT1+POINT2+POINT6);
		CopyFPoint(fp_max+count, fp_buffer+5); count++;
		CopyFPoint(fp_max+count, fp_buffer);   count++;
		CopyFPoint(fp_max+count, fp_buffer+1);   count++;
		FieldWirePosition(phi, sect, rgn, supl, j, lastwire-1, fp_buffer, POINT1+POINT2);
		CopyFPoint(fp_max+count, fp_buffer); count++;
		CopyFPoint(fp_max+count, fp_buffer+1);   count++;
	    }
	    else {
		FieldWirePosition(phi, sect, rgn, supl, j, lastwire, fp_buffer, POINT1+POINT6);
		CopyFPoint(fp_max+count, fp_buffer+5); count++;
		CopyFPoint(fp_max+count, fp_buffer);   count++;
	    }
	}
    }
    
/* now move along layer 0 */
    
    lastwire  = supptr->layers[0].last_wire;
    firstwire = supptr->layers[0].first_wire;
    lastwire  = Hv_sMin(nf, lastwire);
    firstwire = Hv_sMax(ni, firstwire);
    
    for (j = lastwire; j >= firstwire; j--) {
	FieldWirePosition(phi, sect, rgn, supl, 0, j, fp_buffer, POINT2);
	CopyFPoint(fp_max+count, fp_buffer+1); count++;
    }
    
/* now up the right side */
    
    for (j = 0; j < numlay; j++) {
	if (ni <= supptr->layers[j].last_wire) {
	    firstwire = supptr->layers[j].first_wire;
	    firstwire = Hv_sMax(ni, firstwire);
	    FieldWirePosition(phi, sect, rgn, supl, j, firstwire, fp_buffer, POINT3+POINT4);
	    CopyFPoint(fp_max+count, fp_buffer+2); count++;
	    CopyFPoint(fp_max+count, fp_buffer+3); count++;
	}
    }
    
/* done. Copy to output vars */
    
    *np = count;
    size = count*sizeof(Hv_FPoint);
    (*fpts) = (Hv_FPoint *)Hv_Malloc(size);
    memcpy((*fpts), fp_max, size);
    Hv_Free(fp_max);
    
}

/*------ CopyFPoint ------*/

static void CopyFPoint(Hv_FPoint *dfp,
                       Hv_FPoint *sfp)
    
{
    dfp->h = sfp->h;
    dfp->v = sfp->v;
}

/*-------- FixDCSuperLayerRegion ---------*/

static void FixDCSuperLayerRegion(Hv_Item Item)
    
{
    short                  sect;
    short                  superlayer;
    Hv_WorldPolygonData   *wpoly;
    int                    np;
    
    Hv_DestroyRegion(Item->region);
    
    wpoly = (Hv_WorldPolygonData *)(Item->data);
    
    sect       = (short)Item->user2;
    superlayer = (short)Item->user1;
    
    Hv_Free(wpoly->poly);
    Hv_Free(wpoly->fpts);
    
    SuperLayerFPolygon(Item->view, sect, superlayer, &np, &(wpoly->fpts));
    wpoly->numpnts = (short)np;
    wpoly->poly = Hv_NewPoints((int)wpoly->numpnts);
    Hv_WorldPolygonToLocalPolygon(Item->view, np, wpoly->poly, wpoly->fpts);
    Item->region = Hv_CreateRegionFromPolygon(wpoly->poly, wpoly->numpnts);
}


/* --------- DCLayerVisibleWireLimits ------------ */

static Boolean DCLayerVisibleWireLimits(Hv_View View,
					short   sect,
					short   rgn,
					short   suplay,
					short   lay,
					short   *ni,
					short   *nf)
    
/* for the given View, sect, rgn suplay and lay this returns the
   first and last wire visible in the View's hotrect.
   slop controls how many wires it jumps by.
   
   all indices are C indices, and the presence of rgn with
   suplay indicates that this suplay is of the "reduced"
   variety. */
    
{
    short       slop = 1;        /*slop = 1 means no slop!*/
    
    short       firstwire, lastwire;
    short       i;
    Boolean     drawit;
    Boolean     found;
    float       fz, fx;
    Hv_Point    pp;
    short       numlay;
    float       phi;
    ViewData    viewdata = GetViewData(View);
    Hv_FPoint   fpts[6];
    
/* intializations */
    
    
    firstwire = DriftChambers[sect].regions[rgn].superlayers[suplay].layers[lay].first_wire;
    lastwire  = DriftChambers[sect].regions[rgn].superlayers[suplay].layers[lay].last_wire;
    numlay = DriftChambers[sect].regions[rgn].superlayers[suplay].numlayers - 1;
    
    i = firstwire;
    *nf = -1;
    *ni = -1;
    
    drawit = False;
    phi = Hv_DEGTORAD*viewdata->dphi;
    
/* start at firstwire and jump forward until finding a visible wire
   for layer 0, use field wire P2 rather than the sense wire. For last layer,
   use field wire P5 rather than the sense wire.*/
    
    while((!drawit) && (i <= lastwire)) {
	if (lay == 0) {
	    FieldWirePosition(phi, sect, rgn, suplay, lay, i, fpts, POINT2);
	    Hv_WorldToLocal(View, fpts[1].h, fpts[1].v, &(pp.x), &(pp.y));
	}
	else if (lay == numlay) {
	    FieldWirePosition(phi, sect, rgn, suplay, lay, i, fpts, POINT5);
	    Hv_WorldToLocal(View, fpts[4].h, fpts[4].v, &(pp.x), &(pp.y));
	}
	else {
	    SenseWirePosition(phi, sect, rgn, suplay, lay, i, &fz, &fx);
	    Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	}
	
	drawit = Hv_PointInRect(pp, View->hotrect);
	if (!drawit)
	    i += slop;
    }
    
/* now go backwards */
    
    if (drawit) {
	*ni = i;
	found = False;
	i = lastwire;
	
	while ((!found) && (i > *ni)) {
	    if (lay == 0) {
		FieldWirePosition(phi, sect, rgn, suplay, lay, i, fpts, POINT2);
		Hv_WorldToLocal(View, fpts[1].h, fpts[1].v, &(pp.x), &(pp.y));
	    }
	    else if (lay == numlay) {
		FieldWirePosition(phi, sect, rgn, suplay, lay, i, fpts, POINT5);
		Hv_WorldToLocal(View, fpts[4].h, fpts[4].v, &(pp.x), &(pp.y));
	    }
	    else {
		SenseWirePosition(phi, sect, rgn, suplay, lay, i, &fz, &fx);
		Hv_WorldToLocal(View, fz, fx, &(pp.x), &(pp.y));
	    }
	    
	    found = Hv_PointInRect(pp, View->hotrect);
	    if (!found)
		i--;
	}
	*nf = i;
    }
    
    
    return drawit;
    
}


/* --------- DCSuperLayerVisibleWireLimits ------------ */

static Boolean DCSuperLayerVisibleWireLimits(Hv_View View,
					     short   sect,
					     short   rgn,
					     short   supl,
					     short   *ni,
					     short   *nf)
    
/* for the given View, sect, rgn and supl  this returns the
   first and last wire visible in the layouts hotrect. A small amount
   of slop is built in to ensure all wires visible are drawn.
   As soon as one layer is found to have visible wires, those
   limits are assumed to apply to all layers.
   
   all indices are C indices, and the presence of rgn with
   suplay indicates that this suplay is of the "reduced"
   variety. */
    
    
{
    Boolean drawit;
    short   lay;
    short   lastwire;      /* max last wire among visible sectors */
    short   numlay;
    
    short   templay, tempni, tempnf;
    Boolean tempdrawit;
    
    drawit = False;
    lay = 0;
    numlay = DriftChambers[sect].regions[rgn].superlayers[supl].numlayers;
    lastwire = 0;
    
/* first start at layer 0 and work up */
    
    while ((!drawit) && (lay < numlay)) {
	drawit = DCLayerVisibleWireLimits(View, sect, rgn, supl, lay, ni, nf);
	if (!drawit)
	    lay++;
	else {
	    lastwire = Hv_sMax(lastwire,
			       DriftChambers[sect].regions[rgn].superlayers[supl].layers[lay].last_wire);
	    
/* kluge for region 1 axial */
	    
	    if ((rgn == 0) && (supl == 1))
		lastwire++;
	}
	
    }
    
/* now start at layer numlay - 1 and work down */
    
    if (drawit) {
	tempni = *ni;
	tempnf = *nf;
	templay = numlay-1;
	tempdrawit = False;
	
	while ((!tempdrawit) && (templay > lay)) {
	    tempdrawit = DCLayerVisibleWireLimits(View, sect, rgn, supl, templay,
						  &tempni, &tempnf);
	    if (!tempdrawit)
		templay--;
	    else
		lastwire = Hv_sMax(lastwire,
				   DriftChambers[sect].regions[rgn].superlayers[supl].layers[templay].last_wire);
	}
	
	*ni = Hv_sMin(tempni, *ni);
	*nf = Hv_sMax(tempnf, *nf);
    }
    
/* add a little slop*/
    
    *ni = Hv_sMax(0, *ni - 3);
    *nf = Hv_sMin(lastwire, *nf + 3);
    
    return drawit;
}


/* --------- DrawDCWires ------------ */

static void DrawDCWires (Hv_View View,
			 short   sect,
			 short   rgn,
			 short   supl,
			 short   ni,
			 short   nf)
    
/* draws "normal" (as opposed to "hit") wires*/
    
    
{
    Hv_Point       *pnts;
    short           ntot;                 /* total number of wires */
    short           lw, nw, i,j, k, m;
    float           fz, fx;
    short           xt, yt;
    short           nsegments = 0;
    Hv_Segment      *segments = NULL;
    short           x1, x2, x3, x4, y1, y2, y3, y4, x5, y5, x6, y6;
    
    Hv_FPoint       fp_buffer[6];
    float           phi;
    ViewData        viewdata = GetViewData(View);
    Boolean         drawhexagons = False;
    Boolean         drawwires = False;
    int             pixdens;
    short           numlay;
    
    short           firstwire, lastwire;
    
    nw = nf - ni + 1;
    pixdens = PixDens(View);
    
/* draw hexagons? */
    
    drawhexagons = (viewdata->displayHex && (pixdens > dchexagon[rgn]));
    drawwires = drawhexagons || (pixdens > dcnormalwire[rgn]);
    
    if ((!drawhexagons) && (!drawwires))
	return;
    
    phi = Hv_DEGTORAD*viewdata->dphi;
    numlay = DriftChambers[sect].regions[rgn].superlayers[supl].numlayers;
    ntot = numlay * nw;           /* total number of wires in superlayer */
    
    if (drawhexagons) {
	nsegments = 20*nw;       /* approx number of segments */
	segments = (Hv_Segment *)Hv_Malloc(nsegments * sizeof(Hv_Segment));
    }
    
    pnts = Hv_NewPoints(ntot);
    
    k = 0;
    m = 0;
    
    for (i = 0; i < numlay; i++) {
	
	firstwire = DriftChambers[sect].regions[rgn].superlayers[supl].layers[i].first_wire;
	lastwire  = DriftChambers[sect].regions[rgn].superlayers[supl].layers[i].last_wire;
	
	for (j = ni; j <= nf; j++) 
	    if ((j >= firstwire) && (j <= lastwire)) {
		SenseWirePosition(phi, sect, rgn, supl, i, j, &fz, &fx);
		Hv_WorldToLocal(View, fz, fx, &xt, &yt);
		
/* store the wire position for later drawing */
		
		pnts[k].x = xt;
		pnts[k].y = yt;
		k++;
		
		if (drawhexagons) {
		    FieldWirePosition(phi, sect, rgn, supl, i, j, fp_buffer,
				      POINT1+POINT2+POINT3+POINT4);
		    Hv_WorldToLocal(View, fp_buffer[0].h, fp_buffer[0].v, &x1, &y1);
		    Hv_WorldToLocal(View, fp_buffer[1].h, fp_buffer[1].v, &x2, &y2);
		    Hv_WorldToLocal(View, fp_buffer[2].h, fp_buffer[2].v, &x3, &y3);
		    Hv_WorldToLocal(View, fp_buffer[3].h, fp_buffer[3].v, &x4, &y4);
		    Hv_SetSegment(segments+m, x1,y1, x2, y2); m++;
		    Hv_SetSegment(segments+m, x2,y2, x3, y3); m++;
		    Hv_SetSegment(segments+m, x3,y3, x4, y4); m++;
		    
		    if (i == (numlay-1)){
			FieldWirePosition(phi, sect, rgn, supl, i, j, fp_buffer, POINT5+POINT6);
			Hv_WorldToLocal(View, fp_buffer[4].h, fp_buffer[4].v, &x5, &y5);
			Hv_WorldToLocal(View, fp_buffer[5].h, fp_buffer[5].v, &x6, &y6);
			Hv_SetSegment(segments+m, x4, y4, x5, y5); m++;
			Hv_SetSegment(segments+m, x5, y5, x6, y6); m++;
		    }
		    
		    if (m > nsegments) {
			fprintf(stderr, "error in DrawDCWires\n");
			exit(0);
		    }
		    
		}
	    }
    }  /* end i (layer) loop */
    
    ntot = k;
    
/*    Hv_DrawPoints(pnts, ntot, Hv_black); */
    Hv_Free(pnts);
    
/* now draw the cells */
    
    if (drawhexagons) { 
	Hv_DrawSegments(segments, m, dcframecolor);
	
/* need kluge for region 1 axial */
	
	m = 0;
	if ((rgn == 0) && (supl == 1)) {
	    for (i = 4; i > 0; i--) {
		k = DriftChambers[sect].regions[rgn].superlayers[supl].layers[i+1].last_wire;
		lw = DriftChambers[sect].regions[rgn].superlayers[supl].layers[i].last_wire;
		lw = Hv_sMin(lw, nf);
		
		for (j = k; j <= lw; j++) {
		    FieldWirePosition(phi, sect, rgn, supl, i, j, fp_buffer,
				      POINT4+POINT5+POINT6);
		    Hv_WorldToLocal(View, fp_buffer[3].h, fp_buffer[3].v, &x4, &y4);
		    Hv_WorldToLocal(View, fp_buffer[4].h, fp_buffer[4].v, &x5, &y5);
		    Hv_WorldToLocal(View, fp_buffer[5].h, fp_buffer[5].v, &x6, &y6);
		    Hv_SetSegment(segments+m, x4, y4, x5, y5); m++;
		    Hv_SetSegment(segments+m, x5, y5, x6, y6); m++;
		    
		    if (m > nsegments) {
			fprintf(stderr, "error in DrawDCWires at location 2\n");
			exit(0);
		    }
		}
	    }
	    Hv_DrawSegments(segments, m, dcframecolor);
	}
	
	Hv_Free(segments);
    }
}

/*------- DCAfterOffset --------*/

static void DCAfterOffset(Hv_Item Item,
			  short   dh,
			  short   dv)
    
{
    if (Hv_offsetDueToScroll)
	Item->fixregion(Item);
    else
	Hv_PolygonAfterOffset(Item, dh, dv);
}


/* ----- WhatDCLayer -------- */

static Boolean WhatDCLayer(short sect,
			   short rgn,
			   short supl,
			   short *lay,
			   float r,
			   float theta)
    
    
{
    float    rlay, ddr;
    
    DC_Super_Layer   *supPtr;
    
    supPtr =   &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    
/* CORRECTION FOR REGION ONE */
    
    if ((rgn == 0) && (fabs(theta) > fabs(supPtr->layers[0].plane_angle)))
	r  = r*sin(fabs(theta));
    
    *lay = (short)((r - supPtr->Rinner)/(supPtr->dR));
    
/* because of the hexagon structure there is an ambiguity near the border which we
   avoid by simply saying that we are not in any layer if we are close to the border */
    
    ddr = supPtr->dR/6.0;
    rlay = supPtr->Rinner + (*lay)*(supPtr->dR);
    
    if ((r < (rlay + ddr)) || (r > (rlay + supPtr->dR - ddr)))
	*lay = -1;
    
    if (*lay >= supPtr->numlayers)
	*lay = -1;
    
    return (*lay >= 0);
}

/* ----- WhatWire -----------*/

static void WhatWire(Hv_View  View,
		     float    fz,
		     float    fx,
		     Hv_Point pp,
		     short    sect,
		     short    rgn,
		     short    sup,
		     short    *lay,
		     short    *wire)
    
/* given the View, sector, rgn, and sup we are in, and
   given fz and fx in target-centered Cartesian coordinates, this
   returns what region, superlayer, layer and wire we are near. A
   return of -1 means we are not "in" any such thing. */
    
{
    float            r, theta;
    float            dtheta;
    float            thetamin;
    short            lastwire;
    short            numlay;
    Boolean          NonzeroPhi;
    Boolean          StereoWire;
    Boolean          GoodGuess;
    short            Guess, StopGuess;
    Hv_Point         P;
    DC_Layer         *layerPtr;
    DC_Super_Layer   *supptr;
    float            phi;
    ViewData         viewdata = GetViewData(View);
    Hv_Point         xpts[6];
    Hv_FPoint        fpts[6];
    
/*assume failure */
    
    *lay  = -1;
    *wire = -1;
    phi = Hv_DEGTORAD*viewdata->dphi;
    
/* get r and theta in the system whose origin is at the Bogdan center */
    
    DCPolarCoordinates(phi, sect, rgn, sup, fz, fx, &r, &theta);
    
    if (WhatDCLayer(sect, rgn, sup, lay, r, theta)) {
	
/*now get the approximatewire number. THIS IS ONLY
  APPROXIMATE FOR THE CASE OF STEREO WIRES AND NONZERO PHI,
  OR FOR THE STRAIGHT PORTION OF REGION 1. OTHERWISE IT
  GIVES THE CORRECT ANSWER! (For Idealized Geometry) */
	
	supptr = &(DriftChambers[sect].regions[rgn].superlayers[sup]);
	layerPtr = &(supptr->layers[*lay]);
	StereoWire = (fabs(supptr->stereo_angle) > SMALLANG);
	NonzeroPhi = fabs(phi) > SMALLANG;
	
	lastwire = layerPtr->last_wire;
	numlay = supptr->numlayers;
	
	dtheta =   supptr->dtheta;
	thetamin = layerPtr->theta_min;
	
	*wire = layerPtr->first_wire + (short) ((theta - thetamin)/dtheta + 0.5);
	
/* for stereo wires and nonzero phi, we only have a "guess: for the wire*/
	
	if (((StereoWire) && (NonzeroPhi)) ||
	    ((rgn == 0) && (fabs(theta) > fabs(layerPtr->plane_angle)))) {
	    Guess = Hv_sMax(layerPtr->first_wire, *wire - 4);
	    StopGuess = Hv_sMin(layerPtr->last_wire,  *wire + 13);
	    GoodGuess = False;
	    Hv_SetPoint(&P, pp.x, pp.y);
	    
	    while ((!GoodGuess) && (Guess <= StopGuess)) {
		GetDCCellPolygon(View, sect, rgn, sup, *lay, Guess, xpts, fpts);
		GoodGuess = Hv_PointInPolygon(P, xpts, 6);
		
		if (!GoodGuess)
		    Guess++;
	    }
	    
	    if (GoodGuess)
		*wire = Guess;
	    else
		*wire = -1;
	}
	
	if ((*wire < layerPtr->first_wire) || (*wire > layerPtr->last_wire))
	    *wire = -1;
    }
    
}


/* ---- DCPolarCoordinates ----- */

static void  DCPolarCoordinates(float phi,
				short sect,
				short rgn,
				short sup,
				float fz,
				float fx,
				float *r,
				float *theta)
    
/* given fz and fx in target-centered Cartesian coordinates, this
   returns the polar coordinates for the system whose origin is at
   the offset of the given layer (i.e. at the Bogdan Center)*/
    
{
    float    zc, xc;
    
    zc =  DriftChambers[sect].regions[rgn].superlayers[sup].zc;
    xc =  DriftChambers[sect].regions[rgn].superlayers[sup].xc;
    
/* first undo the phi effect */
    
    fx = fx*cos(phi);
    
/* shift origins*/
    
    fz = fz - zc;
    fx = fx - xc;
    
    *r = sqrt(fz*fz + fx*fx);
    *theta = atan2(fx, fz);
}

/*------- GetFirstWireID ---------*/

short   GetFirstWireID(short sect,
		       short layer)
    
/* in this case sect = [1..6] (non c index)
   and layer = 1..36 (non c index) */
    
{
    short             rgn, supl, lay;
    DC_Super_Layer   *supptr;
    
    sect--;
    layer--;
    
    rgn = layer / 12;
    supl = (layer % 12) / 6;
    lay = layer % 6;
    
    supptr = &(DriftChambers[sect].regions[rgn].superlayers[supl]);
    return supptr->layers[lay].first_wire;
}


#undef    SMALLANG
