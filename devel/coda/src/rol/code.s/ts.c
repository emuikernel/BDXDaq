#define FROMSCALER1

/*
 *  ts.c
 *
 *  code to get downloaded to ts
 *
 *  E.Wolin, etc.
 *
 *  26-feb-98
 *
 * Modified by V. Gyurjyan 09-aug-99
 * Added ppc_offset, E.Wolin, 28-mar-2002
 *
 */

#include <stdio.h>
#include <string.h>

#ifdef FROMSCALER1
/* boards moved from scaler1 */
#define LATCH1ADDR     0x000ED0
#define LATCH2ADDR     0x000EE0
#define L1MLU_MEM_ADDR 0x6C0000/*was 0xEC0000*/
#define L1MLU_CSR_ADDR 0x0EC0
#define L2MLU_MEM_ADDR 0x6E0000/*was 0xEE0000*/
#define L2MLU_CSR_ADDR 0x0EE0

/* possible level1/level2 sector based combinations */
#define MAX_OR_COMBS 100
#define MAX_AND_COMBS 100

#define L2PASS 0xaa
#define L2FAIL 0x55

#define L1PASS 1
#define L1FAIL 0

#endif


#ifdef VXWORKS

#include <vxWorks.h> 
#include <semLib.h> 
#include <wdLib.h>
#include <taskLib.h>
#include <tickLib.h>

int sysBusToLocalAdrs();

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

/* board hardwired addresses */
#define TSBASE  0xed0000
#define TSPROG  0xed4000
#define TIBASE  0xff0ed0
#define ORBASE  0xdd0000

/* global PPC board offset */
static unsigned long ppc_offset;


/* local TS program store */
long tsprog_store[4096];
long tspre_store[8];
long tscontrol_store;


/*  holds ti value */
short ti_val;


/* register records whether run_lock is set */
static int run_lock = 0;

/* register records whether level2 is must be turned on */
static int level2 = 0;

/* holds time interval in seconds between force syncs */
static int forceSyncInterval = 0;

/* for OR module stats...33rd word counts any_bit_on , 34th is number of tries */
static unsigned long or_stat_buf[34];
static int or_ticks = 1;       /* units are 1 tick = 10 msec */
static int or_loop  = 1000;    /* 1 loop takes approx 1 micro-sec */


/* prototypes */
extern unsigned long sysClkRateGet();

/*---------------------------------------------------------------*/


/* forces sync event every interval seconds...E.Wolin */
int
force_sync_event(int interval, int arg2, int arg3, int arg4, int arg5, 
                 int arg6, int arg7, int arg8, int arg9, int arg10)
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  /* only force synch if go bit set */
  for(;;)
  {
    taskDelay(sysClkRateGet()*interval); 
    if((*csr&0x1)==0x1) *csr = 0x8;
  }

}


/*---------------------------------------------------------------*/


/* histograms OR module enabled input bit frequencies...E.Wolin */
int
collect_or_stats()
{  
  int i;
  volatile unsigned long or_bits, enabled_bits;
  volatile unsigned long *or_enable;
  volatile unsigned long *or_data;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  or_enable = (volatile unsigned long *)(ppc_offset+ORBASE+0x0);
  or_data   = (volatile unsigned long *)(ppc_offset+ORBASE+0x4);


  /* forever loop */
  for(;;)
  {
    /* only check if some bits enabled */
    enabled_bits = *or_enable;
    if(enabled_bits!=0)
    {
      /* read OR module and record if any enabled bits on during loop */
      or_bits=0;
      for(i=0; i<or_loop; i++)
      {
        or_bits|=((*or_data)&enabled_bits);
      }
      
      /* update stats if any enabled bits on, count number of tries */
      if(or_bits!=0)
      {
        for(i=0; i<32; i++) or_stat_buf[i]+=(or_bits>>i)&0x1;
        or_stat_buf[32]++;
      }
      or_stat_buf[33]++;

    }


    /* sleep...1 tick is 10 msec */
    taskDelay(or_ticks);
  }


  return(0);
}


/*---------------------------------------------------------------*/


/* dumps or stats to stdout...E.Wolin, 29-sep-98 */
int
or_stats()
{  
  volatile unsigned long *or_enable;
  int i,j;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  or_enable = (volatile unsigned long *)(ppc_offset+ORBASE+0x0);


  for(i=0; i<4; i++)
  {
    for(j=0; j<8; j++)
    {
      printf("%10lu  ",or_stat_buf[8*i+j]);
    }
    printf("\n");
  }
  printf("%10lu  %10lu\n",or_stat_buf[32],or_stat_buf[33]);
  printf("0x%8lx\n\n",*or_enable);

  return(0);
}

/*---------------------------------------------------------------*/


/* resets or stats ...E.Wolin, 29-sep-98 */
int
reset_or_stats()
{
  int i;

  for(i=0; i<34; i++) or_stat_buf[i]=0;

  return(0); 
}

/*---------------------------------------------------------------*/


/* set number of iterations of OR stat loop...E.Wolin, 29-sep-98 */
void
set_or_loop(int nloop)
{
  or_loop=nloop;

  return;  
}

/*---------------------------------------------------------------*/


/* set number of ticks of OR wait loop...E.Wolin, 29-sep-98 */
void
set_or_ticks(int nticks)
{
  or_ticks=nticks;

  return; 
}

/*---------------------------------------------------------------*/


/* dumps or register...E.Wolin, 14-oct-98 */
void
or_regs()
{
  volatile unsigned long *or_enable;
  volatile unsigned long *or_data;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  or_enable = (volatile unsigned long *)(ppc_offset+ORBASE+0x0);
  or_data   = (volatile unsigned long *)(ppc_offset+ORBASE+0x4);

  printf("0x%8lx  0x%8lx\n",*or_enable,*or_data);

  return; 
}


/*---------------------------------------------------------------*/


/*  reads TS ROC enable register ... V.Gyurjyan */
void
ts_rocenable()
{
  volatile long *rocmsk;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  rocmsk = (volatile long *)(ppc_offset+TSBASE+0xc);

  printf("%lx\n",*rocmsk);
}


/*--------------------------------------------------------------------*/

/*  reads TS test register ... V.Gyurjyan */
void
ts_testreg()
{
  volatile long *tstest;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  tstest = (volatile long *)(ppc_offset+TSBASE+0x70);

  printf("%lx\n",*tstest);
}


/*--------------------------------------------------------------------*/

/*  reads TS control/status register...E.Wolin */
void
ts_cstatus()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  printf("%lx\n",*csr);
}


/*--------------------------------------------------------------------*/


/*  reads TS state register...E.Wolin */
void
ts_state()
{
  volatile long *state;
  volatile long dummy;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  state = (volatile long *)(ppc_offset+TSBASE+0x6c);

  dummy = *state;              /* must read twice to get dynamic info */
  dummy = *state;
  printf("%lx\n",*state);
}


/*--------------------------------------------------------------------*/


/*  reads TS control register...E.Wolin */
void
ts_control()
{
  volatile long *control;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  control = (volatile long *)(ppc_offset+TSBASE+0x8);

  printf("%lx\n",*control);
}


/* some functions to provide interface to external requests */
void
get_ts_trigbits()
{
  volatile long *control;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  control = (volatile long *)(ppc_offset+TSBASE+0x8);

  {
    int i;

    for(i=1; i<=12; i++)
	{
      printf("%2d",((*control)>>i)&0x1);
	}
    printf("\n");
  }
}

void
set_ts_trigbits(int i1, int i2, int i3, int i4, int i5, int i6,
                int i7, int i8, int i9, int i10, int i11, int i12)
{
  volatile long *control;
  long value;
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  control = (volatile long *)(ppc_offset+TSBASE+0x8);

  {
    printf("input: %1d %1d %1d %1d %1d %1d %1d %1d %1d %1d %1d %1d\n",
      i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12);

    value = *control;
    /*printf("ts_control was 0x%08x\n",value);*/

    value &= 0xffffe001;
    /*printf("ts_control tmp 0x%08x\n",value);*/

    value |= (i1<<1);
    value |= (i2<<2);
    value |= (i3<<3);
    value |= (i4<<4);
    value |= (i5<<5);
    value |= (i6<<6);
    value |= (i7<<7);
    value |= (i8<<8);
    value |= (i9<<9);
    value |= (i10<<10);
    value |= (i11<<11);
    value |= (i12<<12);

    *control = value;
    /*printf("ts_control now 0x%08x\n",value);*/
  }
}

/* program to remotely setup the trig. bits */
void
set_trig_bits(int trig)
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x8);

  /*printf("writing 0x%x to ack reg\n",trig);*/
  *ptr = trig;
}

/*--------------------------------------------------------------------*/


/*  sets TS control register...E.Wolin */
void
set_ts_control(long value)
{
  volatile long *control;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  control = (volatile long *)(ppc_offset+TSBASE+0x8);

  *control = value;
}


/*--------------------------------------------------------------------*/


/*  reads TS scaler0 register...E.Wolin */
void
ts_scaler0()
{
  volatile long *s;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  s = (volatile long *)(ppc_offset+TSBASE+0xc8);

  printf("%lx\n",*s);
}


/*--------------------------------------------------------------------*/


/*  reads TS scaler1 register...E.Wolin */
void
ts_scaler1()
{
  volatile long *s;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  s = (volatile long *)(ppc_offset+TSBASE+0xb0);

  printf("%lx\n",*s);
}


/*--------------------------------------------------------------------*/


/* prints ts control/status go bit...E.Wolin */
void
ts_go_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  printf("%ld\n",((*csr)&0x1));    
}


/* returns ts control/status go bit...E.Wolin */

int
get_ts_go_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  return((*csr)&0x1);
}


/*--------------------------------------------------------------------*/


/*  sets ts control/status go bit...E.Wolin */
void
set_ts_go_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  *csr = 0x1;
}


/*--------------------------------------------------------------------*/


/* resets ts control/status go bit...E.Wolin */
void
reset_ts_go_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  *csr = 0x10000;
}


/*--------------------------------------------------------------------*/


/*  returns ts control/status L1 Enable bit...E.Wolin */
void
ts_l1_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  printf("%d\n",((*csr)&0x20)==0x20);
}


/*--------------------------------------------------------------------*/


/* sets ts control/status L1 Enable bit (uses lower 16 bits)...E.Wolin */
void
set_ts_l1_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  *csr = 0x20;
}


/*--------------------------------------------------------------------*/


/* resets ts control/status L1 enable bit (uses upper 16 bits)...E.Wolin */
void
reset_ts_l1_bit()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  *csr = 0x200000;
}


/*--------------------------------------------------------------------*/


/* resets ts ...E.Wolin */
void
reset_ts()
{
  volatile long *csr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr = (volatile long *)(ppc_offset+TSBASE+0x0);

  *csr=0x8000;
}


/*--------------------------------------------------------------------*/


/* resets TI module */
void
reset_ti()
{
  volatile short *ti_reset;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ti_reset = (volatile short *)(ppc_offset+TIBASE+0x0);

  *ti_reset = 0x80;
}


/*--------------------------------------------------------------------*/


/* sets bit in TI output register */
void
set_ti_outreg(int bit)
{
  volatile short *ti_out;
  short val;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ti_out = (volatile short *)(ppc_offset+TIBASE+0x6);

  if((bit>=0)&&(bit<=7))
  {
    val = *ti_out;
    val |= (0x1<<bit);
    *ti_out = val;
  }
}


/*--------------------------------------------------------------------*/


/* resets bit in TI output register */
void
reset_ti_outreg(int bit)
{
  volatile short *ti_out;
  short val;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ti_out = (volatile short *)(ppc_offset+TIBASE+0x6);

  if((bit>=0)&&(bit<=7))
  {
    val = *ti_out;
    val &= ~(0x1<<bit);
    *ti_out = val;
  }  
}


/*--------------------------------------------------------------------*/


/* reads TI output register */
short
ti_outreg()
{
  volatile short *ti_out;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ti_out = (volatile short *)(ppc_offset+TIBASE+0x6);

  ti_val = *ti_out;

  return(ti_val);
}


/*--------------------------------------------------------------------*/


/* prints TI output register */
void
print_ti_outreg()
{
  volatile short *ti_out;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ti_out = (volatile short *)(ppc_offset+TIBASE+0x6);

  printf("%d",*ti_out);
}


/*--------------------------------------------------------------------*/


/*  print run_lock register */
void
ts_runlock()
{
  printf("%d\n",run_lock);
}


/*--------------------------------------------------------------------*/


/* sets run_lock register */
void
set_ts_runlock()
{
  run_lock = 1;
}


/*--------------------------------------------------------------------*/


/* gets run_lock register */
int
get_ts_runlock()
{
  return(run_lock);
}


/*--------------------------------------------------------------------*/


/* resets run_lock register */
void
reset_ts_runlock()
{
  run_lock = 0;
}


/*--------------------------------------------------------------------*/


/* print level2 register */
void
ts_level2()
{
  printf("%d\n",level2);
}


/*--------------------------------------------------------------------*/


/* sets level2 register */
void
set_ts_level2()
{
  level2 = 1;
}


/*--------------------------------------------------------------------*/


/* gets level2 register */
int
get_ts_level2()
{
  return(level2);
}


/*--------------------------------------------------------------------*/


/* resets level2 register */
void
reset_ts_level2()
{
  level2 = 0;
}


/*--------------------------------------------------------------------*/


/* reads TS prescale registers...E.Wolin */
void
ts_prescale()
{
  int i;
  volatile long *pre;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  pre = (volatile long *)(ppc_offset+TSBASE+0x20);

  
  /* 1st 4 are 24 bit registers */
  for(i=0; i<4; i++)
  {
    printf("%9ld ",(*(pre+i))&0xffffff);
  }
  
  /* 2nd 4 are 16 bit registers */
  for(i=4; i<8; i++)
  {
    printf("%9ld ",(*(pre+i))&0xffff);
  }

  printf("\n");
}  


/*--------------------------------------------------------------------*/


/* sets TS prescale registers...E.Wolin */
void
set_ts_prescale(int p0, int p1, int p2, int p3,
                int p4, int p5, int p6, int p7)
{
  volatile long *pre;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  pre = (volatile long *)(ppc_offset+TSBASE+0x20);
  
  *pre++ = (p0<0) ? 0 : p0;
  *pre++ = (p1<0) ? 0 : p1;
  *pre++ = (p2<0) ? 0 : p2;
  *pre++ = (p3<0) ? 0 : p3;
  *pre++ = (p4<0) ? 0 : p4;
  *pre++ = (p5<0) ? 0 : p5;
  *pre++ = (p6<0) ? 0 : p6;
  *pre++ = (p7<0) ? 0 : p7;
}  


/*--------------------------------------------------------------------*/


/* prints TS prescale registers in Steve B's format...E.Wolin */
void
prescale_info()
{
  int i;
  volatile long *pre;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  pre = (volatile long *)(ppc_offset+TSBASE+0x20);
  
  /* 1st 4 are 24 bit registers */
  for(i=0; i<4; i++)
  {
    printf("prescale[%d] = %ld \n",i,(*(pre+i))&0xffffff);
  }
  
  /* 2nd 4 are 16 bit registers */
  for(i=4; i<8; i++)
  {
    printf("prescale[%d] = %ld \n",i,(*(pre+i))&0xffff);
  }
}


/*--------------------------------------------------------------------*/


/* prints TS registers and program in usual file format...E.Wolin, 19-may-97 */
void
ts_prog()
{
  int i,j;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  /* control registers */
  printf("\n\n*TSREG*\n");
  p = (volatile long *)(ppc_offset+TSBASE+0x0);
  for(i=0; i<128; i+=8)
  {
    for(j=0; j<8; j++)
    {
      printf("%9lx ",*p++);
    }
    printf("\n");
  }
  printf("\n\n");


  /* lookup table */
  printf("\n\n*TSPROG*\n");
  p = (volatile long *)(ppc_offset+TSPROG+0x0);
  for (i=0; i<4096; i+=8)
  {
    for(j=0; j<8; j++)
    {
      printf("%9lx ",*p++);
    }
    printf("\n");
  }
}


/*---------------------------------------------------------------*/


/* prints TS registers...E.Wolin, 19-may-97 */
void
ts_regs()
{
  int i,j;
  volatile long *p;
  unsigned long regs[128];

  /* read TS registers into local array */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSBASE+0x0);

  for(i=0; i<128; i++) regs[i] = *p++;

  /* print regs */
  printf("\n\n*TSREG*\n");
  for (i=0; i<128; i+=8)
  {
    for(j=0; j<8; j++)
    {
      printf("%9lx ",regs[i+j]);
    }
    printf("\n");
  }
}


/*---------------------------------------------------------------*/


/* save TS program...E.Wolin */
void
save_ts_program()
{
  int i;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);

  for(i=0; i<4096; i++) tsprog_store[i] = *(p+i);
}


/*---------------------------------------------------------------*/


/* restore TS program...E.Wolin */
void
restore_ts_program()
{
  int i;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);

  for(i=0; i<4096; i++) *(p+i) = tsprog_store[i];
}


/*---------------------------------------------------------------*/


/* prints all TS registers in hex format...E.Wolin */
void
ts_reg()
{
  int i;
  volatile long *reg;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  reg = (volatile long *)(ppc_offset+TSBASE+0x0);

  for (i=0; i<128; i++) printf("0x%08lx ",*reg++);
  printf("\n");
}


/*---------------------------------------------------------------*/


/* loads default L1 program into TS, original by D.Abbott...E.Wolin, 16-oct-97 */

/* 
 * Construct memory data  ---  in the following model, all trigger patterns 
 *  that form the memory address are assigned to trigger class 1.  For those 
 *  trigger patterns with a single hit, the ROC code is set to be the trigger 
 *  input number.  Otherwise, the ROC code is set to 0xE.  
 *
 *  All LEVEL 1 ACCEPT signals are asserted for every pattern.
 *
 *  If single bit on,    evtype = bit  (bit = 1..12)
 *  If multiple bits on, evtype = 14
 *
 *  If nop bits are on, evtype = 13;
 *
 * Sergey's NOTE: event type 15 is reserved for helicity strib events, see rol/ts2.c
*/

void
load_default_l1_program()
{
  int addr, i, mem;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);
  printf("program TS memory at 0x%08x\n",(int)p);

  /* first copy program into TS program store (local memory array) */
  tsprog_store[0] = 0xFFD3;
  for(addr=1; addr<4096; addr++) tsprog_store[addr] = 0xFFE3;

  /* fix ROC code for single hit patterns  */
  i = 0;
  for(addr=1; addr<4096; addr=2*addr)
  {
    i++;
    tsprog_store[addr] = 0xFF03 + (0x10)*i;
  }

  /* download and compare */        
  for(addr=0; addr<4096; addr++)
  {
    *(p+addr) = tsprog_store[addr];
    mem = (*(p+addr)) & 0xffff;
    if(tsprog_store[addr] != mem)
    {
      printf("?TS memory error %d %lx %x\n",addr,tsprog_store[addr],mem);
    }
  }
}

/* the same, but for new TS2 */
void
ts2_load_default_l1_program()
{
  int addr, i, iprints, mem;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);
  printf("program TS2 memory at 0x%08x\n",(int)p);

  /* first copy program into TS program store (local memory array) */
  tsprog_store[0] = 0xDFF03;
  for(addr=1; addr<4096; addr++) tsprog_store[addr] = 0xEFF03;

  /* fix ROC code for single hit patterns  */
  i = 0;
  for(addr=1; addr<4096; addr=2*addr)
  {
    i++;
    tsprog_store[addr] = 0x0FF03 + (0x10000)*i;
  }

  /* download and compare */
  iprints=0;
  for(addr=0; addr<4096; addr++)
  {
    *(p+addr) = tsprog_store[addr];
    mem = (*(p+addr)) & 0xFFF0F;
    if(tsprog_store[addr] != mem)
    {
      iprints++;
      if(iprints<10) printf("ERROR: TS2 memory: [0x%04x] put 0x%lx, get 0x%04x\n",
        addr,tsprog_store[addr],mem);
    }
  }

  if(iprints==0) printf("TS2 memory successfully programmed\n");
}


/*---------------------------------------------------------------*/


/* loads default L2 program into TS...E.Wolin,VHG  12-jan-1999 */

/* 
 * Construct memory data  ---  in the following model, all trigger patterns 
 *  that form the memory address are assigned to trigger class 2.  For those 
 *  trigger patterns with a single hit, the ROC code is set to be the trigger 
 *  input number.  Otherwise, the ROC code is set to 0xE.  
 *
 *  All LEVEL 1 ACCEPT signals are asserted for every pattern.
 *
 *  If single bit on,    evtype = bit  (bit = 1..12)
 *  If multiple bits on, evtype = 14
 *
 *  if no bits are on, evtype = 13 with clas1 trigger
 *
*/

void
load_default_l2_program()
{
  int addr,i,mem;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);

  /* first copy program into TS program store, then download and check */
  tsprog_store[0] = 0xFFD5;
  for(addr=1; addr<4096; addr++) tsprog_store[addr] = 0xFFE5;
  
  /* fix ROC code for single hit patterns  */
  i = 0;
  for(addr=1; addr<4096; addr=2*addr)
  {
    i++;
    tsprog_store[addr] = 0xFF05 + (0x10)*i;
  }
  
  /* download and compare */        
  for(addr=0; addr<4096; addr++)
  {
    *(p+addr) = tsprog_store[addr];
    mem = (*(p+addr)) & 0xffff;
    if(tsprog_store[addr]!=mem)
    {
      printf("?TS memory error %d %lx %x\n",addr,tsprog_store[addr],mem);
    }
  }

}

/* the same, but for new TS2 */
void
ts2_load_default_l2_program()
{
  int addr,i,mem;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);

  /* first copy program into TS program store, then download and check */
  tsprog_store[0] = 0xDFF05;
  for(addr=1; addr<4096; addr++) tsprog_store[addr] = 0xEFF05;
  
  /* fix ROC code for single hit patterns  */
  i = 0;
  for(addr=1; addr<4096; addr=2*addr)
  {
    i++;
    tsprog_store[addr] = 0x0FF05 + (0x10000)*i;
  }
  
  /* download and compare */        
  for(addr=0; addr<4096; addr++)
  {
    *(p+addr) = tsprog_store[addr];
    mem = (*(p+addr)) & 0xfffff;
    if(tsprog_store[addr]!=mem)
    {
      logMsg("?TS memory error %d %lx %x\n",
        addr,tsprog_store[addr],mem,4,5,6);
    }
  }

  logMsg("TS2 memory successfully programmed\n",1,2,3,4,5,6);
}


/*---------------------------------------------------------------*/

/* 
 * Construct memory data  ---  in the following model, all trigger patterns 
 *  that form the memory address are assigned to trigger class 1.  For those 
 *  trigger patterns with a single hit, the ROC code is set to be the trigger 
 *  input number.  Otherwise, the ROC code is set to 0xE.  
 *
 *  All LEVEL 1 ACCEPT signals are asserted for every pattern.
 *
 *  If single bit on,    evtype = bit  (bit = 1..12)
 *  If multiple bits on, evtype = 14
 *  pattern 1 is illegal (evtype=15) 
 *  pattern 2 is class 2 trigger
 *
*/

void
load_default_l1l2_program()
{
  int addr,i,mem;
  volatile long *p;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  p = (volatile long *)(ppc_offset+TSPROG+0x0);

  /* first copy program into TS program store, then download and check */
  tsprog_store[0] = 0xFFD3;
  for(addr=1; addr<4096; addr++) tsprog_store[addr] = 0xFFE3;

  /* fix ROC code for single hit patterns  */
  i = 0;
  for(addr=1; addr<4096; addr=2*addr)
  {
    i++;
    tsprog_store[addr] = 0xFF03 + (0x10)*i;
  }

  /* 1 illegal (event type 15), 2 means class 2 trigger (event type ???)*/
  tsprog_store[1] = 0xFFF3;
  tsprog_store[3] = 0xFF13;
  tsprog_store[2] = 0xFF25;

  /* download and compare */        
  for(addr=0; addr<4096; addr++)
  {
    *(p+addr) = tsprog_store[addr];
    mem = (*(p+addr)) & 0xffff;
    if(tsprog_store[addr] != mem)
    {
      printf("?TS memory error %d %lx %x\n",addr,tsprog_store[addr],mem);
    }
  }
}


/*---------------------------------------------------------------*/


/* performs L1 trigger rate scan...E.Wolin */
/* assumes TS EN1 and GO connected to L1 ENABLE and RUN/STOP, respectively */
int
l1_rate_scan()
{
  volatile long *csr;
  volatile long *con;
  volatile long *pre;
  volatile long *assign;
  volatile long *scal;
  volatile long *prog;
  long rate;
  unsigned long count[12];
  unsigned long ticks[12];
  unsigned long start,stop;
  int i;


  /* get addresses */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  csr    = (volatile long *)(ppc_offset+TSBASE+0x0);
  con    = (volatile long *)(ppc_offset+TSBASE+0x8);
  pre    = (volatile long *)(ppc_offset+TSBASE+0x20);
  assign = (volatile long *)(ppc_offset+TSBASE+0x78);
  scal   = (volatile long *)(ppc_offset+TSBASE+0xb0);
  prog   = (volatile long *)(ppc_offset+TSPROG+0x0);


  /* check if run in progress...no scan allowed if GO or L1ENABLE bit set */
  if((*csr&0x21) != 0)
  {
    printf("?GO or L1ENABLE set, no scan allowed\n");
    return(1);
  }

  /* save TS program, prescales, l1 mask */
  save_ts_program();
  for(i=0; i<8; i++) tspre_store[i]=*(pre+i);
  tscontrol_store=*con;

  /* clear TS memory, prescales, set l1 mask register, enable L1 trigger */
  for(i=0; i<4096; i++) *(prog+i)=0;
  for(i=0; i<8; i++) *(pre+i)=0;
  *con=0x1FFF;                               /* enable all l1 bits, no strobe, no open prescales */
  *csr=0x20;                                 /* set EN1 */

  /* scan each of 12 channels for about 1/2 sec */
  rate=sysClkRateGet();              	     /* clock rate in ticks/sec */
  for(i=0; i<12; i++)
  {	       
    *assign = i+1;                   	     /* assign scaler */
    *scal = 0;                     	     /* clear scaler */
    start = tickGet();                 	     /* get start time, in ticks */
    *csr = 0x1;                      	     /* set GO to start scan and L1 trigger */
    taskDelay(rate/2);               	     /* wait about 1/2 sec */
    *csr = 0x10000;                  	     /* clear GO to stop scan and L1 trigger */
    stop = tickGet();                  	     /* get stop time, in ticks */
    count[i] = *scal;                          /* store count */
    ticks[i] = stop-start;                     /* store ticks */
  }

  /* reset EN1, restore TS program, prescales, l1 mask */
  *csr=0x200000;
  restore_ts_program();
  for(i=0; i<8; i++) *(pre+i) = tspre_store[i];
  *con = tscontrol_store;

  /* print results */
  for(i=0; i<12; i++) printf("%lu:%lu:%ld ",count[i],ticks[i],rate);
  printf("\n");

  /* done */
  return(0);
}


/*---------------------------------------------------------------*/


/* program to pause a run */

void
pause_run()
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x0);
 
  *ptr = 0x10000;
}


/*---------------------------------------------------------------*/


/* program to resume a run */
void
resume_run()
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x0);
 
  *ptr = 0x1;
  printf("resume_run\n");
}


/*---------------------------------------------------------------*/


/* program to resume a run after pause on next sync*/
void
pause_on_sync_resume_run()
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x0);

  *ptr = 0x3;
  printf("pause_on_sync_resume_run\n");
}



/*---------------------------------------------------------------*/


/*  for dumping ts program...ejw, 19-may-97 */
void
ts_dump()
{
  /* gets info on TS registers */
  int ii,jj;
  volatile int *rdiv;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  /* control registers */
  rdiv = (volatile int *)(ppc_offset+TSBASE+0x0);
  for(ii=0; ii<32; ii+=8)
  {
    for(jj=0; jj<8; jj++)
    {
      printf("%9x ",*rdiv);
      rdiv++;
    }
    printf("\n");
  }
  printf("\n");

  /* lookup table */
  rdiv = (volatile int *)(ppc_offset+TSPROG+0x0);
  for(ii=0; ii<4096; ii+=8)
  {
    for(jj=0; jj<8; jj++)
    {
      printf("%9x ",*rdiv);
      rdiv++;
    }
    printf("\n");
  }
}  


/*---------------------------------------------------------------*/


/* function to enable the TCI cards */
void
tci_on()
{
  volatile short *ptr;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ptr = (volatile short *)(ppc_offset+TIBASE+0x0);

  *ptr = 0xffff;
}


/*---------------------------------------------------------------*/


/* function to disable the TCI cards */
void
tci_off()
{
  volatile short *ptr;

  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ptr = (volatile short *)(ppc_offset+TIBASE+0x0);

  *ptr = 0x0;
}


/*---------------------------------------------------------------*/
int
resetForceSyncIntervalIgnoringGoBit()
{
  forceSyncInterval = 0;
  logMsg("resetForceSyncIntervalIgnoringGoBit: forceSyncInterval set to %d\n",
      forceSyncInterval,2,3,4,5,6);
  return(0);
}

int
getForceSyncInterval()
{
  return(forceSyncInterval);
}

/* parameter 'time' in seconds */
int
setForceSyncInterval(int time)
{
  if(get_ts_go_bit() == 0)
  {
    if(time < 0)        forceSyncInterval = 0; /* 0 means no force syncs */
    else if(time > 100) forceSyncInterval = 100;
    else                forceSyncInterval = time;

    logMsg("setForceSyncInterval: forceSyncInterval set to %d\n",
      forceSyncInterval,2,3,4,5,6);

    return(forceSyncInterval);
  }
  else
  {
    logMsg("ERROR: setForceSyncInterval: 'go' bit is set\n",1,2,3,4,5,6);
    return(0);
  }
}


/* function for forcing synch. event every 'forceSyncInterval' seconds  */
void
forceSyncTask()
{
  extern unsigned long sysClkRateGet();
  volatile int *reg;
  int val;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  reg = (volatile int *)(ppc_offset+TSBASE+0x0);

  for( ; ; )
  {
    if(forceSyncInterval==0) /* if interval==0, wait 1 sec and check again */
    {
      taskDelay(sysClkRateGet());
      continue;
    }
    else
    {
      taskDelay(sysClkRateGet()*forceSyncInterval);
      val = *reg;
      if(val & 0x1) /* check if go is set */
      {
        /*printf("forceSyncTask: 'go' is set\n");*/
	    *reg = 0x8; /* force synch */
      }
      /*printf("forceSyncTask: interval=%d\n",forceSyncInterval);*/
    }
  }
}


/****************************/
/* force sync stuff for TS2 */

typedef struct vme_ts {
  volatile unsigned long csr;
  volatile unsigned long csr2;
  volatile unsigned long trig;
  volatile unsigned long roc;
  volatile unsigned long sync;
  volatile unsigned long trigCount;
  volatile unsigned long trigData;
  volatile unsigned long lrocData;
  volatile unsigned long prescale[8];
  volatile unsigned long timer[5];
  volatile unsigned long intVec;
  volatile unsigned long rocBufStatus;
  volatile unsigned long lrocBufStatus;
  volatile unsigned long rocAckStatus;
  volatile unsigned long userData1;
  volatile unsigned long userData2;
  volatile unsigned long state;
  volatile unsigned long test;
  volatile unsigned long blank1;
  volatile unsigned long scalAssign;
  volatile unsigned long scalControl;
  volatile unsigned long scaler[18];
  volatile unsigned long scalEvent;
  volatile unsigned long scalLive1;
  volatile unsigned long scalLive2;
  volatile unsigned long id;
} VME_TS2;



void
ts2SetProgram1Event(int eventType, int syncFlag)
{
  volatile VME_TS2 *ts;

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);
  ts->userData1 = (eventType&0xF) + ((syncFlag&0x1)<<7); /* 4-bit type ! */
}

void
ts2SetProgram2Event(int eventType, int syncFlag)
{
  volatile VME_TS2 *ts;

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);
  ts->userData2 = (eventType&0xF) + ((syncFlag&0x1)<<7); /* 4-bit type ! */
}

void
ts2SendProgram1Event(void)
{
  volatile VME_TS2 *ts;

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);
  ts->csr = 0x10;
}

void
ts2SendProgram2Event(void)
{
  volatile VME_TS2 *ts;

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);
  ts->csr = 0x20;
}

/* function for forcing synch. event every 10 seconds; use command
      taskSpawn "FORCE_SYNC",119,0,6000,ts2syncTask
   in the boot script */

void
ts2syncTask()
{
  volatile VME_TS2 *ts;
  int word;
  extern unsigned long sysClkRateGet();

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);

  while(1)
  {
    if(forceSyncInterval==0) /* if interval==0, wait 1 sec and check again */
    {
      taskDelay(sysClkRateGet());
    }
    else
    {
      taskDelay(sysClkRateGet()*forceSyncInterval);
      word = ts->csr;
      if(word & 0x1) /* check if 'Go' bit is set */
      {
        /*logMsg("force_synch: 'Go' bit is set - ts2SendProgram2Event\n",0,0,0,0,0,0);*/
        ts2SendProgram2Event();
      }
      else
      {
        /*logMsg("force_synch: 'Go' bit is not set - do nothing\n",0,0,0,0,0,0)*/;
      }
    }
  }

  return;
}

/****************************/

void
ts2_status(void)
{
  volatile VME_TS2 *ts;
  int branch;
  unsigned int status;
  int i;
  int rocenabled;

  sysBusToLocalAdrs(0x39, (long *)TSBASE, (long **)&ts);

  /* CSR1 */
  status = ts->csr;
  printf("CSR 1 (0x0):\n");
  printf("\t                              Go : %d\n", (status>>0)&1);
  printf("\t    Pause on Next scheduled Sync : %d\n", (status>>1)&1);
  printf("\t                  Sync and Pause : %d\n", (status>>2)&1);
  printf("\t             Initiate Sync Event : %d\n", (status>>3)&1);
  printf("\t        Initiate Program 1 Event : %d\n", (status>>4)&1);
  printf("\t        Initiate Program 2 Event : %d\n", (status>>5)&1);
  printf("\t Enable Level 1 (drives outputs) : %d\n", (status>>6)&1);
  printf("\t                Override Inhibit : %d\n", (status>>7)&1);
  printf("\t                       Test Mode : %d\n", (status>>8)&1);
  printf("\t                        Reserved : %d\n", (status>>9)&1);
  printf("\t                           Reset : %d\n", (status>>14)&1);
  printf("\t                      Initialize : %d\n", (status>>15)&1);
  printf("\n");
  printf("\t             Sync Event occurred : %d\n", (status>>16)&1);
  printf("\t        Program 1 Event occurred : %d\n", (status>>17)&1);
  printf("\t        Program 2 Event occurred : %d\n", (status>>18)&1);
  printf("\t              Late Fail occurred : %d\n", (status>>19)&1);
  printf("\t                Inhibit occurred : %d\n", (status>>20)&1);
  printf("\t       Write FIFO error occurred : %d\n", (status>>21)&1);
  printf("\t       Read FIFO error occurreds : %d\n", (status>>22)&1);


  /* CSR2 */
  status = ts->csr2;
  printf("CSR 2 (0x4):\n");
  printf("\t              Enable Scheduled Sync : %d\n", (status>>0)&1);
  printf("\t             Use Clear Permit Timer : %d\n", (status>>1)&1);
  printf("\t               Use Front Busy Timer : %d\n", (status>>2)&1);
  printf("\t               Use Clear Hold Timer : %d\n", (status>>3)&1);
  printf("\t            Use External Front Busy : %d\n", (status>>4)&1);
  printf("\t                  Lock ROC Branch 1 : %d\n", (status>>5)&1);
  printf("\t                  Lock ROC Branch 2 : %d\n", (status>>6)&1);
  printf("\t                  Lock ROC Branch 3 : %d\n", (status>>7)&1);
  printf("\t                  Lock ROC Branch 4 : %d\n", (status>>8)&1);
  printf("\t                  Lock ROC Branch 5 : %d\n", (status>>9)&1);
  printf("\t Enable Program 1 front panel input : %d\n", (status>>10)&1);
  printf("\t                   Enable Interrupt : %d\n", (status>>11)&1);
  printf("\t        Enable local ROC (branch 5) : %d\n", (status>>12)&1);
  printf("\n");

  /* ROC Enable Register */
  rocenabled = ts->roc;
  printf("ROC Enable Register (0x60) val=0x%x:\n",rocenabled);
  for(branch=1; branch<=4; branch++)
  {
	printf("Branch %d: 0x%2x  bits: ", branch, rocenabled&0xFF);
	for(i=0;i<=7;i++)printf("%d",(rocenabled>>(7-i))&0x01);
	printf("\n");
	rocenabled >>=8;
  }
  printf("\n");

  /* ROC Acknowledge Status Register */
  status = ts->rocAckStatus;
  printf("ROC Acknowledge Status Register (0x60): val=0x%x\n",status);
  for(branch=1;branch<=4;branch++)
  {
	printf("Branch %d: 0x%2x  bits: ", branch, status&0xFF);
	for(i=0;i<=7;i++)printf("%d",(status>>(7-i))&0x01);
	printf("  masked: ");
	for(i=0;i<=7;i++)printf("%d",(status>>(7-i))&0x01&(rocenabled>>(7-i)));
	printf("\n");
	status >>=8;
  }
  printf("\n");

  /* Front Busy Timer Register */
  printf("Front Busy Timer Register = %d\n\n",(int)(ts->timer[3]&0xFFFF));

  /* State Register */
  status = ts->state;
  printf("State Register (0x6C):\n");
  printf("\t               Level 1 Accept     : %d\n", (status>>0)&1);
  printf("\t            Start Level 2 Trigger : %d\n", (status>>1)&1);
  printf("\t             Level 2 Pass Latched : %d\n", (status>>2)&1);
  printf("\t             Level 2 Fail Latched : %d\n", (status>>3)&1);
  printf("\t                   Level 2 Accept : %d\n", (status>>4)&1);
  printf("\t            Start Level 3 Trigger : %d\n", (status>>5)&1);
  printf("\t             Level 3 Pass Latched : %d\n", (status>>6)&1);
  printf("\t             Level 3 Fail Latched : %d\n", (status>>7)&1);
  printf("\t                   Level 3 Accept : %d\n", (status>>8)&1);
  printf("\t                            Clear : %d\n", (status>>9)&1);
  printf("\t        Front End Busy (external) : %d\n", (status>>10)&1);
  printf("\t                 External Inhibit : %d\n", (status>>11)&1);
  printf("\t                  Latched Trigger : %d\n", (status>>12)&1);
  printf("\t                          TS Busy : %d\n", (status>>13)&1);
  printf("\t                        TS Active : %d\n", (status>>14)&1);
  printf("\t                         TS Ready : %d\n", (status>>15)&1);
  printf("\t            Main Sequencer Active : %d\n", (status>>16)&1);
  printf("\t Synchronization Sequencer Active : %d\n", (status>>17)&1);
  printf("\t Program 1 Event Sequencer Active : %d\n", (status>>18)&1);
  printf("\t Program 2 Event Sequencer Active : %d\n", (status>>19)&1);
  printf("\n\n");

  /* Trigger Word Count Register */
  status = ts->trigCount;
  printf("Trigger Word Count Register (0x14): %d\n",status&0x0FFF);
}


/*---------------------------------------------------------------*/


/* program to set all 8 prescale factors  */
void
prescale_all(int p1, int p2, int p3, int p4,
             int p5, int p6, int p7, int p8)
{
  volatile int *r1, *r2, *r3, *r4, *r5, *r6, *r7, *r8;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  r1 = (volatile int *)(ppc_offset+TSBASE+0x20);
  r2 = (volatile int *)(ppc_offset+TSBASE+0x24);
  r3 = (volatile int *)(ppc_offset+TSBASE+0x28);
  r4 = (volatile int *)(ppc_offset+TSBASE+0x2c);
  r5 = (volatile int *)(ppc_offset+TSBASE+0x30);
  r6 = (volatile int *)(ppc_offset+TSBASE+0x34);
  r7 = (volatile int *)(ppc_offset+TSBASE+0x38);
  r8 = (volatile int *)(ppc_offset+TSBASE+0x3c);
  
  *r1 = p1;
  *r2 = p2;
  *r3 = p3;
  *r4 = p4;
  *r5 = p5;
  *r6 = p6;
  *r7 = p7;
  *r8 = p8;
}  

/*---------------------------------------------------------------*/


/* program to remotely setup the ack. bits */
void
set_ack_bits(int branch1, int branch2, int branch3, int branch4)
{
  volatile int *ptr;
  int temp;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0xc);

  temp = branch1 + branch2*256  + branch3*256*256 + branch4*256*256*256;
  /* printf("writing 0x%x to ack reg\n",temp); */
  *ptr = temp;
}


/*---------------------------------------------------------------*/


/* set  late_fail mode, clear permit timer set to 0ns....V.Gyurjyan*/
void
late_fail()
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x40);
 
 *ptr = 0;
}


/*---------------------------------------------------------------*/


/* set real rejection mode, clear permit timer set to 3200ns....V. Gyurjyan*/
void
real_rej()
{
  volatile int *ptr;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  ptr = (volatile int *)(ppc_offset+TSBASE+0x40);
 
 *ptr = 80;
}


/*---------------------------------------------------------------*/


/*  sets TS scaler 1 assigned register ... V. Gyurjyan */
void
set_ts_scaler1(long value)
{
  volatile long *control;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  control = (volatile long *)(ppc_offset+TSBASE+0x78);

  *control=value;
}


/*--------------------------------------------------------------------*/

void
l1_regs_get()
{
  volatile unsigned short *reg1, *reg2;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  reg1 = (volatile unsigned short *)(ppc_offset+0x320000);
  reg2 = (volatile unsigned short *)(ppc_offset+0x340000);

  printf("reg1=0x%04x reg2=0x%04x\n",*reg1,*reg2);

  return;
}

void
l1_regs_set(unsigned short dat1, unsigned short dat2)
{
  volatile unsigned short *reg1, *reg2;

  sysBusToLocalAdrs(0x39,0,&ppc_offset);
  reg1 = (volatile unsigned short *)(ppc_offset+0x320000);
  reg2 = (volatile unsigned short *)(ppc_offset+0x340000);

  *reg1 = dat1;
  *reg2 = dat2;
  printf("reg1=0x%04x reg2=0x%04x\n",*reg1,*reg2);

  return;
}



/************************************************************/
/************************************************************/
/******** start of the piece from scalers.c *****************/
#ifdef FROMSCALER1


/* reads level1 latch csr */
void
latch_csr()
{  
  volatile short *p;
 
  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH1ADDR+0x0);
  printf("%10x\n",*p);
  
  return;
}

/* reads level2 latch csr */
void
latch2_csr()
{
  volatile short *p;

  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH2ADDR+0x0);
  printf("%10x\n",*p);
  
  return;
}

/* reads leve1 latch word count*/
void
latch_word_count()
{
  volatile short *p;
 
  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH1ADDR+0x6);
  printf("%10x\n",*p);
  
  return;
}

/* reads leve2 latch word count*/
void
latch2_word_count()
{
  volatile short *p;
 
  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH2ADDR+0x6);
  printf("%10x\n",*p);
  
  return;
}

/* reads level1 latch data word */
void
latch_data()
{
 volatile short *p;

  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH1ADDR+0x8);
  printf("%10x\n",*p);
  
  return;
}

/* reads level2 latch data word */
void
latch2_data()
{
  volatile short *p;

  /*  get PPC global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  p=(unsigned short *)(ppc_offset+LATCH2ADDR+0x8);
  printf("%10x\n",*p);
  
  return;
}

/* download Level1 MLU */
int
download_l1_mlu()
{  
  unsigned long mem, csr;
  volatile unsigned short *mlu_csr, *mlu_mem;
  int ii, iprints, test;
  volatile unsigned short *ptr16;

    
  /* MLU addresses for D16, ppc */
  sysBusToLocalAdrs(0x29,L1MLU_CSR_ADDR,&csr);  /* A16: 0xf0f00000-0x */
  sysBusToLocalAdrs(0x39,L1MLU_MEM_ADDR,&mem);  /* A24: 0xeff00000-0x */

  mlu_csr = (volatile unsigned short *)csr;
  mlu_mem = (volatile unsigned short *)mem;
  printf("csr,mem are: 0x%08x, 0x%08x\n",(int)mlu_csr,(int)mlu_mem);


  /* setup memory write */
  *mlu_csr = 0x3c;


  /* here MLU just converts input levels to output pulses, */
  /*  so address content is just the address (64k shorts)   */
  ptr16 = (volatile unsigned short *)mem;
  for(ii=0; ii<0x10000; ii+=2)
  {
    ptr16[1]=ii+1;
    ptr16[0]=ii;
    EIEIO;
    SYNC;
    ptr16+=2;
  }


  /* read back memory and check */
  iprints=0;
  ptr16 = (volatile unsigned short *)mem;
  for(ii=0; ii<0x10000; ii+=2)
  {
    test = ptr16[1];
    if(test != (ii+1))
	{
      iprints++;
      if(iprints<10) printf("1: L1 mlu address 0x%05x (%d) incorrect value: 0x%05x (%d)\n",ii+1,ii+1,test,test);
	}
    /*else if(ii<100) printf("1: [0x%05x] 0x%05x\n",ii+1,test);*/

    test = ptr16[0];
    if(test != ii)
	{
      iprints++;
      if(iprints<10) printf("2: L1 mlu address 0x%05x (%d) incorrect value: 0x%05x (%d)\n",ii,ii,test,test);
	}
    /*else if(ii<100) printf("2: [0x%05x] 0x%05x\n",ii,test);*/

    EIEIO;
    SYNC;
    ptr16+=2;
  }

  if(iprints==0) printf("L1 mlu memory has been written Ok\n");
  else           printf("L1 mlu memory write ERROR !!!\n");  

  /* set pulsed mode, enable bits, etc. */
  *mlu_csr = 0x3d;

  return(0);
}

/* download Level2 MLU - not in use, just to test MLU */
int
download_l2_mlu()
{  
  unsigned long mem,csr;
  volatile short *mlu_csr, *mlu_mem, test;
  int i, iprints;

    
  /* MLU addresses for D16, ppc */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&csr);  /* A16 */
  sysBusToLocalAdrs(0x39,L2MLU_MEM_ADDR,&mem);  /* A24 */

  mlu_csr = (short *)csr;
  mlu_mem = (short *)mem;
  printf("csr,mem are: 0x%08x, 0x%08x\n",(int)mlu_csr,(int)mlu_mem);


  /* setup memory write */
  *mlu_csr = 0x3c;


  /* here MLU just converts input levels to output pulses, */
  /*  so address content is just the address (64k shorts)   */
  for(i=0; i<0x10000; i++) {*(mlu_mem+i)=(short)i;}


  /* read back memory and check */
  iprints=0;
  for(i=0; i<0x10000; i++)
  {
    test = *(mlu_mem+i);
    if(test!=(short)i)
	{
      iprints++;
      if(iprints<10) printf("L2 mlu address 0x%08x (%d) incorrect value: 0x%08x (%d)\n",(short)i,(short)i,test,test);
	}
  }
  printf("finished checking mlu memory\n");
  

  /* set pulsed mode, enable bits, etc. */
  *mlu_csr = 0x3d;


  return(0);
}

/* download Level2 MLU - not in use, just to test MLU */
int
download_l2_mlu_test1()
{  
  unsigned long mem,csr;
  volatile short *mlu_csr, *mlu_mem, test;
  int i, iprints;

    
  /* MLU addresses for D16, ppc */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&csr);  /* A16 */
  sysBusToLocalAdrs(0x39,L2MLU_MEM_ADDR,&mem);  /* A24 */

  mlu_csr = (short *)csr;
  mlu_mem = (short *)mem;
  printf("csr,mem are: 0x%08x, 0x%08x\n",(int)mlu_csr,(int)mlu_mem);


  /* setup memory write */
  *mlu_csr = 0x3c;


  /* here MLU just converts input levels to output pulses, */
  /*  so address content is just the address (64k shorts)   */
  for(i=0; i<0x10000; i++) {*(mlu_mem+i)=(short)65536/*i*/;}


  /* read back memory and check */
  iprints=0;
  for(i=0; i<0x10000; i++)
  {
    test = *(mlu_mem+i);
    if(test!=(short)65536/*i*/)
	{
      iprints++;
      if(iprints<10) printf("L2 mlu address 0x%08x (%d) incorrect value: 0x%08x (%d)\n",(short)i,(short)i,test,test);
	}
  }
  printf("finished checking mlu memory\n");
  

  /* set pulsed mode, enable bits, etc. */
  *mlu_csr = 0x3d;


  return(0);
}



/********************/
/* L2 MLU functions */
/********************/

/* reads the level2 mlu memory and prints */
void
read_mlu(void)
{
  volatile unsigned short *mlu_mem_dump;
  volatile unsigned short *mlu_csr, mlu_csr_save;
  volatile unsigned long laddr;
  unsigned short value;
  volatile int l;

  /*MLU memory address (PPC -A24/D16 transfers) */
  sysBusToLocalAdrs(0x39, L2MLU_MEM_ADDR, &laddr);
  printf("MLU memory local address = 0x%08x\n",(int)laddr);
  mlu_mem_dump = (volatile unsigned short *)laddr;

  /* L2 MLU csr address (PPC -A16/D16 transfers) */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&laddr);
  printf("L2 MLU csr local address = 0x%08x\n",(int)laddr);
  mlu_csr = (volatile unsigned short *)laddr;

  /*save csr value and open it for reading*/
  mlu_csr_save = (*mlu_csr)&0xFF;
  *mlu_csr = 0;

  /* Read memory and dump */  
  for(l=0; l<0x10000; l++)
  {
    value = *mlu_mem_dump++;
    printf("[0x%04x] = 0x%04x\n",l,value);
  }

  /*restore csr*/
  *mlu_csr = mlu_csr_save;

  return;
}

/* the same but print passes only*/
void
read_mlu_pass(void)
{
  volatile unsigned short *mlu_mem_dump;
  volatile unsigned short *mlu_csr, mlu_csr_save;
  volatile unsigned long laddr;
  unsigned short value;
  volatile int l;

  /*MLU memory address (PPC -A24/D16 transfers) */
  sysBusToLocalAdrs(0x39, L2MLU_MEM_ADDR, &laddr);
  printf("MLU memory local address = 0x%08x\n",(int)laddr);
  mlu_mem_dump = (volatile unsigned short *)laddr;

  /* L2 MLU csr address (PPC -A16/D16 transfers) */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&laddr);
  printf("L2 MLU csr local address = 0x%08x\n",(int)laddr);
  mlu_csr = (volatile unsigned short *)laddr;

  /*save csr value and open it for reading*/
  mlu_csr_save = (*mlu_csr)&0xFF;
  *mlu_csr = 0;
  
  /* Read memory and dump */
  for(l=0; l<0x10000; l++)
  {
    value = *mlu_mem_dump++;
    if( (value&0xFF) != 0x55 ) printf("[0x%04x] = 0x%04x\n",l,value);
  }

  /*restore csr*/
  *mlu_csr = mlu_csr_save;

  return;
}


/*---------------------------------------------------------------*/



/*setup L2 MLU hardware. Parse input string and program the memory */


/*returns the number of bits that have value 1 for 16 bit word */ 
int
bit_count(int x) 
{
  int i, ones = 0;

  /*for(i=0; i<16; i++) sergey: count first 6 bits only, this is how it is used !!??*/
  for(i=0; i<6; i++)
  {
    if(x & 0x1) ones++;
    x>>=1;
  }
  return(ones);
}


/*maps the string with the MLU input bit patern */
int
set_mem_adr_bit(char *ls, int start_word) 
{
  volatile int mlu_address;

  printf("set_mem_adr_bit reached: >%s<\n",ls);
  
  /* setting logic based on single MLU inputs (we have 16 inputs total) */
  if(!strcmp(ls,     "L2S1"))  start_word = start_word + 1;
  else if(!strcmp(ls,"L2S2"))  start_word = start_word + 2;
  else if(!strcmp(ls,"L2S3"))  start_word = start_word + 4;
  else if(!strcmp(ls,"L2S4"))  start_word = start_word + 8;
  else if(!strcmp(ls,"L2S5"))  start_word = start_word + 16;
  else if(!strcmp(ls,"L2S6"))  start_word = start_word + 32;
  else if(!strcmp(ls,"L1S1"))  start_word = start_word + 64;
  else if(!strcmp(ls,"L1S2"))  start_word = start_word + 128;
  else if(!strcmp(ls,"L1S3"))  start_word = start_word + 256;
  else if(!strcmp(ls,"L1S4"))  start_word = start_word + 512;
  else if(!strcmp(ls,"L1S5"))  start_word = start_word + 1024;
  else if(!strcmp(ls,"L1S6"))  start_word = start_word + 2048;
  else if(!strcmp(ls,"L1B7"))  start_word = start_word + 4096;
  else if(!strcmp(ls,"L1B8"))  start_word = start_word + 8192;
  else if(!strcmp(ls,"L1B9"))  start_word = start_word + 16384;
  else if(!strcmp(ls,"L1B10")) start_word = start_word + 32768;

  /* setting more complex logic cases */
  else if(!strcmp(ls,"ANY1")) start_word = -1;
  else if(!strcmp(ls,"ANY2")) start_word = -2;
  else if(!strcmp(ls,"ANY3")) start_word = -3;
  else if(!strcmp(ls,"ANY4")) start_word = -4;
  else if(!strcmp(ls,"ANY5")) start_word = -5;
  else if(!strcmp(ls,"ANY6")) start_word = -6;
  else if(!strcmp(ls,"NOTL2")) start_word = -7;
  else if(!strcmp(ls,"ATL1")) start_word = -8;
  else if(!strcmp(ls,"ATL2")) start_word = -9;
  else if(!strcmp(ls,"ATL3")) start_word = -10;  
  else if(!strcmp(ls,"ATL4")) start_word = -11;
  else if(!strcmp(ls,"ATL5")) start_word = -12;
  else if(!strcmp(ls,"ES1")) start_word = -13;
  else if(!strcmp(ls,"ES2")) start_word = -14;
  else if(!strcmp(ls,"ES3")) start_word = -15;
  else if(!strcmp(ls,"ES4")) start_word = -16;
  else if(!strcmp(ls,"ES5")) start_word = -17;
  else if(!strcmp(ls,"ES6")) start_word = -18;
  else if(!strcmp(ls,"PB7")) start_word = -19;
  else if(!strcmp(ls,"PB8")) start_word = -20;
  else if(!strcmp(ls,"ES1ATL3")) start_word = -21;
  else if(!strcmp(ls,"ES2ATL3")) start_word = -22;
  else if(!strcmp(ls,"ES3ATL3")) start_word = -23;
  else if(!strcmp(ls,"ES4ATL3")) start_word = -24;
  else if(!strcmp(ls,"ES5ATL3")) start_word = -25;
  else if(!strcmp(ls,"ES6ATL3")) start_word = -26;
  else if(!strcmp(ls,"ES7ATL3")) start_word = -27;
  else if(!strcmp(ls,"BIT7NOL2")) start_word = -28;
  else if(!strcmp(ls,"BIT8NOL2")) start_word = -29;
  else if(!strcmp(ls,"ES1ATL2")) start_word = -30;
  else if(!strcmp(ls,"ES2ATL2")) start_word = -31;
  else if(!strcmp(ls,"ES3ATL2")) start_word = -32;
  else if(!strcmp(ls,"ES4ATL2")) start_word = -33;
  else if(!strcmp(ls,"ES5ATL2")) start_word = -34;
  else if(!strcmp(ls,"ES6ATL2")) start_word = -35;
  else if(!strcmp(ls,"ES7ATL2")) start_word = -36;
  else if(!strcmp(ls,"ES12")) start_word = -37;
  else if(!strcmp(ls,"ES13")) start_word = -38;
  else if(!strcmp(ls,"ES14")) start_word = -39;
  else if(!strcmp(ls,"ES15")) start_word = -40;
  else if(!strcmp(ls,"ES16")) start_word = -41;
  else if(!strcmp(ls,"ES23")) start_word = -42;
  else if(!strcmp(ls,"ES24")) start_word = -43;
  else if(!strcmp(ls,"ES25")) start_word = -44;
  else if(!strcmp(ls,"ES26")) start_word = -45;
  else if(!strcmp(ls,"ES34")) start_word = -46;
  else if(!strcmp(ls,"ES35")) start_word = -47;
  else if(!strcmp(ls,"ES36")) start_word = -48;
  else if(!strcmp(ls,"ES45")) start_word = -49;
  else if(!strcmp(ls,"ES46")) start_word = -50;
  else if(!strcmp(ls,"ES56")) start_word = -51;

  else if(!strcmp(ls,"BIT1ATL1")) start_word = -52;
  else if(!strcmp(ls,"BIT2ATL1")) start_word = -53;
  else if(!strcmp(ls,"BIT3ATL1")) start_word = -54;
  else if(!strcmp(ls,"BIT4ATL1")) start_word = -55;
  else if(!strcmp(ls,"BIT5ATL1")) start_word = -56;
  else if(!strcmp(ls,"BIT6ATL1")) start_word = -57;

  else if(!strcmp(ls,"BIT1ATL2")) start_word = -58;
  else if(!strcmp(ls,"BIT2ATL2")) start_word = -59;
  else if(!strcmp(ls,"BIT3ATL2")) start_word = -60;
  else if(!strcmp(ls,"BIT4ATL2")) start_word = -61;
  else if(!strcmp(ls,"BIT5ATL2")) start_word = -62;
  else if(!strcmp(ls,"BIT6ATL2")) start_word = -63;

  else if(!strcmp(ls,"BIT1NOL2")) start_word = -64;
  else if(!strcmp(ls,"BIT2NOL2")) start_word = -65;
  else if(!strcmp(ls,"BIT3NOL2")) start_word = -66;
  else if(!strcmp(ls,"BIT4NOL2")) start_word = -67;
  else if(!strcmp(ls,"BIT5NOL2")) start_word = -68;
  else if(!strcmp(ls,"BIT6NOL2")) start_word = -69;
  else if(!strcmp(ls,"BIT9NOL2")) start_word = -70;
  else if(!strcmp(ls,"BIT10NOL2")) start_word = -71;

  mlu_address = start_word;
  printf("set_mem_adr_bit output %d\n",mlu_address);
  return mlu_address;
}


int
setup_mlu(char *file_name)
{
  FILE *fp;

  unsigned short *memory;
  volatile unsigned short *mlu;

  volatile short *mlu_csr;
  volatile unsigned long laddr;
  int error = 0;
  int MAXLINE = 1000;
 
  /* vars for parsing */
  char line[MAXLINE];
  char *xx[MAX_OR_COMBS];
  char *xx_xx[MAX_AND_COMBS];
  volatile unsigned short mlu_mem_adr[MAX_OR_COMBS];
  volatile int mlu_complex[MAX_OR_COMBS];
  char *aa, *bb;
  volatile int i,j,k,kk,l,m,nn;
  volatile short my_address, my_address_prev;
  unsigned short value;

  
  /* read the level2 trigger file */
  if((fp = fopen(file_name,"r")) == NULL)
  {
    printf("\nEuphrates ERROR: Can't open %s\n",file_name);
    return(1);
  }
  fscanf(fp,"%s",line); /* Sergey: 'line' is not allocated !!?? */
  fclose(fp);

  /* init mlu_mem_adr to 0 */
  for(l=0; l<MAX_OR_COMBS; l++)
  {
    mlu_mem_adr[l] = 0;
    mlu_complex[l] = 0;
  }  

  /* L2 MLU memory address (PPC -A24/D16 transfers) */
  sysBusToLocalAdrs(0x39,L2MLU_MEM_ADDR,&laddr);
  printf("L2 MLU memory local address = 0x%08x\n",(int)laddr);
  mlu = (volatile unsigned short *)laddr;

  /* L2 MLU csr address (PPC -A16/D16 transfers) */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&laddr);
  printf("L2 MLU csr local address = 0x%08x\n",(int)laddr);
  mlu_csr = (short *)laddr;

  memory = 0;
  memory = (unsigned short *) malloc(0x20000); /* 64k 16bit words */
  if(memory<=0)
  {
    printf("Euphrates ERROR: CANNOT ALLOCATE MEMORY !!!\n");
    return(2);
  }

  /*********************************************************/
  /*----------- parsing the input parameter line --------- */
  /* will fill arrays 'mlu_mem_adr' and 'mlu_complex' */
  /* both arrays have dimension MAX_OR_COMBS */
  /* mlu_mem_adr (short) contains bitmap of desired inputs; may have it before complex element as well
     mlu_complex (int) contains negative code of complex operation
   */

  i=0;
  aa = strtok(line,"||");
  xx[i] = aa;

  /* extract OR's */
  while((aa=strtok(NULL,"||")) != NULL) 
  {
    i++;
    xx[i] = aa; 
  }

  /* loop over OR's */
  for(j=0; j<=i; j++)
  {
    printf("OR's ====> %s\n",xx[j]);
    k=0;
    bb = strtok(xx[j],"x");
    xx_xx[k] = bb;
      
    /* extract AND's */
    while((bb=strtok(NULL,"x")) != NULL) 
	{
	  k++;
	  xx_xx[k] = bb; 
	}

    my_address=0;
    my_address_prev = 0;

    /* loop over AND's */
    for(kk=0; kk<=k; kk++)
    {
      printf("AND ====> %s\n",xx_xx[kk]);

      /* for 'simple' operation, accumulate input bitmask in 'my_address' for every AND'ed element */
      /* for complex operation - just return negative operation code */
      my_address = set_mem_adr_bit(xx_xx[kk],my_address);

      printf("output of the set_bit_function %d\n",my_address);

      if(my_address < 0)
      {
        /* add to complex array the code of the complex operation (ex. any1) */
        mlu_complex[j] = my_address;
        mlu_mem_adr[j] = my_address_prev;
        printf(" complex item %d \n", mlu_mem_adr[j]);
        /* IMPORTANT: after complex operations NO single sector selection */ 
        break;
      }

      my_address_prev = my_address; /* remember in case if next element is complex */
      /* effectively here mlu_complex[j]=0 */
      mlu_mem_adr[j] = my_address;
    }

    printf("Addresses number %d is -> %d\n",j,mlu_mem_adr[j]);
    my_address = 0;
    my_address_prev = 0;
  }
  /*------------ end of parsing alogoritm -----------*/




  /* MLU signals description:

     inputs:
               1-6 sector based level2
	           7-12 - sector based l1
	           13-16 - TS bit7,8,9,10

     outputs:
               0,2 - goes to TS fail
               1,3 - goes to TS pass 
	           8-13 sector based level2
               14 -> OR l2
               15 -> pass based 
	         (on input patern and programmed logic
             take the first 6 bit of the address (sector based level2), 
             shift it left 8 bits and or it with the MLU output code)
  */



  /**********************************/
  /* fill the memory with fail code */

  for(l=0; l<0x10000; l++)
  {
    if((l&0x3f))
      *(memory + l) = (L2FAIL|0x4000|((l&0x3f)<<8)); 
    else
      *(memory + l) = L2FAIL; 
  }


  /*********************************************/
  /* update the memory with the defined passes */

  for(l=0; l<MAX_OR_COMBS; l++)
  {
    /* simple operations */
    if((mlu_mem_adr[l] > 0) && (mlu_complex[l] >= 0) )
	{
      printf("===> SIMPLE OPERATION\n");
      if ((mlu_mem_adr[l]&0x3f))
      {

        *(memory + mlu_mem_adr[l]) = ( L2PASS|0xc000|((mlu_mem_adr[l]&0x3f)<<8) ); 
	    printf("setup_mlu: [0x%08x] <- L2PASS(1)\n",mlu_mem_adr[l]);

		/*temporary
        for(kk=0; kk<16; kk++)
        {
          nn = mlu_mem_adr[l] + (kk<<12);
          memory[nn] = ( L2PASS|0xc000|((mlu_mem_adr[l]&0x3f)<<8) ); 
	      printf("setup_mlu: [0x%04x] <- L2PASS(1)\n",nn);
		}
		*/


      }
      else
	  {
        *(memory + mlu_mem_adr[l]) =  L2PASS|0x8000; /* the only 0x8000 */
	    printf("setup_mlu: [0x%04x] <- L2PASS(2)\n",mlu_mem_adr[l]);
	  }
	}
    else if(mlu_complex[l] < 0 )
    {
      printf("===> COMPLEX OPERATION\n");
      /* complex operation */
      switch(mlu_complex[l])
      {

	    /* one and only one track in any sector */
	  case -1:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* two and only two tracks in any sector */
	  case -2:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* three and only three tracks in any sector */
	  case -3:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==3) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* four and only four tracks in any sector */
	  case -4:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==4) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* five and only five tracks in any sector */
	  case -5:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==5) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* six and only six tracks*/
	  case -6:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)==6) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;





	    /* ignore level2 (NOTL2) */
	  case -7:
	    for(m=0;m<=0x3f;m++)
        {
	      if ((m&0x3f)) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8)); 
	      else          *(memory+mlu_mem_adr[l]+m) =  L2PASS;
	    }
	    break;





	    /* At least one track  in any sector (ATL1) */
	  case -8:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* At least two tracks in any sectors (ATL2) */
	  case -9:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* At least three tracks in any sectors (ATL3) */
	  case -10:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)>=3) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* At least four tracks in any sectors (ATL4) */
	  case -11:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)>=4) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;
	    /* At least five tracks in any sectors (ATL5) */
	  case -12:
	    for(m=0;m<=0x3f;m++) if(bit_count(m)>=5) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    break;





	    /* ES1 both l1 and l2 required in sector1  */
	  case -13:
	    for(m=0;m<0x10000;m++) if((m & 0x41) == 0x41) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -13\n");
	    break;
	    /* ES2 both l1 and l2 required in sector2  */
	  case -14:
	    for(m=0;m<=0x10000;m++) if((m & 0x82) == 0x82) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -14\n");
	    break;
	    /* ES3 both l1 and l2 required in sector3  */
	  case -15:
	    for(m=0;m<=0x10000;m++) if((m & 0x104) == 0x104) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -15\n");
	    break;
	    /* ES4 both l1 and l2 required in sector4  */
	  case -16:
	    for(m=0;m<=0x10000;m++) if((m & 0x208) == 0x208) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -16\n");
	    break;
	    /* ES5 both l1 and l2 required in sector5  */
	  case -17:
	    for(m=0;m<=0x10000;m++) if((m & 0x410) == 0x410) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -17\n");
	    break;
	    /* ES6 both l1 and l2 required in sector6  */
	  case -18:
	    for(m=0;m<=0x10000;m++) if((m & 0x820) == 0x820) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -18\n");
	    break;




	  case -19: /* PB7 */
	    for(m=0;m<=0x10000;m++)
	      if((m & 0x1000) == 0x1000)
          {
		    if ((m&0x3f))
            {
		      if(((m&0x3f) & ((m&0xfc0)>>6))) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		      else                            *(memory+m) = (L2PASS|0x4000|((m&0x3f)<<8));
		    }
		    else
              *(memory+m) = L2PASS;
	      }
	    printf("Loading -19\n");
	    break;

	  case -20: /* PB8 */
	    for(m=0;m<=0x10000;m++)
	      if((m & 0x2000) == 0x2000)
          {
		    if ((m&0x3f))
            {
		      if(((m&0x3f) & ((m&0xfc0)>>6))) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		      else                            *(memory+m) = (L2PASS|0x4000|((m&0x3f)<<8));
		    }
		    else
		      *(memory+m) = L2PASS;
	      }
	    printf("Loading -20\n");
	    break;




	    /* ES1ATL3. L1 and L2 are correlated in the sector1 and we have 3 or more tracks  */
      case -21:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x41) == 0x41) && (bit_count(m) >= 3))                              *(memory+m) =  L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) ) *(memory+m) =  L2FAIL|0x4000|((m&0x0003f)<<8);
          else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))      *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
        }
      	printf("Loading -21\n");
	    break;

	    /* ES2ATL3. L1 and L2 are correlated in the sector2 and we have 3 or more tracks  */
      case -22:
	    for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x82) == 0x82) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -22\n");
	    break;

	    /* ES3ATL3. L1 and L2 are correlated in the sector3 and we have 3 or more tracks  */
      case -23:
	    for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x104) == 0x104) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -23\n");
	    break;

	    /* ES4ATL3. L1 and L2 are correlated in the sector4 and we have 3 or more tracks  */
      case -24:
	    for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x208) == 0x208) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -24\n");
	    break;

	    /* ES5ATL3. L1 and L2 are correlated in the sector5 and we have 3 or more tracks  */
      case -25:
	    for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x410) == 0x410) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -25\n");
	    break;

	    /* ES6ATL3. L1 and L2 are correlated in the sector6 and we have 3 or more tracks  */
      case -26:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x820) == 0x820) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -26 \n");
	    break;

	    /* ES7ATL3. correlation between bit 7 and 3 or more tracks  */
      case -27:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x1000) == 0x1000) && (bit_count(m) >= 3))
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -27 \n");
	    break;

	    /* BIT7NOL2. */
      case -28:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x1000) == 0x1000)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -28 \n");
	    break;

	    /* BIT8NOL2. */
      case -29:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x2000) == 0x2000)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -29 \n");
	    break;
	     
	    /* ES1ATL2. L1 and L2 are correlated in the sector1 and we have 2 or more tracks  */
      case -30:
	    for(m=0;m<0x10000;m++)
        {
          if(((m & 0x41) == 0x41) && (bit_count(m) >= 2))
          { 
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	      }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
      	printf("Loading -30\n");
	    break;

	    /* ES2ATL2. L1 and L2 are correlated in the sector2 and we have 2 or more tracks  */
      case -31:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x82) == 0x82) && (bit_count(m) >= 2))
          {
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
          }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
        }
	    printf("Loading -31\n");
	    break;

	    /* ES3ATL2. L1 and L2 are correlated in the sector3 and we have 2 or more tracks  */
      case -32:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x104) == 0x104) && (bit_count(m) >= 2))
          { 
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -32\n");
	    break;

	    /* ES4ATL2. L1 and L2 are correlated in the sector4 and we have 2 or more tracks  */
      case -33:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x208) == 0x208) && (bit_count(m) >= 2))
          {
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
          else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -33\n");
	    break;

	    /* ES5ATL2. L1 and L2 are correlated in the sector5 and we have 2 or more tracks  */
      case -34:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x410) == 0x410) && (bit_count(m) >= 2))
          {
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
          }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
        }
	    printf("Loading -34\n");
	    break;

	    /* ES6ATL2. L1 and L2 are correlated in the sector6 and we have 2 or more tracks  */
      case -35:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x820) == 0x820) && (bit_count(m) >= 2))
          {
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	      }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -35 \n");
	    break;

	    /* ES7ATL2. correlation between bit 7 and 2 or more tracks  */
      case -36:
        for(m=0;m<0x10000;m++)
        {
          if(((m & 0x1000) == 0x1000) && (bit_count(m) >= 2))
          {
            *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	      }
          else if ( ( bit_count(m) > 0 ) && ( ( (*(memory+m)) & L2PASS ) != L2PASS ) )
          {
            *(memory+m) = L2FAIL|0x4000|((m&0x0003f)<<8);
	      }
	      else if ((bit_count(m) == 0 ) && (((*(memory+m)) & L2PASS) != L2PASS ))
            *(memory+m) = (L2FAIL|((m&0x0003f)<<8));
	    }
	    printf("Loading -36 \n");
	    break;





	    /* ES12*/
	  case -37: /*bits 0,1,6,7*/
	    for(m=0;m<0x10000;m++) if((m & 0xc3) == 0xc3) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -37\n");
	    break;

	    /* ES13*/
	  case -38: /*bits 0,2,6,8*/
	    for(m=0;m<=0x10000;m++) if((m & 0x145) == 0x145) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -38\n");
	    break;

	    /* ES14*/
	  case -39:
	    for(m=0;m<=0x10000;m++) if((m & 0x249) == 0x249) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -39\n");
	    break;

	    /* ES15*/
	  case -40:
	    for(m=0;m<=0x10000;m++) if((m & 0x451) == 0x451) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -40\n");
	    break;

	    /* ES16*/
	  case -41:
	    for(m=0;m<=0x10000;m++) if((m & 0x861) == 0x861) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -41\n");
	    break;

	    /* ES23*/
	  case -42:
	    for(m=0;m<=0x10000;m++) if((m & 0x186) == 0x186) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -42\n");
	    break;

	    /* ES24*/
	  case -43:
	    for(m=0;m<=0x10000;m++) if((m & 0x28a) == 0x28a) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -43\n");
	    break;

	    /* ES25*/
	  case -44:
	    for(m=0;m<=0x10000;m++) if((m & 0x492) == 0x492) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -44\n");
	    break;

	    /* ES26*/
	  case -45:
	    for(m=0;m<=0x10000;m++) if((m & 0x8a2) == 0x8a2) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -45\n");
	    break;

	    /* ES34*/
	  case -46:
	    for(m=0;m<=0x10000;m++) if((m & 0x30c) == 0x30c) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -45\n");
	    break;

	    /* ES35*/
	  case -47:
	    for(m=0;m<=0x10000;m++) if((m & 0x514) == 0x514) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -46\n");
	    break;

	    /* ES36*/
	  case -48:
	    for(m=0;m<=0x10000;m++) if((m & 0x924) == 0x924) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -47\n");
	    break;

	    /* ES45*/
	  case -49:
	    for(m=0;m<=0x10000;m++) if((m & 0x618) == 0x618) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -48\n");
	    break;

	    /* ES46*/
	  case -50:
	    for(m=0;m<=0x10000;m++) if((m & 0xa28) == 0xa28) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -49\n");
	    break;

	    /* ES56*/
	  case -51: /*bits 4,5,10,11*/
	    for(m=0;m<=0x10000;m++) if((m & 0xc30) == 0xc30) *(memory+m) = (L2PASS|0xc000|((m&0x3f)<<8));
	    printf("Loading -50\n");
	    break;





	    /* Level1 bit 1 and at least one track  in any sector (BIT1ATL1) */
	  case -52:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x40) == 0x40)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 
	    /* Level1 bit 2 and at least one track  in any sector (BIT2ATL1) */
	  case -53:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x80) == 0x80)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 
	    /* Level1 bit 3 and at least one track  in any sector (BIT3ATL1) */
	  case -54:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x100) == 0x100)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 
	    /* Level1 bit 4 and at least one track  in any sector (BIT4ATL1) */
	  case -55:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x200) == 0x200)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 
	    /* Level1 bit 5 and at least one track  in any sector (BIT5ATL1) */
	  case -56:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x400) == 0x400)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 
	    /* Level1 bit 6 and at least one track  in any sector (BIT6ATL1) */
	  case -57:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x800) == 0x800)
          {
	        if(bit_count(m)>=1) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;
 







	    /* Level1 bit 1 and at least two tracks in any sectors (BIT1ATL2) */
	  case -58:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x40) == 0x40)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;

	    /* Level1 bit 2 and at least two tracks in any sectors (BIT2ATL2) */
	  case -59:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x80) == 0x80)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;

	    /* Level1 bit 3 and at least two tracks in any sectors (BIT3ATL2) */
	  case -60:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x100) == 0x100)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;

	    /* Level1 bit 4 and at least two tracks in any sectors (BIT4ATL2) */
	  case -61:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x200) == 0x200)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;

	    /* Level1 bit 5 and at least two tracks in any sectors (BIT5ATL2) */
	  case -62:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x400) == 0x400)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;

	    /* Level1 bit 6 and at least two tracks in any sectors (BIT6ATL2) */
	  case -63:
	    for(m=0; m<=0x10000; m++)
		{
	      if((m & 0x800) == 0x800)
          {
	        if(bit_count(m)>=2) *(memory+mlu_mem_adr[l]+m) = (L2PASS|0xc000|((m&0x3f)<<8));
		  }
		}
	    break;



	    /* BIT1NOL2 */
      case -64:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x40) == 0x40)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -64 \n");
	    break;

	    /* BIT2NOL2 */
      case -65:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x80) == 0x80)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -65 \n");
	    break;

	    /* BIT3NOL2 */
      case -66:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x100) == 0x100)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -66 \n");
	    break;

	    /* BIT4NOL2 */
      case -67:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x200) == 0x200)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -67 \n");
	    break;

	    /* BIT5NOL2 */
      case -68:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x400) == 0x400)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -68 \n");
	    break;

	    /* BIT6NOL2 */
      case -69:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x800) == 0x800)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -69 \n");
	    break;

	    /* BIT9NOL2 */
      case -70:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x4000) == 0x4000)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -70 \n");
	    break;

	    /* BIT10NOL2 */
      case -71:
        for(m=0;m<0x10000;m++)
        {
	      if(((m & 0x8000) == 0x8000)) *(memory+m) = L2PASS|0xc000|((m&0x0003f)<<8);
	    }
	    printf("Loading -71 \n");
	    break;


      }	
    }
    else if( (mlu_mem_adr[l] == 0) && (mlu_complex[l] == 0) )
	{
      ;
	}
    else
	{
      printf("setup_mlu: unknown combination\n");
      return(3);
	}
  }


/*always accept bit 10 in any combinations*/
printf("setup_mlu: open bit 10\n");
/*
nn=0x8000;
memory[nn] = L2PASS|0x8000;
*/
for(nn=0; nn<0x10000; nn++)
{
  if(nn&0x8000) memory[nn] = L2PASS|0x8000;
}




  /********************************/
  /* rewhite previous by ALL PASS */
  /********************************/
  /*printf("ATTENTION: set L2MLU as ALL PASS\n");
  for(l=0; l<0x10000; l++)
  {
    *(memory + l) = L2PASS; 
	}*/


  /********************************/
  /* rewhite previous by ALL FAIL */
  /********************************/
  /*
  printf("ATTENTION: set L2MLU as ALL FAIL\n");
  for(l=0; l<0x10000; l++)
  {
    *(memory + l) = L2FAIL; 
  }
  */


  /******************************/
  /* Load the memory in the MLU */
  /******************************/

  *mlu_csr = 0;

  /*
  printf("print not-fail L2MLU contents\n");
  for(l=0; l<0x10000; l++)
  {
    if((memory[l]&0xff) != L2FAIL) printf("  will pass at [0x%04x] = 0x%04x\n",l,memory[l]);
  }
  */

  /* load memory */
  printf("Loading the memory \n"); 
  for(l=0; l<0x10000; l++)
  {
    *(mlu + l) = *(memory + l);
    /*printf(" %x=>%x=%x ",l, *(mlu + l), *(memory+l));*/
  }

  /* Read memory and check if correct */
  printf("checking the memory: mlu=0x%08x memory=0x%08x \n",mlu,memory);
  for(l=0; l<0x10000; l++)
  {
    value = *(mlu + l);
    if(value != (*(memory+l)))
    {
      printf("\n Euphrates ERROR: MLU memory write=0x%08x read=0x%08x\n",
        value, *(memory+l));
      free(memory);
      return(2);
	}
  }

  printf("level2 download completed.\n");

  /* set up MLU for front panel access, pulse mode bits 0-3 */
  *mlu_csr = 0x35;

  return(error);
}


#endif
/********* end of the piece from scalers.c ******************/
/************************************************************/
/************************************************************/



#else /* no UNIX version */


void
ts_dummy()
{
  return;
}

#endif


















/* Sergey Boyarinov: new L2 MLU setup routines, similar to Vardan's: they supports
 up to 16 preprogrammable settings controlled by new L1 trigger system; config file
 suppose to have 16 lines corresponding to the 16 combinations of the 4 wires coming
 from Level1 TRG LUT; every line contains logic formula, or PASS, or FAIL */

int
level2MLUCommandDecode(char *code, unsigned short *mlu_address) 
{
  unsigned short bitmask;

  /* setting logic based on single MLU inputs */
  if(!strcmp(code,     "L2S1"))  bitmask = 0x0001;
  else if(!strcmp(code,"L2S2"))  bitmask = 0x0002;
  else if(!strcmp(code,"L2S3"))  bitmask = 0x0004;
  else if(!strcmp(code,"L2S4"))  bitmask = 0x0008;
  else if(!strcmp(code,"L2S5"))  bitmask = 0x0010;
  else if(!strcmp(code,"L2S6"))  bitmask = 0x0020;
  else if(!strcmp(code,"L1S1"))  bitmask = 0x0040;
  else if(!strcmp(code,"L1S2"))  bitmask = 0x0080;
  else if(!strcmp(code,"L1S3"))  bitmask = 0x0100;
  else if(!strcmp(code,"L1S4"))  bitmask = 0x0200;
  else if(!strcmp(code,"L1S5"))  bitmask = 0x0400;
  else if(!strcmp(code,"L1S6"))  bitmask = 0x0800;
  else
  {
    printf("level2MLUCommandDecode ERROR: unknown code >%s< - ignore\n",code);
    return(-1);
  }

  *mlu_address |= bitmask;
  /*
  printf("level2MLUCommandDecode: output 0x%04x\n",*mlu_address);
  */

  return(0);
}

/*
level2MLUSetup("/usr/local/clas/devel/coda/src/rol/code.s/level2test.trg")
level2MLUSetup("/usr/clas/parms/trigger/config/cosmic/level2oppositesectors.trg")
level2MLUSetup("/usr/clas/parms/trigger/config/g9frost/level2_g9frost.trg")
level2MLUSetup("/usr/clas/parms/trigger/config/g9frost/level2_test.trg")
*/
int
level2MLUSetup(char *file_name)
{
  FILE *fp;
  char *ch;

  unsigned short *memory;
  volatile unsigned short *mlu;

  volatile short *mlu_csr;
  volatile unsigned long laddr;
  int error = 0;
  int MAXLINE = 1000;
 
  /* vars for parsing */
  char line[MAXLINE];
  char *xx[MAX_OR_COMBS];
  char *xx_xx[MAX_AND_COMBS];
  volatile unsigned short mlu_mem_adr[MAX_OR_COMBS];
  char *aa, *bb;
  volatile int i,j,k,kk,l,m;
  volatile unsigned short my_address;
  unsigned short value;
  int controlbits, len;

#ifdef VXWORKS
  /* L2 MLU memory address (PPC -A24/D16 transfers) */
  sysBusToLocalAdrs(0x39,L2MLU_MEM_ADDR,&laddr);
  printf("level2MLUSetup: L2 MLU memory local address = 0x%08x\n",(int)laddr);
  mlu = (volatile unsigned short *)laddr;

  /* L2 MLU csr address (PPC -A16/D16 transfers) */
  sysBusToLocalAdrs(0x29,L2MLU_CSR_ADDR,&laddr);
  printf("level2MLUSetup: L2 MLU csr local address = 0x%08x\n",(int)laddr);
  mlu_csr = (short *)laddr;
#endif

  memory = (unsigned short *) malloc(0x20000); /* 64k 16bit words */

  /**********************************/
  /* fill the memory with fail code */

  for(l=0; l<0x10000; l++)
  {
    memory[l] = L2FAIL;
    if((l&0x3f)) memory[l] |= (0x4000|((l&0x3f)<<8));
  }


  
  /* read the level2 trigger file */
  if((fp = fopen(file_name,"r")) == NULL)
  {
    printf("\nlevel2MLUSetup ERROR: Can't open >%s< file\n",file_name);
    return(1);
  }

  controlbits = -1;
  while((ch = fgets(line, MAXLINE, fp)) != NULL)
  {
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      /* init mlu_mem_adr to 0 */
      for(l=0; l<MAX_OR_COMBS; l++) mlu_mem_adr[l] = 0;

      len = strlen(ch);
	  /*
      printf("level2MLUSetup: string len=%d\n",len);
	  */
      controlbits ++;
      if(controlbits>15)
      {
        printf("level2MLUSetup error: only 16 lines are allowed in config file\n");
        break;
	  }

      /* check if config line contains 'pass' or 'fail' */
      if(!strncmp(ch,"PASS",4))
	  {
        for(kk=0; kk<0x1000; kk++)
        {
          value = (controlbits<<12) + kk;
          memory[value] = L2PASS;
        } 
        continue;
	  }
      else if(!strncmp(ch,"FAIL",4))
	  {
        for(kk=0; kk<0x1000; kk++)
        {
          value = (controlbits<<12) + kk;
          memory[value] = L2FAIL;
        } 
        continue;
	  }

      /*********************************************************/
      /*----------- parsing the input parameter line --------- */
      /* will fill array 'mlu_mem_adr', it has dimension MAX_OR_COMBS;
         mlu_mem_adr (short) contains bitmap of desired inputs */

      /* extract OR's */
      i=0;
      aa = strtok(line,"||");
      xx[i] = aa;
      while((aa=strtok(NULL,"||")) != NULL) 
      {
        i++;
        xx[i] = aa; 
      }

      /* loop over OR's */
      for(j=0; j<=i; j++)
      {
		/*
        printf("\nlevel2MLUSetup: OR's ====>%s<==== len=%d\n",xx[j],strlen(xx[j]));
		*/
        /* extract AND's */
        k=0;
        bb = strtok(xx[j],"x");
        xx_xx[k] = bb;
        while((bb=strtok(NULL,"x")) != NULL) 
	    {
	      k++;
	      xx_xx[k] = bb;
	    }

        /* loop over AND's */
        my_address=0;
        for(kk=0; kk<=k; kk++)
        {
		  /*
          printf("level2MLUSetup: AND ====>%s<==== len=%d\n",xx_xx[kk],strlen(xx_xx[kk]));
		  */
          /* accumulate input bitmask in 'my_address' for every AND'ed element */
          len = strlen(xx_xx[kk]);
          if(len>1)
		  { 
            level2MLUCommandDecode(xx_xx[kk],&my_address);
			/*
            printf("level2MLUSetup: output of the set_bit_function 0x%04x\n",my_address);
			*/
            mlu_mem_adr[j] = my_address;
			/*
            printf("\n   level2MLUSetup: OR number [%3d] produces address [0x%04x]\n\n",
              j,mlu_mem_adr[j]);
			*/
		  }

        }
      }

      /*------------ end of parsing algoritm -----------*/





      /* MLU signals description:
         inputs:
               1-6 sector based level2
	           7-12 - sector based l1
	           13-16 - control bits
         outputs:
               0,2 - FAIL (0 goes to TS fail, 2 not connected)
               1,3 - PASS (1 goes to TS pass, 3 not connected)

              ???????????????

	           8-13 sector based level2
               14 -> OR of level2
               15 -> pass based on input patern and programmed logic
             (take the first 6 bit of the address (sector based level2), 
             shift it left 8 bits and or it with the MLU output code)
      */


      /*********************************************/
      /* update the memory with the defined passes */
      printf("\nlevel2MLUSetup: updating memory array (controlbits=0x%02x):\n",controlbits);
      for(l=0; l<MAX_OR_COMBS; l++)
      {
        if(mlu_mem_adr[l] > 0) /*must be always>0 ???*/
	    {
          /*fill all addresses containing 'value' combination*/
          for(kk=0; kk<0x1000; kk++)
          {
            if( (mlu_mem_adr[l]&kk) == mlu_mem_adr[l])
			{
              value = (controlbits<<12) + kk;
              memory[value] =  L2PASS|0x8000;
              if((value&0x3f)) memory[value] |= (0x4000|((value&0x3f)<<8)); 
#ifndef VXWORKS
              printf("   [0x%04x] passed (0x%04x) !!!\n",value,kk);
#endif
			}
            else
			{
			  ;
#ifndef VXWORKS
              value = (controlbits<<12) + kk;
              printf("   [0x%04x] failed (0x%04x)\n",value,kk);
#endif
			}
          }
	    }
      }
      printf("\n");



	}
  }

  fclose(fp);





  /********************************/
  /* rewhite previous by ALL PASS */
  /********************************/
  /*  
  printf("level2MLUSetup: ATTENTION: set L2MLU as ALL PASS\n");
  for(l=0; l<0x10000; l++)
  {
    memory[l] = L2PASS; 
  }
  */

  /********************************/
  /* rewhite previous by ALL FAIL */
  /********************************/
  /*  
  printf("level2MLUSetup: ATTENTION: set L2MLU as ALL FAIL\n");
  for(l=0; l<0x10000; l++)
  {
    memory[l] = L2FAIL; 
  }
  */

  /*
#ifndef VXWORKS
  printf("\nlevel2MLUSetup: (partial) print of the memory array:\n");
  for(l=0; l<0x50; l++)
  {
    printf("memory[0x%04x] = 0x%04x\n",l,memory[l]);
  }
  printf("\n");
#endif
  */

#ifdef VXWORKS
  /******************************/
  /* Load the memory in the MLU */
  /******************************/

  *mlu_csr = 0;
  printf("\nlevel2MLUSetup: Loading the memory \n"); 

  /* load memory */
  for(l=0; l<0x10000; l++)
  {
    *(mlu + l) = memory[l];
    /*printf(" %x=>%x=%x ",l, *(mlu + l), memory[l]);*/
  }

  /* Read memory and check if correct */
  printf("level2MLUSetup: checking the memory: mlu=0x%08x memory=0x%08x \n",mlu,memory);
  for(l=0; l<0x10000; l++)
  {
    value = *(mlu + l);
    if(value != memory[l])
    {
      printf("\nlevel2MLUSetup ERROR: MLU memory write=0x%08x read=0x%08x\n",
        value, memory[l]);
      free(memory);
      return(2);
	}
    /*else
	{
      if(l<0x50) printf("memory[0x%04x] = 0x%04x\n",l,memory[l]);
	}*/
  }

  printf("level2MLUSetup: level2 download completed.\n");

  /* set up MLU for front panel access, pulse mode bits 0-3 */
  *mlu_csr = 0x35;
#endif

  return(error);
}



/* Sergey: generates Level1 lookup tables and writes them to the file */

int
level1LUTCommandDecode(char *code, unsigned short *mlu_address) 
{
  unsigned short bitmask;

  /* setting logic based on single MLU inputs */

  if     (!strcmp(code,"ECP1"))  bitmask = 0x0001;
  else if(!strcmp(code,"ECP2"))  bitmask = 0x0002;
  else if(!strcmp(code,"ECP3"))  bitmask = 0x0004;
  else if(!strcmp(code,"ECP4"))  bitmask = 0x0008;
  else if(!strcmp(code,"ECP5"))  bitmask = 0x0010;
  else if(!strcmp(code,"ECP6"))  bitmask = 0x0020;

  else if(!strcmp(code,"ECC1"))  bitmask = 0x0001;
  else if(!strcmp(code,"ECC2"))  bitmask = 0x0002;
  else if(!strcmp(code,"ECC3"))  bitmask = 0x0004;
  else if(!strcmp(code,"ECC4"))  bitmask = 0x0008;
  else if(!strcmp(code,"ECC5"))  bitmask = 0x0010;
  else if(!strcmp(code,"ECC6"))  bitmask = 0x0020;

  else if(!strcmp(code,"STOF1")) bitmask = 0x0040;
  else if(!strcmp(code,"STOF2")) bitmask = 0x0080;
  else if(!strcmp(code,"STOF3")) bitmask = 0x0100;
  else if(!strcmp(code,"STOF4")) bitmask = 0x0200;
  else if(!strcmp(code,"STOF5")) bitmask = 0x0400;
  else if(!strcmp(code,"STOF6")) bitmask = 0x0800;

  /* version nov 2010, will still use ECP1-6 from previous */
  else if(!strcmp(code,"ECPCC"))  bitmask = 0x0001;
  else if(!strcmp(code,"ECECC"))  bitmask = 0x0001;
  else if(!strcmp(code,"STOFP1")) bitmask = 0x0002;
  else if(!strcmp(code,"STOFP2")) bitmask = 0x0004;
  else if(!strcmp(code,"STOFP3")) bitmask = 0x0008;

  else if(!strcmp(code,"ECE1")) bitmask = 0x0040;
  else if(!strcmp(code,"ECE2")) bitmask = 0x0080;
  else if(!strcmp(code,"ECE3")) bitmask = 0x0100;
  else if(!strcmp(code,"ECE4")) bitmask = 0x0200;
  else if(!strcmp(code,"ECE5")) bitmask = 0x0400;
  else if(!strcmp(code,"ECE6")) bitmask = 0x0800;

  else
  {
    printf("level1LUTCommandDecode ERROR: unknown code >%s< - ignore\n",code);
    return(-1);
  }

  *mlu_address |= bitmask;
  /*
  printf("level1LUTCommandDecode: output 0x%04x\n",*mlu_address);
  */

  return(0);
}

/*
to generate LUTs use 'level1lookupmaker <filename>'
*/
int
level1LUTSetup(char *file_name, char *fout_name)
{
  FILE *fp, *fout;
  char *ch;

  unsigned char *memory;
  volatile unsigned short *mlu;

  volatile short *mlu_csr;
  volatile unsigned long laddr;
  int error = 0;
  int MAXLINE = 1000;
 
  /* vars for parsing */
  char line[MAXLINE];
  char *xx[MAX_OR_COMBS];
  char *xx_xx[MAX_AND_COMBS];
  volatile unsigned short mlu_mem_adr[MAX_OR_COMBS];
  char *aa, *bb;
  volatile int i,j,k,kk,l,m;
  volatile unsigned short my_address;
  unsigned short value;
  int len;

  memory = (unsigned short *) malloc(0x1000); /* we have 12-bit address */

  /* read the level1 trigger file */
  if((fp = fopen(file_name,"r")) == NULL)
  {
    printf("\nlevel1LUTSetup ERROR: Can't open >%s< file\n",file_name);
    return(1);
  }

  while((ch = fgets(line, MAXLINE, fp)) != NULL)
  {
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      /* fill the memory with fail code */
      for(l=0; l<0x1000; l++)
      {
        memory[l] = L1FAIL;
      }
      /* init mlu_mem_adr to 0 */
      for(l=0; l<MAX_OR_COMBS; l++) mlu_mem_adr[l] = 0;

      len = strlen(ch);
	  /*
      printf("level1LUTSetup: string len=%d\n",len);
	  */

      /*********************************************************/
      /*----------- parsing the input parameter line --------- */
      /* will fill array 'mlu_mem_adr', it has dimension MAX_OR_COMBS;
         mlu_mem_adr (short) contains bitmap of desired inputs */

      /* extract OR's */
      i=0;
      aa = strtok(line,"||");
      xx[i] = aa;
      while((aa=strtok(NULL,"||")) != NULL) 
      {
        i++;
        xx[i] = aa; 
      }

      /* loop over OR's */
      for(j=0; j<=i; j++)
      {
		/*
        printf("\nlevel1LUTSetup: OR's ====>%s<==== len=%d\n",xx[j],strlen(xx[j]));
		*/
        /* extract AND's */
        k=0;
        bb = strtok(xx[j],"x");
        xx_xx[k] = bb;
        while((bb=strtok(NULL,"x")) != NULL) 
	    {
	      k++;
	      xx_xx[k] = bb;
	    }

        /* loop over AND's */
        my_address=0;
        for(kk=0; kk<=k; kk++)
        {
		  /*
          printf("level1LUTSetup: AND ====>%s<==== len=%d\n",xx_xx[kk],strlen(xx_xx[kk]));
		  */
          /* accumulate input bitmask in 'my_address' for every AND'ed element */
          len = strlen(xx_xx[kk]);
          if(len>1)
		  { 
            level1LUTCommandDecode(xx_xx[kk],&my_address);
			/*
            printf("level1LUTSetup: output of the set_bit_function 0x%04x\n",my_address);
			*/
            mlu_mem_adr[j] = my_address;
			/*
            printf("\n   level1LUTSetup: OR number [%3d] produces address [0x%04x]\n\n",
              j,mlu_mem_adr[j]);
			*/
		  }

        }
      }

      /*------------ end of parsing algoritm -----------*/



      /*********************************************/
      /* update the memory with the defined passes */
      printf("\nlevel1LUTSetup: updating memory array:\n");
      for(l=0; l<MAX_OR_COMBS; l++)
      {
        if(mlu_mem_adr[l] > 0) /*must be always>0 ???*/
	    {
          /*fill all addresses containing 'value' combination*/
          for(kk=0; kk<0x1000; kk++)
          {
            if( (mlu_mem_adr[l]&kk) == mlu_mem_adr[l])
			{
              value = kk;
              memory[value] =  L1PASS; 
			  /*
              printf("   [0x%04x] passed (0x%04x) !!!\n",value,kk);
			  */
			}
            else
			{
			  ;
			  /*
              value = kk;
              printf("   [0x%04x] failed (0x%04x)\n",value,kk);
			  */
			}
          }
	    }
      }
      printf("\n");



	}
  }

  fclose(fp);



  /* write results to the file */
  
  if((fout = fopen(fout_name,"w")) == NULL)
  {
    printf("\nlevel1LUTSetup ERROR: Can't open >%s< file\n",fout_name);
    return(1);
  }

  for(l=0; l<0x1000; l++)
  {
    fprintf(fout,"%1d\t#%04X\n",memory[l],l);
  }

  fclose(fout);


  printf("level1LUTSetup: level1 LUT calculated.\n");

  return(error);
}



int
level1LUTCommandDecodeNew(char *code, unsigned short *mlu_address) 
{
  unsigned short bitmask;

  /* setting logic based on single MLU inputs */

  if     (!strncmp(code,"ECPCC",5))  bitmask = 0x0001;
  else if(!strncmp(code,"ECECC",5))  bitmask = 0x0001;
  else if(!strncmp(code,"STOFP1",6)) bitmask = 0x0002;
  else if(!strncmp(code,"STOFP2",6)) bitmask = 0x0004;
  else if(!strncmp(code,"STOFP3",6)) bitmask = 0x0008;


  else if(!strncmp(code,"ECP1",4))  bitmask = 0x0001;
  else if(!strncmp(code,"ECP2",4))  bitmask = 0x0002;
  else if(!strncmp(code,"ECP3",4))  bitmask = 0x0004;
  else if(!strncmp(code,"ECP4",4))  bitmask = 0x0008;
  else if(!strncmp(code,"ECP5",4))  bitmask = 0x0010;
  else if(!strncmp(code,"ECP6",4))  bitmask = 0x0020;

  else if(!strncmp(code,"ECE1",4))  bitmask = 0x0040;
  else if(!strncmp(code,"ECE2",4))  bitmask = 0x0080;
  else if(!strncmp(code,"ECE3",4))  bitmask = 0x0100;
  else if(!strncmp(code,"ECE4",4))  bitmask = 0x0200;
  else if(!strncmp(code,"ECE5",4))  bitmask = 0x0400;
  else if(!strncmp(code,"ECE6",4))  bitmask = 0x0800;

  else
  {
    printf("level1LUTCommandDecodeNew ERROR: unknown code >%s< - ignore\n",code);
    return(-1);
  }

  *mlu_address = bitmask;
  /*
  printf("level1LUTCommandDecodeNew: output 0x%04x\n",*mlu_address);
  */

  return(0);
}

/*
to generate LUTs use 'level1lookupmakernew <filename>'
*/
int
level1LUTSetupNew(char *file_name)
{
  FILE *fp, *fout;
  char *ch;
  char fout_name[256];
  int lutnum; /*from 0 to 12*/
  char lutnam[20];

  unsigned char *memory;
  volatile unsigned short *mlu;

  volatile short *mlu_csr;
  volatile unsigned long laddr;
  int error = 0;
  int MAXLINE = 1000;
 
  /* vars for parsing */
  char line[MAXLINE], str[MAXLINE];
  char *xx[MAX_OR_COMBS];
  char *xx_xx[MAX_AND_COMBS];
  volatile unsigned short mlu_mem_adr[MAX_OR_COMBS];
  char *aa, *bb;
  volatile int i,j,k,kk,l,m;
  volatile unsigned short my_address;
  unsigned short value;
  int len;

  memory = (unsigned short *) malloc(0x1000); /* we have 12-bit address */

  /* read the level1 trigger file */
  if((fp = fopen(file_name,"r")) == NULL)
  {
    printf("\nlevel1LUTSetup ERROR: Can't open >%s< file\n",file_name);
    return(1);
  }

  while((ch = fgets(line, MAXLINE, fp)) != NULL)
  {
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else /*process one line*/
    {
      /* fill the memory with fail code */
      for(l=0; l<0x1000; l++) memory[l] = L1FAIL;

      /* init mlu_mem_adr to 0 */
      for(l=0; l<MAX_OR_COMBS; l++) mlu_mem_adr[l] = 0;

      len = strlen(ch);
	  strcpy(str,ch); /*save original string*/
      printf("level1LUTSetup: string len=%d key >%10.10s<\n",len, line);

      if(     !strncmp(ch,"TRIG1ECP1:",10)) {lutnum = 1;  strcpy(lutnam,"_1");}
      else if(!strncmp(ch,"TRIG1ECP2:",10)) {lutnum = 2;  strcpy(lutnam,"_2");}
      else if(!strncmp(ch,"TRIG1ECP3:",10)) {lutnum = 3;  strcpy(lutnam,"_3");}
      else if(!strncmp(ch,"TRIG1ECP4:",10)) {lutnum = 4;  strcpy(lutnam,"_4");}
      else if(!strncmp(ch,"TRIG1ECP5:",10)) {lutnum = 5;  strcpy(lutnam,"_5");}
      else if(!strncmp(ch,"TRIG1ECP6:",10)) {lutnum = 6;  strcpy(lutnam,"_6");}
      else if(!strncmp(ch,"TRIG1ECE1:",10)) {lutnum = 7;  strcpy(lutnam,"_7");}
      else if(!strncmp(ch,"TRIG1ECE2:",10)) {lutnum = 8;  strcpy(lutnam,"_8");}
      else if(!strncmp(ch,"TRIG1ECE3:",10)) {lutnum = 9;  strcpy(lutnam,"_9");}
      else if(!strncmp(ch,"TRIG1ECE4:",10)) {lutnum = 10; strcpy(lutnam,"_10");}
      else if(!strncmp(ch,"TRIG1ECE5:",10)) {lutnum = 11; strcpy(lutnam,"_11");}
      else if(!strncmp(ch,"TRIG1ECE6:",10)) {lutnum = 12; strcpy(lutnam,"_12");}
      else if(!strncmp(ch,"TRIG1TRIG:",10)) {lutnum = 0;  strcpy(lutnam,"_0");}
      else
	  {
        printf("level1LUTSetup: ERROR: UNKNOWN FLAG >%5.5s<\n",ch);
        return(1);
	  }

      /*********************************************************/
      /*----------- parsing the input parameter line --------- */
      /* will fill array 'mlu_mem_adr', it has dimension MAX_OR_COMBS;
         mlu_mem_adr (short) contains bitmap of desired inputs */

      /* extract OR's */
      i=0;
      aa = strtok((char *)&line[11],"||");
      xx[i] = aa;
      while((aa=strtok(NULL,"||")) != NULL) 
      {
        i++;
        xx[i] = aa; 
      }

      /* loop over OR's */
      for(j=0; j<=i; j++)
      {
		/*
        printf("\nlevel1LUTSetup: OR's ====>%s<==== len=%d\n",xx[j],strlen(xx[j]));
		*/
        /* extract AND's */
        k=0;
        bb = strtok(xx[j],"x");
        xx_xx[k] = bb;
        while((bb=strtok(NULL,"x")) != NULL) 
	    {
	      k++;
	      xx_xx[k] = bb;
	    }

        /* loop over AND's */
        for(kk=0; kk<=k; kk++)
        {
		  /*
          printf("level1LUTSetup: AND ====>%s<==== len=%d\n",xx_xx[kk],strlen(xx_xx[kk]));
		  */
          /* accumulate input bitmask in 'my_address' for every AND'ed element */
          len = strlen(xx_xx[kk]);
          if(len>1)
		  { 
            my_address=0;
            level1LUTCommandDecodeNew(xx_xx[kk],&my_address);
			
            printf("level1LUTSetup: output of the set_bit_function 0x%04x\n",my_address);
			
            mlu_mem_adr[j] |= my_address;
			
            printf("\n   level1LUTSetup: OR number [%3d] produces address [0x%04x]\n\n",
              j,mlu_mem_adr[j]);
			
		  }

        }
      }

      /*------------ end of parsing algoritm -----------*/



      /*********************************************/
      /* update the memory with the defined passes */
      printf("\nlevel1LUTSetup: updating memory array:\n");
      for(l=0; l<MAX_OR_COMBS; l++)
      {
        if(mlu_mem_adr[l] > 0) /*must be always>0 ???*/
	    {
          /*fill all addresses containing 'value' combination*/
          if(lutnum==0) len = 0x1000;
          else          len = 0x10;
          for(kk=0; kk<len; kk++)
          {
            if( (mlu_mem_adr[l]&kk) == mlu_mem_adr[l])
			{
              memory[kk] =  L1PASS;
              printf("   [0x%04x] passed !!!\n",kk);
			  
			}
            else
			{
			  ;
              printf("   [0x%04x] failed \n",kk);
			  
			}
          }
	    }
      }
      printf("\n");




      /* write results to the file */
      len = strlen(file_name) - 4;
      strncpy(fout_name,file_name,len);
      fout_name[len] = '\0';
      strcat(fout_name,lutnam);
      strcat(fout_name,".lut");
      printf("-->%s<--\n",fout_name);
	  
      if((fout = fopen(fout_name,"w")) == NULL)
      {
        printf("\nlevel1LUTSetup ERROR: Can't open >%s< file\n",fout_name);
        return(1);
      }
      fprintf(fout,"# Nov 2010 LUT file >%s<\n",fout_name);
      fprintf(fout,"# Logic %s",str);
      if(lutnum==0) len = 0x1000;
      else          len = 0x10;
      for(l=0; l<len; l++)
      {
        fprintf(fout,"%1d\t#%04X\n",memory[l],l);
      }
      fclose(fout);
	  


	}
  }

  fclose(fp);




  printf("level1LUTSetup: level1 LUT calculated.\n");

  return(error);
}



/* Sergey: generates Level2 lookup tables and writes them to the file */

int
level2LUTCommandDecode(char *code, unsigned short *mlu_address) 
{
  unsigned short bitmask;

  if     (!strcmp(code,"TRG1"))  bitmask = 0x0001;
  else if(!strcmp(code,"TRG2"))  bitmask = 0x0002;
  else if(!strcmp(code,"TRG3"))  bitmask = 0x0004;
  else if(!strcmp(code,"TRG4"))  bitmask = 0x0008;
  else if(!strcmp(code,"TRG5"))  bitmask = 0x0010;
  else if(!strcmp(code,"TRG6"))  bitmask = 0x0020;

  else if(!strcmp(code,"TRG7"))  bitmask = 0x0040;
  else if(!strcmp(code,"TRG8"))  bitmask = 0x0080;
  else if(!strcmp(code,"TRG9"))  bitmask = 0x0100;
  else if(!strcmp(code,"TRG10")) bitmask = 0x0200;
  else if(!strcmp(code,"TRG11")) bitmask = 0x0400;
  else if(!strcmp(code,"TRG12")) bitmask = 0x0800;

  else
  {
    printf("level2LUTCommandDecode ERROR: unknown code >%s< - ignore\n",code);
    return(-1);
  }

  *mlu_address |= bitmask;
  /*
  printf("level2LUTCommandDecode: output 0x%04x\n",*mlu_address);
  */

  return(0);
}


/*
to generate LUTs use 'level2lookupmaker <filename>'
   we have 4 output bits instead of 1, so 4 lines in config file
   will be converted into 4 possible combinations of 4 outputs
*/
#define UNUSED_TRIG_BITS_MASK 0x780
int
level2LUTSetup(char *file_name, char *fout_name)
{
  FILE *fp, *fout;
  char *ch;

  unsigned char *memory;
  volatile unsigned short *mlu;

  volatile short *mlu_csr;
  volatile unsigned long laddr;
  int error = 0;
  int MAXLINE = 1000;
 
  /* vars for parsing */
  char line[MAXLINE];
  char *xx[MAX_OR_COMBS];
  char *xx_xx[MAX_AND_COMBS];
  volatile unsigned short mlu_mem_adr[MAX_OR_COMBS];
  char *aa, *bb;
  volatile int i,j,k,kk,l,m;
  volatile unsigned short my_address;
  unsigned short value, ttt;
  int controlbits, len;

  memory = (unsigned short *) malloc(0x1000); /* we have 12-bit address */

  /**********************************/
  /* fill the memory with fail code */

  for(l=0; l<0x1000; l++)
  {
    memory[l] = 0;
  }


  
  /* read the level2 trigger file */
  if((fp = fopen(file_name,"r")) == NULL)
  {
    printf("\nlevel2LUTSetup ERROR: Can't open >%s< file\n",file_name);
    return(1);
  }

  controlbits = 1; /*DIFFERENT FROM level1LUT !!!*/
  while((ch = fgets(line, MAXLINE, fp)) != NULL)
  {
    if( ch[0] == '#' || ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\r' )
    {
      ;
    }
    else
    {
      /* init mlu_mem_adr to 0 */
      for(l=0; l<MAX_OR_COMBS; l++) mlu_mem_adr[l] = 0;

      len = strlen(ch);
	  /*
      printf("level2LUTSetup: string len=%d\n",len);
	  */
      if(controlbits>8) /*DIFFERENT FROM level1LUT !!!*/
      {
        printf("level2LUTSetup error: only 4 lines are allowed in config file\n");
        break;
	  }

      /* check if config line contains 'default' */
      if(!strncmp(ch,"DEFAULT",7))
	  {
        controlbits = controlbits * 2; /*DIFFERENT FROM level1LUT !!!*/
        printf("level2LUTSetup: 'DEFAULT' found in config file\n");
        continue;
	  }

      /*********************************************************/
      /*----------- parsing the input parameter line --------- */
      /* will fill array 'mlu_mem_adr', it has dimension MAX_OR_COMBS;
         mlu_mem_adr (short) contains bitmap of desired inputs */

      /* extract OR's */
      i=0;
      aa = strtok(line,"||");
      xx[i] = aa;
      while((aa=strtok(NULL,"||")) != NULL) 
      {
        i++;
        xx[i] = aa; 
      }

      /* loop over OR's */
      for(j=0; j<=i; j++)
      {
		/*
        printf("\nlevel2LUTSetup: OR's ====>%s<==== len=%d\n",xx[j],strlen(xx[j]));
		*/
        /* extract AND's */
        k=0;
        bb = strtok(xx[j],"x");
        xx_xx[k] = bb;
        while((bb=strtok(NULL,"x")) != NULL) 
	    {
	      k++;
	      xx_xx[k] = bb;
	    }

        /* loop over AND's */
        my_address=0;
        for(kk=0; kk<=k; kk++)
        {
		  /*
          printf("level2LUTSetup: AND ====>%s<==== len=%d\n",xx_xx[kk],strlen(xx_xx[kk]));
		  */
          /* accumulate input bitmask in 'my_address' for every AND'ed element */
          len = strlen(xx_xx[kk]);
          if(len>1)
		  { 
            level2LUTCommandDecode(xx_xx[kk],&my_address);
			
            printf("\n   level2LUTSetup: output of the set_bit_function 0x%04x\n",my_address);
			
            mlu_mem_adr[j] = my_address;
			
            printf("\n   level2LUTSetup: OR number [%3d] produces address [0x%04x]\n\n",
              j,mlu_mem_adr[j]);
			
		  }

        }
      }

      /*------------ end of parsing algoritm -----------*/



      /*********************************************/
      /* update the memory with the defined passes */
      printf("\nlevel2LUTSetup: updating memory array:\n");
      for(l=0; l<MAX_OR_COMBS; l++)
      {
        if(mlu_mem_adr[l] > 0) /*must be always>0 ???*/
	    {
          /*fill all addresses containing 'value' combination*/
          for(kk=0; kk<0x1000; kk++)
          {

			/*NOTE1: specified bit must be THE ONLY one, all other combinations with that bit will not be marked*/

            /*NOTE2: because of bug in firmware unused trigger bits gives unspecified inputs, so we have to
			  work it out by allowing all combinations with those bits; for TPE run - bits 9,10,11*/

			printf("[0x%03x] -> l=%d mlu_mem_adr[l]=0x%03x (0x%03x 0x%03x)\n",kk,l,mlu_mem_adr[l],(0xFFF^UNUSED_TRIG_BITS_MASK),kk&(0xFFF^UNUSED_TRIG_BITS_MASK));

            /*if( (mlu_mem_adr[l]&kk) == mlu_mem_adr[l])*//*DIFFERENT FROM level1LUT !!!*/
            /*if(kk == mlu_mem_adr[l])*/
			if( (kk&mlu_mem_adr[l]) /*&& (!(kk&(0xFFF^UNUSED_TRIG_BITS_MASK)))*/ )
			{
              value = kk;
              memory[value] |= controlbits; /*DIFFERENT FROM level1LUT !!!*/
			  			  
              printf("   [0x%04x] passed as 0x%x, memory has 0x%x now\n",value,controlbits,memory[value]);
			  
			}
            else
			{
			  ;
			  /*
              value = kk;
              printf("   [0x%04x] failed (0x%04x)\n",value,kk);
			  */
			}
          }
	    }
      }
      printf("\n");

      controlbits = controlbits * 2; /*DIFFERENT FROM level1LUT !!!*/

    } /* end of single line in config file */


  } /* end of config file */

  fclose(fp);



  /* write results to the file */
  
  if((fout = fopen(fout_name,"w")) == NULL)
  {
    printf("\nlevel2LUTSetup ERROR: Can't open >%s< file\n",fout_name);
    return(1);
  }

  for(l=0; l<0x1000; l++)
  {
    fprintf(fout,"%1x\t#%04X\n",memory[l],l); /*DIFFERENT FROM level1LUT !!!*/
  }

  fclose(fout);


  printf("level2LUTSetup: level2 LUT calculated.\n");

  return(error);
}

