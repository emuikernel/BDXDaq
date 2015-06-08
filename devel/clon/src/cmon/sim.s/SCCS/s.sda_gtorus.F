h22885
s 00000/00000/00000
d R 1.2 01/11/19 19:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sda_gtorus.F
e
s 00291/00000/00000
d D 1.1 01/11/19 19:05:13 boiarino 1 0
c date and time created 01/11/19 19:05:13 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_gtorus(sxy,x0,y0)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculates Geometry of Coil for Big Torus.
C-                         The shape of coil is approximated by a set of
C-                         grid points {Xi,Yi,Zi} connected by staight lines.
C-
C-   Inputs  : Oxford Data "oxford(6,12)"
C-   Outputs : Grid points "shape(3,106,2)"
C-   Controls: To be called from "usda_anal" at Level=0
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created: MAR 13, 1997  by Bogdan Niczyporuk
C-
C-   Called by sda_torus
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdadraw.inc"
C
C Local variables
C
      INTEGER ibgrd, ipurp, iblue
      REAL sxy,x0,y0, xma,yma, x1,x2, x_grid(200),y_grid(200)
      EQUIVALENCE (sda_wc(2), xma)
      EQUIVALENCE (sda_wc(4), yma)
C
      INTEGER i,j, io,No,Nox, ig,Ng, ifirst
      REAL ymin,zmin, ymax,zmax, phmin,phmax, phi,dphi, Ri,Ro,dR 
      REAL cn, twopi, sag       
C
      PARAMETER (No = 12   )   
      PARAMETER (Ng = 106  )
      PARAMETER (dR = 22.92)
C
      REAL shape(3,Ng,2), oxford(6,No), ndiv(No)
C
C Russion MiniTorus
      INTEGER Nor,Ngr
      REAL dRr
      PARAMETER (Nor = 8   )   
      PARAMETER (Ngr = 36 )
      PARAMETER (dRr = 9.82)
      REAL shaper(3,Ngr,2), rmint(6,Nor), ndivr(Nor)
C
C
      DATA ndivr /5, 5, 1, 11, 10, 1, 1, 1/
      DATA ifirst /1/
      DATA ndiv /21,  9,  1,  8,  9, 11, 20,  1, 14,  1, 10,  0/
C
C Shape of outer loop of BigTorus in coil plane [cm] (Oxford Data).
C     Flag     Y0      Z0       R0      Ystart   Zstart
C        1      2       3        4        5        6
C     -------------------------------------------------
      DATA oxford /
     1   1.,  -4.00,   0.30,  129.20,   102.54,  -72.79,
     2   1.,  66.80, 163.94,   49.10,    47.30,  118.88,        
     3   0.,  66.80, 163.94,   49.10,    17.70,  163.94,     
     4   1.,  66.80, 258.21,   49.10,    17.70,  258.21,
     5   1.,  77.06, 240.46,   69.60,    42.23,  300.72,
     6   1., -27.52,  35.16,  300.00,   108.65,  302.48,
     7   1.,  -2.50,  48.81,  271.50,   235.84,  178.83,
     8   0.,  -2.50,  48.81,  271.50,   214.30, -114.62,
     9   1., 153.12, -86.47,   65.80,   205.66, -126.08,
     &   0., 153.12, -86.47,   65.80,   113.33, -138.88,      
     1   1., 142.04, -99.89,   47.90,   108.30, -133.90,        
C     -------------------------------------------------
     1   1.,  -4.00,   0.30,  129.20,   102.54,  -72.79/
C
C Shape of outer loop of MiniTorus in coil plane [cm] (Russion Data).
C     Flag     Y0      Z0       R0      Ystart   Zstart
C        1      2       3        4        5        6
C     -------------------------------------------------
      DATA rmint /
     1   1.,   1.95,  0.081,   15.05,    16.99,    0.71,
     2   1.,  16.92,  24.56,   13.62,     9.82,   12.93,             
     3   0.,  16.92,  24.56,   13.62,     3.30,   24.55,     
     4   1.,  16.92,  52.30,   13.62,     3.30,   52.30,
     5   1., -29.53,   0.00,   83.56,    25.96,   62.48,
     6   0., -29.53,   0.00,   83.56,    54.05,    0.00,
     7   0.,   0.00,   0.00,    0.00,    54.05, -200.00,
     8   0.,   0.00,   0.00,    0.00,    16.99, -200.00/
C
C
      IF(ifirst.EQ.1) THEN
        ibgrd  = 1
        iblue  = 4
        ipurp  = 6
        twopi = 2.*ACOS(-1.)
        ifirst= 0
      ENDIF
C
C Loop over the Oxford nodes ( 8 circles + 3 straights)
C The 12th node was added to close the loop
C
      ig = 0
      Nox = No -1
      DO io = 1,Nox
        IF(oxford(1,io).EQ.1.) THEN
C Circle sections
          ymin = oxford(5,io) - oxford(2,io)
          zmin = oxford(6,io) - oxford(3,io)
          phmin = ATAN2(ymin,zmin)
          ymax = oxford(5,io+1) - oxford(2,io)
          zmax = oxford(6,io+1) - oxford(3,io)
          phmax = ATAN2(ymax,zmax)
          IF(io.EQ.9) phmax = phmax + twopi
          dphi = ABS(phmin-phmax)/ndiv(io)
          sag  = oxford(4,io)*(1. - COS(dphi/2.))
        ELSE
C Straight sections 
          ymin = oxford(5,io) - oxford(2,io)
          zmin = oxford(6,io) - oxford(3,io)
          phmin = ATAN2(ymin,zmin)
          phmax = 0.
          dphi  = 0.
          sag   = 0.
        ENDIF
        WRITE(6,100) io, phmin,phmax,dphi,sag
        DO i = 1,ndiv(io)
          ig = ig + 1
          cn = i - 1
          IF(io.EQ.1) cn = 1 - i
          phi= phmin + cn*dphi 
          Ri = oxford(4,io)      
          Ro = Ri - dR
          IF(io.EQ.1) Ro = Ri + dR 
          shape(1,ig,1) = 0.
          shape(2,ig,1) = oxford(2,io) + Ri*SIN(phi) 
          shape(3,ig,1) = oxford(3,io) + Ri*COS(phi)
          shape(1,ig,2) = 0.
          shape(2,ig,2) = oxford(2,io) + Ro*SIN(phi) 
          shape(3,ig,2) = oxford(3,io) + Ro*COS(phi) 
          WRITE(6,101) ig,(shape(j,ig,1),j=1,3),(shape(j,ig,2),j=1,3)
        ENDDO
      ENDDO
C
C Closing a loop (last point = first point) 
      shape(1,Ng,1) = shape(1,1,1)
      shape(2,Ng,1) = shape(2,1,1) 
      shape(3,Ng,1) = shape(3,1,1)
      shape(1,Ng,2) = shape(1,1,2)
      shape(2,Ng,2) = shape(2,1,2) 
      shape(3,Ng,2) = shape(3,1,2)
      WRITE(6,100) io 
      WRITE(6,101) Ng,(shape(j,Ng,1),j=1,3),(shape(j,Ng,2),j=1,3)
C
C Loop over the Russion nodes ( 4 circles + 4 straights)
C The 9th node was added to close the loop
C
      ig = 0
      Nox = Nor
      DO io = 1,Nox
       IF(io.LE.5) THEN
        IF(rmint(1,io).EQ.1.) THEN
C Circle sections
          ymin = rmint(5,io) - rmint(2,io)
          zmin = rmint(6,io) - rmint(3,io)
          phmin = ATAN2(ymin,zmin)
          ymax = rmint(5,io+1) - rmint(2,io)
          zmax = rmint(6,io+1) - rmint(3,io)
          phmax = ATAN2(ymax,zmax)
          dphi = ABS(phmin-phmax)/ndivr(io)
          sag  = rmint(4,io)*(1. - COS(dphi/2.))
        ELSE
C Straight sections 
          ymin = rmint(5,io) - rmint(2,io)
          zmin = rmint(6,io) - rmint(3,io)
          phmin = ATAN2(ymin,zmin)
          phmax = 0.
          dphi  = 0.
          sag   = 0.
        ENDIF
        WRITE(6,100) io, phmin,phmax,dphi,sag
        DO i = 1,ndivr(io)
          ig = ig + 1
          cn = i - 1
          IF(io.EQ.1) cn = 1 - i
          phi= phmin + cn*dphi 
          Ri = rmint(4,io)      
          Ro = Ri - dRr
          IF(io.EQ.1) Ro = Ri + dRr 
          shaper(1,ig,1) = 0.
          shaper(2,ig,1) = rmint(2,io) + Ri*SIN(phi) 
          shaper(3,ig,1) = rmint(3,io) + Ri*COS(phi)
          shaper(1,ig,2) = 0.
          shaper(2,ig,2) = rmint(2,io) + Ro*SIN(phi) 
          shaper(3,ig,2) = rmint(3,io) + Ro*COS(phi)
          WRITE(6,101) ig,(shaper(j,ig,1),j=1,3),(shaper(j,ig,2),j=1,3)
        ENDDO
       ELSE
        WRITE(6,100) io
        ig = ig + 1
        IF(io.EQ.6) THEN
          shaper(1,ig,1) = 0.
          shaper(2,ig,1) = rmint(5,io) 
          shaper(3,ig,1) = rmint(6,io)
          shaper(1,ig,2) = 0.
          shaper(2,ig,2) = rmint(5,io) - dRr 
          shaper(3,ig,2) = rmint(6,io) 
        ELSE IF(io.EQ.7) THEN
          shaper(1,ig,1) = 0.
          shaper(2,ig,1) = rmint(5,io) 
          shaper(3,ig,1) = rmint(6,io)
          shaper(1,ig,2) = 0.
          shaper(2,ig,2) = rmint(5,io) - dRr 
          shaper(3,ig,2) = rmint(6,io) + dRr
        ELSE
          shaper(1,ig,1) = 0.
          shaper(2,ig,1) = rmint(5,io) 
          shaper(3,ig,1) = rmint(6,io)
          shaper(1,ig,2) = 0.
          shaper(2,ig,2) = rmint(5,io) + dRr 
          shaper(3,ig,2) = rmint(6,io) + dRr
        ENDIF
        WRITE(6,101) ig,(shaper(j,ig,1),j=1,3),(shaper(j,ig,2),j=1,3)
       ENDIF 
      ENDDO
C
C Closing a loop (last point = first point) 
      shaper(1,Ngr,1) = shaper(1,1,1)
      shaper(2,Ngr,1) = shaper(2,1,1) 
      shaper(3,Ngr,1) = shaper(3,1,1)
      shaper(1,Ngr,2) = shaper(1,1,2)
      shaper(2,Ngr,2) = shaper(2,1,2) 
      shaper(3,Ngr,2) = shaper(3,1,2)
      WRITE(6,100) io 
      WRITE(6,101) Ngr,(shaper(j,Ngr,1),j=1,3),(shaper(j,Ngr,2),j=1,3)
C*      WRITE(44,44) ((shaper(j,ig,1),j=1,3),ig=1,Ngr)
C* 44   FORMAT(6F10.2)
C
C
C Set up a view box for the convenience of hard_copy output
      CALL ISPMCI(ibgrd)
      CALL ISPLCI(ibgrd)
      IF(ABS(sxy).NE.1.) THEN
        CALL IGBOX(0.,xma,0.05,0.74*yma)
        CALL IGTEXT(xma*.05,yma*.72,
     1    'TORUS and MINT   Outer and inner boundaries of the coils'
     2    ,yma/100.,0.,'L')
C
C X-/Y- axes for the mid-plane
        CALL IGAXIS(x0,16.,y0,y0,0.,1.,0,'AU')
        CALL IGTEXT(16.2,y0,'Zc  (beam)',yma/50.,0.,'L')
        CALL IGAXIS(x0,x0,y0,12.,0.,1.,0,'AU')
        CALL IGTEXT(x0,12.2,'Yc',yma/50.,0.,'L')
      ENDIF
C
C Draw a scale factor equivalent to 100cm on the bottom left corner.
      x1 = 1.
      x2 = x1 + ABS(sxy)*100.
      IF(ABS(sxy).NE.1.) THEN
        CALL IGSET('TMSI',yma/140)
        CALL IGAXIS(x1,x2,.2,.2,0.,100.,502,'US')
        CALL IGTEXT(x2,.4,'100 cm',yma/140.,0.,'L')
      ENDIF
C 
C Plot grid points of Oxford Coil shape
      CALL ISPLCI(ipurp)
      DO 12 i = 1,2
        DO 11 ig = 1,Ng
          x_grid(ig) = shape(3,ig,i)*ABS(sxy) + x0
          y_grid(ig) = shape(2,ig,i)*sxy      + y0
 11     CONTINUE
        CALL IPL(Ng,x_grid,y_grid)
 12   CONTINUE
C 
C Plot grid points of Russion Coil shape
      CALL ISPLCI(ipurp)
      DO 22 i = 1,2
        DO 21 ig = 1,Ngr
          x_grid(ig) = shaper(3,ig,i)*ABS(sxy) + x0
          y_grid(ig) = shaper(2,ig,i)*sxy      + y0
 21     CONTINUE
        CALL IPL(Ngr,x_grid,y_grid)
 22   CONTINUE
C
C
 100  FORMAT(' io, phmin,phmax,dphi,sag=',I10,4F10.4)
 101  FORMAT(I10,3F10.2,2X,3F10.2)
C
      RETURN
      END
E 1
