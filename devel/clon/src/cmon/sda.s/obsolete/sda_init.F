      subroutine sda_hinit
C
      implicit none
C
      INTEGER NWPAW
      PARAMETER (NWPAW = 2000000)
      REAL PAW
      COMMON/PAWC/PAW(NWPAW)
C
C Initialization of HBOOK
      CALL HLIMIT(NWPAW)
C
      call dchinit
C
      return
      end
C======================================================================
      subroutine sda_kinit
C
      implicit none
C
#include "sdakeys.inc"
#include "sdadraw.inc"
C
      INTEGER NWKUIP, KWKTYP, iucomp
      PARAMETER (NWKUIP = 30000)
C
C IGTERM is a KUIP-defined subroutine acts as a graphics buffer between screens
      EXTERNAL IGTERM
C
C IF Interactive Version of SDA selected in Configuration File
      IF(iucomp(4HINT ,lmode,1) .NE. 0) THEN
C
C Initialize HIGZ constants
        KWKTYP    = 1
        sda_dv(1) = 20.
        sda_dv(2) = 20.
          sda_vc(1) = 0.
          sda_vc(2) = 1.
          sda_vc(3) = 0.
          sda_vc(4) = 1.
        sda_wc(1) = 0.
        sda_wc(2) = 20.
        sda_wc(3) = 0.
        sda_wc(4) = 20.
C
C Initialize KUIP and HIGZ graphics
        CALL KUINIT(NWKUIP)
        CALL KUTERM(IGTERM)
C
C Initialize HIGZ and request for station type
        CALL IGINIT(0)
        CALL KUPROI('Terminal type > ',KWKTYP)
        PRINT *, 'Terminal ', KWKTYP, ' selected.'
C
C Initialize Graphic Package Control
        CALL IGSSE(6,KWKTYP)
C
C Set up the graphic viewport with max. world co-ordinates (WC = DC).
        IF(KWKTYP.EQ.7878) THEN
          CALL ISWN(5,sda_wc(1),sda_wc(2),sda_wc(3),sda_wc(4))
          CALL ISVP(5,sda_vc(1),sda_vc(2)*.8,sda_vc(3),sda_vc(4)*.8)
          CALL ISELNT(5)
        ELSE
          CALL ISWN(3,sda_wc(1),sda_wc(2),sda_wc(3),sda_wc(4))
          CALL ISVP(3,sda_vc(1),sda_vc(2),sda_vc(3),sda_vc(4))
          CALL ISELNT(3)
        ENDIF
C
C Initialize Command Processor (KUIP) commands
        CALL sda_def
C
C Yield control to the User Interface
        CALL KUEXEC('SET/PROMPT ''SDA >''')
        CALL KUWHAT
C
C End HIGZ Session
        CALL IGEND
C
C DO NOT execute code in "sda_run"
        Nevt = 0
      ENDIF
C
      return
      end
C======================================================================
      SUBROUTINE sda_init
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Initialization of SDA program.
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: BOS, ffgo, dcminit, sda_dinit, sda_ginit, sda_tagcal,
C-          usda_init, sda_def
C-
C-   Created   26-OCT-1990   Bogdan Niczyporuk
C-
C-    Called by SDA_MAIN
C-
C--------1---------2---------3---------4---------5---------6---------7--
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
C Common for the CERN routine "getenvf"
      INTEGER islate
      COMMON/SLATE/ islate(40)
C
C External funtions
      INTEGER iucomp, LNBLNKL, ICFIND
C
C Local variables
      INTEGER dtime, strlen
      CHARACTER*132 fname, dname, sname
      CHARACTER*24 dtimes
      CHARACTER*132  pgm_name
      CHARACTER*64   host_name
      REAL r2r3cor(6,2)
C
C Get JOB Time
      CALL GetUnixTime(dtime)
      CALL GetASCIITime(dtime,dtimes)
C Write SDA Version
      WRITE(6,100)
  100 FORMAT('            *****************************************'/
     1       '            *                                       *'/
     2       '            *   SDA v98d created on Dec. 21, 1998   *'/
     3       '            *       by Bogdan Niczyporuk/JLab       *'/
     4       '            *         (Preliminary Version)         *'/
     5       '            *****************************************')
C Read data cards (Steering Cards: Configuration file "sda.config")
      CALL ffgo
C Initialize the descriptors for cuts (character*25 long)
      CALL dcstati(1,'sda_anal:  Ntrig (Nevt) =')
      CALL dcstati(2,'ana_segm:  Nevt w. segm =')
      CALL dcstati(3,'ana_link:  Nevt w. link =')
      CALL dcstati(4,'ana_prfit: Nevt PR chi2 <')
      CALL dcstati(5,'ana_finde: Nevt beta0   =')
      CALL dcstati(6,'ana_dcam:  Nevt LR chi2 <')
      CALL dcstati(7,'ana_fit:   Nevt FF chi2 <')
      CALL dcstati(8,'sda_anal:  Nevt w. Ntrk =')
C
C Read Magnetic Field(s)
      CALL dcminit
C
C Read Road Dictionary
      CALL prinit
C
C Set simulation flag
      ifsim = 0
      IF(iucomp(4HSIM ,levb,10) .NE. 0) ifsim = 1
      print *,'ifsim=',ifsim
C
C Initialize Geometry
      IF(ifsim .EQ. 1) THEN
        CALL sda_ginit
c        call sda_ginit1
      ELSE
        call dc_make_dgeo(ltrig(1), 1, r2r3cor)
        call dc_geom_banks
        call sda_ginit2
      ENDIF
C
C Get Calibration Constants for current "RunNo"
      print *,'calibrationrun=',ltrig(1)
      CALL sda_calib(ltrig(1))
C
C Tagger initialization
      IF(zbeam(1).EQ.0.) CALL sda_tagcal(ltrig(1))
C
      WRITE(6,*)
C
C Open BOS/FPACK input file (REVB in the configuration file)
      IF(iucomp(4HREVB,levb,10) .NE.0) THEN
        CALL getenvf('REVB',fname)
        IF(islate(1) .EQ. 0) THEN
          WRITE(6,*) 'REVB not set, so opening data file up to user'
        ELSE
          CALL FPARM('OPEN EVBINPUT UNIT=1 FILE="' // fname // '" ')
          WRITE(6,21) fname
        ENDIF
      ENDIF
C
C Open BOS/FPACK output file (WDST in the configuration file)
      IF(iucomp(4HWDST, levb, 10 ) .NE. 0) THEN
        CALL getenvf('WDST',dname)
        IF(islate(1) .EQ. 0) THEN
          WRITE(dname,'(A3,I5.5,A4)') 'DST',ltrig(1),'.A00'
        ENDIF
        CALL FPARM(' OPEN DSTOUTPUT UNIT=2 FILE="'// dname //'" '//
     1               'RECL=36000 SPLITMB=2047 WRITE STATUS=NEW')
        WRITE(6,21) dname
      ENDIF
C
C Open BOS/FPACK output file (WSIM in the configuration file)
      IF(iucomp(4HWSIM, levb, 10 ) .NE. 0) THEN
        CALL getenvf('WSIM',sname)
        IF(islate(1) .EQ. 0) THEN
          WRITE(sname,'(A3,I5.5,A4)') 'SIM',ltrig(1),'.A00'
        ENDIF
        CALL FPARM('OPEN SIMOUTPUT UNIT=3 FILE="' // sname // '" '//
     1               'RECL=36000 SPLITMB=2047 WRITE STATUS=NEW')
        WRITE(6,21) sname
      ENDIF
C
C User initialisation routine - empty so comment it out
**      CALL usda_init
C
 21   FORMAT(' Opened file: ',A68)
      RETURN
      END
