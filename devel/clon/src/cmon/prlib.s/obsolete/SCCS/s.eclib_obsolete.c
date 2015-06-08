h08145
s 00000/00000/00000
d R 1.2 01/11/19 18:57:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/obsolete/eclib_obsolete.c
e
s 01264/00000/00000
d D 1.1 01/11/19 18:57:34 boiarino 1 0
c date and time created 01/11/19 18:57:34 by boiarino
e
u
U
f e 0
t
T
I 1

/**************************************************************/
/********** O B S O L E T E   FUNCTIONS FROM eclib.c **********/
/**************************************************************/

/*
  ecpeakc - get one peak using 'center' prediction

  returns 1 if found 1 peak or 0 if found nothing
  NOTE: only one peak can be reported !
*/

#define NHALF 8 /* the number of strips in every side to be scan */

int
ecpeakc(float threshold, int gap, int center, ECStrip strip[36], ECPeak peak[18])
{
  int i,igap,strip1,nstrip,npeak,min,max,start;
  float str,sumadc,sumtdc,rstrip;

  bzero((char *)peak, 18*sizeof(ECPeak));



  /* look for maximum amplitude strip */
  str = 0.0;
  for(i=center-NHALF; i<center+NHALF; i++)
  {
    printf("new center: [%2d]=%f\n",i,strip[i].energy);
    if(strip[i].energy > str)
    {
      str = strip[i].energy;
      nstrip = i;
    }
  }
  if(strip[nstrip].energy > strip_threshold) center = nstrip;
  printf("new center = %d\n",center);




  /* min & max strips */

  gap = 1;

  min = MAX(0,(center-NHALF));
  printf("min strip=%d (center=%d)\n",min,center);
  igap = 0;
  start = i = center;
  while(strip[i].energy <= strip_threshold && i>= min)
  {
    printf("skip strip %d (energy=%f <= threshold=%f)\n",i,strip[i].energy,strip_threshold);
    i--;
  }
  if(i > min)
  {
    start = i;
    printf("start strip=%d\n",i);
    for(i=start; i>=min; i--)
    {
      if(strip[i].energy <= strip_threshold)
      {
        if(igap >= gap)
        {
          printf("strip2[%2d].energy=%f\n",i,strip[i].energy);
          break;
        }
        else
        {
          igap ++;
          printf("igap=%d\n",igap);
        }
      }
      else
      {
        printf("strip1[%2d].energy=%f\n",i,strip[i].energy);
        igap = 0;
      }
    }
    min = i+igap+1;
  }
  else
  {
    min = -1;
  }


  max = MIN(35,(center+NHALF));
  printf("max strip=%d (center=%d)\n",min,center);
  igap = 0;
  start = i = center;
  while(strip[i].energy <= strip_threshold && i<= max)
  {
    printf("skip strip %d (energy=%f <= threshold=%f)\n",i,strip[i].energy,strip_threshold);
    i++;
  }
  if(i < max)
  {
    start = i;
    printf("start strip=%d\n",i);
    for(i=start; i<=max; i++)
    {
      if(strip[i].energy <= strip_threshold)
      {
        if(igap >= gap)
        {
          printf("strip2[%2d].energy=%f\n",i,strip[i].energy);
          break;
        }
        else
        {
          igap ++;
          printf("igap=%d\n",igap);
        }
      }
      else
      {
        printf("strip1[%2d].energy=%f\n",i,strip[i].energy);
        igap = 0;
      }
    }
    max = i-igap-1;
    printf("i=%d igap=%d max=%d\n",i,igap,max);
  }
  else
  {
    max = -1;
  }


  if(min == -1 && max == -1)
  {
    printf("ecpeakc: no EC cluster\n");
    return(0);
  }
  else if(min == -1)
  {
    i = center;
    while(strip[i].energy <= strip_threshold && i<min) {printf("skip .....+\n"); i++;}
    min = i;
  }
  else if(max == -1)
  {
    i = center;
    while(strip[i].energy <= strip_threshold && i>max) {printf("skip .....-\n"); i--;}
    max = i;
  }
  printf("ecpeakc: min=%d center=%d max=%d\n",min,center,max);



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
  printf("ecpeakc: coord=%f energy=%f npeak=%d\n",peak[0].coord,peak[0].energy,npeak);

  return(npeak);
}


#define ntrmx    500 /* temporary from prlib.c !!!!!!!!!!!!!!!!!!!! */
#define nlnkvect 16  /* temporary from prlib.c !!!!!!!!!!!!!!!!!!!! */
#define nlnkec   12  /* temporary from prlib.c !!!!!!!!!!!!!!!!!!!! */


/*
   ecoutput - fill some output arrays

      sector = [1..6]
*/

int
ecoutput(int itr, int sector, ECPeak peak[3][3][18], int nhits, ECHitPtr hit,
         float lnk_ec[ntrmx][nlnkec])
{
  ECHitPtr hitin, hitout;
  int i, j, io;
  ECParPtr geom;

  hitin = hit;
  for(i=0; i<nhits; i++)
  {
    if(sector == hitin->sector)
    {
      io = hitin->layer - 1;
      geom = &geometry[hitin->sector-1][io];
      /*
      printf("ecoutput: sector=%d  u=%4.1f v=%4.1f w=%4.1f energy=%5.2f\n",hitin->sector,peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0],
                 peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1],peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2],hitin->energy);
      */
      if(io == 2)
      {
        lnk_ec[itr][8]  = peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0];
        lnk_ec[itr][9]  = peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1];
        lnk_ec[itr][10] = peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2];
        lnk_ec[itr][11] = hitin->energy/0.275;
      }
      else if(io == 0)
      {
        lnk_ec[itr][0]  = peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0];
        lnk_ec[itr][1]  = peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1];
        lnk_ec[itr][2]  = peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2];
        lnk_ec[itr][3]  = hitin->energy/0.275;
      }
      else if(io == 1)
      {
        lnk_ec[itr][4]  = peak[io][0][hitin->peak1[0]].coord*36./geom->edge[0];
        lnk_ec[itr][5]  = peak[io][1][hitin->peak1[1]].coord*36./geom->edge[1];
        lnk_ec[itr][6]  = peak[io][2][hitin->peak1[2]].coord*36./geom->edge[2];
        lnk_ec[itr][7]  = hitin->energy/0.275;
      }
    }
    hitin++;
  }
  return(0);
}



int
ecmain_(int *jw, float threshold[3], int option[4], int *sector, int *ntr1,
       int *ntr2, float lnk_vect[ntrmx][nlnkvect], float lnk_ec[ntrmx][nlnkec])
{
  int nsec, i, itr, uvw, io, io_, npsble, gap, center, nhit, npeak[3][3];
  float x, y;
  ECStrip strip[2][3][36], stack[3][36];
  ECPeak peak[3][3][18];
  ECHit hit[1000];
  /*
  int npeak1[3];
  ECPeak peak1[3][18];
  */

  nhit = 0;
  gap = 0;
  nsec = (*sector)-1;

  /*printf("+++++++++++++++++++++ sector %1d itr1=%d itr2=%d +++++++++++++++++++++++++++++++++++++\n",*sector,(*ntr1),(*ntr2));*/
  for(itr=(*ntr1); itr<(*ntr2); itr++)
  {
    if((int)lnk_vect[itr][8]==0 || (int)lnk_vect[itr][9]==0 || (int)lnk_vect[itr][10]==0) break;
    /*printf("ecmain: prediction: u=%4.1f v=%4.1f w=%4.1f\n",lnk_vect[itr][8],lnk_vect[itr][9],lnk_vect[itr][10]);*/
    if(ecstrips(jw, threshold[0], nsec, strip) > 0)
    {
      /* first sum inner and outer parts and found cluster */
      io_ = 2;
      ecstacks(strip, stack);
      for(uvw=0; uvw<3; uvw++)
      {
        center = (int)lnk_vect[itr][8+uvw];
        npeak[io_][uvw] = ecpeakc(threshold[1], gap, center, &stack[uvw][0], &peak[io_][uvw][0]);
      }
      if(npeak[io_][0]==0 || npeak[io_][1]==0 || npeak[io_][2]==0) continue;
      if( (npsble = echit(io_,nsec,&npeak[io_][0],&peak[io_][0][0],&hit[nhit])) == 1)
      {
        nhit = eccorr(threshold[1],io_,nsec,&npeak[io_][0],&peak[io_][0][0],npsble,&hit[nhit]);
        /*printf("npeak= %d %d %d\n",npeak[io_][0],npeak[io_][1],npeak[io_][2]);*/
        if(nhit == 1)
        {
          /* inner and outer parts separately */
          for(io=0; io<2; io++)
          {
            for(uvw=0; uvw<3; uvw++)
            {
              npeak[io][uvw] = ecpeakb(threshold[1], &strip[io][uvw][0], &peak[io_][uvw][0], &peak[io][uvw][0]);
            }
            if( (npsble = echit(io,nsec,&npeak[io][0],&peak[io][0][0],&hit[nhit])) == 1)
            {
              nhit += eccorr(threshold[1],io,nsec,&npeak[io][0],&peak[io][0][0],npsble,&hit[nhit]);
            }
          }
        }
        /*printf("nhit=%d\n",nhit);*/
        ecoutput(itr,nsec+1,&peak[0][0][0],nhit,hit,lnk_ec);
      }
    }
  }

  /*printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n",nsec);*/



/* looking for clusters separately in inner and outer parts

  for(nsec=0; nsec<NSECTOR; nsec++)
  {
    if(ecstrips(jw, threshold[0], nsec, strip) > 0)
    {

      for(io=0; io<2; io++)
      {
        for(uvw=0; uvw<3; uvw++)
        {
          npeak[io][uvw] = ecpeak(threshold[1], gap, 0, 35, &strip[io][uvw][0], &peak[io][uvw][0]);
        }
        if( (npsble = echit(io,nsec,&npeak[io][0],&peak[io][0][0],&hit[nhit])) > 0)
        {
          nhit += eccorr(threshold[1],io,nsec,&npeak[io][0],&peak[io][0][0],npsble,&hit[nhit]);
        }
        else
        {
          ;
        }
      }

    }
  }
  ecbos(jw,nhit,hit);
  ecced(nhit,hit);
*/



/* looking for cluster summing energy in both parts



  for(nsec=0; nsec<NSECTOR; nsec++)
  {
    if(ecstrips(jw, threshold[0], nsec, strip) > 0)
    {
      ecstacks(strip, stack);
      for(uvw=0; uvw<3; uvw++)
      {
        npeak1[uvw] = ecpeak(threshold[1], gap, 0, 35, &stack[uvw][0], &peak1[uvw][0]);
      }
      io = 2;
      if( (npsble = echit(io,nsec,&npeak1[0],peak1,&hit[nhit])) > 0)
      {
        nhit += eccorr(threshold[1],io,nsec,&npeak1[0],peak1,npsble,&hit[nhit]);
      }
      else
      {
        ;
      }
    }
  }
  ecshower(nhit,hit);
  ecced(nhit,hit);

*/


  return(0);
}



/* ecbos.c - filling BOS bank ECHB by all information ( all io and sectors )

  input:  nhits, hit

*/

int
ecbos(int *jw, int nhits, ECHitPtr hit)
{
  ECHitPtr hitin, hitout;
  int nr, ncol, ind, i;

  nr = 0;
  ncol = (sizeof(ECHit)+3)/4;
  ind = etNcreate(jw,"ECHB",nr,ncol,nhits);
  if(ind <= 0) return(-1);

  hitin = hit;
  hitout = (ECHitPtr)&jw[ind];
  for(i=0; i<nhits; i++)
  {
    bcopy((char *)hitin, (char *)hitout, sizeof(ECHit));
    hitin++;
    hitout++;
  }
  return(ind);
}


/* ecshower.c - looking for showers using inner & outer parts
                and filling BOS bank ECRB

  input:  nhits, hit

*/

int
ecshower(int *jw, int nhits, ECHitPtr hit)
{
  ECShower shower[NSHOWER];
  ECHitPtr hitin[NHIT], hitout[NHIT];
  int sec, k, l1, l2, in, out, ish, ncol, nrow, nr, ind;
  float tmp, sigin, sigout, sig, dist, expN, prob;

  tmp = 4.0*sqrt(0.693147); /* alog(2.) = 0.693147 */

  ish = 0;
  for(sec=1; sec<=NSECTOR; sec++)
  {

    l1 = l2 = 0;
    for(k=0; k<nhits; k++)
    {
      if(hit[k].sector == sec)
      {
        if(hit[k].layer == 1)
        {
          hitin[l1++] = &hit[k];
        }
        else
        {
          hitout[l2++] = &hit[k];
        }
      }
    }

/* doesn't work realy: width always = 0 because of bug in ec_dalitz !!!

    for(in=0; in<l1; in++)
    {
      sigin = hitin[in]->width / tmp;
      for(out=0; out<l2; out++)
      {
        sigout = hitout[out]->width / tmp;
        sig = sigin*sigin + sigout*sigout;
        dist = (hitin[in]->i - hitout[out]->i) * (hitin[in]->i - hitout[out]->i) +
               (hitin[in]->j - hitout[out]->j) * (hitin[in]->j - hitout[out]->j);
        if(sig > 0.0)
        {
          expN = MAX(-dist/sig,-70.);
          prob = exp(expN);
        }
        else
        {
          prob = 1.0;
        }
printf("shower1: sec,width,i,j=%d %f %f %f\n",sec,hitin[in]->width,hitin[in]->i,hitin[in]->j);
printf("shower2: sec,width,i,j=%d %f %f %f\n",sec,hitout[out]->width,hitout[out]->i,hitout[out]->j);
printf("shower3: sig,dist,expN,prob=%f %f %f %f\n",sig,dist,expN,prob);

      }
    }
*/


    l1 = MIN(l1,l2);
    k = 0;
    for(in=0; in<l1; in++)
    {
      for(out=0; out<l1; out++)
      {
/*
printf("shower4: s,e_in,e_out=%d %f %f\n",sec,hitin[in]->energy,hitout[out]->energy);
*/
        shower[ish].e_in = hitin[in]->energy;           /* energy found for the inner layer */
        shower[ish].e_out = hitout[out]->energy;        /* energy found for the outer layer */
        shower[ish].dE_in = hitin[in]->denergy;         /* error on the energy found for the inner layer */
        shower[ish].dE_out = hitout[out]->denergy;      /* error on the energy found for the outer layer */
        shower[ish].t_in = hitin[in]->time;             /* time found for the inner layer */
        shower[ish].t_out = hitout[out]->time;          /* time found for the outer layer */
        shower[ish].dt_in = hitin[in]->dtime;           /* error on the time found for the inner layer */
        shower[ish].dt_out = hitout[out]->dtime;        /* error on the time found for the outer layer */
        shower[ish].i_in = hitin[in]->i;                /* sector rectangular coordinate for the inner layer */
        shower[ish].j_in = hitin[in]->j;                /* sector rectangular coordinate for the inner layer */
        shower[ish].i_out = hitout[out]->i;             /* sector rectangular coordinate for the outer layer */
        shower[ish].j_out = hitout[out]->j;             /* sector rectangular coordinate for the outer layer */
        shower[ish].di_in = hitin[in]->di;              /* sector rectangular coordinate error, inner layer */
        shower[ish].dj_in = hitin[in]->dj;              /* sector rectangular coordinate error, inner layer */
        shower[ish].di_out = hitout[out]->di;           /* sector rectangular coordinate error, outer layer */
        shower[ish].dj_out = hitout[out]->dj;           /* sector rectangular coordinate error, outer layer */
        shower[ish].x_in = hitin[in]->x;                /* lab coordinate, inner layer */
        shower[ish].y_in = hitin[in]->y;                /* lab coordinate, inner layer */
        shower[ish].z_in = hitin[in]->z;                /* lab coordinate, inner layer */
        shower[ish].x_out = hitout[out]->x;             /* lab coordinate, outer layer */
        shower[ish].y_out = hitout[out]->y;             /* lab coordinate, outer layer */
        shower[ish].z_out = hitout[out]->z;             /* lab coordinate, outer layer */
        shower[ish].dx_in = hitin[in]->dx;              /* lab coordinate error, inner layer */
        shower[ish].dy_in = hitin[in]->dy;              /* lab coordinate error, inner layer */
        shower[ish].dz_in = hitin[in]->dz;              /* lab coordinate error, inner layer */
        shower[ish].dx_out = hitout[out]->dx;           /* lab coordinate error, outer layer */
        shower[ish].dy_out = hitout[out]->dy;           /* lab coordinate error, outer layer */
        shower[ish].dz_out = hitout[out]->dz;           /* lab coordinate error, outer layer */
        shower[ish].u2_in = hitin[in]->uvw2[0];         /* second moment of u inner hit pattern */
        shower[ish].v2_in = hitin[in]->uvw2[1];         /* second moment of v inner hit pattern */
        shower[ish].w2_in = hitin[in]->uvw2[2];         /* second moment of w inner hit pattern */
        shower[ish].u2_out = hitout[out]->uvw2[0];      /* second moment of u outer hit pattern */
        shower[ish].v2_out = hitout[out]->uvw2[1];      /* second moment of v outer hit pattern */
        shower[ish].w2_out = hitout[out]->uvw2[2];      /* second moment of w outer hit pattern */
        shower[ish].u3_in = hitin[in]->uvw3[0];         /* third moment of u inner hit pattern */
        shower[ish].v3_in = hitin[in]->uvw3[1];         /* third moment of v inner hit pattern */
        shower[ish].w3_in = hitin[in]->uvw3[2];         /* third moment of w inner hit pattern */
        shower[ish].u3_out = hitout[out]->uvw3[0];      /* third moment of u outer hit pattern */
        shower[ish].v3_out = hitout[out]->uvw3[1];      /* third moment of v outer hit pattern */
        shower[ish].w3_out = hitout[out]->uvw3[2];      /* third moment of w outer hit pattern */
        shower[ish].i2 = 0.;          /* second moment of overall shower, sector coordinates */
        shower[ish].j2 = 0.;          /* second moment of overall shower, sector coordinates */
        shower[ish].i3 = 0.;          /* third moment of overall shower, sector coordinates */
        shower[ish].j3 = 0.;          /* third moment of overall shower, sector coordinates */
        shower[ish].istat = sec;                        /* status word */
        if((++ish) > NSHOWER)
        {
          printf("eclib: too many showers\n");
          goto write;
        }

/* TAK CHTO BEREM PERVII VSTRECHNII : */
        if((++k) >= l1)
        {
          /* BEREM PERVII VSTRECHNII */
          goto nextsector;
        }
      }
    }
nextsector:
    ;
  }

write:

  nr = 0;
  ncol = (sizeof(ECShower)+3)/4;
  nrow = ish;
  ind = etNcreate(jw,"ECRB",nr,ncol,nrow);
  if(ind <= 0) return(-1);

  bcopy((char *)&shower[0].e_in, (char *)&jw[ind], nrow*ncol*sizeof(int));

  return(ind);
}


/* ecced.c - filling BOS bank ECPI for Clas Event Display

  input:  nhits, hit

*/

int
ecced(int *jw, int nhits, ECHitPtr hit)
{
  ECHitPtr hitin;
  ECCedPtr ced;
  int nr, ncol, ind, i, j, nhl;

  /* create bank */

  nr = 0;
  ncol = sizeof(ECCed)/4;
  ind = etNcreate(jw,"ECPI",nr,ncol,nhits);
  if(ind <= 0) return(-1);

  /* fill bank */

  hitin = hit;
  ced = (ECCedPtr)&jw[ind];
  nhl = 1;
  for(i=0; i<nhits; i++)
  {
    ced->sector = hitin->sector;    /* sector number */
    ced->layer=hitin->layer+9;      /* layer number: 3->9-whole, 1->10-inner, 2->11-outer */
      if(ced->layer==12) ced->layer=9;
    if(i > 0)
    {
      if(ced->sector == (ced-1)->sector && ced->layer == (ced-1)->layer)
      {
        nhl++;
      }
      else
      {
        for(j=1; j<=nhl; j++) {(ced-j)->nhl = nhl; (ced-j)->ihl = nhl-(j-1);}
        nhl = 1;
      }
    }
    ced->iloc   = hitin->i;         /* position of the hit in the local coordinate system */
    ced->jloc   = hitin->j;         /* position of the hit in the local coordinate system */
    ced->diloc  = hitin->di;        /* i width of the hit */
    ced->djloc  = hitin->dj;        /* j width of the hit */
    ced->radius = hitin->width;     /* radius of the shower */
    ced->energy = hitin->energy;    /* energy */
    ced++;
    hitin++;
  }
  for(j=1; j<=nhl; j++) {(ced-j)->nhl = nhl; (ced-j)->ihl = nhl-(j-1);} /* for last hit */

/*
ced = (ECCedPtr)&jw[ind];
for(i=0; i<nhits; i++)
{
  printf("sec,lay,nhl,ihl,i,j,di,dj,width,energy=%d %d %d %d %f %f %f %f %f %f\n",
ced->sector,ced->layer,ced->nhl,ced->ihl,ced->iloc,ced->jloc,ced->diloc,ced->djloc,ced->radius,ced->energy);
  ced++;
}
*/

  return(ind);
}



/*
   Purpose and Methods : Finds energy cluster the track "itr" points to


   Input   :  itr     - track candidate number

   Outputs :  iflg = 0  means OK
              

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   30-SEP-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcfinde
*/


#include "dclib.h"
#include "sdacalib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define Acl_thr   10     /* visible energy threshold [MeV] */
#define slab_dif  3      /* Difference between SLABclust and SLABtrk */
#define Evis_cut  0.10   /* Cut on Evis/Ptrk */

void
dcecid_(int *jw, int *isec, DCecid *ecid, float *p, int *iflg)
{
  dcecid(jw, *isec, ecid, *p, iflg);
  return;
}

void
dcecid(int *jw, int isec, DCecid *ecid, float p, int *iflg)
{
  int i, j, ih, ic, nc, nh, il, sec, isl, Vtrk,
      du, dv, dw, dif, Ucl, Vcl, Wcl, digi[6][36], Evis[3][36], Etot,
      nsmin, nsmax, fslab, nslab, p1s, p2s, sumE, Epeak, ncl[3],
      Eclust[3][18][5], UVWdif[3][18];
  float Et, Erat;
  int ec_digi[6][36][3];
  int ec_ndig[6];
  int ind, nd, nd2, id;
  int i32[7000];
  short *i16;


  /* Clear arrays */

  *iflg = 2;
  /*printf("dcecid: %4d %4d %4d -> ... \n",ecid->u,ecid->v,ecid->w);*/
  if(ecid->u * ecid->v * ecid->w ==  0) return;

  for(il=0; il<6; il++)
    for(isl=0; isl<36; isl++) digi[il][isl] = 0;
  sec = isec - 1;
  *iflg = 1;

  /* get raw data */

  for(il=0; il<6; il++) ec_ndig[il] = 0;
  i16 = (short *) i32;
  if((ind=etNlink(jw,"EC  ",isec)) > 0)
  {
    if((nd = etNdata(jw,ind)) > 0)
    {
      for(j=0; j<nd; j++) i32[j] = jw[ind+j];
      nd2 = 2*nd - 1;
      for(i=1; i<=nd2; i+=3)
      {
        id = i16[i-1] & 0xFF;
        if(id < 1 || id > NSTRIP) continue;
        il = (i16[i-1] & 0x7F00) >> 8;
        if(il < 1 || il > 6) continue;
        if(ec_ndig[il-1] >= NSTRIP) continue;
        ec_ndig[il-1]++;
        ih = ec_ndig[il-1];
        ec_digi[il-1][ih-1][0] = id;
        /*ec_digi[il-1][ih-1][1] = i16[i+1-1]; not in use here */
        ec_digi[il-1][ih-1][2] = i16[i+2-1];
      }
    }
  }

  /* calibration */

  for(il=0; il<6; il++)
  {
    if(nh = ec_ndig[il])
    {
      for(ih=0; ih<nh; ih++)
      {
        isl = ec_digi[il][ih][0];
        digi[il][isl-1] = ( ec_digi[il][ih][2]
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


printf("old: %4.1f %4.1f %4.1f -> %f\n",(float)Eclust[0][Ucl-1][2],(float)Eclust[1][Vcl-1][2],
  (float)Eclust[2][Wcl-1][2],((float)Et)/0.275/1000.);

a999:

  /* Debugging Printout */

  if(0)
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


/* ecmatch - get shower using prediction for UVW position */

#define NUDIF 6
#define NVDIF 12
#define NWDIF 12
#define ERCUT 0.07   /* Cut on energy ratio */

int
ecmatch(int *jw, const float threshold[3], int *ntrk, PRTRACK *track)
{
  int isec, sec, i, j, itr, itmp, uvw, io, io_, npsble, gap, center[3], nhit, npeak[3][3];
  float x, y;
  ECStrip strip[2][3][36], stack[3][36];
  ECPeak peak[3][3][18];
  PRTRACK *trk;
  ECHit hit[1000];
  ECHitPtr hitin;
  ECParPtr geom;


  int ic, nc, Vtrk, du, dv, dw, dif, Ucl, Vcl, Wcl, Evis[3][36], Etot,
      nsmin, nsmax, fslab, nslab, p1s, p2s, sumE, Epeak, ncl[3],
      Eclust[3][18][5], UVWdif[3][18];
  float Et, Erat;
  int ec_digi[6][6][36][3]; /* [sector][layer][strip][id/tdc/adc] */
  int ec_ndig[6][6];        /* [sector][layer] */
  int ec_have[6];           /* [sector] - if =1 then this sector already obtained */
  int il, ih, ind, nd, nd2, id, nh, isl;
  int digi[6][36], i32[7000];
  short *i16;

  nhit = 0;
  gap = 0;
  trk = track - 1;
  for(sec=0; sec<6; sec++) ec_have[sec] = 0;

  printf("\necmatch: reached, *ntrk=%d trk=%08x\n",*ntrk,trk);
  for(itr=0; itr<(*ntrk); itr++)
  {
    trk++;
    /*printf("ecmatch: ntrack=%d u=%d v=%d w=%d\n",itr,trk->nu, trk->nv, trk->nw);*/

    trk->ec[2].u = 0.0;
    trk->ec[2].v = 0.0;
    trk->ec[2].w = 0.0;
    trk->ec[2].e = 0.0;
    if(trk->nu==0 || trk->nv==0 || trk->nw==0) continue;
    isec = trk->sector;
    sec = isec - 1;
    printf("ecmatch: prediction: u=%4.1f v=%4.1f w=%4.1f (sec=%1d)\n",
      trk->nu/2.,trk->nv/2.,trk->nw/2.,isec);

    /* get raw data for this sector if not done yet */

    /*printf("ecmatch: ec_have[%1d]=%1d\n",sec,ec_have[sec]);*/
    if(!ec_have[sec])
    {
      ec_have[sec] = 1;
      /*printf("ecmatch: read raw data for sector %d\n",isec);*/

      for(il=0; il<6; il++)
        for(isl=0; isl<36; isl++) digi[il][isl] = 0;
      for(il=0; il<6; il++) ec_ndig[sec][il] = 0;
      i16 = (short *) i32;
      if((ind=etNlink(jw,"EC  ",isec)) > 0)
      {
        if((nd = etNdata(jw,ind)) > 0)
        {
          for(j=0; j<nd; j++) i32[j] = jw[ind+j];
          nd2 = 2*nd - 1;
          for(i=1; i<=nd2; i+=3)
          {
            id = i16[i-1] & 0xFF;
            if(id < 1 || id > NSTRIP) continue;
            il = (i16[i-1] & 0x7F00) >> 8;
            if(il < 1 || il > 6) continue;
            if(ec_ndig[sec][il-1] >= NSTRIP) continue;
            ec_ndig[sec][il-1]++;
            ih = ec_ndig[sec][il-1];
            ec_digi[sec][il-1][ih-1][0] = id;
            /*ec_digi[sec][il-1][ih-1][1] = i16[i+1-1]; not in use here */
            ec_digi[sec][il-1][ih-1][2] = i16[i+2-1];
          }
        }
      }
    }

    /* calibration (put it in previous IF !) */

    for(il=0; il<6; il++)
    {
      if(nh = ec_ndig[sec][il])
      {
        for(ih=0; ih<nh; ih++)
        {
          isl = ec_digi[sec][il][ih][0];
          digi[il][isl-1] = ( ec_digi[sec][il][ih][2]
                              - sdacalib_.cal_ec[sec][il][isl-1][5] )
                              / sdacalib_.cal_ec[sec][il][isl-1][2];
          if(digi[il][isl-1] < 0) digi[il][isl-1] = 0;
        }
      }
    }

    /* Sum energy (forward + backward) */

    Etot = 0;
    for(il=0; il<3; il++)
    {
      for(isl=0; isl<36; isl++)
      {
        Evis[il][isl] = digi[il][isl] + digi[il+3][isl];
        Etot += Evis[il][isl];
      }
    }
    printf("ecmatch: Etot=%d\n",Etot);

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
          if(ABS(isl-p1s) < 2)
          {
            continue;
          }
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

    printf("ecmatch: ncl[0]=%d ncl[1]=%d ncl[2]=%d\n",ncl[0],ncl[1],ncl[2]);
    if(ncl[0]*ncl[1]*ncl[2] == 0) continue; /* goto next track candidate */


    /********************************/
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
          dif = Eclust[0][ic-1][j-1] - trk->nu/2;
          UVWdif[0][ic-1] = dif;
          if(ABS(dif) < ABS(du))
          {
            du = dif;
            Ucl= ic;
          }
        }
      }
    }
    printf("ecmatch: Ucl=%d\n",Ucl);
    if(Ucl == 0) continue;
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
          dif = Eclust[1][ic-1][j-1] - trk->nv/2;
          UVWdif[1][ic-1] = dif;
          if(ABS(dif) < ABS(dv))
          {
            dv = dif;
            Vcl= ic;
          }
        }
      }
    }
    printf("ecmatch: Vcl=%d\n",Vcl);
    if(Vcl == 0) continue;
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
          dif = Eclust[2][ic-1][j-1] - trk->nw/2;
          UVWdif[2][ic-1] = dif;
          if(ABS(dif) < ABS(dw))
          {
            dw = dif;
            Wcl= ic;
          }
        }
      }
    }
    printf("ecmatch: Wcl=%d\n",Wcl);
    if(Wcl == 0) continue;
    UVWdif[2][Wcl-1] = dw;

    /* Check track-cluster matching */

    printf("ecmatch: diff in uvw -> %d(%d) %d(%d) %d(%d)\n",
      ABS(du),NUDIF,ABS(dv),NVDIF,ABS(dw),NWDIF);
    if(ABS(du) > NUDIF) continue;
    if(ABS(dv) > NVDIF) continue;
    if(ABS(dw) > NWDIF) continue;

    /* Check energy ratio */
    /* Make a cut on the total visible energy in that sector */

    Et = Eclust[0][Ucl-1][4] + Eclust[1][Vcl-1][4] + Eclust[2][Wcl-1][4];
    Erat = Et / (1000.*(trk->p));
    printf("ecmatch: Et=%f trk->p=%f -> %f (cut=%f)\n",Et,trk->p,Erat,ERCUT);
    if(Erat < ERCUT) continue;


    /* fill output info */

    trk->ec[2].u = Eclust[0][Ucl-1][2];
    trk->ec[2].v = Eclust[1][Vcl-1][2];
    trk->ec[2].w = Eclust[2][Wcl-1][2];
    trk->ec[2].e = (Et / 0.275) * 0.001;

    printf("ecmatch: finaly u=%f v=%f w=%f e=%f\n",
      trk->ec[2].u,trk->ec[2].v,trk->ec[2].w,trk->ec[2].e);

    nhit++;
  }

  return(nhit);
}
E 1
