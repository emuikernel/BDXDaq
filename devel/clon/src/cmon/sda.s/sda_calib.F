C======================================================================
      SUBROUTINE sda_calib(RunNo)
C----------------------------------------------------------------------
C-
C-  Purpose and Methods : The initialization of the SDA Calibration Constants.
C-                        Reads the Data Base and stores in the "sdacalib.inc".
C-
C-   Inputs  : Calibration Data Base
C-   Outputs : Initializes the SDA include file "sdacalib.inc"
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created   5-JUN-1996   Bogdan Niczyporuk
C-
C-    Called by sda_brun
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdacalib.inc"
C
      COMMON ptc_dc
      REAL ptc_dc(192,36,6)
C
C Subroutine parameters
      INTEGER RunNo
C
C External
      INTEGER iucomp, ETNCREATE, ETNLINK
C
C Local variables
      INTEGER i,j, il, id, iv, is, ir, nsect, NR, iret, iwmin,iwmax
C
      INTEGER dcst(6912), dc_st(192,36)
      REAL del,ddel,val, dc2tof, reg4pul(3), t0(6912),t0_dc(192,36)
      real delDC(6,6)
      EQUIVALENCE (t0(1),t0_dc(1,1)), (dcst(1),dc_st(1,1))
C
      REAL st_del(6), st_t1(6), st2tof, stdel(6), veff(6)          
C
      INTEGER ind,utime,ierr,version,isec, nwtrg,nwst,nwdc,nwsc,nwec
      PARAMETER (nwtrg=4, nwst= 32, nwdc= 82952, nwsc= 6914, nwec =7777)
      REAL ctrig(nwtrg), cst(nwst), cdc(nwdc), csc(nwsc), cec(nwec)
      EQUIVALENCE (ctrig(1), Tdel)
      EQUIVALENCE (cst(1)  , cal_st(1,1))
      EQUIVALENCE (cdc(1)  , tc_dc(1,1,1))
      EQUIVALENCE (csc(1)  , cal_sc(1,1,1))
C
      INTEGER Np, Ng, nwtd
      PARAMETER (Ng = 200)
      PARAMETER (nwtd = 2*Ng*36+3*36)
      REAL ftd(nwtd)
      EQUIVALENCE (td_h(1,1),ftd(1))
      REAL ECped(36,6,6)
C
      CHARACTER*100 fname
      CHARACTER*24 utimes
      CHARACTER*7  SecNo(6)
      DATA SecNo /'sector1','sector2','sector3'
     1           ,'sector4','sector5','sector6'/
C
C *** Initialization of calibration constants (temporary stuff)
C
      print *,'s1'
      IF(iucomp(4HINIT,lcali,10) .NE. 0) THEN
C
C Constants related to the TRIG
C
      print *,'s2'
        call triggetcalib(ltrig(1),Tdel,Tjit,DL1,vflt)
      print *,'s3'
C
C Constants for ST
C
        call stgetcalib(ltrig(1),Elos_st,Ethr_st,cal_st)
      print *,'s4'
C
C Constants for DC
C
C
C Bogdan calibrations
ccc always read files        call dcgetcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
ccc always read files     &                  vgas,td_h,td_f)
        call dcreadcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
     &                   vgas,td_h,td_f)


      print *,'s5'
C
C Constants for SC
C
cmovedto sclib.c        call scgetcalib(ltrig(1), Elos_sc, Ethr_sc, cal_sc)
      print *,'s6'
      ENDIF     
C
C *** Reading the "sdacalib.bos"
C 
      print *,'s7'
      IF(iucomp(4HREAD,lcali,10) .NE. 0) THEN
C Get trigger constants
      print *,'s8'
        call trigreadcalib(ltrig(1),Tdel,Tjit,DL1,vflt)
      print *,'s9'
C Get ST constants
        call streadcalib(ltrig(1),Elos_st,Ethr_st,cal_st)
      print *,'s10'
C Get DC constants
        call dcreadcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
     &                   vgas,td_h,td_f)
      print *,'s11'
C Get SC constants
cmovedto sclib.c        call screadcalib(ltrig(1),Elos_sc,Ethr_sc,cal_sc)
      print *,'s12'
      ENDIF
      print *,'s13'
C
 999  CONTINUE
      print *,'s14'
      RETURN
      END
