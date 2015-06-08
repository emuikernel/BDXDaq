h63319
s 00000/00000/00000
d R 1.2 01/11/19 19:06:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/trbos.c
e
s 00030/00000/00000
d D 1.1 01/11/19 19:06:02 boiarino 1 0
c date and time created 01/11/19 19:06:02 by boiarino
e
u
U
f e 0
t
T
I 1

/*
 trbos.c - filling output BOS bank

  input:  iw, ana

*/

#include <stdio.h>
#include "dclib.h"

int
trbos(int *jw, TRevent *ana)
{
  TRevent *anaptr;
  int nr, ncol, ind, i;

  nr = 0;
  ncol = (sizeof(TRevent)+3)/4;
  ind = etNcreate(jw,"TRAK",nr,ncol,1);
  /*printf("trbos: nr=%d ncol=%d nrow=%d -> ind=%d\n",nr,ncol,1,ind);*/
  if(ind <= 0)
  {/*printf("trbos: cannot create TRAK bank: %d %d -> return\n",ncol,1);*/
    return(-1);
  }
  anaptr = (TRevent *)&jw[ind];
  bcopy((char *)ana, (char *)anaptr, sizeof(TRevent));

  return(ind);
}
E 1
