h34612
s 00013/00008/00627
d D 1.2 03/04/17 16:54:02 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:58 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcfield.c
e
s 00635/00000/00000
d D 1.1 01/11/19 19:05:57 boiarino 1 0
c date and time created 01/11/19 19:05:57 by boiarino
e
u
U
f e 0
t
T
I 1
/*

   Purpose and Methods: Performs 3-dim 2-nd order interpolation for each 
                        component of the magnetic field using Taylor series. 

   Inputs  : XX(3)- space point and Magnetic Field Table (see structure BGRID)
   Outputs : F(3) - magnetic field at point XX(3)
   Controls:

   Library belongs: libsda.a

   Calls: none

   Author:     P.Vernin     (Aug. 1987)
   Modified:   M.Guckes     (Oct. 1987)
               B.Niczyporuk (Feb. 1991)
   C remake by Sergey Boyarinov

    Called by dcrkuta

*/


#define ABS(x) ((x) < 0 ? -(x) : (x))

#include <stdio.h>
#include <math.h>
I 3
#include <time.h>
#include <string.h>
#include <stdlib.h>
E 3
#include "dclib.h"
I 3
#include "uttime.h"
E 3

static SDAbgrid sdabgrid_;
static SDAbgrid *grd;
static int nx3, nynx3, NX, NY, NZ;
static float rdx, rdy, rdz, Xmin, Ymin, Zmin, *QQ;
static float tailor[Ng][3][11];

static POLTG poltg_;
static POLTG *pol;
static int mx3, MX, MZ;
static float Xpmin, Zpmin, Xpmax, Zpmax, rddxy, rddz, *QP;
static float tailorp[Ng][2][6];


void dcfieldT();
void dcfieldP();

void
dcfieldt_(float x[3], float f[3])
{
  dcfieldT(x,f);
}

void
dcfieldT(float x[3], float f[3])
{
  int i, j, ixm, iym, izm, itmp;
  float axm, aym, azm, pp, qq, rr, xr, yr, zr, bxfac;

  /* reordering coordinates */
  /* POKA SNARUZHI
  x[0] = xx[1];
  x[1] = xx[2];
  x[2] = xx[0];
  */

  /* Each sector is symmetric in Y-coordinate (DST System) */

  if(x[1] < 0.)
  {
    x[1] = -x[1];
    bxfac = -1.;
  }
  else
  {
    bxfac =  1.;
  }

  /* find intercepts */

  xr  = (x[0]-Xmin) * rdx;
  axm = (float)((int)xr);
  ixm = (int)axm;
  if(ixm < 1 || ixm+2 > NX) {f[0]=f[1]=f[2]=0.; x[1] = bxfac*x[1]; return;}
  pp  = xr - axm;

  yr  = (x[1]-Ymin) * rdy;
  aym = (float)((int)yr);
  iym = (int)aym;
  if(iym < 1 || iym+2 > NY) {f[0]=f[1]=f[2]=0.; x[1] = bxfac*x[1]; return;}
  qq  = yr - aym;

  zr  = (x[2]-Zmin) * rdz;
  azm = (float)((int)zr);
  izm = (int)azm;
  if(izm < 1 || izm+2 > NZ) {f[0]=f[1]=f[2]=0.; x[1] = bxfac*x[1]; return;}
  rr  = zr - azm;


  /* 3-dim 2nd order interpolation inside TORUS for each component
     of the field. We use 11 values of the field */


/*start_timer_();
for(j=0; j<10000; j++)
{
*/


  /* Compute the Taylor's series */


  {
    float *Q;
    Q = (float *) &tailor[izm*nynx3 + iym*nx3 + ixm*3][0][0];
    for(i=0; i<3; i++)
    {
      f[i] = Q[0] + 0.5*( pp*( Q[1] + pp*Q[2] )
                        + qq*( Q[3] + qq*Q[4] )
                        + rr*( Q[5] + rr*Q[6] ) )
           + qq*( pp*Q[7] + rr*Q[8] )
           + pp*rr*( Q[9] + qq*Q[10] );
      Q+=11;

/* DO NOT DO THAT !!! order of calculation could be any and depends on
even Q declaration: works if Q is 'static' and does not work if not
      f[i] = (*Q++) + 0.5*( pp*( (*Q++) + pp*(*Q++) )
                          + qq*( (*Q++) + qq*(*Q++) )
                          + rr*( (*Q++) + rr*(*Q++) ) )
             + qq*( pp*(*Q++) + rr*(*Q++) )
             + pp*rr*( (*Q++) + qq*(*Q++) );
*/

    }
  }


  /* old way
  Q = QQ + izm*nynx3 + iym*nx3 + ixm*3;
  for(i=0; i<3; i++)
  {
    f000 = *Q;
    f00p = *(Q+nynx3);
    f00m = *(Q-nynx3);
    f0p0 = *(Q+nx3);
    f0m0 = *(Q-nx3);
    fp00 = *(Q+3);
    fm00 = *(Q-3);
    f0pp = *(Q+nynx3+nx3);
    fp0p = *(Q+nynx3+3);
    fpp0 = *(Q+nx3+3);
    fppp = *(Q+nynx3+nx3+3);

    cp = fp00+fm00-2.*f000;
    cq = f0p0+f0m0-2.*f000;
    cr = f00p+f00m-2.*f000;
    dp = f000-fp00+fpp0-f0p0;
    dq = f000-f0p0+f0pp-f00p;
    dr = f000-f00p+fp0p-fp00;
    e  = -dp-f0pp+f00p-fp0p+fppp;
    f[i] = f000 + 0.5*( pp*(fp00-fm00) + pp*pp*cp
                      + qq*(f0p0-f0m0) + qq*qq*cq
                      + rr*(f00p-f00m)+rr*rr*cr )
           + pp*qq*dp + qq*rr*dq + pp*rr*dr + pp*qq*rr*e;
    Q++;
  }
  */


/*
}
itmp=1;
stop_timer_(&itmp);*/

  f[0] = bxfac * f[0];
  f[2] = bxfac * f[2];

  /* Restore the original Y-coordinate */
  x[1] = bxfac*x[1];

  /* reordering field components */

  /* POKA SNARUZHI
  temp = f[0];
  f[0] = f[2];
  f[2] = f[1];
  f[1] = temp;
  */

  return;
}

void
dcfieldP(float x[3], float f[3])
{
  int i, ixm, izm;
  float r, bfac, bx;
  float axm, azm, pp, rr, xr, zr;
  float *Q;

  /* reordering coordinates */
  /* POKA SNARUZHI
  x[0] = xx[1];
  x[1] = xx[2];
  x[2] = xx[0];
  */

  /* Each sector is symmetric in Y-coordinate (DST System) */

  /* The Polarized Target Field TABLE was created for Z > 0 */

  if(x[2] < 0.)
  {
    x[2] = -x[2];
    bfac = -1.;
  }
  else
  {
    bfac =  1.;
  }

  /* find intercepts */

  r = sqrt(x[0]*x[0] + x[1]*x[1]);
  if(r<=Xpmin || r>=Xpmax)       {f[0]=f[1]=f[2]=0.; x[2] = bfac*x[2]; return;}
  if(x[2]<=Zpmin || x[2]>=Zpmax) {f[0]=f[1]=f[2]=0.; x[2] = bfac*x[2]; return;}

  xr  = (r - Xpmin) * rddxy;
  axm = (float)((int)xr);
  ixm = (int)axm;
  if(ixm < 1 || ixm > MX-2) {f[0]=f[1]=f[2]=0.; x[2] = bfac*x[2]; return;}
  pp  = xr - axm;

  zr  = (x[2] - Zpmin) * rddz;
  azm = (float)((int)zr);
  izm = (int)azm;
  if(izm < 1 || izm > MZ-2) {f[0]=f[1]=f[2]=0.; x[2] = bfac*x[2]; return;}
  rr  = zr - azm;

  /* 2-dim 2nd order interpolation inside POLTG for each component
     of the field */

  /* Compute the Taylor's series; calculate f[0] and f[2] only */
  /* NOTE: tailorp[][2][6] contains 2 components already ! */

  Q = (float *) &tailorp[izm*mx3 + ixm*3][0][0];
  for(i=0; i<3; i+=2)
  {
    f[i] = Q[0] + 0.5*( pp*(Q[1] + pp*Q[2] )
                      + rr*(Q[3] + rr*Q[4] ) )
                      + pp*rr*Q[5];
    Q+=6;
  }

  /* old way
  Q = QP + izm*mx3 + ixm*3;
  for(i=0; i<3; i+=2)
  {
    f000 = *Q;
    f00p = *(Q+mx3);
    f00m = *(Q-mx3);
    fp00 = *(Q+3);
    fm00 = *(Q-3);
    fp0p = *(Q+mx3+3);

    cp = fp00+fm00-2.*f000;
    cr = f00p+f00m-2.*f000;
    dr = f000-f00p+fp0p-fp00;

    f[i] = f000 + 0.5*( pp*(fp00-fm00) + pp*pp*cp
                      + rr*(f00p-f00m) + rr*rr*cr )
                + pp*rr*dr;
    Q+=2;
  }
*/

  /* Restore the original Y-coordinate */
  x[2] = bfac * x[2];

  /* Rotation around Z */

  bx   = bfac * f[0];
  if(r > 0.0001)
  {
   f[0] = bx * x[0] / r;
   f[1] = bx * x[1] / r;
  }

  /* reordering field components */

  /* POKA SNARUZHI
  temp = f[0];
  f[0] = f[2];
  f[2] = f[1];
  f[1] = temp;
  */

  return;
}




/*
   Purpose and Methods : Reads or create a table of magnetic field.

   Inputs  :
   Outputs : structure BGRID
   Controls:

   Library belongs: libsda.a

   Calls: sda_torus

   Created   28-OCT-1990   Bogdan Niczyporuk

   Called by sda_init
*/

D 3
#include <string.h>
#include <stdlib.h>
E 3
#include "sdakeys.h"
SDAkeys sdakeys_;

void
dcminit_()
{
  int i, NX3, NYNX3, IX,IY,IZ, NYc, Indx, nrw, numra, numrb, icl, ierr,
      numdb, ncol, nrow, nch, nread;
  float time_init, time_out, time_exe, B[3], R[3], Wlen, Sbt, Smt, Spt,
        Cmax_t, Cmax_m, Cmax_p, Qm[Ng];
  char *parm, *fname, str[100];
  char dtimes[25];
  char format1[13];
  char format2[5];
  char kname[9];
  char Tname[9];
  char Mname[9];
  char Pname[9];
  char hname1[9];
  char hname2[9];

  {int i; for(i=0; i<13; i++) format1[i] = ' '; format1[12] = '\0';}
  {int i; for(i=0; i<5; i++) format2[i] = ' '; format2[4] = '\0';}
  {int i; for(i=0; i<9; i++) kname[i] = ' '; kname[8] = '\0';}
  {int i; for(i=0; i<9; i++) Tname[i] = ' '; Tname[8] = '\0';}
  {int i; for(i=0; i<9; i++) Mname[i] = ' '; Mname[8] = '\0';}
  {int i; for(i=0; i<9; i++) Pname[i] = ' '; Pname[8] = '\0';}
  {int i; for(i=0; i<9; i++) hname1[i] = ' '; hname1[8] = '\0';}
  {int i; for(i=0; i<9; i++) hname2[i] = ' '; hname2[8] = '\0';}

  grd = (SDAbgrid *)&sdabgrid_.NX;
  pol = (POLTG *)&poltg_.MX;

  /* Open and read old table */

  if(sdakeys_.mgtyp == 0)
  {
    /* get parameter's directory */
    if((parm = (char *)getenv("CLON_PARMS")) == NULL)
    {
      printf("dcfield: ERROR: CLON_PARMS is not defined - exit.\n");
      exit(0);
    }

   /* initialize just in case nothing has been read */
    strcpy(Tname,"BTORU");
    strcpy(Mname,"BMINT");
    strcpy(Pname,"BPOLT");
    Cmax_t = (float)Imax_t;
    Cmax_m = (float)Imax_m;
    Cmax_p = (float)Imax_p;
    Sbt = Smt = Spt = 0.;

    /* read B for PTF */

    if(ABS(sdakeys_.zmagn[3]) > 0.)
    {
      strcpy(str,"OPEN BGRID UNIT=55 FILE=\"");
       strcat(str,parm);
       strcat(str,"/TRLIB/");
       strcat(str,sdakeys_.bpolfile);
      strcat(str,"\"");
      fparm_(str,strlen(str));
      frname_("BGRID",5);
      frkey_(kname,&numra,&numrb,&icl,&ierr,strlen(kname));
      strcpy(Pname,kname);
      frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,
             strlen(hname1),strlen(format1));
      nrw = ncol*nrow;
      frdat_(&nread,&(pol->MX),&nrw);
	  /*printf("%d %d -> %d %d %d\n",nread,nrw,pol->MX,pol->MY,pol->MZ);*/
      Cmax_p = pol->Cpmax;

      if(pol->Mgr > Ng)
      {
        printf("dcminit: Needs %d, but in structure BGRID is %d\n",
                pol->Mgr,Ng);
        return;
      }

      frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,
             strlen(hname2),strlen(format2));
      nrw = ncol*nrow;
      frdat_(&nread,pol->Qm,&nrw);
	  /*for(i=0; i<nread; i++) if(pol->Qm[i]>5.) printf("[%d]==>%f\n",i,pol->Qm[i]);*/
      fparm_("CLOSE BGRID",11);
D 3
      {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
      getasciitime_(&(pol->dptime),dtimes,strlen(dtimes));
E 3
I 3
      {int i; for(i=0; i<24; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
      getasciitime_((time_t *)&(pol->dptime),dtimes,strlen(dtimes));
E 3
      printf("dcminit: Read file1 for %s created on %s\n",kname,dtimes);
      printf("dcminit: P-field address is %d\n",pol->Qm);

      /* rescale for PTF */
      Spt = sdakeys_.zmagn[3] / Cmax_p;
      for(i=0; i<Ng; i++) pol->Qm[i] = Spt * pol->Qm[i];

    }

    /* read B for Torus */
    if(ABS(sdakeys_.zmagn[1]) > 0.)
    {
      strcpy(str,"OPEN BGRID UNIT=55 FILE=\"");
       strcat(str,parm);
       strcat(str,"/TRLIB/");
       strcat(str,sdakeys_.btorfile);
      strcat(str,"\"");
      fparm_(str,strlen(str));
      frname_("BGRID",5);
      frkey_(kname,&numra,&numrb,&icl,&ierr,strlen(kname));
      strcpy(Tname,kname);
      frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,
             strlen(hname1),strlen(format1));
      nrw = ncol*nrow;
      frdat_(&nread,&(grd->NX),&nrw);
      Cmax_t = grd->Cmax;

      if(grd->Ngr > Ng)
      {
        printf("dcminit: Needs %d, but in structure BGRID is %d\n",
                grd->Ngr,Ng);
        return;
      }

      frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,
             strlen(hname2),strlen(format2));
      nrw = ncol*nrow;
      frdat_(&nread,grd->Q,&nrw);

      fparm_("CLOSE BGRID",11);
D 3
      {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
      getasciitime_(&(grd->dtime),dtimes,strlen(dtimes));
E 3
I 3
      {int i; for(i=0; i<24; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
      getasciitime_((time_t *)&(grd->dtime),dtimes,strlen(dtimes));
E 3
      printf("dcminit: Read file2 for %s created on %s\n",kname,dtimes);
      printf("dcminit: B-field address is %d\n",grd->Q);

      /* rescale for Torus */
      Sbt = sdakeys_.zmagn[1] / Cmax_t;
      for(i=0; i<Ng; i++) grd->Q[i] = Sbt * grd->Q[i];

    }
    else
	{
      for(i=0; i<Ng; i++) grd->Q[i] = 0;
    }

    /* Read B for Mini Torus */

    if(ABS(sdakeys_.zmagn[2]) > 0.)
    {
      strcpy(str,"OPEN BGRID UNIT=56 FILE=\"");
       strcat(str,parm);
       strcat(str,"/TRLIB/");
       strcat(str,sdakeys_.bminfile);
      strcat(str,"\"");
      fparm_(str,strlen(str));
      frname_("BGRID",5);
      frkey_(kname,&numra,&numrb,&icl,&ierr,strlen(kname));
      strcpy(Mname,kname);
      frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,
             strlen(hname1),strlen(format1));
      nrw = ncol*nrow;
      frdat_(&nread,&(grd->NX),&nrw);
      Cmax_m = grd->Cmax;

      if(grd->Ngr > Ng)
      {
        printf("dcminit: Needs %d, but in structure BGRID is %d\n",
                grd->Ngr,Ng);
        return;
      }

      frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,
             strlen(hname2),strlen(format2));
      nrw = ncol*nrow;
      frdat_(&nread,Qm,&nrw);
      fparm_("CLOSE BGRID",11);
D 3
      {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
      getasciitime_(&(grd->dtime),dtimes,strlen(dtimes));
E 3
I 3
	  /*printf("11\n");fflush(stdout);*/
      {int i; for(i=0; i<24; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
	  /*printf("12\n");fflush(stdout);*/
      getasciitime_((time_t *)&(grd->dtime),dtimes,strlen(dtimes));
	  /*printf("13\n");fflush(stdout);*/
E 3
      printf("dcminit: Read file3 for %s created on %s\n",kname,dtimes);

      /* Rescale and Sum the B-fields */
      Smt = sdakeys_.zmagn[2] / Cmax_m;
      for(i=0; i<Ng; i++) grd->Q[i] = grd->Q[i] + Smt * Qm[i];
    }

    printf("dcminit: B-field = %f x %s + %f x %s + %f x %s\n",
                 Sbt,Tname,Smt,Mname,Spt,Pname);


    /*************************************************/
    /* now fill some variables and Tailor series map */
    /*************************************************/

    /* torus or minitorus */

    if(ABS(sdakeys_.zmagn[1]) > 0. || ABS(sdakeys_.zmagn[2]) > 0.)
    {
      int j;
      float tmp1,tmp2,tmp3;
      float cp, cq, cr, dp, dq, dr, e;
      float f000,f00p,f00m,f0p0,f0m0,fp00,fm00,f0pp,fp0p,fpp0,fppp; 

      /* fill some static variables for the torus */

      QQ = grd->Q;
      printf("dcfield: B-field address: %u\n",grd->Q);

      rdx = 1./grd->DX;
      rdy = 1./grd->DY;
      rdz = 1./grd->DZ;
      nx3   = grd->NX * 3;
      nynx3 = grd->NX * grd->NY * 3;

      NX   = grd->NX;
      NY   = grd->NY;
      NZ   = grd->NZ;
      Xmin = grd->Xmin;
      Ymin = grd->Ymin;
      Zmin = grd->Zmin;

      /* fill Tailor's series array for the torus */

      for(j=0; j<Ng; j++)
      {
        float *Q;
        if((j-nynx3) < 0 || (j+nynx3+nx3+3 +3) > Ng) continue;
        Q = (float *) &QQ[j];
        for(i=0; i<3; i++)
        {
          f000 = tailor[j][i][0]  = *Q;
          f00p = *(Q+nynx3);
          f00m = *(Q-nynx3);
          f0p0 = *(Q+nx3);
          f0m0 = *(Q-nx3);
          fp00 = *(Q+3);
          fm00 = *(Q-3);
          f0pp = *(Q+nynx3+nx3);
          fp0p = *(Q+nynx3+3);
          fpp0 = *(Q+nx3+3);
          fppp = *(Q+nynx3+nx3+3);
          tmp1 = tailor[j][i][1] = fp00-fm00;
          cp   = tailor[j][i][2] = fp00+fm00-2.*f000;
          tmp2 = tailor[j][i][3] = f0p0-f0m0;
          cq   = tailor[j][i][4] = f0p0+f0m0-2.*f000;
          tmp3 = tailor[j][i][5] = f00p-f00m;
          cr   = tailor[j][i][6] = f00p+f00m-2.*f000;
          dp   = tailor[j][i][7] = f000-fp00+fpp0-f0p0;
          dq   = tailor[j][i][8] = f000-f0p0+f0pp-f00p;
          dr   = tailor[j][i][9] = f000-f00p+fp0p-fp00;
          e    = tailor[j][i][10] = -dp-f0pp+f00p-fp0p+fppp;
          Q++;
        }
      }
    }

    /* polarized target */

    if(ABS(sdakeys_.zmagn[3]) > 0.)
    {
      int j;
      float cp, cr, dr, tmp1, tmp2;
      float f000,f00p,f00m,fp00,fm00,fp0p;

      QP = pol->Qm;
      printf("dcfield: PT-field address: %u\n",pol->Qm);

      MX = pol->MX;
      MZ = pol->MZ;
      Xpmin = pol->Xpmin;
      Zpmin = pol->Zpmin;
      Xpmax = pol->Xpmin + pol->MX * pol->DDX;
      Zpmax = pol->Zpmin + pol->MZ * pol->DDZ;

      rddxy = 1./pol->DDX;
      rddz  = 1./pol->DDZ;
      mx3   = pol->MX * 3;
      for(j=0; j<Ng; j++)
      {
        float *Q;
        if((j-mx3) < 0 || (j+mx3+3 +3) > Ng) continue;
        Q = (float *) &QP[j];
        for(i=0; i<2; i++)
        {
          f000 = tailorp[j][i][0] = *Q;
          f00p = *(Q+mx3);
          f00m = *(Q-mx3);
          fp00 = *(Q+3);
          fm00 = *(Q-3);
          fp0p = *(Q+mx3+3);
          tmp1 = tailorp[j][i][1] = fp00-fm00;
          cp   = tailorp[j][i][2] = fp00+fm00-2.*f000;
          tmp2 = tailorp[j][i][3] = f00p-f00m;
          cr   = tailorp[j][i][4] = f00p+f00m-2.*f000;
          dr   = tailorp[j][i][5] = f000-f00p+fp0p-fp00;

          Q+=2;
        }
      }

    }
  }

  return;
}


void
dcmreverse_()
{
  int i;

  for(i=0; i<Ng; i++)
  {
    grd->Q[i] = -grd->Q[i];
  }

  return;
}
E 1
