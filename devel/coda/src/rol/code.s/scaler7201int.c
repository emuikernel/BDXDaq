/*
 *  scaler7201int.c -  Struck VME scaler board interrupt handler
 *
 * to do: MYINT_VECTOR, MYINT_LEVEL, SCALER0, SCALER1, ...,
 *        NBUFHLS, ring buffers - all MUST come from outside !!!
 */

#ifdef VXWORKS

#include "vxWorks.h"
#include "stdio.h"
#include "intLib.h"
#include "iv.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"


#include "scaler7201.h"
#include "ttutils.h"



IMPORT STATUS sysIntEnable(int);
IMPORT STATUS sysIntDisable(int);
#ifdef VXWORKSPPC
IMPORT int intDisconnect(int);
IMPORT int sysBusIntAck(int);
#endif
IMPORT STATUS sysClkRateGet();


/* enable scaler interrupts

 This routine takes an interrupt source mask and enables interrupts
 fo the initialized scaler.
     mask:    bit 0  IRQ on start of CIP     
    (1-15)    bit 1  IRQ on FIFO full
              bit 2  IRO on FIFO half full
              bit 3  IRO on FIFO almost full
*/

STATUS
scaler7201intenable(int addr, unsigned int mask, int level, int vector)
{
  volatile unsigned int *csr, *irq;

  csr = (unsigned int *) addr;
  irq = csr + 1;
  logMsg("scaler7201intenable reached, csr = %08x   irq = %08x\n",(int)csr,(int)irq,3,4,5,6);

  if((mask==0) || (mask > 0xf))
  {
    logMsg("VME Interrupt mask=0x%x is out of range\n",mask,2,3,4,5,6);
    return(-1);
  }
  else
  {
    /* Program scaler to generate interrupts */
    *csr = DISABLE_IRQ;   /* disable all interrupt sources */
    logMsg("scaler7201intenable: seting interrupt: mask = %d\n",mask,2,3,4,5,6);
    if(mask & 1) logMsg("scaler7201intenable: seting interrupt: IRQ on start of CIP     enabled\n",
                             1,2,3,4,5,6);
    if(mask & 2) logMsg("scaler7201intenable: seting interrupt: IRQ on FIFO full        enabled\n",
                             1,2,3,4,5,6);
    if(mask & 4) logMsg("scaler7201intenable: seting interrupt: IRQ on FIFO half full   enabled\n",
                             1,2,3,4,5,6);
    if(mask & 8) logMsg("scaler7201intenable: seting interrupt: IRQ on FIFO almost full enabled\n",
                             1,2,3,4,5,6);
    logMsg("scaler7201intenable: seting interrupt: level = %d   vector = %x\n",
                             level,vector,3,4,5,6);
    *irq = (0x800) | (level<<8) | vector;
    *csr = (mask<<20);
    logMsg("scaler7201intenable: set *irq = %08x\n",(int)(*irq),2,3,4,5,6);
  }

  logMsg("scaler7201intenable done, *csr = %08x, *irq = %08x\n",(int)(*csr),(int)(*irq),3,4,5,6);
  return(0);
}

/* disable the scaler interrupts

 RETURNS: OK, or ERROR if not initialized
*/

STATUS
scaler7201intdisable(int addr)
{
  volatile unsigned int *csr;

  csr = (unsigned int *) addr;
  logMsg("scaler7201intdisable reached, csr = %08x\n",(int)csr,2,3,4,5,6);

  *csr = DISABLE_IRQ;
      
  logMsg("scaler7201intdisable done\n",1,2,3,4,5,6);
  return(0);
}

STATUS
scaler7201intSave(int addr, unsigned int *mask0)
{
  volatile unsigned int *csr;

  csr = (unsigned int *) addr;
  /*logMsg("scaler7201intSave reached, csr = %08x\n",(int)csr,2,3,4,5,6);*/

  *mask0 = (*csr) & ENABLE_IRQ;    /* save current IRQ mask */
  *csr = DISABLE_IRQ;            /* disable all interrupt sources */

  return(0);
}

STATUS
scaler7201intRestore(int addr, unsigned int mask0)
{
  volatile unsigned int *csr;

  csr = (unsigned int *) addr;
  /*logMsg("scaler7201intRestore reached, csr = %08x\n",(int)csr,2,3,4,5,6);*/

#ifdef VXWORKSPPC
  /*sysBusIntAck(myint_level);*/ /* from Dave for older Radstone PPC boards */
#endif

  *csr = mask0;   /* restore IRQ mask */

  return(0);
}







/****************/
/*              */
/*   EXAMPLES   */
/*              */
/****************/

#define SCALER0 0xfa508000
#define SCALER1 0xfa303000
#define MASK    0xffff0000   /* unmask 16 lowest channels */
#define NCHN            16
#define NBUFHLS      72000

/* interrupt vector */
#define MYINT_VECTOR 0xf0
/* interrupt level (TI board has level 5) */
#define MYINT_LEVEL 6

static int *ring0, *ring1;

void
scaler7201inthandler0(int addr)
{
  logMsg("scaler7201inthandler reached, addr=0x%08x\n",(int)addr,2,3,4,5,6);

  /* read SCALER0 and SCALER1 
  if(scaler7201status(addr) & FIFO_FULL)
  {
    scaler7201restore(addr, MASK);
    scaler7201control(addr, ENABLE_EXT_NEXT);
  }
  else
  {
    scaler7201readHLS(addr, ring0, 0x11);
  }

  if(scaler7201status(SCALER1) & FIFO_FULL)
  {
    scaler7201restore(SCALER1, MASK);
    scaler7201control(addr, ENABLE_EXT_NEXT);
  }
  else
  {
    scaler7201readHLS(SCALER1, ring1, 0x22);
  }
  */

  return;
}

void
scaler7201inthandler(void)
{
  unsigned int mask0;

  /*sysIntDisable(myint_level);*/
  /*logMsg("scaler7201inthandler: reached\n",0,0,0,0,0,0);*/

  scaler7201intSave(SCALER0, &mask0);
  scaler7201inthandler0(SCALER0);
  scaler7201intRestore(SCALER0, mask0);

  /*sysIntEnable(myint_level);*/
  return;
}

void
ttt0()
{
  int i;

  scaler7201reset(SCALER0);
  scaler7201clear(SCALER0);
  
  scaler7201mask(SCALER0, MASK);
  scaler7201enablenextlogic(SCALER0);
  scaler7201control(SCALER0, ENABLE_MODE2);
  scaler7201control(SCALER0, ENABLE_EXT_DIS);
  scaler7201control(SCALER0, ENABLE_EXT_NEXT);

  ring0 = utRingCreate(NBUFHLS);
  ring1 = utRingCreate(NBUFHLS);

  utIntInit((VOIDFUNCPTR)scaler7201inthandler,0,MYINT_LEVEL,MYINT_VECTOR);
  scaler7201intenable(SCALER0, 0x1, MYINT_LEVEL,MYINT_VECTOR);
  /*scaler7201intenable(SCALER0, 0x4, MYINT_LEVEL,MYINT_VECTOR);*/

  /* SIS3801: 0x1 - many, 0x2 - one, 0x4 - none, 0x8 - working !!! */
  /*scaler7201intenable(SCALER0, 0x8, MYINT_LEVEL,MYINT_VECTOR);*/

  /*
  for(i=0; i<10000000000; i++) i=i;
  scaler7201intdisable(SCALER0);
  return;
  */

  logMsg("scaler7201intdrv0(): 1\n",1,2,3,4,5,6);

  for(i=0; i<10; i++)
  {
    utRingInit(ring0);
    taskDelay (sysClkRateGet());
  }

  scaler7201intdisable(SCALER0);
  /*
  scaler7201reset(SCALER0);
  scaler7201clear(SCALER0);
  */

  logMsg("scaler7201intdrv0(): done\n",1,2,3,4,5,6);

  utRingStatusPrint(ring0);

  return;
}


void
rrr0()
{
  unsigned int len, tmp, status;

  /*logMsg("rrr0: SCALER0=0x%08x\n",SCALER0,2,3,4,5,6);*/
  status = scaler7201status(SCALER0);
  /*logMsg("rrr0: status=0x%08x\n",status,2,3,4,5,6);*/
  len=0;

  while( !(scaler7201status(SCALER0) & FIFO_EMPTY))
  {
    tmp = scaler7201readfifo(SCALER0);
    len++;
    if(!(len%12000))
	{
      /*logMsg("rrr0(): loop ... tmp=0x%08x len=%d\n",tmp,len,3,4,5,6);*/
	}
  }

  /*logMsg("rrr0(): len=%u\n",len,2,3,4,5,6);*/

  return;
}


void
ttt1()
{
  int i;

  scaler7201reset(SCALER0);
  scaler7201clear(SCALER0);
  
  scaler7201mask(SCALER0, MASK);
  scaler7201enablenextlogic(SCALER0);
  /*scaler7201control(SCALER0, DISABLE_EXT_NEXT);*/

  logMsg("ttt1: 0x%08x 0x%08x\n",SCALER0,SCALER1,3,4,5,6);

  /*while(1)*/
{
  for(i=0; i<40; i++)
  {
    scaler7201nextclock(SCALER0);
  }
  rrr0();
}

  logMsg("ttt1: done\n",1,2,3,4,5,6);

  return;
}




void
sss()
{

  /*
  scaler7201reset(SCALER0);
  scaler7201intdisable(SCALER0);
  */
  
  scaler7201intdisable(SCALER1);
  

  return;
}

#else /* no UNIX version */

void
scaler7201int_dummy()
{
  return;
}

#endif




