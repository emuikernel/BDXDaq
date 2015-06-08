h50160
s 00000/00000/00000
d R 1.2 01/11/19 19:02:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_docam.F
e
s 00088/00000/00000
d D 1.1 01/11/19 19:02:33 boiarino 1 0
c date and time created 01/11/19 19:02:33 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE ana_docam(is, itr, digi)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates Distance to the hit wire
C-
C-
C-   Input   :  is          - SuperLayer number
C-              itr         - Track candidate number 
C-              digi(1,la)  - Wire number "iw"
C-              digi(2,la)  - Raw TDC [counts]
C-              digi(3,la)  - Drift Time Td [ns] to the hit wire# "iw"
C-
C-   Outputs :  digi(4,la)  - Drift Distance (measured) Dm [cm]
C-              digi(5,la)  - Sigma of Drift Distance [cm]
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
#include "sdakeys.inc"
#include "sdacalib.inc"
C
      INTEGER nlay
      PARAMETER (nlay = 6)       ! Maximum layers in SuperLayer
C
C Subroutine parameters
      INTEGER is, itr
      REAL digi(5,nlay)
C
C Local variables
      INTEGER il, la, ig
      REAL slope
C
C
C Here is the option of constant velocity in a drift cell (the old way)
C
      IF(zderr(9) .EQ. 1.) THEN
        DO 5 la = 1,nlay
          digi(4,la) = -9.999
          digi(5,la) = zderr(is+1)
          IF(digi(1,la).LE.0.) GO TO 5
          digi(4,la) = digi(3,la)*vgas
 5      CONTINUE
        GO TO 999
      ENDIF
C
C Convert the Drift Time into the Drift Distance:  Dm = f(Td)
C
      DO 10 la = 1,nlay
        il = (is-1)*6 + la
        digi(4,la) = -9.999
C Discard bad or nonexisting wires
        IF(digi(1,la).LE.0.) GO TO 10
C Check Drift Time range: {0 - Tmax}
        IF(digi(3,la).LE.-10. .OR. digi(3,la).GE.td_h(2,il)) THEN
          digi(1,la) = -digi(1,la)
          digi(4,la) = -8.888
          GO TO 10
        ENDIF
        IF(digi(3,la).GT.-10. .AND. digi(3,la).LE.0.) THEN
          digi(3,la) = -digi(3,la)
        ENDIF
C Get Drift Distance       
        ig = digi(3,la)/td_f(1,2,il) + 1.
        slope = (td_f(2,ig+1,il) - td_f(2,ig,il))
     1         /(td_f(1,ig+1,il) - td_f(1,ig,il))
        digi(4,la) = td_f(2,ig,il) + slope*(digi(3,la) - td_f(1,ig,il))
C Get Drift Distance error
        digi(5,la) = zderr(is+1)
 10   CONTINUE
C
  999 RETURN
      END


E 1
