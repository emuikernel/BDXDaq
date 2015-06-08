#include "boardclass.hxx"

class BOARDCLASS: public BOARDCLASSProxy
{
protected :
// Actions
	virtual void SWITCH_ON();
	virtual void SET_ON();
	virtual void SET_OFF();
	virtual void SET_ERROR();
	virtual void SWITCH_OFF();
public:
// Constructors
	BOARDCLASS(char *domain): BOARDCLASSProxy(domain) { };
};

void BOARDCLASS::SWITCH_ON()
{
//	User code for action SWITCH_ON
//	setOFF()/ setON()/ setERROR();
}

void BOARDCLASS::SET_ON()
{
//	User code for action SET_ON
//	setOFF()/ setON()/ setERROR();
}

void BOARDCLASS::SET_OFF()
{
//	User code for action SET_OFF
//	setOFF()/ setON()/ setERROR();
}

void BOARDCLASS::SET_ERROR()
{
//	User code for action SET_ERROR
//	setOFF()/ setON()/ setERROR();
}

void BOARDCLASS::SWITCH_OFF()
{
//	User code for action SWITCH_OFF
//	setOFF()/ setON()/ setERROR();
}

int main(int argc, char *argv[])
{

	BOARDCLASS boardclass(argv[1]);
//	boardclass.setOFF();
	while(1)
	{
		pause();
	}
}

