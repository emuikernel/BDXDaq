//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CMRootServer                                                               //
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
#include <TRandom.h>
#include <TButton.h>
#include <TLine.h>
#include <TArc.h>


#include "bos.h"
#include "bosio.h"
#include "bosfun.h"
#include "etbosio.h"


#include "CMRootServer.h"


ClassImp(CMRootServer)




//_____________________________________________________________________________
CMRootServer::CMRootServer(Option_t *fname, CMDisplay *mDisplay) : CMClient(fname, mDisplay)
{
// Create one CMRootServer object

  hbook = new CMHbook();
  printf("CMRootServer::CMRootServer: hbook=0x%08x\n",hbook);

  downloaded = 0;

  strcpy(filename,fname);
  printf("CMRootServer: Config File >%s<\n",filename);

  runnumber = oldrunnumber = 0;
}


//_____________________________________________________________________________
void CMRootServer::Download()
{
//
  printf("CMRootServer::Download\n");


  // read config file
  /*nview = viewer->ReadConfig(filename, hbook);*/

  /* temporary !!! have to get it from hist1.conf */
  nhistos = 0;

  histos[nhistos++] = 12;
  histos[nhistos++] = 13;
  histos[nhistos++] = 14;
  histos[nhistos++] = 15;
  histos[nhistos++] = 16;
  histos[nhistos++] = 158;

  histos[nhistos++] = 21;
  histos[nhistos++] = 22;
  histos[nhistos++] = 32;
  histos[nhistos++] = 33;
  histos[nhistos++] = 34;
  histos[nhistos++] = 35;

  histos[nhistos++] = 131;
  histos[nhistos++] = 132;
  histos[nhistos++] = 133;
  histos[nhistos++] = 134;
  histos[nhistos++] = 135;
  histos[nhistos++] = 136;

  histos[nhistos++] = 211;
  histos[nhistos++] = 212;
  histos[nhistos++] = 213;
  histos[nhistos++] = 214;
  histos[nhistos++] = 215;
  histos[nhistos++] = 216;

  histos[nhistos++] = 221;
  histos[nhistos++] = 222;
  histos[nhistos++] = 223;
  histos[nhistos++] = 224;
  histos[nhistos++] = 225;
  histos[nhistos++] = 226;

  histos[nhistos++] = 151;
  histos[nhistos++] = 152;
  histos[nhistos++] = 153;
  histos[nhistos++] = 154; /*33*/
  histos[nhistos++] = 155;
  histos[nhistos++] = 156;

  histos[nhistos++] = 111;
  histos[nhistos++] = 112; /*37*/
  histos[nhistos++] = 113;
  histos[nhistos++] = 114;
  histos[nhistos++] = 115;
  histos[nhistos++] = 116;

  histos[nhistos++] = 951;
  histos[nhistos++] = 952;
  histos[nhistos++] = 953;
  histos[nhistos++] = 954;
  histos[nhistos++] = 955;
  histos[nhistos++] = 956;
  histos[nhistos++] = 961;
  histos[nhistos++] = 962;
  histos[nhistos++] = 963;
  histos[nhistos++] = 964;
  histos[nhistos++] = 965;
  histos[nhistos++] = 966;

  histos[nhistos++] = 971;
  histos[nhistos++] = 972;
  histos[nhistos++] = 973;
  histos[nhistos++] = 974;
  histos[nhistos++] = 975;
  histos[nhistos++] = 976;
  histos[nhistos++] = 981;
  histos[nhistos++] = 982;
  histos[nhistos++] = 983;
  histos[nhistos++] = 984;
  histos[nhistos++] = 985;
  histos[nhistos++] = 986;
  histos[nhistos++] = 991;
  histos[nhistos++] = 992;
  histos[nhistos++] = 993;
  histos[nhistos++] = 994;
  histos[nhistos++] = 995;
  histos[nhistos++] = 996;

  histos[nhistos++] = 901;
  histos[nhistos++] = 902;
  histos[nhistos++] = 903;
  histos[nhistos++] = 904;
  histos[nhistos++] = 905;
  histos[nhistos++] = 906;
  histos[nhistos++] = 911;
  histos[nhistos++] = 912;
  histos[nhistos++] = 913;
  histos[nhistos++] = 914;
  histos[nhistos++] = 915;
  histos[nhistos++] = 916;
  histos[nhistos++] = 921;
  histos[nhistos++] = 922;
  histos[nhistos++] = 923;
  histos[nhistos++] = 924;
  histos[nhistos++] = 925;
  histos[nhistos++] = 926;

  histos[nhistos++] = 1001;
  histos[nhistos++] = 1002;
  histos[nhistos++] = 1003;
  histos[nhistos++] = 1004;
  histos[nhistos++] = 1005;
  histos[nhistos++] = 1006;
  histos[nhistos++] = 1007;
  histos[nhistos++] = 1008;
  histos[nhistos++] = 1009;
  histos[nhistos++] = 1010;
  histos[nhistos++] = 1011;
  histos[nhistos++] = 1012;
  histos[nhistos++] = 1013;
  histos[nhistos++] = 1014;
  histos[nhistos++] = 1015;
  histos[nhistos++] = 1016;
  histos[nhistos++] = 1017;
  histos[nhistos++] = 1018;
  histos[nhistos++] = 1019;
  histos[nhistos++] = 1020;
  histos[nhistos++] = 1021;
  histos[nhistos++] = 1022;
  histos[nhistos++] = 1023;
  histos[nhistos++] = 1024;
  histos[nhistos++] = 1025;
  histos[nhistos++] = 1026;
  histos[nhistos++] = 1027;
  histos[nhistos++] = 1028;
  histos[nhistos++] = 1029;
  histos[nhistos++] = 1030;

  histos[nhistos++] = 1041;
  histos[nhistos++] = 1042;
  histos[nhistos++] = 1043;
  histos[nhistos++] = 1044;
  histos[nhistos++] = 1045;
  histos[nhistos++] = 1046;
  histos[nhistos++] = 1047;
  histos[nhistos++] = 1048;
  histos[nhistos++] = 1049;
  histos[nhistos++] = 1050;
  histos[nhistos++] = 1051;
  histos[nhistos++] = 1052;
  histos[nhistos++] = 1053;
  histos[nhistos++] = 1054;
  histos[nhistos++] = 1055;
  histos[nhistos++] = 1056;
  histos[nhistos++] = 1057;
  histos[nhistos++] = 1058;
  histos[nhistos++] = 1059;
  histos[nhistos++] = 1060;
  histos[nhistos++] = 1061;
  histos[nhistos++] = 1062;
  histos[nhistos++] = 1063;
  histos[nhistos++] = 1064;
  histos[nhistos++] = 1065;
  histos[nhistos++] = 1066;
  histos[nhistos++] = 1067;
  histos[nhistos++] = 1068;
  histos[nhistos++] = 1069;
  histos[nhistos++] = 1070;



  this->Hist();

  downloaded = 1;
}


//_____________________________________________________________________________
void CMRootServer::Prestart(Int_t *iw)
{
//
  printf("CMRootServer::Prestart\n");
  Int_t i, j;

  nevents = 0;
  nevents1 = 0;

  this->Reset();

  prestarted = 1;
}



//_____________________________________________________________________________
void CMRootServer::Go(Int_t *iw)
{
//
  printf("CMRootServer::Go() reached\n");fflush(stdout);

  time0 = time(NULL);
  printf("CMRootServer::Go() done\n");fflush(stdout);
}


//_____________________________________________________________________________
void CMRootServer::End(Int_t *iw)
{
//
  Int_t ret;

  printf("CMRootServer::End() reached\n");fflush(stdout);

}


//_____________________________________________________________________________
int CMRootServer::Event(Int_t *iw)
{
// one event processing

  //printf("CMRootServer::Event() reached\n");fflush(stdout);
  Int_t ind, ret, tmp, *buf;

  nevents ++;

  // obtain run number
  if((ind = etNlink(iw,"HEAD",0)) > 0)
  {
    buf = (Int_t *) &iw[ind];
	//printf("%d %d %d %d %d %d %d %d\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    runnumber = buf[1];

    //if run number changed, clean histograms
    if(runnumber != oldrunnumber)
    {
      oldrunnumber = runnumber;
      this->Reset();
    }

  }

  // read bos banks and create Root histograms
  // then convert desired histograms to timeline plots

  //printf("CMRootServer: Event()\n");fflush(stdout);
  ret = hbook->BosToRoot(iw);
  //printf("CMRootServer: Event() done, ret=%d\n",ret);fflush(stdout);

  return(ret);
}



//____________________________________________________________________________
void CMRootServer::Hist()
{
// Create views

  Int_t i;

}



//____________________________________________________________________________
void CMRootServer::Update()
{
// update drawing

  Int_t time1, ret;
  char dirname[128], cmd[128];

  //printf("CMRootServer::Update() reached nhistos=%d\n",nhistos);fflush(stdout);

  time1 = time(NULL)-time0;
  //if(time1 > 0)
  //{
  //  printf("Event rate: overall %5.0f events/sec, useful %5.0f events/sec\n",
  //  ((Float_t)nevents)/((Float_t)time1),((Float_t)nevents1)/((Float_t)time1));
  //}



  gSystem->Exec("(rm -f /tmp/root_server.root)");
  TFile *hfile = new TFile("/tmp/root_server.root","NEW","root_server file");
  for(Int_t i=1; i<=12; i++)
  {
    sprintf(dirname,"page_%02d",i);
    TDirectory *page = hfile->mkdir(dirname);
  }


  /* write histograms in according to config file */

  //printf("CMRootServer::Update() start writing ..\n");fflush(stdout);
  for(Int_t i=0; i<nhistos; i++)
  {
    /* TODO: create directories in according to config file */
    if(i==0)
    {
      sprintf(dirname,"page_%02d",1);
      hfile->cd(dirname);
	}
    else if(i==6)
    {
      sprintf(dirname,"page_%02d",2);
      hfile->cd(dirname);
	}
    else if(i==12)
    {
      sprintf(dirname,"page_%02d",3);
      hfile->cd(dirname);
	}
    else if(i==18)
    {
      sprintf(dirname,"page_%02d",4);
      hfile->cd(dirname);
	}
    else if(i==24)
    {
      sprintf(dirname,"page_%02d",5);
      hfile->cd(dirname);
	}
    else if(i==30)
    {
      sprintf(dirname,"page_%02d",6);
      hfile->cd(dirname);
	}
    else if(i==36)
    {
      sprintf(dirname,"page_%02d",7);
      hfile->cd(dirname);
	}
    else if(i==42)
    {
      sprintf(dirname,"page_%02d",8);
      hfile->cd(dirname);
	}
    else if(i==54)
    {
      sprintf(dirname,"page_%02d",9);
      hfile->cd(dirname);
	}
    else if(i==72)
    {
      sprintf(dirname,"page_%02d",10);
      hfile->cd(dirname);
	}
    else if(i==90)
    {
      sprintf(dirname,"page_%02d",11);
      hfile->cd(dirname);
	}
    else if(i==120)
    {
      sprintf(dirname,"page_%02d",12);
      hfile->cd(dirname);
	}


    if(hbook->hist1[histos[i]]) hbook->hist1[histos[i]]->Write();
    else if(hbook->hist2[histos[i]]) hbook->hist2[histos[i]]->Write();
  }
  //printf("CMRootServer::Update() wrote\n");fflush(stdout);


  hfile->Close();

  //printf("CMRootServer::Update() copying file ..\n");fflush(stdout);
  sprintf(cmd,
    "(cp /tmp/root_server.root /mon/rootfiles/run%06d.root; rm -f /tmp/root_server.root)",
    runnumber);
  //printf("cmd >%s<\n",cmd);
  gSystem->Exec(cmd);
  //printf("CMRootServer::Update() copied\n");fflush(stdout);

}


//_____________________________________________________________________________
void CMRootServer::Reset()
{
// Reset histograms

  Int_t i;

  printf("hbook=0x%08x\n",hbook);
  if(hbook) hbook->Reset();

}

