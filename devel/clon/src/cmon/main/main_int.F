
#include "sinclude/sda_def.F"

C======================================================================
      PROGRAM main_int
C----------------------------------------------------------------------
C
      IMPLICIT NONE
      SAVE
#include "sdabos.inc"
#include "sdakeys.inc"
#include "wcs.inc"
C
C Local variables
      INTEGER inter
C
C Init FIRST (BCS) BOS array to store data
      call bos(iw,ndimb)
C Init SECOND (WCS) BOS array to store geometry etc constants
      call bos(jw,jndimb)
C set interactive flag to 1
      inter = 1
C Initialization of SDA
      call cminit
C PAW, HIGZ and KUIP initialization
      call uthinit
      call utkinit
C Closing of SDA
      call dclast(iw)
C
      STOP
      END
C======================================================================
      SUBROUTINE sda_enter
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Read parameter(s) that come after the command
C-                         typed in by the user in SDA
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: sda_draw, sda_dsect
C-
C-   Created   6-JUNE-1991   Billy K. Leung
C-   Modified  29-MAY-1992   Bogdan Niczyporuk
C-
C-    Called by SDA_DEF
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdabos.inc"
#include "sdakeys.inc"
#include "sdadraw.inc"
C
C External function
      INTEGER  iucomp
C
C Local variables
      INTEGER NPAR, ipar(10)
      INTEGER index, i, nswitch, iunit, iwk, ntrigger
      REAL par(10)
      INTEGER iret, ifevb, inter, iwmin,iwmax
      REAL pmin,pmax,charge,thetamin,thetamax,ltarget,rtarget,ztarget
C
      CHARACTER*32 CMD
      CHARACTER*4 num
C formal to make 'dcdsect' and 'dcdvert' happy
      integer jw(100), ana(100)
C
C  CMD - sda command; NPAR - number of parameters follow CMD
      CALL KUPATL(CMD,NPAR)
C
      IF (CMD.EQ.'BGRD') THEN
        IF (NPAR.GT.1) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1, NPAR
            CALL KUGETR(zbgrd(index))
          ENDDO
          WRITE(6,101) zbgrd(1)
  101     FORMAT(' BGRD ',F4.1)
        ENDIF
C
      ELSE IF (CMD.EQ.'DERR') THEN
        nswitch = 8
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1, NPAR
            CALL KUGETR(zderr(index))
          ENDDO
          WRITE(*,*) ' Mults       DC resolutins for SL1-SL6      DCeff'                 
          WRITE(6,102) (zderr(i), i=1,nswitch)
  102     FORMAT(F6.1,1X,6F6.3,1X,F6.3)
        ENDIF
C
      ELSE IF (CMD.EQ.'DSEC') THEN
        nswitch = 4
        IF(NPAR.LE.nswitch) THEN
          ipar(1) = 1        ! sector
          par(2)  = 0.02     ! scale
          par(3)  = 7.0      ! PosZ
          par(4)  = 3.0      ! PosY
          DO i = 1,NPAR
            IF(i.LE.1) CALL KUGETI(ipar(i))
            IF(i.GT.1) CALL KUGETR( par(i))
          ENDDO
          CALL ICLRWK(0,1)
          CALL dcdvert(jw, ana, ipar(1), par(2),par(3),par(4))
        ELSE
          WRITE(*,*) ' Too many parameters. See HELP Draw'
        ENDIF
C
      ELSE IF (CMD.EQ.'KINE') THEN
        nswitch = 9
        IF (NPAR.GT.9) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO i = 1,NPAR
            CALL KUGETR(zkine(i))
          ENDDO
          WRITE(*,*)'  Type      Q   Pmin   Pmax  THmin  THmax PHImin'//      
     +           ' PHImax      mass'
          WRITE(6,103) (zkine(i),i=1,nswitch)
  103     FORMAT(2F7.1,6F7.2,F10.6)
        ENDIF
C
      ELSE IF (CMD.EQ.'META') THEN
        ipar(1) =  10
        ipar(2) = -112
        par(3)  = sda_dv(1)
        par(4)  = sda_dv(2)
        IF (NPAR.LT.1) THEN
          iunit = ipar(1)
          iwk   = ipar(2)
          CALL KUPROI('Logical Unit  > ', iunit)
          CALL KUPROI('WorkStation   > ', iwk)
          CALL KUPROR('Device Length > ', sda_dv(1))
          CALL KUPROR('Device Width  > ', sda_dv(2))
        ELSE
          DO i=1, NPAR
            IF(i.LE.2) CALL KUGETI(ipar(i))
            IF(i.GT.2) CALL KUGETR( par(i))
          ENDDO
          iunit     = ipar(1)
          iwk       = ipar(2)
          sda_dv(1) = par(3)
          sda_dv(2) = par(4)
        ENDIF
        OPEN(UNIT=iunit,FILE='sda.metafile',FORM='formatted',
     1       STATUS='unknown')
        CALL IGMETA(iunit,iwk)
        WRITE(*,*) 'META ',iunit,' ',iwk,sda_dv(1),sda_dv(2)
        CALL IGRNG(sda_dv(1),sda_dv(2))
        CALL ISWN(4,sda_wc(1),sda_wc(2),sda_wc(3),sda_wc(4))
        CALL ISVP(4,sda_vc(1),sda_vc(2),sda_vc(3),sda_vc(4))
        CALL ISELNT(4)
C
      ELSE IF (CMD.EQ.'ANAL') THEN
        nswitch = 10
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1, NPAR
            CALL KUGETI(lanal(index))
          ENDDO
          WRITE(*,*) ' Level Templ nlaSL1 nlaSL2 nlaSL3 nlaSL4 nlaSL5'//                                              
     +' nlaSL6 MaxSeg DeadChan'
          WRITE(6,104) (lanal(i), i=1,nswitch)
  104     FORMAT(2I6,8I7)
        ENDIF
C
      ELSE IF (CMD.EQ.'STEP') THEN
        nswitch = 3
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1,NPAR
            CALL KUGETR(zstep(index))
          ENDDO
          WRITE(*,*) ' STEPmin  STEPmax   Dangle'
          WRITE(6,105) (zstep(i), i=1,nswitch)
  105     FORMAT(2F9.1,F9.4)
        ENDIF
C
      ELSE IF (CMD.EQ.'SWIT') THEN
        nswitch = 10
        IF ((NPAR.LT.1).OR.(NPAR.GT.2)) THEN
          WRITE(*,*) 'switch 1-10 or HELP SWIT'
        ELSE
          CALL KUGETI(index)
          IF (index.GT.nswitch) THEN
            WRITE(*,*) 'switch number not available!!'
          ELSE
            WRITE(num(1:2),FMT='(I1)') isw(index)
            CALL KUPROI('New value > ',isw(index))
          ENDIF
        ENDIF
        WRITE(*,*) 'isw1 isw2 isw3 isw4 isw5 isw6 isw7 isw8 isw9 isw10 '
        WRITE(6,106) (isw(i), i=1,nswitch)
  106   FORMAT(10I5)
C
      ELSE IF (CMD.EQ.'TFIT') THEN
        nswitch = 6
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1,NPAR
            CALL KUGETR(ztfit(index))
          ENDDO
          WRITE(*,*) '  Iter  VertC TMflag ChiDif IPLmin IPLmax'             
          WRITE(6,107) (ztfit(i), i=1,nswitch)
  107     FORMAT(3F7.1,F7.2,2F7.1)
        ENDIF
C
      ELSE IF (CMD.EQ.'CUTS') THEN
        nswitch = 10
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1, NPAR
            CALL KUGETR(zcut(index))
          ENDDO
          WRITE(*,*) '   Nevt Nclust  Nlink PRchi2  Finde LRchi2 FFchi2
     1Ntr/ev  t0Tag  t0ToF'        
          WRITE(6,108) (zcut(i), i=1,nswitch)
  108     FORMAT(1X,10F7.2)
        ENDIF
C
      ELSE IF (CMD.EQ.'MAGN') THEN
        WRITE(*,*) '   B-type  +/-Itorus  +/-Iminit  +/-Iptarg'
        WRITE(6,109) zmagn(1),zmagn(2),zmagn(3),zmagn(4)
  109   FORMAT(5X,A4,1X,F10.3,1X,F10.3,1X,F10.3)
C
      ELSE IF (CMD.EQ.'PRNT') THEN
        nswitch = 7
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1, NPAR
            CALL KUGETI(lprnt(index))
          ENDDO
          WRITE(*,*) '  PRNT(L) Init(-) Revb(0) Segm(1) PFit(2) LRam(3)
     1FFit(4) DST(2,4)'                               
          WRITE(6,110) (lprnt(i), i=1,nswitch)
  110     FORMAT(7X,7I8)
        ENDIF
C
      ELSE IF (CMD.EQ.'GCOR') THEN
        nswitch = 11
        IF (NPAR.GT.nswitch) THEN
          WRITE(*,*) 'Too many parameter(s)!!'
        ELSE
          DO index = 1,NPAR
            CALL KUGETR(zgcor(index))
          ENDDO
          WRITE(*,*) ' DPHB1   DXB1   DYB1  DPHB2   DXB2   DYB2  DPHB3                    
     1 DXB3   DYB3 DXBCAR DYBCAR'                     
          WRITE(6,111) (zgcor(i), i=1,nswitch)
  111     FORMAT(9F7.3,2F7.2)
        ENDIF
C
      ELSE IF (CMD.EQ.'CLEAR') THEN
        CALL ICLRWK(0,1)
C
      ELSE IF (CMD.EQ.'TRIGGER') THEN
        ntrigger = 1
        IF(NPAR.GT.0) THEN
          CALL KUGETI(ntrigger)
        ENDIF
C set interactive flag to 1
        inter = 1
C
        IF(iucomp(4HSIM ,levb,10).NE.0) THEN
          print *,'SIMULATION mode'
          ifevb = 2
        ELSE
          print *,'RECONSTRUCTION mode'
          ifevb = 1
        ENDIF
C
        DO i=1,ntrigger
          IF(ifevb.EQ.1) THEN ! Read data event
            call cmloop(iw,ifevb,inter,Nevt,iret)
          ELSE IF(ifevb.EQ.2) THEN ! Road simulation
            call sim_loop(iw,ifevb,inter,Nevt,iwmin,iwmax,pmin,pmax,
     &           charge,thetamin,thetamax,ltarget,rtarget,ztarget)
          ENDIF
          call dcerun(iw)
          call dch2hbook()
        ENDDO
C print and draw last event in the loop
        call dc_statp
        call iclrwk(0,1)
        call dcdsect(iw, ana, 1,  .008,  4.0, 10.6)
        call dcdsect(iw, ana, 4, -.008,  4.0, 10.6)
        call dcdsect(iw, ana, 2,  .008, 14.0, 10.6)
        call dcdsect(iw, ana, 5, -.008, 14.0, 10.6)
        call dcdsect(iw, ana, 3,  .008,  9.0,  4.0)
        call dcdsect(iw, ana, 6, -.008,  9.0,  4.0)
C
      ELSE
      ENDIF
C
      RETURN
      END

