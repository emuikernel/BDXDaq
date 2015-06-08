C======================================================================
      SUBROUTINE sda_snake(X,F)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods: Performs 3-dim 2-nd order interpolation for each 
C-                        component of the magnetic field using Taylor series. 
C-
C-   Inputs  : X(3) -space point and Magnetic Field Table (see "sdabgrid.inc")
C-   Outputs : F(3) -magnetic field at point X(3)
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Author:     P.Vernin     (Aug. 1987)
C-   Modified:   M.Guckes     (Oct. 1987)
C-               B.Niczyporuk (Feb. 1991)
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
      REAL X(3),F(3)
C
C Local variables
      INTEGER I,ii,NX3,NYNX3, IXM,IYM,IZM, BXFAC
      REAL    RDX,RDY,RDZ, AXM,AYM,AZM, PP,QQ,RR, XR,YR,ZR
      REAL    CP,CQ,CR, DP,DQ,DR,E
      REAL    F000,F00P,F00M,F0P0,F0M0,FP00,FM00,F0PP,FP0P,FPP0,FPPP
      LOGICAL LFIRST
      DATA    LFIRST /.TRUE./
C
C
C Each sector is symmetric in Y-coordinate (DST System)
C
      IF(X(2).LT.0.) THEN
        X(2) = -X(2)
        BXFAC = -1.
      ELSE
        BXFAC =  1.
      ENDIF
C
      IF(LFIRST) THEN
        LFIRST = .FALSE.
        RDX = 1./DX
        RDY = 1./DY
        RDZ = 1./DZ
        NX3   = NX*3
        NYNX3 = NY*NX*3
      ENDIF
C
C Find intercepts.
C
      XR  = (X(1)-XMIN)*RDX
      AXM = AINT(XR)
      IXM = IFIX(AXM)
      IF(IXM.LT.1 .OR. IXM+2.GT.NX) GO TO 100
      PP  = XR-AXM
C
      YR  = (X(2)-YMIN)*RDY
      AYM = AINT(YR)
      IYM = IFIX(AYM)
      IF(IYM.LT.1 .OR. IYM+2.GT.NY) GO TO 100
      QQ  = YR-AYM
C
      ZR  = (X(3)-ZMIN)*RDZ
      AZM = AINT(ZR)
      IZM = IFIX(AZM)
      IF(IZM.LT.1 .OR. IZM+2.GT.NZ) GO TO 100
      RR  = ZR-AZM
C
C 3-dim 2nd order interpolation inside TORUS for each component of the field.
C We use 11 values of the field:


c      call start_timer()
c      do ii=1,10000


      DO 22 I=1,3
        F000= Q((IZM  )*NYNX3 + (IYM  )*NX3 + (IXM  )*3 + I)
        F00P= Q((IZM+1)*NYNX3 + (IYM  )*NX3 + (IXM  )*3 + I)
        F00M= Q((IZM-1)*NYNX3 + (IYM  )*NX3 + (IXM  )*3 + I)
        F0P0= Q((IZM  )*NYNX3 + (IYM+1)*NX3 + (IXM  )*3 + I)
        F0M0= Q((IZM  )*NYNX3 + (IYM-1)*NX3 + (IXM  )*3 + I)
        FP00= Q((IZM  )*NYNX3 + (IYM  )*NX3 + (IXM+1)*3 + I)
        FM00= Q((IZM  )*NYNX3 + (IYM  )*NX3 + (IXM-1)*3 + I)
        F0PP= Q((IZM+1)*NYNX3 + (IYM+1)*NX3 + (IXM  )*3 + I)
        FP0P= Q((IZM+1)*NYNX3 + (IYM  )*NX3 + (IXM+1)*3 + I)
        FPP0= Q((IZM  )*NYNX3 + (IYM+1)*NX3 + (IXM+1)*3 + I)
        FPPP= Q((IZM+1)*NYNX3 + (IYM+1)*NX3 + (IXM+1)*3 + I)
        CP = FP00+FM00-2.*F000
        CQ = F0P0+F0M0-2.*F000
        CR = F00P+F00M-2.*F000
        DP = F000-FP00+FPP0-F0P0
        DQ = F000-F0P0+F0PP-F00P
        DR = F000-F00P+FP0P-FP00
        E  = -DP-F0PP+F00P-FP0P+FPPP
C Compute the Taylor's series:
        F(I)= F000+0.5*(PP*(FP00-FM00)+PP*PP*CP
     1                 +QQ*(F0P0-F0M0)+QQ*QQ*CQ
     2                 +RR*(F00P-F00M)+RR*RR*CR)
     3                 +PP*QQ*DP+QQ*RR*DQ+PP*RR*DR+PP*QQ*RR*E
 22   CONTINUE


c      enddo
c      call stop_timer(1)


      F(1) = BXFAC*F(1)
      F(3) = BXFAC*F(3)
      GO TO 999
C
C Point is outside of the grid
 100  CONTINUE
      F(1) = 0.
      F(2) = 0.
      F(3) = 0.
C
 999  CONTINUE
C Restore the original Y-coordinate
      X(2) = BXFAC*X(2)
C
      RETURN
      END





