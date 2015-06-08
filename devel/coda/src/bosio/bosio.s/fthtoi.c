/*
     fthtoi.c - Translate FormaT from Hollerith TO Integer array
                ( takes about 25 mikro seconds )
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define RSHFT(value,bits) ( (value) >> (bits) )
#define AND(x1,x2)        ( (x1) & (x2) )
#define OR(x1,x2)         ( (x1) | (x2) )
#define MAX(a,b)          ( (a) > (b) ? (a) : (b) )

void
fthtoi_(int *IHFMT, int *IHF, int *IFMT, int *NFMT)
{
  static int N,IEFMT,IER,LNF,ICH,KF,LEV,NR,LI,L,IHP,NX;
  static char *FMT, FM, CHA[4];
  static char *digit;

  /* initialize array */

  IEFMT = 0;
  N = 0;

  /* translate format */

  IER = 0;
  LNF = 4 * (*IHF);
  ICH = IHFMT[0];

  /* get char*4 from hollerith 4H */

#if defined(Linux) || defined(SunOS_i86pc)
  sprintf(CHA,"%c%c%c%c",AND      (ICH    ,255),
                         AND(RSHFT(ICH, 8),255),
                         AND(RSHFT(ICH,16),255),
                         AND(RSHFT(ICH,24),255));
#else
  sprintf(CHA,"%c%c%c%c",AND(RSHFT(ICH,24),255),
                         AND(RSHFT(ICH,16),255),
                         AND(RSHFT(ICH, 8),255),
                         AND      (ICH    ,255));
#endif

  FMT = CHA;

  /* test special formats */

  if(LNF >= 1)
  {
    if(FMT[0] == 'B' || FMT[0] == 'b')
    {
      /* B... code */
      KF = 0;
      if(LNF >= 2)
      {
        if(FMT[1] == '8')  KF = 5;
      }
      if(KF == 0 && LNF >= 3)
      {
        if(FMT[1] == '0' && FMT[2] == '8') KF = 5;
        if(FMT[1] == '1' && FMT[2] == '6') KF = 4;
        if(FMT[1] == '3' && FMT[2] == '2') KF = 6;
      }
      if(KF != 0)
      {
        IFMT[0] = KF;
        *NFMT = 1;
        return;
      }
    }
  }
  LEV = 0;
  NR = 0;

  /* loop over all characters */

  LI = 0;
  IHP = 1;
  for(L=1; L <= 4 * (*IHF); L++)
  {
    LI ++;
    if(LI == 5)
    {
      LI = 1;
      IHP ++;
      ICH = IHFMT[IHP-1];

      /* get char*4 from hollerith 4H */

#if defined(Linux) || defined(SunOS_i86pc)
      sprintf(CHA,"%c%c%c%c",AND      (ICH    ,255),
                             AND(RSHFT(ICH, 8),255),
                             AND(RSHFT(ICH,16),255),
                             AND(RSHFT(ICH,24),255));
#else
      sprintf(CHA,"%c%c%c%c",AND(RSHFT(ICH,24),255),
                             AND(RSHFT(ICH,16),255),
                             AND(RSHFT(ICH, 8),255),
                             AND      (ICH    ,255));
#endif

      FMT = CHA;
    }

    FM = FMT[LI-1];
    if(FM == ' ') goto a10;

    if(isdigit(FM))  /* a number */
    {
      if(NR < 0) IER ++;
      digit = &FM;
      NX = atoi(digit);
      NR = NX + 10*MAX(0,NR);
      goto a10;
    }
    else if(FM == '(')      /* a left parenthesis            NR/0 */
    {
      if(NR < 0) IER ++;
      LEV ++;
      N ++;
      IFMT[N-1] = 10*MAX(1,NR);
      NR = 0;
      goto a10;
    }
    else if(FM == ')')      /* a right parenthesis           0/0 */
    {
      if(NR >= 0) IER ++;
      LEV --;
      N ++;
      IFMT[N-1] = 0;
    }
    else if(FM == ',')      /* a komma */
    {
      if(NR >= 0) IER ++;
      NR = 0;
      goto a10;
    }
    else                    /* a F or I or A or J or K or Z ... */
    {

      if(FM == 'F' || FM == 'f') KF = 1;
      else if(FM == 'I' || FM == 'i') KF = 2;
      else if(FM == 'A' || FM == 'a') KF = 3;
      else if(FM == 'J' || FM == 'j') KF = 4;
      else if(FM == 'K' || FM == 'k') KF = 5;
      else if(FM == 'Z' || FM == 'z') KF = 6;
      else if(FM == 'C' || FM == 'c') KF = 7;
      else KF = 0;

      if(KF != 0)
      {
        if(NR < 0) IER ++;
        N ++;
        IFMT[N-1] = KF + 10*MAX(1,NR);
      }
      else
      {
        /* illegal character */
        IER ++;
      }
    }
    NR = -1;

a10:
    ;
  }

  if(LEV != 0) IER ++;
  if(IER != 0) IEFMT = -1;
  *NFMT = N;
  if(IEFMT != 0)
  {
    IFMT[0] = 9;
    *NFMT = 1;
  }

  return;
}
