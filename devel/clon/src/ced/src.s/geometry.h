/*
---------------------------------------------------------------------------
-
-   	File:     geometry.h
-
-	Header file for the Geometry data Structures
-       for the CLAS Single Event Display Package
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
*/


#ifndef __CEDGEOH
#define __CEDGEOH

#define     POINT1        01             /* bit indicating point 1 of the wire hexagon */
#define     POINT2        02             /* bit indicating point 2 of the wire hexagon */
#define     POINT3        04             /* bit indicating point 3 of the wire hexagon */
#define     POINT4        010            /* bit indicating point 4 of the wire hexagon */
#define     POINT5        020            /* bit indicating point 5 of the wire hexagon */
#define     POINT6        040            /* bit indicating point 6 of the wire hexagon */

/* EMC related  */

#define  INNER_EMC      0
#define  OUTER_EMC      1
#define  BOTH_EMC       2


#define  U_PLANE        0
#define  V_PLANE        1
#define  W_PLANE        2

#define  X_PLANE        0
#define  Y_PLANE        1


/*------------ TAGGER DATA STRUCTURES -----------*/

typedef struct tagger {
    int     numTC;      /*number of t counters */
    int     numEC;      /*number of e counters */

/* arrays will be allocated to be the size of the numb of t counters */

    float  *k_min;      /*k/Eo min that hits that t counter when it's 1st one hit*/
    float  *k_max1;     /*k/Eo max that hits that counter when it's 1st one hit*/
    float  *k_max;      /*max k/Eo for which electron hits the counter */
    int    *e_lo1;      /*lowest e counter when that tc is the 1st one hit*/
    int    *e_hi1;      /*highest e counter when that tc is the 1st one hit*/
} Tagger_Package;

/* --------  CERENKOV DATA STRUCTURES ------------*/

typedef struct cerenk
{
  Hv_FPoint     P[4];                   /*midplane corners of cerenkov*/
} Cerenk;

typedef struct cerenk_package
{
 Cerenk       cerenks[NUM_CERENK];
} Cerenk_Package;

/*--------------- EC DATA STRUCTURES -----------*/


typedef struct ijpoint
{
  float i;    /* i coordinate (in front plane shower counter i,j coords */
  float j;    /* j coordinate (in front plane shower counter i,j coords */
} ijPoint;

typedef struct ijcorners
{
  ijPoint    corners[4];       /* used for corners of shower counter strips */
} ijCorners;


/*----- large angle (italian) calorimeters ----*/

typedef struct largeangleecal_package
{
  Boolean    activeA;  /*means to turn on and off 1st italian ecal*/
  Boolean    activeB;  /*means to turn on and off 2nd italian ecal*/
  Hv_FPoint  PA[6];    /*polygon vertices in midplane for "first"*/
  Hv_FPoint  PB[6];    /*polygon vertices in midplane for "second"*/
} LargeAngleEcal_Package;

typedef struct shower
{

/* the first set of numbers are for the midplane views */

  float      length;                 /*length of shower counter*/
  float      width;                  /*width of shower counter*/
  float      thickness;              /*thickness of shower counter*/
  Hv_FPoint     P[4];                   /*corners of shower counter*/
} Shower;

typedef struct shower_package
{
  Shower       showers[NUM_SHOWER];    /*expect NUM_SHOWER = 2 for front & rear shower counters*/

/* these numbers are for the "shower-fanned" view */

/*rear plane will use same numbers as front plane --- these are never used in a geometrically
  "faithful" way */

  ijCorners  Ustrips[NUM_STRIP];    /*four corners of U strips for both planes*/
  ijCorners  Vstrips[NUM_STRIP];    /*four corners of V strips for both planes*/
  ijCorners  Wstrips[NUM_STRIP];    /*four corners of W strips for both planes*/

  ijPoint    cFront[3];   /* ij points for front plane triangle corners */
  ijPoint    cRear[3];    /* ij points for rear plane triangle corners */
  Cart3D     R0;          /* perpendicular vect from targ to inner plane (origin of ij) */
  float      CosT, SinT;  /* cosine and sine of angle between R0 and z axis (calculated) */
  float      DeltaK;       /* offset along the vector R0 between the front and rear planes */

} Shower_Package;


/*------------ SCINTILLATOR DATA STRUCTURES ----------*/

typedef struct scint
{
  float         length;          /*length of scintillator*/
  float         width;          /*width of scintillator*/
  Hv_FPoint     P[4];          /*corners of scintillator*/
} Scint;

typedef struct scint_package
{
 Scint       scints[NUM_SCINT];
} Scint_Package;

/*--------  DRIFT CHAMBER DATA STRUCTURES ------------*/


/* this first structure just collects everything
   needed to specify a drift chamber wire --
   so that a pointer to one of these can be
   passed rather than a list of the fields */

typedef  struct   dcwirespec
{
  short   sector;      /*sector [0..5] */
  short   region;      /*region [0..2] */
  short   superlayer;  /*superlayer [0 or 1]*/
  short   layer;       /*layer [0..5]*/
  short   wire;        /*wire [0..] */
} DCWireSpec;


typedef struct dc_layer
{
  float     radius;          /*radius from offset center in meters*/
  float     theta_min;       /*minimum theta (radians - midplane)*/
  float     theta_max;       /*maximum theta (radians - midplane)*/
  short     lastcircwire;    /*Cindex of last wire in lay 3 circ region (applic only for region 1*/
  float     plane_angle;     /*angle at which plane part (as in reg 1) starts (make = Pi in no plane part)*/
  short     num_wires;       /*number of wires in this layer*/
  short     first_wire;      /* CINDEX of first wire -- always 0 except in reg 1 */
  short     last_wire;       /* CINDEX id of last wire -- always = num_wires-1 except reg 1 */
} DC_Layer;

typedef struct dc_super_layer
{
  short     numlayers;       /*how many layers there are*/
  float     dlen;            /*distance between wires (const for plane part of reg 1)*/
  float     dR;              /*radial separation between layers */
  float     Rinner, Router;  /*radial limits of the superlayer*/
  float     zc;              /*horizontal center (origin at target)*/
  float     xc;              /*vertical center   (origin at target)*/
  float     dtheta;          /*angular separation (radians) for circular part wrt bogdan cent*/
  float     stereo_angle;    /*stereo angle of wire (radians) i.e. zero for axial wires*/
  DC_Layer  *layers;         /*data for each layer*/


/* limit wires used for graphics speedup */

  short           LimitLeftWire;
  short           LimitTopWire;
  short           LimitRightWire;
  short           LimitBottomWire;

} DC_Super_Layer;

typedef struct dc_region
{
  float           Rinner, Router;    /*radial limits of the region*/
  DC_Super_Layer  superlayers[2];    /*each region has two super layers*/
} DC_Region;

typedef struct dc_package
{
  DC_Region      regions[3];        /*a dc package is comprised of three regions*/
} DC_Package;

typedef struct dcpixelcontrol     /*sets limits for various aspects of DC drawing*/
{
  int NormalWireCutoff;          /*pixel/meter where normal (non-hit) wires are turned on*/
  int HitWireLargeCutoff;        /*pixel/meter density when hits switch from small to big wires*/
  int HexagonCutoff;             /*pixel/meter density when hexagons turn on*/
  int HitCircleCutoff;           /*pixel/meter density when hit circles turn on*/
} DCPixelControl;

/* ------- global variables -------*/


extern DCPixelControl  DCpixels[MAX_REGIONS];    /* Drift Chamber pixel control*/
extern Boolean         DCGeomBanks; /* will -> True if uses DC geo banks */

/*Detector Packages*/

extern DC_Package              DriftChambers[NUM_SECT];      /*one package for each sector*/
extern Scint_Package           Scintillators[NUM_SECT];      /*one package for each sect*/
extern Shower_Package          ShowerCounters[NUM_SECT];     /*shower counter geometry*/
extern Cerenk_Package          Cerenkovs[NUM_SECT];          /*cerenkov counters */
extern LargeAngleEcal_Package  LargeAngleEcal[NUM_SECT];     /*italian calorimeters*/

extern Hv_Point                *StartCounterPolys[NUM_SECT][NUM_STARTSEG];  /* work space for start counter drawing */
extern Hv_FPoint               *StartCounterFPolys[NUM_SECT][NUM_STARTSEG]; /* work space for start counter drawing */

extern Tagger_Package          Tagger;

/*------- function prototypes -----------*/

extern int DCNumWires(int sect,
		      int reg,
		      int supl,
		      int lay);

extern int DCLastWire(int sect,
		      int reg,
		      int supl,
		      int lay);

extern int DCNumLayers(int sect,
		       int reg,
		       int supl);

extern void SwitchSectorGeometry(Hv_View  View);

extern void   InitGeometry(void);

extern void   ReInitGeometry(void);

extern void BosGeometryDump(void);

extern void   FixProjectedRegions(Hv_View   View);

extern void   FixWPolygonBasedRegion(Hv_Item   Item);

extern short SetItemSector(Hv_Item   Item);

extern short GoodSector(Hv_View View);

#endif

