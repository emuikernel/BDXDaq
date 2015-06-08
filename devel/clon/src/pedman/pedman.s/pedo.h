
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

/*------------cuts on the pedestal distribution ----*/
#define MIN 100
#define MAX 800
#define SC_MAP_NAME   "sc.trans"
#define EC_MAP_NAME   "ec.trans"
#define CC_MAP_NAME   "cc.trans"
#define PS_MAP_NAME   "ps.trans"
#define PC_MAP_NAME   "pc.trans"
#define TC_MAP_NAME   "tac.trans"
#define EC1_MAP_NAME  "lac.trans"
#define ST_MAP_NAME   "st.trans"   /* old start counter */
#define STN_MAP_NAME  "stn.trans"  /* new start counter */

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
  ROC_DMUX_t *RC_look; /* Pointer to lookup table from demux routines. */
} Crate_struct;

#define CRATE_COUNT 5   /* Tables for 5 crates. */


/* Function Prototypes. */
void decode_arguments(int *argc,char **argv);
void init_crate(Crate_struct *Cr);

/*sergey void find_first_event(clasHEAD_t **Head);*/
void find_first_event(BOSIO *descriptor, int *runnumber);

void process_data_file(BOSIO *descriptor);
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





