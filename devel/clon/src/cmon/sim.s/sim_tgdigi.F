C======================================================================
      SUBROUTINE sim_tgdigi
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : 
C-    simulates T- and E-counter digitizations for given energy
C-         (assuming that start time at CLAS center = 0.0)
C-         (distance radiator-CLAS center: 21.98m = 73.317378 nsec)
C-
C-   Inputs  :     [cf. sdakeys.inc,sdavgen.inc:
C-                     zbeam(2): electron energy;   Ebeam: photon energy]
C-   Outputs : none
C-
C-   Controls:
C-
C-   Library belongs: 
C-
C-   Calls: 
C-
C-   Created   26-JUL-1998   Franz Klein
C-
C-   Called by ???
C-
C--------1---------2---------3---------4---------5---------6---------7--
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
      SAVE

#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdadigi.inc"
      REAL ET_window,TAG_RF_MEAN,TAG_RF_10SIGMA,TAG_TCUT(2),RF_OFFSET,
     &     RF_SLOPE(2),Tbound(122),Ebound(768),Eaver(0:767),Edev(767)
      INTEGER tagT_status(61),etmin(384),etmax(384),t1bin(61),t2bin(61)
      REAL slopeTL(61),slopeTR(61),TpeakL(61),TpeakR(61),TagTCi(121),
     &     slopeE,Epeak(384)
c  local variables
      INTEGER  i, id, irnd, etdiff, ebin, tbin, ifirst
      REAL  Enorm, xval, SRAN, SGAUS
      DATA ifirst/1/
c----------------------------------------------------------------
      if(ifirst.ne.0) then
        ifirst = 0
c get calibration; before it was in common block so CHECK NEEDED !!!
        call tggetcalib(ltrig(1),ET_window,TAG_RF_MEAN,TAG_RF_10SIGMA,
     &                  TAG_TCUT,RF_OFFSET,RF_SLOPE,Tbound,Ebound,
     &                  Eaver,Edev,tagT_status,etmin,etmax,t1bin,t2bin,
     &                  slopeTL,slopeTR,TpeakL,TpeakR,TagTCi,slopeE,Epeak)
ccc      print *,'+=+ ',ET_window,slopeTL(22),slopeTR(61),Epeak(333)
      endif
c
      tgt_ndig = 0
      tge_ndig = 0
      if (zkine(1).EQ.1) goto 100
      Enorm = Ebeam/zbeam(2)
      if(Enorm.GE.Ebound(1) .OR. Enorm.LE.Ebound(768)) then
        RETURN
      endif

      ebin=1
      do while (Enorm.LT.Ebound(ebin+1))
         ebin = ebin +1
      enddo
      tge_digi(1,1) = (ebin+1)/2
      id = tge_digi(1,1)
      if (MOD(ebin,2).EQ.0) then
         tge_ndig = 2
         tge_digi(1,2) = id +1
         etdiff = etmax(tge_digi(1,2))-etmin(id)
      else
         tge_ndig = 1
         etdiff = etmax(id)-etmin(id)
      endif
      if (etdiff.GT.2) then
         irnd = INT(SRAN(iseed)*etdiff-0.01)
         tgt_ndig = 2
         tgt_digi(1,1) = etmin(id)+irnd
         tgt_digi(1,2) = tgt_digi(1,1)+1
         tbin = tgt_digi(1,1)*2
      else
         tgt_ndig = 1
         irnd = INT(SRAN(iseed)*etdiff)
         tgt_digi(1,1) = etmin(id)+irnd
         tbin = tgt_digi(1,1)*2 -1
      endif
      do i=1,tge_ndig
         id = tge_digi(1,i)
         xval = TAG_RF_MEAN + SGAUS(iseed)*ET_window/5.
         tge_digi(2,i) = IFIX((xval + Epeak(id))/slopeE)
      enddo
      do i=1,tgt_ndig
         id = tgt_digi(1,i)
         xval = TAG_RF_MEAN + SGAUS(iseed)*TAG_RF_10SIGMA/10. +
     &            TagTCi(tbin) + TpeakL(id)
         tgt_digi(2,i) = IFIX(xval/slopeTL(id))
         xval = TAG_RF_MEAN + SGAUS(iseed)*TAG_RF_10SIGMA/10. +
     &            TagTCi(tbin) + TpeakR(id)
         tgt_digi(3,i) = IFIX(xval/slopeTR(id))
      enddo
c
c  RF signal:  (assuming 100 psec resol.)
c
      xval = SRAN(iseed)*40.+6.
      xval = (IFIX(xval)+0.5)*2.004 + SGAUS(iseed)*0.10
      bm_digi(1,6) = 6
      bm_digi(2,6) = IFIX(xval/RF_SLOPE(1))
      xval = SRAN(iseed)*40.+7.
      xval = (IFIX(xval)+0.5)*2.004 + SGAUS(iseed)*0.10
      bm_digi(1,7) = 7
      bm_digi(2,7) = IFIX((xval - RF_OFFSET)/RF_SLOPE(2))
      RETURN

c  for zkine(1)=1 (single particle tracking) we generate constant
c  tagger entries (for timing purpose in ana_finde.F)
100   CONTINUE
      tge_ndig = 1
      tge_digi(1,1) = 125
      tge_digi(2,1) = IFIX((TAG_RF_MEAN + Epeak(125))/slopeE)
      tgt_ndig = 1
      tgt_digi(1,1) = 22
      xval = TAG_RF_MEAN + TagTCi(43) + TpeakL(22)
      tgt_digi(2,1) = IFIX(xval/slopeTL(22))
      xval = TAG_RF_MEAN + TagTCi(43) + TpeakR(22)
      tgt_digi(3,1) = IFIX(xval/slopeTR(22))
      bm_digi(1,6) = 6
      bm_digi(2,6) = 1000
      bm_digi(1,7) = 7
      bm_digi(2,7) = 1000
C
      RETURN      
      END

c--------------------------------------------------------------------
