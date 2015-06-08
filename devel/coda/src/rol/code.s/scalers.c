/*
 *  scalers
 *
 *  code for hardware scaler readout, reset, level2 programming, etc.
 *
 *  still to do:
 *     table driven scaler readout
 *
 *  Use 0 for local scaler address to generate 16 placeholder words, value -1
 *  Offset calculation works for A24 only!
 *
 *  Wolin, 18-sep-98
 *  Gyurjyan, 08/10/99
 *
 *
 *  Thanks for the example codes provided by Dave Abbott and Ed Jastrzembski
 *
 *  Added new deffinitions for the partial l1/l2 correlations: Gyurjyan 25-Feb-2004
 * More modifications: Gyurjyan 05-Mar-2004
 */

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vxWorks.h> 

/* external prototypes */
int sysBusToLocalAdrs();


/*  constants for struck scalers (warning...just set up for channels 0-7) */
#define MASK             0xffffff00
#define NCHAN            8
#define FIFO_FULL        0x00001000
#define FIFO_EMPTY       0x00000100
#define ENABLE_EXT_NEXT  0x00010000
#define DISABLE_EXT_NEXT 0x01000000
#define ENABLE_TEST      0x00000020
#define DISABLE_TEST     0x00002000
#define ENABLE_25MHZ     0x00000010
#define DISABLE_25MHZ    0x00001000


/* global PPC board offset */
static unsigned long ppc_offset;


/* addresses */
#define FLEX_ADDR      0x0ED0


/* possible level1/level2 sector based combinations */
#define MAX_OR_COMBS 100
#define MAX_AND_COMBS 100


/* local addresses of CAEN scaler modules in scaler1  */
unsigned long trscal[]={0x030000,0x040000,0x050000,0x060000,0x070000,0x080000};
unsigned long ntrscal=sizeof(trscal)/sizeof(unsigned long);


/* local addresses of CAEN scaler modules in scaler2 */
unsigned long ecscal[]={0x020000,0x030000,0x040000,0x050000,0x060000,0x070000};
unsigned long necscal=sizeof(ecscal)/sizeof(unsigned long);

unsigned long scscal[]={0x0a0000,0x0b0000,0x0c0000,0x0d0000,0x0e0000,0x0f0000,
			0x100000,0x110000,0x120000,0x130000,0x140000,0x150000};
unsigned long nscscal=sizeof(scscal)/sizeof(unsigned long);


/* local addresses of CAEN,STRUCK scaler modules in scaler3 */
unsigned long stscal[]={0x050000,0x060000};
unsigned long ccscal[]={0x600000,0x700000,0x800000};
unsigned long nstscal=sizeof(stscal)/sizeof(unsigned long);
unsigned long nccscal=sizeof(ccscal)/sizeof(unsigned long);


/* local addresses of CAEN scaler modules in scaler4 */
unsigned long tgscal[]={0x020000,0x030000,0x040000,0x050000,0x060000,0x070000,
			0x080000,0x090000,0x0a0000,0x0b0000,0x0c0000,0x0d0000,
			0x0e0000};
unsigned long ntgscal=sizeof(tgscal)/sizeof(unsigned long);


/* local address of STRUCK scaler in scalerp */
unsigned long helscal[]={0x303000};
unsigned long nhelscal=sizeof(helscal)/sizeof(unsigned long);


/*---------------------------------------------------------------*/


/* resets CAEN scaler module at full address fulladdr */
void
reset_caen_scaler(unsigned long fulladdr)
{  
  volatile short *ptr;
  
  if((fulladdr&0xffffff)==0)return;
  
  ptr=(short *)(fulladdr+0x50);
  *ptr=0;
  ptr=(short *)(fulladdr+0x54);
  *ptr=0;
  
}  


/*--------------------------------------------------------------------*/


/* copies caen scaler module data into buffer */
/* if 24 bits of addr=0, just inserts 16 zeros */
void
read_caen_scaler(unsigned long fulladdr, unsigned long *buf)
{  
  int i;
  volatile unsigned long *p;
  
  
  if((fulladdr&0xffffff)!=0) {
    p = (unsigned long *)(fulladdr+0x10);
    for(i=0; i<16; i++) *buf++=*p++;
  } else {
    for(i=0; i<16; i++) *buf++=(unsigned long) 0;
  }
  
  return;
}


/*---------------------------------------------------------------*/


int
str7201status(int addr)
{  
  volatile unsigned int *bufptr;
  bufptr = (unsigned int *) addr;
  
  return(*bufptr);
}


/*--------------------------------------------------------------------*/


void
str7201control(int addr, int value)
{  
  volatile unsigned int *bufptr;
  bufptr = (unsigned int *) addr;
  
  *bufptr = value;
}


/*--------------------------------------------------------------------*/


void
str7201mask(int addr, int value)
{  
  volatile unsigned int *bufptr;
  bufptr = (unsigned int *) (addr + 0xc);
  
  *bufptr = value;
}


/*--------------------------------------------------------------------*/


void str7201reset(int addr) {

  volatile unsigned int *bufptr;

  if(addr==0)return;

  bufptr = (unsigned int *) (addr + 0x60);
  *bufptr = 0x0;                            /* arbitrary data ! */
}


/*--------------------------------------------------------------------*/


int str7201readfifo(int addr) {

  volatile unsigned int *bufptr;
  bufptr = (unsigned int *) ((addr & 0xefffffff) + 0x100); /* 0xf.. -> 0xe.. */

  return(*bufptr);
}


/*--------------------------------------------------------------------*/


void str7201enablenextlogic(int addr)
{
  volatile unsigned int *bufptr;
  bufptr = (unsigned int *) (addr + 0x28);

  *bufptr = 0x0; /* arbitrary data ! */
}


/*--------------------------------------------------------------------*/


int str7201read(int addr, long *value) {

  volatile long *outbuf;
  long len;

  outbuf = value; /* output buffer have to be allocated in calling function */
  len = *value;
  while( !(str7201status(addr) & FIFO_EMPTY))
  {
    *outbuf++ = str7201readfifo(addr);
    len--;
    if(len == 0) break;
  }

  return((int)(outbuf - value));
}


/*--------------------------------------------------------------------*/


int read_str7201_scaler(unsigned long addr, unsigned long *buf, int nfree) {

  if(addr!=0) {

    if(str7201status(addr) & FIFO_FULL) {       /* FIFO full - clean it up */
	str7201reset(addr);                     /* FIFO must be clear after FULL condition !!! */
	str7201mask(addr, MASK);                /* disable all channels except 0-7 */
	str7201enablenextlogic(addr);
	str7201control(addr, ENABLE_EXT_NEXT);
	/*	str7201ledon(addr);   turns on led */
	return 0;

    } else {                                       /* FIFO not full - read it out */
      *buf = ((nfree-8)/NCHAN)*NCHAN;              /* required number of words */
      return str7201read(addr,buf);
    }

  } else {
    return 0;
  }
}


/*---------------------------------------------------------------*/


/* reads polar scaler crate */
void read_scalerp() {

  int k;

  /* struck scaler */
  printf("\n\n*HLS*\n");
  for(k=0; k<nhelscal; k++) {
    /*  need something here ??? */
  }

  return;
}


/*---------------------------------------------------------------*/


/* reads scaler crate 1 */
void read_scaler1() {

  int i,j,k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* TRGS scalers */
  printf("\n\n*TRGS*\n");
  for(k=0; k<ntrscal; k++){
    p=(unsigned long*)(offset+trscal[k]+0x10);
    for(j=0; j<2; j++){
      for(i=0; i<8; i++){
	if(trscal[k]!=0) {
	  printf("%11lu ",*(p+j*8+i));
	} else {
	  printf("%11lu ",(unsigned long) 0);
	}
      }
      printf("\n");
    }
  }
  printf("*EOD*\n");

  return;
}


/*---------------------------------------------------------------*/


/* reads trgs scalers */
void read_trgs() {
  
  int i,k;
  volatile unsigned long *p;
  unsigned long offset;
  
  sysBusToLocalAdrs(0x39,0,&offset);
  
  
  /* TRGS scalers */
  for(k=0; k<ntrscal; k++){
    p=(unsigned long*)(offset+trscal[k]+0x10);
    for(i=0; i<16; i++){
      if(trscal[k]!=0) {
	printf("%11lu ",*(p+i));
      } else {
	printf("%11lu ",(unsigned long) 0);
      }
    }
  }
  printf("\n");
  return;
}

/*---------------------------------------------------------------*/


/* reads scaler crate 2 */
void read_scaler2() {

  int i,j,k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* EC scalers */
  printf("\n\n*EC*\n");
  for(k=0; k<necscal; k++){
    p=(unsigned long*)(offset+ecscal[k]+0x10);
    for(j=0; j<2; j++){
      for(i=0; i<8; i++){
	if(ecscal[k]!=0) {
	  printf("%11lu ",*(p+j*8+i));
	} else {
	  printf("%11lu ",(unsigned long) 0);
	}
      }
      printf("\n");
    }
  }
  printf("*EOD*\n");

  
  /* SC scalers */
  printf("\n\n*SC*\n");
  for(k=0; k<nscscal; k++){
    p=(unsigned long*)(offset+scscal[k]+0x10);
    for(j=0; j<2; j++){
      for(i=0; i<8; i++){
	if(scscal[k]!=0) {
	  printf("%11lu ",*(p+j*8+i));
	} else {
	  printf("%11lu ",(unsigned long) 0);
	}
      }
      printf("\n");
    }
  }
  printf("*EOD*\n");
  
  return;
}


/*---------------------------------------------------------------*/


/* reads scaler crate 3 (just ST so far...)  */
void read_scaler3() {

  int i,j,k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* st scalers */
  printf("\n\n*ST*\n");
  for(k=0; k<nstscal; k++)
  {
    p=(unsigned long*)(offset+stscal[k]+0x10);
    for(j=0; j<2; j++)
    {
      for(i=0; i<8; i++)
      {
	    if(stscal[k] != 0)
        {
	      printf("%11lu ",*(p+j*8+i));
	    }
        else
        {
	      printf("%11lu ",(unsigned long) 0);
	    }
      }
      printf("\n");
    }
  }
  printf("*EOD*\n");
  
  return;
}


/*---------------------------------------------------------------*/


/* reads scaler crate 4 */
void read_scaler4() {

  int i,j,k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* tagger scalers */
  printf("\n\n*TG*\n");
  for(k=0; k<ntgscal; k++){
    p=(unsigned long*)(offset+tgscal[k]+0x10);
    for(j=0; j<2; j++){
      for(i=0; i<8; i++){
	if(tgscal[k]!=0) {
	  printf("%11lu ",*(p+j*8+i));
	} else {
	  printf("%11lu ",(unsigned long) 0);
	}
      }
      printf("\n");
    }
  }
  printf("*EOD*\n");
  
  return;
}


/*---------------------------------------------------------------*/


void reset_scalerp() {

  int k;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* reset helicity scalers */
  for(k=0; k<nhelscal; k++) {
    str7201control(offset+helscal[k], DISABLE_EXT_NEXT);
    str7201reset(offset+helscal[k]);
                                             /* for ECL version ALL open inputs should be disabled !!! */
    str7201mask(offset+helscal[k], MASK);    /* disable all channels except 0-7 */
    str7201enablenextlogic(offset+helscal[k]);
    /*  str7201nextclock(offset+helscal[k]);   for switching buffers  */
    str7201control(offset+helscal[k], ENABLE_EXT_NEXT);
  }

  return;
}


/*---------------------------------------------------------------*/


void reset_scaler1() {

  int k;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* reset trigger scalers */
  for(k=0; k<ntrscal; k++) reset_caen_scaler(offset+trscal[k]);

  return;
}


/*---------------------------------------------------------------*/


void reset_scaler2() {

  int k;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* reset ec scalers */
  for(k=0; k<necscal; k++) reset_caen_scaler(offset+ecscal[k]);

  /* reset sc scalers */
  for(k=0; k<nscscal; k++) reset_caen_scaler(offset+scscal[k]);

  return;

}


/*---------------------------------------------------------------*/


void reset_scaler3() {

  int k;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* reset st scalers */
  for(k=0; k<nstscal; k++) reset_caen_scaler(offset+stscal[k]);

  return;

}


/*---------------------------------------------------------------*/


void reset_scaler4() {

  int k;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  /* reset tagger scalers */
  for(k=0; k<ntgscal; k++) reset_caen_scaler(offset+tgscal[k]);

  return;

}


/*---------------------------------------------------------------*/


/* read eor scalers from crate 1 */
void read_scaler1_eor() {

  int i,j,k;
  volatile unsigned long *p;
  unsigned long offset;

  sysBusToLocalAdrs(0x39,0,&offset);


  for(k=0; k<ntrscal; k++){
    p=(unsigned long*)(offset+trscal[k]+0x10);
    for(j=0; j<2; j++){
      for(i=0; i<8; i++){
	if(trscal[k]!=0) {
	  printf("%11lu ",*(p+j*8+i));
	} else {
	  printf("%11lu ",(unsigned long) 0);
	}
      }
    }
  }
  printf("\n");

  return;
}


/*---------------------------------------------------------------*/


/* Program Flex I/O board as Fin/Fout board. The card will accept the 
input signals ( NIM, sigle ended ECL, or diff. ECL), will convert them 
to diff. ECL, with funning out through 4 output channels. */

void
setup_flexio() {

  unsigned long laddr;

  struct flex_struct {
    short csr1;
    short port1_data;
    short csr2;
    short port2_data;
    short vector;
  } ;

  volatile struct flex_struct *flex_io;


  sysBusToLocalAdrs(0x29,FLEX_ADDR,&laddr);

  flex_io = (struct flex_struct *)laddr;
  
  flex_io->csr1 = 0x8000;
  flex_io->port1_data = 0;
  flex_io->port2_data = 0;
  
  flex_io->csr1 = 0x6;        /* set port 1 for external pulse mode */
  flex_io->port1_data = 0xF;  /* enable port1 data 0-3 channels */
  
  flex_io->csr2 = 0x6;        /* set port 1 for external pulse mode */
  flex_io->port2_data = 0xF;  /* enable port1 data 0-3 channels */
  
return;
}

/*---------------------------------------------------------------*/

#else /* no UNIX version */

void
scalers_dummy()
{
  return;
}

#endif
