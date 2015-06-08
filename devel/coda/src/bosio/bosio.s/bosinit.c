/*DECK ID>, BOSINIT. */
/*
	Initialization of the BOS structure in the global segment or
	in the allocated memory.
*/
 
#include <math.h>
#include "bos.h"
 
#ifndef VAX

void 
bosinit_(int *jw, int *ndim)
{
  bosInit(jw,*ndim);
}
 
#endif
 
static SYS sys;
static int sysnames = 100;

int 
bosInit(int *jw, int ndim)
{
  BOSptr b;
  int *w;
  char *lnk="+LNK";
  char *nam="+NAM";
  char *ptr="+PTR";
  char *idf="+IDF";
  char *fmt="+FMT";
  int *ilnk, *inam, *iptr, *iidf, *ifmt;
  int i, j, nama;
  static int ifmta[9] = {0,4,0,10,13,10,1000002,0,0};
  static int first=1;
 
  b = (BOSptr)jw;
  w = jw-1;
  ilnk = (int *)lnk;
  inam = (int *)nam;
  iptr = (int *)ptr;
  iidf = (int *)idf;
  ifmt = (int *)fmt;
  nama = *ifmt;

  /*printf("BOSINIT reached, jw=0x%08x ndim=%d\n",jw,ndim);*/

  if(ndim <= 0) return(berr("bosInit",BOS_INITIALIZATION_ERROR));
  ndim = ( ndim > 1000) ? ndim : 1000 ;
  if(first)	/* first call */
  {
    first = 0;
    sys.names = sysnames;			/* the number of the names */
    sys.lub = 2147483647;		    /* the number of banks to be printed */
/* prime number for HASH function */
    sys.nprim = sys.names + (sys.names%2) + 1;
a:  sys.nprim = sys.nprim - 2;
    j = (int) sqrt((double)sys.nprim);
    for(i=3; i<=j; i=i+2)
    {
      if((i*(sys.nprim/i)) == sys.nprim) goto a;
    }
    sys.nresr = 100;	/* the number of the reserved words */
    sys.nlplm = 40;   /* the number of banks of the same name for tree search */
                      /* ( if there are more than sys.nlplm bankks of the same
                           name, a work bank of indices/numbers will be created
                           to allow binary search in nlink/mlink ) */
    sys.narr = 0;
    for(i=0; i<100; i++) sys.iarr[i] = 0;
    sys.tleft = 0;	/* CALL TIMEL(tleft) was here in FORTRAN */
  }
  else		/* check wether the array is already initialized */
  {
    for(i=0; i<sys.narr; i++)
    {
      if((int)jw == sys.iarr[i])
      {
        printf("Array initialized already, address = %d\n",(int)jw);
        return(berr("bosInit",BOS_INITIALIZATION_ERROR));
      }
    }
  }
 
  sys.iarr[sys.narr] = (int)jw;		/* store location of the new array */
  sys.narr = sys.narr + 1;
  sys.nsyst = sizeof(BOS)/sizeof(int);	/* the number of system words */
  for(i=1; i<=ndim; i++) w[i] = 0;	/* clean up array */
 
  /* initialize array */
 
  b->s = &sys;			/* adddress of system structure */
  b->ick = MAGIC;		/* checking word */
  b->njw = ndim;		/* length of array */
  b->inm = sys.names + sys.nsyst + 1;	/* first index of named bank area */
  i = (int) &w[b->inm];		/* named banks on a page boundary (?) */
  j = ((i+127)/128)*128;
  b->inm = b->inm + j - i;
  b->igp = b->inm;		/* first index of gap */
  b->iwk = ndim + 1;		/* first index of work bank area */
  b->ign = ndim;		/* index of lowest deleted named bank */
 
  /* create system work bank for link indices */
 
  b->idl = 0;	/* index of link bank */
  if(bosWcreate(jw,&b->idl,sys.names,1) < 0)
                             return(berr("bosInit",BOS_ARRAY_LENGTH_TOO_SMALL));
  w[b->idl-INAM] = *ilnk;
 
  /* create system work banks ( was ONLY in basic array ) */
 
  /* work bank for names */
  if(bosWcreate(jw,&b->idnam,sys.names,1) < 0)
                             return(berr("bosInit",BOS_ARRAY_LENGTH_TOO_SMALL));
  w[b->idnam] = 0;
  w[b->idnam-INAM] = *inam;
  /* work bank for pointers */
  if(bosWcreate(jw,&b->idptr,sys.names+sys.nprim,1) < 0)
                             return(berr("bosInit",BOS_ARRAY_LENGTH_TOO_SMALL));
  w[b->idptr-INAM] = *iptr;
  /* work bank for bank format indices */
  if(bosWcreate(jw,&b->idfmt,sys.names,1) < 0)
                             return(berr("bosInit",BOS_ARRAY_LENGTH_TOO_SMALL));
  w[b->idfmt-INAM] = *iidf;
 
  if(bosWcreate(jw,&w[b->idfmt+1],9,1) < 0)
                             return(berr("bosInit",BOS_ARRAY_LENGTH_TOO_SMALL));
  w[w[b->idfmt+1]-INAM] = nama;
  ifmta[0] = nama;
  bcopy((char *)ifmta, (char *) &w[w[b->idfmt+1]+1], 36);	/* 36 = 9*4 */
 
  return(0);
}

void
bnames_(int *namax)
{
  bnames(*namax);
}
void
bosInames(int *jw, int maxnames)
{
  bnames(maxnames);
}
void
bnames(int namax)
{
  printf("bnames: %d\n",namax);
  sysnames = (10 > (namax)) ? 10 : (namax);
}

void
bnres_()
{
  bnres();
}
void
bnres()
{
  printf("bnres\n");
  exit(0);
  sys.nresr = sys.nresr + 100;
}

int 
berr(char *who, int error)
{
  printf("Error in BOS: routine %s, error %d\n",who,error);
  return(error);
}
 
