#ifndef DVCSScope_H
#define DVCSScope_H

#include "RootHeader.h"
#include "VMERemote.h"
#include "HTrigMap.h"
#include "ICTrigMap.h"

#define BTN_TRIGSTART				120
#define BTN_TRIGSTOP				121
#define BTN_TRIGNEXT				122
#define BTN_TRIGPREV				123
#define BTN_TRIGAUTO				124

#define SDR_TCLUSTERCNTTRIG			210
#define SDR_HCLUSTERCNTTRIG			211
#define SDR_NHCLUSTERCNTTRIG		212

#define MAX_CLUSTERS				7

class DVCSScope	: public TGCompositeFrame
{
public:
	DVCSScope(const TGWindow *p, VMERemote *pRemoteClient) : TGCompositeFrame(p, 10, 10)
	{
		SetLayoutManager(new TGVerticalLayout(this));

		pClient = pRemoteClient;
		ScopeDisplayPos = 0xFFFF;

		pFrameTriggerButtons = new TGHorizontalFrame(this);
		pTriggerAuto = new TGTextButton(pFrameTriggerButtons, new TGHotString("Trigger Auto"), BTN_TRIGAUTO);
		pTriggerStart = new TGTextButton(pFrameTriggerButtons, new TGHotString("Trigger Start"), BTN_TRIGSTART);
		pTriggerStop = new TGTextButton(pFrameTriggerButtons, new TGHotString("Trigger Stop"), BTN_TRIGSTOP);
		pTriggerNext = new TGTextButton(pFrameTriggerButtons, new TGHotString("Trigger Next"), BTN_TRIGNEXT);
		pTriggerPrev = new TGTextButton(pFrameTriggerButtons, new TGHotString("Trigger Prev"), BTN_TRIGPREV);
		pTextTriggerTime = new TGTextEntry(pFrameTriggerButtons, pTextBufTriggerTime = new TGTextBuffer(10));
		pTextTriggerTime->SetWidth(80);
		pTextTriggerTime->SetEnabled(kFALSE);
		pTextBufTriggerTime->AddText(0, "@T+0ns");
		pFrameTriggerButtons->AddFrame(pTriggerAuto, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerButtons->AddFrame(pTriggerStart, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerButtons->AddFrame(pTriggerStop, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerButtons->AddFrame(pTriggerNext, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerButtons->AddFrame(pTriggerPrev, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerButtons->AddFrame(pTextTriggerTime, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pTriggerStop->SetEnabled(kFALSE);
		pTriggerAuto->Associate(this);
		pTriggerStart->Associate(this);
		pTriggerStop->Associate(this);
		pTriggerNext->Associate(this);
		pTriggerPrev->Associate(this);
		AddFrame(pFrameTriggerButtons, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

		pFrameTriggerVars1 = new TGHorizontalFrame(this);
		pTextTClusterCntQ[0] = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCntQ[0]->SetText("Q1-TCluster:0"); pTextTClusterCntQ[0]->SetEnabled(kFALSE); pTextTClusterCntQ[0]->SetWidth(110);
		pTextTClusterCntQ[1] = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCntQ[1]->SetText("Q2-TCluster:0"); pTextTClusterCntQ[1]->SetEnabled(kFALSE); pTextTClusterCntQ[1]->SetWidth(110);
		pTextTClusterCntQ[2] = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCntQ[2]->SetText("Q3-TCluster:0"); pTextTClusterCntQ[2]->SetEnabled(kFALSE); pTextTClusterCntQ[2]->SetWidth(110);
		pTextTClusterCntQ[3] = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCntQ[3]->SetText("Q4-TCluster:0"); pTextTClusterCntQ[3]->SetEnabled(kFALSE); pTextTClusterCntQ[3]->SetWidth(110);
		pTextTClusterCnt = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCnt->SetText("TCluster:0"); pTextTClusterCnt->SetEnabled(kFALSE); pTextTClusterCnt->SetWidth(110);
		pTextTClusterCntTrig = new TGTextEntry(pFrameTriggerVars1, new TGTextBuffer(15)); pTextTClusterCntTrig->SetText("Trigger:X"); pTextTClusterCntTrig->SetEnabled(kFALSE); pTextTClusterCntTrig->SetWidth(110);
		pSliderTClusterCntTrig = new TGHSlider(pFrameTriggerVars1, 100, kSlider1 | kScaleBoth, SDR_TCLUSTERCNTTRIG);
		pSliderTClusterCntTrig->SetRange(0, MAX_CLUSTERS+1);
		pSliderTClusterCntTrig->SetPosition(MAX_CLUSTERS+1);
		pSliderTClusterCntTrig->SetWidth(80);
		pSliderTClusterCntTrig->Associate(this);
		pFrameTriggerVars1->AddFrame(pTextTClusterCntQ[0], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pTextTClusterCntQ[1], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pTextTClusterCntQ[2], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pTextTClusterCntQ[3], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pTextTClusterCnt, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pTextTClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars1->AddFrame(pSliderTClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		AddFrame(pFrameTriggerVars1, new TGLayoutHints(kLHintsExpandX));

		pFrameTriggerVars2 = new TGHorizontalFrame(this);
		pTextHClusterCntQ[0] = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCntQ[0]->SetText("Q1-HCluster:0"); pTextHClusterCntQ[0]->SetEnabled(kFALSE); pTextHClusterCntQ[0]->SetWidth(110);
		pTextHClusterCntQ[1] = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCntQ[1]->SetText("Q2-HCluster:0"); pTextHClusterCntQ[1]->SetEnabled(kFALSE); pTextHClusterCntQ[1]->SetWidth(110);
		pTextHClusterCntQ[2] = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCntQ[2]->SetText("Q3-HCluster:0"); pTextHClusterCntQ[2]->SetEnabled(kFALSE); pTextHClusterCntQ[2]->SetWidth(110);
		pTextHClusterCntQ[3] = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCntQ[3]->SetText("Q4-HCluster:0"); pTextHClusterCntQ[3]->SetEnabled(kFALSE); pTextHClusterCntQ[3]->SetWidth(110);
		pTextHClusterCnt = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCnt->SetText("HCluster:0"); pTextHClusterCnt->SetEnabled(kFALSE); pTextHClusterCnt->SetWidth(110);
		pTextHClusterCntTrig = new TGTextEntry(pFrameTriggerVars2, new TGTextBuffer(15)); pTextHClusterCntTrig->SetText("Trigger:X"); pTextHClusterCntTrig->SetEnabled(kFALSE); pTextHClusterCntTrig->SetWidth(110);
		pSliderHClusterCntTrig = new TGHSlider(pFrameTriggerVars2, 100, kSlider1 | kScaleBoth, SDR_HCLUSTERCNTTRIG);
		pSliderHClusterCntTrig->SetRange(0, MAX_CLUSTERS+1);
		pSliderHClusterCntTrig->SetPosition(MAX_CLUSTERS+1);
		pSliderHClusterCntTrig->SetWidth(80);
		pSliderHClusterCntTrig->Associate(this);
		pFrameTriggerVars2->AddFrame(pTextHClusterCntQ[0], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pTextHClusterCntQ[1], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pTextHClusterCntQ[2], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pTextHClusterCntQ[3], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pTextHClusterCnt, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pTextHClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars2->AddFrame(pSliderHClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		AddFrame(pFrameTriggerVars2, new TGLayoutHints(kLHintsExpandX));

		pFrameTriggerVars3 = new TGHorizontalFrame(this);
		pTextNHClusterCntQ[0] = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCntQ[0]->SetText("Q1-NHCluster:0"); pTextNHClusterCntQ[0]->SetEnabled(kFALSE); pTextNHClusterCntQ[0]->SetWidth(110);
		pTextNHClusterCntQ[1] = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCntQ[1]->SetText("Q2-NHCluster:0"); pTextNHClusterCntQ[1]->SetEnabled(kFALSE); pTextNHClusterCntQ[1]->SetWidth(110);
		pTextNHClusterCntQ[2] = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCntQ[2]->SetText("Q3-NHCluster:0"); pTextNHClusterCntQ[2]->SetEnabled(kFALSE); pTextNHClusterCntQ[2]->SetWidth(110);
		pTextNHClusterCntQ[3] = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCntQ[3]->SetText("Q4-NHCluster:0"); pTextNHClusterCntQ[3]->SetEnabled(kFALSE); pTextNHClusterCntQ[3]->SetWidth(110);
		pTextNHClusterCnt = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCnt->SetText("NHCluster:0"); pTextNHClusterCnt->SetEnabled(kFALSE); pTextNHClusterCnt->SetWidth(110);
		pTextNHClusterCntTrig = new TGTextEntry(pFrameTriggerVars3, new TGTextBuffer(15)); pTextNHClusterCntTrig->SetText("Trigger:X"); pTextNHClusterCntTrig->SetEnabled(kFALSE); pTextNHClusterCntTrig->SetWidth(110);
		pSliderNHClusterCntTrig = new TGHSlider(pFrameTriggerVars3, 100, kSlider1 | kScaleBoth, SDR_NHCLUSTERCNTTRIG);
		pSliderNHClusterCntTrig->SetRange(0, MAX_CLUSTERS+1);
		pSliderNHClusterCntTrig->SetPosition(MAX_CLUSTERS+1);
		pSliderNHClusterCntTrig->SetWidth(80);
		pSliderNHClusterCntTrig->Associate(this);
		pFrameTriggerVars3->AddFrame(pTextNHClusterCntQ[0], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pTextNHClusterCntQ[1], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pTextNHClusterCntQ[2], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pTextNHClusterCntQ[3], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pTextNHClusterCnt, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pTextNHClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		pFrameTriggerVars3->AddFrame(pSliderNHClusterCntTrig, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX));
		AddFrame(pFrameTriggerVars3, new TGLayoutHints(kLHintsExpandX));

		pFrameICPlots = new TGHorizontalFrame(this);
		pICTrigCapture = new ICTrigMap("IC Trigger Capture", pFrameICPlots, kFALSE);
		pICTrigPattern = new ICTrigMap("IC Trigger Pattern", pFrameICPlots);
		pFrameICPlots->AddFrame(pICTrigCapture, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsLeft));
		pFrameICPlots->AddFrame(pICTrigPattern, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsRight));
		AddFrame(pFrameICPlots, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsBottom));

		pFrameHPlots = new TGHorizontalFrame(this);
		pHTrigCapture = new HTrigMap("Hodoscope Trigger Capture", pFrameHPlots, kFALSE);
		pHTrigPattern = new HTrigMap("Hodoscope Trigger Pattern", pFrameHPlots);
		pFrameHPlots->AddFrame(pHTrigCapture, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsLeft));
		pFrameHPlots->AddFrame(pHTrigPattern, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsRight));
		AddFrame(pFrameHPlots, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsBottom));

		pTimerUpdate = new TTimer(this, 100, kTRUE);
	}
private:
	Bool_t				bAutoMode;

	TGHorizontalFrame	*pFrameTriggerButtons;
	TGTextButton		*pTriggerAuto;
	TGTextButton		*pTriggerStart;
	TGTextButton		*pTriggerStop;
	TGTextButton		*pTriggerNext;
	TGTextButton		*pTriggerPrev;
	TGTextEntry			*pTextTriggerTime;
	TGTextBuffer		*pTextBufTriggerTime;

	TGHorizontalFrame	*pFrameTriggerVars1;
	TGHorizontalFrame	*pFrameTriggerVars2;
	TGHorizontalFrame	*pFrameTriggerVars3;
	TGTextEntry			*pTextTClusterCntQ[4];
	TGTextEntry			*pTextHClusterCntQ[4];
	TGTextEntry			*pTextNHClusterCntQ[4];
	TGTextEntry			*pTextTClusterCnt;
	TGTextEntry			*pTextHClusterCnt;
	TGTextEntry			*pTextNHClusterCnt;
	TGTextEntry			*pTextTClusterCntTrig;
	TGTextEntry			*pTextHClusterCntTrig;
	TGTextEntry			*pTextNHClusterCntTrig;
	TGHSlider			*pSliderTClusterCntTrig;
	TGHSlider			*pSliderHClusterCntTrig;
	TGHSlider			*pSliderNHClusterCntTrig;

	TGHorizontalFrame	*pFrameICPlots;
	ICTrigMap			*pICTrigPattern;
	ICTrigMap			*pICTrigCapture;

	TGHorizontalFrame	*pFrameHPlots;
	HTrigMap			*pHTrigPattern;
	HTrigMap			*pHTrigCapture;

	TTimer				*pTimerUpdate;
	VMERemote			*pClient;

	unsigned short		ScopeDisplayPos;
	unsigned short		ScopeBufferQ1[2049];
	unsigned short		ScopePointerQ1;
	unsigned short		ScopeBufferQ2[2049];
	unsigned short		ScopePointerQ2;
	unsigned short		ScopeBufferQ3[2049];
	unsigned short		ScopePointerQ3;
	unsigned short		ScopeBufferQ4[2049];
	unsigned short		ScopePointerQ4;
	unsigned short		ScopeBufferM[2049];
	unsigned short		ScopePointerM;


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
				case BTN_TRIGSTART:
					bAutoMode = kFALSE;
					StartTrigger();
					break;
				case BTN_TRIGSTOP:
					StopTrigger();
					break;
				case BTN_TRIGNEXT:
					NextTrigger();
					break;
				case BTN_TRIGPREV:
					PrevTrigger();
					break;
				case BTN_TRIGAUTO:
					bAutoMode = kTRUE;
					StartTrigger();
					break;
				default:
					printf("button id %d pressed\n", parm1);
					break;
				}
			}
			break;
		case kC_HSLIDER:
			switch(parm1)
			{
			case SDR_TCLUSTERCNTTRIG:
				if(pSliderTClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
					sprintf(buf, "Trigger:X");
				else
					sprintf(buf, "Trigger:%d", pSliderTClusterCntTrig->GetPosition());

				pTextTClusterCntTrig->SetText(buf);
				break;
			case SDR_HCLUSTERCNTTRIG:
				if(pSliderHClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
					sprintf(buf, "Trigger:X");
				else
					sprintf(buf, "Trigger:%d", pSliderHClusterCntTrig->GetPosition());

				pTextHClusterCntTrig->SetText(buf);
				break;
			case SDR_NHCLUSTERCNTTRIG:
				if(pSliderNHClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
					sprintf(buf, "Trigger:X");
				else
					sprintf(buf, "Trigger:%d", pSliderNHClusterCntTrig->GetPosition());

				pTextNHClusterCntTrig->SetText(buf);
				break;
			}
			break;
		default:
			break;
		}
		return kTRUE;
	}

	virtual Bool_t HandleTimer(TTimer *t)
	{
		if(pTimerUpdate->HasTimedOut())
			CheckTrigger();

		return kTRUE;
	}

	void CheckTrigger()
	{
		pTimerUpdate->TurnOff();
		unsigned short val;
		if(pClient->ReadVME16(DVCS_M_BASE + A_SCOPE_STATUS, &val))
		{
			if(!(val & 0x0001))
			{
				pTimerUpdate->TurnOn();
				return;
			}
			CaptureTriggerData();

			if(bAutoMode)
			{
				pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_STATUS, 0x0000);
				pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_STATUS, 0x0002);
				pTimerUpdate->TurnOn();
				return;
			}
		}
		StopTrigger();
	}

	void StartTrigger()
	{
		unsigned int QBaseAddresses[] = {DVCS_Q1_BASE, DVCS_Q2_BASE, DVCS_Q3_BASE, DVCS_Q4_BASE};
		for(int i = 0; i < 4; i++)
		{
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x00, pICTrigPattern->GetTowerTrigVal(i, 0));															// 0-15
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x02, pICTrigPattern->GetTowerTrigVal(i, 1));															// 16-31
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x04, pICTrigPattern->GetTowerTrigVal(i, 2));															// 32-47
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x06, pICTrigPattern->GetTowerTrigVal(i, 3));															// 48-63
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x08, pICTrigPattern->GetTowerTrigVal(i, 4));															// 64-79
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x0A, pICTrigPattern->GetTowerTrigVal(i, 5));															// 80-95
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x0C, pICTrigPattern->GetTowerTrigVal(i, 6) | (pHTrigPattern->GetTowerTrigVal(i, 0)<<10));			// 96-111
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x0E, (pHTrigPattern->GetTowerTrigVal(i, 0)>>6) & 0x00FF);											// 112-127
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x10, pICTrigPattern->GetClusterTrigVal(i, 0)<<4);													// 128-143
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x12, (pICTrigPattern->GetClusterTrigVal(i, 0)>>12) | (pICTrigPattern->GetClusterTrigVal(i, 1)<<4));	// 144-159
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x14, (pICTrigPattern->GetClusterTrigVal(i, 1)>>12) | (pICTrigPattern->GetClusterTrigVal(i, 2)<<4));	// 160-175
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x16, (pICTrigPattern->GetClusterTrigVal(i, 2)>>12) | (pICTrigPattern->GetClusterTrigVal(i, 3)<<4));	// 176-191
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x18, (pICTrigPattern->GetClusterTrigVal(i, 3)>>12) | (pICTrigPattern->GetClusterTrigVal(i, 4)<<4));	// 192-207
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x1A, 0x0000);																						// 208-223
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x1C, 0x0000);																						// 224-239
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_VAL_BASE + 0x1E, 0x0000);																						// 240-255

			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x00, pICTrigPattern->GetTowerIgnoreVal(i, 0));															// 0-15
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x02, pICTrigPattern->GetTowerIgnoreVal(i, 1));															// 16-31
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x04, pICTrigPattern->GetTowerIgnoreVal(i, 2));															// 32-47
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x06, pICTrigPattern->GetTowerIgnoreVal(i, 3));															// 48-63
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x08, pICTrigPattern->GetTowerIgnoreVal(i, 4));															// 64-79
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x0A, pICTrigPattern->GetTowerIgnoreVal(i, 5));															// 80-95
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x0C, pICTrigPattern->GetTowerIgnoreVal(i, 6) | (pHTrigPattern->GetTowerIgnoreVal(i, 0)<<10));				// 96-111
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x0E, ((pHTrigPattern->GetTowerIgnoreVal(i, 0)>>6) & 0x00FF) | 0xFF00);									// 112-127
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x10, 0xF | (pICTrigPattern->GetClusterIgnoreVal(i, 0)<<4));												// 128-143
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x12, (pICTrigPattern->GetClusterIgnoreVal(i, 0)>>12) | (pICTrigPattern->GetClusterIgnoreVal(i, 1)<<4));	// 144-159
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x14, (pICTrigPattern->GetClusterIgnoreVal(i, 1)>>12) | (pICTrigPattern->GetClusterIgnoreVal(i, 2)<<4));	// 160-175
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x16, (pICTrigPattern->GetClusterIgnoreVal(i, 2)>>12) | (pICTrigPattern->GetClusterIgnoreVal(i, 3)<<4));	// 176-191
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x18, (pICTrigPattern->GetClusterIgnoreVal(i, 3)>>12) | (pICTrigPattern->GetClusterIgnoreVal(i, 4)<<4) | 0xFE00);	// 192-207
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x1A, 0xFFFF);																								// 208-223
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x1C, 0xFFFF);																								// 224-239
			pClient->WriteVME16(QBaseAddresses[i] + A_SCOPE_IGNORE_BASE + 0x1E, 0xFFFF);																								// 240-255
		}
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x00, pICTrigPattern->GetMClusterTrigVal(0, 0));															// 0-15
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x02, pICTrigPattern->GetMClusterTrigVal(0, 1));															// 16-31
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x04, pICTrigPattern->GetMClusterTrigVal(0, 2) | (pICTrigPattern->GetMClusterTrigVal(1, 0)<<5));			// 32-47
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x06, (pICTrigPattern->GetMClusterTrigVal(1, 0)>>11) | (pICTrigPattern->GetMClusterTrigVal(1, 1)<<5));		// 48-63
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x08, (pICTrigPattern->GetMClusterTrigVal(1, 1)>>11) | (pICTrigPattern->GetMClusterTrigVal(1, 2)<<5) | (pICTrigPattern->GetMClusterTrigVal(2, 0)<<10));	// 64-79
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x0A, (pICTrigPattern->GetMClusterTrigVal(2, 0)>>6) | (pICTrigPattern->GetMClusterTrigVal(2, 1)<<10));		// 80-95
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x0C, (pICTrigPattern->GetMClusterTrigVal(2, 1)>>6) | (pICTrigPattern->GetMClusterTrigVal(2, 2)<<10) | (pICTrigPattern->GetMClusterTrigVal(3, 0)<<15));	// 96-111
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x0E, (pICTrigPattern->GetMClusterTrigVal(3, 0)>>1) | (pICTrigPattern->GetMClusterTrigVal(3, 1)<<15));		// 112-127
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x10, (pICTrigPattern->GetMClusterTrigVal(3, 1)>>1) | (pICTrigPattern->GetMClusterTrigVal(3, 2)<<15));		// 128-143
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x12, (pICTrigPattern->GetMClusterTrigVal(3, 2)>>1) | (pSliderTClusterCntTrig->GetPosition()<<4) | (pSliderHClusterCntTrig->GetPosition()<<11));	// 144-159
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x14, (pSliderHClusterCntTrig->GetPosition()>>5) | (pSliderNHClusterCntTrig->GetPosition()<<2));													// 160-175
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x16, 0x0000);																								// 176-191
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x18, 0x0000);																								// 192-207
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x1A, 0x0000);																								// 208-223
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x1C, 0x0000);																								// 224-239
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_VAL_BASE + 0x1E, 0x0000);																								// 240-255

		unsigned int TClusterCntIgnore;
		if(pSliderTClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
			TClusterCntIgnore = 0x7F;
		else
			TClusterCntIgnore = 0;

		unsigned int HClusterCntIgnore;
		if(pSliderHClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
			HClusterCntIgnore = 0x7F;
		else
			HClusterCntIgnore = 0;

		unsigned int NHClusterCntIgnore;
		if(pSliderNHClusterCntTrig->GetPosition() == MAX_CLUSTERS+1)
			NHClusterCntIgnore = 0x7F;
		else
			NHClusterCntIgnore = 0;

		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x00, pICTrigPattern->GetMClusterIgnoreVal(0, 0));															// 0-15
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x02, pICTrigPattern->GetMClusterIgnoreVal(0, 1));															// 16-31
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x04, pICTrigPattern->GetMClusterIgnoreVal(0, 2) | (pICTrigPattern->GetMClusterIgnoreVal(1, 0)<<5));			// 32-47
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x06, (pICTrigPattern->GetMClusterIgnoreVal(1, 0)>>11) | (pICTrigPattern->GetMClusterIgnoreVal(1, 1)<<5));	// 48-63
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x08, (pICTrigPattern->GetMClusterIgnoreVal(1, 1)>>11) | (pICTrigPattern->GetMClusterIgnoreVal(1, 2)<<5) | (pICTrigPattern->GetMClusterIgnoreVal(2, 0)<<10));	// 64-79
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x0A, (pICTrigPattern->GetMClusterIgnoreVal(2, 0)>>6) | (pICTrigPattern->GetMClusterIgnoreVal(2, 1)<<10));	// 80-95
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x0C, (pICTrigPattern->GetMClusterIgnoreVal(2, 1)>>6) | (pICTrigPattern->GetMClusterIgnoreVal(2, 2)<<10) | (pICTrigPattern->GetMClusterIgnoreVal(3, 0)<<15));	// 96-111
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x0E, (pICTrigPattern->GetMClusterIgnoreVal(3, 0)>>1) | (pICTrigPattern->GetMClusterIgnoreVal(3, 1)<<15));	// 112-127
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x10, (pICTrigPattern->GetMClusterIgnoreVal(3, 1)>>1) | (pICTrigPattern->GetMClusterIgnoreVal(3, 2)<<15));	// 128-143
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x12, (pICTrigPattern->GetMClusterIgnoreVal(3, 2)>>1) | (TClusterCntIgnore<<4) | (HClusterCntIgnore<<11));	// 144-159
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x14, (HClusterCntIgnore>>5) | (NHClusterCntIgnore<<2));														// 160-175
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x16, 0x0000);																								// 176-191
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x18, 0x0000);																								// 192-207
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x1A, 0x0000);																								// 208-223
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x1C, 0x0000);																								// 224-239
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_IGNORE_BASE + 0x1E, 0x0000);																								// 240-255

		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_STATUS, 0x0000);
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_STATUS, 0x0002);

		pTimerUpdate->TurnOn();	
		pTriggerStop->SetEnabled(kTRUE);
		pTriggerStart->SetEnabled(kFALSE);
		pTriggerAuto->SetEnabled(kFALSE);
	}

	void StopTrigger()
	{
		pTriggerStop->SetEnabled(kFALSE);
		pTriggerStart->SetEnabled(kTRUE);
		pTriggerAuto->SetEnabled(kTRUE);
		pTimerUpdate->TurnOff();
		pClient->WriteVME16(DVCS_M_BASE + A_SCOPE_STATUS, 0x0000);
	}

	void NextTrigger()
	{
		if(ScopeDisplayPos + 3 < 128+2)
		{
			ScopeDisplayPos += 3;
			UpdateTriggerCaptures();

			char buf[20];
			sprintf(buf, "@T%+dns", (ScopeDisplayPos-63)*5);
			pTextBufTriggerTime->Clear();
			pTextBufTriggerTime->AddText(0, buf);
			gClient->NeedRedraw(pTextTriggerTime);
		}
	}

	void PrevTrigger()
	{
		if(ScopeDisplayPos > 3+2)
		{
			ScopeDisplayPos -= 3;
			UpdateTriggerCaptures();

			char buf[20];
			sprintf(buf, "@T%+dns", (ScopeDisplayPos-63)*5);
			pTextBufTriggerTime->Clear();
			pTextBufTriggerTime->AddText(0, buf);
			gClient->NeedRedraw(pTextTriggerTime);
		}
	}

	void CaptureTriggerData()
	{
		printf("Read Scope Buffers Started...\n");
		ReadoutTriggerBuffer(DVCS_M_BASE, &ScopePointerM, ScopeBufferM);
		ReadoutTriggerBuffer(DVCS_Q1_BASE, &ScopePointerQ1, ScopeBufferQ1);
		ReadoutTriggerBuffer(DVCS_Q2_BASE, &ScopePointerQ2, ScopeBufferQ2);
		ReadoutTriggerBuffer(DVCS_Q3_BASE, &ScopePointerQ3, ScopeBufferQ3);
		ReadoutTriggerBuffer(DVCS_Q4_BASE, &ScopePointerQ4, ScopeBufferQ4);
		printf("Q1 Wr Pos=%d\n", (int)ScopePointerQ1);
		printf("Q2 Wr Pos=%d\n", (int)ScopePointerQ2);
		printf("Q3 Wr Pos=%d\n", (int)ScopePointerQ3);
		printf("Q4 Wr Pos=%d\n", (int)ScopePointerQ4);
		printf("M Wr Pos=%d\n", (int)ScopePointerM);
		printf("Read Scope Buffers Ended...\n");

		ScopeDisplayPos = 63;
		UpdateTriggerCaptures();

		char buf[20];
		sprintf(buf, "@T%+dns", (ScopeDisplayPos-63)*5);
		pTextBufTriggerTime->Clear();
		pTextBufTriggerTime->AddText(0, buf);
		gClient->NeedRedraw(pTextTriggerTime);
	}

	Bool_t ReadoutTriggerBuffer(unsigned int base, unsigned short *pWritePointer, unsigned short *buf)
	{
		*pWritePointer = 0;

		if(!pClient->ReadVME16(base + A_SCOPE_WR_POS, pWritePointer))
			return kFALSE;

		if(!pClient->BlkReadVME16(base + A_SCOPE_BUF, buf, 2049))
			return kFALSE;

		for(int i = 0; i < 2048; i++)
			buf[i] = buf[i+1];

		return kTRUE;
	}

	int GetScopeBufBit(int q, int bit, int toffset, int count = 1)
	{
		unsigned int result = 0;
		unsigned int mask = 1;

		while(count--)
		{
			if( (q == 0) && (ScopeBufferQ1[((ScopePointerQ1 + ScopeDisplayPos + toffset) & 0x7F) * 16 + (bit>>4)] & (1<<(bit&0xF))) )	result |= mask;
			if( (q == 1) && (ScopeBufferQ2[((ScopePointerQ2 + ScopeDisplayPos + toffset) & 0x7F) * 16 + (bit>>4)] & (1<<(bit&0xF))) )	result |= mask;
			if( (q == 2) && (ScopeBufferQ3[((ScopePointerQ3 + ScopeDisplayPos + toffset) & 0x7F) * 16 + (bit>>4)] & (1<<(bit&0xF))) )	result |= mask;
			if( (q == 3) && (ScopeBufferQ4[((ScopePointerQ4 + ScopeDisplayPos + toffset) & 0x7F) * 16 + (bit>>4)] & (1<<(bit&0xF))) )	result |= mask;
			if( (q == 4) && (ScopeBufferM[((ScopePointerM + ScopeDisplayPos + toffset) & 0x7F) * 16 + (bit>>4)] & (1<<(bit&0xF))) )	result |= mask;
			mask <<= 1;
			bit++;
		}

		return result;
	}

	void UpdateTriggerCaptures()
	{
		int i;
		for(i = 0; i < 202; i++)
		{
			if(i < 106)
			{
				if(GetScopeBufBit(0, i, -2))	pICTrigCapture->SetICVal(0, i, IC_HISTVAL_1);
				else						pICTrigCapture->SetICVal(0, i, IC_HISTVAL_0);
				if(GetScopeBufBit(1, i, -2))	pICTrigCapture->SetICVal(1, i, IC_HISTVAL_1);
				else						pICTrigCapture->SetICVal(1, i, IC_HISTVAL_0);
				if(GetScopeBufBit(2, i, -2))	pICTrigCapture->SetICVal(2, i, IC_HISTVAL_1);
				else						pICTrigCapture->SetICVal(2, i, IC_HISTVAL_0);
				if(GetScopeBufBit(3, i, -2))	pICTrigCapture->SetICVal(3, i, IC_HISTVAL_1);
				else						pICTrigCapture->SetICVal(3, i, IC_HISTVAL_0);
			}
			
			if( (106 <= i) && (i <= 119) )
			{
				if(GetScopeBufBit(0, i, -2))	pHTrigCapture->SetHVal(0, i-106, H_HISTVAL_1);
				else						pHTrigCapture->SetHVal(0, i-106, H_HISTVAL_0);
				if(GetScopeBufBit(1, i, -2))	pHTrigCapture->SetHVal(1, i-106, H_HISTVAL_1);
				else						pHTrigCapture->SetHVal(1, i-106, H_HISTVAL_0);
				if(GetScopeBufBit(2, i, -2))	pHTrigCapture->SetHVal(2, i-106, H_HISTVAL_1);
				else						pHTrigCapture->SetHVal(2, i-106, H_HISTVAL_0);
				if(GetScopeBufBit(3, i, -2))	pHTrigCapture->SetHVal(3, i-106, H_HISTVAL_1);
				else						pHTrigCapture->SetHVal(3, i-106, H_HISTVAL_0);
			}

			if( (132 <= i) && (i <= 200) )
			{
				if(GetScopeBufBit(0, i, -2) && (pICTrigCapture->GetICVal(0, QClusterMap[i-132]) == IC_HISTVAL_1))	pICTrigCapture->SetICVal(0, QClusterMap[i-132], IC_HISTVAL_C1);
				else if(GetScopeBufBit(0, i, -2))																	pICTrigCapture->SetICVal(0, QClusterMap[i-132], IC_HISTVAL_C0);
				if(GetScopeBufBit(1, i, -2) && (pICTrigCapture->GetICVal(1, QClusterMap[i-132]) == IC_HISTVAL_1))	pICTrigCapture->SetICVal(1, QClusterMap[i-132], IC_HISTVAL_C1);
				else if(GetScopeBufBit(1, i, -2))																	pICTrigCapture->SetICVal(1, QClusterMap[i-132], IC_HISTVAL_C0);
				if(GetScopeBufBit(2, i, -2) && (pICTrigCapture->GetICVal(2, QClusterMap[i-132]) == IC_HISTVAL_1))	pICTrigCapture->SetICVal(2, QClusterMap[i-132], IC_HISTVAL_C1);
				else if(GetScopeBufBit(2, i, -2))																	pICTrigCapture->SetICVal(2, QClusterMap[i-132], IC_HISTVAL_C0);
				if(GetScopeBufBit(3, i, -2) && (pICTrigCapture->GetICVal(3, QClusterMap[i-132]) == IC_HISTVAL_1))	pICTrigCapture->SetICVal(3, QClusterMap[i-132], IC_HISTVAL_C1);
				else if(GetScopeBufBit(3, i, -2))																	pICTrigCapture->SetICVal(3, QClusterMap[i-132], IC_HISTVAL_C0);
			}
		}

		for(i = 0; i <= 36; i++)
		{
			if(GetScopeBufBit(4, i, 2) && (pICTrigCapture->GetICVal(0, MClusterMap[i]) == IC_HISTVAL_1))
				pICTrigCapture->SetICVal(0, MClusterMap[i], IC_HISTVAL_C1);
			else if(GetScopeBufBit(4, i, 2))
				pICTrigCapture->SetICVal(0, MClusterMap[i], IC_HISTVAL_C0);

			if(GetScopeBufBit(4, i+37, 2) && (pICTrigCapture->GetICVal(1, MClusterMap[i]) == IC_HISTVAL_1))
				pICTrigCapture->SetICVal(1, MClusterMap[i], IC_HISTVAL_C1);
			else if(GetScopeBufBit(4, i+37, 2))
				pICTrigCapture->SetICVal(1, MClusterMap[i], IC_HISTVAL_C0);

			if(GetScopeBufBit(4, i+74, 2) && (pICTrigCapture->GetICVal(2, MClusterMap[i]) == IC_HISTVAL_1))
				pICTrigCapture->SetICVal(2, MClusterMap[i], IC_HISTVAL_C1);
			else if(GetScopeBufBit(4, i+74, 2))
				pICTrigCapture->SetICVal(2, MClusterMap[i], IC_HISTVAL_C0);

			if(GetScopeBufBit(4, i+111, 2) && (pICTrigCapture->GetICVal(3, MClusterMap[i]) == IC_HISTVAL_1))
				pICTrigCapture->SetICVal(3, MClusterMap[i], IC_HISTVAL_C1);
			else if(GetScopeBufBit(4, i+111, 2))
				pICTrigCapture->SetICVal(3, MClusterMap[i], IC_HISTVAL_C0);
		}

		char buf[20];
		for(i = 0; i < 4; i++)
		{
			sprintf(buf, "Q%d-TCluster:%d", i+1, GetScopeBufBit(i, 201, -2, 4));
			pTextTClusterCntQ[i]->SetText(buf);

			sprintf(buf, "Q%d-HCluster:%d", i+1, GetScopeBufBit(i, 205, -2, 4));
			pTextHClusterCntQ[i]->SetText(buf);

			sprintf(buf, "Q%d-NHCluster:%d", i+1, GetScopeBufBit(i, 209, -2, 4));
			pTextNHClusterCntQ[i]->SetText(buf);
		}
		sprintf(buf, "TCluster:%d", GetScopeBufBit(4, 148, 2, 7));
		pTextTClusterCnt->SetText(buf);

		sprintf(buf, "HCluster:%d", GetScopeBufBit(4, 155, 2, 7));
		pTextHClusterCnt->SetText(buf);

		sprintf(buf, "NHCluster:%d", GetScopeBufBit(4, 162, 2, 7));
		pTextNHClusterCnt->SetText(buf);

		pICTrigCapture->TriggerMapUpdate();
		pHTrigCapture->TriggerMapUpdate();
	}

};

#endif
