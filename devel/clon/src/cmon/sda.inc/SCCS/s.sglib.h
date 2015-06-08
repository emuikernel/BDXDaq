h15660
s 00002/00000/00027
d D 1.4 07/10/26 14:18:21 boiarino 5 4
c bla
e
s 00001/00069/00026
d D 1.3 02/04/30 15:46:43 boiarino 4 3
c more some stuff to sgutil.h and include it
c 
e
s 00054/00002/00041
d D 1.2 02/04/30 13:43:37 boiarino 3 1
c new version
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sglib.h
e
s 00043/00000/00000
d D 1.1 01/11/19 19:00:39 boiarino 1 0
c date and time created 01/11/19 19:00:39 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _SGLIB_

/* sglib.h */

#ifdef	__cplusplus
extern "C" {
#endif

I 3
D 4
/* Is 'long long' has swaped long inside ??? */
E 4
I 4
#include "sgutil.h"
E 4

E 3
D 4
typedef struct word128 *Word128Ptr;
typedef struct word128
{
D 3
  unsigned long long words[2]; /* words[0] is least significant */
E 3
I 3
  unsigned long words[4]; /* words[0] is least significant */
E 3
} Word128;

typedef struct word192 *Word192Ptr;
typedef struct word192
{
  unsigned long words[6]; /* words[0] is least significant */
} Word192;

typedef struct dc0data  *DC0DataPtr;
typedef struct dc0data
{
  unsigned short id; /* wire - layer are bytes stuffed into id */
  unsigned short tdc;
} DC0Data;

typedef enum {False, True} Boolean;
I 3
typedef unsigned int (*UIFUNCPTR) ();
E 3

I 3

/* structures for the segment finding algorithms */

#define NGAP  7 /* +/- in stereo shift */
#define NHL  20 /* # hits per layer */
#define NCR  20 /* # clusters per region */
#define NLAY  6 /* max number of layers in superlayer */

typedef struct prla
{
  int   nhit;
  short iw[NHL];
  short tdc[NHL];
} PRLA;

typedef struct prsl
{
  int  iwmin;
  int  iwmax;
  int  ird;      /* wire # in layer 3 to be used in road finding
                    (can be missing in la[] !!!) */
  PRLA la[NLAY];
} PRSL;

typedef struct prrg
{
  UIFUNCPTR axmin;
  UIFUNCPTR stmin;
  UIFUNCPTR rgmin;
  /*for example:*/
  /*UIFUNCPTR fun = clust->axmin;*/
  /*printf("%d - %d\n",e2,fun(e0,e1,e2));*/
  int  nax;
  int  nst; /* if nst<0 then nax=# of pairs */
  PRSL ax[NCR];
  PRSL st[NCR];
} PRRG; /* segments for one region */

typedef struct prsec
{
  PRRG rg[3];
} PRSEC; /* segments for one sector */




E 4
E 3
/* function prototypes (only these will be called from outside) */

void sginit();
D 3
void sglib(int *jw, int isec, int *ifail);
E 3
I 3
void sginitfun(PRSEC sgm);
void sglib(int *jw, int isec, int opt, int *nsgm, PRSEC *sgm);
void sgprint(PRSEC *sgm);
void sgroad(PRSEC *sgm, int ncl[3], unsigned char ird[3][2][nclmx]);
E 3

I 5
int sgtrigger(int *jw, int isec, int nsl, int layershifts[6]);
int sgremovenoise(int *jw, int isec, int nsl, int layershifts[6]);
E 5

#ifdef	__cplusplus
}
#endif

#define _SGLIB_
#endif


E 1
