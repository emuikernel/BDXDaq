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
   30-oct-2007: rewrite everything a la offline version, including pipeline TDCs

   Called by dcfinde

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dclib.h"
#include "tglib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;



static int debug; /* set to 1 if TGLIB in a debug list in cmon.config */

#define MAXSTRLEN  1000
#define NUMFIELD1     9

#define MAX_hit      20

#define MOD(x1,x2) ( (x1) - (int)((x1)/(x2)) * (x2) )
#define ABS(x)     ( (x) < 0 ? -(x) : (x) )
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define Tag_max_hit 2000
#define MAXTC   2000
#define MAXEC   2000
#define MAXECTC 2000
#define MAXTAGR 2000
#define MAXHITS 2048

// status bits
#define TL_PRESENT    0x01
#define TR_PRESENT    0x02
#define TLR_PRESENT   0x03
#define E_PRESENT     0x04
#define MULTIPLE_HITS 0x08
#define TRIPPLE_T     0x10
#define MULTIPLE_T    0x20    //E-hit matched to more than one T-hit
#define MULTIPLE_E    0x40    //T-hit matched to more than one E-hit

typedef int QSORT_PROC_t(const void *, const void *); 

/***************************/
/* locally used structures */

/* raw T-conuters data */
typedef struct {
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
  int NBgoodE;
  int Good_E_id[MAXEC];
  float Good_E_val[MAXEC];
} TagECfiltered_t;
TagECfiltered_t tagecfiltered_;

typedef struct {
  int Good_T_id;
  float Good_T_L_val;
  float Good_T_R_val;
} TagTCfiltered_t;
TagTCfiltered_t tagtcfiltered_[MAXTC];
int NBgoodT;

typedef struct {
  int id;
  float t_mean[2];
  float tl[2];
  float tr[2];
  int status;
} TAGT_bin_t;
TAGT_bin_t Tchan_hit[MAXTC];

typedef struct {
  int id;
  float t[2];
  int status;
} TAGE_bin_t;
TAGE_bin_t Echan_hit[MAXEC];

typedef struct {
  int tid;
  int eid; 
  int Thit;
  int Ehit;
  int status;
} ET_hit_t;
ET_hit_t ET_coincidence[MAXECTC];

typedef struct {
  float Mean_T_val[2][MAXTC];
  float RF_T_diff[2][MAXTC];
  float Diff_T_val[2][MAXTC];
  float T_time[2][MAXTC];
} TagTcorr_t;
TagTcorr_t tagtcorr_;






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
  tagr_t tagr[MAXECTC/*1 if real bos bank*/];
} clasTAGR_t;
clasTAGR_t Tagr; /* will not fill bos bank, use local structure instead */

/* following prototypes were used:
tag_read_from_BOS.F (fill arrays by raw data)
tag_filter_translate.F (convert raw data to 'ns')
tag_filter_out.F (convert raw data to 'ns')
tagcal_read_map.F (read constants from maps)
*/


/*************************************************/
/*************************************************/
/********* structures filled by tginit() *********/


/***********************************/
/*********** ASCII files ***********/

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






/****************************/
/*********** Maps ***********/

/* from map */
typedef struct {
  float ECchWidth;
  float TCchWidthL[61];
  float TCchWidthR[61];
  float TagTDCCalTMM;
  float TAGTDCCalEMM;
} tagTDCCal_t;
tagTDCCal_t tagtdccal_;

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

/* from map */
typedef struct {
  float posTpeakL[61];
  float posTpeakR[61];
  float posEpeak[384];
  float tagTDCbaseEpeakMM;
  float tagTDCbaseTpeakMM;
} tagTDCbasePeak_t;
tagTDCbasePeak_t tagtdcbasepeak_;

/* hardware status(map): dead channels etc */
typedef struct {
  int E_hw_status[384];
  int TL_hw_status[61];
  int TR_hw_status[61];
} tagHW_status_t;
tagHW_status_t hwstatus_;

/* from map: offset between tagger and tof */
typedef struct {
  float tag2tof;
} tagtotof_t;
tagtotof_t tagtotof_;

/* from map: energy correction table */
typedef struct {
  float ecor[767];
} tag_ecor_t;
tag_ecor_t ecor_table_;






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














int Tagger_convert = 0; /* ?????????????????????????? */


/**************************************************************************/

float
rf_corr_time(float time)
{
  float rf_st_time = time;

/*temporary until implemented*/
return(time);

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
  {
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
    {
      ih = i16[0];
      if(ih==6)
      {
        bmdigi[0][0] = i16[0];
        bmdigi[0][1] = i16[1];
      }
      i16 += 2;
    }
  }
}

  rf[0] = bmdigi[5][1] * tgcal->RF_SLOPE[0];
  rf[1] = bmdigi[6][1] * tgcal->RF_SLOPE[1];

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
    tag_res[k-1][2] = tgcal->Eaver[tag_Ebin[j-1]];
    tag_res[k-1][3] = (tgcal->Tbound[tag_Tbin[j-1]-1]
                     + tgcal->Tbound[tag_Tbin[j-1]+1-1])/2.;
    tag_res[k-1][4] = tag_Etime[j-1];
    tag_res[k-1][5] = tag_Ttime[j-1] - tgcal->TagTCi[tag_Tbin[j-1]-1];
    xval1 = (tag_res[k-1][5] - rf[2]  - 1.002) / 2.004;
    tag_res[k-1][6] = rf[2] + ((int)(xval1))*2.004;

uthfill(ana->histf, 201, xval1, 0., 1.);
uthfill(ana->histf, 202, tag_res[k-1][6], 0., 1.);

  }
*/



  return rf_st_time;
}

/* sorting functions */
int
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
ECh_sort(TAGE_bin_t *hit1, TAGE_bin_t *hit2)
{
  /* sort by id first */
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
  /* then by time */
  if (hit1->t[0] != hit2->t[0]) return hit1->t[0] - hit2->t[0];
  /* error, identical hits */
  fprintf(stderr, "Ech_sort -E- identiacal hits, wrong !!!!\n");

  return(0);
}
int
TCh_sort(TAGT_bin_t *hit1, TAGT_bin_t *hit2)
{
  /* sort by id first */
  if (hit1->id != hit2->id) return hit1->id - hit2->id;
  /* then by time */
  if (hit1->t_mean[0] != hit2->t_mean[0]) return hit1->t_mean[0] - hit2->t_mean[0];
  /* error, identical hits */
  fprintf(stderr, "Tch_sort -E- identiacal hits, wrong !!!!\n");

  return(0);
}


void
tglib_(int *jw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7])
{
  tglib(jw,ana,ntagh,tag_res);
}

void
tglib(int *jw, TRevent *ana, int *ntagh, float tag_res[TAG_MAX_HITS][7])
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

  int Good_TL_Id[MAXTC], Good_TR_Id[MAXTC], NbGoodTL, NbGoodTR;
  float	Good_TL_val[MAXTC], Good_TR_val[MAXTC];
  float diff, bestdiff;
  int best_hit;

  clasTAGR_t *TAGR = NULL;
  

  /* reset everything before we go any further */
  *ntagh = 0;
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
        tcl[Tgtl_NbHits].id = id;
        tcl[Tgtl_NbHits].val = i16[1];
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
        tcr[Tgtr_NbHits].id = id;
        tcr[Tgtr_NbHits].val = i16[1];
uthfill(ana->histf, 169, (float)i16[1], 0., 1.);
        Tgtr_NbHits ++;
	  }
      i16 += 2;
    }
  }
  /*printf("T-counters data ---\n");
  for(i=0; i<tgtndig; i++)
    printf("[%2d] %5d %5d\n",tgtdigi[i][0],tgtdigi[i][1],tgtdigi[i][2]);*/

  if(Tgtl_NbHits>1) qsort(tcl, Tgtl_NbHits, sizeof(TagTC_t), (QSORT_PROC_t*)tcraw_sort); 
  if(Tgtr_NbHits>1) qsort(tcr, Tgtr_NbHits, sizeof(TagTC_t), (QSORT_PROC_t*)tcraw_sort); 
  

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
        ec[Tag_E_NbHits].id = id;
        ec[Tag_E_NbHits].val = i16[1];
		/*printf("E raw: [%d] id=%d tdc=%d\n",i,id,i16[1]);*/
uthfill(ana->histf, 174, (float)i16[1], 0., 1.);
        Tag_E_NbHits ++;
	  }
      i16 += 2;
    }
  }
  /*printf("Tag_E_NbHits=%d\n",Tag_E_NbHits);*/
  if(Tag_E_NbHits>1) qsort(ec, Tag_E_NbHits, sizeof(TagEC_t), (QSORT_PROC_t*)ecraw_sort); 


  /* Find the number of E counters in a reasonable time window */    
  tagecfiltered_.NBgoodE = 0;
  for(i=0; i<Tag_E_NbHits; i++)
  {
	/*
printf("E-hits: [%d] id=%d tdc=%d (%f %f %d)\n",
i,ec[i].id,ec[i].val,tagparam_.E_tdc_min,tagparam_.E_tdc_max,hwstatus_.E_hw_status[ec[i].id]);
	*/
    if(ec[i].val>tagparam_.E_tdc_min && ec[i].val<tagparam_.E_tdc_max &&
       ec[i].id>0 && ec[i].id<=384 &&
       hwstatus_.E_hw_status[ec[i].id]==0)
	{
	  tagecfiltered_.Good_E_id[tagecfiltered_.NBgoodE] = ec[i].id;
      tagecfiltered_.Good_E_val[tagecfiltered_.NBgoodE] =
        - tagtdccal_.ECchWidth * ((float)ec[i].val)/1000.- tagtdcbasepeak_.posEpeak[ec[i].id];
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
	tcl[i].used = 0;
	tcr[i].used = 0;
  }

  /* convert TGTL to ns */
  NbGoodTL = 0;
  for(i=0; i<Tgtl_NbHits; i++)
  {
	if(hwstatus_.TL_hw_status[tcl[i].id]==0)
	{
	  Good_TL_Id[NbGoodTL] = tcl[i].id;
	  if(Tagger_convert==0) /*old scheme*/
      {
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[tcl[i].id]*((float)tcl[i].val)/1000.
		  - tagtdcbasepeak_.posTpeakL[tcl[i].id];
		/*
printf("==> [%d] (%f) = -(%f)*(%f)/1000.-(%f)\n",
i,Good_TL_val[NbGoodTL],tagtdccal_.TCchWidthL[tcl[i].id],(float)tcl[i].val,tagtdcbasepeak_.posTpeakL[tcl[i].id]);
		*/
	  }
	  else /*new scheme*/
	  {
		Good_TL_val[NbGoodTL] = -tagtdccal_.TCchWidthL[tcl[i].id]/1000.*((float)tcl[i].val)
          - tagtdcbasepeak_.posTpeakL[tcl[i].id];
	  }
/*printf("Tleft: [%d] id=%d tdc=%d val=%f\n",i,tcl[i].id,tcl[i].val,Good_TL_val[NbGoodTL]);*/
uthfill(ana->histf, 170, Good_TL_val[NbGoodTL], 0., 1.);
      if(debug)
      {
        iii=1001+Good_TL_Id[NbGoodTL];
        uthfill(ana->histf, iii, Good_TL_val[NbGoodTL], 0., 1.);
      }
	  NbGoodTL ++;
    }
  }

  /* convert TGTR to ns */
  NbGoodTR = 0;
  for(i=0; i<Tgtr_NbHits; i++)
  {
    if(hwstatus_.TR_hw_status[tcr[i].id]==0)
	{
	  Good_TR_Id[NbGoodTR] = tcr[i].id;
	  if(Tagger_convert==0) /*old scheme*/
	  {
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[tcr[i].id]*((float)tcr[i].val)/1000.
		  - tagtdcbasepeak_.posTpeakR[tcr[i].id];
      }
	  else /*new scheme*/ 
	  {
		Good_TR_val[NbGoodTR] = -tagtdccal_.TCchWidthR[tcr[i].id]/1000.*((float)tcr[i].val)
          - tagtdcbasepeak_.posTpeakR[tcr[i].id];
	  }
uthfill(ana->histf, 171, Good_TR_val[NbGoodTR], 0., 1.);
      if(debug)
      {
        iii=1201+Good_TR_Id[NbGoodTR];
        uthfill(ana->histf, iii, Good_TR_val[NbGoodTR], 0., 1.);
      }
	  NbGoodTR ++;
	}
  }
  /* done with conversion */
  NBgoodT = 0;  

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
			if (tcr[ir].used == 0)
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
		tagtcfiltered_[NBgoodT].Good_T_id = Good_TL_Id[il];
		tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TL_val[il];
		tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TR_val[best_hit];
uthfill(ana->histf, 172,
tagtcfiltered_[NBgoodT].Good_T_L_val-tagtcfiltered_[NBgoodT].Good_T_R_val,
0., 1.);
		tcl[il].used = 1;
		tcr[best_hit].used = 1;			  
		NBgoodT ++;
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
	  if(tcl[il].used==0 && hwstatus_.TR_hw_status[Good_TL_Id[il]]==1)
	  {
		if ( NBgoodT < MAXTC )
		{
		  tagtcfiltered_[NBgoodT].Good_T_id = Good_TL_Id[il];
		  tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TL_val[il];
		  tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TL_val[il];
		  tcl[il].used = 1;
		  NBgoodT ++;
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
	  if (tcr[ir].used==0 && hwstatus_.TL_hw_status[Good_TR_Id[ir]]==1)
	  {
		if ( NBgoodT < MAXTC )
		{
		  tagtcfiltered_[NBgoodT].Good_T_id = Good_TR_Id[ir];
		  tagtcfiltered_[NBgoodT].Good_T_L_val = Good_TR_val[ir];
		  tagtcfiltered_[NBgoodT].Good_T_R_val = Good_TR_val[ir];
		  tcr[ir].used = 1;
		  NBgoodT ++;
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
  if(NBgoodT>0) tag_sort_array2_(tagtcfiltered_.Good_T_id,tagtcfiltered_.Good_T_L_val,tagtcfiltered_.Good_T_R_val,NBgoodT);
  */
  qsort(tagtcfiltered_, NBgoodT, sizeof(TagTCfiltered_t), (QSORT_PROC_t*)tcfiltered_sort);
  if(NBgoodT<MAXTC) tagtcfiltered_[NBgoodT].Good_T_id = 0;


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
    Tchan_hit[i].t_mean[0] = 0.;
    Tchan_hit[i].t_mean[1] = 0.;
    Tchan_hit[i].tl[0]     = 0.;
    Tchan_hit[i].tl[1]     = 0.;
    Tchan_hit[i].tr[0]     = 0.;
    Tchan_hit[i].tr[1]     = 0.;
    Tchan_hit[i].status    = 0;
    usedT[i] = 0;
  }


  /* loop over and find overlaps of adjacent counters */
  k=0;
  if(NBgoodT)
  {
    for (i=0; i<NBgoodT; i++)
    {
      if (usedT[i]) continue; /* make sure we use each hit once */
	  /*
printf("Good_T: [%d] %f %f)\n",i,tagtcfiltered_[i].Good_T_L_val,tagtcfiltered_[i].Good_T_R_val);
	  */
      tmean1 = (tagtcfiltered_[i].Good_T_L_val + tagtcfiltered_[i].Good_T_R_val)/2;
      for (j=i+1; j<NBgoodT; j++)
      {
	    if (usedT[j] | usedT[i]) continue; /* make sure we use each hit once */
	    if(tagtcfiltered_[j].Good_T_id == tagtcfiltered_[i].Good_T_id+1)
        {
	      tmean2 = (tagtcfiltered_[j].Good_T_L_val + tagtcfiltered_[j].Good_T_R_val)/2;
	      if( fabs(tmean1-tmean2) < tagparam_.adj_T_coinc/2)
          {
	        if ( k<MAXTC)
            {
	          Tchan_hit[k].id        = 2*tagtcfiltered_[i].Good_T_id; /* overlap id, even number */
			  /*
printf("tmean=%f %f\n",tmean1,tmean2);
			  */
	          Tchan_hit[k].t_mean[0] = tmean1;
	          Tchan_hit[k].t_mean[1] = tmean2;
	          Tchan_hit[k].tl[0]     =  tagtcfiltered_[i].Good_T_L_val;
	          Tchan_hit[k].tl[1]     =  tagtcfiltered_[j].Good_T_L_val;
	          Tchan_hit[k].tr[0]     =  tagtcfiltered_[i].Good_T_R_val;
	          Tchan_hit[k].tr[1]     =  tagtcfiltered_[j].Good_T_R_val;
	          Tchan_hit[k].status   |= TLR_PRESENT;
	      
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
  if(NBgoodT)
  {
    for (i=0; i<NBgoodT; i++)
    {
      if (usedT[i]) continue; //make sure we use each hit once
      if ( k<MAXTC)
      {
	    Tchan_hit[k].id = 2*tagtcfiltered_[i].Good_T_id-1; /* non-overlap id, odd number */
	    tmean1 = (tagtcfiltered_[i].Good_T_L_val + tagtcfiltered_[i].Good_T_R_val)/2;
	
	    Tchan_hit[k].t_mean[0] = tmean1;
	    Tchan_hit[k].tl[0]     =  tagtcfiltered_[i].Good_T_L_val;
	    Tchan_hit[k].tr[0]     =  tagtcfiltered_[i].Good_T_R_val;
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
/*
printf("tglib: n_tch_hits=%d\n",n_tch_hits);
*/
  // now we will order hits by id an time
  
  if(n_tch_hits>1) qsort(Tchan_hit,n_tch_hits,sizeof(TAGT_bin_t),(QSORT_PROC_t*)TCh_sort); 
  




  /*
step 3: pick RF time closest to T-counter time
  */



  /* now do RF correction and fill common/TagTcorr/ */
  
  for (i=0; i<n_tch_hits; i++)
  {
    tagtcorr_.Mean_T_val[0][i] = Tchan_hit[i].t_mean[0] - tagcival_.TagTCi[Tchan_hit[i].id-1];
    tagtcorr_.Diff_T_val[0][i] = (Tchan_hit[i].tr[0] - Tchan_hit[i].tl[0])/2; // for calibration purposes only
    tagtcorr_.T_time[0][i]     =  rf_corr_time(tagtcorr_.Mean_T_val[0][i]);
    if( Tchan_hit[i].id%2 == 0) /*overlap*/
    {
      tagtcorr_.Mean_T_val[1][i] = Tchan_hit[i].t_mean[1] - tagcival_.TagTCi[Tchan_hit[i].id-1];
      tagtcorr_.Diff_T_val[1][i] = (Tchan_hit[i].tr[1] - Tchan_hit[i].tl[1])/2;
      tagtcorr_.T_time[1][i]     =  rf_corr_time(tagtcorr_.Mean_T_val[1][i]);
    }
  }



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
  

  //  fprintf(stderr," ET_window %f \n",tagparam_.ET_window);

  k = 0;
  for (i=0; i<n_ech_hits; i++)
  {   
    eid = (Echan_hit[i].id + 1)/2; /* we need to unbin them back eid is eithe first of the overlap or single */
    
    if(1) /* do ET geometry matching */
	{      
      TCH_min = ectcmaps_.tchmin[eid-1]; /* tchmin and tchmax are possible T-counter partners for this E counter */

      if(Echan_hit[i].id%2) 
        TCH_max=ectcmaps_.tchmax[eid-1]; /* single E */
      else 
	    TCH_max=ectcmaps_.tchmax[eid-1+1]; /* overlap of two Es */
    }
    else  /* do not do ET geometry matching */
	{
      TCH_min = 1;
      TCH_max = 121;
    }
    
    for (j=0; j<n_tch_hits; j++) /* loop over T hits */
    {
	  /*
printf("ET: [%d] %d (%d %d ) %f %f (%f)\n",j,Tchan_hit[j].id,TCH_min,TCH_max,Echan_hit[i].t[0],Tchan_hit[j].t_mean[0],tagparam_.ET_window/2);
	  */
      /* check geometry first */
      if(Tchan_hit[j].id > TCH_max) break;     /* get out of the loop ( we have sorted list !) */
      if(Tchan_hit[j].id < TCH_min) continue;  /* not there yet */
      
      /* geometry matched, check timing */

      if(fabs(Echan_hit[i].t[0] - Tchan_hit[j].t_mean[0]) < tagparam_.ET_window/2) /*ET_window=20ns*/
      {
	    if ( k<MAXECTC )
        {
	      ET_coincidence[k].tid     = Tchan_hit[j].id;
	      ET_coincidence[k].eid     = Echan_hit[i].id;
	      ET_coincidence[k].Thit    = j; /* pointers to extract timing later */
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
  n_et_coin_hits = k;
  /*printf("================================== tglib: n_et_coin_hits=%d\n",n_et_coin_hits);*/

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

  /*take energy from our config file (must be in MeV)*/
  tagruncontrol_.Beam_energy=sdakeys_.zbeam[1]*1000.;
  /*printf("tagruncontrol_.Beam_energy=%f (%f)\n",tagruncontrol_.Beam_energy,sdakeys_.zbeam[1]);*/

  /* fill TAGR bank */
  if(n_et_coin_hits)
  {
    /*dropAllBanks(&bcs_,"TAGR");*/
    TAGR = &Tagr; /* do not fill bos bank, use local structure instead */
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
	    /*[2]*//*TAGR->tagr[i].erg *= ecor_table_.ecor[TAGR->tagr[i].e_id-1]*/; /* energy correction (optional) */
      }

      /* fills output array */
      *ntagh = MIN(TAG_MAX_HITS,n_et_coin_hits);
      /*printf("=============================== tglib: *ntagh=%d\n",*ntagh);*/
      for (i=0; i<(*ntagh); i++)
      {
        tag_res[i][0] = TAGR->tagr[i].e_id;
        tag_res[i][1] = TAGR->tagr[i].t_id;
        tag_res[i][2] = TAGR->tagr[i].erg; /* OR shell we send corrected one ? not ready anyway ... */
        tag_res[i][3] = 0; /* ??? */
        tag_res[i][4] = 0; /* ??? */
        tag_res[i][5] = TAGR->tagr[i].ttag;
uthfill(ana->histf, 173, TAGR->tagr[i].ttag, 0., 1.);
        tag_res[i][6] = TAGR->tagr[i].tpho;
      }

    }
  }



/*old prints, need to adjust for new arrays/structures etc
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
*/

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

  debug = 0;
  for(i=0; i<sdakeys_.ndebu; i++)
  {
    if(!strncmp("TGLIB",(char *)&sdakeys_.debu[i][0],5)) {debug = 1; break;}
  }

  tggetcalib(run);

  return;
}


/* returns tagger constants from MAP */

void
tggetcalib_(int *runno)
{
  tggetcalib(*runno);
}

#define MAXSTRLNG 1000

void
tggetcalib(int run)
{
  FILE *fd;
  char map[1000], dat[1000], fname[1000], *dir, str[MAXSTRLNG], *s;
  float val, a[10], buf[10000];
  int nread, i, j, k, j1, j2, j3, j4, j5;

  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("tggetcalib: CLON_PARMS not defined !!!\n");
    exit(0);
  }

  /*************************************/
  /* T-counter partners for E-counters */

  /* NOTE: do not forget to extract the part of 'tagETcoinc_ALL.dat' with
	 appropriate E0 and place it into 'tagETcoinc.dat'; in a future should
     be reading 'tagETcoinc_ALL.dat' directly and pick closest energy */
  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagETcoinc");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
    for(i=0; i<384; i++)
    {
      int i1,i2,i3,i4,i5,i6;
      nread = fscanf(fd,"%d%d%d%d%d%d%d",&k,&i1,&i2,&i3,&i4,&i5,&i6);
      if(nread != 7)
      {
        printf("tggetcalib: error 1 reading data file - exit\n");
        exit(0);
      }

      j = k - 1;
      ectcmaps_.tmin[j] = i1;
      ectcmaps_.tmax[j] = i2;
      ectcmaps_.t1min[j] = i3;
      ectcmaps_.t1max[j] = i4;
      ectcmaps_.tchmin[j] = i5;
      ectcmaps_.tchmax[j] = i6;
    }
    fclose(fd);
  }
  else
  {
    printf("tggetcalib: cannot open data file - exit\n");
    exit(0);
  }


  /**********************/
  /* general parameters */

  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagparam");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
    for(i=0; i<11; i++) 
    {
      s=fgets(str,1000,fd);
      if (s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
          printf("tggetcalib: Error 2 reading data file - exit\n");
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
  }
  else
  {
    printf("tggetcalib: cannot open data file - exit\n");
    exit(0);
  }



  /* define Time Cuts and Time windows
  *TAG_RF_MEAN = sdakeys_.zcut[8];
  *TAG_RF_10SIGMA =  15.;
  TAG_TCUT[0] = 20.;
  TAG_TCUT[1] = 40.;
  *RF_OFFSET   = 1.82;
  RF_SLOPE[0] = 4.906E-2;
  RF_SLOPE[1] = 4.906E-2;
  */


  /*************************************************/
  /* TODO: read 'tagT-boundaries_ALL.dat' directly */

  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagT-boundaries");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
    nread = fscanf(fd,"%d%f%f%f",
      &k,&tagenergyboundaries_.Tboundary[2],&val,&tagenergyboundaries_.Tboundary[0]);
    if(nread != 4)
    {
      printf("tggetcalib: error 3 reading data file - exit\n");
      exit(0);
    }

    for(i=2; i<=60; i++)
    {
      nread = fscanf(fd,"%d%f%f%f",
        &k,&tagenergyboundaries_.Tboundary[2*i+1-1],&val,&tagenergyboundaries_.Tboundary[2*i-2-1]);
      if(nread != 4)
      {
        printf("tggetcalib: error 4 reading data file - exit\n");
        exit(0);
      }
    }

    nread = fscanf(fd,"%d%f%f%f",
      &k,&tagenergyboundaries_.Tboundary[121],&val,&tagenergyboundaries_.Tboundary[119]);
    if(nread != 4)
    {
      printf("tggetcalib: error 5 reading data file - exit\n");
      exit(0);
    }
  }

  /***************************************************/
  /** TODO: read 'tagE-boundaries_ALL.dat' directly **/

  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"tagE-boundaries");
  printf("tggetcalib: reading data file >%s< for run # %d\n",fname,run);

  if((fd=fopen(fname,"r")) != NULL)
  {
    nread = fscanf(fd,"%d%f%f%f%f",&j1,
                   &tagenergyboundaries_.Eboundary[1],&tagenergyboundaries_.Eboundary[0],
				   &tagenergyboundaries_.Eaverage[0],&tagenergyboundaries_.Edeviation[0]);
    if(nread != 5)
    {
      printf("tggetcalib: error 6 reading data file - exit\n");
      exit(0);
    }

    for(i=1; i<767; i++)
    {
      nread = fscanf(fd,"%d%f%f%f%f",&j1,
                     &tagenergyboundaries_.Eboundary[i+1],&val,
				     &tagenergyboundaries_.Eaverage[i],&tagenergyboundaries_.Edeviation[i]);
      if(nread != 5)
      {
        printf("tggetcalib: error 7 reading data file - exit\n");
        exit(0);
      }
    }
  }


  /****************************************/
  /* read calibration constants from Maps */
  /****************************************/

  printf("tggetcalib: reading calibration constants from MAP\n");

  sprintf(map,"%s/Maps/%s.map",dir,"TAG_CALIB");
  if((fd=fopen(map,"r")) != NULL)
  {
    fclose(fd);
    printf("tggetcalib: reading calibration map >%s< for run # %d\n",map,run);
  }
  else
  {
    printf("tggetcalib: error opening map file >%s< - exit\n",map);
    exit(0);
  }

  /* T TDC constants */

  tagtdccal_.TagTDCCalTMM = 1; /* need to set it to 0 if Map read failed */
  map_get_float(map,"tag_t","slope_left",61,tagtdccal_.TCchWidthL,run,&k);
  map_get_float(map,"tag_t","slope_right",61,tagtdccal_.TCchWidthR,run,&k);

  /* E TDC constant */

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
  }
  */


  /*
  for(i=0; i<61; i++)
  {
    slopeTR[i] = slopeTR[i] / 1000.;
    slopeTL[i] = slopeTL[i] / 1000.;
  }
  *slopeE = - *slopeE / 1000.;
  */

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

  /* return if debug flag do not set */
  if(debug==0) return;

  printf("tghbookcalib reached\n"); fflush(stdout);
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("tghbookcalib: CLON_PARMS not defined !!!\n");
    exit(0);
  }

  sprintf(fname,"%s/cmon/tglib/%s.dat",dir,"TAG_HISTS");
  if((fd=fopen(fname,"r")) != NULL)
  {
    printf("tghbookcalib: reading config file >%s<\n",fname);
    nline = 0;
    do
    {
      s = fgets(str,MAXSTRLEN,fd);
      nline++;
      if(s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
          printf("tghbookcalib: Error 8 reading config file, line # %i\n",nline);
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
        for(i=0; i<=dim; i++)
        {
          if(nbin > 0) uthbook1(ana->histf,histid+i,mapname,nbin,xmin,xmax);
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
  else
  {
    printf("tghbookcalib: cannot open config file >%s< - exit\n",fname);
    exit(0);
  }

  return;
}


