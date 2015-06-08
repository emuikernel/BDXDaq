h06242
s 00000/00000/00000
d R 1.2 01/11/19 19:02:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_snake_pt.F
e
s 00118/00000/00000
d D 1.1 01/11/19 19:02:40 boiarino 1 0
c date and time created 01/11/19 19:02:40 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_snake_pt(X,F)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods: Performs 2-dim 2-nd order interpolation for each 
C-                        component of the magnetic field using Taylor series. 
C-
C-   Inputs  : X(3) -space point and Magnetic Field Table (see "sdabgrid.inc")
C-   Outputs : F(3) -magnetic field at point X(3)
C-   Controls:
C-
C-   Library belongs: libsda.a
C-  
C-   Called by: sda_helix
C-
C-   Calls: none
C-
C-   Author:     P.Vernin     (Aug. 1987)
C-               F.Klein      (Sep. 1998)
C-   Modified by B.Niczyporuk (Oct. 1998)
C- 
C- 
C-    Called by sda_helix
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdabgrid.inc"
C
C Subroutine variables
      REAL X(3), F(3)
C
C Local variables
      INTEGER I, IXM, IZM, MX3
      REAL    Xpmax, Zpmax, RDDXY, RDDZ, R, Bfac, Bx
      REAL    AXM,AZM, PP,RR, XR,ZR
      REAL    CP, CR, DR
      REAL    F000, F00P, F00M, FP00, FM00, FP0P
      LOGICAL LFIRST
      DATA    LFIRST /.TRUE./
C
C
C Each sector is symmetric in Y-coordinate (DST System)
C
      IF(LFIRST) THEN
        LFIRST = .FALSE.
        Xpmax = Xpmin + MX*DDX
        Zpmax = Zpmin + MZ*DDZ
        RDDXY = 1./DDX
        RDDZ  = 1./DDZ
        MX3   = MX*3
      ENDIF
C
C The Polarized Target Field TABLE was created for Z > 0
C
      IF(X(3).LT.0.) THEN
        X(3) = -X(3)
        Bfac = -1.
      ELSE
        Bfac =  1.
      ENDIF
C
      R = SQRT(X(1)*X(1) + X(2)*X(2))
      IF(R.LE.Xpmin .OR. R.GE.Xpmax) GO TO 100
      IF(X(3).LE.Zpmin  .OR. X(3).GE.Zpmax) GO TO 100
      XR  = (R - Xpmin)*RDDXY
      AXM = AINT(XR)
      IXM = IFIX(AXM)
      IF(IXM.LT.1 .OR. IXM.GT.MX-2) GO TO 100
      PP  = XR-AXM
C
      ZR  = (X(3)-Zpmin)*RDDZ
      AZM = AINT(ZR)
      IZM = IFIX(AZM)
      IF(IZM.LT.1 .OR. IZM.GT.MZ-2) GO TO 100
      RR  = ZR-AZM
C
C 2-dim 2nd order interpolation inside POLTARG for each component of the field.
C
      DO 11 i = 1,3
        IF(i.EQ.2) GO TO 11
        F000= Qm((IZM  )*MX3 + (IXM  )*3 + I)
        F00P= Qm((IZM+1)*MX3 + (IXM  )*3 + I)
        F00M= Qm((IZM-1)*MX3 + (IXM  )*3 + I)
        FP00= Qm((IZM  )*MX3 + (IXM+1)*3 + I)
        FM00= Qm((IZM  )*MX3 + (IXM-1)*3 + I)
        FP0P= Qm((IZM+1)*MX3 + (IXM+1)*3 + I)
        CP = FP00+FM00-2.*F000
        CR = F00P+F00M-2.*F000
        DR = F000-F00P+FP0P-FP00
        F(I)= F000+0.5*(PP*(FP00-FM00)+PP*PP*CP
     1                   +RR*(F00P-F00M)+RR*RR*CR)+PP*RR*DR
 11   CONTINUE
      X(3) = Bfac*X(3)
      Bx = Bfac*F(1)
C
C Rotation around Z
      IF(R .GT. 0.0001) THEN
        F(1) = Bx*X(1)/R
        F(2) = Bx*X(2)/R
      ENDIF
C
      RETURN
C
C Point is outside of the grid
 100  CONTINUE
      X(3) = Bfac*X(3)
      F(1) = 0.
      F(2) = 0.
      F(3) = 0.
C
      RETURN
      END


E 1
