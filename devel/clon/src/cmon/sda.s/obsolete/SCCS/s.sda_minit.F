h42375
s 00000/00000/00000
d R 1.2 01/11/19 19:02:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_minit.F
e
s 00300/00000/00000
d D 1.1 01/11/19 19:02:39 boiarino 1 0
c date and time created 01/11/19 19:02:39 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_minit
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Reads or create a table of magnetic field.
C-
C-   Inputs  :
C-   Outputs : COMMON/sdabgrid/
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: sda_torus
C-
C-   Created   28-OCT-1990   Bogdan Niczyporuk
C-   Modified  10-SEP-1998   Franz Klein
C-
C-   Called by sda_init
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdabgrid.inc"
C
C Local variables
      INTEGER i, NX3,NYNX3, IX,IY,IZ, NYc, Indx, nrw
      INTEGER numra,numrb,icl,ierr, numdb,ncol,nrow,nch, nread
      REAL time_init, time_out, time_exe
      REAL B(3),R(3), Wlen, Sbt,Smt,Spt, Cmax_t,Cmax_m,Cmax_p
C
      CHARACTER*24 dtimes
      CHARACTER*64 fname
      CHARACTER*12 format1
      CHARACTER*4  format2
      CHARACTER*8 kname,Tname,Mname,Pname, hname1,hname2
C
C Open and read old table
C
      Mx=0
      IF(mgtyp .EQ. 0) THEN
        IF(zmagn(3)*zmagn(4).NE.0.) THEN
          WRITE(6,*) ' *** One can not have both BMINT & BPOLT  ***'
          WRITE(6,*) ' *** Check MAGN card in your  sda.config  ***'
          STOP
        ENDIF
        IF(zmagn(2).EQ.0..AND.zmagn(3).EQ.0..AND.zmagn(4).EQ.0.) THEN
          WRITE(6,*) ' *** All currents in MAGN card are zeros  ***'          
          WRITE(6,*) ' *** The option selected " MAGN   NONE    ***'          
          MGTYP = 5
          GOTO 999
        ENDIF
C Initialize just in case all currents are set to zero's
        Tname  = 'BTORU'
        Mname  = 'BMINT'
        Pname  = 'BPOLT'
        Cmax_t = Imax_t
        Cmax_m = Imax_m
        Cmax_p = Imax_p
        Sbt    = 0.
        Smt    = 0.
        Spt    = 0.
C
C Read B for PTF
        IF(ABS(zmagn(4)).GT.0.) THEN
          CALL getenvf('BPOL',fname)
          CALL FPARM('OPEN BGRID UNIT=55 FILE="' // fname // '"')
          CALL FRNAME('BGRID')
          CALL FRKEY(kname,numra,numrb,icl,ierr)
          Pname = kname
          CALL FRHDR(hname1,numdb,ncol,nrow,format1,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,MX,nrw)
          Cmax_p = Cpmax
          IF(Mgr.GT.Ng) THEN
            WRITE(6,100) Mgr, Ng
            STOP
          ENDIF
          CALL FRHDR(hname2,numdb,ncol,nrow,format2,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,Qm,nrw)
          CALL FPARM('CLOSE BGRID')
          CALL GetASCIITime(dptime,dtimes)
          WRITE(6,106) kname,dtimes
C Rescale the polarized target field
          Spt = zmagn(4)/Cmax_p
          DO i = 1,Ng
            Qm(i) = Spt*Qm(i)
          ENDDO
        ENDIF
      print *,'==============>',zmagn(2)
C
C Read B for Torus
        IF(ABS(zmagn(2)).GT.0.) THEN
          CALL getenvf('BTOR',fname)
          CALL FPARM('OPEN BGRID UNIT=55 FILE="' // fname // '"')
          CALL FRNAME('BGRID')
          CALL FRKEY(kname,numra,numrb,icl,ierr)
          Tname = kname
          CALL FRHDR(hname1,numdb,ncol,nrow,format1,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,NX,nrw)
          Cmax_t = Cmax
          IF(Ngr.GT.Ng) THEN
            WRITE(6,100) Ngr, Ng
            STOP
          ENDIF
          CALL FRHDR(hname2,numdb,ncol,nrow,format2,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,Q,nrw)

          CALL FPARM('CLOSE BGRID')
          CALL GetASCIITime(dtime,dtimes)
          WRITE(6,101) kname,dtimes
C Rescale the big torus field
          Sbt = zmagn(2)/Cmax_t
          DO i = 1,Ng
            Q(i) = Sbt*Q(i)
          ENDDO
        ELSE
          DO i = 1,Ng
            Q(i) = 0.
          ENDDO
        ENDIF
C
C Read B for Mini Torus
        IF(ABS(zmagn(3)).GT.0. .AND. Mx.EQ.0) THEN
          CALL getenvf('BMIN',fname)
          CALL FPARM('OPEN BGRID UNIT=56 FILE="' // fname // '"')
          CALL FRNAME('BGRID')
          CALL FRKEY(kname,numra,numrb,icl,ierr)
          Mname = kname
          CALL FRHDR(hname1,numdb,ncol,nrow,format1,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,NX,nrw)
          Cmax_m = Cmax
          IF(Ngr.GT.Ng) THEN
            WRITE(6,100) Ngr, Ng
            STOP
          ENDIF
          CALL FRHDR(hname2,numdb,ncol,nrow,format2,nch,ierr)
          nrw = ncol*nrow
          CALL FRDAT(nread,Qm,nrw)
          CALL FPARM('CLOSE BGRID')
          CALL GetASCIITime(dtime,dtimes)
          WRITE(6,104) kname,dtimes
C Rescale the mini torus field and add to the big torus field
          Smt = zmagn(3)/Cmax_m
          DO i = 1,Ng
            Q(i) = Q(i) + Smt*Qm(i)
          ENDDO
        ENDIF
        WRITE(6,105) Sbt,Tname, Smt,Mname, Spt,Pname
      ENDIF
C
C Calculate field and Write Table (for MAGN  with 'TORU' or 'MINT' or 'POLT')
C
      IF((mgtyp.GE.1.AND.mgtyp.LE.3) .AND. zgrid(1).GT.0.) THEN
        WRITE(6,*)' Start calculation of field map for type',mgtyp
        CALL timex(time_init)
C Get grid values from data card
        NX = zgrid(2)
        NY = zgrid(3)
        NZ = zgrid(4)
        Xmin = zgrid(5)
        Ymin = zgrid(6)
        Zmin = zgrid(7)
        DX = zgrid(8)
        DY = zgrid(9)
        DZ = zgrid(10)
        Ngr = NX*NY*NZ*3
        IF(Ngr.GT.Ng) THEN
          WRITE(6,100) Ngr, Ng
          STOP
        ENDIF
        IF(mgtyp.EQ.3) GO TO 5
C
        NX3   = NX*3
        NYNX3 = NY*NX3
C
        R(3) = Zmin - DZ
        DO 3 IZ = 1,NZ
          R(3) = R(3) + DZ
C
          R(2) = Ymin - DY
          DO 2 IY = 1,NY
            R(2) = R(2) + DY
C
            R(1) = Xmin - DX
            DO 1 IX = 1,NX
              R(1) = R(1) + DX
C
              Wlen = ABS(R(1))*0.57735 + 3.*DY
              NYc = INT(Wlen/DY + 0.1)
              IF(NYc.GT.NY) NYc = NY
C
              IF(IY.GT.NYc) THEN
                B(1) = 0.
                B(2) = 0.
                B(3) = 0.
              ELSE
                CALL sda_torus(R,B,mgtyp)
              ENDIF
C
              Indx = (IZ-1)*NYNX3 + (IY-1)*NX3 + (IX-1)*3 + 1
              CALL UCOPY(B,Q(Indx),3)
C
    1       CONTINUE
    2     CONTINUE
    3   CONTINUE
        GO TO 9
C
C Calculation of Polarized Target Field  grids (in plane XZ)
C
 5      CONTINUE
        IF(NY.NE.1) NY=1
        NX3   = NX*3
        R(2) = 0.0
C
        R(3) = Zmin - DZ
        DO IZ = 1,NZ
          R(3) = R(3) + DZ
C
          R(1) = Xmin - DX
          DO  IX = 1,NX
            R(1) = R(1) + DX
C
            CALL sda_ptf(R,B)
C
            Indx = (IZ-1)*NX3 + (IX-1)*3 +1
            CALL UCOPY(B,Q(Indx),3)
          ENDDO
        ENDDO
C
C Open and write new table
C
 9      CONTINUE
C
        CALL FPARM(' OPEN BGRID UNIT=55 FILE="bgrid.fpk"'//
     1             'RECL=36000 WRITE STATUS=NEW')                          
C
        CALL GetUnixTime(dtime)
        IF(mgtyp.EQ.1) THEN
          kname = 'BTORU'
          Cmax  = Imax_t
        ENDIF
        IF(mgtyp.EQ.2) THEN
          kname = 'BMINT'
          Cmax = Imax_m
        ENDIF
        IF(mgtyp.EQ.3) THEN
          kname = 'BPOLT'
          Cmax = Imax_p
        ENDIF
C
        CALL FWNAME('BGRID')
C  numra < 9999  (run No.)
C  numrb = mgtyp (Big TORUS or Mini TORUS or PTF magnetic field)
C    icl = 11   (clasification 11 - 20  offline analysis)
C----------------- kname numra numrb icl
        CALL FWKEY(kname, 9999,mgtyp, 11)
        nrw = 12
C----------------- hname1,numdb ncol nrow   format1
        CALL FWHDR('BHDR',    1, nrw,   1,'3I,6F,3I')
        CALL FWDAT(nrw,NX)
        nrw = Ngr
C----------------- hname1,numdb ncol nrow   format2
        CALL FWHDR('BGRD',    1, nrw,   1,   'F'    )
        CALL FWDAT(nrw,Q)
        CALL FWEND(ierr)
        IF(ierr.NE.0) WRITE(6,*) ' *** bgrid_to_fpk error ***'
        CALL FWEOD
        CALL FPARM('CLOSE BGRID')
C
        WRITE(6,102) NX,NY,NZ,Xmin,Ymin,Zmin,DX,DY,DZ,Cmax,dtime,Ngr
C Use this Table if there are events to be analyzed (change initial MGTYP)
        mgtyp = 0
        CALL timex(time_out)
        time_exe = time_out - time_init
        WRITE(6,103) time_exe
      ENDIF
C
  100 FORMAT(/' sda_minit.F: Needs',I8,' but in "sdabgrid.inc" is ',I8)
  101 FORMAT( ' Read "bgrid_t.fpk"  file for ',A8,' created on  ',A24)
  102 FORMAT(/' B-table "bgrid.fpk" written '/3I4,1X,6F7.1,1X,I4,I11,I8)       
  103 FORMAT(/' CPU time for calulation of TABLE =', F10.3)
  104 FORMAT( ' Read "bgrid_m.fpk"  file for ',A8,' created on  ',A24)
  105 FORMAT( ' Field=',F6.3,'x',A5,' + ',F6.3,'x',A5,' + ',F6.3,'x',A5)                                  
  106 FORMAT( ' Read "bgrid_p.fpk"  file for ',A8,' created on  ',A24)
C
 999  CONTINUE
C
C call new version
      call dcminit
C
      RETURN
      END
E 1
