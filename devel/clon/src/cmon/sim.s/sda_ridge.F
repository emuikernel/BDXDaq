C======================================================================
      SUBROUTINE sda_ridge(R,S1,S2,BT)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods: Calculate magnetic field in R of one straight 
C-                        element of current. The current flows from S1 to S2.
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Author:     P.Vernin  (Aug. 1987)
C-   Modified:   1-SEP-1990   Bogdan Niczyporuk
C-
C-    Called by sda_torus
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      REAL S1(3),S2(3),V1(3),V2(3),C(3),R(3),BT(3),U(3)
      REAL V1SQ,V2SQ,CSQ,COS1,COS2,USQ,FACT2,COEF
C
C
C V1: Vector to one end of line
      V1(1) = S1(1)-R(1)
      V1(2) = S1(2)-R(2)
      V1(3) = S1(3)-R(3)
C
C V1: Vector to other end of line
      V2(1) = S2(1)-R(1)
      V2(2) = S2(2)-R(2)
      V2(3) = S2(3)-R(3)
C
C C: The line itself
      C(1) = S2(1)-S1(1)
      C(2) = S2(2)-S1(2)
      C(3) = S2(3)-S1(3)
C
      V1SQ = V1(1)*V1(1) + V1(2)*V1(2) + V1(3)*V1(3)
      V2SQ = V2(1)*V2(1) + V2(2)*V2(2) + V2(3)*V2(3)
      CSQ  = C(1)*C(1) + C(2)*C(2) + C(3)*C(3)
      IF (CSQ.LE.0. .OR. V1SQ.LE.0. .OR. V2SQ.LE.0.) RETURN
C
C U: Vector product of C x V1
      CALL CROSS(C, V1, U)
C
      COS1  = (V1(1)*C(1)+V1(2)*C(2)+V1(3)*C(3)) / SQRT(V1SQ*CSQ)
      COS2  = (V2(1)*C(1)+V2(2)*C(2)+V2(3)*C(3)) / SQRT(V2SQ*CSQ)
      USQ   = U(1)*U(1) + U(2)*U(2) + U(3)*U(3)
      FACT2 = V1SQ*USQ*(1.-COS1*COS1)
      IF (FACT2.LE.0.) RETURN
      COEF  = (COS2-COS1) / SQRT(FACT2)
C
      BT(1) = BT(1)+COEF*U(1)
      BT(2) = BT(2)+COEF*U(2)
      BT(3) = BT(3)+COEF*U(3)
C
      RETURN
      END
