h10752
s 00000/00000/00000
d R 1.2 01/11/19 19:12:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/obsolete/ana_mvprod.F
e
s 00035/00000/00000
d D 1.1 01/11/19 19:12:43 boiarino 1 0
c date and time created 01/11/19 19:12:43 by boiarino
e
u
U
f e 0
t
T
I 1
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
E 1
