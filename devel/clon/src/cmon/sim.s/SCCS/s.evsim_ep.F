h63287
s 00000/00000/00000
d R 1.2 01/11/19 19:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/evsim_ep.F
e
s 00111/00000/00000
d D 1.1 01/11/19 19:05:13 boiarino 1 0
c date and time created 01/11/19 19:05:13 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE evsim_ep(Pbeam,Q2min,Q2max)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Generates  e- + p -> e- + p + p event
C-
C-   Inputs  : Pbeam, Q2min, Q2max
C- 
C-   Outputs : PL {Px,Py,Pz,E,m} for each particle in the event
C-
C-   Controls: None
C-
C-   Created    2-FEB-1993   Bogdan Niczyporuk
C-
C-
C-    Called by usda_evtgen
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaevgen.inc"
C
C External Functions
      REAL SRAN
C
C Subroutine parameters
      REAL Pbeam, Q2min, Q2max
C
      INTEGER j,k, ifirst
      REAL Sig, Sigmax, Q2, Enu, Pe,Ee,THe,CTHe,PHe, phi,Ptot,theta
      REAL pi,twopi,raddeg, AME,AMP
C
      DATA AME,AMP /0.000511, 0.938272/
      DATA ifirst /1/
C
C
      IF(ifirst.EQ.1) THEN
        Ebeam = SQRT(Pbeam*Pbeam + AME*AME)
        SIGmax  = 1./(Q2min*Q2min)
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        raddeg = 180./pi
        ifirst  = 0
      ENDIF
C
 10   CONTINUE
C
C Select Q2 value
C
      IF(Q2max.LE.Q2min) THEN
        Q2 = Q2min
      ELSE
        Q2 = Q2min + SRAN(iseed)*(Q2max - Q2min)
C Calculate d(SIGMA)/d(Q2) = 1./Q**4
        SIG = 1./(Q2*Q2)
        IF(SIGmax*SRAN(iseed).GT.SIG) GO TO 10
      ENDIF
C
C Info on electron in Lab
C             
      Enu = Q2/(2.*AMP)
      IF(Enu .LE. 0.) GO TO 10
      Ee = Ebeam - Enu
      Pe = SQRT(Ee*Ee - AME*AME)
        CTHe = (2.*Ebeam*Ee - 2.*AME*AME - Q2)/(2.*Pbeam*Pe)
        IF(ABS(CTHe) .GT. 1.) GO TO 10
        THe = ACOS(ABS(CTHe))
        IF(CTHe.LT.0.) THe = pi - THe
        PHe = twopi*SRAN(iseed)
C Electron
      PL(1,1) = Pe*SIN(THe)*COS(PHe)
      PL(2,1) = Pe*SIN(THe)*SIN(PHe)
      PL(3,1) = Pe*COS(THe)
      PL(4,1) = Ee
      PL(5,1) = AME
C Proton
      PL(1,2) =           - PL(1,1)
      PL(2,2) =           - PL(2,1)
      PL(3,2) =     Pbeam - PL(3,1)
      PL(4,2) = AMP+Ebeam - PL(4,1)
      PL(5,2) = AMP
C
C Store variables in COMMON
      Qsqr    = Q2
      Wmass   = AMP
      Ttr     = 0.
      THstar  = 0.
      PHstar  = 0.
C
C Small debugging
      IF(isw(1).EQ.2 .AND. ievt.LE.Ndbg) THEN
        DO k = 1,2
          Ptot = SQRT(PL(1,K)*PL(1,K)+PL(2,K)*PL(2,K)+PL(3,K)*PL(3,K))
          theta = ACOS ( ABS(PL(3,K))/Ptot )*raddeg
          IF(PL(3,K) .LT. 0.) theta = 180. - theta
          phi = ATAN2( PL(2,K),PL(1,K) )*raddeg
          IF(phi .LT. 0.) phi = phi + 360.
          WRITE(6,101) (PL(J,K),J=1,5),Ptot,theta,phi
        ENDDO
        WRITE(6,102) Qsqr,Wmass,Ttr,THstar,PHstar
      ENDIF
C
 101  FORMAT(' PL,P,TH,PH=',5F8.4,1X,F8.4,2F8.2)
 102  FORMAT(' Q2,W,t,TH,PH =',5F10.4)
C
      RETURN 
      END
E 1
