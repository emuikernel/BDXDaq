h06508
s 00000/00000/00000
d R 1.2 01/11/19 19:05:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sgaus.F
e
s 00038/00000/00000
d D 1.1 01/11/19 19:05:14 boiarino 1 0
c date and time created 01/11/19 19:05:14 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      FUNCTION SGAUS(IDUM)
C----------------------------------------------------------------------
C-
C- Purpose and Methods : Gaussion Random Numbers (from Numerical Recipes).
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Function parameters
      INTEGER IDUM
      REAL    SRAN, SGAUS
C
C Local parameters
      INTEGER ISET
      REAL    V1, V2, R, FAC, GSET
      DATA ISET/0/
C
C
      IF(ISET.EQ.0) THEN
 1      V1 = 2.*SRAN(IDUM) - 1.
        V2 = 2.*SRAN(IDUM) - 1.
        R  = V1**2 + V2**2
        IF(R.GE.1. .OR. R.EQ.0.) GO TO 1
        FAC  = SQRT(-2.*LOG(R)/R)
        GSET = V1*FAC
        SGAUS= V2*FAC
        ISET = 1
      ELSE
        SGAUS= GSET
        ISET = 0
      ENDIF
C
      RETURN
      END
E 1
