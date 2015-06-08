/*
   Purpose and Methods : Perform a fit of the track candidate.


   Inputs  : isec      - Sector number being analyzed
             Level     - Level of analysis
             svin(9)   - Starting (final) values of the track parameters

    /sdakeys/ - Control Cards
          Ndbg   - Number of events to debug (TRIG card)
          ievt   - Current event number
          mgtyp  - Selected type of the magnetic field (MAGN card)
                     =0 for TABL - table of magnetic fild
                     =1 for TORU - calculates field for torus
                     =2 for MINT - calculates field for mini torus
                     =3 for TOPM - calculates field for torus + mini torus
                     =4 for UNIF - uniform field (along wire) of -3.0 kG
                     =5 for NONE - No magnetic field
          lmode  - Interactive or Batch version (MODE card)
          isw    - Contains parameters of SWIT card
          lmatx  - Contains parameters of MATX card
          lout   - Contains parameters of LOUT card
          lanal  - Contains parameters of ANAL card
          ztfit  - Contains parameters of TFIT card
          ztarg  - Contains parameters of TARG card
          zbeam  - Contains parameters of BEAM card

          trp   - Swimming results
          trp(1,ip) |
          trp(2,ip) |--> Track point {x,y,z} on a plane 
          trp(3,ip) |
          trp(4,ip)  |
          trp(5,ip)  |--> Track direction at track point on a plane
          trp(6,ip)  |
          trp(7,ip)  --> Track length from the target to a layer [cm]
          tmat(5,5,ip)-> Transport matrix at track point on a plane

    /sdageom/dc_wpmid  - DC Wire position
          dc_wpmid(1,iw,il,isec) |
          dc_wpmid(2,iw,il,isec) |-> Wire position in the middle plane
          dc_wpmid(3,iw,il,isec) |

    /sdageom/dc_wdir   - DC Wire Direction
          dc_wdir(1,iw,il,isec)  |
          dc_wdir(2,iw,il,isec)  |-> Wire direction (unit vector)
          dc_wdir(3,iw,il,isec)  |

    trk[il-1][12]   - DC track info for layer "il" & track "itr"

      trk[il-1][0]  - Wire number                                {L=2,3}
      trk[il-1][3]  - Measured distance of closest approach [cm] {L = 3}
      trk[il-1][4]  - Sigma of distance of closest approach [cm] {L=2,3}
      trk[il-1][6]  - Fitted distance of closest approach [cm]   {L=2,4}
      trk[il-1][11] - Status of the hit in the layer "il"        {L=2,3}

   Outputs : svin(9)   - fitted (corrected) track parameters
                         {x,y,z,Px/P,Py/P,Pz/P,p,m,Q} at the target position.
             chifit    - Chi Square of the fit

   Controls:             See DATA card "TFIT"


   Library belongs    : libana.a

   Calls              : dcswim, dcmxinv, dcmvprod

   Created   30-OCT-1990   Bogdan Niczyporuk
   Modified  22-JUNE-1992  Billy Leung (IMPLICIT NONE implemented)
   C remake by Sergey Boyarinov

   Called by ana_prfit, ana_fit
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdageom.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define BIG    1.E+10
#define PIBY2  PI/2.
#define RADDEG 180./PI

#define MACROS(ind) \
        dtrsp[ilay-1][0] = tmat[ind][0][3]; \
        dtrsp[ilay-1][1] = tmat[ind][1][4] * u[2]; \
        dtrsp[ilay-1][2] = tmat[ind][2][3]; \
        dtrsp[ilay-1][3] = tmat[ind][3][3]; \
        dtrsp[ilay-1][4] = tmat[ind][4][4] * u[2]; \
	    /* No magnetic field but transport matrix was computed (matx=1) */ \
        if(sdakeys_.mgtyp == 5) \
        { \
          diff[0] = trp[ind][0] - svin[0]; \
          diff[1] = trp[ind][1] - svin[1]; \
          diff[2] = trp[ind][2] - svin[2]; \
          itmp = 3; \
          dtrsp[ilay-1][0] = -svin[6] * vdot_(diff,u,&itmp); \
        }


int iucomp_();
float vdot_();

void
dctrfit_(int *jw, int *isec, int *Level, float trp[npln][7],
                DCtrk trk[36], float svin[9], int *inter, float *chifit)
{
  dctrfit(jw, *isec, *Level, trp, trk, svin, *inter, chifit);
  return;
}

void
dctrfit(int *jw, int isec, int Level, float trp[npln][7],
                DCtrk trk[36], float svin[9], int inter, float *chifit)
{
  int i, j, k, il, iw, is, ilay, iter, iterm, iterl, sec;
  int if_mat, if_draw, maxTM, itmp;
  int i1, i2, i3, ipmin, ipmax, ilmin, ilmax;
  float u0[3],u[3],a0[3],vt0[3];
  float dtrsp[npl_dc][5], vx[ndc_max];
  float tmat[npln][5][5];
  float alfa[5][5], beta[5], dx[5], weight[ndc_max];
  float diff[3], temp[3];
  float chisq, chi2old, cosL0, d0, z0;
  float dcatr, det, free;
  float lambda, lambda0, sphi;
  float tht, fii, fii0, thetd, phid, sigm;
  float pabs, chi_dif;

  static float ex[3] = {1.,0.,0.}, ez[3] = {0.,0.,1.};
  /* Trajectory migration due to multiple scattering for p = 1GeV
  static float ems[npl_dc] = { 0.014, 0.014, 0.014, 0.014, 0.014, 0.014,
                               0.016, 0.016, 0.016, 0.016, 0.016, 0.016,
                               0.121, 0.121, 0.121, 0.121, 0.121, 0.121,
                               0.139, 0.139, 0.139, 0.139, 0.139, 0.139,
                               0.298, 0.298, 0.298, 0.298, 0.298, 0.298,
                               0.340, 0.340, 0.340, 0.340, 0.340, 0.340 };
  */

  sec = isec - 1;
  *chifit  = 1000.0; /* Initialize chi2old to a big value */
  chi2old = BIG;
  if(sdakeys_.ztfit[3] > 0.0) chi_dif = sdakeys_.ztfit[3];
  else                        chi_dif = 0.10;

  /* Form the uvz unit-vectors co-ordinates in the direction of the
     initial position of the track */

  cross_(ez,&svin[3],u0);
  itmp = 3;
  vunit_(u0,u0,&itmp);

  /* ... */

  ipmin  = sdakeys_.ztfit[4];
  ipmax  = sdakeys_.ztfit[5];
  if(ipmin >= 4)
  {
    ilmin  = ipmin - 3;
    ilmax  = ipmax - 3;
    iw = ABS(trk[ilmin-1].iw);
    vt0[0] = sdageom_.dc_wpmid[sec][ilmin-1][iw-1][0];
    vt0[1] = sdageom_.dc_wpmid[sec][ilmin-1][iw-1][1];
    vt0[2] = sdageom_.dc_wpmid[sec][ilmin-1][iw-1][2];
  }
  else
  {
    ilmin  = 1;
    ilmax  = ipmax - 3;
    vt0[0] = 0.0;
    vt0[1] = 0.0;
    vt0[2] = 0.0;
    sda_vertex_(jw,&isec,vt0);
  }
  diff[0] = svin[0] - vt0[0];
  diff[1] = svin[1] - vt0[1];
  diff[2] = svin[2] - vt0[2];

  /* --- */

  d0 = vdot_(diff,u0,&itmp);
  z0 = svin[2];
  sphi  = atan2(svin[4],svin[3]);

  /* Track Fitting iterations */

  iterm = sdakeys_.ztfit[0];
  for(iter=1; iter<=iterm; iter++)
  {
    maxTM = sdakeys_.ztfit[2];
    if(Level == 4 && iter > maxTM)
    {
      if_mat = 0;
    }
    else
    {
      if_mat = 1;
    }

    /* Trajectory propagation */

    dcswim(&sdageom_.pln[sec][0][0],ipmin,ipmax,if_mat,inter,svin,
                         trp,tmat,&sdageom_.nstep_xyz,sdageom_.xyz);

    /* Calculation of derivatives (dtrsp) and dcat */

    lambda0 = asin(svin[5]);
    cosL0   = cos(lambda0);
    fii0    = atan2(svin[4],svin[3]);
    ilay    = 0;
    chisq   = 0.0;

    /* Vertex constraint due to transverse beam size (electron beam) */

    if(sdakeys_.ztfit[1] == 1. || sdakeys_.ztfit[1] == 3.)
    {
      ilay++;
      cross_(ex,&trp[0][3],u);
      itmp = 3;
      vunit_(u,u,&itmp);

      MACROS(0);

      /* Calculation of vector VX(ilay) = Dmeas - Dtrial and  chi2 */
      itmp = 3;
      dcatr = vdot_(&trp[0][0],u,&itmp);
      vx[ilay-1] = sdakeys_.zbeam[3] - dcatr;
      /* Chi-square accumulated with weight correction */
      sigm = sdakeys_.zbeam[6];
      if(sigm < 0.005) sigm = 0.18;
      weight[ilay-1] = 1.0 / (sigm*sigm);
      chisq += vx[ilay-1]*vx[ilay-1]*weight[ilay-1];
    }

    /* Vertex constraint due to target length along the beam (very thin targets) */
goto a10;
    if(sdakeys_.ztfit[1] == 2. || sdakeys_.ztfit[1] == 3.)
    {
      ilay++;
      cross_(ez,&trp[0][3],u);
      itmp = 3;
      vunit_(u,u,&itmp);

      MACROS(0);

      /* Calculation of vector VX(ilay) = Dmeas - Dtrial and chi2 */
      itmp = 3;
      dcatr = vdot_(&trp[0][0],u,&itmp);
      vx[ilay-1] = -sdakeys_.ztarg[2] * sin(sphi) - dcatr;
      /* Chi-square accumulated with weight correction */
      sigm = sdakeys_.ztarg[3];           /* This is half target length */
      if(sigm < 0.005) sigm = 0.50;
      sigm = sigm * sin(sphi) / 1.732;    /* Flat distr. along target */
      weight[ilay-1] = 1.0 / (sigm*sigm);
      chisq += vx[ilay-1] * vx[ilay-1] * weight[ilay-1];
    }
a10:

    /* Now loop over all layers of DC */

    for(il=ilmin; il<=ilmax; il++)
    {
      if(sdakeys_.lout[il-1] > 0) continue; /* Discard this layer */
      if(trk[il-1].status > 0.0) continue;  /* Discard this hit */
      is = (il-1) / 6 + 1;
      iw = trk[il-1].iw;
      ilay++;
      cross_(&sdageom_.dc_wdir[sec][il-1][iw-1][0],&trp[il+(ndc_min-1)-1][3],u);
      itmp = 3;
      vunit_(u,u,&itmp);

      MACROS(il+(ndc_min-1)-1);
      /* Calculation of vector VX(ilay) = Dmeas - Dtrial and chi2 */
      diff[0] = trp[il+(ndc_min-1)-1][0] - sdageom_.dc_wpmid[sec][il-1][iw-1][0];
      diff[1] = trp[il+(ndc_min-1)-1][1] - sdageom_.dc_wpmid[sec][il-1][iw-1][1];
      diff[2] = trp[il+(ndc_min-1)-1][2] - sdageom_.dc_wpmid[sec][il-1][iw-1][2];
      itmp = 3;
      trk[il-1].Df = vdot_(diff,u,&itmp);       /* Dcat */
      vx[ilay-1] = trk[il-1].Dm - trk[il-1].Df; /* Dcam-Dcat */
      /* Chi-square accumulated with weight correction */
      sigm = trk[il-1].SigDm;
      weight[ilay-1] = 1.0/(sigm*sigm);
      chisq += vx[ilay-1]*vx[ilay-1]*weight[ilay-1];
    }

    /* Normalize chi-squre by the no. of degree of freedom */
    if(ilay < 6) continue;
    free   = (float)ilay - 5.0;
    if(chisq > 0.001) *chifit = chisq / free;
    iterl = iter;

    /* Calculation of matrix alfa[5][5] */
    for(j=0; j<5; j++)
    {
      for(k=0; k<5; k++)
      {
        alfa[k][j] = 0.0;
        for(il=0; il<ilay; il++)
        {
          alfa[k][j] += dtrsp[il][j] * dtrsp[il][k];
        }
      }
    }

    /* Calculation of vector beta[5] */
    dcmvprod(&dtrsp[0][0],vx,beta,5,ilay);

    /* Invertion of matrix alfa[5][5] */
    dcmxinv(alfa,5,&det);
    if(det == 0.0) continue;

    /* Calculation of corrections dx[5] */
    dcmvprod(&alfa[0][0],beta,dx,5,5);
	/*printf("dx=%f %f %f %f %f\n",dx[0],dx[1],dx[2],dx[3],dx[4]);*/

    /* Get new {p,Theta,Phi,d0,z0} and svin from previous svin
       and corrections dx */
    if(sdakeys_.mgtyp == 5) dx[0] = 0.0;
    pabs = 1. / ( 1./svin[6] + dx[0] );
    lambda = lambda0 + dx[1];
    tht = PIBY2 - lambda;
    fii = fii0 + dx[2];
    /* New DCA & Z0 at origin */
    d0 += dx[3];
    z0 += dx[4];
    /* New starting values for the next tracing */
    svin[3] = sin(tht)*cos(fii);
    svin[4] = sin(tht)*sin(fii);
    svin[5] = cos(tht);
    if(ipmin >= 4)
    {
      cross_(ez,&svin[3],a0);
      itmp = 3;
      vunit_(a0,a0,&itmp);
      svin[0] = vt0[0] + d0*a0[0] - 3.*svin[3];
      svin[1] = vt0[1] + d0*a0[1] - 3.*svin[4];
      svin[2] = z0;
    }
    else
    {
      svin[0] = -d0 / sin(fii);
      svin[1] =  svin[1];
      svin[2] =  z0;
    }
    svin[6] = pabs;

    if(ABS(chi2old-(*chifit)) <= chi_dif) break; /* early exit from for(iter)*/
    chi2old = *chifit;

  } /* end of iter - loop over iterations */


  /* Print the result of last iteration */
  if((sdakeys_.isw[0] == 1) && (sdakeys_.ievt <= sdakeys_.Ndbg))
  {
    iterl = iter - 1;
    thetd = tht * RADDEG;
    phid  = fii * RADDEG;
	/* ilnk not defined here any more !!!
    printf("\n LEVEL = %2d  isec/itrk/iter = %3d%3d%3d  Chi2 = %12.6e\n",
           Level,sec+1,ilnk,iter,*chifit);
	*/
    printf("\n LEVEL = %2d  isec/iter = %3d%3d  Chi2 = %12.6e\n",
           Level,sec+1,iterl,*chifit);
    printf(" Cor: 1/P,L0,PH=%8.5f%10.5f%10.5f   D0,Z0=%8.4f        %8.4f\n"
           " New:  P ,TH,PH=%8.5f%10.5f%10.5f   D0,Z0=%8.4f        %8.4f\n"
    "                                              Vertex=%8.4f%8.4f%8.4f\n",
           dx[0],dx[1],dx[2],dx[3],dx[4],pabs,thetd,phid,d0,z0,
           svin[0],svin[1],svin[2]);
  }

  /* Check if make sense to perform the global swimming */
  if(*chifit > 100.) return;

  /* Calculation of final trajectory and chi-square */

  if_draw = inter;
  if(sdakeys_.lanal[0] == 4 && Level == 2) if_draw = 0;

  dcswim(&sdageom_.pln[sec][0][0],ipmin,npln,0,if_draw,svin,
                    trp,tmat,&sdageom_.nstep_xyz,sdageom_.xyz);

  ilay   = 0;
  chisq  = 0.0;
  *chifit = 1000.0;
  for(il=ilmin; il<=ilmax; il++)
  {
    if(sdakeys_.lout[il-1] == 1) continue; /* Discard this layer */
    if(trk[il-1].status > 0.0) continue;   /* Discard this hit */
    is = (il-1)/6 + 1;
    iw = trk[il-1].iw;

    cross_(&sdageom_.dc_wdir[sec][il-1][iw-1][0],&trp[il+(ndc_min-1)-1][3],u);
    itmp = 3;
    vunit_(u,u,&itmp);
    /* Calculation of vector vx(ilay) = Dcam - Dcat and chi2 */
    diff[0] = trp[il+(ndc_min-1)-1][0] - sdageom_.dc_wpmid[sec][il-1][iw-1][0];
    diff[1] = trp[il+(ndc_min-1)-1][1] - sdageom_.dc_wpmid[sec][il-1][iw-1][1];
    diff[2] = trp[il+(ndc_min-1)-1][2] - sdageom_.dc_wpmid[sec][il-1][iw-1][2];
    itmp=3;
    trk[il-1].Df = vdot_(diff,u,&itmp);       /* Dcat */
    /* Chi-square accumulated with weight correction */
    if(sdakeys_.lout[il-1] == 2) continue;
    ilay++;
    vx[ilay-1] = trk[il-1].Dm - trk[il-1].Df; /* Dcam - Dcat */
    sigm = trk[il-1].SigDm;
    weight[ilay-1] = 1./(sigm*sigm);
    chisq += vx[ilay-1]*vx[ilay-1]*weight[ilay-1];
  }

  if(ilay > 5)
  {
    free   = (float)ilay - 5.0;
    if(chisq > 0.001) *chifit = chisq / free;
  }
  if(sdakeys_.isw[0] == 1 && sdakeys_.ievt <= sdakeys_.Ndbg)
  {
    printf(" Iter=%2d   Chi2 =%12.6e\n",iterl,*chifit);
  }

  return;
}

