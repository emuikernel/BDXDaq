C======================================================================
      SUBROUTINE ana_twalk(ih,id,isec,cal_sc,sc_digi, twL,twR,tm,iret)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Returns tdc time corrected for Time of Walk
C-
C-   Inputs  :  ih    - hit No in the "sc_digi(i,ih,1,isec) array 
C-              id    - SCslab No [1 - 48]
C-              isec  - sector No [1 - 6]
C-              cal_sc - SC calibration
C-              sc_digi - SC digitization
C-
C-   Outputs :  twL   - left time corrected for Time of Walk
C-              twR   - right time corrected for Time of Walk
C-              tm    - avarage time corrected for Time of Walk
C-              iret  - return flag [1 OK, 0 failed]
C-   Controls:
C-
C-   Calls: none
C-
C-   Created    22-FEB-1998   Bogdan Niczyporuk
C-
C-   Called by ana_betaw
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine parameters
      INTEGER ih, id, isec, iret, sc_digi(5,48,1,6)
      REAL    tm, cal_sc(24,48,6)
C
C Local variables
      REAL tdcL,tdcR, adcL,adcR, tL,tR, twL,twR, aL,aR,a0
     1    ,tw0L,tw0R, w0,w1,w2
      REAL adc_thr, adc_ref, tdc_max, adc_max
C
      DATA adc_thr  /  35.0/
      DATA adc_ref  / 600.0/
      DATA tdc_max  /4095.0/
      DATA adc_max  /8191.0/
C
C
      iret = 0
C
      IF(id .NE. sc_digi(1,ih,1,isec)) GO TO 999
C
      tdcL = sc_digi(2,ih,1,isec)
      IF(tdcL.LT.1 .OR. tdcL.GE.tdc_max) GO TO 999
      tdcR = sc_digi(4,ih,1,isec)
      IF(tdcR.LT.1 .OR. tdcR.GE.tdc_max) GO TO 999
C
      tL = cal_sc(21,id,isec)  + tdcL/cal_sc(3,id,isec)
     1    +cal_sc(23,id,isec)*tdcL*tdcL
      tR = cal_sc(22,id,isec) + tdcR/cal_sc(4,id,isec)
     1    +cal_sc(24,id,isec)*tdcR*tdcR  
      a0   = adc_ref/adc_thr
      adcL = sc_digi(3,ih,1,isec) - cal_sc(11,id,isec)
      adcR = sc_digi(5,ih,1,isec) - cal_sc(12,id,isec)
C
C Left adc
      IF(adcL.GT.0. .AND. adcL.LT.adc_max) THEN
        w0 = cal_sc(13,id,isec)
        w1 = cal_sc(15,id,isec)
        w2 = cal_sc(17,id,isec)
C
        IF(a0 .LT. w0) THEN
          tw0L = w1/(a0**w2)
        ELSE
          tw0L = (w1+w1*w2)/(w0**w2) - (a0*w1*w2)/(w0**(1.+w2)) 
        ENDIF
C
        aL = adcL/adc_thr
        IF(aL .LT. w0) THEN
          twL = tL + tw0L - w1/(aL**w2)
        ELSE
          twL = tL + tw0L -(w1+w1*w2)/(w0**w2) +(aL*w1*w2)/(w0**(1.+w2))
        ENDIF
      ELSE
        twL = tL
      ENDIF
C
C Right adc
      IF(adcR.GT.0. .AND. adcR.LT.adc_max) THEN
        w0 = cal_sc(14,id,isec)
        w1 = cal_sc(16,id,isec)
        w2 = cal_sc(18,id,isec)
C
        IF(a0 .LT. w0) THEN
          tw0R = w1/(a0**w2)
        ELSE
          tw0R = (w1+w1*w2)/(w0**w2) - (a0*w1*w2)/(w0**(1.+w2)) 
        ENDIF
C
        aR = adcR/adc_thr
        IF(aR .LT. w0) THEN
          twR = tR + tw0R - w1/(aR**w2)
        ELSE
          twR = tR + tw0R -(w1+w1*w2)/(w0**w2) +(aR*w1*w2)/(w0**(1.+w2)) 
        ENDIF
      ELSE
        twR = tR
      ENDIF
C
C Avaraged corrected time
      tm = (twL + twR)/2.
      iret = 1
C
 999  CONTINUE
      RETURN
      END
