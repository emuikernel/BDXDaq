/*
  ---------------------------------------------------------------------------
  -
  -   	File:     data.h
  -
  -	typedefs for ced version 2.0
  -
  -										
  -  Revision history:								
  -                     							        
  -  Date       Programmer     Comments						
  --------------------------------------------------------------------------------
  -  10/19/94	DPH		Initial Version					
  -------------------------------------------------------------------------------
  */

#ifndef __CEDDATAH
#define __CEDDATAH

/* huge words are used in the dc noise reduction */

typedef struct hugeword   *HugeWordPtr;

typedef struct hugeword
{
  short           n;      /*number of longs needed */
  unsigned long   *words; /*words[0] is least significant */
} HugeWord;

/* data structure pointed to by data field in each  View */

typedef struct viewdatarec *ViewData;

typedef struct viewdatarec
{
    
/* display control variables (not all are relevant
   for all views) */
    
    Boolean         displayTagger;
    Boolean         displayStart;
    Boolean         displayEvent;
    Boolean         displayBeamLine;
    Boolean         displayField;
    Boolean         displayDC;
    Boolean         displaySC;
    Boolean         displayCC;
    Boolean         displayEC;
    Boolean         displayX;          /* display X EC1 (long) strips */
    Boolean         displayY;          /* display Y EC1 (short) strips */
    Boolean         displayU;          /* display U EC strips */
    Boolean         displayV;          /* display V EC strips */
    Boolean         displayW;          /* display W EC strips */
    Boolean         displayDCA;        /* dist closest approach*/
    Boolean         displayHex;        /* dist closest approach*/
    Boolean         suppressTDC;       /* suppress data with zero TDC */
    Boolean         suppressNoise;     /* suppress display of noise */
    Boolean         displayDead;       /* display dead wires */

    Boolean         displayDC0;        /* display DC0 banks */
    Boolean         displayDC1;        /* display DC1 banks */

    Boolean         displayHBT;        /* display hit based tracks */
    Boolean         displayTBT;        /* display time based tracks */

    short           hb_color;          /* hit based track color */
    short           tb_color;          /* time based track color */

    int             dc1control;        /* all or just pointed to by tracks */

    
    int             showwhat;          /* single or accumulated eevnts */
    int             algorithm;         /* for color histos */
    
/* visiblesectors & phi is relevant for sector views */
    
    short           visiblesectors;
    float           phi;             /* in ced2.0, phi is absolute phi [0, 2Pi] */
    short           dphi;            /* offset from midplane (controlled by slider) */
    Hv_FRect        worlds[4];       /* copy of world system so that each option button "remembers"*/
    
/* ecplane is relevant for ECVIEWS */
    
    short           ecplane;

/* relevant for  accumulated histos in calorimeter view */

    short           ecshowwhat;
    short           ec1showwhat;
    
/* various Hv_Items saved for fast selectability purposes */
    
    Hv_Item         AnchorItem;
    Hv_Item         FieldDisplayItem;
    Hv_Item         FieldRainbowItem;
    Hv_Item         FieldmapItem;
    Hv_Item         SectorBoxItem;
    Hv_Item         SectorHexItem;
    Hv_Item         InnerADCRainbowItem;     /* forward EC */
    Hv_Item         OuterADCRainbowItem;     /* forward EC */
    Hv_Item         EC1InnerADCRainbowItem;  /* italian EC */
    Hv_Item         EC1OuterADCRainbowItem;  /* italian EC */
    Hv_Item         EnergyRainbowItem;
    Hv_Item         ADCRainbowItem;
    Hv_Item         AccumulatedHitsItem[2];  /* allow 2 per view */
    Hv_Item         AverageTDCItem[3];
    Hv_Item         MessageBoxItem;
    Hv_Item         BeamLineItem;
    
/* detector items  */
    
    Hv_Item         CerenkovItems[NUM_CERENK2];       /* for hi & low sector */
    Hv_Item         ScintPlaneItems[NUM_SCINTPLANE2]; /* for hi & low sector */
    Hv_Item         FannedScintItems[NUM_SECT];   /* 1 per sector */
    Hv_Item         ForwardECViewECItems[NUM_SECT];   /* 1 per sector */
    Hv_Item         ForwardECSectViewItems[NUM_SHOWER2];  /* one inner, one outer */
    Hv_Item         DCItems[NUM_SUPERLAYER2];
    Hv_Item         AllDCItems[6][NUM_SUPERLAYER];
    Hv_Item         LAECItems[NUM_SECT];
    Hv_Item         SectViewEC1Items[2];
    Hv_Item         TaggerItem;
    Hv_Item         StartItem; /* start counter */

/* color buttons */
    
    Hv_Item         ColorButtons[10];
    
/* these eccolors are for the sect view */
    
    

    short           cerenkovframecolor;
    short           cerenkovlofillcolor;
    short           cerenkovhifillcolor;
    short           cerenkovhitcolor;
    
    short           fannedeccolor;
    short           fannedechitcolor;
    short           fannedecbgcolor[3];
    short           fannedecstripcolor;

/* fanned scint view */
    
    short           fannedscintcolor;
    short           fannedscinthitcolor;
    short           fannedscintbgcolor;
    short           fannedscintstripcolor;
    

/* for tagger drawing */

    float tg_axisx;
    float tg_axiszmin;
    float tg_axiszmax;
    float tg_x[6];

    
} ViewDataRec;


/* --------- a structure used by shared feedback slots -------*/

typedef struct sharedfeedback {
    short    tag;         /* shared tag */         
    char    *label[2];    /* up to 2 things can share a slot */
    short    currentslot; /* current slot */
} SharedFeedback;


typedef struct cart3D         /*ordinary Cartesian 3D vector */
{
    float  x;
    float  y;
    float  z;
} Cart3D;

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
   Note: all field maps use the global fmap for data,
   thus they do not contain dat of type FieldMap struct */

typedef struct fieldmapdatatype  *FieldMapDataTypePtr;

#endif
