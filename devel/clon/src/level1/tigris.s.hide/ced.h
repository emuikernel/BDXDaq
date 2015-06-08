/*
---------------------------------------------------------------------------
-
-   	File:     ced.h
-
-	main header file for ced version 2.0
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  10/19/94	DPH		Initial Version					
-  06/26/95     JAM             add structures for tigris view.

-------------------------------------------------------------------------------
*/


#ifndef __CEDH
#define __CEDH

/* Item Tags for ced specific items*/

#define     BEAMLINE         1
#define     FIELDMAP         2
#define     SCINTITEM        3
#define     SECTORHEXITEM    4
#define     SECTORSHOWERITEM 5
#define     FANNEDSHOWERITEM 6

/* feedback tags */

#define     ANCHORDISTANCE   1
#define     ZPOSITION        2
#define     XPOSITION        3
#define     YPOSITION        4
#define     DISTANCE         5
#define     ANGLE            6
#define     PHI              7
#define     WHATSECTOR       8
#define     EVENT            9
#define     BMAG            10
#define     BVECT           11
#define     WHATSCINT       12
#define     WHATSUPERLAYER  13
#define     WHATLAYER       14
#define     WHATWIRE        15
#define     OCCUPANCY       16
#define     WHATCERENK      17
#define     WHATSHOWER      18
#define     WHATUVW         19
#define     TDCL            20
#define     ADCL            21
#define     TDCR            22
#define     ADCR            23
#define     PIXENERGY       24
#define     PIXID           25
#define     UTDC            26
#define     UADC            27
#define     VTDC            28
#define     VADC            29
#define     WTDC            30
#define     WADC            31

/* View Types */

#define     SECTOR       1
#define     FORWARDEC    2
#define     BACKWARDEC   3
#define     ALLDC        4
#define     SCINT        5
#define     TAGGER       6
#define     COSMIC       7

/* visible sectors for sector views */

#define     SECTORS14       1
#define     SECTORS25       2
#define     SECTORS36       3
#define     MOSTDATASECTOR  4

/* shower planes */

#define     ECINNER        1
#define     ECOUTER        2
#define     ECBOTH         3

/* strip directions */

#define     USTRIP         0
#define     VSTRIP         1
#define     WSTRIP         2

/* defined types */

/* ---------------------
 *  A scintillator (Hv_Item) is made up of the following:
 *
 *
 *  2 - which group (1-6 SD's for 1-6 SE's) (1-6 SD's for 7-12 SE's)
 *  3 - which part of a scint paddle (1, 2, 3 = SD1, SD2, SD3, ...; 
 *      one must infer from the paddle number (1-48) if you are on SD1-3 or
 *      SD4-6.) 
 *  48- there are 48 scint paddles. Notice that the back 32 are currently wired
 *      together to cut costs (this is important when I do file I/O later)
 */

typedef struct ScintData {
  char bits [2][3][48] ;
  char state[2][3][48];
} ScintData ;

typedef struct SpareData {
  char bits[2][16] ;               /* not used yet */
} SpareData ;

typedef struct ExtraData {
  char bits[2][3][48] ;            /* not used yet */
} ExtraData ;

/* 
 *  12 scint events per sector.
 *  32 products (term OR_OP term) per scint event 
 *  16 bits per scint event
 */
typedef struct ScintEvent
{
  char products[12][32][16];
} ScintEvent ;

typedef struct VisSectorDef
{
  ScintData sd ;   
  SpareData sp ;
  ExtraData ed ;
  ScintEvent se;
} VisSectorDef ;

typedef struct TriggerInfo
{
  Hv_Item   SItem1;
  Hv_Item   SItem2;
  Hv_Item   RItem;
  Hv_Item   Feedback;
  Hv_Item   tgboundbox1;
  Hv_Item   tgboundbox2;
  Hv_Item   titem[4]; /* 4 text areas on the right side of Trigger View */
  int       can_not_do[8][2][6];
  char      operator[4];
  char      *trigString[2][4];
  int       rstate;
  int       which6;
  int       butSet1;
  int       butSet2;
  Hv_Item   bAnd1, bOr1, bNot1, bClr1;
  Hv_Item   bAnd2, bOr2, bNot2, bClr2;
  Hv_Item   sb[36];
  Hv_Item   bTopTrig1;
  Hv_Item   bBotTrig1;
  Hv_Item   bTopTrig2;
  Hv_Item   bBotTrig2;

} TriggerInfo ;

typedef struct BitBoxStruct {  /* used by bit boxes to hold neat data */
  int nhordivs;
  int nvertdivs;
  int rangel;
  int rangeh;
  int rbase;
  int vbase;
  int color;
} BitBoxStruct;

/* data structure pointed to by data field in each  View */

typedef struct viewdatarec *ViewData;

typedef struct viewdatarec
{

/* display control variables (not all are relevant
   for all views) */

  Boolean         displayEvent;
  Boolean         displayTracks;
  Boolean         displayBeamLine;
  Boolean         displayField;
  Boolean         displayDC;
  Boolean         displaySC;
  Boolean         displayCC;
  Boolean         displayEC;
  Boolean         displayU;
  Boolean         displayV;
  Boolean         displayW;

/* visiblesectors & phi is relevant for sector views */

  short           visiblesectors;
  float           phi;             /* in ced2.0, phi is absolute phi [0, 2Pi] */
  short           dphi;            /* offset from midplane (controlled by slider) */
  Hv_FRect        worlds[4];       /* copy of world system so that each option button "remembers"*/

/* ecplane is relevant for ECVIEWS */

  short           ecplane;


/* various Hv_Items saved for fast selectability purposes */

  Hv_Item         AnchorItem;
  Hv_Item         FieldDisplayItem;
  Hv_Item         FieldRainbowItem;
  Hv_Item         FieldmapItem;
  Hv_Item         SectorBoxItem;
  Hv_Item         SectorHexItem;
  Hv_Item         InnerADCRainbowItem;
  Hv_Item         OuterADCRainbowItem;
  Hv_Item         EnergyRainbowItem;

  Hv_ItemListData ScintItemList;
  Hv_ItemListData ECItemList;

/* colors */

  short           scintframecolor;
  short           scintfillcolor;
  short           scinthitcolor;
  short           ecframecolor[3]; /* array for inner, outer */
  short           ecfillcolor[3];  /* array for inner, outer */
  short           echitcolor;
  short           fannedeccolor;
  short           fannedechitcolor;
  short           fannedecbgcolor;
  short           fannedecstripcolor;

/* Specific info for Tigris... */

  int             sectorNumber ;
  int             sglphase ;
  int             ButState6_12;    /* 0 = lower 6, 6 = upper 6 SE's */
  int             OldCommentIndex;  /* remember the last comments index */
  Hv_Item         evcommentbox;
  Hv_Item         activeTextItem;
  Hv_Item         itemIsInvert ;
  Hv_Item         eitem [6] ;
  Hv_Item         SELabels [6];
  Hv_Item         ArrayOScintillators[4];
  Hv_Item         butAnd;
  Hv_Item         butOr;
  Hv_Item         butNot;
  Hv_Item         butClr ;
  Hv_Item         butChk ;
  BitBoxStruct    *bbs1;
  BitBoxStruct    *bbs2;
  BitBoxStruct    *bbs3;
  BitBoxStruct    *bbs4;
  Hv_Item         ScintDFront16bits;
  Hv_Item         ScintDBack32bits;  
  Hv_Item         ExtraDataBits;
  Hv_Item         SpareDataBits;

} ViewDataRec;

/*
typedef struct cart3D         ordinary Cartesian 3D vector 
{
  float  x;
  float  y;
  float  z;
} Cart3D;

*/
/*----- magnetic field (and related)  types ------*/

typedef long     ivect[3];             /*long int vector*/
typedef float    fvect[3];             /*float vector*/

typedef enum {Xcomp, Ycomp, Zcomp} Vcomp;       /*vector components*/

typedef struct fieldmap  *FieldMapPtr;

/* structure to hold 3D magnetic field */

typedef struct fieldmapstruct
{
  ivect            Nxyz;       /* #grid points in each direction*/ 
  unsigned long    NxNy;       /* =Nx*Ny -- saved to avoid repeated calc*/
  unsigned long    ngr;        /* = 3*Nx*Ny*Nz = # magnetic field components stored */
  fvect            Rmin;       /* minimum values of field extent in each direction*/
  fvect            Rmax;       /* maximum values of field extent in each direction*/
  fvect            dR;         /* gridsize spacing in each direction*/
  fvect            RdR;        /* 1/dR; stored to prevent repeated calc*/
  fvect            *fmapptr;   /* pointer to actual field map data*/
  float            scalefact;  /* to scale field from nominal value*/
} FieldMap;

/* fieldmap data used for field map Drawn Objects
   Note: all field maps use the gloab fmap for data,
   thus they do not contain dat of type FieldMap struct */

typedef struct fieldmapdatatype  *FieldMapDataTypePtr;



#endif



