h21742
s 00007/00003/00072
d D 1.2 03/04/17 16:46:05 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosout.c
e
s 00075/00000/00000
d D 1.1 00/08/10 11:10:06 boiarino 1 0
c date and time created 00/08/10 11:10:06 by boiarino
e
u
U
f e 0
t
T
I 1

I 3
/* bosout.c */

#include <stdio.h>
E 3
#include "bosio.h"

/* #define COND_DEBUG */
#define DEBUG  
#include "dbgpr.h"

D 3
int bosout(int descriptor, int *buf)
E 3
I 3
int
bosout(BOSIO *descriptor, int *buf)
E 3
{
D 3
  BOSIOptr st;
E 3
I 3
  BOSIO *st;
E 3
  int error, ircn, nbytes, ii;

D 3
  st = (BOSIOptr)descriptor;
E 3
I 3
  st = descriptor;
E 3

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
E 1
