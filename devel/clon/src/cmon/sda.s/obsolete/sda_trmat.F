C======================================================================
      SUBROUTINE sda_trmat(new,vect,vout,sl,bfld,tmat,iflag)
C----------------------------------------------------------------------
C-
C-    Purpose:  Calculates transport matrix for one step.
C-              Records matrix at point of layer penetration.
C-
C-    Inputs:  NEW = 1 TRANSMAT called 1-st time, otherwise: NEW = 0
C-             VECT(9)  -  vector {x,y,z,px/p,py/p,pz/p,p,m,Q} at begining step
C-             VOUT(9)  -  vector at end of step
C-             SL       -  Current step length
C-             BFLD(3)  -  Magnetic field: Bx,By,Bz
C-             IPL      -  Index of current layer (plane)
C-             IFLAG       = 1 track crossed a layer
C-                         = 0 not
C-    Output:  ERTRSP (5,5,nch)      - Transport matrix
C-  Controls:         
C-
C-  Library belongs: libsda.a
C-
C-     Calls: dcxmm55
C-
C-    Author:  S. Laubach, B. Niczyporuk (Jul. 1990)
C-    Modified by: B. Niczyporuk in October 1990 (see CEBAF-PR-91-004)
C-                 B. Niczyporuk on Dec.9, 1996 (when 1-st plane < 1-st step) 
C-
C-    Called by sda_swim
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C mgtyp
***#include "sdakeys.inc"
C sda_tmat
***#include "sdageom.inc"
C
      INTEGER new, iflag
      REAL    vect(9), vout(9), bfld(3), sl, tmat(5,5)
C
C Local variables
      INTEGER i,k
      REAL cfact,pm12,sinl,cosl,cosl1,sinp,cosp,hamx,fact,b0,b2,b3,tgl 
      REAL a(5,5),b(5,5), tn(3),hn(3),ch
C
      DATA cfact / 2.9979251E-4 /
C
C ERROR PROPAGATION ON A HELIX
C
      ch    = vect(9)
      tn(1) = vect(4) + vout(4)
      tn(2) = vect(5) + vout(5)
      tn(3) = vect(6) + vout(6)
      pm12  = 1. / SQRT(tn(1)**2 + tn(2)**2 + tn(3)**2)
      tn(1) = tn(1) * pm12
      tn(2) = tn(2) * pm12
      tn(3) = tn(3) * pm12
C
      sinl = tn(3)
      cosl = SQRT(ABS(1. - sinl**2))
      IF (cosl.EQ.0) THEN
        WRITE(6,*)'Particle travels in z-direction'
        GO TO 999
      ENDIF
      cosl1 = 1./cosl
      sinp  = tn(2) * cosl1
      cosp  = tn(1) * cosl1
C
C DEFINE TRANSFORMATION MATRIX BETWEEN X1 AND X2 FOR
C NEUTRAL PARTICLE OR FIELDFREE REGION
C
      DO i = 1,5
        DO k = 1,5
          a(i,k) = 0.
        ENDDO
        a(i,i) = 1.
      ENDDO
      a(4,3) = sl * cosl
      a(5,2) = sl
C
      IF(ch .EQ. 0.) GO TO 100
***      IF(mgtyp.EQ.5) GO TO 100
      pm12 = 2./(vect(7) + vout(7))
      hamx  = SQRT(bfld(1)**2 + bfld(2)**2 + bfld(3)**2)*pm12
      IF(hamx.LE.0.005) GO TO 100
C
C DEFINE AVERAGE MAGNETIC FIELD AND GRADIENT
C
      fact = ch*cfact
      hn(1) = bfld(1)*fact
      hn(2) = bfld(2)*fact
      hn(3) = bfld(3)*fact
C
      b0  =  hn(1)*cosp + hn(2)*sinp
      b2  = -hn(1)*sinp + hn(2)*cosp
      b3  = -b0*sinl + hn(3)*cosl
      tgl =  sinl*cosl1
C
C COMPLETE TRANSFORMATION MATRIX BETWEEN ERRORS AT X1 AND X2
C
      a(2,1) =  sl*b2
      a(2,3) = -b0*sl*pm12
      a(2,4) =  b2*b3*pm12*sl*pm12
      a(2,5) = -b2*b2*pm12*sl*pm12
C
      a(3,1) = -sl*b3*cosl1
      a(3,2) =  b0*sl*pm12*cosl1**2
      a(3,3) =  1. + tgl*b2*sl*pm12
      a(3,4) = -b3*b3*pm12*sl*pm12*cosl1
      a(3,5) =  b3*b2*pm12*sl*pm12*cosl1
C
      a(4,5) = -b3*tgl*sl*pm12
C
      a(5,4) =  b3*tgl*sl*pm12
C
  100 CONTINUE
C
C new = 0  transformation matrix is updated
C new = 1  transformation matrix is initialized
C
      IF(new.EQ.1) THEN
        new = 0
        DO i = 1,5
          DO k = 1,5
            b(i,k) = a(i,k)
          ENDDO
        ENDDO
C Already the track crossed VTX plane on the 1-st step (modified: Dec.9,1996)
        IF(iflag.EQ.1) THEN
          DO i = 1,5
            DO k = 1,5
              tmat(i,k) = a(i,k)
            ENDDO
          ENDDO
        ENDIF
        GO TO 999
      ENDIF
C
      IF(iflag.EQ.1) THEN
C Track have cross a layer (plane), store transport matrix tmat(5,5)
        CALL sda_xmm55(a,b,tmat(1,1))
      ELSE
C Not crossed, continue the matrix error propagation
        CALL sda_xmm55(a,b,b)
      ENDIF
C
  999 CONTINUE
      RETURN
      END
