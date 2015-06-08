
/* dvcstrig.c - main file for the ROOT-based DVCS trigger GUI */


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
#include "HallBTriggerGUI.h"


int main(int argc, char* argv[])
{
	TApplication App("HallBTrigger GUI", &argc, argv);

	if(gROOT->IsBatch())
	{
		fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
		return 1;
	}

	if(argc != 2)
	{
		fprintf(stderr, "Error - usage: HallBTriggerGUI <hostname>\n");
		return 2;
	}

	HallBTriggerGUI *pHallBTriggerGUI = new HallBTriggerGUI(gClient->GetRoot(), 400, 200, argv[1]);
	while(1)
	{
 		gSystem->ProcessEvents();
		gSystem->Sleep(1);
	}
	return 0;
}

