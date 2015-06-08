h00529
s 00000/00000/00000
d R 1.2 01/11/19 19:02:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/user_sda.F
e
s 01070/00000/00000
d D 1.1 01/11/19 19:02:41 boiarino 1 0
c date and time created 01/11/19 19:02:41 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE usda_init
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : User initialization routine (begin of JOB)
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Created   1-SEP-1993   Bogdan Niczyporuk
C-
C-    Called by sda_init
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      RETURN
      END
C======================================================================
      SUBROUTINE usda_evtgen
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Here user may call his Event Generator.
C-                         This routine is called by SDA when the simulation
C-                         has been selected in the configuration file (see 
C-                         "sda.config" and "sdaevgen.inc" for further details)
C-
C-   Inputs  :
C- 
C-   Outputs : User fills the array {ev_in(9,it),it=1,ntrack}.
C_             For each track "it" the following values must be defined: 
C_             {X,Y,Z,Px/P,Py/P,Pz/P,P,m,Q} in the Detector Coordinate System 
C-             (DCS where a beam direction is along the Z-axis)
C-
C-   Controls:
C-
C-   Calls: evsim_ep  (+ user event generator routines)
C-
C-   Created   30-OCT-1990   Bogdan Niczyporuk
C-
C-    Called by sim_main
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdaevgen.inc"
C
C External Functions
      REAL SRAN, SGAUS
C
C Local variables
      INTEGER it, itmax, it_max, ifirst
C
      REAL Pmin,Pmax
C
C Other local variables
      REAL p, th, ph
      REAL pi, twopi, degrad, raddeg
C
      integer iw_
      real iwmin, iwmax
      real xmin,xmax,ymin,ymax,zmin,zmax
      real thetamin,thetamax,phimin,phimax
      real dx,dy,dz,dphi,dtheta,dp
C
      DATA ifirst /1/
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        degrad = pi/180.                     
        raddeg = 180./pi
      ENDIF
C
C      IF(zkine(1).EQ.x) THEN
C
C                 DC Wire position, Wire Direction & WireLen for layers: 1 - 36
C dc_wpmid(1,iw,il,isec) | along beam     |
C dc_wpmid(2,iw,il,isec) | vertical       |-> Wire position in the middle plane
C dc_wpmid(3,iw,il,isec) | along wire (0) |
C dc_wlen(1,iw,il,isec)  --> Half wire length (+z or +y AMPL,Reg.1 opposit)


c
        pmin = 0.01
        pmax = 1.0
        dp = 0.01


c wire number
        iwmin = 4.
        iwmax = 120.
        iw_ = INT(iwmin + (iwmax+1.-iwmin)*SRAN(iseed))
c        print *,'iw_=',iw_
c        print *,'?',dc_wpmid(1,iw_,1,1),dc_wpmid(2,iw_,1,1)
c vertical
        xmin = dc_wpmid(2,iw_,1,1)-0.5
        xmax = dc_wpmid(2,iw_,1,1)-0.5
        dx = 1.
c aling wire
        ymin = -dc_wlen(1,iw_,1,1)
        ymax = dc_wlen(1,iw_,1,1)
        dy = 5.
c aling beam
        zmin = dc_wpmid(1,iw_,1,1)-0.5
        zmax = dc_wpmid(1,iw_,1,1)-0.5
        dz = 1.
c
        thetamin = 1.
        thetamax = 140.
        dtheta = 1.
c
        phimin = -25.
        phimax = 25.
        dphi = 1.
c
c
c
        ev_in(1,1) = xmin + (xmax-xmin)*SRAN(iseed)
        ev_in(2,1) = ymin + (ymax-ymin)*SRAN(iseed)
        ev_in(3,1) = zmin + (zmax-zmin)*SRAN(iseed)
        ph = (phimin + (phimax-phimin)*SRAN(iseed))*degrad
        th = (thetamin + (thetamax-thetamin)*SRAN(iseed))*degrad
        ev_in(4,1) = SIN(th)*COS(ph)
        ev_in(5,1) = SIN(th)*SIN(ph)
        ev_in(6,1) = COS(th)
        ev_in(7,1) = pmin + (pmax-pmin)*SRAN(iseed)
        ev_in(8,1) = 0.0005
        ev_in(9,1) = 1.
        ntrack = 1
        
c        print *,'my:  p=', ev_in(7,1),'  theta=',TH*raddeg,'  phi=',PH*raddeg,'  x,y,z=',
c     &         ev_in(1,1),ev_in(2,1),ev_in(3,1)


C      ENDIF
C
      RETURN
      END
C======================================================================
      SUBROUTINE usda_brun
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This routine is called at the Begin of each Run.
C-                         Initialize your Histograms, ...
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Calls: user routines ?
C-
C-   Created    19-OCT-1995   Bogdan Niczyporuk
C-
C-   Called by sda_brun
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
C External functions
      INTEGER iucomp
C
      CALL HBOOK1(20001,' test $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20002,' test $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20003,' test $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20004,' test $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20005,' test $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20006,' test $', 150, 1.0, 151.0, 0.)
C
      CALL HBOOK1(20011,' fit $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20012,' fit $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20013,' fit $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20014,' fit $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20015,' fit $', 150, 1.0, 151.0, 0.)
      CALL HBOOK1(20016,' fit $', 150, 1.0, 151.0, 0.)
C
      CALL HBOOK1(20021,' p $', 150, 1.0, 151.0, 0.)
C
      CALL HBOOK1(17010,' delta-vect(1) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(17020,' delta-vect(2) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(17030,' delta-vect(3) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(17040,' delta-vect(4) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(17050,' delta-vect(5) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(17060,' delta-vect(6) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(17061,' delta-theta (degree)$', 100,  -50.,  50., 0.)
      CALL HBOOK1(17062,' delta-phi (degree)$', 100,  -50.,  50., 0.)
      CALL HBOOK1(17070,' (delta-vect(7))/vect(7) %% $', 100, -100.0,
     & 100.0, 0.)
      CALL HBOOK1(17071,' (delta-vect(7))/vect(7) %% (>1GeV) $', 100,
     & -100.0,  100.0, 0.)
      CALL HBOOK1(17072,' (delta-vect(7))/vect(7) %% (<1GeV and >0.3GeV)
     & $', 100,  -100.0,  100.0, 0.)
      CALL HBOOK1(17073,' (delta-vect(7))/vect(7) %% (<0.3GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(17080,' ntof-tof $', 10,  -5.0,  5.0, 0.)
      CALL HBOOK1(17091,' nu-nu $', 100,  -50.0,  50.0, 0.)
      CALL HBOOK1(17092,' nv-nv $', 100,  -50.0,  50.0, 0.)
      CALL HBOOK1(17093,' nw-nw $', 100,  -50.0,  50.0, 0.)
      CALL HBOOK1(17094,' e-e $', 100,  -2.5,  2.5, 0.)
      CALL HBOOK1(17095,' e-e $', 100,  -100.,  100., 0.)
      CALL HBOOK1(17096,' e-e $', 100,  -2.5,  2.5, 0.)
      CALL HBOOK1(17097,' e-e $', 100,  -100.,  100., 0.)
      CALL HBOOK2(17098,' ein vs eout $', 40,  0.0,  1.5, 40, 0.0,
     &  1.5, 0.)
      CALL HBOOK2(17099,' ein vs eout $', 40,  0.0,  1.5, 40, 0.0,
     &  1.5, 0.)
      CALL HBOOK1(17100,' ch-ch $', 10,  -5.0,  5.0, 0.)
      CALL HBOOK2(17101,' ein vs eout (-)$',40,0.0,1.5,40,0.0,1.5,0.)
      CALL HBOOK2(17102,' ein vs eout (+)$',40,0.0,1.5,40,0.0,1.5,0.)
C
      CALL HBOOK1(17200,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(17201,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(17202,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(17203,' Q2 $', 200, 0.0, 3.0, 0.)
      CALL HBOOK1(17204,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(17205,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(17206,' W2 $', 200, 0.0, 8.0, 0.)
C
      CALL HBOOK1(27010,' delta-vect(1) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(27020,' delta-vect(2) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(27030,' delta-vect(3) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(27040,' delta-vect(4) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(27050,' delta-vect(5) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(27060,' delta-vect(6) $', 100,  -0.2,  0.2, 0.)

      CALL HBOOK1(27070,' (delta-vect(7))/vect(7) %% $', 100,  -100.0,
     &  100.0, 0.)
      CALL HBOOK1(27071,' (delta-vect(7))/vect(7) %% (>1GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(27072,' (delta-vect(7))/vect(7) %% (<1GeV and >0.3GeV)
     & $', 100,  -100.0,  100.0, 0.)
      CALL HBOOK1(27073,' (delta-vect(7))/vect(7) %% (<0.3GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(27074,' (delta-vect(7))/vect(7) %% (<0.1GeV) $', 100,
     &  -100.0,  100.0, 0.)

      CALL HBOOK1(27075,' (delta-vect(7))/vect(7) %% $', 100,  -100.0,
     &  100.0, 0.)
      CALL HBOOK1(27076,' (delta-vect(7))/vect(7) %% (>1GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(27077,' (delta-vect(7))/vect(7) %% (<1GeV and >0.3GeV)
     & $', 100,  -100.0,  100.0, 0.)
      CALL HBOOK1(27078,' (delta-vect(7))/vect(7) %% (<0.3GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(27079,' (delta-vect(7))/vect(7) %% (<0.1GeV) $', 100,
     &  -100.0,  100.0, 0.)

      CALL HBOOK1(27080,' ntof-tof $', 10,  -5.0,  5.0, 0.)
      CALL HBOOK1(27091,' nu-nu $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(27092,' nv-nv $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(27093,' nw-nw $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(27094,' e-e $', 100,  -2.5,  2.5, 0.)
      CALL HBOOK1(27095,' e-e $', 100,  -100.,  100., 0.)
      CALL HBOOK2(27099,' ein vs eout $', 40,  0.0,  1.5, 40, 0.0,  1.5,
     & 0.)
      CALL HBOOK1(27100,' ch-ch $', 10,  -5.0,  5.0, 0.)
      CALL HBOOK2(27101,' ein vs eout (-)$',40,0.0,1.5,40,0.0,1.5,0.)
      CALL HBOOK2(27102,' ein vs eout (+)$',40,0.0,1.5,40,0.0,1.5,0.)
C
      CALL HBOOK1(27200,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(27201,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(27202,' W2 $', 200, 0.0, 8.0, 0.)
      CALL HBOOK1(27203,' Q2 $', 200, 0.0, 3.0, 0.)
      CALL HBOOK1(27204,' W2 $', 200, 0.0, 8.0, 0.)
C
      CALL HBOOK1(37010,' delta-vect(1) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(37020,' delta-vect(2) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(37030,' delta-vect(3) $', 100, -20.0, 20.0, 0.)
      CALL HBOOK1(37040,' delta-vect(4) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(37050,' delta-vect(5) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(37060,' delta-vect(6) $', 100,  -0.2,  0.2, 0.)
      CALL HBOOK1(37070,' (delta-vect(7))/vect(7) %% $', 100,  -100.0,
     &  100.0, 0.)
      CALL HBOOK1(37071,' (delta-vect(7))/vect(7) %% (>1GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(37072,' (delta-vect(7))/vect(7) %% (<1GeV and >0.3GeV)
     & $', 100,  -100.0,  100.0, 0.)
      CALL HBOOK1(37073,' (delta-vect(7))/vect(7) %% (<0.3GeV) $', 100,
     &  -100.0,  100.0, 0.)
      CALL HBOOK1(37080,' ntof-tof $', 10,  -5.0,  5.0, 0.)
      CALL HBOOK1(37091,' nu-nu $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(37092,' nv-nv $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(37093,' nw-nw $', 20,  -10.0,  10.0, 0.)
      CALL HBOOK1(37094,' e-e $', 100,  -2.5,  2.5, 0.)
      CALL HBOOK1(37095,' e-e $', 100,  -100.,  100., 0.)
      CALL HBOOK1(37100,' ch-ch $', 10,  -5.0,  5.0, 0.)
C
      IF(iucomp(4HBAT ,lmode,1).EQ.0) GO TO 999
C
C Initialize HBOOK Histograms
C
      IF(ABS(zkine(1)).EQ.1.) THEN    
C PR-fit, Sector Coordinate System (SCS)
        IF(lanal(1).LT.4) THEN
          CALL HBOOK1(1,' [V]^2! (PR)   $', 20,  0.0,10.0, 0.)
          CALL HBOOK1(2,' [D]p/p        $', 50, -0.5, 0.5, 0.)
          CALL HBOOK1(3,' [DQ]?S! (mr)  $', 50,-200.0,200.0, 0.)
          CALL HBOOK1(4,' [DF]?S! (mr)  $', 50, -50.0, 50.0, 0.)
          CALL HBOOK1(5,' [D]do?S! (cm) $', 40, -5.0, 5.0, 0.)
          CALL HBOOK1(6,' [D]zo?S! (cm) $', 40,-50.0,50.0, 0.)
        ELSE
C FF-fit, Sector Coordinate System (SCS)
          CALL HBOOK1(1,' [V]^2! (FF)   $', 40, 0.00,40.0, 0.)
          CALL HBOOK1(2,' [D]p/p        $', 80,-0.20,0.20, 0.)
          CALL HBOOK1(3,' [DQ]?S! (mr)  $', 80,-20.0,20.0, 0.)
          CALL HBOOK1(4,' [DF]?S! (mr)  $', 80,-20.0,20.0, 0.)
          CALL HBOOK1(5,' [D]do?S! (cm) $', 40,-1.00,1.00, 0.)
          CALL HBOOK1(6,' [D]zo?S! (cm) $', 50,-5.00,5.00, 0.)
        ENDIF
      ENDIF
C
C Analysis of electroproducton or photoproduction data
C
      CALL HBOOK1(11,' [V]^2!   itrk0 $', 40,  0.0, 40.0, 0.)
      CALL HBOOK1(12,' p  (GeV) itrk0 $', 60,  0.0,  3.0, 0.)
      CALL HBOOK1(13,' TH (deg) itrk0 $',180,  0.0,180.0, 0.)
      CALL HBOOK1(14,' PHI(deg) itrk0 $',180,  0.0,360.0, 0.)
      CALL HBOOK1(15,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1501,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1502,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1503,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1504,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1505,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(1506,' Zv (cm)  itrk0 $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(150,' Zv (cm)  itrk0 $',480,-180.0, 60.0, 0.)
      CALL HBOOK1(151,' Zv (cm) walls $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(152,' Zv (cm) walls $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(153,' Zv (cm) walls $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(154,' Zv (cm) walls $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(155,' Zv (cm) walls $', 80,-80.0, -40.0, 0.)
      CALL HBOOK1(16,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1601,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1602,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1603,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1604,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1605,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1606,' Yv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(17,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1701,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1702,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1703,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1704,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1705,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
      CALL HBOOK1(1706,' Xv (cm)  itrk0 $', 40,-10.0, 10.0, 0.)
C     
      IF(zbeam(1).LT.-0.1) THEN
        CALL HBOOK1(21,' Q^2!?obs! itrk0$',100,  0.00, 3.00, 0.)        
        CALL HBOOK1(22,' W?obs!  itrk0  $',100,  0.60, 1.60, 0.)
        CALL HBOOK1(22220,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22221,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22222,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22223,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22224,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22225,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(22226,' W $',200,0.6,2.6,0.)
        CALL HBOOK1(221,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2211,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2212,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2213,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2214,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2215,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2216,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(222,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2221,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2222,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2223,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2224,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2225,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(2226,' W?obs!  itrk0  $',100,  0.70, 2.70, 0.)
        CALL HBOOK1(23,' MMep^2!?obs! el$', 50,-0.025,0.025, 0.)
        CALL HBOOK1(231,' MMep^2!?obs! el$', 50,-0.025,0.025, 0.)
        CALL HBOOK1(232,' MMep^2!?obs! el$', 50,-0.025,0.025, 0.)
        CALL HBOOK1(24,' MMep?obs!  inel$',100, -1.00, 2.00, 0.)
        CALL HBOOK1(241,' MM2ep?obs!  inel$',100, -1.00, 2.00, 0.)
        CALL HBOOK1(25,' MMeK^+!?obs!   $',110,  0.90, 2.00, 0.)
        CALL HBOOK1(26,' MMe[p]^+!?obs! $',200,  0.40, 2.40, 0.)

        call hbook2(250,'RASTER YvsX $',100,0.0,8200.0,100,0.0,8200.0,0.)
        call hbook1(251,'RASTER X $',100, 0.0, 8200.0, 0.)
        call hbook1(252,'RASTER Y $',100, 0.0, 8200.0, 0.)

        call hbook2(260,'RASTER YvsX $',100,-2.0,2.0,100,-2.0,2.0,0.)
        call hbook1(261,'RASTER X $',100, -2.0, 2.0, 0.)
        call hbook1(262,'RASTER Y $',100, -2.0, 2.0, 0.)
        call hbook2(270,'RASTER YvsX $',100,-2.0,2.0,100,-20.0,20.0,0.)

        call hbook1(271,'X $',100, 0.0, 100.0, 0.)
        call hbook1(272,'Y $',100, 0.0, 100.0, 0.)
        call hbook1(273,'Z $',100, -50.0, 50.0, 0.)
        call hbook1(274,'DX $',80, 0.4, 1.2, 0.)
        call hbook1(275,'DY $',80, 0.0, 0.8, 0.)
        call hbook1(276,'DZ $',80, -0.4, 0.4, 0.)
        call hbook1(277,'P $',100, 0.0, 2.0, 0.)
        call hbook2(7172,'YvsX $',100,0.0,100.0,100,0.0,100.0,0.)
        call hbook2(7273,'YvsX $',100,0.0,100.0,100,-50.0,50.0,0.)
        call hbook2(7173,'YvsX $',100,0.0,100.0,100,-50.0,50.0,0.)

        call hbook1(1271,'X $',100, 0.0, 100.0, 0.)
        call hbook1(1272,'Y $',100, 0.0, 100.0, 0.)
        call hbook1(1273,'Z $',100, -50.0, 50.0, 0.)
        call hbook1(1274,'DX $',80, 0.4, 1.2, 0.)
        call hbook1(1275,'DY $',80, 0.0, 0.8, 0.)
        call hbook1(1276,'DZ $',80, -0.4, 0.4, 0.)
        call hbook1(1277,'P $',100, 0.0, 2.0, 0.)
        call hbook2(17172,'YvsX $',100,0.0,100.0,100,0.0,100.0,0.)
        call hbook2(17273,'YvsX $',100,0.0,100.0,100,-50.0,50.0,0.)
        call hbook2(17173,'YvsX $',100,0.0,100.0,100,-50.0,50.0,0.)

        call hbook2(280,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(281,'X $',100, -80.0, -40.0, 0.)
        call hbook1(282,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(283,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2801,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2811,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2821,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2831,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2802,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2812,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2822,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2832,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2803,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2813,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2823,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2833,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2804,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2814,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2824,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2834,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2805,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2815,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2825,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2835,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(2806,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(2816,'X $',100, -80.0, -40.0, 0.)
        call hbook1(2826,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(2836,'Z $',100, -20.0, 20.0, 0.)

        call hbook2(480,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(481,'X $',100, -80.0, -40.0, 0.)
        call hbook1(482,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(483,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4801,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4811,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4821,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4831,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4802,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4812,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4822,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4832,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4803,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4813,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4823,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4833,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4804,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4814,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4824,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4834,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4805,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4815,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4825,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4835,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(4806,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(4816,'X $',100, -80.0, -40.0, 0.)
        call hbook1(4826,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(4836,'Z $',100, -20.0, 20.0, 0.)

        call hbook2(580,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(581,'X $',100, -80.0, -40.0, 0.)
        call hbook1(582,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(583,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5801,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5811,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5821,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5831,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5802,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5812,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5822,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5832,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5803,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5813,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5823,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5833,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5804,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5814,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5824,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5834,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5805,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5815,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5825,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5835,'Z $',100, -20.0, 20.0, 0.)
        call hbook2(5806,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook1(5816,'X $',100, -80.0, -40.0, 0.)
        call hbook1(5826,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(5836,'Z $',100, -20.0, 20.0, 0.)

        call hbook2(28011,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook2(28012,'RASTER YvsX $',100,-5.0,5.0,100,-5.0,5.0,0.)
        call hbook2(290,'YvsX $',100,-10.0,10.0,100,-80.0,40.0,0.)
        call hbook1(281,'X $',100, -80.0, -40.0, 0.)
        call hbook1(282,'Y $',100, -2.0, 2.0, 0.)
        call hbook1(283,'Z $',100, -20.0, 20.0, 0.)
        call hbook1(291,'STEP size (cm)$',100, 0.0, 11.0, 0.)
        call hbook1(292,'STEP size (cm)$',100, 0.0, 1.0, 0.)
        call hbook1(301,'BFLD(X)$',100, 0.0, 100.0, 0.)
        call hbook1(302,'BFLD(Y)$',100, 0.0, 100.0, 0.)
        call hbook1(303,'BFLD(Z)$',100, 0.0, 100.0, 0.)
        call hbook1(304,'dbeam-tmp$',100, -10.0, 10.0, 0.)
        call hbook1(305,'dbeam$',100, 0.0, 10.0, 0.)
        call hbook1(306,'tmp$',100, 0.0, 10.0, 0.)
C
        CALL HBOOK1(31,' W?obs!  itrk0 s1$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(32,' W?obs!  itrk0 s2$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(33,' W?obs!  itrk0 s3$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(34,' W?obs!  itrk0 s4$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(35,' W?obs!  itrk0 s5$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(36,' W?obs!  itrk0 s6$',200,  0.50, 1.50, 0.)
      ELSE
        CALL HBOOK1(21,' T?tag! itrk0   $',200, -20.0,  0.0, 0.)
        CALL HBOOK1(22,' T?tof! itrk0   $',150,  10.0, 40.0, 0.)
        CALL HBOOK1(23,' W?obs! itrk0   $',230,  0.50, 2.80, 0.)
        CALL HBOOK1(24,' MM[g]p?obs!    $',150,  0.00, 1.50, 0.)
        CALL HBOOK1(241,' MM2ep?obs!  inel$',100, -1.00, 2.00, 0.)
        CALL HBOOK1(25,' MM[g]K^+!?obs! $',110,  0.90, 2.00, 0.)
        CALL HBOOK1(26,' MM[gp]^+!?obs! $',200,  0.40, 2.40, 0.)
        CALL HBOOK2(27,' Beta vs p  h^+!$', 50,0.,2.0, 50,0.,2.0, 0.)
C
        CALL HBOOK1(31,' MM[gp]^+!?obs! s1$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(32,' MM[gp]^+!?obs! s2$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(33,' MM[gp]^+!?obs! s3$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(34,' MM[gp]^+!?obs! s4$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(35,' MM[gp]^+!?obs! s5$',200,  0.50, 1.50, 0.)
        CALL HBOOK1(36,' MM[gp]^+!?obs! s6$',200,  0.50, 1.50, 0.)
      ENDIF 
C
 999  CONTINUE
      RETURN
      END
C=======================================================================
      SUBROUTINE usda_anal(iw,rw,Level,trkp,ntr_out,itag0,ev_out,tag_res)
C-----------------------------------------------------------------------
C-
C-   Purpose and Methods : User continuation of an event analysis
C-
C-   Inputs  :
C-   Outputs :
C-   Controls: Level = 0  -SDA read (or simulated) an event
C-             Level = 1  -Segment finding and linking (DC) has been completed
C-             Level = 2  -Fitting to the wire positions has been performed
C-             Level = 3  -Using all subsystems the +/-Dmes has been derived
C-             Level = 4  -Final fit and track parameters has been derived  
C-
C-   Calls:    User may call any other routines ?
C-
C-   Created    1-SEP-1993   Bogdan Niczyporuk
C-
C-    Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdacalib.inc"
#include "sdadigi.inc"
C
C Subroutine parameters
      INTEGER iw(1000),Level,ntr_out,itag0
      REAL rw(1000),trkp(12,nplane),ev_out(36,ntrmx),tag_res(7,10)
C
C External Functions
      INTEGER ETNLINK
      REAL VDOT
C
C Local variables
      INTEGER i, i0, ip, ipel, it, ihis, ifirst, isec
      INTEGER ihsc,ipsc,idsc, iret, ihel(2,3000), imme, itmp

      integer jw(100)
      integer*2 jw16(200)
      equivalence(jw(1),jw16(1))

      integer nstak
      parameter (nstak=5)
      integer helstak(nstak)

      REAL bklow, bkhi, t0sc,twL,twR,tm,dif,tmp,tmptot
      REAL MMe,MM2e, MMx,MM2x
      REAL P4in(4), P4e(4), P4p(4), P4out(4)
      REAL d0,z0,p0,th0,phi0, d,x,y,z,p,th,phi, ez(3),u(3),diff,raddeg
      REAL AMp, AMk, AMpi,AMe
      real evout7,theta_,phi_,massage
C
      DATA AMp /0.938272/
      DATA AMk /0.493677/
      DATA AMpi/0.140000/
      DATA AMe /0.000511/
C
      DATA ifirst /1/
      DATA ez/0.,0.,1./
      data tmptot/0./
C
      integer ind, ncol_tagr
      data ncol_tagr/6/
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        raddeg = 180./ACOS(-1.)
      ENDIF
C 
C
C     U S E R   A N A L Y S I S   C O D E  here
C
C
C Following are some example of my quality checks
C
C

C Histogramming only for BATCH mode
      IF(lanal(1).EQ.2 .AND. Level.NE.2)   GO TO 999
      IF(lanal(1).EQ.4 .AND. Level.NE.4)   GO TO 999
      IF(lanal(1).EQ.0 .AND. ntr_out.EQ.0) GO TO 999
C
C Here we do histogramming for simulated single track in sector#1
      IF(ABS(zkine(1)).EQ.1.) THEN
        it = 1
        ihis = 1
C Calculate the simulated track parameters (MCIN bank)
        CALL CROSS(ez,evin(4,it),u)
        CALL VUNIT(u,u,3)
        d0  = VDOT(evin(1,it),u,3)
        z0  = evin(3,it)
        p0  = evin(7,it)
        th0 = ACOS(evin(6,it))
        phi0= ATAN2(evin(5,it),evin(4,it))
C Calculate the reconstracted track parameters
c        CALL CROSS(ez,trkp(4,1,it),u)
c        CALL VUNIT(u,u,3)
c        d   = VDOT(trkp(1,1,it),u,3)
c        z   = trkp(3,1,it)
c        p   = trkp(7,1,it)
c        th  = ACOS(trkp(6,1,it))
c        phi = ATAN2(trkp(5,1,it),trkp(4,1,it))
c        CALL HFILL(ihis, trkp(12,1,it),0., 1.)
        CALL CROSS(ez,trkp(4,1),u)
        CALL VUNIT(u,u,3)
        d   = VDOT(trkp(1,1),u,3)
        z   = trkp(3,1)
        p   = trkp(7,1)
        th  = ACOS(trkp(6,1))
        phi = ATAN2(trkp(5,1),trkp(4,1))
        CALL HFILL(ihis, trkp(12,1),0., 1.)
C Histogramming
        diff = (p - p0)/p0
        CALL HFILL(ihis+1, diff,0., 1.)
        diff = (th - th0)*1000.
        CALL HFILL(ihis+2, diff,0., 1.)
        diff = (phi - phi0)*1000.
        CALL HFILL(ihis+3, diff,0., 1.)
        diff = d - d0
        CALL HFILL(ihis+4, diff,0., 1.)
        diff = z - z0
        CALL HFILL(ihis+5, diff,0., 1.)
      ENDIF
C
C Data Analysis (e,g)
C
      IF(ntr_out .EQ. 0) GO TO 999
      i0  = 0
C Find Electron or track with beta using ST, ToF & TAG (i.e. "itrk0" <==> "i0")
      DO it = 1,ntr_out
        IF(ev_out(1,it).GT.0.1) THEN
          i0 = it
          GOTO 5
        ENDIF
      ENDDO
 5    CONTINUE
      IF(i0 .EQ. 0) GO TO 999
C 
C Plot the reconstracted track parameters
ccc      x   = ev_outx(i0) ! ev_out(1,i0)
      y   = ev_out(2,i0)
      z   = ev_out(3,i0)
      p   = ev_out(7,i0)
      th  = ACOS(ev_out(6,i0))*raddeg
      phi = ATAN2(ev_out(5,i0),ev_out(4,i0))*raddeg
      IF(phi.LT.0.) phi = phi + 360.
      IF(zmagn(4).NE.0.) THEN
        isec = (phi + 30.)/60. + 1.
        IF(isec.GT.6) isec = isec - 6
      ELSE
        isec = ev_out(11,i0)/10.
      ENDIF
      CALL HFILL(11, ev_out(12,i0),0., 1.)
      CALL HFILL(12,  p,0., 1.)
      CALL HFILL(13, th,0., 1.)
      CALL HFILL(14,phi,0., 1.)
      CALL HFILL(15,  z,0., 1.)
      CALL HFILL(1500+isec,  z,0., 1.)
      CALL HFILL(150, z,0., 1.)
      CALL HFILL(16,  y,0., 1.)
      CALL HFILL(1600+isec,  y,0., 1.)
      CALL HFILL(17,  x,0., 1.)
      CALL HFILL(1700+isec,  x,0., 1.)

c      print *,'user_sda: i0=',i0,'  y=',y,'  z=',z

C
C Electroproduction
C
ccc      IF(zbeam(1) .LT.-0.1. AND. isec.EQ.1) THEN
      IF(zbeam(1) .LT.-0.1) THEN
        Ebeam = zbeam(2)
        P4in(1) = 0.
        P4in(2) = 0.
        P4in(3) = SQRT(Ebeam**2 - AMe**2)
        P4in(4) = Ebeam + AMp
C massaging data
        theta_ = ACOS(ev_out(6,i0))
        phi_ = ATAN2(ev_out(5,i0),ev_out(4,i0))
**        evout7 = massage(ev_out(7,i0),isec,theta_,phi_)
**      print *,'massage: ',ev_out(7,i0),'->',evout7
        evout7 = ev_out(7,i0)
C Final electron 4-momentum
        P4e(1) = evout7*ev_out(4,i0)
        P4e(2) = evout7*ev_out(5,i0)
        P4e(3) = evout7*ev_out(6,i0)
        P4e(4) = SQRT(evout7**2 + AMe**2)
C Qsqr observed
        Qsqr = 2.*Ebeam*P4e(4)*(1. - ev_out(6,i0))
        CALL HFILL(21, Qsqr,0., 1.)
C W (or MM) if only electron is measured
        CALL VSUB(P4in,P4e,P4out,4)
c*      MM2e = P4out(4)**2 - VDOT(P4out,P4out,3)
        MM2e = 2.*AMp*(Ebeam - P4e(4)) + AMp**2 - Qsqr
        MMe = 0.
        IF(MM2e.GT.0.) MMe = SQRT(MM2e)

**c SB select elastic events for DST
**      if(MMe .LT. 1.05) then 
        CALL HFILL(22, MMe, 0., 1.)
        CALL HFILL(22220, MMe, 0., 1.)
        CALL HFILL(22220+isec, MMe, 0., 1.)
**      else
**        call etndrop(iw,'TRAK',0)
**      endif

c fill raster hist

        ind = etnlink(iw,'FBPM',0)
        if(ind.GT.0) then
          jw(1) = iw(ind+1)
          jw(2) = iw(ind+2)
          jw(3) = iw(ind+3)
          call hfill(250,float(jw16(3)),float(jw16(6)),1.)
          call hfill(251,float(jw16(3)),0.,1.)
          call hfill(252,float(jw16(6)),0.,1.)
        endif

c update helicity info

        ind = etnlink(iw,'TGBI',0)
        if(ind.GT.0) then
          itmp = ISHFT(iw(ind+1),-15)
          itmp = IAND(itmp,1)
***          if(ev_out(12,i0).GT.0.8.AND.ev_out(12,i0).LT.1.2) then
          if(itmp.EQ.1) then
            CALL HFILL(221, MMe, 0., 1.)
            CALL HFILL(221*10+isec, MMe, 0., 1.)
          else if(itmp.EQ.0) then
            CALL HFILL(222, MMe,0., 1.)
            CALL HFILL(222*10+isec, MMe, 0., 1.)
          else
            print *,'ERROR: helicity=',itmp
            stop
          endif
***          endif
          tmptot = tmptot + 1.
        endif

c        print *,'costheta1(u)=',ev_out(6,i0),' Ebeam=',Ebeam
c        print *,'E1(u)=',P4e(4),'  nu=',Ebeam - P4e(4),
c     &  '  Q**2=',Qsqr

        CALL HFILL(30+isec, MMe, 0., 1.)
C
C Check if there are at least two tracks and one is positive
        IF(ntr_out.LT.2) GO TO 999
        ip = 0
        DO 10 it = 1,ntr_out
          IF(ev_out(9,it).LT.1.0) GO TO 10
          ip   = ip + 1
          ipel = it
 10     CONTINUE
C
C Elastic scattering events
        IF(MMe.GT.0.85 .AND. MMe.LT.1.05 .AND. ip.EQ.1) THEN
          phi0 = ATAN2(ev_out(5,ipel),ev_out(4,ipel))*raddeg
          IF(phi0.LT.0.) phi0 = phi0 + 360.
          d = ABS(phi0 - phi)
          IF(d.GT.175. .AND. d.LT.185) THEN
            CALL VSUB(P4in,P4e,P4out,4)
            P4p(1) = ev_out(7,ipel)*ev_out(4,ipel)
            P4p(2) = ev_out(7,ipel)*ev_out(5,ipel)
            P4p(3) = ev_out(7,ipel)*ev_out(6,ipel)
            P4p(4) = SQRT(ev_out(7,ipel)**2 + AMp**2)
            CALL VSUB(P4out,P4p,P4out,4)
            MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)
            CALL HFILL(23, MM2x,0., 1.)

            if(itmp.EQ.1) then
              CALL HFILL(231, MM2x, 0., 1.)
            else if(itmp.EQ.0) then
              CALL HFILL(232, MM2x, 0., 1.)
            else
              print *,'ERROR: helicity=',itmp
              stop
            endif

          ENDIF
        ENDIF
C
C Electroproduction of pi0, pi+ and K+
        IF(MMe.LT.1.10 .OR. ip.LT.1) GO TO 999
C
        DO 20 ip = 1,ntr_out
          IF(ev_out(9,ip).LT.1.0) GO TO 20
C Plot Beta for the positive hadrons
          CALL HFILL(27, ev_out(7,ip),ev_out(8,ip), 1.)                    
C Kill elastic scattering
          phi0 = ATAN2(ev_out(5,ip),ev_out(4,ip))*raddeg
          IF(phi0.LT.0.) phi0 = phi0 + 360.
          d0 = phi0 - phi - 180.
          IF(d0.GE.-5. .AND. d0.LE.5.) GO TO 20
C Particle's identification
          p0  = ev_out(7,ip)
          bklow = p0/SQRT(p0**2 + 0.716**2)
          bkhi  = p0/SQRT(p0**2 + 0.316**2)
C Final h+ 3-momentum
          P4p(1) = p0*ev_out(4,ip)
          P4p(2) = p0*ev_out(5,ip)
          P4p(3) = p0*ev_out(6,ip)
C MM if proton is measured
          IF(ev_out(8,ip).LT.bklow) THEN
            CALL VSUB(P4in,P4e,P4out,4)
            P4p(4) = SQRT(ev_out(7,ip)**2 + AMp**2)
            CALL VSUB(P4out,P4p,P4out,4)
            MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)

      th  = ACOS(ev_out(6,ip))*raddeg
      CALL HFILL(241, MM2x,0., 1.)
      if(MM2x.LT.0.AND.th.GT.60.AND.th.LT.110) THEN
c        print *,'th=',th,' degree','  Zp=',ev_out(3,ip),'  Ze=',ev_out(3,i0)
        CALL HFILL(151, ev_out(3,ip),0., 1.)
        if(ev_out(3,ip).LT.3.AND.ev_out(3,ip).GT.0.)
     &    CALL HFILL(152, ev_out(3,i0),0., 1.)
        if(ev_out(3,ip).LT.-1.AND.ev_out(3,ip).GT.-4.)
     &    CALL HFILL(153, ev_out(3,i0),0., 1.)
        dif = ABS(ev_out(3,ip)-ev_out(3,i0))
        if(dif.LT.1.) then
          CALL HFILL(154, ev_out(3,ip),0., 1.)
          CALL HFILL(155, ev_out(3,i0),0., 1.)
        endif
      endif

            MMx = -0.1
            IF(MM2x.GT.0.) MMx = SQRT(MM2x)
            CALL HFILL(24, MMx,0., 1.)
          ELSE
C MM if pion+ is measured
            IF(ev_out(8,it).GE.bkhi) THEN
              CALL VSUB(P4in,P4e,P4out,4)
              P4p(4) = SQRT(ev_out(7,ip)**2 + AMpi**2)
              CALL VSUB(P4out,P4p,P4out,4)
              MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)
              MMx = -0.1
              IF(MM2x.GT.0.) MMx = SQRT(MM2x)          
              CALL HFILL(26, MMx,0., 1.)
            ELSE
C MM if kaon is measured
              IF(MMe .GT. 1.60) THEN                                       
                CALL VSUB(P4in,P4e,P4out,4)
                P4p(4) = SQRT(ev_out(7,ip)**2 + AMk**2)
                CALL VSUB(P4out,P4p,P4out,4)
                MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)
                MMx = -0.1
                IF(MM2x.GT.0.) MMx = SQRT(MM2x)            
                CALL HFILL(25, MMx,0., 1.)
              ENDIF
            ENDIF              ! IF(ev_out(8,it).GE.bkhi)
          ENDIF                ! IF(ev_out(8,ip).LT.bklow) 
 20     CONTINUE               ! Loop over tracks
      ENDIF                    ! IF(zbeam(1).LT.-0.1)
C
C Photoproduction
C
      IF(zbeam(1) .EQ. 0.) THEN
        itag0 = ev_out(1,i0)
C Reconstructed photon energy "Ebeam" (if SIM one overwrites the simulated Ebeam)

      ind = etnlink(iw,'TAGR', 0)
      if(ind.GT.0) then
        print *,'tag_res(3,itag0)=',tag_res(3,itag0),
     &          ' en=',rw(ind+(itag0-1)*ncol_tagr+1),
     &          ' itag0=',itag0,' ncol_tagr=',ncol_tagr
      endif

        Ebeam = tag_res(3,itag0)*zbeam(2)
        P4in(1) = 0.
        P4in(2) = 0.
        P4in(3) = Ebeam
        P4in(4) = Ebeam + AMp
C T-counter time (RF-cor "tag_res(7,itag0)" or RF-uncor "tag_res(6,itag0)")
        CALL HFILL(21, tag_res(6,itag0),0., 1.)
C SC Time "t0sc" for itrk0
        ihsc = ev_out(28,i0)/10000.
        ipsc = (ev_out(28,i0) - ihsc*10000.)/100.
        idsc = ev_out(28,i0) - ihsc*10000. - ipsc*100.
        CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
        t0sc = tm +cal_sc(20,idsc,isec) -ev_out(27,i0)/vflt/ev_out(8,i0)
        CALL HFILL(22, t0sc,0., 1.)                    
C Center of Mass Energy (W)
        Wmass = SQRT(AMp**2 + 2.*Ebeam*AMp)
        CALL HFILL(23, Wmass,0., 1.)                    
C
C Examine all tracks and identify: g + p --> pi+ + X   or
C                                  g + p --> K+  + X   or
C                                  g + p --> p   + X 
        DO 40 ip = 1,ntr_out
          IF(ev_out(9,ip).LT.1.0) GO TO 40
C Plot Beta for the positive hadrons
          CALL HFILL(27, ev_out(7,ip),ev_out(8,ip), 1.)                    
          p0  = ev_out(7,ip)
          bklow = p0/SQRT(p0**2 + 0.716**2)
          bkhi  = p0/SQRT(p0**2 + 0.316**2)
C Final h+ 3-momentum
          P4p(1) = p0*ev_out(4,ip)
          P4p(2) = p0*ev_out(5,ip)
          P4p(3) = p0*ev_out(6,ip)
C MM if proton is measured
          IF(ev_out(8,ip).LT.bklow) THEN
            P4p(4) = SQRT(p0**2 + AMp**2)
            CALL VSUB(P4in,P4p,P4out,4)
            MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)



      th  = ACOS(ev_out(6,ip))*raddeg
      CALL HFILL(241, MM2x,0., 1.)
      if(MM2x.LT.0.AND.th.GT.60.AND.th.LT.110) THEN
c        print *,'th=',th,' degree','  Zp=',ev_out(3,ip),'  Ze=',ev_out(3,i0)
        CALL HFILL(151, ev_out(3,ip),0., 1.)
        if(ev_out(3,ip).LT.3.AND.ev_out(3,ip).GT.0.)
     &    CALL HFILL(152, ev_out(3,i0),0., 1.)
        if(ev_out(3,ip).LT.-1.AND.ev_out(3,ip).GT.-4.)
     &    CALL HFILL(153, ev_out(3,i0),0., 1.)
        dif = ABS(ev_out(3,ip)-ev_out(3,i0))
        if(dif.LT.1.) then
          CALL HFILL(154, ev_out(3,ip),0., 1.)
          CALL HFILL(155, ev_out(3,i0),0., 1.)
        endif
      endif


            MMx = -0.1
            IF(MM2x.GT.0.) MMx = SQRT(MM2x)
            CALL HFILL(24, MMx,0., 1.)
          ELSE
C MM if pion+ is measured
            IF(ev_out(8,ip).GE.bkhi) THEN
              P4p(4) = SQRT(p0**2 + AMpi**2)
              CALL VSUB(P4in,P4p,P4out,4)
              MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)
              MMx = 0.
              IF(MM2x.GT.0.) MMx = SQRT(MM2x)
              CALL HFILL(26, MMx,0., 1.)
              CALL HFILL(30+isec, MMx,0., 1.)
            ELSE
C MM if kaon+ is measured
              IF(Wmass.GT.1.6) THEN                  
                P4p(4) = SQRT(p0**2 + AMk**2)
                CALL VSUB(P4in,P4p,P4out,4)
                MM2x = P4out(4)**2 - VDOT(P4out,P4out,3)
                MMx = 0.
                IF(MM2x.GT.0.) MMx = SQRT(MM2x)
                CALL HFILL(25, MMx,0., 1.)
              ENDIF
            ENDIF
          ENDIF        ! IF(ev_out(8,it).LT.bklow)
 40     CONTINUE       ! Loop over tracks
      ENDIF            ! IF(zbeam(1).EQ. 0.)
C
 999  CONTINUE
C
      RETURN
      END
C======================================================================
      SUBROUTINE usda_erun
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This routine is called at the End of each Run
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Calls: User may call his routines
C-
C-   Created    19-OCT-1995   Bogdan Niczyporuk
C-
C-   Called by sda_erun
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      RETURN
      END
C======================================================================
      SUBROUTINE usda_last
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : User job finishing (last routine user calls)
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Calls: user routines ?
C-
C-   Created    1-SEP-1993   Bogdan Niczyporuk
C-
C-   Called by sda_last
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
      RETURN
      END








E 1
