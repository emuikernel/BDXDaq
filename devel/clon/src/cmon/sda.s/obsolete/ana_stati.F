C======================================================================
      SUBROUTINE ana_stati(icut,strg)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Initialize cut tallies.
C-
C-
C-   Inputs  : icut       - cut type
C-           : strg       - cut description
C-
C-   Outputs :
C-
C-   Controls:
C-
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created  JUL-20-1992  Billy Leung
C-
C-   Called by sda_init
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      INTEGER type, dtype
      LOGICAL ack
      CHARACTER*25 caption
C
      COMMON/ErrBook/ type(10), dtype(10), ack(10), caption(10)
C
C Subroutine parameters
      INTEGER icut
      CHARACTER *(*) strg
C
C Local variables
      INTEGER ifirst, i
      DATA ifirst /1/
C
C
      IF (ifirst .EQ. 1) THEN
        ifirst = 0
        DO i = 1,10
          ack(i)  = .FALSE.
          type(i) = 0
        ENDDO 
      ENDIF
C
      IF ((icut.GE.1) .AND. (icut.LE.10)) THEN
        ack(icut) = .TRUE.
        caption(icut) = strg
      ENDIF
C
      RETURN
      END
