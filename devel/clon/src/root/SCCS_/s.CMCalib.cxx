h58873
s 00001/00002/00382
d D 1.2 02/03/26 15:17:12 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 16:44:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/root/CMCalib.cxx
e
s 00384/00000/00000
d D 1.1 01/11/19 16:44:41 boiarino 1 0
c date and time created 01/11/19 16:44:41 by boiarino
e
u
U
f e 0
t
T
I 1
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMCalib                                                             //
//                                                                      //
//  CLAS Online Calibration                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


#include <TROOT.h>
D 3
#include "TRandom.h"
E 3
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"

#include "CMCalib.h"
CMCalib *gCMCalib; // to use it in buttons

ClassImp(CMCalib)


#define ABS(x)      ((x) < 0 ? -(x) : (x))


// fast reconstruction stuff
#include "dclib.h"
#include "wcs.h"


//_____________________________________________________________________________
CMCalib::CMCalib(Option_t *fname) : CMClient()
{
// Create one CMCalib object

  gCMCalib = this;

  hbook = new CMHbook();
  viewer = new CMViewer(fname);

  downloaded = 0;

  strcpy(filename,fname);
  printf("CMCalib: Config File >%s<\n",filename);

}


//_____________________________________________________________________________
void CMCalib::Download()
{
//
  Int_t i;

  // read config file

// TEMPORARY !!!
D 3
  nview = viewer->ReadConfig("/usr/local/clas/parms/cmon/tglib/hist1.conf");
E 3
I 3
nview=viewer->ReadConfig("/usr/local/clas/parms/cmon/tglib/hist1.conf",hbook);
E 3

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
void CMCalib::Prestart(Int_t *iw)
{
//
  Int_t i, j;

  prestarted = 1;
  nevents = nevents1 = 0;

  Reset();

  bosInit(wcs_.jw,JNDIMB);
  cminit();

}



//_____________________________________________________________________________
void CMCalib::Go(Int_t *iw)
{
//
  time0 = time(NULL);

}


//_____________________________________________________________________________
void CMCalib::End(Int_t *iw)
{
//
  dcerun(iw);
  dclast(iw);

}


//_____________________________________________________________________________
int CMCalib::Event(Int_t *iw)
{
// one event processing

  //printf("CMCalib::Event() reached\n");
  Int_t ret;

  nevents ++;
  cmloop(iw,0,0,0,&ret); // process one event
  ret = hbook->BosToRoot(iw);

  return(ret);
}



//_____________________________________________________________________________
void CMCalib::Hist()
{
// Create views
  Int_t i;
  for(i=0; i<nview; i++) viewer->ViewCreate(i, hbook);

}



//_____________________________________________________________________________
void CMCalib::Update()
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
void CMCalib::Reset()
{
  hbook->Clean();
}

//_____________________________________________________________________________
void CMCalib::Buttons()
{
// Create the buttons

  printf("CMCalib::Buttons reached, m_StatPad=0x%08x\n",m_StatPad);

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
  Float_t x0 = 0.01;
  Float_t x01 = 0.54;
  Float_t x02 = 0.58;
  Float_t x1 = 0.98;

  TButton *button; // button coordinates is always in range [0,1]

  char *but1 = "CalibPtr->PreviousView()";
  button = new TButton("Previous View",but1,x0,y-dbutton,x01,y);
  button->SetFillColor(butcolor);
  button->Draw();
  char *but2 = "CalibPtr->NextView()";
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
  char *but3 = "CalibPtr->ReadCalibration()";
  button = new TButton("Read Calibration",but3,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc2   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc2->Draw();

  y -= dbutton + dy*6.;
  char *but4 = "CalibPtr->WhiteCalibration()";
  button = new TButton("Write Calibration",but4,x0,y-dbutton,x1,y);
  button->SetFillColor(38);
  button->Draw();
  m_Arc3   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc3->Draw();

  y -= dbutton + dy*6.;
  char *but5 = "CalibPtr->Menu()";
  button = new TButton("Main Menu",but5,x0,y-dbutton,x1,y);
  button->SetFillColor(butcolor);
  button->Draw();
  m_Arc4   = new TArc(dxtr/2.,(y-dbutton-dy*3.)*dytr,rarc);
  m_Arc4->Draw();

  Status();

}

//_____________________________________________________________________________
void CMCalib::Status()
{
// check status and update status pad

  if(callprlib) m_Arc2->SetFillColor(kGreen); else m_Arc2->SetFillColor(kRed);
  if(calltrlib) m_Arc3->SetFillColor(kGreen); else m_Arc3->SetFillColor(kRed);
  if(downloaded) m_Arc4->SetFillColor(kGreen); else m_Arc4->SetFillColor(kRed);

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
Int_t CMCalib::ReadConfig(Option_t *fileconf)
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
void CMCalib::WriteConfig(Option_t *fileconf)
{

}

//_____________________________________________________________________________
void CMCalib::Menu()
{
  fDialog = new CMCalibMenu(gClient->GetRoot(), gClient->GetRoot(), 400, 200);
}


E 1
