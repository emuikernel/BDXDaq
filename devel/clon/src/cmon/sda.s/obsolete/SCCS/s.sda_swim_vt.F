h21644
s 00000/00000/00000
d R 1.2 01/11/19 19:02:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_swim_vt.F
e
s 00299/00000/00000
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
      SUBROUTINE sda_swim_vt(iw, isec, svin, step, level, IF_draw,
     &                       svout, vertex)
C-----------------------------------------------------------------------
C-
C-   Purpose and Methods : Back swimming in the PT magnetic field.
C-                         Calculates track points on the target plane#1.
C-
C-   Inputs  : isec    - current sector number
C-             svin(i) - Track Parameters at L4 {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
C-             step    - step size (cm)
C-             level   - reconstruction level: 2 for HBT, 4 for TBT
C-
C-   Outputs : svout(i)- Track Parameters at VT {x,y,z,Px/P,Py/P,Pz/P,P,m,Q}
C-             vertex(10)
C-
C-   Controls:
C-
C-   Library belongs    : libsda.a
C-
C-   Calls   : sda_helix, sda_ipln
C-
C-   Created   NOV-10-1998  Bogdan Niczyporuk
C-
C-   Called by ana_prfit and ana_fit
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C sda_pln(),sda_trp(),ndc_min,npln
#include "sdageom.inc"
C
C
C Subroutine parameters
      INTEGER isec, level, iw(1000), IF_draw
      REAL svin(9), step, svout(9), vertex(10)
C External
      REAL VMOD
C
C Local variables
C
      INTEGER i, j, ist, ip,iflag, nstep
      REAL ECon, Bold, Dangle, sint, stot, s4
      REAL STmin, STmax, Dang, Dplane
      REAL vect(9), vout(9), xint(9), bfld(3)
C
      real dbeam,dbeamold,sda_point2line,sda_line2line
      real bdir(3),bpoint(3),rr0(3),rr1(3)
      real tmp, tmp1, tmp2
C
      integer nstep_xyztmp1,nstep_xyztmp2
      real xyztmp1(3,mxstep),xyztmp2(3,mxstep)
C
      data bdir/1.,0.,0./ ! in Sector Coordinate System beam along X
      DATA ECon/2.99792E-4/
C
C Swimming parameters
      DATA STmin/ 0.10/
      DATA STmax/ 5.00/
      DATA Dang / 0.001/
C
*      print *,'sda_swim_vt reached'
C
C get vertex position in 'isec' Sector Coordinate System
      call sda_vertex(iw,isec,bpoint)
C
C copy P,m,Q to the output array; they will not be changed
      DO i = 7,9
        vect(i) = svin(i)
        svout(i) = svin(i)
      ENDDO
C Reverse the Charge and the Directions of the Track
      vect(9) = -svin(9)
      DO i = 1,3
        vect(i) = svin(i)
        vect(i+3) = -svin(i+3)
      ENDDO
C
C Initialise indices
C
      Dangle = Dang/ECon
      nstep = 10000
      stot  = 0.
C
C Save initial Plane#1 parameters
      Dplane= sda_pln(8,1,isec) 
C
C Initialize hit array
C
      DO j=1,3
        sda_trp(j,1)   = 1000.
        sda_trp(j+3,1) = 0.
      ENDDO
      sda_trp(7,1) = 0.
      sda_pln(8,1,isec) = 0.001
C
C Get 'bfld' in order to derive the first step size
      step = 0.01
      CALL dchelix(step, vect, vout, bfld)
**      CALL dcrkuta(step, vect, vout, bfld)
      dbeamold = sda_point2line(vout,bpoint,bdir)
C
C save drawing info if any
      IF(IF_draw.EQ.1.AND.level.eq.4) THEN
        nstep_xyztmp1 = nstep_xyz
        DO j=1,nstep_xyztmp1
          xyztmp1(1,j) = xyz(1,j)
          xyztmp1(2,j) = xyz(2,j)
          xyztmp1(3,j) = xyz(3,j)
        ENDDO
      ENDIF
C
C Do loop traces particle through detector
C
      DO 100 ist = 1,nstep
C
C Calculate a step size
        IF(svin(9).NE.0.) THEN ! charged particle
          Bold = VMOD(bfld,3)
          IF(Bold.GT.0.05) THEN
            step = Dangle*vect(7)/Bold
            IF(step.LT.STmin) step = STmin
            IF(step.GT.STmax) step = STmax
          ELSE
            step = STmax
          ENDIF
        ELSE                   ! neutral particle
          step = 10. 
        ENDIF
*      if(step .GT. 0.05) step = 0.05
        call hfill(291,step,0.,1.)
C
C do step
        CALL dchelix(step, vect, vout, bfld)
**        CALL dcrkuta(step, vect, vout, bfld)
C
C calculate the distance to the beam line
        dbeam = sda_point2line(vout,bpoint,bdir)
      print *,'dbeam=',dbeam,'  dbeamold=',dbeamold
****        print *,'vout=',vout(1),vout(2),vout(3),' dbeam=',dbeam
****        print *,'  (cos= ',vout(4),vout(5),vout(6),' )'
C
C check if track reached beam line closest approach
        IF(dbeam .GE. dbeamold) THEN


          tmp = sda_line2line(vout,vout(4),bpoint,bdir,rr0,rr1)
*          print *,'level=',level,' dbeam=',dbeam,' tmp=',tmp
          dbeam = tmp

          DO i=1,7
            xint(i) = vout(i)
          ENDDO

          xint(1) = rr0(1)
          xint(2) = rr0(2)
          xint(3) = rr0(3)
****          print *,'xint=',xint(1),xint(2),xint(3),' dbeam=',dbeam

          if(level.eq.4) then
          if(isec.EQ.1) then
            call hfill(301,bfld(1),0.,1.)
            call hfill(302,bfld(2),0.,1.)
            call hfill(303,bfld(3),0.,1.)
            call hfill(304,dbeam-tmp,0.,1.)
            call hfill(305,dbeam,0.,1.)
            call hfill(306,tmp,0.,1.)
            call hfill(292,step,0.,1.)
***            print *,'xint=',xint(1),xint(2),xint(3)
            call hfill(270,xint(2),xint(3),1.)

            call hfill(271,svin(1),0.,1.)
            call hfill(272,svin(2),0.,1.)
            call hfill(273,svin(3),0.,1.)
            call hfill(274,svin(4),0.,1.)
            call hfill(275,svin(5),0.,1.)
            call hfill(276,svin(6),0.,1.)
            call hfill(277,svin(7),0.,1.)
            call hfill(7172,svin(1),svin(2),1.)
            call hfill(7273,svin(2),svin(3),1.)
            call hfill(7173,svin(1),svin(3),1.)

            tmp1 = xint(2)-bpoint(2)
            tmp2 = xint(3)-bpoint(3)
            if(tmp2.GT.-0.5 .AND. tmp2.LT.0.5 .AND.
     &         tmp1.GT.-0.1 .AND. tmp1.LT.0.1) then
            call hfill(1271,svin(1),0.,1.)
            call hfill(1272,svin(2),0.,1.)
            call hfill(1273,svin(3),0.,1.) ! ROGATII 
            call hfill(1274,svin(4),0.,1.)
            call hfill(1275,svin(5),0.,1.)
            call hfill(1276,svin(6),0.,1.) ! ROGATII 
            call hfill(1277,svin(7),0.,1.)
            call hfill(17172,svin(1),svin(2),1.)
            call hfill(17273,svin(2),svin(3),1.)
            call hfill(17173,svin(1),svin(3),1.)
          endif
          endif

            vertex(1) = xint(1)
            vertex(2) = xint(2)
            vertex(3) = xint(3)
            vertex(4) = xint(4)
            vertex(5) = xint(5)
            vertex(6) = xint(6)
            vertex(7) = xint(7)
            vertex(8) = bpoint(1)
            vertex(9) = bpoint(2)
            vertex(10) = bpoint(3)

          endif
cccc      print *,'xint=',xint
          ip = 1
          DO i = 1,3
            sda_trp(i,ip)   = xint(i)
            sda_trp(i+3,ip) =-xint(i+3)
            svout(i)        = xint(i)
            svout(i+3)      =-xint(i+3)
          ENDDO
          stot = stot + sint
**          sda_trp(7,ip) = sda_pln(8,ip,isec) ! track length

C
C Store points for trajectory drawing
          IF(IF_draw.EQ.1.AND.level.eq.4 .AND. ist.LE.mxstep) THEN
            xyztmp2(1,ist) = xint(1)
            xyztmp2(2,ist) = xint(2)
            xyztmp2(3,ist) = xint(3)
            nstep_xyztmp2  = ist
          ENDIF

          GOTO 999
        ELSE
          dbeamold = dbeam
C
C Store points for trajectory drawing
          IF(IF_draw.EQ.1.AND.level.eq.4 .AND. ist.LE.mxstep) THEN
            xyztmp2(1,ist) = vect(1)
            xyztmp2(2,ist) = vect(2)
            xyztmp2(3,ist) = vect(3)
            nstep_xyztmp2  = ist
          ENDIF

        ENDIF

        stot = stot + step
        DO j=1,9
          vect(j) = vout(j)
        ENDDO
 100  CONTINUE                      ! End loop over Nstep
C
 999  CONTINUE
C
C final drawing array
      IF(IF_draw.EQ.1.AND.level.eq.4) THEN
      print *,'== ',nstep_xyztmp1,nstep_xyztmp2
c copy vertex2layer1 trajectory
        DO j=1,nstep_xyztmp2
          xyz(1,j) = xyztmp2(1,nstep_xyztmp2+1-j)
          xyz(2,j) = xyztmp2(2,nstep_xyztmp2+1-j)
          xyz(3,j) = xyztmp2(3,nstep_xyztmp2+1-j)
        ENDDO
*      do j=1,nstep_xyztmp2
*        print *,'grid=',j,'  xyz=',xyz(1,j),xyz(2,j),xyz(3,j)
*      enddo
c copy the rest of trajectory
        DO j=1,nstep_xyztmp1
          xyz(1,j+nstep_xyztmp2) = xyztmp1(1,j)
          xyz(2,j+nstep_xyztmp2) = xyztmp1(2,j)
          xyz(3,j+nstep_xyztmp2) = xyztmp1(3,j)
          if(j+nstep_xyztmp2.GE.mxstep) goto 1001
        ENDDO
        j = j - 1
 1001   continue
        nstep_xyz = nstep_xyztmp2 + j
      ENDIF
C
C print xyz array
      do j=1,nstep_xyz
        print *,'grid=',j,'  xyz=',xyz(1,j),xyz(2,j),xyz(3,j)
      enddo
C
C Correct path lengths for all DC planes
      s4 = sda_trp(7,4)
      DO i = ndc_min,npln
        IF(sda_trp(1,i).LT.999.) sda_trp(7,i) = sda_trp(7,i) - s4 + stot 
      ENDDO
C
C Restore initial parameters for Plane#1
      sda_pln(8,1,isec) = Dplane
C
      RETURN
      END



E 1
