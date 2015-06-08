/*
----------------------------------------------------
-							
-  File:    geometry.c				
-							
-  Summary:						
-            reads the geometry database
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
#include "dc.h"

extern int  mlink_();
#ifdef WITH_OFFLINE
extern int  dc_make_dgeo_();
extern int  dc_geom_banks_();
#endif

/*------ local prototypes -------*/

static FILE  *GetGeometryFile(void);

static void  ReadCerenkovGeometry(FILE   *fp,
				  int   sect);

static void  ReadItalianEcalGeometry(FILE   *fp,
				     int   sect);

static void ReadNextCorners(FILE     *fp,
			    char     *line,
			    ijCorners   *tc);

static void  ReadShowerGeometry(FILE   *fp,
				int   sect);

static void ReadTaggerGeometry(FILE *fp);

static void  ReadDCGeometry(FILE  *fp,
			    int   sector);

static void  ReadScintGeometry(FILE *fp,
			       int sect);
static void GeoDump(FILE  *fp);

static void DCDump(FILE *fp);

static void ScintillatorDump(FILE *fp);

static void CerenkovDump(FILE *fp);

static void ForwardECDump(FILE *fp);

static void LargeAngleECDump(FILE *fp);

static void  FourPointMidPoint(Hv_FPoint   *mp,
			       Hv_FPoint   *pps);

static void ForwardCerenkovNominalLWD(int ID,
				      float *L,
				      float *W,
				      float *D);

static void SuperLayerWireLimits(void);

static int  ReadVersion(FILE  *fp);


/*here are record names locating places in the file*/

char *dcrecnames[6] = {
  "[SECTOR1_CHAMB]",
  "[SECTOR2_CHAMB]",
  "[SECTOR3_CHAMB]",
  "[SECTOR4_CHAMB]",
  "[SECTOR5_CHAMB]",
  "[SECTOR6_CHAMB]"
};
  
char *screcnames[6] = {
  "[SECTOR1_SCINT]",
  "[SECTOR2_SCINT]",
  "[SECTOR3_SCINT]",
  "[SECTOR4_SCINT]",
  "[SECTOR5_SCINT]",
  "[SECTOR6_SCINT]"
};

char *ccrecnames[6] = {
  "[SECTOR1_CERENK]",
  "[SECTOR2_CERENK]",
  "[SECTOR3_CERENK]",
  "[SECTOR4_CERENK]",
  "[SECTOR5_CERENK]",
  "[SECTOR6_CERENK]"
};

char *ec1recnames[6] = {
  "[SECTOR1_ITALIAN_ECAL]",
  "[SECTOR2_ITALIAN_ECAL]",
  "[SECTOR3_ITALIAN_ECAL]",
  "[SECTOR4_ITALIAN_ECAL]",
  "[SECTOR5_ITALIAN_ECAL]",
  "[SECTOR6_ITALIAN_ECAL]"
};

char *ecrecnames[6] = {
  "[SECTOR1_SHOWER]",
  "[SECTOR2_SHOWER]",
  "[SECTOR3_SHOWER]",
  "[SECTOR4_SHOWER]",
  "[SECTOR5_SHOWER]",
  "[SECTOR6_SHOWER]"
};

int   version = 1000;
char  *versionrecname = "VERSION_REC";
char  *tagrecname  = "[PHOTON_TAGGER]";
char  *endrecname = "[ENDREC]";

/* ------- global variables -------*/


DCPixelControl          DCpixels[MAX_REGIONS];    /* Drift Chamber pixel control*/
DC_Package              DriftChambers[6];         /*one package for each sector*/
Scint_Package           Scintillators[6];         /*one package for each sect*/
Shower_Package          ShowerCounters[6];        /*shower counter geometry*/
Cerenk_Package          Cerenkovs[6];             /*cerenkov counters */
LargeAngleEcal_Package  LargeAngleEcal[6];        /*italian calorimeters*/
Tagger_Package          Tagger;                   /*photon tagger*/
Hv_Point                *StartCounterPolys[NUM_SECT][NUM_STARTSEG];  /* work space for start counter drawing */
Hv_FPoint               *StartCounterFPolys[NUM_SECT][NUM_STARTSEG]; /* work space for start counter drawing */


/*--------- DCNumWires --------------*/

int DCNumWires(int sect,
	       int reg,
	       int supl,
	       int lay)

/*******************************
  sect:   [0..5]
  reg:    [0..2]
  supl:   [0..1]
  lay:    [0..5]
********************************/

{
  return (int)DriftChambers[sect].regions[reg].superlayers[supl].layers[lay].num_wires;
}


/*--------- DCLastWire --------------*/

int DCLastWire(int sect,
	       int reg,
	       int supl,
	       int lay)

/*******************************
  sect:   [0..5]
  reg:    [0..2]
  supl:   [0..1]
  lay:    [0..5]
********************************/

{
  return (int)DriftChambers[sect].regions[reg].superlayers[supl].layers[lay].last_wire;
}


/*------------ DCNumLayers -------------*/

int DCNumLayers(int sect,
		int reg,
		int supl)

/*******************************
  sect:   [0..5]
  reg:    [0..2]
  supl:   [0..1]
********************************/

{
  return (int)DriftChambers[sect].regions[reg].superlayers[supl].numlayers;
}

/*-------- SwitchSectorGeometry --------*/

void  SwitchSectorGeometry(View)

Hv_View    View;

{
  if (View->tag != SECTOR)
    return;

  SetScintGeometrySector(View);
  SetCerenkovGeometrySector(View);
  SetShowerGeometrySector(View);
  SetDCGeometrySector(View);
}


/*--------- ReInitGeometry ------*/

void     ReInitGeometry()

{
    int  force;
    int  i, j;

    int          k;
    DCGMDataPtr  dcgm;
    

    fprintf(stderr, "Reinitializing geometry bos banks for run number: %d\n", runnumber);


/* initialize the bos geometry banks */

    DCGeomBanks = True;
#ifdef WITH_OFFLINE
    dc_make_dgeo_(&runnumber, &force);
    dc_geom_banks_();
#else
    printf("CED compiled without 'WITH_OFFLINE' flag, problem may occur\n");
#endif
    for (i = 0; i < 6; i++) {
	j = i+1;
	bosIndexDCGM[i] = 0;
	bosIndexDCGW[i] = 0;
	bosDCGM[i] = NULL;
	bosDCGW[i] = NULL;

	bosIndexDCGM[i] = mlink_(wcs_.iw, bosBankNameDCGM, &j, 4);
	bosIndexDCGW[i] = mlink_(wcs_.iw, bosBankNameDCGW, &j, 4);

	if (bosIndexDCGM[i] != 0) {
	    bosDCGM[i]    = (DCGMDataPtr)(&(wcs_.iw[bosIndexDCGM[i]]));

	    dcgm = bosDCGM[i];

	    if (i == 0) { 
/*
		for (k = 0; k < 36; k++) {
		    fprintf(stderr, "(%f, %f %f) %f %f %f %f %f %d %d %f %d\n",
			    dcgm->x_curve,
			    dcgm->y_curve,
			    dcgm->z_curve,
			    dcgm->r_curve,
			    dcgm->theta_start,
			    dcgm->d_theta,
			    dcgm->theta_min,
			    dcgm->theta_max,
			    dcgm->min_wire,
			    dcgm->max_wire,
			    dcgm->stereo,
			    dcgm->max_cylw); 
		    dcgm++;
		}
*/
	    }
	}
	else {
	    fprintf(stderr, "Did NOT find DCGM bank for sector %d\n", i+1);
	    DCGeomBanks = False;
	}

	if (bosIndexDCGW[i] != 0) {
	    bosDCGW[i]    = (DCGWDataPtr)(&(wcs_.iw[bosIndexDCGW[i]]));
	}
	else {
	    fprintf(stderr, "Did NOT find DCGW bank for sector %d\n", i+1);
	    DCGeomBanks = False;
	}

    }

    if (DCGeomBanks)
	fprintf(stderr, "Using BOS geometry banks for Drift Chambers.\n");
    else
	fprintf(stderr, "Using ced geometry database for Drift Chambers.\n");

}

/*--------- InitGeometry ------*/

void     InitGeometry()

{
  FILE     *fp;
  int      sect;

  fp = GetGeometryFile();

  if (fp == NULL) {
    fprintf(stderr, "Cannot run without geometry. Exiting\n");
    exit(1);
  }

/* got the file, now try to read it, staring with version */

  version = ReadVersion(fp);
  
/*Now read the drift chamber data*/

  for (sect = 0; sect < 6; sect++)
    if (Hv_ReportDatabaseSearch(fp, dcrecnames[sect], True))
      ReadDCGeometry(fp, sect);

/* for drift chambers, call an auxillary routine
   that calculates the leftmost, topmost, rightmost,
   and bottommost wires. These will be useful for
   speeding up some drawing algorithms */

  SuperLayerWireLimits();
  
/*Scintillator Data*/

  for (sect = 0; sect < 6; sect++)
    if (Hv_ReportDatabaseSearch(fp, screcnames[sect], True))
      ReadScintGeometry(fp, sect);
  
/*Cerenkov Data*/

  for (sect = 0; sect < 6; sect++)
    if (Hv_ReportDatabaseSearch(fp, ccrecnames[sect], True))
      ReadCerenkovGeometry(fp, sect);

/* Large angle italian ecal data */

  for (sect = 0; sect < 6; sect++)
    if (Hv_ReportDatabaseSearch(fp, ec1recnames[sect], True))
      ReadItalianEcalGeometry(fp, sect);

/* Forward (small angle) Shower Counter Data*/

  for (sect = 0; sect < 6; sect++) {
    if (Hv_ReportDatabaseSearch(fp, ecrecnames[sect], True)) {
      ReadShowerGeometry(fp, sect);
    }
  }

/* Photon Tagger */

  Tagger.numTC  = 0;
  Tagger.numEC  = 0;
  Tagger.k_min  = NULL;
  Tagger.k_max1 = NULL;
  Tagger.k_max  = NULL;
  Tagger.e_lo1  = NULL;
  Tagger.e_hi1  = NULL;

  if (version > 2001) {
      if (Hv_ReportDatabaseSearch(fp, tagrecname, True))
	  ReadTaggerGeometry(fp);
  }
  else {
      fprintf(stderr, "The photon tagger geometry was NOT read in.\n");
  }

  /* Start counter space */

  for (sect = 0; sect < 6; sect++) {
    StartCounterPolys[sect][0] = Hv_NewPoints(3); 
    StartCounterPolys[sect][1] = Hv_NewPoints(4); 

    StartCounterFPolys[sect][0] = Hv_NewFPoints(3); 
    StartCounterFPolys[sect][1] = Hv_NewFPoints(4); 
  }

  
}

/*-------- BosGeometryDump -------*/

void BosGeometryDump()

/***********************************
  Dumps the geometry data in the form
  described in the geometry paper CLAS
  note 95-008. Dumps to two places:
  stdout and "bosgeo.dmp"
*************************************/

{
  FILE   *fp;
  
  GeoDump(stdout);
  
  fp = fopen("bosgeo.dmp", "w");
  GeoDump(fp);
}


/*-------- FixProjectedRegions ------*/

void     FixProjectedRegions(View)

Hv_View      View;

/************************************
  Fix the regions of the items that
  project onto the midplane.
*************************************/

{
  Hv_Item          temp;

  for (temp = View->items->first; temp != NULL; temp = temp->next) 
    if (temp->domain == Hv_INSIDEHOTRECT)
      if ((temp->fixregion == FixWPolygonBasedRegion) || (temp->tag == DCITEM))
	temp->fixregion(temp);

}

/*---------- FixWPolygonBasedRegion --------*/

void     FixWPolygonBasedRegion(Item)

Hv_Item     Item;

/****************************************
  Should be a good fixregion algorithm for
  any world polygon based items.
****************************************/

{
  Hv_View               View = Item->view;
  Hv_WorldPolygonData  *wpoly;
  Hv_FPoint            *fpts;
  int                   i;
  short                 np;

  Hv_DestroyRegion(Item->region);
  Item->region = NULL;
  
  wpoly = (Hv_WorldPolygonData *)(Item->data);

  np = wpoly->numpnts;
  fpts = (Hv_FPoint *)Hv_Malloc(np*sizeof(Hv_FPoint));

  if (wpoly->poly != NULL)
    Hv_Free(wpoly->poly);
    
  if (wpoly->fpts != NULL) {

    for (i = 0; i < np; i++) {
      fpts[i].h = wpoly->fpts[i].h;
      fpts[i].v = MidplaneXtoProjectedX(View, wpoly->fpts[i].v);
    }

    wpoly->poly = Hv_NewPoints((int)np);
    Hv_WorldPolygonToLocalPolygon(View, np, wpoly->poly, fpts);
    Item->region = Hv_CreateRegionFromPolygon(wpoly->poly, np);
    Hv_ClipBox(Item->region, Item->area);
    Hv_LocalRectToWorldRect(View, Item->area, Item->worldarea);
  }
}


/*-------- SetItemSector ----------*/

short SetItemSector(Item)

Hv_Item     Item;

/***************************************
  Set the item's sector in the user2
  field.
***************************************/

{
  Hv_View              View = Item->view;
  int                  vs, sect;

  if (View->tag != SECTOR)
    return 0;
  
  vs = GoodSector(View);

/* the user2 field holds the sector. Make sure it is still consistent, since
   via the option buttons  the sectors may have changed. Note also that the
   items for the "upper" sectors [0,1,2] are easy to determine. */

  if (Item->user2 < 3)   /* upper sector */
    sect = vs;
  else
    sect = vs + 3;
  
  Item->user2 = (short)sect;
  return (short)sect;
}


/*------ GoodSector ---------*/

short GoodSector(Hv_View View) {

    short sect;

    ViewData viewdata = GetViewData(View);

/* the absolute val will handle the "Most Data" sects */

    sect = abs(viewdata->visiblesectors) - 1;

/* now check for the "EC Sum" case */

    if (sect > 99)
	sect -= 100;

    return sect;  /* returns a C Index*/
}


/*------ GetGeometryFile -------*/

static FILE   *GetGeometryFile(void)

{
  FILE     *fp;
  char     *fname0 = "clas.geometry";
  char     *fname;

  fp = Hv_LookForNamedFile(fname0, Hv_ioPaths, "r");

  if (!fp) {
    if (Hv_Question("Can't find the geometry file. Do you want to look for it?")) {
      fname = Hv_FileSelect("Please locate the ced geometry file.", "*.geo*", NULL);
      fp = fopen(fname, "r");
      Hv_Free(fname);
    }
  }

  return  fp;
}

/*------ ReadCerenkovGeometry ------- */

static void   ReadCerenkovGeometry(FILE *fp,
				   int   sect)

/*Reads the  geometry for specified sector
  from fp into the global variable Cerenkovs*/

{
  int     i;          /*looper*/
  char    *line;      /*input line*/
  int     num;
  float   z1, x1, z2, x2, z3, x3, z4, x4;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/
  
  for (i=0;  i < NUM_CERENK;  i++) {
    Hv_GetNextNonComment(fp, line);

    sscanf(line, "%d %f %f %f %f %f %f %f %f", &num,
	   &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

    Cerenkovs[sect].cerenks[i].P[0].h = z1;
    Cerenkovs[sect].cerenks[i].P[0].v = x1;
    Cerenkovs[sect].cerenks[i].P[1].h = z2;
    Cerenkovs[sect].cerenks[i].P[1].v = x2;
    Cerenkovs[sect].cerenks[i].P[2].h = z3;
    Cerenkovs[sect].cerenks[i].P[2].v = x3;
    Cerenkovs[sect].cerenks[i].P[3].h = z4;
    Cerenkovs[sect].cerenks[i].P[3].v = x4;
  }
  free(line);
}


/*------ ReadItalianEcalGeometry ------- */

static void   ReadItalianEcalGeometry(FILE *fp,
				      int   sect)

/*Reads the  geometry for specified sector
  from fp into the global variable LargeAngleEcal*/

{
  char    *line;      /*input line*/
  float   z1, x1, z2, x2, z3, x3, z4, x4, z5, x5, z6, x6;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/

  LargeAngleEcal[sect].activeA = (sect < 2);   /*default: not on*/
  LargeAngleEcal[sect].activeB = False;   /*default: not on*/

  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
	 &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4, &z5, &x5, &z6, &x6);

    LargeAngleEcal[sect].PA[0].h = z1;
    LargeAngleEcal[sect].PA[0].v = x1;
    LargeAngleEcal[sect].PA[1].h = z2;
    LargeAngleEcal[sect].PA[1].v = x2;
    LargeAngleEcal[sect].PA[2].h = z3;
    LargeAngleEcal[sect].PA[2].v = x3;
    LargeAngleEcal[sect].PA[3].h = z4;
    LargeAngleEcal[sect].PA[3].v = x4;
    LargeAngleEcal[sect].PA[4].h = z5;
    LargeAngleEcal[sect].PA[4].v = x5;
    LargeAngleEcal[sect].PA[5].h = z6;
    LargeAngleEcal[sect].PA[5].v = x6;
  
  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
	 &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4, &z5, &x5, &z6, &x6);

    LargeAngleEcal[sect].PB[0].h = z1;
    LargeAngleEcal[sect].PB[0].v = x1;
    LargeAngleEcal[sect].PB[1].h = z2;
    LargeAngleEcal[sect].PB[1].v = x2;
    LargeAngleEcal[sect].PB[2].h = z3;
    LargeAngleEcal[sect].PB[2].v = x3;
    LargeAngleEcal[sect].PB[3].h = z4;
    LargeAngleEcal[sect].PB[3].v = x4;
    LargeAngleEcal[sect].PB[4].h = z5;
    LargeAngleEcal[sect].PB[4].v = x5;
    LargeAngleEcal[sect].PB[5].h = z6;
    LargeAngleEcal[sect].PB[5].v = x6;

  free(line);
}

/*------ ReadNextCorners --------- */

static void ReadNextCorners(FILE        *fp,
			    char        *line,
			    ijCorners   *tc)
{
  int        num;
  
  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%d %f %f %f %f %f %f %f %f", &num, 
	 &(tc->corners[0].i), &(tc->corners[0].j),
	 &(tc->corners[1].i), &(tc->corners[1].j),
	 &(tc->corners[2].i), &(tc->corners[2].j),
	 &(tc->corners[3].i), &(tc->corners[3].j));

}

/*------ ReadShowerGeometry ------- */

static void  ReadShowerGeometry(FILE   *fp,
				int   sect)

/*Reads the  shower-counter geometry for specified sector
  from fp into the global variable ShowerCounters*/

{
  int             i,j;          /*loopers*/
  char            *line;        /*input line*/
  int             num;
  float           length, width, thickness, rho;
  float           z1, x1, z2, x2, z3, x3, z4, x4;
  float           xp1, yp1, zp1, xp2, yp2, xp3, yp3;
  Shower_Package  *sp;
  line = (char *) Hv_Malloc(Hv_LINESIZE);

  sp = &(ShowerCounters[sect]);         /* makes code cleaner */

/*keep in mind that "C" forces us to start indices at zero*/


/*first, read in the midpoint and corners of the midplane projections */  
  for (i=0;  i < NUM_SHOWER;  i++) {

    Hv_GetNextNonComment(fp, line);
    
    sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f %f", &num,
	   &length, &width, &thickness, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);
    

    sp->showers[i].length = length;
    sp->showers[i].width =  width;
    sp->showers[i].thickness = thickness;
    sp->showers[i].P[0].h = z1;
    sp->showers[i].P[0].v = x1;
    sp->showers[i].P[1].h = z2;
    sp->showers[i].P[1].v = x2;
    sp->showers[i].P[2].h = z3;
    sp->showers[i].P[2].v = x3;
    sp->showers[i].P[3].h = z4;
    sp->showers[i].P[3].v = x4;
  }
  
/* now onto the numbers used in the front-rear view*/


  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%f %f %f %f %f %f %f", &xp1, &yp1,&xp2, &yp2,  &xp3, &yp3, &z3);
      
  sp->cFront[0].i = xp1;
  sp->cFront[0].j = yp1;
  sp->cFront[1].i = xp2;
  sp->cFront[1].j = yp2;
  sp->cFront[2].i = xp3;
  sp->cFront[2].j = yp3;

  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%f %f %f %f %f %f %f", &xp1, &yp1,&xp2, &yp2,  &xp3, &yp3, &z3);
      
  sp->cRear[0].i = xp1;
  sp->cRear[0].j = yp1;
  sp->cRear[1].i = xp2;
  sp->cRear[1].j = yp2;
  sp->cRear[2].i = xp3;
  sp->cRear[2].j = yp3;

/* now read the perpendicular vector */

  Hv_GetNextNonComment(fp, line);
  sscanf(line, "%f %f %f", &xp1, &yp1, &zp1);
  sp->R0.x = xp1;
  sp->R0.y = yp1;
  sp->R0.z = zp1;

/* calculate the angles between R0 and the z axis */

  rho = Cart3DLength(&(sp->R0));
  sp->CosT = sp->R0.z/rho;
  sp->SinT = sqrt(1.0 - (sp->CosT*sp->CosT));
  if (sect > 2)
    sp->SinT = -sp->SinT;

/* now read the separation between the two planes */

  Hv_GetNextNonComment(fp, line);
  sscanf(line, "%f", &xp1);
  sp->DeltaK = xp1;


/* now read in the corners of the strips */

  for (j = 0; j < NUM_STRIP; j++)
    ReadNextCorners(fp, line, &(sp->Ustrips[j]));

  for (j = 0; j < NUM_STRIP; j++)
    ReadNextCorners(fp, line, &(sp->Vstrips[j]));

  for (j = 0; j < NUM_STRIP; j++)
    ReadNextCorners(fp, line, &(sp->Wstrips[j]));
      
  free(line);
}


/*------ ReadTaggerGeometry ---- */

static void ReadTaggerGeometry(FILE *fp)

{
  int   i;          /*looper*/
  char  *line;      /*input line*/
  int   ind;
  float  k_max, k_max1, k_min;
  int    e_lo1, e_hi1;

  line = (char *) Hv_Malloc(Hv_LINESIZE);
       
  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%d", &(Tagger.numTC));

  Hv_GetNextNonComment(fp, line);

  sscanf(line, "%d", &(Tagger.numEC));

/*  fprintf(stderr, "there are %d tcs and %d ecs\n", Tagger.numTC, Tagger.numEC);*/

/* allocate the arrays */

  Tagger.k_min  = (float *)Hv_Malloc(Tagger.numTC*sizeof(float));
  Tagger.k_max1 = (float *)Hv_Malloc(Tagger.numTC*sizeof(float));
  Tagger.k_max  = (float *)Hv_Malloc(Tagger.numTC*sizeof(float));
  Tagger.e_lo1 = (int *)Hv_Malloc(Tagger.numTC*sizeof(int));
  Tagger.e_hi1 = (int *)Hv_Malloc(Tagger.numTC*sizeof(int));

  for (i = 0; i < Tagger.numTC; i++) {
      Hv_GetNextNonComment(fp, line);
      sscanf(line, "%d %f %f %f %d %d", &ind, &k_min, &k_max1, &k_max, &e_lo1, &e_hi1);
      ind--;

      if ((ind >= 0) && (ind < Tagger.numTC)) {
	  Tagger.k_min[ind]  = k_min;
	  Tagger.k_max1[ind] = k_max1;
	  Tagger.k_max[ind]  = k_max;
	  Tagger.e_lo1[ind]  = e_lo1;
	  Tagger.e_hi1[ind]  = e_hi1;
      }
      else
	  fprintf(stderr, "Found an index out of range while reading tagger data.\n");

  }


  free (line);
}


/*------ ReadDCGeometry ------- */

static void  ReadDCGeometry(FILE  *fp,
			    int   sector)

/*Reads the drift chamber geometry for specified sector
  from fp into the global variable DriftChambers*/

{
  char  *line;         /*input line*/

  int       numlay, wire90, suplayer,  layer, region, nw, dummy;
  float     radius, zc, xc, thetamin, thetamax, sterang, planeang;
  short     i;
  float     dlen, dtheta, dtheta2, dr;
  float     piover2, cos30;
  int       layeroffset;
  int       sectorsign;
  int       firstwire = 1;

  DC_Super_Layer  *supptr;
  DC_Layer        *layerptr;
  DC_Region       *regptr;

  piover2 = 2.0*atan(1.0);
  cos30 = sqrt(3.0)/2.0;
  if (sector > 2)
    sectorsign = -1;
  else
    sectorsign = 1;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/

/* i is looping over superlayers */

  for (i = 0;  i < 6;  i++) {
    region = i / 2;      /* while i = 0,1,2,3,4,5; region = 0,0,1,1,2,2 */
    suplayer = i % 2;    /* while i = 0,1,2,3,4,5; suplayer = 0,1,0,1,0,1 */

    regptr = &(DriftChambers[sector].regions[region]);
    supptr = &(regptr->superlayers[suplayer]);

/* read some information applicable to entire super layer.
   The radius, thetamin, theta max and nw apply to layer 3*/
    
    Hv_GetNextNonComment(fp, line);

    sscanf(line, "%d %f %f %f %f %f %f %d",
	   &numlay, &zc, &xc, &radius, &thetamin, &thetamax, &sterang, &wire90);

    supptr->numlayers = (short)numlay;

/* 3/1094  MODIFICATION: denominators changed from nw-1 to nw+1 to
   reflect the fact that they refer to GUARD WIRES not the first sense wire.
   This also caused the addition/subtraction of dtheta from thetamin/thetamax
   so that for the rest of the program they mean the same as before: the
   thetas for the 1ts and last SENSE wire */ 

    if (region == 0)
      dtheta = sectorsign*(piover2 - fabs(thetamin))/wire90;
    else
      dtheta = (thetamax - thetamin)/(wire90 + 1);
    
    dtheta2 = dtheta/2.0;

    thetamin += dtheta;   /*shift from GUARD to 1st  SENSE */
    thetamax -= dtheta;   /*shift from GUARD to Last SENSE */

    dlen = fabs(radius*dtheta);  /*for layer 3 only*/
    dr = cos30*dlen;        /*same for all layers*/

/* allocate space for the layers */

    supptr->layers = (DC_Layer *)Hv_Malloc((supptr->numlayers)*sizeof(DC_Layer));

    supptr->zc     = zc;
    supptr->xc     = xc;
    supptr->stereo_angle = sterang;
    supptr->dtheta = dtheta;

/* now get remaining entries for each layer */
    

    for (layer = 0; layer < supptr->numlayers; layer++) {
      layerptr = (DC_Layer *)(&(supptr->layers[layer]));

      layerptr->radius = radius + (layer-2)*dr;
      
      if ((layer % 2) == 0) {
	layerptr->theta_min = thetamin;
	layerptr->theta_max = thetamax;
      }
      else {
	layerptr->theta_min = thetamin + dtheta2;
	layerptr->theta_max = thetamax + dtheta2;
      }

      Hv_GetNextNonComment(fp, line);

/* some version dependence here */

      if (version > 1500)
	sscanf(line, "%d %d %d", &dummy, &nw, &firstwire);
      else 
	sscanf(line, "%d %d", &dummy, &nw);

/* more useful to think of firstwire as a deviation from
   its usual value of 1, hence the -1  THUS AFTER THE NEXT
   TWO LINES first_wire and last_wire ARE C INDICES*/

      layerptr->first_wire = (short)(firstwire - 1);      
      layerptr->last_wire = layerptr->first_wire + nw - 1;

      planeang = sectorsign*Hv_PI;


      if (region == 0) {
	planeang = sectorsign*piover2;
	if ((layer % 2) != 0)
	  planeang += dtheta2;

/*layeroffset is used to get the trinagular piece at front of region 1*/

	layeroffset = 1 - ((layer+1)/2);
	layerptr->theta_min += layeroffset*dtheta;
	layerptr->theta_max += layeroffset*dtheta;

	layerptr->lastcircwire = layerptr->first_wire + 
	  (short)((planeang - layerptr->theta_min)/supptr->dtheta);

      }

/* calculate dtheta, which depends on whether there is a plane (straight) portion
   of the dc. This should only be true in region 1 */

      if (layer == 2)  {    /*Based on SDA params, which are based on 3rd sense layer*/
	supptr->dlen = fabs(radius*supptr->dtheta);
	supptr->dR = (supptr->dlen)*cos30;
      }

      layerptr->plane_angle  = planeang;
      layerptr->num_wires    = nw;

/*fprintf(stderr, "(%d) %d %d \t%d\n", layer,
	layerptr->first_wire,
	layerptr->last_wire,
	layerptr->num_wires);
*/
    }  /* end of layer loop */
    
    supptr->Rinner = supptr->layers[0].radius - (supptr->dR/2.0);
    supptr->Router = supptr->layers[supptr->numlayers-1].radius + (supptr->dR/2.0);
    
    if (suplayer == 0)
      regptr->Rinner = supptr->Rinner;
    
    if (suplayer == 1)
      regptr->Router = supptr->Router;
  }

  free(line);

}

/*------ ReadScintGeometry ------- */

static void  ReadScintGeometry(FILE *fp,
			       int sect)

/*Reads the scintillator geometry for specified sect
  from fp into the global variable Scintillatorss*/

{
  int     i;          /*looper*/
  char    *line;      /*input line*/
  int     num;
  float   length, width;
  float   z1, x1, z2, x2, z3, x3, z4, x4;

  line = (char *) Hv_Malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/
  
  for (i=0;  i < NUM_SCINT;  i++) {
      Hv_GetNextNonComment(fp, line);

      sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f", &num,
	     &length, &width, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

      Scintillators[sect].scints[i].length = length;
      Scintillators[sect].scints[i].width =  width;
      Scintillators[sect].scints[i].P[0].h = z1;
      Scintillators[sect].scints[i].P[0].v = x1;
      Scintillators[sect].scints[i].P[1].h = z2;
      Scintillators[sect].scints[i].P[1].v = x2;
      Scintillators[sect].scints[i].P[2].h = z3;
      Scintillators[sect].scints[i].P[2].v = x3;
      Scintillators[sect].scints[i].P[3].h = z4;
      Scintillators[sect].scints[i].P[3].v = x4;
    }

  free(line);
}


/*-------- GeoDump -------*/

static void GeoDump(FILE *fp)

{
  if (fp == NULL)
    return;

  DCDump(fp);
  ScintillatorDump(fp);
  CerenkovDump(fp);
  ForwardECDump(fp);
  LargeAngleECDump(fp);
}

/*---- DCDump ------*/

static void DCDump(FILE *fp)

{
  int   sect;

  fprintf(fp, "\n!***** [DRIFT CHAMBERS] *****\n");

  for (sect = 0; sect < 6; sect++) {
    fprintf(fp, "%s\n", dcrecnames[sect]);
    fprintf(fp, "%s\n", endrecname);
  }

}

/*---- ScintillatorDump ------*/

static void ScintillatorDump(FILE *fp)

{
  int            sect;
  Scint_Package  sp;
  int            i;
  Scint          scint;
  Hv_FPoint      *pp;
  Hv_FPoint      mp;

  float          xl, yl, zl;
  float          xh, yh, zh;
  float          dx, dz, t, t1, t2;
  float          l2;

  fprintf(fp, "\n!***** [SCINTILLATORS] *****\n");

  for (sect = 0; sect < 6; sect++) {
    fprintf(fp, "%s\n", screcnames[sect]);
    fprintf(fp, "!ID IP    XL       YL       ZL       XH       YH       ZH      W   T\n");

/* always use sect 0 package for transformation to work right */

    sp = Scintillators[0];
/*    sp = Scintillators[sect]; */

    for (i = 0; i < NUM_SCINT; i++) {
      scint = sp.scints[i];
      pp = scint.P;
      FourPointMidPoint(&mp, pp);

      l2 = scint.length/2.0;

/* compute the "average" thickness */

      dx = pp[3].v - pp[0].v;
      dz = pp[3].h - pp[0].h;
      t1 = sqrt(dx*dx + dz*dz);

      dx = pp[2].v - pp[1].v;
      dz = pp[2].h - pp[1].h;
      t2 = sqrt(dx*dx + dz*dz);

      t = 0.5*(t1+t2);

/* get the local (ced) coordinates */

      xl = mp.v;
      yl = -l2;
      zl = mp.h;

      xh = mp.v;
      yh = l2;
      zh = mp.h;

/* get the clas coordinates */

      CEDxyToCLASxy(xl, yl, &xl, &yl, sect);
      CEDxyToCLASxy(xh, yh, &xh, &yh, sect);

      fprintf(fp,
	      " %-3d%-3d%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f%-5.1f%-5.2f\n",
	      i+1, PanelFromID(i+1), xl, yl, zl, xh, yh, zh, scint.width, t);
    }
    fprintf(fp, "%s\n", endrecname);
  }
  
}

/*---- CerenkovDump ------*/

static void CerenkovDump(FILE *fp)

{
  int                sect;
  Cerenk_Package     cc;
  int                id, cedid;
  Hv_FPoint          *pp;
  Hv_FPoint          mp;
  float              L, W, D;   /* length width depth cm */
  float              xl, yl, zl;
  float              xh, yh, zh;

  fprintf(fp, "\n!***** [CERENKOV] *****\n");

  for (sect = 0; sect < 6; sect++) {
    fprintf(fp, "%s\n", ccrecnames[sect]);
    fprintf(fp, "!ID   XL       YL       ZL       XH       YH       ZH      Win     D\n");

/* user the first sector so that transformation work */

    cc = Cerenkovs[0];

/*    cc = Cerenkovs[sect]; */

    for (cedid = 0; cedid < NUM_CERENK; cedid++) {

      pp = cc.cerenks[cedid].P;
      FourPointMidPoint(&mp, pp);

/* according to CLAS note, print only the low phi (odd id) */

      id = 2*cedid + 1; /*  [1, .. 35] */

/* get the length (sort of a  half length) */

      ForwardCerenkovNominalLWD(id, &L, &W, &D);

/* get the local (ced) coordinates */

      xl = mp.v;
      yl = -L;
      zl = mp.h;

      xh = mp.v;
      yh = L;
      zh = mp.h;

/* get the clas coordinates */

      CEDxyToCLASxy(xl, yl, &xl, &yl, sect);
      CEDxyToCLASxy(xh, yh, &xh, &yh, sect);

      fprintf(fp, " %-3d%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f%-9.2f\n",
	      id, xl, yl, zl, xh, yh, zh, W, D);

    }

    fprintf(fp, "%s\n", endrecname);
  }

}

/*---- ForwardECDump ------*/

static void ForwardECDump(FILE *fp)

{
  int   sect;

  fprintf(fp, "\n!***** [Forward EC] *****\n");

  for (sect = 0; sect < 6; sect++) {
    fprintf(fp, "%s\n", ecrecnames[sect]);
    fprintf(fp, "%s\n", endrecname);
  }

}

/*---- LargeAngleECDump ------*/

static void LargeAngleECDump(FILE *fp)

{
  int   sect;

  fprintf(fp, "\n!***** [LARGE ANGLE EC] *****\n");

  for (sect = 0; sect < 6; sect++) {
    fprintf(fp, "%s\n", ec1recnames[sect]);
    fprintf(fp, "%s\n", endrecname);
  }

}

#define TAN30    0.577350
#define PHIGAP   10.0      /*cm*/

/*-------- ForwardCerenkovNominalLWD ------*/

static void ForwardCerenkovNominalLWD(int ID,
				      float *L,
				      float *W,
				      float *D)

/* given the NON C ONDEX ID, this returns the nominal
   length, inner width ant thickness of each slab.
   It uses the single parameter
   GAP which is the distance from the phi = +- 30 degree
   planes */

{
  int            cedid;  /* the 0..17 ced index */
  Hv_FPoint     *pp;
  Hv_FPoint      mp;
  Cerenk_Package cc;
  float          dv, dh, t1, t2;
  
  cedid = (ID-1)/2;
  cc = Cerenkovs[0];

  pp = cc.cerenks[cedid].P;
  FourPointMidPoint(&mp, pp);

  *L = mp.v*TAN30 - PHIGAP;
  dh = pp[1].h - pp[0].h;
  dv = pp[1].v - pp[0].v;

  *W = sqrt(dv*dv + dh*dh);

/* compute the "average" thickness */
  
  dv = pp[3].v - pp[0].v;
  dh = pp[3].h - pp[0].h;
  t1 = sqrt(dv*dv + dh*dh);
  
  dv = pp[2].v - pp[1].v;
  dh = pp[2].h - pp[1].h;
  t2 = sqrt(dv*dv + dh*dh);
  
  *D = 0.5*(t1+t2);
}

#undef TAN30
#undef PHIGAP

/*-------- FourPointMidPoint --------*/

static void  FourPointMidPoint(Hv_FPoint   *mp,
			       Hv_FPoint   *pps)

/* pps is a four point array */

{
  mp->h = (pps[0].h + pps[1].h + pps[2].h + pps[3].h)/4.0;
  mp->v = (pps[0].v + pps[1].v + pps[2].v + pps[3].v)/4.0;
}


/* ---- SuperLayerWireLimits ----------- */

static void SuperLayerWireLimits(void)

/*This routine fills in the wires numbers that are the leftmost, rightmost,
  topmost and bottom most for a given sect, rgn, and suplay. This is independent
  of the View and ALSO INDEPENDENT OF LAYER.

  This rotuine should be called only once, when the DC database is read during
  initialization. It finds the wire numbers of the wires with the greatest extent
  in four directions. This is used later to generate an enclosing rectangle.

  NOTE: do not confuse this with DCSuperLayerVisibleWireLimits
  (found in dc.c) , which, for a given View, returns the limit of VISIBLE wires.
*/

{
  short          sect;
  short          rgn;
  short          suplay;

  short          L;
  short          T;
  short          R;
  short          B;
  short          wire;
  short          firstwire, lastwire;
  float          fz, fx;
  float          left, top, right, bottom;
  DC_Super_Layer *supPtr;

  for (sect = 0; sect < 6; sect++)
    for (rgn = 0; rgn < 3; rgn++)
      for (suplay = 0; suplay < 2; suplay++) {
	L = 0;
	T = 0;
	R = 0;
	B = 0;

	supPtr = &(DriftChambers[sect].regions[rgn].superlayers[suplay]);

	SenseWirePosition(0.0, sect, rgn, suplay, 0, supPtr->layers[0].first_wire, &fz, &fx);
	left = fz;
	right = fz;
	top = fx;
	bottom = fx;

/* for all sectors, layer two (C index 1) 
   has the most wires */

	lastwire = supPtr->layers[1].last_wire;
	firstwire = supPtr->layers[1].first_wire;
	
	for (wire = firstwire+1; wire <= lastwire; wire++) {
	  SenseWirePosition(0.0, sect, rgn, suplay, 0, wire, &fz, &fx);

	  if (fz < left) {
	    L = wire;
	    left = fz;
	  }
	  
	  if (fz > right) {
	    R = wire;
	    right = fz;
	  }
	  
	  if (fx > top) {
	    T = wire;
	    top = fx;
	  }
	  
	  if (fx < bottom) {
	    B = wire;
	    bottom = fx;
	  }

	}
	
	supPtr->LimitLeftWire = L;
	supPtr->LimitTopWire = T;
	supPtr->LimitRightWire = R;
	supPtr->LimitBottomWire = B;
	
      }
}



/*---------- ReadVersion --------*/

static int   ReadVersion(FILE  *fp )

{
  char *line;
  int  vers = 1000;
  
  rewind(fp);
  line = (char *) Hv_Malloc(Hv_LINESIZE);

  while (Hv_FindRecord(fp, versionrecname, False, line)) {
    Hv_GetNextNonComment(fp, line);
    sscanf(line, "%d", &vers);
  } /* while find rec */
  
  Hv_Free(line);
  
  fprintf(stderr,  "geometry file version: %-7.3f\n",  ((float)vers)/1000.0);
  return vers;
}



