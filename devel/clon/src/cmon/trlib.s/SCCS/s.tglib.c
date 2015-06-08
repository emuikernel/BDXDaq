h12867
s 00009/00010/01624
d D 1.7 07/11/02 20:29:24 fklein 8 7
c *** empty log message ***
e
s 00022/00022/01612
d D 1.6 07/11/02 10:51:08 boiarino 7 6
c *** empty log message ***
e
s 00006/00003/01628
d D 1.5 07/11/01 18:50:58 boiarino 6 5
c some more fixes
c 
e
s 00185/00134/01446
d D 1.4 07/11/01 14:40:10 boiarino 5 4
c seems working, but need more checks
c 
e
s 01105/00564/00475
d D 1.3 07/10/30 21:22:20 boiarino 4 3
c implementing new offline-like version in progress,
c does not work yet, just intermediate code
c 
e
s 00001/00000/01038
d D 1.2 03/04/17 16:55:06 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/tglib.c
e
s 01038/00000/00000
d D 1.1 01/11/19 19:06:01 boiarino 1 0
c date and time created 01/11/19 19:06:01 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   tglib.c

   Purpose and Methods : tagger package

   Inputs  : none
   Outputs :

   fills tag_res array:
                       ntagh :  number of valid tagger hits
                                (T & E counters successfully matched)
                       tag_res[n][0] :  binid for E-counter (1...767)
                       tag_res[n][1] :  binid for T-counter (1...121) 
                       tag_res[n][2] :  energy for E-counter (0.209 ... 0.953)
                       tag_res[n][3] :  energy for T-counter (0.206 ... 0.954)
                       tag_res[n][4] :  time for E-counter hit 
                       tag_res[n][5] :  time for T-counter hit (mean value)
                       tag_res[n][6] :  time for T-counter hit (RF corrected) 

                     binids:    (2*i-1) for hit in single counter (i) 
                                (2*i) for hit in adjacent counters (i,i+1)
                                real counter id : (binid+1)/2
                     energies : tagger energy information 
                                as ratios of primary electron energy 
                     times :    tagger time information (w.r.t. CLAS-trigger)
   Controls:

   Library belongs: 

   Calls: 

   Created   12-JUL-1998   Franz Klein
   C remake by Sergey Boyarinov
I 4
   30-oct-2007: rewrite everything a la offline version, including pipeline TDCs
E 4

   Called by dcfinde

*/

#include <stdio.h>
I 3
#include <stdlib.h>
I 4
#include <string.h>
#include <math.h>
E 4
E 3

#include "dclib.h"
#include "tglib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

D 4
typedef struct tgcalib
{
  /* from maps */
  float slopeTL[61];
  float slopeTR[61];
  float TpeakL[61];
  float TpeakR[61];
  float TagTCi[121];
  float slopeE;
  float Epeak[384];
  /* tag2tof */
E 4

D 4
  /* from tagETcoinc.dat */
  int   etmin[384];
  int   etmax[384];
  int   t1bin[61];
  int   t2bin[61];
E 4

D 4
  /* from tagparam.dat */
  float ET_window;

  /* from tagT-boundaries.dat */
  float Tbound[122];

  /* from tagE-boundaries.dat */
  float Ebound[768];
  float Eaver[768]; /* !!!!!!!!! was Eaver(0:767) !!!!!!!!!! */
  float Edev[767];

  /* from sda.config */
  float TAG_RF_MEAN;

  /* hardcoded */
  float TAG_RF_10SIGMA;
  float TAG_TCUT[2];
  float RF_OFFSET;
  float RF_SLOPE[2];
  int   tagT_status[61];

} TGCALIB;

static TGCALIB tgcalib;
E 4
static int debug; /* set to 1 if TGLIB in a debug list in cmon.config */

#define MAXSTRLEN  1000
#define NUMFIELD1     9

D 8
#define TAG_MAX_HITS 10
E 8
#define MAX_hit      20

#define MOD(x1,x2) ( (x1) - (int)((x1)/(x2)) * (x2) )
#define ABS(x)     ( (x) < 0 ? -(x) : (x) )
I 4
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
E 4

D 4
void
tglib_(int *jw, TRevent *ana, int *ntagh, float tag_res[10][7])
{
  tglib(jw,ana,ntagh,tag_res);
}
E 4
I 4
#define Tag_max_hit 2000
#define MAXTC   2000
#define MAXEC   2000
#define MAXECTC 2000
#define MAXTAGR 2000
#define MAXHITS 2048
E 4

D 4
void
tglib(int *jw, TRevent *ana, int *ntagh, float tag_res[10][7])
{
  int hitno[TAG_MAX_HITS];
  int nhit_T, nhit_E[384], ifEexist, nhits1;
  int tag_Tbin[MAX_hit], tag_Ebin[MAX_hit];
  float tag_Tmean[61], tag_Etime[MAX_hit]; 
  float tag_Tdiff[61], tag_Ttime[MAX_hit];
  int tag_hitno[61];
  float tag_Emean[384][4];
E 4
I 4
// status bits
#define TL_PRESENT    0x01
#define TR_PRESENT    0x02
#define TLR_PRESENT   0x03
#define E_PRESENT     0x04
#define MULTIPLE_HITS 0x08
#define TRIPPLE_T     0x10
#define MULTIPLE_T    0x20    //E-hit matched to more than one T-hit
#define MULTIPLE_E    0x40    //T-hit matched to more than one E-hit
E 4

D 4
  float xval1, xval2, rf[3];
  int id, id1, id2, i, j, k, k1, k2, ih;
  int ind;
E 4
I 4
typedef int QSORT_PROC_t(const void *, const void *); 
E 4

D 4
  int tgtndig, tgtdigi[61][3]; /* tgtndig - number of hits from T-counters */
                               /* tgtdigi[61][3] - id, left TDC, right TDC */
  int tgendig, tgedigi[384][2];
  int bmndig, bmdigi[7][3];
E 4
I 4
/***************************/
/* locally used structures */
E 4

I 5
/* raw T-conuters data */
E 5
D 4
  float tleft[61] = { 26.2, 28.0, 28.7, 27.5, 27.1,
                      24.2, 24.3, 28.2, 26.2, 27.0,
                      28.8, 25.6,
                                  35.6, 34.9, 34.3,
                      35.3, 34.6, 33.3, 35.7, 35.7,
                      33.9, 33.9, 33.5, 34.1, 33.0,
                      34.1, 34.0, 34.5, 34.2, 34.2,
                      34.1, 33.9, 34.3, 34.5, 33.9,
                      33.4, 36.1, 34.5, 36.6, 36.0,
                      33.3, 36.0, 33.2, 33.6, 34.3,
                      36.5, 36.8, 32.7, 31.0, 35.2,
                      31.8, 31.5, 32.5, 30.7, 31.8,
                      31.0, 0.0, 0.0, 0.0, 0.0, 0.0};
E 4
I 4
typedef struct {
I 5
  int id;
  int val;
  int used;
} TagTC_t;
TagTC_t tcl[MAXHITS];
TagTC_t tcr[MAXHITS];
int Tgtl_NbHits;
int Tgtr_NbHits;

/* raw E-counters data */
typedef struct {
  int id;
  int val;
} TagEC_t;
TagEC_t ec[MAXEC];
int Tag_E_NbHits;

typedef struct {
E 5
  int NBgoodE;
  int Good_E_id[MAXEC];
  float Good_E_val[MAXEC];
} TagECfiltered_t;
TagECfiltered_t tagecfiltered_;
E 4

D 4
  float tright[61] ={ 17.9, 17.2, 18.1, 17.1, 17.9,
                      18.0, 15.8, 16.7, 14.1, 16.5,
                      17.1, 17.4,
                                  26.3, 26.6, 29.3,
                      26.4, 26.5, 28.9, 29.3, 22.6,
                      27.0, 28.2, 29.2, 26.9, 27.6,
                      27.5, 26.7, 28.8, 21.9, 27.7,
                      25.9, 28.3, 25.4, 26.8, 28.0,
                      27.7, 29.1, 29.1, 28.3, 27.6,
                      29.6, 27.8, 27.0, 27.2, 28.6,
                      31.4, 28.1, 27.9, 31.2, 25.2,
                      24.1, 24.5, 25.9, 35.4, 27.2,
                      24.0, 0.0, 0.0, 0.0, 0.0, 0.0};
E 4
I 4
typedef struct {
D 5
  int NBgoodT;
  int Good_T_id[MAXTC];
  float Good_T_L_val[MAXTC];
  float Good_T_R_val[MAXTC];
E 5
I 5
  int Good_T_id;
  float Good_T_L_val;
  float Good_T_R_val;
E 5
} TagTCfiltered_t;
D 5
TagTCfiltered_t tagtcfiltered_;
E 5
I 5
TagTCfiltered_t tagtcfiltered_[MAXTC];
int NBgoodT;
E 5
E 4

D 4
  float ecount[384] = {
    37.0, 36.2, 35.5, 37.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0, 34.0,
    34.0, 34.0, 34.0, 34.0 };
                       
E 4
I 4
typedef struct {
  int id;
  float t_mean[2];
  float tl[2];
  float tr[2];
  int status;
} TAGT_bin_t;
TAGT_bin_t Tchan_hit[MAXTC];
E 4

I 4
typedef struct {
  int id;
  float t[2];
  int status;
} TAGE_bin_t;
TAGE_bin_t Echan_hit[MAXEC];
E 4

D 4
  TGCALIB *tgcal;
  int *tag_nhT, tag_nhT0[62];  /* was tag_nhT(0:61) !!! */
E 4
I 4
typedef struct {
  int tid;
  int eid; 
  int Thit;
  int Ehit;
  int status;
} ET_hit_t;
ET_hit_t ET_coincidence[MAXECTC];
E 4

D 4
  tgcal = (TGCALIB *) &tgcalib.ET_window;
  tag_nhT = &tag_nhT0[1];
  *ntagh = 0;
E 4
I 4
typedef struct {
  float Mean_T_val[2][MAXTC];
  float RF_T_diff[2][MAXTC];
  float Diff_T_val[2][MAXTC];
  float T_time[2][MAXTC];
} TagTcorr_t;
TagTcorr_t tagtcorr_;
E 4

D 4
  /* initialize local arrays */
E 4

D 4
  nhits1   = 0;
  nhit_T   = 0;
  for(j=0; j<384; j++) nhit_E[j] = 0;
  for(j=0; j<61; j++)
  {
    tag_Tmean[j] = 0.0;
    tag_nhT[j]   = 0;
  }
  tag_nhT0[0] = 0;
E 4

D 4
  /**************/
  /* T counters */
  /**************/
E 4

D 4
  GETTGTDATA;
E 4

D 4
  /* cut on reasonable TDC values (converted to time)
     for left and right PMT */
  for(j=0; j<tgtndig; j++)
  {
    id   = tgtdigi[j][0] - 1;
E 4

D 4
/* g6c only */
if(id > 11) continue;
/* g6c only */
E 4
I 4
/* output bank format */
typedef struct {
  int ncol;
  int nrow;
  char name[4];
  int nr;
  int nextIndex;
  int nwords;
} bankHeader_t;
typedef struct {
  float erg;   /*  Energy of the photon in GeV */
  float ttag;  /*  Time of the photon has reconstructed in the Tagger */
  float tpho;  /*  Time of the photon after RF correction */
  int stat;    /*  Status ( 7 or 15 are Good) other values have problems (see tag_process_TAGR.F) */
  int t_id;    /*  T counter Id */
  int e_id;    /*  E counter Id */
} tagr_t;
typedef struct {
  bankHeader_t bank;
D 5
  tagr_t tagr[1];
E 5
I 5
  tagr_t tagr[MAXECTC/*1 if real bos bank*/];
E 5
} clasTAGR_t;
I 5
clasTAGR_t Tagr; /* will not fill bos bank, use local structure instead */
E 5
E 4

D 4
uthfill(ana->histf, 161, tgcal->slopeTL[id], 0., 1.);
uthfill(ana->histf, 162, tgcal->slopeTR[id], 0., 1.);
uthfill(ana->histf, 163, tgcal->TpeakL[id], 0., 1.);
uthfill(ana->histf, 164, tgcal->TpeakR[id], 0., 1.);
uthfill(ana->histf, 167, (float)tgtdigi[j][1], 0., 1.);
uthfill(ana->histf, 169, (float)tgtdigi[j][2], 0., 1.);
E 4
D 5

E 5
D 4
    /* slopeTL = 0.048ns, TpeakL = 24.0-30. (about) */
    xval1 = tgtdigi[j][1]*tgcal->slopeTL[id] - tgcal->TpeakL[id]; /* left */
    xval2 = tgtdigi[j][2]*tgcal->slopeTR[id] - tgcal->TpeakR[id]; /* right */
E 4
I 4
/* following prototypes were used:
tag_read_from_BOS.F (fill arrays by raw data)
tag_filter_translate.F (convert raw data to 'ns')
tag_filter_out.F (convert raw data to 'ns')
tagcal_read_map.F (read constants from maps)
*/
E 4


D 4
    /* if one PMT always missing set it here */
    if(tgcal->tagT_status[id] == 1) xval1 = xval2;
    if(tgcal->tagT_status[id] == 2) xval2 = xval1;
E 4
I 4
/*************************************************/
/*************************************************/
/********* structures filled by tginit() *********/
E 4


D 4
xval1 = xval1 + (27.0 - tleft[id]);
xval2 = xval2 + (27.0 - tright[id]);
E 4
I 4
/***********************************/
/*********** ASCII files ***********/
E 4

I 4
/* input params: T-counter partners for E-counters */
/* from file 'tagETcoinc_ALL.dat' (choose closest energy) */
typedef struct {
  int ectcmapsMM; /* not used */
  int tmin[384];
  int tmax[384];
  int tbinmnin[767]; /* not used */
  int t1min[384];
  int t1max[384];
  int tchmin[384];
  int tchmax[384];
} ectcmaps_t;
ectcmaps_t ectcmaps_;
E 4

D 4
uthfill(ana->histf, 171, xval1, 0., 1.);
uthfill(ana->histf, 172, xval2, 0., 1.);
uthfill(ana->histf, 170, xval1-xval2, 0., 1.);
E 4
I 4
/* input params from file 'tagparam.dat' */
typedef struct {
  double adj_T_coinc;
  double adj_E_coinc;
  double ET_window;
  double E_tdc_min;
  double E_tdc_max;
  double TAG_DSD_window;
  double TAG_ST_window;
  double TAG_DSD_Xtrawindow;
  double TAG_ST_Xtrawindow;
  double PS_coinc_window;
  double TLR_window;
} TagParam_t;
TagParam_t tagparam_;
E 4

D 4
if(debug)
{
  i=1001+id;
  uthfill(ana->histf, i, xval1, 0., 1.);
  /*uthfill(ana->histf, i, (float)tgtdigi[j][1], 0., 1.);*/
  i=1201+id;
  uthfill(ana->histf, i, xval2, 0., 1.);
  /*uthfill(ana->histf, i, (float)tgtdigi[j][2], 0., 1.);*/
}
E 4
I 4
/* input params from file 'tagT-boundaries_ALL.dat' and 'tagE-boundaries_ALL.dat' */
typedef struct {
  float Tboundary[122];
  float Eboundary[768];
  float Eaverage[767]; /* only that is used here */
  float Edeviation[767];
  int tagEboundsMM;
  int tagTboundsMM;
} tagEnergyBounadaries_t;
tagEnergyBounadaries_t tagenergyboundaries_;
E 4

D 4
	if(xval1 > tgcal->TAG_TCUT[0] && xval1 < tgcal->TAG_TCUT[1] &&
       xval2 > tgcal->TAG_TCUT[0] && xval2 < tgcal->TAG_TCUT[1])
    {
      nhit_T ++;
      tag_Tmean[id] = (xval1+xval2)/2.0;
uthfill(ana->histf, 173, tag_Tmean[id], 0., 1.);
    }
  }
E 4


D 4
  /* exit if no valid T-counter entry */
/*if(nhit_T == 0) goto a900;*/
E 4


D 4
  /***************/
  /*  E counters */
  /***************/
E 4

D 4
  GETTGEDATA;
E 4
I 4
/****************************/
/*********** Maps ***********/
E 4

D 4
  ifEexist = 0;
  for(j=0; j<tgendig; j++)
  {
    id    = tgedigi[j][0] - 1;
    /* sloteE = -0.5ns, Epeak = -610. (about) */
    xval1 = tgedigi[j][1] * tgcal->slopeE - tgcal->Epeak[id];
E 4
I 4
/* from map */
typedef struct {
  float ECchWidth;
  float TCchWidthL[61];
  float TCchWidthR[61];
  float TagTDCCalTMM;
  float TAGTDCCalEMM;
} tagTDCCal_t;
tagTDCCal_t tagtdccal_;
E 4

D 4
xval1 = xval1 + 22.0 + (34.0 - ecount[id]);
uthfill(ana->histf, 174, xval1, 0., 1.);
uthfill(ana->histf, 175, xval1, 0., 1.);
E 4
I 4
/* from map: for RF correction */
typedef struct {
  float TagTCi[121];
  float TagCivalMM;
} tagCiVal_t;
tagCiVal_t tagcival_;
/*???
typedef struct {
  float TagTCi[121];
  int TagCivalMM;
  float TagTCiMean;
}  TagCival_t;
TagCival_t tagcival_;
*/
E 4

D 4
if(debug)
{
  i=1601+id;
  uthfill(ana->histf, i, xval1, 0., 1.);
}
E 4
I 4
/* from map */
typedef struct {
  float posTpeakL[61];
  float posTpeakR[61];
  float posEpeak[384];
  float tagTDCbaseEpeakMM;
  float tagTDCbaseTpeakMM;
} tagTDCbasePeak_t;
tagTDCbasePeak_t tagtdcbasepeak_;
E 4

D 4
    /* ----- cut for reasonable time window  */
    /* ----- regard up to 4 hits per counter */
E 4
I 4
/* hardware status(map): dead channels etc */
typedef struct {
  int E_hw_status[384];
  int TL_hw_status[61];
  int TR_hw_status[61];
} tagHW_status_t;
tagHW_status_t hwstatus_;
E 4

D 4
    if(nhit_E[id] < 4 &&
       xval1 > tgcal->TAG_TCUT[0] && xval1 < tgcal->TAG_TCUT[1])
    {
      ifEexist = 1;
      tag_Emean[id][nhit_E[id]] = xval1;
      nhit_E[id] ++;
    }
  }
E 4
I 4
/* from map: offset between tagger and tof */
typedef struct {
  float tag2tof;
} tagtotof_t;
tagtotof_t tagtotof_;
E 4

I 4
/* from map: energy correction table */
typedef struct {
  float ecor[767];
} tag_ecor_t;
tag_ecor_t ecor_table_;
E 4


D 4
  /* (1) for individual T-counters */
E 4

D 4
  if(ifEexist)
  {
    for(j=1; j<=61; j++)
    {
      if(tag_Tmean[j-1] == 0.0) continue;
      k1 = tgcal->t1bin[j-1]; /* lower E counter for T counter */
      k2 = tgcal->t2bin[j-1]; /* higher E counter for T counter */
      id1 = -1;
      for(k=k1; k<=k2; k++)
      {
        ih = 0;
        xval2 = tgcal->ET_window;
        for(i=1; i<=nhit_E[k-1]; i++)
        {
          /* time diff between E and T counters */
          xval1 = ABS(tag_Emean[k-1][i-1] - tag_Tmean[j-1]);
          if(xval1 < xval2)
          {
            xval2 = xval1; /* looking for closest E to T */
            ih    = i;
          }
        }
E 4

D 4
        if(ih > 0)
        {
          if(id1 == k-1)
          {
            tag_Ebin[nhits1-1]  = 2*id1;
            if(xval2 < tag_Tdiff[nhits1-1])
            {
              tag_Tdiff[nhits1-1] = xval2;
              tag_Etime[nhits1-1] = tag_Emean[k-1][ih-1];
            }
          }
          else
          {
            id1 = k;
            tag_nhT[j-1]++;
            tag_Tbin[nhits1]  = j*2-1;
            tag_Ebin[nhits1]  = k*2-1;
            tag_Tdiff[nhits1] = xval2;
            tag_Ttime[nhits1] = tag_Tmean[j-1];
            tag_Etime[nhits1] = tag_Emean[k-1][ih-1];
            tag_hitno[j-1] = ++nhits1;
          }
        }
      }
E 4

D 4
      /* (2) for hits in neighbouring T-counters
           (except T-counters 18-20: overlap up to 2 counters) */
E 4

D 4
      if(j == 20 && tag_nhT[17] > 0)
      {
        tag_hitno[18] = tag_hitno[17] + tag_nhT[18];
        tag_nhT[18] = tag_nhT[17] + tag_nhT[18];
      }
E 4
I 4
/* input param(MAP): only beam energy is used here, will get it online */
typedef struct {
  float Beam_energy; /* used */
  float PS_current; /* not used */
  float tager_current; /* not used */
  float torus_current; /* not used */
  int tagger_do_t_trans; /* not used */
  int use_tgtlr; /* old/new tdc flag, not in use here */
  int et_map_from_db; /* not used */
} tagRunControl_t;
tagRunControl_t tagruncontrol_;
E 4

D 4
      if(tag_nhT[j-1]*tag_nhT[j-2] == 0) continue;
      for(i=1; i<=tag_nhT[j-1]; i++)
      {
        k1 = tag_hitno[j-1] - i + 1;
        id = MOD(tag_Ebin[k1-1],2);
        for(k=1; k<=tag_nhT[j-2]; k++)
        {
          k2 = tag_hitno[j-2] - k + 1;
          if(ABS(tag_Ebin[k1-1]-tag_Ebin[k2-1]) < 2)
          {
            if(id == 1)
            {
              id1 = k2;
              id2 = k1;
            }
            else
            {
              id1 = k1;
              id2 = k2;
            }
            tag_Ebin[id1-1] = (tag_Ebin[id1-1]+tag_Ebin[id2-1])/2;
            tag_Ebin[id2-1] = -tag_Ebin[id2-1];
            tag_Tbin[id1-1] = (j-1)*2;
            if(tag_Tdiff[id2-1] < tag_Tdiff[id1-1])
            {
              tag_Ttime[id1-1] = tag_Ttime[id2-1];
              tag_Etime[id1-1] = tag_Etime[id2-1];
              tag_Tdiff[id1-1] = tag_Tdiff[id2-1];
            }
          }
        }
      }
    }
  }
E 4


D 4
  /* (3) no coincidence ? take only T-counters */
E 4

D 4
  if(nhits1 == 0)
E 4
I 4










int Tagger_convert = 0; /* ?????????????????????????? */


/**************************************************************************/

float
rf_corr_time(float time)
{
  float rf_st_time = time;
I 5

/*temporary until implemented*/
return(time);

E 5
  /*must redo
  clasCL01_t *CL01 = getBank(&bcs_, "CL01");
  if (CL01){
    rf_st_time = time - rf_correction(time, CL01->cl01[0].rf);
  }
  */

  /*our old stuff
{
  int i, nrow;
  unsigned short *i16;
  bmndig = 0;
  for(ih=0; ih<7; ih++)
E 4
  {
D 4
    j=1;
    while(j <= 61)
E 4
I 4
    bmdigi[ih][0] = 0;
    bmdigi[ih][1] = 0;
    bmdigi[ih][2] = 0;
  }
  if((ind=etNlink(jw,"RFT ",0)) > 0)
  {
    i16 = (unsigned short *) &jw[ind];
    nrow = etNdata(jw,ind);
    bmndig = 1;
    for(i=0; i<nrow; i++)
E 4
    {
D 4
      if(tag_Tmean[j-1] != 0.0)
E 4
I 4
      ih = i16[0];
      if(ih==6)
E 4
      {
D 4
        k1 = 0;
        if(j == 61 || tag_Tmean[j+1-1] == 0.0) /* TAG_RF_MEAN = zcut(9) */
		{
          if(ABS(tag_Tmean[j-1]-tgcal->TAG_RF_MEAN) < tgcal->TAG_RF_10SIGMA)
          {
            k  = j;
            k1 = 2*j - 1;
          }
        }
        else
        {
          if(ABS(tag_Tmean[j-1]-tgcal->TAG_RF_MEAN) < tgcal->TAG_RF_10SIGMA)
          {
            k = j;
            k1= 2*j;
            j ++;
          }
          else if(ABS(tag_Tmean[j+1-1]-tgcal->TAG_RF_MEAN) < tgcal->TAG_RF_10SIGMA)
          {
            k = j+1;
            k1= 2*j;
            j ++;
          }
        }

        if(k1 > 0)
        {
          nhits1 ++;
          tag_Tbin[nhits1-1] = k1;
          tag_Ebin[nhits1-1] = 0;
          tag_Etime[nhits1-1] = 0.0;
          tag_Ttime[nhits1-1] = tag_Tmean[k-1];
          tag_Tdiff[nhits1-1] = ABS(tag_Tmean[k-1]-tgcal->TAG_RF_MEAN);
        }
E 4
I 4
        bmdigi[0][0] = i16[0];
        bmdigi[0][1] = i16[1];
E 4
      }
D 4
      j ++;
E 4
I 4
      i16 += 2;
E 4
    }
  }
I 4
}
E 4

D 4
  /* (4)  get RF times */


  GETBMDATA;
E 4
  rf[0] = bmdigi[5][1] * tgcal->RF_SLOPE[0];
  rf[1] = bmdigi[6][1] * tgcal->RF_SLOPE[1];
D 4
/*printf("rf[0]=%f rf[1]=%f\n",rf[0],rf[1]);*/
E 4

  if(rf[0] > 15. && rf[0] < 78.)
  {
    rf[2] = rf[0];
  }
  else
  {
    rf[2] = rf[1] + tgcal->RF_OFFSET;
  }
uthfill(ana->histf, 176, rf[0], 0., 1.);
uthfill(ana->histf, 177, rf[1], 0., 1.);
uthfill(ana->histf, 178, rf[2], 0., 1.);
uthfill(ana->histf, 179, rf[0]-rf[1], 0., 1.);

D 4
  /* (4)  correct timing for RF and copy to output */

E 4
  k1 = nhits1;

  while(k1 > 0 && (tag_Ebin[k1-1] <= 0 ||
     ABS(tag_Tmean[k1-1]-tgcal->TAG_RF_MEAN) > tgcal->TAG_RF_10SIGMA) )
  {
    k1 --;
  }

  if(k1 > 0)
  {
    *ntagh = 1;
    hitno[0] = k1;
  }
  for(j=1; j<=nhits1; j++)
  {
    if(j != k1 && tag_Ebin[j-1] >= 0 &&  *ntagh < TAG_MAX_HITS)
    {
      (*ntagh) ++;
      hitno[(*ntagh)-1] = j;
    }
  }

  for(k=1; k<=(*ntagh); k++)
  {
    j=hitno[k-1];
    tag_res[k-1][0] = tag_Ebin[j-1];
    tag_res[k-1][1] = tag_Tbin[j-1];
D 4
    tag_res[k-1][2] = tgcal->Eaver[tag_Ebin[j-1]]; /* ! was Eaver(0:767) */
E 4
I 4
    tag_res[k-1][2] = tgcal->Eaver[tag_Ebin[j-1]];
E 4
    tag_res[k-1][3] = (tgcal->Tbound[tag_Tbin[j-1]-1]
                     + tgcal->Tbound[tag_Tbin[j-1]+1-1])/2.;
    tag_res[k-1][4] = tag_Etime[j-1];
    tag_res[k-1][5] = tag_Ttime[j-1] - tgcal->TagTCi[tag_Tbin[j-1]-1];
    xval1 = (tag_res[k-1][5] - rf[2]  - 1.002) / 2.004;
    tag_res[k-1][6] = rf[2] + ((int)(xval1))*2.004;

uthfill(ana->histf, 201, xval1, 0., 1.);
uthfill(ana->histf, 202, tag_res[k-1][6], 0., 1.);

  }
I 4
*/
E 4


I 4

  return rf_st_time;
}

/* sorting functions */
int
I 5
tcraw_sort(TagTC_t *hit1, TagTC_t *hit2)
{
  /* sort by id */
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
  return(0);
}
int
ecraw_sort(TagEC_t *hit1, TagEC_t *hit2)
{
  /* sort by id */
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
  return(0);
}
int
tcfiltered_sort(TagTCfiltered_t *hit1, TagTCfiltered_t *hit2)
{
  /* sort by id */
  if (hit1->Good_T_id != hit2->Good_T_id) return hit1->Good_T_id - hit2->Good_T_id;
  return(0);
}
int
E 5
ECh_sort(TAGE_bin_t *hit1, TAGE_bin_t *hit2)
{
D 5
  /*clasTAGE_t *TAGE = NULL;*/
  // sort by id firts
E 5
I 5
  /* sort by id first */
E 5
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
D 5
  // then by time
E 5
I 5
  /* then by time */
E 5
  if (hit1->t[0] != hit2->t[0]) return hit1->t[0] - hit2->t[0];
D 5
  // error, identical hits
E 5
I 5
  /* error, identical hits */
E 5
  fprintf(stderr, "Ech_sort -E- identiacal hits, wrong !!!!\n");

  return(0);
}
int
TCh_sort(TAGT_bin_t *hit1, TAGT_bin_t *hit2)
{
D 5
  // sort by id firts
E 5
I 5
  /* sort by id first */
E 5
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
D 5
  // then by time
E 5
I 5
  /* then by time */
E 5
  if (hit1->t_mean[0] != hit2->t_mean[0]) return hit1->t_mean[0] - hit2->t_mean[0];
D 5
  // error, identical hits
E 5
I 5
  /* error, identical hits */
E 5
  fprintf(stderr, "Tch_sort -E- identiacal hits, wrong !!!!\n");

  return(0);
}


void
D 8
tglib_(int *jw, TRevent *ana, int *ntagh, float tag_res[10][7])
E 8
I 8
tglib_(int *jw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7])
E 8
{
  tglib(jw,ana,ntagh,tag_res);
}

void
D 8
tglib(int *jw, TRevent *ana, int *ntagh, float tag_res[10][7])
E 8
I 8
tglib(int *jw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7])
E 8
{
  int ind, id, il, ir, nrow, iii;
  unsigned short *i16;
  int i = 0;
  int j = 0;  
  int k = 0;
  int usedE[MAXEC];
  int usedT[MAXTC];
  
  int n_tch_hits = 0;
  int n_ech_hits = 0;
  int n_et_coin_hits = 0;
  
  float tmean1;
  float tmean2;
  
  int TCH_min;
  int TCH_max;
  
  int eid;

D 5
  /* raw E-counters data */
  int Tag_E_NbHits;
  int ec_id[MAXEC];
  int ec_val[MAXEC];

  /* raw T-conuters data */
  int Tgtl_NbHits;
  int itc_id_left[MAXHITS];
  int itc_val_left[MAXHITS];
  int tcl_used[MAXHITS];

  int Tgtr_NbHits;
  int itc_id_right[MAXHITS];
  int itc_val_right[MAXHITS];
  int tcr_used[MAXHITS];

E 5
  int Good_TL_Id[MAXTC], Good_TR_Id[MAXTC], NbGoodTL, NbGoodTR;
  float	Good_TL_val[MAXTC], Good_TR_val[MAXTC];
  float diff, bestdiff;
  int best_hit;

  clasTAGR_t *TAGR = NULL;
  

  /* reset everything before we go any further */
I 5
  *ntagh = 0;
E 5
  for (i=0; i<MAXEC; i++)
  {
    Echan_hit[i].id    = 0;
    Echan_hit[i].t[0]  = 0.;
    Echan_hit[i].t[1]  = 0.;
    Echan_hit[i].status = 0;
    usedE[i] = 0;
  }

  /************************************************/
  /* step 0: read raw data and convert it to 'ns' */

  /* get T-counters TDC data */
  Tgtl_NbHits = 0;
  if((ind=etNlink(jw,"TGTL",0)) > 0)
  {
    i16 = (unsigned short *) &jw[ind];
    nrow = etNrow(jw,ind);
    for(i=0; i<nrow; i++)
    {
      id = i16[0];
      if(id > 0 && id <= 61) /* filter data here */
      {
D 5
        itc_id_left[Tgtl_NbHits] = id;
        itc_val_left[Tgtl_NbHits] = i16[1];
E 5
I 5
        tcl[Tgtl_NbHits].id = id;
        tcl[Tgtl_NbHits].val = i16[1];
E 5
uthfill(ana->histf, 167, (float)i16[1], 0., 1.);
        Tgtl_NbHits ++;
	  }
      i16 += 2;
    }
  }
  Tgtr_NbHits = 0;
  if((ind=etNlink(jw,"TGTR",0)) > 0)
  {
    i16 = (unsigned short *) &jw[ind];
    nrow = etNrow(jw,ind);
    for(i=0; i<nrow; i++)
    {
      id = i16[0];
      if(id > 0 && id <= 61) /* filter data here */
      {
D 5
        itc_id_right[Tgtr_NbHits] = id;
        itc_val_right[Tgtr_NbHits] = i16[1];
E 5
I 5
        tcr[Tgtr_NbHits].id = id;
        tcr[Tgtr_NbHits].val = i16[1];
E 5
uthfill(ana->histf, 169, (float)i16[1], 0., 1.);
        Tgtr_NbHits ++;
	  }
      i16 += 2;
    }
  }
  /*printf("T-counters data ---\n");
  for(i=0; i<tgtndig; i++)
    printf("[%2d] %5d %5d\n",tgtdigi[i][0],tgtdigi[i][1],tgtdigi[i][2]);*/

D 5
  /* WHY NEED IT ?????
  tag_sort_array_(itc_id_left,itc_val_left,Tgtl_NbHits)
  tag_sort_array_(itc_id_left,itc_val_left,Tgtl_NbHits)
  */
E 5
I 5
  if(Tgtl_NbHits>1) qsort(tcl, Tgtl_NbHits, sizeof(TagTC_t), (QSORT_PROC_t*)tcraw_sort); 
  if(Tgtr_NbHits>1) qsort(tcr, Tgtr_NbHits, sizeof(TagTC_t), (QSORT_PROC_t*)tcraw_sort); 
  
E 5

  /* get E-counters TDC data */
  Tag_E_NbHits = 0;
  if((ind=etNlink(jw,"TAGE",0)) > 0 || (ind=etNlink(jw,"TAGE",1)) > 0)
  {
    i16 = (unsigned short *) &jw[ind];
    nrow = etNrow(jw,ind);
    for(i=0; i<nrow; i++)
    {
      id = i16[0];
	  /*printf("i=%d (id=%d tdc=%d)\n",i,id,i16[1]);*/
      if(id > 0 && id <=384) /* filter data here */
      {
D 5
        ec_id[Tag_E_NbHits] = id;
        ec_val[Tag_E_NbHits] = i16[1];
E 5
I 5
        ec[Tag_E_NbHits].id = id;
        ec[Tag_E_NbHits].val = i16[1];
E 5
		/*printf("E raw: [%d] id=%d tdc=%d\n",i,id,i16[1]);*/
uthfill(ana->histf, 174, (float)i16[1], 0., 1.);
        Tag_E_NbHits ++;
	  }
      i16 += 2;
    }
  }
  /*printf("Tag_E_NbHits=%d\n",Tag_E_NbHits);*/
I 5
  if(Tag_E_NbHits>1) qsort(ec, Tag_E_NbHits, sizeof(TagEC_t), (QSORT_PROC_t*)ecraw_sort); 
E 5

D 5
  /* NEED IT ???
  tag_sort_array_(ec_id,ec_val,Tag_E_NbHits)
  */
E 5

D 5

E 5
  /* Find the number of E counters in a reasonable time window */    
  tagecfiltered_.NBgoodE = 0;
  for(i=0; i<Tag_E_NbHits; i++)
  {
	/*
printf("E-hits: [%d] id=%d tdc=%d (%f %f %d)\n",
D 5
i,ec_id[i],ec_val[i],tagparam_.E_tdc_min,tagparam_.E_tdc_max,hwstatus_.E_hw_status[ec_id[i]]);
E 5
I 5
i,ec[i].id,ec[i].val,tagparam_.E_tdc_min,tagparam_.E_tdc_max,hwstatus_.E_hw_status[ec[i].id]);
E 5
	*/
D 5
    if(ec_val[i]>tagparam_.E_tdc_min && ec_val[i]<tagparam_.E_tdc_max &&
       ec_id[i]>0 && ec_id[i]<=384 &&
       hwstatus_.E_hw_status[ec_id[i]]==0)
E 5
I 5
    if(ec[i].val>tagparam_.E_tdc_min && ec[i].val<tagparam_.E_tdc_max &&
       ec[i].id>0 && ec[i].id<=384 &&
       hwstatus_.E_hw_status[ec[i].id]==0)
E 5
	{
D 5
	  tagecfiltered_.Good_E_id[tagecfiltered_.NBgoodE] = ec_id[i];
E 5
I 5
	  tagecfiltered_.Good_E_id[tagecfiltered_.NBgoodE] = ec[i].id;
E 5
      tagecfiltered_.Good_E_val[tagecfiltered_.NBgoodE] =
D 5
        - tagtdccal_.ECchWidth * ((float)ec_val[i])/1000.- tagtdcbasepeak_.posEpeak[ec_id[i]];
E 5
I 5
        - tagtdccal_.ECchWidth * ((float)ec[i].val)/1000.- tagtdcbasepeak_.posEpeak[ec[i].id];
E 5
	  /*printf("good val = %f[%d]\n",tagecfiltered_.Good_E_val[tagecfiltered_.NBgoodE],tagecfiltered_.NBgoodE);*/
uthfill(ana->histf, 175, tagecfiltered_.Good_E_val[tagecfiltered_.NBgoodE], 0., 1.);
      if(debug)
      {
        iii=1601+tagecfiltered_.Good_E_id[tagecfiltered_.NBgoodE];
        uthfill(ana->histf, iii, tagecfiltered_.Good_E_val[tagecfiltered_.NBgoodE], 0., 1.);
      }

	  tagecfiltered_.NBgoodE ++;
	}
  }
  if(tagecfiltered_.NBgoodE<MAXEC) tagecfiltered_.Good_E_id[tagecfiltered_.NBgoodE] = 0;





  for(i=0; i<MAXHITS; i++) /*reset arrays for used hits*/
  {
D 5
	tcl_used[i] = 0;
	tcr_used[i] = 0;
E 5
I 5
	tcl[i].used = 0;
	tcr[i].used = 0;
E 5
  }

  /* convert TGTL to ns */
  NbGoodTL = 0;
  for(i=0; i<Tgtl_NbHits; i++)
  {
D 5
	if(hwstatus_.TL_hw_status[itc_id_left[i]]==0)
E 5
I 5
	if(hwstatus_.TL_hw_status[tcl[i].id]==0)
E 5
	{
D 5
	  Good_TL_Id[NbGoodTL] = itc_id_left[i];
E 5
I 5
	  Good_TL_Id[NbGoodTL] = tcl[i].id;
E 5
	  if(Tagger_convert==0) /*old scheme*/
      {
D 5
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[itc_id_left[i]]*((float)itc_val_left[i])/1000.
		  - tagtdcbasepeak_.posTpeakL[itc_id_left[i]];
E 5
I 5
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[tcl[i].id]*((float)tcl[i].val)/1000.
		  - tagtdcbasepeak_.posTpeakL[tcl[i].id];
		/*
printf("==> [%d] (%f) = -(%f)*(%f)/1000.-(%f)\n",
i,Good_TL_val[NbGoodTL],tagtdccal_.TCchWidthL[tcl[i].id],(float)tcl[i].val,tagtdcbasepeak_.posTpeakL[tcl[i].id]);
		*/
E 5
	  }
	  else /*new scheme*/
	  {
D 5
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[itc_id_left[i]]/1000.*((float)itc_val_left[i])
          - tagtdcbasepeak_.posTpeakL[itc_id_left[i]];
E 5
I 5
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[tcl[i].id]/1000.*((float)tcl[i].val)
          - tagtdcbasepeak_.posTpeakL[tcl[i].id];
E 5
	  }
D 5
/*printf("Tleft: [%d] id=%d tdc=%d val=%f\n",i,itc_id_left[i],itc_val_left[i],Good_TL_val[NbGoodTL]);*/
E 5
I 5
/*printf("Tleft: [%d] id=%d tdc=%d val=%f\n",i,tcl[i].id,tcl[i].val,Good_TL_val[NbGoodTL]);*/
E 5
uthfill(ana->histf, 170, Good_TL_val[NbGoodTL], 0., 1.);
D 7
if(debug)
{
  iii=1001+Good_TL_Id[NbGoodTL];
  uthfill(ana->histf, iii, Good_TL_val[NbGoodTL], 0., 1.);
}
E 7
I 7
      if(debug)
      {
        iii=1001+Good_TL_Id[NbGoodTL];
        uthfill(ana->histf, iii, Good_TL_val[NbGoodTL], 0., 1.);
      }
E 7
	  NbGoodTL ++;
    }
  }

  /* convert TGTR to ns */
  NbGoodTR = 0;
  for(i=0; i<Tgtr_NbHits; i++)
  {
D 5
    if(hwstatus_.TR_hw_status[itc_id_right[i]]==0)
E 5
I 5
    if(hwstatus_.TR_hw_status[tcr[i].id]==0)
E 5
	{
D 5
	  Good_TR_Id[NbGoodTR] = itc_id_right[i];
E 5
I 5
	  Good_TR_Id[NbGoodTR] = tcr[i].id;
E 5
	  if(Tagger_convert==0) /*old scheme*/
	  {
D 5
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[itc_id_right[i]]*((float)itc_val_right[i])/1000.
		  - tagtdcbasepeak_.posTpeakR[itc_id_right[i]];
E 5
I 5
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[tcr[i].id]*((float)tcr[i].val)/1000.
		  - tagtdcbasepeak_.posTpeakR[tcr[i].id];
E 5
      }
	  else /*new scheme*/ 
	  {
D 5
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[itc_id_right[i]]/1000.*((float)itc_val_right[i])
          - tagtdcbasepeak_.posTpeakR[itc_id_right[i]];
E 5
I 5
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[tcr[i].id]/1000.*((float)tcr[i].val)
          - tagtdcbasepeak_.posTpeakR[tcr[i].id];
E 5
	  }
uthfill(ana->histf, 171, Good_TR_val[NbGoodTR], 0., 1.);
D 7
if(debug)
{
  iii=1201+Good_TR_Id[NbGoodTR];
  uthfill(ana->histf, iii, Good_TR_val[NbGoodTR], 0., 1.);
}
E 7
I 7
      if(debug)
      {
        iii=1201+Good_TR_Id[NbGoodTR];
        uthfill(ana->histf, iii, Good_TR_val[NbGoodTR], 0., 1.);
      }
E 7
	  NbGoodTR ++;
	}
  }
  /* done with conversion */
D 5
  tagtcfiltered_.NBgoodT = 0;  
E 5
I 5
  NBgoodT = 0;  
E 5

  /* now will do left/right time matching */
  if(NbGoodTL>0)
  {
	for(il=0; il<NbGoodTL; il++)
    {
	  bestdiff = tagparam_.TLR_window;
	  best_hit = 0;
	  if(NbGoodTL>0)
	  {
		for(ir=0; ir<NbGoodTR; ir++)
        {
		  if(Good_TR_Id[ir] == Good_TL_Id[il] )
		  {
D 5
			if (tcr_used[ir] == 0)
E 5
I 5
			if (tcr[ir].used == 0)
E 5
			{
			  diff = abs(Good_TL_val[il] - Good_TR_val[ir]);
			  if (diff <= bestdiff)
			  {
                best_hit = ir; /*candidate*/
				bestdiff = diff;
			  }
			}
		  }
		}
	  }
	  if(best_hit != 0)
	  {
D 5
		tagtcfiltered_.Good_T_id[tagtcfiltered_.NBgoodT] = Good_TL_Id[il];
		tagtcfiltered_.Good_T_L_val[tagtcfiltered_.NBgoodT] = Good_TL_val[il];
		tagtcfiltered_.Good_T_R_val[tagtcfiltered_.NBgoodT] = Good_TR_val[best_hit];
E 5
I 5
		tagtcfiltered_[NBgoodT].Good_T_id = Good_TL_Id[il];
		tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TL_val[il];
		tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TR_val[best_hit];
E 5
uthfill(ana->histf, 172,
D 5
tagtcfiltered_.Good_T_L_val[tagtcfiltered_.NBgoodT]-tagtcfiltered_.Good_T_R_val[tagtcfiltered_.NBgoodT],
E 5
I 5
tagtcfiltered_[NBgoodT].Good_T_L_val-tagtcfiltered_[NBgoodT].Good_T_R_val,
E 5
0., 1.);
D 5
		tcl_used[il] = 1;
		tcr_used[best_hit] = 1;			  
		tagtcfiltered_.NBgoodT ++;
E 5
I 5
		tcl[il].used = 1;
		tcr[best_hit].used = 1;			  
		NBgoodT ++;
E 5
	  }
    }
  }

  /*
    here goes peice for unused hits 
    depending on the HW status for L/R PMTs we will recover those with one good PMT
    by assigning the same time for its partner.
  */

  /* lefts first */
  if(NbGoodTL>0)
  {
	for(il=0; il<NbGoodTL; il++)
	{
D 5
	  if(tcl_used[il]==0 && hwstatus_.TR_hw_status[Good_TL_Id[il]]==1)
E 5
I 5
	  if(tcl[il].used==0 && hwstatus_.TR_hw_status[Good_TL_Id[il]]==1)
E 5
	  {
D 5
		if ( tagtcfiltered_.NBgoodT < MAXTC )
E 5
I 5
		if ( NBgoodT < MAXTC )
E 5
		{
D 5
		  tagtcfiltered_.Good_T_id[tagtcfiltered_.NBgoodT] = Good_TL_Id[il];
		  tagtcfiltered_.Good_T_L_val[tagtcfiltered_.NBgoodT] = Good_TL_val[il];
		  tagtcfiltered_.Good_T_R_val[tagtcfiltered_.NBgoodT] = Good_TL_val[il];
		  tcl_used[il] = 1;
		  tagtcfiltered_.NBgoodT ++;
E 5
I 5
		  tagtcfiltered_[NBgoodT].Good_T_id = Good_TL_Id[il];
		  tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TL_val[il];
		  tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TL_val[il];
		  tcl[il].used = 1;
		  NBgoodT ++;
E 5
		}
		else
		{
		  printf("WARN: Number of T-hits exceeds MAXTC\n");
		}
	  }
	}
  }
  /* now do rights */
  if(NbGoodTR>0)
  {
	for(ir=0; ir<NbGoodTR; ir++)
	{
D 5
	  if (tcr_used[ir]==0 && hwstatus_.TL_hw_status[Good_TR_Id[ir]]==1)
E 5
I 5
	  if (tcr[ir].used==0 && hwstatus_.TL_hw_status[Good_TR_Id[ir]]==1)
E 5
	  {
D 5
		if ( tagtcfiltered_.NBgoodT < MAXTC )
E 5
I 5
		if ( NBgoodT < MAXTC )
E 5
		{
D 5
		  tagtcfiltered_.Good_T_id[tagtcfiltered_.NBgoodT] = Good_TR_Id[ir];
		  tagtcfiltered_.Good_T_L_val[tagtcfiltered_.NBgoodT] = Good_TR_val[ir];
		  tagtcfiltered_.Good_T_R_val[tagtcfiltered_.NBgoodT] = Good_TR_val[ir];
		  tcr_used[ir] = 1;
		  tagtcfiltered_.NBgoodT ++;
E 5
I 5
		  tagtcfiltered_[NBgoodT].Good_T_id = Good_TR_Id[ir];
		  tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TR_val[ir];
		  tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TR_val[ir];
		  tcr[ir].used = 1;
		  NBgoodT ++;
E 5
		}
		else
		{
		  printf("WARN: Number of T-hits exceeds MAXTC\n");
		}
	  }
	}
  }

  /* sort by Tid */
  /* NEED IT ???
D 5
  if(tagtcfiltered_.NBgoodT>0) tag_sort_array2_(tagtcfiltered_.Good_T_id,tagtcfiltered_.Good_T_L_val,tagtcfiltered_.Good_T_R_val,tagtcfiltered_.NBgoodT);
  if(tagtcfiltered_.NBgoodT<MAXTC) tagtcfiltered_.Good_T_id(tagtcfiltered_.NBgoodT + 1) = 0;
E 5
I 5
  if(NBgoodT>0) tag_sort_array2_(tagtcfiltered_.Good_T_id,tagtcfiltered_.Good_T_L_val,tagtcfiltered_.Good_T_R_val,NBgoodT);
E 5
  */
I 5
  qsort(tagtcfiltered_, NBgoodT, sizeof(TagTCfiltered_t), (QSORT_PROC_t*)tcfiltered_sort);
  if(NBgoodT<MAXTC) tagtcfiltered_[NBgoodT].Good_T_id = 0;
E 5


/*
printf("tglib(T): %f > %f, %f < %f, %f > %f, %f < %f\n",
xval1,tgcal->TAG_TCUT[0],xval1,tgcal->TAG_TCUT[1],
xval2,tgcal->TAG_TCUT[0],xval2,tgcal->TAG_TCUT[1]);
	if(xval1 > tgcal->TAG_TCUT[0] && xval1 < tgcal->TAG_TCUT[1] &&
       xval2 > tgcal->TAG_TCUT[0] && xval2 < tgcal->TAG_TCUT[1])
    {
      nhit_T ++;
      tag_Tmean[id] = (xval1+xval2)/2.0;
uthfill(ana->histf, 173, tag_Tmean[id], 0., 1.);
	}
*/

  /* exit if no valid T-counter entry */
/*if(nhit_T == 0) goto a900;*/





  /* step 1: process E-counters
   384 bins become 767 bins; 2 hits become one if in adjacent counters and time is close
   input: data in 'ns', see tag_read_from_BOS, tag_filter_out
   output: Echan_hit, n_ech_hits
  */
  
  /* loop over and find overlaps of adjacent E-counters */
  
  k=0;
  if(tagecfiltered_.NBgoodE)
  {
    for (i=0; i<tagecfiltered_.NBgoodE; i++)
    {
      if (usedE[i]) continue; /* make sure we use each hit once */
      for (j=i+1; j<tagecfiltered_.NBgoodE; j++)
      {
	    if (usedE[j] | usedE[i]) continue; /* make sure we use each hit once */
	    if(tagecfiltered_.Good_E_id[j] == tagecfiltered_.Good_E_id[i]+1) /* ...Good_E... means after time window filtering */
        {
	      if( fabs(tagecfiltered_.Good_E_val[j]-tagecfiltered_.Good_E_val[i]) <= tagparam_.adj_E_coinc/2)
          {
	        if( k < MAXEC)
            {
	          Echan_hit[k].id = 2*tagecfiltered_.Good_E_id[i]; /* overlap id, even number from 1 to 767 (even only here !) */
	          Echan_hit[k].t[0] =  tagecfiltered_.Good_E_val[i];
	          Echan_hit[k].t[1] =  tagecfiltered_.Good_E_val[j];
	          Echan_hit[k].status |= E_PRESENT; /* set flag that E counter exist */
	          k++; /* count them */
	          usedE[i]++; /* mark both hits as used */
	          usedE[j]++;
	        }
	        else
            {
	          fprintf(stderr, "TAG -W- Too many E-hits!!\n");
	          break;
	        }
	      }
	    }
      }
    }
  }
  /* now do another loop to copy non-overlap hits */
  if(tagecfiltered_.NBgoodE)
  {
    for (i=0; i<tagecfiltered_.NBgoodE; i++)
    {
      if (usedE[i]) continue; /* make sure we use each hit once */
      if( k < MAXEC)
      {
	    Echan_hit[k].id = 2*tagecfiltered_.Good_E_id[i]-1; /* non-overlap id, odd number */
	    Echan_hit[k].t[0] =  tagecfiltered_.Good_E_val[i];
	    Echan_hit[k].status |= E_PRESENT;	    
	    k++; /* count them */
	    usedE[i]++; /* mark both hits as used */
      }
      else
      {
	    fprintf(stderr, "TAG -W- Too many E-hits!!\n");
	    break;
      }
    }
  }
  
  n_ech_hits = k; /* This is total number of hits, overlap and non-overlap */
  

  /* now we will order hits by id an time */
  
  if(n_ech_hits>1) qsort(Echan_hit, n_ech_hits, sizeof(TAGE_bin_t), (QSORT_PROC_t*)ECh_sort); 





  /* step 2: T-counters processing
     left-right match done already, and in 'ns'; 'id' goes from 1 to 121
  */

  for (i=0; i<MAXTC; i++)
  {
    Tchan_hit[i].id        = 0;
D 5
    Tchan_hit[i].t_mean[0] = 0;
    Tchan_hit[i].t_mean[1] = 0;
E 5
I 5
    Tchan_hit[i].t_mean[0] = 0.;
    Tchan_hit[i].t_mean[1] = 0.;
E 5
    Tchan_hit[i].tl[0]     = 0.;
    Tchan_hit[i].tl[1]     = 0.;
    Tchan_hit[i].tr[0]     = 0.;
    Tchan_hit[i].tr[1]     = 0.;
    Tchan_hit[i].status    = 0;
    usedT[i] = 0;
  }


  /* loop over and find overlaps of adjacent counters */
  k=0;
D 5
  if(tagtcfiltered_.NBgoodT)
E 5
I 5
  if(NBgoodT)
E 5
  {
D 5
    for (i=0; i<tagtcfiltered_.NBgoodT; i++)
E 5
I 5
    for (i=0; i<NBgoodT; i++)
E 5
    {
D 5
      if (usedT[i]) continue; // make sure we use each hit once
      tmean1 = (tagtcfiltered_.Good_T_L_val[i] + tagtcfiltered_.Good_T_R_val[i])/2;
      for (j=i+1; j<tagtcfiltered_.NBgoodT; j++)
E 5
I 5
      if (usedT[i]) continue; /* make sure we use each hit once */
	  /*
printf("Good_T: [%d] %f %f)\n",i,tagtcfiltered_[i].Good_T_L_val,tagtcfiltered_[i].Good_T_R_val);
	  */
      tmean1 = (tagtcfiltered_[i].Good_T_L_val + tagtcfiltered_[i].Good_T_R_val)/2;
      for (j=i+1; j<NBgoodT; j++)
E 5
      {
D 5
	    if (usedT[j] | usedT[i]) continue; //make sure we use each hit once
	    if(tagtcfiltered_.Good_T_id[j] == tagtcfiltered_.Good_T_id[i]+1)
E 5
I 5
	    if (usedT[j] | usedT[i]) continue; /* make sure we use each hit once */
	    if(tagtcfiltered_[j].Good_T_id == tagtcfiltered_[i].Good_T_id+1)
E 5
        {
D 5
	      tmean2 = (tagtcfiltered_.Good_T_L_val[j] + tagtcfiltered_.Good_T_R_val[j])/2;
E 5
I 5
	      tmean2 = (tagtcfiltered_[j].Good_T_L_val + tagtcfiltered_[j].Good_T_R_val)/2;
E 5
	      if( fabs(tmean1-tmean2) < tagparam_.adj_T_coinc/2)
          {
	        if ( k<MAXTC)
            {
D 5
	          Tchan_hit[k].id        = 2*tagtcfiltered_.Good_T_id[i]; //overlap id, even number	    
E 5
I 5
	          Tchan_hit[k].id        = 2*tagtcfiltered_[i].Good_T_id; /* overlap id, even number */
			  /*
printf("tmean=%f %f\n",tmean1,tmean2);
			  */
E 5
	          Tchan_hit[k].t_mean[0] = tmean1;
	          Tchan_hit[k].t_mean[1] = tmean2;
D 5
	          Tchan_hit[k].tl[0]     =  tagtcfiltered_.Good_T_L_val[i];
	          Tchan_hit[k].tl[1]     =  tagtcfiltered_.Good_T_L_val[j];
	          Tchan_hit[k].tr[0]     =  tagtcfiltered_.Good_T_R_val[i];
	          Tchan_hit[k].tr[1]     =  tagtcfiltered_.Good_T_R_val[j];
	          Tchan_hit[k].status   |= TLR_PRESENT;      
E 5
I 5
	          Tchan_hit[k].tl[0]     =  tagtcfiltered_[i].Good_T_L_val;
	          Tchan_hit[k].tl[1]     =  tagtcfiltered_[j].Good_T_L_val;
	          Tchan_hit[k].tr[0]     =  tagtcfiltered_[i].Good_T_R_val;
	          Tchan_hit[k].tr[1]     =  tagtcfiltered_[j].Good_T_R_val;
	          Tchan_hit[k].status   |= TLR_PRESENT;
E 5
	      
	          k++; // count them
	          usedT[i]++; // mark both hits as used
	          usedT[j]++;
	        }
	        else
            {
	          fprintf (stderr, "\n Number of T-hits exceeds MAXTC=%d \n", MAXTC);
	          break; //exit from the loop
	        }
	      }
	    }
      }
    }
  }
  // now do another loop to copy non-overlap hits
D 5
  if(tagtcfiltered_.NBgoodT)
E 5
I 5
  if(NBgoodT)
E 5
  {
D 5
    for (i=0; i<tagtcfiltered_.NBgoodT; i++)
E 5
I 5
    for (i=0; i<NBgoodT; i++)
E 5
    {
      if (usedT[i]) continue; //make sure we use each hit once
      if ( k<MAXTC)
      {
D 5
	    Tchan_hit[k].id = 2*tagtcfiltered_.Good_T_id[i]-1; //non-overlap id, odd number
	    tmean1 = (tagtcfiltered_.Good_T_L_val[i] + tagtcfiltered_.Good_T_R_val[i])/2;
E 5
I 5
	    Tchan_hit[k].id = 2*tagtcfiltered_[i].Good_T_id-1; /* non-overlap id, odd number */
	    tmean1 = (tagtcfiltered_[i].Good_T_L_val + tagtcfiltered_[i].Good_T_R_val)/2;
E 5
	
	    Tchan_hit[k].t_mean[0] = tmean1;
D 5
	    Tchan_hit[k].tl[0]     =  tagtcfiltered_.Good_T_L_val[i];
	    Tchan_hit[k].tr[0]     =  tagtcfiltered_.Good_T_R_val[i];
E 5
I 5
	    Tchan_hit[k].tl[0]     =  tagtcfiltered_[i].Good_T_L_val;
	    Tchan_hit[k].tr[0]     =  tagtcfiltered_[i].Good_T_R_val;
E 5
	    Tchan_hit[k].status   |= TLR_PRESENT;      
	
	    k++; // count them
	    usedT[i]++; // mark hit as used
      }
      else
      {
	    fprintf (stderr, "\n Number of T-hits exceeds MAXTC=%d \n", MAXTC);
	    break; //exit from the loop
      }
    }
  }
  
  n_tch_hits = k; // Thish is total number of hits, overlap and non-overlap
D 5

E 5
I 5
/*
printf("tglib: n_tch_hits=%d\n",n_tch_hits);
*/
E 5
  // now we will order hits by id an time
  
  if(n_tch_hits>1) qsort(Tchan_hit,n_tch_hits,sizeof(TAGT_bin_t),(QSORT_PROC_t*)TCh_sort); 
  




  /*
step 3: pick RF time closest to T-counter time
  */



D 5
  // now do RF correction and fill common/TagTcorr/
E 5
I 5
  /* now do RF correction and fill common/TagTcorr/ */
E 5
  
  for (i=0; i<n_tch_hits; i++)
  {
    tagtcorr_.Mean_T_val[0][i] = Tchan_hit[i].t_mean[0] - tagcival_.TagTCi[Tchan_hit[i].id-1];
    tagtcorr_.Diff_T_val[0][i] = (Tchan_hit[i].tr[0] - Tchan_hit[i].tl[0])/2; // for calibration purposes only
    tagtcorr_.T_time[0][i]     =  rf_corr_time(tagtcorr_.Mean_T_val[0][i]);
D 5
    if( Tchan_hit[i].id%2 == 0) //overlap
E 5
I 5
    if( Tchan_hit[i].id%2 == 0) /*overlap*/
E 5
    {
      tagtcorr_.Mean_T_val[1][i] = Tchan_hit[i].t_mean[1] - tagcival_.TagTCi[Tchan_hit[i].id-1];
      tagtcorr_.Diff_T_val[1][i] = (Tchan_hit[i].tr[1] - Tchan_hit[i].tl[1])/2;
      tagtcorr_.T_time[1][i]     =  rf_corr_time(tagtcorr_.Mean_T_val[1][i]);
    }
  }


I 5

E 5
  /* step 4
   */



  /* correction for T-counters 18,19 and 20 (tripple coins possible; 20 is first big one) */
  for (i=0; i<n_tch_hits; i++)
  {
    if (Tchan_hit[i].id <35) continue;
    if (Tchan_hit[i].id >39) break;
    if (Tchan_hit[i].id == 35 || Tchan_hit[i].id == 36)
    {
      for (j=i+1; j<n_tch_hits; j++)
      {
	    if (Tchan_hit[j].id >39) break;
	    if (Tchan_hit[j].id == 39 &&
            fabs(Tchan_hit[i].t_mean[0]-Tchan_hit[j].t_mean[0]) < tagparam_.adj_T_coinc/2)
                 Tchan_hit[j].status = 0; // reset status for T20 
      }
    }
    else if (Tchan_hit[i].id == 38) Tchan_hit[i].id == 37;
  }



  /*
step 5
  */

  // now we can do ET matching
  
  // reset arrays
  
  for (i=0; i<MAXTC; i++) usedT[i] = 0;
  for (i=0; i<MAXEC; i++) usedE[i] = 0;
  
  for (i=0; i<MAXECTC; i++)
  {
    ET_coincidence[i].tid    = 0;
    ET_coincidence[i].eid    = 0;
    ET_coincidence[i].Thit   = -1;
    ET_coincidence[i].Ehit   = -1;
    ET_coincidence[i].status = 0;
  }
  
D 5
  k = 0;
E 5

  //  fprintf(stderr," ET_window %f \n",tagparam_.ET_window);

I 5
  k = 0;
E 5
  for (i=0; i<n_ech_hits; i++)
  {   
D 5
    eid = (Echan_hit[i].id + 1)/2; // we need to unbin them back eid is eithe first of the overlap or single
E 5
I 5
    eid = (Echan_hit[i].id + 1)/2; /* we need to unbin them back eid is eithe first of the overlap or single */
E 5
    
    if(1) /* do ET geometry matching */
	{      
D 5
      TCH_min = ectcmaps_.tchmin[eid-1]; // tchmin and tchmax are possible T-counter partners for this E counter
E 5
I 5
      TCH_min = ectcmaps_.tchmin[eid-1]; /* tchmin and tchmax are possible T-counter partners for this E counter */
E 5

      if(Echan_hit[i].id%2) 
D 5
        TCH_max=ectcmaps_.tchmax[eid-1]; // single E
E 5
I 5
        TCH_max=ectcmaps_.tchmax[eid-1]; /* single E */
E 5
      else 
D 5
	    TCH_max=ectcmaps_.tchmax[eid-1+1]; // overlap of two Es
E 5
I 5
	    TCH_max=ectcmaps_.tchmax[eid-1+1]; /* overlap of two Es */
E 5
    }
    else  /* do not do ET geometry matching */
	{
      TCH_min = 1;
      TCH_max = 121;
    }
    
D 5
    for (j=0; j<n_tch_hits; j++) // loop over T hits
E 5
I 5
    for (j=0; j<n_tch_hits; j++) /* loop over T hits */
E 5
    {
D 5
      // check geometry first
      if(Tchan_hit[j].id > TCH_max) break;     //get out of the loop ( we have sorted list !)
      if(Tchan_hit[j].id < TCH_min) continue;  //not there yet
E 5
I 5
	  /*
printf("ET: [%d] %d (%d %d ) %f %f (%f)\n",j,Tchan_hit[j].id,TCH_min,TCH_max,Echan_hit[i].t[0],Tchan_hit[j].t_mean[0],tagparam_.ET_window/2);
	  */
      /* check geometry first */
      if(Tchan_hit[j].id > TCH_max) break;     /* get out of the loop ( we have sorted list !) */
      if(Tchan_hit[j].id < TCH_min) continue;  /* not there yet */
E 5
      
D 5
      // geometry matched, check timing
E 5
I 5
      /* geometry matched, check timing */
E 5

D 5
      if(fabs(Echan_hit[i].t[0] - Tchan_hit[j].t_mean[0]) < tagparam_.ET_window/2) //ET_window=20ns
E 5
I 5
      if(fabs(Echan_hit[i].t[0] - Tchan_hit[j].t_mean[0]) < tagparam_.ET_window/2) /*ET_window=20ns*/
E 5
      {
	    if ( k<MAXECTC )
        {
	      ET_coincidence[k].tid     = Tchan_hit[j].id;
	      ET_coincidence[k].eid     = Echan_hit[i].id;
D 5
	      ET_coincidence[k].Thit    = j; /* pointers to extrach timing later */
E 5
I 5
	      ET_coincidence[k].Thit    = j; /* pointers to extract timing later */
E 5
	      ET_coincidence[k].Ehit    = i; /* */
	      ET_coincidence[k].status |= Tchan_hit[j].status;
	      ET_coincidence[k].status |= Echan_hit[i].status;

	      usedT[j]++;
	      usedE[i]++;
	      k++;
	    }
	    else
        {	  
	      fprintf(stderr, "Too many ET coincidences\n");
	      break;
	    }
      }
    }
  }
D 5
  
E 5
  n_et_coin_hits = k;
I 5
D 8
/*printf("tglib: n_et_coin_hits=%d\n",n_et_coin_hits);*/
E 8
I 8
  /*printf("================================== tglib: n_et_coin_hits=%d\n",n_et_coin_hits);*/
E 8
E 5

  /* loop over found coincidences to check how many times each hit was used and update status accordingly */

  for (i=0; i<n_et_coin_hits; i++)
  {
    if (n_et_coin_hits > 1) ET_coincidence[i].status |= MULTIPLE_HITS;
    if (usedE[ET_coincidence[i].Ehit] != 1)  ET_coincidence[i].status |= MULTIPLE_T;
    if (usedT[ET_coincidence[i].Thit] != 1)  ET_coincidence[i].status |= MULTIPLE_E;
  }



  /*
step6 : output
  */

I 6
D 8
  /*take energy from our config file*/
  tagruncontrol_.Beam_energy=sdakeys_.zbeam[1];
E 8
I 8
  /*take energy from our config file (must be in MeV)*/
  tagruncontrol_.Beam_energy=sdakeys_.zbeam[1]*1000.;
E 8
  /*printf("tagruncontrol_.Beam_energy=%f (%f)\n",tagruncontrol_.Beam_energy,sdakeys_.zbeam[1]);*/
E 6
D 5
printf("tglib: n_et_coin_hits=%d\n",n_et_coin_hits);
E 5

  /* fill TAGR bank */
  if(n_et_coin_hits)
  {
    /*dropAllBanks(&bcs_,"TAGR");*/
I 5
    TAGR = &Tagr; /* do not fill bos bank, use local structure instead */
E 5
    if (1/*TAGR = makeBank(&bcs_, "TAGR", 1, sizeof(tagr_t)/4, n_et_coin_hits)*/)
    {
      for (i=0; i<n_et_coin_hits; i++)
      {
		/*[1]*/TAGR->tagr[i].t_id = ET_coincidence[i].tid;
	    /*[0]*/TAGR->tagr[i].e_id = ET_coincidence[i].eid;
	    /*[5] (without tag2tof ??)*/TAGR->tagr[i].ttag = tagtcorr_.Mean_T_val[0][ET_coincidence[i].Thit] + tagtotof_.tag2tof; /*offset between tagger and tof*/
	    /*[6]*/TAGR->tagr[i].tpho = tagtcorr_.T_time[0][ET_coincidence[i].Thit] + tagtotof_.tag2tof;
	    /*no*/TAGR->tagr[i].stat = ET_coincidence[i].status;
	    /*[2]*/TAGR->tagr[i].erg  = tagruncontrol_.Beam_energy*tagenergyboundaries_.Eaverage[ET_coincidence[i].eid-1]/1000;
D 6
	    /*[2]*/TAGR->tagr[i].erg *= ecor_table_.ecor[TAGR->tagr[i].e_id-1]; /* energy correction (optional) */
E 6
I 6
	    /*[2]*//*TAGR->tagr[i].erg *= ecor_table_.ecor[TAGR->tagr[i].e_id-1]*/; /* energy correction (optional) */
E 6
      }

D 5
      /* fill old sda array */
      ntagh = MIN(10,n_et_coin_hits);
      for (i=0; i<ntagh; i++)
E 5
I 5
      /* fills output array */
D 8
      *ntagh = MIN(10,n_et_coin_hits);
/*printf("tglib: *ntagh=%d\n",*ntagh);*/
      for (i=0; i<*ntagh; i++)
E 8
I 8
      *ntagh = MIN(TAG_MAX_HITS,n_et_coin_hits);
      /*printf("=============================== tglib: *ntagh=%d\n",*ntagh);*/
      for (i=0; i<(*ntagh); i++)
E 8
E 5
      {
        tag_res[i][0] = TAGR->tagr[i].e_id;
        tag_res[i][1] = TAGR->tagr[i].t_id;
        tag_res[i][2] = TAGR->tagr[i].erg; /* OR shell we send corrected one ? not ready anyway ... */
        tag_res[i][3] = 0; /* ??? */
        tag_res[i][4] = 0; /* ??? */
        tag_res[i][5] = TAGR->tagr[i].ttag;
        tag_res[i][6] = TAGR->tagr[i].tpho;
D 8
	  }
E 8
I 8
      }
E 8

    }
  }



/*old prints, need to adjust for new arrays/structures etc
E 4
a900:

  if(sdakeys_.lprnt[2] > 0)
  {
    printf("tglib info:  Ebin   Tbin    Een    Ten   Etime  Ttime  rf_corr\n");
    for(i=1; i<=(*ntagh); i++)
    {
      printf("          %7.3f%7.3f%7.3f%7.3f%7.3f%7.3f%7.3f\n",
         tag_res[i-1][0],tag_res[i-1][1],tag_res[i-1][2],tag_res[i-1][3],
         tag_res[i-1][4],tag_res[i-1][5],tag_res[i-1][6]);
    }
    for(i=1; i<=nhits1; i++)
    {
      printf("hit %d %7.3f%7.3f%7.3f%7.3f%7.3f\n",
          i,tag_Tbin[i-1],tag_Ebin[i-1],tag_Ttime[i-1],
          tag_Etime[i-1],tag_Tdiff[i-1]);
    }
  }
I 4
*/
E 4

  return;
}



/*
   Purpose and Methods : 
     read tagger calibration files and fill TGCALIB structure

   Inputs  : irun  = run number
   Outputs : none

   Controls:

   Library belongs: 

   Calls: 

   Created   12-JUL-1998   Franz Klein
   C remake by Sergey Boyarinov

   Called by sda_init
*/

void
tginit_(int *run)
{
  tginit(*run);
}

void
tginit(int run)
{
  int i;
D 4
  TGCALIB *tgcal;
  tgcal = (TGCALIB *) &tgcalib.ET_window;
E 4

  debug = 0;
  for(i=0; i<sdakeys_.ndebu; i++)
  {
    if(!strncmp("TGLIB",(char *)&sdakeys_.debu[i][0],5)) {debug = 1; break;}
  }

D 4
  tggetcalib(run,&tgcal->ET_window,&tgcal->TAG_RF_MEAN,&tgcal->TAG_RF_10SIGMA,
             tgcal->TAG_TCUT,&tgcal->RF_OFFSET,tgcal->RF_SLOPE,tgcal->Tbound,
             tgcal->Ebound,tgcal->Eaver,tgcal->Edev,tgcal->tagT_status,
             tgcal->etmin,tgcal->etmax,tgcal->t1bin,tgcal->t2bin,
             tgcal->slopeTL,tgcal->slopeTR,tgcal->TpeakL,tgcal->TpeakR,
             tgcal->TagTCi,&tgcal->slopeE,tgcal->Epeak);
E 4
I 4
  tggetcalib(run);
E 4

  return;
}


/* returns tagger constants from MAP */

void
D 4
tggetcalib_(int *runno, float *ET_window, float *TAG_RF_MEAN,
            float *TAG_RF_10SIGMA, float TAG_TCUT[2], float *RF_OFFSET,
            float RF_SLOPE[2], float Tbound[122], float Ebound[768],
            float Eaver[768], float Edev[767], int tagT_status[61],
            int etmin[384], int etmax[384], int t1bin[61], int t2bin[61],
            float slopeTL[61], float slopeTR[61], float TpeakL[61],
            float TpeakR[61], float TagTCi[121], float *slopeE,
            float Epeak[384])
E 4
I 4
tggetcalib_(int *runno)
E 4
{
D 4
  tggetcalib(*runno,ET_window,TAG_RF_MEAN,TAG_RF_10SIGMA,TAG_TCUT,RF_OFFSET,
             RF_SLOPE,Tbound,Ebound,Eaver,Edev,tagT_status,etmin,etmax,
             t1bin,t2bin,slopeTL,slopeTR,TpeakL,TpeakR,TagTCi,slopeE,Epeak);
E 4
I 4
  tggetcalib(*runno);
E 4
}

#define MAXSTRLNG 1000

void
D 4
tggetcalib(int run, float *ET_window, float *TAG_RF_MEAN,
            float *TAG_RF_10SIGMA, float TAG_TCUT[2], float *RF_OFFSET,
            float RF_SLOPE[2], float Tbound[122], float Ebound[768],
            float Eaver[768], float Edev[767], int tagT_status[61],
            int etmin[384], int etmax[384], int t1bin[61], int t2bin[61],
            float slopeTL[61], float slopeTR[61], float TpeakL[61],
            float TpeakR[61], float TagTCi[121], float *slopeE,
            float Epeak[384])
E 4
I 4
tggetcalib(int run)
E 4
{
  FILE *fd;
  char map[1000], dat[1000], fname[1000], *dir, str[MAXSTRLNG], *s;
D 4
  float val, val1, ADJ_T_COINC, ADJ_E_COINC, E_TDC_MIN, E_TDC_MAX;
  float xmin, xmax, TAG_DSD_window, TAG_ST_window, TAG_DSD_Xtrawindow;
  float TAG_ST_Xtrawindow, PS_COINC_WINDOW, a[10], buf[10000];
E 4
I 4
  float val, a[10], buf[10000];
E 4
  int nread, i, j, k, j1, j2, j3, j4, j5;
D 7
  int nline, nmaps, badline=0, comment, dim, histid, nbin;
  char mapname[100], setname[100], subsetname[100], type[100];
E 7

D 4

E 4
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("tggetcalib: CLON_PARMS not defined !!!\n");
    exit(0);
  }

D 4
  for(i=0; i<61; i++)
  {
    t1bin[i] = 0;
    t2bin[i] = 0;
    tagT_status[i] = 0;
  }
  tagT_status[53] = 2;
  tagT_status[60] = 2;
E 4
I 4
  /*************************************/
  /* T-counter partners for E-counters */
E 4

I 4
  /* NOTE: do not forget to extract the part of 'tagETcoinc_ALL.dat' with
	 appropriate E0 and place it into 'tagETcoinc.dat'; in a future should
     be reading 'tagETcoinc_ALL.dat' directly and pick closest energy */
E 4
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagETcoinc");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

D 4

E 4
  if((fd=fopen(fname,"r")) != NULL)
  {
D 4
    for(i=1; i<=384; i++)
E 4
I 4
    for(i=0; i<384; i++)
E 4
    {
D 4
      int i1,i2;
      nread =
        fscanf(fd,"%d%d%d",&k,&i1,&i2);
      if(nread != 3)
E 4
I 4
      int i1,i2,i3,i4,i5,i6;
      nread = fscanf(fd,"%d%d%d%d%d%d%d",&k,&i1,&i2,&i3,&i4,&i5,&i6);
      if(nread != 7)
E 4
      {
D 4
        printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
        printf("tggetcalib: error 1 reading data file - exit\n");
E 4
        exit(0);
      }

D 4
      etmin[k-1] = i1;
      etmax[k-1] = i2;
      for(j=etmin[i-1]; j<=etmax[i-1]; j++)
      {
        if(t1bin[j-1] == 0) t1bin[j-1] = i;
        if(t2bin[j-1] < i)  t2bin[j-1] = i;
      }
E 4
I 4
      j = k - 1;
      ectcmaps_.tmin[j] = i1;
      ectcmaps_.tmax[j] = i2;
      ectcmaps_.t1min[j] = i3;
      ectcmaps_.t1max[j] = i4;
      ectcmaps_.tchmin[j] = i5;
      ectcmaps_.tchmax[j] = i6;
E 4
    }
    fclose(fd);
  }
  else
  {
    printf("tggetcalib: cannot open data file - exit\n");
    exit(0);
  }

I 4

  /**********************/
  /* general parameters */

E 4
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagparam");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
D 4
    for(i=0; i<10; i++) 
E 4
I 4
    for(i=0; i<11; i++) 
E 4
    {
      s=fgets(str,1000,fd);
      if (s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
D 4
          printf("tggetcalib: Error reading data file - exit\n");
E 4
I 4
          printf("tggetcalib: Error 2 reading data file - exit\n");
E 4
          perror("tggetcalib: Error in fgets()");
        }
		if(feof(fd))   /* check for End-Of-File */
        {
          printf("tggetcalib: EOF during reading data file - exit\n");
        }
        exit(0);
      }
	  /*printf("tggetcalib: String %s",str);*/
	  nread = sscanf(str,"%f",&a[i]);
	  /*printf("tggetcalib: a[%d]=%f\n",i,a[i]);*/		
    }

    fclose(fd);

D 4
    ADJ_T_COINC          = a[0];
    ADJ_E_COINC          = a[1];
    *ET_window           = a[2];
    E_TDC_MIN            = a[3];
    E_TDC_MAX            = a[4];
    TAG_DSD_window       = a[5];
    TAG_ST_window        = a[6];
    TAG_DSD_Xtrawindow   = a[7];
    TAG_ST_Xtrawindow    = a[8];
    PS_COINC_WINDOW      = a[9];
E 4
I 4
    tagparam_.adj_T_coinc        = a[0];
    tagparam_.adj_E_coinc        = a[1];
    tagparam_.ET_window          = a[2];
    tagparam_.E_tdc_min          = a[3];
    tagparam_.E_tdc_max          = a[4];
    tagparam_.TAG_DSD_window     = a[5];
    tagparam_.TAG_ST_window      = a[6];
    tagparam_.TAG_DSD_Xtrawindow = a[7];
    tagparam_.TAG_ST_Xtrawindow  = a[8];
    tagparam_.PS_coinc_window    = a[9];
    tagparam_.TLR_window         = a[9];
E 4
  }
  else
  {
    printf("tggetcalib: cannot open data file - exit\n");
    exit(0);
  }
D 4
  *ET_window = *ET_window / 2.;
E 4

D 4
  /* define Time Cuts and Time windows */  
E 4

I 4

  /* define Time Cuts and Time windows
E 4
  *TAG_RF_MEAN = sdakeys_.zcut[8];
  *TAG_RF_10SIGMA =  15.;
D 4

E 4
  TAG_TCUT[0] = 20.;
  TAG_TCUT[1] = 40.;
D 4

E 4
  *RF_OFFSET   = 1.82;
  RF_SLOPE[0] = 4.906E-2;
  RF_SLOPE[1] = 4.906E-2;
I 4
  */
E 4

I 4

  /*************************************************/
  /* TODO: read 'tagT-boundaries_ALL.dat' directly */

E 4
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagT-boundaries");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
    nread = fscanf(fd,"%d%f%f%f",
D 4
      &k,&Tbound[2],&val,&Tbound[0]);
E 4
I 4
      &k,&tagenergyboundaries_.Tboundary[2],&val,&tagenergyboundaries_.Tboundary[0]);
E 4
    if(nread != 4)
    {
D 4
      printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
      printf("tggetcalib: error 3 reading data file - exit\n");
E 4
      exit(0);
    }

    for(i=2; i<=60; i++)
    {
      nread = fscanf(fd,"%d%f%f%f",
D 4
        &k,&Tbound[2*i+1-1],&val,&Tbound[2*i-2-1]);
E 4
I 4
        &k,&tagenergyboundaries_.Tboundary[2*i+1-1],&val,&tagenergyboundaries_.Tboundary[2*i-2-1]);
E 4
      if(nread != 4)
      {
D 4
        printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
        printf("tggetcalib: error 4 reading data file - exit\n");
E 4
        exit(0);
      }
    }

    nread = fscanf(fd,"%d%f%f%f",
D 4
      &k,&Tbound[121],&val,&Tbound[119]);
E 4
I 4
      &k,&tagenergyboundaries_.Tboundary[121],&val,&tagenergyboundaries_.Tboundary[119]);
E 4
    if(nread != 4)
    {
D 4
      printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
      printf("tggetcalib: error 5 reading data file - exit\n");
E 4
      exit(0);
    }
  }

I 4
  /***************************************************/
  /** TODO: read 'tagE-boundaries_ALL.dat' directly **/

E 4
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagE-boundaries");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

D 4
/* if Eaver[..-1] then TBT efficiency 5% up ... */
E 4
  if((fd=fopen(fname,"r")) != NULL)
  {
D 4
    /*Ebound[-1]=Tbound[0] + 0.01;*/
    nread = fscanf(fd,"%d%d%d%d%d%f%f%f%f",
      &j1,&j2,&j3,&j4,&j5,
      &Eaver[1],&Edev[0],
      &Ebound[1],&Ebound[0]);
    if(nread != 9)
E 4
I 4
    nread = fscanf(fd,"%d%f%f%f%f",&j1,
                   &tagenergyboundaries_.Eboundary[1],&tagenergyboundaries_.Eboundary[0],
				   &tagenergyboundaries_.Eaverage[0],&tagenergyboundaries_.Edeviation[0]);
    if(nread != 5)
E 4
    {
D 4
      printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
      printf("tggetcalib: error 6 reading data file - exit\n");
E 4
      exit(0);
    }

D 4
    for(i=2; i<=767; i+=2)
E 4
I 4
    for(i=1; i<767; i++)
E 4
    {
D 4
      nread = fscanf(fd,"%d%d%d%d%d%f%f%f%f%f",
        &j1,&j2,&j3,&j4,&j5,&val,
        &Eaver[i],&Edev[i-1],
        &Ebound[i],&val1);
      if(nread != 10)
E 4
I 4
      nread = fscanf(fd,"%d%f%f%f%f",&j1,
                     &tagenergyboundaries_.Eboundary[i+1],&val,
				     &tagenergyboundaries_.Eaverage[i],&tagenergyboundaries_.Edeviation[i]);
      if(nread != 5)
E 4
      {
D 4
        printf("tggetcalib: error reading data file - exit\n");
E 4
I 4
        printf("tggetcalib: error 7 reading data file - exit\n");
E 4
        exit(0);
      }
D 4

      nread = fscanf(fd,"%d%d%d%d%d%f%f%f%f",
        &j1,&j2,&j3,&j4,&j5,
        &Eaver[i+1],&Edev[i],
        &Ebound[i+1],&val1);
      if(nread != 9)
      {
        printf("tggetcalib: error reading data file - exit\n");
        exit(0);
      }
E 4
    }
D 4
    /*Ebound[768] = Tbound[120] - 0.01;*/
E 4
  }

D 4
  if(debug)
  {
    printf("tggetcalib: using ASCII calibration format\n");
    sprintf(dat,"%s/cmon/tglib/%s.dat",dir,"TAG_CALIB");
    printf("tggetcalib: reading calibration file >%s<\n",dat); fflush(stdout);
E 4

D 4
    if((fd=fopen(dat,"r")) != NULL)
    {
      nline = 0;
      nmaps = 0;
      do 
      {
        s = fgets(str,MAXSTRLNG,fd);
        nline++;
        if(s == NULL)
        {
          if(ferror(fd)) /* check for errors */
          {
            printf("tggetcalib: Error reading config file, line # %i\n",nline);
            perror("tggetcalib: Error in fgets()");
          }
          if(feof(fd))   /* check for End-Of-File */
          {
            printf("tggetcalib: Found end of config file, line # %i\n",nline);
          }
          break;         /* Stop reading */
        }
        str[strlen(str)-1] = 0;
        comment = (*str==0 || *str=='#' || *str=='!' || *str==';');
        badline = 0;
        printf("String [%3d] >%s<\n",nline,str);
        if(!comment)
        {
	      nread = sscanf(str,"%s %s %s %i %s %i %i %f %f",
            mapname,setname,subsetname,&dim,type,&histid,&nbin,&xmin,&xmax);
	      badline = (nread != 9);
	      if(badline)
          {
	  	    printf("tggetcalib: Error pasing line # %i ;"
                   " nItems=%i Nfields=%i ; \n"
				   "Line was:\"%s\"\n",nline,nread,9,str);
		    break;
		  }
          else
          {
            printf("dim=%d\n",dim); fflush(stdout);
            for(i=0; i<dim; i++)
            {
              fscanf(fd,"%f",&buf[i]);
              printf("buf[%3d]=%f\n",i,buf[i]); fflush(stdout);
              /*uthbook1(ana->histf,histid+i,mapname,nbin,xmin,xmax);*/
            }
E 4
I 4
  /****************************************/
  /* read calibration constants from Maps */
  /****************************************/
E 4

D 4
            if(!strcmp(subsetname,"slope_left"))
            {
              for(i=0; i<dim; i++) slopeTL[i] = buf[i];
            }
            else if(!strcmp(subsetname,"slope_right"))
            {
              for(i=0; i<dim; i++) slopeTR[i] = buf[i];
            }
            else if(!strcmp(subsetname,"dt_left"))
            {
              for(i=0; i<dim; i++) TpeakL[i] = buf[i];
            }
            else if(!strcmp(subsetname,"dt_right"))
            {
              for(i=0; i<dim; i++) TpeakR[i] = buf[i];
            }
            else if(!strcmp(subsetname,"ci"))
            {
              for(i=0; i<dim; i++) TagTCi[i] = buf[i];
            }
            else if(!strcmp(subsetname,"slope"))
            {
              *slopeE = buf[0];
            }
            else if(!strcmp(subsetname,"dt"))
            {
              for(i=0; i<dim; i++) Epeak[i] = buf[i];
            }
            else
            {
              printf("tggetcalib: unknown subset name >%s< - exit.\n",subsetname);
            }
          }
	    }
        else
        {
		  printf("tggetcalib: Found comment at line # %i\n",nline);
	    }		  
      } while(1);
E 4
I 4
  printf("tggetcalib: reading calibration constants from MAP\n");
E 4

D 4
      fclose(fd);
    }
E 4
I 4
D 5
  sprintf(map,"%s/cmon/tglib/%s.map",dir,"TAG_CALIB");
  printf("tggetcalib: reading calibration file >%s< for run # %d\n",map,run);
E 5
I 5
  sprintf(map,"%s/Maps/%s.map",dir,"TAG_CALIB");
  if((fd=fopen(map,"r")) != NULL)
  {
    fclose(fd);
D 6
    printf("tggetcalib: reading calibration file >%s< for run # %d\n",map,run);
E 6
I 6
    printf("tggetcalib: reading calibration map >%s< for run # %d\n",map,run);
E 6
  }
  else
  {
D 6
    printf("tggetcalib: error opening map file - exit\n");
E 6
I 6
    printf("tggetcalib: error opening map file >%s< - exit\n",map);
E 6
    exit(0);
  }
E 5
E 4

D 4
  }
  else
  {
    printf("tggetcalib: using MAP calibration format\n");
    sprintf(map,"%s/cmon/tglib/%s.map",dir,"TAG_CALIB");
    printf("tggetcalib: reading calibration file >%s< for run # %d\n",map,run);
E 4
I 4
  /* T TDC constants */
E 4

D 4
    map_get_float(map,"tag_t","slope_left",61,slopeTL,run,&k);
    map_get_float(map,"tag_t","slope_right",61,slopeTR,run,&k);
    map_get_float(map,"tag_t","dt_left",61,TpeakL,run,&k);
    map_get_float(map,"tag_t","dt_right",61,TpeakR,run,&k);
    map_get_float(map,"tag_t","ci",121,TagTCi,run,&k);
E 4
I 4
  tagtdccal_.TagTDCCalTMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_t","slope_left",61,tagtdccal_.TCchWidthL,run,&k);
  map_get_float(map,"tag_t","slope_right",61,tagtdccal_.TCchWidthR,run,&k);
E 4

D 4
    map_get_float(map,"tag_e","slope",1,slopeE,run,&k);
    map_get_float(map,"tag_e","dt",384,Epeak,run,&k);
E 4
I 4
  /* E TDC constant */
E 4

D 4
    /*map_get_float(map,"tag2tof","value",1,&tag2tof,run,&k);*/
E 4
I 4
  tagtdccal_.TAGTDCCalEMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_e","slope",1,&tagtdccal_.ECchWidth,run,&k);

  /* Ci constants */

  tagcival_.TagCivalMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_t","ci",121,tagcival_.TagTCi,run,&k);

  /* T peaks positions constants */

  tagtdcbasepeak_.tagTDCbaseTpeakMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_t","dt_left",61,tagtdcbasepeak_.posTpeakL,run,&k);
  map_get_float(map,"tag_t","dt_right",61,tagtdcbasepeak_.posTpeakR,run,&k);

  /* E peaks position constants */

  tagtdcbasepeak_.tagTDCbaseEpeakMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_e","dt",384,tagtdcbasepeak_.posEpeak,run,&k);

  /* Hardware status */

  map_get_int(map,"status","tage",      384, hwstatus_.E_hw_status,run, &k);
  map_get_int(map,"status","tagt_left",  61, hwstatus_.TL_hw_status, run, &k);
  map_get_int(map,"status","tagt_right", 61, hwstatus_.TR_hw_status, run, &k);

  /* overall alignment of the tagger by respect to the TOF */

  map_get_float(map,"tag2tof","value",1,&tagtotof_.tag2tof,run,&k);

  /* energy correction table */

  /*for(i=0; i<767; i++) ecor_table_.ecor[i] = 1.;*/
  map_get_float(map,"tag_e","ecor",767,ecor_table_.ecor,run,&k);



  /* fill some histograms
  for(i=0; i<61; i++)
  {
    uthfill(ana->histf, 161, tagtdccal_.TCchWidthL[i], 0., 1.);
    uthfill(ana->histf, 162, tagtdccal_.TCchWidthR[i], 0., 1.);
    uthfill(ana->histf, 163, tagtdcbasepeak_.posTpeakL[id], 0., 1.);
    uthfill(ana->histf, 164, tagtdcbasepeak_.posTpeakR[id], 0., 1.);
E 4
  }
I 4
  */
E 4

I 4

  /*
E 4
  for(i=0; i<61; i++)
  {
    slopeTR[i] = slopeTR[i] / 1000.;
    slopeTL[i] = slopeTL[i] / 1000.;
  }
  *slopeE = - *slopeE / 1000.;
I 4
  */
E 4

  /*
  printf("tglib: T-counters:  t1bin     t2bin    Tpeakl    Tpeakr\n");
  for(i=0; i<61; i++)
  {
    printf("%25d%10d%10d%10d\n",t1bin[i],t2bin[i],
                                TpeakL[i],TpeakR[i]);
  }
  */

  return;
}


/* calibration histogramming */

void
tghbookcalib_(TRevent *ana)
{
  tghbookcalib(ana);
}

void
tghbookcalib(TRevent *ana)
{
  int i, nline, nread, dim, histid, comment, badline, nbin;
  float xmin, xmax, tmp;
  FILE *fd;
  char fname[1000], *dir, str[MAXSTRLEN], *s;
  char libname[10], mapname[100], setname[100], subsetname[100], type[100];

I 7
  /* return if debug flag do not set */
  if(debug==0) return;

E 7
  printf("tghbookcalib reached\n"); fflush(stdout);
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("tghbookcalib: CLON_PARMS not defined !!!\n");
    exit(0);
  }

D 7
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"TAG_CALIB");
  printf("tghbookcalib: reading config file >%s<\n",fname);

E 7
I 7
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"TAG_HISTS");
E 7
  if((fd=fopen(fname,"r")) != NULL)
  {
I 7
    printf("tghbookcalib: reading config file >%s<\n",fname);
E 7
    nline = 0;
    do
    {
      s = fgets(str,MAXSTRLEN,fd);
      nline++;
      if(s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
D 4
          printf("tghbookcalib: Error reading config file, line # %i\n",nline);
E 4
I 4
          printf("tghbookcalib: Error 8 reading config file, line # %i\n",nline);
E 4
          perror("tghbookcalib: Error in fgets()");
        }
        if(feof(fd))   /* check for End-Of-File */
        {
          printf("tghbookcalib: Found end of config file, line # %i\n",nline);
        }
        break;         /* Stop reading */
      }
      str[strlen(str)-1] = 0;
      comment = (*str==0 || *str=='#' || *str=='!' || *str==';');
      badline = 0;
      printf("String %i: >%s<\n",nline,str);
      if(!comment)
      {
	    nread = sscanf(str,"%s %s %s %i %s %i %i %f %f",
          mapname,setname,subsetname,&dim,type,&histid,&nbin,&xmin,&xmax);
	    badline = (nread != NUMFIELD1);
	    if(badline)
        {
	  	  printf("tghbookcalib: Error pasing line # %i ;"
                 " nItems=%i Nfields=%i ; \n"
				 "Line was:\"%s\"\n",nline,nread,NUMFIELD1,str);
		  break;
		}
D 7
        else
E 7
I 7
        for(i=0; i<=dim; i++)
E 7
        {
D 7
          for(i=0; i<dim; i++)
          {
            fscanf(fd,"%f",&tmp); /* just skip it */
            if(debug && nbin > 0)
                 uthbook1(ana->histf,histid+i,mapname,nbin,xmin,xmax);
          }
E 7
I 7
          if(nbin > 0) uthbook1(ana->histf,histid+i,mapname,nbin,xmin,xmax);
E 7
        }
	  }
      else
      {
		printf("tghbookcalib: Found comment at line # %i\n",nline);
	  }		  
    } 
	while(1);

    fclose(fd);
  }
I 7
  else
  {
    printf("tghbookcalib: cannot open config file >%s< - exit\n",fname);
    exit(0);
  }
E 7

  return;
}


E 1
