h18137
s 00000/00000/00000
d R 1.2 01/11/19 19:05:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_trig.F
e
s 00196/00000/00000
d D 1.1 01/11/19 19:05:17 boiarino 1 0
c date and time created 01/11/19 19:05:17 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_trig(ifail)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Trigger simulation
C-
C-   Inputs  : Swimming results and geometry for CC,SC,EC
C-   Outputs : Trigger time in "sdadigi.inc"
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: none
C-
C-   Created   11-DEC-1995   Bogdan Niczyporuk
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
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdacalib.inc"
C
C External Functions
      REAL SGAUS
C
C Subroutine parameters
      INTEGER ifail
C
C Local variables
      INTEGER it, ip, jp, ipmin,ipmax, icc,isc,iec, isec, ifirst
      REAL beta, tFL,tfulL,tfulR,Tmin,T, hsclen, Tjitter, x0,dx
      REAL tn30, PHT
C
      DATA ifirst /1/
      DATA tn30 /0.57735 /    ! TAN(30deg)
      DATA PHT  /30.0    /    ! Space for Photo Tube
C
C
      IF(ifirst.EQ.1) THEN
        ipmin = ltrig(5) ! =0
        ipmax = ltrig(5)
        IF(ltrig(5) .LE. 0) THEN
          ipmin = 1
          ipmax = 4
        ENDIF
        icc = 0
        IF(ltrig(6).GT.0) icc = 1
        iec = 0
        IF(ltrig(7).GT.0) iec = 1
        ifirst = 0
      ENDIF
C
      ifail = 1
      Tmin = 1000.
C
C Real Photon Trigger: 1) the smallest time (tFL+tPR) in a any SC plane
C                     
C Electron Trigger:    1) the smallest time (tFL+tPR) in the forward SC plane
C                      2) signal (adc) in forward CC
C                      3) energy (adc) in forward EC
C
      DO 20 it = 1,ntrack
        IF(evin(9,it).EQ.0.) then                 ! No Trig. with neutrals
          GOTO 20
        endif
        IF(zbeam(1).LT.-0.01) THEN
          IF(evin(8,it).GT.0.0009) then
            GOTO 20
          endif
          ipmin = 1
          ipmax = 1 ! set =4 if want all 4 planes !!! use card: BEAM 0.0 ... for that
        ENDIF
        isec = itrin_sec(it)
        DO 10 ip = ipmin,ipmax
          jp = ncc_max + ip
C
C Single electron trigger (forward CC,SC & EC)
          IF(zbeam(1).LT.-0.01 .AND. evin(8,it).LT.0.0009) THEN
C If electron required in trigger check CC-acceptance and signal in CC
C (not in use in dictionary generating)
            IF(icc.EQ.1) THEN
              IF(cc_trps(1,1,it) .GT. 999.) then
                GOTO 10
              endif
              IF(cc_trps(2,1,it) .LE. sda_pln(13,ncc_min,isec)) then
                GOTO 10
              endif        
              IF(cc_trps(1,1,it) .LE. sda_pln(14,ncc_min,isec)) then
                GOTO 10
              endif
            ENDIF
C If electron required in trigger check EC-acceptance and energy in forward EC
C (not in use in dictionary generating)
            IF(iec.EQ.1) THEN 
              IF(ec_trps(1,1,it) .GT. 999.) then
                GOTO 20
              endif
              IF(ec_trps(2,1,it) .LE. sda_pln(13,nec_min,isec)) then
                GOTO 10
              endif
              IF(ec_trps(2,1,it) .GE. sda_pln(15,nec_min,isec)) then
                GOTO 10
              endif
            ENDIF
          ENDIF
C
C Find SC slab the track points to
          isc = 0
C Check if SC hit does exists
          IF(sc_trps(1,ip,it) .GT. 999.) then
*            print *,'sim_trig: SC hit does not exist - 1'
            GOTO 10
          endif                           
C Check boundaries of SC plane (x-coordinate)
          x0 = sc_trps(1,ip,it)
*          print *,'sim_trig: SC hit: ', sda_pln(14,jp,isec),'<',x0,'<',sda_pln(12,jp,isec)
          IF(x0.GT.sda_pln(12,jp,isec) .OR. 
     1       x0.LE.sda_pln(14,jp,isec)) then
*            print *,'sim_trig: SC hit does not exist - 2: no ',
*     &      sda_pln(14,jp,isec),'<',x0,'<',sda_pln(12,jp,isec)
            GOTO 10
          endif
C Check SC length
          hsclen = ABS(sc_trps(2,ip,it)*tn30) - PHT
          IF(ABS(sc_trps(3,ip,it)). GE. hsclen) then
*            print *,'sim_trig: SC hit does not exist - 3'
            GOTO 10
          endif
C
          IF(ip.LT.3) THEN
            dx   = (sda_pln(12,jp,isec) - sda_pln(14,jp,isec))
     1            /sda_pln(18,jp,isec)
            isc  = (sda_pln(12,jp,isec) - x0)/dx
            IF(ip.EQ.1) isc  = isc + 1
            IF(ip.EQ.2) isc  = isc + 24
            GO TO 9
          ELSE
            IF(x0.GT.sda_pln(16,jp,isec)) THEN
              dx   = (sda_pln(12,jp,isec) - sda_pln(16,jp,isec))
     1              /sda_pln(18,jp,isec)
              isc  = (sda_pln(12,jp,isec) - x0)/dx
              IF(ip.EQ.3) isc  = isc + 35
              IF(ip.EQ.4) isc  = isc + 43
              GO TO 9
            ELSE
              dx   = (sda_pln(16,jp,isec) - sda_pln(14,jp,isec))
     1              /sda_pln(19,jp,isec)
              isc  = (sda_pln(16,jp,isec) - x0)/dx
              IF(ip.EQ.3) isc  = isc + 40
              IF(ip.EQ.4) isc  = isc + 47
              GO TO 9
            ENDIF
          ENDIF
 9        CONTINUE
          IF(isc.EQ.0) then
*            print *,'sim_trig: SC hit does not exist - 4'
            GOTO 10
          endif
C
C Velocity of particle
          beta = vflt*evin(7,it)/SQRT(evin(8,it)**2 + evin(7,it)**2)
C
C Flight time [ns]
          tFL = sc_trps(7,ip,it)/beta
C
C Calculate propagation times [ns] to the Phototubes
          tfulL = tFL + (hsclen + sc_trps(3,ip,it))/cal_sc(7,isc,isec)
          tfulR = tFL + (hsclen - sc_trps(3,ip,it))/cal_sc(7,isc,isec)
C
C Find the minimum time
          T = MIN(tfulL,tfulR)
          IF(T .LT. Tmin) Tmin = T
 10     CONTINUE
 20   CONTINUE
C
      IF(Tmin.GE.999.9) then
*        print *,'sim_trig: Tmin=',Tmin
        GOTO 999
      endif
      ifail = 0
      Tjitter = 0.
      IF(zbeam(1).LT.-0.1) Tjitter = Tjit*SGAUS(iseed)
      Ttrig = Tdel + Tjitter
C*    Ttrig = Tdel + Tjitter + Tmin
C
 999  CONTINUE
      RETURN
      END
E 1
