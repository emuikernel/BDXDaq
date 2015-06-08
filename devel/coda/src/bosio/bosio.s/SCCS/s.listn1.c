h14800
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/listn1.c
e
s 00053/00000/00000
d D 1.1 00/08/10 11:10:15 boiarino 1 0
c date and time created 00/08/10 11:10:15 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, LISTN1. */
/*
*/
 
#include "bos.h"
 
int listn1(int *jw, char *list, int *jls, int *nls)
{
  BOSptr b;
  int *w, lenls;
 
  w = jw-1;
  b = (BOSptr)jw;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  lenls = strlen(list);
  
  if(lenls == 0)
  {
    *jls = -1;
  }
  else if(lenls == 1)	/* list is name of list */
  {
    switch(*list)
    {
      case 'C': *jls = 0;
                break;
      case 'E': *jls = 1;
                break;
      case 'R': *jls = 2;
                break;
      case 'S': *jls = 3;
                break;
      case 'T': *jls = 4;
                break;
      case '0': *jls = -1;
                break;
      default : printf("listn1: ERROR list >%1.1s<\n",list);
                *jls = -1;
    }
    if(*jls >= 0) *nls = w[b->ilt[*jls]];
    else /*printf("NU I ..... ???\n")*/;
    
  }
  else			/* list is bank names */
  {
    *nls = lenls/4;
    *jls = 10;
  }
 
  return(0);
}
 
E 1
