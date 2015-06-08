h38008
s 00000/00000/00000
d R 1.2 01/11/19 19:05:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_dcdigi.F
e
s 00222/00000/00000
d D 1.1 01/11/19 19:05:15 boiarino 1 0
c date and time created 01/11/19 19:05:15 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_dcdigi
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Digitalization of hits recorded in D.C. layers.
C-
C-   Inputs  : Swimming results
C-   Outputs : dc_digi & dc_ndig arrays
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: none
C-
C-   Created   30-OCT-1990   Bogdan Niczyporuk
C-   Modified October, 1992 Pavel Degtyarenko
C-
C-    Called by sim_main
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdacalib.inc"
#include "sdakeys.inc"
C
C External Functions
      REAL SRAN, VDOT
C
C Local variables
C
      INTEGER il,ip,it,is,iwir,iwire,isec, nwmin,nwmax,NLinSL(6),ifirst       
      INTEGER ii,ih,idi,k,la3, itdc, ifnew,ifold, iflg, iacc(6)
      REAL philay, xh,yh, phi,phiw, dis,disw, ast, zhp, phis, diss,
     &     dca, tfl, td, tpr, tfull, beta, tdc, tc, tnhase
      REAL pi, degrad, a(3), diff(3), dlen, phi_pln
C
      DATA ifirst /1/
      DATA NLinSL / 4, 6, 6, 6, 6, 6/
C
C
      IF(ifirst.EQ.1) THEN
        pi    = ACOS(-1.)
        degrad= pi/180.
        tnhase= TAN(30.*degrad)
        ifirst= 0
      ENDIF
C
C Loop over all tracks in a simulated event
C
      DO 200 it = 1,ntrack
      IF(evin(9,it).EQ.0.) GO TO 200             ! No DIGI for Q = 0
      isec = itrin_sec(it)
C
C Velocity of a particle
      beta = vflt*evin(7,it)/SQRT(evin(8,it)**2 + evin(7,it)**2)
C
C
      DO is = 1,6
        iacc(is) = 0
      ENDDO
      DO 100 il = 1,npl_dc
        ip = il + nst_max
C Find superlayer #
        is = (il-1)/6 + 1
C 3-d layer in SL
        la3= (is-1)*6 + 3 + nst_max
C Check if there is a hits in this layer
        IF(dc_trps(1,il,it).GT.999..OR.dc_trps(2,il,it).GT.999.) GOTO 100
C Check z-coordinate (along a wire)
        IF(ABS(dc_trps(2,il,it)*tnhase)-ABS(dc_trps(3,il,it))                        
     1                                .LT.sda_pln(20,ip,isec)) GOTO 100                
C Get offset for given superlayer (dc_phimin of the 1-st sense wire)
        philay = sda_pln(12,ip,isec)
C
C Translation to the center of curvature
        xh = dc_trps(1,il,it) - sda_pln(1,ip,isec)
        yh = dc_trps(2,il,it) - sda_pln(2,ip,isec)
C
C Get PHI of the hit
        phi     = ATAN2(yh,xh)
        phi_pln = pi
        IF(il.LE.12) phi_pln=ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec))             
C
C Get wire number and PHI of the wire
        IF(phi.LE.phi_pln) THEN
C Cylindrical part
          iwir = IFIX( (phi -philay +1.5*sda_pln(13,ip,isec))
     1          /sda_pln(13,ip,isec))                             
          phiw = philay + FLOAT(iwir-1)*sda_pln(13,ip,isec)
        ELSE
C Plane part
          dlen =sda_pln(7,la3,isec)*sda_pln(13,la3,isec)
          dis  =sda_pln(7,ip,isec)*(TAN(phi - phi_pln)
     1          +phi_pln - (sda_pln(17,ip,isec)-1.)
     2          *sda_pln(13,ip,isec)-philay)                             
          iwir =IFIX( sda_pln(17,ip,isec) + (dis + 0.5*dlen)/dlen )  
          disw =dlen*(FLOAT(iwir) - sda_pln(17,ip,isec))
        ENDIF
C
C Installation of stereo wires for SL# 1,4,6
        ast = sda_pln(18,ip,isec)
        IF(ABS(ast).GT.0.001) THEN
          IF(phi.LE.phi_pln) THEN
C Cylindrical part
            zhp = dc_trps(3,il,it)*SIN(ast)
     1          + (phi-phiw)*COS(ast)*sda_pln(7,ip,isec)               
            phis = phiw + ATAN( zhp/sda_pln(7,ip,isec) )
            iwir =IFIX((phis-philay +1.5*sda_pln(13,ip,isec))
     1            /sda_pln(13,ip,isec))                      
          ELSE
C Plane part
            zhp = dc_trps(3,il,it)*SIN(ast) + (dis-disw)*COS(ast)
            diss = disw + zhp
            iwir = IFIX( (diss + (sda_pln(17,ip,isec) + 0.5)*dlen)/dlen)       
          ENDIF
        ENDIF
C
C Check wire range
        IF(lanal(2).EQ.1.OR.lanal(2).EQ.2) THEN       
          nwmin = 1
          nwmax = sda_pln(14,ip,isec)
        ELSE
          nwmin = sda_pln(15,ip,isec)
          nwmax = sda_pln(16,ip,isec)
        ENDIF
c      print *,'1: il=',il,'  iwir=',iwir,'   nwmin=',nwmin,'  nwmax=',nwmax
        IF(iwir.LT.nwmin .OR. iwir.GT.nwmax) GO TO 100
C
C Count number of layers with hit wires
        IF(il.EQ.5 .OR. il.EQ.6) GO TO 10 
        iacc(is) = iacc(is) + 1
 10     CONTINUE
C
C Store info, and check if neighboring wires have been fired by that track
C
        DO 90 ih = 1,3
          IF(ih.EQ.1) iwire = iwir
          IF(ih.EQ.2) iwire = iwir - 1
          IF(ih.EQ.3) iwire = iwir + 1
C
C Check wire# range
          IF(iwire.LT.nwmin .OR. iwire.GT.nwmax) GO TO 90
C
C Calculate a distance of closest approach to the wire
          CALL CROSS(dc_wdir(1,iwire,il,isec),dc_trps(4,il,it),a)
          CALL VUNIT(a,a,3)
          DO ii=1,3
            diff(ii) = dc_trps(ii,il,it) - dc_wpmid(ii,iwire,il,isec)
          ENDDO
          dca = VDOT(diff,a,3)
C Check cell radius
          IF(iwire .LE. sda_pln(17,ip,isec)) THEN
            IF(ABS(dca) .GT. sda_pln(19,ip,isec))     GOTO 90
          ELSE
            IF(ABS(dca) .GT. 0.5*dlen) GOTO 90
          ENDIF
C
C Introduce the inefficiency of drift cell
          IF(SRAN(iseed).GT.zderr(8)) THEN
            print *,'sim_dcdigi: WARNING: inefficiency !!!'
            GOTO 90
          ENDIF
C
C Get Drift Time and resolution
          CALL sim_dtotime(il,dca, td,iflg)
          IF(iflg.GT.0) GO TO 90
C
C Flight time [ns]
          tfl = dc_trps(7,il,it)/beta
C
C Propagation time along a wire [ns]
          IF(il.LT.13) THEN
            tpr =(dc_wlen(2,iwire,il,isec)-dc_trps(3,il,it))/vprp_dc(is)                    
          ELSE                
            tpr =(dc_wlen(1,iwire,il,isec)-dc_trps(3,il,it))/vprp_dc(is)
          ENDIF                
C
C Full time
          tfull = tfl + td + tpr
          ifnew = NINT(Tsl_dc*tfull)
C
C Measured TDC
          tc  = tc_dc(iwire,il,isec)
          tdc = Ttrig + tc - tfull
          itdc= NINT(Tsl_dc*tdc)
C
C Check if "iwire" already has been hit
          idi = dc_ndig(il,isec)
          IF(idi.GT.0) THEN
            DO k = 1,idi
             IF(dc_digi(1,k,il,isec).EQ.iwire) THEN
              ifold = NINT(Tsl_dc*(Ttrig + tc)) - dc_digi(2,k,il,isec)                 
              IF(ifnew.LT.ifold) dc_digi(2,k,il,isec) = itdc                
              GOTO 80
             ENDIF
            ENDDO
          ENDIF
C Store new hit
          idi = idi + 1
          dc_ndig(il,isec)       = idi
          dc_digi(1,idi,il,isec) = iwire
          dc_digi(2,idi,il,isec) = itdc
 80       CONTINUE
C
 90     CONTINUE                 ! End loop over neighboring wires
 100  CONTINUE                   ! End loop over the layers

      itrin_acc(2,it) = 0
      DO is = 1,6
        IF(iacc(is).EQ.NLinSL(is)) itrin_acc(2,it) = itrin_acc(2,it) +1                
      ENDDO
 200  CONTINUE                   ! End loop over all tracks
C
 999  CONTINUE
      RETURN
      END
E 1
