h19984
s 00060/00036/00134
d D 1.4 07/11/01 18:53:50 boiarino 5 4
c *** empty log message ***
e
s 00101/00049/00069
d D 1.3 07/11/01 14:57:22 boiarino 4 3
c new version from Franz
c 
e
s 00001/00000/00117
d D 1.2 03/04/17 16:54:54 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/stlib.c
e
s 00117/00000/00000
d D 1.1 01/11/19 19:06:01 boiarino 1 0
c date and time created 01/11/19 19:06:01 by boiarino
e
u
U
f e 0
t
T
I 1

/* stlib.c - start counter library */

#include <stdio.h>
I 3
#include <stdlib.h>
E 3
#include <math.h>
#include "sdakeys.h"
SDAkeys sdakeys_;

/* returns ST constants from SDA calibration file */
I 4
/* corrected for new start counter */
E 4

#define NWST 30

I 5
/* called from 'sda.s/sda_calib.F' only */
E 5
void
D 4
streadcalib_(int *runno, float *Elos_st, float *Ethr_st, float cal_st[6][5])
E 4
I 4
streadcalib_(int *runno, float *Elos_st, float *Ethr_st, float cal_st[24][10])
E 4
{
D 4
  int i;
  float data[NWST+2], *cst;
E 4
I 4
  int i,j, j1;
  float data[NWST+2];
E 4
  char *parm, fname[256];

  if((parm = (char *) getenv("CLON_PARMS")) == NULL)
  {
    printf("stlib: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }
  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.cbosfile);
  printf("stlib: reading calibration file >%s< for run # %d\n",fname,*runno);

  if(utGetFpackRecord(fname, "CST ", NWST+2, data))
  {
D 4
    cst = (float *) cal_st;
    for(i=0; i<NWST; i++) cst[i] = data[i];
E 4
I 4
    for(j=0; j< 6; i++) 
      for(i=0; i< 5; i++) 
	for(j1=0; j1<4; j1++) cal_st[j*4+j1][i] = data[i+j*5];
E 4
    *Elos_st = data[NWST];
    *Ethr_st = data[NWST+1];
  }
D 4

E 4
I 4
  for(j=0; j<24; j++) {
    cal_st[j][5] = 1000.;
    cal_st[j][6] = 0.;    /* default: no timewalk correction */
    cal_st[j][7] = 0.;
    cal_st[j][8] = cal_st[j][3];  /* default: same prop.vel. in leg and nose */
    cal_st[j][9] = 0.;
  }
E 4
  return;
}

/* returns ST constants from MAP */

void
D 4
stgetcalib_(int *runno, float *Elos_st, float *Ethr_st, float cal_st[6][5])
E 4
I 4
stgetcalib_(int *runno, float *Elos_st, float *Ethr_st, float cal_st[24][10])
E 4
{
I 5
  FILE *fd;
E 5
D 4
  int id, run, iret;
E 4
I 4
  int id, i, j, run, iret;
E 4
  float st_del[6], st_t1[6], st2tof, stdel[6], veff[6];
I 4
  float stn_del[24], stn_t0[24], stn_t1[24];
  float adc_atten[24], adc_scale[24];
  float veff_leg[24], veff_nose[24], veff_nose2[24];
  float adc_max[24], stn_W1[24], stn_W2[24];
E 4
  char map[1000], *dir;

D 4
  run = *runno;
  run = 11578; /* was in SDA !!! */

E 4
D 5
  if((dir = (char *)getenv("CLAS_PARMS")) == NULL)
D 4
  {
    printf("stlib: CLAS_PARMS not defined !!!\n");
    exit(0);
  }
E 4
I 4
    {
      printf("stlib: CLAS_PARMS not defined !!!\n");
      exit(0);
    }
E 5
I 5
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("stlib: CLON_PARMS not defined !!!\n");
    exit(0);
  }
E 5
E 4

D 4
  *Elos_st = 2.018592;           /* energy loss [Mev/cm]   */
  *Ethr_st = 0.05;               /* energy threshold [MeV] */
E 4
I 4
D 5
  if(*runno<43475 ) {   /* old start counter (run 43475=start of g11) */
E 5
I 5
  if(*runno<43475 )   /* old start counter (run 43475=start of g11) */
  {
E 5
    run = 11578; /* was in SDA !!! (g1c) */
E 4

I 4
    *Elos_st = 2.018592;           /* energy loss [Mev/cm]   */
    *Ethr_st = 0.05;               /* energy threshold [MeV] */

E 4
  /* for Start Counter "id" is a sector number */

D 4
  sprintf(map,"%s/Maps/%s.map",dir,"ST_CALIB");
  printf("stlib: reading calibration file >%s< for run # %d\n",map,run);
E 4
I 4
    sprintf(map,"%s/Maps/%s.map",dir,"ST_CALIB");
D 5
    printf("stlib: reading calibration file >%s< for run # %d\n",map,run);
E 5
I 5
    if((fd=fopen(map,"r")) != NULL)
    {
      printf("stlib: reading calibration map >%s< for run # %d\n",map,run);
      fclose(fd);
    }
    else
    {
      printf("stlib: error opening map file >%s< - exit\n",map);
      exit(0);
    }
E 5
    
    map_get_float(map,"delta_T", "pair2pair",3, st_del   ,  run, &iret);
    map_get_float(map,"delta_T", "side2side",3, &st_del[3], run, &iret);
    map_get_float(map,"T1",      "value",    6, st_t1,      run, &iret);
    map_get_float(map,"st2tof",  "value",    1, &st2tof,    run, &iret);
    map_get_float(map,"veff_leg","value",    6, veff,       run, &iret);
    
    stdel[0] = st_del[0] + st2tof;
    stdel[1] =  stdel[0] + st_del[3];
    stdel[2] =  stdel[0] + st_del[1];
    stdel[3] =  stdel[2] + st_del[4];
    stdel[4] =  stdel[0] + st_del[2];
    stdel[5] =  stdel[4] + st_del[5];
    printf("stlib: MAP ST Cal = %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
	   stdel[0],stdel[1],stdel[2],stdel[3],stdel[4],stdel[5]);
E 4

D 4
  map_get_float(map,"delta_T", "pair2pair",3, st_del   ,  run, &iret);
  map_get_float(map,"delta_T", "side2side",3, &st_del[3], run, &iret);
  map_get_float(map,"T1",      "value",    6, st_t1,      run, &iret);
  map_get_float(map,"st2tof",  "value",    1, &st2tof,    run, &iret);
  map_get_float(map,"veff_leg","value",    6, veff,       run, &iret);

  stdel[0] = st_del[0] + st2tof;
  stdel[1] =  stdel[0] + st_del[3];
  stdel[2] =  stdel[0] + st_del[1];
  stdel[3] =  stdel[2] + st_del[4];
  stdel[4] =  stdel[0] + st_del[2];
  stdel[5] =  stdel[4] + st_del[5];
  printf("stlib: MAP ST Cal = %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
                stdel[0],stdel[1],stdel[2],stdel[3],stdel[4],stdel[5]);

E 4
D 5
  /*************************/
  /* SDA Calibration of ST */ 
E 5
I 5
    /*************************/
    /* SDA Calibration of ST */ 
E 5

D 5
  /* New (Dec.16,1998) due to that ToF calib. moved by ~1ns */
E 5
I 5
    /* New (Dec.16,1998) due to that ToF calib. moved by ~1ns */
E 5
D 4
  stdel[0] = 85.54;
  stdel[1] = 96.38;
  stdel[2] = 88.21;
  stdel[3] = 88.15;
  stdel[4] = 90.61;
  stdel[5] = 86.78;
E 4
I 4
    stdel[0] = 85.54;
    stdel[1] = 96.38;
    stdel[2] = 88.21;
    stdel[3] = 88.15;
    stdel[4] = 90.61;
    stdel[5] = 86.78;
E 4

D 5
  /* Old
  stdel[0] = 86.75;
  stdel[1] = 97.77;
  stdel[2] = 89.44;
  stdel[3] = 89.48;
  stdel[4] = 92.00;
  stdel[5] = 87.99;
  */
E 5
I 5
    /* Old
    stdel[0] = 86.75;
    stdel[1] = 97.77;
    stdel[2] = 89.44;
    stdel[3] = 89.48;
    stdel[4] = 92.00;
    stdel[5] = 87.99;
    */
E 5

D 4
  printf("stlib: SDA ST Cal = %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
                stdel[0],stdel[1],stdel[2],stdel[3],stdel[4],stdel[5]);
E 4
I 4
    printf("stlib: SDA ST Cal = %8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
	   stdel[0],stdel[1],stdel[2],stdel[3],stdel[4],stdel[5]);
E 4

D 4
  for(id=0; id<6; id++)
  {
    cal_st[id][0] = stdel[id];     /* cable length [ns] */
    cal_st[id][1] = 1./st_t1[id];  /* slope [cnts/ns] */
    cal_st[id][2] = 30.;           /* ADC conversion [cnts/MeV] */
    cal_st[id][3] = veff[id];      /* velocity [cm/ns] of signal propagation */
    cal_st[id][4] = 300.;          /* attenuation length [cm] in a slab */
E 4
I 4
D 5
    for(id=0; id<6; id++) {
      for(j=0; j<4; j++) {
	cal_st[id*4+j][0] = stdel[id];     /* cable length [ns] */
	cal_st[id*4+j][1] = 1./st_t1[id];  /* slope [cnts/ns] */
	cal_st[id*4+j][2] = 30.;           /* ADC conversion [cnts/MeV] */
	cal_st[id*4+j][3] = veff[id];      /* velocity [cm/ns] of signal propagation */
	cal_st[id*4+j][4] = 300.;          /* attenuation length [cm] in a slab */
	cal_st[id*4+j][5] = 1000.;
	cal_st[id*4+j][6] = 0.;
	cal_st[id*4+j][7] = 0.;
	cal_st[id*4+j][8] = veff[id];
	cal_st[id*4+j][9] = 0.;
E 5
I 5
    for(id=0; id<6; id++)
    {
      for(j=0; j<4; j++)
      {
	    cal_st[id*4+j][0] = stdel[id];     /* cable length [ns] */
	    cal_st[id*4+j][1] = 1./st_t1[id];  /* slope [cnts/ns] */
	    cal_st[id*4+j][2] = 30.;           /* ADC conversion [cnts/MeV] */
	    cal_st[id*4+j][3] = veff[id]; /* velocity [cm/ns] of signal propagation */
	    cal_st[id*4+j][4] = 300.;     /* attenuation length [cm] in a slab */
	    cal_st[id*4+j][5] = 1000.;
	    cal_st[id*4+j][6] = 0.;
	    cal_st[id*4+j][7] = 0.;
	    cal_st[id*4+j][8] = veff[id];
	    cal_st[id*4+j][9] = 0.;
E 5
      }
    }

E 4
  }
I 4
D 5
  else {   /* new start counter: STN_CALIB map */
E 4

E 5
I 5
  else   /* new start counter: STN_CALIB map */
  {
E 5
I 4
    sprintf(map,"%s/Maps/%s.map",dir,"STN_CALIB");
D 5
    printf("stlib: reading calibration file >%s< for run # %d\n",map,*runno);
E 5
I 5
    if((fd=fopen(map,"r")) != NULL)
    {
      printf("stlib: reading calibration map >%s< for run # %d\n",map,*runno);
      fclose(fd);
    }
    else
    {
      printf("stlib: error opening map file >%s< - exit\n",map);
      exit(0);
    }
E 5
    
    map_get_float(map,"delta_T", "pd2pd",    24, stn_del,   *runno, &iret);
    map_get_float(map,"delta_T", "st2tof",    1, &st2tof,   *runno, &iret);
D 5
    map_get_float(map,"T0",      "value",    24,  stn_t0,   *runno, &iret);
    map_get_float(map,"T1",      "value",    24,  stn_t1,   *runno, &iret);
E 5
I 5
    map_get_float(map,"T0",      "value",    24, stn_t0,    *runno, &iret);
    map_get_float(map,"T1",      "value",    24, stn_t1,    *runno, &iret);
E 5
    map_get_float(map,"adc2edep","reg1exp",  24, adc_atten, *runno, &iret);
    map_get_float(map,"adc2edep","reg1scale",24, adc_scale, *runno, &iret);
    map_get_float(map,"veff",    "leg",      24, veff_leg,  *runno, &iret);
    map_get_float(map,"veff",    "nose",     24, veff_nose, *runno, &iret);
    map_get_float(map,"veff",    "nose2",    24, veff_nose2,*runno, &iret);
    map_get_float(map,"adc_max", "value",    24, adc_max,   *runno, &iret);
    map_get_float(map,"W1",      "value",    24, stn_W1,    *runno, &iret);
    map_get_float(map,"W2",      "value",    24, stn_W2,    *runno, &iret);
    
D 5
    for( id=0; id<24; id++) {
E 5
I 5
    for(id=0; id<24; id++)
    {
E 5
      cal_st[id][0] = -(stn_del[id] + stn_t0[id] - st2tof);
      cal_st[id][1] = -1./stn_t1[id];          /* pipeline tdc */
      cal_st[id][2] = adc_scale[id];
      cal_st[id][3] = veff_leg[id];
      cal_st[id][4] = adc_atten[id] ? 1./adc_atten[id] : 40.;
      cal_st[id][5] = adc_max[id];
      cal_st[id][6] = stn_W1[id];
      cal_st[id][7] = stn_W2[id];
      cal_st[id][8] = veff_nose[id];
      cal_st[id][9] = veff_nose2[id];
    }

  }

E 4
  return;
}




E 1
