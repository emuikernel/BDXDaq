/* Second BOS array include file */

/*
      INTEGER JPAD, JW, jndimb
      PARAMETER (jndimb = 700000)
C
      COMMON/WCS/ JPAD(5), JW(jndimb)
      REAL RJW(jndimb)
      EQUIVALENCE (JW(1),RJW(1))
C
      save /WCS/
*/

#define JNDIMB 700000

typedef struct wcscommon
{
  int jpad[5];
  int jw[JNDIMB];
} WCScommon;

WCScommon wcs_;
