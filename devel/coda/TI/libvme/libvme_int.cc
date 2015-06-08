

#include <iostream>
#include <iomanip>
using namespace std;

#include <linux/timex.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>


#include "libvme_int.h"

static bool vmeIntThreadReady;
static int vmeIntThreadStatus;
static bool vmeIntQuit;
static bool vmeIntDone;
static pthread_t vmeInt_pthread;
static VOIDFUNCPTR9 *vmeInt_routine;
static int vmeInt_arg;

void*  vmeIntThread(void *arg);

//----------------------
// vmeIntConnect
//----------------------
int vmeIntConnect(VOIDFUNCPTR9 routine, int arg)
{
	// Launch a thread to watch for interrupts.
	vmeInt_routine = routine;
	vmeInt_arg = arg;
	vmeIntThreadReady = false;
	vmeIntQuit = false;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if(getuid()==0)pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // Non-standard scheduling policy can only be used by root!
	pthread_create(&vmeInt_pthread, &attr,vmeIntThread, NULL);
	
	// Wait until thread is ready before returning control
	// to calling routine
	while(!vmeIntThreadReady)usleep(1000);
	
	return vmeIntThreadStatus;
}

//----------------------
// vmeIntThread
//----------------------
void*  vmeIntThread(void *arg)
{
	// Open VME interrupt device
	char *devname = "/dev/vme_int";
	int fd = open(devname, O_RDWR);
	if(fd<0){
		cerr<<"Error opening \""<<devname<<"\""<<endl;
		vmeIntThreadStatus = -1;
		vmeIntThreadReady = true;
		return NULL;
	}
	
	// Read an interrupt data packet. This will block
	// until it is available.
	int last_irq_count = 0;
	vmeIntThreadStatus = 0;
	vmeIntThreadReady = true;
	do{
		vme_int_data_t vme_int_data;
		int nread = read(fd, &vme_int_data, sizeof(vme_int_data));
		if(nread == sizeof(vme_int_data)){

			// Check if we missed an IRQ by making sure it is
			// sequential with the last one we got
			if(vme_int_data.irq_count - last_irq_count>1){
				cerr<<" Missed an IRQ! ("<<vme_int_data.irq_count<<" > "<<last_irq_count<<"+1)"<<endl;
			}
			last_irq_count = vme_int_data.irq_count;
			
			// OK, call the user supplied ISR
			(*vmeInt_routine)(vmeInt_arg);
			
		}else{
			// Must have timed out. Do nothing
		}
	}while(!vmeIntQuit);
	
	// Close VME interrupt device
	close(fd);
	
	vmeIntDone = true;
	return NULL;
}

//----------------------
// vmeIntDisconnect
//----------------------
int vmeIntDisconnect(void)
{
	// flag thread to exit after timeout
	vmeIntDone = false;
	vmeIntQuit = true;
	
	// Wait until thread is done before returning control
	// to calling routine
	for(int i=0; i<5500; i++){ // 5500= 5.5 seconds at 1000usec/iteration
		if(vmeIntDone)break;
		usleep(1000);
	}

	return 0;
}

