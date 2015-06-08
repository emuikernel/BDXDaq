#ifndef __BOSH
#define __BOSH

#define  NDIM      800000
#define  WDIM      800000

#define EVENTCACHESIZE 10   /* number of events it caches */

#define ETPRESCALE     1
#define ETQSIZE        20
#define ETSTATION      "CED"

/*--------------------------------------------
FOR USE IN DC NOISE REMOVAL TEST
----------------------------------------*/

typedef struct dcbits  *DCBitDataPtr;

typedef struct dcbits {
    HugeWordPtr    data[6];
    int            nlayer;
    int            nwire;  /*max LAST WIRE ID */
} DCBitData;


/*-----------------------------------------------------
FOR EVENT REFERENCE SEE CLAS NOTE 94-12
------------------------------------------------------*/

typedef struct bosbank {
  int    pad[5];
  int    iw[NDIM];
} BOSBank;


/*----------------------------------------------------------
  data structure for accumulated DC data
  each REGION in each sector will have one of these
----------------------------------------------------------*/

typedef struct accdc
{
    unsigned int      maxhits;
    int               scale;

/* for each superlayer, store the hottest wire */

    int               hotid; /* 256*layer + wire*/

    unsigned int      counts[12][192]; /* counts for each wire */
    float             tdcsum[12][192];
    float             occsum[2]; /* one for each superlayer */
} AccumulatedDC;


/*----------------------------------------------------------
  data structure for accumulated Scintillator data 
----------------------------------------------------------*/

typedef struct accsc
{
  unsigned int      maxhits;
  int               scale;
  unsigned int      counts[NUM_SCINT];

} AccumulatedSC;



/*----------------------------------------------------------
  data structure for accumulated Forward EC data 
----------------------------------------------------------*/

typedef struct acec
{
  unsigned int      maxhits;
  int               scale;
  unsigned int      counts[2][3][36];  /* plane (0..1), type (0..2), strip (0..35) */
} AccumulatedEC;


/*----------------------------------------------------------
  data structure for accumulated Italian LAC data 
----------------------------------------------------------*/

typedef struct acec1
{
  unsigned int      maxhits;
  int               scale;
  unsigned int      counts[2][2][40];   /* plane (0..1), type (0..2), strip (0..39) */
} AccumulatedEC1;


/*---------------------------------------------
 data structure for component status
-------------------------------------------------------------------*/

typedef struct componentstatus  *ComponentStatusPtr;

typedef struct componentstatus
{
  unsigned short id;
  short status;
} ComponentStatus;



/*---------------------------------------------
 data structure for any component with the standard id, tdc, adc format
-------------------------------------------------------------------*/

typedef struct normaldata  *NormalDataPtr;

typedef struct normaldata
{
  unsigned short id;
  unsigned short tdc;
  unsigned short adc;
} NormalData;


/*------------------------------
structure for DCGM bank
-----------------------------*/

typedef struct dcgmdata *DCGMDataPtr;

typedef struct dcgmdata {
    float x_curve;      /* x center of curvature (cm) */
    float y_curve;      /* y center of curvature (cm) */
    float z_curve;      /* z center of curvature (cm) */
    float r_curve;      /* radius of curvature (cm) */

/* theta_start: angle of first logical wire WRT the center of curvature (rad) */

    float theta_start;

/* d_theta: delta theta between wires WRT center of curvature */    

    float d_theta;

    float x_nmid; /* x normal vector to the midplane */
    float y_nmid; /* y normal vector to the midplane */
    float z_nmid; /* z normal vector to the midplane */

    float theta_min; /* theta of first physical wire (rad) */
    float theta_max; /*theta of first physical wire (rad) */

    int   min_wire;   /* minimum physical wire number*/
    int   max_wire;   /* maximum physical wire number*/
    
    float stereo;     /* approximate stereo angle*/
    float cell_size;  /* cell size (cm)*/
    float x_norm;     /* x normal vector to the plane(region 1) */
    float y_norm;     /* y normal vector to the plane(region 1) */
    float z_norm;     /* z normal vector to the plane(region 1) */
    float p_dist;     /* distance of plane to origin(cm) (region 1)*/
    float p_sep;      /* planar separation(cm) (region 1) */
    int   max_cylw;   /* maximum cylindrical wire */
} DCGMdata;


/*------------------------------
structure for DCGW bank
-----------------------------*/

typedef struct dcgwdata *DCGWDataPtr;

typedef struct dcgwdata {
    float x_mid;    /* x at midplane of wire (cm)*/
    float y_mid;    /* y at midplane of wire (cm)*/
    float z_mid;    /* z at midplane of wire (cm)*/
    float x_dir;    /* x direction cosine along wire */
    float y_dir;    /* y direction cosine along wire */
    float z_dir;    /* z direction cosine along wire */
    float w_len;    /* wire length from midplane to amplifier (cm) */
    float w_len_hv; /* wire length from midplane to HV (cm) */
} DCGWdata;

/*------------------------------
structure for part bank
-----------------------------*/

typedef struct partdata *PARTDataPtr;

typedef struct partdata {
    int   pid;     /*  particle id (GEANT) */
    float x;       /* vertex x*/
    float y;       /* vertex y*/
    float z;       /* vertex z*/
    float E;       /* energy*/
    float px;      /* momentum x*/
    float py;      /* momentum y*/
    float pz;      /* momentum z*/
    float q;       /*  charge */
    int   trkid;   /*  track pointer */
    float qpid;    /*  quality factor for the pid */
    float qtrk;    /*  quality factor for the trk */
    int   flags;   /*  set of flags defining track (ie, BEAM) */
} PARTData;


/*----------------------------------------------
structure for hit-based Track candidates "HBTR"
-----------------------------------------------*/

typedef struct hbtrdata  *HBTRDataPtr;

typedef struct hbtrdata
{
    float x;        /*  x */
    float y;        /*  y 'Vertex' position {x,y,z} */
    float z;        /*  z */
    float px;       /*  Px */
    float py;       /*  Py  momentum at 'vertex' {Px,Py,Pz} */
    float pz;       /*  Pz */
    float q;        /*  charge   (straight tracks: set q=0.) */
    float chi2;     /*  Chisquare for this Track */
    int itr_sec;    /*  Trackno_in_Sector + Sector*100 */
} HBTRData;


/*----------------------------------------------
structure for time-based Track candidates "TBTR"
-----------------------------------------------*/

typedef struct tbtrdata  *TBTRDataPtr;

typedef struct tbtrdata
{
    float x;        /*  x */
    float y;        /*  y 'Vertex' position {x,y,z} */
    float z;        /*  z */
    float px;       /*  Px */
    float py;       /*  Py  momentum at 'vertex' {Px,Py,Pz} */
    float pz;       /*  Pz */
    float q;        /*  charge   (straight tracks: set q=0.) */
    float chi2;     /*  Chisquare for this Track */
    int itr_sec;    /*  Trackno_in_Sector + Sector*100 */
    int itr_hbt;    /*  Trackno in HBTR for this track */
} TBTRData;


/*-------------------------------------
 * structure for HBLA (Hit based layers) bank
 * this is for DC hits
 *
 * 34 rows per track
 *-------------------------------------*/

typedef struct hbladata  *HBLADataPtr;

typedef struct hbladata
{
    int trk_pln;    /*  (track_number) *100 + Trk_plane_number */
    float x;        /*  z coord [cm]  for track in this plane */
    float y;        /*  y coord [cm]  for track in this plane */
    float z;        /*  z coord [cm]  for track in this plane */
    float cx;       /*  direction cosine (cx) at coord.{x,y,z} */
    float cy;       /*  direction cosine (cy) at coord.{x,y,z} */
    float cz;       /*  direction cosine (cz) at coord.{x,y,z} */
    float tlen;     /*  track length [cm] from origin to this plane */
    int dc1;        /*  Pointer to DC1 bank */
    int stat;       /*  Status of the hit */
    int wire;       /*  Wire number  */
    float dtime;    /*  drift time [ns]  (not corrected for start ime) */
    float alpha;    /*  track angle (relative to R of SL) [deg] */
    float wlen;     /*  Wire length (hit pos. to preamp)  [cm] */
    float sgdoca;   /*  sigma DOCA */
    float fitdoca;  /*  Fitted DOCA */
} HBLAData;


/*-------------------------------------
 * structure for HDPL (Hit based layers) bank
 * non-DC planes
 *
 * 10 rows per track
 *-------------------------------------*/

typedef struct hdpldata  *HDPLDataPtr;

typedef struct hdpldata
{
    int trk_pln;    /*  (track_number) *100 + plane_number */
    float x;        /*  x coord [cm]  for track in this plane */
    float y;        /*  y coord [cm]  for track in this plane */
    float z;        /*  z coord [cm]  for track in this plane */
    float cx;       /*  direction cosine (cx) at coord.{x,y,z} */
    float cy;       /*  direction cosine (cy) at coord.{x,y,z} */
    float cz;       /*  direction cosine (cz) at coord.{x,y,z} */
    float tlen;     /*  track length [cm] from origin to this plane */
} HDPLData;


/*-------------------------------------
 * structure for TDPL (Time based layers) bank
 * non-DC planes
 *
 * 10 rows per track
 *-------------------------------------*/

typedef struct tdpldata  *TDPLDataPtr;

typedef struct tdpldata
{
    int trk_pln;    /*  (track_number) *100 + plane_number */
    float x;        /*  x coord [cm]  for track in this plane */
    float y;        /*  y coord [cm]  for track in this plane */
    float z;        /*  z coord [cm]  for track in this plane */
    float cx;       /*  direction cosine (cx) at coord.{x,y,z} */
    float cy;       /*  direction cosine (cy) at coord.{x,y,z} */
    float cz;       /*  direction cosine (cz) at coord.{x,y,z} */
    float tlen;     /*  track length [cm] from origin to this plane */
} TDPLData;



/*-------------------------------------
 * structure for TBLA (Time based layers) bank
 * this is for DC hits
 *
 * 34 rows per track
 *-------------------------------------*/

typedef struct tbladata  *TBLADataPtr;

typedef struct tbladata
{
    int trk_pln;    /*  (track_number) *100 + Trk_plane_number */
    float x;        /*  z coord [cm]  for track in this plane */
    float y;        /*  y coord [cm]  for track in this plane */
    float z;        /*  z coord [cm]  for track in this plane */
    float Bx;       /*  B field x component (kG) */
    float By;       /*  B field y component (kG) */
    float Bz;       /*  B field z component (kG) */
    float tlen;     /*  track length [cm] from origin to this plane */
    int dc1;        /*  Pointer to DC1 bank */
    int stat;       /*  Status of the hit */
    int wire;       /*  Wire number  */
    float dtime;    /*  drift time [ns]  (not corrected for start ime) */
    float alpha;    /*  track angle (relative to R of SL) [deg] */
    float wlen;     /*  Wire length (hit pos. to preamp)  [cm] */
    float sgdoca;   /*  sigma DOCA (cm) */
    float fitdoca;  /*  Fitted DOCA (cm) */
    float calcdoca; /*  calculated DOCA (via dtime) [cm]*/
} TBLAData;




/*-------------------------------------
structure for holding DC hits
-----------------------------------------*/

/**********************
 the struct is for the new
 DC0 banks without an ADC
************************/

typedef struct dc0data  *DC0DataPtr;

typedef struct dc0data
{

/* wire - layer are bytes stuffed into id */

  unsigned short id;
  unsigned short tdc;
} DC0Data;


/**********************
 DC1 banks have an int
 id and a float time
************************/

typedef struct dc1data  *DC1DataPtr;

typedef struct dc1data
{

/* wire - layer are bytes stuffed into id */

  int   id;
  float time;
} DC1Data;


/************************
   Level 2 segment banks
************************/

typedef struct segdata  *SEGDataPtr;

typedef struct segdata
{
  unsigned short superlayer;
  unsigned short segid;
  unsigned short rsvd1;  /*reserved*/
  unsigned short rsvd2;  /*reserved*/
} SEGData;

/*-------------------------------------------
 structure for holding EC hits.
   The layer is defined as

   1-6 (uvw inner, uvw outer) for FORWARD ANGLE 
   1-4 (xy inner, xy outer) for LARGE ANGLE
-----------------------------------------------*/

typedef NormalDataPtr ECDataPtr;
typedef NormalData    ECData;

/*-------------------------------------
structure for analyzed ec bank
--------------------------------------*/
typedef struct pidata *PiDataPtr;

typedef struct pidata
{
    int     id;     /* sector number*/
    int     layer;  /* 9:both 10:inner 11:outer */
    int     hitid;  /* low 16: num hits; high 16: hit id */
    float   iloc;   /* GeV */
    float   jloc;   /* ns */
    float   diloc;  /* cm */
    float   djloc;  /* cm */
    float   r;      /* show radius */
    float   e;      /* energy */
} PiData;

/*----------------------------------
 data structure for scintillator data
--------------------------------------*/

typedef struct scdata  *SCDataPtr;

typedef struct scdata
{
  unsigned short id;
  unsigned short tdcleft;
  unsigned short adcleft;
  unsigned short tdcright;
  unsigned short adcright;
} SCData;

/*----------------------------------
 data structure for new start counter TDC
--------------------------------------*/

typedef struct stn0data  *STN0DataPtr;

typedef struct stn0data {
  unsigned short id;
  unsigned short tdc;
} STN0Data;

/*----------------------------------
 data structure for new start counter ADC
--------------------------------------*/

typedef struct stn1data  *STN1DataPtr;

typedef struct stn1data {
  unsigned short id;
  unsigned short adc;
} STN1Data;


/*----------------------------------
 data structure for tagger E counter
--------------------------------------*/

typedef struct tagedata  *TAGEDataPtr;

typedef struct tagedata
{
  unsigned short id;
  unsigned short tdc;
} TAGEData;


/*----------------------------------
 data structure for tagger T counter
--------------------------------------*/

typedef struct tagtdata  *TAGTDataPtr;

typedef struct tagtdata
{
  unsigned short id;
  unsigned short tdcleft;
  unsigned short tdcright;
} TAGTData;

/* EC1 data has the same format as scint data,
   a left and right tdc and adc */

typedef SCDataPtr EC1DataPtr;
typedef SCData    EC1Data;

/*-------------------------------------
 data structure for HEAD data bank
--------------------------------------*/

typedef struct headdata  *HEADDataPtr;

typedef struct headdata
{
  int        VersionNumber;
  int        RunNumber;
  int        EventNumber;
  int        EventTime;
  int        EventType;
  int        ROCStatus;
  int        EventClass;
  int        EventWeight;
} HEADData;

/*-------------------------------------
 data structure for TGBI data bank
--------------------------------------*/

typedef struct tgbidata  *TGBIDataPtr;

typedef struct tgbidata
{
  int        res1;
  int        res2;
  int        res3;
  int        res4;
  int        res5;
} TGBIData;

/*------------------------------------
 data structure for MC input data bank
----------------------------------------*/

typedef struct mcindata  *MCINDataPtr;

typedef struct mcindata
{
  float        Xo;
  float        Yo;
  float        Zo;
  float        Px;    /* direction cosine, i.e. Px/P */
  float        Py;    /* direction cosine, i.e. Py/P */
  float        Pz;    /* direction cosine, i.e. Pz/P */
  float        P;
  float        M;
  float        Q;
  int          LundID; 
} MCINData;


/*------------------------------------
 data structure for MCVX input data bank
--------------------------------------*/

typedef struct mcvxdata *MCVXDataPtr;

typedef struct mcvxdata 
{
  float        X;
  float        Y;
  float        Z;
  float        tof;
  int          vtx_flag;
} MCVXData ;

/*-----------------------------------
 data structure for MCTK input data bank
-------------------------------------*/

typedef struct mctkdata *MCTKDataPtr;

typedef struct mctkdata
{
  float        Cx;
  float        Cy;
  float        Cz;
  float        P;
  float        M;
  float        Q;
  int          LundID;
  int          track_flag;
  int          beg_vtx;
  int          end_vtx;
  int          parent;
} MCTKData ;

/*------ global variables ------*/

extern   int     bosNumEvents;
extern   char   *bosFileName;
extern   char   *bosDataName;


extern   int     bosNdim;
extern   int     bosUnit;

/* variables added for handling output */

extern   char   *bosOutFileName;
extern   char   *bosOutDataName;
extern   int     bosOutUnit;

extern   BOSBank  bcs_;    /* main bos common */
extern   BOSBank  wcs_;    /* additional bos common */

extern   int     bosIndexDC0[6];  /* new DC */
extern   int     bosIndexDC1[6];  /* with massaged float time */

/* geometry banks */

extern   int     bosIndexDCGM[6];  /* DCGM geo bank */
extern   int     bosIndexDCGW[6];  /* DCGW geo bank */

extern   int     bosIndexSEG[6];  /*level 2 segments */
extern   int     bosIndexCC[6];
extern   int     bosIndexCC1[6];
extern   int     bosIndexSC[6];
extern   int     bosIndexEC[6];
extern   int     bosIndexEC1[6];

extern   int     bosIndexTAGE;
extern   int     bosIndexTAGT;
extern   int     bosIndexSTN0;
extern   int     bosIndexSTN1;
extern   int     bosIndexHEAD;
extern   int     bosIndexTGBI;
extern   int     bosIndexMCIN;
extern   int     bosIndexMCVX;
extern   int     bosIndexMCTK;
extern   int     bosIndexECPI;   /*Stephan's EC recon bank */


/* track related */


extern   int     bosIndexHBLA[6]; /* hit based layers */
extern   int     bosIndexHDPL[6]; /* hit based non dc planes */

extern   int     bosIndexTBLA[6]; /* time based layers */
extern   int     bosIndexTDPL[6]; /* time based non dc planes */

extern   int     bosIndexHBTR;    /* hit based tracking result */
extern   int     bosIndexTBTR;    /* time based tracking result */
extern   int     bosIndexPART;    /* particle id bank (4 vector) */

extern   char   *bosBankNameDC0;  /* new dc */
extern   char   *bosBankNameDC1;  /* massaged float time */
extern   char   *bosBankNameDC;   /* old dc */
extern   char   *bosBankNameSEG;  /*level 2 segments */
extern   char   *bosBankNameCC;
extern   char   *bosBankNameCC1;
extern   char   *bosBankNameSC;
extern   char   *bosBankNameEC;
extern   char   *bosBankNameEC1;
extern   char   *bosBankNameECPI;

/* geo banks */

extern   char   *bosBankNameDCGM;
extern   char   *bosBankNameDCGW;

/* anachronisms needed to link with sact lib */

extern   char   *bosBankNamePI;
extern   char   *bosBankNamePO;

extern   char   *bosBankNameTAGE;
extern   char   *bosBankNameTAGT;
extern   char   *bosBankNameHEAD;
extern   char   *bosBankNameTGBI;
extern   char   *bosBankNameSTN0;
extern   char   *bosBankNameSTN1;
extern   char   *bosBankNameMCIN;

extern   char   *bosBankNameHBTR;    /* hit based tracking result */
extern   char   *bosBankNameTBTR;    /* time based tracking result */
extern   char   *bosBankNamePART;    /* 4 vectors */

extern   char   *bosBankNameHBLA;  /* hit based layers */
extern   char   *bosBankNameHDPL;  /* hit based non dc planes */

extern   char   *bosBankNameTBLA;  /* time based layers */
extern   char   *bosBankNameTDPL;  /* time based non dc planes */

extern   Boolean bosSectorHasData[6];
extern   int     bosSectorDataSize[6];

/* data pointers */

extern   DC0DataPtr     bosDC0[6];  /* new dc */
extern   DC1DataPtr     bosDC1[6];  /* massaged float time */
extern   SEGDataPtr     bosSEG[6];  /* level 2 segments*/
extern   NormalDataPtr  bosCC[6];
extern   NormalDataPtr  bosCC1[6];
extern   SCDataPtr      bosSC[6];
extern   ECDataPtr      bosEC[6];
extern   EC1DataPtr     bosEC1[6];

extern   PiDataPtr      bosECPI;

/* geo banks */

extern   DCGMDataPtr     bosDCGM[6];
extern   DCGWDataPtr     bosDCGW[6];


extern   TAGEDataPtr    bosTAGE;
extern   TAGTDataPtr    bosTAGT;
extern   HEADDataPtr    bosHEAD;
extern   TGBIDataPtr    bosTGBI;
extern   STN0DataPtr    bosSTN0;
extern   STN1DataPtr    bosSTN1;
extern   MCINDataPtr    bosMCIN;
extern   MCVXDataPtr    bosMCVX;
extern   MCTKDataPtr    bosMCTK;


/* data pointers for tracks */

extern   HBLADataPtr    bosHBLA[6];  /* hit based layers */
extern   HDPLDataPtr    bosHDPL[6];  /* hit based non DC planes */
extern   HBTRDataPtr    bosHBTR;     /* hit based tracking result */

extern   TBLADataPtr    bosTBLA[6];  /* time based layers */
extern   TDPLDataPtr    bosTDPL[6];  /* time based non DC planes */
extern   TBTRDataPtr    bosTBTR;     /* time based tracking result */

extern   PARTDataPtr    bosPART;     /* four vectors */

extern   int            bosNumDC0[6];  /*new DC */
extern   int            bosNumDC1[6];  /*massaged float time*/
extern   int            bosNumSEG[6];  /*level 2 segments*/
extern   int            bosNumCC[6];
extern   int            bosNumCC1[6];
extern   int            bosNumSC[6];
extern   int            bosNumEC[6];
extern   int            bosNumEC1[6];
extern   int            bosNumECPI;

extern   int            bosNumTAGE;
extern   int            bosNumTAGT;
extern   int            bosNumSTN0;
extern   int            bosNumSTN1;
extern   int            bosNumMCIN;
extern   int            bosNumMCVX;

/* for DC Noise removal test */

extern   DCBitData       dcbitdata[6][3][2];  /* sect, rgn, supl */
extern   DCBitData       cleandcbitdata[6][3][2];  /* sect, rgn, supl */

/* used for tracks */

extern   int            bosNumHBLA[6]; /* hit based layers */
extern   int            bosNumHDPL[6]; /* hit based non DC planes */
extern   int            bosNumHBTR;    /* hit based tracking result */

extern   int            bosNumTBLA[6]; /* time based layers */
extern   int            bosNumTDPL[6]; /* time based non DC planes */
extern   int            bosNumTBTR;    /* time based tracking result */
extern   int            bosNumPART;    /* four vectors */

extern   ComponentStatusPtr dcstatus[6][3][2] /* sect, rgn, supl */;

/* accumulated data for color histograms */

extern   AccumulatedDC   accdc[6][3]; 
extern   AccumulatedSC   accsc[6]; 
extern   AccumulatedEC   accec[6]; 
extern   AccumulatedEC1  accec1[6]; 


extern   long           accumEventCount;
extern   int            triggerFilter; /* to filter on event type */
extern   int            triggerMaxTries;  /* number times will try to find a trigger */
extern   int            triggerBits[];

extern   Hv_Widget      evnextmenu;
extern   Hv_Widget      evprevmenu;
extern   Hv_Widget      evgotomenu;
extern   Hv_Widget      evwritemenu;
extern   Hv_Widget      evoutputmenu;
extern   Hv_Widget      evclosemenu;
extern   Hv_Widget      evopenmenu;
extern   Hv_Widget      evetmenu;

extern   Hv_Widget      evintervalmenu;
extern   Hv_Widget      evfiltermenu;

extern Hv_TextPosition   evpos;
extern Hv_Widget         evtext;

extern Hv_Widget         evdc0[6];
extern Hv_Widget         evdc1[6];
extern Hv_Widget         evseg[6];
extern Hv_Widget         evsc[6];
extern Hv_Widget         evcc[6];
extern Hv_Widget         evcc1[6];
extern Hv_Widget         evec[6];
extern Hv_Widget         evec1[6];

extern Hv_Widget         evhbla[6];  /* hit based layer banks */
extern Hv_Widget         evhdpl[6];  /* hit based non dc planes */

extern Hv_Widget         evtbla[6];  /* time based layer banks */
extern Hv_Widget         evtdpl[6];  /* time based non dc planes */

extern char             *eclayernames[4];


/* these nasties have to do with event caching, 
   so that "prev" can work much faster */

extern  int             cacheIndex;
extern  int            *eventCache[EVENTCACHESIZE];


extern  char           *etsession;
#endif





