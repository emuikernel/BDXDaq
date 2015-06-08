/*
   Purpose and Methods : Finds energy cluster the track "itr" points to


   Input   :  itr     - track candidate number

   Outputs :  iflg = 0  means OK
              

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   30-SEP-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by ana_finde
*/

#include <stdio.h>
#include "dclib.h"
#include "sdakeys.h"
#include "sdadigi.h"
#include "sdageom.h"
#include "sdacalib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

void
dcecid_(int *isec, DCecid *ecid, float *p, int *iflg)
{
  dcecid(*isec, ecid, *p, iflg);
  return;
}

void
dcecid(int isec, DCecid *ecid, float p, int *iflg)
{
  static int i, j, ih, ic, nc, nh, il, sec, isl, Vtrk,
             du, dv, dw, dif, Ucl, Vcl, Wcl, digi[6][36], Evis[3][36], Etot,
             nsmin, nsmax, fslab, nslab, p1s, p2s, sumE, Epeak, ncl[3],
             Eclust[3][18][5], UVWdif[3][18];
  static int ip, itmp, NoShSim;
  static float Et, Erat;
  static int ifirst = 1;

  static int Acl_thr = 10;      /* visible energy threshold [MeV] */
  static int slab_dif = 3;      /* Difference between SLABclust and SLABtrk */
  static float Evis_cut = 0.10; /* Cut on Evis/Ptrk */


  if(ifirst == 1)
  {
    ifirst = 0;
    ip     = nec_min;
    NoShSim = 0;
    itmp = 10;
    if(iucomp_("SIM ",&sdakeys_.levb,&itmp,4) != 0 &&
       sdakeys_.zcut[4] == 0.) NoShSim = 1;
  }

  /* Clear arrays */

  *iflg = 2;
  for(il=0; il<6; il++)
    for(isl=0; isl<36; isl++) digi[il][isl] = 0;
  sec = isec - 1;

  /* Check boundaries of EC triangle at distance of "d+dd" from origin */
  /* TAKE CARE IN CALLING FUNCTION (ana_finde)

IF USE IT, CHANGE IN FOLLOWING (ecid->u) -> su ETC - SEE FORTRAN VERSION

  IF(lanal(1).LT.2) THEN
    isec   = ev__out(11,itr)/10.
    sw = ev__out(36,itr)/10000.
    sv =(ev__out(36,itr) - sw*10000.)/100.
    su = ev__out(36,itr) - sw*10000. - sv*100.
  ELSE
    isec = itr_sect(itr)
    su = itr_ecid(1,itr)
    sv = itr_ecid(2,itr)
    sw = itr_ecid(3,itr)
  ENDIF
  */
  if(ecid->u * ecid->v * ecid->w ==  0) return;

  /*
C The branch for Acceptance calculation without Shower simulation
      IF(NoShSim.EQ.1) THEN
        IF(su.LT.2 .OR. su.GT.35) GOTO 999
        IF(sv.LT.2 .OR. sv.GT.35) GOTO 999
        IF(sw.LT.2 .OR. sw.GT.35) GOTO 999
        iflg = 1
        p = trkp(7,ip,itr)
        IF(lanal(1).LT.2) p = ev__out(7,itr)
        Et = 1000.*p*0.2/3.
C Store dummy info so that analysis will not corrupt
        DO il = 1,3
          e_clust(1,il) = suvw(il)
          e_clust(2,il) = 1
          e_clust(3,il) = suvw(il)
          e_clust(4,il) = 0
          e_clust(5,il) = Et
          e_clust(6,il) = 0
        ENDDO
      ENDIF
*/

  *iflg = 1;

  /* Unpack the "ec_digi" array */




  for(il=0; il<6; il++)
  {
    if(nh = sdadigi_.ec_ndig[sec][il])
    {
      for(ih=0; ih<nh; ih++)
      {
        isl = sdadigi_.ec_digi[sec][il][ih][0];
        digi[il][isl-1] = ( sdadigi_.ec_digi[sec][il][ih][2]
                            - sdacalib_.cal_ec[sec][il][isl-1][5] )
                            / sdacalib_.cal_ec[sec][il][isl-1][2];
        if(digi[il][isl-1] < 0) digi[il][isl-1] = 0;
      }
    }
  }




  /* Sum energy (forward + backward) */

  Etot = 0;
  for(il=1; il<=3; il++)
  {
    for(isl=1; isl<=36; isl++)
    {
      Evis[il-1][isl-1] = digi[il-1][isl-1] + digi[il+3-1][isl-1];
      Etot += Evis[il-1][isl-1];
    }
  }

/*
C Make a cut on the total visible energy in that sector
      IF(mgtyp.EQ.5 .OR. ABS(zmagn(2)).LT.0.01) THEN
        p = 1.0
      ELSE
        p = trkp(7,ip,itr)
        IF(lanal(1).LT.2) p = ev__out(7,itr)
      ENDIF
*/

  /* Find energy clusters in three views {u,v,w} */

  for(il=0; il<3; il++)
  {
    nc    = 0;
    fslab = 0;
    nslab = 0;
    p1s  = 0;
    p2s  = 0;
    sumE  = 0;
    for(isl=1; isl<=36; isl++)
    {
      if(Evis[il][isl-1] > 0)
      {
        if(fslab == 0)
        {
          fslab = isl;
          p1s  = isl;
          Epeak = Evis[il][isl-1];
        }
        nslab++;
        if(Evis[il][isl-1] > Epeak)
        {
          p1s  = isl;
          Epeak = Evis[il][isl-1];
        }
        sumE += Evis[il][isl-1];
        if(isl == 36 || Evis[il][isl+1-1] == 0)
        {
          nc++;
          Eclust[il][nc-1][0] = fslab;
          Eclust[il][nc-1][1] = nslab;
          Eclust[il][nc-1][2] = p1s;
          Eclust[il][nc-1][3] = 0;
          Eclust[il][nc-1][4] = sumE;
          fslab = 0;
          nslab = 0;
          p1s  = 0;
          sumE  = 0;
        }
      }
    }
    ncl[il] = nc;

    /* Now search for 2-nd peak (p2s) */

    for(i=1; i<=ncl[il]; i++)
    {
      p1s  = Eclust[il][i-1][2];
      nsmin = Eclust[il][i-1][0];
      nsmax = nsmin + Eclust[il][i-1][1] - 1;
      Epeak = 0;
      for(isl=nsmin; isl<=nsmax; isl++)
      {
        if(ABS(isl-p1s) < 2) continue;
        if(Evis[il][isl-1] > Epeak)
        {
          Epeak = Evis[il][isl-1];
          p2s  = isl;
        }
      }
      if(p2s < 2 || p2s > 35)
      {
        p2s = 0;
        continue;
      }
      if(Evis[il][p2s-1-1] > Epeak || Evis[il][p2s+1-1] > Epeak)
      {
        p2s = 0;
        continue;
      }
      Eclust[il][i-1][3] = p2s;
      p2s = 0;
    }

  } /* End DO over layers */

  if(ncl[0]*ncl[1]*ncl[2] == 0) goto a999;

  /* Track matching with clusters */

  /* U-view */

  du = 99;
  Ucl= 0;
  for(ic=1; ic<=ncl[0]; ic++)
  {
    UVWdif[0][ic-1] = 99;
    if(Eclust[0][ic-1][4] < Acl_thr) continue;
    for(j=3; j<=4; j++)
    {
      if(Eclust[0][ic-1][j-1] > 1 && Eclust[0][ic-1][j-1] < 36)
      {
        dif = Eclust[0][ic-1][j-1] - ecid->u;
        UVWdif[0][ic-1] = dif;
        if(ABS(dif) < ABS(du))
        {
          du = dif;
          Ucl= ic;
        }
      }
    }
  }
  if(Ucl == 0) goto a999;
  UVWdif[0][Ucl-1] = du;

  /* V-view */

  dv = 99;
  Vcl= 0;
  for(ic=1; ic<=ncl[1]; ic++)
  {
    UVWdif[1][ic-1] = 99;
    if(Eclust[1][ic-1][4] < Acl_thr) continue;
    for(j=3; j<=4; j++)
    {
      if(Eclust[1][ic-1][j-1] > 1 && Eclust[1][ic-1][j-1] < 36)
      {
        dif = Eclust[1][ic-1][j-1] - ecid->v;
        UVWdif[1][ic-1] = dif;
        if(ABS(dif) < ABS(dv))
        {
          dv = dif;
          Vcl= ic;
        }
      }
    }
  }
  if(Vcl == 0) goto a999;
  UVWdif[1][Vcl-1] = dv;

  /* W-view */

  dw = 99;
  Wcl= 0;
  for(ic=1; ic<=ncl[2]; ic++)
  {
    UVWdif[2][ic-1] = 99;
    if(Eclust[2][ic-1][4] < Acl_thr) continue;
    for(j=3; j<=4; j++)
    {
      if(Eclust[2][ic-1][j-1] > 1 && Eclust[2][ic-1][j-1] < 36)
      {
        dif = Eclust[2][ic-1][j-1] - ecid->w;
        UVWdif[2][ic-1] = dif;
        if(ABS(dif) < ABS(dw))
        {
          dw = dif;
          Wcl= ic;
        }
      }
    }
  }
  if(Wcl == 0) goto a999;
  UVWdif[2][Wcl-1] = dw;

  /* Check track-cluster matching */

  if(ABS(du) > slab_dif) goto a999;
  if(ABS(dv) > slab_dif) goto a999;
  if(ABS(dw) > slab_dif) goto a999;

  /* Check energy ratio */
  /* Make a cut on the total visible energy in that sector */

  Et = Eclust[0][Ucl-1][4] + Eclust[1][Vcl-1][4] + Eclust[2][Wcl-1][4];
  Erat = Et / (1000.*p);
  if(Erat < Evis_cut) goto a999;
  *iflg = 0;

  /* Store the results - NOBODY NEED IT YET !!! <-- still true ???
  {
    int e_clust[3][6];
    for(il=0; il<3; il++)
    {
      if(il == 0)
      {
        ic = Ucl;
        e_clust[il][5] = du;
      }
      if(il == 1)
      {
        ic = Vcl;
        e_clust[il][5] = dv;
      }
      if(il == 2)
      {
        ic = Wcl;
        e_clust[il][5] = dw;
      }
      for(i=0; i<5; i++) e_clust[il][i] = Eclust[il][ic-1][i];
    }
  }
  */

a999:

  /* Debugging Printout */

  if(sdakeys_.lprnt[3] == 2)
  {
    /*printf("\n  isec =%2d  itr =%3d  Etot =%5d  (e ID)\n",sec+1,itr,Etot);*/
    printf("\n  isec =%2d  Etot =%5d  (e ID)\n",sec+1,Etot);
    printf("\n  isl       U       V       W\n");
    for(isl=0; isl<36; isl++)
      printf("%5d%8d%8d%8d\n",isl+1,Evis[0][isl],Evis[1][isl],Evis[2][isl]);
    for(il=0; il<3; il++)
    {
      if(il == 0) Vtrk = ecid->u;
      else if(il == 1) Vtrk = ecid->v;
      else if(il == 2) Vtrk = ecid->w;

      if(il == 0) printf("\n U-view     Uf   Nu  Up1  Up2  EcU   Utrk   dU\n");
      if(il == 1) printf("\n V-view     Vf   Nv  Vp1  Vp2  EcV   Vtrk   dV\n");
      if(il == 2) printf("\n W-view     Wf   Nw  Wp1  Wp2  EcW   Wtrk   dW\n");
      for(ic=0; ic<ncl[il]; ic++)
      {
        printf("%5d     %5d%5d%5d%5d%5d%7d%5d\n",ic+1,Eclust[il][ic][0],
            Eclust[il][ic][1],Eclust[il][ic][2],Eclust[il][ic][3],
            Eclust[il][ic][4],Vtrk,UVWdif[il][ic]);
      }
    }
  }

  return;
}
