C======================================================================
      SUBROUTINE sim_stdigi
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Digitalization of hits recorded in ST planes.
C-
C-   Inputs  : Swimming results & ST Geometry
C-   Outputs : st_digi & st_ndig arrays
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: none
C-
C-   Created   1-JUL-1996   Bogdan Niczyporuk
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
C Local variables
C
      INTEGER id1,id2, it, ip, ih, iacc, isec
      REAL dd, d1, d2, xx, cosa, beta
      REAL tFL, pr1, pr2, tf1, tf2, a1, a2, adc, adc1, adc2, digi(3,6)
C
C Clear the local array
      DO ih = 1,st_ndig
        digi(1,ih) = 0.
        digi(2,ih) = 0.
        digi(3,ih) = 0.
      ENDDO
C
C Loop over all track in a simulated event
      DO 200 it = 1,ntrack
        IF(evin(9,it).EQ.0.) GO TO 200          ! No DIGI for Q = 0
        isec = itrin_sec(it)
        iacc = 0
C
C Find a plane# and check boundaries of that plane
        ip   = 0
        xx = st_trps(1,1,it)
        IF(xx.GT.sda_pln(14,2,isec).AND.xx.LE.sda_pln(12,2,isec)) ip = 1
        xx = st_trps(1,2,it)
        IF(xx.GT.sda_pln(14,3,isec).AND.xx.LE.sda_pln(12,3,isec)) ip = 2
        IF(ip.EQ.0) GO TO 200
C
        id1 = itrin_sec(it)
        IF(MOD(id1,2).EQ.1) id2 = id1 + 1
        IF(MOD(id1,2).EQ.0) id2 = id1 - 1
        iacc = iacc + 1
C
C Calculate total amplidude [MeV]
        cosa = sda_pln(4,ip+1,isec)*st_trps(4,ip,it)                    
     1        +sda_pln(5,ip+1,isec)*st_trps(5,ip,it)
     2        +sda_pln(6,ip+1,isec)*st_trps(6,ip,it)        
        adc = Elos_st*sda_pln(20,ip+1,isec)/cosa
C
C Flight time [ns]
        beta = vflt*evin(7,it)/SQRT(evin(8,it)**2 + evin(7,it)**2)
        tFL = st_trps(7,ip,it)/beta
C
C Calculate SC lengths to the opposite Phototubes
        IF(ip.EQ.1) THEN
          dd = 0.
          d1 = ABS(sda_pln(14,ip+1,isec)) + st_trps(1,ip,it)
          d2 = ABS(sda_pln(12,ip+1,isec)) - st_trps(1,ip,it)
     1        + 2.*sda_pln(16,ip+2,isec)  + sda_pln(16,ip+1,isec)
        ELSE
          dd = SQRT( (sda_pln(14,ip+1,isec)-st_trps(1,ip,it))**2
     1              +(sda_pln(15,ip+1,isec)-st_trps(2,ip,it))**2)
          d1 = sda_pln(16,ip,isec) + dd
          d2 = sda_pln(16,ip,isec) - dd + 2.*sda_pln(16,ip+1,isec)
        ENDIF
C            
C Calculate flight & propagation times [ns] to the Phototubes
        pr1 = d1/cal_st(4,id1)
        pr2 = d2/cal_st(4,id2)
        tf1 = tFL + pr1                
        tf2 = tFL + pr2
C
C Calculate attenuated amplidudes [MeV]
        a1  = adc*exp(-d1/cal_st(5,id1))
        a2  = adc*exp(-d2/cal_st(5,id2))
C
C Check for double hits in a slab {take smaller "tf1" time & sum of ADC's}
        DO ih = 1,st_ndig
          IF(id1.EQ.digi(1,ih)) THEN
            IF(digi(2,id1).LT.tf1) tf1 = digi(2,id1)
            adc1 = a1 + digi(3,id1)
            IF(digi(2,id2).LT.tf2) tf2 = digi(2,id2)
            adc2 = a2 + digi(3,id2)
            GO TO 11
          ENDIF 
        ENDDO
        adc1 = a1
        adc2 = a2
C
C Store digitalization results in "st_digi"
 11     CONTINUE
        st_digi(1,id1) = id1
        st_digi(2,id1) = NINT((cal_st(1,id1) +tf1 -Ttrig)*cal_st(2,id1))
        st_digi(3,id1) = NINT(adc1*cal_st(3,id1))
          digi(1,id1)  = id1
          digi(2,id1)  = tf1
          digi(3,id1)  = adc1
        st_digi(1,id2) = id2
        st_digi(2,id2) = NINT((cal_st(1,id2) +tf2 -Ttrig)*cal_st(2,id2))
        st_digi(3,id2) = NINT(adc2*cal_st(3,id2))
          digi(1,id2)  = id2
          digi(2,id2)  = tf2
          digi(3,id2)  = adc2
        itrin_acc(1,it) = iacc
 200  CONTINUE                     ! End of loop over all tracks 
C
 999  CONTINUE
      RETURN
      END
