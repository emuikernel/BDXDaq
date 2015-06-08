#include <smirtl.hxx>

class BProxy: public SmiProxy
{
protected :
// Actions
	virtual void SWITCH_ON() {};
	virtual void SET_ON() {};
	virtual void SET_OFF() {};
	virtual void SET_ERROR() {};
	virtual void SWITCH_OFF() {};
// Command Handler
	void smiCommandHandler();
public:
// States
	void setOFF() {setState("OFF");};
	void setON() {setState("ON");};
	void setERROR() {setState("ERROR");};
// Object Parameters
// Constructors
//	B:HV000:0Proxy(char *domain): SmiProxy("B:HV000:0") { attach(domain);};
        BProxy(char *domain, char *name): SmiProxy() { attach(domain, name);};
};

void BProxy::smiCommandHandler()
{
	if(testAction("SWITCH_ON"))
		SWITCH_ON();
	else if(testAction("SET_ON"))
		SET_ON();
	else if(testAction("SET_OFF"))
		SET_OFF();
	else if(testAction("SET_ERROR"))
		SET_ERROR();
	else if(testAction("SWITCH_OFF"))
		SWITCH_OFF();
}
