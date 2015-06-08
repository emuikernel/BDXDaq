
/* cmroot.c - main file for the ROOT-based CLAS online GUI */


#include <stdlib.h>
#include <stdio.h>

#ifdef WIN
#include <conio.h>
#include <w32pragma.h>
#endif

#include <TApplication.h>
#include <TROOT.h>
#include <TServerSocket.h>
#include <TSystem.h>
#include <TStyle.h>


#include "CMDisplay.h"
#include "CMDaq.h"
#include "CMHist.h"
#include "CMTestSetup.h"

extern CMDisplay *gCMDisplay;
extern CMDaq *gCMDaq;
extern CMHist *gCMHist;
extern CMTestSetup *gCMTestSetup;

int
main(int argc, char **argv)
{
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libbosio.so"); // BOSIO library
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libcodatt.so"); // TT library
  gSystem->Load("$CODA/$OSTYPE_MACHINE/lib/libet.so"); // ET
#ifndef Linux
  gSystem->Load("/usr/lib/librt.so");
  gSystem->Load("/usr/lib/libpthread.so");
#endif
  //gSystem->Load("./$OSTYPE_MACHINE/lib/libcmroot.so");

  TApplication App("CMROOT", &argc, argv);

  if(gROOT->IsBatch())
  {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return(1);
  }


  printf("argc=%d\n",argc);
  if(argc != 3)
  {
    fprintf(stderr, "You must specify a session name and hist config file\n");
    return(2);
  }


  char temp[256];
  printf("argv=%s\n",argv[1]);
  sprintf(temp,"/tmp/et_sys_%s",argv[1]);

  gCMDisplay = new CMDisplay("CLAS Online Monitor");
  gCMDaq = new CMDaq(temp,gCMDisplay);


  printf("argv=%s\n",argv[2]);
  sprintf(temp,"./%s.conf",argv[2]);

  // if config file name is 'testsetup', call corresponding object
  // otherwise call CMHist
  if(!strcmp(argv[2],"testsetup"))
  {
    gStyle->SetStatW(0.35);
    gStyle->SetStatH(0.35);
    gCMTestSetup = new CMTestSetup("dummy",gCMDisplay);
  }
  else
  {
    gCMHist = new CMHist(temp,gCMDisplay);
  }


  while(1)
  {
 	gSystem->ProcessEvents();
	gSystem->Sleep(1);
  }

  return(0);
}

