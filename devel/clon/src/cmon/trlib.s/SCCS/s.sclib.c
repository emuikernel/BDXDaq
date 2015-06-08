h51396
s 00001/00001/00849
d D 1.8 07/11/04 14:35:13 boiarino 9 8
c *** empty log message ***
e
s 00004/00004/00846
d D 1.7 07/11/03 13:14:44 boiarino 8 7
c *** empty log message ***
e
s 00036/00036/00814
d D 1.6 07/11/03 11:50:34 boiarino 7 6
c *** empty log message ***
e
s 00002/00002/00848
d D 1.5 07/11/02 20:29:16 fklein 6 5
c *** empty log message ***
e
s 00005/00000/00845
d D 1.4 07/11/02 17:08:47 boiarino 5 4
c *** empty log message ***
e
s 00013/00003/00832
d D 1.3 07/11/01 16:29:10 boiarino 4 3
c CLAS_PARMS -> CLON_PARMS
c 
e
s 00001/00000/00834
d D 1.2 03/04/17 16:54:44 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/sclib.c
e
s 00834/00000/00000
d D 1.1 01/11/19 19:06:01 boiarino 1 0
c date and time created 01/11/19 19:06:01 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : Find SC slab "idsc" in data the track points to


   Input   :  isec - sector number
              x0 - ... for this track

   Outputs :  scid[0] = idsc
              scid[1] = ipsc
              scid[2] = ihsc
              iflg = 0  means OK

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   3-MAY-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcdcam, dcfinde, dcfit
*/

#include <stdio.h>
I 3
#include <stdlib.h>
E 3
#include <string.h>
#include <math.h>

#include "dclib.h"
#include "sdageom.h"
#include "sdakeys.h"
SDAkeys sdakeys_;


/* SC calibration constants structure */

typedef struct sccalib
{
  float Lcabletime; /* [ 0] cable length [ns] for left P.T. */
  float Rcabletime; /* [ 1] cable length [ns] for right P.T. */
  float Ltdc1;      /* [ 2] slope [cnts/ns] for left P.T. */
  float Rtdc1;      /* [ 3] slope [cnts/ns] for right P.T. */
  float Ladc0;   /* [ 4] spare (ADC conversion [cnts/Mev] for left P.T.) */
  float Radc0;   /* [ 5] spare (ADC conversion [cnts/Mev] for right P.T.) */
  float velocity;   /* [ 6] velocity [cm/ns] of signal propagation in slab */
  float atten;   /* [ 7] spare (attenuation length [cm] in a slab) */
  int   bad1;    /* [ 8] spare (mark bad slab ?) */
  int   bad2;    /* [ 9] spare (mark bad slab ?) */
  float Lpedestal;  /* [10] pedestals for left  P.T. */
  float Rpedestal;  /* [11] pedestals for right P.T. */
  float Lwalk0;     /* [12] time walk constant "w0" left */
  float Rwalk0;     /* [13] time walk constant "w0" right */
  float Lwalk1;     /* [14] time walk constant "w1" left */
  float Rwalk1;     /* [15] time walk constant "w1" right */
  float Lwalk2;     /* [16] time walk constant "w2" left */
  float Rwalk3;     /* [17] time walk constant "w2" right */
  float laser;   /* [18] spare ((twL - twR)/2  Laser) */
  float pad2pad;    /* [19] (twL + twR)/2  Data or Cosmic */
  float Ltdc0;      /* [20] T0_TDC -offset to slope, left */
  float Rtdc0;      /* [21] T0_TDC -offset to slope, right */
  float Ltdc2;      /* [22] T2_TDC -quadratic term to slope, left */
  float Rtdc2;      /* [23] T2_TDC -quadratic term to slope, right */
} SCCALIB;

D 7
static SCCALIB sccalib[6][48];
E 7
I 7
static SCCALIB sccalib[6][57];
E 7
static float Elos_sc; /* energy loss [Mev/cm] in SC slab for MIP */
static float Ethr_sc; /* energy threshold [MeV] before attenuation */


#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define vflt   29.97925
#define TM     4094
D 8
#define NMAP   288
D 7
#define NWSC   6*48*24
E 7
I 7
#define NWSC   6*57*24
E 8
I 8
#define NMAP   (6*57)
#define NWSC   (6*57*24)
E 8
E 7

/* the number of channels in TDC and ADC boards */
D 9
#define tdc_max 4095.0
E 9
I 9
#define tdc_max 8000.0
E 9
#define adc_max 8191.0

/* channel corresponding to the leading-edge discriminator threshold
   of 20 mV (approximately 35 channels) NIM */
#define Vt 35.0

/* The gain-matching procedure is accomplished by adjusting PMT high voltage
   so that normally incident MIPs produce a peak in ADC channel 600 after
   pedestal substruction for every counter. The value was chosen from two
   considerations: the maximum count of the ADC is 8191, and we expect
   a dynamic range of 8 which sets the upper limit. NIM */
#define Vmip 600.0

/* time-walk-correction function */
#define Fw(x) ((x) < w0 ? \
     ( w1/pow((x),w2) ) : \
     ( w1*(1.+w2)/pow(w0,w2)-w1*w2*(x)/pow(w0,(1.+w2)) ))


/* temporary until both in use */
void
D 7
old2newCalib(float cal_sc[6][48][24])
E 7
I 7
old2newCalib(float cal_sc[6][57][24])
E 7
{
  int isec, id;

  for(isec=0; isec<6; isec++)
  {
D 7
    for(id=0; id<48; id++)
E 7
I 7
    for(id=0; id<57; id++)
E 7
    {
      sccalib[isec][id].Ltdc0      = cal_sc[isec][id][20];
      sccalib[isec][id].Rtdc0      = cal_sc[isec][id][21];
      sccalib[isec][id].Ltdc1      = cal_sc[isec][id][2];
      sccalib[isec][id].Rtdc1      = cal_sc[isec][id][3];
      sccalib[isec][id].Ltdc2      = cal_sc[isec][id][22];
      sccalib[isec][id].Rtdc2      = cal_sc[isec][id][23];
      sccalib[isec][id].Ladc0      = cal_sc[isec][id][4];
      sccalib[isec][id].Radc0      = cal_sc[isec][id][5];
      sccalib[isec][id].Lcabletime = cal_sc[isec][id][0];
      sccalib[isec][id].Rcabletime = cal_sc[isec][id][1];
      sccalib[isec][id].velocity   = cal_sc[isec][id][6];
      sccalib[isec][id].atten      = cal_sc[isec][id][7];
      sccalib[isec][id].bad1 = (int) cal_sc[isec][id][8];
      sccalib[isec][id].bad2 = (int) cal_sc[isec][id][9];
      sccalib[isec][id].Lpedestal  = cal_sc[isec][id][10];
      sccalib[isec][id].Rpedestal  = cal_sc[isec][id][11];
      sccalib[isec][id].Lwalk0     = cal_sc[isec][id][12];
      sccalib[isec][id].Rwalk0     = cal_sc[isec][id][13];
      sccalib[isec][id].Lwalk1     = cal_sc[isec][id][14];
      sccalib[isec][id].Rwalk1     = cal_sc[isec][id][15];
      sccalib[isec][id].Lwalk2     = cal_sc[isec][id][16];
      sccalib[isec][id].Rwalk3     = cal_sc[isec][id][17];
      sccalib[isec][id].laser      = cal_sc[isec][id][18];
      sccalib[isec][id].pad2pad    = cal_sc[isec][id][19];
    }
  }

  return;  
}

/* returns SC constants from SDA calibration file */

void
D 7
screadcalib(int runno, float *Elos_sc,float *Ethr_sc,float cal_sc[6][48][24])
E 7
I 7
screadcalib(int runno, float *Elos_sc,float *Ethr_sc,float cal_sc[6][57][24])
E 7
{
  int i;
  float data[NWSC+2], *csc;
  char *parm, fname[256];

  if((parm = (char *) getenv("CLON_PARMS")) == NULL)
  {
    printf("sclib: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }
  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.cbosfile);
  printf("sclib: reading calibration file >%s< for run # %d\n",fname,runno);

  if(utGetFpackRecord(fname, "CSC ", NWSC+2, data))
  {
    csc = (float *) cal_sc;
    for(i=0; i<NWSC; i++) csc[i] = data[i];
    *Elos_sc = data[NWSC];
    *Ethr_sc = data[NWSC+1];
  }

  old2newCalib(cal_sc);

  return;
}

void
D 7
screadcalib_(int *runno, float *Elos_sc,float *Ethr_sc,float cal_sc[6][48][24])
E 7
I 7
screadcalib_(int *runno, float *Elos_sc,float *Ethr_sc,float cal_sc[6][57][24])
E 7
{
  screadcalib(*runno, Elos_sc, Ethr_sc, cal_sc);  
}


/* returns SC constants from MAP */

void
D 7
scgetcalib(int runno, float *Elos_sc, float *Ethr_sc,float cal_sc[6][48][24])
E 7
I 7
scgetcalib(int runno, float *Elos_sc, float *Ethr_sc,float cal_sc[6][57][24])
E 7
{
I 4
  FILE *fd;
E 4
  int isec, iv, id, run, iret;
D 7
  int statL[6][48], statR[6][48];
  float pedL[6][48],pedR[6][48],t0L[6][48],t0R[6][48],t1L[6][48],t1R[6][48],
        t2L[6][48],t2R[6][48],sc_lr[6][48],sc_p2p[6][48],w0L[6][48],w0R[6][48],
        w1L[6][48],w1R[6][48],w2L[6][48],w2R[6][48];
  float delL[6][48];
  float delR[6][48];
E 7
I 7
  int statL[6][57], statR[6][57];
  float pedL[6][57],pedR[6][57],t0L[6][57],t0R[6][57],t1L[6][57],t1R[6][57],
        t2L[6][57],t2R[6][57],sc_lr[6][57],sc_p2p[6][57],w0L[6][57],w0R[6][57],
        w1L[6][57],w1R[6][57],w2L[6][57],w2R[6][57];
  float delL[6][57];
  float delR[6][57];
E 7
  char map[1000], *dir;

  /*****************************************/
  /*********** init cable delays ***********/

  for(id= 0; id<23; id++) delL[0][id] = 48.3;
D 7
  for(id=23; id<48; id++) delL[0][id] = 38.2;
E 7
I 7
  for(id=23; id<57; id++) delL[0][id] = 38.2;
E 7
  for(id= 0; id<23; id++) delL[1][id] = 34.7;
D 7
  for(id=23; id<48; id++) delL[1][id] = 27.6;
E 7
I 7
  for(id=23; id<57; id++) delL[1][id] = 27.6;
E 7
  for(id= 0; id<23; id++) delL[2][id] = 36.9;
D 7
  for(id=23; id<48; id++) delL[2][id] = 31.1;
E 7
I 7
  for(id=23; id<57; id++) delL[2][id] = 31.1;
E 7
  for(id= 0; id<23; id++) delL[3][id] = 41.5;
D 7
  for(id=23; id<48; id++) delL[3][id] = 36.8;
E 7
I 7
  for(id=23; id<57; id++) delL[3][id] = 36.8;
E 7
  for(id= 0; id<23; id++) delL[4][id] = 35.0;
D 7
  for(id=23; id<48; id++) delL[4][id] = 22.9;
E 7
I 7
  for(id=23; id<57; id++) delL[4][id] = 22.9;
E 7
  for(id= 0; id<23; id++) delL[5][id] = 37.1;
D 7
  for(id=23; id<48; id++) delL[5][id] = 28.1;
E 7
I 7
  for(id=23; id<57; id++) delL[5][id] = 28.1;
E 7

  for(id= 0; id<23; id++) delR[0][id] = 44.3;
D 7
  for(id=23; id<48; id++) delR[0][id] = 38.4;
E 7
I 7
  for(id=23; id<57; id++) delR[0][id] = 38.4;
E 7
  for(id= 0; id<23; id++) delR[1][id] = 36.0;
D 7
  for(id=23; id<48; id++) delR[1][id] = 24.7;
E 7
I 7
  for(id=23; id<57; id++) delR[1][id] = 24.7;
E 7
  for(id= 0; id<23; id++) delR[2][id] = 34.3;
D 7
  for(id=23; id<48; id++) delR[2][id] = 31.9;
E 7
I 7
  for(id=23; id<57; id++) delR[2][id] = 31.9;
E 7
  for(id= 0; id<23; id++) delR[3][id] = 37.6;
D 7
  for(id=23; id<48; id++) delR[3][id] = 33.6;
E 7
I 7
  for(id=23; id<57; id++) delR[3][id] = 33.6;
E 7
  for(id= 0; id<23; id++) delR[4][id] = 38.2;
D 7
  for(id=23; id<48; id++) delR[4][id] = 22.6;
E 7
I 7
  for(id=23; id<57; id++) delR[4][id] = 22.6;
E 7
  for(id= 0; id<23; id++) delR[5][id] = 34.1;
D 7
  for(id=23; id<48; id++) delR[5][id] = 29.2;
E 7
I 7
  for(id=23; id<57; id++) delR[5][id] = 29.2;
E 7

  /*****************************************/
  /*****************************************/

  run = runno;

D 4
  if((dir = (char *)getenv("CLAS_PARMS")) == NULL)
E 4
I 4
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
E 4
  {
D 4
    printf("sclib: CLAS_PARMS not defined !!!\n");
E 4
I 4
    printf("sclib: CLON_PARMS not defined !!!\n");
E 4
    exit(0);
  }

  *Elos_sc = 2.018592;           /* energy loss [Mev/cm] */
  *Ethr_sc = 1.0;                /* energy threshold [MeV] */

  sprintf(map,"%s/Maps/%s.map",dir,"SC_CALIBRATIONS");
D 4
  printf("sclib: reading calibration file >%s< for run # %d\n",map,run);
E 4
I 4
  if((fd=fopen(map,"r")) != NULL)
  {
D 8
    printf("sclib: reading calibration file >%s< for run # %d\n",map,run);
E 8
I 8
    printf("sclib: reading calibration map >%s< for run # %d\n",map,run);
E 8
    fclose(fd);
  }
  else
  {
    printf("sclib: error opening map file >%s< - exit\n",map);
    exit(0);
  }
E 4


/*
sc/sc_geom.c:  map_get_float(map, "fwd_carriage", "position", 3, (float *)(&fwd_carr_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "north_clam", "position", 3, (float *)(&n_clam_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "south_clam", "position", 3, (float *)(&s_clam_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_1", "position", 3, (float *)(&space_f1_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_2", "position", 3, (float *)(&space_f2_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_3", "position", 3, (float *)(&space_f3_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_4", "position", 3, (float *)(&space_f4_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_5", "position", 3, (float *)(&space_f5_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_6", "position", 3, (float *)(&space_f6_pos.x), runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "fwd_carriage", "rotation", 3, fwd_carr_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "north_clam", "rotation", 3, n_clam_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "south_clam", "rotation", 3, s_clam_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_1", "rotation", 3, space_f1_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_2", "rotation", 3, space_f2_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_3", "rotation", 3, space_f3_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_4", "rotation", 3, space_f4_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_5", "rotation", 3, space_f5_ang, runno, &firsttime);
sc/sc_geom.c:  map_get_float(map, "space_frame_6", "rotation", 3, space_f6_ang, runno, &firsttime);

sc/sc_init.c:  map_get_float(map,"delta_T","left_right",SC_NPADDLES,lr0,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,"pulser","normalisation",1,&pulse_norm,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,"time_walk","ref_adc",1,&Vmip,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,"Yoffset","value",SC_NPADDLES,&yoffset,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,"delta_T","coupled_paddle1",54,&paddle1,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,"delta_T","coupled_paddle2",54,&paddle2,runno,&firsttime);
sc/sc_init.c:    map_get_float(map,"delta_T","paddle2paddle",SC_NPADDLES,c2c,runno,
sc/sc_init.c:    map_get_float(map,"delta_T","panel2panel",SC_NPANELS,p2p,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,subsystem,"left",SC_NPADDLES,sc_cal->left,runno,&firsttime);
sc/sc_init.c:  map_get_float(map,subsystem,"right",SC_NPADDLES,sc_cal->right,runno,&firsttime);
sc/sc_init.c:  map_get_int(map,"status","left",SC_NPADDLES,sc_status.left,runno,&firsttime);
sc/sc_init.c:  map_get_int(map,"status","right",SC_NPADDLES,sc_status.right,runno,&firsttime);
*/



  map_get_int(map,"status","left",  NMAP, statL, run, &iret);
  map_get_int(map,"status","right", NMAP, statR, run, &iret);

  map_get_float(map,"pedestals","left",  NMAP, pedL, run, &iret);
  map_get_float(map,"pedestals","right", NMAP, pedR, run, &iret);

  map_get_float(map,"T0_TDC", "left",  NMAP, t0L, run, &iret);
  map_get_float(map,"T0_TDC", "right", NMAP, t0R, run, &iret);

  map_get_float(map,"T1", "left",  NMAP, t1L, run, &iret);
  map_get_float(map,"T1", "right", NMAP, t1R, run, &iret);

  map_get_float(map,"T2", "left",  NMAP, t2L, run, &iret);
  map_get_float(map,"T2", "right", NMAP, t2R, run, &iret);

D 8
  map_get_float(map,"delta_T", "left_right",    NMAP,  sc_lr, run, &iret);
E 8
I 8
  map_get_float(map,"delta_T", "left_right",    NMAP,  sc_lr, run, &iret); /* not in Map any more !!! */
E 8
  map_get_float(map,"delta_T", "paddle2paddle", NMAP, sc_p2p, run, &iret);

  map_get_float(map,"WALK_A0", "left", NMAP, w0L, run, &iret);
  map_get_float(map,"WALK_A0", "right", NMAP, w0R, run, &iret);

  map_get_float(map,"WALK1", "left", NMAP, w1L, run, &iret);
  map_get_float(map,"WALK1", "right", NMAP, w1R, run, &iret);

  map_get_float(map,"WALK2", "left", NMAP, w2L, run, &iret);
  map_get_float(map,"WALK2", "right", NMAP, w2R, run, &iret);

  for(isec=0; isec<6; isec++)
  {
D 7
    for(id=0; id<48; id++)
E 7
I 7
    for(id=0; id<57; id++)
E 7
    {
      cal_sc[isec][id][0] = delL[isec][id]; /* cable length [ns] for  left PT */
      cal_sc[isec][id][1] = delR[isec][id]; /* cable length [ns] for right PT */
      cal_sc[isec][id][2] = 1./t1L[isec][id]; /* slope [cnts/ns] for  left PT */
      cal_sc[isec][id][3] = 1./t1R[isec][id]; /* slope [cnts/ns] for right PT */
      cal_sc[isec][id][4] = 60.;        /* ADC conv. [cnts/MeV] for  left PT */ 
      cal_sc[isec][id][5] = 60.;        /* ADC conv. [cnts/MeV] for right PT */
      cal_sc[isec][id][6] = 16.;        /* velocity of propagation [cm/ns] */
      cal_sc[isec][id][7] = 300.;       /* attenuation length [cm] in a slab */
      cal_sc[isec][id][8] = statL[isec][id]; /* StatusL (0-OK,1-ADC,2-TDC,3-both) */
      cal_sc[isec][id][9] = statR[isec][id]; /* StatusR (0-OK,1-ADC,2-TDC,3-both) */
      cal_sc[isec][id][10]= pedL[isec][id]; /* PED for left  PT [cnts] */
      cal_sc[isec][id][11]= pedR[isec][id]; /* PED for right PT [cnts] */
      cal_sc[isec][id][12]= w0L[isec][id]; /* time walk constant "w0" left  */
      cal_sc[isec][id][13]= w0R[isec][id]; /* time walk constant "w0" right */
      cal_sc[isec][id][14]= w1L[isec][id]; /* time walk constant "w1" left  */
      cal_sc[isec][id][15]= w1R[isec][id]; /* time walk constant "w1" right */
      cal_sc[isec][id][16]= w2L[isec][id]; /* time walk constant "w2" left  */
      cal_sc[isec][id][17]= w2R[isec][id]; /* time walk constant "w2" right */
      cal_sc[isec][id][18]= sc_lr[isec][id]; /* (twL - twR)/2  Laser */
      cal_sc[isec][id][19]= sc_p2p[isec][id]; /* (twL + twR)/2  Data or Cosmic */
      cal_sc[isec][id][20]= t0L[isec][id]; /* T0_TDC -offset to Lslope */
      cal_sc[isec][id][21]= t0R[isec][id]; /* T0_TDC -offset to Rslope */
      cal_sc[isec][id][22]= t2R[isec][id]; /* T2_TDC -quadratic term to Lslope */
      cal_sc[isec][id][23]= t2R[isec][id]; /* T2_TDC -quadratic term to Rslope */
    }
  }

  old2newCalib(cal_sc);

  return;
}

void
D 7
scgetcalib_(int *runno, float *Elos_sc, float *Ethr_sc,float cal_sc[6][48][24])
E 7
I 7
scgetcalib_(int *runno, float *Elos_sc, float *Ethr_sc,float cal_sc[6][57][24])
E 7
{
  scgetcalib(*runno, Elos_sc, Ethr_sc, cal_sc);
}



/* sc initialization */

int
scinit(int runno)
{
  int itmp;
D 7
  float cal_sc[6][48][24];
E 7
I 7
  float cal_sc[6][57][24];
E 7

I 5
/*always read file (not sure if map is good)
screadcalib(runno, &Elos_sc, &Ethr_sc, cal_sc);
return(0);
*/

E 5
  itmp = 10;
  if(iucomp_("READ",&sdakeys_.lcali,&itmp,4) != 0)
  {
    screadcalib(runno, &Elos_sc, &Ethr_sc, cal_sc);
  }
  else if(iucomp_("INIT",&sdakeys_.lcali,&itmp,4) != 0)
  {
    scgetcalib(runno, &Elos_sc, &Ethr_sc, cal_sc);
  }

  return(0);
}

int
scinit_(int *runno)
{
  scinit(*runno);

  return(0);
}


/* sc match */

void
scmatch_(int *jw, TRevent *ana, int *isec, DCscid *scid, float x0[npl_sc],
         int *iflg)
{
  scmatch(jw, ana, *isec, scid, x0, iflg);
  return;
}

void
scmatch(int *jw, TRevent *ana, int isec, DCscid *scid, float x0[npl_sc],
        int *iflg)
{
  int i, ip, jp, id, ind;
D 7
  int isc, islab, scndig, scdigi[48][5], adc, max_adc, ipsc, ihsc, idsc;
E 7
I 7
  int isc, islab, scndig, scdigi[57][5], adc, max_adc, ipsc, ihsc, idsc;
E 7
  float xx0, dx;
  SCCALIB *sccal;
  sccal = (SCCALIB *) &sccalib[isec-1][id-1];

  /* Derive from the track hit position the slab "id" from the data */

  scid->slab  = 0;
  scid->plane = 0;
  scid->hit   = 0;
  *iflg= 1;
  isc = 0;
  jp  = 0;
  for(ip=nsc_min; ip<=nsc_max; ip++)
  {
    jp++;
    xx0 = x0[ip-nsc_min];
    if(xx0 > 999.) continue;
    /* Check if hit is outside of 4-planes) */
    if(xx0 > sdageom_.pln[isec-1][nsc_min-1][11])
    {
      ipsc = nsc_min;
      isc  = 1;
      break;
    }
    if(xx0 <= sdageom_.pln[isec-1][nsc_max-1][13])
    {
      ipsc = nsc_max;
D 7
      isc  = 48;
E 7
I 7
      isc  = 57;
E 7
      break;
    }
    /* Hit is inside of one plane */
    if(xx0 > sdageom_.pln[isec-1][ip-1][11]
      || xx0 <= sdageom_.pln[isec-1][ip-1][13])
      continue;
    ipsc = ip;
    if(jp < 3)
    {
      dx = (sdageom_.pln[isec-1][ip-1][11] - sdageom_.pln[isec-1][ip-1][13])
          / sdageom_.pln[isec-1][ip-1][17];
      islab = (sdageom_.pln[isec-1][ip-1][11] - xx0) / dx;
      if(jp == 1) isc = islab + 1;
      if(jp == 2) isc = islab + 24;
      break;
    }
    else
    {
      if(xx0 > sdageom_.pln[isec-1][ip-1][15])
      {
        dx = (sdageom_.pln[isec-1][ip-1][11] - sdageom_.pln[isec-1][ip-1][15])
            / sdageom_.pln[isec-1][ip-1][17];
        islab = (sdageom_.pln[isec-1][ip-1][11] - xx0) / dx;
        if(jp == 3) isc  = islab + 35;
        if(jp == 4) isc  = islab + 43;
        break;
      }
      else
      {
        dx = (sdageom_.pln[isec-1][ip-1][15] - sdageom_.pln[isec-1][ip-1][13])
            / sdageom_.pln[isec-1][ip-1][18];
        islab = (sdageom_.pln[isec-1][ip-1][15] - xx0) / dx;
        if(jp == 3) isc  = islab + 40;
        if(jp == 4) isc  = islab + 47;
        break;
      }
    }
  }
uthfill(ana->histf, 91, (float)isc, 0., 1.);
  if(isc == 0) return;
uthfill(ana->histf, 92, (float)isc, 0., 1.);

  /*********************************************/
  /* Now find match of "isc" with data SC "id" */

  ihsc = 0;
  max_adc = 10;

  /* get raw data */

  GETSCDATA;

  /* match with raw data */

  if(scndig <= 0) return;
uthfill(ana->histf, 93, (float)isc, 0., 1.);
  for(i=0; i<scndig; i++)
  {
    if(ABS(scdigi[i][0] - isc) > 1) continue;
    if(scdigi[i][1] < 1 || scdigi[i][1] > TM) continue;
    if(scdigi[i][3] < 1 || scdigi[i][3] > TM) continue;
    adc = scdigi[i][2] + scdigi[i][4];
    if(adc > max_adc)
    {
      max_adc = adc;
      ihsc = i+1;
    }
  }
  if(ihsc == 0) return;
uthfill(ana->histf, 94, (float)isc, 0., 1.);
  idsc = scdigi[ihsc-1][0];


  /* No bad slab checking for data and single track simulation */

  if(sdakeys_.ifsim == 1 && sdakeys_.lanal[9] > 0)
  {
    id = idsc;
    if(sccal->bad1 > 0 || sccal->bad2 > 0) return;
  }
uthfill(ana->histf, 95, (float)isc, 0., 1.);

  *iflg = 0;
  scid->slab  = idsc;
  scid->plane = ipsc;
  scid->hit   = ihsc;
  scid->tdcl  = scdigi[ihsc-1][1];
  scid->adcl  = scdigi[ihsc-1][2];
  scid->tdcr  = scdigi[ihsc-1][3];
  scid->adcr  = scdigi[ihsc-1][4];

  return;
}



/*
   Purpose and Methods : Returns tdc time corrected for Time of Walk
                         (see NIM A432(1999)265)

   Inputs  :  ih      - hit No in the sc_digi(i,ih,1,isec) array 
D 7
              id      - SCslab No [1 - 48]
E 7
I 7
              id      - SCslab No [1 - 57]
E 7
              isec    - sector No [1 - 6]
              sccalib - SC calibration
              digi    - SC digitization

   Outputs :  twL   - left time corrected for Time of Walk
              twR   - right time corrected for Time of Walk
              tm    - avarage time corrected for Time of Walk
              iret  - return flag [1 OK, 0 failed]
   Controls:

   Calls: none

   Created    22-FEB-1998   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by ana_finde, ana_fit
*/

void
D 7
sctwalk_(int *ih, int *id, int *isec, int digi[48][5], float *twL, float *twR,
E 7
I 7
sctwalk_(int *ih, int *id, int *isec, int digi[57][5], float *twL, float *twR,
E 7
         float *tm, float *tsc, int *iret)
{
  sctwalk(*ih,*id,*isec,digi,twL,twR,tm,tsc,iret);
  return;
}

void
D 7
sctwalk(int ih, int id, int isec, int digi[48][5], float *twL, float *twR,
E 7
I 7
sctwalk(int ih, int id, int isec, int digi[57][5], float *twL, float *twR,
E 7
        float *tm, float *tsc, int *iret)
{
  float tdcL,tdcR, adcL,adcR, tL,tR, aL,aR,a0,tw0L,tw0R, w0,w1,w2;
  SCCALIB *sccal;
  sccal = (SCCALIB *) &sccalib[isec-1][id-1];

  *iret = 0;
  if(id != digi[ih-1][0]) return;

  /* get left and right timing from raw data */

  tdcL = digi[ih-1][1];
  if(tdcL < 1 || tdcL >= tdc_max) return;
  tdcR = digi[ih-1][3];
  if(tdcR < 1 || tdcR >= tdc_max) return;

  /*
    TDC calibration. A quadratic equation, which gives a good representation
    of the data is used to convert the raw TDC data to time units (ns):

      t = c0 + c1*T + c2*T*T

    where typical values of c0 ~ 1ns, c1 ~ 0.0495 ns/ch, and
    c2 ~ 5*E-08 ns/(ch*ch). The constant terms, arbitrary at this point, were
    constrained so that the average of the 64 channels of each FASTBUS card
    was zero.
  */

D 6
  tL = sccal->Ltdc0 + tdcL/sccal->Ltdc1 + sccal->Ltdc2*tdcL*tdcL;
  tR = sccal->Rtdc0 + tdcR/sccal->Rtdc1 + sccal->Rtdc2*tdcR*tdcR; 
E 6
I 6
  tL = sccal->Ltdc0 + tdcL/sccal->Ltdc1/* + sccal->Ltdc2*tdcL*tdcL*/;
  tR = sccal->Rtdc0 + tdcR/sccal->Rtdc1/* + sccal->Rtdc2*tdcR*tdcR*/; 
E 6

  /* to correct for time-walk, we perform software corrections of the form

       Tw = T - Fw((ADC-Pedestal)/Vt) + Fw(600/Vt)
  */

  a0 = Vmip/Vt; /* 600./35.*/

  /* get left and right ADC and subtract pedestals */

  adcL = digi[ih-1][2] - sccal->Lpedestal;
  adcR = digi[ih-1][4] - sccal->Rpedestal;

  /* Left timing */

  if(adcL > 0. && adcL < adc_max)
  {
    w0 = sccal->Lwalk0;
    w1 = sccal->Lwalk1;
    w2 = sccal->Lwalk2;
    aL = adcL/Vt;
    *twL = tL - Fw(aL) + Fw(a0);
  }
  else
  {
    *twL = tL;
  }

  /* Right timing */

  if(adcR > 0. && adcR < adc_max)
  {
    w0 = sccal->Rwalk0;
    w1 = sccal->Rwalk1;
    w2 = sccal->Rwalk3;
    aR = adcR/Vt;
    *twR = tR - Fw(aR) + Fw(a0);
  }
  else
  {
    *twR = tR;
  }

  /* Avaraged corrected time */

  *tm = ((*twL) + (*twR))/2.;

  /* correction on a paddle-to-paddle dependence */

  *tsc  = (*tm) + sccal->pad2pad;

  *iret = 1;

  return;
}


/*
     twalk for simulation
 */

void
scswalk_(int *jw, int *isec, int *idsc, int *ihsc, float *z, float *tPR,
         float *tFL)
{
  scswalk(jw, *isec, *idsc, *ihsc, *z, tPR, tFL);
}

void
scswalk(int *jw, int isec, int idsc, int ihsc, float z, float *tPR, float *tFL)
{
  int i, ind;
D 7
  int scndig, scdigi[48][5]; /* SC data */
E 7
I 7
  int scndig, scdigi[57][5]; /* SC data */
E 7
  SCCALIB *sccal;
  sccal = (SCCALIB *) &sccalib[isec-1][idsc-1];

  GETSCDATA;

  if( scdigi[ihsc-1][2] > scdigi[ihsc-1][4] )
  {
    *tPR = ( sdageom_.sc_hleng[isec-1][idsc-1] + z ) / sccal->velocity;
    *tFL = scdigi[ihsc-1][1] / sccal->Ltdc1 - (*tPR) - sccal->Lcabletime;
  }
  else
  {
    *tPR = ( sdageom_.sc_hleng[isec-1][idsc-1] - z ) / sccal->velocity;
    *tFL = scdigi[ihsc-1][3] / sccal->Rtdc1 - (*tPR) - sccal->Rcabletime;
  }

  return;
}



/*

   Purpose and Methods : Calculates particle velocity for track "itr"


   Input   :  itr        - track candidate number
              ana->itrk0 - track number with known "ID"
              ana->beta0 - velocity of the track "itrk0"

   Outputs :  beta    - particle velocity
              iflg = 0  means OK

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   8-MAY-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcdcam
*/

void
scbeta_(int *jw, TRevent *ana, int *itr, int *ifsim, float *beta, int *iflg,
        float *tFLe, float *Se, float *tzero)
{
  scbeta(jw, ana, *itr, *ifsim, beta, iflg, tFLe, Se, tzero);
}

void
scbeta(int *jw, TRevent *ana, int itr, int ifsim, float *beta, int *iflg,
       float *tFLe, float *Se, float *tzero)
{
  DCtrack *track, *track0;
D 7
  int scndig, scdigi[48][5]; /* SC data */
E 7
I 7
  int scndig, scdigi[57][5]; /* SC data */
E 7
  int i, ind;
  int idsc, ipsc, ihsc, isec, iret;
  float tPR, tFLm, twL, twR, tm, tsc;

  *iflg = 1;

  /* Get info for the particle with known "ID" */

  if(itr == 1)
  {
    track0 = (DCtrack *) &ana->track[ana->itrk0-1]; /* pointer to track */

    idsc = track0->scid.slab;
    ipsc = track0->scid.plane;
    ihsc = track0->scid.hit;
    isec = track0->sect;
    GETSCDATA;
    if(sdakeys_.ifsim == 1)
    {
      scswalk(jw, isec, idsc, ihsc, track0->trkp[ipsc-1].z, &tPR, tFLe);
      *Se = track0->trkp[ipsc-1].s;
    }
    else
	{
      sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
      *tzero = tsc - track0->trkp[ipsc-1].s / vflt / ana->beta0;
	}
  }

  /* Calculate particle velocity */

  if(itr == ana->itrk0)
  {
    *beta = ana->beta0;
    *iflg = 0;
  }
  else
  {
    track = (DCtrack *) &ana->track[itr-1]; /* pointer to track */

    idsc = track->scid.slab;
    ipsc = track->scid.plane;
    ihsc = track->scid.hit;
    isec = track->sect;
    GETSCDATA;
    if(sdakeys_.ifsim == 1)
    {
      scswalk(jw,isec,idsc,ihsc, track->trkp[ipsc-1].z, &tPR, &tFLm);
      *beta = track->trkp[ipsc-1].s * ana->beta0
            / (vflt * ana->beta0 * (tFLm-(*tFLe)) + (*Se));
    }
    else
    {
      sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
      tFLm  = tsc - (*tzero);         
      *beta = track->trkp[ipsc-1].s/(vflt*tFLm);
    }
    *iflg = 0;
  }

  return;
}


/*
   Purpose and Methods : get t0

   Input   :  is          - superlayer number
              itr         - track candidate number
              beta        - particle velocity

   Outputs :  t0

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   27-FEB-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by ana_dcam
*/

void
scdtime_(int *jw, TRevent *ana, int *is, int *itr, int *idsc, int *ipsc,
         int *ihsc, int *isec, float *beta, float *t0)
{
  scdtime(jw,ana,*is,*itr,*idsc,*ipsc,*ihsc,*isec,*beta,t0);
}

void
scdtime(int *jw, TRevent *ana, int is, int itr, int idsc, int ipsc,
        int ihsc, int isec, float beta, float *t0)
{
  DCtrack *track;
D 7
  int scndig, scdigi[48][5]; /* SC data */
E 7
I 7
  int scndig, scdigi[57][5]; /* SC data */
E 7
  int i, ind, iret;
  int la, il;
  float tPR, tFL, twL, twR, tm, tsc;

  /* Derive from the track hit position the slab "id"
     from the data (for 1-st SL) */

  track = (DCtrack *) &ana->track[itr-1]; /* pointer to track */

  /* ToF part. Get TRIG time "t0" */

  GETSCDATA;
  if(sdakeys_.ifsim == 1)
  {
    scswalk(jw,isec,idsc,ihsc,track->trkp[ipsc-1].z,&tPR,&tFL);
    *t0 = track->trkp[ipsc-1].s / (beta*vflt) - tFL;
  }
  else
  {
    sctwalk(ihsc,idsc,isec,scdigi,&twL,&twR,&tm,&tsc,&iret);
    *t0 = track->trkp[ipsc-1].s / (beta*vflt) - tsc;
  }

  return;
}


E 1
