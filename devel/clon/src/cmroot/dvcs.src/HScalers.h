#ifndef HScalers_H
#define HScalers_H

#include "RootHeader.h"
#include "VMERemote.h"
#include "HTrigMap.h"

#define BTN_UPDATESCALERS			102
#define BTN_LOGONOFF				103
#define SDR_SCALERRATE				200

class HScalers	: public TGCompositeFrame
{
public:
	HScalers(const TGWindow *p, VMERemote *pRemoteClient) : TGCompositeFrame(p, 10, 10)
	{
		SetLayoutManager(new TGVerticalLayout(this));
		pClient = pRemoteClient;

		// Butttons Frame
		pFrameScalerButtons = new TGHorizontalFrame(this);
		pButtonScalerUpdate = new TGTextButton(pFrameScalerButtons, new TGHotString("Update Scalers"), BTN_UPDATESCALERS);
		pButtonScalerUpdate->Associate(this);
		pFrameScalerButtons->AddFrame(pButtonScalerUpdate, new TGLayoutHints(kLHintsCenterX));
		pButtonToggleLogMode = new TGTextButton(pFrameScalerButtons, new TGHotString("Z-Log On/Off"), BTN_LOGONOFF);
		pButtonToggleLogMode->Associate(this);
		pFrameScalerButtons->AddFrame(pButtonToggleLogMode, new TGLayoutHints(kLHintsCenterX));
		pSliderScalerSampleRate = new TGHSlider(pFrameScalerButtons, 100, kSlider1 | kScaleBoth, SDR_SCALERRATE);
		pSliderScalerSampleRate->Associate(this);
		pSliderScalerSampleRate->SetRange(1, 60);
		pSliderScalerSampleRate->SetPosition(60);
		pFrameScalerButtons->AddFrame(pSliderScalerSampleRate, new TGLayoutHints(kLHintsCenterX));
		pTextScalerSampleRate = new TGTextEntry(pFrameScalerButtons, new TGTextBuffer(30));
		pTextScalerSampleRate->SetWidth(110);
		pTextScalerSampleRate->SetText("manual");
		pTextScalerSampleRate->SetEnabled(kFALSE);
		pFrameScalerButtons->AddFrame(pTextScalerSampleRate, new TGLayoutHints(kLHintsCenterX));
		AddFrame(pFrameScalerButtons, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

		pCanvasHodoscopeScalerMap = new TRootEmbeddedCanvas("Hodoscope Scalers", this);
		pCanvasHodoscopeScalerMap->GetCanvas()->SetBorderMode(0);
		AddFrame(pCanvasHodoscopeScalerMap, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

		pCanvasHodoscopeScalerMap->GetCanvas()->cd();
		pHistHodoscopeScalerMap = new TH2I("Hodoscope Scalers", "Hodoscope Scalers", 8, 1, 9, 8, 1, 9);
		pHistHodoscopeScalerMap->SetStats(kFALSE);
		pHistHodoscopeScalerMap->SetBarOffset(0);
		pHistHodoscopeScalerMap->SetBarWidth(0.25);
		pHistHodoscopeScalerMap->GetXaxis()->CenterLabels();
		pHistHodoscopeScalerMap->GetXaxis()->SetNdivisions(8, kFALSE);
		pHistHodoscopeScalerMap->GetYaxis()->CenterLabels();
		pHistHodoscopeScalerMap->GetYaxis()->SetNdivisions(8, kFALSE);
		pHistHodoscopeScalerMap->SetMinimum(-10e-9);
		pHistHodoscopeScalerMap->GetXaxis()->SetTickLength(1);
		pHistHodoscopeScalerMap->GetYaxis()->SetTickLength(1);
		pHistHodoscopeScalerMap->Draw("COLZ");

		pTimerUpdate = new TTimer(this, 60000, kTRUE);
		pTimerUpdate->TurnOff();
	}
private:
	TRootEmbeddedCanvas	*pCanvasHodoscopeScalerMap;
	TH2I				*pHistHodoscopeScalerMap;
	TGHorizontalFrame	*pFrameScalerButtons;
	TGTextButton		*pButtonScalerUpdate;
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
					if(pCanvasHodoscopeScalerMap->GetCanvas()->GetLogz())
						pCanvasHodoscopeScalerMap->GetCanvas()->SetLogz(0);
					else
						pCanvasHodoscopeScalerMap->GetCanvas()->SetLogz(1);

					pCanvasHodoscopeScalerMap->GetCanvas()->Modified();
					pCanvasHodoscopeScalerMap->GetCanvas()->Update();
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
		}
		return kTRUE;
	}

	void ScalerMapUpdate()
	{
		int i, j;
		for(i = 1; i < 9; i++)
		{
			for(j = 1; j < 9; j++)
			{
				if(pCanvasHodoscopeScalerMap->GetCanvas()->GetLogz())
					pHistHodoscopeScalerMap->SetBinContent(i, j, 0);
				else
					pHistHodoscopeScalerMap->SetBinContent(i, j, -1);
			}
		}

		if(!pClient->WriteVME16(DVCS_Q1_BASE + A_SCALERS, 0) ||	!pClient->WriteVME16(DVCS_Q2_BASE + A_SCALERS, 0) ||
		   !pClient->WriteVME16(DVCS_Q3_BASE + A_SCALERS, 0) || !pClient->WriteVME16(DVCS_Q4_BASE + A_SCALERS, 0) )
			return;

		unsigned int pScalers[14];
		if(GetQuadrantScalers(DVCS_Q1_BASE, pScalers))
		{
			for(i = 0; i < 14; i++)
				pHistHodoscopeScalerMap->SetBinContent(HQuadrantMap[i].x, HQuadrantMap[i].y, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q2_BASE, pScalers))
		{
			for(i = 0; i < 14; i++)
				pHistHodoscopeScalerMap->SetBinContent(9-HQuadrantMap[i].y, HQuadrantMap[i].x, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q3_BASE, pScalers))
		{
			for(i = 0; i < 14; i++)
				pHistHodoscopeScalerMap->SetBinContent(9-HQuadrantMap[i].x, 9-HQuadrantMap[i].y, pScalers[i]);
		}
		if(GetQuadrantScalers(DVCS_Q4_BASE, pScalers))
		{
			for(i = 0; i < 14; i++)
				pHistHodoscopeScalerMap->SetBinContent(HQuadrantMap[i].y, 9-HQuadrantMap[i].x, pScalers[i]);
		}

		if(!pClient->WriteVME16(DVCS_Q1_BASE + A_SCALERS, 1) ||	!pClient->WriteVME16(DVCS_Q2_BASE + A_SCALERS, 1) ||
		   !pClient->WriteVME16(DVCS_Q3_BASE + A_SCALERS, 1) || !pClient->WriteVME16(DVCS_Q4_BASE + A_SCALERS, 1) )
			return;

		pCanvasHodoscopeScalerMap->GetCanvas()->cd();
		pHistHodoscopeScalerMap->Draw("COLZTEXT");
		pCanvasHodoscopeScalerMap->GetCanvas()->Modified();
		pCanvasHodoscopeScalerMap->GetCanvas()->Update();
	}

	virtual Bool_t HandleTimer(TTimer *t)
	{
		if(pTimerUpdate->HasTimedOut() && (pSliderScalerSampleRate->GetPosition() != 60) )
			ScalerMapUpdate();

		return kTRUE;
	}

	Bool_t GetQuadrantScalers(unsigned int base, unsigned int *pScalers)
	{
		if(!pClient->BlkReadVME16(base + A_HSCALER_Q0H, (unsigned short *)pScalers, 28))
			return kFALSE;

		if(HTONL(1) != 1)
		{
			for(int i = 0; i < 14; i++)
				pScalers[i] = (pScalers[i]<<16) | (pScalers[i]>>16);
		}
		return kTRUE;
	}

};

#endif
