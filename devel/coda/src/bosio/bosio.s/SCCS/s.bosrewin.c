h42362
s 00005/00004/00034
d D 1.2 03/04/17 16:46:27 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosrewin.c
e
s 00038/00000/00000
d D 1.1 00/08/10 11:10:07 boiarino 1 0
c date and time created 00/08/10 11:10:07 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSREWIN. */
 
I 3
#include <stdio.h>
E 3
#include "bosio.h"
 
D 3
int bosrewind_(int *descriptor)
E 3
I 3
int bosrewind_(BOSIO *descriptor)
E 3
{
D 3
  return(bosRewind(*descriptor));
E 3
I 3
  return(bosRewind(descriptor));
E 3
}
 
D 3
int bosRewind(int descriptor)
E 3
I 3
int bosRewind(BOSIO *descriptor)
E 3
{
  BOSIOptr st;
  int i, error;
 
D 3
  st = (BOSIOptr)descriptor;	/* set pointer */
E 3
I 3
  st = descriptor; /* set pointer */
E 3
 
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
 
E 1
