h56497
s 00004/00004/00253
d D 1.9 07/10/21 23:05:58 boiarino 10 9
c #define nsgmx    600/*60*//*those four increased for Konstantin's test*/
c #define nsgcmx   300/*30*/
c #define nclmx    1000/*100*/
c 
e
s 00001/00001/00256
d D 1.8 07/04/18 12:12:23 boiarino 9 8
c #define ntrmx  1000
c 
e
s 00013/00010/00244
d D 1.7 02/05/13 13:45:18 boiarino 8 7
c #define NTRACK   ntrmx 
c 
e
s 00000/00008/00254
d D 1.6 02/05/01 00:07:37 boiarino 7 6
c remove PRevent definition
c 
e
s 00008/00000/00254
d D 1.5 02/04/30 23:54:27 boiarino 6 5
c add:
c /* for coda_prtr.c only ??? */
c typedef struct prevent
c {
c   UThistf *histf; /* histograms */
c   int     ntrk;
c   PRTRACK *trk;
c } PRevent;
c 
e
s 00007/00088/00247
d D 1.4 02/04/30 13:41:47 boiarino 5 4
c new segment finding version
c 
e
s 00005/00001/00330
d D 1.3 02/03/26 15:15:10 boiarino 4 3
c *** empty log message ***
e
s 00010/00000/00321
d D 1.2 01/12/19 15:00:09 boiarino 3 1
c add
c 
c typedef struct prevent
c {
c   UThistf *histf; /* histograms */
c   int     ntrk;
c   PRTRACK *trk;
c } PRevent;
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/prlib.h
e
s 00321/00000/00000
d D 1.1 01/11/19 19:00:36 boiarino 1 0
c date and time created 01/11/19 19:00:36 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _PRLIB_

I 3
D 5
#include "uthbook.h"

E 5
E 3
#ifdef	__cplusplus
extern "C" {
#endif


#undef SL5

/* prlib.h - header file for prlib package */

D 8
#define NLAY     6        /* the number of layers in the superlayer */
#define NTRACK   500      /* the maximum number of tracks in ... */
#define NFOUND   200      /* the maximum number of segments in ... */
#define NS2      142
#define MAX_NPAT 200
#define NLISTAX  256*64*256  /* 22 bits */
#define NLISTST  256*128*128  /* 22 bits */
#define NO_HIT   0
E 8

D 8
/* definitions for SDA related functions - have to be consistent with SDA !!! */

E 8
I 8
/* definitions for SDA related functions - have to be consistent with SDA ! */
/* and with dclib.h - IT IS BAD !!! */
E 8
D 10
#define nsgmx  60
#define nsgcmx 30
#define nclmx  100
D 9
#define ntrmx  50
E 9
I 9
#define ntrmx  1000 /*50*/
E 10
I 10
#define nsgmx    600/*60*//*those four increased for Konstantin's test*/
#define nsgcmx   300/*30*/
#define nclmx    1000/*100*/
#define ntrmx    1000 /*50*/
E 10
E 9
#define nlnkvect 16
#define nlnkec   12
#define NVEC 11

I 8
/* */

#define NLAY     6        /* the number of layers in the superlayer */
#define NTRACK   ntrmx    /* the maximum number of tracks in ... */
#define NFOUND   200      /* the maximum number of segments in ... */
#define NS2      142
#define MAX_NPAT 200
#define NLISTAX  256*64*256  /* 22 bits */
#define NLISTST  256*128*128  /* 22 bits */
#define NO_HIT   0

E 8
/* accuracy */

#define VECT1 10.         /* x -  1 mm */
#define VECT2 10.         /* y -  1 mm */
#define VECT3 1.          /* z -  1 cm */
#define VECT4 100.        /* dx - 1 % */
#define VECT5 100.        /* dy - 1 % */
#define VECT6 100.        /* dz - 1 % */
#define VECT7 1000.       /* p -  1 MeV/c */

#define P0    0.02        /* GeV/c */
#define DP    0.025       /* 2.5 % (acctual PR resolution about 8-11 %) */

D 5
/* segment structures */
E 5

D 5
typedef struct segmdict
{
  int   ipat[6];
} SEGMdict;
E 5

D 5
typedef struct
{
  unsigned nwhit    : 2;
  unsigned sign0    : 1;
  unsigned iw0      : 4;
  unsigned sign1    : 1;
  unsigned iw1      : 4;
  unsigned sign2    : 1;
  unsigned iw2      : 4;
  unsigned sign3    : 1;
  unsigned iw3      : 4;
  unsigned sign4    : 1;
  unsigned iw4      : 4;
  unsigned sign5    : 1;
  unsigned iw5      : 4;
} DCsegm;
E 5

D 5
/* could we use that ???
typedef struct
{
  signed nwhit : 2;
  signed iw0   : 5;
  signed iw1   : 5;
  signed iw2   : 5;
  signed iw3   : 5;
  signed iw4   : 5;
  signed iw5   : 5;
} DCsgm;
typedef struct
{
  signed nwhit : 2;
  signed iw[6] : 5;
} DCsgm;
*/


E 5
/* road structures */

D 5
#define MAXNPAT 10
E 5

I 5
/* for compatibility */
#define MAXNPAT 10
E 5
typedef struct linksegm
{
  char  npat[6];
  char  ipat[6][MAXNPAT];
} LINKsegm;




D 5

E 5
typedef struct
{
  unsigned path : 24;
  unsigned p    : 8; /* for reconstruction only */

D 5
  union
  {
    struct
    {
      unsigned p  : 16; /* for simulation only */
      unsigned np : 14;
      signed charge : 2;
    } s;
    struct
    {
      unsigned gr0  : 5;
      unsigned gr1  : 5;
      unsigned gr2  : 5;
      unsigned gr3  : 5;
      unsigned gr4  : 5;
      unsigned gr5  : 5;
      signed charge : 2;
    } r;
  } u;

E 5
  unsigned ntof : 6;
  unsigned nu   : 7;
  unsigned nv   : 7;
  unsigned nw   : 7;
D 5
  unsigned res  : 5;
E 5
I 5
  signed charge : 2;
  unsigned res : 3;
E 5

  short x;
  short y;

  char  z;
  char  dx;
  char  dy;
  char  dz;

#ifdef SIM
I 5
  unsigned short psim;
  unsigned short np;

E 5
  char  npat[6];
  char  ipat[6][MAXNPAT];
#endif

} DCroad;




typedef struct
{
  int           nroads;
  unsigned char min[4];
  unsigned char max[4];
  DCroad        *road;
} DCstereo;
typedef DCstereo DCaxial;






/* track structures */

#define MINHITSEGM 3
#define NHL 20

typedef struct eccluster
{
  float u;
  float v;
  float w;
  float e;
} ECCLUSTER;

D 5
typedef struct prsegment
{
  int   nw;
  short iw[6];
  short tdc[6];
} PRSEGMENT;



E 5
typedef struct prlayer
{
  int   nhit;
  short iw[NHL];
  short tdc[NHL];
} PRLAYER;

typedef struct prcluster
{
  int       iwmin;
  int       iwmax;
  PRLAYER   layer[6];

} PRCLUSTER;

typedef struct prtrack
{
  int       sector;
  PRCLUSTER cluster[6];
  int       ntrk;
  float     vect[6];
  float     p;
  int       charge;
  int       ntof;
  int       nu;
  int       nv;
  int       nw;
  ECCLUSTER ec[3];
} PRTRACK;


I 6
D 7
/* for coda_prtr.c only ??? */
typedef struct prevent
{
  UThistf *histf; /* histograms */
  int     ntrk;
  PRTRACK *trk;
} PRevent;
E 7
E 6
I 3
D 5
typedef struct prevent
{
  UThistf *histf; /* histograms */
  int     ntrk;
  PRTRACK *trk;
} PRevent;
E 5

I 6
D 7

E 7
E 6
D 5

E 3
/* info about tracks in current sector - for run time only */
typedef struct sectrk
{
  int ntrack_firstinsector;
} SECTRK;


E 5
/* used in dcdsect.c now; input: isec, output: dcndig[], dcdigi[][][] */
#define GETDCDATA \
  nd = 0; \
  {int il; for(il=0; il<36; il++) dcndig[il] = 0;} \
  if((ind = etNlink(jw,"DC0 ",isec)) > 0) \
  { \
    int is, id, nd2, minw, maxw, itdc, ih, i32[7000]; \
    short dcid, *i16; \
    /* Gates for Sep. Runs:     .5 mics      1 mics      2 mics */ \
    static int min_tdc[6] = { 1000, 1000, 2000, 2000, 2000, 2000 }; \
    static int max_tdc[6] = { 2500, 2500, 5000, 5000, 7000, 7000 }; \
    /* Exchanged signal cables in DC Sector1 Region2: */ \
    /* dctt(n) <-> dctt(16+n) */ \
    static short dctt[32] = \
              { 3441, 3442, 3443, 3697, 3698, 3953, 3954, 3955, \
                4209, 4210, 4465, 4466, 4467, 4721, 4722, 4723, \
                4981, 4982, 4983, 5237, 5238, 5493, 5494, 5495, \
                5749, 5750, 6005, 6006, 6007, 6261, 6262, 6263 }; \
    static int NWL[6] = { 130, 142, 192, 192, 192, 192 }; /* max wire */ \
    i16 = (short *) i32; \
    if((nd=etNdata(jw,ind)) > 0) \
    { \
      if(nd > 6528) printf(" EVIN: Nw/sect > 6528 and is %d\n",nd); \
      for(j=1; j<=nd; j++) i32[j-1] = jw[ind+j-1]; \
      nd2 = 2*nd; \
      for(i=1; i<=nd2; i+=2) \
      { \
        id = i16[i-1] & 0xff; \
        il = (i16[i-1] & 0x7f00) >> 8; \
        is   = (il - 1)/6 + 1; \
        itdc = i16[i+1-1]; \
        if(il < 1 || il > 36) continue; \
        /* old way */ \
        if(id < 1 || id > NWL[is-1]) continue; \
        /* new way - cannot use in PRLIB, there is no sdageom there ! */ \
        /*minw = sdageom_.pln[isec-1][il+3-1][14]; \
        maxw = sdageom_.pln[isec-1][il+3-1][15]; \
        if(id < minw || id > maxw) \
        { \
          printf("GETDCDATA: Wire# out of range: il=%d iw=%d\n",il,id); \
          continue; \
        }*/ \
        /* Exchanged signal cable in Sector1 Region2 Wires 113-119 (F.K.) */ \
        if(isec == 1 && (is+1)/2 == 2 && id>112 && id<120) \
        { \
          dcid = i16[i-1]; \
          for(j=1; j<=32; j++) \
          { \
            if(dcid == dctt[j-1]) \
            { \
              if(j > 16) \
                dcid = dctt[j-16-1]; \
              else \
                dcid = dctt[j+16-1]; \
              break; \
            } \
          } \
          id = dcid & 0xff; \
          il = (dcid & 0x7f00) >> 8; \
          is  = (il + 5)/6; \
        } \
        /* end of exchange cable */ \
        if(itdc <= min_tdc[is-1] || itdc > max_tdc[is-1]) continue; \
        if(dcndig[il-1] >= 192) continue; \
        dcndig[il-1]++; \
        ih = dcndig[il-1]; \
        dcdigi[il-1][ih-1][0] = id; \
        dcdigi[il-1][ih-1][1] = itdc; \
      } \
    } \
  }



/* functions */

void prinit(char *filename, int lanal1, int lanal2, int lanal5);
int  prlib(int *iw, PRTRACK *track);
int  eclib(int *iw, const float threshold[3], int *ntrk, PRTRACK *trk);
int  prbos(int *iw, int *ntrk, PRTRACK *trk);

D 4
int  l3init(int run);
E 4
I 4
void l3config(char *);
void l3init(int runno);
E 4
int  l3lib(int *jw, int *ifail);

I 4
void l3_reset();
void l3_init(int runno);
int  l3_event(int *jw, int *keepit);
E 4

#ifdef	__cplusplus
}
#endif

#define _PRLIB_
#endif







E 1
