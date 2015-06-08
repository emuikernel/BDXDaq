
/* ttutils.c - some useful functions to be used by codatt project */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VXWORKS

#include "iv.h"
#include "logLib.h"
#include "intLib.h"

IMPORT STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT STATUS sysIntEnable(int);

#ifdef VXWORKSPPC
IMPORT int intDisconnect(int);
#endif

#endif

#include "ttutils.h"

/********************************/
/* ring buffer service routines */
/********************************/

#define RING_OK                0
#define RING_EMPTY            -1
#define RING_FULL             -1

typedef struct
{
  int len;    /* the length (in words) of the ring buffer */
  int nwords; /* the number of the words in the ring buffer */
  int *begin; /* pointer to the begining of the ring buffer */
  int *end;   /* pointer to the end of the ring buffer */
  int *start; /* pointer to the begining of data in the ring buffer */
  int *stop;  /* pointer to the end of data in the ring buffer */
} RING;



int *
utRingAlloc(int length)
{
  int nhead;
  int *ring;
  RING *r;

  nhead = sizeof(RING)/sizeof(int); /* # of the words in RING structure */
  ring = (int *) malloc((length+nhead)*sizeof(int));
  r = (RING *) ring;
  r->len = length;                  /* ring length */

  return(ring);
}

void
utRingFree(int *ring)
{
  free(ring);

  return;
}

int
utRingInit(int *ring)
{
  int nhead, length;
  RING *r;
  r = (RING *) ring;

  nhead = sizeof(RING)/sizeof(int); /* # of the words in RING structure */
  r->nwords = 0;                    /* the number of data words */

  length = r->len + nhead;          /* the length of allocated space */
  r->begin = (int *) &ring[nhead];
  r->end = (int *) &ring[length-1];

  r->start = r->stop = r->begin;
  /*
  printf("utRingInit: Ring buffer at address 0x%08x has been initialized.\n",
            (int)ring);
  printf("utRingInit: Data buffer size is %d words.\n",r->len);
  */
  return(RING_OK);
}

int *
utRingCreate(int length)
{
  int *ring;

  ring = utRingAlloc(length);
  utRingInit(ring);

  return(ring);
}

/*
   write one word into ring buffer; returns RING_OK if word have been written
   or RING_FULL if not
*/

int
utRingWriteWord(int *ring, int word)
{
  RING *r;
  r = (RING *) ring;

  if(r->stop == r->start && r->nwords)  /* last time we have reached our own tail */
  {
    return(RING_FULL);
  }

  if(r->stop > r->end)  /* last time we have reached the end of the ring buffer */
  {
    if(r->start == r->begin) /* ring have been filled */
    {
      return(RING_FULL);
    }
    else
    {
      r->stop = r->begin; /* will write from the begining */
    }
  }

  *r->stop++ = word; 
  r->nwords ++;
  return(RING_OK);
}


int
utRingGetNwords(int *ring)
{
  RING *r;
  r = (RING *) ring;

  return(r->nwords);
}

int
utRingReadWord(int *ring)
{
  int word;
  RING *r;
  r = (RING *) ring;

  if(!r->nwords)
  {
    return(RING_EMPTY);
  }

  word = *r->start++;
  r->nwords --;

  if(!r->nwords) /* got the last word from the ring buffer */
  {
    r->start = r->stop = r->begin;
  }
  else if(r->start > r->end) /* we have reached the end of the ring buffer */
  {
    r->start = r->begin;
  }

  return(word);
}


void
utRingStatusPrint(int *ring)
{
  int i, *start;
  RING *r;
  r = (RING *) ring;

  start = r->start;

  for(i=0; i<r->nwords; i++) printf("-> 0x%08x\n",*start++);

  return;
}

void
utRingStatusDraw(int *ring)
{
  int i, start, stop;
  static char *cbuf;
  static first=1;

  RING *r;
  r = (RING *) ring;

  if(first)
  {
    first = 0;
    cbuf = (char *) malloc(r->len+1);
    cbuf[r->len] = ' '; /* do not CR */
  }

  for(i=0; i<r->len; i++) cbuf[i] = '_';
  printf(" ");
  if(!r->nwords)
  {
    start = r->start - r->begin;
    stop = r->stop - r->begin;
    for(i=0; i<r->len; i++) printf("%c",cbuf[i]);
    printf("   no any data: start=%d stop=%d r->nwords=%d\n",
       start,stop,r->nwords);    
  }
  else if(r->start < r->stop)
  {
    start = r->start - r->begin;
    stop = r->stop - r->begin;
    for(i=start; i<stop; i++) cbuf[i] = 'X';
    for(i=0; i<r->len; i++) printf("%c",cbuf[i]);
    printf("  start < stop: start=%d stop=%d r->nwords=%d\n",
       start,stop,r->nwords);
  }
  else
  {
    start = r->start - r->begin;
    stop = r->stop - r->begin;
    for(i=0; i<stop; i++) cbuf[i] = 'X';
    for(i=start; i<r->len; i++) cbuf[i] = 'X';
    for(i=0; i<r->len; i++) printf("%c",cbuf[i]);
    printf("  start > stop: start=%d stop=%d r->nwords=%d\n",
       start,stop,r->nwords);
  }

  return;
}


/* testing ring */

void
utRingTest()
{
  int i, j, nloops=1, *ring, len=9, status1=0, status2=0, count=0;
  int datain=0, dataout=0;
  char ch[2];

  ch[0] = ' ';
  ch[1] = '\0';
  ring = utRingCreate(len);


  /* reading empty ring */

  count = 0;
  for(i=0; i<5; i++)
  {
    if((status1=utRingReadWord(ring)) == RING_EMPTY) count ++;
    printf("status1    %2d -> ",status1);
    utRingStatusDraw(ring);
  }
  if(count == 5)
  {
    printf("reading empty ring ............... OK\n");
  }
  else
  {
    printf("ERROR reading empty ring - exit.\n");
    exit(0);
  }


  /* reaching the tail */

  while(1)
  {
    for(j=0; j<nloops; j++)
	{
      for(i=0; i<2; i++)
      {
        status1 = utRingWriteWord(ring, ++datain);
        if(status1) break;
        printf("writing %5d -> ",datain);
        utRingStatusDraw(ring);
        count ++;
      }
      if(status1) break;
      for(i=0; i<1; i++)
      {
        status2 = utRingReadWord(ring);
        printf("reading %5d -> ",dataout);
        utRingStatusDraw(ring);
        if(status2 == RING_EMPTY)
        {
          printf("trying to read empty ring\n");
          break;
		}
        if(status2 != dataout + 1)
		{
          printf("ERROR: status2=%d, previous dataout=%d\n",status2,dataout);
          exit(0);
	    }
        else
		{
          dataout++;
	    }
      }
      if(status1 || status2 < 0) break;
	}
    if(status1 || status2 < 0) break;
  }

  printf("reaching the tail ................ OK\n");


  /* reading until ring becomes empty */

  count = 0;
  do
  {
    status1 = utRingReadWord(ring);
    printf("status1    %2d -> ",status1);
    utRingStatusDraw(ring);
  } while(status1 != RING_EMPTY);

  printf("reaching the empty ............... OK\n");

  utRingFree(ring);

  return;
}



/******************************/
/* interrupt service routines */
/******************************/

#ifdef VXWORKS

STATUS
utIntInit(VOIDFUNCPTR handler, int arg, int level, int vec)
{
  VOIDFUNCPTR *vector;
  FUNCPTR oldfunc, newfunc;

  logMsg("utIntInit reached\n",1,2,3,4,5,6);

  vector = INUM_TO_IVEC(vec);
  logMsg("utIntInit: interrupt number %x(%d) -> interrupt vector %x(%d)\n",
                      vec,vec,(int)vector,(int)vector,5,6);

#ifdef VXWORKSPPC
  if((intDisconnect((int)vector) != 0))
  {
    logMsg("utIntInit: error disconnecting interrupt\n",1,2,3,4,5,6);
    return(-1);
  }
  else
  {
    logMsg("utIntInit: interrupt handler disconnected\n",1,2,3,4,5,6);
  }
#endif

  oldfunc = intVecGet((FUNCPTR *)vector);
  logMsg("utIntInit: oldfunc = %08x\n",(int)oldfunc,2,3,4,5,6);

/*
  newfunc = intHandlerCreate((FUNCPTR)handler,0);
  logMsg("utIntInit: newfunc         = %08x\n",(int)newfunc,2,3,4,5,6);

  intVecSet((FUNCPTR *)vector,newfunc);
  logMsg("utIntInit: newfunc(return) = %08x\n",intVecGet((FUNCPTR *)vector),2,3,4,5,6);
*/

  intConnect(vector,handler,0);

  newfunc = intVecGet((FUNCPTR *)vector);
  logMsg("utIntInit: newfunc = %08x\n",(int)newfunc,2,3,4,5,6);

#ifdef VXWORKSPPC
  logMsg("utIntInit: enable MPIC interrupts for vector %x\n",(int)vector,2,3,4,5,6);
  intEnable(vec);
#endif

  sysIntEnable(level);

  logMsg("utIntInit done\n",1,2,3,4,5,6);
  return(0);
}


/*************************/
/* some usefull routines */
/*************************/

unsigned long
getAddress(int addr, int am)
{
  unsigned long laddr;
  int res;
  logMsg("getAddress reached\n",1,2,3,4,5,6);
  res = sysBusToLocalAdrs(am,(char *)(addr&0xffffff),(char **)&laddr);
  logMsg("getAddress: res=%d, laddr=%08x\n",res,laddr,3,4,5,6);

  return(laddr);
}




/******************************/
/* boot parameters operations */
/******************************/

/* from mv2304/config.h, mv2400/config.h, mv5100/config.h */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */

/* from all/configAll.h */
#define BOOT_LINE_OFFSET        0x4200
#define BOOT_LINE_ADRS	((char *) (LOCAL_MEM_LOCAL_ADRS+BOOT_LINE_OFFSET))
#define	BOOT_LINE_SIZE	255	/* use 255 bytes for bootline */

/* my staff */
#define TNLEN 3 /* length of 'tn=' */

void
utBootPrint()
{
  bootParamsShow(BOOT_LINE_ADRS);

  return;
}

void
utBootPrintTest()
{
  char str[BOOT_LINE_SIZE];

  str[BOOT_LINE_SIZE-1] = '\0';

  utBootGetTargetName(str,strlen(str));
  printf("utBootPrintTest: target name >%s<\n",str);

  return;
}

void
utBootGetTargetName(char *tn, int tnlen)
{
  int i, j, len;
  char *str = (char *)BOOT_LINE_ADRS;

  len = strlen(str);
  /*printf("len = %d, str >%s<\n",len,str);*/

  for(i=0; i<(len-TNLEN); i++)
  {
    if(!strncmp((char *)&str[i],"tn=",3)) break;
  }
  /*printf("i=%d\n",i);*/

  if(i >= (len-TNLEN))
  {
    printf("utBootTargetName ERROR: cannot find 'tn=' in boot string\n");
    return;
  }
  i += 3; /* skip 'tn=' */

  for(j=i; j<len; j++)
  {
    if(str[j] == ' ') break;
  }
  /*printf("j=%d\n",j);*/

  len = j-i;
  if(tnlen < len) len = tnlen;
  /*printf("i=%d, len=%d\n",i,len);*/
  strncpy((char *)tn, (char *)&str[i], len);
  tn[len] = '\0';
  /*printf("utBootTargetName: target name >%s<\n",tn);*/

  return;
}

#endif
