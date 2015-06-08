h09622
s 00000/00000/00000
d R 1.2 01/11/19 19:02:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_erun.F
e
s 00107/00000/00000
d D 1.1 01/11/19 19:02:37 boiarino 1 0
c date and time created 01/11/19 19:02:37 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_erun(iw)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : This Routine is called at the End of each RUN
C-                        
C-
C-   Inputs  : REVB
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: usda_erun, sda_last
C-
C-   Created    9-OCT-1995   Bogdan Niczyporuk
C-
C-
C-   Called by sda_anal
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
      INTEGER iucomp, ETNLINK, ETNCREATE
C
C Local variables
      INTEGER ind, icycle, uout,ierr
      INTEGER Version,RunNo,EvtNo,RunDate,EvtType,RocSt,EvtClass,EvtWt        
      REAL time_evt
      CHARACTER*24 dtimes 
      CHARACTER*10  NameHis
C
C
C Report End of Run
      RunNo = 0
      ind = ETNLINK(IW,'HEAD',0)
      IF(ind.NE.0) THEN
        Version  = IW(ind+1)
        RunNo    = IW(ind+2)
        IF(RunNo.EQ.0) RunNo = ltrig(1)
        EvtNo    = IW(ind+3)
        RunDate  = IW(ind+4)
        EvtType  = IW(ind+5)
        RocSt    = IW(ind+6)
        EvtClass = IW(ind+7)
        EvtWt    = IW(ind+8)
      ENDIF
      IF(iucomp(4HWDST,levb,10) .NE. 0) THEN
        CALL ETLDROP(IW,'E')
        CALL ETLCTL(IW,'E=','HEAD')
        ind = ETNCREATE(IW,'HEAD',0, 8,1)
        IW(ind+1) = Version
        IW(ind+2) = RunNo
        IW(ind+3) = EvtNo 
        IW(ind+4) = RunDate
        IW(ind+5) = EvtType
        IW(ind+6) = RocSt
        IW(ind+7) = EvtClass
        IW(ind+8) = EvtWt
        uout = 2
        CALL FWBOS(IW,uout,'E',ierr)
      ENDIF
      IF(RunNo.EQ.0) THEN
        RunNo = ltrig(1)
        CALL GetUnixTime(RunDate)
      ENDIF
      CALL GetASCIITime(RunDate,dtimes)
      WRITE(6,1001) RunNo,EvtType,EvtClass,dtimes
C
C Compute the average CPU time per one event
      IF(iucomp(4HBAT ,lmode,1) .NE. 0) THEN

        call dcstatp1
c        CALL timex(time(10))
c        time_evt = 0.
c        IF(type(1).GT.0) time_evt = (time(10) - time(9))/FLOAT(type(1))
c        WRITE(6,*)' '
c        WRITE(6,1002) type(1), (time(10) - time(9))/60., time_evt

C
C Set names for histogram file
        WRITE(NameHis,'(A1,I5.5,A4)') 'r',ltrig(1),'.his'
        CALL HROPEN(53,'RHIS',NameHis,'N',1024,ierr)
C Write and Close the HBOOK Histograms
        CALL HROUT(0,icycle,'T')
        CALL HREND('RHIS')
        CLOSE (53)
      ENDIF
C
C User closing - empty so comment it out
**      CALL usda_erun
C
 1001 FORMAT(//' END RUN#',I6,'  Type/Clas=',I4,'/',I2,'  Date= ',A24/)
 1002 FORMAT(' Number of events:',I8,'  Time [min]:',F10.3,
     &       '  Time/event [s]:',F10.3)
C
      RETURN
      END
E 1
