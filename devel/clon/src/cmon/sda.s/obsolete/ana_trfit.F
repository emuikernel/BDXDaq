C======================================================================
      SUBROUTINE ana_trfit(jw,isec,trk,Level,svin,chifit)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Perform a fit of the one track candidate
C-
C-
C-   Inputs  : isec       - Sector number being analyzed 
C-             trk(12,36) - DC track info for the current track
C-             Level      - Level of analysis
C-             svin(9)    - Starting (final) values of the track parameters
C-
C-   Outputs : svin(9)    - fitted (corrected) track parameters
C-                         {x,y,z,Px/P,Py/P,Pz/P,p,m,Q} at the target position.
C-             chifit     - Chi Square of the fit
C-
C-   Controls:             See DATA card "TFIT"
C-
C-
C-   Library belongs    : libana.a
C-
C-   Calls              : dcswim, dcmxinv, dcmvprod
C-
C-   Created   30-OCT-1990   Bogdan Niczyporuk
C-   Modified  22-JUNE-1992  Billy Leung (IMPLICIT NONE implemented)
C-
C-   Called by ana_prfit, ana_fit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
C
C Constants
      REAL big, raddeg, pi, piby2
C        
C Subroutine parameters
      INTEGER isec, Level, jw(1000)
      REAL trk(12,36), svin(9), chifit
C
C External functions
      INTEGER iucomp
      REAL    VDOT
C
C Local variables
      INTEGER i, j, k, il, iw, is, ilay, inter, iter, iterm, iterl
      INTEGER ifirst, if_mat, if_draw, matx, maxTM
      INTEGER ipmin,ipmax, ilmin,ilmax
C
      REAL ex(3),ez(3), u0(3),u(3), a0(3),vt0(3)
      REAL dtrsp(5,npl_dc), vx(ndc_max)
      REAL alfa(5,5),beta(5),dx(5), ems(npl_dc), weight(ndc_max)        
      REAL diff(3), temp(3)
      REAL chisq, chi2old, cosL0, D0, Z0
      REAL dcatr, det, free
      REAL lambda, lambda0, sphi
      REAL tht, fii, fii0, thetd, phid, sigm
      REAL pabs, chi_dif
C
      DATA ifirst/1/
      DATA chi_dif/0.10/
      DATA ex  /1.,0.,0./
      DATA ez  /0.,0.,1./
C
C Trajectory migration due to multiple scattering for p = 1 GeV
      DATA ems /6*0.014, 6*0.016, 6*0.121, 6*0.139, 6*0.298, 6*0.340/        
C
C
      IF(ifirst.EQ.1) THEN
        inter  = 0
        IF(iucomp(4HINT ,lmode,1).NE.0) inter = 1
        matx  = 1
        IF(iucomp(4HDRV ,lmatx,10).NE.0) matx = 0
        big    = 1.E+10
        pi     = ACOS(-1.)
        piby2  = pi/2.
        raddeg = 180./pi
        ifirst = 0
      ENDIF
C
C Initialize chi2old to a big value.
      chifit  = 1000.
      chi2old = big
      IF(ztfit(4).GT.0.) chi_dif = ztfit(4)
C
C Form the uvz unit-vectors co-ordinates in the direction of the
C initial position of the track
      CALL CROSS(ez,svin(4),u0)
      CALL VUNIT(u0,u0,3)

      ipmin  = ztfit(5)
      ipmax  = ztfit(6)
      IF(ipmin.GE.4) THEN
        ilmin  = ipmin - 3
        ilmax  = ipmax - 3
        iw = ABS(trk(1,ilmin))
        vt0(1) = dc_wpmid(1,iw,ilmin,isec)
        vt0(2) = dc_wpmid(2,iw,ilmin,isec)
        vt0(3) = dc_wpmid(3,iw,ilmin,isec)
      ELSE
        ilmin  = 1
        ilmax  = ipmax - 3
CCC
CCC
        vt0(1) = 0.
        vt0(2) = 0.
        vt0(3) = 0.
      call sda_vertex(jw,isec,vt0)
CCC
CCC
      ENDIF
      diff(1) = svin(1) - vt0(1)
      diff(2) = svin(2) - vt0(2)
      diff(3) = svin(3) - vt0(3)

      D0 = VDOT(diff,u0,3)    
      Z0 = svin(3)
      sphi  = ATAN2(svin(5),svin(4))
C
C Track Fitting iterations
C
      iterm = ztfit(1)
      DO 200 iter = 1,iterm
C
c      print *,'iter,svin=',iter,svin
c      print *,'D0,Z0,sphi=',D0,Z0,sphi
C
        maxTM = ztfit(3)
        IF(Level.EQ.4 .AND. iter.GT.maxTM) THEN
          if_mat = 0
        ELSE
          if_mat = 1
        ENDIF
C
C Trajectory propagation
**        CALL sda_swim(isec, ipmin, ipmax, if_mat, inter, svin)
        CALL dcswim(sda_pln(1,1,isec),ipmin,ipmax,if_mat,inter,svin,
     &              sda_trp,sda_tmat,nstep_xyz,xyz)
c      print *,'trfit: iter # ',iter,' svin=',svin
C
C Calculation of derivatives (dtrsp) and dcat
        lambda0= ASIN(svin(6))
        cosL0  = COS(lambda0)
        fii0   = ATAN2(svin(5),svin(4))
        ilay   = 0
        chisq  = 0.
C
C Vertex constraint due to transverse beam size (electron beam)
C
        IF(ztfit(2).EQ.1. .OR. ztfit(2).EQ.3.) THEN
          ilay = ilay + 1
          CALL CROSS(ex,sda_trp(4,1), u)
          CALL VUNIT(u,u,3)
C MACROS
          dtrsp(1,ilay) = sda_tmat(4,1,1)
          dtrsp(2,ilay) = sda_tmat(5,2,1)*u(3)
          dtrsp(3,ilay) = sda_tmat(4,3,1)
          dtrsp(4,ilay) = sda_tmat(4,4,1)
          dtrsp(5,ilay) = sda_tmat(5,5,1)*u(3)
C No magnetic field but transport matrix was computed (matx=1)
          IF(mgtyp.EQ.5) THEN
            diff(1) = sda_trp(1,1) - svin(1)
            diff(2) = sda_trp(2,1) - svin(2)
            diff(3) = sda_trp(3,1) - svin(3)
            dtrsp(1,ilay) = -svin(7)*VDOT(diff,u,3)
          ENDIF
C MACROS
C Calculation of vector VX(ilay) = Dmeas - Dtrial and  chi2
          dcatr = VDOT(sda_trp(1,1),u,3)
          vx(ilay) = zbeam(4) - dcatr
C Chi-square accumulated with weight correction
          sigm = zbeam(7)
          IF(sigm.LT.0.005) sigm = 0.18
*      print *,'sigm=',sigm
          weight(ilay) = 1./sigm**2
          chisq = chisq + vx(ilay)*vx(ilay)*weight(ilay)
        ENDIF

cboyC
cboyC Vertex constraint due to target length along the beam (very thin targets)
cboyC
cboy        IF(ztfit(2).EQ.2. .OR. ztfit(2).EQ.3.) THEN
cboy          ilay = ilay + 1
cboy          CALL CROSS(ez,sda_trp(4,1), u)
cboy          CALL VUNIT(u,u,3)
CboyC MACROS
cboy          dtrsp(1,ilay) = sda_tmat(4,1,1)
cboy          dtrsp(2,ilay) = sda_tmat(5,2,1)*u(3)
cboy          dtrsp(3,ilay) = sda_tmat(4,3,1)
cboy          dtrsp(4,ilay) = sda_tmat(4,4,1)
cboy          dtrsp(5,ilay) = sda_tmat(5,5,1)*u(3)
cboyC No magnetic field but transport matrix was computed (matx=1)
cboy          IF(mgtyp.EQ.5) THEN
cboy      stop
cboy            diff(1) = sda_trp(1,1) - svin(1)
cboy            diff(2) = sda_trp(2,1) - svin(2)
cboy            diff(3) = sda_trp(3,1) - svin(3)
cboy            dtrsp(1,ilay) = -svin(7)*VDOT(diff,u,3)
cboy          ENDIF
cboyC MACROS
cboyC Calculation of vector VX(ilay) = Dmeas - Dtrial and  chi2
cboy          dcatr = VDOT(sda_trp(1,1),u,3)
cboy          vx(ilay) = -ztarg(3)*SIN(sphi) - dcatr
cboyC Chi-square accumulated with weight correction
cboy          sigm = ztarg(4)                       ! This is half target length
cboy          IF(sigm.LT.0.005) sigm = 0.50
cboy          sigm = sigm*SIN(sphi)/1.732           ! Flat distr. along target 
cboy          weight(ilay) = 1./sigm**2
cboy          chisq = chisq + vx(ilay)*vx(ilay)*weight(ilay)
cboy        ENDIF

C
C Now loop over all layers of DC
C
        DO 30 il = ilmin,ilmax
          IF(lout(il).GT.0) THEN                        ! Discard this layer
*            print *,'lout(',il,')=',lout(il)
            GOTO 30
          ENDIF
          IF(trk(12,il).GT.0.) THEN                ! Discard this hit
*            print *,'trk(12,',il,')=',trk(12,il)
            GOTO 30
          ENDIF
          is = (il-1)/6 + 1
          iw = trk(1,il)
          ilay = ilay + 1
          CALL CROSS(dc_wdir(1,iw,il,isec),dc_trp(4,il), u)
          CALL VUNIT(u,u,3)
C MACROS
          dtrsp(1,ilay) = dc_tmat(4,1,il)
          dtrsp(2,ilay) = dc_tmat(5,2,il)*u(3)
          dtrsp(3,ilay) = dc_tmat(4,3,il)
          dtrsp(4,ilay) = dc_tmat(4,4,il)
          dtrsp(5,ilay) = dc_tmat(5,5,il)*u(3)
C No magnetic field but transport matrix was computed (matx=1)
          IF(mgtyp.EQ.5) THEN
            diff(1) = dc_trp(1,il) - svin(1)
            diff(2) = dc_trp(2,il) - svin(2)
            diff(3) = dc_trp(3,il) - svin(3)
            dtrsp(1,ilay) = -svin(7)*VDOT(diff,u,3)
          ENDIF
C MACROS
C Calculation of vector VX(ilay) = Dmeas - Dtrial and  chi2
          diff(1) = dc_trp(1,il) - dc_wpmid(1,iw,il,isec)
          diff(2) = dc_trp(2,il) - dc_wpmid(2,iw,il,isec)
          diff(3) = dc_trp(3,il) - dc_wpmid(3,iw,il,isec)
          trk(7,il) = VDOT(diff,u,3)                         ! Dcat
*      print *,'trk(7,',il,')=',trk(7,il)
          vx(ilay)  = trk(4,il) - trk(7,il)             ! Dcam - Dcat
C Chi-square accumulated with weight correction
          sigm = trk(5,il)
          weight(ilay) = (1./sigm)**2
          chisq = chisq + vx(ilay)*vx(ilay)*weight(ilay)
   30   CONTINUE
C
C Normalize chi-squre by the no. of degree of freedom
        IF(ilay.LT.6) GO TO 200
        free   = FLOAT(ilay) - 5.
        IF(chisq.GT.0.001) chifit = chisq/free
        iterl  = iter
C
C Calculation of matrix alfa(5,5)
        DO j=1,5
          DO k=1,5
            alfa(j,k) = 0.
            DO il=1,ilay
              alfa(j,k) = alfa(j,k) + dtrsp(j,il)*dtrsp(k,il)
            ENDDO
          ENDDO
        ENDDO
C
C Calculation of vector BETA(5)
        CALL dcmvprod(dtrsp,vx,beta,5,ilay)
C
C Invertion of matrix ALFA(5,5)
        CALL dcmxinv(alfa,5,det)
        IF (det.EQ.0.) GO TO 200
C
C Calculation of corrections DX(5)
        CALL dcmvprod(alfa,beta,dx,5,5)
c      print *,'dx=',dx
C
C Get new {p,Theta,Phi,d0,z0} and svin from previous svin and
C corrections DX
        IF(mgtyp.EQ.5) dx(1) = 0.
        pabs = 1./( 1./svin(7) + dx(1) )
        lambda = lambda0 + dx(2)
        tht = piby2 - lambda
        fii = fii0 + dx(3)
C New DCA & Z0 at origin
        D0 = D0 + dx(4)
        Z0 = Z0 + dx(5)
C New starting values for the next tracing
        svin(4) = SIN(tht)*COS(fii)
        svin(5) = SIN(tht)*SIN(fii)
        svin(6) = COS(tht)
        IF(ipmin.GE.4) THEN     
          CALL CROSS(ez,svin(4),a0)
          CALL VUNIT(a0,a0,3)
          svin(1) = vt0(1) + D0*a0(1) - 3.*svin(4)
          svin(2) = vt0(2) + D0*a0(2) - 3.*svin(5)
          svin(3) = Z0
        ELSE
          svin(1) = -D0/SIN(fii)
          svin(2) =  svin(2)
          svin(3) =  Z0
        ENDIF
        svin(7) = pabs
*      print *,'trfit: hi2=',chifit,' hi2old=',chi2old,' chi_dif=',chi_dif
C
        IF (ABS(chi2old-chifit).LE.chi_dif) GO TO 201   ! early exit
        chi2old = chifit
  200 CONTINUE                    ! End of loop over iterations
C
C
C
  201 CONTINUE
C
C Print the result of last iteration
      IF((isw(1).EQ.1) .AND. (ievt.LE.Ndbg)) THEN
        iterl = iter - 1
        thetd = tht*raddeg
        phid  = fii*raddeg
        WRITE(6,301) Level,isec,iterl,chifit
        WRITE(6,302) DX, pabs,thetd,phid,D0,Z0,(svin(i),i=1,3)
      ENDIF
C
C Check if make sense to perform the global swimming
      IF(chifit.GT.100.) GO TO 999
C
C Calculation of final trajectory and chi-square
C
      if_draw = inter
      IF(lanal(1).EQ.4 .AND. Level.EQ.2) if_draw = 0
C
**      CALL sda_swim(isec,  ipmin,   npln,     0,if_draw, svin)
      CALL dcswim(sda_pln(1,1,isec),ipmin,npln,0,if_draw,svin,
     &              sda_trp,sda_tmat,nstep_xyz,xyz)
C
      ilay   = 0
      chisq  = 0.
      chifit = 1000.
      DO 240 il = ilmin,ilmax
        IF(lout(il).EQ.1) GO TO 240                       ! Discard this layer
        IF(trk(12,il).GT.0.) GO TO 240               ! Discard this hit
        is = (il-1)/6 + 1
        iw = trk(1,il)
C
        CALL CROSS(dc_wdir(1,iw,il,isec),dc_trp(4,il), u)
        CALL VUNIT(u,u,3)
C Calculation of vector vx(ilay) = Dcam - Dcat  and  chi2
        diff(1) = dc_trp(1,il) - dc_wpmid(1,iw,il,isec)
        diff(2) = dc_trp(2,il) - dc_wpmid(2,iw,il,isec)
        diff(3) = dc_trp(3,il) - dc_wpmid(3,iw,il,isec)
        trk(7,il) = VDOT(diff,u,3)                   ! Dcat
C Chi-square accumulated with weight correction
        IF(lout(il).EQ.2) GO TO 240
        ilay = ilay + 1
        vx(ilay) = trk(4,il) - trk(7,il)        ! Dcam - Dcat
        sigm = trk(5,il)
        weight(ilay) = (1./sigm)**2
        chisq = chisq + vx(ilay)*vx(ilay)*weight(ilay)
  240 CONTINUE
C
      IF(ilay.GT.5) THEN
        free   = FLOAT(ilay) - 5.
        IF(chisq.GT.0.001) chifit = chisq/free
      ENDIF
      IF(isw(1).EQ.1 .AND. ievt.LE.Ndbg) THEN
        WRITE(6,303) iterl,chifit
      ENDIF
C
C
  301 FORMAT(/' LEVEL =',I2,'  isec/iter =',2I3,'  Chi2 =',E12.6)
  302 FORMAT(' Cor: 1/P,L0,PH=',F8.5,2F10.5,'   D0,Z0=',F8.4,8X,F8.
     1      4/
     2      ' New:  P ,TH,PH=',F8.5,2F10.5,'   D0,Z0=',F8.4,8X,F8.4/
     3      13X,'                                 Vertex=',3F8.4)
  303 FORMAT(' Iter=',I2,'   Chi2 =',E12.6)
C
  999 CONTINUE
      RETURN
      END
