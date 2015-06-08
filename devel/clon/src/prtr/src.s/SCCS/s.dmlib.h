h63593
s 00019/00000/00135
d D 1.4 10/07/24 15:49:55 boiarino 4 3
c *** empty log message ***
e
s 00026/00002/00109
d D 1.3 10/07/18 23:44:14 boiarino 3 2
c *** empty log message ***
e
s 00047/00016/00064
d D 1.2 10/07/18 18:15:50 boiarino 2 1
c *** empty log message ***
e
s 00080/00000/00000
d D 1.1 07/06/28 23:24:33 boiarino 1 0
c date and time created 07/06/28 23:24:33 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _DMLIB_

#ifdef	__cplusplus
extern "C" {
#endif

/* dmlib.h - header file for data monitor library */

#define NPARAM  10
D 2
#define NAMLEN  10
E 2
I 2
#define NAMLEN  20
E 2
#define TITLEN  80
#define LISTLEN 1000
I 2
#define NSETS   10000
E 2



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
D 2
  unsigned int count;
E 2
  char         title[TITLEN];
  char         sname[NAMLEN];
  char         snorm[NAMLEN];
I 2
  unsigned int count;
E 2
  int          nbanks;
  DMBANK      *banks;
} DMSET;




D 2
/**********************************************************/
/* =============== lookup table tree ==================== */
/* index = tree[bankid*bank#].setid[high_id*low_id*place] */
/*   where index = 0 or set_index(in static global array) */
/* do sets[index].count ++; */
E 2
I 2
/*****************/
/*****************/
/* new sctucture */
E 2

D 2
typedef struct dmindex
E 2
I 2
typedef struct dmlowids
E 2
{
D 2
  int          nsets; /* the number of sets contains that bank */
  int         *setid; /* list of indexes of sets contains that bank */
} DMHLPL;
E 2
I 2
D 3
  int nsets; /* the number of sets contains that bank */
  int *setid; /* list of indexes of sets contains that bank */
E 3
I 3
  short nsets; /* the number of sets contains that bank */
  short *setid; /* list of indexes of sets contains that bank */
E 3
} DMLOWIDS;
E 2

D 2
typedef struct dmtree
E 2
I 2
typedef struct dmhighids
E 2
{
D 2
  unsigned int nama;  /* bank name as 4-byte integer */
  int          iset;  /* current set number */
  DMHLPL      *hlpl;
} DMTREE;
E 2
I 2
  /*int nentries;*/
  DMLOWIDS *lowid;
} DMHIGHIDS;
E 2

I 2
typedef struct dmbanknums
{
  /*int nentries;*/
  DMHIGHIDS *highid;
} DMBANKNUMS;
E 2

I 2
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

I 4
typedef struct adbboard
{
  int parent;
  int number;
  int sector;
  int layer;
  int wire;
} ADBBOARD;

E 4
I 3
typedef struct tdccrate
{
  int number;
  int sector;
  int layer;
  int wire;
} TDCCRATE;
E 3

I 4
typedef struct tdcboard
{
  int parent;
  int number;
  int sector;
  int layer;
  int wire;
} TDCBOARD;

E 4
I 3
typedef struct lvcrate
{
  int number;
  int sector;
  int layer;
  int wire;
} LVCRATE;

typedef struct lvfuse
{
I 4
  int parent;
E 4
  int number;
  int sector;
  int layer;
  int wire;
} LVFUSE;


E 3

E 2



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
E 1
