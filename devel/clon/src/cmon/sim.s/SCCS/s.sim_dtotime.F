h17819
s 00000/00000/00000
d R 1.2 01/11/19 19:05:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sim_dtotime.F
e
s 00080/00000/00000
d D 1.1 01/11/19 19:05:15 boiarino 1 0
c date and time created 01/11/19 19:05:15 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sim_dtotime(il,dca, dtime,iflg)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates Drift Time from Diatance to the hit wire
C-
C-
C-   Input   :  il          - Layer number
C-              dca         - Distance Closest Approach to the hit wire
C- 
C-   Output  :  dtime       - Measured Drift Time
C-              iflg        - iflg = 0  OK,  iflg = 1  dtime out of range
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : none
C-
C-   Created   27-MAY-1997  Bogdan Niczyporuk
C-
C-   Called by sim_dcdigi, sim_bgrd
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
C
C Subroutine parameters
      INTEGER il, iflg
      REAL dca, dtime
C
C External functions
      REAL SGAUS
C
C Local variables
      INTEGER i, is, ig, Ng
      REAL d, ds, dd, dtdd
C
C Convert the Drift Time into Drift Distance:  dtime = f'(dca)
C
      iflg = 0
      is   = (il-1)/6 + 1
      d    = ABS(dca)
C
C Get d smeared
      d    = d + zderr(is+1)*SGAUS(iseed)
      ds   = ABS(d)
      dd   = ds - td_h(3,il)
      IF(dd .GE. 0.) ds = td_h(3,il) - dd
C
C Here is an option of constant velocity in a drift cell (the old way)
C
      IF(zderr(9).EQ.1.) THEN
        dtime = ABS(ds/vgas)
        GO TO 999
      ENDIF
C
C Now get measured Drift Time 
C 
      Ng = td_h(1,il) - 1.      
      DO i = 1,Ng
        IF(ds.GE.td_f(2,i,il) .AND. ds.LT.td_f(2,i+1,il)) THEN
          ig = i
          GO TO 12
        ENDIF
      ENDDO
 12   CONTINUE
      dtdd  = (td_f(1,ig+1,il) - td_f(1,ig,il))
     1       /(td_f(2,ig+1,il) - td_f(2,ig,il))
      dtime = td_f(1,ig,il) + dtdd*(ds - td_f(2,ig,il))
C
C
  999 RETURN
      END
E 1
