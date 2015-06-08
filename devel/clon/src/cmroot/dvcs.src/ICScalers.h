#ifndef ICScalers_H
#define ICScalers_H

#include "RootHeader.h"
#include "VMERemote.h"
#include "ICTrigMap.h"

#define BTN_UPDATESCALERS			102
#define BTN_LOGONOFF				103
#define SDR_SCALERRATE				200

class ICScalers	: public TGCompositeFrame
{
public:
	ICScalers(const TGWindow *p, VMERemote *pRemoteClient) : TGCompositeFrame(p, 10, 10)
	{
		SetLayoutManager(new TGVerticalLayout(this));
		pClient = pRemoteClient;

		// Butttons Frame
		pFrameButtons = new TGHorizontalFrame(this);
		pButtonUpdateScalers = new TGTextButton(pFrameButtons, new TGHotString("Update Scalers"), BTN_UPDATESCALERS);
		pFrameButtons->AddFrame(pButtonUpdateScalers, new TGLayoutHints(kLHintsCenterX));
		pButtonUpdateScalers->Associate(this);
		pButtonToggleLogMode = new TGTextButton(pFrameButtons, new TGHotString("Z-Log On/Off"), BTN_LOGONOFF);
		pButtonToggleLogMode->Associate(this);
		pFrameButtons->AddFrame(pButtonToggleLogMode, new TGLayoutHints(kLHintsCenterX));
		pSliderScalerSampleRate = new TGHSlider(pFrameButtons, 100, kSlider1 | kScaleBoth, SDR_SCALERRATE);
		pSliderScalerSampleRate->Associate(this);
		pSliderScalerSampleRate->SetRange(1, 60);
		pSliderScalerSampleRate->SetPosition(60);
		pFrameButtons->AddFrame(pSliderScalerSampleRate, new TGLayoutHints(kLHintsCenterX));
		pTextScalerSampleRate = new TGTextEntry(pFrameButtons, new TGTextBuffer(30));
		pTextScalerSampleRate->SetWidth(110);
		pTextScalerSampleRate->SetText("manual");
		pTextScalerSampleRate->SetEnabled(kFALSE);
		pFrameButtons->AddFrame(pTextScalerSampleRate, new TGLayoutHints(kLHintsCenterX));
		AddFrame(pFrameButtons, new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX));

		pTimerUpdate = new TTimer(this, 60000, kTRUE);
		pTimerUpdate->TurnOff();

		pCanvasScalerMap = new TRootEmbeddedCanvas("IC Scalers", this, 450, 400);
		pCanvasScalerMap->GetCanvas()->SetBorderMode(0);
		AddFrame(pCanvasScalerMap, new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY));

		pCanvasScalerMap->GetCanvas()->cd();
//		pCanvasScalerMap->GetCanvas()->SetRightMargin(0.2);
		pHistScalerMap = new TH2I("IC Scalers", "IC Scalers", 23, 1, 24, 23, 1, 24);
//		pHistScalerMap->SetStats(kFALSE);
		pHistScalerMap->GetXaxis()->CenterLabels();
		pHistScalerMap->GetXaxis()->SetNdivisions(23, kFALSE);
		pHistScalerMap->GetYaxis()->CenterLabels();
		pHistScalerMap->GetYaxis()->SetNdivisions(23, kFALSE);
		pHistScalerMap->SetMinimum(-10e-9);
		pHistScalerMap->SetBarOffset(0);
		pHistScalerMap->SetBarWidth(0.25);
		pHistScalerMap->GetXaxis()->SetTickLength(1);
		pHistScalerMap->GetYaxis()->SetTickLength(1);
		pHistScalerMap->Draw("COLZTEXT");
	}
private:
	TRootEmbeddedCanvas	*pCanvasScalerMap;
	TH2I				*pHistScalerMap;
	TGHorizontalFrame	*pFrameButtons;
	TGTextButton		*pButtonUpdateScalers;
	TGTextButton		*pButtonToggleLogMode;
	TGHSlider			*pSliderScalerSampleRate;
	TGTextEntry			*pTextScalerSampleRate;

	TTimer				*pTimerUpdate;
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
				case BTN_UPDATESCALERS:
					ScalerMapUpdate();
					break;
				case BTN_LOGONOFF:
					if(pCanvasScalerMap->GetCanvas()->GetLogz())
						pCanvasScalerMap->GetCanvas()->SetLogz(0);
					else
						pCanvasScalerMap->GetCanvas()->SetLogz(1);

					pCanvasScalerMap->GetCanvas()->Modified();
					pCanvasScalerMap->GetCanvas()->Update();
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
			case SDR_SCALERRATE:
				if(pSliderScalerSampleRate->GetPosition() == 60)
				{
					sprintf(buf, "manual");
					pTextScalerSampleRate->SetText(buf);
					pTimerUpdate->TurnOff();
				}
				else
				{
					sprintf(buf, "every %dsec", pSliderScalerSampleRate->GetPosition());
					pTextScalerSampleRate->SetText(buf);
					pTimerUpdate->Start(pSliderScalerSampleRate->GetPosition() * 1000);
				}
				break;
			}
		default:
			break;
		}
		return kTRUE;
	}

	virtual Bool_t HandleTimer(TTimer *t)
	{
		if(pTimerUpdate->HasTimedOut() && (pSliderScalerSampleRate->GetPosition() != 60) )
			ScalerMapUpdate();

		return kTRUE;
	}

	void ScalerMapUpdate()
	{
		if(!pClient->WriteVME16(DVCS_Q2_BASE + A_SCALERS, 0) ||	!pClient->WriteVME16(DVCS_Q3_BASE + A_SCALERS, 0) ||
		   !pClient->WriteVME16(DVCS_Q4_BASE + A_SCALERS, 0) || !pClient->WriteVME16(DVCS_Q1_BASE + A_SCALERS, 0) )
			return;

		int i, j;
		for(i = 1; i < 24; i++)
		{
			for(j = 1; j < 24; j++)
			{
				if(pCanvasScalerMap->GetCanvas()->GetLogz())
					pHistScalerMap->SetBinContent(i, j, 0);
				else
					pHistScalerMap->SetBinContent(i, j, -1);
			}
		}

		unsigned int pScalers[106];

		if(GetQuadrantScalers(DVCS_Q2_BASE, pScalers))
		{
			for(i = 0; i < 106; i++)
				pHistScalerMap->SetBinContent(24-ICQuadrantMap[i].y, ICQuadrantMap[i].x, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q3_BASE, pScalers))
		{
			for(i = 0; i < 106; i++)
				pHistScalerMap->SetBinContent(24-ICQuadrantMap[i].x, 24-ICQuadrantMap[i].y, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q4_BASE, pScalers))
		{
			for(i = 0; i < 106; i++)
				pHistScalerMap->SetBinContent(ICQuadrantMap[i].y, 24-ICQuadrantMap[i].x, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q1_BASE, pScalers))
		{
			for(i = 0; i < 106; i++)
				pHistScalerMap->SetBinContent(ICQuadrantMap[i].x, ICQuadrantMap[i].y, pScalers[i]);
		}
		if(!pClient->WriteVME16(!pClient->WriteVME16(DVCS_Q2_BASE + A_SCALERS, 1) || !pClient->WriteVME16(DVCS_Q3_BASE + A_SCALERS, 1) ||
		   !pClient->WriteVME16(DVCS_Q4_BASE + A_SCALERS, 1) || DVCS_Q1_BASE + A_SCALERS, 1) )
			return;
/*
for(i = 0; i < 106; i++)
{
	pHistScalerMap->SetBinContent(24-ICQuadrantMap[i].y, ICQuadrantMap[i].x, 100+rand()%50);
	pHistScalerMap->SetBinContent(24-ICQuadrantMap[i].x, 24-ICQuadrantMap[i].y, 100+rand()%50);
	pHistScalerMap->SetBinContent(ICQuadrantMap[i].y, 24-ICQuadrantMap[i].x, 100+rand()%50);
	pHistScalerMap->SetBinContent(ICQuadrantMap[i].x, ICQuadrantMap[i].y, 100+rand()%50);
}
*/
		pCanvasScalerMap->GetCanvas()->cd();
		pHistScalerMap->Draw("COLZTEXT");
		pCanvasScalerMap->GetCanvas()->Modified();
		pCanvasScalerMap->GetCanvas()->Update();
	}

	Bool_t GetQuadrantScalers(unsigned int base, unsigned int *pScalers)
	{
		if(!pClient->BlkReadVME16(base + A_SCALER_BASE, (unsigned short *)pScalers, 212))
			return kFALSE;
		
		if(HTONL(1) != 1)
		{
			for(int i = 0; i < 106; i++)
				pScalers[i] = (pScalers[i]<<16) | (pScalers[i]>>16);
		}
		return kTRUE;
	}

};

#endif