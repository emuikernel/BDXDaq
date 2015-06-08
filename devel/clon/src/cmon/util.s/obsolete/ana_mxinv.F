C======================================================================
      SUBROUTINE ana_mxinv(ARRAY,NORDER,DET)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Invert a symmetric matrix ARRAY and calculate
C-                         and calculate its determinant DET
C-
C-   Inputs  : Norder, ARRAY(Norder,Norder)
C-   Outputs : Inverted ARRAY and its DET
C-   Controls:
C-
C-   Library belongs: libana.a
C-
C-   Calls          : none
C-
C-   Created   25-OCT-1990   Bogdan Niczyporuk
C-   Source    "IBM System/360 Scientific Subroutine Package"
C-
C-    Called by ana_trfit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
      INTEGER NORDER
      REAL    ARRAY(5,5), DET
C Local variables
      INTEGER I,J,K,L, IK(10), JK(10)
      REAL    AMAX, SAVE
C
      DET=1.
      DO 100 K=1,NORDER
C
C       FIND LARGEST ELEMENT ARRAY(I,J) IN REST OF MATRIX
C
        AMAX=0.
   21   DO 30 I=K,NORDER
          DO 30 J=K,NORDER
            IF(ABS(AMAX)-ABS(ARRAY(I,J))) 24,24,30
C       IF(DABS(AMAX)-DABS(ARRAY(I,J)))  24,24,30
   24       AMAX=ARRAY(I,J)
            IK(K)=I
            JK(K)=J
   30   CONTINUE
C
C       INTERCHANGE ROWS AND COLUMNS TO PUT AMAX IN ARRAY(K,K)
C
        IF(AMAX)  41,32,41
   32   DET=0.
        GOTO 140
   41   I=IK(K)
        IF(I-K)  21,51,43
   43   DO 50 J=1,NORDER
          SAVE=ARRAY(K,J)
          ARRAY(K,J)=ARRAY(I,J)
   50   ARRAY(I,J)=-SAVE
   51   J=JK(K)
        IF(J-K)  21,61,53
   53   DO 60 I=1,NORDER
          SAVE=ARRAY(I,K)
          ARRAY(I,K)=ARRAY(I,J)
   60   ARRAY(I,J)=-SAVE
C
C       ACCUMULATE ELEMENTS OF INVERSE MATRIX
C
   61   DO 70 I=1,NORDER
          IF(I-K)  63,70,63
   63     ARRAY(I,K)=-ARRAY(I,K)/AMAX
   70   CONTINUE
        DO 80 I=1,NORDER
          DO 80 J=1,NORDER
            IF(I-K)  74,80,74
   74       IF(J-K)  75,80,75
   75       ARRAY(I,J)=ARRAY(I,J)+ARRAY(I,K)*ARRAY(K,J)
   80   CONTINUE
        DO 90 J=1,NORDER
          IF(J-K)  83,90,83
   83     ARRAY(K,J)=ARRAY(K,J)/AMAX
   90   CONTINUE
        ARRAY(K,K)=1./AMAX
  100 CONTINUE
C 100    DET=DET*AMAX
C
C       RESTORE ORDERING OF MATRIX
C
      DO 130 L=1,NORDER
        K=NORDER-L+1
        J=IK(K)
        IF(J-K)  111,111,105
  105   DO 110 I=1,NORDER
          SAVE=ARRAY(I,K)
          ARRAY(I,K)=-ARRAY(I,J)
  110   ARRAY(I,J)=SAVE
  111   I=JK(K)
        IF(I-K)  130,130,113
  113   DO 120 J=1,NORDER
          SAVE=ARRAY(K,J)
          ARRAY(K,J)=-ARRAY(I,J)
  120   ARRAY(I,J)=SAVE
  130 CONTINUE
C
  140 RETURN
      END


