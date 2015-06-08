h44049
s 00000/00000/00000
d R 1.2 01/11/19 19:00:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/wcs.h
e
s 00022/00000/00000
d D 1.1 01/11/19 19:00:41 boiarino 1 0
c date and time created 01/11/19 19:00:41 by boiarino
e
u
U
f e 0
t
T
I 1
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
E 1
