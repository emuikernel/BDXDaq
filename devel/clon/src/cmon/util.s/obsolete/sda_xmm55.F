C======================================================================
      SUBROUTINE sda_xmm55(A,B,C)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Fast multiplication of 5X5 matreces: A*B -> C
C-                         (B and C may well be the same matrix).
C-
C-   Inputs  : A(25), B(25)
C-   Outputs : C(25)
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Author: A. Haas (Freiburg University)
C-   Modified:  17-OCT-1990   Bogdan Niczyporuk
C-
C-
C-   Called by SDA_TMAT
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
      REAL A(25),B(25),C(25)
C Local variables
      INTEGER J1, J2, J3, J4, J5
      REAL    B1J,B2J,B3J,B4J,B5J
C
      DO 10 J5=5,25,5
        B5J=B(J5)
        J4=J5-1
        B4J=B(J4)
        J3=J4-1
        B3J=B(J3)
        J2=J3-1
        B2J=B(J2)
        J1=J2-1
        B1J=B(J1)
        C(J1)=A( 1)*B1J+A( 6)*B2J+A(11)*B3J+A(16)*B4J+A(21)*B5J
        C(J2)=A( 2)*B1J+A( 7)*B2J+A(12)*B3J+A(17)*B4J+A(22)*B5J
        C(J3)=A( 3)*B1J+A( 8)*B2J+A(13)*B3J+A(18)*B4J+A(23)*B5J
        C(J4)=A( 4)*B1J+A( 9)*B2J+A(14)*B3J+A(19)*B4J+A(24)*B5J
        C(J5)=A( 5)*B1J+A(10)*B2J+A(15)*B3J+A(20)*B4J+A(25)*B5J
   10 CONTINUE
C
      RETURN
      END


