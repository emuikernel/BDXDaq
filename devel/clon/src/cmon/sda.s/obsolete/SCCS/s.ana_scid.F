h06140
s 00000/00000/00000
d R 1.2 01/11/19 19:02:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_scid.F
e
s 00135/00000/00000
d D 1.1 01/11/19 19:02:34 boiarino 1 0
c date and time created 01/11/19 19:02:34 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE ana_scid(itr, iflg)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Find SC slab "idsc" in data the track points to
C-
C-
C-   Input   :  itr     - track candidate number
C-
C-   Outputs :  itr_scid(1,itr) = idsc
C-              itr_scid(2,itr) = ipsc
C-              itr_scid(3,itr) = ihsc
C-              iflg = 0  means OK
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   3-MAY-1996  Bogdan Niczyporuk
C-
C-   Called by ana_dcam, ana_finde
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
#include "sdaanal.inc"
C
C Subroutine parameters
      INTEGER itr, iflg
C
C Local variables
      INTEGER i, ip, jp, id
      INTEGER isc, islab, isec, ndig, adc, max_adc, ipsc, ihsc, idsc, tm
      REAL x0, dx
C
      DATA tm /4094/
C
C
C Derive from the track hit position the slab "id" from the data
C
      itr_scid(1,itr) = 0
      itr_scid(2,itr) = 0
      itr_scid(3,itr) = 0
      iflg= 1
      isc = 0
      jp  = 0
      isec = itr_sect(itr)
      DO 10 ip = nsc_min,nsc_max
        jp = jp + 1
        x0 = trkp(1,ip,itr)
        IF(x0 .GT. 999.) GO TO 10
C Check if hit is outside of 4-planes)
        IF(x0 .GT. sda_pln(12,nsc_min,isec)) THEN
          ipsc = nsc_min
          isc  = 1
          GO TO 11
        ENDIF
        IF(x0 .LE. sda_pln(14,nsc_max,isec)) THEN
          ipsc = nsc_max
          isc  = 48
          GO TO 11
        ENDIF
C Hit is inside of one plane
        IF(x0.GT.sda_pln(12,ip,isec) .OR. x0.LE.sda_pln(14,ip,isec))
     1                                                         GO TO 10
        ipsc = ip
        IF(jp.LT.3) THEN
          dx   = (sda_pln(12,ip,isec) - sda_pln(14,ip,isec))
     1          /sda_pln(18,ip,isec)       
          islab= (sda_pln(12,ip,isec) - x0)/dx
          IF(jp.EQ.1) isc  = islab + 1
          IF(jp.EQ.2) isc  = islab + 24
          GO TO 11
        ELSE
          IF(x0.GT.sda_pln(16,ip,isec)) THEN
            dx   = (sda_pln(12,ip,isec) - sda_pln(16,ip,isec))
     1            /sda_pln(18,ip,isec)
            islab= (sda_pln(12,ip,isec) - x0)/dx
            IF(jp.EQ.3) isc  = islab + 35
            IF(jp.EQ.4) isc  = islab + 43
            GO TO 11
          ELSE
            dx   = (sda_pln(16,ip,isec) - sda_pln(14,ip,isec))
     1            /sda_pln(19,ip,isec)
            islab= (sda_pln(16,ip,isec) - x0)/dx
            IF(jp.EQ.3) isc  = islab + 40
            IF(jp.EQ.4) isc  = islab + 47
            GO TO 11
          ENDIF
        ENDIF
 10   CONTINUE
C
 11   CONTINUE
      IF(isc.EQ.0) GO TO 999
C
C Now find match of "isc" with data SC "id"
      ihsc = 0
      max_adc = 10
      ndig = sc_ndig(1,isec)
      IF(ndig.LE.0) GO TO 999
      DO 20 i = 1,ndig
       IF(IABS(sc_digi(1,i,1,isec) - isc) .GT. 1) GO TO 20
       IF(sc_digi(2,i,1,isec).LT.1.OR.sc_digi(2,i,1,isec).GT.tm) GOTO 20
       IF(sc_digi(4,i,1,isec).LT.1.OR.sc_digi(4,i,1,isec).GT.tm) GOTO 20
       adc = sc_digi(3,i,1,isec) + sc_digi(5,i,1,isec)
       IF(adc .GT. max_adc) THEN
         max_adc = adc
         ihsc = i 
       ENDIF
 20   CONTINUE
      IF(ihsc.EQ.0) GO TO 999
C
C No bad slab checking for data and single track simulation
      idsc = sc_digi(1,ihsc,1,isec)
      IF(ifsim.EQ.1 .AND. lanal(10).GT.0) THEN
        id = idsc
        IF(cal_sc(9,id,isec).GT.0..OR.cal_sc(10,id,isec).GT.0.) GOTO 999
      ENDIF
C
      iflg = 0
      itr_scid(1,itr) = idsc
      itr_scid(2,itr) = ipsc
      itr_scid(3,itr) = ihsc
C
  999 RETURN
      END
E 1
