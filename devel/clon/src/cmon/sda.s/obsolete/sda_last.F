C======================================================================
      SUBROUTINE sda_last(iw)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods   : Finishing of SDA program.
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: sda_dlast, usda_last, dcstatp
C-
C-   Created     19-OCT-1990  Bogdan Niczyporuk
C-
C-   Called by sda_erun
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaevgen.inc"
C
      INTEGER iw(1000)
C
C External functions
      INTEGER iucomp
      REAL    SRAN 
C
cc      INTEGER type, dtype, i
cc      LOGICAL ack
cc      CHARACTER*25 caption   
cc      COMMON/ErrBook/ type(10), dtype(10), ack(10), caption(10)
C
C Local variables
      INTEGER Ntrig, Nrec, ierr, dtime
      REAL Etrig, Eacc, Erec, RNlast
      CHARACTER*24 dtimes 
C
C
C User last routine - empty so comment it out
**      CALL usda_last
C
C Trigger, acceptance, reconstruction efficiencies
      IF(iucomp(4HSIM ,levb,10).NE.0) THEN

        call dcstatp2
c        Ntrig = type(1)
c        Etrig = Ntrig/FLOAT(Nevin_prod)
c        Eacc  = Nevin_acc/FLOAT(Nevin_prod)
c        Nrec  = type(8)                    
c        Erec  = Nrec/FLOAT(Nevin_acc)
c        WRITE(6,10) Nevin_prod, Ntrig,Etrig, Nevin_acc,Eacc, Nrec,Erec
c 10     FORMAT(/' Nprod =',I7/
c     1          ' Ntrig =',I7,'     Etrig = Ntrig/Nprod =',F10.4/
c     2          ' Nacc  =',I7,'     Eacc  = Nacc/Nprod  =',F10.4/
c     3          ' Nrec  =',I7,'     Erec  = Nrec/Nacc   =',F10.4)


        RNlast = SRAN(iseed)
        WRITE(6,11) RNlast,iseed
 11     FORMAT(/' Last RAN = ',F10.8,' last iseed = ',I10)
      ENDIF      
C
C Print statistics for the used cuts (if Batch Mode)
      IF(iucomp(4HBAT ,lmode,1) .NE. 0) THEN
        CALL dcstatp
c        print *,'cleanup counters ...'
cc        do i=1,10
cc          type(i) = 0
cc        enddo
c        print *,'... done.'
      ENDIF
C
C Write dictionary (templets) if selected in script "ftn33"
cboy      CALL prwrite ! moved to makedict/makefulldict !!!
C
C Write last DST buffer "DSTOUTPUT" for reconstructed events
      IF(iucomp(4HWDST,levb,10).NE.0) CALL FWBOS(IW,2,'0',ierr)
C
C Write last EVB buffer "SIMOUTPUT" for simulated events
      IF(iucomp(4HWSIM,levb,10).NE.0) CALL FWBOS(IW,3,'0',ierr)
C
C Print I/O statistic and close the all opened FPACK units
      CALL FPSTAT
      CALL FPARM('CLOSE')
C
C Get JOB Time
      CALL GetUnixTime(dtime)
      CALL GetASCIITime(dtime,dtimes)
      WRITE(6,101) dtimes
 101  FORMAT(/'                 JOB End= ',A24/)
C
 999  CONTINUE
      RETURN
      END
