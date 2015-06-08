h25745
s 00000/00000/00000
d R 1.2 01/11/19 19:05:13 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/evsim_boost.F
e
s 00044/00000/00000
d D 1.1 01/11/19 19:05:12 boiarino 1 0
c date and time created 01/11/19 19:05:12 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE evsim_boost(BETA,P)    
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Makes Lorentz Boost (Lab <--> CM)
C-
C-   Inputs  : P(4) is four vector (Px,Py,Pz,E) 
C-             BETA is CM velocity
C-                  If BETA < 0. Boost from Lab to center mass system.
C-                  If BETA > 0. Boost from center mass to Lab system.
C- 
C-   Outputs : P(4)
C-   Controls: None
C-
C-   Library belongs: libsda.a
C-
C-   Created : 16-SEPT-1989   Bogdan Niczyporuk
C-
C-
C-    Called by ? ...
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Input variables
      REAL BETA, P(4)
C
C Local variables  
      REAL*8 DP(4), DBETA, DGA 
C
      IF(BETA**2.GT.1E-20) THEN    
        DP(3) = P(3)   
        DP(4) = P(4)    
        DBETA = BETA    
        DGA=1D0/DSQRT(1D0 - DBETA**2)  
        P(3) = DGA*(DP(3) + DP(4)*DBETA)    
        P(4) = DGA*(DP(4) + DP(3)*DBETA) 
      ENDIF
C 
      RETURN    
      END   
E 1
