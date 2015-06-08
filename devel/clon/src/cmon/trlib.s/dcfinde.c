/*
   Purpose and Methods : Identify electron or other particle
                         with Start Counter


   Input   :  include files

   Outputs :  itrk0  - track number
              beta0  - velocity "beta0" of the track "itrk0" 

   Controls:


   Library belongs    : libsda.a

   Calls              : dcecid, scmatch

   Created   28-FEB-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by trlib
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "prlib.h"
#include "sclib.h"
#include "tglib.h"
#include "sdageom.h"
#include "sdacalib.h" /* photoproduction only */
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define vflt 29.97925


void
dcfinde_(int *jw, TRevent *ana, int *ifail)
{
  dcfinde(jw,ana,ifail);
  return;
}

void
dcfinde(int *jw, TRevent *ana, int *ifail)
{
  PRTRACK *prr;
  DCtrack *track;
  int ind;
  int i, j, itrk, iflg, id, ipst, idst, idsc, ipsc, ihsc;
  int isec, itag, ih, it, iret;
  float tp, tsc, tst, Ssc, Sst, d1, xx, Bsign,t0tag,t0tof,toffs,dif;
  float twL, twR, tm, beta, p, tst0, tsc0;

  float t0st; /* in NEW SDA it is in COMMON; do we need it in another subroutine ? */

  ana->itag0 = 0;
  ana->itrk0 = 0;
  t0st = 0.0;


  if((ind = etNlink(jw,"PATH",0)) <= 0) return;
  prr = (PRTRACK *)&jw[ind];

  /* Photoproduction. Define the particle velocity using SC, ST & TG */

  if(sdakeys_.zbeam[0] == 0.)
  {
/*
printf("dcfinde: photoproduction\n");
*/
    *ifail = 0;
    t0tag = sdakeys_.zcut[8]; /* !!! */
    t0tof = sdakeys_.zcut[9]; /* !!! */
    toffs = t0tof - t0tag;

    /* If data analysis call "ana_tag" */

	/*printf("call tglib\n");*/
    tglib(jw,ana,&ana->ntagh,ana->tag_res);
    if(ana->ntagh <= 0)
    {
      /*printf("fail tglib: %d\n",ana->ntagh);*/
      goto a999;
	}
    else
    {
      /*printf("pass tglib: %d\n",ana->ntagh);*/
      float ttt = (float)ana->ntagh;
      /*printf("ttt=%f\n",ttt);*/
      uthfill(ana->histf, 81, ttt, 0., 1.);
    }

    /* Find a track with hits in SC & ST Counters */

    for(itrk=1; itrk<=ana->ntrack; itrk++)
    {
      if(ana->track[itrk-1].level != 2) continue;

      /* Check chisquare of the track fiting for Level = 2 */
      if(ana->track[itrk-1].trkp[0].chisq > sdakeys_.zcut[3]) continue;

uthfill(ana->histf, 82, ana->track[itrk-1].trkp[0].chisq, 0., 1.);

      isec = ana->track[itrk-1].sect;
      {
        float x0[npl_sc];
        for(i=0; i<npl_sc; i++) x0[i]=ana->track[itrk-1].trkp[nsc_min-1+i].x;
        scmatch(jw, ana, isec, &ana->track[itrk-1].scid, x0, &iflg);
      }
uthfill(ana->histf, 83, ana->track[itrk-1].trkp[0].chisq, 0., 1.);
      if(iflg > 0) continue;
uthfill(ana->histf, 84, ana->track[itrk-1].trkp[0].chisq, 0., 1.);


      /* get time of ST */
      itag = 0;
      {
        int stndig[7], stdigi[24][2];
        GETSTDATA;
        if(stndig[isec]==0) continue;

        ipst = 0;
        xx = ana->track[itrk-1].trkp[1].x;
        if(xx < 999. && xx <= sdageom_.pln[isec-1][1][11]) ipst = 2;             
        xx = ana->track[itrk-1].trkp[2].x;
        if(xx < 999. && xx > sdageom_.pln[isec-1][2][13]) ipst = 3;
        if(ipst == 0) continue;

uthfill(ana->histf, 85, ana->track[itrk-1].trkp[0].chisq, 0., 1.);
/* between here and 86 - lost factor 3 !!! */ 

        float d2 = 0.;      /* distance along nose */
        float diffmin=13.;  /* time difference between photon and ST hit */
                            /* was 3.0 */

        if(ipst == 2)
        {
          d1 = ABS(sdageom_.pln[isec-1][ipst-1][13])
             + ana->track[itrk-1].trkp[ipst-1].x;
        }
        else
        {
          d1 = sdageom_.pln[isec-1][ipst-2][15];
	      d2 = sqrt(
     pow((sdageom_.pln[isec-1][ipst-1][13]-ana->track[itrk-1].trkp[ipst-1].x),2.)
   + pow((sdageom_.pln[isec-1][ipst-1][14]-ana->track[itrk-1].trkp[ipst-1].y),2.));
        }

	    /* loop over all ST or STN slabs in sector */
        for(id=isec*4-4; id<isec*4; id++)
        {
	      if( !stdigi[id][0] ) continue;

	      tst = stdigi[id][0] / sdacalib_.cal_st[id][1]
                - sdacalib_.cal_st[id][0];
	      /* timewalk correction */
	      if(stdigi[id][1] > sdacalib_.cal_st[id][5]) 
	        tst += sdacalib_.cal_st[id][6]/
	                 (sdacalib_.cal_st[id][5] - sdacalib_.cal_st[id][7]);
	      else 
	        tst += sdacalib_.cal_st[id][6]/
	                 ((stdigi[id][1]>50.? stdigi[id][1]:50.) - sdacalib_.cal_st[id][7]);

uthfill(ana->histf,180+isec, tst, 0., 1.);
          tst = tst - d1/sdacalib_.cal_st[id][3] 
	             - d2/sdacalib_.cal_st[id][8] - sdacalib_.cal_st[id][9]*d2*d2;

uthfill(ana->histf,190+isec, tst, 0., 1.);


          /* Check time difference: ST - TAG */

	      for(ih = 1; ih<=ana->ntagh; ih++)
	      {
		/*
printf("ana->tag_res[%d][2]=%f\n",ih-1,ana->tag_res[ih-1][2]);
		*/
	        if(ana->tag_res[ih-1][2] <= 0.) continue;
	        dif = tst - ana->tag_res[ih-1][5] - ana->track[itrk-1].trkp[ipst-1].s/vflt;
	        if(sdakeys_.ifsim == 1) dif += t0tof;

uthfill(ana->histf, 87, dif-toffs, 0., 1.);
uthfill(ana->histf, 187, dif-toffs, 0., 1.);

            if(ABS(dif - toffs) < diffmin)
            {
              itag = ih;
              idst = id;
              t0st = tst;
              diffmin = ABS(dif - toffs);
            }
	    /*
printf("dif-toffs=%f (%f-%f) diffmin=%f\n",dif-toffs,dif,toffs,diffmin);
	    */
          }
	    }   /* loop over ST hits in sector */

      }
	  
      if(itag == 0) continue;
uthfill(ana->histf, 86, ana->track[itrk-1].trkp[0].chisq, 0., 1.);

      /* Get time of SC */

      {
        int scndig, scdigi[57][5];
        GETSCDATA;

        idsc = ana->track[itrk-1].scid.slab;
        ipsc = ana->track[itrk-1].scid.plane;
        ihsc = ana->track[itrk-1].scid.hit;
        if(sdakeys_.ifsim == 1)
        {
          scswalk(jw,isec,idsc,ihsc,ana->track[itrk-1].trkp[ipsc-1].z,&tp,&tsc);
		  /*printf("dcfinde: simulation\n");*/
        }
        else
        {
          sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
		  /*printf("dcfinde: reconstruction: %f %f %f %f %d\n",twL,twR,tm,tsc,iret);*/
        }
      }

      /* Now calculate beta for the track# itrk */
      beta = (ana->track[itrk-1].trkp[ipsc-1].s
            - ana->track[itrk-1].trkp[ipst-1].s)
           / (tsc-t0st)/vflt;
/*
printf("-------> beta=%f (track2sc=%f track2st=%f tsc=%f t0st=%f lightvelocity=%f)\n",
beta,ana->track[itrk-1].trkp[ipsc-1].s,ana->track[itrk-1].trkp[ipst-1].s,tsc,t0st,vflt);
*/
      if(beta < 0.09 || beta > 1.5) continue;
uthfill(ana->histf, 89, ana->track[itrk-1].trkp[0].chisq, 0., 1.);

      /* Check time difference: SC - TAG using current beta */
      dif = tsc - ana->track[itrk-1].trkp[ipsc-1].s
          / vflt / beta - ana->tag_res[itag-1][5];
      if(sdakeys_.ifsim == 1) dif += t0tof;

uthfill(ana->histf, 88, dif-toffs,0., 1.);
uthfill(ana->histf, 188, dif-toffs,0., 1.);
/*
printf("=======> dif-toffs = %f (%f %f)\n",dif-toffs,dif,toffs);
*/
      if(ABS(dif - toffs) > 13.0) continue; /* was 3.0 */

      ana->itrk0 = itrk;
      ana->itag0 = itag;
      ana->beta0 = beta;
      /*tzero = tsc - ana->track[itrk-1].trkp[ipsc-1].s/vflt/beta; need it outside ? */
      *ifail = 1;

      /* Time alignment of ST relative to SC */
      if(ana->track[itrk-1].trkp[0].q < 0. && ana->track[itrk-1].trkp[0].p > 0.4)
      {
        tsc0 = tsc  - ana->track[itrk-1].trkp[ipsc-1].s/vflt;
        tst0 = t0st - ana->track[itrk-1].trkp[ipst-1].s/vflt
                   + sdacalib_.cal_st[idst][0];
uthfill(ana->histf, 210+isec, tsc0-tst0, 0., 1.);
      }

      goto a999;
    }

    goto a999;
  }







  /* Electroproduction. The goal is to identify the electron. */

  if(sdakeys_.zbeam[0] < 0.)
  {
/*
printf("dcfinde: electroproduction\n");
*/
    *ifail = -1;
    ana->itrk0 = 0;

    for(itrk=1; itrk<=ana->ntrack; itrk++)
    {
      track = (DCtrack *) &ana->track[itrk-1]; /* pointer to current track */

      if(track->level != 2) continue;

      /* Check chisquare of the track fiting for Level = 2 */
      if(track->trkp[0].chisq > sdakeys_.zcut[3]) continue;

      /* Check charge sign of the current track */
      if(ABS(sdakeys_.zkine[0]) !=1. && track->trkp[0].q != -1.)
      {
        continue;
      }

      /* Identify the electron track in ToF Counters (SC) */
      isec = track->sect;
      {
        float x0[npl_sc];
        for(i=0; i<npl_sc; i++) x0[i]=track->trkp[nsc_min-1+i].x;
        scmatch(jw, ana, isec, &track->scid, x0, &iflg);
      }
      if(iflg > 0) continue;

      /* Identify the current track in Shower Counter (EC forward) */
      p = track->trkp[nec_min-1].p;
      if(sdakeys_.lanal[0] < 2) p = ana->ev_out[itrk-1][6];


/*********************************/
/* EC cluster match in PRLIB now */

      /* dcecid(jw,track->sect, &track->ecid, p, &iflg); done in PRLIB */
/*
printf("prr: %4.1f %4.1f %4.1f -> %f\n",prr[itrk-1].ec[2].u,prr[itrk-1].ec[2].v,
  prr[itrk-1].ec[2].w,prr[itrk-1].ec[2].e);
*/
/*if(prr[itrk-1].ec[2].e<0.00001 && iflg == 0) exit(0);*/

      /* if(iflg == 2) continue;   Track hit point outside EC volume */

/* EC cluster match in PRLIB now */
/*********************************/

      if(track->ecid.u * track->ecid.v * track->ecid.w ==  0) iflg = 2;
      else if(prr[itrk-1].ec[2].e<0.00001)                    iflg = 1;
      else                                                    iflg = 0;

      /* Store the results */

      /* Track hit inside EC volume */
      if(iflg == 1 && sdakeys_.zcut[4] == 0.)
      {
        ana->itrk0 = itrk;
        ana->beta0 = 1.;
        *ifail = 0;
        goto a999;
      }

      /* Track inside EC volume & complete EC analysis */
      if(iflg == 0)
      {
        ana->itrk0 = itrk;
        ana->beta0 = 1.;
        *ifail = 1;
        goto a999;
      }

    }
  }

a999:

  /* Tagging electron track or track with Beta determined
     using ST-counter & ToF */

  if(sdakeys_.lanal[0] == 2 && *ifail == 1)
  {
    for(it=1; it<ana->ntr_out; it++)
    {
      ana->ev_out[it-1][0] = 0.0;
      if(ana->ev_out[it-1][9] == ana->itrk0)
      {
        ana->ev_out[it-1][0] = 1;
        if(sdakeys_.zbeam[0] == 0.) ana->ev_out[it-1][0] = ana->itag0;
        ana->ev_out[it-1][7] = ana->beta0;
      }
    }
  }

  return;
}





