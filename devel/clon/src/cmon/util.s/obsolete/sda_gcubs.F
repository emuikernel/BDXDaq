C======================================================================
      SUBROUTINE sda_gcubs(X,Y,D1,D2,A)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates a cubic through P1,(-X,Y1),(X,Y2),P2
C-                         where Y2=-Y1
C-                         Y = A(1) + A(2)*X + A(3)*X**2 + A(4)*X**3
C-                         The coordinate system is assumed to be the cms
C-                         system of P1,P2.
C-   Inputs  : X,Y,D1,D2
C-   Outputs : A(4)
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Author:   H. Boerner
C-   Modified: 1-OCT-1990   Bogdan Niczyporuk
C-
C-
C-    Called by SDA_CYL, SDA_PLN
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      REAL X,Y,D1,D2,A(4)
C
C Local variables
      REAL FACT
C
C
      IF (X.EQ.0.) GO TO 10
C
      FACT   = (D1 - D2) * 0.25
      A(1)   = - 1. * FACT * X
      A(3)   = FACT / X
      A(2)   = (6. * Y - (D1 + D2) * X) / (4. * X)
      A(4)   = ((D1 + D2)*X - 2.*Y) / (4.*X**3)
      RETURN
C
   10 A(1)   = 0.
      A(2)   = 1.
      A(3)   = 0.
      A(4)   = 0.
C
      RETURN
      END
