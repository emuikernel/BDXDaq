C======================================================================
      SUBROUTINE ana_dtime(isec_,is,itr,itr_new,itrscid,trk_,trkp_,
     &                     beta,ifsim_, digi)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Derive Drift Time from raw TDC
C-
C-
C-   Input   :  is          - superlayer number
C-              itr         - track candidate number
C-              itr_new = 1   when called 1-st time for a track "itr" 
C-              beta        - particle velocity
C-              digi(1,la)  - hit wire number
C-              digi(2,la)  - raw TDC for the hit wire "digi(1,la)"
C-
C-   Outputs :  digi(3,la)  - drift time to the hit wire "digi(1,la)"
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   27-FEB-1996  Bogdan Niczyporuk
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
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdacalib.inc"
C
C Subroutine parameters
      INTEGER isec_, is, itr, itr_new, itrscid(3), ifsim_
      REAL digi(5,6), beta, trk_(12,36), trkp_(12,nplane)
C
C Local variables
      INTEGER la, il0, il, iw, nlay, iret
      INTEGER isec, ipsc, ihsc, idsc                
      REAL t0, tcL,tcR, tFLs,tPRs, tFLd,tPRd, twL,twR,tm
C
      DATA nlay /6/
C
C
C Derive from the track hit position the slab "id" from the data (for 1-st SL) 
      IF(itr_new .EQ. 1) THEN
        itr_new = 0
        idsc = itrscid(1)
        ipsc = itrscid(2)
        ihsc = itrscid(3)
        isec = isec_
      ENDIF
C  
C ToF part. Get TRIG time "t0".
      IF(ifsim_.EQ.1) THEN
        tFLs = trkp_(10,ipsc)/(beta*vflt)
        IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
          tPRs=(sc_hleng(idsc,isec) +trkp_(3,ipsc))/cal_sc(7,idsc,isec)
          tcL =cal_sc(1,idsc,isec)
          t0  =tcL +tPRs +tFLs -sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec)
        ELSE
          tPRs=(sc_hleng(idsc,isec) -trkp_(3,ipsc))/cal_sc(7,idsc,isec)
          tcR =cal_sc(2,idsc,isec)
          t0  =tcR +tPRs +tFLs -sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec)
        ENDIF       
      ELSE
        CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
        t0 = -(tm - trkp_(10,ipsc)/(beta*vflt) + cal_sc(20,idsc,isec))
      ENDIF
C
C Derive drift time from raw TDC
      il0 = (is - 1)*nlay
      DO 10 la = 1,nlay
        iw = digi(1,la)
        IF(iw.LE.0) GO TO 10
        il = il0 + la
        tFLd = trk_(8,il)/(beta*vflt)
        tPRd = trk_(9,il)/vprp_dc(is)
        digi(3,la) = tc_dc(iw,il,isec)-tPRd-tFLd-digi(2,la)/Tsl_dc + t0
 10   CONTINUE
C
  999 RETURN
      END
