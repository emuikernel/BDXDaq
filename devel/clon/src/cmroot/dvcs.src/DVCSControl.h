#ifndef DVCSControl_H
#define DVCSControl_H

#include "RootHeader.h"
#include "VMERemote.h"
#include "DVCSBoardRegs.h"

#define BTN_CONNECT					100
#define BTN_DISCONNECT				101
#define BTN_RESET					103
#define BTN_CONTROLVIEWUPDATE		124
#define BTN_HODOSCOPEUPDATESCALERS	125
#define BTN_CONTROLPROGMTRIG		126
#define BTN_CONTROLPROGQ1TRIG		127
#define BTN_CONTROLPROGQ2TRIG		128
#define BTN_CONTROLPROGQ3TRIG		129
#define BTN_CONTROLPROGQ4TRIG		130
#define SDR_CLUSTERTHRESHOLD		213

class DVCSControl	: public TGCompositeFrame
{
public:
	DVCSControl(const TGWindow *p, VMERemote *pRemoteClient) : TGCompositeFrame(p, 10, 10)
	{
		SetLayoutManager(new TGVerticalLayout(this));
		pClient = pRemoteClient;

		pFrameControlButtons = new TGHorizontalFrame(this);
		pButtonReset = new TGTextButton(pFrameControlButtons, new TGHotString("Reset"), BTN_RESET);
		pButtonUpdateControlView = new TGTextButton(pFrameControlButtons, new TGHotString("Update Register View"), BTN_CONTROLVIEWUPDATE);
		pButtonProgramMTrigger = new TGTextButton(pFrameControlButtons, new TGHotString("PROG MTrigger"), BTN_CONTROLPROGMTRIG);
		pButtonProgramQ1Trigger = new TGTextButton(pFrameControlButtons, new TGHotString("PROG Q1Trigger"), BTN_CONTROLPROGQ1TRIG);
		pButtonProgramQ2Trigger = new TGTextButton(pFrameControlButtons, new TGHotString("PROG Q2Trigger"), BTN_CONTROLPROGQ2TRIG);
		pButtonProgramQ3Trigger = new TGTextButton(pFrameControlButtons, new TGHotString("PROG Q3Trigger"), BTN_CONTROLPROGQ3TRIG);
		pButtonProgramQ4Trigger = new TGTextButton(pFrameControlButtons, new TGHotString("PROG Q4Trigger"), BTN_CONTROLPROGQ4TRIG);

		pButtonReset->Associate(this);
		pButtonUpdateControlView->Associate(this);
		pButtonProgramMTrigger->Associate(this);
		pButtonProgramQ1Trigger->Associate(this);
		pButtonProgramQ2Trigger->Associate(this);
		pButtonProgramQ3Trigger->Associate(this);
		pButtonProgramQ4Trigger->Associate(this);
		pFrameControlButtons->AddFrame(pButtonReset, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonUpdateControlView, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonProgramMTrigger, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonProgramQ1Trigger, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonProgramQ2Trigger, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonProgramQ3Trigger, new TGLayoutHints(kLHintsCenterX));
		pFrameControlButtons->AddFrame(pButtonProgramQ4Trigger, new TGLayoutHints(kLHintsCenterX));
		AddFrame(pFrameControlButtons, new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX));

		pFrameClusterThreshold = new TGHorizontalFrame(this);
		pTextClusterThreshold = new TGTextEntry(pFrameClusterThreshold, new TGTextBuffer(30));
		pTextClusterThreshold->SetWidth(120);
		pTextClusterThreshold->SetText("Cluster Threshold: 1");
		pTextClusterThreshold->SetEnabled(kFALSE);
		pFrameClusterThreshold->AddFrame(pTextClusterThreshold, new TGLayoutHints(kLHintsCenterX));
		pSliderClusterThreshold = new TGHSlider(pFrameClusterThreshold, 100, kSlider1 | kScaleBoth, SDR_CLUSTERTHRESHOLD);
		pSliderClusterThreshold->Associate(this);
		pSliderClusterThreshold->SetRange(0, 8);
		pSliderClusterThreshold->SetPosition(1);
		pFrameClusterThreshold->AddFrame(pSliderClusterThreshold, new TGLayoutHints(kLHintsCenterX));
		pProgressDownload = new TGHProgressBar(pFrameClusterThreshold);
		pProgressDownload->SetWidth(200);
		pProgressDownload->SetMax(720);
		pProgressDownload->SetMin(0);
		pProgressDownload->SetPosition(0);
		pFrameClusterThreshold->AddFrame(pProgressDownload, new TGLayoutHints(kLHintsCenterX));
		AddFrame(pFrameClusterThreshold);

		pTextViewControlRegs = new TGTextView(this);
		AddFrame(pTextViewControlRegs, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	}
private:
	TGHorizontalFrame	*pFrameControlButtons;
	TGTextButton		*pButtonReset;
	TGTextButton		*pButtonUpdateControlView;
	TGTextButton		*pButtonProgramMTrigger;
	TGTextButton		*pButtonProgramQ1Trigger;
	TGTextButton		*pButtonProgramQ2Trigger;
	TGTextButton		*pButtonProgramQ3Trigger;
	TGTextButton		*pButtonProgramQ4Trigger;
	TGHorizontalFrame	*pFrameClusterThreshold;
	TGHSlider			*pSliderClusterThreshold;
	TGTextEntry			*pTextClusterThreshold;
	TGHProgressBar		*pProgressDownload;
	TGTextView			*pTextViewControlRegs;

	VMERemote			*pClient;

	virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t)
	{
		char buf[40];
		switch(GET_MSG(msg))
		{
		case kC_COMMAND:
			switch(GET_SUBMSG(msg))
			{
			case kCM_BUTTON:
				switch(parm1)
				{
				case BTN_RESET:
					ResetBoards();
					break;
				case BTN_CONTROLVIEWUPDATE:
					UpdateRegisterInfo();
					break;
				case BTN_CONTROLPROGMTRIG:
					printf("Programming MTrigger Module...\n");
					pClient->ProgramModule(DVCS_M_BASE, "DVCSMTrigger.rbf", pProgressDownload);
					CompleteDownload(DVCS_M_BASE);
					break;
				case BTN_CONTROLPROGQ1TRIG:
					printf("Programming Q1Trigger Module...\n");
					pClient->ProgramModule(DVCS_Q1_BASE, "DVCSQTrigger_Q1.rbf", pProgressDownload);
					CompleteDownload(DVCS_Q1_BASE);
					break;
				case BTN_CONTROLPROGQ2TRIG:
					printf("Programming Q2Trigger Module...\n");
					pClient->ProgramModule(DVCS_Q2_BASE, "DVCSQTrigger_Q2.rbf", pProgressDownload);
					CompleteDownload(DVCS_Q2_BASE);
					break;
				case BTN_CONTROLPROGQ3TRIG:
					printf("Programming Q3Trigger Module...\n");
					pClient->ProgramModule(DVCS_Q3_BASE, "DVCSQTrigger_Q3.rbf", pProgressDownload);
					CompleteDownload(DVCS_Q3_BASE);
					break;
				case BTN_CONTROLPROGQ4TRIG:
					printf("Programming Q4Trigger Module...\n");
					pClient->ProgramModule(DVCS_Q4_BASE, "DVCSQTrigger_Q4.rbf", pProgressDownload);
					CompleteDownload(DVCS_Q4_BASE);
					break;
				default:
					printf("button id %d pressed\n", parm1);
					break;
				}
				break;
			}
			break;
		case kC_HSLIDER:
			switch(parm1)
			{
			case SDR_CLUSTERTHRESHOLD:
				sprintf(buf, "Cluster Threshold: %d", pSliderClusterThreshold->GetPosition());
				pTextClusterThreshold->SetText(buf);
				if(pClient->IsValid())
				{
					pClient->WriteVME16(DVCS_M_BASE + A_THRESHOLD, pSliderClusterThreshold->GetPosition());
					pClient->WriteVME16(DVCS_Q1_BASE + A_THRESHOLD, pSliderClusterThreshold->GetPosition());
					pClient->WriteVME16(DVCS_Q2_BASE + A_THRESHOLD, pSliderClusterThreshold->GetPosition());
					pClient->WriteVME16(DVCS_Q3_BASE + A_THRESHOLD, pSliderClusterThreshold->GetPosition());
					pClient->WriteVME16(DVCS_Q4_BASE + A_THRESHOLD, pSliderClusterThreshold->GetPosition());
				}
				break;
			}
			break;
		}
		return kTRUE;
	}

	void CompleteDownload(unsigned int base)
	{
		printf("Resetting Modules...\n");
		pClient->WriteVME16(base + 0x8016, 1);
		gSystem->Sleep(2000);
		ResetBoards();
		gSystem->Sleep(100);
		UpdateRegisterInfo();
		pProgressDownload->SetPosition(0);
	}

	void ResetBoards()
	{
		pClient->WriteVME16(DVCS_M_BASE + A_QCONTROL, 0x0001);
		pClient->WriteVME16(DVCS_M_BASE + A_QCONTROL, 0x0000);
	}

	void UpdateRegisterInfo()
	{
		char buf[300];
		unsigned short val;
		unsigned int VersionMaj, VersionMin;
		unsigned int BusErrors, Errors;
		unsigned int BoardID;
		unsigned int ClusterThreshold;
		pTextViewControlRegs->Clear();

		sprintf(buf, "DVCS GUI Built: %s %s", __DATE__, __TIME__);	pTextViewControlRegs->AddLineFast(buf);

		pClient->ReadVME16(DVCS_M_BASE + A_REVISION, &val);					VersionMaj = val>>8; VersionMin = val & 0xFF;
		pClient->ReadVME16(DVCS_M_BASE + A_ERRORS, &val);					Errors = val;
		pClient->ReadVME16(DVCS_M_BASE + A_BOARDIDS, &val);					BoardID = val;
		pClient->ReadVME16(DVCS_M_BASE + A_THRESHOLD, &val);					ClusterThreshold = val;
		sprintf(buf, "Master DVCS Trigger Board");					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Version: v%u.%u", VersionMaj, VersionMin);	pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Errors: 0x%04X", Errors);					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Board IDs: 0x%04X", BoardID);				pTextViewControlRegs->AddLineFast(buf);
		pTextViewControlRegs->AddLineFast("");

		pClient->ReadVME16(DVCS_Q1_BASE + A_REVISION, &val);		VersionMaj = val>>8; VersionMin = val & 0xFF;
		pClient->ReadVME16(DVCS_Q1_BASE + A_ERRORS, &val);			Errors = val;
		pClient->ReadVME16(DVCS_Q1_BASE + A_BOARDIDS, &val);		BoardID = val;
		pClient->ReadVME16(DVCS_Q1_BASE + A_THRESHOLD, &val);		ClusterThreshold = val;
		pClient->ReadVME16(DVCS_M_BASE + A_STATUS_Q1, &val);		BusErrors = val;
		sprintf(buf, "Q1 DVCS Trigger Board");						pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Version: v%u.%u", VersionMaj, VersionMin);	pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Errors: 0x%04X", Errors);					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Board IDs: 0x%04X", BoardID);				pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Bus Errors: %u", BusErrors);				pTextViewControlRegs->AddLineFast(buf);
		pTextViewControlRegs->AddLineFast("");

		pClient->ReadVME16(DVCS_Q2_BASE + A_REVISION, &val);		VersionMaj = val>>8; VersionMin = val & 0xFF;
		pClient->ReadVME16(DVCS_Q2_BASE + A_ERRORS, &val);			Errors = val;
		pClient->ReadVME16(DVCS_Q2_BASE + A_BOARDIDS, &val);		BoardID = val;
		pClient->ReadVME16(DVCS_Q2_BASE + A_THRESHOLD, &val);		ClusterThreshold = val;
		pClient->ReadVME16(DVCS_M_BASE + A_STATUS_Q2, &val);		BusErrors = val;
		sprintf(buf, "Q2 DVCS Trigger Board");						pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Version: v%u.%u", VersionMaj, VersionMin);	pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Errors: 0x%04X", Errors);					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Board IDs: 0x%04X", BoardID);				pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Bus Errors: %u", BusErrors);				pTextViewControlRegs->AddLineFast(buf);
		pTextViewControlRegs->AddLineFast("");

		pClient->ReadVME16(DVCS_Q3_BASE + A_REVISION, &val);		VersionMaj = val>>8; VersionMin = val & 0xFF;
		pClient->ReadVME16(DVCS_Q3_BASE + A_ERRORS, &val);			Errors = val;
		pClient->ReadVME16(DVCS_Q3_BASE + A_BOARDIDS, &val);		BoardID = val;
		pClient->ReadVME16(DVCS_Q3_BASE + A_THRESHOLD, &val);		ClusterThreshold = val;
		pClient->ReadVME16(DVCS_M_BASE + A_STATUS_Q3, &val);		BusErrors = val;
		sprintf(buf, "Q3 DVCS Trigger Board");						pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Version: v%u.%u", VersionMaj, VersionMin);	pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Errors: 0x%04X", Errors);					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Board IDs: 0x%04X", BoardID);				pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Bus Errors: %u", BusErrors);				pTextViewControlRegs->AddLineFast(buf);
		pTextViewControlRegs->AddLineFast("");

		pClient->ReadVME16(DVCS_Q4_BASE + A_REVISION, &val);		VersionMaj = val>>8; VersionMin = val & 0xFF;
		pClient->ReadVME16(DVCS_Q4_BASE + A_ERRORS, &val);			Errors = val;
		pClient->ReadVME16(DVCS_Q4_BASE + A_BOARDIDS, &val);		BoardID = val;
		pClient->ReadVME16(DVCS_Q4_BASE + A_THRESHOLD, &val);		ClusterThreshold = val;
		pClient->ReadVME16(DVCS_M_BASE + A_STATUS_Q4, &val);		BusErrors = val;
		sprintf(buf, "Q4 DVCS Trigger Board");						pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Version: v%u.%u", VersionMaj, VersionMin);	pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Errors: 0x%04X", Errors);					pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Board IDs: 0x%04X", BoardID);				pTextViewControlRegs->AddLineFast(buf);
		sprintf(buf, "   Bus Errors: %u", BusErrors);				pTextViewControlRegs->AddLineFast(buf);
		pTextViewControlRegs->AddLineFast("");

		pTextViewControlRegs->Update();

		pClient->ReadVME16(DVCS_M_BASE + A_THRESHOLD, &val);
		sprintf(buf, "Cluster Threshold: %d", val);
		pTextClusterThreshold->SetText(buf);
		pSliderClusterThreshold->SetPosition(val);
	}
};

#endif
