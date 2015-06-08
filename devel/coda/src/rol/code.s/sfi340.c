
/* sfi340.c - SFI library */

#ifdef VXWORKS

/* vxWorks headers */
#include <vxWorks.h> 
#include <semLib.h> 
#include <wdLib.h>
#include <taskLib.h>
#include <stdio.h>  
#include <sfi.h>
/* CODA headers */
#include "sfi.h"
/* CLAS headers */
#include "sfi340.h"

/* SFI NIM output control functions; if value=1, sets output 'nout'
   which is 0,1 or 2; if value=0 - resets those */
int
sfiNIMOutput(struct sfiStruct sfi, int nout, int value)
{
  if(nout<0 || nout>2) return(-1);
  if(value!=0 && value!=1) return(-2);

  if(value==1)
  {
    if(nout==0)
      *sfi.writeVmeOutSignalReg = 0x100; /* set output 0 */
    else if(nout==1)
      *sfi.writeVmeOutSignalReg = 0x200; /* set output 1 */
    else if(nout==2)
      *sfi.writeVmeOutSignalReg = 0x400; /* set output 2 */
  }
  else
  {
    if(nout==0)
      *sfi.writeVmeOutSignalReg = 0x1000000; /* reset output 0 */
    else if(nout==1)
      *sfi.writeVmeOutSignalReg = 0x2000000; /* reset output 1 */
    else if(nout==2)
      *sfi.writeVmeOutSignalReg = 0x4000000; /* reset output 2 */
  }

  return(0);
}

/*---------------------------------------------------------------*/


/* function to test SFI output registers, no ATM...D.Abbott */
void
sfi340do_loop()
{
  int ii;
  int *in_ptr;

  in_ptr = (int *) 0xc1f01000; 
  for(ii=0; ii<10; ii++)
  {
    *in_ptr = ii;
    taskDelay(50);
    printf("In loop \n");
  }
  *in_ptr = 0xf;
  printf("here \n");
  *in_ptr = 0xf0000;
}


/*---------------------------------------------------------------*/


/* function to test SFI output registers, w/ATM...D.Abbott */
void
sfi340loop_atm()
{
  int ii;
  int *in_ptr;

  in_ptr = (int *) 0xc2001000;
  for(ii=0; ii<10; ii++)
  {
    *in_ptr = ii;
    taskDelay(50);
    printf("In loop \n");
  } 
  *in_ptr = 0xf;
  printf("here \n");
  *in_ptr = 0xf0000;
}


/*---------------------------------------------------------------*/


/* function to test SFI output registers, general case...D.Abbott */
void
sfi340loop_all()
{
  int ii;
  int *in_ptr;
  extern volatile struct sfiStruct sfi;

  for(ii=0; ii<15; ii++)
  {
    *sfi.writeVmeOutSignalReg = ii;
    taskDelay(50);
    printf("In loop \n");
  } 
  *sfi.writeVmeOutSignalReg = 0xf;
  printf("here \n");
  *sfi.writeVmeOutSignalReg = 0xf0000;
}


/*---------------------------------------------------------------*/

#else /* no UNIX version */

void
sfi340_dummy()
{
  return;
}

#endif
