/*DECK ID>, BOSLNAME. */
/*
     RETURN N-TH NAME OF LIST
*/
 
#include "bos.h"
 
#ifndef VAX
 
int
nlist_(int *jw, int *n, char *list, int lenlist)
{
  char *lst;
  int status;
 
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  status = bosLname(jw,*n,lst);
  FREE(lst);
 
  return(status);
}
 
int
boslname_(int *jw, int *n, char *list, int lenlist)
{
  char *lst;
  int status;
 
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  status = bosLname(jw,*n,lst);
  FREE(lst);
 
  return(status);
}
 
#endif
 
int
bosLname(int *jw, int n, char *list)
{
  BOSptr b;
  int *w, jls, nls, ils, nn, nami;
  int status, *st;
  char *ch = "    ";
 
  w = jw-1;
  b = (BOSptr)jw;
  st = (int *)ch;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  status = *st;
  if(n <= 0) return(status);
  nn = 0;
 
  listn1(jw,list,&jls,&nls);
  ils = 0;
a:
  nami = listn2(jw,list,&ils,&jls,&nls);
 
  if(nami == 0) return(status);
  nn++;
  if( n != nn) goto a;
  status = w[b->idnam + nami - b->s->nsyst];
 
  return(status);
}
 
