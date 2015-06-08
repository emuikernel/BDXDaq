h42889
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosngetf.c
e
s 00068/00000/00000
d D 1.1 00/08/10 11:10:05 boiarino 1 0
c date and time created 00/08/10 11:10:05 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSNGETF. */
 
#include "bos.h"

#ifndef VAX
 
void bosngetformat_(int *jw, char *name, char *fmt, int lename, int lenfmt)
{
  char *nam, *fm;
  int i;

  nam = (char *) MALLOC(lename+1);
  strncpy(nam,name,lename);
  nam[lename] = '\0';

  fm = (char *) MALLOC(lenfmt+1);
  for(i=0; i<lenfmt; i++) fm[i] = ' ';
  fm[lenfmt] = '\0';

  bosNgetformat(jw,nam,fm);

  bcopy((char *)fm, (char *)fmt, lenfmt);
 
  FREE(fm);
  FREE(nam);
}
 
#endif

int bosNgetformat(int *jw, void *name, char *fmt)
{
  BOSptr b;

  int *w, i, id, *nam, nama, nami, nch, lenfmt;
  char *ch;

  b = (BOSptr)jw;
  w = jw - 1;
  nam =  (int *)name;
  nama = *nam;

  lenfmt = strlen(fmt);
  nami = mamind(jw,nama);

  for(i=0; i<lenfmt; i++) fmt[i] = ' ';

  if((id = w[b->idfmt + nami - b->s->nsyst]) != 0)
  {
    if(w[id+3] != 0)
    {
      if(lenfmt < 3) return(-1);
      if(w[id+3] == 1) {fmt[0]= 'B'; fmt[1]= '3'; fmt[2]= '2';}
      if(w[id+3] == 2) {fmt[0]= 'B'; fmt[1]= '1'; fmt[2]= '6';}
      if(w[id+3] == 3) {fmt[0]= 'B'; fmt[1]= '0'; fmt[2]= '8';}
    }
    else
    {
      ch = (char *)&w[id+4];
      nch = (strlen(ch) < lenfmt) ? strlen(ch) : lenfmt;
      bcopy((char *)ch, (char *)fmt, nch);
    }
  }

  return(0);
}



E 1
