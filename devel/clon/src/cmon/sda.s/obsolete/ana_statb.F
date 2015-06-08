C======================================================================
      SUBROUTINE ana_statb(icut)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : keep tallies on different types of
C-                         analysis cuts
C-
C-
C-   Inputs  : icut       - cut type
C-
C-   Outputs :
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created  JUL-20-1992  Billy Leung
C-
C-   Called by ana_xxxx routines
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
      INTEGER type, dtype
      LOGICAL ack
      CHARACTER*25 caption
C
      COMMON/ErrBook/ type(10), dtype(10), ack(10), caption(10)
C
C Subroutine parameters
      INTEGER icut
C
C Local variables
      INTEGER i, ievt_old, ival
C
      IF(icut .EQ. 1) THEN
        ival = 0
        IF(ievt.GT.ievt_old) ival = 1
      ENDIF
C
      DO i = 1,10
        IF( (icut .EQ. i) .AND. ack(icut) ) THEN
          dtype(i) = 1
          type(i) = type(i) + ival
        ENDIF
      ENDDO
C
      IF(icut .EQ. 1) ievt_old = ievt
C
      RETURN
      END


