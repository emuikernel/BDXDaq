C======================================================================
      SUBROUTINE ana_mvprod(A,B,C,M,N)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Symmetric Matrix by Vector: C(j) --> A(i,j)xB(i)
C-
C-   Inputs  : N = Number of points (=< 36), M = number of parameters (=5)
C-   Outputs : C(M)
C-   Controls:
C-
C-   Library belongs: libana.a
C-
C-   Calls          : none
C-
C-   Created   25-OCT-1990   Bogdan Niczyporuk
C-
C-   Called by ana_trfit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
      INTEGER M, N
      REAL    A(M,N), B(N), C(M)
C Local variables
      INTEGER I, J
C
      DO I=1,M
        C(I) = 0.
        DO J=1,N
          C(I) = C(I) + A(I,J)*B(J)
        ENDDO
      ENDDO
C
      RETURN
      END
