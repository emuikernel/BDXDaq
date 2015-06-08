/*
 *  scaler560.c - C.A.E.N. VME board library - Sergey Boyarinov
 *
 */

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vxWorks.h> 
#include <logLib.h>

#include "scaler560.h"

/* base address A23-A08 could be set by switches on a board */

/*---------------------------------------------------------------*/
/*--------------------------- scaler560 -------------------------*/
/*---------------------------------------------------------------*/

/* low level functions */


/* version (bits 15:12) and serial number (bits 11:0) */
int
scaler560getVersion(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0xFE);

  *value = *reg;

  return(0);
}

/* manufacturer number (bits 15:10) and module type (bits 9:0) */
int
scaler560getBoardID(UINT32 addr, UINT16 *value)
{

  volatile UINT16 *reg = (UINT16 *) (addr + 0xFC);

  *value = *reg;

  return(0);
}

/* allow the automatic localization of the module */
int
scaler560getGEOAddress(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0xFA);

  *value = *reg;

  return(0);
}

/* gets status register

 8 lower bits shows 8 sections status: bit=0 - 2 32-bit scalers
                                       bit=1 - 1 64-bit scaler
 bit-to-section table:
    bit 7 - section 4 status
    bit 6 - section 5 status
    bit 5 - section 6 status
    bit 4 - section 7 status
    bit 3 - section 0 status
    bit 2 - section 1 status
    bit 1 - section 2 status
    bit 0 - section 3 status
*/
int
scaler560getStatus(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x58);

  *value = (*reg) & 0x00FF;

  return(0);
}

/* increase all channels if module is configured with
   16 independent channels */
int
scaler560increment(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x56);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

/* clears the VETO; the scalers are able to count if no external VETO */
int
scaler560resetVETO(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x54);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

/* sets the VETO; the scalers are not able to count */
int
scaler560setVETO(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x52);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

/* gets the status of the VETO latched during the last read counter operation

     0 - the module was in the inhibit state when the channel has been read
     1 - the module was able to count when the cannel has been read; in that
         case the counter value previously read has been latched 'on fly' and
         may be not correct
*/
int
scaler560getVETO(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x06);

  *value = ((*reg) >> 7) & 0x1;

  return(0);
}

/* all the scalers are cleared;
   the VME interrupt request (if asserted) is removed;
   the VME interrupt generation is disabled
*/
int
scaler560clear(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x50);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

/* reads 16 words from all scaler channels to array
   which must be 16 words at least */
int
scaler560read(UINT32 addr, UINT32 *array)
{
  int i;
  volatile UINT32 *reg = (UINT32 *) (addr + 0x10);

  for(i=0; i<16; i++)
  {
    array[i] = *reg++;
    /*printf("[%2d] -> %u\n",i,array[i]);*/
  }

  return(0);
}


/* Interrupt operations */

int
scaler560setInterruptLevel(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x06);

  if(value > 0x0007) return(-1);
  *reg = value & 0x0007;

  return(0);
}

int
scaler560getInterruptLevel(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x06);

  *value = (*reg) & 0x0007;

  return(0);
}

int
scaler560setInterruptVector(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x04);

  if(value > 0x00FF) return(-1);
  *reg = value & 0x00FF;

  return(0);
}

int
scaler560getInterruptVector(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x04);

  *value = (*reg) & 0x00FF;

  return(0);
}

/* interrupt will be generated if most significant bit becomes true

   bit0=1 - interrupt from section 0 is enabled
   .......................................
   bit7=1 - interrupt from section 7 is enabled
*/
int
scaler560setInterruptSource(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x0E);

  if(value > 0x00FF) return(-1);
  *reg = value & 0x00FF;

  return(0);
}

int
scaler560getInterruptSource(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x0E);

  *value = (*reg) & 0x00FF;

  return(0);
}


/*****************************************************/
/************* interrupt functions *******************/
/*****************************************************/

int
scaler560intenable(int addr, unsigned int source, int level, int vector)
{
  unsigned short value1,value2,value3;
  volatile UINT16 *reg = (UINT16 *) (addr + 0x08);

  /* check for source out of range */
  if((source<1) || (source>0xFF))
  {
    printf("scaler560intenable: ERROR: source %d is out of range(1-0xFF)\n",
                 source);
    return(-1);
  }

  scaler560setInterruptLevel (addr, (unsigned short)level);
  scaler560setInterruptVector(addr, (unsigned short)vector);
  scaler560setInterruptSource(addr, (unsigned short)source);

  scaler560getInterruptLevel (addr, &value2);
  scaler560getInterruptVector(addr, &value3);
  scaler560getInterruptSource(addr, &value1);

  printf("scaler560intenable: addr=0x%08x source=%d level=%d vector=%d\n",
                              addr,value1,value2,value3);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

int
scaler560intdisable(int addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x0A);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

int
scaler560intClear(int addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x0C);

  *reg = 0x1; /* arbitrary data */

  return(0);
}


int
scaler560intSave(int addr, unsigned int *mask0)
{
  scaler560intdisable(addr);
  scaler560intClear(addr);

  return(0);
}

int
scaler560intRestore(int addr, unsigned int mask0)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x08);

  *reg = 0x1; /* arbitrary data */

  return(0);
}


/* test */

#include "taskLib.h"

/*
#define SCALER560 0x610000
*/
#define SCALER560 0x50000
int sysBusToLocalAdrs();
int sysClkRateGet();


void
scaler560test()
{
  int i;
  unsigned int addr, buf[16];

  sysBusToLocalAdrs(AM24N,SCALER560,&addr);
  printf("addr=0x%08x\n",addr);

  scaler560clear(addr);
  scaler560resetVETO(addr);
  for(i=0; i<100; i++)
  {
    taskDelay (sysClkRateGet() * 1);
    scaler560read(addr, buf);
    printf("[%d] %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",i,
	  buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],
      buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);
  }

  return;
}

void
scaler560vpk(int runnum, int lambda1, int lambda2, int dlambda, int interval)
{
  FILE *fd;
  int i, nseconds;
  unsigned int addr, buf[16];
  char fname[80];

  sysBusToLocalAdrs(AM24N,SCALER560,&addr);
  printf("addr=0x%08x\n",addr);

  nseconds = ((lambda2-lambda1)/dlambda+1)*interval + interval;

  printf("/work/vpk/hromator_%06d.txt\n",runnum);
  sprintf(fname,"/work/vpk/hromator_%06d.txt",runnum);
  if( (fd = fopen(fname,"w")) == NULL)
  {
    printf("Can't create output file >%s<\n",fname);
    return;
  }

  printf("run=%d, lambda1=%d, lambda2=%d, dlambda=%d, interval=%d, total=%d sec\n",
    runnum,lambda1,lambda2,dlambda,interval,nseconds);
  fprintf(fd,"run=%d, lambda1=%d, lambda2=%d, dlambda=%d, interval=%d, total=%d sec\n",
    runnum,lambda1,lambda2,dlambda,interval,nseconds);

  scaler560clear(addr);
  scaler560resetVETO(addr);
  for(i=0; i<nseconds; i++)
  {
    taskDelay (sysClkRateGet() * 1);
    scaler560read(addr, buf);
	if( !(i%interval) )
	{
      printf("New interval\n");
      fprintf(fd,"New interval\n");
	}
    printf("  [%3d] %d %d %d %d %d %d %d %d\n",i,
	  buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    fprintf(fd,"  [%3d] %d %d %d %d %d %d %d %d\n",i,
	  buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    scaler560clear(addr);
    scaler560resetVETO(addr);
  }

  fclose(fd);

  return;
}

#else /* no UNIX version */

void
scaler560_dummy()
{
  return;
}

#endif
