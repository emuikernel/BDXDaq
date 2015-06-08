//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMSpars                                                             //
//                                                                      //
//  CLAS Sparsification Package                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>
#include <TGraph.h>
#include <TNtuple.h>
#include <TFile.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"

#include "CMSpars.h"
CMSpars *gCMSpars; // to use it in buttons

ClassImp(CMSpars)


#define ABS(x)      ((x) < 0 ? -(x) : (x))


//_____________________________________________________________________________
CMSpars::CMSpars(Option_t *fname) : CMClient()
{
// Create one CMSpars object

  gCMSpars = this;

  hbook = new CMHbook();
  viewer = new CMViewer(fname);

  downloaded = 0;

  strcpy(filename,fname);
  printf("CMSpars: Config File >%s<\n",filename);

}


//_____________________________________________________________________________
void CMSpars::Download()
{
//
  Int_t i;

  // read config file

// TEMPORARY !!!
nview=viewer->ReadConfig("/usr/local/clas/parms/pedman/hist.conf",hbook);

  nmap = ReadConfig(filename);
  for(i=0; i<nmap; i++)
  {
    printf("[%3d] %s %s %s %d %s %d %d %f %f\n",i,
         mapname[i], setname[i], subsetname[i],
         dim[i], maptype[i], histid[i], nbin[i], xmin[i], xmax[i]);
  }

  this->Hist();
  this->Buttons();

  downloaded = 1;
}


//_____________________________________________________________________________
void CMSpars::Prestart(Int_t *iw)
{
//
  Int_t i, j;

  prestarted = 1;
  nevents = nevents1 = 0;

  Reset();

}



//_____________________________________________________________________________
void CMSpars::Go(Int_t *iw)
{
//
  time0 = time(NULL);

}


//_____________________________________________________________________________
void CMSpars::End(Int_t *iw)
{
//

}


//_____________________________________________________________________________
int CMSpars::Event(Int_t *iw)
{
// one event processing

  //printf("CMSpars::Event() reached\n");
  Int_t ret;

  nevents ++;

  ret = 1;
  return(ret);
}



//_____________________________________________________________________________
void CMSpars::Hist()
{
// Create views
  Int_t i;
  for(i=0; i<nview; i++) viewer->ViewCreate(i, hbook);

}



//_____________________________________________________________________________
void CMSpars::Update()
{
// update drawing

  m_Pad->cd();
  m_Pad->GetListOfPrimitives()->Clear("nodelete");
  viewer->ViewPlot(iview);

  Int_t time1;
  time1 = time(NULL)-time0;
  if(time1 > 0)
  {
    printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
    ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  }
}

//_____________________________________________________________________________
void CMSpars::Reset()
{
  hbook->Clean();
}

//_____________________________________________________________________________
void CMSpars::Buttons()
{
// Create the buttons

  printf("CMSpars::Buttons reached, m_StatPad=0x%08x\n",m_StatPad);

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
  char tmp[256];

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
  Float_t x0 = 0.01;
  Float_t x01 = 0.54;
  Float_t x02 = 0.58;
  Float_t x1 = 0.98;

  TButton *button; // button coordinates is always in range [0,1]

  char *but1 = "SparsPtr->PreviousView()";
  button = new TButton("Previous View",but1,x0,y-dbutton,x01,y);
  button->SetFillColor(butcolor);
  button->Draw();
  char *but2 = "SparsPtr->NextView()";
  button = new TButton("Next View",but2,x02,y-dbutton,x1,y);
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
  //sprintf(tmp,"SparsPtr->HistView(\"%s\")",cratename);
  //printf("tmp >%s<\n",tmp);
  char *but3 = "SparsPtr->HistView()";
  button = new TButton("View Histograms",but3,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc2   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc2->Draw();

  y -= dbutton + dy*6.;
  char *but4 = "SparsPtr->WhiteCalibration()";
  button = new TButton("Write Calibration",but4,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc3   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc3->Draw();

  y -= dbutton + dy*6.;
  char *but5 = "SparsPtr->MenuCrate()";
  button = new TButton("Crate Name",but5,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc4   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc4->Draw();

  y -= dbutton + dy*6.;
  char *but6 = "SparsPtr->Menu()";
  button = new TButton("Main Menu",but6,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc5   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc5->Draw();

  Status();

}

//_____________________________________________________________________________
void CMSpars::Status()
{
// check status and update status pad

  if(callprlib) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  if(calltrlib) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
  if(downloaded) m_Arc4->SetFillColor(kGreen); else m_Arc4->SetFillColor(kRed);
  if(downloaded) m_Arc5->SetFillColor(kGreen); else m_Arc5->SetFillColor(kRed);

  m_Arc11->SetFillColor(kYellow);
  m_Arc12->SetFillColor(kYellow);
  m_Arc13->SetFillColor(kYellow);
  m_Arc14->SetFillColor(kYellow);
  if(iview == 0)      m_Arc11->SetFillColor(kGreen);
  else if(iview == 1) m_Arc12->SetFillColor(kGreen);
  else if(iview == 2) m_Arc13->SetFillColor(kGreen);
  else if(iview == 3) m_Arc14->SetFillColor(kGreen);

  m_StatPad->Modified();

  this->Update();

}

#define MAXSTRLNG 100
#define NUMFIELD1   9
//_____________________________________________________________________________
Int_t CMSpars::ReadConfig(Option_t *fileconf)
{
  FILE *fd;
  Int_t nmaps, key, number, ngr, ih, iv;
  Int_t nread=0, badline=0, nline, comment, i;
  char str[MAXSTRLNG], *s, nm[10];
  Float_t tmp;

  if((fd=fopen(fileconf,"r")) != NULL)
  {
    nline = 0;
    nmaps = 0;
    do
    {
      s = fgets(str,MAXSTRLNG,fd);
      nline++;
      if(s == NULL)
      {
        if(ferror(fd)) /* check for errors */
        {
          printf("ReadConfig(): Error reading config file, line # %i\n",nline);
          perror("ReadConfig(): Error in fgets()");
        }
        if(feof(fd))   /* check for End-Of-File */
        {
          printf("ReadConfig(): Found end of config file, line # %i\n",nline);
        }
        break;         /* Stop reading */
      }
      str[strlen(str)-1] = 0;
      comment = (*str==0 || *str=='#' || *str=='!' || *str==';');
      badline = 0;
      printf("String [%3d] >%s<\n",nline,str);
      if(!comment)
      {
	    nread = sscanf(str,"%s %s %s %i %s %i %i %f %f",
         mapname[nmaps],setname[nmaps],subsetname[nmaps],&dim[nmaps],
         maptype[nmaps],&histid[nmaps],&nbin[nmaps],&xmin[nmaps],&xmax[nmaps]);
	    badline = (nread != NUMFIELD1);
	    if(badline)
        {
	  	  printf("ReadConfig(): Error pasing line # %i ;"
                 " nItems=%i Nfields=%i ; \n"
				 "Line was:\"%s\"\n",nline,nread,NUMFIELD1,str);
		  break;
		}
        else
        {
          for(i=0; i<dim[nmaps]; i++) fscanf(fd,"%f",&tmp); /* just skip it */
          nmaps ++;
        }
	  }
      else
      {
		printf("Found comment at line # %i\n",nline);
	  }		  
    } while(1);
    printf("Final count: nmaps=%d\n",nmaps);

    fclose(fd);
  }
  else
  {
    printf("ReadConfig(): cannot open file >%s<\n",fileconf);
  }

  return(nmaps);
}


//_____________________________________________________________________________
void CMSpars::WriteConfig(Option_t *fileconf)
{

}

//_____________________________________________________________________________
void CMSpars::MenuCrate()
{
  printf("cratename >%s<\n",cratename);
  fDialog1 =
new CMSparsMenuCrate(gClient->GetRoot(),gClient->GetRoot(),400,200,cratename);
  printf("MenuCrate done\n");
}

//_____________________________________________________________________________
void CMSpars::Menu()
{
  fDialog2 = new CMSparsMenu(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
}


//_____________________________________________________________________________
void CMSpars::HistView()
{
  TPad *pad;
  TGraph *gr;
  TH1F *h1;
  TH2F *h21, *h22;
  TNtuple *ncc;
  FILE *spar_read, *spar_load, *calconst, *fp;
  TFile *f;
  char *dname = "/usr/local/clas/parms/pedman";
  char fname[256], tmp[256];

  const Int_t n = 704;

   Int_t i=1;
   Int_t nn, ncols1, ncols2;
   Float_t hard[n], load[n], slot[n], x[n];
   Float_t y1[n], y2[n], y3[n], y4[n], y5[n];
   Double_t zz1[n], zz2[n];
   Axis_t xx1[n], xx2[n], yy1[n], yy2[n];
   Float_t slot_hard, channel_hard, spar_hard, ped_hard; 
   Float_t slot_load, channel_load, value_load, sigma, offset; 
   Float_t calcon, slot_cal, channel_cal;

   Float_t crate1, slot1, channel1, value1;
   Int_t ncols;
   Int_t nlines1 = 0;
  const Int_t kUPDATE = 7000;

  printf("cratename >%s<\n",cratename);

  sprintf(fname,"%s/spar_archive/%s_hard.spar",dname,cratename);
  printf("fname1 >%s<\n",fname);
  if((spar_read = fopen(fname,"r")) == NULL)
  {
    printf("CMSpars::HistView: can not open file >%s< - return\n",fname);
    return;
  }

  sprintf(fname,"%s/archive/%s.ped",dname,cratename);
  printf("fname2 >%s<\n",fname);
  if((spar_load = fopen(fname,"r")) == NULL)
  {
    printf("CMSpars::HistView: can not open file >%s< - return\n",fname);
    fclose(spar_read);
    return;
  }

  sprintf(fname,"%s/Calib/%s.cal",dname,cratename);
  printf("fname3 >%s<\n",fname);
  if((calconst = fopen(fname,"r")) == NULL)
  {
    printf("CMSpars::HistView: can not open file >%s< - return\n",fname);
    fclose(spar_read);
    fclose(spar_load);
    return;
  }

  if(!strcmp(cratename,"cc1"))      nn = 256;
  else if(!strcmp(cratename,"ec1")) nn = 704;
  else if(!strcmp(cratename,"ec2")) nn = 704;
  else if(!strcmp(cratename,"lac1")) nn = 512;
  else if(!strcmp(cratename,"sc1")) nn = 640;
  else nn = 256;

  printf("nn=%d\n",nn);

  for(i=0; i<nn; i++)
  {
    ncols1 = fscanf(spar_read,"%f %f %f %f \n",
                                &slot_hard,&channel_hard,&spar_hard,&ped_hard);
    if(ncols1 < 0 && ncols1 == 111) break;

    ncols2 = fscanf(spar_load,"%f %f %f %f %f \n",
                          &slot_load,&channel_load,&value_load,&sigma,&offset);
    if(ncols2 < 0) break;

    ncols2 = fscanf(calconst,"%f %f %f  \n",&slot_cal,&channel_cal,&calcon);
    if(ncols2 < 0) break;    

    hard[i] = ped_hard;
    load[i] = value_load;
    slot[i] = slot_hard;

    x[i] = i;

    y1[i] = value_load / (ped_hard*calcon);
    // COMMENTED OUT IF YOU ARE ALREADY CALIBRATED
    // y1[i] = value_load/ped_hard;
    // fprintf(calconst,"%f %f %f \n",slot_hard,channel_hard,load[i]/hard[i]);
    // printf("%f %f %f \n",hard[i],load[i], y1[i]);

    y2[i] = spar_hard / (value_load+offset);

    xx1[i] = channel_hard;
    yy1[i] = slot_hard;
    zz1[i] = log( value_load / (ped_hard*calcon));

    xx2[i] = channel_load;
    yy2[i] = slot_load;
    zz2[i] = sigma;

  }

  printf("after for() loop i=%d\n",i);
  fclose(spar_read);
  fclose(spar_load);
  fclose(calconst);


  // Draw Histograms

  m_Pad->cd();


  pad = viewer->GetPad(0,0,0);
  pad->cd();
  gr = new TGraph(nn,x,y1);
  gr->SetFillColor(19);
  gr->SetLineColor(3);
  gr->SetLineWidth(-12);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  gr->Draw("ASP");
  gr->GetHistogram()->SetXTitle("Pedestals                        Channel");
  gr->GetHistogram()->SetYTitle("Old/Current");


  pad = viewer->GetPad(0,0,1);
  pad->cd();
  gr = new TGraph(nn,slot,y2);
  gr->SetFillColor(19);
  gr->SetLineColor(3);
  gr->SetLineWidth(-12);
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  gr->Draw("ASP");
  gr->GetHistogram()->SetXTitle("Spars                        Slot");
  gr->GetHistogram()->SetYTitle("Load/Measured");


  h21 = new TH2F("h21","log(Old/Current)",70,0,69,26,0,25);
  h21->FillN(nn,xx1,yy1,zz1);
  h21->SetContour(40);
  h21->SetFillColor(1);
  pad = viewer->GetPad(1,0,0);
  pad->cd();
  pad->SetTheta(90);
  pad->SetPhi(0);
  h21->Draw("lego2");
  pad = viewer->GetPad(1,0,1);
  pad->cd();
  pad->SetTheta(45);
  pad->SetPhi(40);
  h21->Draw("lego2");


  h22 = new TH2F("h22","Sigma",70,0,69,26,0,25);
  h22->FillN(nn,xx2,yy2,zz2);
  h22->SetContour(40);
  h22->SetFillColor(1);
  pad = viewer->GetPad(2,0,0);
  pad->cd();
  pad->SetTheta(90);
  pad->SetPhi(0);
  h22->Draw("lego2");
  pad = viewer->GetPad(2,0,1);
  pad->cd();
  pad->SetTheta(45);
  pad->SetPhi(40);
  h22->Draw("lego2");


m_Pad->Modified();
m_Pad->Update();


  // N-tuples

  sprintf(fname,"%s/raw/%s_root.dat",dname,cratename);
  printf("fname4 >%s<\n",fname);
  if((fp = fopen(fname,"r")) == NULL)
  {
    printf("CMSpars::HistView: can not open file >%s< - return\n",fname);
    return;
  }

  sprintf(fname,"%s.root",cratename);
  printf("fname5 >%s<\n",fname);
  f = new TFile(fname,"RECREATE");

  h1 = new TH1F("h1","Pedestal distribution",100,0,1000);
  ncc = new TNtuple("ncc","Ntuple","crate:slot:channel:value");

  h1->SetFillColor(48);

  pad = viewer->GetPad(3,0,0);
  pad->cd();
  while(1)
  {
    ncols = fscanf(fp,"%f %f %f %f \n",&crate1,&slot1,&channel1,&value1);
    if(ncols < 0) break;
    if(value1 <=0) printf("crate=%8f, slot=%8f, channel=%8f, value=%8f\n",crate1,slot1,channel1,value1);
    h1->Fill(value1);
    ncc->Fill(crate1,slot1,channel1,value1);
    if(!nlines1%kUPDATE)
    {
      h1->Draw();
      pad->Modified();
      pad->Update();
     }
     nlines1++;
     //printf("%d\n",nlines1);
   }
   printf(" found %d points\n",nlines1);
   pad->Update();
   h1->SetXTitle("Channel");
   h1->SetYTitle("Evt");
  
   fclose(fp);
   f->Write();

m_Pad->Modified();
m_Pad->Update();

}



