h48027
s 00102/00126/00098
d D 1.2 06/11/28 20:08:12 boiarino 2 1
c *** empty log message ***
e
s 00224/00000/00000
d D 1.1 06/09/30 10:30:23 boiarino 1 0
c date and time created 06/09/30 10:30:23 by boiarino
e
u
U
f e 0
t
T
I 1

I 2
/*sergey
#include "include/ntypes.h"
#include "include/bostypes.h"
#include "include/demux_roc.h"
*/

/*sergey: some stuff from above files, until redone*/

#define MAX_NUM_TOK 11
#define TOKEN_DEPTH 32
#define MAX_STR_LEN 256
#define TOKEN_SEP  " (),\t\n" /* Token separator string */
#define QUOTE_SEP  "\"`\'"     /* Define limiters of "quoted" strings. */

typedef struct {
  int ncol;
  int nrow;
  char name[4];
  int nr;
  int nextIndex;
  int nwords;
} bankHeader_t;
typedef struct {
  int data;
} rc_t;
typedef struct {
  bankHeader_t bank;
  rc_t         rc[1];
} clasRC_t;

#define DEMUX_NUM_SLABS 385      /* Maximum for "slab" enry. */
#define DEMUX_NUM_LAYERS 37      /* Max for Layers           */
#define DEMUX_NUM_SECTORS 7      /* Max for sectors          */
#define DEMUX_NUM_MODULES 26     /* Max for Modules.         */
typedef struct {
  unsigned short nummod;         /* Number of the modules defined in crate. redundant. */
  unsigned short type;           /* Module type =0 for ADC, =1 for TDC, =2 for DC ADC */
  struct {
    char           bankname[5];  /* Name of bank to store to. */
    unsigned short sector;       /* Sector Column */
    unsigned short layer;        /* sd1 column */
    unsigned short slab;         /* sd2 column */
    unsigned short place;        /* Index into the BOS bank struct, assuming int16's */
  } ch[64];                      /* Allow for 64 channels to a module. */
} ROC_DMUX_MODULE_t;              /* One structure for each module. */

typedef struct {
  unsigned short crate;          /* Crate number. */
  ROC_DMUX_MODULE_t *mod[DEMUX_NUM_MODULES];    /* Array of pointers to modules. =NULL when no module in slot. */
} ROC_DMUX_t;

typedef struct {
  unsigned short *slot;
  unsigned short *chan;
} ROC_MUX_SLAB_t;

typedef struct {
  ROC_MUX_SLAB_t *sl[DEMUX_NUM_SLABS];  
} ROC_MUX_LAYER_t;

typedef struct {
  ROC_MUX_LAYER_t *layer[DEMUX_NUM_LAYERS];
} ROC_MUX_SECTOR_t;

typedef struct {
  ROC_MUX_SECTOR_t *sec[DEMUX_NUM_SECTORS];
} ROC_MUX_t;
typedef struct {
  int islot;
  int ichan;
  int idat;
  int itype;
} RAW_DATA_t;

/* prototypes*/
ROC_DMUX_t *alloc_decode_roc(int mode);
char *quoted_strtok(char *instr,const char *tokenstring,const char *sptokstring);
char *squeeze_space(char *string);
int kill_left_space(char *string);
char *fgets_clean(char *s, int size, FILE *pt);
int read_n_tokens(int num_tokens,char **token,FILE *infp);
void init_decode_roc(char * rocfile,
                ROC_DMUX_t *rtab,
					 ROC_MUX_t *dtab);
int decode_raw_data(clasRC_t *RC, int n_call, ROC_DMUX_t *RC_look,
					int tdc_slot_low, int tdc_slot_high, RAW_DATA_t *R);




/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/* pedo.h */

E 2
/*------------cuts on the pedestal distribution ----*/
#define MIN 100
#define MAX 800
#define SC_MAP_NAME "sc.trans"
#define EC_MAP_NAME "ec.trans"
#define CC_MAP_NAME "cc.trans"
#define PS_MAP_NAME "ps.trans"
#define PC_MAP_NAME "pc.trans"
#define TC_MAP_NAME "tac.trans"
#define ST_MAP_NAME "st.trans"
#define EC1_MAP_NAME "lac.trans"

D 2
/*----------- get  main env variables ----------*/
char *getenv();
char *clonparms;
char *clonwsrc;

                       /* run parameters */

static char *msql_database = "clasrun";
static char *session;
static int run;
static int zero = 0;

int pflag = 0;
int flag = 0;

int ev_num;
int evtnum_min;
int evtnum_max;
int Crate_num; 
char map_filename[128];
char inputfile[128];
char *root_name;
char roc_tab_file[128];
FILE *fpec1, *fpec2, *fpcc, *fpsc, *fplac;
FILE *pedlog;

E 2
/* map structure */
typedef struct {
  float *arr;
  char *mapfile;
  char *name;
  char *sub_name;
} Map_struct;


/* Structure to store fit results. */
typedef struct { 
  int amp;
  int cent;
  double sigma;
} Fit_struct;

/* Histogram Structure. */
typedef struct { 
  int max_dat;
  int *hist;
  int data;
} Hist_struct;  

/* Slot Structure stores one Module information. 64 Channels. */
typedef struct { 
  int num;           /* Slot number. */
  int type;          /* 0 = ADC, 1 = TDC */
  Hist_struct Chan[64]; /* Data for 64 channels. */
  Fit_struct   Fit[64];  /* Storage for Fit for 64 channels. */
} Slot_struct;          /* Storage for ONE slots. */

/* Crate structure stores 25 slots plus crate info. */
typedef struct { 
  int num;
  int offset;
  int err_count;
  short skip;       /* If set, skip this crate. See parse_arguments. */
  char *BOSname;    /* Name of corresp. BOS bank. */
  char *sparname;   /* Name of outpur sparsification table file. */
  char *TT_file;    /* Name of Translation Table file. */
  int  tdc_slot_low; /* Low Slot: Slot number below which all cards are ADC's. */
  int  tdc_slot_high;/* High Slot: Slot Number above which all cards are ADC's.*/
  Slot_struct Slot[25]; /* Space for 25 Slots. */
D 2
  /*sergeyROC_DMUX_t *RC_look;*/ /* Pointer to lookup table from demux routines. */
}Crate_struct;
E 2
I 2
  ROC_DMUX_t *RC_look; /* Pointer to lookup table from demux routines. */
} Crate_struct;
E 2

#define CRATE_COUNT 5   /* Tables for 5 crates. */

D 2
Crate_struct Crate[CRATE_COUNT] = { 
  
  /* CRATE DEFINITION: Store info for each crate here,
   *
   * Column:    Use:
   * 1          Crate Number.
   * 2          Offset = number of channels above pedestal that spar is set.
   * 3          Error count.
   * 4          skip flag - set with switch -skip.
   * 5          BosBank Name of RAW data. Should be RC## where ## is crate number
   * 6          Sparsification Table file name = croc##.spar.
   * 7          Translation Table filename.
   * 8          Low Slot: Slot number below which all cards are ADC's.
   * 9          High Slot: Slot Number above which all cards are ADC's. 
   *10          Array of Slots.
   *11          Pointer to Demux lookup table.
   *
   * The two entries #8 and #9  are only used when no TT tables is found. 
   */
  {12,10,0,0,"RC12","croc07.spar","ROC12.tab",7,99,{0*25},/*sergey NULL*/}, /* Crate 12  = CC & TAGT */
  {13,10,0,0,"RC13","croc06.spar","ROC13.tab",8,17,{0*25},/*sergey NULL*/},  /* Crate 13 = SC & CALL */
  {14,15,0,0,"RC14","croc08.spar","ROC14.tab",15,99,{0*25},/*sergey NULL*/}, /* Crate 14 = EC1  */
  {15,15,0,0,"RC15","croc11.spar","ROC15.tab",0,13,{0*25},/*sergey NULL*/},  /* Crate 15 = EC2  */
  {16,15,0,0,"RC16","LAC.spar"   ,"ROC16.tab",9,19,{0*25},/*sergey NULL*/}}; /* Crate 16 = LAC */
E 2

D 2
Map_struct Mcc[2] = {
  {NULL,CC_MAP_NAME,"pedestals","mean"} ,
  {NULL,CC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mcc_id[2] = {
  {NULL,CC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,CC_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdsps[2] = {
  {NULL,PS_MAP_NAME,"pedestals","mean"} ,
  {NULL,PS_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdsps_id[2] = {
  {NULL,PS_MAP_NAME,"pedestals","sigma"} ,
  {NULL,PS_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdspc[2] = {
  {NULL,PC_MAP_NAME,"pedestals","mean"} ,
  {NULL,PC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdspc_id[2] = {
  {NULL,PC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,PC_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdstc[2] = {
  {NULL,TC_MAP_NAME,"pedestals","mean"} ,
  {NULL,TC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdstc_id[2] = {
  {NULL,TC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,TC_MAP_NAME,"pedestals","sigma"}};

Map_struct Msc[4] = {
  {NULL,SC_MAP_NAME,"pedestals","left"} ,
  {NULL,SC_MAP_NAME,"pedu","left"},
  {NULL,SC_MAP_NAME,"pedestals","right"},
  {NULL,SC_MAP_NAME,"pedu","right"}};
Map_struct Msc_id[2] = {
  {NULL,SC_MAP_NAME,"pedu","right"} ,
  {NULL,SC_MAP_NAME,"pedu","right"}};

Map_struct Mst[2] = {
  {NULL,ST_MAP_NAME,"pedestals","mean"} ,
  {NULL,ST_MAP_NAME,"pedestals","sigma"}};
Map_struct Mst_id[2] = {
  {NULL,ST_MAP_NAME,"pedestals","sigma"} ,
  {NULL,ST_MAP_NAME,"pedestals","sigma"}};

Map_struct Mec[12] = {
  {NULL,EC_MAP_NAME,"inner","u"} ,{NULL,EC_MAP_NAME,"inneru","u"},
  {NULL,EC_MAP_NAME,"inner","v"} ,{NULL,EC_MAP_NAME,"inneru","v"},
  {NULL,EC_MAP_NAME,"inner","w"} ,{NULL,EC_MAP_NAME,"inneru","w"},
  {NULL,EC_MAP_NAME,"outer","u"} ,{NULL,EC_MAP_NAME,"outeru","u"},
  {NULL,EC_MAP_NAME,"outer","v"} ,{NULL,EC_MAP_NAME,"outeru","v"},
  {NULL,EC_MAP_NAME,"outer","w"} ,{NULL,EC_MAP_NAME,"outeru","w"}};


Map_struct Mec_id[12] = {
  {NULL,EC_MAP_NAME,"inner","u"} ,{NULL,EC_MAP_NAME,"inneru","u"},
  {NULL,EC_MAP_NAME,"inner","v"} ,{NULL,EC_MAP_NAME,"inneru","v"},
  {NULL,EC_MAP_NAME,"inner","w"} ,{NULL,EC_MAP_NAME,"inneru","w"},
  {NULL,EC_MAP_NAME,"outer","u"} ,{NULL,EC_MAP_NAME,"outeru","u"},
  {NULL,EC_MAP_NAME,"outer","v"} ,{NULL,EC_MAP_NAME,"outeru","v"},
  {NULL,EC_MAP_NAME,"outer","w"} ,{NULL,EC_MAP_NAME,"outeru","w"}};


Map_struct Mec1[4] = {
  {NULL,EC1_MAP_NAME,"pedestals","left"} ,
  {NULL,EC1_MAP_NAME,"pedu","left"},
  {NULL,EC1_MAP_NAME,"pedestals","right"},
  {NULL,EC1_MAP_NAME,"pedu","right"}};
Map_struct Mec1_id[2] = {
  {NULL,EC1_MAP_NAME,"pedu","right"} ,
  {NULL,EC1_MAP_NAME,"pedu","right"}};


E 2
/* Function Prototypes. */
void decode_arguments(int *argc,char **argv);
void init_crate(Crate_struct *Cr,int crate_count);
D 2
/*void find_first_event(clasHEAD_t **Head);*/
void process_data_file(void);
E 2
I 2

/*sergey void find_first_event(clasHEAD_t **Head);*/
void find_first_event(BOSIO *descriptor, int *runnumber);

void process_data_file(BOSIO *descriptor);
E 2
void update_histogram(Crate_struct *Crate,int Crate_num,
		      int islot,int ichan,int idat);
void creat_spar(Crate_struct *Cr,int CR_num, int rnum);
void creat_sig(Crate_struct *Cr,int CR_num, int rnum);

int put_values_in_map(Map_struct *M, Map_struct *Mid, int maplen, int num_m, int Rnum, char *root_name);
int fill_map(Crate_struct *Cr,int CR_num);
void creat_map(Crate_struct *Cr,int CR_num, int rnum);


	/*
   * For SC:
   *   ind determines the "place" in he bos bank: 0-> TDC left, 1-> ADC left 2->TDC right ...
   *   isd1 is not used.
   *   isd2 points to the paddle number.
   * 
   * The MAP is packed as: M[0] -> Left   M[1] -> Sigma_Left  M[2]-> Right M[3]->Sigma_Right.
   * Index into the map is isec*48+isd2.
   *
   */ 

	    /* The isd1 (layer) determines which {inner,outer}x{u,v,w} plane: 0->Ui 1->Vi ... 5->Wo 
	     * The M struct array has 0->Ui,ped 1->Ui,sigma 2->Vi,ped ....
	     * so  (isd1)*2 converts isd1 to the index in M array for pedestals,
	     * and (isd1)*2+1 for sigmas.
	     *
	     * Within the Map array, we have (isec)*36+isd2 for the entry we want.
	     * 
	     * Notice: isec isd1 isd2 were already corrected with -1.
	     */





E 1
