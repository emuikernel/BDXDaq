C======================================================================
      FUNCTION SRAN(IDUM)
C----------------------------------------------------------------------
C-
C- Purpose and Methods : Flat Random Numbers (from Numerical Recipes:"RAN2").
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Function parameters
      INTEGER IDUM
      REAL    SRAN
C
C Local parameters
      INTEGER M, IA, IC
      REAL RM
      PARAMETER (M=714025, IA=1366, IC=150889, RM=1.4005112E-6)
C
C Local variables
      INTEGER J, IY, IFF, IR(97)
      DATA IFF /0/
C
C
      IF(IDUM.LT.0 .OR. IFF.EQ.0) THEN
        IFF = 1
        IDUM = MOD(IC-IDUM,M)
        DO 11 J=1,97
          IDUM = MOD(IA*IDUM+IC,M)
          IR(J) = IDUM
 11     CONTINUE
        IDUM = MOD(IA*IDUM+IC,M)
        IY = IDUM
      ENDIF
C
      J = 1 + (97*IY)/M
      IF(J.GT.97 .OR. J.LT.1) PAUSE
      IY   = IR(J)
      SRAN = IY*RM
      IDUM = MOD(IA*IDUM+IC,M)
      IR(J)= IDUM
C
      RETURN
      END
