/*DECK ID>, BOSREWIN. */
 
#include <stdio.h>
#include "bosio.h"
 
int bosrewind_(BOSIO *descriptor)
{
  return(bosRewind(descriptor));
}
 
int bosRewind(BOSIO *descriptor)
{
  BOSIOptr st;
  int i, error;
 
  st = descriptor; /* set pointer */
 
  error = 0;
  st->cursta = 5;	/* set status to rewind */
  if(st->client != 0)	/* remote file */
  {
    if(st->client < 0)	/* network channel is dead, try to recover */
    {
      error = bosnres(descriptor,st->client,st->stream,0);
      return(error);	/* if error != 0 , reconnect failed */
    }
    rewinc(&st->client,&st->stream,&error);
    if(error == -2) error = bosnres(descriptor,st->client,st->stream,0);
  }
  else			/* local file */
  {
    crewnd(st->stream,st->medium);
  }
 
  if(st->access == 2) st->recnum = 0;	/* otherwise lseek would fail ! */
 
  return(error);
}
 
