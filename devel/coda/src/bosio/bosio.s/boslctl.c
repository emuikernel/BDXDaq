/*DECK ID>, BOSLCTL. */
/*
     opt = 'LO'  where L is 'C','E','R','S','T' and O is '=','+','-','*'
*/
 
#include "bos.h"
 
#ifndef VAX
 
void boslctl_(int *jw, char *opt, char *list, int lenopt, int lenlist)
{
  char *op, *lst;
 
  op = (char *) MALLOC(lenopt+1);
  strncpy(op,opt,lenopt);
  op[lenopt] = '\0';
  lst = (char *) MALLOC(lenlist+1);
  strncpy(lst,list,lenlist);
  lst[lenlist] = '\0';
  bosLctl(jw,op,lst);
  FREE(op);
  FREE(lst);
}
 
#endif
 
int bosLctl(int *jw, char *opt, char *list)
{
  BOSptr b;
  int *w, *ilst, il, ip, id, nl, nd, jls, nls, ils, i, nll, j, nami;
  char *clst = "+LST";

  /*printf("BLIST reached: opt >%s<   list >%s<\n",opt,list);*/

  w = jw-1;
  b = (BOSptr)jw;
  ilst = (int *)clst;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  b->s->jw1 = 0;
 
  switch(*opt)	/* check first option character */
  {
    case 'C': il = 0;
              break;
    case 'E': il = 1;
              break;
    case 'R': il = 2;
              break;
    case 'S': il = 3;
              break;
    case 'T': il = 4;
              break;
    default : il = -1;
  }
 
  switch(*(opt+1))	/* check second option character */
  {
    case '=': ip = 1;
              break;
    case '+': ip = 2;
              break;
    case '-': ip = 3;
              break;
    default : ip = 0;
  }

  if(il < 0 || ip == 0) return(BLIST_WRONG_ARGUMENT);


  if((ip == 1 || ip == 2) && *list == '*') /* add all existing banks to specified list */
  {

    /* drop, if exist, work bank for specified list */
    if(b->ilt[il] != 0) bosWdrop(jw,&b->ilt[il]);

    /* how many banks we have */
    nd = w[b->idnam];

    /* create work bank for specified list */
    if(bosWcreate(jw,&b->ilt[il],nd,1) < 0) return(INSUFFICIENT_SPACE);
    w[b->ilt[il]-INAM] = *ilst;

    /* fill work bank */
    for(i=1; i<=w[b->idnam]; i++)
    {
      w[b->ilt[il]+i] = mamind(jw,w[b->idnam+i]);
    }

    /* w[b->ilt[il]] = nd; */

    b->s->jw1 = 0;
    return(0);
  }


  /* work bank for list */
 
  if(b->ilt[il] == 0)
  {
    if(wbanc(jw,&b->ilt[il],0,1) < 0) return(INSUFFICIENT_SPACE);
    w[b->ilt[il]-INAM] = *ilst;
  }
 
  /* what option we have ? */

  id = b->ilt[il];  /* index of work bank for specified list */

  nl = w[id];       /* the number of words in this bank (useful words !?) */

  if(ip == 1)       /* create new list of names */
  {
    ip = 2;
    nl = 0;
  }
  nd = w[id-INR] - NHW;  /* == nl here, will be incremented below */

  listn1(jw,list,&jls,&nls);
  ils = 0;
a:
  nami = listn2(jw,list,&ils,&jls,&nls); /* return name-index for next name in list of names, or zero */

  if(nami == 0)
  {
    w[id] = nl;
    b->s->jw1 = 0;
    return(0);
  }

 
  if(ip == 2)	/* add new name to list of names */
  {
    for(i=1; i<=nl; i++) if(w[i+id] == nami) goto a; /* looking for name in specified list */
                                                     /* if already there, goto next name in list of names */

    if(nl == nd)		/* if bank of names is full, create new one */
    {
      w[id] = nl;
      nd = nd + 10;
      if(wbanc(jw,&b->ilt[il],nd,1) < 0) return(INSUFFICIENT_SPACE);
      id = b->ilt[il];
    }

    /* check how many names we have already; should not exceed sys.names */
    if(nl >= b->s->names)
    {
      printf("the number of bank names %d exceeded - call bnames_() to inrease\n",b->s->names);
      exit(1);
	}

    nl++;        		/* increment the number of words */
    w[id+nl] = nami;		/* put name in the list */
    goto a;
  }

  if(ip == 3)	/* delete name from list of names */
  {
    nll = nl;
    for(i=1; i<=nll; i++)	/* looking for given name in the list */
    {
      if(w[i+id] == nami)	/* if exist, delete it */
      {
        nl --;
        for(j=i; j<=nl; j++) w[j+id] = w[j+id+1];
        goto a;
      }
    }
    goto a;
  }

  return(BLIST_WRONG_ARGUMENT);

}
 
