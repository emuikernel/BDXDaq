#ifndef ICTRIGMAP_H
#define ICTRIGMAP_H

#include "RootHeader.h"

#define IC_HISTVAL_X		1
#define IC_HISTVAL_C0		25
#define IC_HISTVAL_0		50
#define IC_HISTVAL_C1		75
#define IC_HISTVAL_1		100

class ICTrigMap : public TGCompositeFrame
{
public:
	ICTrigMap(const char *title, const TGWindow *p = 0, Bool_t edit = kTRUE, UInt_t w = 100, UInt_t h = 100);

	void TriggerMapUpdate();
	void SetICVal(int q, int index, int val);
	int GetICVal(int q, int index);

	unsigned short GetTowerTrigVal(int q, int index);
	unsigned short GetTowerIgnoreVal(int q, int index);
	unsigned short GetClusterTrigVal(int q, int index);
	unsigned short GetClusterIgnoreVal(int q, int index);
	unsigned short GetMClusterTrigVal(int q, int index);
	unsigned short GetMClusterIgnoreVal(int q, int index);
private:
	TRootEmbeddedCanvas	*pCanvas;
	TH2I				*pHistICTriggerMap;

	char TrigMatch_Q1[106];
	char TrigMatch_Q2[106];
	char TrigMatch_Q3[106];
	char TrigMatch_Q4[106];

	virtual Bool_t HandleButton(Event_t *evt);
};

typedef struct
{
	int x, y;
} Coord;

extern Coord ICQuadrantMap[106];
extern int QClusterMap[69];
extern int MClusterMap[37];

#endif
