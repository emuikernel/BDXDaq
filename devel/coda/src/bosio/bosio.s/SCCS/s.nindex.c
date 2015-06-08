h57827
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/nindex.c
e
s 00031/00000/00000
d D 1.1 00/08/10 11:10:16 boiarino 1 0
c date and time created 00/08/10 11:10:16 by boiarino
e
u
U
f e 0
t
T
I 1
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
 
E 1
