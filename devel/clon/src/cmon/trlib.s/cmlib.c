
/* cmlib.c - CLAS Monitor pattern recognition and track reconstruction library

             static variables can be used here - not a multithreaded routines

    /sdageom/dc_wpmid  - DC Wire position
          dc_wpmid(1,iw,il,isec) |
          dc_wpmid(2,iw,il,isec) |-> Wire position in the middle plane
          dc_wpmid(3,iw,il,isec) |

    /sdageom/dc_wdir   - DC Wire Direction
          dc_wdir(1,iw,il,isec)  |
          dc_wdir(2,iw,il,isec)  |-> Wire direction (unit vector)
          dc_wdir(3,iw,il,isec)  |

  BUG report: boiarino@jlab.org

*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "uttime.h"
#include "prlib.h"
#include "dclib.h"
#include "sdadraw.h"
#include "sdaevgen.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

/*
extern int ntrack;
extern PRTRACK track[NTRACK];
*/
#define NPOINT 34
int
trackpoints(int itr, int *n, int x[NPOINT], int y[NPOINT], int z[NPOINT])
{
  int k;

  /*printf("trackpoints: %d\n",itr);*/

/*
  for(it=1; it<=ced_trk[isec-1]; it++)
  {
    int ipl;

    ilnk = ced_lnk[isec-1][it-1];
    printf("ilnk=%d\n",ilnk);

    k = 0;
    for(il=1; il<=npl_dc; il++)
    {
      if(il < 5 || il > 6)
      {
        ipl = nst_max + il;
        printf("isec=%d it=%d\n",isec,ced_itr[isec-1][it-1]);
        x[k] = ana->track[ilnk-1].trkp[ipl-1].y;
        y[k] = ana->track[ilnk-1].trkp[ipl-1].z;
        z[k] = ana->track[ilnk-1].trkp[ipl-1].x;
        printf("x,y,z = %f %f %f\n",x[k],y[k],z[k]);
        k++;
      }
    }
  }
*/
  *n = k;

  return(0);
}


/* 
   Purpose and Methods : Initialization of SDA program.

   Inputs  :
   Outputs :
   Controls:

   Library belongs: libsda.a

   Calls: BOS, trconfig, dcminit, sda_dinit, sda_ginit, sda_tagcal,
          usda_init, sda_def

   Created   26-OCT-1990   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by SDA_MAIN
*/

void
cminit_()
{
  cminit();
}

void
cminit()
{
  trconfig();   /* read 'cmon.config' file */
  l3init(9999); /* level 3 trigger initialization */
  prinit_();    /* pattern recognition initialization */
  trinit_();    /* track reconstruction initialization */

  return;
}



/* event loop; to process 1 event set Nevents to 0 */
/* use ifevb=0 if do not want cmloop() to read event */

void
cmloop_(int *iw, int *ifevb, int *inter, int *Nevents, int *iret)
{
  cmloop(iw,*ifevb,*inter,*Nevents,iret);
}

void
cmloop(int *iw, int ifevb, int inter, int Nevents, int *iret)
{
  /* allocate structures and set pointers */
  static TRevent ana;
  static UThistf histf[NHIST];
  static first = 1;
  int itmp, tmp, tmp1, ierr, ievent;

  if(first)
  {
    uthsetlocal(histf);
    ana.histf = &histf[0];
    dchbook(ana.histf);
    tghbookcalib(&ana); /* calibration histograms */

    first = 0;
  }

  /* Get initial time */
  timex_(&sdakeys_.time[8]);

  if(inter == 0) dcstatreset(&ana.stat);

  do
  {
    if((ievent%1000)==0) printf("cmloop: Event %d\n",ievent);

    if(inter == 1) dcstatreset(&ana.stat);

    /* read event if ifevb != 0 */
    if(ifevb)
    {
      etLdrop(iw,"E");
      etNgarb(iw);
      itmp=1;
/*printf("-1\n");fflush(stdout);*/
      frbos_(iw,&itmp,"E",iret,1);
/*printf("-2\n");fflush(stdout);*/
      if(*iret == -1 || *iret > 0)
      { /* End of File or Read Error */
        printf(" End-of-File flag encounted, iret = %d\n",*iret);
        break;
      }
/*printf("-6\n");fflush(stdout);*/
    }

    /* process one event */
/*printf("-7\n");fflush(stdout);*/
    cmlib(iw,&ana,ifevb,inter,&ievent,0);
/*printf("-8\n");fflush(stdout);*/
    dchfill(iw,&ana,ana.ntr_out,ana.itag0,ana.ev_out,ana.tag_res);
/*printf("-9\n");fflush(stdout);*/

    /* Print statistics and draw if Interective */
    if(inter == 1)
    {
      dcstataddstatic(&ana.stat);
      return;
    }

  } while(ievent < Nevents);

  /* add statistic to global structure */
  if(inter == 0) dcstataddstatic(&ana.stat);

  return;
}


/*
   Purpose and Methods : The main routine for an event reconstruction. 
                         
   Inputs  :  iftbt = 0 - full reconstruction
                    = 1 - TBT only


   Outputs :  DST

   Controls:

   Library belongs: libana.a

   Calls: dcbrun, prlib, trlib, dcfit,
          dcstatb, dcstatp, dctodst                      

   Created   JUNE-9-1992  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by sda_main (or any other package)

*/

void
cmlib_(int *iw, TRevent *ana, int *ifevb, int *inter, int *ievent, int *iftbt)
{
  cmlib(iw,ana,*ifevb,*inter,ievent,*iftbt);
}

void
cmlib(int *iw, TRevent *ana, int ifevb, int inter, int *ievent, int iftbt)
{
  int n, is, iucomp, etNlink();
  int i, j, k, iwl3, istat[6], iii, itmp, itmp1, itmp6;
  int ifail, ind, iret;
  int ecdigi2[3];
  char dtimes[25];
  static int ifdst, ifirst = 1;

  /*static TRtrack evnt[ntrmx];*/
  /*ana->evnt = evnt;*/

  /* Set to zero the number of the track candidates, reconstructed tracks,
     and tracks to be drawn */

  ana->nevnt = 0;
  ana->ntr_out = 0;
  sdadraw_.ntrdraw  = 0;
  sdadraw_.ntrdraw0 = 0;

  if((ind = etNlink(iw,"HEAD",0)) <=0) return;
  sdakeys_.ievt = iw[ind+2]; /* get event number from HEAD bank */

  /* Begin of RUN */

  if(iw[ind+6] == 17 || iw[ind+6] == 18)
  {
    *ievent = sdakeys_.ievt;
    if(ifirst == 1)
    {
      ifirst = 0;
      ifdst = 0;
      itmp = 10;
      if(iucomp_("WDST",&sdakeys_.levb,&itmp,4) != 0) ifdst = 1;
      dcbrun_(iw);
    }
    if(iw[ind+6] == 18)
    {
      getasciitime_((time_t *)&iw[ind+3],dtimes,strlen(dtimes));
      printf("\n BEGIN RUN#%6d  Type/Clas=%4d/%2d Date= %24.24s\n\n",
                                       iw[ind+1],iw[ind+4],iw[ind+6],dtimes);
    }
    if(ifdst == 1)
    {
      etLctl(iw,"E=","HEAD");
      itmp=2;
      fwbos_(iw,&itmp,"E",&iret,1);
      etLdrop(iw,"E");
      etNgarb(iw);
    }
    return;
  }
  else if(iw[ind+6] == 20) /* end of run */
  {
    return;
  }
  else if(iw[ind+6] > 0 && iw[ind+6] < 16) /* Data, unpack an event */
  {
    *ievent = sdakeys_.ievt;
    if(ifirst == 1)
    {
      ifirst = 0;
      dcbrun_(iw);
    }
    if(iftbt == 0) /* full reconstruction */
    {
      if(sdakeys_.ievt >= sdakeys_.Nevt)
      {
        printf("cmlib: event number = %d > max = %d - return\n",
          sdakeys_.ievt,sdakeys_.Nevt);
        *ievent = sdakeys_.Nevt;
        return;
      }
      /*dcevin(iw, &ana->ntr_out, ana->ev_out, &ana->ntagh, ana->tag_res);*/
    }
  }
  else
  {
    *ievent = 0;
    return;
  }

  /* If Interactive */
  if(inter == 1)
  {
    sdakeys_.Ndbg  = 999999;
    printf("\n                  *** Event %8d Triggered ***\n",sdakeys_.ievt);
  }

  /* Report the number of entries to ANAL program
     (# of events to be analyzed) */
  dcstatb(&ana->stat,1);
  uthfill(ana->histf, 18, 0.5, 0., 1.);
  uthfill(ana->histf, 19, 0.5, 0., 1.);

  if(iftbt == 0)
  {
    /* Level 3 */
    l3lib(iw,&ifail); /* returns 1 if L3 is Ok */
    if(ifail == 0) goto a999;
    dcstatb(&ana->stat,2);
    uthfill(ana->histf, 18, 1.5, 0., 1.);
    uthfill(ana->histf, 19, 1.5, 0., 1.);

    /* Pattern Recognition */
    /*start_timer_();*/
    prlib_(iw,&ifail);
    /*itmp=4000;stop_timer_(&itmp);*/

    /* Check number of track candidates versus required multiplicity */
    if(ifail < sdakeys_.zcut[7]) goto a999;
    dcstatb(&ana->stat,3);
    uthfill(ana->histf, 18, 2.5, 0., 1.);
    uthfill(ana->histf, 19, 2.5, 0., 1.);
    /*for(i=0; i<ifail; i++) dcstatb(&ana->stat,3);*/

    if(sdakeys_.lanal[0] < 2) goto a999;

    /* Track reconstruction */
    /*start_timer_();*/
    trlib(iw,ana,sdakeys_.lanal[0],sdakeys_.zcut[4],sdakeys_.zcut[7],inter);
    /*itmp = 4000;*/
    /*stop_timer_(&itmp);*/
  }
  else
  {
    /* get HBT results from TRAK bank */

    /* Make final fits to the measured: {DCA +/- SIGdca}i=1,N for each track */
/* see in sda.s/obsolete; have to be modified !!!
    isec = sdakeys_.zgcor[9];
    sda_trak2anal_(iw,iw,&isec);
    tmp2ana(ana,itrk0,beta0,itag0,ntagh,tzero,ntr_link,itr_level,
      itr_sect,lnk_clust,lnk_segm,itr_scid,itr_ecid,trk,trkp,segm,clust,
      nclust,nsegmc,tag_res,ntr_out,ev_out,lnk_vect)
*/
    dcfit(iw,ana,4,&ana->ntr_out,ana->ev_out,inter,&ifail);
    /*printf("ifail=%d\n",ifail);*/
    if(ifail == 0) goto a999;
    if(ifail >= sdakeys_.zcut[7])
    {
      dcstatb(&ana->stat,7);
      uthfill(ana->histf, 18, 6.5, 0., 1.);
      uthfill(ana->histf, 19, 6.5, 0., 1.);
    }
    /* Number of reconstructed event */
    if(ana->ntr_out >= sdakeys_.zcut[7])
    {
      dcstatb(&ana->stat,8);
      uthfill(ana->histf, 18, 7.5, 0., 1.);
      uthfill(ana->histf, 19, 7.5, 0., 1.);
    }
  }

  /* At present write the complete input event as DST (testing) */
a999:

  /* Print EVNT bank */
  if((sdakeys_.lprnt[6] == 1 || sdakeys_.lprnt[6] == 2) && ana->ntr_out > 0)
        dcpdst(ana->ntr_out,ana->ev_out,ana->ntagh,ana->tag_res);

  if(ifdst == 1)
  {
    /*if((iii = etNlink(iw,"TRAK",0)) > 0)*/
    if((iii = etNlink(iw,"HBTR",0)) > 0)
    {
      dctodst(iw,ana->ntr_out,ana->ev_out,ana->ntagh,ana->tag_res);

	  /*
      if(ana->ntr_out > 1)
      {
        int i0, i1, it, good;
        it = 0;
        good=0;
        i0 = ((int)ana->ev_out[it][10]/10);
        if(i0==1) i1=4;
        else if(i0==2) i1=5;
        else if(i0==3) i1=6;
        else if(i0==4) i1=1;
        else if(i0==5) i1=2;
        else if(i0==6) i1=3;
        for(it=1; it<ana->ntr_out; it++)
        {
          if( ((int)(ana->ev_out[it][10]/10))==i1) good++;
        }
        if(good>0)
        {
          dctodst(iw,ana->ntr_out,ana->ev_out,ana->ntagh,ana->tag_res);
        }
      }
	  */

    }
  }

  return;
}


