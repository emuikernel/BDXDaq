/*DECK ID>, NINDEX. */
/*
     RETURNS INDI = INDEX OF BANK WITH NAME-INDEX NAMI AND NUMBER NR
                    OR ZERO
             INDJ = PREVIOUS INDEX (JW(INDJ-INXT) CONTAINS INDI)
*/
 
#include "bos.h"
 
int nindex(int *jw, int nr, int nami, int *indi, int *indj)
{
  BOSptr b;
  int *w, nlpl;
 
  b = (BOSptr)jw;
  w = jw-1;
 
    *indi = nami + 1;
    nlpl = 0;
a:  *indj = *indi;
    *indi = w[(*indj)-INXT];
    if((*indi) != 0)
    {
      nlpl++;
      if(w[(*indi)-INR] < nr) goto a;
      if(w[(*indi)-INR] > nr) *indi = 0;
    }
 
    return(nlpl);
}
 
