#ifndef HTRIGMAP_H
#define HTRIGMAP_H

#include "RootHeader.h"

#define H_HISTVAL_0		13
#define H_HISTVAL_1		25
#define H_HISTVAL_X		1

class HTrigMap : public TGCompositeFrame
{
public:
	HTrigMap(const char *title, const TGWindow *p = 0, Bool_t edit = kTRUE, UInt_t w = 100, UInt_t h = 100);

	void TriggerMapUpdate();
	void SetHVal(int q, int index, int val);
	unsigned short GetTowerTrigVal(int q, int index);
	unsigned short GetTowerIgnoreVal(int q, int index);
private:
	TRootEmbeddedCanvas	*pCanvas;
	TH2I				*pHistHTriggerMap;

	char TrigMatch_Q1[14];
	char TrigMatch_Q2[14];
	char TrigMatch_Q3[14];
	char TrigMatch_Q4[14];

	virtual Bool_t HandleButton(Event_t *evt);
};

typedef struct
{
	int x, y;
} HCoord;

extern HCoord HQuadrantMap[14];

#endif
