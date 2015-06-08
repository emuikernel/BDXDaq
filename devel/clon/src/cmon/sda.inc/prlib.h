#ifndef _PRLIB_

#ifdef	__cplusplus
extern "C" {
#endif


#undef SL5

/* prlib.h - header file for prlib package */


/* definitions for SDA related functions - have to be consistent with SDA ! */
/* and with dclib.h - IT IS BAD !!! */
#define nsgmx    60/*600*//*those four increased for Konstantin's test*/
#define nsgcmx   30/*300*/
#define nclmx    100/*1000*/
#define ntrmx    50 /*1000*/
#define nlnkvect 16
#define nlnkec   12
#define NVEC 11

/* */

#define NLAY     6        /* the number of layers in the superlayer */
#define NTRACK   ntrmx    /* the maximum number of tracks in ... */
#define NFOUND   200      /* the maximum number of segments in ... */
#define NS2      142
#define MAX_NPAT 200
#define NLISTAX  256*64*256  /* 22 bits */
#define NLISTST  256*128*128  /* 22 bits */
#define NO_HIT   0

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




/* road structures */


/* for compatibility */
#define MAXNPAT 10
typedef struct linksegm
{
  char  npat[6];
  char  ipat[6][MAXNPAT];
} LINKsegm;




typedef struct
{
  unsigned path : 24;
  unsigned p    : 8; /* for reconstruction only */

  unsigned ntof : 6;
  unsigned nu   : 7;
  unsigned nv   : 7;
  unsigned nw   : 7;
  signed charge : 2;
  unsigned res : 3;

  short x;
  short y;

  char  z;
  char  dx;
  char  dy;
  char  dz;

#ifdef SIM
  unsigned short psim;
  unsigned short np;

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

void l3config(char *);
void l3init(int runno);
int  l3lib(int *jw, int *ifail);

void l3_reset();
void l3_init(int runno);
int  l3_event(int *jw, int *keepit);

#ifdef	__cplusplus
}
#endif

#define _PRLIB_
#endif







