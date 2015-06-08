/*DECK ID>, BOSNPRIN. */
/*
 print named bank
*/
 
#include "bos.h"
 
#ifndef VAX
 
void
bosnprint_(int *jw, char *name, int *nr, int lenname)
{
  char *nam;
 
  nam = (char *) MALLOC(lenname+1);
  strncpy(nam,name,lenname);
  nam[lenname] = '\0';
  bosNprint(jw,nam,*nr);
  FREE(nam);
}
 
#endif
 
void
bosNprint(int *jw, char *name, int nr)
{
  BOSptr b;
  int ind;
 
  b = (BOSptr)jw;
 
  if(b->s->lub > 0)
  {
    if((ind=bosNlink(jw,name,nr)) > 0)
    {
      bosprint(jw,ind);
    }
    else
    {
      printf("\nbosNprint: can't print bank \"%4.4s\", number=%d : ind=%d\n\n",
                  name,nr,ind);
    }
  }
}
 
