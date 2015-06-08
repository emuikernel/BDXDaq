
/* bosout.c */

#include <stdio.h>
#include "bosio.h"

/* #define COND_DEBUG */
#define DEBUG  
#include "dbgpr.h"

int
bosout(BOSIO *descriptor, int *buf)
{
  BOSIO *st;
  int error, ircn, nbytes, ii;

  st = descriptor;

  error = 0;

  nbytes = buf[0]*sizeof(int);

  if(st->access == 1)		/* sequential write */
  {
    st->outnum++;
    ircn = 0;

/* !!!!!!!!!!! remote files - as direct access !!!!! */
    if (st->client != 0) ircn = st->recnum + 1;

  }
  else if(st->access == 2)	/* direct write - determine record number */
  {
    ircn = st->outnum;
  }

  if(st->client != 0)
  {
    if(ircn != 0)
      ircn = (ircn-1)*nbytes + 1;
    if(st->client < 0)	/* network channel is dead, try to reconnect */
    {
      error = bosnres(descriptor,st->client,st->stream,1);
      if(error != 0) return(error);	/* reconnection failed */
    }
a:
    writec(&st->client,&st->stream,&ircn,&nbytes,buf,&error);
    if(error == -2)	/* network channel is dead, try to reconnect */
    {
      error = bosnres(descriptor,st->client,st->stream,1);
      if(error == 0) goto a;	/* reconnection successful */
      error = -1;
    }
  }
  else
  {
    error = cput(st->stream,ircn,buf[0],buf,st->medium);
  }
 
  if(st->access == 1)		/* sequential write */
  {
    if(error != 0) {
      DPR1("bosout : cput error = %i\n",error);
      return(error);
    }
    else
      st->recnum++;
  }
  else if(st->access == 2)	/* direct write - determine record number */
  {
    if(error != 0)
      return(error);
    else
      st->recnum = ircn;
  }
 
 
  return(error);
}
