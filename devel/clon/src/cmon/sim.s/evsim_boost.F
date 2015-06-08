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
