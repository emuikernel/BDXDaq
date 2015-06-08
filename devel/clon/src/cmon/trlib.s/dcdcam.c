/*
   dcdcam.c - derive "dcam" for each hit of the candidate track

   Inputs  : include files

   Controls: ifail

   Library belongs    : libsda.a

   Calls              : scmatch, scbeta, scdtime, dcdocam, dclramb

   Created   JUN-12-1992  Bogdan Niczyporuk
   Modified  FEB-28-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by sda_anal
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "dclib.h"
#include "uthbook.h"
#include "sdageom.h"
#include "sdacalib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define NLAY   6
#define vflt   29.97925
#define nsuper 6

#define NWDC1 (6*36*192)
#define NWDC  (NWDC1+NWDC1+6+1+1) /* 82952 */

#define NWTD  (2*200*36+3*36)


/* returns DC constants from SDA calibration file */

void
dcreadcalib_(int *runno, float tc_dc[6][36][192], int dc_stat[6][36][192],
             float vprp_dc[6], float *Tsl_dc, float *vgas,
             float td_h[36][3], float td_f[36][200][2])
{
  FILE *fd;
  int i;
  float data[NWDC], *cdc, *in;
  char *parm, fname[256];

  if((parm = (char *) getenv("CLON_PARMS")) == NULL)
  {
    printf("dcdcam: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }

  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.cbosfile);
  if((fd=fopen(fname,"r")) != NULL)
  {
    printf("dcdcam: reading calibration file >%s<\n",fname);
    fclose(fd);
  }
  else
  {
    printf("dcdcam: error opening calibration file >%s< - exit\n",fname);
    exit(0);
  }

  if(utGetFpackRecord(fname, "CDC ", NWDC, data))
  {
    in = data;
    cdc = (float *) tc_dc;
    for(i=0; i<NWDC1; i++) cdc[i] = *in++;
    cdc = (float *) dc_stat;
    for(i=0; i<NWDC1; i++) cdc[i] = *in++;
    for(i=0; i<6; i++) vprp_dc[i] = *in++;
    *Tsl_dc = *in++;
    *vgas = *in++;
  }

  if(utGetFpackRecord(fname, "DCTD", NWTD, data))
  {
    in = data;
    cdc = (float *) td_h;
    for(i=0; i<(36*3); i++) cdc[i] = *in++;
    cdc = (float *) td_f;
    for(i=0; i<(36*200*2); i++) cdc[i] = *in++;
  }

  return;
}


/* returns DC constants from MAP */

void
dcgetcalib_(int *runno, float tc_dc[6][36][192], int dc_stat[6][36][192],
             float vprp_dc[6], float *Tsl_dc, float *vgas,
             float td_h[36][3], float td_f[36][200][2])
{
  FILE *fd;
  int i,j,isec,is,il,iret,ir,iwmin,iwmax,*dcst,dc_st[36][192],run,Np;
  float del,ddel,val,dc2tof,reg4pul[3],*t0,t0_dc[36][192];
  char map[1000], map1[1000], *dir;
  char *SecNo[6] = {"sector1","sector2","sector3",
                    "sector4","sector5","sector6"};
  /* Dec97 Runs >= 7841 */
  float delDC[6][6] = { 1020., 1020.,  2150., 2140.,  3135., 3135.,
                        1020., 1020.,  2150., 2150.,  3135., 3150.,
                        1020., 1020.,  2050., 2050.,  3035., 3040.,
                        1020., 1020.,  2050., 2050.,  3035., 3105.,
                        1030., 1030.,  2050., 2050.,  3125., 3130.,
                        1020., 1020.,  2150., 2150.,  3135., 3140. };

  run = *runno;
  t0 = (float *) t0_dc;
  dcst = (int *) dc_st;

  /* SDA calibrations */

  ddel= 70.;
  for(isec=1; isec<=6; isec++)
  {
    for(il=1; il<=36; il++)
    {
      is = (il - 1)/6 + 1;
      del = delDC[isec-1][is-1];
      for(i=1; i<=192; i++)
      {
        val = del;
        if(isec == 6)
        {
          if(il == 1  && (i >= 8  && i <= 23)) val = del - ddel;
          if(il == 2  && (i >= 7  && i <= 22)) val = del - ddel;
          if(il == 3  && (i >= 7  && i <= 22)) val = del - ddel;
          if(il == 4  && (i >= 6  && i <= 21)) val = del - ddel;
          if(il == 7  && (i >= 12 && i <= 27)) val = del - ddel;
          if(il == 8  && (i >= 11 && i <= 26)) val = del - ddel;
          if(il == 9  && (i >= 11 && i <= 26)) val = del - ddel;
          if(il == 10 && (i >= 10 && i <= 25)) val = del - ddel;
          if(il == 11 && (i >= 10 && i <= 25)) val = del - ddel;
          if(il == 12 && (i >= 9  && i <= 24)) val = del - ddel;
        }
        tc_dc[isec-1][il-1][i-1] = val;  /* DL1 - Tc [ns] for each wire */
      }
    }
  }

  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("dcdcam: CLON_PARMS not defined !!!\n");
    exit(0);
  }

  sprintf(map,"%s/Maps/%s.map",dir,"DC_TDLY");
  if((fd=fopen(map,"r")) != NULL)
  {
    printf("dcdcam: reading calibration map file >%s< for run # %d\n",map,run);
    fclose(fd);
  }
  else
  {
    printf("dcdcam: error opening map file >%s< - exit\n",map);
    exit(0);
  }

  sprintf(map1,"%s/Maps/%s.map",dir,"DC_STATUS");
  if((fd=fopen(map1,"r")) != NULL)
  {
    printf("dcdcam: reading calibration file >%s< for run # %d\n",map1,run);
    fclose(fd);
  }
  else
  {
    printf("dcdcam: error opening map file >%s< - exit\n",map1);
    exit(0);
  }

  /* Read pulser constants (MAP) for DC */

  map_get_float(map,"Delays","DC2TOF",        1, &dc2tof, run, &iret);
  map_get_float(map,"Delays","Region4Pulser", 3, reg4pul, run, &iret);

  /* E-beam
  dc2tof     =  dc2tof    - 0.; 
  reg4pul[0] = reg4pul[0] - 10.;
  reg4pul[1] = reg4pul[1] - 10.;
  reg4pul[2] = reg4pul[2] - 10.;
  */

  /* Photon beam
  dc2tof     =  dc2tof    - 0.;
  reg4pul[0] = reg4pul[0] - 5.;
  reg4pul[1] = reg4pul[1] - 5.;
  reg4pul[2] = reg4pul[2] - 5.;
  */

  for(il=0; il<36; il++) for(i=1; i<=192; i++) dc_st[il][i] = 0;

  for(isec=0; isec<6; isec++)
  {
    map_get_float(map,SecNo[isec],"T0",     6912, t0,   run, &iret);
    map_get_int(map1, SecNo[isec],"status", 6912, dcst, run, &iret);
    for(il=1; il<=36; il++)
    {
      ir = (il - 1)/12 + 1;
      iwmin = sdageom_.pln[isec][il+3-1][14];
      iwmax = sdageom_.pln[isec][il+3-1][15];
      for(i=1; i<=192; i++)
      {
        tc_dc[isec][il-1][i-1]   = t0_dc[il-1][i-1] + reg4pul[ir-1] + dc2tof;
        dc_stat[isec][il-1][i-1] = dc_st[il-1][i-1];
        if(i < iwmin || i > iwmax) dc_stat[isec][il-1][i-1] = 1;
      }
    }
  }

  /* velocity along a wire [cm/ns] per SLayer */
  for(is=0; is<6; is++) vprp_dc[is] = 16.0;

  *Tsl_dc = 2.;     /* slope [cnts/ns] */
  *vgas   = 0.005;  /* drift velocity [cm/ns] */

  /* SDA time-to-distance file */
/* WHAT WAS THIS ABOUT ????????????????????
printf("ERROR in dcdcam.c: file not there !!! ???\n");
exit(0);
*/
  if((fd=fopen("$CLON_PARMS/TRLIB/DCtd.dat","r")) != NULL)
  {
    printf("dcdcam: read 'DCtd.dat' file\n");
    for(il=1; il<=36; il++)
    {
      fscanf(fd,"%f%f%f",&td_h[il-1][0],&td_h[il-1][1],&td_h[il-1][2]);
      Np = td_h[il-1][0];
      for(j=0; j<Np; j++)
      {
        fscanf(fd,"%f%f",&td_f[il-1][j][0],&td_f[il-1][j][1]);
      }
    }
    fclose(fd);
  }

printf("Sergey 3-nov-2007: try to use DC Maps, nothing works, use Bogdan's file instead\n");
exit(0);

  return;
}


void
dcdcam_(int *jw, TRevent *ana, int *ifail)
{
  dcdcam(jw, ana, ifail);
  return;
}

#include "prlib.h"

void
dcdcam(int *jw, TRevent *ana, int *ifail)
{
  DCtrack *track, *track0;
  PRTRACK *prr;
  int i,k,il,iflg,ilmin,is,ilnk,isg,la,lab,ind,il0,iw;
  int nsgc,n_seg,n_trk,isec,Level,ndigi[NLAY];
  float chibest, alfa, beta, betap, betag, slopD, dat0,dat0B,slopDbest,
         chib, cut, digib[NLAY][5], IntSg[ntrmx][6];
  float tFLe, Se, tzero, t0, tFLd, tPRd, digi[NLAY][nhlmx][5];
  int ipsc, ihsc, idsc;
  static int NLSL[6] = {4, 6, 6, 6, 6, 6};

  Level = 3;
  n_trk = 0;
  *ifail = 0;
  cut = sdakeys_.zcut[5];

  if((ind = etNlink(jw,"PATH",0)) <= 0) return;
  prr = (PRTRACK *)&jw[ind];

  /* Loop over linked track candidates */

  if(ana->ntrack == 0) return;

  /* Loop over Track Candidates (linked Clusters "ntrack") */

  for(ilnk=1; ilnk<=ana->ntrack; ilnk++)
  {
    track = (DCtrack *) &ana->track[ilnk-1]; /* set pointer to current track */

    /* Check if HBT was successful */
    if(track->level != 2) continue;
    isec = track->sect;
    if(sdakeys_.lprnt[4] == 1) printf("\n\n LEVEL=%2d  Sector=%2d  itrk=%2d"
                             "  (L/R Ambig)\n",Level,isec,ilnk);

    /* Find SC slab "id" for track candidate "ilnk" other then electron */
    if(ilnk != ana->itrk0)
    {
      {
        float x0[npl_sc];
        for(i=0; i<npl_sc; i++) x0[i] = track->trkp[nsc_min-1+i].x;
        scmatch(jw, ana, isec, &track->scid, x0, &iflg);
      }
/*printf("dcdcam1 %d\n",ilnk);*/
      if(iflg > 0) continue;
/*printf("dcdcam2 %d\n",ilnk);*/
    }

    /* Determine the velocity "beta" from Time of Flight */
    scbeta(jw,ana,ilnk,sdakeys_.ifsim,&beta,&iflg,&tFLe,&Se,&tzero);

/*printf("dcdcam3 %d\n",ilnk);*/
    if(iflg > 0) continue;
/*printf("dcdcam4 %d\n",ilnk);*/

    /* Resolve the L-R ambiguity */
    n_seg   = 0;
    idsc = track->scid.slab;  /* for scdtime */
    ipsc = track->scid.plane; /* for scdtime */
    ihsc = track->scid.hit;   /* for scdtime */
    for(is=1; is<=nsuper; is++)
    {
      ilmin = (is-1)*NLAY;
      alfa = track->trk[ilmin+2].alfa;
/*printf("alfa[%1d]=%f (%f)\n",is,alfa,track->trk[ilmin].alfa); fflush(stdout);*/
      /* Get t0 */
      scdtime(jw,ana,is,ilnk,idsc,ipsc,ihsc,isec,beta,&t0);

      /* Get iw and tdc[counts] */
      for(la=0; la<NLAY; la++)
      {
        ndigi[la] = prr[ilnk-1].cluster[is-1].layer[la].nhit;
/*
printf("dcdcam: ilnk=%d is=%d la=%d ndigi=%d\n",ilnk,is,la,ndigi[la]); fflush(stdout);
*/
        for(k=0; k<ndigi[la]; k++)
        {
          digi[la][k][0] = prr[ilnk-1].cluster[is-1].layer[la].iw[k];
          digi[la][k][1] = prr[ilnk-1].cluster[is-1].layer[la].tdc[k];
        }
      }
/*
printf("dcdcam: ndigi-> %d %d %d %d %d %d\n",
ndigi[0],ndigi[1],ndigi[2],ndigi[3],ndigi[4],ndigi[5]); fflush(stdout);
*/
      /* Derive drift time from raw TDC */
      il0 = (is - 1) * NLAY;
      for(la=0; la<NLAY; la++)
      {
        for(k=0; k<ndigi[la]; k++)
        {
          iw = digi[la][k][0];
          if(iw<=0 || iw>192)
          {
            printf("dcdcam: ERROR iw=%d (is=%d digi[%1d][%1d][0]=%f)\n",
              iw,is,la,k,digi[la][k][0]); fflush(stdout);
            continue;
          }
          il = il0 + la;
          tFLd = track->trk[il].s / (beta * vflt);
          tPRd = track->trk[il].Wlen / sdacalib_.vprp_dc[is-1];
          digi[la][k][2] = sdacalib_.tc_dc[isec-1][il][iw-1]
                  - tPRd - tFLd - digi[la][k][1] / sdacalib_.Tsl_dc + t0;
        }
      }

      /* Get measured distance of closest approach */
      /* bad hits will be removed from 'digi' array */
      dcdocam(is,ndigi,digi);

      /* fit hits in one superlayer (so-called L-R Ambiguity) */
      /* bad hits will be marked as negative wire number */
      dcfitsl(isec,is,ndigi,digi,alfa, &chibest,&slopDbest,&dat0B,digib);


      if(sdakeys_.lprnt[4] == 1)
      {
        printf(" SL CL    La1    La2"
                   "    La3    La4    La5    La6            SlopD / Chi2\n");
        printf("\n%3d%3d%7.0f%7.0f%7.0f%7.0f%7.0f%7.0f       %7.3f\n"
                "          %7.3f%7.3f%7.3f%7.3f%7.3f%7.3f   %12.5e\n\n",
              is,0,digib[0][0],digib[1][0],digib[2][0],digib[3][0],
              digib[4][0],digib[5][0],slopDbest,digib[0][3],
              digib[1][3],digib[2][3],digib[3][3],digib[4][3],digib[5][3],
              chibest);
      }

      /* Check if L-R Ambiguity was successfully resolved */
      if(chibest > 20.0) chibest = 19.5;
/*printf("dcdcam: at is=%d chibest=%f (cut=%f)\n",is,chibest,cut); fflush(stdout);*/
      if(chibest > cut) continue; /* skip superlayer */
      n_seg++;
 
      /* Store info in "trk( , , )" array needed for final fit (with DT) */

      IntSg[ilnk-1][is-1] = dat0B;
      for(la=0; la<NLAY; la++)
      {
        il = la + ilmin;
        track->trk[il].iw    = digib[la][0]; /* iw */
        if(digib[la][0] > 0.)
        {
          uthfill(ana->histf, 900+((is-1)/2)*10+isec,
          (float)(digib[la][0]), ((float)(il)+0.5),1.);
        }
        track->trk[il].tdc   = digib[la][1]; /* Raw tdc [cnts] */
        track->trk[il].Td    = digib[la][2]; /* Td [ns] */
        track->trk[il].Dm    = digib[la][3]; /* Dm [cm] */
        track->trk[il].SigDm = digib[la][4]; /* SigDm [cm] */
        if(ana->itrk0 == ilnk)
        {
          track->trk[il].beta = ana->beta0; /* Velocity = beta0[c] */
        }
        else
        {
          track->trk[il].beta = beta; /* Velocity = beta [c] */
        }
        track->trk[il].sector = 10*isec + Level; /* Analysis Level & Sect */
        if(track->trk[il].iw > 0.)
        {
          track->trk[il].status = 0.;              /* Status OK */
        }
        else
		{
          track->trk[il].status = 1.;              /* Status not OK */
		}
      }
    } /* End of loop over the superlayers */

    /* Count tracks with resolved L-R Ambiguity in six SuperLayers */

/*printf("dcdcam5 %d (%d %f)\n",ilnk,n_seg,sdakeys_.zcut[2]); fflush(stdout);*/
    if(n_seg == sdakeys_.zcut[2])
    {
/*printf("dcdcam6 %d (%d %f)\n",ilnk,n_seg,sdakeys_.zcut[2]); fflush(stdout);*/
      n_trk++;
      track->level = Level;
      track->sect  = isec;
    }

  } /* End of loop over linked clusters */
  *ifail = n_trk;

  return;
}










