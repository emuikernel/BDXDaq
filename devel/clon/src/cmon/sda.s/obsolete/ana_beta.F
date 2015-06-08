C======================================================================
      SUBROUTINE ana_beta(itr,itrk0,itr_sect,itr_scid,beta0,trkp,ifsim,
     &                    beta,iflg)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates particle velocity for track "itr"
C-
C-
C-   Input   :  itr     - track candidate number
C-              itrk0   - track number with known "ID"
C-              beta0   - velocity of the track "itrk0"
C-
C-   Outputs :  beta    - particle velocity
C-              iflg = 0  means OK
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   8-MAY-1996  Bogdan Niczyporuk
C-
C-   Called by ana_dcam
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaparam.inc"
c sc_hleng
#include "sdageom.inc"
c sc_digi
#include "sdadigi.inc"
c cal_sc,vflt
#include "sdacalib.inc"
C
C Subroutine parameters
      INTEGER itr, itrk0, itr_sect(ntrmx), iflg, itr_scid(3,ntrmx)
      INTEGER ifsim
      REAL    beta, beta0, trkp(12,nplane,ntrmx)
C
C Local variables
      INTEGER idsc, ipsc, ihsc, isec, iret
      REAL tPR, tFLe, tFLm, Se, Sm, twL, twR, tm
      real tzero ! in sdaanal.inc, but seems nobody use it
C
C
      iflg = 1
C
C Get info for the particle with known "ID"
      IF(itr.EQ.1) THEN
        idsc = itr_scid(1,itrk0)
        ipsc = itr_scid(2,itrk0)
        ihsc = itr_scid(3,itrk0)
        isec = itr_sect(itrk0)
        IF(ifsim.EQ.1) THEN
          IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
           tPR =(sc_hleng(idsc,isec)+trkp(3,ipsc,itrk0))
     1         /cal_sc(7,idsc,isec)                                     
           tFLe=sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec)
     1         -tPR -cal_sc(1,idsc,isec)               
          ELSE
           tPR =(sc_hleng(idsc,isec)-trkp(3,ipsc,itrk0))
     1         /cal_sc(7,idsc,isec)       
           tFLe=sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec)
     1         -tPR -cal_sc(2,idsc,isec)        
          ENDIF       
          Se = trkp(10,ipsc,itrk0)
        ELSE
          CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
          tzero= tm -trkp(10,ipsc,itrk0)/vflt/beta0 +cal_sc(20,idsc,isec)
        ENDIF
      ENDIF
C
C Calculate particle velocity
      IF(itr .EQ. itrk0) THEN
        beta = beta0
        iflg = 0
      ELSE
        idsc = itr_scid(1,itr)
        ipsc = itr_scid(2,itr)
        ihsc = itr_scid(3,itr)
        isec = itr_sect(itr)
        IF(ifsim.EQ.1) THEN
          IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
           tPR = (sc_hleng(idsc,isec) +trkp(3,ipsc,itr))
     1          /cal_sc(7,idsc,isec)                               
           tFLm= sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec) -tPR
     1          -cal_sc(1,idsc,isec)               
          ELSE
           tPR = (sc_hleng(idsc,isec) -trkp(3,ipsc,itr))
     1          /cal_sc(7,idsc,isec)      
           tFLm= sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec) -tPR
     1          -cal_sc(2,idsc,isec)
          ENDIF       
          Sm = trkp(10,ipsc,itr)
          beta = Sm*beta0/(vflt*beta0*(tFLm - tFLe) + Se)
        ELSE
          CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
          tFLm  = tm + cal_sc(20,idsc,isec) - tzero         
          beta = trkp(10,ipsc,itr)/(vflt*tFLm)
        ENDIF
        iflg = 0
      ENDIF
C
  999 RETURN
      END
