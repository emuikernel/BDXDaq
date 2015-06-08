
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coda.h"
#include "tt.h"


extern TTSPtr ttp; 

static char* Version = " VERSION: ttraw.c v1.0 - initial release " ;
static char* CompileTime = DAYTIME;


/******************************************************************************

  Routine        : TT_MakeRawBank

  Parameters     : bufin - input(CODA) fragment
                   bufout - output(BOS) fragment (loading from 3rd CODA word !)

  Discription    : This routine translates one fragment from CODA to
		           BOS format for "row" data

******************************************************************************/

int
TT_MakeRawBank(long *bufin, long *bufout)
{
  long *iw;
  long *fb, *hit;
  int len, ind, nrow;
  char bbn[8];

  iw = &bufout[2]; /* pointer to BOS array */

  nrow = bufin[0]; /* the number of words */
  fb = &bufin[1]; /* set input data pointer on first data word */

  sprintf(bbn,"RC%2.2i",ROC);
  if((ind = bosNopen(iw,*((unsigned long *)bbn),0,1,nrow)) <= 0)
  {
    printf("TT_MakeRawBank: ERROR1: bosNopen returned %d !!!\n",ind);
    return(len);
  }

  hit = &iw[ind+1]; /* set output data pointer */

  /*printf("raw: ========== nrow=%d\n",nrow);*/
  while((long *)fb < (&bufin[1] + nrow))
  {
/*
printf("raw: sl=%2d ch=%2d data=%5d\n",
((*fb)>>27)&0x1f,((*fb)>>17)&0x7f,(*fb)&0xffff);
*/
    *hit++ = *fb++; /* copy data */
  }

  len = bosNclose(iw,ind,1,nrow);

  /* returns bank length (long words) */
  return( len );
}

