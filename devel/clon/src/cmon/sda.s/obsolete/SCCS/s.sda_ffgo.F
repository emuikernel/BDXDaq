h28144
s 00000/00000/00000
d R 1.2 01/11/19 19:02:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_ffgo.F
e
s 00126/00000/00000
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
      SUBROUTINE sda_ffgo
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Reads the DATA cards needed for PROGRAM flow.
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created   16-OCT-1990   Bogdan Niczyporuk
C-
C-    Called by sda_init
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
C
C Local variables
      INTEGER i, il, iucomp
C
C
C Initialise FFREAD
      zgrid(1) = 0.
c      CALL ffinit(1000)
c      CALL ffset('LINP',33)
C
C Define keys
c      CALL ffkey('MODE',lmode,1,'MIXE')
c      CALL ffkey('RNDM',iseed,1,'INTE')
c      CALL ffkey('STEP',zstep,10,'REAL')
c      CALL ffkey('ANAL',lanal,10,'INTE')
c      CALL ffkey('CUTS',zcut, 10,'REAL')
c      CALL ffkey('PRNT',lprnt,10,'INTE')
C
c      CALL ffkey('TRIG',ltrig,10,'INTE')
c      CALL ffkey('SWIT',isw,  10,'INTE')
c      CALL ffkey('TIME',time, 10,'REAL')
c      CALL ffkey('MAGN',zmagn,10,'REAL')
c      CALL ffkey('GRID',zgrid,10,'REAL')
c      CALL ffkey('GEOM',lgeom,10,'MIXE')
c      CALL ffkey('GCOR',zgcor,11,'REAL')
c      CALL ffkey('CALI',lcali,10,'MIXE')
c      CALL ffkey('DERR',zderr,10,'REAL')
C
c      CALL ffkey('KINE',zkine,20,'REAL')
c      CALL ffkey('TARG',ztarg,10,'REAL')
c      CALL ffkey('BEAM',zbeam,10,'REAL')
c      CALL ffkey('EVBU',levb, 10,'MIXE')
c      CALL ffkey('LOUT',lout, 36,'INTE')
c      CALL ffkey('TFIT',ztfit,10,'REAL')
c      CALL ffkey('MATX',lmatx,10,'MIXE')
c      CALL ffkey('BGRD',zbgrd,14,'REAL')
C
C Some defaults
      DO i = 1,10
        isw(i)  = 0
        lprnt(i)= 0
      ENDDO
      DO il = 1,36
        lout(il)  = 0
      ENDDO
      zkine(1) = 0.
      zgrid(1) = 0.
C
C Read data cards

cboy#ifdef SIM
cboy      OPEN(UNIT=33,FILE='sda.config',FORM='formatted',STATUS='old')
cboy#endif
      CALL ffgo
      WRITE(6,*)
      WRITE(6,*) 'Read "sda.config" file from your current directory'
cboy#ifdef SIM
cboy      CLOSE (UNIT=33)
cboy#endif




c      print *,'keys:',Run,Nevt,Ndbg,ievt,iseed,mgtyp,lmode,isim,ifsim,             
c     2   step,ltrig,lprnt,time,isw,lgeom,zkine,
c     3   zderr,zmagn,lmatx,zgrid,ztfit,                      
c     4   levb,zbgrd,lout,lanal,zstep,zcut,
c     5   ztarg,zbeam,lcali,zgcor










C Interpret some DATA cards
      IF (iucomp(4HTABL,zmagn,10).NE.0) mgtyp = 0
      IF (iucomp(4HTORU,zmagn,10).NE.0) mgtyp = 1
      IF (iucomp(4HMINT,zmagn,10).NE.0) mgtyp = 2
      IF (iucomp(4HPOLT,zmagn,10).NE.0) mgtyp = 3
      IF (iucomp(4HUNIF,zmagn,10).NE.0) mgtyp = 4
      IF (iucomp(4HNONE,zmagn,10).NE.0) mgtyp = 5
C
      ievt = 0
      Nevt = ltrig(2)
      Ndbg = ltrig(3)
      isim = 0
      ifsim= 0
C
      IF(ztfit(6) .EQ. 0.) ztfit(6) = 39.
      IF(ztfit(5) .EQ. 0.) THEN
        ztfit(5) = 1.
        IF(zmagn(4).NE.0.) ztfit(5) = 4.
      ENDIF
C
C
      RETURN
      END
E 1
