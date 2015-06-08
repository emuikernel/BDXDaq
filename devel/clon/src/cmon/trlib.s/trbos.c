
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
