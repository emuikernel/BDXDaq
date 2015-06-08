/*
 *  ti97.c -  TI (Trigger Interface) board library
 *
 *    NOTE: interrupt level cannot be changed by software !!!
 */

#ifdef VXWORKS

#include "vxWorks.h"
#include "stdio.h"
#include "intLib.h"
#include "iv.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"

#include "ti97.h"
#include "ttutils.h"


IMPORT STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT STATUS sysIntEnable(int);
IMPORT STATUS sysIntDisable(int);
IMPORT STATUS sysClkRateGet();

/*****************************************************/
/**************** low level functions ****************/
/*****************************************************/

int
ti97reset(int addr)
{
  volatile TI97 *ti = (TI97 *) addr;

  ti->csr = 0x80;

  return(0);
}

int
ti97setExternalMode(int addr)
{
  volatile TI97 *ti = (TI97 *) addr;
  volatile unsigned short csr;

  csr = (ti->csr & 0x1E) + 0x1;
  ti->csr = csr;
printf("---> 0x%08x ---> 0x%08x\n",csr,ti->csr);

  return(0);
}

int
ti97setTSMode(int addr)
{
  volatile TI97 *ti = (TI97 *) addr;
  volatile unsigned short csr;

  csr = ti->csr & 0x1E;
  ti->csr = csr;

  return(0);
}

int
ti97getInterruptLevel(int addr, unsigned short *level)
{
  volatile TI97 *ti = (TI97 *) addr;

  *level = (ti->csr >> 8) & 0x7;

  return(0);
}

int
ti97getInterruptVector(int addr, unsigned short *vector)
{
  volatile TI97 *ti = (TI97 *) addr;

  *vector = ti->vec & 0xFF;

  return(0);
}

int
ti97getTrigger(int addr, unsigned short *data)
{
  volatile TI97 *ti = (TI97 *) addr;

  *data = ti->dat & 0xFF;

  return(0);
}


/*****************************************************/
/************* interrupt functions *******************/
/*****************************************************/

int
ti97intEnable(int addr, int vector)
{
  volatile TI97 *ti = (TI97 *) addr;
  volatile unsigned short csr;
  unsigned short value1,value2;

  csr = (ti->csr & 0x19) + 0x6; /* save csr and set bits 1 and 2 */

  ti->csr = 0x80; /* reset board */
  ti->vec = vector; /* set interrupt vector */

  ti97getInterruptLevel(addr, &value1);
  ti97getInterruptVector(addr, &value2);
  printf("ti97intenable: addr=0x%08x csr=0x%08x level=%1d vector=%02x\n",
                  addr,csr,value1,value2);

  ti->csr = csr; /* enable trigger and interrupt */

  return(0);
}

int
ti97intDisable(int addr)
{
  volatile TI97 *ti = (TI97 *) addr;
  volatile unsigned short csr;

  csr = ti->csr & 0x19; /* save csr reseting bits 1 and 2 */
  ti->csr = csr;

  return(0);
}

int
ti97intSave(int addr)
{
  /*ti97intDisable(addr);*/

  return(0);
}

int
ti97intRestore(int addr)
{
  volatile TI97 *ti = (TI97 *) addr;
  /*unsigned short csr;*/

  /*csr = (ti->csr & 0x19) + 0x6;*/ /* save csr and set bits 1 and 2 */
  /*ti->csr = csr;*/ /* enable trigger and interrupt */

  ti->dat = 0x8000; /* external mode: */
                    /* clears the busy state, allowing new trigger */


  return(0);
}



/****************/
/*              */
/*   EXAMPLES   */
/*              */
/****************/

/*
#define TIADR      0xefff0ed0
#define INTLEVEL   5
*/
#define TIADR      0xefff0ee0
#define INTLEVEL   6

#define INTVECTOR  0xf8



void
ti97inthandler(void)
{
  sysIntDisable(INTLEVEL);
  ti97intSave(TIADR);

  logMsg("ti97inthandler reached\n",1,2,3,4,5,6);

  ti97intRestore(TIADR);
  sysIntEnable(INTLEVEL);

  return;
}

void
titest()
{
  int i;

  ti97reset(TIADR);
  ti97setExternalMode(TIADR);

  utIntInit((VOIDFUNCPTR)ti97inthandler,0,INTLEVEL,INTVECTOR);
  ti97intEnable(TIADR,INTVECTOR);
  for(i=0; i<10; i++)
  {
    taskDelay (sysClkRateGet());
  }
  ti97intDisable(TIADR);

  return;
}

#else /* no UNIX version */

void
ti97_dummy()
{
  return;
}

#endif









