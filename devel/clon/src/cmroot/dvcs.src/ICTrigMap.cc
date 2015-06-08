#include "ICTrigMap.h"

Coord ICQuadrantMap[106] = {
	{13,23}, {14,23}, {15,23}, {16,23}, {17,23},
	{13,22}, {14,22}, {15,22}, {16,22}, {17,22}, {18,22}, 
	{13,21}, {14,21}, {15,21}, {16,21}, {17,21}, {18,21}, {19,21}, 
	{13,20}, {14,20}, {15,20}, {16,20}, {17,20}, {18,20}, {19,20}, {20,20},
	{13,19}, {14,19}, {15,19}, {16,19}, {17,19}, {18,19}, {19,19}, {20,19}, {21,19},
	{13,18}, {14,18}, {15,18}, {16,18}, {17,18}, {18,18}, {19,18}, {20,18}, {21,18}, {22,18},
	{13,17}, {14,17}, {15,17}, {16,17}, {17,17}, {18,17}, {19,17}, {20,17}, {21,17}, {22,17}, {23,17},
	{13,16}, {14,16}, {15,16}, {16,16}, {17,16}, {18,16}, {19,16}, {20,16}, {21,16}, {22,16}, {23,16},
	{13,15}, {14,15}, {15,15}, {16,15}, {17,15}, {18,15}, {19,15}, {20,15}, {21,15}, {22,15}, {23,15},
	         {14,14}, {15,14}, {16,14}, {17,14}, {18,14}, {19,14}, {20,14}, {21,14}, {22,14}, {23,14},
	                  {15,13}, {16,13}, {17,13}, {18,13}, {19,13}, {20,13}, {21,13}, {22,13}, {23,13},
	                  {15,12}, {16,12}, {17,12}, {18,12}, {19,12}, {20,12}, {21,12}, {22,12}, {23,12}
	};

int QClusterMap[69] = {
	 2, 3, 4,
	 7, 8, 9,10,
	13,14,15,16,17,
	20,21,22,23,24,25,
	28,29,30,31,32,33,34,
	37,38,39,40,41,42,43,44,
	47,48,49,50,51,52,53,54,55,
	58,59,60,61,62,63,64,65,66,
	69,70,71,72,73,74,75,76,77,
	79,80,81,82,83,84,85,86,87
	};

int MClusterMap[37] = {
	 0, 1,
	 5, 6,
	11,12,
	18,19,
	26,27,
	35,36,
	45,46,
	56,57,
	67,68
	  ,78,
	      88,89,90,91,92,93,94,95,96,
		  97,98,99,100,101,102,103,104,105
	};

ICTrigMap::ICTrigMap(const char *title, const TGWindow *p, Bool_t edit, UInt_t w, UInt_t h)
	: TGCompositeFrame(p, w, h)
{
	pCanvas = new TRootEmbeddedCanvas(NULL, this, w, h);
	pCanvas->GetCanvas()->SetBorderMode(0);
	AddFrame(pCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	pCanvas->GetCanvas()->cd();
	pHistICTriggerMap = new TH2I(NULL, title, 23, 1, 24, 23, 1, 24);
	pHistICTriggerMap->SetStats(kFALSE);
	pHistICTriggerMap->GetXaxis()->CenterLabels();
	pHistICTriggerMap->GetXaxis()->SetNdivisions(23, kFALSE);
	pHistICTriggerMap->GetYaxis()->CenterLabels();
	pHistICTriggerMap->GetYaxis()->SetNdivisions(23, kFALSE);
	pHistICTriggerMap->GetXaxis()->SetTickLength(1);
	pHistICTriggerMap->GetYaxis()->SetTickLength(1);
	pCanvas->GetCanvas()->SetEditable(kFALSE);

	if(edit)
	{
		gVirtualX->GrabButton(fId, kAnyButton, kAnyModifier, kButtonPressMask, kNone, kNone);
		memset(TrigMatch_Q1, IC_HISTVAL_X, sizeof(TrigMatch_Q1));
		memset(TrigMatch_Q2, IC_HISTVAL_X, sizeof(TrigMatch_Q2));
		memset(TrigMatch_Q3, IC_HISTVAL_X, sizeof(TrigMatch_Q3));
		memset(TrigMatch_Q4, IC_HISTVAL_X, sizeof(TrigMatch_Q4));
	}
	else
	{
		memset(TrigMatch_Q1, IC_HISTVAL_0, sizeof(TrigMatch_Q1));
		memset(TrigMatch_Q2, IC_HISTVAL_0, sizeof(TrigMatch_Q2));
		memset(TrigMatch_Q3, IC_HISTVAL_0, sizeof(TrigMatch_Q3));
		memset(TrigMatch_Q4, IC_HISTVAL_0, sizeof(TrigMatch_Q4));
	}

	MapSubwindows();
	Resize();
	MapWindow();

	TriggerMapUpdate();
}

Bool_t ICTrigMap::HandleButton(Event_t *evt)
{
	if(evt->fCode == kButton1)
	{
		int w = pCanvas->GetWidth(), h = pCanvas->GetHeight();
		int minX = w/10, maxX = w*9/10;
		int minY = h/10, maxY = h*9/10;
		if( (evt->fX > minX) && (evt->fX < maxX) && (evt->fY > minY) && (evt->fY < maxY) )
		{
			int nx = 1+23 * (evt->fX - minX) / (maxX - minX);
			int ny = 23-23 * (evt->fY - minY) / (maxY - minY);

			for(int i = 0; i < 106; i++)
			{
				char *p = NULL;
				if( (ICQuadrantMap[i].x == nx) && (ICQuadrantMap[i].y == ny) )
					p = &TrigMatch_Q1[i];
				if( (24-ICQuadrantMap[i].y == nx) && (ICQuadrantMap[i].x == ny) )
					p = &TrigMatch_Q2[i];
				if( (24-ICQuadrantMap[i].x == nx) && (24-ICQuadrantMap[i].y == ny) )
					p = &TrigMatch_Q3[i];
				if( (ICQuadrantMap[i].y == nx) && (24-ICQuadrantMap[i].x == ny) )
					p = &TrigMatch_Q4[i];
				if(p)
				{
					if(*p == IC_HISTVAL_X)
						*p = IC_HISTVAL_0;
					else if(*p == IC_HISTVAL_0)
						*p = IC_HISTVAL_1;
					else if(*p == IC_HISTVAL_1)
						*p = IC_HISTVAL_C0;
					else if(*p == IC_HISTVAL_C0)
						*p = IC_HISTVAL_C1;
					else
						*p = IC_HISTVAL_X;
					TriggerMapUpdate();
					break;
				}
			}
		}
	}
	return kTRUE;
}

void ICTrigMap::TriggerMapUpdate()
{
	int i, j;

	for(i = 0; i < 23; i++)
	for(j = 0; j < 23; j++)
	{
		if( (i==12) && (j==12) )
			pHistICTriggerMap->SetBinContent(i, j, IC_HISTVAL_1);
		else
			pHistICTriggerMap->SetBinContent(i, j, 0.0);
	}

	for(i = 0; i < 106; i++)
	{
		if( ((ICQuadrantMap[i].x ^ ICQuadrantMap[i].y) & 0x1) && (TrigMatch_Q1[i] == IC_HISTVAL_X) )
			pHistICTriggerMap->SetBinContent(ICQuadrantMap[i].x, ICQuadrantMap[i].y, TrigMatch_Q1[i]+5);
		else
			pHistICTriggerMap->SetBinContent(ICQuadrantMap[i].x, ICQuadrantMap[i].y, TrigMatch_Q1[i]);

		if( ((ICQuadrantMap[i].x ^ ICQuadrantMap[i].y) & 0x1) && (TrigMatch_Q2[i] == IC_HISTVAL_X) )
			pHistICTriggerMap->SetBinContent(24-ICQuadrantMap[i].y, ICQuadrantMap[i].x, TrigMatch_Q2[i]+5);
		else
			pHistICTriggerMap->SetBinContent(24-ICQuadrantMap[i].y, ICQuadrantMap[i].x, TrigMatch_Q2[i]);

		if( ((ICQuadrantMap[i].x ^ ICQuadrantMap[i].y) & 0x1) && (TrigMatch_Q3[i] == IC_HISTVAL_X) )
			pHistICTriggerMap->SetBinContent(24-ICQuadrantMap[i].x, 24-ICQuadrantMap[i].y, TrigMatch_Q3[i]+5);
		else
			pHistICTriggerMap->SetBinContent(24-ICQuadrantMap[i].x, 24-ICQuadrantMap[i].y, TrigMatch_Q3[i]);

		if( ((ICQuadrantMap[i].x ^ ICQuadrantMap[i].y) & 0x1) && (TrigMatch_Q4[i] == IC_HISTVAL_X) )
			pHistICTriggerMap->SetBinContent(ICQuadrantMap[i].y, 24-ICQuadrantMap[i].x, TrigMatch_Q4[i]+5);
		else
			pHistICTriggerMap->SetBinContent(ICQuadrantMap[i].y, 24-ICQuadrantMap[i].x, TrigMatch_Q4[i]);
	}
	pCanvas->GetCanvas()->cd();
	pCanvas->GetCanvas()->SetEditable(kTRUE);
	pHistICTriggerMap->Draw("COL");
	pCanvas->GetCanvas()->SetEditable(kFALSE);
	pCanvas->GetCanvas()->Modified();
	pCanvas->GetCanvas()->Update();
}

void ICTrigMap::SetICVal(int q, int index, int val)
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

unsigned short ICTrigMap::GetTowerTrigVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 106)
			break;

		char val;
		if(q == 0)
			val = TrigMatch_Q1[i];
		else if(q == 1)
			val = TrigMatch_Q2[i];
		else if(q == 2)
			val = TrigMatch_Q3[i];
		else if(q == 3)
			val = TrigMatch_Q4[i];
		if( (val == IC_HISTVAL_1) || (val == IC_HISTVAL_C1) )
			result |= mask;
		mask <<= 1;
	}
	return result;
}

unsigned short ICTrigMap::GetTowerIgnoreVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 106)
			break;

		char val = -1;
		if(q == 0)
			val = TrigMatch_Q1[i];
		else if(q == 1)
			val = TrigMatch_Q2[i];
		else if(q == 2)
			val = TrigMatch_Q3[i];
		else if(q == 3)
			val = TrigMatch_Q4[i];
		if(val == IC_HISTVAL_X)
			result |= mask;
		mask <<= 1;
	}
	return result;
}

unsigned short ICTrigMap::GetClusterTrigVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 69)
			break;

		char val;
		if(q == 0)
			val = TrigMatch_Q1[QClusterMap[i]];
		else if(q == 1)
			val = TrigMatch_Q2[QClusterMap[i]];
		else if(q == 2)
			val = TrigMatch_Q3[QClusterMap[i]];
		else if(q == 3)
			val = TrigMatch_Q4[QClusterMap[i]];
		if( (val == IC_HISTVAL_C0) || (val == IC_HISTVAL_C1) )
			result |= mask;
		mask <<= 1;
	}
	return result;
}

unsigned short ICTrigMap::GetClusterIgnoreVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(unsigned int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 69)
			break;

		char val;
		if(q == 0)
			val = TrigMatch_Q1[QClusterMap[i]];
		else if(q == 1)
			val = TrigMatch_Q2[QClusterMap[i]];
		else if(q == 2)
			val = TrigMatch_Q3[QClusterMap[i]];
		else if(q == 3)
			val = TrigMatch_Q4[QClusterMap[i]];
		if( (val != IC_HISTVAL_C0) && (val != IC_HISTVAL_C1) )
			result |= mask;
		mask <<= 1;
	}
	return result;
}

unsigned short ICTrigMap::GetMClusterTrigVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 37)
			break;

		char val;
		if(q == 0)
			val = TrigMatch_Q1[MClusterMap[i]];
		else if(q == 1)
			val = TrigMatch_Q2[MClusterMap[i]];
		else if(q == 2)
			val = TrigMatch_Q3[MClusterMap[i]];
		else if(q == 3)
			val = TrigMatch_Q4[MClusterMap[i]];
		if( (val == IC_HISTVAL_C0) || (val == IC_HISTVAL_C1) )
			result |= mask;
		mask <<= 1;

	}
	return result;
}

unsigned short ICTrigMap::GetMClusterIgnoreVal(int q, int index)
{
	unsigned int result = 0;
	unsigned int mask = 0x1;
	for(int i = index*16; i < (index*16+16); i++)
	{
		if(i >= 37)
			break;

		char val;
		if(q == 0)
			val = TrigMatch_Q1[MClusterMap[i]];
		else if(q == 1)
			val = TrigMatch_Q2[MClusterMap[i]];
		else if(q == 2)
			val = TrigMatch_Q3[MClusterMap[i]];
		else if(q == 3)
			val = TrigMatch_Q4[MClusterMap[i]];
		if( (val != IC_HISTVAL_C0) && (val != IC_HISTVAL_C1) )
			result |= mask;
		mask <<= 1;

	}
	return result;
}

int ICTrigMap::GetICVal(int q, int index)
{
	if(q == 0)
		return TrigMatch_Q1[index];
	else if(q == 1)
		return TrigMatch_Q2[index];
	else if(q == 2)
		return TrigMatch_Q3[index];
	else if(q == 3)
		return TrigMatch_Q4[index];

	return 0;
}
