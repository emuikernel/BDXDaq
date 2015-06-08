      SUBROUTINE TRACEQ (LUNP,LEV)
C
C CERN PROGLIB# N105    TRACEQ          .VERSION KERNFOR  4.23  891215
C ORIG.  4/01/89  JZ
C
*     unit number for standard output
      PARAMETER    (LUNSTP=6)

      LUN = LUNP
      IF (LUN.EQ.0)  LUN = LUNSTP
      WRITE (LUN,9001)
cboy      RETURN
 9001 FORMAT (/' TRACEQ: EXIT !!!')
      STOP
      END
