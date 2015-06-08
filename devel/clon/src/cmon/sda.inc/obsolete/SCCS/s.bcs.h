h42762
s 00000/00000/00000
d R 1.2 01/11/19 19:01:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/obsolete/bcs.h
e
s 00020/00000/00000
d D 1.1 01/11/19 19:01:19 boiarino 1 0
c date and time created 01/11/19 19:01:19 by boiarino
e
u
U
f e 0
t
T
I 1

/* Basic BOS array include file */

/* FORTRAN PROTOTYPE:

      INTEGER IPAD, IW, ndimb
      PARAMETER (ndimb = 700000)
      COMMON/BCS/ IPAD(5), IW(ndimb)
      REAL RW(ndimb)
      EQUIVALENCE (IW(1),RW(1))
*/

#define NBCS 700000

typedef struct boscommon
{
  int ipad[5];
  int iw[NBCS];
} BOScommon;

E 1
