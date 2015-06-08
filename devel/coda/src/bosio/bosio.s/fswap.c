/*
  fswap.c - byte swapping for array irec[0]...irec[nc-1], if nc > 0
            if nc = 0, taken from irec[2] (after conversion)
            irec[2] is the byte swapping control word

      called from bosin (so we swap on input only)
*/

#include <stdio.h>
#include "bosio.h"

#define LSHFT(value,bits) ( (value) << (bits) )
#define RSHFT(value,bits) ( (value) >> (bits) )
#define AND(x1,x2)        ( (x1) & (x2) )
#define OR(x1,x2)         ( (x1) | (x2) )

void
fswap_(unsigned int *irec, int *nc)
{
  int i, nusedw;
  static unsigned int NB1234, NB3412, NB4321, MASK2, MASK3;
  static int first = 1;

  if(first)
  {
    NB1234 = 1+256*(2+256*(3+256*4));
    NB3412 = 3+256*(4+256*(1+256*2));
    NB4321 = 4+256*(3+256*(2+256*1));
    /* byte masks */
    MASK2 = 255*256;
    MASK3 = 255*256*256;

    first = 0;
  }

  /*printf("irec[2]=0x%08x\n",irec[2]); fflush(stdout);*/

  if(irec[2] == NB1234) /* no conversion necessary */
  {
	/*printf("no swap !\n"); fflush(stdout);*/
    ;
  }
  else if(irec[2] == NB3412) /* convert first two words */
  {
    irec[0] = OR(LSHFT(irec[0],16),RSHFT(irec[0],16));
    irec[1] = OR(LSHFT(irec[1],16),RSHFT(irec[1],16));
    if(*nc == 0)
    {
      nusedw = irec[1];
/*printf("nusedw1=%d\n",nusedw); fflush(stdout);*/
    }
    else
    {
      nusedw = *nc;
/*printf("nusedw2=%d\n",nusedw); fflush(stdout);*/
    }
    /* convert remaining words */
    for(i=3; i<=nusedw; i++)
      irec[i-1] = OR(LSHFT(irec[i-1],16),RSHFT(irec[i-1],16));
  }
  else if(irec[2] == NB4321) /* convert first two words */
  {
    irec[0] =
      OR( OR (LSHFT(irec[0],24), AND(LSHFT(irec[0],8), MASK3) ),
      OR(AND (RSHFT(irec[0], 8),MASK2), RSHFT(irec[0],24) ) );
    irec[1] =
      OR(OR(LSHFT(irec[1],24),AND(LSHFT(irec[1],8),MASK3)),
      OR(AND(RSHFT(irec[1],8),MASK2),RSHFT(irec[1],24)));
    if(*nc == 0)
    {
      nusedw = irec[1];
/*printf("nusedw3=%d\n",nusedw); fflush(stdout);*/
    }
    else
    {
      nusedw = *nc;
/*printf("nusedw4=%d\n",nusedw); fflush(stdout);*/
    }
    /* convert remaining words */
    for(i=3; i<=nusedw; i++)
      irec[i-1] =
           OR(  OR(LSHFT(irec[i-1],24), AND(LSHFT(irec[i-1],8),MASK3)),
           OR( AND(RSHFT(irec[i-1],8),MASK2),RSHFT(irec[i-1],24)));
  }

  return;
}




/* special version to swap 'et' buffers where first 2 words are absent,   */
/* so records starts from rec which corresponds to &irec[2] in fswap_(), */
/* and nwords here equal to (*nc - 2) in fswap_() (accually it is taken from record) */

#define ETNB1234 0x04030201
#define ETNB3412 0x02010403
#define ETNB4321 0x01020304
#define ETMASK2  0x0000ff00
#define ETMASK3  0x00ff0000

void
etfswap(unsigned int *rec)
{
  int i, nwords;
  /*
  static unsigned int ETNB1234, ETNB3412, ETNB4321, ETMASK2, ETMASK3;
  static int first = 1;
  if(first)
  {
    ETNB1234 = 1+256*(2+256*(3+256*4));
    ETNB3412 = 3+256*(4+256*(1+256*2));
    ETNB4321 = 4+256*(3+256*(2+256*1));
    ETMASK2 = 255*256;
    ETMASK3 = 255*256*256;
    first = 0;
  }
  */

  if(rec[0] == ETNB1234) /* no conversion necessary */
  {
    ;
  }
  else if(rec[0] == ETNB3412)
  {
    nwords =  RECHEADLEN + OR(LSHFT(rec[10],16),RSHFT(rec[10],16));
    for(i=0; i<nwords; i++)
    {
      rec[i] = OR(LSHFT(rec[i],16),RSHFT(rec[i],16));
    }
  }
  else if(rec[0] == ETNB4321)
  {
    nwords = RECHEADLEN +
      OR( OR (LSHFT(rec[10],24), AND(LSHFT(rec[10],8), ETMASK3) ),
      OR(AND (RSHFT(rec[10], 8),ETMASK2), RSHFT(rec[10],24) ) );
    for(i=0; i<nwords; i++)
    {
      rec[i] =
           OR(  OR(LSHFT(rec[i],24), AND(LSHFT(rec[i],8),ETMASK3)),
           OR( AND(RSHFT(rec[i],8),ETMASK2),RSHFT(rec[i],24)));
    }
  }
  else
  {
    printf("etfswap ERROR: illegal pattern = 0x%08x\n",rec[0]);
  }

  return;
}



