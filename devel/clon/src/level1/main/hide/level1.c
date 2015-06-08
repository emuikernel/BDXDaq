
#ifdef VXWORKS

/* level1.c - level1 boards operations */

/* IMPORTANT: write all arrays backward to avoid PowerPC-related problems
  (PPC will try to use D32 or even D64 to speed things up, while hadrware
   support D16 only; writing backward effectively turns that feature off);
   reading will be backward as well just in case ..
*/


#include <stdio.h>
#include <vxLib.h>
#include "level1.h"

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

int sysBusToLocalAdrs();

/* global offset */
static unsigned long offset;

/* addresses of the level1 boards */
static unsigned long l1addresses[33] =
                   { PHASE1_S1_PH0, PHASE1_S2_PH0, PHASE1_S3_PH0,
                     PHASE1_S4_PH0, PHASE1_S5_PH0, PHASE1_S6_PH0,
                     PHASE1_S1_PH1, PHASE1_S2_PH1, PHASE1_S3_PH1,
                     PHASE1_S4_PH1, PHASE1_S5_PH1, PHASE1_S6_PH1,
                     PHASE2_S1, PHASE2_S2, PHASE2_S3,
                     PHASE2_S4, PHASE2_S5, PHASE2_S6,
                     PHASE2_S1_RT1, PHASE2_S2_RT1, PHASE2_S3_RT1,
                     PHASE2_S4_RT1, PHASE2_S5_RT1, PHASE2_S6_RT1,
                     PHASE2_S1_RT2, PHASE2_S2_RT2, PHASE2_S3_RT2,
                     PHASE2_S4_RT2, PHASE2_S5_RT2, PHASE2_S6_RT2,
                     PHASE3, PHASE3_REG1, PHASE3_REG2 };

#ifdef TEST_SETUP
static int routers[33] = {1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          0,0,0};
#endif


/****************************************************/
/* download level1 info from memory to the hardware */
/****************************************************/

int
level1_download(unsigned short *buffer)
{
  volatile unsigned short *addr;
  unsigned short *mem, tmp;
  unsigned short tst = 0xa5a5;
  int i, j;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* check to make sure that level1 address space is available */
  for(i=0; i<33; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    if(vxMemProbe((char *)addr, VX_WRITE, sizeof(tst), (char *)&tst) == ERROR)
    {
      printf("Error writing value %d to address %x.\n",tst,(int)addr);
      return(-1);
    }
    else
    {
      printf("[%2d] write to %x ok ...\n",i,(int)addr);
    }
  }
  for(i=0; i<33; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
    mem = (unsigned short *)((int)buffer+l1addresses[i]);
    if(vxMemProbe((char *)mem, VX_READ, sizeof(tst), (char *)&tst) == ERROR)
    {
      printf("Error reading value %d from address %x.\n",tst,(int)mem);
      return(-1);
    }
    else
    {
      printf("[%2d] read from %x ok ...\n",i,(int)mem);
    }
  }

  /* phase 1 */

  for(i=0; i<12; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif

    /* download data to the hardware */
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    mem = (unsigned short *)((int)buffer+l1addresses[i]);
    /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
    for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];

    /* read data back from hardware end check */
    for(j=(BOARDSIZE-1); j>=0; j--)
	{
      tmp = addr[j];
      if(tmp != mem[j])
      {
        printf("level1_download ERROR1: [%d] 0x%08x != 0x%08x (addr 0x%08x)\n",
          j,tmp,mem[j],&addr[j]);
        return(-1);
	  }
	}
  }

  /* phase 2 */

  for(i=12; i<18; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif

    /* download data to the hardware */
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    mem = (unsigned short *)((int)buffer+l1addresses[i]);
    /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
    for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];

    /* read data back from hardware end check */
    for(j=(BOARDSIZE-1); j>=0; j--)
	{
      tmp = addr[j];
      if(tmp != mem[j])
      {
        printf("level1_download ERROR2: [%d] 0x%08x != 0x%08x (addr 0x%08x)\n",
          j,tmp,mem[j],&addr[j]);
        return(-1);
	  }
	}

    addr = (volatile unsigned short *)(offset+l1addresses[i+6]);
    *addr = 0;
    EIEIO;
    SYNC;
    tmp = *addr;
    if(tmp != 0)
    {
      printf("level1_download ERROR3: 0x%08x != 0\n",tmp);
      return(-1);
    }

    addr = (volatile unsigned short *)(offset+l1addresses[i+12]);
    *addr = 0;
    EIEIO;
    SYNC;
    tmp = *addr;
    if(tmp != 0)
    {
      printf("level1_download ERROR4: 0x%08x != 0\n",tmp,*mem);
      return(-1);
    }
  }

  /* phase 3 */

#ifndef TEST_SETUP
  /* download data to the hardware */
  addr = (volatile unsigned short *)(offset+l1addresses[30]);
  mem = (unsigned short *)((int)buffer+l1addresses[30]);
  /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
  for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];

  /* read data back from hardware end check */
  for(j=(BOARDSIZE-1); j>=0; j--)
  {
    tmp = addr[j];
    if(tmp != mem[j])
    {
      printf("level1_download ERROR5: [%d] 0x%08x != 0x%08x\n",j,tmp,mem[j]);
      return(-1);
	}
  }

  addr = (volatile unsigned short *)(offset+l1addresses[31]);
  mem = (unsigned short *)((int)buffer+l1addresses[31]);
  *addr = *mem;
  EIEIO;
  SYNC;
  tmp = *addr;
  if(tmp != *mem)
  {
    printf("level1_download ERROR6: 0x%08x != 0x%08x\n",tmp,*mem);
    return(-1);
  }

  addr = (volatile unsigned short *)(offset+l1addresses[32]);
  mem = (unsigned short *)((int)buffer+l1addresses[32]);
  *addr = *mem;
  EIEIO;
  SYNC;
  tmp = *addr;
  if(tmp != *mem)
  {
    printf("level1_download ERROR7: 0x%08x != 0x%08x\n",tmp,*mem);
    return(-1);
  }
#endif

  return(0);
}


/**************************************************/
/* upload level1 info from hardware to the memory */
/**************************************************/

int
level1_upload(unsigned short *buffer)
{
  volatile unsigned short *addr;
  unsigned short *mem;
  int i, j;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* cleanup memory */

  mem = (unsigned short *)buffer;
  for(i=0; i<BIGDATA/sizeof(short); i++) mem[i] = 0;

  /* phase 1 */

  for(i=0; i<12; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    mem = (unsigned short *)(((int)buffer)+l1addresses[i]);
    /*for(j=0; j<BOARDSIZE; j++)*/
    for(j=(BOARDSIZE-1); j>=0; j--)
    {
      mem[j] = addr[j];
    }
  }

  /* phase 2 */

  for(i=12; i<18; i++)
  {
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    mem = (unsigned short *)(((int)buffer)+l1addresses[i]);
    /*for(j=0; j<BOARDSIZE; j++)*/
    for(j=(BOARDSIZE-1); j>=0; j--)
    {
      mem[j] = addr[j];
	}


    /* ???
    addr = (unsigned short *)(offset+l1addresses[i+6]);
    *addr = 0;

    addr = (unsigned short *)(offset+l1addresses[i+12]);
    *addr = 0;
    */
  }

  /* phase 3 */

#ifndef TEST_SETUP
  addr = (volatile unsigned short *)(offset+l1addresses[30]);
  mem = (unsigned short *)(((int)buffer)+l1addresses[30]);
  /*for(j=0; j<BOARDSIZE; j++)*/
  for(j=(BOARDSIZE-1); j>=0; j--)
  {
    mem[j] = addr[j];
  }

  addr = (volatile unsigned short *)(offset+l1addresses[31]);
  mem = (unsigned short *)(((int)buffer)+l1addresses[31]);
  *mem = *addr;

  addr = (volatile unsigned short *)(offset+l1addresses[32]);
  mem = (unsigned short *)(((int)buffer)+l1addresses[32]);
  *mem = *addr;
#endif

  return(0);
}

#else
/*
void
level1_dummy()
*/
int
main()
{
  printf("level1: VXWORKS version only\n");
}

#endif
