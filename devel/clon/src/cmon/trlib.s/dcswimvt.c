/*
   Purpose and Methods : Back swimming in the PT magnetic field.
                         Calculates track points on the target plane#1.

   Inputs  : isec    - current sector number
             svin(i) - Track Parameters at L4 {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
             step    - step size (cm) -----> LOCAL NOW !!!!!!!!!!!!!!!!!
             level   - reconstruction level: 2 for HBT, 4 for TBT

   Outputs : svout(i)- Track Parameters at VT {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
             vertex(10)

   Controls:

   Library belongs    : libsda.a

   Calls   : dchelix, ...

   Created   NOV-10-1998  Bogdan Niczyporuk

   Called by ana_prfit and ana_fit

*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

float vmod_(float *,int *);
float sda_point2line_();
float sda_line2line_();

#define ECon  2.99792E-4
/* Swimming parameters */
#define STmin  0.10
#define STmax  5.00
#define Dang   0.001

static float bdir[3] = {1.,0.,0.}; /* in SCS beam along the X axis */

void
dcswimvt_(int *iw, float pln[npln][20], int *isec, float svin[9], float *stepp,
          int *level, int *if_draw, float svout[9], float vertex[10],
          float trp[npln][7], int *nxyz, float xyz[mxstep][3])
{
  dcswimvt(iw,pln,*isec,svin,*stepp,*level,*if_draw,svout,vertex,trp,nxyz,xyz);
}

void
dcswimvt(int *iw, float pln[npln][20], int isec, float svin[9], float stepp,
          int level, int if_draw, float svout[9], float vertex[10],
          float trp[npln][7], int *nxyz, float xyz[mxstep][3])
{
  int i, j, ist, ip, iflag, nstep, itmp;
  float Bold, Dangle, sint, stot, s4, Dplane,step;
  float vect[9], vout[9], xint[9], bfld[9];
  float dbeam,dbeamold;
  float bpoint[3],rr0[3],rr1[3];
  float tmp, tmp1, tmp2;
  int nstep_xyztmp1, nstep_xyztmp2;
  float xyztmp1[mxstep][3],xyztmp2[mxstep][3];

  /* get vertex position in 'isec' Sector Coordinate System */
  sda_vertex_(iw,&isec,bpoint);
  /*printf("bpoint= %f %f %f\n",bpoint[0],bpoint[1],bpoint[2]);*/

  /* copy P,m,Q to the output array; they will not be changed */
  for(i=6; i<9; i++)
  {
    vect[i] = svin[i];
    svout[i] = svin[i];
  }

  /* Reverse the Charge and the Direction of the Track */
  vect[8] = -svin[8];
  for(i=0; i<3; i++)
  {
    vect[i] = svin[i];
    vect[i+3] = -svin[i+3];
  }  

  /* Initialise indices */
  Dangle = Dang/ECon;
  nstep = 10000;
  stot  = 0.0;

  /* Save initial Plane#1 parameters */
  Dplane = pln[0][7]; 

  /* Initialize hit array */
  for(j=0; j<3; j++)
  {
    trp[0][j]   = 1000.;
    trp[0][j+3] = 0.;
  }
  trp[0][6] = 0.;
  pln[0][7] = 0.001;

  /* Get 'bfld' in order to derive the first step size */
  step = 0.01;
  dchelix(step, vect, vout, bfld);
  /*dcrkuta(step, vect, vout, bfld);*/
  dbeamold = sda_point2line_(vout,bpoint,bdir);

  /* save drawing info if any */
  if(if_draw == 1 && level == 4)
  {
    nstep_xyztmp1 = *nxyz;
    for(j=0; j<nstep_xyztmp1; j++)
	{
      xyztmp1[j][0] = xyz[j][0];
      xyztmp1[j][1] = xyz[j][1];
      xyztmp1[j][2] = xyz[j][2];
    }
  }

  /* Do loop traces particle through detector */

  for(ist=1; ist<=nstep; ist++)
  {
    /* Calculate a step size */
	if(svin[8] != 0.) /* charged particle */
	{
      itmp=3;
      Bold = vmod_(bfld,&itmp);
      if(Bold > 0.05)
      {
        step = Dangle*vect[6]/Bold;
        if(step < STmin) step = STmin;
        if(step > STmax) step = STmax;
      }
      else
      {
        step = STmax;
      }
    }
    else              /* neutral particle */
    {
      step = 10.;
    }

	/* do step */
    dchelix(step, vect, vout, bfld);
	/*dcrkuta(step, vect, vout, bfld);*/

    /* calculate the distance to the beam line */
    dbeam = sda_point2line_(vout,bpoint,bdir);
	/*
printf("dbeam=%f dbeamold=%f xyz=%8.3f%8.3f%8.3f\n",
dbeam,dbeamold,vout[0],vout[1],vout[2]); 
	*/
    /* check if track reached beam line closest approach */
    if(dbeam >= dbeamold)
	{
      dbeam = sda_line2line_(vout,&vout[3],bpoint,bdir,rr0,rr1);
      for(i=0; i<7; i++) xint[i] = vout[i];
      for(i=0; i<3; i++) xint[i] = rr0[i];
      for(i=0; i<7; i++) vertex[i] = xint[i];
      for(i=0; i<3; i++) vertex[i+7] = bpoint[i];
      ip = 1;
      for(i=0; i<3; i++)
	  {
        trp[ip-1][i]   = xint[i];
        trp[ip-1][i+3] =-xint[i+3];
        svout[i]       = xint[i];
        svout[i+3]     =-xint[i+3];
	  }
      stot += sint;
      /*sda_trp(7,ip) = sda_pln(8,ip,isec) - track length */
      /* Store points for trajectory drawing */
      if(if_draw == 1  && level == 4 && ist <= mxstep)
	  {
        xyztmp2[ist-1][0] = xint[0];
        xyztmp2[ist-1][1] = xint[1];
        xyztmp2[ist-1][2] = xint[2];
        nstep_xyztmp2  = ist;
      }
	  /*
if(if_draw==1 && level==4) printf("final: ist=%d -> %f %f %f\n",ist,xint[0],xint[1],xint[2]);
	  */
      break; /* exit from loop over Nstep */
    }
    else
    {
      dbeamold = dbeam;
      /* Store points for trajectory drawing */
      if(if_draw == 1 && level == 4  && ist <= mxstep)
      {
        xyztmp2[ist-1][0] = vect[0];
        xyztmp2[ist-1][1] = vect[1];
        xyztmp2[ist-1][2] = vect[2];
        nstep_xyztmp2 = ist;
      }
	  /*
if(if_draw==1 && level==4) printf("ddddd: ist=%d -> %f %f %f\n",ist,vect[0],vect[1],vect[2]);
	  */
    }

    stot += step;
    for(j=0; j<9; j++) vect[j] = vout[j];
  } /* End loop over Nstep */

  /* final drawing array */
  if(if_draw==1 && level==4)
  {
	/*
printf("== %d %d\n",nstep_xyztmp1,nstep_xyztmp2);
	*/
    /* copy vertex2layer1 trajectory */
    for(j=0; j<nstep_xyztmp2; j++)
	{
      xyz[j][0] = xyztmp2[nstep_xyztmp2-j-1][0];
      xyz[j][1] = xyztmp2[nstep_xyztmp2-j-1][1];
      xyz[j][2] = xyztmp2[nstep_xyztmp2-j-1][2];
    }
    /* copy the rest of trajectory */
    for(j=1; j<=nstep_xyztmp1; j++)
    {
      xyz[j-1+nstep_xyztmp2][0] = xyztmp1[j-1][0];
      xyz[j-1+nstep_xyztmp2][1] = xyztmp1[j-1][1];
      xyz[j-1+nstep_xyztmp2][2] = xyztmp1[j-1][2];
      if(j+nstep_xyztmp2 >= mxstep) goto a10;
	}
    j--;
a10:
    *nxyz = nstep_xyztmp2 + j;
  }

  /* print xyz array
  for(j=0; j<(*nxyz); j++)
  {
    printf("grid=%4d  xyz=%8.3f%8.3f%8.3f\n",j+1,xyz[j][0],xyz[j][1],xyz[j][2]);
  }
  */

  /* Correct path lengths for all DC planes */
  s4 = trp[3][6];
  for(i=ndc_min-1; i<npln; i++)
  {
    if(trp[i][0]<999.) trp[i][6] += stot - s4; 
  }

  /* Restore initial parameters for Plane#1 */
  pln[0][7] = Dplane;

  return;
}














