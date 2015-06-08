h36820
s 00000/00000/00000
d R 1.2 01/11/19 19:06:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcswim.c
e
s 00317/00000/00000
d D 1.1 01/11/19 19:06:00 boiarino 1 0
c date and time created 01/11/19 19:06:00 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : Track swimming (traj. propagation) in magnetic
                         field. Calculates track points on detector planes.

   Inputs  : pln(20,npln) - plane array
             ipl_min - first plane to crossed during swimming
             ipl_max - last plane to crossed during swimming
             if_mat  - when set to 1, Transport Matrix is calculated
                       during swimming
             if_draw - when set to 1, x,y -coordinates at each step
                       are stored during swimming (convenient for drawing
                       routines
             vins(9) - Starting values {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
             /sdakeys/ztarg(3) - parameters of TARG card
             /sdakeys/zstep(1),zstep(2),zstep(3) - parameters of STEP card

   Outputs : trp         - swimming results
             tmat        - transport matrix at track point on a plane
             nxyz        - the number of steps for current track
             xyz(3,nxyz) - {x,y,z} for a given step
             /sdakeys/step - step size ---------> LOCAL NOW !!!!!!!!!!!!!!!

   Controls: /sdakeys/isim - simulation(=1)/reconstruction(!=1) switch

   Library belongs    : libsda.a

   Calls   : dcrkuta, dcicyl, dcipln, dctrmat

   Created   OCT-26-1990  Bogdan Niczyporuk
   Modified  AUG-2-1992   Billy Leung

   Called by sim_main & ana_trfit
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ECon   2.99792e-4
#define del_s  60.0
#define tan30  0.57735027
#define pi     3.14159265

float vmod_(float *,int *);

void
dcswim_(float pln[npln][20], int *ipl_min, int *ipl_max, int *if_mat,
             int *if_draw, float vins[9], float trp[npln][7],
             float tmat[npln][5][5], int *nxyz, float xyz[mxstep][3])
{
  dcswim(pln,*ipl_min,*ipl_max,*if_mat,*if_draw,vins,trp,tmat,nxyz,xyz);
}

void
dcswim(float pln[npln][20], int ipl_min, int ipl_max, int if_mat,
             int if_draw, float vins[9], float trp[npln][7],
             float tmat[npln][5][5], int *nxyz, float xyz[mxstep][3])
{
  int i,j,ist,ip,ipl,ipp,iflag,iflagp,new,nstep,ncyl_min,icross;
  int itmp,itmp1,itmp2,newsec;
  float Binv, Bold, Dangle, sint, stot, s_first, x0[2], sdakeys_step;
  float vect[9], vout[9], xint[9], bfld[3], phi, phimin;
  float bb[25];

  /* swim begins */

  x0[0] = pln[1][11] - vins[0];
  x0[1] = pln[1][12] - vins[1];
  phimin = atan2(x0[1],x0[0]);

  if(ipl_min <= 0) return;

  Dangle = sdakeys_.zstep[2] / ECon;

  /* Buffer "vect" is used in DO loop */

  for(i=0; i<9; i++) vect[i] = vins[i];

  /* Initialise hit array */

  for(i=0; i<ipl_max; i++)
  {
    for(j=0; j<3; j++)
    {
      trp[i][j] = 1000.0;
      trp[i][j+3] = 0.0;
    }
    trp[i][6] = 0.0;
  }

  /* Initialise indices */

  nstep  = 1000. / sdakeys_.zstep[0];
  new    = 1;
  ipl    = ipl_min;
  stot   = 0.;
  icross = 0;
  newsec = 0;

  /* Get B-field in order to derive the first step size
     when PTF is present (SIM) */
  sdakeys_step = 0.01;
  dchelix_(&sdakeys_step, vect, vout, bfld);

  /* Do loop traces particle through detector */

  for(ist=0; ist<nstep; ist++)
  {
    /* Calculate a step size */

    if(vins[8] != 0.)
    {
      itmp = 3;
      Bold = vmod_(bfld, &itmp);
      if(sdakeys_.zstep[0] < sdakeys_.zstep[1])
      {
        if(Bold < 0.005)
        {
          Binv = 200.0;
        }
        else
        {
          Binv = 1.0 / Bold;
        }
        sdakeys_step = Dangle * vect[6] * Binv;
        if(sdakeys_step < sdakeys_.zstep[0]) sdakeys_step=sdakeys_.zstep[0];
        if(sdakeys_step > sdakeys_.zstep[1]) sdakeys_step=sdakeys_.zstep[1];
      }
      else
      {
        sdakeys_step = sdakeys_.zstep[0];
      }
    }
    else
    {
      sdakeys_step = 10.0;
    }

    /* first step size have to smaller then a distance to the 1-st plane */
    if(ist == 0) sdakeys_step = 0.02;

    /* call Helix or  Runge-Kutta */

    dchelix_(&sdakeys_step, vect, vout, bfld);
    /*dcrkuta_(&sdakeys_step, vect, vout, bfld);*/
    ip = ipl;

/*
C
C Check whether sector boundaries were crossed if SIM & PT Field present
        IF(isim.EQ.0)      then
*          print *,'sda_swim: sector boundaries were crossed'
          GO TO 9
        endif
        IF(zmagn(4).EQ.0.) then
*      print *,'222: zmagn(4)=',zmagn(4)
          GO TO 9
        endif
        IF(newsec.EQ.1)    then
*          print *,'sda_swim: new sector'
          GO TO 9
        endif
      print *,'sda_swim: stop 1'
      stop
        tanpos = vout(3)/ABS(vout(2))
        tanphi = vout(6)/ABS(vout(5))
        IF(tanpos.GT.tan30 .AND. tanphi.GT.tan30) THEN
          asec = pi/3.
          isec = isec + 1
          IF(isec.GT.6) isec = 1
        ELSE IF(tanpos.LT.-tan30 .AND. tanphi.LT.-tan30) THEN
          asec = -pi/3.
          isec = isec - 1
          IF(isec.LT.1) isec = 6
        ELSE
          asec = 0.
        ENDIF
        IF(asec.NE.0.) THEN
          svin(1) = vect(1)
          svin(2) = vect(2)*cos(asec) + vect(3)*sin(asec)
          svin(3) =-vect(2)*sin(asec) + vect(3)*cos(asec)
          svin(4) = vect(4)
          svin(5) = vect(5)*cos(asec) + vect(6)*sin(asec)
          svin(6) =-vect(5)*sin(asec) + vect(6)*cos(asec)
          sout(1) = vout(1)
          sout(2) = vout(2)*cos(asec) + vout(3)*sin(asec)
          sout(3) =-vout(2)*sin(asec) + vout(3)*cos(asec)
          sout(4) = vout(4)
          sout(5) = vout(5)*cos(asec) + vout(6)*sin(asec)
          sout(6) =-vout(5)*sin(asec) + vout(6)*cos(asec)
          DO i = 1,6
            vect(i) = svin(i)
            vout(i) = sout(i)
          ENDDO
          newsec = 1
        ENDIF
  9     CONTINUE
*/

    /* Store points for trajectory drawing */
    if(if_draw == 1 && ist < mxstep)
    {
      xyz[ist][0] = vect[0];
      xyz[ist][1] = vect[1];
      xyz[ist][2] = vect[2];
      *nxyz  = ist + 1;
    }

    /* It is assumed here that a track never cross again the same layer,
      but may cross a few layers within the same (constant) step length. */

    /* Find intersection with cylinder or plane */
    ncyl_min = ndc_min + 12;

a10:

    sint = sdakeys_step;
    if(ipl < ndc_min)                              /* planes near target */
    {
      if(ipl == 2)
      {
        phi = atan2(vout[4],vout[3]);
        if(phi < phimin) ipl = 3;
      }
      dcipln_(pln,vect,vout,&ipl, &sint,xint,&iflag);
    }
    else if(ipl >= ndc_min && ipl < ncyl_min)      /* Reg.1 */
    {
      if(vect[0] < 0. && vout[0] < 0.)
      {
        dcipln_(pln,vect,vout,&ipl, &sint,xint,&iflag);
      }
      else if((vect[0]<0. && vout[0]>0.) || (vect[0]>0. && vout[0]<0.))
	  {
        dcipln_(pln,vect,vout,&ipl, &sint,xint,&iflag);
        if(iflag != 1) dcicyl_(pln,vect,vout,&ipl, &sint,xint,&iflag);
	  }
      else
      {
        dcicyl_(pln,vect,vout,&ipl, &sint,xint,&iflag);
      }
    }
    else if(ipl >= ncyl_min && ipl <= ncc_max)     /* Reg.2,3 & CC */
    {
      dcicyl_(pln,vect,vout,&ipl, &sint,xint,&iflag);
    }
    else if(ipl_max >= nsc_min)                    /* SC & EC */
    {
      iflag = 0;
      for(ipp = nsc_min; ipp<=ipl_max; ipp++)
      {
        dcipln_(pln,vect,vout,&ipp, &sint,xint,&iflagp);
        if(iflagp == 1)
        {
          for(i=0; i<6; i++) trp[ipp-1][i] = xint[i];
          trp[ipp-1][6] = stot + sint;
          icross ++;
          if(icross == 1) s_first = trp[ipp-1][6];
        }
      }
      if(icross == 0)
      {
        goto a20;
      }
      else
      {
        if((stot-s_first) > del_s) return;
      }
    }
    else
    {
      ;
    }

    if(iflag == 1)
    {
      /* track crossed a cylinder or plane */
      if(if_mat > 0 && ipl <= ndc_max)
      {
        dctrmat_(&new,vect,xint,&sint,bfld,bb,&tmat[ipl-1][0][0],&iflag);
      }
      for(i=0; i<6; i++) trp[ipl-1][i] = xint[i];
      trp[ipl-1][6] = stot + sint;
      if(ipl == 2)
      {
        ipl = ipl + 2;
      }
      else
      {
        ipl = ipl + 1;
      }
      /* Check if last layer has been reached */
      if(ipl > ipl_max) return;
      /* Check next layer crossing within the same step */
      goto a10;
    }

a20:

    if(if_mat > 0 && ipl <= ndc_max)
    {
      itmp = 0;
      dctrmat_(&new,vect,vout,&sdakeys_step,bfld,bb,tmat,&itmp);
    }

    stot += sdakeys_step;
    /*if(vout[4] <= 0. || stot > 700.) return;*/
    if(stot > 700.) return;
    for(j=0; j<9; j++) vect[j] = vout[j];

  }       /* end of loop over nstep */

  return;
}

E 1
