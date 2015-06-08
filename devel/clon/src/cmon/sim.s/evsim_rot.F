C======================================================================
      SUBROUTINE evsim_rot(idir,the,phi, p)    
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Rotation of a 3-vector: {x,y,z} <---> {x',y',z'}
C-
C-   Inputs  : the, phi  are angles of new z' axis defined
C-                       in master reference frame (x,y,z).
C-             idir = 1  Rotation from (x',y',z') to (x,y,z) reference frame.
C-             idir =-1  Rotation from (x,y,z) to (x',y',z') reference frame.
C-             p(3)      is 3-vector {Px,Py,Pz} or any 3-vector {x,y,z} 
C- 
C-   Outputs : p(3)
C-
C-   Controls: None
C-
C-   Library belongs: libsda.a
C-
C-   Created: 16-SEPT-1989   Bogdan Niczyporuk
C-
C-   Called by ? ... 
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Input variables
      INTEGER idir
      REAL    the, phi, p(3)
C local variables
      INTEGER j
      REAL    ROT(3,3), pv(3)  
C
      IF(the**2+phi**2 .GT. 1E-20) THEN   
        ROT(1,1) =  COS(the)*COS(phi)  
        ROT(1,2) = -SIN(phi)  
        ROT(1,3) =  SIN(the)*COS(phi)  
        ROT(2,1) =  COS(the)*SIN(phi)  
        ROT(2,2) =  COS(phi)   
        ROT(2,3) =  SIN(the)*SIN(phi)  
        ROT(3,1) = -SIN(the)  
        ROT(3,2) =  0. 
        ROT(3,3) =  COS(the)
C   
        DO j = 1,3    
          pv(j) = p(j)
        ENDDO    
        DO j = 1,3    
          IF(idir.GE.0) THEN
            p(j) = ROT(j,1)*pv(1) + ROT(j,2)*pv(2) + ROT(j,3)*pv(3)
          ELSE
            p(j) = ROT(1,j)*pv(1) + ROT(2,j)*pv(2) + ROT(3,j)*pv(3)
          ENDIF 
        ENDDO
      ENDIF
C
      RETURN    
      END   
