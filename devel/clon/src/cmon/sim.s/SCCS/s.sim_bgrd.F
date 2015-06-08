h46100
s 00000/00000/00000
d R 1.2 01/11/19 19:05:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_bgrd.F
e
s 00113/00000/00000
d D 1.1 01/11/19 19:05:14 boiarino 1 0
c date and time created 01/11/19 19:05:14 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_bgrd(isec)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Simulation of background hits in Drift Chamber.
C-                         Results are stored in dc_digi & dc_ndig arrays.
C-
C-   Inputs  : isec  - Sector number
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsim.a
C-
C-   Calls: sim_dtotime
C-
C-   Created   7-MAY-1991   Bogdan Niczyporuk
C-
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
#include "sdadigi.inc"
#include "sdageom.inc"
#include "sdacalib.inc"
C
C External Functions
      REAL SRAN
C
C Subroutine variables
      INTEGER isec
C
C Local variables
      INTEGER k, il, ip, la3, is, ih, nh, itdc, iwh, nbhit, iflg, nlay       
      REAL tfl, dca, td, tpr, tdc, tfull, zhit
C
C
      DATA nlay /6/      ! Max# of layers in SL
C
C      
C Check if BGRD flag is on
      IF(zbgrd(1).LT.0.1) GO TO 999
C
C
      DO 100 il = 1,npl_dc
        ip = il + nst_max
        nh = dc_ndig(il,isec)
        is  = (il-1)/nlay + 1                    ! Superlayer #
        la3 = (is-1)*nlay + 3                    ! 3-d layer in Superlayer 
C
C Uncorrelated background hits (from kev photons) are overlayed on track hits
C
        nbhit = zbgrd(is+1)*zbgrd(1)             ! Number BGRD hits in a layer
        DO 20 ih = 1,nbhit
C
C "iwh" is the hitted wire number
          iwh = sda_pln(15,ip,isec)+SRAN(iseed)*(sda_pln(16,ip,isec)
     1         -sda_pln(15,ip,isec))        
C
C "zhit" is the position along wire
          zhit = -SRAN(iseed)*dc_wlen(2,iwh,il,isec)
          IF(SRAN(iseed).GE.0.5) zhit=SRAN(iseed)*dc_wlen(1,iwh,il,isec)        
C
C Drift time [ns]
          k = la3 + nst_max
          IF(iwh.LE.sda_pln(17,ip,isec)) k = il + nst_max
          dca = SRAN(iseed)*sda_pln(19,k,isec)
          CALL sim_dtotime(is,dca, td,iflg)
          IF(iflg.GT.0) GO TO 20
C
C Flight time [ns] for photons
          tfl=SQRT(dc_wpmid(1,iwh,il,isec)**2+dc_wpmid(2,iwh,il,isec)**2            
     1                                       + zhit**2)/vflt
C
C Propagation time along a wire [ns]
          IF(il.LT.13) THEN
            tpr = (dc_wlen(2,iwh,il,isec) - zhit)/vprp_dc(is)
          ELSE
            tpr = (dc_wlen(1,iwh,il,isec) - zhit)/vprp_dc(is)
          ENDIF
C
C Full time
          tfull = tfl + td + tpr
C
C Measured TDC
          tdc = Ttrig + tc_dc(iwh,il,isec) -  tfull
          itdc= NINT(Tsl_dc*tdc)
C
C Check if "iwire" already has been hit
          IF(nh.GT.0) THEN
            DO k = 1,nh
             IF(dc_digi(1,k,il,isec).EQ.iwh) THEN
              IF(itdc.GT.dc_digi(2,k,il,isec)) dc_digi(2,k,il,isec)=itdc                       
              GO TO 20
             ENDIF
            ENDDO
          ENDIF
C Store new hits
          nh = nh + 1
          dc_ndig(il,isec)      = nh
          dc_digi(1,nh,il,isec) = iwh
          dc_digi(2,nh,il,isec) = itdc
   20   CONTINUE                            ! End loop over the BGRD hits
  100 CONTINUE                              ! End loop over the layers
C
 999  CONTINUE
      RETURN
      END
E 1
