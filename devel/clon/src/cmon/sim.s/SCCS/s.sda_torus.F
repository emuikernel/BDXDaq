h22591
s 00000/00000/00000
d R 1.2 01/11/19 19:05:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/sda_torus.F
e
s 00326/00000/00000
d D 1.1 01/11/19 19:05:14 boiarino 1 0
c date and time created 01/11/19 19:05:14 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_torus(X,F,mgtype)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Calculate magnetic field. According Bio-Savarta law
C-                         it is summed over all straight current elements in
C-                         a loop. The coil may consist of many loops.
C-
C-   Inputs  : X(3)  - x,y,z coordinates
C-   Outputs : F(3)  - Bx,By,Bz magnetic field
C-   Controls: mgtype = 1 for Big  Torus
C-             mgtype = 2 for Mini Torus
C-
C-   Library belongs: libsda.a
C-
C-   Calls: sda_ridge
C-
C-   Author:     P.Vernin  (Aug. 1987)
C-   Modified:   Feb. 1991  by Bogdan Niczyporuk
C-               Mar. 1997  by Bogdan Niczyporuk (Oxford & Russion coils) 
C-
C-   Called by sda_minit, sda_helix
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C
C Subroutine parameters
      INTEGER mgtype
      REAL    X(3), F(3)
C
C Local variables
      INTEGER i,j,k,m, io,No,Nox, il,nloops, is,nshape, nlY,nlX, ilY,ilX          
      REAL ymin,zmin, ymax,zmax, phmin,phmax, phi,dphi, R0,Ri,dR,ddR, cn 
      REAL BT(3), dX0, dX, ddX
      REAL XNORM, XNORM2              
      REAL piby2,pi,twopi, sinsec(6), cossec(6)
      LOGICAL   LFIRST
      INTEGER   nsect
C
C Big Torus
      PARAMETER (No = 12   )
      PARAMETER (dX = 4.53 )   
      PARAMETER (dR = 22.92)
      PARAMETER (nlY = 54, nlX = 4)
      PARAMETER (nloops = nlY*nlX, nshape = 105)
C
      INTEGER ndiv(No) 
      REAL shape(3,nshape,nloops), oxford(6,No) 
      REAL SU(3,nshape)
C
C Mini Torus
      INTEGER Nor, nloopr, nshaper
      REAL dRr, ddRr, y0,z0
      PARAMETER (Nor = 8   )   
      PARAMETER (dRr = 9.82)
      PARAMETER (nloopr = 5, nshaper = 35)
      INTEGER ndivr(Nor)
      REAL shaper(3,nshaper,nloopr), rmint(6,Nor)
      REAL SUr(3,nshaper)
C
      DATA LFIRST /.TRUE./
      DATA nsect  /6/
      DATA ndiv /21,  9,  1,  8,  9, 11, 20,  1, 14,  1, 10,  0/
C
      DATA ndivr /5, 5, 1, 11, 10, 1, 1, 1/
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
C At first entry
C
      IF(LFIRST) THEN
        LFIRST = .FALSE.
C
        pi    = ACOS(-1.)
        piby2 = pi/2.
        twopi = 2.*pi
C
C Initialize array for rotation to sectors
        DO i = 1,6
          sinsec(i) = SIN(FLOAT(i-1) * (twopi/6.))
          cossec(i) = COS(FLOAT(i-1) * (twopi/6.))
        ENDDO
C
C Define the normalization constants for the big Torus and mini Torus:
C   Big Torus 
C       Imax_t = 3860 [A] 
C       Nloops = 4*54 = 216 [No of loops in a sector]
C       Xnorm = 216*3860*E-4 = 83.376 [A Turns]
C
        XNORM  = 83.376/FLOAT(nloops)
C
C   Mini Torus
C       Imax_m = 8000 [A]
C       Nloops = 5 [No of loops in a sector]
C       Xnorm2 = 5*8000E-4 = 4.0 [A Turns]
C        
        XNORM2 = 4.0/nloopr
C
C
C Loop over the Oxford nodes ( 8 circles + 3 straights)
C The 12th node was added to close the loop
C
        IF(mgtype.EQ.1) THEN
          ddR = dR/nlY
          ddX = dX/nlX
          dX0 = -dX/2. - ddX/2.         
          is = 0
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
            ELSE
C Straight sections 
              ymin = oxford(5,io) - oxford(2,io)
              zmin = oxford(6,io) - oxford(3,io)
              phmin = ATAN2(ymin,zmin)
              phmax = 0.
              dphi  = 0.
            ENDIF
            DO i = 1,ndiv(io)
              is = is + 1
              cn = i - 1
              IF(io.EQ.1) cn = 1 - i
              phi= phmin + cn*dphi 
              R0 = oxford(4,io) + ddR/2.
              IF(io.EQ.1) R0 = oxford(4,io) - ddR/2.
              il = 0
              DO ilX = 1,nlX
                DO ilY = 1,nlY
                  il = il + 1      
                  Ri = R0 - ilY*ddR
                  IF(io.EQ.1) Ri = R0 + ilY*ddR 
                  shape(1,is,il) = dX0 + ilX*ddX
                  shape(2,is,il) = oxford(2,io) + Ri*SIN(phi) 
                  shape(3,is,il) = oxford(3,io) + Ri*COS(phi)
                ENDDO             ! EndDo over nlY 
              ENDDO               ! EndDo over nlX
            ENDDO                 ! EndDo over ndiv
          ENDDO                   ! EndDo over Nox
        ENDIF                     ! End IF(mgtype.EQ.1)
C
C Loop over the Oxford nodes (4 circles + 4 straights)
C
        IF(mgtype.EQ.2) THEN
          ddRr = dRr/nloopr
          is = 0
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
              ELSE
C Straight sections 
                ymin = rmint(5,io) - rmint(2,io)
                zmin = rmint(6,io) - rmint(3,io)
                phmin = ATAN2(ymin,zmin)
                phmax = 0.
                dphi  = 0.
              ENDIF
              DO i = 1,ndivr(io)
                is = is + 1
                cn = i - 1
                IF(io.EQ.1) cn = 1 - i
                phi= phmin + cn*dphi 
                R0 = rmint(4,io) + ddRr/2.
                IF(io.EQ.1) R0 = rmint(4,io) - ddRr/2.
                DO il = 1,nloopr      
                  Ri = R0 - il*ddRr
                  IF(io.EQ.1) Ri = R0 + il*ddRr 
                  shaper(1,is,il) = 0.
                  shaper(2,is,il) = rmint(2,io) + Ri*SIN(phi) 
                  shaper(3,is,il) = rmint(3,io) + Ri*COS(phi)
                ENDDO               ! EndDo over nloopr
              ENDDO                 ! EndDo over ndiv
            ELSE
              is = is + 1
              IF(io.EQ.6) THEN
                y0 = rmint(5,io) + ddRr/2.
                z0 = rmint(6,io)
                DO il = 1,nloopr      
                  shaper(1,is,il) = 0.
                  shaper(2,is,il) = y0 - il*ddRr 
                  shaper(3,is,il) = z0
                ENDDO 
              ENDIF
              IF(io.EQ.7) THEN
                y0 = rmint(5,io) + ddRr/2.
                z0 = rmint(6,io) - ddRr/2. 
                DO il = 1,nloopr      
                  shaper(1,is,il) = 0.
                  shaper(2,is,il) = y0 - il*ddRr 
                  shaper(3,is,il) = z0 + il*ddRr
                ENDDO 
              ENDIF
              IF(io.EQ.8) THEN
                y0 = rmint(5,io) - ddRr/2.
                z0 = rmint(6,io) - ddRr/2.
                DO il = 1,nloopr      
                  shaper(1,is,il) = 0.
                  shaper(2,is,il) = y0 + il*ddRr 
                  shaper(3,is,il) = z0 + il*ddRr
                ENDDO 
              ENDIF
            ENDIF                   ! IF(io.LE.5)
          ENDDO                     ! EndDo over Nox
        ENDIF                       ! End IF(mgtype.EQ.2)
C
      ENDIF                         ! End IF(LFIRST) THEN
C
C
C For any further entry the subroutine begins here
C
C Big Torus
C
      IF(mgtype.EQ.1) THEN
C Reset field
        BT(1) = 0.
        BT(2) = 0.
        BT(3) = 0.
C Start loop over the individual loops for each coil
        DO 100 i =1,nloops
C Rotate the coil to get position of the other sectors
          DO is = 1,nsect
            DO j = 1,nshape
              SU(1,j) = shape(1,j,i)*COSSEC(is)-shape(2,j,i)*SINSEC(is)                        
              SU(2,j) = shape(1,j,i)*SINSEC(is)+shape(2,j,i)*COSSEC(is)
              SU(3,j) = shape(3,j,i)
            ENDDO
            DO k = 1,nshape
              m = k + 1
C Close the loop
              IF(m .GT. nshape) m = m - nshape
C And sum the field of all these straight elements
              CALL sda_ridge(X,SU(1,k),SU(1,m), BT)
            ENDDO
          ENDDO               ! EndDo over sectors
  100   CONTINUE              ! EndDo over loops
C
        F(1) = XNORM*BT(1)
        F(2) = XNORM*BT(2)
        F(3) = XNORM*BT(3)
      ENDIF
C
C Mini Torus
C
      IF(mgtype.EQ.2) THEN
C Reset MTorus field
        BT(1) = 0.
        BT(2) = 0.
        BT(3) = 0.
C Start loop over the individual loops for each coil
        DO 200 i = 1,nloopr
C Rotate the coil to get position of the others
          DO is = 1,nsect
            DO j = 1,nshaper
              SUr(1,j) =-shaper(2,j,i)*SINSEC(is)
              SUr(2,j) = shaper(2,j,i)*COSSEC(is)
              SUr(3,j) = shaper(3,j,i)
            ENDDO
            DO k = 1,nshaper
              m = k + 1
C Close the loop
              IF(m .GT. nshaper) m = m - nshaper
C Sum the field of all these straight elements
              CALL sda_ridge(X,SUr(1,k),SUr(1,m), BT)
            ENDDO
          ENDDO               ! EndDo over sectors
  200   CONTINUE              ! EndDo over loops
C
        F(1) = XNORM2*BT(1)
        F(2) = XNORM2*BT(2)
        F(3) = XNORM2*BT(3)
      ENDIF
C
      RETURN
      END
E 1
