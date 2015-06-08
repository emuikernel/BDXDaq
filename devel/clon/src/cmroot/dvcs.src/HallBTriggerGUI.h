#ifndef HallBTriggerGUI_H
#define HallBTriggerGUI_H

#include "RootHeader.h"
#include "VMERemote.h"
#include "ICScalers.h"
#include "HScalers.h"
#include "DVCSControl.h"
#include "DVCSScope.h"

class HallBTriggerGUI : public TGMainFrame
{
private:
	void HandleDisconnect();

	virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
	virtual void CloseWindow();

	VMERemote			*pVMERemoteDVCSTrigger;
	TGTab				*pTabMain;
	DVCSControl			*pTabDVCSControl;
	ICScalers			*pTabICScalers;
	DVCSScope			*pTabDVCSScope;
	HScalers			*pTabHScalers;

public:
	HallBTriggerGUI(const TGWindow *p, unsigned int w, unsigned int h, const char *host);
};

#endif
