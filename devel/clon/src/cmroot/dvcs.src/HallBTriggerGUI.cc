#include <stdio.h>
#include "HallBTriggerGUI.h"

HallBTriggerGUI::HallBTriggerGUI(const TGWindow *p, unsigned int w, unsigned int h, const char *host)
	: TGMainFrame(p, w, h)
{
	SetCleanup(kDeepCleanup);
	SetLayoutManager(new TGVerticalLayout(this));

	gStyle->SetPalette(1, NULL);
	pTabMain = NULL;

	pVMERemoteDVCSTrigger = new VMERemote(host, VMESERVER_PORT);

	TGCompositeFrame *tFrame;
	pTabMain = new TGTab(this);
	// Tab Frame
	tFrame = pTabMain->AddTab("ICScalers");		tFrame->AddFrame(new ICScalers(tFrame, pVMERemoteDVCSTrigger), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	tFrame = pTabMain->AddTab("HScalers");		tFrame->AddFrame(new HScalers(tFrame, pVMERemoteDVCSTrigger), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	tFrame = pTabMain->AddTab("DVCSScope");		tFrame->AddFrame(new DVCSScope(tFrame, pVMERemoteDVCSTrigger), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	tFrame = pTabMain->AddTab("DVCSControl");	tFrame->AddFrame(new DVCSControl(tFrame, pVMERemoteDVCSTrigger), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	AddFrame(pTabMain, new TGLayoutHints(kLHintsBottom | kLHintsRight | kLHintsExpandX | kLHintsExpandY));
	// End Tab Frame

	MapSubwindows();
	Resize();
	SetWindowName("DVCS GUI");
	MapWindow();
}

Bool_t HallBTriggerGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
	switch(GET_MSG(msg))
	{
	case kC_COMMAND:
		switch(GET_SUBMSG(msg))
		{
		case kCM_BUTTON:
			switch(parm1)
			{
			default:
				printf("button id %d pressed\n", parm1);
				break;
			}
			break;
		}
	}
	return kTRUE;
}

void HallBTriggerGUI::HandleDisconnect()
{
	if(pVMERemoteDVCSTrigger->IsValid())
	{
		pVMERemoteDVCSTrigger->Close();
		printf("Disconnected from host: %s\n", pVMERemoteDVCSTrigger->pHostName);
	}
}

void HallBTriggerGUI::CloseWindow()
{
	HandleDisconnect();
	gApplication->Terminate();
}
