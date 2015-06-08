/*
     Convert 1 (IEEE) to 4 (DECS)

        Convert the data of array (IARR(I), I=1...NWRD)
        using the format code      IFMT(J), J=1...NFMT)
     If NSKIP not zero, then NSKIP words have been converted with the
     given format before and format use has to start with word NSKIP+1.
*/

#include <stdio.h>

#define LSHFT(value,bits) ( (value) << (bits) )
#define RSHFT(value,bits) ( (value) >> (bits) )
#define AND(x1,x2)        ( (x1) & (x2) )
#define OR(x1,x2)         ( (x1) | (x2) )
#define MOD(x1,x2)        ( (x1) - (int)((x1)/(x2)) * (x2) )
#define MIN(a,b)          ( (a) < (b) ? (a) : (b) )

void cnvr14_(int *iarr, int *nwrd, int *nskip, int *ifmt, int *nfmt)
{
  static int lv[10][3],mskip,imt,ib,ncnf,kcnf,lev,iterm,ia,i,jarr;

  if(*nwrd <= 0) return;
  /* initialize format use */
  imt = 0;
  ib = 0;
  mskip = *nskip;
  if(*nfmt == 1)
  {
    /* only one item */
    ncnf = 999999999;
    kcnf = MOD(ifmt[0],10);
  }
  else
  {
    /* more than one item */
    ncnf = 0;
    lev  = 0;
    iterm = 0;
  }
  goto a15;
  /* get next format code */

a5:

  imt++;
  if(imt > *nfmt)
  {
    /* end of format stmt reached, back to last parenthesis */
    imt = iterm;
    goto a5;
  }
  if(ifmt[imt-1] == 0)
  {
    /* right parenthesis */
    lv[lev-1][2] ++;
    if(lv[lev-1][2] >= lv[lev-1][1])
    {
      iterm = lv[lev-1][0] - 1;
      lev--;
    }
    else
    {
      imt = lv[lev-1][0];
    }
  }
  else
  {
    ncnf = ifmt[imt-1] / 10;
    kcnf = ifmt[imt-1]-10*ncnf;
    if(kcnf == 0)
    {
      /* left parenthesis */
      lev++;
      lv[lev-1][0] = imt;
      lv[lev-1][1] = ncnf;
      lv[lev-1][2] = 0;
    }
    else
    {
      /* format F or I or A or */
      if(imt != (*nfmt)-1 || lev == 0) goto a15;
      if(imt != lv[lev-1][0]+1)     goto a15;
      /* speed up case of repeated format at the end */
      ncnf = 999999999;
      goto a15;
    }
  }
  goto a5;

a15:

  if(ncnf == 0) goto a5;
  if(mskip > 0)
  {
    /* still some words to skip */
    if(mskip >= ncnf)
    {
      /* end of skipping not yet reached */
      mskip = mskip - ncnf;
      /* ncnf = 0; */
      goto a5;
    }
    else
    {
      /* end of skipping reached */
      ncnf = ncnf - mskip;
      mskip = 0;
    }
  }
  /* define limits for next conversion and update counter */
  ia = ib + 1;
  ib = MIN((*nwrd),ib+ncnf);
  ncnf = ncnf-ib+ia-1;
  /* Convert words IA...IB according to type KCNF */
  if(kcnf == 1)
  {
    /* Floating point 32 bits format */
    ;
  }
  else if(kcnf == 2)
  {
    /* Integer word format */
    ;
  }
  else if(kcnf == 3)
  {
    /* Hollerith */
    for(i=ia; i<=ib; i++)
    {
      jarr = iarr[i-1];
      iarr[i-1] = OR( OR( OR(LSHFT(jarr,24),RSHFT(jarr, 24)),
                  LSHFT( AND(jarr,65280),8)),
                  RSHFT( AND(jarr,16711680), 8));
    }
  }
  else if(kcnf == 4)
  {
    /* Integer 16 bits */
    for(i=ia; i<=ib; i++) iarr[i-1] =
                                 OR(LSHFT(iarr[i-1],16),RSHFT(iarr[i-1], 16));
  }
  else if(kcnf == 5)
  {
    /* Integer 8 bits */
    for(i=ia; i<=ib; i++)
    {
      jarr = iarr[i-1];
      iarr[i-1] = OR( OR( OR(LSHFT(jarr,24),RSHFT(jarr, 24)),
                    LSHFT( AND(jarr,65280),8)),
                    RSHFT( AND(jarr,16711680), 8));
    }
  }
  else if(kcnf == 6)
  {
    /* Integer 32 bits */
    ;
  }

  if(ib < *nwrd) goto a15;

  return;
}
