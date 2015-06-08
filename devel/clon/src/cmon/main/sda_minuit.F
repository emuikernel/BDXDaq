      program sda_minuit
      external fcn1
*      open(unit=5, file='minuit.dat', status='OLD')
*      open(unit=6, file='minuit.out', status='NEW', form='FORMATTED')
cc      call mintio(5,6,7)
      call mn1minuit(fcn1,0)
      stop
      end
C======================================================================
      subroutine fcn1(npar,grad,fval,xval,iflag,futil)
c
      implicit double precision (a-h,o-z)
c
#include "wcs.inc"
#include "sdabos.inc"
C just to pass zgcor() to sda_ginit
#include "sdakeys.inc"
C
      INTEGER NWPAW
      PARAMETER (NWPAW = 2000000)
      REAL PAW
      COMMON/PAWC/PAW(NWPAW)
c
      dimension grad(*), xval(*)
      external futil
      REAL r2r3cor(6,2)
c
c Initialization
c
      if(iflag .eq. 1) then
        call bos(jw,jndimb)
        call bos(iw,ndimb)
C Read data cards
***        call sda_ffgo
C
C sda_ffgo.F - before cards reading
C
        zgrid(1) = 0.
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
C instead of 'sda.config' reading do following
C
        CALL UCTOH('BAT ',lmode,4,4) ! lmode = 'BAT '
        CALL UCTOH('REVB',levb(1),4,4) ! levb(1) = 'REVB'
        ltrig(1) = 14317 ! file number to be used by calibration
        ltrig(2) = 100000 !? the number of events to be processed
        CALL UCTOH('TABL',zmagn(1),4,4) ! zmagn(1) = 'TABL'
        zmagn(2) = 1500.
        zmagn(3) = 0.
        zmagn(4) = 121.8
        CALL UCTOH('INIT',lgeom(1),4,4) ! lgeom(1) = 'INIT'
        CALL UCTOH('READ',lcali(1),4,4) ! lcali(1) = 'READ'
        ztarg(1) = 0.0
        ztarg(2) = 0.0
        ztarg(3) = -59.0
        zbeam(1) = -1.0
        zbeam(2) = 1.606
        zbeam(7) = 0.2
        zstep(1) = 1.0
        zstep(2) = 10.0
        zstep(3) = 0.005
        lanal(1) = 4 ! Level=4 is TBT
        zcut(1) = 0.0
        zcut(2) = 6.0
        zcut(3) = 6.0
        zcut(4) = 10.0
        zcut(5) = 1.0
        zcut(6) = 15.0
        zcut(7) = 20.0
        zcut(8) = 1.0
        zcut(9) = -11.48
        zcut(10) = 20.00
        ztfit(1) = 5.
        ztfit(2) = 0.
        ztfit(3) = 2.
        ztfit(4) = 0.1
        ztfit(5) = 4.
        CALL UCTOH('MATX',lmatx(1),4,4) ! lmatx(1) = 'MATX'
        zderr(1) = 1.
        zderr(2) = 0.055
        zderr(3) = 0.058
        zderr(4) = 0.062
        zderr(5) = 0.070
        zderr(6) = 0.079
        zderr(7) = 0.100
        zderr(8) = 0.98
        zderr(9) = 0
C
C sda_ffgo.F - after cards reading
C
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
***
C Read/Write magnetic field table (see cards in "ftn33")
        call dcminit
C Initialize/Write/Read geometry
        zgcor(1) = xval(13)
        zgcor(10) = xval(14)
        do i = 1,6
          r2r3cor(i,1) = xval(i)
          r2r3cor(i,2) = xval(i+6)
        enddo

c        call sda_ginit1

        call dc_make_dgeo(ltrig(1), 2, r2r3cor)
        call dc_geom_banks
        call sda_ginit2

C Initialization of HBOOK
        call hlimit(NWPAW)
C Initialization of HPLOT
        if(zgcor(1).GT.0.1) then
          call hplint(1)
          call hplzon(3,2,1,' ')
          call hplopt('FIT',1)
        endif
C Get Calibration Constants for current "RunNo"
        print *,'calibration run=',ltrig(1)
        call sda_calib(ltrig(1))
C
ccccccccccccccccccccc
c initial parameters
ccccccccccccccccccccc
        call mn1seti('CLAS Alignment')
c                           value step min max
ccc        call mn1pars('1 ''r1x'' ',ierr)
ccc        call mn1pars('2 ''r1y'' ',ierr)
ccc        call mn1pars('3 ''r1z'' ',ierr)
c sector #1 (old field)
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x''   -0.695112 0.1', ierr)
c        call mn1pars('5 ''r2y'' 2.59102E-02 0.1', ierr)
c        call mn1pars('6 ''r2z''    0.654016 0.1', ierr)
c        call mn1pars('7 ''r3x''   -0.986066 0.1', ierr)
c        call mn1pars('8 ''r3y''    0.646809 0.1', ierr)
c        call mn1pars('9 ''r3z''    -1.27698 0.1', ierr)
c sector 2
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x'' 0.15469  0.1', ierr)
c        call mn1pars('5 ''r2y'' -0.46066E-01 0.1', ierr)
c        call mn1pars('6 ''r2z''  1.1037 0.1', ierr)
c        call mn1pars('7 ''r3x''  0.20088 0.1', ierr)
c        call mn1pars('8 ''r3y'' 0.10494 0.1', ierr)
c        call mn1pars('9 ''r3z'' -0.24000E-01 0.1', ierr)
c sector 3
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x'' 0.41418  0.5', ierr)
c        call mn1pars('5 ''r2y'' -0.14421 0.5', ierr)
c        call mn1pars('6 ''r2z''  1.0894 0.5', ierr)
c        call mn1pars('7 ''r3x''   0.28070 0.5', ierr)
c        call mn1pars('8 ''r3y'' 0.68847E-01 0.5', ierr)
c        call mn1pars('9 ''r3z''   0.62371E-01 0.5', ierr)
c sector 4
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x'' 0.44836 0.5', ierr)
c        call mn1pars('5 ''r2y'' 0.13832E-01 0.5', ierr)
c        call mn1pars('6 ''r2z'' -0.19587 0.5', ierr)
c        call mn1pars('7 ''r3x'' 0.23690 0.5', ierr)
c        call mn1pars('8 ''r3y'' -0.20927E-01 0.5', ierr)
c        call mn1pars('9 ''r3z''  0.68245E-01 0.5', ierr)
c sector 5
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x'' -1.0139 0.5', ierr)
c        call mn1pars('5 ''r2y'' -0.35501 0.5', ierr)
c        call mn1pars('6 ''r2z'' 0.54159 0.5', ierr)
c        call mn1pars('7 ''r3x'' -0.95601 0.5', ierr)
c        call mn1pars('8 ''r3y'' 0.21451 0.5', ierr)
c        call mn1pars('9 ''r3z'' 0.32344E-01 0.5', ierr)
c sector 6
c        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
c        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
c        call mn1pars('4 ''r2x''  0.23126 0.5', ierr)
c        call mn1pars('5 ''r2y'' -0.19731 0.5', ierr)
c        call mn1pars('6 ''r2z''   2.8787 0.5', ierr)
c        call mn1pars('7 ''r3x''  1.1781  0.5', ierr)
c        call mn1pars('8 ''r3y''  0.71564 0.5', ierr)
c        call mn1pars('9 ''r3z'' -0.51583E-01 0.5', ierr)
c
c sector 0
*        call mn1pars('2 ''angle2z'' 0.0 0.01', ierr)
*        call mn1pars('3 ''angle3z'' 0.0 0.01', ierr)
*        call mn1pars('4 ''r2x'' 0.0 0.5', ierr)
*        call mn1pars('5 ''r2y'' 0.0 0.5', ierr)
*        call mn1pars('6 ''r2z'' 0.0 0.5', ierr)
*        call mn1pars('7 ''r3x'' 0.0 0.5', ierr)
*        call mn1pars('8 ''r3y'' 0.0 0.5', ierr)
*        call mn1pars('9 ''r3z'' 0.0 0.5', ierr)
c

c        call mn1pars('2 ''angle2z''  -0.19057E-02 0.001', ierr)
c        call mn1pars('3 ''angle3z'' 0.32667E-02 0.001', ierr)
c        call mn1pars('4 ''r2x'' -1.064  0.1', ierr)
c        call mn1pars('5 ''r2y''  0.707E-01 0.1', ierr)
c        call mn1pars('6 ''r2z''  0.0 0.5', ierr)
c        call mn1pars('7 ''r3x'' 0.8411 0.1', ierr)
c        call mn1pars('8 ''r3y'' -0.25732 0.1', ierr)
c        call mn1pars('9 ''r3z''  0.0 0.5', ierr)
c
c        call mn1pars('10 ''sector'' 5.',ierr)
c        if(istat) print *,'ierr=',ierr
ccccccccccccccccccccc
c
ccccccccccccccccccccc
      endif
c
c Calculate GRAD, the first derivatives of fval (OPTIONAL)
c
      if(iflag .eq. 2) then
      endif
c
c Calculate the value of the function 'fval'
c

c recalculate geometry ccccccccccc
      do i = 1,6
        r2r3cor(i,1) = xval(i)
        r2r3cor(i,2) = xval(i+6)
        print *,'cor(',i,') = ',r2r3cor(i,1),r2r3cor(i,2)
      enddo

c      call sda_ginit1

      call dc_make_dgeo(ltrig(1), 2, r2r3cor)
      call dc_geom_banks
      call sda_ginit2

cccccccccccccccccccccccccccccccccc

      fval = sda_fval(iflag)
c
c Perform final calculations
c
      if(iflag .eq. 3) then
        call dclast(iw)
      endif
c
      return
      end
C======================================================================
      real function sda_fval(iflag)
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : return 'fval' for MINUIT
C-
C-    Inputs  :
C-    Outputs :
C-    Controls: Terminates if "Nevt" processed  or if the EOF reached.
C- 
C-    Library belongs: libsda.a
C-
C-    Calls: sda_anal, dcerun, dch2hbook, sim_main
C-
C-    Created   24-JAN-1997   Bogdan Niczyporuk
C-
C_    Called by sda_main
C- 
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdabos.inc"
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdaevgen.inc"
C
      integer iflag
      integer iquest
      COMMON/QUEST/ iquest(100)
c
      integer ana(1000000)
c
      real fun1,fun2
      external fun1,fun2
      real par(24),parstep(11),pmin(11),pmax(11),sigpar(11)
      common/fun1par/par
C
C External function
      real hstati
C
C Local variables
      INTEGER i, ifirst, ntot
      INTEGER iret, ifevb, inter, ievent, Nevents
      REAL npeak1,npeak2
      REAL pi, twopi, degrad, raddeg, chi2, chi2trak1, chi2trak2
      REAL deltaW,deltaV,deltaX,tmp,mean,rms,tmp1,peak1,peak2
C
      DATA ifevb /1/ ! SDA reads input REVB
      DATA ifirst /1/
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        degrad = pi/180.
        raddeg = 180./pi
        npeak1 = 100.
        npeak2 = 100.
      ENDIF
C
C check if interactive mode
      inter = 0
      ifsim = 0
      ievent = Nevt
      Nevents = Nevt
      CALL timex(time(9)) ! Get initial time
C
      CALL ETLDROP(IW,'E')
      CALL ETNGARB(IW)
C
      sda_fval = 0

      if(zgcor(1).GT.0.1.AND.iflag.EQ.1) then
        OPEN(UNIT=10,FILE='minuit.ps',FORM='FORMATTED',STATUS='UNKNOWN')
        call IGMETA(10,-111)
      endif

C
C loop over the events in file 0
C
      goto 888

      CALL UCTOH('NONE',zmagn(1),4,4) ! zmagn(1) = 'NONE'
      call hreset(11,' ')

      CALL FPARM('OPEN EVBINPUT UNIT=1 FILE="../DATA/DST26760.A00" ')
      DO i=1,3000
        CALL FRBOS(IW,1,'E',iret)
        IF(iret.EQ.-1 .OR. iret.GT.0) THEN ! End of File or Read Error
          WRITE(6,*) ' End-of-File flag encounted, iret =',iret
          CALL dcerun(iw)
          call dch2hbook()
          GOTO 77
        ENDIF
        CALL  cmlib(iw,ana,ifevb,inter,ievent,1) ! need to be tested !!!
*        CALL sda_anal(iw,rw,ifevb,inter,ievent,1)
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
      ENDDO
 77   CALL FPARM('CLOSE UNIT=1')
C
C copy histograms
C

 888  continue

C
C loop over the events in file 1
C

      CALL UCTOH('TABL',zmagn(1),4,4) ! zmagn(1) = 'TABL'
      call hreset(11,' ')
      call hreset(22,' ')
      call hreset(283,' ')

      CALL FPARM('OPEN EVBINPUT UNIT=1 FILE="../DATA/DST25663.A00" ')
      DO i=1,30000
        CALL FRBOS(IW,1,'E',iret)
        IF(iret.EQ.-1 .OR. iret.GT.0) THEN ! End of File or Read Error
          WRITE(6,*) ' End-of-File flag encounted, iret =',iret
          CALL dcerun(iw)
          call dch2hbook()
          GOTO 7
        ENDIF
        CALL cmlib(iw,ana,ifevb,inter,ievent,1) ! need to be tested !!!
*        CALL sda_anal(iw,rw,ifevb,inter,ievent,1)
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
      ENDDO
 7    CALL FPARM('CLOSE UNIT=1')
C
C copy histograms
C
      call hcopy(11,10011,' ')
      call hcopy(22,10022,' ')
      call hcopy(283,10283,' ')

      if(zgcor(1).GT.0.1) call hplot(10011,' ',' ',0)
      mean = hstati(10011,1,' ',0)
      rms = hstati(10011,2,' ',0)
      tmp = hstati(10011,3,' ',0)
      print *,'iflag=',iflag
      if(iflag.EQ.1) then
        chi2trak1 = mean
        print *,'chi2: mean,rms,tmp=', mean,rms,tmp
        print *,' -> chi2trak1=', chi2trak1
      endif
c require reasonable tracking chi2
      sda_fval = sda_fval + mean/chi2trak1
      print *,'fval(0)=',sda_fval

      par(1) = npeak1
        parstep(1) = 10.
        pmin(1) = npeak1/2.
        pmax(1) = npeak1*2.
      par(2) = 0.938
        parstep(2) = 0.005 ! parstep(2) = 0.005
        pmin(2) = 0.84
        pmax(2) = 1.04
      par(3) = 0.015
        parstep(3) = 0.001
        pmin(3) = 0.01
        pmax(3) = 0.03
      par(4) = 0.          ! offset against elastic peak
        parstep(4) = 0.0 ! parstep(4) = 5.
        pmin(4) = -npeak1/2.
        pmax(4) = npeak1/2.
      par(5) = 0.020       ! offset against elastic peak
        parstep(5) = 0.001
        pmin(5) = 0.015
        pmax(5) = 0.040
      par(6) = 0.045
        parstep(6) = 0.001
        pmin(6) = 0.03
        pmax(6) = 0.07
      par(7) = npeak1
        parstep(7) = 10.
        pmin(7) = npeak1/2.
        pmax(7) = npeak1*2.
      par(8) = 1.23
        parstep(8) = 0.1 ! parstep(8) = 0.1
        pmin(8) = 1.1
        pmax(8) = 1.36
      par(9) = 0.15
        parstep(9) = 0.01
        pmin(9) = 0.1
        pmax(9) = 0.25
      par(10) = -npeak1/2.
        parstep(10) = 5.
      par(11) = npeak1/2.
        parstep(11) = 5.
      iquest(11) = 15
      iquest(12) = 99
      call hfith(10022,fun1,'BR',11,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10022,' ',' ',0)

      peak1 = par(2)
      if(iflag.EQ.1) then
        npeak1 = par(1)
        print *,' -> npeak1=',npeak1
      endif

      mean = hstati(10022,1,' ',0)
      rms = hstati(10022,2,' ',0)
      tmp = hstati(10022,3,' ',0)

c require reasonable chi2
**      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
      sda_fval = sda_fval + chi2/1.3
c elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(3)**2)/0.015
c quazi-elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(6)**2)/0.045
c pool elastic peak to its nominal position
cc      sda_fval = sda_fval + sqrt((0.938-par(2))**2)/0.015
c keep elastic peak in boundaries
cc      if(par(2).GT.0.97) then
cc        sda_fval = sda_fval + (par(2)-0.97)*100.
cc      else if(par(2).LT.0.91) then
cc        sda_fval = sda_fval + (0.91-par(2))*100.
cc      endif
      print *,'fval(1)=',sda_fval,' mean,rms,tmp=',mean,rms,tmp

      par(1) = 2000.
      par(2) = 0.0
      par(3) = 1.2
      call hfithn(10283,'G',' ',3,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10283,' ',' ',0)
c require reasonable chi2
**      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
        sda_fval = sda_fval + chi2/1.5
c vertex sigma
      tmp1 =  sqrt(par(3)**2)/1.2
      sda_fval = sda_fval + tmp1
      print *,'fval(2)=',sda_fval
c vertex position
c      tmp1 = sqrt(par(2)**2)/1.5
c      sda_fval = sda_fval + tmp1
c      print *,'fval(3)=',sda_fval

C
C loop over the events in file 2
C

      call hreset(11,' ')
      call hreset(22,' ')
      call hreset(283,' ')

      call dcmreverse
c      DO i = 1,Ng
c        Q(i) = -Q(i)
c      ENDDO

      CALL FPARM('OPEN EVBINPUT UNIT=1 FILE="../DATA/DST26295.A00" ')
      DO i=1,30000
        CALL FRBOS(IW,1,'E',iret)
        IF(iret.EQ.-1 .OR. iret.GT.0) THEN ! End of File or Read Error
          WRITE(6,*) ' End-of-File flag encounted, iret =',iret
          CALL dcerun(iw)
          call dch2hbook()
          GOTO 14
        ENDIF
        CALL cmlib(iw,ana,ifevb,inter,ievent,1) ! need to be tested !!!
*        CALL sda_anal(iw,rw,ifevb,inter,ievent,1)
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
      ENDDO
 14   CALL FPARM('CLOSE UNIT=1')

      call dcmreverse
c      DO i = 1,Ng
c        Q(i) = -Q(i)
c      ENDDO

C
C copy histograms
C
      call hcopy(11,10011,' ')
      call hcopy(22,10022,' ')
      call hcopy(283,10283,' ')

      if(zgcor(1).GT.0.1) call hplot(10011,' ',' ',0)
      mean = hstati(10011,1,' ',0)
      rms = hstati(10011,2,' ',0)
      tmp = hstati(10011,3,' ',0)
      print *,'iflag=',iflag
      if(iflag.EQ.1) then
        chi2trak2 = mean
        print *,'chi2: mean,rms,tmp=', mean,rms,tmp
        print *,' -> chi2trak2=', chi2trak2
      endif
c require reasonable tracking chi2
      sda_fval = sda_fval + mean/chi2trak2
      print *,'fval(0)=',sda_fval

      par(1) = npeak2
        parstep(1) = 10.
        pmin(1) = npeak2/2.
        pmax(1) = npeak2*2.
      par(2) = 0.938
        parstep(2) = 0.005 ! parstep(2) = 0.005
        pmin(2) = 0.84
        pmax(2) = 1.04
      par(3) = 0.015
        parstep(3) = 0.001
        pmin(3) = 0.01
        pmax(3) = 0.03
      par(4) = 0.          ! offset against elastic peak
        parstep(4) = 0.0 ! parstep(4) = 5.
        pmin(4) = -npeak2/2.
        pmax(4) = npeak2/2.
      par(5) = 0.020       ! offset against elastic peak
        parstep(5) = 0.001
        pmin(5) = 0.015
        pmax(5) = 0.040
      par(6) = 0.045
        parstep(6) = 0.001
        pmin(6) = 0.03
        pmax(6) = 0.07
      par(7) = npeak2/2.5
        parstep(7) = 10.
        pmin(7) = npeak2/5.
        pmax(7) = npeak2*5.
      par(8) = 1.23
        parstep(8) = 0.1 ! parstep(8) = 0.1
        pmin(8) = 1.1
        pmax(8) = 1.36
      par(9) = 0.15
        parstep(9) = 0.01
        pmin(9) = 0.1
        pmax(9) = 0.25
      par(10) = -npeak2/2.5
        parstep(10) = 5.
      par(11) = npeak2/2.5
        parstep(11) = 5.
      iquest(11) = 15
      iquest(12) = 99
      call hfith(10022,fun1,'BR',11,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10022,' ',' ',0)

      peak2 = par(2)
      if(iflag.EQ.1) then
        npeak2 = par(1)
        print *,' -> npeak2=',npeak2
      endif

      mean = hstati(10022,1,' ',0)
      rms = hstati(10022,2,' ',0)
      tmp = hstati(10022,3,' ',0)

c require reasonable chi2
**      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
      sda_fval = sda_fval + chi2/1.3
c elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(3)**2)/0.015
c quazi-elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(6)**2)/0.045
c pool elastic peak to its nominal position
cc      sda_fval = sda_fval + sqrt((0.938-par(2))**2)/0.015
c keep elastic peak in boundaries
cc      if(par(2).GT.0.97) then
cc        sda_fval = sda_fval + (par(2)-0.97)*100.
cc      else if(par(2).LT.0.91) then
cc        sda_fval = sda_fval + (0.91-par(2))*100.
cc      endif
      print *,'fval(1)=',sda_fval,' mean,rms,tmp=',mean,rms,tmp

      par(1) = 2000.
      par(2) = 0.0
      par(3) = 1.4
      call hfithn(10283,'G',' ',3,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10283,' ',' ',0)
c require reasonable chi2
**      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
        sda_fval = sda_fval + chi2/1.5
c vertex sigma
      tmp1 =  sqrt(par(3)**2)/1.4
      sda_fval = sda_fval + tmp1
      print *,'fval(2)=',sda_fval
c vertex position
c      tmp1 = sqrt(par(2)**2)/1.8
c      sda_fval = sda_fval + tmp1
c      print *,'fval(3)=',sda_fval


      goto 900

C
C loop over the events in file 3 (2.284 inbending)
C

ccc 888  continue

      CALL UCTOH('TABL',zmagn(1),4,4) ! zmagn(1) = 'TABL'
      zbeam(2) = 2.284
      call hreset(11,' ')
      call hreset(22220,' ')
      call hreset(283,' ')

      CALL FPARM('OPEN EVBINPUT UNIT=1 FILE="../DATA/DST27246.A00" ')
      DO i=1,100000
        CALL FRBOS(IW,1,'E',iret)
        IF(iret.EQ.-1 .OR. iret.GT.0) THEN ! End of File or Read Error
          WRITE(6,*) ' End-of-File flag encounted, iret =',iret
          CALL dcerun(iw)
          call dch2hbook()
          GOTO 71
        ENDIF
        CALL cmlib(iw,ana,ifevb,inter,ievent,1) ! need to be tested !!!
*        CALL sda_anal(iw,rw,ifevb,inter,ievent,1)
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
      ENDDO
 71   CALL FPARM('CLOSE UNIT=1')
C
C copy histograms
C
      call hcopy(11,10011,' ')
      call hcopy(22220,10022,' ')
      call hcopy(283,10283,' ')

      if(zgcor(1).GT.0.1) call hplot(10011,' ',' ',0)
      mean = hstati(10011,1,' ',0)
      rms = hstati(10011,2,' ',0)
      tmp = hstati(10011,3,' ',0)
      print *,'iflag=',iflag
      if(iflag.EQ.1) then
        chi2trak1 = mean
        print *,'chi2: mean,rms,tmp=', mean,rms,tmp
        print *,' -> chi2trak1=', chi2trak1
      endif
c require reasonable tracking chi2
      sda_fval = sda_fval + mean/chi2trak1
      print *,'fval(0)=',sda_fval

      par(1) = npeak1
        parstep(1) = 10.
        pmin(1) = npeak1/2.
        pmax(1) = npeak1*2.
      par(2) = 0.938
        parstep(2) = 0.0
        pmin(2) = 0.84
        pmax(2) = 1.04
      par(3) = 0.015
        parstep(3) = 0.001
        pmin(3) = 0.01
        pmax(3) = 0.03
      par(4) = 0.          ! offset against elastic peak
        parstep(4) = 5.
        pmin(4) = -npeak1/8.
        pmax(4) = npeak1/8.
      par(5) = 0.020       ! offset against elastic peak
        parstep(5) = 0.001
        pmin(5) = 0.015
        pmax(5) = 0.040
      par(6) = 0.045
        parstep(6) = 0.001
        pmin(6) = 0.03
        pmax(6) = 0.07
      par(7) = npeak1
        parstep(7) = 10.
        pmin(7) = npeak1/2.
        pmax(7) = npeak1*10.
      par(8) = 1.23
        parstep(8) = 0.0
        pmin(8) = 1.1
        pmax(8) = 1.3
      par(9) = 0.1
        parstep(9) = 0.01
        pmin(9) = 0.05
        pmax(9) = 0.15

      par(10) = npeak1
        parstep(10) = 10.
        pmin(10) = npeak1/2.
        pmax(10) = npeak1*2.
      par(11) = 1.5
        parstep(11) = 0.1
        pmin(11) = 1.4
        pmax(11) = 1.6
      par(12) = 1.0
        parstep(12) = 0.1
        pmin(12) = 0.2
        pmax(12) = 2.0

      par(13) = -75.
        parstep(13) = 1.
        pmin(13) = -125.
        pmax(13) = -25.
      par(14) = 100.
        parstep(14) = 1.
        pmin(14) = 50.
        pmax(14) = 150.

      iquest(11) = 1
      iquest(12) = 100
      call hfith(10022,fun2,'BR',14,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10022,' ',' ',0)

      peak1 = par(2)
      if(iflag.EQ.1) then
        npeak1 = par(1)
        print *,' -> npeak1=',npeak1
      endif

      mean = hstati(10022,1,' ',0)
      rms = hstati(10022,2,' ',0)
      tmp = hstati(10022,3,' ',0)

c require reasonable chi2
      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
c elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(3)**2)/0.015
c quazi-elastic gauss sigma
      sda_fval = sda_fval + sqrt(par(6)**2)/0.045
c pool elastic peak to its nominal position
**      sda_fval = sda_fval + sqrt((0.938-par(2))**2)/0.015
c keep elastic peak in boundaries
cc      if(par(2).GT.0.97) then
cc        sda_fval = sda_fval + (par(2)-0.97)*100.
cc      else if(par(2).LT.0.91) then
cc        sda_fval = sda_fval + (0.91-par(2))*100.
cc      endif
      print *,'fval(1)=',sda_fval,' mean,rms,tmp=',mean,rms,tmp

      par(1) = 2000.
        parstep(1) = 10.
        pmin(1) = 1000.
        pmax(1) = 3000.
      par(2) = 0.0
        parstep(2) = 0.1
        pmin(2) = -3.0
        pmax(2) = 3.0
      par(3) = 1.2
        parstep(3) = 0.1
        pmin(3) = 0.1
        pmax(3) = 5.0
      par(4) = 200.
        parstep(4) = 10.
        pmin(4) = 100.
        pmax(4) = 300.
      par(5) = 0.0
        parstep(5) = 0.1
        pmin(5) = -3.0
        pmax(5) = 3.0
      par(6) = 2.5
        parstep(6) = 0.1
        pmin(6) = 0.1
        pmax(6) = 10.0
      call hfithn(10283,'G+G','B',6,par,parstep,pmin,pmax,sigpar,chi2)
      if(zgcor(1).GT.0.1) call hplot(10283,' ',' ',0)
c require reasonable chi2
**      if(chi2.GT.2.) sda_fval = sda_fval + (chi2-2.)
**        sda_fval = sda_fval + chi2/1.5
c vertex sigma
      tmp1 =  sqrt(par(3)**2)/1.2
      sda_fval = sda_fval + tmp1
      print *,'fval(2)=',sda_fval
c vertex position
c      tmp1 = sqrt(par(2)**2)/1.5
c      sda_fval = sda_fval + tmp1
c      print *,'fval(3)=',sda_fval










C
C end of loop over the events
C

 900  continue


C
C comparision between data sets
C
c elastic peaks have to be in the same position
      sda_fval = sda_fval + sqrt((peak1-peak2)**2)/0.015
      print *,'fval(final)=',sda_fval
C
C update window
C
      if(zgcor(1).GT.0.1) call IGTERM

      if(zgcor(1).GT.0.1.AND.iflag.EQ.1) then 
        call IGMETA(0,0)
ccc        call ICLWK(1)
        CLOSE(10)
      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C use elastic peak
c      call hunpak(22,content,' ',0)
c      fmax = 0.
c      imax=0
c      do i=1,100
c        if(fmax .LT. content(i)) then
c          fmax = content(i)
c          imax=i
c        endif
c      enddo
*      par(1) = 80.
*      par(2) = 0.94
*      par(3) = 0.03
*      iquest(11) = 35
*      iquest(12) = 60
*      call hfithn(22,'G','R',3,par,parstep,pmin,pmax,sigpar,chi2)
*      call hplot(22,' ',' ',0)
**      call hprint(22)
cc      deltaW = ((0.938-par(2))**2)/(0.06**2)
*      mean = hstati(22,1,' ',0)
*      rms = hstati(22,2,' ',0)
*      tmp = hstati(22,3,' ',0)
*      deltaW = ((0.938-mean)**2)/(0.06**2)
*      sda_fval = sda_fval + (rms/0.06)**2 + deltaW + deltaV + deltaX 
cc      sda_fval = sda_fval + (par(3)/0.06)**2 + deltaW + deltaV 
c      sda_fval = sda_fval + (par(3)/0.03)**2 + deltaW + chi2/100.
c
c keep efficiency
c
*      ntot=363
*      if(tmp.LT.ntot) sda_fval = sda_fval + (ntot-tmp)
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      call hreset(22,' ')
      call hreset(283,' ')
      call hdelet(10022)
      call hdelet(10283)
C
      RETURN
      END

