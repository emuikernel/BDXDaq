C======================================================================
      PROGRAM main_bat
C----------------------------------------------------------------------
C
      IMPLICIT NONE
      SAVE
#include "sdabos.inc"
#include "sdakeys.inc"
#include "wcs.inc"
C External function
      INTEGER  iucomp
C
C Local variables
      INTEGER ifevb, inter, iret, iwmin, iwmax
      REAL pmin,pmax,charge,thetamin,thetamax,ltarget,rtarget,ztarget
C
C Init FIRST (BCS) BOS array to store data
      call bos(iw,ndimb)
C Init SECOND (WCS) BOS array to store geometry etc constants
      call bos(jw,jndimb)
C set interactive flag to 0
      inter = 0
C Initialization of SDA
      call cminit
C Event processing
      IF(iucomp(4HSIM ,levb,10).NE.0) THEN
        print *,'SIMULATION mode - DOES NOT WORK - road simulation only !!!'
        ifevb = 2
        call sim_loop(iw,ifevb,inter,Nevt,iwmin,iwmax,pmin,pmax,
     &       charge,thetamin,thetamax,ltarget,rtarget,ztarget)
      ELSE
        print *,'RECONSTRUCTION mode'
        ifevb = 1
        call cmloop(iw,ifevb,inter,Nevt,iret)
      ENDIF
      call dcerun(iw)
      call dch2hbook()
C Closing of SDA
      call dclast(iw)
C
      STOP
      END


