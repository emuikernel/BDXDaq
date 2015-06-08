h38352
s 00000/00000/00000
d R 1.2 01/11/19 19:05:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_scdigi.F
e
s 00269/00000/00000
d D 1.1 01/11/19 19:05:16 boiarino 1 0
c date and time created 01/11/19 19:05:16 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_scdigi
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Digitalization of hits recorded in SC planes.
C-
C-   Inputs  : Swimming results & SC Geometry
C-   Outputs : sc_digi & sc_ndig arrays
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: none
C-
C-   Created   12-JUL-1995   Bogdan Niczyporuk
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
c
#include "sdacalib.inc"
**      real vflt
**      parameter (vflt=29.97925)
**      real cal_sc(24,48,6), Elos_sc, Ethr_sc
C
C Local variables
C
      INTEGER i,j,k, id, it, ip, ih, isec, id0, isc1,isc2, idold, iacc
      INTEGER nslab(6)        
      REAL cosa, trL, x1(3), x2(3), adc1,adc2, d1,d2, Etot,beta
      REAL tFL, tfulL, tfulR, aL,aR, adcL,adcR, digi(5,48,6)       
      REAL t0L,t0R
      INTEGER ifirst
      DATA ifirst/1/
C
      if(ifirst.EQ.1) then
        ifirst = 0
**        call scgetcalib(ltrig(1), Elos_sc, Ethr_sc, cal_sc)

c        print *,'=>',Elos_sc,Elos_sc_
c        print *,'=>',Ethr_sc,Ethr_sc_
c        do i=1,24
c        do j=1,48
c        do k=1,6
c          print *,'->',cal_sc(i,j,k),cal_sc_(i,j,k)
c        enddo
c        enddo
c        enddo

      endif
C
C Loop over all track in a simulated event
      DO 200 it = 1,ntrack
      IF(evin(9,it).EQ.0.) GO TO 200          ! No DIGI for Q = 0
      isec = itrin_sec(it)
C
      nslab(1) = sda_pln(18,nsc_min,  isec)
      nslab(2) = sda_pln(18,nsc_min+1,isec)
      nslab(3) = sda_pln(18,nsc_min+2,isec)
      nslab(4) = sda_pln(19,nsc_min+2,isec)
      nslab(5) = sda_pln(18,nsc_min+3,isec)
      nslab(6) = sda_pln(19,nsc_min+3,isec)
C
C Velocity of particle
      beta = vflt*evin(7,it)/SQRT(evin(8,it)**2 + evin(7,it)**2)
C
C Loop over all 6 planes (see routine "sc_geom.F" for details) 
      id0  = 0
      iacc = 0
      DO 100 i = 1,6
        IF(i.GT.1) id0 = id0 + nslab(i-1)
C Get SDA plane# (id: 1 - 4)
        ip = i
        IF(i.EQ.4) ip = 3
        IF(i.GT.4) ip = 4
        x1(1) = sc_trps(1,ip,it) 
        x1(2) = sc_trps(2,ip,it) 
        x1(3) = sc_trps(3,ip,it)
C Check if track reached plane "ip"
        IF(x1(1).EQ.1000..OR.x1(2).EQ.1000.) GO TO 100
C Calculate total track length in a plane "trL"         
        cosa = sda_pln(4,ip+ncc_min,isec)*sc_trps(4,ip,it)                    
     1        +sda_pln(5,ip+ncc_min,isec)*sc_trps(5,ip,it)
     2        +sda_pln(6,ip+ncc_min,isec)*sc_trps(6,ip,it)        
        trL  = sda_pln(20,ip+ncc_min,isec)/cosa
C Calculate total amplidude [MeV]
        Etot = Elos_sc*trL
C Calculate exiting point
        x2(1) = x1(1) + trL*sc_trps(4,ip,it)   
        x2(2) = x1(2) + trL*sc_trps(5,ip,it) 
        x2(3) = x1(3) + trL*sc_trps(6,ip,it)
C Check plane boundaries (along a beam direction0
        j = nslab(i) + 1
        IF(x1(1).GE.sc_grid(1,1,i,isec) .AND. 
     1     x2(1).GE.sc_grid(3,1,i,isec)) GO TO 100 
        IF(x1(1).LE.sc_grid(1,j,i,isec) .AND. 
     1     x2(1).LE.sc_grid(3,j,i,isec)) GO TO 100
C
C Loop over all slabs in a given plane
        DO k = 1,nslab(i)
          isc1 = 0
          isc2 = 0
          adc1 = 0.
          adc2 = 0.
          IF(x1(1).LT.sc_grid(1,k,i,isec).AND.x1(1).GT.
     1                           sc_grid(1,k+1,i,isec)) THEN                 
C Track is inside a slab
           IF(x2(1).LT.sc_grid(3,k,i,isec).AND.x2(1).GT.
     1                            sc_grid(3,k+1,i,isec)) THEN
             isc1 = k
             adc1 = Elos_sc*trL
             GO TO 10
C Track is crossing right boundary
           ELSE IF(x2(1) .GT. sc_grid(3,k,i,isec)) THEN
             d1 = ABS(x1(1) - sc_grid(1,k,i,isec))
             d2 = ABS(x2(1) - sc_grid(3,k,i,isec))
             adc1 = Elos_sc*trL*(d1/(d1+d2))
             IF(adc1.GT.Ethr_sc) isc1 = k
             IF(k.NE.1) THEN
               adc2 = Elos_sc*trL*(d2/(d1+d2))
               IF(adc2.GT.Ethr_sc) isc2 = k - 1
             ENDIF
             GO TO 10
C Track is crossing left boundary
           ELSE IF(x2(1) .LT. sc_grid(3,k+1,i,isec)) THEN
             d1 = ABS(x1(1) - sc_grid(1,k+1,i,isec))
             d2 = ABS(x2(1) - sc_grid(3,k+1,i,isec))
             adc1 = Elos_sc*trL*(d1/(d1+d2))
             IF(adc1.GT.Ethr_sc) isc1 = k
             IF(k.NE.nslab(ip)) THEN
               adc2 = Elos_sc*trL*(d2/(d1+d2))
               IF(adc2.GT.Ethr_sc) isc2 = k + 1
             ENDIF
             GO TO 10
           ELSE
           ENDIF
          ENDIF          
        ENDDO               
C  End of loop over slabs in a given plane
C
 10     CONTINUE
        IF(isc1.EQ.0 .AND. isc2.EQ.0) GO TO 100
C Flight time [ns]
        tFL = sc_trps(7,ip,it)/beta
C
C 
C Track hit a slab (entering plane if "isc1.NE.0")
        IF(isc1.NE.0) THEN
C Get "id"
          id = id0 + isc1
          IF(id.LT.1 .OR. id.GT.48) GO TO 20
C Check hit along SC slab
          IF(ABS(x1(3)).GE.sc_hleng(id,isec)) GO TO 20
C Calculate propagation times [ns] to the Phototubes
          tfulL = tFL + (sc_hleng(id,isec) + x1(3))/cal_sc(7,id,isec)                
          tfulR = tFL + (sc_hleng(id,isec) - x1(3))/cal_sc(7,id,isec)
C Calculate attenuated amplidudes [MeV]
          aL   = adc1*exp(-(sc_hleng(id,isec) +x1(3))/cal_sc(8,id,isec))
          aR   = adc1*exp(-(sc_hleng(id,isec) -x1(3))/cal_sc(8,id,isec))
C
C Check for double hits in a slab {take smaller "tfulX" time & sum of ADC's}
          ih = sc_ndig(1,isec)
          IF(ih.GT.0) THEN
            DO k = 1,ih
              idold = digi(1,k,isec)
              IF(id.EQ.idold) THEN
                IF(digi(2,k,isec).LT.tfulL) tfulL = digi(2,k,isec)
                IF(digi(4,k,isec).LT.tfulR) tfulR = digi(4,k,isec)
                adcL = aL + digi(3,k,isec)
                adcR = aR + digi(5,k,isec)
                GO TO 11
              ENDIF 
            ENDDO
          ENDIF
          ih = ih + 1
          adcL = aL
          adcR = aR
C
C Store digitalization results in "sc_digi"
 11       CONTINUE
          t0L = cal_sc(1,id,isec)
          t0R = cal_sc(2,id,isec)
          sc_ndig(1,isec)     =ih
          sc_digi(1,ih,1,isec)=id
c      print *,'tof id=',sc_digi(1,ih,1,isec),'  ih=',ih
          sc_digi(2,ih,1,isec)=NINT((t0L+tfulL-Ttrig)*cal_sc(3,id,isec))                                     
          sc_digi(3,ih,1,isec)=NINT(adcL*cal_sc(5,id,isec))
     1                        +cal_sc(11,id,isec)
          sc_digi(4,ih,1,isec)=NINT((t0R+tfulR-Ttrig)*cal_sc(4,id,isec))
          sc_digi(5,ih,1,isec)=NINT(adcR*cal_sc(6,id,isec))
     1                        +cal_sc(12,id,isec)
          digi(1,ih,isec) = id
          digi(2,ih,isec) = tfulL
          digi(3,ih,isec) = adcL
          digi(4,ih,isec) = tfulR
          digi(5,ih,isec) = adcR
          iacc = iacc + 1
        ENDIF
 20     CONTINUE
C
C
C A track also crossed neighbor slab (if "isc2.NE.0")
        IF(isc2.NE.0) THEN
C Get "id"
          id = id0 + isc2
          IF(id.LT.1 .OR. id.GT.48) GO TO 30
C Check hit along SC slab
          IF(ABS(x1(3)).GT.sc_hleng(id,isec)) GO TO 30
C Calculate propagation times [ns] to the Phototubes
          tfulL = tFL + (sc_hleng(id,isec) + x1(3))/cal_sc(7,id,isec)                
          tfulR = tFL + (sc_hleng(id,isec) - x1(3))/cal_sc(7,id,isec)
C Calculate attenuated amplidudes [MeV]
          aL   = adc2*exp(-(sc_hleng(id,isec) +x1(3))/cal_sc(8,id,isec))        
          aR   = adc2*exp(-(sc_hleng(id,isec) -x1(3))/cal_sc(8,id,isec))
C
C Check for double hits in a slab {take smaller "tfulX" time & sum of ADC's}
          ih = sc_ndig(1,isec)
          IF(ih.GT.0) THEN
            DO k = 1,ih
              idold = digi(1,k,isec)
              IF(id.EQ.idold) THEN
                IF(digi(2,k,isec).LT.tfulL) tfulL = digi(2,k,isec)
                IF(digi(4,k,isec).LT.tfulR) tfulR = digi(4,k,isec)
                adcL = aL + digi(3,k,isec)
                adcR = aR + digi(5,k,isec)
                GO TO 21
              ENDIF 
            ENDDO
          ENDIF
          ih = ih + 1
          adcL = aL
          adcR = aR
C
C Store digitalization results in "sc_digi"
 21       CONTINUE
          t0L = cal_sc(1,id,isec)
          t0R = cal_sc(2,id,isec)
          sc_ndig(1,isec)     =ih
          sc_digi(1,ih,1,isec)=id
          sc_digi(2,ih,1,isec)=NINT((t0L+tfulL-Ttrig)*cal_sc(3,id,isec))
          sc_digi(3,ih,1,isec)=NINT(adcL*cal_sc(5,id,isec))
     1                        +cal_sc(11,id,isec)
          sc_digi(4,ih,1,isec)=NINT((t0R+tfulR-Ttrig)*cal_sc(4,id,isec))
          sc_digi(5,ih,1,isec)=NINT(adcR*cal_sc(6,id,isec))
     1                        +cal_sc(12,id,isec)
          digi(1,ih,isec) = id
          digi(2,ih,isec) = tfulL
          digi(3,ih,isec) = adcL
          digi(4,ih,isec) = tfulR
          digi(5,ih,isec) = adcR
          iacc = iacc + 1
        ENDIF
 30     CONTINUE
 100  CONTINUE              ! End of loop over 6 planes
      itrin_acc(4,it) = iacc
 200  CONTINUE              ! End of loop over all tracks 
C
 999  CONTINUE
      RETURN
      END
E 1
