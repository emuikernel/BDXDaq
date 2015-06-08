c
c returns vertex coordinates in Sector Coordinate System
c
      subroutine sda_vertex(iw,isec,vertex)
c
      IMPLICIT NONE
      integer isec,iw(1000)
      real vertex(3)
c
      integer jw(100)
      integer*2 jw16(200)
      equivalence(jw(1),jw16(1))
      real bcenter(3),bdir(3), boffset(2), bpos(3), CXras, CYras
      real Xras,Yras,DXras,DYras,RXras,RYras,CALIBras,time,timeold
      integer first, ind, etnlink
c
      data first/1/
c 1.6 outbending
c      data bcenter/0.0,0.5,0.0/ ! in Detector Coordinate System
c      data Xras/3648./, Yras/4184./   ! raster center (ADC counts)
c      data DXras/5200./, DYras/5200./ ! raster diameter (ADC counts)
c      data RXras/220./, RYras/220./   ! raster radius (mV)
c      data CALIBras/400./ ! how many mV per cm
c 2.2 GeV
      data bcenter/0.0,0.5,0.0/ ! in Detector Coordinate System
      data Xras/4074./, Yras/4141./   ! raster center (ADC counts)
      data DXras/2400./, DYras/2400./ ! raster diameter (ADC counts)
      data RXras/280./, RYras/280./   ! raster radius (mV)
      data CALIBras/570./ ! how many mV per cm
c 5.7 outbending
c      data bcenter/0.0,0.5,0.0/ ! in Detector Coordinate System
c      data Xras/4074./, Yras/4141./   ! raster center (ADC counts)
c      data DXras/6000./, DYras/6000./ ! raster diameter (ADC counts)
c      data RXras/850./, RYras/850./   ! raster radius (mV)
c      data CALIBras/1425./ ! how many mV per cm
c
      if(first.ne.0) then
        first = 0
        timeold=0
cc        call uthbook1(265,'RASTER X',1000, 0., 4000.)
cc        call uthbook1(266,'RASTER Y',1000, 0., 4000.)
        CXras = DXras/2./(RXras/CALIBras) ! ADC counts per cm
        CYras = DYras/2./(RYras/CALIBras) ! ADC counts per cm
      endif
c
      ind = etnlink(iw,'FBPM',0)
      ind = 0 ! do not use raster info (gamma run for example)
      if(ind.GT.0) then
        jw(1) = iw(ind+1)
        jw(2) = iw(ind+2)
        jw(3) = iw(ind+3)
        boffset(1) = (float(jw16(3))-Xras)/CXras
        boffset(2) = (float(jw16(6))-Yras)/CYras
        bpos(1) = bcenter(1) - boffset(1) ! have to be '-'
        bpos(2) = bcenter(2) - boffset(2) ! have to be '-'
        bpos(3) = bcenter(3)
cc        call uthfill(260,bpos(1),bpos(2),1.)
cc        call uthfill(261,bpos(1),0.,1.)
cc        call uthfill(262,bpos(2),0.,1.)

        ind = etnlink(iw,'TGBI',0)
        if(ind.GT.0) then
          time = iw(ind+3) - 6049918
          if(time.GT.timeold) then
            timeold = time
cc            call uthfill(265,time/100.,0.,bpos(1))
cc            call uthfill(266,time/100.,0.,bpos(2))
          endif
        endif

        call sda_det2sec3(bpos,vertex,isec)
**          print *,isec,' 111=> vertex=',vertex(1),vertex(2),vertex(3)
      else
        call sda_det2sec3(bcenter,vertex,isec)
**          print *,isec,' 222=> vertex=',vertex(1),vertex(2),vertex(3)
      endif
c
      return
      end
C=======================================================================
      real function sda_point2line(r0,r1,d)
C
C calculates the distance from the point 'r0' to the line which goes
C from point 'r1' to the 'd' direction ( 'd' is unit vector )
C
C          ->   -> ->
C dist = | d x (r0-r1) |
C
C see: Korn(1970) page 83
C
      real r0(3),r1(3),d(3)
      real v1(3),v2(3)
C
      call vsub(r0,r1,v1,3)
      call cross(d,v1,v2)
      sda_point2line = VMOD(v2,3)
C
      return
      end
C=======================================================================
      real function sda_line2line(r0,dd0,r1,dd1,rr0,rr1)
C
C calculates the distance from the line which goes from point 'r0' to
C the 'd0' direction to the line which goes from point 'r1' to
C the 'd1' direction ( 'd0' and 'd1' are unit vectors )
C
C           -> ->      ->   ->
C        | (r0-r1) * ( d0 x d1 ) | 
C dist = -------------------------
C                ->   ->
C              | d0 x d1 |
C
C rr0 & rr1 - points at closest approach
C
C see: Korn(1970) page 84
C
      real r0(3),r1(3),d0(3),d1(3),dd0(3),dd1(3),rr0(3),rr1(3)
      real v1(3),v2(3),v3(3),tmp,t0,t1
C
c      call vsub(r0,r1,v1,3)
c      call cross(dd0,dd1,v2)
c      tmp = v1(1)*v2(1)+v1(2)*v2(2)+v1(3)*v2(3)
c      sda_line2line = tmp / VMOD(v2,3)
C
      call vunit(dd0,d0,3)
      call vunit(dd1,d1,3)
      call vsub(r1,r0,v1,3) ! distance between current locations
      tmp = vdot(d0,d1,3) ! Cos(gamma)
      if(tmp.GT.0.999 .AND. tmp.LT.1.001) then
        print *,'sda_line2line: two lines are parallel'
        sda_line2line = 10000.
        return
      endif
c distance to the closest approach points along a lines
      do i=1,3
        v2(i) = tmp * d1(i)
      enddo
      call vsub(d0,v2,v3,3)
      t0 =  (1./(1.-tmp*tmp))*vdot(v1,v3,3)

      do i=1,3
        v2(i) = tmp * d0(i)
      enddo
      call vsub(d1,v2,v3,3)
      t1 = (-1./(1.-tmp*tmp))*vdot(v1,v3,3)
C points of closest approach
      do i=1,3
        rr0(i) = r0(i) + t0 * d0(i)
        rr1(i) = r1(i) + t1 * d1(i)
      enddo
C distance of closest approach
      call vsub(rr0,rr1,v1,3)
      sda_line2line = vmod(v1,3)
C
      return
      end
C======================================================================
      SUBROUTINE sda_det2sec3(d,s,isec)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Vector transformation from Detector to Sector
C-                         Coordinate System and reverse { DCS <--> SCS }
C-
C-   Inputs  : d(3) - vector in Detector Coordinate System
C-             isec - sector number
C-   Outputs : Vector s(3) in Sector Coordinate System
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
*C Find sector number
*      phi = ATAN2(d(2),d(1))*raddeg
*      IF(phi.LT.0.) phi = phi + 360.
*      isec = (phi + 30.)/60. + 1.
*      IF(isec.GT.6) isec = isec - 6
C Rotation
      s(1) = d(3)
      s(2) = d(1)*cossec(isec) + d(2)*sinsec(isec)        
      s(3) =-d(1)*sinsec(isec) + d(2)*cossec(isec)
      RETURN
      END



