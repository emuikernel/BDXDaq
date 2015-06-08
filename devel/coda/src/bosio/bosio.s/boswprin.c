
/*DECK ID>, BOSWPRIN. */
/*
 print named or work bank
*/
 
#include "bos.h"
 
#ifndef VAX
 
void boswprint_(int *jw, int *id)
{
  bosprint(jw,*id);
}
 
#endif
 
void bosWprint(int *jw, int id)
{
  bosprint(jw,id);
}
 
#include <stdio.h>
 
int bosprint(int *jw, int id)
{
  BOSptr b;
  int *w, i, j, ifmt, n1, n3, nwrd, np, nami;
  float *f;
  char *ch;
  unsigned int   *b32;
  unsigned short *b16;
  unsigned char  *b08;
 
  w = jw-1;
  b = (BOSptr)jw;
  f = (float *)w;
 
  if(b->ick != MAGIC) return(ARRAY_NOT_INITIALIZED);
  if(b->s->lub <= 0) return(0);
  if(id <= 0) return(0);
  if(id < b->iwk)	/* named bank */
  {
    if(bosNlink(jw,(char *)&w[id-INAM],w[id-INR]) != id) return(0);
    ch = (char *)&w[id-INAM];
    printf("\n N-bank %c%c%c%c",ch[0],ch[1],ch[2],ch[3]);
    printf(" number %5d with %6d I*4 data words",w[id-INR],w[id]);
    printf(" at index %6d\n",id);
    printf("       ( index to next bank %7d )\n\n",w[id-INXT]);
    nami = mamind(jw,w[id-INAM]);
    if((ifmt = w[b->idfmt+nami-b->s->nsyst]) == 0)	/* format undefined */
    {
      j = 1;
      bbufm(&w[id+1],j,w[id]);
      if(b->s->lub == 1) printf(" Print limit for bank printout reached.\n");
      (b->s->lub)--;
      return(0);
    }
    if(w[ifmt+3] == 0)	/* format codes (I,F,..) */
    {
      ch = (char *)&w[ifmt+4];
      for(i = 0; i < (w[ifmt]-3)*4; i++) if(ch[i] != 'A') goto a40;
      ch = (char *)&w[id+1];	/* only text */
      for(i = 0; i < w[id]*4; i++)
      {
        printf("%c",ch[i]);
        if((i%16) == 0) printf("\n");
      }
      if(b->s->lub == 1) printf(" Print limit for bank printout reached.\n");
      (b->s->lub)--;
      return(0);
a40:/* use format description */
      ch = (char *)&w[ifmt+4];
      j=0;
      while(ch[j] != ' ' && ch[j] != '\0' && j < (w[ifmt]-3)*4) j++;
      fprnn(&w[id+1],w[id],&w[ifmt+4],j);
    }
    else		/* format is B32 or B16 or B08 */
    {
      ifmt = w[ifmt+3];	/* 1 for B32, 2 for B16, 3 for B08 */
      n1 = 1;
      n3 = 5*ifmt;
      if(ifmt == 3) ifmt = 4;
      nwrd = ifmt * w[id];
      if(nwrd == 0)
      {
        if(b->s->lub == 1) printf(" Print limit for bank printout reached.\n");
        (b->s->lub)--;
        return(0);
      }
/* print loop */
      do
      {
        n3 = (n3 < (nwrd-n1+1)) ? n3 : (nwrd-n1+1);
        np = (n1+n3-1)%1000;
        if(ifmt == 1)
        {
          printf("%5d - %3d  ",n1,np);
          b32 = (unsigned int *)&w[id+1];
          for(j=0; j<n3; j++) printf("%12d",b32[j+n1-1]);
        }
        else if(ifmt == 2)
        {
          printf("%5d - %3d  ",n1,np);
          b16 = (unsigned short *)&w[id+1];
          for(j=0; j<n3; j++) printf("%6d",b16[j+n1-1]);
        }
        else if(ifmt == 4)
        {
          printf("%5d - %3d  ",n1,np);
          b08 = (unsigned char *)&w[id+1];
          for(j=0; j<n3; j++) printf("%4d",b08[j+n1-1]);
        }
        printf("\n");
        n1 = n1 + n3;
      }
      while(n1 < nwrd);
/* end of print loop */
    }
    if(b->s->lub == 1) printf(" Print limit for bank printout reached.\n");
    (b->s->lub)--;
    return(0);
  }
  else if(id != b->njw)		/* work bank */
  {
    ch = (char *)&w[id-INAM];
    printf("\n W-bank %c%c%c%c",ch[0],ch[1],ch[2],ch[3]);
    printf(" with %6d I*4 data words at index %6d\n",w[id],id);
    printf("       ( total length %7d )\n\n",w[id-INXT]);
    j = 1;
    bbufm(&w[id+1],j,w[id]);
    if(b->s->lub == 1) printf(" Print limit for bank printout reached.\n");
    (b->s->lub)--;
    return(0);
  }
 
  return(0);
}
 
fprnn(int *xw, int nwrd, char *ifmt, int nfmt)
{
  int i, ii, iterm, lev, n, ja, j, jb, m, l, ll, k, js, jp, lv[4][10];
  char line[79];
  char str[13];
  float *f;
 
  str[12] = '\0';
 
  if(nfmt == 0) return;
  line[78] = '\0';
  i = -1;
  iterm = -1;
  lev = 0;
  n = 0;
  ja = 1;	/* initialize printout */
 
  /* print one line */
  while(1 == 1)
  {
    j = ja;
    jb = ((j+4) < nwrd) ? (j+4) : nwrd;
    for(ii=0; ii<78; ii++) line[ii]=' ';	/* clean up line */
a20:
    if(n == 0) goto a50;
a30:
    m = (n < (jb-j+1)) ? n : (jb-j+1);
    n = n - m;
    for(l=1; l<=m; l++)
    {
      ll = 12*(j-ja+l) + 11;
      if(k == 1)	/* float */
      {
        f = (float *)&xw[j+l-2];
        sprintf(str,"%12f",*f);
        js = 12;
      }
      else if(k == 2)	/* int */
      {
        sprintf(str,"%12d",xw[j+l-2]);
        js = 12;
      }
      else if(k == 3)	/* char */
      {
        js = 4;
        bcopy((char *)&xw[j+l-2], (char *)&str[0], js);
      }
      bcopy((char *)&str[0], (char *)&line[ll-js], js);
    }
    j = j + m;
    if(j <= jb) goto a20;
    jp = jb % 1000;
    sprintf(str,"%6d",ja);
    bcopy((char *)&str[0], (char *)&line[0], 6);
    line[6] = '-';
    sprintf(str,"%3d",jp);
    bcopy((char *)&str[0], (char *)&line[7], 3);
    {int ii; for(ii=0; ii<78; ii++) printf("%c",line[ii]);printf("\n");}
    /*printf("%s\n",line);*/
    if(jb == nwrd) return;
    ja = jb + 1;
  }
 
a50:	/* get next format code */
 
  i++;
  if(i >= nfmt)
  {
    i = iterm;
    goto a50;
  }

  if(ifmt[i] == ',' || ifmt[i] == ' ') i++; /* skip comma or space */

  if(ifmt[i] == ')') /* right parenthesis */
  {
    lv[2][lev] ++;
    if(lv[2][lev] >= lv[1][lev])
    {
      iterm = lv[0][lev] - 1;
      lev--;
    }
    else
    {
      i = lv[0][lev];
    }
  }
  else
  {

    n = 0;
    while(isdigit(ifmt[i])) /* if digits - set n */
    {
      switch(ifmt[i])
      {
        case '0': m=0;
                  break;
        case '1': m=1;
                  break;
        case '2': m=2;
                  break;
        case '3': m=3;
                  break;
        case '4': m=4;
                  break;
        case '5': m=5;
                  break;
        case '6': m=6;
                  break;
        case '7': m=7;
                  break;
        case '8': m=8;
                  break;
        case '9': m=9;
                  break;
        default:  m=1;
      }
      n = n*10 + m;
      i++;
    }
    if(n==0) n=1;

    if(ifmt[i] == '(') /* left parenthesis */
    {
      k=0;
      lev++;
      lv[0][lev] = i;
      lv[1][lev] = n;
      lv[2][lev] = 0;
    }
    else
    {
      if(ifmt[i] == 'F') k = 1;
      if(ifmt[i] == 'I') k = 2;
      if(ifmt[i] == 'A') k = 3;
 
      if(i != (nfmt-1)) goto a30;
      if(lev == 0) goto a30;
      if(i != (lv[0][lev]+1)) goto a30;
      n = 999999999;
      goto a30;
    }
  }
  goto a50;
 
}
 
bbufm(int *arr, int i1, int i2)
{
  int i;
  float *f;
 
  f = (float *) arr;
  for(i=i1-1; i<i2; i++)
  {
    printf(" ind=%12d    int=%12d    float=%12f\n",i+1,arr[i],f[i]);
  }
  return;
}
 
