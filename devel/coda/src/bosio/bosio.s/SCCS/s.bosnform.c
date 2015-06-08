h30352
s 00005/00001/00178
d D 1.2 03/04/17 16:45:38 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosnform.c
e
s 00179/00000/00000
d D 1.1 00/08/10 11:10:04 boiarino 1 0
c date and time created 00/08/10 11:10:04 by boiarino
e
u
U
f e 0
t
T
I 1
/*
  bosnformat.c - define format of the bank 'name'
 
      1   0
      2   =3 DEFINED BY CALL
      3   BIT CODE (=1 B32, =2 B16, =3 B08)
      4...FMT CODES, IF BIT CODE = 0
 
      FMT CODE   translated from...
 
      N*10       N'('
             0    ')'
      N*10 + 1   N'F'          floating point
      N*10 + 2   N'I'          integer
      N*10 + 3   N'A'          text (4char per word)
      N*10 + 4   N'J'          16-bit integer
      N*10 + 5   N'K'          8-bit integer
*/
D 3
 
E 3
I 3

#include <stdio.h> 
#include <stdlib.h> 

E 3
#include "bos.h"
I 3
#include "bosio.h"
E 3
 
#ifndef VAX
 
void bosnformat_(int *jw, char *name, char *fmt, int lename, int lenfmt)
{
  char *nam, *fm;
 
  nam = (char *) MALLOC(lename+1);
  strncpy(nam,name,lename);
  nam[lename] = '\0';
  fm = (char *) MALLOC(lenfmt+1);
  strncpy(fm,fmt,lenfmt);
  fm[lenfmt] = '\0';
  bosNformat(jw,nam,fm);
  FREE(fm);
  FREE(nam);
}
 
#endif
 
int
bosNformat(int *jw, void *name, char *fmt)
{
  BOSptr b;
  int *w, *ifmt, nfmt, n, kf, lev, nr, l, id;
  int *ilst;
  char *fm;
  char *clst = "+FMT";
  int *nam, nama, nami;
 
  w = jw-1;
  b = (BOSptr)jw;
  ilst = (int *)clst;
  nam =  (int *)name;
  nama = *nam;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  nami = mamind(jw,nama);
 
  nfmt = strlen(fmt);		/* allocate nfmt+3 for ifmt */
  ifmt = (int *) MALLOC((nfmt+3)*sizeof(int));
 
  /* form array ifmt */
 
  ifmt[0] = w[b->idnam + nami - b->s->nsyst];
  ifmt[1] = 3;
  ifmt[2] = 0;
  n = 2;
 
  /* test special formats B32, B16 and B08 */
 
  kf = 0;
  if     (strncmp(fmt,"B32",3) == 0) kf = 1;
  else if(strncmp(fmt,"B16",3) == 0) kf = 2;
  else if(strncmp(fmt,"B08",3) == 0) kf = 3;
  else if(strncmp(fmt,"B8",2)  == 0) kf = 3;
  if(kf != 0)
  {
    ifmt[2] = kf;
    goto exit;
  }
 
 
 
/* following piece up to label exit lives just for format checking !!! */
 
 
  /* translate format */
 
  lev = 0;	/* left brackets counter - for checking purpose */
  nr = 0;	/* forming number */
 
  for(l=0; l<nfmt; l++)
  {
    switch(fmt[l])
    {
      case ' ': break;
 
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': if(nr < 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                nr = (int)fmt[l] - (int)'0' + 10*nr;
                break;
 
      case '(': if(nr < 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                lev++;
                n++;
                ifmt[n] = (1 > nr) ? 1 : nr ;
                ifmt[n] = 10*ifmt[n];
                nr = 0;
                break;
 
      case ')': if(nr >= 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                lev--;
                n++;
                ifmt[n] = 0;
                nr = -1;
                break;
 
      case ',': if(nr >= 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                nr = 0;
                break;
 
      default : kf = 0;
                if(fmt[l] == 'F') kf = 1;
                if(fmt[l] == 'I') kf = 2;
                if(fmt[l] == 'A') kf = 3;
                if(fmt[l] == 'J') kf = 4;
                if(fmt[l] == 'K') kf = 5;
                if(kf != 0)
                {
                  if(nr < 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                  n++;
                  ifmt[n] = (1 > nr) ? 1 : nr ;
                  ifmt[n] = kf + 10*ifmt[n];
                }
                else
                {
                  if(fmt[l] != ' ') { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
                }
                nr = -1;
    } /* switch */
  }
 
  if(lev != 0) { FREE(ifmt); return(MKFMT_BAD_FORMAT); }
 
exit:
 
  /* nfmt = n + 1;	 real length of the ifmt array */
 

  /* create temp buffer for 'fmt', will modify it !!! */
  fm = (char *) MALLOC(nfmt+1);

  strncpy(fm,fmt,nfmt);
  fm[nfmt] = '\0';
  nfmt++;
  bcopy((char *)fm, (char *)&ifmt[3], nfmt);
  FREE(fm);
  nfmt = 3 + (nfmt+3)/4;
 
  /* store in work bank */
 
  id = b->idfmt + nami - b->s->nsyst;
  wbanc(jw,&w[id],nfmt,1);
  w[w[id]-INAM] = *ilst;
  bcopy((char *)ifmt, (char *) &w[w[id]+1], nfmt<<2);
 
  FREE(ifmt);
  return(0);
}
 
E 1
