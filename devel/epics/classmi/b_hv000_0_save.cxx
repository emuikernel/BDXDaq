#include "b_hv000_0.hxx"
#include <stdio.h>

class B:HV000:0: public B:HV000:0Proxy
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
	B:HV000:0(char *domain): B:HV000:0Proxy(domain) { };
};

void B:HV000:0::SWITCH_ON()
{
//	User code for action SWITCH_ON
//	setOFF()/ setON()/ setERROR();
}

void B:HV000:0::SET_ON()
{
//	User code for action SET_ON
//	setOFF()/ setON()/ setERROR();
}

void B:HV000:0::SET_OFF()
{
//	User code for action SET_OFF
//	setOFF()/ setON()/ setERROR();
}

void B:HV000:0::SET_ERROR()
{
//	User code for action SET_ERROR
//	setOFF()/ setON()/ setERROR();
}

void B:HV000:0::SWITCH_OFF()
{
//	User code for action SWITCH_OFF
//	setOFF()/ setON()/ setERROR();
}

int main(int argc, char *argv[])
{
        BProxy b:hv000:0(argv[1], "b:hv000:0");
        BProxy b:hv000:1(argv[1], "b:hv000:1");
	b:hv000:0.setOFF();
	b:hv000:1.setOFF();

//	B:HV000:0 b:hv000:0(argv[1]);
//	b:hv000:0.setOFF();
	while(1)
	{
		pause();
	}
}

#ifdef HHH

#include "mf1.hxx"
#include <stdio.h>

class MF1: public MF1Proxy
{
protected :
// Actions
	virtual void SWITCH_ON();
	virtual void SWITCH_OFF();
public:
// Constructors
///	MF1(char *domain): MF1Proxy(domain) { };
  MF1(char *domain, char *name): MF1Proxy(domain, name) { };	
};

void MF1::SWITCH_ON()
{
//	User code for action SWITCH_ON
//	setOFF()/ setON()/ setERROR();
setON();
}

void MF1::SWITCH_OFF()
{
//	User code for action SWITCH_OFF
//	setOFF()/ setON()/ setERROR();
setOFF();
}

int main(int argc, char *argv[])
{

///	MF1 mf1(argv[1]);

        MF1 mf1(argv[1], "MF1");
	MF1 mf2(argv[1], "MF2");
	mf1.setOFF();
	mf2.setOFF();
	while(1)
	{
	  printf("pause=========\n");
		pause();
	}
}

#endif