#include "HTrigMap.h"

HCoord HQuadrantMap[14] = {
	{5,8}, {6,8}, {7,8},
	{5,7}, {6,7}, {7,7}, {8,7},
	{5,6}, {6,6}, {7,6}, {8,6},
	       {6,5}, {7,5}, {8,5}
	};
/*
HCoord HodoscopeMap[56] = {
	       {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1},
	{2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2}, {9,2},
	{2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3}, {8,3}, {9,3},
	{2,4}, {3,4}, {4,4},               {7,4}, {8,4}, {9,4},
	{2,5}, {3,5}, {4,5},               {7,5}, {8,5}, {9,5},
	{2,6}, {3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6}, {9,6},
	{2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {8,7}, {9,7},
	       {3,8}, {4,8}, {5,8}, {6,8}, {7,8}, {8,8}
	};
*/

HTrigMap::HTrigMap(const char *title, const TGWindow *p, Bool_t edit, UInt_t w, UInt_t h)
	: TGCompositeFrame(p, w, h)
{
	pCanvas = new TRootEmbeddedCanvas(NULL, this, w, h);
	pCanvas->GetCanvas()->SetBorderMode(0);
	AddFrame(pCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	pHistHTriggerMap = new TH2I(NULL, title, 8, 1, 9, 8, 1, 9);
	pHistHTriggerMap->SetStats(kFALSE);
	pHistHTriggerMap->GetXaxis()->CenterLabels();
	pHistHTriggerMap->GetXaxis()->SetNdivisions(8, kFALSE);
	pHistHTriggerMap->GetYaxis()->CenterLabels();
	pHistHTriggerMap->GetYaxis()->SetNdivisions(8, kFALSE);
	pHistHTriggerMap->GetXaxis()->SetTickLength(1);
	pHistHTriggerMap->GetYaxis()->SetTickLength(1);
	pCanvas->GetCanvas()->SetEditable(kFALSE);

	if(edit)
	{
		gVirtualX->GrabButton(fId, kButton1, kAnyModifier, kButtonPressMask, kNone, kNone);
		memset(TrigMatch_Q1, H_HISTVAL_X, sizeof(TrigMatch_Q1));
		memset(TrigMatch_Q2, H_HISTVAL_X, sizeof(TrigMatch_Q2));
		memset(TrigMatch_Q3, H_HISTVAL_X, sizeof(TrigMatch_Q3));
		memset(TrigMatch_Q4, H_HISTVAL_X, sizeof(TrigMatch_Q4));
	}
	else
	{
		memset(TrigMatch_Q1, H_HISTVAL_0, sizeof(TrigMatch_Q1));
		memset(TrigMatch_Q2, H_HISTVAL_0, sizeof(TrigMatch_Q2));
		memset(TrigMatch_Q3, H_HISTVAL_0, sizeof(TrigMatch_Q3));
		memset(TrigMatch_Q4, H_HISTVAL_0, sizeof(TrigMatch_Q4));
	}

	MapSubwindows();
	Resize();
	MapWindow();

	TriggerMapUpdate();
}

Bool_t HTrigMap::HandleButton(Event_t *evt)
{
	if(evt->fCode == kButton1)
	{
		int w = pCanvas->GetWidth(), h = pCanvas->GetHeight();
		int minX = w/10, maxX = w*9/10;
		int minY = h/10, maxY = h*9/10;
		if( (evt->fX > minX) && (evt->fX < maxX) && (evt->fY > minY) && (evt->fY < maxY) )
		{
			int nx = 1+8 * (evt->fX - minX) / (maxX - minX);
			int ny = 8-8 * (evt->fY - minY) / (maxY - minY);

			for(int i = 0; i < 14; i++)
			{
				char *p = NULL;
				if( (HQuadrantMap[i].x == nx) && (HQuadrantMap[i].y == ny) )
					p = &TrigMatch_Q1[i];
				if( (9-HQuadrantMap[i].y == nx) && (HQuadrantMap[i].x == ny) )
					p = &TrigMatch_Q2[i];
				if( (9-HQuadrantMap[i].x == nx) && (9-HQuadrantMap[i].y == ny) )
					p = &TrigMatch_Q3[i];
				if( (HQuadrantMap[i].y == nx) && (9-HQuadrantMap[i].x == ny) )
					p = &TrigMatch_Q4[i];
				if(p)
				{
					if(*p == H_HISTVAL_X)
						*p = H_HISTVAL_0;
					else if(*p == H_HISTVAL_0)
						*p = H_HISTVAL_1;
					else
						*p = H_HISTVAL_X;
					TriggerMapUpdate();
					break;
				}
			}
		}
	}
	return kTRUE;
}

void HTrigMap::TriggerMapUpdate()
{
	int i, j;

	for(i = 0; i < 8; i++)
	for(j = 0; j < 8; j++)
	{
		if( (i>=4) && (i<=5) && (j>=4) && (j<=5) )
			pHistHTriggerMap->SetBinContent(i, j, H_HISTVAL_1+1);
		else
			pHistHTriggerMap->SetBinContent(i, j, 0.0);
	}

	for(i = 0; i < 14; i++)
	{
		if((HQuadrantMap[i].x ^ HQuadrantMap[i].y) & 0x1)
		{
			pHistHTriggerMap->SetBinContent(HQuadrantMap[i].x, HQuadrantMap[i].y, TrigMatch_Q1[i]);
			pHistHTriggerMap->SetBinContent(9-HQuadrantMap[i].y, HQuadrantMap[i].x, TrigMatch_Q2[i]+1);
			pHistHTriggerMap->SetBinContent(9-HQuadrantMap[i].x, 9-HQuadrantMap[i].y, TrigMatch_Q3[i]);
			pHistHTriggerMap->SetBinContent(HQuadrantMap[i].y, 9-HQuadrantMap[i].x, TrigMatch_Q4[i]+1);
		}
		else
		{
			pHistHTriggerMap->SetBinContent(HQuadrantMap[i].x, HQuadrantMap[i].y, TrigMatch_Q1[i]+1);
			pHistHTriggerMap->SetBinContent(9-HQuadrantMap[i].y, HQuadrantMap[i].x, TrigMatch_Q2[i]);
			pHistHTriggerMap->SetBinContent(9-HQuadrantMap[i].x, 9-HQuadrantMap[i].y, TrigMatch_Q3[i]+1);
			pHistHTriggerMap->SetBinContent(HQuadrantMap[i].y, 9-HQuadrantMap[i].x, TrigMatch_Q4[i]);
		}
	}
	pCanvas->GetCanvas()->cd();
	pCanvas->GetCanvas()->SetEditable(kTRUE);
	pHistHTriggerMap->Draw("COL");
	pCanvas->GetCanvas()->SetEditable(kFALSE);
	pCanvas->GetCanvas()->Modified();
	pCanvas->GetCanvas()->Update();
}

void HTrigMap::SetHVal(int q, int index, int val)
{
	if(q == 0)
		TrigMatch_Q1[index] = val;
	else if(q == 1)
		TrigMatch_Q2[index] = val;
	else if(q == 2)
		TrigMatch_Q3[index] = val;
	else if(q == 3)
		TrigMatch_Q4[index] = val;
}

unsigned short HTrigMap::GetTowerTrigVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = 0; i < 14; i++)
	{
		char val;
		if(q == 0)
			val = TrigMatch_Q1[i];
		else if(q == 1)
			val = TrigMatch_Q2[i];
		else if(q == 2)
			val = TrigMatch_Q3[i];
		else if(q == 3)
			val = TrigMatch_Q4[i];
		if(val == H_HISTVAL_1)
			result |= mask;
		mask <<= 1;
	}
	return result;
}

unsigned short HTrigMap::GetTowerIgnoreVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = 0; i < 14; i++)
	{
		char val;
		if(q == 0)
			val = TrigMatch_Q1[i];
		else if(q == 1)
			val = TrigMatch_Q2[i];
		else if(q == 2)
			val = TrigMatch_Q3[i];
		else if(q == 3)
			val = TrigMatch_Q4[i];
		if(val == H_HISTVAL_X)
			result |= mask;
		mask <<= 1;
	}
	return result;
}
