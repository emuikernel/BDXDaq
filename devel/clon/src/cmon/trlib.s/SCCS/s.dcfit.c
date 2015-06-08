h61862
s 00003/00001/00781
d D 1.6 07/11/03 11:45:55 boiarino 7 6
c *** empty log message ***
e
s 00041/00006/00741
d D 1.5 02/09/23 17:23:59 boiarino 6 5
c ...
e
s 00000/00000/00747
d D 1.4 02/05/13 13:45:43 boiarino 5 4
c nothing
c 
e
s 00000/00002/00747
d D 1.3 02/04/30 13:59:24 boiarino 4 3
c remove:
c     etNformat(jw,"HBTR","F");
c     etNformat(jw,"TBTR","F");
c 
e
s 00002/00000/00747
d D 1.2 01/11/29 11:35:00 boiarino 3 1
c nothing
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcfit.c
e
s 00747/00000/00000
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
   Purpose and Methods :
              Level = 2: Performs track fitting using the stored wire
                         positions and momentum from the pattern
                         recognition templates
              Level = 4: Final fitting with corrected drift times

  !!!!!!!!!! INSTEAD OF BOTH ANA_FIT() AND ANA_PRFIT() !!!!!!!!!!!!

   Output  : ntr_out, ev_out[][]

   Controls: Level = 2 - hit base tracking
                   = 4 - time base tracking

   Library belongs    : libana.a

   Calls              : dctrfit, dcrotsd, scmatch, dcpdst 

   Created   JUNE-15-1992  Bogdan Niczyporuk
   Modified  JUNE-16-1992  Billy Leung

   Called by sda_anal
*/

#include <stdio.h>
I 6
#include <stdlib.h>
E 6
#include <math.h>
I 6
#include <string.h>
E 6
#include <strings.h>
I 6

E 6
#include "dclib.h"
#include "sdageom.h"
#include "sdadraw.h"
#include "sdacalib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

I 6
#include "prlib.h"

E 6
#define CED 1

#ifdef CED

#define ncol_hbla 16
#define ncol_dpl   8
#define ncol_hbtr  9
#define ncol_tbla 17
#define ncol_tdpl  8
#define ncol_tbtr 10

#endif

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define NLAY   6
#define NDC    36
#define TAN20  0.36397023
#define vflt   29.97925
#define dd     18.45     /* dd = 15*1.23 (1-st SC plane of Backward EC) */
#define nsuper 6

int iucomp_();

void
dcfit_(int *jw, TRevent *ana, int *Level, int *ntr_out,
       float ev_out[ntrmx][36], int *inter, int *ifail)
{
  dcfit(jw, ana, *Level, ntr_out, ev_out, *inter, ifail);
  return;
}

D 6
#include "prlib.h"

E 6
void
dcfit(int *jw, TRevent *ana, int Level, int *ntr_out, float ev_out[ntrmx][36],
           int inter, int *ifail)
{
  DCtrack *track, *track0;
  PRTRACK *prr;
  int i, k, il, ll, it, ip, iw, ilnk, ist, n_trk, isec, iw_,
             if_draw, ie, itr, idsc, ipsc, ihsc, is, isg, ilmin, la,
             iw3, iw15, iw27, itmp, iflg, iret,ind,iw1,iw2;
  float trp[npln][7],svin[9],svinn[9],svout[9],tmat[npln][5][5],
               chifit, sphi, stet, vertex[10], x00[npl_sc],
               phi, dx21, dx32, dy21, dy32, defl, pmean, slope, b,
               d,x,y, u,v,w, x0[3], spsi, tpsi, sml, rat, t0, twL,twR,tm,
               tPR, tFLe, tFLm, tsc, Se, Sm, ev_outx[ntrmx];


  /* Save the initial parameters and replace them with the new ones */

/* NOTE: the number of iterations for Level==2 was (ztfit[0]-1.) in sda */

  n_trk = 0;
  if(ana->ntrack == 0) goto a999;

  if((ind = etNlink(jw,"PATH",0)) <= 0) goto a999;
  prr = (PRTRACK *)&jw[ind];

  /* Set flag "if_draw = 1" if interactive job selected */
  if_draw = inter;

  /* Loop over all track candidates prepared by Pattern Recognition routines
    (Level=4) or over linked track candidates (Level=2) */

  *ifail = 0;
  for(ilnk = 1; ilnk <= ana->ntrack; ilnk++)
  {
    track = (DCtrack *) &ana->track[ilnk-1]; /* set pointer to current track */

    track->ecid.u = 0;
    track->ecid.v = 0;
    track->ecid.w = 0;

    /* Check if fit is successful */

    if(Level == 2 && track->level != 1) continue;
    if(Level == 4 && track->level != 3) continue;

    isec = track->sect;
    if(Level == 2)
    {
      /* Store wire numbers for fitting without drift times */
      for(is=1; is<=nsuper; is++)
      {
        ilmin = (is-1)*NLAY;
        for(la=0; la<NLAY; la++)
        {
          il = la + ilmin;
          ip = il + nst_max;
          
          if(prr[ilnk-1].cluster[is-1].layer[la].nhit > 0)
          {
            track->trk[il].iw = prr[ilnk-1].cluster[is-1].layer[la].iw[0];
            track->trk[il].tdc= prr[ilnk-1].cluster[is-1].layer[la].tdc[0];
            track->trk[il].status = 0.0;              /* Status OK */
          }
          else
          {
            track->trk[il].iw = -1.;
            track->trk[il].status = 1.0;              /* Status notOK */
          }
		  
          track->trk[il].Td = 0.0;                    /* Tdrift [ns] */
          track->trk[il].Dm = 0.0;                    /* Dmeas. [cm] */
          track->trk[il].SigDm =
              sdageom_.pln[isec-1][ip][18]/1.732; /* SIGm [cm] */
          track->trk[il].beta = 0.0;                  /* Velocity beta [c] */
          track->trk[il].Df = 0.0;                    /* Dfit [cm] */
          track->trk[il].s = 0.0;                     /* TrkLen [cm] */
          track->trk[il].Wlen = 0.0;                  /* WireLen [cm] */
          track->trk[il].alfa = 0.0;                  /* Alpha [rad] */
          track->trk[il].sector = 0.;/*10 * isec + 1;*/  /* Analysis Level & Sect */
        }
      }

      /* set initial parameters for fit */

      ip = sdakeys_.ztfit[4];
      /* One may start the fitting at Plane#1 (target  plane)
         or Plane#4 (DC layer#1) */
      if(ip >= 4)
	  {
        for(i=0; i<6; i++) svin[i] = track->vect[i];
      }
      else
      {
        svin[0] = sdakeys_.ztarg[2]; /* x */
        svin[1] = 0.0;               /* y */
        svin[2] = 0.0;               /* z */

        /* search for first non-missing layer */
        il = 0;
        while(track->trk[il].iw <= 0.) il++;
        iw_ = ABS(track->trk[il].iw);

        sphi = atan2(sdageom_.dc_wpmid[isec-1][il][iw_-1][1],
                     sdageom_.dc_wpmid[isec-1][il][iw_-1][0]-svin[0]);
        svin[3] = cos(sphi);         /* Cx */
        svin[4] = sin(sphi);         /* Cy */
        svin[5] = 0.0;               /* Cz */
      }

      svin[6] = track->vect[6]; /* Initial momentum */
      if(svin[6] < 0.02) svin[6] = 0.02;
      svin[7] = 0.0;                 /* Velocity of a particle */

      /* charge of a particle */
      if(sdakeys_.mgtyp == 5 || ABS(sdakeys_.zmagn[1]) < 1.)
      {
        svin[8] = 1.0; /* no field ????????? */
      }
      else
      {
        svin[8] = track->vect[11];
      }
    }
    else
    {
      bcopy((char *)&track->trkp[0], (char *)svin, 9*4);
    }

    /* Fit using the reconstructed drift times (Level=4) or
       Fit to wire positon only (drift times not used) (Level=2) */

    if(sdakeys_.ztfit[0] > 0.)
        dctrfit(jw,isec,Level,trp,&track->trk[0],svin,inter,&chifit);

    /* Check if fit was successful (Level=4) or
       Chi-square (for Fit to wire position) cut (Level=2) */

    if(Level == 2)
    {
      if(chifit > sdakeys_.zcut[3]) continue;
    }
    else
    {
/*printf("CHIFIT(TRFIT)=%f\n",chifit);*/
      if(chifit > sdakeys_.zcut[6])
      {
        chifit = 19.5;
        continue;
      }
    }
    n_trk++;



    /* Swimming back to the target plane#1 */

    if(sdakeys_.ztfit[4] == 4.)
    {
      for(i=0; i<6; i++) svinn[i] = trp[ndc_min-1][i];
      for(i=6; i<9; i++) svinn[i] = svin[i];
      dcswimvt(jw,&sdageom_.pln[isec-1][0][0],isec,svinn,sdakeys_.step,Level,
               if_draw,svout,vertex,trp,&sdageom_.nstep_xyz,sdageom_.xyz);
      if(Level == 4) for(i=0; i<9; i++) svin[i] = svout[i];

      /* If photoproduction get Track Point for Start Counter
         plane#2 or 3 (no PTF) */

      if(sdakeys_.zbeam[0] == 0.)
      {
        if(Level == 2)
        {
          dcswim(&sdageom_.pln[isec-1][0][0],1,3,0,0,svout,
                 trp,tmat,&sdageom_.nstep_xyz,sdageom_.xyz);
/*printf("svout[0]=%f (%f %f %f)\n",svout[0],trp[0][0],trp[1][0],trp[2][0]);*/
        }
        else
        {
          dcswim(&sdageom_.pln[isec-1][0][0],1,3,0,0,svin,
                 trp,tmat,&sdageom_.nstep_xyz,sdageom_.xyz);
        }
      }
    }

    /* Level = 4:
       Store the results of FF fit, which was not rejected from the above cuts
       Level = 2:
       Store the results of PR fit, which was not rejected from the above cuts
    */

    track->level = Level;
    track->sect  = isec;

    /*************************************************************/
    /* fake iw for missing layers to make following piece happy: */
    /*************** iw = ABS(track->trk[il].iw) *****************/
I 6
    /* !!!!!!!!! PEREDELAT' !!!!!!!!! */
E 6
    for(is=0; is<6; is++)
    {
      iw1 = is*NLAY;
D 6
      while(track->trk[iw1].iw <=0.) iw1++;
E 6
I 6
      while(track->trk[iw1].iw <=0.)
      {
/*printf("1track->trk[%2d].iw=%f\n",iw1,track->trk[iw1].iw);*/
        iw1++;
      }
/*printf("1track->trk[%2d].iw=%f\n",iw1,track->trk[iw1].iw);*/
E 6
      iw2 = is*NLAY+5;
D 6
      while(track->trk[iw2].iw <=0.) iw2--;
      slope = (track->trk[iw2].iw - track->trk[iw1].iw)/(float)(iw2-iw1);
E 6
I 6
      while(track->trk[iw2].iw <=0.)
      {
/*printf("2track->trk[%2d].iw=%f\n",iw2,track->trk[iw2].iw);*/
        iw2--;
      }
/*printf("2track->trk[%2d].iw=%f\n",iw2,track->trk[iw2].iw);*/

      if(iw1<iw2)
      {
        slope = (track->trk[iw2].iw - track->trk[iw1].iw)/(float)(iw2-iw1);
      }
      else
      {
        slope = 0.0;
I 7
		/*sergey ???!!!
E 7
        printf("dcfit: WARN: iw1=%d iw2=%d -> use slope=%f\n",iw1,iw2,slope);
I 7
		*/
E 7
      }

E 6
      b = track->trk[iw1].iw - slope*(float)(iw1-is*NLAY);
      for(ll=is*NLAY; ll<is*NLAY+6; ll++)
      {
        if(track->trk[ll].iw <=0.) track->trk[ll].iw = - ( slope*(float)(ll-is*NLAY) + b ); 
D 6
      } 
E 6
I 6
      }

E 6
	  /*printf("is=%1d minmax->%d-%d slope=%f\n",is,iw1,iw2,slope);*/
I 6

E 6
    }
	/*for(ll=0; ll<NDC; ll++) printf("%4.0f",track->trk[ll].iw);printf("\n\n");*/
    /************************************************************/
    /************************************************************/

    for(il=0; il<NDC; il++)
    {
      ip = il + nst_max;
I 6
/*printf("101: il=%d, iw=%f\n",il,track->trk[il].iw);fflush(stdout);*/
E 6
      iw = ABS(track->trk[il].iw);
      track->trk[il].s = trp[il+(ndc_min-1)][6];  /* S */
I 6
/*printf("102\n");fflush(stdout);*/
E 6
      if(il < 12)
      {
I 6
/*printf("103\n");fflush(stdout);*/
/*printf("103-1: il=%d\n",il);fflush(stdout);*/
track->trk[il].Wlen = 0;
/*printf("103-2: %d: %d: %d: %d\n",isec,il,iw,ndc_min);fflush(stdout);*/
E 6
        track->trk[il].Wlen = sdageom_.dc_wlen[isec-1][il][iw-1][1]
                                          - trp[il+(ndc_min-1)][2];  /* Wlen */
I 6
/*printf("104\n");fflush(stdout);*/
E 6
      }
      else
      {
I 6
/*printf("105\n");fflush(stdout);*/
E 6
        track->trk[il].Wlen = sdageom_.dc_wlen[isec-1][il][iw-1][0]
                                          - trp[il+(ndc_min-1)][2];  /* Wlen */
I 6
/*printf("106\n");fflush(stdout);*/
E 6
      }

      /* get 'alfa' angle (for dcfitsl) */
      phi =  atan2(trp[il+(ndc_min-1)][4], trp[il+(ndc_min-1)][3]);
/*
printf("???? %f %f %f %f %f %f -> %f\n",
trp[il+(ndc_min-1)][0],trp[il+(ndc_min-1)][1],trp[il+(ndc_min-1)][2],
trp[il+(ndc_min-1)][3],trp[il+(ndc_min-1)][4],trp[il+(ndc_min-1)][5],phi);
printf("??? alfa0: iw=%d il=%d ip=%d isec=%d -> %f\n",iw,il,ip,isec,sdageom_.pln[isec-1][ip][16]);
*/
      if(iw > sdageom_.pln[isec-1][ip][16]) /* plane part of DC */
      {
        track->trk[il].alfa = phi - atan2(sdageom_.pln[isec-1][ip][4],
                                   sdageom_.pln[isec-1][ip][3]);
/*
printf("??? alfa1=%f (iw=%d il=%d ip=%d) (%f %f %f)\n",track->trk[il].alfa,iw,il,ip,
sdageom_.pln[isec-1][ip][3],sdageom_.pln[isec-1][ip][4],
atan2(sdageom_.pln[isec-1][ip][4],sdageom_.pln[isec-1][ip][3]));
*/
      }
      else
      {
        track->trk[il].alfa = phi - sdageom_.pln[isec-1][ip][11] -
                            (iw-1)*sdageom_.pln[isec-1][ip][12];
/*
printf("??? alfa2=%f (iw=%d il=%d ip=%d)\n",track->trk[il].alfa,iw,il,ip);
*/
      }
      track->trk[il].sector = 10 * isec + Level; /* Level & Sect */
    }







    if(Level == 4) svin[7] = track->trk[0].beta; /* particle velosity (L=4) */

    /* First plane is */
    bcopy((char *)svin, (char *)&track->trkp[0], 9*4);
    track->trkp[0].s = 0.0;                 /* s (track length) */
    track->trkp[0].sector = 10*isec+Level;  /* Analysis Level & Sector */
    track->trkp[0].chisq = chifit;          /* Reduced Chi-Square */
    for(ip=1; ip<npln; ip++)
    {
      bcopy((char *)&trp[ip][0], (char *)&track->trkp[ip], 9*4);
      track->trkp[ip].p  = svin[6];         /* p */
      track->trkp[ip].beta  = svin[7];      /* velocity of a particle */
      track->trkp[ip].q  = svin[8];         /* Q */
      track->trkp[ip].s = trp[ip][6];       /* s (track length) */
      track->trkp[ip].sector=10*isec+Level; /*Analysis Level & Sector*/
      track->trkp[ip].chisq = chifit;       /* Reduced Chi-Square */
    }

    /* Find the {u,v,w} slabs in forward EC where the current track points
       to check if track has reached the EC plane? */

    ip    = nec_min;
    if(track->trkp[ip-1].x <= 999.)
    {
      /* Define "x,y,z,u,v,w" in local EC C.S. {i,j,k} where k perp. to plane */
      d     = sdageom_.pln[isec-1][ip-1][7];
      x0[0] = track->trkp[ip-1].x-sdageom_.pln[isec-1][ip-1][0];
      x0[1] = track->trkp[ip-1].y-sdageom_.pln[isec-1][ip-1][1];
      x0[2] = track->trkp[ip-1].z-sdageom_.pln[isec-1][ip-1][2];
      x = sqrt( (x0[0]-sdageom_.pln[isec-1][ip-1][3]*d)
               *(x0[0]-sdageom_.pln[isec-1][ip-1][3]*d)
               +(x0[1]-sdageom_.pln[isec-1][ip-1][4]*d)
               *(x0[1]-sdageom_.pln[isec-1][ip-1][4]*d) );
      if(x0[0] > sdageom_.pln[isec-1][ip-1][3]*d) x = -x;
      y = x0[2];
      /* Particle direction in the local system */
      phi = atan2(track->trkp[ip-1].Cy,track->trkp[ip-1].Cx)
           -atan2(sdageom_.pln[isec-1][ip-1][4],
                  sdageom_.pln[isec-1][ip-1][3]);
      u = sin(acos(track->trkp[ip-1].Cz))*sin(phi);
      v = track->trkp[ip-1].Cz;
      w = sin(acos(track->trkp[ip-1].Cz))*cos(phi);
      x = x + u*dd/w;
      y = y + v*dd/w;
      spsi = sin(sdageom_.pln[isec-1][ip-1][17]/2.);
      tpsi = tan(sdageom_.pln[isec-1][ip-1][17]/2.);
      sml  = (sdageom_.pln[isec-1][ip-1][7] + dd) * TAN20;
      rat = (sml + x)*tpsi;
      /* Check boundaries of EC triangle at distance "d+dd" from origine
         and fill 'ecid' if it is Ok */
      if(ABS(x) < sml && ABS(y) < rat)
      {
        track->ecid.u = 37. - 18.*(1. - x/sml);
        track->ecid.v = 37. - 18.*spsi*(sml + x + y/tpsi)/sml;
        track->ecid.w = 37. - 18.*spsi*(sml + x - y/tpsi)/sml;
      }
    }

    /* Fill the array "ev_out(36,it)" i.e. DST bank 'EVTB' */

    if((Level==2 && sdakeys_.lanal[0] < 4 || Level==4 && sdakeys_.lanal[0]==4 )
      && (*ntr_out) < ntrmx)
    {
      if(Level==4)
      {
/*
printf("sc %d -> %d %d\n",track->scid.slab,track->scid.tdcl,track->scid.tdcr);
*/
if(track->scid.tdcl>0)
  uthfill(ana->histf, 950+isec, (float)track->scid.slab-0.5, 0., 1.);
if(track->scid.tdcr>0)
  uthfill(ana->histf, 960+isec, (float)track->scid.slab-0.5, 0., 1.);
	  }

      if(Level==2)
      {
        for(i=0; i<npl_sc; i++) x00[i]=track->trkp[nsc_min-1+i].x;
        scmatch(jw, ana, isec, &track->scid, x00, &iflg);
        /*boy if(iflg==1) GOTO 30;*/
      }
      (*ntr_out)++;
      it = *ntr_out;
      dcrotsd(&ev_out[it-1][0],svin,&isec);
      ev_out[it-1][9] = ilnk;
      ev_out[it-1][10] = 10*isec + Level;
      ev_out[it-1][11] = chifit;

	  /* Tagging electron track or track with Beta determined
         using ST-counter & ToF */
      if(Level == 2)
      {
        ev_out[it-1][0]  = 0.0;
      }
      else
      {
        ev_outx[it-1] = ev_out[it-1][0]; /* save X vertex */
        ev_out[it-1][0] = 0.0;
        if(ilnk == ana->itrk0)
        {
          ev_out[it-1][0] = 1.0;
          if(sdakeys_.zbeam[0] == 0.) ev_out[it-1][0] = ana->itag0;
        }
      }

      /* Add the trajectory prediction at plane of CC */
      ip = ncc_min;
      il = 13;
      for(i=0; i<6; i++)
      {
        bcopy((char *)&track->trkp[ip-1], (char *)&ev_out[it-1][il-1], 6*4);
        il++;
      }
      ev_out[it-1][18] = track->trkp[ip-1].s;
      ev_out[it-1][19] = 0.0;

      /* Add the trajectory prediction at plane#1 of SC */
      ip = track->scid.plane;
      il = 21;
      for(i=0; i<6; i++)
      {
        bcopy((char *)&track->trkp[ip-1], (char *)&ev_out[it-1][il-1], 6*4);
        il++;
      }
      ev_out[it-1][26] = track->trkp[ip-1].s;
      ev_out[it-1][27] = track->scid.slab + 100*track->scid.plane
                                        + 10000*track->scid.hit;

      /* Add the trajectory prediction at plane of EC */
      ip = nec_min;
      il = 29;
      for(i=0; i<6; i++)
      {
        bcopy((char *)&track->trkp[ip-1], (char *)&ev_out[it-1][il-1], 6*4);
        il++;
      }
      ev_out[it-1][34] = track->trkp[ip-1].s;
      ev_out[it-1][35] = track->ecid.u +100*track->ecid.v +10000*track->ecid.w;

      if(Level==4)
      {
/*
printf("ec %d %d %d\n",track->ecid.u,track->ecid.v,track->ecid.w);
*/
uthfill(ana->histf, 970+isec, (float)track->ecid.u-0.5, 0., 1.);
uthfill(ana->histf, 980+isec, (float)track->ecid.v-0.5, 0., 1.);
uthfill(ana->histf, 990+isec, (float)track->ecid.w-0.5, 0., 1.);
	  }
    }

    /* Store trajectories for drawing (same code for L2 & L4 - PEREDELAT')*/
    if(if_draw == 1)
    {
      if(Level == 2 && sdadraw_.ntrdraw0 < max_draw)
      {
        sdadraw_.ntrdraw0++;
        sdadraw_.draw0_isec[sdadraw_.ntrdraw0-1] = isec;
        sdadraw_.draw0_nstep[sdadraw_.ntrdraw0-1] = sdageom_.nstep_xyz;
        for(ist=1; ist<=sdageom_.nstep_xyz; ist++)
        {
          sdadraw_.draw0_xyz[sdadraw_.ntrdraw0-1][ist-1][0] =
                                          sdageom_.xyz[ist-1][0];
          sdadraw_.draw0_xyz[sdadraw_.ntrdraw0-1][ist-1][1] =
                                          sdageom_.xyz[ist-1][1];
          sdadraw_.draw0_xyz[sdadraw_.ntrdraw0-1][ist-1][2] =
                                          sdageom_.xyz[ist-1][2];
        }
        /* Store ST hits */
        k = 0;
        for(il=nst_min; il<=nst_max; il++)
        {
          k++;
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][0] = trp[il-1][0];
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][1] = trp[il-1][1];
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][2] = trp[il-1][2];
        }
        /* Store CC, SC and EC hits */
        k = 2;
        for(il=ncc_min; il<=nec_max; il++)
        {
          k++;
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][0] = trp[il-1][0];
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][1] = trp[il-1][1];
          sdadraw_.draw0_hit[sdadraw_.ntrdraw0-1][k-1][2] = trp[il-1][2];
        }
      }
      else if(Level == 4 && sdadraw_.ntrdraw < max_draw)
      {
        sdadraw_.ntrdraw++;
        sdadraw_.draw_isec[sdadraw_.ntrdraw-1] = isec;
        sdadraw_.draw_nstep[sdadraw_.ntrdraw-1] = sdageom_.nstep_xyz;
        for(ist=1; ist<=sdageom_.nstep_xyz; ist++)
        {
          sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist-1][0] =
                                          sdageom_.xyz[ist-1][0];
          sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist-1][1] =
                                          sdageom_.xyz[ist-1][1];
          sdadraw_.draw_xyz[sdadraw_.ntrdraw-1][ist-1][2] =
                                          sdageom_.xyz[ist-1][2];
        }
        /* Store ST hits */
        k = 0;
        for(il=nst_min; il<=nst_max; il++)
        {
          k++;
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][0] = trp[il-1][0];
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][1] = trp[il-1][1];
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][2] = trp[il-1][2];
        }
        /* Store CC, SC and EC hits */
        k = 2;
        for(il=ncc_min; il<=nec_max; il++)
        {
          k++;
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][0] = trp[il-1][0];
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][1] = trp[il-1][1];
          sdadraw_.draw_hit[sdadraw_.ntrdraw-1][k-1][2] = trp[il-1][2];
        }
      }    
    }

    /* Print track info for FF fit - former dcptrk() */

    if(Level == 2 && sdakeys_.lprnt[3] == 1 ||
       Level == 4 && sdakeys_.lprnt[5] == 1)
    {
      int i, il, ip, iw, iq, isl, Level;
      char *det[npln];

      /* was 'if(first)', but then should be static ... */
      {
        det[0] = "VT";
        for(ip = nst_min; ip<=nst_max; ip++) det[ip-1] = "ST";
        for(ip = ndc_min; ip<=ndc_max; ip++) det[ip-1] = "DC";
        det[ncc_min-1] = "CC";
        for(ip = nsc_min; ip<=nsc_max; ip++) det[ip-1] = "SC";
        for(ip = nec_min; ip<=nec_max; ip++) det[ip-1] = "EC";
      }

      Level = track->trkp[0].sector;
      printf("\n\n LEVEL=%2d  Sector=%2d  itrk=%2d  CHISQ=%10.3e\n",
              Level,isec,ilnk,track->trkp[0].chisq);
	  /* replace by info from prlib.h
      printf(" Linked Clusters in SLs =");
      for(i=0; i<6; i++) printf("%4d",track->clust[i]);
      if(Level == 2)
      {
        printf("\n Linked Segments in SLs =");
        for(i=0; i<6; i++) printf("%4d",track->segm[i]);
      }
	  */
      if(Level == 4)
      {
        printf("\n Linked Segments in SLs =");
        /* next line commented out - dcdcam() don't fill this array any more*/
        /*for(i=0; i<6; i++) printf("%4d",ana->dca_segm[ilnk-1][isec-1][i]);*/
      }

      printf("\n il   iw    tdc     Td     Dm   SIGm   beta"
             "     Dt   Tlen   Wlen   alfa  S/L Stat\n");
      for(il=1; il<=npl_dc; il++)
      {
        iw = track->trk[il-1].iw;
        isl= track->trk[il-1].sector;
        if(il == 5 || il == 6) continue;
        if(fmod(il,6) == 1 && il > 1) printf("\n");
        printf("%3d%5d%7.1f%7.1f%7.3f%7.3f%7.4f%7.3f%7.1f%7.1f%7.4f%5d%5.1f\n",
                il, iw,
                track->trk[il-1].tdc,  track->trk[il-1].Td,
                track->trk[il-1].Dm,   track->trk[il-1].SigDm,
                track->trk[il-1].beta, track->trk[il-1].Df,
                track->trk[il-1].s,    track->trk[il-1].Wlen,
                track->trk[il-1].alfa, isl, track->trk[il-1].status);
      }

      printf("\n          X        Y        Z     Cx     Cy     Cz"
             "      p     beta   Q   Tlen S/L\n");
      for(ip = 1; ip<=npln; ip++)
      {
        if(ip > 3 && ip <= 39) continue;
        iq = track->trkp[ip-1].q;
        isl= track->trkp[ip-1].sector;
       printf("%2s %2d%9.3f%9.3f%9.3f%7.4f%7.4f%7.4f %6.3f %6.4f%3d%7.2f%4d\n",
                det[ip-1], ip,
                track->trkp[ip-1].x,  track->trkp[ip-1].y,
                track->trkp[ip-1].z,  track->trkp[ip-1].Cx,
                track->trkp[ip-1].Cy, track->trkp[ip-1].Cz,
                track->trkp[ip-1].p,  track->trkp[ip-1].beta,
                iq, track->trkp[ip-1].s, isl);
      }
    }


  } /* End of loop over linked clusters (ilnk) */
  *ifail = n_trk;

  /* if Level 4 (TBT) - recalculate the particle's velocity */

  if(Level == 4)
  {
D 7
    int scndig, scdigi[48][5]; /* SC data */
E 7
I 7
    int scndig, scdigi[57][5]; /* SC data */
E 7
    if((*ntr_out) == 0) goto a999;

    if(sdakeys_.ifsim == 1)
    {
      ie = 0;
      track0 = (DCtrack *) &ana->track[ana->itrk0-1]; /* pointer to track */
      for(it = 1; it<=(*ntr_out); it++)
      {
        if( ev_out[it-1][9] == ana->itrk0 )
        {
          ie = it;
          idsc = track0->scid.slab;
          ipsc = track0->scid.plane;
          ihsc = track0->scid.hit;
          isec = track0->sect;

          GETSCDATA;
          scswalk(jw,isec,idsc,ihsc,track0->trkp[ipsc-1].z,&tPR,&tFLe);
          ev_out[it-1][7] = ana->beta0;
          Se = track0->trkp[ipsc-1].s;
        }
      }
      if(ie == 0) goto a999;
      itr = 0;
      for(it = 1; it<=(*ntr_out); it++)
      {
        if(ev_out[it-1][9] != ana->itrk0)
        {
          itr = ev_out[it-1][9];
          track = (DCtrack *) &ana->track[itr-1]; /* pointer to track */
          idsc = track->scid.slab;
          ipsc = track->scid.plane;
          ihsc = track->scid.hit;
          isec = track->sect;

          GETSCDATA;
          scswalk(jw,isec,idsc,ihsc,track->trkp[ipsc-1].z,&tPR,&tFLm);
          Sm   = track->trkp[ipsc-1].s;
          ev_out[it-1][7] = Sm * ana->beta0
                     / (vflt*ana->beta0*(tFLm - tFLe) + Se);
        }
      }
    }
    else
    {
      ie = 0;
      for(it=1; it<=(*ntr_out); it++)
      {
        if(ev_out[it-1][9] == ana->itrk0)
        {
          track0 = (DCtrack *)&ana->track[ana->itrk0-1]; /* pointer to track */
          ie = it;
          idsc = track0->scid.slab;
          ipsc = track0->scid.plane;
          ihsc = track0->scid.hit;
          isec = track0->sect;

          GETSCDATA;

          sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
          Se = track0->trkp[ipsc-1].s;
          t0 = tsc - Se/vflt/ana->beta0;
          ev_out[it-1][7] = ana->beta0;
        }
      }
      if(ie == 0) goto a999;
      for(it=1; it<=(*ntr_out); it++)
      {
        if(ev_out[it-1][9] != ana->itrk0)
        {
          itr = ev_out[it-1][9];
          track = (DCtrack *) &ana->track[itr-1]; /* pointer to track */
          idsc = track->scid.slab;
          ipsc = track->scid.plane;
          ihsc = track->scid.hit;
          isec = track->sect;

          GETSCDATA;

          sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
          Sm   = track->trkp[ipsc-1].s;
          tFLm  = tsc - t0;
          ev_out[it-1][7] = Sm/(vflt*tFLm);
        }
      }
    }
  }

#ifdef CED

  /* fill RECSIS banks */

  if(Level == 2)
  {
I 3
D 4
    etNformat(jw,"HBTR","F");
E 4
E 3
    tr_makeHBTR(jw, ana, *ntr_out, ev_out, ev_outx);
  }
  else
  {
    /*tr_makeDC1(jw);*/
I 3
D 4
    etNformat(jw,"TBTR","F");
E 4
E 3
    tr_makeTBTR(jw, ana, *ntr_out, ev_out, ev_outx);
  }

#endif

a999:

  return;
}






E 1
