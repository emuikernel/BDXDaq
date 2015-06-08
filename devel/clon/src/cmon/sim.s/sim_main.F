C======================================================================
      SUBROUTINE sim_main(ifail)
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Main routine for an event simulation.
C-
C-    Inputs  :
C-    Outputs :
C-    Controls: 
C-
C-    Library belongs: libsim.a
C-
C-    Calls: usda_evtgen, dcrotds, sda_vrots, dcswim, sim_pevb, sim_trig
C-           sim_dcdigi,sim_ccdigi,sim_scdigi,sim_ecdigi, sim_bgrd, sim_tobos
C-
C-    Created    14-MAR-1995   Bogdan Niczyporuk
C-
C-
C-    Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdabos.inc"
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadraw.inc"
#include "sdadigi.inc"
C
C Parameter
      INTEGER ifail
C
C External
      INTEGER iucomp
C
C Local variables
      INTEGER isec, ip,il,it,k,kel,ist,inter,if_draw,nsect,ifirst, Level
      INTEGER ev_acc, ipl_min
      REAL vins(9), smin_old, smax_old, dang_old
C
      real sda_trp(7,npln), sda_tmat(5,5,npln) ! from sdageom.inc
C
      DATA ifirst/1/
      DATA nsect /6/
C
      IF(ifirst.EQ.1) THEN
        Nevin_prod = 0
        Nevin_acc  = 0
        inter      = 0
        IF(iucomp(4HINT ,lmode,1 ).NE.0) inter = 1
        ifirst     = 0
      ENDIF
C
      Nevin_prod = Nevin_prod + 1
C Clear arrays needed for digitalization
      bm_ndig = 7
      DO k = 1,bm_ndig
        bm_digi(1,k) = 0
        bm_digi(2,k) = 0
        bm_digi(3,k) = 0
      ENDDO
      st_ndig  = 0
      tgt_ndig = 0
      tge_ndig = 0
      DO k = 1,nsect
        st_digi(1,k) = 0
        st_digi(2,k) = 0
        st_digi(3,k) = 0
      ENDDO
      DO isec = 1,nsect
        DO il = 1,npl_dc
          dc_ndig(il,isec) = 0
        ENDDO
        il = 1
        cc_ndig(il,isec) = 0
        sc_ndig(il,isec) = 0
        DO il = 1,6
          ec_ndig(il,isec) = 0
        ENDDO
        DO il = 1,4
          ec1_ndig(il,isec) = 0
        ENDDO
      ENDDO
C
      Level = 0
      Ttrig = 444.
      ntrdraw = 0
      if_draw = 0
      IF(lanal(1).LE.1) if_draw = inter
C
      IF(lanal(2).EQ.1.OR.lanal(2).EQ.2) THEN
        ipl_min = 4
      ELSE
        ipl_min = 2
      ENDIF
C
C Loop over all tracks in an event (swimming)
C
      it = 1
      isim = 1
      DO k = 1,5
        itrin_acc(k,1) = -1
      ENDDO
      isec = 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      CALL dcrotds(ev_in(1,1),evin(1,1),isec)
      itrin_sec(1) = isec
      evin(2,1) = ev_in(1,1)
      evin(1,1) = ev_in(3,1)
      evin(3,1) = ev_in(2,1)
c        print *,'!!!!!!!!',evin(1,1),evin(2,1),evin(3,1)
C
      DO k = 1,9
        vins(k) = evin(k,1)
      ENDDO
C
C In case of PT Field set new STEP parameters (and save old)
      IF(zmagn(4).NE.0.) THEN
        smin_old = zstep(1)
        zstep(1) = 0.10
        smax_old = zstep(2)
        zstep(2) = 5.00
        dang_old = zstep(3)
        zstep(3) = 0.001
      ENDIF
C
      CALL dcswim(sda_pln(1,1,isec),ipl_min,npln,0,if_draw,vins,
     &             sda_trp,sda_tmat,nstep_xyz,xyz)
C
C In case of PT Field store possible new Sector#
      itrin_sec(1) = isec
C
C Store trajectories for trigger simulation and digitalization
C
C Start Counter
      il = 0
      DO ip = nst_min,nst_max
        il = il + 1
        DO k = 1,7
          st_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
C Drift Chamber
      il = 0
      DO ip = ndc_min,ndc_max
        il = il + 1
        DO k = 1,7
          dc_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
C Cerenkov Counter
      il = 0
      DO ip = ncc_min,ncc_max
        il = il + 1
        DO k = 1,7
          cc_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
C Scintillator Counter
      il = 0
      DO ip = nsc_min,nsc_max
        il = il + 1
        DO k = 1,7
          sc_trps(k,il,1) = sda_trp(k,ip)
      print*,'sim_main:sc_trps(',k,',',il,')=',sc_trps(k,il,1)
        ENDDO
      ENDDO




*      IF(sc_trps(1,1,1) .GT. 999.) print *,'sim_main 1'
*      IF(sc_trps(1,2,1) .GT. 999.) print *,'sim_main 2'
*      IF(sc_trps(1,3,1) .GT. 999.) print *,'sim_main 3'
*      IF(sc_trps(1,4,1) .GT. 999.) print *,'sim_main 4'



C Shower Counter
      il = 0
      DO ip = nec_min,nec_max
        il = il + 1
        DO k = 1,7
          ec_trps(k,il,1) = sda_trp(k,ip)
        ENDDO
      ENDDO
C Store trajectories for drawing when there is no PR & FF fitting
      IF(if_draw.EQ.1 .AND. ntrdraw.LE.max_draw) THEN
        ntrdraw = ntrdraw + 1
        draw_isec(ntrdraw)  = itrin_sec(1)
        draw_nstep(ntrdraw) = nstep_xyz
        DO ist = 1,nstep_xyz
          draw_xyz(1,ist,ntrdraw) = xyz(1,ist)
          draw_xyz(2,ist,ntrdraw) = xyz(2,ist)
          draw_xyz(3,ist,ntrdraw) = xyz(3,ist)
        ENDDO
C Store ST hits
        k = 0
        DO il = nst_min,nst_max
          k = k + 1
          draw_hit(1,k,ntrdraw) = sda_trp(1,il)
          draw_hit(2,k,ntrdraw) = sda_trp(2,il)
          draw_hit(3,k,ntrdraw) = sda_trp(3,il)
        ENDDO
C Store CC,SC and EC hits
        k = 2
        DO il = ncc_min,nec_max
          k = k + 1
          draw_hit(1,k,ntrdraw) = sda_trp(1,il)
          draw_hit(2,k,ntrdraw) = sda_trp(2,il)
          draw_hit(3,k,ntrdraw) = sda_trp(3,il)
        ENDDO
      ENDIF
C
C No Trigger (no Data) just only swimming
      IF(ltrig(5).GT.4) THEN
        lanal(1) = 0
        GOTO 999
      ENDIF
C
C Trigger simulation
      CALL sim_trig(ifail)
      IF(ifail.GT.0) then
*        print *,'sim_main: ifail(sim_trig1)=',ifail
        GOTO 999
      endif
*        print *,'sim_main: ifail(sim_trig2)=',ifail
C
C Digitalization of all detector subsystems
C
      IF(zbeam(1).EQ.0.) THEN
        st_ndig = nsect
        CALL sim_stdigi
        CALL sim_tgdigi
      ENDIF
*      call start_microtimer()
      CALL sim_dcdigi
*      call stop_microtimer(10000)
      CALL sim_ccdigi
      CALL sim_scdigi
      CALL sim_ecdigi
      CALL sim_ec1digi
C
C Define here the event acceptance for DC, SC and EC
      k  = 0
      kel= 0
      DO 30 it = 1,ntrack
        IF(evin(9,it).EQ.0.) GOTO 30                     ! Only charged tracks
        IF(zbeam(1).LT.0.) THEN
          IF(evin(8,it).LT.0.001 .AND. itrin_acc(5,it).GT.0) kel = 1
        ELSE
          IF(itrin_acc(1,it).GT.0) kel = 1        
        ENDIF
        IF(itrin_acc(2,it).EQ.6 .AND. itrin_acc(4,it).GT.0) k = k + 1                  
 30   CONTINUE
      k = k*kel
      ev_acc = 0
      IF(k.GE.zcut(8)) THEN
        ev_acc = 1
        Nevin_acc = Nevin_acc + 1
      ENDIF
*C
*C Add background to the hits in Drift Chambers
*      IF(isw(2).EQ.0) THEN
*        DO isec = 1,nsect
*          CALL sim_bgrd(isec)
*        ENDDO
*      ELSE
*        CALL sim_bgrd(isw(2))
*      ENDIF
*C
*C Store the simulation results (REVB) in BOS Banks
      ievt = ievt + 1
*      CALL sim_tobos
C
C Flag  isim  = 1  during the similation, otherwise is zero
C Flag  ifsim = 1  during the analysis of simulated data, otherwise is zero
      isim = 0
      ifsim= 1
C
C In case of PT Field restore old STEP parameters
      IF(zmagn(4).NE.0.) THEN
        zstep(1) = smin_old
        zstep(2) = smax_old
        zstep(3) = dang_old
      ENDIF
C
C Print EVB
      IF(lprnt(2).NE.0) CALL sim_pevb(iw)
C
C Print Acceptance
      IF(lprnt(2).EQ.2) THEN
        k = zbeam(1)
        WRITE(6,41) ev_acc,k
        DO it = 1,ntrack
          WRITE(6,42) it,evin(8,it),(itrin_acc(k,it),k=1,5)
        ENDDO
      ENDIF
 41   FORMAT(/' EvtAccept =',I2,'  Qbeam =',I2,
     1        ' (Value =-1 NA,  = 0 NotAcc,  > 0 Acc)'/
     2        ' itr    Mass   ST   DC   CC   SC   EC')               
 42   FORMAT(I4,F8.5,5I5)
C
  999 CONTINUE
      RETURN
      END




