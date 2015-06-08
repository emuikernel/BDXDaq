C======================================================================
      SUBROUTINE sda_tagcal(irun)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : 
C-    read tagger calibration files and put it into COMMON /sdatagcal/
C-
C-   Inputs  : irun  = run number
C-   Outputs : none
C-
C-   Controls:
C-
C-   Library belongs: 
C-
C-   Calls: 
C-
C-   Created   12-JUL-1998   Franz Klein
C-
C-   Called by sda_init
C-
C--------1---------2---------3---------4---------5---------6---------7--
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
      SAVE

#include "sdatagcal.inc"
#include "sdakeys.inc"

C Common for CERN routine "getenvf"
      INTEGER islate
      COMMON/SLATE/ islate(40)

C  input: run number
      INTEGER  irun

c  local variables

      CHARACTER*98  fname, dirname
      REAL  val, val1
      REAL  ADJ_T_COINC, ADJ_E_COINC, E_TDC_MIN, E_TDC_MAX
      REAL  TAG_DSD_window, TAG_ST_window, TAG_DSD_Xtrawindow
      REAL  TAG_ST_Xtrawindow, PS_COINC_WINDOW
      INTEGER  ilun, i,j,k, j1,j2,j3,j4,j5
      DATA ilun /38/

c----------------------------------------------------------------

      call getenvf('CLAS_PARMS',dirname)
      if (islate(1).EQ.0) then
         print *,'sda_tagcal: CLAS_PARMS is not defined - exit'
         stop
         islate(1) = 17
      endif

      do i=1,61
         t1bin(i) = 0
         t2bin(i) = 0
         tagT_status(i) = 0
      enddo
      tagT_status(54) = 2
      tagT_status(61) = 2

      fname = dirname(1:islate(1))//'/tagETcoinc.dat'
      write(6,*)'Reading ',fname
      open(ilun,file=fname,status='old')
      do i=1,384
         read(ilun,'(I4,2I3)',err=121) k,etmin(k),etmax(k) 
         do j=etmin(i),etmax(i)
            if (t1bin(j).EQ.0) t1bin(j)=i
            if (t2bin(j).LT.i) t2bin(j)=i
         enddo
      enddo
      goto 122
 121  write(6,*)' Error reading "tagETcoinc.dat" ; line=',i
      stop
 122  close(Ilun)

      fname = dirname(1:islate(1))//'/tagparam.dat'
      open(Ilun,file=fname,status='old')
      read(Ilun,'(F8.2)') ADJ_T_COINC
      read(Ilun,'(F8.2)') ADJ_E_COINC
      read(Ilun,'(F8.2)') ET_window
      read(Ilun,'(F8.2)') E_TDC_MIN
      read(Ilun,'(F8.2)') E_TDC_MAX
      read(Ilun,'(F8.2)') TAG_DSD_window
      read(Ilun,'(F8.2)') TAG_ST_window
      read(Ilun,'(F8.2)') TAG_DSD_Xtrawindow
      read(Ilun,'(F8.2)') TAG_ST_Xtrawindow
      read(Ilun,'(F8.2)') PS_COINC_WINDOW
      close(Ilun)
      ET_window = ET_window/2.
c
c  define Time Cuts and Time windows
c  

      TAG_RF_MEAN = zcut(9)
Cold      TAG_RF_10SIGMA =  0.228*10.
      TAG_RF_10SIGMA =  15.

Cold      TAG_TCUT(1) = -30.
Cold      TAG_TCUT(2) =   5.
      TAG_TCUT(1) = 20.
      TAG_TCUT(2) = 40.

      RF_OFFSET   = 1.82
      RF_SLOPE(1) = 4.906E-2
      RF_SLOPE(2) = 4.906E-2

      fname = dirname(1:islate(1))//'/tagT-boundaries.dat'
      open (ilun,file=fname,status='old',err=131)
      write(6,*)'Reading energy T boundaries from file '//fname
 1100 FORMAT (I10,F10.5,F10.5,F10.5)
      i = 1
      read (ilun,1100,err=132) k,Tbound(3),val,Tbound(1)
      do i=2,60
         read (ilun,1100,err=132) k,Tbound(2*i+1),val,Tbound(2*i-2)
      enddo
      read (ilun,1100,err=132) k,Tbound(122),val,Tbound(120)
      goto 133
 131  write(6,*)'Open-Error ',fname
 132  write(6,*)'Error reading T-boundaries; line=',i
      stop
 133  close(ilun)

      fname = dirname(1:islate(1))//'/tagE-boundaries.dat'
      open (ilun,file=fname,status='old',err=141)
      write(6,*)'Reading energy E boundaries from file '//fname
 1110 FORMAT (I5,I6,I6,I6,I6,F9.1,F8.6,F8.6,F8.6,F8.6)
      i = 1
c      Ebound(0)=Tbound(1)+0.01
      read(ilun,1110,err=142) j1,j2,j3,j4,j5,val,
     &         Eaver(1),Edev(1),Ebound(2),Ebound(1)
      write(6,1110)j1,j2,j3,j4,j5,val,Eaver(1),Edev(1),Ebound(2),Ebound(1)
      do i=2,767
         read(ilun,1110,err=142) j1,j2,j3,j4,j5,val,
     &         Eaver(i),Edev(i),Ebound(i+1),val1
      write(6,1110)j1,j2,j3,j4,j5,val,Eaver(i),Edev(i),Ebound(i+1),val1
      enddo
c      Ebound(769) = Tbound(121)-0.01
      goto 143
 141  write(6,*)'Open Error ',fname
 142  write(6,*)'Error reading E-boundaries; line=',i
      stop
 143  close(ilun)

      fname = dirname(1:islate(1))//'/Maps/TAG_CALIB.map'
      write(6,*) 'sda_tagcal: Reading map from ',fname
      call map_get_float(fname,'tag_t','slope_left',
     &                          61,slopeTL,irun,k)
      call map_get_float(fname,'tag_t','slope_right',
     &                          61,slopeTR,irun,k)
      call map_get_float(fname,'tag_t','dt_left',
     &                          61,TpeakL,irun,k)
      call map_get_float(fname,'tag_t','dt_right',
     &                          61,TpeakR,irun,k)
      call map_get_float(fname,'tag_t','ci',121,TagTCi,irun,k)
      do i=1,61
         slopeTR(i) = slopeTR(i)/1000.
         slopeTL(i) = slopeTL(i)/1000.
      enddo
      call map_get_float(fname,'tag_e','slope',1,slopeE,irun,k)
      call map_get_float(fname,'tag_e','dt',384,Epeak,irun,k)
      slopeE = -slopeE/1000.
c      call map_get_float(fname,'tag2tof','value',1,tag2tof,irun,k)
c      write(6,*)'for T-ctrs:  t1bin,  t2bin,    Tpeakl,   Tpeakr'
c      do i=1,61
c         write(6,*) t1bin(i),t2bin(i),Tpeakl(i),Tpeakr(i)
c      enddo
      END

c--------------------------------------------------------------------
