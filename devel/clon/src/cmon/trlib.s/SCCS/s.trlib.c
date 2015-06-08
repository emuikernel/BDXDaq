h46786
s 00005/00000/00164
d D 1.2 01/12/10 12:27:05 boiarino 3 1
c add HIST 19
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/trlib.c
e
s 00164/00000/00000
d D 1.1 01/11/19 19:06:02 boiarino 1 0
c date and time created 01/11/19 19:06:02 by boiarino
e
u
U
f e 0
t
T
I 1

/* trlib.c - track reconstruction library (former sda_trak) */

#include <stdio.h>
#include "dclib.h"
#include "prlib.h"

int
checkTrigger(int *iw)
{
  int ind;
  unsigned int *ptr;

  if((ind = etNlink(iw,"TGBI",0)) > 0)
  {
    ptr = (unsigned int *)&iw[ind];
	/*
    printf("TGBI: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                            ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
	*/
    if( ptr[0]&0x3 ) return(1); /* first 2 bits only !!! */

  }

  return(1);
}

int
initfromPR(int *iw, TRevent *ana)
{
  PRTRACK *trkptr, *road;
  int i, j, itr, itr1, is, iv;
  int *ptri, ind, nhits, tmp;
  float *ptrf;

  /* cleanup */

  ana->nevnt = 0;
  ana->ntrack = 0;
  ana->ntr_out = 0;
  ana->itrk0 = 0;

  if((ind = etNlink(iw,"PATH",0)) > 0)
  {
    road = (PRTRACK *)&iw[ind];
    ana->ntrack = (etNdata(iw,ind)*sizeof(int))/sizeof(PRTRACK);

    for(itr=0; itr<ana->ntrack; itr++)
    {
      trkptr = &road[itr];

      ana->track[itr].level = 1;
      ana->track[itr].sect  = trkptr->sector;

      for(iv=0; iv<6; iv++) ana->track[itr].vect[iv] = trkptr->vect[iv];
      /* correction !!! */
      ana->track[itr].vect[0] -= 3.2;
      ana->track[itr].vect[1] += 0.8;

      ana->track[itr].vect[6] = trkptr->p;

      ana->track[itr].vect[7] = (float)trkptr->ntof;
      ana->track[itr].vect[8] = (float)trkptr->nu / 2.;
      ana->track[itr].vect[9] = (float)trkptr->nv / 2.;
      ana->track[itr].vect[10] = (float)trkptr->nw / 2.;
      ana->track[itr].vect[11] = (float)trkptr->charge;
    }

    return(ana->ntrack);
  }

  return(0);
}



void
trlib_(int *iw, TRevent *ana, int *lanal1,float *zcut5,float *zcut8,int *inter)
{
  trlib(iw, ana, *lanal1, *zcut5, *zcut8, *inter);
}

void
trlib(int *iw, TRevent *ana, int lanal1, float zcut5, float zcut8, int inter)
{
  int i, ifail;

  ifail = 0;

  /* get initial info from PR */
  if(!initfromPR(iw,ana)) goto exit;

  /* check tgigger bits */
  if(!checkTrigger(iw)) goto exit;

  /* Fitting to the wire positions only (no timing involved) */
  dcfit(iw,ana,2,&ana->ntr_out,ana->ev_out,inter,&ifail);
  if(ifail == 0) goto exit;
  /*for(i=0; i<ifail; i++)*/
  {
    dcstatb(&ana->stat,4);
    uthfill(ana->histf, 18, 3.5, 0., 1.);
I 3
    uthfill(ana->histf, 19, 3.5, 0., 1.);
E 3
  }

  /* Identify electron with CC,SC & EC  or beta of any particle with ST & SC */
  dcfinde(iw,ana,&ifail);
  /*goto exit;*/
  if(ifail >= zcut5)
  {
    dcstatb(&ana->stat,5);
    uthfill(ana->histf, 18, 4.5, 0., 1.);
I 3
    uthfill(ana->histf, 19, 4.5, 0., 1.);
E 3
  }
  if(lanal1 < 3) goto exit;

  /* Check if electron  or beta any of particle (photoproduction) has been found */
  if(ifail < zcut5) goto exit;
  /* Derive DCA and errors for hits on candidate tracks and resolve L-R Ambiguity */
  dcdcam(iw,ana,&ifail);
/*printf("IFAIL(CAM)=%d\n",ifail); fflush(stdout);*/
  if(ifail == 0) goto exit;
  if(ifail >= zcut8)
  {
    dcstatb(&ana->stat,6);
    uthfill(ana->histf, 18, 5.5, 0., 1.);
I 3
    uthfill(ana->histf, 19, 5.5, 0., 1.);
E 3
  }
/*
  save HBT results to TRAK bank
  sda_anal2trak_(iw,iw);
  get HBT results from TRAK bank
  itmp = 0;
  sda_trak2anal_(iw,iw,&itmp);
*/

  if(lanal1 < 4) goto exit;
  /* Make final fits to the measured: {DCA +/- SIGdca}i=1,N for each track */
  dcfit(iw,ana,4,&ana->ntr_out,ana->ev_out,inter,&ifail);
/*printf("IFAIL(FIT)=%d\n",ifail); fflush(stdout);*/
  if(ifail == 0) goto exit;
  if(ifail >= zcut8)
  {
    dcstatb(&ana->stat,7);
    uthfill(ana->histf, 18, 6.5, 0., 1.);
I 3
    uthfill(ana->histf, 19, 6.5, 0., 1.);
E 3
  }

  /* Number of reconstructed event */
  if(ana->ntr_out >= zcut8)
  {
    dcstatb(&ana->stat,8);
    uthfill(ana->histf, 18, 7.5, 0., 1.);
I 3
    uthfill(ana->histf, 19, 7.5, 0., 1.);
E 3
  }

exit:
  ;

  return;
}








E 1
