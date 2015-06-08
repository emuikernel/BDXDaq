h02032
s 00000/00000/00000
d R 1.2 01/11/19 19:02:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_rotds.F
e
s 00102/00000/00000
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
      SUBROUTINE sda_rotds(d,s,isec)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Vector transformation from Detector to Sector
C-                         Coordinate System and reverse { DCS <--> SCS }
C-
C-   Inputs  : Vector d(3) in DCS  or s(3) & isec in SCS [call sda_rotsd]
C-   Outputs : Vector s(3) in SCS and sector number "isec"
C-   Controls: None
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created    13-FEB-1996   Bogdan Niczyporuk
C-
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C Subroutine parameters
      INTEGER isec
      REAL d(9), s(9)
C
C Local variables
      INTEGER i, ifirst
      REAL phi, pi, twopi, raddeg, sinsec(6), cossec(6)        
C
      DATA ifirst/1/
C
C
C Replacement and Rotation (input: d(3) ) {DCS --> SCS}
C
      IF(ifirst.EQ.1) THEN
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        raddeg = 180./pi
C Initialize array for rotation to sectors
        DO i=1,6
          sinsec(i) = SIN(FLOAT(i-1) * (twopi/6.))
          cossec(i) = COS(FLOAT(i-1) * (twopi/6.))
        ENDDO
        ifirst = 0
      ENDIF
C
C Find sector number
      phi = ATAN2(d(5),d(4))*raddeg
      IF(phi.LT.0.) phi = phi + 360.
      isec = (phi + 30.)/60. + 1.
      IF(isec.GT.6) isec = isec - 6
C Vertex {x,y,z}
      s(1) = d(3)
      s(2) = d(1)*cossec(isec) + d(2)*sinsec(isec)        
      s(3) =-d(1)*sinsec(isec) + d(2)*cossec(isec)
C Direction cosines {Cx,Cy,Cz}
      s(4) = d(6)
      s(5) = d(4)*cossec(isec) + d(5)*sinsec(isec)        
      s(6) =-d(4)*sinsec(isec) + d(5)*cossec(isec)
C {p,m,Q}
      s(7) = d(7)
      s(8) = d(8)
      s(9) = d(9)
      RETURN
C 
C Replacement and Rotation (input: s(3) & isec )  {SCS --> DCS}
C
      ENTRY sda_rotsd(d,s,isec)
C
      IF(ifirst.EQ.1) THEN
        pi     = ACOS(-1.)
        twopi  = 2.*pi
        raddeg = 180./pi
C Initialize array for rotation to sectors
        DO i=1,6
          sinsec(i) = SIN(FLOAT(i-1) * (twopi/6.))
          cossec(i) = COS(FLOAT(i-1) * (twopi/6.))
        ENDDO
        ifirst = 0
      ENDIF
C
C Vertex {x,y,z}
      d(1) = s(2)*cossec(isec) - s(3)*sinsec(isec)
      d(2) = s(2)*sinsec(isec) + s(3)*cossec(isec)
      d(3) = s(1)
**      d(1) = s(2)
**      d(2) = s(3)
**      d(3) = s(1)
C Direction cosines {Cx,Cy,Cz}
      d(4) = s(5)*cossec(isec) - s(6)*sinsec(isec)
      d(5) = s(5)*sinsec(isec) + s(6)*cossec(isec)
      d(6) = s(4)
C {p,m,Q} 
      d(7) = s(7)
      d(8) = s(8)
      d(9) = s(9)
C
      RETURN
      END
E 1
