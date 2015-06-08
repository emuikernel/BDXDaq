C======================================================================
      SUBROUTINE ana_statp
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Prints No. of events left after each cut.
C-
C-
C-   Inputs  :
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
C-   Called by sda_run
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
C External
      INTEGER iucomp
C
      INTEGER type, dtype
      LOGICAL ack
      CHARACTER*25 caption
C
      COMMON/ErrBook/ type(10), dtype(10), ack(10), caption(10)
C
C Local variables
      INTEGER i, inter, ifirst
      DATA ifirst /1/
C
C
      IF(ifirst.EQ.1) THEN
        inter = 0
        IF(iucomp(4HINT ,lmode,1) .NE. 0) inter = 1
        ifirst = 0
      ENDIF
C
      i = 1
      DO WHILE ((.NOT.ack(i)).AND. (i.LE.10))
        i = i + 1
      ENDDO
      IF(i .LE. 10) THEN
        IF(inter.EQ.1) THEN
          WRITE(6,11)
        ELSE
          WRITE(6,12)
        ENDIF
      ENDIF
C
C Report tallied analysis failures
      DO i = 1,10
        IF (ack(i)) THEN
          IF(inter.EQ.1) THEN
            WRITE(6,13) i, caption(i), zcut(i), type(i), dtype(i)
            dtype(i) = 0
          ELSE
            WRITE(6,13) i, caption(i), zcut(i), type(i)
          ENDIF
        ENDIF
      ENDDO
C
      RETURN
 11   FORMAT(//1X,'Cut#',1X,'Description of cut',7X,' Value','    Passed                        
     1  Yes/No'/)
 12   FORMAT(//1X,'Cut#',1X,'Description of cut',7X,' Value','    Passed                
     1  '/)
 13   FORMAT(1X,I4,1X,A25,F6.1,I10,I6)
C
      END


