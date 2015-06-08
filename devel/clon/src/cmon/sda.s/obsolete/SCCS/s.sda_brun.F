h53336
s 00000/00000/00000
d R 1.2 01/11/19 19:02:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_brun.F
e
s 00067/00000/00000
d D 1.1 01/11/19 19:02:36 boiarino 1 0
c date and time created 01/11/19 19:02:36 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_brun(iw)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This Routine is called at the begin of each RUN
C-                        
C-
C-   Inputs  : REVB
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: usda_brun, sda_calib
C-
C-   Created    9-OCT-1995   Bogdan Niczyporuk
C-
C-
C-   Called by SDA_RUN
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
C Subroutine variables
      INTEGER iw(1000)
C
C External
      INTEGER ETNLINK
C
C Local variables
      INTEGER ind, RunNo, EvtType, EvtClass, RunDate
      CHARACTER*24 dtimes
C
      print *,'sda_brun: iseed =',iseed
C
C Report Begin of Run
      ind = ETNLINK(IW,'HEAD',0)
      IF(ind.NE.0) THEN

        RunNo    = IW(ind+2)
ccc        RunNo = 14317

        IF(RunNo.EQ.0) RunNo = ltrig(1)
        RunDate  = IW(ind+4)
        EvtType  = IW(ind+5)
        EvtClass = IW(ind+7)
        CALL GetASCIITime(RunDate,dtimes)
        WRITE(6,1001) RunNo,EvtType,EvtClass,dtimes
      ELSE
        RunNo = ltrig(1)
      ENDIF
C
C Initialize your Histograms
*      CALL usda_brun
      call dchinit
C
C
 1001 FORMAT(//' BEGIN RUN#',I6,'  Type/Clas=',I4,'/',I2,' Date= ',A24/)
C
 999  CONTINUE
      RETURN
      END
E 1
