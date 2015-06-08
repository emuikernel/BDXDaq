//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMSda                                                               //
//                                                                      //
//  Clas Monitor Histogramming                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
#include "TRandom.h"
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"
#include "uthbook.h"

/*
#include "trlib.h"
#include "uthbook.h"
#include "prlib.h"
#include "eclib.h"
*/

typedef struct
{
  float x;          /* x                                    */
  float y;          /* y 'Vertex' position {x,y,z}          */
  float z;          /* z                                    */
  float px;         /* Px                                   */
  float py;         /* Py  momentum at 'vertex' {Px,Py,Pz}  */
  float pz;         /* Pz                                   */
  float q;          /* charge   (straight tracks: set q=0.) */
  float chi2;       /* Chisquare for this Track             */
  int   itr_sec;    /* Trackno_in_Sector + Sector*100       */
  int   itr_hbt;    /* Trackno in HBTR for this track       */
} TBTR;

#include "CMSda.h"
CMSda *gCMSda; // to use it in buttons

#include "CMDisplay.h" // to get gCMDisplay
extern CMDisplay *gCMDisplay;

#include "CMDaq.h"     // to get gCMDaq
extern CMDaq *gCMDaq;


ClassImp(CMSda)

#define ABS(x)      ((x) < 0 ? -(x) : (x))
#define raddeg 57.2957795130823209

#define PB 70.
#define T  0

#define Q_1  0.15
#define Q_2  0.5
#define MMe1 0.93
#define MMe2 0.96
#define MMe3 1.18
#define MMe4 1.26

//static Float_t threshold[3] = {0.0001,0.0001,0.0003};
//static Int_t option[4] = {0,0,0,0};
//static PRTRACK *TrkPtr;

static TText *sss = NULL;


Float_t z[5],x[5],y[5],errorz[5];

//______________________________________________________________________________
//Double_t func(Double_t x, Double_t mean, Double_t sigma)
//{
//   // Calculate a gaussian function with mean and sigma
//    if (sigma == 0) return 1.e30;
//    Double_t arg = (x-mean)/sigma;
//    return TMath::Exp(-0.5*arg*arg);
//}

//______________________________________________________________________________
Double_t func(float x,float y,Double_t *par)
{
 Double_t value=( (par[0]*par[0])/(x*x)-1)/ ( par[1]+par[2]*y-par[3]*y*y);
 return value;
}

//______________________________________________________________________________
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   const Int_t nbins = 5;
   Int_t i;

//calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   for (i=0;i<nbins; i++) {
     delta  = (z[i]-func(x[i],y[i],par))/errorz[i];
     chisq += delta*delta;
   }
   f = chisq;
}


/*******************/
/*** local HBOOK ***/

TH1F *hist1[NHIST];
TH2F *hist2[NHIST];
UThisti histi[NHIST];
UThistf histf[NHIST];


/* get all existing histograms */

Int_t CMSda::CMbos2root(int *jw)
{
  Int_t ind, id, ret;

  ret = 0;
  if((ind=etNamind(jw,"HISS")+1) > 0)
  {
    while ((ind=etNnext(jw,ind)) > 0)
    {
      printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISS(jw, ind);
      ret |= CMhist2root(etNnum(jw,ind));
    }
  }

  if((ind=etNamind(jw,"HISI")+1) > 0)
  {
    while ((ind=etNnext(jw,ind)) > 0)
    {
      printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISI(jw, ind);
      ret |= CMhist2root(etNnum(jw,ind));
    }
  }

  if((ind=etNamind(jw,"HISF")+1) > 0)
  {
    while ((ind=etNnext(jw,ind)) > 0)
    {
      //printf("=> %d %d %d %d\n",etNnum(jw,ind),etNdata(jw,ind),etNcol(jw,ind),etNrow(jw,ind));
      CMgetHISF(jw, ind);
      ret |= CMhistf2root(etNnum(jw,ind));
    }
  }

  return(ret);
}


/* book and fill ROOT histogram */

Int_t CMSda::CMhist2root(Int_t id)
{
  Int_t ibinx, ibiny;
  char title[20];

  if(histi[id].nbinx > 0)
  {
    //printf("bos2root: [%d] nbinx=%d nbiny=%d\n",id,histi[id].nbinx,histi[id].nbiny);
    if(histi[id].nbiny == 0)
    {
      if(hist1[id]==NULL)
      {
        sprintf(title,"hist1[%d]",id);
        hist1[id]  = new TH1F(title,histi[id].title,
                            histi[id].nbinx,histi[id].xmin,histi[id].xmax);
        printf("CMhist2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
      }
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        hist1[id]->Fill((float)ibinx,(float)histi[id].buf[ibinx]);
    }
    else
    {
      if(hist2[id]==NULL)
      {
        sprintf(title,"hist2[%d]",id);
        hist2[id]  = new TH2F(title,histi[id].title,
                            histi[id].nbinx,histi[id].xmin,histi[id].xmax,
                            histi[id].nbiny,histi[id].ymin,histi[id].ymax);
        printf("CMhist2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
      }
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
          hist2[id]->Fill((float)ibinx,(float)ibiny,
                          (float)histi[id].buf2[ibinx][ibiny]);
    }

    return(1);
  }

}

Int_t CMSda::CMhistf2root(Int_t id)
{
  Int_t ibinx, ibiny;
  Float_t x, y;
  char title[20];

  if(histf[id].nbinx > 0)
  {
    //printf("bos2root: [%d] nbinx=%d nbiny=%d\n",id,histf[id].nbinx,histf[id].nbiny);
    if(histf[id].nbiny == 0)
    {
      if(hist1[id]==NULL)
      {
        sprintf(title,"hist1[%d]",id);
        hist1[id]  = new TH1F(title,histf[id].title,
                            histf[id].nbinx,histf[id].xmin,histf[id].xmax);
        printf("CMhistf2root: &hist1[%d]=0x%08x\n",id,hist1[id]);
      }
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
      {
x = histf[id].xmin + (((float)ibinx) + 0.5)*histf[id].dx;
        hist1[id]->Fill((float)x,(float)histf[id].buf[ibinx]);
      }
    }
    else
    {
      if(hist2[id]==NULL)
      {
        sprintf(title,"hist2[%d]",id);
        hist2[id]  = new TH2F(title,histf[id].title,
                            histf[id].nbinx,histf[id].xmin,histf[id].xmax,
                            histf[id].nbiny,histf[id].ymin,histf[id].ymax);
        printf("CMhist2root: &hist2[%d]=0x%08x\n",id,hist2[id]);
      }
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histf[id].nbiny; ibiny++)
          hist2[id]->Fill((float)ibinx,(float)ibiny,
                          (float)histf[id].buf2[ibinx][ibiny]);
    }

    return(1);
  }

}


/* get histogram from HISF bank */

Int_t CMSda::CMgetHISF(int *jw, int ind)
{
  Int_t i, id, nw2, nch, ibinx, ibiny, packed;
  float *fbuf, *fstop;

  id = etNnum(jw,ind);
  //printf("HISF [%d] found\n",id);

  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  //printf("HISF [%d] 1\n",id);fflush(stdout);
  if(histf[id].nbinx > 0 || histf[id].nbiny > 0)
  {
    printf("utbos2h: WARN id=%d already exist -  replace it\n",id);
    free(histf[id].title);
    if(histf[id].nbiny == 0)
    {
      free(histf[id].buf);
    }
    else
    {
      for(i=0; i<histf[id].nbinx; i++) free(histf[id].buf2[i]);
      free(histf[id].buf2);
    }
    histf[id].nbinx = 0;
  }

  nw2 = etNcol(jw,ind)*etNrow(jw,ind);
  printf("bos2root: nw2=%d (ncol=%d nrow=%d)\n",
                    nw2,etNcol(jw,ind),etNrow(jw,ind));fflush(stdout);

  fbuf = (float *)&jw[ind];
  fstop = fbuf + nw2;

  float f[12];
  packed               = f[0] = *fbuf++;
  histf[id].nbinx      = f[1] = *fbuf++;
  histf[id].xmin       = f[2] = *fbuf++;
  histf[id].xmax       = f[3] = *fbuf++;
  histf[id].xunderflow = f[4] = *fbuf++;
  histf[id].xoverflow  = f[5] = *fbuf++;
  histf[id].nbiny      = f[6] = *fbuf++;
  histf[id].ymin       = f[7] = *fbuf++;
  histf[id].ymax       = f[8] = *fbuf++;
  histf[id].yunderflow = f[9] = *fbuf++;
  histf[id].yoverflow  = f[10]= *fbuf++;
  histf[id].entries    = f[11]= *fbuf++;

  printf("bos2root: [%d] -> %f %f %f %f %f %f %f %f %f %f %f %f\n",id,
          f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11]);

histf[id].dx = (histf[id].xmax - histf[id].xmin)/(float)histf[id].nbinx;

  histf[id].nwtitle = *fbuf++;
  nch = 4*histf[id].nwtitle + 1;
  histf[id].title = (char *) malloc(nch);
  strncpy(histf[id].title, (char *)fbuf, nch-1);
  histf[id].title[nch-1] = '\0';
  fbuf += histf[id].nwtitle;

  if(packed == NORMAL_F)
  {
    if(histf[id].nbiny == 0) /* 1-dim */
    {
      histf[id].buf = (float *) calloc(histf[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
      {
        histf[id].buf[ibinx] = *fbuf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histf[id].buf2 = (float **) calloc(histf[id].nbinx,sizeof(int));
      for(i=0; i<histf[id].nbinx; i++)
        histf[id].buf2[i] = (float *) calloc(histf[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histf[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histf[id].nbiny; ibiny++)
        {
          histf[id].buf2[ibinx][ibiny] = *fbuf++;
        }
      }
    }
  }
  else
  {
    printf("PACKED format for HISF is not supported yet\n");
    return(0);
  }

  return(0);
}


/* get histogram from HISS bank */

Int_t CMSda::CMgetHISS(int *jw, int ind)
{
  Int_t i, id, nw2, nch, ibinx, ibiny, packed;
  unsigned short *buf2, *stop2;

  //printf("bos2root: bank 'HISS' found !\n");

  id = etNnum(jw,ind);
  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  if(histi[id].nbinx > 0 || histi[id].nbiny > 0)
  {
    //printf("utbos2h: WARN id=%d already exist - replace it\n",id);
    free(histi[id].title);
    if(histi[id].nbiny == 0)
    {
      free(histi[id].buf);
    }
    else
    {
      for(i=0; i<histi[id].nbinx; i++) free(histi[id].buf2[i]);
      free(histi[id].buf2);
    }
    histi[id].nbinx = 0;
  }

  buf2 = (unsigned short *)&jw[ind];
  nw2 = etNcol(jw,ind)*etNrow(jw,ind);
  //printf("utbos2h: nw2=%d\n",nw2);
  stop2 = buf2 + nw2;

  packed              = *buf2++;
  histi[id].nbinx      = *buf2++;
  histi[id].xmin       = *buf2++;
  histi[id].xmax       = *buf2++;
  histi[id].xunderflow = *buf2++;
  histi[id].xoverflow  = *buf2++;
  histi[id].nbiny      = *buf2++;
  histi[id].ymin       = *buf2++;
  histi[id].ymax       = *buf2++;
  histi[id].yunderflow = *buf2++;
  histi[id].yoverflow  = *buf2++;
  histi[id].entries    = *buf2++;

  histi[id].nwtitle = (*buf2++) / 2;
  nch = 4*histi[id].nwtitle + 1;
  histi[id].title = (char *) malloc(nch);
  strncpy(histi[id].title, (char *)buf2, nch-1);
  histi[id].title[nch-1] = '\0';
  buf2 += 2*histi[id].nwtitle;

  if(packed == NORMAL_I2)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        histi[id].buf[ibinx] = *buf2++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
        {
          histi[id].buf2[ibinx][ibiny] = *buf2++;
        }
      }
    }
  }
  else if(packed == PACKED_I2)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++) histi[id].buf[ibinx] = 0;
      while(buf2 < stop2)
      {
        ibinx = *buf2++;
        histi[id].buf[ibinx] = *buf2++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
          histi[id].buf2[ibinx][ibiny] = 0;
      while(buf2 < stop2)
      {
        ibinx = *buf2++;
        ibiny = *buf2++;
        histi[id].buf2[ibinx][ibiny] = *buf2++;
      }
    }
  }

  return(0);
}


/* get histogram from HISI bank - NEED DEBUG !!!!!!!!!!!!!! */

Int_t CMSda::CMgetHISI(int *jw, int ind)
{
  Int_t i, id, nw, nch, ibinx, ibiny, packed;
  unsigned long *buf, *stop;

  //printf("bos2root: bank 'HISI' found !\n");

  id = etNnum(jw,ind);
  if(id <=0 || id >=NHIST)
  {
    printf("utbos2h: ERROR id=%d, must be from 1 to %d\n",id,NHIST);
    return(0);
  }

  if(histi[id].nbinx > 0 || histi[id].nbiny > 0)
  {
    //printf("utbos2h: WARN id=%d already exist - replace it\n",id);
    free(histi[id].title);
    if(histi[id].nbiny == 0)
    {
      free(histi[id].buf);
    }
    else
    {
      for(i=0; i<histi[id].nbinx; i++) free(histi[id].buf2[i]);
      free(histi[id].buf2);
    }
    histi[id].nbinx = 0;
  }

  nw = etNcol(jw,ind)*etNrow(jw,ind);
  //printf("utbos2h: nw=%d\n",nw);

  buf = (unsigned long *)&jw[ind];
  stop = buf + nw;

  packed              = *buf++;
  histi[id].nbinx      = *buf++;
  histi[id].xmin       = *buf++;
  histi[id].xmax       = *buf++;
  histi[id].xunderflow = *buf++;
  histi[id].xoverflow  = *buf++;
  histi[id].nbiny      = *buf++;
  histi[id].ymin       = *buf++;
  histi[id].ymax       = *buf++;
  histi[id].yunderflow = *buf++;
  histi[id].yoverflow  = *buf++;
  histi[id].entries    = *buf++;

  histi[id].nwtitle = *buf++;
  nch = 4*histi[id].nwtitle + 1;
  histi[id].title = (char *) malloc(nch);
  strncpy(histi[id].title, (char *)buf, nch-1);
  histi[id].title[nch-1] = '\0';
  buf += histi[id].nwtitle;

  if(packed == NORMAL_I4)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        histi[id].buf[ibinx] = *buf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
      {
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
        {
          histi[id].buf2[ibinx][ibiny] = *buf++;
        }
      }
    }
  }
  else if(packed == PACKED_I4)
  {
    if(histi[id].nbiny == 0) /* 1-dim */
    {
      histi[id].buf = (int *) calloc(histi[id].nbinx,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++) histi[id].buf[ibinx] = 0;
      while(buf < stop)
      {
        ibinx = *buf++;
        histi[id].buf[ibinx] = *buf++;
      }
    }
    else                    /* 2-dim */ /* NEED CHECK !!! */
    {
      histi[id].buf2 = (int **) calloc(histi[id].nbinx,sizeof(int));
      for(i=0; i<histi[id].nbinx; i++)
        histi[id].buf2[i] = (int *) calloc(histi[id].nbiny,sizeof(int));
      for(ibinx=0; ibinx<histi[id].nbinx; ibinx++)
        for(ibiny=0; ibiny<histi[id].nbiny; ibiny++)
          histi[id].buf2[ibinx][ibiny] = 0;
      while(buf < stop)
      {
        ibinx = *buf++;
        ibiny = *buf++;
        histi[id].buf2[ibinx][ibiny] = *buf++;
      }
    }
  }

  return(0);
}


/* get one histogram from BOS bank */

Int_t CMSda::CMgetHistFromBOS(int *jw, int id)
{
  Int_t ret, ind;
  unsigned long *buf, *stop;

  if( (ind = etNlink(jw,"HISF",id)) > 0)
  {
    CMgetHISF(jw, ind);
  }
  else if( (ind = etNlink(jw,"HISS",id)) > 0)
  {
    CMgetHISS(jw, ind);
  }
  else
  {
    //printf("found nothing\n");
    return(0); // no such histogram
  }

  ret = CMhist2root(id);

  return(ret);
}



/*******************/
/*******************/



//______________________________________________________________________________
CMSda::CMSda(Option_t *fname) : CMClient()
{
// Create one CMSda object

  gCMSda = this;

  setnum = 4;
  callprlib = 0;
  calltrlib = 0;
  downloaded = 0;

  fMMe1 = MMe1;
  fMMe2 = MMe2;
  fMMe3 = MMe3;
  fMMe4 = MMe4;

  fQ1 = Q_1;
  fQ2 = Q_2;

  fPB = PB;
  fT = T;


  //UInt_t test = 538976288;
  //UInt_t test = 1110521376;
  //printf("\n\n  %08x   >%4.4s<\n\n",test,&test);

  Int_t i;
  for(i=0; i<NHIST; i++) {delete hist1[i]; hist1[i]=NULL;}
  for(i=0; i<NHIST; i++) {delete hist2[i]; hist2[i]=NULL;}

  //Download();
}


//_____________________________________________________________________________
void CMSda::Download()
{
//

//boy  sda_init_();

/* 'C only' version 
  prinit("/scratch/boiarino/DICT/prlink_1500_0_121_0.7_4.0_e.bos",0,3,3);
*/
  //TrkPtr = (PRTRACK *) malloc(NTRACK*sizeof(PRTRACK));

/*
  // set environment variables
  // read 'sda.config' file
  trconfig();
  // read calibration
  printf("use calibration for the run # %d\n",sdakeys_.ltrig[0]);
  sda_calib_(&sdakeys_.ltrig[0]);
*/

  this->Hist();
  this->Buttons();

  downloaded = 1;
}


//_____________________________________________________________________________
void CMSda::Prestart(Int_t *iw)
{
//
  Int_t i, j;

/* 'C only' version 
  ecinit(iw, option[0], option[1], option[2], 20000);
*/

  for(i=0; i<QRANGE; i++)
  {
    for(j=0; j<MMRANGE; j++)
	{
      Nhel[i][j] = Nhelbar[i][j] = 0;
    }
  }
  nevents = 0;
  nevents1 = 0;
  prestarted = 1;

  Reset();
}



//______________________________________________________________________________
void CMSda::Go(Int_t *iw)
{
//

  time0 = time(NULL);
}


//______________________________________________________________________________
void CMSda::End(Int_t *iw)
{
//

//boy  sda_erun_(iw);
//boy  sda_last_(iw);
}


//______________________________________________________________________________
int CMSda::Event(Int_t *iw)
{
// one event processing

  PRTRACK *trkptr;
  //TRevent *ana;
  Int_t ret, ind, ind1, itmp, itmp0, itmp1, icol, i, j, irow, ntracks, tof, id, nhits;
  Float_t dalitz, mom, *rw, pt;
  short *schit, *fbpm;
  unsigned long *buf;
  TBTR *trk;

  nevents ++;
  ret = 0;

  //printf("CMSda::Event() reached\n");



  if((ind = etNlink(iw,"FBPM",1)) > 0)
  {
    ret = 1;
    icol = iw[ind-6];
    irow = iw[ind-5];

    fbpm = (short *)&iw[ind];
    itmp = irow / 8;
    for(j=0; j<itmp; j++)
    {
      adc1->Fill((float)fbpm[2]);
      adc2->Fill((float)fbpm[5]);
      adc3->Fill((float)fbpm[8]);
      adc4->Fill((float)fbpm[11]);
      fbpm += (8*icol);
    }

    //printf("found FBPM bank, ind=%d icol=%d irow=%d\n",ind,icol,irow);
    //fbpm = (short *)&iw[ind];
    //for(j=0; j<irow; j++)
    //{
    //  printf("id=%d tdc=%d adc=%d\n",fbpm[0],fbpm[1],fbpm[2]);
    //  fbpm += icol;
    //}
  }

  if((ind = etNlink(iw,"FBPM",2)) > 0)
  {
    ret = 1;
    icol = iw[ind-6];
    irow = iw[ind-5];

    fbpm = (short *)&iw[ind];
    itmp = irow / 8;
    for(j=0; j<itmp; j++)
    {
      adc5->Fill((float)fbpm[2]);
      adc6->Fill((float)fbpm[5]);
      adc7->Fill((float)fbpm[8]);
      adc8->Fill((float)fbpm[11]);
      fbpm += (8*icol);
    }

    //printf("found FBPM bank, ind=%d icol=%d irow=%d\n",ind,icol,irow);
    //fbpm = (short *)&iw[ind];
    //for(j=0; j<irow; j++)
    //{
    //  printf("id=%d tdc=%d adc=%d\n",fbpm[0],fbpm[1],fbpm[2]);
    //  fbpm += icol;
    //}
  }

  if((ind = etNlink(iw,"RC22",0)) > 0)
  {
    ret = 1;
    icol = iw[ind-6];
    irow = iw[ind-5];
    buf = (unsigned long *)&iw[ind];

      adc5->Fill((float)(buf[1]&0xFFF));
      adc6->Fill((float)(buf[3]&0xFFF));
      adc7->Fill((float)(buf[5]&0xFFF));
      adc8->Fill((float)(buf[7]&0xFFF));
      if((buf[5]&0xFFF)>200 && (buf[1]&0xFFF)>200)
      {
        printf("--> adp=%d counter=%d\n",buf[1]&0xFFF,buf[5]&0xFFF);
        adc5adc6->Fill((float)(buf[5]&0xFFF),(float)(buf[1]&0xFFF));
      }

    if(icol != 1 || irow != 34) printf("icol=%d irow=%d\n",icol,irow);
  }





/* 'C only' version - not ready for the tracking

  // do road finding if required
  if(callprlib)
  {
    if((ntracks=prlib(iw,TrkPtr)) > 0)
    {
      eclib(iw, threshold, &ntracks, TrkPtr);
      prbos(iw, &ntracks, TrkPtr);
      ret=1;
    }
  }
goto exit;
*/

  // do road finding if required
//boy  if(callprlib)
//boy  {
//boy    sda_evin_(iw,iw);
//boy    sda_cleananal_();
//boy    prlib_(iw);
//boy  }

  // road finding histogramming
  if((ind=etNlink(iw,"PATH",0)) > 0)
  {
    ret=1;
    trkptr = (PRTRACK *)&iw[ind];
    ntracks = (etNdata(iw,ind)*sizeof(int))/sizeof(PRTRACK);

    //printf("found PATH bank, ntracks=%d\n",ntracks);
    ret=1;
    for(i=0; i<ntracks; i++)
    {
      //printf("mom[%3d]=%d\n",i,trkptr->p);
      dalitz = (Float_t)(trkptr->nu/2.+trkptr->nv/2.+trkptr->nw/2.);
      //printf("u,v,w=%f %f %f dalitz=%f\n",trkptr->nu/2.,trkptr->nv/2.,trkptr->nw/2.,dalitz);
      mom = (float)trkptr->p;
      if(mom < 0.01) continue;
      //mom1->Fill(mom*1000.);
      if(dalitz > 1.)
      {
        if(trkptr->charge < 0)
        {
          tdc1adc1->Fill(trkptr->ec[0].e/mom,trkptr->ec[1].e/mom);
          //printf("tdc2 fill: %f(= %f / %f)\n",trkptr->ec[2].e/mom,trkptr->ec[2].e,mom);
          tdc2->Fill(trkptr->ec[2].e/mom);
          //if(trkptr->ec[2].e/mom > 1.) scaler[3]++;
        }
        else
        {
          tdc2adc2->Fill(trkptr->ec[0].e/mom,trkptr->ec[1].e/mom);
        }
      }
      adc2->Fill((float)trkptr->ntof);
      //scaler[0]++;
      //printf("(float)trkptr->ntof=%f\n",(float)trkptr->ntof);

      if((ind = etNlink(iw,"SC  ",trkptr->sector)) > 0)
      {
        irow = iw[ind-5];
        schit = (short *)&iw[ind];
        for(j=0; j<irow; j++)
        {
          //printf("id=%d tof=%d %d\n",schit[0],schit[2],schit[4]);
          if(ABS(trkptr->ntof-schit[0])<2)
          {
            if(schit[2]>10 || schit[4]>10)
            {
              tdc1->Fill((float)schit[0]);
              //scaler[1]++;
              break;
            }
          }
          schit += 5;
        }
        schit = (short *)&iw[ind];
        for(j=0; j<irow; j++)
        {
          //printf("id=%d tof=%d %d\n",schit[0],schit[2],schit[4]);
          if(ABS(trkptr->ntof-schit[0])<2)
          {
            if(schit[2]>10 && schit[4]>10)
            {
              //adc1->Fill((float)schit[0]);
              //scaler[2]++;
              break;
            }
          }
          schit += 5;
        }
      }

    }

  }



  // do track fitting if required
//boy  if(calltrlib)
//boy  {
//boy    static Int_t isecm=1, nsect=6, lanal1=4;
//boy    static Float_t zcut5=1.0, zcut8=1.0;
//boy    ret = 0;
//boy    sda_path2anal_(iw,&isecm,&nsect);
//boy    sda_trak_(iw,iw,&lanal1,&zcut5,&zcut8);
//boy  }



  // fill after-reconstruction hists

  //if((ind = etNlink(iw,"TBTR",0)) > 0)
  if(0) // disable it
  {
    ret = 1;
//printf("found TBTR bank, ind=%d\n",ind);
    irow = iw[ind-5];
    icol = iw[ind-6];

    trk = (TBTR *)&iw[ind];

    Float_t Ebeam, P4in[4], Pin, P4e[4], Pe, Q2, AMe, AMp, AMk, AMpi;
    Float_t MMe, MM2e;

// masses
    AMp = 0.938272;
    AMk = 0.493677;
    AMpi= 0.140000;
    AMe = 0.000511;
// beam energy
    Ebeam = 1.6; // take from EPICS
// input beam
    P4in[0] = 0.;
    P4in[1] = 0.;
    P4in[2] = sqrt(Ebeam*Ebeam - AMe*AMe);
    P4in[3] = Ebeam + AMp;


    for(j=0; j<irow; j++)
    {
      if(trk->q<0.999999 || trk->q>1.000001) continue;
      nevents1 ++;
// output electron
      P4e[0] = trk->px;
      P4e[1] = trk->py;
      P4e[2] = trk->pz;
      Pe     = sqrt(P4e[0]*P4e[0] + P4e[1]*P4e[1] + P4e[2]*P4e[2]);
      P4e[3] = sqrt(Pe*Pe + AMe*AMe);
// Q2 observed
      Q2 = 2.*Ebeam*P4e[3]*(1. - trk->pz/Pe);
      qsqrt->Fill(Q2);
// W (or MM) if only electron is measured
      MM2e = 2.*AMp*(Ebeam - P4e[3]) + AMp*AMp - Q2;
      if(MM2e > 0.)
      {
        MMe = sqrt(MM2e);
        melal->Fill(MMe);
        ebeam->Fill(Pe);

        if((ind1 = etNlink(iw,"TGBI",0)) > 0)
        {
          itmp = (iw[ind1] >> 15) & 0x1;
          itmp0 = (Int_t)(Q2*QRES);
          itmp1 = (Int_t)(MMe*MMRES);
          if(itmp0>=0 && itmp0<QRANGE && itmp1>=0 && itmp1<MMRANGE)
          {
            if(itmp==1)
		    {
              melal1->Fill(MMe);
              if(Q2 < fQ1)               melal11->Fill(MMe);
              if(Q2 >= fQ1 && Q2 <= fQ2) melal12->Fill(MMe);
              if(Q2 > fQ2)               melal13->Fill(MMe);
              Nhel[itmp0][itmp1] ++;
		    }
            else if(itmp==0)
		    {
              melal2->Fill(MMe);
              if(Q2 < fQ1)               melal21->Fill(MMe);
              if(Q2 >= fQ1 && Q2 <= fQ2) melal22->Fill(MMe);
              if(Q2 > fQ2)               melal23->Fill(MMe);
              Nhelbar[itmp0][itmp1] ++;
		    }
            else
		    {
              printf("CMSda: ERROR: helicity=%d\n",itmp);
              exit(0);
		    }
          }
        }


        //if(MMe > 0.8 && MMe < 1.2)
        //{
        //  melel->Fill(MMe);
        //}
      }
      else
      {
        MMe = 0.;
      }

      //printf("track %d    x=%f y=%f z=%f   Px=%f Py=%f Pz=%f Q=%f\n",
      //      j,trk->x,trk->y,trk->z,trk->px,trk->py,trk->pz,trk->q);
      trk++;
    }

/*
    //printf("irow=%d icol=%d\n",irow,icol);
    rw = (Float_t *)iw;
    for(j=0; j<irow; j++)
    {
      //printf("track %d    x=%f y=%f z=%f   Px=%f Py=%f Pz=%f Q=%f\n",
      //j,rw[ind+0],rw[ind+1],rw[ind+2],rw[ind+3],rw[ind+4],rw[ind+5],rw[ind+6]);

      if(rw[ind+0] > 0.1)
      {
        target1->Fill(rw[ind+1]);
        target2->Fill(rw[ind+2]);
        pt = sqrt(rw[ind+3]*rw[ind+3]+rw[ind+4]*rw[ind+4])/rw[ind+5];
        if(pt > 2.1) target3->Fill(rw[ind+2]);
      }
      ind += icol;
    }
*/

  }


  // Track Reconstruction Bank

  ret = 0;
  ret |= CMbos2root(iw);

/*
  ret |= CMgetHistFromBOS(iw,501);
  ret |= CMgetHistFromBOS(iw,502);
  ret |= CMgetHistFromBOS(iw,503);
  ret |= CMgetHistFromBOS(iw,504);
*/

/*
  ret |= CMgetHistFromBOS(iw,87);
  ret |= CMgetHistFromBOS(iw,187);
*/

/* use HIST banks !!!
  if((ind = etNlink(iw,"TRAK",0)) > 0)
  {
    Float_t mom, th, phi;
    Int_t isec;

    nevents1 ++;
    ret = 1;
    //printf("found TRAK bank, ind=%d\n",ind);
    irow = iw[ind-5];
    icol = iw[ind-6];

    ana = (TRevent *)&iw[ind];
    mom = (Float_t)(ana->track[0].trkp[0].p);
//printf("mom=%f\n",mom);
    if(mom>0.2)
    {
      mom1->Fill(mom*1000.);
      target1->Fill((Float_t)(ana->track[0].trkp[0].z));
      target2->Fill((Float_t)(ana->track[0].trkp[0].x));
      target3->Fill((Float_t)(ana->track[0].trkp[0].x));

      th  = acos((Float_t)(ana->track[0].trkp[0].Cx))*raddeg;
      phi = atan2((Float_t)(ana->track[0].trkp[0].Cy),
                  (Float_t)(ana->track[0].trkp[0].Cz))*raddeg;
      if(phi < 0.) phi += 360.0;
      if(sdakeys_.zmagn[3] != 0.)
      {
        isec = (phi + 30.)/60. + 1.;
        if(isec > 6) isec -= 6;
      }
      else
      {
        isec = ana->track[0].trkp[0].sector/10.;
      }
phi = phi - 90. + 60.*(isec-1);
if(phi < 0.) phi += 360.0;
if(phi >360.) phi -= 360.0;
//printf("phi=%f isec=%d\n",phi,isec);

      cx->Fill(th);
      cy->Fill(phi);
    }
    dchfill(iw,ana->ntr_out,ana->itag0,ana->ev_out,ana->tag_res);
  }
*/

  // statistic bank

  if((ind=etNlink(iw,"+STA",0)) > 0)
  {
    for(icol=0; icol<8; icol++) scaler[icol] = iw[ind+icol];
  }


exit:

  etNdrop(iw,"PATH",0);

  return(ret);
}



//_____________________________________________________________________________
void CMSda::Hist()
{
// Create histograms and pads if not done yet

  printf("CMSda::Hist() reached\n");
  if(!tdc1)
  {
    adc1 = new TH1F("adc1","Lecroy ADC channel 1",4096,0,4095);
    adc2 = new TH1F("adc2","Lecroy ADC channel 2",4096,0,4095);
    adc3 = new TH1F("adc3","Lecroy ADC channel 3",4096,0,4095);
    adc4 = new TH1F("adc4","Lecroy ADC channel 4",4096,0,4095);
    adc5 = new TH1F("adc5","Lecroy ADC channel 5",4096,0,4095);
    adc5adc6 = new TH2F("adc5adc6","ADC5 vs ADC6",400,100,4100,400,100,4100);
    adc6 = new TH1F("adc6","Lecroy ADC channel 6",4096,0,4095);
    adc7 = new TH1F("adc7","Lecroy ADC channel 7",4096,0,4095);
    adc8 = new TH1F("adc8","Lecroy ADC channel 8",4096,0,4095);
    adc1->SetFillColor(48);
    adc2->SetFillColor(48);
    adc3->SetFillColor(48);
    adc4->SetFillColor(48);
    adc5->SetFillColor(48);
    adc5adc6->SetFillColor(48);
    adc6->SetFillColor(48);
    adc7->SetFillColor(48);
    adc8->SetFillColor(48);

    tdc1     = new TH1F("apmt3","TOF(match-any)",50,0,50);
    tdc1adc1 = new TH2F("adc1_tdc1","ECinner vs ECouter(negative)",
                        110,0.1,1.2,110,0.1,1.2);
    tdc2     = new TH1F("apmt1","ECtotal(negative)",200,0.1,2.1);
    tdc2adc2 = new TH2F("adc2_tdc2","ECinner vs ECouter(pozitive)",
                        110,0.1,1.2,110,0.1,1.2);
    tdc1->SetFillColor(48);
    tdc2->SetFillColor(48);

    target1 = new TH1F("target1","Target (along beam view)",400,-40.,40.);
    target2 = new TH1F("target2","Target (side view)",400,-40.,40.);
    target3 = new TH1F("target3","Target walls (side view)",400,-40.,40.);

    cx = new TH1F("cx","Cx",180,0.,180.);
    cy = new TH1F("cy","Cy",360,0.,360.);
    cz = new TH1F("cz","Cz",100,-1.,1.);

    mom1 = new TH1F("mom1","Secondary particles spectrum (MeV/c)",400,0.0,4000.0);
    ebeam = new TH1F("ebeam","Secondary electron spectrum (GeV/c)",100,0.0,2.0);
    qsqrt = new TH1F("qsqrt","Q**2",75,0.0,1.5);
    melal = new TH1F("melal","all electrons",100,0.7,2.7);
    melal00 = new TH1F("melal00","all electrons, subtract",100,0.7,2.7);
    melal01 = new TH1F("melal01","Q2 < Q_1, subtract",100,0.7,2.7);
    melal02 = new TH1F("melal02","Q_1 <= Q2 <= Q_2, subtract",100,0.7,2.7);
    melal03 = new TH1F("melal03","Q2 > Q_2, subtract",100,0.7,2.7);
    melal00->Sumw2();
    melal01->Sumw2();
    melal02->Sumw2();
    melal03->Sumw2();
    melel = new TH1F("melel","elastic electrons",40,0.8,1.2);
    wmass = new TH1F("wmass","Center of Mass energy (GeV)",100,0.7,2.7);
    melal1 = new TH1F("melal1","all electrons, helicity",100,0.7,2.7);
    melal2 = new TH1F("melal2","all electrons, helicity-bar",100,0.7,2.7);
    melal11 = new TH1F("melal11","Q2 < Q_1, helicity",100,0.7,2.7);
    melal21 = new TH1F("melal21","Q2 < Q_1, helicity-bar",100,0.7,2.7);
    melal12 = new TH1F("melal12","Q_1 < Q2 < Q_2, helicity",100,0.7,2.7);
    melal22 = new TH1F("melal22","Q_1 < Q2 < Q_2, helicity-bar",100,0.7,2.7);
    melal13 = new TH1F("melal13","Q2 > Q_2, helicity",100,0.7,2.7);
    melal23 = new TH1F("melal23","Q2 > Q_2, helicity-bar",100,0.7,2.7);
  }
  else
  {
    Reset();
  }

  this->Create1();
  this->Create2();
  //this->Create3();
  this->Create4();

}



//______________________________________________________________________________
void CMSda::Update()
{
// update drawing

  if(setnum == 1)
  {
    this->Plot1();
  }
  else if(setnum == 2)
  {
    this->Plot2();
  }
  else if(setnum == 3)
  {
    //this->Plot3();
    this->Plot4();
  }
  else if(setnum == 4)
  {
    this->Plot4();
  }
  else
  {
    printf("CMSda::Update(): ERROR - unknown set of histograms\n");
  }

  Int_t time1;
  time1 = time(NULL)-time0;
  if(time1 > 0)
  {
    printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
    ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  }

}


//_____________________________________________________________________________
void CMSda::Status()
{
// check status and update status pad

  if(callprlib) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  if(calltrlib) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
  if(downloaded) m_Arc4->SetFillColor(kGreen); else m_Arc4->SetFillColor(kRed);

  m_Arc11->SetFillColor(kYellow);
  m_Arc12->SetFillColor(kYellow);
  m_Arc13->SetFillColor(kYellow);
  m_Arc14->SetFillColor(kYellow);
  if(setnum == 1)      m_Arc11->SetFillColor(kGreen);
  else if(setnum == 2) m_Arc12->SetFillColor(kGreen);
  else if(setnum == 3) m_Arc13->SetFillColor(kGreen);
  else if(setnum == 4) m_Arc14->SetFillColor(kGreen);

  m_StatPad->Modified();

  this->Update();

}


//_____________________________________________________________________________
void CMSda::ChangeSetnum()
{
// Set a set number

  if(setnum == 1)
  {
    setnum = 2;
  }
  else if(setnum == 2)
  {
    setnum = 3;
  }
  else if(setnum == 3)
  {
    setnum = 4;
  }
  else
  {
    setnum = 1;
  }
  printf("CMSda::ChangeSetnum(): set setnum=%d\n",setnum);

  Status();

}


//_____________________________________________________________________________
void CMSda::ChangePrlib()
{
// (un)set callprlib

  if(callprlib == 0)
  {
    callprlib = 1;
  }
  else
  {
    callprlib = 0;
  }

  printf("CMSda::ChangePrlib(): set callprlib=%d\n",callprlib);

  Status();

}


//_____________________________________________________________________________
void CMSda::ChangeTrlib()
{
// (un)set calltrlib

  if(calltrlib == 0)
  {
    calltrlib = 1;
  }
  else
  {
    calltrlib = 0;
  }

  printf("CMSda::ChangeTrlib(): set calltrlib=%d\n",calltrlib);

  Status();

}


//_____________________________________________________________________________
void CMSda::Buttons()
{
// Create the buttons

  printf("CMSda::Buttons reached, m_StatPad=0x%08x\n",m_StatPad);

  Float_t dx1;
  Float_t dy1;
  Float_t& a=dx1;
  Float_t& b=dy1;
  Float_t xt;
  Float_t xxt;
  Float_t yt;
  Float_t dyt;
  Float_t ddyt;
  Float_t xarc;
  Float_t rarc;
  TText *t = new TText();


  m_StatPad->cd();


  Float_t dxtr;
  Float_t dytr;

  //Float_t& c=dxtr;
  //Float_t& d=dytr;
  //m_StatPad->GetRange(a,b,c,d);
  //printf("dxtr,dytr = %f %f - %f %f\n",c,d,dxtr,dytr);

  dxtr = 0.15;
  dytr = 0.45;
  m_StatPad->Range(0,0,dxtr,dytr);

  xt   = 0.4*dxtr;
  xxt  = 0.5*dxtr;
  yt   = 0.88*dytr;
  dyt  = 0.12*dytr;
  ddyt = 0.4*dyt;
  xarc = 0.8*dxtr;
  rarc = 0.15*dyt;

  Int_t butcolor = 33;
  Float_t dbutton = 0.08;
  Float_t y  = 0.96;
  Float_t dy = 0.014;
  Float_t x0 = 0.05;
  Float_t x1 = 0.95;

  TButton *button; // button coordinates is always in range [0,1]

  char *but1 = "SdaPtr->ChangeSetnum()";
  button = new TButton("Change View",but1,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc11   = new TArc(dxtr*0.2,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc11->Draw();
  m_Arc12   = new TArc(dxtr*0.4,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc12->Draw();
  m_Arc13   = new TArc(dxtr*0.6,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc13->Draw();
  m_Arc14   = new TArc(dxtr*0.8,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc14->Draw();

  y -= dbutton + dy*6.;
  char *but2 = "SdaPtr->ChangePrlib()";
  button = new TButton("Road Finding (ON/OFF)",but2,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc2   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc2->Draw();

  y -= dbutton + dy*6.;
  char *but3 = "SdaPtr->ChangeTrlib()";
  button = new TButton("Track Fiting (ON/OFF)",but3,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc3   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc3->Draw();

  y -= dbutton + dy*6.;
  char *but4 = "SdaPtr->Download()";
  button = new TButton("Download",but4,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc4   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc4->Draw();

  Status();

}


//______________________________________________________________________________
void CMSda::Reset()
{
// Reset histograms

  Int_t i;

  for(i=0; i<NHIST; i++) {delete hist1[i]; hist1[i]=NULL;}
  for(i=0; i<NHIST; i++) {delete hist2[i]; hist2[i]=NULL;}

  adc1->Reset();
  adc2->Reset();
  adc3->Reset();
  adc4->Reset();
  adc5->Reset();
  adc5adc6->Reset();
  adc6->Reset();
  adc7->Reset();
  adc8->Reset();

  tdc1->Reset();
  tdc1adc1->Reset();
  tdc2->Reset();
  tdc2adc2->Reset();

  target1->Reset();
  target2->Reset();
  target3->Reset();

  cx->Reset();
  cy->Reset();
  cz->Reset();

  mom1->Reset();
  ebeam->Reset();
  qsqrt->Reset();
  melal->Reset();
  melal00->Reset();
  melal01->Reset();
  melal02->Reset();
  melal03->Reset();
  melal1->Reset();
  melal2->Reset();
  melal11->Reset();
  melal21->Reset();
  melal12->Reset();
  melal22->Reset();
  melal13->Reset();
  melal23->Reset();
  melel->Reset();
  wmass->Reset();

  // clean some variables
  Int_t j;
  for(j=0; j<NDIM; j++)
  {
    scaler[j] = 0;
    oldscaler[j] = 0;
    overflow[j] = 0;
  }
  for(j=0; j<4; j++) scaler2[j] = 0.0;

}



//______________________________________________________________________________
void CMSda::Create1()
{
// Create pads

  m_Pad->cd();
  if(!pad11)
  {
    pad11 = new TPad("pad11","This is pad11",0.001,0.760,0.299,0.999,21);
    pad12 = new TPad("pad12","This is pad12",0.001,0.510,0.299,0.759,21);
    pad13 = new TPad("pad13","This is pad13",0.001,0.260,0.299,0.509,21);
    pad14 = new TPad("pad14","This is pad14",0.001,0.001,0.299,0.259,21);
    pad15 = new TPad("pad15","This is pad15",0.300,0.510,0.649,0.999,21);
    pad16 = new TPad("pad16","This is pad16",0.300,0.001,0.649,0.509,21);
    pad17 = new TPad("pad17","This is pad17",0.650,0.001,0.999,0.339,21);
    pad18 = new TPad("pad18","This is pad18",0.650,0.340,0.999,0.999,21);
  }

}

//______________________________________________________________________________
void CMSda::Delete1()
{
// Delete pads

  m_Pad->cd();
  if(pad11)
  {
    delete pad11;
    delete pad12;
    delete pad13;
    delete pad14;
    delete pad15;
    delete pad16;
    delete pad17;
    delete pad18;
  }

}

//______________________________________________________________________________
void CMSda::Plot1()
{
// Draw pads

  pad11->cd();
  if(hist1[21]) hist1[21]->Draw();
  pad12->cd();
  if(hist1[22]) hist1[22]->Draw();
  pad14->cd();
  if(hist1[23]) hist1[23]->Draw();
  pad15->cd();
  if(hist1[24]) hist1[24]->Draw();
  mom1->Draw();

  pad13->cd();
  if(hist1[25]) hist1[25]->Draw();
  pad16->cd();
  if(hist1[26]) hist1[26]->Draw();


  // Draw scalers

  Option_t *title2[4];
  title2[0] = "Total";
  title2[1] = "PREC";
  title2[2] = "HBTR";
  title2[3] = "TBTR";
  if(scaler[0] > 0)
  {
    scaler2[0] = (Float_t)scaler[1]/(Float_t)scaler[0];
    scaler2[1] = (Float_t)scaler[2]/(Float_t)scaler[0];
    scaler2[2] = ((Float_t)scaler[3]*2.)/(Float_t)scaler[0];
    scaler2[3] = 0.0;
  }
  pad18->cd();
  char counter[18];
  Int_t j;
  for(j=0; j<8; j++)
  {
    if(!overflow[j] && oldscaler[j] > scaler[j]) overflow[j] = 1;
    if(!overflow[j]) oldscaler[j] = scaler[j];
    if(overflow[j])
    {
      sprintf(counter,"%1d*%09u",j,scaler[j]);
    }
    else
    {
      sprintf(counter,"%1d=%09u",j,scaler[j]);
    }
    if(scal[j])
    {
      delete scal[j];
      scal[j] = (TText *)0;
    }
    scal[j] = new TText(0.5,0.92-0.12*j,counter);
    scal[j]->SetTextAlign(22);
    scal[j]->SetTextSize(0.18);
    scal[j]->Draw();
  }
  pad17->cd();
  char counter2[18];
  for(j=8; j<12; j++)
  {
    sprintf(counter2,"%7.7s=%5.2f",title2[j-8],scaler2[j-8]);
    if(scal[j])
    {
      delete scal[j];
      scal[j] = (TText *)0;
    }
    scal[j] = new TText(0.5,0.88-0.25*(j-8),counter2);
    scal[j]->SetTextAlign(22);
    scal[j]->SetTextSize(0.18);
    scal[j]->Draw();
  }



  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  pad11->Draw();
  pad12->Draw();
  pad13->Draw();
  pad14->Draw();
  pad15->Draw();
  pad16->Draw();
  pad17->Draw();
  pad18->Draw();

}

//______________________________________________________________________________
void CMSda::Create2()
{
// Create pads

  m_Pad->cd();
  if(!pad21)
  {
    pad21 = new TPad("pad21","This is pad21",0.001,0.500,0.500,0.999,21);
    pad22 = new TPad("pad22","This is pad22",0.001,0.001,0.500,0.499,21);
    pad23 = new TPad("pad23","This is pad23",0.501,0.500,0.999,0.999,21);
    pad24 = new TPad("pad24","This is pad24",0.501,0.001,0.999,0.499,21);
  }


/*
  pad21->cd();
  if(hist1[15]) hist1[15]->Draw();
  //if(hist1[15]) hist1[15]->Draw("E0");


  pad22->cd();
  //if(hist1[151]) hist1[151]->Draw();
  target1->Draw();

  pad23->cd();
  //if(hist1[154]) hist1[154]->Draw();
  target2->Draw();

  pad24->cd();
  pad24->SetLogy();
  //if(hist1[153]) hist1[153]->Draw();
  target3->Draw();
*/

}

//______________________________________________________________________________
void CMSda::Delete2()
{
// Delete pads

  m_Pad->cd();
  if(pad21)
  {
    delete pad21;
    delete pad22;
    delete pad23;
    delete pad24;
  }

}

//______________________________________________________________________________
void CMSda::Plot2()
{

// Draw pads

  Int_t i1=21,i2=22,i3=24,i4=26;

if(hist1[i1]!=NULL)
{
  printf("1 -> 0x%08x\n",hist1[i1]);
  pad21->cd();
  hist1[i1]->Draw();
}
if(hist1[i2]!=NULL)
{
  printf("2 -> 0x%08x\n",hist1[i2]);
  pad22->cd();
  hist1[i2]->Draw();
}
if(hist1[i3]!=NULL)
{
  printf("3 -> 0x%08x\n",hist1[i3]);
  pad23->cd();
  hist1[i3]->Draw();
}
if(hist1[i4]!=NULL)
{
  printf("4 -> 0x%08x\n",hist1[i4]);
  pad24->cd();
  hist1[i4]->Draw();
}

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  pad21->Draw();
  pad22->Draw();
  pad23->Draw();
  pad24->Draw();

}




//______________________________________________________________________________
void CMSda::Create3()
{
// Create pads

  if(!pad31)
  {
    pad31 = new TPad("pad31","This is pad31",0.001,0.500,0.500,0.999,21);
    pad32 = new TPad("pad32","This is pad32",0.001,0.001,0.500,0.499,21);
    pad33 = new TPad("pad33","This is pad33",0.501,0.500,0.999,0.999,21);
    pad34 = new TPad("pad34","This is pad34",0.501,0.001,0.999,0.499,21);

    pad31->cd();
    melal1->Draw();
    pad32->cd();
    melal2->Draw();
    pad33->cd();
    melal00->Draw("E0");
  }


// Create the buttons for the pad34

  pad34->cd();

  Int_t butcolor = 33;
  Float_t dbutton = 0.08;
  Float_t y  = 0.1;
  Float_t dy = 0.014;
  Float_t ddy = 0.2;
  Float_t x0 = 0.01;
  Float_t x1 = 0.99;
  Float_t dx = 0.05;
  char *but;
  Float_t x0shift;
  TButton *button; // button coordinates is always in range [0,1]
  TText *t;
  TLine *line;

  x0shift = (x1 - x0)/4.;
  x0 += 0.025;
  xMMe1 = x0+dx*2.;
  xMMe2 = x0+x0shift+dx*2.;
  xMMe3 = x0+x0shift*2.+dx*2.;
  xMMe4 = x0+x0shift*3.+dx*2.;


  but = "SdaPtr->MMe1Down()";
  button = new TButton("<",but,x0,y-dbutton,x0+dx,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->Q1Down()";
  button = new TButton("<",but,x0,y+ddy-dbutton,x0+dx,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->MMe1Up()";
  button = new TButton(">",but,x0+dx*3.,y-dbutton,x0+dx*4.,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->Q1Up()";
  button = new TButton(">",but,x0+dx*3.,y+ddy-dbutton,x0+dx*4.,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  x0 += x0shift;

  but = "SdaPtr->MMe2Down()";
  button = new TButton("<",but,x0,y-dbutton,x0+dx,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->Q2Down()";
  button = new TButton("<",but,x0,y+ddy-dbutton,x0+dx,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->MMe2Up()";
  button = new TButton(">",but,x0+dx*3.,y-dbutton,x0+dx*4.,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->Q2Up()";
  button = new TButton(">",but,x0+dx*3.,y+ddy-dbutton,x0+dx*4.,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  x0 += x0shift;

  but = "SdaPtr->MMe3Down()";
  button = new TButton("<",but,x0,y-dbutton,x0+dx,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->PBDown()";
  button = new TButton("<",but,x0,y+ddy-dbutton,x0+dx,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->MMe3Up()";
  button = new TButton(">",but,x0+dx*3.,y-dbutton,x0+dx*4.,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->PBUp()";
  button = new TButton(">",but,x0+dx*3.,y+ddy-dbutton,x0+dx*4.,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();


  x0 += x0shift;

  but = "SdaPtr->MMe4Down()";
  button = new TButton("<",but,x0,y-dbutton,x0+dx,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->TChange()";
  button = new TButton("T",but,x0,y+ddy-dbutton,x0+dx,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->MMe4Up()";
  button = new TButton(">",but,x0+dx*3.,y-dbutton,x0+dx*4.,y);
  button->SetFillColor(butcolor);
  button->Draw();

  but = "SdaPtr->Menu()";
  button = new TButton("M",but,x0+dx*3.,y+ddy-dbutton,x0+dx*4.,y+ddy);
  button->SetFillColor(butcolor);
  button->Draw();

// Draw labels

  line = new TLine(0.01,ddy,0.99,ddy);
  line->SetLineColor(kRed);
  line->Draw();

  t = new TText(0.5,0.15,"  Elastic Peak             Delta Peak   ");
  t->SetTextAlign(22);
  t->SetTextSize(0.10);
  t->SetTextColor(kBlue);
  t->Draw();

  line = new TLine(0.01,ddy*2.,0.99,ddy*2.);
  line->SetLineColor(kRed);
  line->Draw();

  t = new TText(0.5,0.15+ddy,"     Q2 range               BeamPolar.   Target");
  t->SetTextAlign(22);
  t->SetTextSize(0.10);
  t->SetTextColor(kBlue);
  t->Draw();

// Draw scalers

  Int_t j;
  for(j=12; j<15; j++)
  {
    scal[j] = new TText();
    scal[j]->SetTextAlign(22);
    scal[j]->SetTextSize(0.10);
    scal[j]->Draw();
  }
  for(j=0; j<4; j++)
  {
    MMText[j] = new TText();
    MMText[j]->SetTextAlign(22);
    MMText[j]->SetTextSize(0.08);
    MMText[j]->Draw();
  }
  for(j=0; j<2; j++)
  {
    QText[j] = new TText();
    QText[j]->SetTextAlign(22);
    QText[j]->SetTextSize(0.08);
    QText[j]->Draw();
  }

  TTText = new TText();
  TTText->SetTextAlign(22);
  TTText->SetTextSize(0.08);
  TTText->Draw();

  PBText = new TText();
  PBText->SetTextAlign(22);
  PBText->SetTextSize(0.08);
  PBText->Draw();

}

//______________________________________________________________________________
void CMSda::Menu()
{
  //fMenu = new TestMainFrame(gClient->GetRoot(), 400, 220);
  fDialog = new TestDialog(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
  //fMsg = new TestMsgBox(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
  //fSlider = new TestSliders(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
}

//______________________________________________________________________________
void CMSda::Delete3()
{
// Delete pads

  m_Pad->cd();
  if(pad31)
  {
    delete pad31;
    delete pad32;
    delete pad33;
    delete pad34;
  }

}

//______________________________________________________________________________
void CMSda::Plot3()
{
// Update histos and scalers

//  The following options are supported for 1-D types:
//    "AH"     : Draw histogram, but not the axis labels and tick marks
//    "B"      : Bar chart option
//    "C"      : Draw a smooth Curve througth the histogram bins
//    "E"      : Draw error bars
//    "E0"     : Draw error bars including bins with o contents
//    "E1"     : Draw error bars with perpendicular lines at the edges
//    "E2"     : Draw error bars with rectangles
//    "E3"     : Draw a fill area througth the end points of the vertical error bars
//    "E4"     : Draw a smoothed filled area through the end points of the error bars
//    "L"      : Draw a line througth the bin contents
//    "P"      : Draw current marker at each bin
//    "*H"     : Draw histogram with a * at each bin

  Int_t i, j;
  Float_t tmp0, tmp1, err0, err1, Ptarget, Pbeam, Factor[2];
  Float_t value[4], error[4];
  char counter2[40];
  Option_t *title[6];
  Float_t Ne[2], Nd[2];
  Int_t ibin[4], ibinq[2];

  melal00->Add(melal1,melal2,1.,-1.);

  Factor[0] = 0.098;
  Factor[1] = 0.057; // ???
  Pbeam = fPB/100.; // beam polarization (%)


// calculate a target polarization

  for(j=0; j<2; j++) Ne[j] = Nd[j] = 0;
  ibinq[0] = (Int_t)(fQ1*QRES);
  ibinq[1] = (Int_t)(fQ2*QRES);
  ibin[0] = (Int_t)(fMMe1*MMRES);
  ibin[1] = (Int_t)(fMMe2*MMRES);
  ibin[2] = (Int_t)(fMMe3*MMRES);
  ibin[3] = (Int_t)(fMMe4*MMRES);
  for(i=ibinq[0]; i<ibinq[1]; i++)
  {
    for(j=ibin[0]; j<ibin[1]; j++)
    {
      Ne[0] += Nhel[i][j];
      Ne[1] += Nhelbar[i][j];
    }
    for(j=ibin[2]; j<ibin[3]; j++)
    {
      Nd[0] += Nhel[i][j];
      Nd[1] += Nhelbar[i][j];
    }
  }

  tmp0 = tmp1 = 0.;
  err0 = err1 = 0.;
  Ptarget = 0.;
  if((tmp0=Ne[0]+Ne[1]) > 0.)
  {
    err0 = 1./sqrt(tmp0);
    tmp0 = ((Float_t)(Ne[0]-Ne[1]))/tmp0;
  }
  if((tmp1=Nd[0]+Nd[1]) > 0.)
  {
    err1 = 1./sqrt(tmp1);
    tmp1 = ((Float_t)(Nd[0]-Nd[1]))/tmp1;
  }

  Ptarget = (tmp0-tmp1)/(Factor[fT]*Pbeam);
printf("use Factor=%f, Pbeam=%f\n",Factor[fT],Pbeam);
  //printf("Raw asymmetries:\n  elastic peak (%4.1f+-%3.1f)%%, delta peak (%4.1f+-%3.1f)%%\n",
  //   tmp0*100.,err0*100.,tmp1*100.,err1*100.);
  //printf("  target polarization: (%4.1f+-%3.1f)%%\n",Ptarget*100.);

  value[0] = tmp0*100.;
  value[1] = tmp1*100.;
  value[2] = Ptarget*100.;
  error[0] = err0*100.;
  error[1] = err1*100.;
  error[2] = (err0+err1)/(Factor[fT]*Pbeam)*100.;

  title[0] = "Elastic peak";
  title[1] = "Delta   peak";
  title[2] = "Ptarget     ";

  for(j=12; j<15; j++)
  {
    sprintf(counter2,"%21.21s = (%4.1f+-%3.1f)%%",title[j-12],value[j-12],error[j-12]);
    scal[j]->SetText(0.5,0.88-0.12*(j-12),counter2);
  }

// Draw MMe values in between buttons

  value[0] = fMMe1;
  value[1] = fMMe2;
  value[2] = fMMe3;
  value[3] = fMMe4;
  error[0] = xMMe1;
  error[1] = xMMe2;
  error[2] = xMMe3;
  error[3] = xMMe4;

  for(j=0; j<4; j++)
  {
    sprintf(counter2,"%4.2f",value[j]);
    MMText[j]->SetText(error[j],0.06,counter2);
  }

  value[0] = fQ1;
  value[1] = fQ2;
  for(j=0; j<2; j++)
  {
    sprintf(counter2,"%4.2f",value[j]);
    QText[j]->SetText(error[j],0.06+0.2,counter2);
  }

  sprintf(counter2,"%2.0f%%",fPB);
  PBText->SetText(error[2],0.06+0.2,counter2);

  if(fT==0) sprintf(counter2,"%3.3s","NH3");
  else      sprintf(counter2,"%3.3s","ND3");
  TTText->SetText(error[3],0.06+0.2,counter2);


// Draw pads

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  pad31->Draw();
  pad32->Draw();
  pad33->Draw();
  pad34->Draw();

}



//______________________________________________________________________________
void CMSda::Create4()
{
// Create pads

  m_Pad->cd();
  if(!pad41)
  {
    pad41 = new TPad("pad41","This is pad41",0.001,0.500,0.500,0.999,21);
    pad42 = new TPad("pad42","This is pad42",0.001,0.001,0.500,0.499,21);
    pad43 = new TPad("pad43","This is pad43",0.501,0.500,0.999,0.999,21);
    pad44 = new TPad("pad44","This is pad44",0.501,0.001,0.999,0.499,21);
  }

/*
  pad41->cd();
  adc5->Draw();
  pad42->cd();
  adc6->Draw();
  pad43->cd();
  adc7->Draw();
  pad44->cd();
  adc5adc6->Draw();
  //adc8->Draw();
*/


}

//______________________________________________________________________________
void CMSda::Delete4()
{
// Delete pads

  m_Pad->cd();
  if(pad41)
  {
    delete pad41;
    delete pad42;
    delete pad43;
    delete pad44;
  }

}

//______________________________________________________________________________
void CMSda::Plot4()
{

// Draw pads

  Int_t i1=501,i2=502,i3=151,i4=154;

if(hist1[i1]!=NULL)
{
  printf("1 -> 0x%08x\n",hist1[i1]);
  pad41->cd();
  hist1[i1]->Draw();
}
if(hist1[i2]!=NULL)
{
  printf("2 -> 0x%08x\n",hist1[i2]);
  pad42->cd();
  hist1[i2]->Draw();
}
if(hist1[i3]!=NULL)
{
  printf("3 -> 0x%08x\n",hist1[i3]);
  pad43->cd();
  hist1[i3]->Draw();
}
if(hist1[i4]!=NULL)
{
  printf("4 -> 0x%08x\n",hist1[i4]);
  pad44->cd();
  hist1[i4]->Draw();
}

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  pad41->Draw();
  pad42->Draw();
  pad43->Draw();
  pad44->Draw();

}










