h60454
s 00002/00000/01117
d D 1.6 07/11/05 18:16:54 boiarino 7 6
c implement check for layer and strip number
c 
e
s 00015/00003/01102
d D 1.5 07/11/01 16:27:08 boiarino 6 5
c CLAS_PARMS -> CLON_PARMS
c 
e
s 00002/00002/01103
d D 1.4 03/04/17 16:50:17 boiarino 5 4
c *** empty log message ***
e
s 00001/00002/01104
d D 1.3 02/04/30 14:16:05 boiarino 4 3
c cosmetic
c 
e
s 00002/00001/01104
d D 1.2 01/11/28 22:59:49 boiarino 3 1
c comment out ECPI creation
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:57:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/eclib.c
e
s 01105/00000/00000
d D 1.1 01/11/19 18:57:04 boiarino 1 0
c date and time created 01/11/19 18:57:04 by boiarino
e
u
U
f e 0
t
T
I 1

/************************************************

  eclib.c - forward calorimeter reconstruction

  BUG report: boiarino@jlab.org

  TO DO: 'gap' parameter in ecpeak() not in use -> use it !!! see ecpeakc()

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eclib.h"
#include "prlib.h"

#include "sdakeys.h"
SDAkeys sdakeys_;

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))


/* variables global for present file */

static int runnum = 0;
static float strip_threshold;
static ECPar geometry[6][3]; /* 6 sectors, 3 parts(inner,outer,whole) */

static float e0 [NSECTOR][NLAYER][NSTRIP]; /* pedestals (channels) */
static float ech[NSECTOR][NLAYER][NSTRIP];
static float t0 [NSECTOR][NLAYER][NSTRIP]; /* delays (nsec) */
static float tch[NSECTOR][NLAYER][NSTRIP];
static float atten[NSECTOR][3][3][NSTRIP]; /* NSECTOR sectors, 3 parts(inner,outer,whole), 3 views, NSTRIP strips */





/* returns EC constants from SDA calibration file */

#define NWEC 6*6*36*6

void
ecreadcalib_(int *runno, float *Ethr_ec,float cal_ec[6][6][36][6])
{
  int i;
  float data[NWEC+1], *cec;
  char *parm, fname[256];

  if((parm = (char *) getenv("CLON_PARMS")) == NULL)
  {
    printf("eclib: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }
  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.cbosfile);
  printf("eclib: reading calibration file >%s< for run # %d\n",fname,*runno);

  if(utGetFpackRecord(fname, "CEC ", NWEC+1, data))
  {
    cec = (float *) cal_ec;
    for(i=0; i<NWEC; i++) cec[i] = data[i];
    *Ethr_ec = data[NWEC];
  }

  return;
}

/* returns EC constants (ideal mostly - for simulation) */

void
ecgetcalib_(int *runno, float *Ethr_ec, float cal_ec[6][6][36][6])
{
  int isec, iv, id;

  *Ethr_ec = 1.0;              /* visible energy threshold [MeV] */
  for(isec=0; isec<6; isec++)  /* 6 sectors */
  {
    for(iv=0; iv<6; iv++)      /* iv = ui,vi,wi,uo,vo,wo */
    {
      for(id=0; id<36; id++)   /* 36 slab towers */
      {
        cal_ec[isec][iv][id][0] = 30.;  /* cable length [ns] */
        cal_ec[isec][iv][id][1] = 22.;  /* slope [cnts/ns]  */
        cal_ec[isec][iv][id][2] = 10.;  /* ADC conversion [cnts/MeV] */
        cal_ec[isec][iv][id][3] = 18.1; /* velocity of signal propagation [cm/ns] */
        cal_ec[isec][iv][id][4] = 376.; /* attenuation length [cm] */
        cal_ec[isec][iv][id][5] = e0[isec][iv][id]; /* pedestals [cnts] */
      }
    }
  }

  return;
}




/* ecinit, ecinitgeom, ecinitcalib - EC initialization */

int
ecinitgeom()
{
  int i, sec;



  /* geometry constants:

                                                          /\
                                                         /\\\
             y-axis                            y-axis   /\\\\\
               |                                 \     /\\\\\\\
               |                                  \   /\\\\\\\\\
  *=========================*                      \ /\\\\\\\\\\\
   \\          |          //                       A*\\\\\\\\\\\\\
    \\         |         //                       .  \\\\\\\\\\\\\\
     \\        |        //                            \\\\\\\\\\\\\\
      \\       |       //                      .     yhi \\\\\\\\\\\\
    ___\\ _____|_____ //_____ x-axis                    \\\\\\\\\\\\\\
        \\     |     //                     .            \\\\\\\\\\\\\\
         \\    |    //                                 .B* \\\\\\\\\\\\\
          \\   |   //                     .         .      \\\\\\\\\\\\\\
           \\  |  //                             .          \\\\\\\\\\\\\\
            \\b| //                    .      .           ylow \\\\\\\\\\\\
             \\|//                         .EcBsc_r           \\\\\\\\\\\\\\
              \+/                   .   .                      \\\\\\\\\\\\\\
               +                     .  \                      C*------------
               |                . .     EcBsc_a
 __ Beam ____________________ . ________ | ______________________________   __>


   */

  float EcBsc_r = 510.32;   /* distance from target to forward calorimeter plane (cm) */
  float EcBsc_a = 25.0;     /* angle between beam line and perpendicular to calorimeter (degree) */
  float ylow = -182.974;    /* distance from perpendicular to bottom on forward plane (cm) */
  float yhi = 189.956;      /* distance from perpendicular to top on forward plane (cm) */
  float dylow = 0.43708;    /* difference between ylow(layer+1) and ylow(layer) (cm) */
  float dyhi = 0.45419;     /* difference between yhi(layer+1) and yhi(layer) (cm) */
  float thickness = 1.2381; /* thickness of the one layer (plastic + lead) */
  float tgrho = 1.95325;    /* tg(b) */
  float surf[2] = {1.,16.};
  float ylow_, yhi_, xlow_, xhi_;
  float EcBsc_d[2], EcBsc_H[2], EcBsc_H1[2], EcBsc_H2[2];
  float ratio, depth, H1;

  /* geometry calculations */

  for(i=0; i<2; i++)
  {
    ylow_ = ylow - dylow*(surf[i]-1.);
    yhi_ = yhi + dyhi*(surf[i]-1.);
    xlow_=(ylow_-yhi_)/tgrho;
    xhi_=-xlow_;
    EcBsc_d[i] = (surf[i]-1.)*thickness;
    EcBsc_H[i] = yhi_-ylow_;
    EcBsc_H1[i] = ABS(ylow_);
    EcBsc_H2[i] = yhi_;
    for(sec=0; sec<NSECTOR; sec++)
    {
      geometry[sec][i].edge[0] = sqrt(xlow_*xlow_ + (yhi_-ylow_)*(yhi_-ylow_));
      geometry[sec][i].edge[1] = sqrt((xlow_-xhi_)*(xlow_-xhi_));
      geometry[sec][i].edge[2] = sqrt(xhi_*xhi_ + (yhi_-ylow_)*(yhi_-ylow_));

      geometry[sec][i].h  = EcBsc_H[i];
      geometry[sec][i].h1 = EcBsc_H1[i];
      geometry[sec][i].h2 = EcBsc_H2[i];
      geometry[sec][i].d  = EcBsc_d[i];

      geometry[sec][i].the = EcBsc_a*PI/180.;
      geometry[sec][i].phi = sec*2*PI/6.;

      geometry[sec][i].r   = EcBsc_r;
    }
  }

  for(sec=0; sec<NSECTOR; sec++)
  {
    for(i=0; i<2; i++)
    {
      geometry[sec][i].a   = 25.0;   /* degrees for sector one */
      geometry[sec][i].v   = 50.098; /* closest vertical distance to beam */
    }

    ratio = ( geometry[sec][1].edge[0] / geometry[sec][0].edge[0] +
              geometry[sec][1].edge[1] / geometry[sec][0].edge[1] )/2.;
    depth = geometry[sec][0].r * (ratio - 1);
    H1 = (geometry[sec][0].r*sin(geometry[sec][0].a*PI/180.) - geometry[sec][0].v) /
            cos(geometry[sec][0].a*PI/180.);
    geometry[sec][0].H1 = H1;         /* inner */
    geometry[sec][1].H1 = ratio * H1; /* outer */
  }

  /* copy inner part geometry to whole calorimeter geometry */

  for(sec=0; sec<NSECTOR; sec++)
  {
    bcopy((char *)&geometry[sec][0].r, (char *)&geometry[sec][2].r, sizeof(ECPar));
  }

  return(0);
}


int ecinitcalib(int runnum, int def_adc, int def_tdc, int def_atten);
int
ecinitcalib_(int *runnum, int *def_adc, int *def_tdc, int *def_atten)
{
  ecinitcalib(*runnum, *def_adc, *def_tdc, *def_atten);
}
int
ecinitcalib(int runnum, int def_adc, int def_tdc, int def_atten)
{
I 6
  FILE *fd;
E 6
  float values[NSTRIP*NSECTOR];
  int i,j,k,l,firsttime;
  char map[1000], *dir;
  char *item[6] = {"InnerU","InnerV","InnerW","OuterU","OuterV","OuterW"};
  char *item9[9] = {"InnerU","InnerV","InnerW",
                    "OuterU","OuterV","OuterW",
                    "WholeU","WholeV","WholeW"};

D 6
  if((dir = (char *)getenv("CLAS_PARMS")) == NULL)
E 6
I 6
  if((dir = (char *)getenv("CLON_PARMS")) == NULL)
E 6
  {
D 6
    printf("eclib: CLAS_PARMS not defined !!!\n");
E 6
I 6
    printf("eclib: CLON_PARMS not defined !!!\n");
E 6
    exit(0);
  }

  sprintf(map,"%s/Maps/%s.map",dir,"EC_CALIB");
D 6
  printf("eclib: reading file %s\n",map);
E 6
I 6
  if((fd=fopen(map,"r")) != NULL)
  {
    printf("eclib: reading calibration map >%s< for run # %d\n",map,runnum);
    fclose(fd);
  }
  else
  {
    printf("eclib: error opening map file >%s< - exit\n",map);
    exit(0);
  }


E 6
D 5
  printf("eclib: def_adc=%d     def_tdc=%d\n",def_adc,def_tdc);
E 5
I 5
  printf("eclib: def_adc=%d    def_tdc=%d\n",def_adc,def_tdc);fflush(stdout);
E 5

  for(i=0; i<NSECTOR; i++)
  {
    for(j=0; j<NLAYER; j++)
    {
      for(k=0; k<NSTRIP; k++)
      {
        e0 [i][j][k] = 0.0;
        ech[i][j][k] = 0.0001;
        t0 [i][j][k] = 0.0;
        tch[i][j][k] = 0.05;
      }
    }
  }

  if(!def_adc)
  {
    for(j=0; j<NLAYER; j++)
    {
      map_get_float(map,"EC_PEDESTALS",item[j],
                    NSTRIP*NSECTOR,values,runnum,&firsttime);
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
        {
          if(values[NSTRIP*i+k] != 0) e0[i][j][k] = values[NSTRIP*i+k];
        }
      }

      map_get_float(map,"EC_GAIN",item[j],
                    NSTRIP*NSECTOR,values,runnum,&firsttime);
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
        {
          if(values[NSTRIP*i+k] != 0) ech[i][j][k] = values[NSTRIP*i+k];
        }
      }

    }
  }

  if(!def_tdc)
  {
    for(j=0; j<NLAYER; j++)
    {
       map_get_float(map,"EC_To",item[j],
                     NSTRIP*NSECTOR,values,runnum,&firsttime);
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
        {
          if(values[NSTRIP*i+k] != 0) t0[i][j][k] = values[NSTRIP*i+k];
        }
      }

      map_get_float(map,"EC_Tch",item[j],
                    NSTRIP*NSECTOR,values,runnum,&firsttime);
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
        {
          if(values[NSTRIP*i+k] != 0) tch[i][j][k] = values[NSTRIP*i+k];
        } 
      }
    }
  }

D 5
  printf("eclib: def_atten=%d\n",def_atten);
E 5
I 5
  printf("eclib: def_atten =%d\n",def_atten);fflush(stdout);
E 5
  for(j=0; j<3; j++)
  {
    for(l=0; l<3; l++)
    {
      for(i=0; i<NSECTOR; i++)
      {
        for(k=0; k<NSTRIP; k++)
        {
          atten[i][j][l][k] = 376.0;
        }
      }
    }
  }

  if(!def_atten)
  {
    for(j=0; j<3; j++) /* inner,outer,whole */
    {
      for(l=0; l<3; l++) /* u,v,w */
      {
        map_get_float(map,"EC_ATTEN",item9[j*3+l],
                      NSTRIP*NSECTOR,values,runnum,&firsttime);
        for(i=0; i<NSECTOR; i++)
        {
          for(k=0; k<NSTRIP; k++)
          {
            if(values[NSTRIP*i+k] != 0) atten[i][j][l][k] = values[NSTRIP*i+k];
          }
        }
      }
    }
  }


  return(0);
}

int ecinit(int runnum, int def_adc, int def_tdc, int def_atten);
int
ecinit_(int *runnum, int *def_adc, int *def_tdc, int *def_atten)
{
  ecinit(*runnum,*def_adc,*def_tdc,*def_atten);
  return(0);
}
int
ecinit(int runnum, int def_adc, int def_tdc, int def_atten)
{
  ecinitgeom();
  ecinitcalib(runnum,def_adc,def_tdc,def_atten);
  return(0);
}


/* ecstrips.c - loading strip info for particular sector

  input:  sector - sector number

  output: strip[][].energy - strip energy (MeV)
          strip[][].time   - strip time (ns)
*/


int
ecstrips(int *jw, float threshold, int sec, ECStrip strip[NLAYER][NSTRIP])
{
  ECDataPtr ec;
  int i, j, k, ind, nhits, str, layer, error, ii;
  float energy;

  ind = etNlink(jw,"EC  ",sec+1);
  if(ind <= 0) return(-1);

  strip_threshold = threshold;
  bzero((char *)strip,NLAYER*NSTRIP*sizeof(ECStrip));

  nhits = (etNdata(jw,ind)*sizeof(int))/sizeof(ECData);
  ec = (ECDataPtr)&jw[ind];

  for(i=0; i<nhits; i++)
  {
    str   = ec->strip-1;
    layer = ec->layer-1;
I 7
    if(str<0 || str>=NSTRIP) continue; /*check strip number range*/
    if(layer<0 || layer>=NLAYER) continue; /*check layer number range*/
E 7

    if((energy = (ec->adc - e0[sec][layer][str]) * ech[sec][layer][str]) >= strip_threshold)
    {
      strip[layer][str].energy = energy;
/*
printf("ecstrips: sec,layer,str,energy=%d %d %d %d(%f)\n",sec+1,layer+1,str+1,ec->adc,energy);
*/
      strip[layer][str].time =
        ec->tdc * tch[sec][layer][str] - t0[sec][layer][str] ;
    }

    ec++;
  }

  return(ind);
}


/* ecstacks.c - summing inner and outer parts

*/

void
ecstacks(ECStrip strip[NLAYER][NSTRIP], ECStrip stack[3][NSTRIP])
{
  int str,lay;

  for(lay=0; lay<3; lay++)
  {
    for(str=0; str<NSTRIP; str++)
    {
      stack[lay][str].energy = strip[lay][str].energy + strip[lay+3][str].energy;
/*
if(stack[lay][str].energy >= strip_threshold)
printf("ecstacks: layer,str(from 0),strip,strip,stack=%d %d %f %f %f\n",lay+1,str,strip[lay][str].energy,strip[lay+3][str].energy,stack[lay][str].energy);
*/
    }
  }

  return;
}


/* ecpeak.c - looking for peaks in particular layer

  input:  strip[].energy - strip energy (MeV)
          strip[].time   - strip time (ns)
          threshold      - 
          gap            - 
          min            - 
          max            - 

  output: npeak  - the number of peaks obtained
          peak   - peaks information
*/

static int
peak_compare(ECPeak *i, ECPeak *j)
{
  if (i->energy < j->energy) return (1);
  if (i->energy > j->energy) return (-1);
  return (0);
}

int
ecpeak(float threshold, int gap, int min, int max, ECStrip strip[36], ECPeak peak[18])
{
  int i, strip1, nstrip, npeak;
  float str, sumadc, sumtdc, rstrip;

  bzero((char *)peak, 18*sizeof(ECPeak));

  npeak  = -1;
  strip1 = 0;
  nstrip = 0;
  str  = 0.0;
  sumadc = 0.0;
  sumtdc = 0.0;
  rstrip = 0.0;

  if(min <  0) min = 0;
  if(max > 35) max = 35;
  if(min > max) {/*printf("ecpeak: ERROR: min=%d > max=%d\n",min,max);*/ return(-1);}

  /*printf("ecpeak: min=%d max=%d\n",min,max);*/
  for(i=min; i<=max; i++)
  {
    if(strip[i].energy > strip_threshold)
    {
      /*printf("ecpeak: strip[%2d].energy=%f\n",i,strip[i].energy);*/
      if(strip1 == 0) strip1 = i;
      nstrip++;
      sumadc = sumadc + strip[i].energy;
      str  = str + (i+0.5)*strip[i].energy;
      rstrip = rstrip + (i+0.5)*(i+0.5)*strip[i].energy;
      sumtdc = sumtdc + strip[i].time;
      /*printf("ecpeak: before if -> %d %d %f %f\n",i,NSTRIP-1,strip[i+1].energy,strip_threshold);*/
      if(i >= max || strip[i+1].energy <= strip_threshold)
      {
        npeak++;
        str = str/sumadc;

        peak[npeak].strip1 = strip1;
        peak[npeak].stripn = nstrip;
        peak[npeak].coord  = str;
        peak[npeak].energy = sumadc;
        peak[npeak].time   = sumtdc/nstrip;

        rstrip = rstrip/sumadc - str*str;
        rstrip = (rstrip > (1./12.)) ? rstrip : (1./12.);
        peak[npeak].width = sqrt(rstrip);
        strip1 = 0;
        nstrip = 0;
        str  = 0.0;
        sumadc = 0.0;
        sumtdc = 0.0;
        rstrip = 0.0;
      }
    }
  }
  npeak++;
  /*printf("ecpeak: found %d peaks\n",npeak);*/

  /* sorting output array in decreasing energy order */

  qsort((void *)peak, npeak, sizeof(ECPeak), (int (*) (const void *, const void *))peak_compare);
  npeak = 0;
  while(peak[npeak].energy > threshold) npeak++;
  /*printf("ecpeak: finaly %d peaks (threshold=%f)\n",npeak,threshold);*/

  return(npeak);
}


/*
  ecpeakb - get one peak using already reconstructed peak
*/

int
ecpeakb(float threshold, ECStrip strip[36], ECPeak peak1[18], ECPeak peak[18])
{
  int i,igap,strip1,nstrip,npeak,min,max,start;
  float str,sumadc,sumtdc,rstrip;

  bzero((char *)peak, 18*sizeof(ECPeak));

  /* min & max strips */

  min = peak1[0].strip1;
  max = peak1[0].strip1 + peak1[0].stripn - 1;

  /*printf("ecpeakb: max=%d min=%d\n",max,min);*/

  npeak  = 0;
  strip1 = min;
  nstrip = max-min+1;
  str  = 0.0;
  sumadc = 0.0;
  sumtdc = 0.0;
  rstrip = 0.0;

  for(i=min; i<=max; i++)
  {
    sumadc += strip[i].energy;
    str    += (i+0.5)*strip[i].energy;
    rstrip += (i+0.5)*(i+0.5)*strip[i].energy;
    sumtdc += strip[i].time;
  }

  str = str/sumadc;

  peak[npeak].strip1 = strip1;
  peak[npeak].stripn = nstrip;
  peak[npeak].coord  = str;
  peak[npeak].energy = sumadc;
  peak[npeak].time   = sumtdc/nstrip;

  rstrip = rstrip/sumadc - str*str;
  rstrip = (rstrip > (1./12.)) ? rstrip : (1./12.);
  peak[npeak].width = sqrt(rstrip);

  if(peak[npeak].energy > threshold) npeak = 1;

  return(npeak);
}


/* echit - looking for hits in particular part ( inner , outer, whole )

  input:  geom   - geometry parameters
          npeak  - the number of peaks obtained
          peak   - peaks information

  output: hit    - hits information
*/

int
echit(int io, int sector, int npeak[3], ECPeak peak[3][18], ECHit hit[NHIT])
{
  int i, id, u, v, w, npsble, edge;
  float spread,sum_edge,dltz,rmsi,rmsj,rms2,lu,lv,lw,aa,h,h1,h2;
  float iuv,ivw,iwu,juv,jvw,jwu;
  ECParPtr geom;

  geom = &geometry[sector][io]; /* set geometry pointer */

/*
printf("geom: %d %d %f %f %f\n",io+1,sector+1,geom->edge[0],geom->edge[1],geom->edge[2]);
*/


  /* number->cm for coord and width */

  for(edge=0; edge<3; edge++)
  {
    for(id=0; id<npeak[edge]; id++)
    {
      peak[edge][id].coord = peak[edge][id].coord * geom->edge[edge] / 36.;
      peak[edge][id].width = peak[edge][id].width * geom->edge[edge] / 36.;
      peak[edge][id].tmp = 0.0;
    }
  }

  npsble = -1;

  for(u=0; u<npeak[0]; u++)
  {
    for(v=0; v<npeak[1]; v++)
    {
      for(w=0; w<npeak[2]; w++)
      {

        dltz = peak[0][u].coord/geom->edge[0] +
               peak[1][v].coord/geom->edge[1] +
               peak[2][w].coord/geom->edge[2];

        spread = ( peak[0][u].width +
                   peak[1][v].width +
                   peak[2][w].width ) *2.*sqrt(12.);

        sum_edge = geom->edge[0] + geom->edge[1] + geom->edge[2];

        if(ABS(dltz-2.)*sum_edge < spread)
        {
          npsble++;
          if(npsble == NHIT) return(-1);

          if(npsble == 20) /* temporary for compartibility !!! */
          {
            /*printf("eclib: too many possible hits\n");*/
            return(-1);
          }

          hit[npsble].sector = sector + 1; /* in hit[] sector = 1..6 */
          hit[npsble].layer = io + 1;      /* in hit[] io = 1..2 */

          hit[npsble].i = geom->h *
                          ( peak[0][u].coord/geom->edge[0] -
                            peak[1][v].coord/geom->edge[1] -
                            peak[2][w].coord/geom->edge[2] ) / 2. +
                          geom->h2;

          hit[npsble].j = geom->edge[1] *
                          ( peak[2][w].coord/geom->edge[2] -
                            peak[1][v].coord/geom->edge[1] ) / 2.;

          hit[npsble].k = geom->d;


          hit[npsble].di = sqrt( peak[0][u].width * peak[0][u].width / (geom->edge[0] * geom->edge[0]) +
                                 peak[1][v].width * peak[1][v].width / (geom->edge[1] * geom->edge[1]) +
                                 peak[2][w].width * peak[2][w].width / (geom->edge[2] * geom->edge[2]) ) *
                           geom->h / 2.;
          hit[npsble].dj = sqrt( peak[2][w].width * peak[2][w].width / (geom->edge[2] * geom->edge[2]) +
                                 peak[1][v].width * peak[1][v].width / (geom->edge[1] * geom->edge[1]) ) *
                           geom->edge[1] / 2.;
          /*hit[npsble].width = sqrt( hit[npsble].di*hit[npsble].di + hit[npsble].dj*hit[npsble].dj );*/


/* Serguei's
          lu = geom->edge[0];
          lv = geom->edge[1];
          lw = geom->edge[2];
	  h = sqrt(lu*lu-lv*lv/4.);
          h1 = geom->H1;
          h2 = h - h1;
          aa = lv/2.;
          iuv = h*(peak[0][u].coord/lu - h1/h);
          juv = aa*(2.-peak[0][u].coord/lu - 2.*peak[1][v].coord/lv);
          ivw = h*(1.+h2/h-peak[2][w].coord/lw-peak[1][v].coord/lv);
          jvw = aa*(peak[2][w].coord/lw-peak[1][v].coord/lv);
          iwu = iuv;
          jwu = aa*(peak[0][u].coord/lu-2.+2.*peak[2][w].coord/lw);
          rmsi = ((iuv-hit[npsble].i)*(iuv-hit[npsble].i) +
                  (ivw-hit[npsble].i)*(ivw-hit[npsble].i) +
                  (iwu-hit[npsble].i)*(iwu-hit[npsble].i))/3.;
          rmsj = ((juv-hit[npsble].j)*(juv-hit[npsble].j) +
                  (jvw-hit[npsble].j)*(jvw-hit[npsble].j) +
                  (jwu-hit[npsble].j)*(jwu-hit[npsble].j))/3.;
          rms2 = rmsi + rmsj;
          hit[npsble].di = sqrt(rmsi);
          hit[npsble].dj = sqrt(rmsj);
          hit[npsble].width = sqrt(rms2);
*/


          hit[npsble].energy = peak[0][u].energy +
                               peak[1][v].energy +
                               peak[2][w].energy;
          hit[npsble].peak1[0] = u;
          hit[npsble].peak1[1] = v;
          hit[npsble].peak1[2] = w;
          hit[npsble].peakn[0] = 1; /* temporary one peak per layer only */
          hit[npsble].peakn[1] = 1;
          hit[npsble].peakn[2] = 1;
          peak[0][u].tmp = peak[0][u].tmp + hit[npsble].energy;
          peak[1][v].tmp = peak[1][v].tmp + hit[npsble].energy;
          peak[2][w].tmp = peak[2][w].tmp + hit[npsble].energy;
/*
printf("echit: sec=%d io=%d ihit=%d iu=%4.1f iv=%4.1f iw=%4.1f energy=%5.2f\n",sector+1,io+1,npsble+1,
        peak[0][u].coord,peak[1][v].coord,peak[2][w].coord,hit[npsble].energy);
*/
        }
      }
    }
  }

  return(npsble+1);
}


/* eccorr.c - attenuation correction

  input:  geom   - geometry parameters
          npeak  - the number of peaks obtained
          peak   - peaks information

  output: hit    - hits information
*/

static int
hit_compare(ECHit *i, ECHit *j)
{
  if (i->energy < j->energy) return (1);
  if (i->energy > j->energy) return (-1);
  return (0);
}

int
eccorr(float th, int io, int sector, int npeak[3], ECPeak peak[3][18], int npsble, ECHit hit[NHIT])
{
  ECParPtr geom;
  int i, j, k, l, ith, axis, ihit, peakid, npks, nhit;
  float energy, attn, lat, path[3], fraction[3], *atlen[3];

  geom = &geometry[sector][io]; /* set geometry pointer */

  for(l=0; l<3; l++) atlen[l] = atten[sector][io][l];

  /* sorting output array in decreasing energy order */

  qsort((void *)hit, npsble, sizeof(ECHit), (int (*) (const void *, const void *))hit_compare);
  nhit = 0;
  while(hit[nhit].energy > th && nhit < NHIT && nhit < npsble) nhit++;

  /* loop for all hits */

  for(ihit=0; ihit<nhit; ihit++)
  {
    ecpath(geom,hit[ihit].i,hit[ihit].j,path);
/*
printf("2: I,J - U,V,W=%f %f %f %f %f\n",hit[ihit].i,hit[ihit].j,path[0],path[1],path[2]);
*/
    energy = hit[ihit].energy;
    hit[ihit].energy = 0.;
    hit[ihit].time   = 0.;
    hit[ihit].width  = 0.;

    /* loop for 3 axis */

    npks = 0;
    for(axis=0; axis<3; axis++)
    {
      npks++;
      peakid = hit[ihit].peak1[axis];
      fraction[axis] = energy / peak[axis][peakid].tmp;

      lat = 0.;
      for(ith=0; ith<peak[axis][peakid].stripn; ith++)
      {
        lat = lat + atlen[axis][peak[axis][peakid].strip1+ith];
      }
      lat = lat / peak[axis][peakid].stripn;
      attn = exp(-path[axis]/lat);
/*
printf("3: axis,lat,dst=%d %f %f\n",axis+1,lat,path[axis]);
*/
      hit[ihit].energy = hit[ihit].energy + fraction[axis] *
                         peak[axis][peakid].energy / attn;
      hit[ihit].time = hit[ihit].time + peak[axis][peakid].time;

      hit[ihit].uvw2[axis] = peak[axis][peakid].width;
      hit[ihit].width = hit[ihit].width + peak[axis][peakid].width;
    }
    hit[ihit].width = hit[ihit].width / 3;
/*
printf("4: ecfit_peaks_hits (%d)l,s,energy,width=%d %d %f %f\n",ihit+1,io+1,sector+1,hit[ihit].energy,hit[ihit].width);
*/
    npks = MAX(npks,1);
    hit[ihit].time = hit[ihit].time / npks;

    ecxyz(geom,hit[ihit].i,hit[ihit].j,hit[ihit].k,&hit[ihit].x);
/*
printf("5: ecfit_peaks_hits (%d)   l,s,i,j,k,x,y,z=%d %d %f %f %f %f %f %f\n",ihit+1,io+1,sector+1,
                                 hit[ihit].i,hit[ihit].j,hit[ihit].k,hit[ihit].x,hit[ihit].y,hit[ihit].z);
*/
    /*
printf("eccorr: energy=%f\n",hit[0].energy);
    */
  }

  return(nhit);
}



/******************************************************************
         forward calorimeter reconstruction main functions
  *****************************************************************/


/* eclib - get shower using prediction for UVW position */

#define NU 2 /* area to search U peak */
#define NV 8 /* area to search V peak */
#define NW 8 /* area to search W peak */

int
eclib(int *jw, const float threshold[3], int *ntrk, PRTRACK *track)
{
  int nsec, i, j, itr, itmp, uvw, iu, iv, iw, io, io_, npsble, gap, center[3], nhit, npeak[3][3];
  float x, y, tmp1, tmp2, tmp2e, en1, en2;
  ECStrip strip[2][3][36], stack[3][36];
  ECPeak peak[3][3][18];
  PRTRACK *trk;
  ECHit hit[1000];
  ECHitPtr hitin;
  ECParPtr geom;

  nhit = 0;
  gap = 0;
  trk = track - 1;

  /*printf("\neclib: reached, *ntrk=%d trk=%08x\n",*ntrk,trk);*/
  for(itr=0; itr<(*ntrk); itr++)
  {
    trk++;
    /*printf("eclib: ntrack=%d u=%d v=%d w=%d\n",itr,trk->nu, trk->nv, trk->nw);*/

    /* clean up */
    trk->ec[0].u = trk->ec[1].u = trk->ec[2].u = 0.0;
    trk->ec[0].v = trk->ec[1].v = trk->ec[2].v = 0.0;
    trk->ec[0].w = trk->ec[1].w = trk->ec[2].w = 0.0;
    trk->ec[0].e = trk->ec[1].e = trk->ec[2].e = 0.0;

    if(trk->nu==0 || trk->nv==0 || trk->nw==0) continue;
    /*printf("eclib: prediction: u=%4.1f v=%4.1f w=%4.1f\n",trk->nu/2.,trk->nv/2.,trk->nw/2.);*/
    nsec = trk->sector - 1;
    itmp = ecstrips(jw, threshold[0], nsec, strip);
    if(itmp > 0)
    {
      /* first sum inner and outer parts and found cluster */
      io_ = 2;
      ecstacks(strip, stack);
      center[0] = (int)(trk->nu/2);
      center[1] = (int)(trk->nv/2);
      center[2] = (int)(trk->nw/2);

      /* look for the peak in 'U' layer;
         have the best prediction accuracy here - comparable then EC resolution */
      npeak[io_][0]=ecpeak(threshold[1],gap,center[0]-NU,center[0]+NU,&stack[0][0],&peak[io_][0][0]);
      /* looks for peaks in 'V' and 'W' layers;
         have not so good prediction accuracy here - PHI angle is not defined as good as THETA */
      npeak[io_][1]=ecpeak(threshold[1],gap,center[1]-NV,center[1]+NV,&stack[1][0],&peak[io_][1][0]);
      npeak[io_][2]=ecpeak(threshold[1],gap,center[2]-NW,center[2]+NW,&stack[2][0],&peak[io_][2][0]);

      /*printf("eclib: npeak[%d] -> %d %d %d\n",io_,npeak[io_][0],npeak[io_][1],npeak[io_][2]);*/
      if(npeak[io_][0]==0 || npeak[io_][1]==0 || npeak[io_][2]==0) continue;

      /* */
      if( (npsble = echit(io_,nsec,&npeak[io_][0],&peak[io_][0][0],&hit[nhit])) > 0)
      {
        itmp = 0;
        nhit += itmp=eccorr(threshold[1],io_,nsec,&npeak[io_][0],&peak[io_][0][0],npsble,&hit[nhit]);
        /*printf("eclib: npeak= %d %d %d\n",npeak[io_][0],npeak[io_][1],npeak[io_][2]);*/

        /* inner and outer parts separately */
        if(itmp)
        {
          for(io=0; io<2; io++)
          {
            for(uvw=0; uvw<3; uvw++)
            {
              npeak[io][uvw]=ecpeakb(threshold[1],&strip[io][uvw][0],&peak[io_][uvw][0],&peak[io][uvw][0]);
            }
            if( (npsble = echit(io,nsec,&npeak[io][0],&peak[io][0][0],&hit[nhit])) == 1)
            {
              nhit += eccorr(threshold[1],io,nsec,&npeak[io][0],&peak[io][0][0],npsble,&hit[nhit]);
            }
          }
        }

        /*printf("nhit=%d\n",nhit);*/

      }
      /*else printf("eclib: npsble=%d\n",npsble);*/
    }

    /* fill output info */

    hitin = hit;
    for(i=0; i<nhit; i++)
    {
      io = hitin->layer - 1;
      geom = &geometry[hitin->sector-1][io];
      /*
      printf("eclib: sector=%d io=%d u=%4.1f v=%4.1f w=%4.1f energy=%5.2f\n",hitin->sector,io,peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0],
               peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1],peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2],hitin->energy);
      */
      trk->ec[io].u = peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0];
      trk->ec[io].v = peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1];
      trk->ec[io].w = peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2];
      trk->ec[io].e = hitin->energy/0.275;

      hitin++;
    }

  }

D 3
  /* fill ECPI bank */
E 3
I 3
D 4
  /* fill ECPI bank 
E 4
I 4
  /* fill ECPI bank */
E 4
E 3
  ec_makeECPI(jw,nhit,&hit);
I 3
D 4
  */
E 4
E 3

  return(nhit);
}



/* search for shower without coordinate predictions */
/* option = 0 - combine inner and outer parts
          = 1 - do both parts separately
   returns: bits 0-5 corresponds to sectors 1-6 with shower
            if returns zero - no one shower were found
*/

int
ecl3(int *jw, const float threshold[3], const int option)
{
  int ret, sec, uvw, gap, io, npsble, nhit[6];
  ECStrip strip[2][3][36], stack[3][36];
  int npeak[3][3], npeak1[3];
  ECPeak peak[3][3][18];
  ECPeak peak1[3][18];
  ECHit hit[1000];

  ret = 0;
  for(sec=0; sec<6; sec++) nhit[sec] = 0;
  gap = 0; /* make it option ? */
  if(option == 1)
  {
    /* looking for clusters separately in inner and outer parts */
    for(sec=0; sec<NSECTOR; sec++)
    {
      if(ecstrips(jw, threshold[0], sec, strip) > 0)
      {

        for(io=0; io<2; io++)
        {
          for(uvw=0; uvw<3; uvw++)
          {
            npeak[io][uvw] = ecpeak(threshold[1], gap, 0, 35, &strip[io][uvw][0], &peak[io][uvw][0]);
          }
          if( (npsble = echit(io,sec,&npeak[io][0],&peak[io][0][0],&hit[nhit[sec]])) > 0)
          {
            nhit[sec] +=
              eccorr(threshold[2],io,sec,&npeak[io][0],&peak[io][0][0],npsble,&hit[nhit[sec]]);
          }
          else
          {
            ;
          }
        }

      }
    }
  }
  else
  {
    /* looking for cluster summing energy in both parts */
    for(sec=0; sec<NSECTOR; sec++)
    {
      if(ecstrips(jw, threshold[0], sec, strip) > 0)
      {
        ecstacks(strip, stack);
        for(uvw=0; uvw<3; uvw++)
        {
          npeak1[uvw] = ecpeak(threshold[1], gap, 0, 35, &stack[uvw][0], &peak1[uvw][0]);
        }
        io = 2;
        if( (npsble = echit(io,sec,&npeak1[0],peak1,&hit[nhit[sec]])) > 0)
        {
          nhit[sec] += eccorr(threshold[2],io,sec,&npeak1[0],peak1,npsble,&hit[nhit[sec]]);
        }
        else
        {
          ;
        }
      }
    }
  }

  for(sec=0; sec<6; sec++) if(nhit[sec]) ret |= 1<<sec;
  /*
  printf("ret=0x%08x nhit=%d %d %d %d %d %d\n",
    ret,nhit[0],nhit[1],nhit[2],nhit[3],nhit[4],nhit[5]);
  */

  return(ret);
}



/*********************************************************
                some useful utilities
**********************************************************/


/* ecpath.c

  di - I coordinate
  dj - J coordinate

 */

int
ecpath(ECParPtr geom, float di, float dj, float path[3])
{
  float lu, lv, lw, h, h1, du, dv, dw;

  lu = geom->edge[0];
  lv = geom->edge[1];
  lw = geom->edge[2];
  h = sqrt(lu*lu-lv*lv/4.);
  h1 = geom->h1;

  du = (di/h + h1/h)*lu;
  dv = lv - h1/2./h*lv - dj - lv/2./h*di;
  dw = lw/lv*dj - lw/2./h*di - h1/2./h*lw + lw;

  path[0] = du/lu*lv - (lv-dv);
  path[1] = dv/lv*lw - (lw-dw);
  path[2] = dw/lw*lu - (lu-du);

  return(0);
}


/* ecxyz.c

  *di - I coordinate
  *dj - J coordinate
  *dk - K coordinate

 */

int
ecxyz(ECParPtr geom, float di, float dj, float dk, float xyz[3])
{
  int j;
  float phi, the, pv[3];
  /*static*/ float rot[3][3];

  the = geom->the;
  phi = geom->phi;

  rot[0][0] = cos(the) * cos(phi);
  rot[0][1] = -sin(phi);
  rot[0][2] = sin(the) * cos(phi);
  rot[1][0] = cos(the) * sin(phi);
  rot[1][1] = cos(phi);
  rot[1][2] = sin(the) * sin(phi);
  rot[2][0] = -sin(the);
  rot[2][1] = 0.;
  rot[2][2] = cos(the);

  for(j=0; j<3; j++) pv[j] = 0.;
  for(j=0; j<3; j++)
  {
    pv[j] = pv[j] + rot[j][0]*di;
    pv[j] = pv[j] + rot[j][1]*dj;
    pv[j] = pv[j] + rot[j][2]*dk;
  }

  xyz[0] = pv[0] + geom->r * sin(the) * cos(phi);
  xyz[1] = pv[1] + geom->r * sin(the) * sin(phi);
  xyz[2] = pv[2] + geom->r * cos(the);
 
/* note for reverse xyz->ijk
 *     do j=1,3
 *       do i=1,3
 *         pv(j)=rot(i,j)*P(i)
 *       enddo
 *     enddo
 * since this is a unitary matrix
 */

  return(0);
}




E 1
