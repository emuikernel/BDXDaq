#ifndef TT_H
#define TT_H

/* tt.h */

#ifndef CLON_PARMS
#define CLON_PARMS "/usr/local/clas/parms"
#endif

#ifndef TCL_SOURCE_DIR
#define TCL_SOURCE_DIR "/home/clasrun/rol"
#endif

static char* TTDir = CLON_PARMS"/TT";

#define MSG(f)  \
printf("%s at line %i : " f ,__FILE__,__LINE__)
#define MSG1(f,p1) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1)
#define MSG2(f,p1,p2) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2)
#define MSG3(f,p1,p2,p3) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3)
#define MSG4(f,p1,p2,p3,p4) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4)
#define MSG5(f,p1,p2,p3,p4,p5) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4,p5)
#define MSG6(f,p1,p2,p3,p4,p5,p6) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4,p5,p6)
#define MSG7(f,p1,p2,p3,p4,p5,p6,p7) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4,p5,p6,p7)
#define MSG8(f,p1,p2,p3,p4,p5,p6,p7,p8) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4,p5,p6,p7,p8)
#define MSG9(f,p1,p2,p3,p4,p5,p6,p7,p8,p9) \
printf("%s at line %i : " f ,__FILE__,__LINE__,p1,p2,p3,p4,p5,p6,p7,p8,p9)

/********* Translation table **********/


typedef struct 
{
  unsigned long  wannaraw;                         /* do we need raw banks ?                */
  unsigned long  profile;                          /* do we need profiling banks ?          */
  unsigned long  notrans;                          /* do we need profiling banks ?          */
  unsigned long  roc;                              /* roc#                                  */
  unsigned long  statname;                         /* statistic bank name                   */
  unsigned long  nstatwd;                          /* the number of statistic words         */
  unsigned long  type  [NUM_SLOTS];		           /* board type                            */
  unsigned long  nplace[NUM_SLOTS]   [MAXCHANNEL]; /* the number of places for BOS bank row */
  unsigned long  idnr  [NUM_SLOTS]   [MAXCHANNEL]; /* BOS bank idnr                 */
  unsigned long  name  [NUM_SLOTS]   [MAXCHANNEL]; /* BOS bank name array                   */

  unsigned short nr    [NUM_SLOTS]   [MAXCHANNEL]; /* BOS bank number array                 */
  unsigned short place [NUM_SLOTS]   [MAXCHANNEL]; /* place of dataword in BOS Bank         */

  unsigned short id    [NUM_SLOTS]   [MAXCHANNEL]; /* id array for not DC                   */
  unsigned short id2   [NUM_SLOTS][2][MAXCHANNEL]; /* id array for DC only                  */

  unsigned short *partner[MAXID];                  /* Partner array for not DC              */
  unsigned short ids    [MAXID];                   /* Partner array for not DC              */
} TTS;
typedef TTS *TTSPtr;

typedef int (*TrFun) (long *, long *, TTSPtr);

typedef struct 
{
  unsigned long  nm;  /* BOS Bank name                 */
  unsigned long  nr;  /* BOS Bank number               */
  unsigned long  sd1; /* SubDivision 1                 */
  unsigned long  sd2; /* SubDivision 2                 */
  unsigned long  pl;  /* Place of data in BOS Bank row */
  unsigned long  st;  /* Status of chanell             */
  unsigned long  roc; /* ROC number                    */
  unsigned long  sl;  /* Slot number                   */
  unsigned long  typ; /* Type of slot                  */
  unsigned long  chn; /* Channel                       */
} TTRow;
typedef TTRow *TTRPtr;


/* PMC stuff */
#ifdef VXWORKS
#ifdef VXWORKSPPC

typedef struct 
{
  volatile long   dataReady;
  volatile long   *bufin;
  volatile long   lenin;
  volatile long   *bufout;
  volatile long   lenout;
} TTPMC;
typedef TTPMC *TTPMCPtr;

#endif
#endif



/******* Workaround of problem with stdio.h in CRL *********/
#ifdef WORKAROUND_CRL
typedef void  FILE ;
#endif 
/********************* Function prototipes *****************/

TTSPtr  TT_LoadROCTT(int, TTSPtr);
void    TT_Prestart(int pid);
void    TT_Go();
int     TT_Main(long *, long *, int);
int     TT_MakeRawBank(long *, long *);
int     TT_MakeDCStatBank(long *, RCST *);
void    TT_Prestart_TranslateDCBank();

int     TT_TranslateDCBank(long *, long *, TTSPtr);
int     TT_TranslateTAGEBank(long *, long *, TTSPtr);
int     TT_TranslateTLV1Bank(long *, long *, TTSPtr);
int     TT_TranslateVMEBank(long *, long *, TTSPtr);
int     TT_TranslateVMETAGEBank(long *, long *, TTSPtr);
int     TT_TranslateVMEF1Bank(long *, long *, TTSPtr);
int     TT_TranslateNonDCBank(long *, long *, TTSPtr);
int     TT_TranslateTPCBank(long *, long *, TTSPtr);
int     TT_MakeVmeBank(long *, long *, TTSPtr);
int     TT_TranslateFADCBank(long *, long *, TTSPtr ttp);
int     TT_TranslatePCALFBBank(long *, long *, TTSPtr);
int     TT_TranslateHPSBank(long *, long *, TTSPtr);
int     TT_TranslateSVTBank(long *, long *, TTSPtr);
int     TT_TranslateDCRBBank(long *, long *, TTSPtr);
int     TT_TranslateDCRBGTPBank(long *, long *, TTSPtr);
int     TT_TranslateTDCBank(long *, long *, TTSPtr);

int     TT_DC_demux(unsigned short **, MTDCPtr, int, TTSPtr);
int     TT_DC_removenoise(long *iw, int ind, int len);

int     TT_ReadRaw(long *, int);

void    TT_PrintBufOut(int, long *);

int  TT_FPRdRec(int, int **, int *, int *);
int  TT_FPNLink(int *, int, int ) ;
void TT_PrintFPACKLogRec(int *, int, TTSPtr *);
void TT_PrintFPACKRecHead(int *p);
void TT_PrintFPACKDatHead(int *p);

void TT_PrintROCBuffer (int, long*, TTSPtr *);

void TT_PrintBankBody(int, int, int, int, int, int *, TTSPtr *);
void TT_PrintB16Bank (int, int, int, unsigned short *);
void TT_PrintHexBank (int, int, void *);
void TT_PrintHeadBank(int *);
void TT_PrintProfileBank(int , void *);
void TT_PrintDCStatBank(int , void *);
void TT_PrintRawBank (int *, int, TTSPtr);
void TT_PrintHead(int *, TTSPtr);
void TT_Print1881(int *, TTSPtr);
void TT_Print1877(int *, TTSPtr);
void TT_Print1872(int *, TTSPtr);

void TT_SetDumpForm (int );
void TT_SetDumpWidth (int );

int TT_CheckROCBOSDatHead(int *p);
int TT_CheckFPACKDatHead(int *p);
/*int checkpartner(void);*/

/********************* Profiler stuff *****************/

typedef struct {
  double mean,sigma;
  unsigned long   i;
} TMean;

typedef struct {
  long signature ;
  long entryes   ;
  long underflow ;
  long overflow  ;
  long nch       ;
  double min     ; 
  double max     ;
  double step    ;
  TMean *mean    ;
} THist;

double *PRF_init (int) ;
double  PRF_store(int) ;
double  PRF_store(int nt) ;
TMean  *PRF_mbook(void ) ;
int     PRF_mfill(TMean*, double) ;
THist  *PRF_hbook1(int, double, double) ;
int     PRF_hfill1(THist *, double, double) ;
void    PRF_hdump(THist *, char* ) ;
int     PRF_h2rocbos(THist *, long *, int, int) ;

extern double PRF_CallOverhead;
extern double PRF_Resolution;

/********************* Profiler stuff *****************/


#define TT_INTFORMAT 0
#define TT_HEXFORMAT 1
#define MAXROC 17


#define DBG if (0) printf


#define WANNARAW  ttp->wannaraw
#define PROFILE   ttp->profile
#define ROC       ttp->roc
#define TT_IDNR   ttp->idnr
#define NAME      ttp->name
#define NR        ttp->nr
#define ID        ttp->id
#define SLTYPE    ttp->type
#define NBOSCOL   ttp->nplace
#define PLACE     ttp->place

#define CHANNEL   fb->channel
#define SLOT      fb->slot




#endif
