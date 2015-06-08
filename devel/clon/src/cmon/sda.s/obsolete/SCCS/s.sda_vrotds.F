h33153
s 00000/00000/00000
d R 1.2 01/11/19 19:02:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_vrotds.F
e
s 00083/00000/00000
d D 1.1 01/11/19 19:02:41 boiarino 1 0
c date and time created 01/11/19 19:02:41 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_vrotds(d,s,isec)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Vector transformation from Detector to Sector
C-                         Coordinate System and reverse { DCS <--> SCS }
C-
C-   Inputs  : Vector d(3) in DCS  [or s(3) & isec in SCS]
C-   Outputs : Vector s(3) & isec in SCS  [or d(3) in DCS]
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
      REAL d(3), s(3)
C
C Local variables
      INTEGER i, ifirst
      REAL phi, pi, twopi, raddeg, sinsec(6), cossec(6)        
C
      DATA ifirst/1/
C
C
C Rotation (input: d(3), output: s(3) & isec ) {DCS --> SCS}
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
      phi = ATAN2(d(2),d(1))*raddeg
      IF(phi.LT.0.) phi = phi + 360.
      isec = (phi + 30.)/60. + 1.
      IF(isec.GT.6) isec = isec - 6
C Rotation
      s(1) = d(3)
      s(2) = d(1)*cossec(isec) + d(2)*sinsec(isec)        
      s(3) =-d(1)*sinsec(isec) + d(2)*cossec(isec)
      RETURN
C
C 
C Rotation (input: s(3) & isec, output: d(3) )  {SCS --> DCS}
C
      ENTRY sda_vrotsd(d,s,isec)
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
      d(1) = s(2)*cossec(isec) - s(3)*sinsec(isec)
      d(2) = s(2)*sinsec(isec) + s(3)*cossec(isec)
      d(3) = s(1)
C
      RETURN
      END
E 1
