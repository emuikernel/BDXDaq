
#define LINUXTI

/* bonusrol1.cc - UNIX first readout list (polling mode) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS

#include <iostream>
#include <iomanip>
using namespace std;

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>



#ifdef LINUXTI

#include "libvme.h"
VMEBus *vmebus=NULL;

#define TIRADR 0x0ED0
#define VME_VEC 0xec

typedef struct
{
  int vector;
  int irq;
  int irq_count;
  int read_count;
} vme_int_data_t;

vme_int_data_t vme_int_data;

static int last_irq_count;
static int Nreads;
static char *devname = "/dev/vme_int";
static int fd;

#endif



#endif




#include "circbuf.h" /* ttbosio.h inside */




void davetrig(unsigned long, unsigned long);
void davetrig_done();





/*****************************/
/* former 'crl' control keys */

/* readout list BONUSROL1 */
#define ROL_NAME__ "BONUSROL1"

/* polling */
#define POLLING_MODE


/* need in one place in trigger_dispatch.h; must be undefined in rol1 */
#undef EVENT_MODE


/*???*/
/* 0: External Mode   1: Trigger Supervisor Mode */
#define TS_MODE  0
#define IRQ_SOURCE  0x10


#define INIT_NAME bonusrol1__init

#include "rol.h"


/* test readout */
#include "TEST_source.h"

/************************/
/************************/

#include "coda.h"
#include "tt.h"

static char *rcname = "RC00";

long decrement;     /* local decrement counter */
extern long nevent; /* local event counter */
long mynev;


static void
__download()
{
  int poolsize;

#ifdef POLLING_MODE
  rol->poll = 1;
#endif



  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");






  printf("INFO: User Download 1 Executed\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source TEST */
  TEST_INIT;


  /* Register a sync trigger source (up to 32 sources) */
  /*CTRIGRSS(TEST, 1, davetrig, davetrig_done);*/ /* second arg=1 - what is that ? */
{
  trigRtns[trigId]  = (FUNCPTR1) (davetrig);
  doneRtns[trigId]  = (FUNCPTR3) (davetrig_done);
  syncTRtns[trigId] = (FUNCPTR2) TEST_TEST;
  Tcode[trigId]     = 1;
  ttypeRtns[trigId] = (FUNCPTR4) TEST_TTYPE;
  TEST_SYNC(code,trigId);
  trigId++;
}




  /* Register a physics event type (up to 16 types) */
  CRTTYPE(1, TEST, 1);
  CRTTYPE(2, TEST, 1);
  CRTTYPE(3, TEST, 1);
  CRTTYPE(4, TEST, 1);
  CRTTYPE(5, TEST, 1);
  CRTTYPE(6, TEST, 1);
  CRTTYPE(7, TEST, 1);
  CRTTYPE(8, TEST, 1);
  CRTTYPE(9, TEST, 1);

  rol->poll = 1;


  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


  printf("INFO: User Prestart 1 executed\n");

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}

static void
__pause()
{
  CDODISABLE(TEST,1,0);

  printf("INFO: User Pause 1 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  CDOENABLE(TEST,1,1);

#ifdef LINUXTI

  /* Open a connection to the Universe chip and set it up to
	 communicate on the VME bus */
  char space = VME_A16 | VME_SIZE_D16;
  int which_img = 1;
  vmebus = new VMEBus(0x00000000, 0x0000FFFF, space, which_img);
  if(!vmebus)
  {
	cerr<<"Error initializing VMEBus object."<<endl;
    return;
  }
  cout<<"Successfully opened VME."<<endl;

  // Open VME interrupt device
  if((fd = open(devname, O_RDWR)) < 0)
  {
	cerr<<"Error opening \""<<devname<<"\""<<endl;
	return;
  }

  last_irq_count = 0;
  Nreads = 0;


  /*
struct vme_tir {
  volatile unsigned short tir_csr;
  volatile unsigned short tir_vec;
  volatile unsigned short tir_dat;
  volatile unsigned short tir_oport;
  volatile unsigned short tir_iport;
} VME_TIR;
  tir[1]->tir_csr = 0x80; - reset 
  tir[1]->tir_vec = VME_VEC; - set interrupt vector 
  tir[1]->tir_csr = 0x7; - enable external triggers (0x3 - pooling) 
  tir[1]->tir_csr = 0x80; - disable triggers 
  tir[1]->tir_dat = 0x8000; - acknowledge trigger 
  */

  // TI board settings
  vmebus->WriteWord(TIRADR, 0x0080); // reset
  //vmebus->WriteWord(TIRADR+2, VME_VEC); // set interrupt vector

  vmebus->WriteWord(TIRADR, 0x0007); // enable external triggers

#endif

  mynev = 0;

  printf("INFO: User Go 1 Executed\n");

  return;
}

static void
__end()
{
  int ii, total_count, rem_count;

#ifdef LINUXTI

  vmebus->WriteWord(TIRADR+0, 0x80); // reset to disable triggers

  /* Close VME interrupt device */
  close(fd);

  // Close Universe driver
  delete vmebus;
  cout<<"Closed VME."<<endl;


#endif

  CDODISABLE(TEST,1,0);

  printf("INFO: User End 1 Executed\n");

  return;
}


void
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, len1, type, lock_key, *tmp;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len2;
  unsigned long *buf, *dabufp1, *dabufp2;
  int *jw, *secondword;
  DANODE *outEvent;


#ifdef LINUXTI

  // Read an interrupt data packet. This will block
  // until it is available.


  //vmebus->WriteWord(TIRADR, 0x0007); // enable external triggers

//cout<<"Acknowledging ..."<<endl;
vmebus->WriteWord(TIRADR+4, 0x8000); // "Acknowledge" interrupt

//cout<<"Reading .."<<endl;
  int nread = read(fd, &vme_int_data, sizeof(vme_int_data));
  //cout<<".. complete"<<endl;
  if(nread == sizeof(vme_int_data))
  {
    if(vme_int_data.irq_count - last_irq_count>1)
    {
      ;
      cerr<<" Missed IRQ!"<<endl;
    }
    last_irq_count = vme_int_data.irq_count;
    Nreads++;

    //cout<<" read_count = "<<vme_int_data.read_count<<endl;
    //cout<<"  irq_count = "<<vme_int_data.irq_count<<endl;
    //cout<<"        irq = "<<vme_int_data.irq<<endl;
    //cout<<"     vector = "<<vme_int_data.vector<<endl;
  }
  else
  {
    ;
    cout<<" error reading from vme_int"<<endl;
  }

#else

sleep(1);

#endif

  rol->dabufp = (long *) 0;
  CEOPEN(EVTYPE, BT_UI4);

  jw = (int *)(rol->dabufp);
  jw[-2] = 1;
  secondword = (int *)(rol->dabufp - 1);




    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;



    len = 4;
    for(ii=0; ii<len; ii++)
    {
      *rol->dabufp++ = ii;
    }


    if(bosMclose_(jw,ind,1,len) == 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }


  CECLOSE;

  return;
}


void
davetrig_done()
{
  return;
}  

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

#ifdef LINUXTI

  //cout<<"Acknowledging ..."<<endl;
  //vmebus->WriteWord(TIRADR+4, 0x8000); // "Acknowledge" interrupt

#endif

  return;
}
  
static void
__status()
{
  return;
}  
