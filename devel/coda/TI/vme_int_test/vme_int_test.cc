

#include <iostream>
#include <iomanip>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "libvme.h"

VMEBus *vmebus=NULL;

#define TIB_ADDR 0x0ED0

typedef struct {
	int vector;
	int irq;
	int irq_count;
	int read_count;
}vme_int_data_t;
vme_int_data_t vme_int_data;


int main(int narg, char* argv[])
{








	// Open a connection to the Universe chip and set it up to
	// communicate on the VME bus.
	int which_img = 1;
	char space = VME_A16 | VME_SIZE_D16;
	vmebus = new VMEBus(0x00000000, 0x0000FFFF, space, which_img);
	if(!vmebus){
		cerr<<"Error initializing VMEBus object."<<endl;
		return -1;
	}
	cout<<"Successfully opened VME."<<endl;

	// Open VME interrupt device
	char *devname = "/dev/vme_int";
	int fd = open(devname, O_RDWR);
	if(fd<0){
		cerr<<"Error opening \""<<devname<<"\""<<endl;
		return -1;
	}












	
	// Read an interrupt data packet. This will block
	// until it is available.
	int last_irq_count = 0;
	int Nreads = 0;
	time_t start_time = time(NULL);
	do{
		//cout<<"Reading ..."<<endl;
		int nread = read(fd, &vme_int_data, sizeof(vme_int_data));
		if(nread == sizeof(vme_int_data)){
		
			// "Acknowledge" interrupt
			vmebus->WriteWord(TIB_ADDR+6, 0x00F0);
			vmebus->WriteWord(TIB_ADDR+4, 0x8000);

			if(vme_int_data.irq_count - last_irq_count>1){
				cerr<<" Missed IRQ!"<<endl;
			}
			last_irq_count = vme_int_data.irq_count;
			Nreads++;
			
			//cout<<" read_count = "<<vme_int_data.read_count<<endl;
			//cout<<"  irq_count = "<<vme_int_data.irq_count<<endl;
			//cout<<"        irq = "<<vme_int_data.irq<<endl;
			//cout<<"     vector = "<<vme_int_data.vector<<endl;
		}else{
			cout<<" error reading from vme_int"<<endl;
		}
	}while(time(NULL) - start_time < 1000);
	
	float hz = (float)Nreads/10.0;
	cout<<"Read "<<Nreads<<" interrupts in 10 seconds ("<<hz<<" Hz)"<<endl;














	// Close VME interrupt device
	close(fd);
	
	return 0;
}




