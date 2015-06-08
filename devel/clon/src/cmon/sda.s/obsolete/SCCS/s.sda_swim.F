h40170
s 00000/00000/00000
d R 1.2 01/11/19 19:02:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_swim.F
e
s 00291/00000/00000
d D 1.1 01/11/19 19:02:40 boiarino 1 0
c date and time created 01/11/19 19:02:40 by boiarino
e
u
U
f e 0
t
T
I 1
C=======================================================================
      SUBROUTINE sda_swim(isec,ipl_min,ipl_max,IF_mat,IF_draw,vins)
C-----------------------------------------------------------------------
C-
C-   Purpose and Methods : Track swimming (traj. propagation) in magnetic
C-                         field. Calculates track points on detector planes.
C-
C-   Inputs  : isec    - current sector number                      
C-             ipl_min - first plane to crossed during swimming
C-             ipl_max - last plane to crossed during swimming
C-             IF_mat  - when set to 1, Transport Matrix is calculated
C-                       during swimming
C-             IF_draw - when set to 1, x,y -coordinates at each step
C-                       are stored during swimming (convenient for drawing
C-                       routines
C-
C-             vins(9) - Starting values {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
C-
C-   Outputs : Results stored in COMMON/sdageom/
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls   : dchelix, dcicyl, dcipln, dctrmat & sda_muls
C-
C-   Created   OCT-26-1990  Bogdan Niczyporuk
C-   ModIFied  AUG-2-1992   Billy Leung
C-   ModIFied  NOV-13-1998  Franz Klein (simulated track can go to other sector)
C-
C-   Called by sim_main & ana_trfit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C zstep, step, isim, zmagn
#include "sdakeys.inc"
#include "sdageom.inc"
C
C
C Subroutine parameters
      INTEGER isec, ipl_min, ipl_max, IF_mat, IF_draw
      REAL vins(9)
C External
      REAL VMOD
C
C Local variables
C
      INTEGER i, j, ist, ip, ipl, ipp, IFlag, IFlagp, newsec
      INTEGER new,nstep,ncyl_min,icross
      REAL ECon, Binv, Bold, Dangle, sint, stot, s_first, del_s, x0(2)
      REAL vect(9), vout(9), xint(9), bfld(3), phi,phimin
      REAL svin(9), sout(9), asec, tanpos, tanphi, tan30, pi
C
      DATA ECon/2.99792E-4/
      DATA del_s/60./
      DATA tan30, pi /0.57735027, 3.14159265/
C
C
C SDA_SWIM begins
C
      x0(1) = sda_pln(12,2,isec) - vins(1)
      x0(2) = sda_pln(13,2,isec) - vins(2)
      phimin = ATAN2( x0(2),x0(1) )
C
      IF(ipl_min.LE.0) GO TO 999
C
      Dangle = zstep(3)/ECon
C
C Buffer "vect" is used in DO loop
C
      DO i=1,9
        vect(i) = vins(i)
      ENDDO
C
C Initialise hit array
C
      DO i = 1,ipl_max
        DO j=1,3
          sda_trp(j,i)=1000.
          sda_trp(j+3,i) = 0.
        ENDDO
        sda_trp(7,i) = 0.
      ENDDO
C
C Initialise indices
C
      nstep  = 1000./zstep(1)
      new    = 1
      ipl    = ipl_min
      stot   = 0.
      icross = 0
      newsec = 0
C
C Get B-field in order to derive the first step size when PTF is present (SIM)
      step = 0.01
      CALL dchelix(step, vect, vout, bfld)
**      CALL dcrkuta(step, vect, vout, bfld)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C Do loop traces particle through detector
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      DO 100 ist = 1,nstep
C
C Calculate a step size
        IF(vins(9).NE.0.) THEN
          Bold = VMOD(bfld,3)
          IF(zstep(1).LT.zstep(2)) THEN
            IF(Bold.LT.0.005) THEN
              Binv = 200.
            ELSE
              Binv = 1./Bold
            ENDIF
            step = Dangle*vect(7)*Binv
            IF(step.LT.zstep(1)) step = zstep(1)
            IF(step.GT.zstep(2)) step = zstep(2)
          ELSE
            step = zstep(1)
          ENDIF
        ELSE
          step = 10.
        ENDIF
C First step size have to be smaller THEN a distance to the 1-st plane
        IF(ist.EQ.1)  step = 0.02
C
C do a step in a magnetic field
        CALL dchelix(step, vect, vout, bfld)
**        CALL dcrkuta(step, vect, vout, bfld)
        ip = ipl
*        print *,'sda_swim: ip=',ip
C
C Check whether sector boundaries were crossed if SIM & PT Field present
        IF(isim.EQ.0)      then
*          print *,'sda_swim: sector boundaries were crossed'
          GO TO 9
        endif
        IF(zmagn(4).EQ.0.) then
*      print *,'222: zmagn(4)=',zmagn(4)
          GO TO 9
        endif
        IF(newsec.EQ.1)    then
*          print *,'sda_swim: new sector'
          GO TO 9
        endif
      print *,'sda_swim: stop 1'
      stop
        tanpos = vout(3)/ABS(vout(2))
        tanphi = vout(6)/ABS(vout(5))
        IF(tanpos.GT.tan30 .AND. tanphi.GT.tan30) THEN
          asec = pi/3.
          isec = isec + 1
          IF(isec.GT.6) isec = 1
        ELSE IF(tanpos.LT.-tan30 .AND. tanphi.LT.-tan30) THEN
          asec = -pi/3.
          isec = isec - 1
          IF(isec.LT.1) isec = 6
        ELSE
          asec = 0.
        ENDIF
        IF(asec.NE.0.) THEN
          svin(1) = vect(1)
          svin(2) = vect(2)*cos(asec) + vect(3)*sin(asec)
          svin(3) =-vect(2)*sin(asec) + vect(3)*cos(asec)
          svin(4) = vect(4)
          svin(5) = vect(5)*cos(asec) + vect(6)*sin(asec)
          svin(6) =-vect(5)*sin(asec) + vect(6)*cos(asec)
          sout(1) = vout(1)
          sout(2) = vout(2)*cos(asec) + vout(3)*sin(asec)
          sout(3) =-vout(2)*sin(asec) + vout(3)*cos(asec)
          sout(4) = vout(4)
          sout(5) = vout(5)*cos(asec) + vout(6)*sin(asec)
          sout(6) =-vout(5)*sin(asec) + vout(6)*cos(asec)
          DO i = 1,6
            vect(i) = svin(i)
            vout(i) = sout(i)
          ENDDO
          newsec = 1
        ENDIF
  9     CONTINUE
C
C Store points for trajectory drawing
        IF(IF_draw.EQ.1 .AND. ist.LE.mxstep) THEN
**      print *,'vect=',vect(1),vect(2),vect(3)
**      print *,'  (cos= ',vect(4),vect(5),vect(6),' )'
          xyz(1,ist) = vect(1)
          xyz(2,ist) = vect(2)
          xyz(3,ist) = vect(3)
          nstep_xyz  = ist
        ENDIF
C
C It is assumed here that a track never cross again the same layer,
C but may cross a few layers within the same (constant) step length.
C
C Find intersection with cylinder or plane
        ncyl_min = ndc_min + 12
C
 10     CONTINUE
        sint = step
        IF(ipl.LT.ndc_min) THEN                              ! Planes near targ.
          IF(ipl.EQ.2) THEN
            phi = ATAN2(vout(5),vout(4))
            IF(phi.LT.phimin) ipl = 3
          ENDIF
          CALL dcipln(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
        ELSE IF(ipl.GE.ndc_min .AND. ipl.LT.ncyl_min) THEN   ! Reg.1
          IF(vect(1).LT.0. .AND. vout(1).LT.0.) THEN
            CALL dcipln(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
          ELSE IF((vect(1).LT.0. .AND. vout(1).GT.0.) .OR.
     &            (vect(1).GT.0. .AND. vout(1).LT.0.)) THEN
            CALL dcipln(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
            if(IFlag.EQ.1) goto 15
            CALL dcicyl(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
 15         continue
         ELSE
            CALL dcicyl(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
          ENDIF
        ELSE IF(ipl.GE.ncyl_min .AND. ipl.LE.ncc_max) THEN   ! Reg.2,3 & CC
          CALL dcicyl(sda_pln(1,1,isec),vect,vout,ipl, sint,xint,IFlag)
        ELSE IF (ipl_max.GE.nsc_min) THEN                    ! SC & EC
          IFlag = 0
          DO ipp = nsc_min,ipl_max
            CALL dcipln(sda_pln(1,1,isec),vect,vout,ipp, sint,xint,IFlagp)
            IF(IFlagp.EQ.1) THEN
              DO i = 1,6
                sda_trp(i,ipp) = xint(i)
              ENDDO
              sda_trp(7,ipp) = stot + sint
              icross = icross + 1
              IF(icross.EQ.1) s_first = sda_trp(7,ipp)
            ENDIF
          ENDDO
          IF(icross.EQ.0) THEN
            GO TO 20
          ELSE
            IF((stot-s_first).GT.del_s) GO TO 999
          ENDIF 
        ELSE
        ENDIF
C
        IF(IFlag.EQ.1) THEN
C Track crossed a cylinder or plane
          IF(IF_mat.GT.0 .AND. ipl.LE.ndc_max) THEN
            CALL dctrmat(new,vect,xint,sint,bfld,sda_tmat(1,1,ipl),IFlag)
          ENDIF
          DO i = 1,6
            sda_trp(i,ipl) = xint(i)
          ENDDO
          sda_trp(7,ipl) = stot + sint
          IF(ipl.EQ.2) THEN
            ipl = ipl + 2
          ELSE
            ipl = ipl + 1
          ENDIF
*          print *,'sda_swim: increased ipl=',ipl
C Check IF last layer has been reached
          IF(ipl.GT.ipl_max) then
*            print *,'sda_swim: last layer =',ipl,' > ipl_max=',ipl_max
            GO TO 999
          endif
C Check next layer crossing within the same step
          GO TO 10
        ENDIF
C
 20     CONTINUE
        IF(IF_mat.GT.0 .AND. ipl.LE.ndc_max) THEN
          CALL dctrmat(new,vect,vout,step,bfld,sda_tmat(1,1,1),0)
        ENDIF
C
        stot = stot + step
*      print *,'sda_swim: stot=',stot,'   step=',step,'   ipl=',ipl
*boy        IF(vout(5).LT.0.0 .OR. stot.GT.700.) GO TO 999
        IF(stot.GT.700.) then
ccc      print *,'sda_swim: stot=',stot,' > 700.'
          GO TO 999
        endif
        DO j=1,9
          vect(j) = vout(j)
        ENDDO
 100  CONTINUE
CCCCCCCCCCCCCCCCCCCCCCCCC
C End loop over Nstep
CCCCCCCCCCCCCCCCCCCCCCCCC
 999  CONTINUE
C
      RETURN
      END



E 1
