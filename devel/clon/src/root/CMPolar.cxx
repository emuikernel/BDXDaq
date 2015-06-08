
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// CMPolar                                                              //
//                                                                      //
// Moller Polarimeter Class                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include <TROOT.h>
#include <TText.h>
#include <TPad.h>
#include <TArc.h>

#include "etbosio.h"

#include "CMPolar.h"

#include "CMDisplay.h" // to get gCMDisplay
extern CMDisplay *gCMDisplay;

#include "CMDaq.h" // to get gCMDaq
extern CMDaq *gCMDaq;

ClassImp(CMPolar)

static Int_t ibnk, iptr, str_old, hel_old;
static Int_t nevent = 0;
static Int_t mynev  = 0;
static Int_t mynev0 = 0;
static Float_t fnev;

/* L.R Moller coincidences channel number (channel number starting from 0) */
#define NCOIN   5
/* L.R Moller accidentals channel number */
#define NACCI   6
/* normalizing scaler in HLS bank */
/* currently =3 -> SLM */
#define NCHAN   3
/* delayed mode */
#define NDELAY  0

//_____________________________________________________________________________
CMPolar::CMPolar(Option_t *fname) : CMClient()
{
// Create an CMPolar object

}

//_____________________________________________________________________________
void CMPolar::Download()
{
// call it ones and forever ...

  Int_t j;

  printf("CMPolar::Download() reached\n");

  ibnk = 0;
  iptr = 0;
  str_old = -1;
  hel_old = 0;
  mynev  = 0;
  mynev0 = 0;

  this->Hist();
}

//_____________________________________________________________________________
void CMPolar::Prestart(Int_t *jw)
{
// call it in the begining of every run

}

//_____________________________________________________________________________
int CMPolar::Event(Int_t *jw)
{
// Process one event

  Int_t ind,ind0,irow,icol,irow0,icol0,i,j,ret,hel,str,index;
  short *hit;

  //printf("CMpolar::Event() reached\n");

  ret = 0;

  // get 'HEAD' bank

  if((ind = etNlink(jw,"HEAD",0)) != 0)
  {
    if(jw[ind+4] >= 10) // not physics event (EPIC etc)
    {
      //printf("CMPolar: event number 0 - goto next event\n");
      return(ret);
    }
    if(mynev == 0)
    {
      mynev ++;
      mynev0 = jw[ind+2] - 1;
    }
    else
    {
      mynev ++;
      //printf("mynev=%d mynev0=%d event#=%d\n",mynev,mynev0,jw[ind+2]);
      if((jw[ind+2]-mynev0)!=0) fnev=((float)mynev/((float)(jw[ind+2]-mynev0)))*100.;
    }
    //if(mynev%nevents_max==0) printf("===> got %d(%6.2f)%% events\n",mynev,fnev);

    if(jw[ind+6] == 17)
    {
      printf("Prestart ...\n");
      Reset();
      tdc1->Reset();
      adc1->Reset();
      adc3->Reset();
      tdc2->Reset();
      adc2->Reset();
      tdc2adc2->Reset();
      adc4->Reset();
      for(j=0; j<NDIM; j++)
      {
        scaler[j] = 0;
        scaler_h[j] = 0;
        scaler_hbar[j] = 0;
        oldscaler[j] = 0;
        overflow[j] = 0;
      }
      for(j=0; j<4; j++) scaler2[j] = 0.0;
      old_asym=0.0;
      nev=0;
      mynev = 0;
    }
    else if(jw[ind+6] == 18)
    {
      printf("Go ...\n");
    }
    else if(jw[ind+6] == 20)
    {
      printf("End ...\n");
    }
    ret = ret | 1;
  }




  // get 'HLS ' banks

  if((ind0 = etNlink(jw,"HLS ",0)) > 0)
  {
    nevent++;
    irow0 = jw[ind0-5];
    icol0 = jw[ind0-6];
    //printf("irow0=%d icol0=%d\n",irow0,icol0);

    // process data directry from the BOS banks

    for(i=0; i<irow0; i++)
    {
      hel = (jw[ind0+i*icol0]&0x40000000)>>30;
      str = (jw[ind0+i*icol0]&0x80000000)>>31;
      if(str_old == -1 && str == 0) continue; // starting run with str=1
      if(str != str_old)
      {
        str_old = str;
      }
      else
      {
        if(!str)
        {
          ibnk = 0;
          s_1[0][iptr] = (jw[ind0+i*icol0+NCOIN])&0x00ffffff;
          s_1[1][iptr] = (jw[ind0+i*icol0+NACCI])&0x00ffffff;
          s_1[2][iptr] = hel;
          s_1[3][iptr] = (jw[ind0+i*icol0+NCHAN])&0x00ffffff;
          //printf("str=%1d hel=%1d s_1=%d %d\n",str,hel,s_1[0][iptr],s_1[1][iptr]);
          iptr++;
          //printf("iptr=%d\n",iptr);
        }
        else
        {
          ibnk = 1;
          s_2[0][iptr] = (jw[ind0+i*icol0+NCOIN])&0x00ffffff;
          s_2[1][iptr] = (jw[ind0+i*icol0+NACCI])&0x00ffffff;
          s_2[2][iptr] = hel;
          s_2[3][iptr] = (jw[ind0+i*icol0+NCHAN])&0x00ffffff;
          //printf("str=%1d hel=%1d s_2=%d %d\n",str,hel,s_2[0][iptr],s_2[1][iptr]);
        }
      }
    }

    // fill scalers taking into account delayed helicity info
    //printf("1: iptr = %d\n",iptr);
    //printf("scalers1 = %9d %9d %9d %9d\n",scaler[0],scaler[1],scaler[2],scaler[3]);
    for(j=0; j<iptr-NDELAY; j++)
    {
      //printf("0: %d\n",s_1[2][j+NDELAY]);
      if(s_1[2][j+NDELAY])
      {
        //printf("1: %3d %3d %3d %3d\n",s_1[0][j],s_2[0][j],s_1[1][j],s_2[1][j]);
        scaler[0] += s_1[0][j];
        scaler[1] += s_2[0][j];
        scaler[2] += s_1[1][j];
        scaler[3] += s_2[1][j];
        HLSscaler[0][2] += s_1[3][j];
        HLSscaler[1][2] += s_2[3][j];
      }
      else
      {
        //printf("2: %3d %3d %3d %3d\n",s_2[0][j],s_1[0][j],s_2[1][j],s_1[1][j]);
        scaler[0] += s_2[0][j];
        scaler[1] += s_1[0][j];
        scaler[2] += s_2[1][j];
        scaler[3] += s_1[1][j];
        HLSscaler[0][2] += s_2[3][j];
        HLSscaler[1][2] += s_1[3][j];
      }
      //printf(" %3d -> %3d %3d (%1d) %3d %3d (%1d) -> hel=%1d\n",
      //                 j,s_2[0][j],s_2[1][j],s_2[2][j],
      //                 s_1[0][j],s_1[1][j],s_1[2][j],s_1[2][j+NDELAY]);
    }
    //printf("scalers2 = %9d %9d %9d %9d\n",scaler[0],scaler[1],scaler[2],scaler[3]);
    for(j=0; j<NDELAY; j++)
    {
      s_1[0][j] = s_1[0][iptr-NDELAY+j];
      s_2[0][j] = s_2[0][iptr-NDELAY+j];
      s_1[1][j] = s_1[1][iptr-NDELAY+j];
      s_2[1][j] = s_2[1][iptr-NDELAY+j];
      s_1[2][j] = s_1[2][iptr-NDELAY+j];
      s_2[2][j] = s_2[2][iptr-NDELAY+j];
      s_1[3][j] = s_1[3][iptr-NDELAY+j];
      s_2[3][j] = s_2[3][iptr-NDELAY+j];
    }
    if(ibnk)
    {
      s_2[0][NDELAY] = s_2[0][iptr];
      s_2[1][NDELAY] = s_2[1][iptr];
      s_2[2][NDELAY] = s_2[2][iptr];
      s_2[3][NDELAY] = s_2[3][iptr];
    }
    iptr = NDELAY;
  }
  //else
  //{
  //  printf("CMPolar: error - no HLS !!!\n");
  //}


  // get MPTA (tdc & adc) bank

  if((ind = etNlink(jw,"MPTA",0)) > 0)
  {
    irow = jw[ind-5];
    icol = jw[ind-6];
    //printf("irow=%d icol=%d\n",irow,icol);
    hit = (short *) &jw[ind];
    for(i=0; i<irow; i++)
    {
      index = i*icol;
      tdc[i] = (float)(hit[index+1]);
      adc[i] = (float)(hit[index+2]);
      //printf("  nevent %6d index %2d id %2d tdc %f adc %f\n",
      //             nevent,index,hit[index],tdc[i],adc[i]);
    }

    // fill histograms

    adc1->Fill(adc[0]);
    adc2->Fill(adc[1]);
    //adc3->Fill(-50.0);
    //adc1adc2->Fill(adc[0],adc[1]);
    //tdc1adc1->Fill(tdc[0],adc[0]);
    //adc4->Fill(old_asym-scaler2[2]);
    tdc2adc2->Fill(adc[14],adc[15]);

    ret = ret | 1;
  }

  //printf("CMPolar::Event() ret=%d\n",ret);
  return(ret);
}


//______________________________________________________________________________
void CMPolar::Hist()
{
// Create histograms and pads if not done yet

  Int_t j;

  // Histograms
  if(!tdc1)
  {
    tdc1     = new TH1F("apmt3","apmt3",100,-500,500);
    adc1     = new TH1F("adc1","adc1",100,0,4096);
    //     tdc1adc1 = new TH2F("adc1_tdc1","adc1 vs tdc1",40,0,1000,40,0,4096);
    adc3 = new TH1F("Asym","Asym",100,-100,100);
    tdc2     = new TH1F("apmt1","apmt1",100,-500,500);
    adc2     = new TH1F("adc2","adc2",100,0,4096);
    tdc2adc2 = new TH2F("adc2_tdc2","Raster X -vs- Raster Y",40,0,4096,40,0,4096);
    //     adc1adc2 = new TH2F("adc1_adc2","adc2 vs adc1",40,0,4096,40,0,4096);
    adc4 = new TH1F("adc4","adc4",100,-200,200);

    // Set a fill color for the TH1F
    tdc1->SetFillColor(48);
    adc1->SetFillColor(48);
    tdc2->SetFillColor(48);
    adc2->SetFillColor(48);
  }
  else
  {
    tdc1->Reset();
    adc1->Reset();
    //     tdc1adc1->Reset();
    adc3->Reset();
    tdc2->Reset();
    adc2->Reset();
    tdc2adc2->Reset();
    //     adc1adc2->Reset();
    adc4->Reset();
  }

  // Pads
  m_Pad->cd();
  if(!pad1)
  {
    pad1 = new TPad("pad1","This is pad1",0.01,0.76,0.30,0.99,21);
    pad2 = new TPad("pad2","This is pad2",0.01,0.51,0.30,0.74,21);
    pad3 = new TPad("pad3","This is pad3",0.32,0.51,0.70,0.99,21);
    pad4 = new TPad("pad4","This is pad4",0.01,0.26,0.30,0.49,21);
    pad5 = new TPad("pad5","This is pad5",0.01,0.01,0.30,0.24,21);
    pad6 = new TPad("pad6","This is pad6",0.32,0.01,0.70,0.49,21);
    pad7 = new TPad("pad7","This is pad7",0.72,0.01,0.99,0.34,21);
    pad8 = new TPad("pad8","This is pad8",0.72,0.35,0.99,0.99,21);

    pad1->cd();
    adc1->Draw();
    pad2->cd();
    adc2->Draw();
    pad3->cd();
    adc3->Draw();
    pad4->cd();
    tdc2->Draw();
    pad5->cd();
    tdc1->Draw();
    pad6->cd();
    tdc2adc2->Draw();
    //adc4->Draw();

  }

  pad8->cd();
  for(j=0; j<8; j++)
  {
    scal[j] = new TText();
    scal[j]->SetTextAlign(22);
    scal[j]->SetTextSize(0.18);
    scal[j]->Draw();
  }

  pad7->cd();
  for(j=8; j<12; j++)
  {
    scal[j] = new TText();
    scal[j]->SetTextAlign(22);
    scal[j]->SetTextSize(0.18);
    scal[j]->Draw();
  }

}


//______________________________________________________________________________
void CMPolar::Reset()
{
   // Reset some variables
   nev = 0;
   Int_t j;
   for(j=0; j<NDIM; j++)
   {
     adc[j]       = 0.0;
     tdc[j]       = 0.0;
     scaler[j]    = 0;
     scaler_h[j]    = 0;
     scaler_hbar[j]    = 0;
     oldscaler[j] = 0;
     overflow[j]  = 0;
   }
   for(j=0; j<4; j++) scaler2[j] = 0.0;
   old_asym=0;
   nevent_stat=0;
}



//______________________________________________________________________________
void CMPolar::Update()
{
// update drawing

printf("CMPolar::Update() reached\n");
  Int_t j;

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  pad4->Draw();
  pad5->Draw();
  pad6->Draw();
  pad7->Draw();
  pad8->Draw();


  // set secondary scalers
  Option_t *title2[4];
  title2[0] = "(0-2)/4";
  if(scaler[4] != 0)
    scaler2[0] = ((Float_t)scaler[0] - (Float_t)scaler[2])/(Float_t)scaler[4];
  title2[1] = "(1-3)/5";
  if(scaler[5] != 0)
    scaler2[1] = ((Float_t)scaler[1] - (Float_t)scaler[3])/(Float_t)scaler[5];
  title2[2] = "P(%)";
  Float_t tmp1, tmp2, tmp;
  // If beam current monitor in normalize
  // normalize using PMT in last channel changed for E1 1999
  if(HLSscaler[0][2] && HLSscaler[1][2])
  {
    tmp1 = ((Float_t)scaler[0] - (Float_t)scaler[2])/((Float_t) HLSscaler[0][2]);
    tmp2 = ((Float_t)scaler[1] - (Float_t)scaler[3])/((Float_t) HLSscaler[1][2]);
  }
  else  // un-normalized calculation
  {
    tmp1 = (Float_t)scaler[0] - (Float_t)scaler[2];
    tmp2 = (Float_t)scaler[1] - (Float_t)scaler[3];
  }

  //Reals minus accidentals with normalization
  if((tmp1+tmp2) > 0.000001 || (tmp1+tmp2) < -0.000001)
  {
    if(scaler[0]>0 && scaler[1]>0) old_asym=scaler2[2];
    if(nevent_stat>1 && scaler[0] && scaler[1] )
    {
	  scaler2[2] = (((tmp1-tmp2)/(tmp1+tmp2)) / ((-7./9.)*0.08)) * 100.;
	  printf("polarization=%f (%f %f)\n",scaler2[2],tmp1,tmp2);
      // in the calculation below both scalers are from HLS+ and HLS+ banks
      // so normalization will give measure of the amount of current in each helicity state
      adc3->Fill(scaler2[2]);
	  // use difference in sccumulating PMT scalers (from HLS+ bank) as signal
	  // of when there are about an equal number of events in each helicity state
      //printf("HLS[2]=%d-%d\n",HLSscaler[0][2],HLSscaler[1][2]);
      if(HLSscaler[0][11] && abs((int)(HLSscaler[0][11]-HLSscaler[1][11]))< 10000)
      {
        tdc2->Fill((Float_t) (HLSscaler[0][9]-HLSscaler[1][9])/((Float_t) (HLSscaler[0][9]+HLSscaler[1][9])));
        tdc1->Fill((Float_t) (HLSscaler[0][11]-HLSscaler[1][11])/((Float_t) (HLSscaler[0][11]+HLSscaler[1][11])));
      }
    }
    else
    {
      nevent_stat=1;
      scaler2[3] = 0.0;
    }
  }

  title2[3] = "dP(%)";
  nevent_stat++;
  if(old_asym && nevent_stat && scaler2[2])
  {
    tmp1=old_asym-scaler2[2];
    if(nevent_stat >2 && scaler[0] && scaler[1])
      scaler2[3]=sqrt((scaler2[3]*scaler2[3]*(nevent_stat-2)+tmp1*tmp1)/(nevent_stat-1));
  }
  else
  {
    scaler2[3]=0.0;
  }




  //if(nevent_stat>50 && scaler2[3] < 3.0) printf("Pol = %g +/- %g \t #ev=%d \n",scaler2[2],scaler2[3],nevent_stat);




  // scalers
  pad8->cd();
  char *counter = "00000000000000000";
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
    scal[j]->SetText(0.5,0.92-0.12*j,counter);
  }
  pad7->cd();
  char *counter2 = "00000000000000000";
  for(j=8; j<12; j++)
  {
    sprintf(counter2,"%7.7s=%5.2f",title2[j-8],scaler2[j-8]);
    scal[j]->SetText(0.5,0.88-0.25*(j-8),counter2);
  }

}



