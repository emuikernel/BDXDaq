h02976
s 00016/00006/00055
d D 1.3 01/11/19 15:51:26 boiarino 4 3
c minor
c 
e
s 00008/00001/00053
d D 1.2 01/10/30 11:36:07 boiarino 3 1
c add nlist_() for backward compartibility
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/boslname.c
e
s 00054/00000/00000
d D 1.1 00/08/10 11:10:03 boiarino 1 0
c date and time created 00/08/10 11:10:03 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, BOSLNAME. */
/*
     RETURN N-TH NAME OF LIST
*/
 
#include "bos.h"
 
#ifndef VAX
 
D 4
int boslname_(int *jw, int *n, char *list, int lenlist)
E 4
I 4
int
nlist_(int *jw, int *n, char *list, int lenlist)
E 4
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
 
D 4
#endif
 
E 4
D 3
int bosLname(int *jw, int n, char *list)
E 3
I 3
int
D 4
nlist_(int *jw, int *n, char *list)
E 4
I 4
boslname_(int *jw, int *n, char *list, int lenlist)
E 4
E 3
{
I 3
D 4
  return( bosLname(jw, *n, list) );
E 4
I 4
  char *lst;
  int status;
 
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  status = bosLname(jw,*n,lst);
  FREE(lst);
 
  return(status);
E 4
}
D 4

E 4
I 4
 
#endif
 
E 4
int
bosLname(int *jw, int n, char *list)
{
E 3
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
 
E 1
