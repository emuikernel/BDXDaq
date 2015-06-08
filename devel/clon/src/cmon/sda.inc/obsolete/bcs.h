
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

