/*
     Translate FormaT from Character TO Integer array
     ( takes about 25 mikro seconds )

     fmt code   translated from ..
     N*10       N'('
             0    ')'
     N*10 + 1   N'F'   floating point
     N*10 + 2   N'I'   integer
     N*10 + 3   N'A'   hollerith character (4)
     N*10 + 4   N'J'   16-bit integers (2)
     N*10 + 5   N'K'   8-bit integers (4)
     N*10 + 6   N'Z'   32-bit integers
          + 7   N'C    character
          + 8          last integer (stop flag) ?
          + 9          error flag               ?
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX(a,b)          ( (a) > (b) ? (a) : (b) )

void ftctoi_(char *FMT, int *IFMT, int *NFMT, int LNF)
{
  static int IEFMT, N, IER, KF, LEV, NR, L, NX;
  static char FM;
  static char *digit;

  /* initialize array */

  IEFMT = 0;
  N = 0;

  /* translate format */

  IER = 0;

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

  for(L=1; L<=LNF; L++)
  {
    FM = FMT[L-1];

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
