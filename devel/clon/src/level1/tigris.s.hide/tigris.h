
/**************************/
/* tigris.h - main header */
/**************************/

/**********************/
/* former tgeometry.h */

#define MAX_REGIONS       3 /* Total number of regions in sector */
#define NUM_CERENK       18 /* number of cerenkov counters per sector */
#define NUM_SHOWER        2 /* number of shower counter planes per sector */
#define NUM_STRIP        36 /* number of shower counter strips per direction */
#define NUM_SCINT        48 /* number of scints per sector */
#define NUM_T_COUNTERS   48 /* number of t counters in the photon tagger */


#define     POINT1        01 /* bit indicating point 1 of the wire hexagon */
#define     POINT2        02 /* bit indicating point 2 of the wire hexagon */
#define     POINT3        04 /* bit indicating point 3 of the wire hexagon */
#define     POINT4       010 /* bit indicating point 4 of the wire hexagon */
#define     POINT5       020 /* bit indicating point 5 of the wire hexagon */
#define     POINT6       040 /* bit indicating point 6 of the wire hexagon */

/* EMC related  */

#define  INNER_EMC      0
#define  OUTER_EMC      1
#define  BOTH_EMC       2


#define  U_PLANE        0
#define  V_PLANE        1
#define  W_PLANE        2

#define  X_PLANE        0
#define  Y_PLANE        1

typedef struct cart3D         /*ordinary Cartesian 3D vector */
{
  float  x;
  float  y;
  float  z;
} Cart3D;

/* --------  CERENKOV DATA STRUCTURES ------------*/

typedef struct cerenk
{
  Hv_FPoint P[4];                   /*midplane corners of cerenkov*/
} Cerenk;

typedef struct cerenk_package
{
  Cerenk cerenks[NUM_CERENK];
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
  float       length;          /*length of scintillator*/
  float        width;          /*width of scintillator*/
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
  float     radius;              /*radius from offset center in meters*/
  float     theta_min;           /*minimum theta (radians - midplane)*/
  float     theta_max;           /*maximum theta (radians - midplane)*/
  short     lastcircwire;        /*index (C index) of last wire in lay 3 circular region (applic only for region 1*/
  float     plane_angle;         /*angle at which plane part (as in region 1) starts  (make = Pi in no plane part)*/
  short     num_wires;           /*number of wires in this layer*/
} DC_Layer;

typedef struct dc_super_layer
{
  short     numlayers;           /*how many layers there are*/
  float     dlen;                /*distance between wires (const for plane part of reg 1)*/
  float     dR;                  /*radial separation between layers */
  float     Rinner, Router;      /*radial limits of the superlayer*/
  float     zc;                  /*horizontal center (origin at target)*/
  float     xc;                  /*vertical center   (origin at target)*/
  float     dtheta;              /*angular separation (radians) for circular part wrt bogdan cent*/
  float     stereo_angle;        /*stereo angle of wire (radians) i.e. zero for axial wires*/
  DC_Layer  *layers;             /*data for each layer*/
} DC_Super_Layer;

typedef struct dc_region
{
  float           Rinner, Router;    /*radial limits of the region*/
  DC_Super_Layer  superlayers[2];    /*each region has two super layers*/
} DC_Region;

typedef struct dc_package
{
  DC_Region regions[3];        /*a dc package is comprised of three regions*/
} DC_Package;

typedef struct dcpixelcontrol     /*sets limits for various aspects of DC drawing*/
{
  int NormalWireCutoff;          /*pixel/meter where normal (non-hit) wires are turned on*/
  int HitWireLargeCutoff;        /*pixel/meter density when hits switch from small to big wires*/
  int HexagonCutoff;             /*pixel/meter density when hexagons turn on*/
  int HitCircleCutoff;           /*pixel/meter density when hit circles turn on*/
} DCPixelControl;


/****************/
/* former def.h */

#define DebugLevel     22
#define PARS            2
#define MACH            1
#define UNUSED_VAR     -1	
#define TOKEN_LENGTH   32
#define ALPHA          10
#define DIGIT          11
#define AND_OP         12
#define OR_OP          14
#define DB   debug_buffer

struct Look
{
  char *theline;
  char *thepoint;
  int  thelength;
  int  type;
  char ch;
  int  num;
  char s[TOKEN_LENGTH];
};


/**********************/
/* former scintlook.h */

typedef struct LitBitButs
{
  int howmany;
  int litebitbut[2];
} LitBitButs;

typedef struct BitsLookup
{
  LitBitButs b;
  int howMany;
  int litBits[5];
} BitsLookup;


/*******************/
/* former simple.h */

#define SIMPLE_FB_SCINT         0
#define SIMPLE_FB_WORLDX        2
#define SIMPLE_FB_WORLDY        3
#define SIZEOBUF             4096


/****************/
/* former sgl.h */

#define MaxSE 12            /* Maximum number of Sector Events per sector in phase2 */
                            /* May change # but must create new phase 2 view to use */
#define MaxProducts 32      /* Maximum number of products for each SE term          */
                            /* May change # without any additional program changes  */
#define MaxTrig 9           /* Maximum number (1-8) of phase 3 triggers             */
                            /* May change # but will require changes to Event Processor view to use */
#define MaxTrigProducts 256 /* Maximum number of products for all phase 3 triggers  */
                            /* May change # without additional program changes      */

/* secdef defines structure for SD, ED, and SE data for a single sector */
struct SLsecdef
{
  int SD[2][6][33];                     /* holds sector data bits for each Sector and phase */
                                        /* 6-10 are spare data bits.                        */
  int SP[2][4][17];                     /* holds spare data bits for each Sector and phase  */
  int ED[2][6][33];                     /* holds extra data bits for each Sector and phase  */
  int SE[MaxSE][MaxProducts][16];       /* holds sector event data for each Sector          */
  char *comment[MaxSE];              /* Comment for each SE in phase 2                   */
};

/* phase3_def defines structure for holding all the event processor trigger data */
struct phase3_def
{
  int Trig[MaxTrig][MaxTrigProducts][19]; /* holds all triggers, with products and data          */
  int Trig_Delay[MaxTrig];                /* holds Pre-Trigger pulse stretch value for each trig */
  int Enable_Async[MaxTrig];              /* holds enable asynchronous input data for each trig  */
};






extern   void   simpleInit();
extern   void   SetupScintView();
extern   void   SetupSectView();
extern   void   scintViewFeedback();
extern   void   sectViewFeedback();

/* PROTOTYPES */

/* ced_geometry */
void InitGeometry();

/* cradle.c */
void skip_op();
int  Is_SE();
int  Is_SD();
int  Is_FC();
int  Is_CC();
int  Is_ED();
int  Is_SP();
int  Is_and_op();
int  Is_or_op();
int  Is_White_Space();
int  Is_Digit();
int  Is_Alpha();
void init(char *str);
void show_look_ahead();
void Skip_White();
int  get_number(char *b);
int  get_name(char *b);
void clean_look();
int  lex(char *bfr);

/* textitem.c */
void JAM_FieldSingleClick(Hv_Event hevent);
void JAM_TextInit(Hv_Item Item, Hv_AttributeArray attributes);
void JAM_CreateMTextEdit(Hv_Event hevent);
void TRIG_SingleClick(Hv_Event hevent);

/* evbutton.c */
int  EV_case(int z, int j, char *flag);
void flush_those_textItems(ViewData vdata);
void EVSERadioClick(Hv_Event hevent);
void mov2sgl(int nphase);
void movPhase12sgl(int phase);
void movSE2sgl();
void downloadSE();
void buildEventArea(Hv_View View);

/* feedback.c */
void scintViewFeedback(Hv_View View, Hv_Point pp);

/* sectorhex.c */
Hv_Item MallocSectorHex(Hv_View View, int num);

/* menus.c */
void AsyncRadioClick(Hv_Event hevent);
void SyncControl(Hv_Widget w);
void AsyncControl(Hv_Widget w);
void DownloadControl(Hv_Widget w);

/* scintbuts.c */
void bbSDclick(Hv_Event hevent);
void bbEDclick(Hv_Event hevent);
void bbSPclick(Hv_Event hevent);
void DrawSDbits(Hv_Item Item, Hv_Region region);
void DrawEDbits(Hv_Item Item, Hv_Region region);
void DrawSPbits(Hv_Item Item, Hv_Region region);

/* init.c */
void simpleInit();
char *newViewTitle(char *oldtitle, int opt);
void initScintEventStructs(int sector);
void ReadFromFile();
void SaveToFile();

/* setupview.c */
void DoBigViewSetup(Hv_View View, Hv_Rect *hotrect);

/* setup.c */
ViewData GetViewData(Hv_View View);
float MidplaneXtoProjectedX(Hv_View View, float x);
void SetupScintView(Hv_View View);
void SetupSectView(Hv_View View);

/* sectbuttons.c */
void trigAndButClick(Hv_Event hevent);
void trigOrButClick(Hv_Event hevent);
void trigNotButClick(Hv_Event hevent);
void trigClrButClick(Hv_Event hevent);
void refresh_trigr(int zzz);
void SERadioClick(Hv_Event hevent);
void DrawTrigButtons(Hv_View View, Hv_Item Item, int n);
void TrigParser();
void downloadTrig();

/* scintillator.c */
void init_bitslookup();
void LightScintillators(Hv_View view, int sc, int sd, Hv_Point *pt);
Hv_Item MallocScintillator(Hv_View View, int sect, short ID);
void SetScintGeometrySector(Hv_View View);
void GetSlabPolygon(Hv_View View, short sect, short slab, Hv_Point *poly);
void DrawAllScintItems(Hv_View View);
void ScintSlabLimits(short ID, short *minid, short *maxid);

/* fileio.c */
int  ReadValues();
int  SaveFinalValues();
void bigdata_cleanup();
void download_phase1(int sector_num, int phase);
void download_phase2(int sector_num);
void download_phase3();







