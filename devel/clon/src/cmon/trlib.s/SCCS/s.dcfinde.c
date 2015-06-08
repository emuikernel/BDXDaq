h02273
s 00006/00005/00372
d D 1.10 07/11/03 13:31:16 boiarino 11 10
c *** empty log message ***
e
s 00020/00014/00357
d D 1.9 07/11/03 13:14:34 boiarino 10 9
c *** empty log message ***
e
s 00001/00001/00370
d D 1.8 07/11/03 11:45:22 boiarino 9 8
c *** empty log message ***
e
s 00015/00007/00356
d D 1.7 07/11/02 20:29:08 fklein 8 7
c *** empty log message ***
e
s 00008/00002/00355
d D 1.6 07/11/02 11:12:00 boiarino 7 6
c *** empty log message ***
e
s 00031/00030/00326
d D 1.5 07/11/01 18:53:09 boiarino 6 5
c *** empty log message ***
e
s 00063/00037/00293
d D 1.4 07/11/01 15:18:03 boiarino 5 4
c new version from Franz : new Start Counter implemented
c 
e
s 00002/00000/00328
d D 1.3 07/11/01 14:45:53 boiarino 4 3
c *** empty log message ***
e
s 00010/00002/00318
d D 1.2 07/10/26 20:26:13 boiarino 3 1
c bla
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcfinde.c
e
s 00320/00000/00000
d D 1.1 01/11/19 19:05:58 boiarino 1 0
c date and time created 01/11/19 19:05:58 by boiarino
e
u
U
f e 0
t
T
I 1
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
D 5
  int i, j, itrk, iflg, ip, idsc, ipsc, ihsc, isec, itag, ih, it, iret;
E 5
I 5
  int i, j, itrk, iflg, id, ipst, idst, idsc, ipsc, ihsc;
  int isec, itag, ih, it, iret;
E 5
  float tp, tsc, tst, Ssc, Sst, d1, xx, Bsign,t0tag,t0tof,toffs,dif;
  float twL, twR, tm, beta, p, tst0, tsc0;

  float t0st; /* in NEW SDA it is in COMMON; do we need it in another subroutine ? */

  ana->itag0 = 0;
  ana->itrk0 = 0;
  t0st = 0.0;

I 3

E 3
  if((ind = etNlink(jw,"PATH",0)) <= 0) return;
  prr = (PRTRACK *)&jw[ind];

  /* Photoproduction. Define the particle velocity using SC, ST & TG */

  if(sdakeys_.zbeam[0] == 0.)
  {
I 3
/*
printf("dcfinde: photoproduction\n");
*/
E 3
    *ifail = 0;
D 3

E 3
    t0tag = sdakeys_.zcut[8]; /* !!! */
    t0tof = sdakeys_.zcut[9]; /* !!! */
    toffs = t0tof - t0tag;

    /* If data analysis call "ana_tag" */

I 3
D 10
printf("call tglib\n");
E 10
I 10
	/*printf("call tglib\n");*/
E 10
E 3
    tglib(jw,ana,&ana->ntagh,ana->tag_res);
I 4
D 5
printf("after tglib: %d\n",ana->ntagh);
E 5
E 4
    if(ana->ntagh <= 0) goto a999;
D 3

E 3
I 3
D 10
printf("pass tglib: %d\n",ana->ntagh);
E 3
D 8
uthfill(ana->histf, 81, ana->ntagh, 0., 1.);
E 8
I 8
{
  float ttt = (float)ana->ntagh;
printf("ttt=%f\n",ttt);
uthfill(ana->histf, 81, ttt, 0., 1.);
}
E 10
I 10
	/*printf("pass tglib: %d\n",ana->ntagh);*/
    {
      float ttt = (float)ana->ntagh;
      /*printf("ttt=%f\n",ttt);*/
      uthfill(ana->histf, 81, ttt, 0., 1.);
    }
E 10
E 8

I 4
D 5

E 5
E 4
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

D 6
      /* get time of ST */
E 6

I 6
      /* get time of ST */
E 6
I 5
      itag = 0;
E 5
      {
D 5
        int stndig, stdigi[6][3];
E 5
I 5
        int stndig[7], stdigi[24][2];
E 5
        GETSTDATA;
D 10

D 5
        if(stdigi[isec-1][0] != isec || stdigi[isec-1][1] < 1) continue;
        ip = 0;
E 5
I 5
        
E 10
        if(stndig[isec]==0) continue;
I 10

E 10
        ipst = 0;
E 5
        xx = ana->track[itrk-1].trkp[1].x;
D 5
        if(xx < 999. && xx <= sdageom_.pln[isec-1][1][11]) ip = 2;             
E 5
I 5
        if(xx < 999. && xx <= sdageom_.pln[isec-1][1][11]) ipst = 2;             
E 5
        xx = ana->track[itrk-1].trkp[2].x;
D 5
        if(xx < 999. && xx > sdageom_.pln[isec-1][2][13]) ip = 3;
        if(ip == 0) continue;
E 5
I 5
        if(xx < 999. && xx > sdageom_.pln[isec-1][2][13]) ipst = 3;
        if(ipst == 0) continue;
E 5

uthfill(ana->histf, 85, ana->track[itrk-1].trkp[0].chisq, 0., 1.);
/* between here and 86 - lost factor 3 !!! */ 

I 3
D 6
printf("bla1\n");
E 3
D 5
        if(ip == 2)
E 5
I 5

E 6
        float d2 = 0.;      /* distance along nose */
D 6
	float diffmin=13.;  /* time difference between photon and ST hit */
E 6
I 6
D 8
	    float diffmin=13.;  /* time difference between photon and ST hit */
E 8
I 8
D 10
	float diffmin=13.;  /* time difference between photon and ST hit */
E 10
I 10
        float diffmin=13.;  /* time difference between photon and ST hit */
E 10
E 8
E 6
                            /* was 3.0 */

        if(ipst == 2)
E 5
        {
D 5
          d1 = ABS(sdageom_.pln[isec-1][ip-1][13])
             + ana->track[itrk-1].trkp[ip-1].x;
E 5
I 5
          d1 = ABS(sdageom_.pln[isec-1][ipst-1][13])
             + ana->track[itrk-1].trkp[ipst-1].x;
E 5
        }
        else
        {
D 5
          d1 = sdageom_.pln[isec-1][ip-2][15] + sqrt(
     pow((sdageom_.pln[isec-1][ip-1][13]-ana->track[itrk-1].trkp[ip-1].x),2.)
   + pow((sdageom_.pln[isec-1][ip-1][14]-ana->track[itrk-1].trkp[ip-1].y),2.));
E 5
I 5
          d1 = sdageom_.pln[isec-1][ipst-2][15];
D 6
	  d2 = sqrt(
E 6
I 6
	      d2 = sqrt(
E 6
     pow((sdageom_.pln[isec-1][ipst-1][13]-ana->track[itrk-1].trkp[ipst-1].x),2.)
   + pow((sdageom_.pln[isec-1][ipst-1][14]-ana->track[itrk-1].trkp[ipst-1].y),2.));
E 5
        }
D 5
        tst = stdigi[isec-1][1] / sdacalib_.cal_st[isec-1][1]
            - sdacalib_.cal_st[isec-1][0];
E 5
I 5

D 6
	/* loop over all ST or STN slabs in sector */
        for(id=isec*4-4; id<isec*4; ih++) {
	  if( !stdigi[id][0] ) continue;
E 6
I 6
	    /* loop over all ST or STN slabs in sector */
        for(id=isec*4-4; id<isec*4; id++)
        {
	      if( !stdigi[id][0] ) continue;
E 6

D 6
	  tst = stdigi[id][0] / sdacalib_.cal_st[id][1]
E 6
I 6
	      tst = stdigi[id][0] / sdacalib_.cal_st[id][1]
E 6
                - sdacalib_.cal_st[id][0];
D 6
	  /* timewalk correction */
	  if(stdigi[id][1] > sdacalib_.cal_st[id][5]) 
	    tst += sdacalib_.cal_st[id][6]/
	      (sdacalib_.cal_st[id][5] - sdacalib_.cal_st[id][7]);
	  else 
	    tst += sdacalib_.cal_st[id][6]/
	      ((stdigi[id][1]>50.? stdigi[id][1]:50.) - sdacalib_.cal_st[id][7]);
E 6
I 6
	      /* timewalk correction */
	      if(stdigi[id][1] > sdacalib_.cal_st[id][5]) 
	        tst += sdacalib_.cal_st[id][6]/
	                 (sdacalib_.cal_st[id][5] - sdacalib_.cal_st[id][7]);
	      else 
	        tst += sdacalib_.cal_st[id][6]/
	                 ((stdigi[id][1]>50.? stdigi[id][1]:50.) - sdacalib_.cal_st[id][7]);
E 6

E 5
uthfill(ana->histf,180+isec, tst, 0., 1.);
D 5
        tst = tst - d1/sdacalib_.cal_st[isec-1][3];
E 5
I 5
          tst = tst - d1/sdacalib_.cal_st[id][3] 
D 6
	    - d2/sdacalib_.cal_st[id][8] - sdacalib_.cal_st[id][9]*d2*d2;
E 6
I 6
	             - d2/sdacalib_.cal_st[id][8] - sdacalib_.cal_st[id][9]*d2*d2;
E 6

E 5
uthfill(ana->histf,190+isec, tst, 0., 1.);
D 5
      }
E 5
I 5
D 6
      
E 6
E 5

D 5
      /* Check time difference: ST - TAG neglecting flight time to ST */
E 5
I 5
D 6
      /* Check time difference: ST - TAG */
E 6
E 5

D 5
      itag = 0;
      for(ih = 1; ih<=ana->ntagh; ih++)
      {
        if(ana->tag_res[ih-1][2] <= 0.) continue;
        dif = tst - ana->tag_res[ih-1][5] - ana->track[itrk-1].trkp[ip-1].s/vflt;
        if(sdakeys_.ifsim == 1) dif += t0tof;
E 5
I 5
D 6
	  for(ih = 1; ih<=ana->ntagh; ih++)
	  {
	    if(ana->tag_res[ih-1][2] <= 0.) continue;
	    dif = tst - ana->tag_res[ih-1][5] - ana->track[itrk-1].trkp[ipst-1].s/vflt;
	    if(sdakeys_.ifsim == 1) dif += t0tof;
E 6
I 6
          /* Check time difference: ST - TAG */
E 6
E 5

I 6
	      for(ih = 1; ih<=ana->ntagh; ih++)
	      {
I 8
		/*
E 8
printf("ana->tag_res[%d][2]=%f\n",ih-1,ana->tag_res[ih-1][2]);
I 8
		*/
E 8
	        if(ana->tag_res[ih-1][2] <= 0.) continue;
	        dif = tst - ana->tag_res[ih-1][5] - ana->track[itrk-1].trkp[ipst-1].s/vflt;
	        if(sdakeys_.ifsim == 1) dif += t0tof;

D 7
printf("dif-toffs=%f (%f-%f)\n",dif-toffs,dif,toffs);
E 7
E 6
uthfill(ana->histf, 87, dif-toffs, 0., 1.);
uthfill(ana->histf, 187, dif-toffs, 0., 1.);

D 5
        if(ABS(dif - toffs) < 13.0) /* was 3.0 */
        {
          itag = ih;
          break;
        }
E 5
I 5
            if(ABS(dif - toffs) < diffmin)
            {
D 6
	      itag = ih;
	      idst = id;
	      t0st = tst;
	      diffmin = ABS(dif - toffs);
            } 
E 6
I 6
D 8
	          itag = ih;
	          idst = id;
	          t0st = tst;
	          diffmin = ABS(dif - toffs);
E 8
I 8
D 10
	      itag = ih;
	      idst = id;
E 10
I 10
              itag = ih;
              idst = id;
E 10
              t0st = tst;
              diffmin = ABS(dif - toffs);
E 8
            }
I 8
	    /*
E 8
I 7
printf("dif-toffs=%f (%f-%f) diffmin=%f\n",dif-toffs,dif,toffs,diffmin);
I 8
	    */
E 8
E 7
E 6
          }
D 6
	}   /* loop over ST hits in sector */
E 6
I 6
D 8
	    }   /* loop over ST hits in sector */
E 8
I 8
D 10
	}   /* loop over ST hits in sector */
E 10
I 10
	    }   /* loop over ST hits in sector */
E 10
E 8
E 6

E 5
      }
I 5
	  
E 5
      if(itag == 0) continue;
uthfill(ana->histf, 86, ana->track[itrk-1].trkp[0].chisq, 0., 1.);

      /* Get time of SC */

      {
D 9
        int scndig, scdigi[48][5];
E 9
I 9
        int scndig, scdigi[57][5];
E 9
        GETSCDATA;

        idsc = ana->track[itrk-1].scid.slab;
        ipsc = ana->track[itrk-1].scid.plane;
        ihsc = ana->track[itrk-1].scid.hit;
        if(sdakeys_.ifsim == 1)
        {
          scswalk(jw,isec,idsc,ihsc,ana->track[itrk-1].trkp[ipsc-1].z,&tp,&tsc);
I 7
D 11
printf("dcfinde: simulation\n");
E 11
I 11
		  /*printf("dcfinde: simulation\n");*/
E 11
E 7
        }
        else
        {
          sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
I 7
D 11
printf("dcfinde: reconstruction: %f %f %f %f %d\n",twL,twR,tm,tsc,iret);
E 11
I 11
		  /*printf("dcfinde: reconstruction: %f %f %f %f %d\n",twL,twR,tm,tsc,iret);*/
E 11
E 7
        }
      }

      /* Now calculate beta for the track# itrk */
      beta = (ana->track[itrk-1].trkp[ipsc-1].s
D 5
            - ana->track[itrk-1].trkp[ip-1].s)
           / (tsc-tst)/vflt;
E 5
I 5
            - ana->track[itrk-1].trkp[ipst-1].s)
           / (tsc-t0st)/vflt;
E 5
D 7
      if(beta < 0.1 || beta > 1.5) continue;
E 7
I 7
D 11

E 11
I 11
/*
E 11
D 10
printf("-------> beta=%f (%f %f %f)\n",beta,tsc,t0st,vflt);
E 10
I 10
printf("-------> beta=%f (track2sc=%f track2st=%f tsc=%f t0st=%f lightvelocity=%f)\n",
beta,ana->track[itrk-1].trkp[ipsc-1].s,ana->track[itrk-1].trkp[ipst-1].s,tsc,t0st,vflt);
E 10
D 11

E 11
I 11
*/
E 11
      if(beta < 0.09 || beta > 1.5) continue;
E 7
uthfill(ana->histf, 89, ana->track[itrk-1].trkp[0].chisq, 0., 1.);

      /* Check time difference: SC - TAG using current beta */
      dif = tsc - ana->track[itrk-1].trkp[ipsc-1].s
          / vflt / beta - ana->tag_res[itag-1][5];
      if(sdakeys_.ifsim == 1) dif += t0tof;

uthfill(ana->histf, 88, dif-toffs,0., 1.);
uthfill(ana->histf, 188, dif-toffs,0., 1.);
D 11

E 11
I 11
/*
E 11
I 7
printf("=======> dif-toffs = %f (%f %f)\n",dif-toffs,dif,toffs);
I 11
*/
E 11
E 7
      if(ABS(dif - toffs) > 13.0) continue; /* was 3.0 */

      ana->itrk0 = itrk;
      ana->itag0 = itag;
      ana->beta0 = beta;
      /*tzero = tsc - ana->track[itrk-1].trkp[ipsc-1].s/vflt/beta; need it outside ? */
D 5
      t0st  = tst;
E 5
      *ifail = 1;

      /* Time alignment of ST relative to SC */
      if(ana->track[itrk-1].trkp[0].q < 0. && ana->track[itrk-1].trkp[0].p > 0.4)
      {
D 5
        tsc0 = tsc - ana->track[itrk-1].trkp[ipsc-1].s/vflt;
        tst0 = tst - ana->track[itrk-1].trkp[ip-1].s/vflt
                   + sdacalib_.cal_st[isec-1][0];
E 5
I 5
        tsc0 = tsc  - ana->track[itrk-1].trkp[ipsc-1].s/vflt;
        tst0 = t0st - ana->track[itrk-1].trkp[ipst-1].s/vflt
                   + sdacalib_.cal_st[idst][0];
E 5
uthfill(ana->histf, 210+isec, tsc0-tst0, 0., 1.);
      }

      goto a999;
    }

    goto a999;
  }

I 10






E 10
  /* Electroproduction. The goal is to identify the electron. */

  if(sdakeys_.zbeam[0] < 0.)
  {
I 3
/*
printf("dcfinde: electroproduction\n");
*/
E 3
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





E 1
