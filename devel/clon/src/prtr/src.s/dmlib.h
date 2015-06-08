#ifndef _DMLIB_

#ifdef	__cplusplus
extern "C" {
#endif

/* dmlib.h - header file for data monitor library */

#define NPARAM  10
#define NAMLEN  20
#define TITLEN  80
#define LISTLEN 1000
#define NSETS   10000



/***************************************************/
/* =================== sets ====================== */
/* sets described in *.dmcfg files; every set will */
/* generate (at least) one EPICS name              */

/* single bank */
typedef struct dmbank
{
  char banktype[NAMLEN];
  char bankname[NAMLEN];
  int  bankid;
  int  par[NPARAM];
} DMBANK;

/* set of banks belong the same set */
typedef struct dmset
{
  char         title[TITLEN];
  char         sname[NAMLEN];
  char         snorm[NAMLEN];
  unsigned int count;
  int          nbanks;
  DMBANK      *banks;
} DMSET;




/*****************/
/*****************/
/* new sctucture */

typedef struct dmlowids
{
  short nsets; /* the number of sets contains that bank */
  short *setid; /* list of indexes of sets contains that bank */
} DMLOWIDS;

typedef struct dmhighids
{
  /*int nentries;*/
  DMLOWIDS *lowid;
} DMHIGHIDS;

typedef struct dmbanknums
{
  /*int nentries;*/
  DMHIGHIDS *highid;
} DMBANKNUMS;

/* will be an array, not calloced, we have about 200 of them */
typedef struct dmbankids
{
  /*int nentries;*/
  DMBANKNUMS *banknum;
} DMBANKIDS;

/*****************/
/*****************/







/* DC structures */

typedef struct adbcrate
{
  int number;
  int sector;
  int layer;
  int wire;
} ADBCRATE;

typedef struct adbboard
{
  int parent;
  int number;
  int sector;
  int layer;
  int wire;
} ADBBOARD;

typedef struct tdccrate
{
  int number;
  int sector;
  int layer;
  int wire;
} TDCCRATE;

typedef struct tdcboard
{
  int parent;
  int number;
  int sector;
  int layer;
  int wire;
} TDCBOARD;

typedef struct lvcrate
{
  int number;
  int sector;
  int layer;
  int wire;
} LVCRATE;

typedef struct lvfuse
{
  int parent;
  int number;
  int sector;
  int layer;
  int wire;
} LVFUSE;






/*********************************************/
/* ========== function prototypes ========== */

int  dmparse(char *fname, int iset);
int  dmconfig(char *dir);
void dmlib(int *jw);


#ifdef	__cplusplus
}
#endif

#define _DMLIB_
#endif
