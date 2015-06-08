#ifndef _DCLIB_


#include "uthbook.h"


#ifdef	__cplusplus
extern "C" {
#endif

/* dclib.h - header file for dclib package */

/*
 Magnetic field Table calculated for toroidal magnet with six coils
 (Table is calculated in a box which cover only a half of sector)
                                 Bogdan Niczyporuk, March 1991
*/
#define Ng     1000000
#define Imax_t 3860
#define Imax_m 8000
#define Imax_p 124
typedef struct bgrid
{
  int     NX;  /* | the number of bins  */
  int     NY;  /* | for each coordinate */
  int     NZ;  /* | in DST Lab system   */
  float   Xmin;   /* | Defines edges  */
  float   Ymin;   /* | of the box in  */
  float   Zmin;   /* | DST Lab system */
  float   DX;   /* | the number of bins  */
  float   DY;   /* | for each coordinate */
  float   DZ;   /* | in DST Lab system   */
  int     Cmax;   /* maximum current [A] in a loop for big or mini Torus */
  int     dtime;  /* unix time */
  int     Ngr;    /* 3*NX*NY*NZ - the number of grids */
  float   Q[Ng];
} SDAbgrid;

typedef struct poltg
{
  int     MX;  /* | the number of bins  */
  int     MY;  /* | for each coordinate */
  int     MZ;  /* | in DST Lab system   */
  float   Xpmin;   /* | Defines edges  */
  float   Ypmin;   /* | of the box in  */
  float   Zpmin;   /* | DST Lab system */
  float   DDX;   /* | the number of bins  */
  float   DDY;   /* | for each coordinate */
  float   DDZ;   /* | in DST Lab system   */
  int     Cpmax;   /* maximum current [A] in a loop for big or mini Torus */
  int     dptime;  /* unix time */
  int     Mgr;    /* 3*NX*NY*NZ - the number of grids */
  float   Qm[Ng];
} POLTG;

/* piece from sdageom.h */
#define npln    46
#define npl_st  2
#define npl_dc  36
#define npl_cc  1
#define npl_ec  2
#define nst_min 2
#define nst_max ((nst_min)+npl_st-1)
#define ndc_min ((nst_max)+1)
#define ndc_max ((ndc_min)+npl_dc-1)
#define ncc_min ((ndc_max)+1)
#define ncc_max ((ncc_min)+npl_cc-1)
#define nsc_min ((ncc_max)+1)
#define nsc_max ((nsc_min)+npl_sc-1)
#define nec_min ((nsc_max)+1)
#define nec_max ((nec_min)+npl_ec-1)
#define mxstep  10000
/* end of piece from sdageom.h */


/* for sclib */
typedef struct dcscid
{
  int slab;    /* SC Slab Number track "itr" points to */
  int plane;   /* SC Plane Number track "itr" points to (SDA plane !) */
  int hit;     /* index 'index' in scdigi[index-1][5] (see sclib.c) */
  int tdcl;
  int adcl;
  int tdcr;
  int adcr;
} DCscid;

/* must be consistent with prlib.h - IT IS BAD !!! */
#define nsgmx    60/*600*//*those four increased for Konstantin's test*/
#define nsgcmx   30/*300*/
#define nclmx    100/*1000*/
#define ntrmx    50 /*1000*/
#define nplane   46
#define nlnkvect 16
#define nlnkec   12
#define nhlmx    20  /* =NHL-max number of hits in one cluster in one layer */


/* DC track info for particular layer & particular track */

typedef struct dctrk
{
  float iw;      /* Wire number                                 {L=2,3} */
  float tdc;     /* Raw TDC [cnts]                              {L=2,3} */
  float Td;      /* Drift time in a cell [ns]                   {L = 3} */
  float Dm;      /* Measured distance of closest approach [cm]  {L = 3} */
  float SigDm;   /* Sigma of distance of closest approach [cm]  {L=2,3} */
  float beta;    /* Particle velocity [v/c]                     {L = 3} */
  float Df;      /* Fitted distance of closest approach [cm]    {L=2,4} */
  float s;       /* Track length [cm] from origin to this layer {L=2,4} */
  float Wlen;    /* Wire length from the hit to PreAmp [cm]     {L=2,4} */
  float alfa;    /* Track angle relative to the R of SL [rad]   {L=2,4} */
  float sector;  /* 10*isector + Level                          {L=2,4} */
  float status;  /* Status of the hit in this layer             {L=2,3} */
} DCtrk;

/* Track point info for particular plane (1st plane is
   vertex plane and last is EC plane) & particular track */

typedef struct dctrkp
{
  float x;       /* x |                                                 */
  float y;       /* y |--> Track point coordinates [cm]         {L=2,4} */
  float z;       /* z |                                                 */
  float Cx;      /* Cx |                                                */
  float Cy;      /* Cy |--> Track direction (unit vector)       {L=2,4} */
  float Cz;      /* Cz |                                                */
  float p;       /* Particle momentum [GeV]                     {L=2,4} */
  float beta;    /* Particle velocity [v/c]                     {L = 3} */
  float q;       /* Charge [units: -1, 0, +1]                   {L = 2} */
  float s;       /* Track length [cm] from origin to this plane {L=2,4} */
  float sector;  /* 10*isector + Level                          {L=2,4} */
  float chisq;   /* Reduced Chi-Square value                    {L=2,4} */
} DCtrkp;

typedef struct dcecid
{
  int     u;       /* U Slab Number track "itr" points to */
  int     v;       /* V Slab Number track "itr" points to */
  int     w;       /* W Slab Number track "itr" points to */
} DCecid;

typedef struct dctrack
{
  int     level;          /* level of analysis for this track */
  int     sect;           /* sector number of this track */
  float   vect[nlnkvect]; /* parameter prediction from PR */
  DCscid  scid;           /* */
  DCecid  ecid;           /* */
  DCtrk   trk[36];        /* */
  DCtrkp  trkp[nplane];   /* */
} DCtrack;

/* former ev__out[][] - not in use yet !!! */
typedef struct trtrack
{
  float vect[7]; /* vertex X - Set to zero (since we do not measure it) */
                 /* vertex Y - Signed distance to the Z-axis [cm] */
                 /* vertex Z - Vertex position along the Z-axis [cm] */
                 /* | */
                 /* | Direction (unit vector) of the track at target */
                 /* | */
                 /* Momentum of a track [GeV] */

  float beta;    /* Track velocity [v/c] (set to zero for Level = 2) */
  float charge;  /* Charge of a track [-1, 0, +1] */
  float itrk;    /* index of corresponding track in DCtrack structure */
  float status;  /* 10*Sector+Level */
  float chi2;    /* Reduced Chi-Square value */

  /* do we need it ? in this form ? */
  float ccevt[8]; /* x,y,z,Cx,Cy,Cz,s,CCid <- CC-plane (cyl.) */
  float scevt[8]; /* x,y,z,Cx,Cy,Cz,s,SCid <- SC-plane */
  float ecevt[8]; /* x,y,z,Cx,Cy,Cz,s,Uid+100*Vid+10000*Wid <- EC-plane */

} TRtrack;

typedef struct trstat
{
  int ack[10];
  int dtype[10];
  int type[10];
  char caption[10][25];
} TRstat;

#define TAG_MAX_HITS 40 /* size of output tglib array*/

typedef struct trevent
{
  int     itrk0; /* Electron (or other if photoproduction) track# {L = 2} */
  float   beta0; /* Particle velocity of track "itrk0"            {L = 2} */

  int     itag0;
  int     ntagh;
  float   tag_res[TAG_MAX_HITS][7];

  int     ntrack;   /* Number of the Track Candidates (linked clusters) */
  DCtrack track[ntrmx];

  TRstat  stat; /* statistic */
  UThistf *histf; /* histograms */



  int     ntr_out;
  float   ev_out[ntrmx][36];

  int     nevnt;
  TRtrack *evnt;



} TRevent;

#include "sclib.h"


            /*************************************/
            /*        FUNCTION PROTOTYPES        */
            /*************************************/

void cminit_();
void cminit();

void cmloop_(int *iw,int *ifevb,int *inter,int *Nevents,int *iret);
void cmloop(int *iw,int ifevb,int inter,int Nevents,int *iret);

void cmlib_(int *iw, TRevent *ana, int *ifevb, int *inter, int *ievent, int *iftbt);
void cmlib(int *iw, TRevent *ana, int ifevb, int inter, int *ievent, int iftbt);

void trinit_();
void trinit();

void trlib_(int *iw, TRevent *ana, int *lanal1, float *zcut5, float *zcut8,
            int *inter);
void trlib(int *iw, TRevent *ana, int lanal1, float zcut5, float zcut8,
           int inter);

int  trbos(int *jw, TRevent *ana);

void dcfield_(float *,float *);
void dcfield(float *,float *);

void dcgcubs_(float *,float *,float *,float *,float *);
void dcgcubs(float,float,float,float,float *);

void dcicyl_(float a[npln][20],float *,float *,int *,float *,float *,int *);
void dcicyl(float a[npln][20],float *,float *,int ,float *,float *,int *);

void dcipln_(float a[npln][20],float *,float *,int *,float *,float *,int *);
void dcipln(float a[npln][20],float *,float *,int ,float *,float *,int *);

void dcrkuta_(float *,float *,float *,float *);
void dcrkuta(float,float *,float *,float *);

void dchelix_(float *,float *,float *,float *);
void dchelix(float,float *,float *,float *);

void dcswim_(float pln[npln][20],int *,int *,int *,int *,float *,
      float trp[npln][7],float tmat[npln][5][5],int *,float xyz[mxstep][3]);
void dcswim(float pln[npln][20],int ,int ,int ,int ,float *,
      float trp[npln][7],float tmat[npln][5][5],int *,float xyz[mxstep][3]);

void dcswimvt_(int *, float pln[npln][20], int *, float svin[9], float *,
          int *, int *, float svout[9], float vertex[10],
          float trp[npln][7], int *, float xyz[mxstep][3]);
void dcswimvt(int *, float pln[npln][20], int , float svin[9], float,
          int , int , float svout[9], float vertex[10],
          float trp[npln][7], int *, float xyz[mxstep][3]);

void dcmvprod_(float *, float *, float *, int *, int *);
void dcmvprod(float *, float *, float *, int , int );

void dcmxinv_(float array[5][5], int *norder, float *det);
void dcmxinv(float array[5][5], int norder, float *det);

void dctrfit_(int *, int *isec, int *Level, float trp[npln][7],
                DCtrk trk[36], float svin[9], int *inter, float *chifit);
void dctrfit(int *, int isec, int Level, float trp[npln][7],
                DCtrk trk[36], float svin[9], int inter, float *chifit);

void dcfit_(int *iw, TRevent *ana, int *Level, int *ntr_out,
         float ev_out[ntrmx][36], int *inter, int *ifail);
void dcfit(int *iw, TRevent *ana, int Level, int *ntr_out,
         float ev_out[ntrmx][36], int inter, int *ifail);

void dcrotds_(float d[9], float s[9], int *isec);
void dcrotds(float d[9], float s[9], int *isec);
void dcrotsd_(float d[9], float s[9], int *isec);
void dcrotsd(float d[9], float s[9], int *isec);

void dcptrk_(int *ilnk, int *isec);
void dcptrk(int ilnk, int isec);

void dcpdst_(int *, float ev_out[ntrmx][36], int *, float tag_res[TAG_MAX_HITS][7]);
void dcpdst(int , float ev_out[ntrmx][36], int , float tag_res[TAG_MAX_HITS][7]);

void dcdocam_(int *is, int *, float digi[6][nhlmx][5]);
void dcdocam(int is, int *, float digi[6][nhlmx][5]);

void dclramb_(int *,int *,float dcdigi[6][5],float *,float *,float *,float *);
void dclramb(int,int,float dcdigi[6][5],float,float *,float *,float *);

void dcfitsl_(int *,int *,int *,float digi[6][nhlmx][5],float *,
              float *,float *,float *,float digib[6][5]);
void dcfitsl(int,int,int *,float digi[6][nhlmx][5],float,
             float *,float *,float *,float digib[6][5]);

void dcdcam_(int *, TRevent *ana, int *ifail);
void dcdcam(int *, TRevent *ana, int *ifail);

void dcecid_(int *iw, int *isec, DCecid *ecid, float *p_in, int *iflg);
void dcecid(int *iw, int isec, DCecid *ecid, float p_in, int *iflg);

void tginit_(int *);
void tginit(int );

void tghbookcalib_(TRevent *ana);
void tghbookcalib(TRevent *ana);

void tggetcalib_(int *runno);
void tggetcalib(int runno);

void tglib_(int *iw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7]);
void tglib(int *iw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7]);

void dcfinde_(int *iw, TRevent *ana, int *ifail);
void dcfinde(int *iw, TRevent *ana, int *ifail);

void dc_stataddstatic_();
void dc_stataddstatic();
void dcstataddstatic(TRstat *stat);
void dcstatadd(TRstat *dst, TRstat *src);

void dc_statreset_();
void dc_statreset();
void dcstatreset(TRstat *stat);
void dcstatclean(TRstat *stat);

void dc_statb_(int *icut);
void dc_statb(int icut);
void dcstatb(TRstat *stat, int icut);

void dc_stati_(int *icut, char *strg, int len);
void dc_stati(int icut, char *strg);
void dcstati(TRstat *stat, int icut, char *strg);
void dcstatinit(TRstat *stat);
void dc_statinit_();

void dc_statp_();
void dc_statp();
void dcstatp(TRstat *stat);
void dc_statp1_();
void dc_statp1();
void dcstatp1(TRstat *stat);
void dc_statp2_();
void dc_statp2();
void dcstatp2(TRstat *stat);

void dcdict_(int *isec, int *ifail, TRevent *ana);
void dcdict(int isec, int *ifail, TRevent *ana);

void dcsegm_(int *isec, int *ifail, TRevent *ana);
void dcsegm(int isec, int *ifail, TRevent *ana);

void dclink_(int *isec, int *ifail, TRevent *ana);
void dclink(int isec, int *ifail, TRevent *ana);

void dcflags_(int ipat[][6], int ipat_flg[], int *nrec, int *nelem, int *nly);
void dcflags(int ipat[][6], int ipat_flg[], int nrec, int nelem, int nly);

void dcdinit_();
void dcdinit();

void dcdlast_();
void dcdlast();

void dctodst_(int *, int *,float ev_out[ntrmx][36],int *,float tag_res[TAG_MAX_HITS][7]);
void dctodst(int *, int , float ev_out[ntrmx][36], int , float tag_res[TAG_MAX_HITS][7]);

void dcbrun_(int *);
void dcbrun(int *);

void dclast_(int *);
void dclast(int *);

void dcerun_(int *);
void dcerun(int *);

void dcevin_(int *, int *,float ev_out[ntrmx][36], int *,float tag_res[TAG_MAX_HITS][7]);
void dcevin(int *, int *,float ev_out[ntrmx][36], int *,float tag_res[TAG_MAX_HITS][7]);

void trconfig_();
void trconfig();

void dcpevb_();
void dcpevb();

void dcanal_(int *, int *, int *);
void dcanal(int *, int *, int *);

void dctrig_();
void dctrig();

void dcginit_();
void dcginit();

void uthinit_();
void uthinit();

void utkinit_();
void utkinit();

void dchbook_(UThistf *histf);
void dchbook(UThistf *histf);

void dch2hbook_();
void dch2hbook();

void dchfill_(int *, TRevent *ana, int *, int *,
                         float ev_out[ntrmx][36], float tag_res[TAG_MAX_HITS][7]);
void dchfill(int *, TRevent *ana, int, int,
                         float ev_out[ntrmx][36], float tag_res[TAG_MAX_HITS][7]);

void dcdsect_(int *, TRevent *, int *, float *, float *, float *);
void dcdsect(int *, TRevent *, int, float, float, float);
void dcdvert_(int *, TRevent *, int *, float *, float *, float *);
void dcdvert(int *, TRevent *, int, float, float, float);

void sda_calib_(int *);
void dccalib_(int *, int *RunNo);
void dccalib(int *, int RunNo);

void dctrmat_(int *,float *,float *,float *,float *,float *,float *,int *);

void dcxmm55_(float *, float *, float *);


/* SC functions */

void scbeta_(int *, TRevent *ana, int *, int *ifsim,
             float *beta, int *iflg, float *, float *, float *);
void scbeta(int *, TRevent *ana, int , int ifsim,
            float *beta, int *iflg, float *, float *, float *);

void scdtime_(int *, TRevent *ana, int *is, int *itr, int *, int *, int *,
              int *, float *beta, float *);
void scdtime(int *, TRevent *ana, int is, int itr, int , int , int ,
             int , float beta, float *);

void scswalk_(int *, int *, int *, int *, float *, float *, float *);
void scswalk(int *, int , int , int , float , float *, float *);

void sctwalk_(int *, int *, int *, int digi[57][5], float *, float *,
              float *, float *, int *);
void sctwalk(int, int, int, int digi[57][5], float *, float *,
             float *, float *, int *);


/* some CERNLIB prototypes */

void lfit_(float *x, float *y, int *l, int *key, float *a, float *b, float *e);
void lfit(float *x, float *y, int l, int key, float *a, float *b, float *e);

void lfitw_(float *x, float *y, float *w, int *l, int *key, float *a, float *b, float *e);
void lfitw(float *x, float *y, float *w, int l, int key, float *a, float *b, float *e);

void ucopy_(int *a, int *b, int *n);
void ucopy(int *a, int *b, int n);

int ibset_(int *, int *);

int btest_(int *, int *);

void cross_(float x[3], float y[3], float z[3]);
void cross(float x[3], float y[3], float z[3]);

float vmod_(float *, int *);
float vmod(float *, int);

void vunit_(float *a, float *b, int *n);
void vunit(float *a, float *b, int n);

void vsub_(float *a, float *b, float *x, int *n);
void vsub(float *a, float *b, float *x, int n);

float vdot_(float *x, float *y, int *n);
float vdot(float *x, float *y, int n);

int iucomp_(void *, void *, int *, int);

float sran_(int *idum);

/* some others from utcern.c */

float iw2rw_(float *iw);
void parint_(float *x, float *a, float *f, int *n, float *r);
float parint(float x, float *a, float *f, int n);
float massage_(float *p, int *isec, float *theta_, float *phi_);
void boost_(float *bet, float p1[3], float *e1, float p2[3], float *e2);
void rotate_(int *idir_, float *the_, float *phi_, float p1[3], float p2[3]);


/* some others */
void dc_make_dgeo_(int *runno, int *force, float r2r3cor[2][6]);


                          /***************/
                          /* M A C R O S */
                          /***************/

#define HROPEN hropen_

#define HROUT hrout_

#define HREND hrend_


#define HBOOK1(id,chars,nbin,xmin,xmax,w) \
{ \
  int id_m, nbin_m; \
  float xmin_m, xmax_m, w_m; \
  char *chars_m; \
  id_m = id; \
  chars_m = chars; \
  nbin_m = nbin; \
  xmin_m = xmin; \
  xmax_m = xmax; \
  w_m = w; \
  hbook1_(&id_m,chars_m,&nbin_m,&xmin_m,&xmax_m,&w_m,strlen(chars_m)); \
}

#define HBOOK2(id,chars,nbinx,xmin,xmax,nbiny,ymin,ymax,w) \
{ \
  int id_m, nbinx_m, nbiny_m; \
  float xmin_m, xmax_m, ymin_m, ymax_m, w_m; \
  char *chars_m; \
  id_m = id; \
  chars_m = chars; \
  nbinx_m = nbinx; \
  xmin_m = xmin; \
  xmax_m = xmax; \
  nbiny_m = nbiny; \
  ymin_m = ymin; \
  ymax_m = ymax; \
  w_m = w; \
  hbook2_(&id_m,chars_m,&nbinx_m,&xmin_m,&xmax_m,&nbiny_m,&ymin_m,&ymax_m,&w_m,strlen(chars_m)); \
}

#define HFILL(id, x, y, w) \
        { \
          int id_m; \
          float x_m, y_m, w_m; \
          id_m = id; \
          x_m = x; \
          y_m = y; \
          w_m = w; \
          hfill_(&id_m, &x_m, &y_m, &w_m); \
        }

#define IGAXIS(x0,x1,y0,y1,wmin,wmax,ndiv,chopt) \
        { \
          float x0_m,x1_m,y0_m,y1_m,wmin_m,wmax_m; \
          int ndiv_m; \
          char *chopt_m; \
          x0_m = x0; \
          x1_m = x1; \
          y0_m = y0; \
          y1_m = y1; \
          wmin_m = wmin; \
          wmax_m = wmax; \
          ndiv_m = ndiv; \
          chopt_m = chopt; \
          igaxis_(&x0_m,&x1_m,&y0_m,&y1_m,&wmin_m,&wmax_m,&ndiv_m,chopt_m,strlen(chopt_m)); \
		}

#define ISPLCI(icoli) \
        { \
          int icoli_m; \
          icoli_m = icoli; \
          isplci_(&icoli_m); \
		}

#define ISPMCI(icoli) \
        { \
          int icoli_m; \
          icoli_m = icoli; \
          ispmci_(&icoli_m); \
		}

#define ISMK(mtype) \
        { \
          int mtype_m; \
          mtype_m = mtype; \
          ismk_(&mtype_m); \
		}

#define IGBOX(x1,x2,y1,y2) \
        { \
          float x1_m,x2_m,y1_m,y2_m; \
          x1_m = x1; \
          x2_m = x2; \
          y1_m = y1; \
          y2_m = y2; \
          igbox_(&x1_m,&x2_m,&y1_m,&y2_m); \
		}

#define IGTEXT(x,y,chars,size,angle,chopt) \
        { \
          float x_m,y_m,size_m,angle_m; \
          char *chars_m, *chopt_m; \
          x_m = x; \
          y_m = y; \
          size_m = size; \
          angle_m = angle; \
          chars_m = chars; \
          chopt_m = chopt; \
          igtext_(&x_m,&y_m,chars_m,&size_m,&angle_m,chopt_m,strlen(chars_m),strlen(chopt_m)); \
		}

#define IPL(n, x, y) \
        { \
          int n_m; \
          n_m = n; \
          ipl_(&n_m,x,y); \
		}

#define IPM(n, x, y) \
        { \
          int n_m; \
          n_m = n; \
          ipm_(&n_m,x,y); \
		}

#define IGSET(chname, val) \
        { \
          char *chname_m; \
          float val_m; \
          chname_m = chname; \
          val_m = val; \
          igset_(chname_m,&val_m,strlen(chname_m)); \
		}


/*

#define IGAXIS(x0,x1,y0,y1,wmin,wmax,ndiv,chopt)

#define ISPLCI(icoli)

#define ISPMCI(icoli)

#define ISMK(mtype)

#define IGBOX(x1,x2,y1,y2)

#define IGTEXT(x,y,chars,size,angle,chopt)

#define IPL(n, x, y)

#define IPM(n, x, y)

#define IGSET(chname, val)

*/





#ifdef	__cplusplus
}
#endif

#define _DCLIB_
#endif


