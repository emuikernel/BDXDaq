h59431
s 00062/00030/00253
d D 1.9 08/04/01 16:36:42 boiarino 10 9
c backward write/read to overcome PowerPC D16->D32 optimization
c 
e
s 00054/00001/00229
d D 1.8 08/04/01 15:21:55 boiarino 9 8
c extra check
c 
e
s 00003/00002/00227
d D 1.7 08/04/01 14:48:41 boiarino 8 7
c *** empty log message ***
e
s 00004/00001/00225
d D 1.6 07/10/12 13:09:29 boiarino 7 6
c *** empty log message ***
e
s 00005/00005/00221
d D 1.5 07/03/20 17:07:08 boiarino 6 5
c *** empty log message ***
e
s 00010/00002/00216
d D 1.4 07/03/20 16:38:10 boiarino 5 4
c *** empty log message ***
e
s 00033/00000/00185
d D 1.3 05/02/25 14:40:38 boiarino 4 3
c *** empty log message ***
e
s 00017/00015/00168
d D 1.2 04/02/03 20:56:48 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 02/07/06 23:44:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/level1.c
e
s 00183/00000/00000
d D 1.1 02/07/06 23:44:05 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

I 5
#ifdef VXWORKS

E 5
/* level1.c - level1 boards operations */

I 10
/* IMPORTANT: write all arrays backward to avoid PowerPC-related problems
  (PPC will try to use D32 or even D64 to speed things up, while hadrware
   support D16 only; writing backward effectively turns that feature off);
   reading will be backward as well just in case ..
*/
E 10
I 4
D 5
#undef TEST_SETUP
E 5

I 10

E 10
D 5

E 5
E 4
#include <stdio.h>
#include <vxLib.h>
#include "level1.h"

I 10
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

E 10
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

I 4
#ifdef TEST_SETUP
D 6
static int routers[33] = {0,0,1,0,0,0,
                          0,0,1,0,0,0,
                          0,0,1,0,0,0,
                          0,0,1,0,0,0,
                          0,0,1,0,0,0,
E 6
I 6
static int routers[33] = {1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
                          1,0,0,0,0,0,
E 6
                          0,0,0};
#endif
E 4


/****************************************************/
/* download level1 info from memory to the hardware */
/****************************************************/

int
level1_download(unsigned short *buffer)
{
D 3
  unsigned short *addr, *mem;
E 3
I 3
  volatile unsigned short *addr;
D 9
  unsigned short *mem;
E 9
I 9
  unsigned short *mem, tmp;
E 9
E 3
  unsigned short tst = 0xa5a5;
  int i, j;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* check to make sure that level1 address space is available */
  for(i=0; i<33; i++)
  {
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
E 4
D 3
    addr = (unsigned short *)(offset+l1addresses[i]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
E 3
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
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
E 4
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
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
I 9

    /* download data to the hardware */
E 9
E 4
D 3
    addr = (unsigned short *)(offset+l1addresses[i]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
E 3
    mem = (unsigned short *)((int)buffer+l1addresses[i]);
D 10
    for(j=0; j<BOARDSIZE; j++) *addr++ = *mem++;
E 10
I 10
    /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
    for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];
E 10
I 9

    /* read data back from hardware end check */
D 10
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    for(j=0; j<BOARDSIZE; j++)
E 10
I 10
    for(j=(BOARDSIZE-1); j>=0; j--)
E 10
	{
D 10
      tmp = *addr++;
E 10
I 10
      tmp = addr[j];
E 10
      if(tmp != mem[j])
      {
D 10
        printf("level1_download ERROR1: [%d] 0x%08x != 0x%08x\n",j,tmp,mem[j]);
E 10
I 10
        printf("level1_download ERROR1: [%d] 0x%08x != 0x%08x (addr 0x%08x)\n",
          j,tmp,mem[j],&addr[j]);
E 10
        return(-1);
	  }
	}
E 9
  }

  /* phase 2 */

  for(i=12; i<18; i++)
  {
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
I 9

    /* download data to the hardware */
E 9
E 4
D 3
    addr = (unsigned short *)(offset+l1addresses[i]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
E 3
    mem = (unsigned short *)((int)buffer+l1addresses[i]);
D 10
    for(j=0; j<BOARDSIZE; j++) *addr++ = *mem++;
E 10
I 10
    /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
    for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];
E 10

I 9
    /* read data back from hardware end check */
D 10
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
    for(j=0; j<BOARDSIZE; j++)
E 10
I 10
    for(j=(BOARDSIZE-1); j>=0; j--)
E 10
	{
D 10
      tmp = *addr++;
E 10
I 10
      tmp = addr[j];
E 10
      if(tmp != mem[j])
      {
D 10
        printf("level1_download ERROR2: [%d] 0x%08x != 0x%08x\n",j,tmp,mem[j]);
E 10
I 10
        printf("level1_download ERROR2: [%d] 0x%08x != 0x%08x (addr 0x%08x)\n",
          j,tmp,mem[j],&addr[j]);
E 10
        return(-1);
	  }
	}

E 9
D 3
    addr = (unsigned short *)(offset+l1addresses[i+6]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i+6]);
E 3
    *addr = 0;
I 10
    EIEIO;
    SYNC;
    tmp = *addr;
    if(tmp != 0)
    {
      printf("level1_download ERROR3: 0x%08x != 0\n",tmp);
      return(-1);
    }
E 10

D 3
    addr = (unsigned short *)(offset+l1addresses[i+12]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i+12]);
E 3
    *addr = 0;
I 10
    EIEIO;
    SYNC;
    tmp = *addr;
    if(tmp != 0)
    {
      printf("level1_download ERROR4: 0x%08x != 0\n",tmp,*mem);
      return(-1);
    }
E 10
  }

  /* phase 3 */

I 4
#ifndef TEST_SETUP
I 9
  /* download data to the hardware */
E 9
E 4
D 3
  addr = (unsigned short *)(offset+l1addresses[30]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[30]);
E 3
  mem = (unsigned short *)((int)buffer+l1addresses[30]);
D 10
  for(j=0; j<BOARDSIZE; j++) *addr++ = *mem++;
E 10
I 10
  /*for(j=0; j<BOARDSIZE; j++) addr[j] = mem[j];*/
  for(j=(BOARDSIZE-1); j>=0; j--) addr[j] = mem[j];
E 10

I 9
  /* read data back from hardware end check */
D 10
  addr = (volatile unsigned short *)(offset+l1addresses[30]);
  for(j=0; j<BOARDSIZE; j++)
E 10
I 10
  for(j=(BOARDSIZE-1); j>=0; j--)
E 10
  {
D 10
    tmp = *addr++;
E 10
I 10
    tmp = addr[j];
E 10
    if(tmp != mem[j])
    {
D 10
      printf("level1_download ERROR3: [%d] 0x%08x != 0x%08x\n",j,tmp,mem[j]);
E 10
I 10
      printf("level1_download ERROR5: [%d] 0x%08x != 0x%08x\n",j,tmp,mem[j]);
E 10
      return(-1);
	}
  }

E 9
D 3
  addr = (unsigned short *)(offset+l1addresses[31]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[31]);
E 3
  mem = (unsigned short *)((int)buffer+l1addresses[31]);
  *addr = *mem;
I 10
  EIEIO;
  SYNC;
E 10
I 9
  tmp = *addr;
  if(tmp != *mem)
  {
D 10
    printf("level1_download ERROR4: 0x%08x != 0x%08x\n",tmp,*mem);
E 10
I 10
    printf("level1_download ERROR6: 0x%08x != 0x%08x\n",tmp,*mem);
E 10
    return(-1);
  }
E 9

D 3
  addr = (unsigned short *)(offset+l1addresses[32]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[32]);
E 3
  mem = (unsigned short *)((int)buffer+l1addresses[32]);
  *addr = *mem;
I 10
  EIEIO;
  SYNC;
E 10
I 9
  tmp = *addr;
  if(tmp != *mem)
  {
D 10
    printf("level1_download ERROR5: 0x%08x != 0x%08x\n",tmp,*mem);
E 10
I 10
    printf("level1_download ERROR7: 0x%08x != 0x%08x\n",tmp,*mem);
E 10
    return(-1);
  }
E 9
I 4
#endif
E 4

  return(0);
}


/**************************************************/
/* upload level1 info from hardware to the memory */
/**************************************************/

int
level1_upload(unsigned short *buffer)
{
D 3
  unsigned short *addr, *mem;
E 3
I 3
  volatile unsigned short *addr;
  unsigned short *mem;
E 3
  int i, j;

  sysBusToLocalAdrs(0x39,0,&offset);

  /* cleanup memory */

  mem = (unsigned short *)buffer;
D 10
  for(i=0; i<BIGDATA/sizeof(short); i++) *mem++ = 0;
E 10
I 10
  for(i=0; i<BIGDATA/sizeof(short); i++) mem[i] = 0;
E 10

  /* phase 1 */

  for(i=0; i<12; i++)
  {
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
E 4
D 3
    addr = (unsigned short *)(offset+l1addresses[i]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
E 3
    mem = (unsigned short *)(((int)buffer)+l1addresses[i]);
D 10
    printf("1 addr->0x%08x mem=0x%08x\n",(int)addr,(int)mem);
    for(j=0; j<BOARDSIZE; j++)
E 10
I 10
    /*for(j=0; j<BOARDSIZE; j++)*/
    for(j=(BOARDSIZE-1); j>=0; j--)
E 10
    {
D 10
      *mem++ = *addr++;
      if(j==0||j==1) printf("--->0x%04x->0x%04x\n",*(addr-1),*(mem-1));
E 10
I 10
      mem[j] = addr[j];
E 10
    }
  }

  /* phase 2 */

  for(i=12; i<18; i++)
  {
I 4
#ifdef TEST_SETUP
    if(routers[i] == 0) continue;
#endif
E 4
D 3
    addr = (unsigned short *)(offset+l1addresses[i]);
E 3
I 3
    addr = (volatile unsigned short *)(offset+l1addresses[i]);
E 3
    mem = (unsigned short *)(((int)buffer)+l1addresses[i]);
D 10
    printf("2 addr->0x%08x mem=0x%08x\n",(int)addr,(int)mem);
    for(j=0; j<BOARDSIZE; j++)
E 10
I 10
    /*for(j=0; j<BOARDSIZE; j++)*/
    for(j=(BOARDSIZE-1); j>=0; j--)
E 10
    {
D 10
      *mem++ = *addr++;
      if(j==0||j==1) printf("--->0x%04x->0x%04x\n",*(addr-1),*(mem-1));
D 8
      if(((int)(addr-1))==0xfa0c2800) printf("[0x%08x]===>0x%04x(%d)->0x%04x(%d)\n",
                                (int)(addr-1),*(addr-1),*(addr-1),*(mem-1),*(mem-1));
E 8
I 8
      if(((int)(addr-1))==(offset+0x0c2800))
                     printf("[0x%08x]===>0x%04x(%d)->0x%04x(%d)\n",
                       (int)(addr-1),*(addr-1),*(addr-1),*(mem-1),*(mem-1));
E 10
I 10
      mem[j] = addr[j];
E 10
E 8
	}
I 10


E 10
    /* ???
    addr = (unsigned short *)(offset+l1addresses[i+6]);
    *addr = 0;

    addr = (unsigned short *)(offset+l1addresses[i+12]);
    *addr = 0;
    */
  }

  /* phase 3 */

I 4
#ifndef TEST_SETUP
E 4
D 3
  addr = (unsigned short *)(offset+l1addresses[30]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[30]);
E 3
  mem = (unsigned short *)(((int)buffer)+l1addresses[30]);
D 10
  for(j=0; j<BOARDSIZE; j++) *mem++ = *addr++;
E 10
I 10
  /*for(j=0; j<BOARDSIZE; j++)*/
  for(j=(BOARDSIZE-1); j>=0; j--)
  {
    mem[j] = addr[j];
  }
E 10

D 3
  addr = (unsigned short *)(offset+l1addresses[31]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[31]);
E 3
  mem = (unsigned short *)(((int)buffer)+l1addresses[31]);
  *mem = *addr;

D 3
  addr = (unsigned short *)(offset+l1addresses[32]);
E 3
I 3
  addr = (volatile unsigned short *)(offset+l1addresses[32]);
E 3
  mem = (unsigned short *)(((int)buffer)+l1addresses[32]);
  *mem = *addr;
I 4
#endif
E 4

  return(0);
}

I 5
#else
E 5
D 7

E 7
I 7
/*
E 7
I 5
void
level1_dummy()
I 7
*/
int
main()
E 7
{
  printf("level1: VXWORKS version only\n");
}

#endif
E 5
E 1
