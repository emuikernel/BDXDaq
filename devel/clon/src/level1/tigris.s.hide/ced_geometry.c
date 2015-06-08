
#ifndef VXWORKS

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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"


Scint_Package Scintillators[6]; /* global */


/* local variables */

/*DCPixelControl DCpixels[MAX_REGIONS];*/    /* Drift Chamber pixel control*/
static DC_Package DriftChambers[6];
static Shower_Package ShowerCounters[6]; /* shower counter geometry */
static Cerenk_Package Cerenkovs[6]; /* cerenkov counters */
static LargeAngleEcal_Package LargeAngleEcal[6]; /* italian calorimeters */
static short E_per_T_Count[NUM_T_COUNTERS]; /* num e counters for each t counter in photon tagger*/

/* local prototypes */

static FILE *GetGeometryFile();
static void  ReadScintGeometry();
static void  ReadDCGeometry();
static void  ReadTaggerGeometry();
static void  ReadShowerGeometry();
static void  ReadNextCorners();
static void  ReadItalianEcalGeometry();
static void  ReadCerenkovGeometry();
static float Cart3DLength(Cart3D *);



/*--------- InitGeometry ------*/

void
InitGeometry()
{
  FILE *fp;

/*here are record names locating places in the file*/

  char  *REC0NAME_CHAMB = "[SECTOR1_CHAMB]";
  char  *REC1NAME_CHAMB = "[SECTOR2_CHAMB]";
  char  *REC2NAME_CHAMB = "[SECTOR3_CHAMB]";
  char  *REC3NAME_CHAMB = "[SECTOR4_CHAMB]";
  char  *REC4NAME_CHAMB = "[SECTOR5_CHAMB]";
  char  *REC5NAME_CHAMB = "[SECTOR6_CHAMB]";
  
  char  *REC0NAME_SCINT = "[SECTOR1_SCINT]";
  char  *REC1NAME_SCINT = "[SECTOR2_SCINT]";
  char  *REC2NAME_SCINT = "[SECTOR3_SCINT]";
  char  *REC3NAME_SCINT = "[SECTOR4_SCINT]";
  char  *REC4NAME_SCINT = "[SECTOR5_SCINT]";
  char  *REC5NAME_SCINT = "[SECTOR6_SCINT]";

  char  *REC0NAME_CERENK = "[SECTOR1_CERENK]";
  char  *REC1NAME_CERENK = "[SECTOR2_CERENK]";
  char  *REC2NAME_CERENK = "[SECTOR3_CERENK]";
  char  *REC3NAME_CERENK = "[SECTOR4_CERENK]";
  char  *REC4NAME_CERENK = "[SECTOR5_CERENK]";
  char  *REC5NAME_CERENK = "[SECTOR6_CERENK]";

  char  *REC0NAME_ITALIAN = "[SECTOR1_ITALIAN_ECAL]";
  char  *REC1NAME_ITALIAN = "[SECTOR2_ITALIAN_ECAL]";
  char  *REC2NAME_ITALIAN = "[SECTOR3_ITALIAN_ECAL]";
  char  *REC3NAME_ITALIAN = "[SECTOR4_ITALIAN_ECAL]";
  char  *REC4NAME_ITALIAN = "[SECTOR5_ITALIAN_ECAL]";
  char  *REC5NAME_ITALIAN = "[SECTOR6_ITALIAN_ECAL]";

  char  *REC0NAME_SHOWER = "[SECTOR1_SHOWER]";
  char  *REC1NAME_SHOWER = "[SECTOR2_SHOWER]";
  char  *REC2NAME_SHOWER = "[SECTOR3_SHOWER]";
  char  *REC3NAME_SHOWER = "[SECTOR4_SHOWER]";
  char  *REC4NAME_SHOWER = "[SECTOR5_SHOWER]";
  char  *REC5NAME_SHOWER = "[SECTOR6_SHOWER]";

  char  *RECNAME_TAGGER  = "[PHOTON_TAGGER]";

  fp = GetGeometryFile();

  if (fp == NULL) {
    fprintf(stderr, "Cannot run without geometry. Exiting\n");
    exit(1);
  }

/* got the file, now try to read it */

  
/*First read the drift chamber data*/

  if (Hv_ReportDatabaseSearch(fp, REC0NAME_CHAMB, True))
    ReadDCGeometry(fp, 0);

  if (Hv_ReportDatabaseSearch(fp, REC1NAME_CHAMB, True))
    ReadDCGeometry(fp, 1);

  if (Hv_ReportDatabaseSearch(fp, REC2NAME_CHAMB, True))
    ReadDCGeometry(fp, 2);

  if (Hv_ReportDatabaseSearch(fp, REC3NAME_CHAMB, True))
    ReadDCGeometry(fp, 3);
    
  if (Hv_ReportDatabaseSearch(fp, REC4NAME_CHAMB, True))
    ReadDCGeometry(fp, 4);
    
  if (Hv_ReportDatabaseSearch(fp, REC5NAME_CHAMB, True))
    ReadDCGeometry(fp, 5);
  
/*Scintillator Data*/

  if (Hv_ReportDatabaseSearch(fp, REC0NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 0);
  
  if (Hv_ReportDatabaseSearch(fp, REC1NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 1);
  
  if (Hv_ReportDatabaseSearch(fp, REC2NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 2);
  
  if (Hv_ReportDatabaseSearch(fp, REC3NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 3);
  
  if (Hv_ReportDatabaseSearch(fp, REC4NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 4);
  
  if (Hv_ReportDatabaseSearch(fp, REC5NAME_SCINT, True) != 0)
    ReadScintGeometry(fp, 5);
  
/*Cerenkov Data*/

  if (Hv_ReportDatabaseSearch(fp, REC0NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 0);
  
  if (Hv_ReportDatabaseSearch(fp, REC1NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 1);
  
  if (Hv_ReportDatabaseSearch(fp, REC2NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 2);
  
  if (Hv_ReportDatabaseSearch(fp, REC3NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 3);
  
  if (Hv_ReportDatabaseSearch(fp, REC4NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 4);
  
  if (Hv_ReportDatabaseSearch(fp, REC5NAME_CERENK, True))
    ReadCerenkovGeometry(fp, 5);

/* Large angle italian ecal data */

  
  if (Hv_ReportDatabaseSearch(fp, REC0NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 0);
  
  if (Hv_ReportDatabaseSearch(fp, REC1NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 1);
  
  if (Hv_ReportDatabaseSearch(fp, REC2NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 2);
  
  if (Hv_ReportDatabaseSearch(fp, REC3NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 3);
  
  if (Hv_ReportDatabaseSearch(fp, REC4NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 4);
  
  if (Hv_ReportDatabaseSearch(fp, REC5NAME_ITALIAN, True))
    ReadItalianEcalGeometry(fp, 5);


/*Shower Counter Data*/

  if (Hv_ReportDatabaseSearch(fp, REC0NAME_SHOWER, True))
    ReadShowerGeometry(fp, 0);
  
  if (Hv_ReportDatabaseSearch(fp, REC1NAME_SHOWER, True))
    ReadShowerGeometry(fp, 1);
  
  if (Hv_ReportDatabaseSearch(fp, REC2NAME_SHOWER, True))
    ReadShowerGeometry(fp, 2);
  
  if (Hv_ReportDatabaseSearch(fp, REC3NAME_SHOWER, True))
    ReadShowerGeometry(fp, 3);
  
  if (Hv_ReportDatabaseSearch(fp, REC4NAME_SHOWER, True))
    ReadShowerGeometry(fp, 4);
  
  if (Hv_ReportDatabaseSearch(fp, REC5NAME_SHOWER, True))
    ReadShowerGeometry(fp, 5);

/* Photon Tagger */

  if (Hv_ReportDatabaseSearch(fp, RECNAME_TAGGER, True))
    ReadTaggerGeometry(fp);


}

/*------ GetGeometryFile -------*/

static FILE *
GetGeometryFile()
{
  FILE     *fp;
  char     *fname0 = "ascii.geo";
  Boolean  veryfirst = True;

  if (veryfirst) {
    veryfirst = False;
    Hv_AddPath(".");
    Hv_AddPath("/clas/clas02/usr/users/heddle/BER");
  }

  fp = Hv_LookForNamedFile(fname0, Hv_ioPaths, "r");

  if (fp == NULL)
    fprintf(stderr, "WARNING: Could not open ASCII geometry file.\n");

  return  fp;
}



/*------ ReadCerenkovGeometry ------- */

static void
ReadCerenkovGeometry(fp, sect)

/*Reads the  geometry for specified sector
  from fp into the global variable Cerenkovs*/

FILE     *fp;
int      sect;

{
  int     i;          /*looper*/
  char    *line;      /*input line*/
  int     num;
  float   z1, x1, z2, x2, z3, x3, z4, x4;

  line = (char *) malloc(Hv_LINESIZE);

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

static void
ReadItalianEcalGeometry(fp, sect)

/*Reads the  geometry for specified sector
  from fp into the global variable LargeAngleEcal*/

FILE     *fp;
int      sect;

{
  char    *line;      /*input line*/
  float   z1, x1, z2, x2, z3, x3, z4, x4, z5, x5, z6, x6;

  line = (char *) malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/

  LargeAngleEcal[sect].activeA = False;   /*default: not on*/
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

static void
ReadNextCorners(fp, line, tc)

FILE          *fp;
char          *line;
ijCorners     *tc;

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

static void
ReadShowerGeometry(fp, sect)

/*Reads the  shower-counter geometry for specified sector
  from fp into the global variable ShowerCounters*/

FILE     *fp;
int      sect;

{
  int             i,j;          /*loopers*/
  char            *line;        /*input line*/
  int             num;
  float           length, width, thickness, rho;
  float           z1, x1, z2, x2, z3, x3, z4, x4;
  float           xp1, yp1, zp1, xp2, yp2, xp3, yp3;
  Shower_Package  *sp;
  line = (char *) malloc(Hv_LINESIZE);

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

static void
ReadTaggerGeometry(fp)

FILE *fp;

{
  int   i;          /*looper*/
  char  *line;      /*input line*/

  line = (char *) malloc(Hv_LINESIZE);

/*keep in mind that "C" forces us to start indices at zero*/
  
       
  Hv_GetNextNonComment(fp, line);

  E_per_T_Count[0] = ((short)(atoi(strtok (line, " "))));

  for (i=1; i<NUM_T_COUNTERS; i++)
    {
      E_per_T_Count[i] = ((short)(atoi(strtok (NULL, " "))));
    }

  free (line);
}


/*------ ReadDCGeometry ------- */

static void
ReadDCGeometry(fp, sector)

/*Reads the drift chamber geometry for specified sector
  from fp into the global variable DriftChambers*/

FILE    *fp;
int     sector;

{
  char  *line;         /*input line*/

  int       numlay, wire90, suplayer,  layer, region, nw, dummy;
  float     radius, zc, xc, thetamin, thetamax, sterang, planeang;
  short     i;
  float     dlen, dtheta, dtheta2, dr;
  float     piover2, cos30;
  int       layeroffset;
  int       sectorsign;

  DC_Super_Layer  *supptr;
  DC_Layer        *layerptr;
  DC_Region       *regptr;

  piover2 = 2.0*atan(1.0);
  cos30 = sqrt(3.0)/2.0;
  if (sector > 2)
    sectorsign = -1;
  else
    sectorsign = 1;

  line = (char *) malloc(Hv_LINESIZE);

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

    supptr->layers = (DC_Layer *)malloc((supptr->numlayers)*sizeof(DC_Layer));

    supptr->zc     = zc;
    supptr->xc     = xc;
    supptr->stereo_angle = sterang;
    supptr->dtheta = dtheta;

/* now get remaining entries for each layer */

/*fprintf(stderr, "-----------\nsect = %d  reg = %d  suplay = %d  dtheta = %f  dlen = %f  dr = %f\n",
	sector+1, region+1, suplayer+1, dtheta, dlen, dr);*/


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
      sscanf(line, "%d %d", &dummy, &nw);
      planeang = sectorsign*Hv_PI;


      if (region == 0) {
	planeang = sectorsign*piover2;
	if ((layer % 2) != 0)
	  planeang += dtheta2;

/*layeroffset is used to get the trinagular piece at front of region 1*/

	layeroffset = 1 - ((layer+1)/2);
/*fprintf(stderr, "layer: %d  layeroffset: %d\n", layer+1, layeroffset);*/
	layerptr->theta_min += layeroffset*dtheta;
	layerptr->theta_max += layeroffset*dtheta;

	layerptr->lastcircwire = (short)((planeang - layerptr->theta_min)/supptr->dtheta);
      }

/* calculate dtheta, which depends on whether there is a plane (straight) portion
   of the dc. This should only be true in region 1 */

      if (layer == 2)  {    /*Based on SDA params, which are based on 3rd sense layer*/
	supptr->dlen = fabs(radius*supptr->dtheta);
	supptr->dR = (supptr->dlen)*cos30;
      }

      layerptr->plane_angle  = planeang;
      layerptr->num_wires    = nw;
/*
fprintf(stderr, "%2d  %f  %5.2f  %5.2f  %f  %f  %4d  %f  %f\n",
	layer+1, layerptr->radius, layerptr->zc, layerptr->xc, layerptr->theta_min-dtheta,
	layerptr->theta_max+dtheta, layerptr->num_wires, layerptr->stereo_angle, layerptr->plane_angle);
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

static void
ReadScintGeometry(fp, sect)

/*Reads the scintillator geometry for specified sect
  from fp into the global variable Scintillatorss*/

FILE *fp;
int  sect;

{
  int     i;          /*looper*/
  char    *line;      /*input line*/
  int     num;
  float   length, width;
  float   z1, x1, z2, x2, z3, x3, z4, x4;

  line = (char *) malloc(Hv_LINESIZE);

  /* keep in mind that "C" forces us to start indices at zero */
  for (i=0; i<23; i++) {
     Hv_GetNextNonComment(fp, line);
     sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f", &num,
        &length, &width, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

     Scintillators[sect].scints[i].length = length;
     Scintillators[sect].scints[i].width =  width;
     Scintillators[sect].scints[i].P[0].h = z1;
     Scintillators[sect].scints[i].P[0].v = x1;
     Scintillators[sect].scints[i].P[1].h = z2;
     Scintillators[sect].scints[i].P[1].v = x2;
     Scintillators[sect].scints[i].P[2].h = z3+50;
     Scintillators[sect].scints[i].P[2].v = x3+25;
     Scintillators[sect].scints[i].P[3].h = z4+50;
     Scintillators[sect].scints[i].P[3].v = x4+25;
  }
  for (i=23; i<34; i++) {
     Hv_GetNextNonComment(fp, line);
     sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f", &num,
        &length, &width, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

     Scintillators[sect].scints[i].length = length;
     Scintillators[sect].scints[i].width =  width;
     Scintillators[sect].scints[i].P[0].h = z1-10;
     Scintillators[sect].scints[i].P[0].v = x1-50;
     Scintillators[sect].scints[i].P[1].h = z2-10;
     Scintillators[sect].scints[i].P[1].v = x2-50;
     Scintillators[sect].scints[i].P[2].h = z3+5;
     Scintillators[sect].scints[i].P[2].v = x3+5;
     Scintillators[sect].scints[i].P[3].h = z4+5;
     Scintillators[sect].scints[i].P[3].v = x4+5;
  }
  for (i=34; i<42; i++) {
     Hv_GetNextNonComment(fp, line);
     sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f", &num,
        &length, &width, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

     Scintillators[sect].scints[i].length = length;
     Scintillators[sect].scints[i].width =  width;
     Scintillators[sect].scints[i].P[0].h = z1;
     Scintillators[sect].scints[i].P[0].v = x1;
     Scintillators[sect].scints[i].P[1].h = z2;
     Scintillators[sect].scints[i].P[1].v = x2;
     Scintillators[sect].scints[i].P[2].h = z3-15;
     Scintillators[sect].scints[i].P[2].v = x3+53;
     Scintillators[sect].scints[i].P[3].h = z4-15;
     Scintillators[sect].scints[i].P[3].v = x4+53;
  }
  for (i=42; i<48; i++) {
     Hv_GetNextNonComment(fp, line);
     sscanf(line, "%d %f %f %f %f %f %f %f %f %f %f", &num,
        &length, &width, &z1, &x1, &z2, &x2, &z3, &x3, &z4, &x4);

     Scintillators[sect].scints[i].length = length;
     Scintillators[sect].scints[i].width =  width;
     Scintillators[sect].scints[i].P[0].h = z1-20;
     Scintillators[sect].scints[i].P[0].v = x1+50;
     Scintillators[sect].scints[i].P[1].h = z2-20;
     Scintillators[sect].scints[i].P[1].v = x2+50;
     Scintillators[sect].scints[i].P[2].h = z3-49;
     Scintillators[sect].scints[i].P[2].v = x3+97;
     Scintillators[sect].scints[i].P[3].h = z4-49;
     Scintillators[sect].scints[i].P[3].v = x4+97;
  }

  free(line);
}

 
/* ------ Cart3DLength ------- */

static float
Cart3DLength(R)

/* usual vector magnitude function */

Cart3D   *R;
     
{
  return(sqrt(R->x*R->x + R->y*R->y + R->z*R->z));
}

#else /* UNIX only */

void
ced_geometry_dummy()
{
}

#endif
