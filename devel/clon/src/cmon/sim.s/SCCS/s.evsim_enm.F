h30851
s 00000/00000/00000
d R 1.2 01/11/19 19:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/evsim_enm.F
e
s 00204/00000/00000
d D 1.1 01/11/19 19:05:13 boiarino 1 0
c date and time created 01/11/19 19:05:13 by boiarino
e
u
U
f e 0
t
T
I 1
C========1=========2=========3=========4=========5=========7=========7==        
      SUBROUTINE evsim_enm(Pbeam,Q2min,Q2max,Wmin,Wmax,THmin,THmax)         
C-----------------------------------------------------------------------
C-
C-   Purpose and Methods : Generates  e- + p -> e- + K + L
C-                                 or e- + p -> e- + pi+ n
C-
C-   Inputs  : Pbeam, Q2min, Q2max, Wmin, Wmax, TH*min, TH*max
C- 
C-   Outputs : PL {Px,Py,Pz,E,m} for each particle in the event
C-
C-   Controls: None
C-
C-   Created   16-FEB-1993   Bogdan Niczyporuk
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
C Subroutine variables
      REAL Pbeam, Q2min,Q2max, Wmin,Wmax, THmin,THmax
C
C External Functions
      REAL SRAN
C
C Local variables
      INTEGER j,k, ifirst
      REAL AME,AMP, AM1,AM2, Q2,W,t,tp,THS,PHS,Eps,Anu
      REAL R1,RR,a1,a2,a3, t_min,t_max, EE,PE, THE,CTHE,PHE, E1S,P1S,E2S        
      REAL PQX,PQY,PQZ, P_Q,E_Q,THE_Q,PHI_Q,BETA_Q, Ppik,cospik,THpik
      REAL QQ2, THq,PHIq, Ptot,THETA,PHI, THSr
      REAL pi,twopi,raddeg, SIG,SIGmax
C
      DATA ifirst  /1/
      DATA AME,AMP /0.000511, 0.938272/
C
      iFermi = 0
C
      IF(ifirst.EQ.1) THEN
        SIGmax  = 1./(Q2min*Q2min)
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        raddeg = 180./pi
        ifirst  = 0
      ENDIF
C
C Final particles
      R1 = SRAN(iseed)
      IF( R1.LT.zkine(10) ) THEN
        AM1 = 0.14000
        AM2 = 0.93956
      ELSE
        AM1 = 0.49365
        AM2 = 1.11560
      ENDIF
      

 10   CONTINUE
C
C Select Q2 value
C
      IF(Q2max.LE.Q2min) THEN
        Q2 = Q2min
      ELSE
C Calculate d(SIGMA)/d(Q2) = 1./Q**4
        Q2 = Q2min + SRAN(iseed)*(Q2max - Q2min)
        SIG = 1./(Q2*Q2)
        IF(SIGmax*SRAN(iseed).GT.SIG) GO TO 10
      ENDIF
C
C Select W value
C
      IF(Wmax.LE.Wmin) THEN
        W = Wmin
      ELSE
        W = Wmin + SRAN(iseed)*(Wmax - Wmin)
      ENDIF
C
C Calculate of Epsilon
C
      Ebeam = SQRT(Pbeam*Pbeam + AME*AME)
      Anu = (W*W + Q2 - AMP*AMP)/(2.*AMP)
      IF(Anu .LE. 0.) GO TO 10
      RR = 4.*Ebeam*(Ebeam - Anu) 
      Eps = (RR - Q2)/(RR + 2.*Anu**2 + Q2)
C
C Calculation tmin, tmax
C
      a1 = (-Q2 -AM1**2 -AMP**2 + AM2**2)/(2.*W)
      a2 = SQRT( ((W**2 - Q2 -   AMP**2)/(2.*W))**2 + Q2 )
      a3 = SQRT( ((W**2 +AM1**2 -AM2**2)/(2.*W))**2 - AM1**2 )
      t_min = (a2 - a3)**2 - a1**2
      t_max = (a2 + a3)**2 - a1**2 
C
C Info on electron in Lab
C             
      EE = Ebeam - Anu
      IF(EE .LE. 0.1) GO TO 10
      PE = SQRT(EE*EE - AME*AME)
        CTHE = (2.*EBEAM*EE - 2.*AME*AME - Q2)/(2.*Pbeam*PE)
        IF(ABS(CTHE) .GE. 1.) GO TO 10
        THE = ACOS(CTHE)
        PHE = TWOPI*SRAN(iseed)
      PL(1,1) = PE*SIN(THE)*COS(PHE)
      PL(2,1) = PE*SIN(THE)*SIN(PHE)
      PL(3,1) = PE*COS(THE)
      PL(4,1) = EE
      PL(5,1) = AME
C      
C    Info on virtual photon: P_Q, E_Q, THE_Q, PHI_Q
C
      PQX =       - PL(1,1)
      PQY =       - PL(2,1)
      PQZ = Pbeam - PL(3,1)
      P_Q = SQRT(PQX*PQX + PQY*PQY + PQZ*PQZ)
      E_Q = Ebeam - PL(4,1)
      THE_Q = ACOS (ABS(PQZ)/P_Q)
      IF(PQZ .LT. 0.) THE_Q = PI - THE_Q
      PHI_Q = ATAN2 (PQY,PQX)
      IF(PHI_Q .LT. 0.) PHI_Q = PHI_Q + TWOPI
      BETA_Q = P_Q/( E_Q + AMP )
C
C Info on AM1 in Resonance CM system
C
      IF(THmax.LE.THmin) THEN
        THS = THmin
      ELSE
        THS = THmin + (THmax - THmin)*SRAN(iseed)
      ENDIF
      PHS = TWOPI*SRAN(iseed)
      E1S = (W*W + AM1*AM1 - AM2*AM2)/(2.*W)
      IF(E1S .LE. AM1) GO TO 10
      P1S = SQRT(E1S*E1S - AM1*AM1)
C Get Pion(Kaon) in CM
        PL(1,2) = P1S*SIN(THS)*COS(PHS)
        PL(2,2) = P1S*SIN(THS)*SIN(PHS)
        PL(3,2) = P1S*COS(THS)
        PL(4,2) = E1S
        PL(5,2) = AM1
C Neutron(Lambda) in CM
        E2S = W - E1S
        IF(E2S .LE. AM2) GO TO 10
        PL(1,3) = -PL(1,2)
        PL(2,3) = -PL(2,2)
        PL(3,3) = -PL(3,2)
        PL(4,3) = E2S
        PL(5,3) = AM2
C Get pion(kaon) and neutron(lambda) in Lab system
      CALL evsim_boost( BETA_Q, PL(1,2) )
       Ppik = SQRT(PL(1,2)**2 + PL(2,2)**2 + PL(3,2)**2)
       COSpik = PL(3,2)/Ppik
       THpik = 0.
       IF(ABS(COSpik).GE.1.) GO TO 10
       THpik = ACOS(COSpik)*raddeg      
      CALL evsim_rot(1, THE_Q, PHI_Q, PL(1,2) ) 
      CALL evsim_boost( BETA_Q, PL(1,3) ) 
      CALL evsim_rot(1, THE_Q, PHI_Q, PL(1,3) )
Ccalculation of t
       t = 2.*AMP*PL(4,3) - AMP*AMP - AM2*AM2
       tp= t - t_min
       IF(tp.LE.0.) GO TO 10
C
C Store variables in COMMON
      Qsqr   = Q2
      Wmass  = W
      Ttr    = tp
      THstar = THS
      PHstar = PHS
C
      IF(isw(1).EQ.2 .AND. ievt.LE.Ndbg) THEN
        THq = THE_Q*RADDEG
        PHIq= PHI_Q*RADDEG
        WRITE(6,100) Ebeam
        QQ2 = -Q2
        WRITE(6,101) PQX,PQY,PQZ,E_Q,QQ2, P_Q,THq,PHIq
        WRITE(6,*) '  '
        DO K=1,3
          PTOT = SQRT(PL(1,K)*PL(1,K)+PL(2,K)*PL(2,K)+PL(3,K)*PL(3,K))
          THETA = ACOS ( ABS(PL(3,K))/PTOT )*RADDEG
          IF(PL(3,K) .LT. 0.) THETA = 180. - THETA
          PHI = ATAN2( PL(2,K),PL(1,K) )*RADDEG
          IF(PHI .LT. 0.) PHI = PHI + 360.
          WRITE(6,101) (PL(J,K),J=1,5),PTOT,THETA,PHI
        ENDDO
        THSr =THS*RADDEG
        WRITE(6,102) THpik,THSr, t,Wmass, t_min,t_max,a1,a2,a3
      ENDIF
C
 100  FORMAT(/' Virtual Photon Info for Ebeam =',F8.2)
 101  FORMAT(' PL,P,TH,PH=',5F8.4,1X,F8.4,2F8.2)
 102  FORMAT(/' TH,THcm, t,W, tmin,tmax =',3(1X,2F8.4)/
     1        ' a1,a2,a3                =',3F9.4)

 999  CONTINUE
      RETURN 
      END
E 1
