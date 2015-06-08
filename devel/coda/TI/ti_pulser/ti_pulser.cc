
#include <iostream>
#include <iomanip>
using namespace std;

#include <time.h>
#include <unistd.h>

#include "libvme.h"

VMEBus *vmebus=NULL;

#define TIB_ADDR 0x0ED0

void PrintBinary(unsigned short val);

//---------------
// main
//---------------
int main(int narg, char* argv[])
{
	// Open a connection to the Universe chip and set it up to
	// communicate on the VME bus.
	int which_img = 0;
	char space = VME_A16 | VME_SIZE_D16;
	vmebus = new VMEBus(0x00000000, 0x0000FFFF, space, which_img);
	if(!vmebus){
		cerr<<"Error initializing VMEBus object."<<endl;
		return -1;
	}
	cout<<"Successfully opened VME."<<endl;

	unsigned int PCI_ID = vmebus->ReadUniReg(0x00);
	unsigned int LSI0_CTL = vmebus->ReadUniReg(0x100);
	unsigned int LSI0_BS = vmebus->ReadUniReg(0x104);
	unsigned int LSI0_BD = vmebus->ReadUniReg(0x108);
	unsigned int LSI0_TO = vmebus->ReadUniReg(0x10C);
	cout<<"PCI_ID = 0x"<<hex<<PCI_ID<<dec<<endl;
	cout<<"LSI0_CTL = 0x"<<hex<<LSI0_CTL<<dec<<endl;
	cout<<"LSI0_BS = 0x"<<hex<<LSI0_BS<<dec<<endl;
	cout<<"LSI0_BD = 0x"<<hex<<LSI0_BD<<dec<<endl;
	cout<<"LSI0_TO = 0x"<<hex<<LSI0_TO<<dec<<endl;
	
	
	cout<<"Resetting TIB ..."<<endl<<endl;
	vmebus->WriteWord(TIB_ADDR, 0x0080);
	
	cout<<"Enabling external interrupts"<<endl<<endl;
	vmebus->WriteWord(TIB_ADDR, 0x0007);
	
	cout<<"Starting writes to output register..."<<endl;
	time_t start_time = time(NULL);
	int Npulses = 0;
	do{
		vmebus->WriteWord(TIB_ADDR+6, 0x000F);
		vmebus->WriteWord(TIB_ADDR+6, 0x0000);
		
		///unsigned short csr = vmebus->ReadWord(TIB_ADDR);
		//cout<<" -- output bit 0 pulsed -- ";
		//PrintBinary(csr);
		//cout<<endl;
		
		// "Acknowledge" interrupt
		//vmebus->WriteWord(TIB_ADDR+4, 0x8000);
		Npulses++;
		usleep(1000);
		
	}while(time(NULL) - start_time < 2000);
	

	cout<<"Disabling interrupts"<<endl<<endl;
	vmebus->WriteWord(TIB_ADDR, 0x0);

	// Close Universe driver
	delete vmebus;
	cout<<"Closed VME."<<endl;

	float hz = (float)Npulses/20.0;
	cout<<"Read "<<Npulses<<" pulses sent in 20 seconds ("<<hz<<" Hz)"<<endl;

	return 0;
}

//-------------
// PrintBinary
//-------------
void PrintBinary(unsigned short val)
{
	for(int i=15; i>=0; i--){
		cout<<((val>>i)&0x01);
		if(i%8 == 0)cout<<" ";
	}
}

