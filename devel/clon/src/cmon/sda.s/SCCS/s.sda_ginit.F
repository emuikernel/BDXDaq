h53478
s 00000/00000/00000
d R 1.2 01/11/19 19:02:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/sda_ginit.F
e
s 00869/00000/00000
d D 1.1 01/11/19 19:01:59 boiarino 1 0
c date and time created 01/11/19 19:01:59 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_ginit
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : SDA Geometry initialization and/or Writes BOS bank
C-                         and/or Reads BOS bank.
C-
C-   Inputs  :
C-   Outputs :
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created    18-OCT-1990   Bogdan Niczyporuk
C-
C-   Called by sda_init
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
C External functions
      INTEGER iucomp, ETNCREATE, ETNLINK
      REAL VDOT
C
C Local variables
      INTEGER i,j,k, ip,il, ind, isc,nsl,id, ic, Ncol, Nrow, utime, ierr
      INTEGER nw,is,la3,ireg,la, ncell, isec, version, Nword, isec_cor        
      REAL dc_dphi, dc_cutz(3), mini_stag, phi_stag, deltaR, vp(3)      
      REAL phiw, dc_wpend(6), dlen, spac, gbyb, wxbywz, wybywz, phi_pln
      REAL degrad, psi, DX, DY, PHT, tan30, dc_cor(9,6), dccor(3,3,6)
      REAL dccor2(3,3,6)
      EQUIVALENCE ( dc_cor(1,1),dccor(1,1,1) ) 
      CHARACTER*3 DET(npln)
      CHARACTER*64 fname
      CHARACTER*24 utimes
      CHARACTER*8  kname
C
C Vertex plane
      REAL r_vt,d_vt, x0_vt(3), phi_vt_ort,phi_vt_min,phi_vt_max
C Start Counter plane
      REAL r_st(2),d_st(2), x0_st(3,2), phi_st_ort(2)
     1                                , phi_st_min(2),phi_st_max(2)
C Drift Chabers
      REAL rsupr(8,6), w_offset(6), wlast_SL2(6), wlast_SL3(6),
     1       rsu(8,6,6), wlast_SL4(6), dc_cut(3)
C Cerenkov Counter
      REAL r_cc,d_cc, x0_cc(3), phi_cc_ort,phib_cc_min, phib_cc_max
C Scintilator counters
      INTEGER nslab(2,4)
      REAL sc_xymid(6,4), phi_ort(4)
C Electromagnetic Counter plane
      REAL r_ec(2),d_ec(2),dd_ec(2), x0_ec(3,2), phi_ec_ort(2)
     1                                , phi_ec_min(2),phi_ec_max(2)        
c
      integer ifirst
      data ifirst/1/
C
C=================== DATA BLOCK ========================================
C
C
C Vertex plane used in fitting
      DATA r_vt       /0.00/
      DATA d_vt       /0.05/
      DATA x0_vt      /0.,0.,0./
      DATA phi_vt_ort /90.0/
      DATA phi_vt_min /  0.1/
      DATA phi_vt_max /179.9/
C
C Start Counter
      DATA r_st       / 0.00,  0.00 /
      DATA d_st       /11.00, 20.026/
      DATA x0_st      /0.,0.,0., 0.,0.,0./
      DATA phi_st_ort /90.00, 60.00/
      DATA phi_st_min / 27.646,  7.00 /
      DATA phi_st_max /152.354, 27.646/
C
C Drift Chambers 
C  Parameters for superlayers (3-d layer in each SL):
C                 1-st g.w. last g.w.
      DATA rsupr/
C          1         2         3         4      5    6       7      8
C          R        PHImin    PHImax     X0     Y0   Z0  Nwires  phi_plane
     1  110.9339,  35.9188, 124.0652,   0.0, -53.0, 0.0,  128.0,  90.0,
     2  118.3126,  33.4644, 123.6084,   0.0, -53.0, 0.0,  140.0,  90.0,
     3  235.2600,  25.0000, 133.7000,  29.3, -67.6, 0.0,  192.0, 180.0,
     4  251.0600,  23.9000, 134.3000,  29.3, -67.6, 0.0,  192.0, 180.0,
     5  338.2993,  21.1800, 138.1800,  48.0, -70.0, 0.0,  192.0, 180.0,
     6  364.6003,  21.1800, 138.1800,  48.0, -70.0, 0.0,  192.0, 180.0/
C
C Wire offset for Reg.1 (SL1 & SL2) and last wire numbers for SL2, SL3 & SL4
      DATA w_offset  /  4.,   3.,   3.,   2.,   2.,   1./
      DATA wlast_SL2 /142., 142., 142., 126., 121., 120./
      DATA wlast_SL3 /184., 185., 186., 187., 188., 189./
      DATA wlast_SL4 /189., 189., 190., 191., 192., 192./
C Distances from endplate to mid-coil plane for 3 regions (wire cutoff)
      DATA dc_cut    /3.302, 11.5199, 14.97/
C
C                    DC Alignment (Design Geometry without Corrections)
C                    --------------------------------------------------
      DATA dccor/
C  Sec/   Dx1    Dy1    Dz1     Dx2    Dy2    Dz2     Dx3    Dy3    Dz3
     1  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     2  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     3  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     4  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     5  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     6  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000/
C
C                      DC Alignments (Runs >= 14000)
C                      ------------------------------------------------
      DATA dccor2/ 
C  Sec/   Dx1    Dy1    Dz1     Dx2    Dy2    Dz2     Dx3    Dy3    Dz3
     1  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     2  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     3  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     4  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     5  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,
     6  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.000, 0.000/
c     1  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.330,-3.300,
c     2  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000,-0.130,-1.200,
c     3  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.200,-0.500,
c     4  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.100, 0.300,
c     5  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.420,-0.200,
c     6  0.000, 0.000, 0.000,  0.000, 0.000, 0.000,  0.000, 0.450,-2.200/
C
C Cerenkov Counter
      DATA r_cc       /394.0/         ! radius of front arc (from B. Center)
      DATA d_cc       /480.0/         ! distance to back plane
      DATA x0_cc      /48.0,-70.0, 0./! Bogdan Center
      DATA phi_cc_ort /25.00/         ! ortvector to the back place
      DATA phib_cc_min /18.0/         ! PHIBmin (relative to Bogdan Center)
      DATA phib_cc_max /57.0/         ! PHIBmax (relative to Bogdan Center)
C
C Scintilator Counter 
C Elton parameters of January 7, 1994 for SC counters
C
C                    Xright  Yright   Xleft   Yleft    Xmid     Ymid
      DATA sc_xymid/
     1               510.79,  69.42, 363.65, 384.95,    0.0,    0.0,
     2               357.37, 372.14, 120.72, 435.55,    0.0,    0.0,
     3               124.68, 441.43,-111.97, 378.02,  17.11, 412.61,
     4              -112.24, 383.89,-295.59, 230.04,-248.79, 269.31/
      DATA nslab  /    23, 0,    11, 0,     5, 3,     4, 2/
      DATA phi_ort/  25.0,  75.0, 105.0, 130.0/
      DATA PHT    /30.0  /                             ! Space for Photo Tube
      DATA tan30  /0.57735/                            ! TAN(30deg)
C
C Electromagnetic Counter
      DATA r_ec       /  0.00,   0.00/
      DATA d_ec       / 510.54, 490.2/
      DATA dd_ec      /  47.60, 55.90/       ! ~ 39x1.00(sc) + 38x0.23(Pb) and
C                                            ! ~ 33x1.50(sc) + 32x0.20(Pb)
      DATA x0_ec      /0.,0.,0., 0.,0.,0./
      DATA phi_ec_ort / 25.00,  75.00/
      DATA phi_ec_min /  5.00, 49.53/
      DATA phi_ec_max / 45.00, 72.99/
C
C==================== End of DATA BLOCK ================================
C bosio
      INTEGER ndimb
      PARAMETER (ndimb = 700000)
      INTEGER IW(ndimb)
      REAL RW(ndimb)
      EQUIVALENCE (IW(1),RW(1))
      if(ifirst) then
        ifirst = 0
        CALL BOS(IW,ndimb)
      endif
C bosio
C
      degrad = ACOS(-1.)/180.
      gbyb   = -1/tan30
      dc_cutz(1) = dc_cut(1)/COS(30.*degrad)
      dc_cutz(2) = dc_cut(2)/COS(30.*degrad)
      dc_cutz(3) = dc_cut(3)/COS(30.*degrad)
C Convert angles from DEGtoRAD for DC (loop over SuperLayers)
      DO is = 1,6
        rsupr(2,is) = rsupr(2,is)*degrad
        rsupr(3,is) = rsupr(3,is)*degrad
        rsupr(8,is) = rsupr(8,is)*degrad
      ENDDO
C
      DET(1) = 'VTX'
      DET(2) = 'ST1'
      DET(3) = 'ST2'
      DO ip = 4,9
        DET(ip) = 'DC1'
      ENDDO
      DO ip = 10,15
        DET(ip) = 'DC2'
      ENDDO
      DO ip = 16,21
        DET(ip) = 'DC3'
      ENDDO
      DO ip = 22,27
        DET(ip) = 'DC4'
      ENDDO
      DO ip = 28,33
        DET(ip) = 'DC5'
      ENDDO
      DO ip = 34,39
        DET(ip) = 'DC6'
      ENDDO
      DET(40) = 'CC '
      DET(41) = 'SC1'
      DET(42) = 'SC2'
      DET(43) = 'SC3'
      DET(44) = 'SC4'
      DET(45) = 'EC1'
      DET(46) = 'EC2'
C
C
C Initialization of CLAS Geometry with option of writing BOS bank
C
      IF(iucomp(4HINIT,lgeom,10) .NE. 0) THEN
      DO isec = 1,6
C
C *** Vertex plane (1-nd plane) used in fitting
C
C Center of curvature for arc or offset for plane
      sda_pln(1,1,isec) = x0_vt(1)
      sda_pln(2,1,isec) = x0_vt(2)
      sda_pln(3,1,isec) = x0_vt(3)
C Unit vector normal to the plane (irrelevent for arc)
      sda_pln(4,1,isec) = COS(phi_vt_ort*degrad)
      sda_pln(5,1,isec) = SIN(phi_vt_ort*degrad)
      sda_pln(6,1,isec) = 0.
C
      sda_pln(7,1,isec) = r_vt               ! Radius (irrelevent for plane)
      sda_pln(8,1,isec) = d_vt               ! Distance (irrelevent for arc)
      sda_pln(9,1,isec) = 0.
      sda_pln(10,1,isec)= 0.
      sda_pln(11,1,isec)= 0.
C Xright,Yright and Xleft,Yleft
      psi = (phi_vt_ort - phi_vt_min)*degrad
      sda_pln(12,1,isec) = 
     1   COS(phi_vt_min*degrad)*sda_pln(8,1,isec)/COS(psi)   
      sda_pln(13,1,isec) =
     1   SIN(phi_vt_min*degrad)*sda_pln(8,1,isec)/COS(psi)
      psi = (phi_vt_ort - phi_vt_max)*degrad
      sda_pln(14,1,isec) =
     1   COS(phi_vt_max*degrad)*sda_pln(8,1,isec)/COS(psi)
      sda_pln(15,1,isec) =
     1   SIN(phi_vt_max*degrad)*sda_pln(8,1,isec)/COS(psi)
      sda_pln(16,1,isec) = 0.
      sda_pln(17,1,isec) = 0.
      sda_pln(18,1,isec) = 0.
      sda_pln(19,1,isec) = 0.
      sda_pln(20,1,isec) = 0.
C
C
C *** Start counter planes (two planes)
C
      i = 0
      DO ip = nst_min,nst_max
        i = i + 1
C Store offset corrections (so far shift along the beam)
        sda_pln(9,ip,isec) = 0.
        sda_pln(10,ip,isec)= ztarg(5)           ! Shift of the Start Counter
        sda_pln(11,ip,isec)= 0.
C Center of curvature for arc or offset for plane
        sda_pln(1,ip,isec) = x0_st(1,i) + sda_pln(10,ip,isec)
        sda_pln(2,ip,isec) = x0_st(2,i)
        sda_pln(3,ip,isec) = x0_st(3,i)
C Unit vector normal to the plane (irrelevent for arc)
        sda_pln(4,ip,isec) = COS(phi_st_ort(i)*degrad)
        sda_pln(5,ip,isec) = SIN(phi_st_ort(i)*degrad)
        sda_pln(6,ip,isec) = 0.
C
        sda_pln(7,ip,isec) = r_st(i)           ! Radius (irrelevent for plane)
        sda_pln(8,ip,isec) = d_st(i)           ! Distance (irrelevent for arc)
C Xright,Yright and Xleft,Yleft
        psi = (phi_st_ort(i) - phi_st_min(i))*degrad
        sda_pln(12,ip,isec) = sda_pln(1,ip,isec)
     1     +COS(phi_st_min(i)*degrad)*sda_pln(8,ip,isec)/COS(psi)
        sda_pln(13,ip,isec) = sda_pln(2,ip,isec)
     1     +SIN(phi_st_min(i)*degrad)*sda_pln(8,ip,isec)/COS(psi)
        psi = (phi_st_ort(i) - phi_st_max(i))*degrad
        sda_pln(14,ip,isec) = sda_pln(1,ip,isec)
     1     +COS(phi_st_max(i)*degrad)*sda_pln(8,ip,isec)/COS(psi)
        sda_pln(15,ip,isec) = sda_pln(2,ip,isec) 
     1     +SIN(phi_st_max(i)*degrad)*sda_pln(8,ip,isec)/COS(psi)
C The Lengths of Start Counters 
        sda_pln(16,ip,isec) = SQRT( 
     1     (sda_pln(12,ip,isec)-sda_pln(14,ip,isec))**2  
     2    +(sda_pln(13,ip,isec)-sda_pln(15,ip,isec))**2 )
        sda_pln(17,ip,isec) = 0.
C Nslabs/sector, RadLen, Tsc (thickness)
        sda_pln(18,ip,isec) = 1.00
        sda_pln(19,ip,isec) = 42.4
        sda_pln(20,ip,isec) = 0.30
      ENDDO
C
C
C *** Geometry of Drift Chamber
C
C Get DC geometry corrections (alignments)
      DO 20 j = 1,3                          ! Region
      DO 20 i = 1,3
        dccor(i,j,isec) = dccor2(i,j,isec)
 20   CONTINUE
C
C Alignment of DC:
C  i.e. to modify DC geom. for given sector without re-compile of "user_sda.F"
       isec_cor = zgcor(10)                     ! Sec# for DC to be corrected
       IF(isec_cor .EQ. isec) THEN
         DO i = 4,9
           dc_cor(i,isec) = zgcor(i)
         ENDDO
       ENDIF
C
C Loop over the layers of DC
      DO il = 1,npl_dc
        ip   = il + nst_max                     ! Plane #
        ireg = (il-1)/12 + 1                    ! Region #
        is   = (il-1)/6 + 1                     ! Superlayer #
        la   = il - (is-1)*6                    ! Layer # in superlayer
C
C DC geometry alignmaents
        DO i = 1,8
          rsu(i,is,isec) = rsupr(i,is)
          IF(i.EQ.4) rsu(i,is,isec) = rsu(i,is,isec) +dccor(1,ireg,isec)
          IF(i.EQ.5) rsu(i,is,isec) = rsu(i,is,isec) +dccor(2,ireg,isec)
          IF(i.EQ.6) rsu(i,is,isec) = rsu(i,is,isec) +dccor(3,ireg,isec)
        ENDDO
C
C Starting phi angle of the plane part (Reg.1)
        phi_pln = rsu(8,is,isec)
C
C Get "dphi" between sense wires in given layer "il"
        ncell = rsu(7,is,isec) + 1
        IF(is.LT.3) THEN
          dc_dphi = (phi_pln-rsu(2,is,isec)+TAN(rsu(3,is,isec)-phi_pln))
     1             /ncell
        ELSE
          dc_dphi = (rsu(3,is,isec)-rsu(2,is,isec))/ncell       
        ENDIF
C Store "dphi"
        sda_pln(13,ip,isec) = dc_dphi
C
C Mini stagger for Region 1 only
        mini_stag = 0.
        IF(is.LT.3) mini_stag = 0.03
        phi_stag= mini_stag/rsu(1,is,isec)
C Add big stagger for even layers
        IF(MOD(il,2).EQ.0) phi_stag = -phi_stag + 0.5*dc_dphi
C
        IF(is.LT.3) THEN
C Total No of logical wires
          sda_pln(14,ip,isec)  = rsu(7,is,isec) + w_offset(3) - 1.
C First physical wire #
          sda_pln(15,ip,isec)  = w_offset(la)
C Last physical wire #
          sda_pln(16,ip,isec)  = sda_pln(14,ip,isec)
          IF(is.EQ.2) sda_pln(16,ip,isec) = wlast_SL2(la)
C Phi of 1-st logical wire in Bogdan Center Cordinate System
          sda_pln(12,ip,isec) = rsu(2,is,isec) - dc_dphi + phi_stag
        ELSE
          sda_pln(14,ip,isec)  = rsu(7,is,isec) 
          sda_pln(15,ip,isec)  = 1.
          sda_pln(16,ip,isec)  = sda_pln(14,ip,isec)
          IF(is.EQ.3) sda_pln(16,ip,isec) = wlast_SL3(la)
          IF(is.EQ.4) sda_pln(16,ip,isec) = wlast_SL4(la)
C Phi of 1-st logical wire in Bogdan Center Cordinate System
          sda_pln(12,ip,isec) = rsu(2,is,isec) + dc_dphi + phi_stag
        ENDIF
C
C Last wire # in cyl. part of DC (see "sda_ginit.F")        
        nw = sda_pln(14,ip,isec)
        DO i = 1,nw
          phiw = sda_pln(12,ip,isec) + (i-1)*sda_pln(13,ip,isec)
          IF(phiw.LE.phi_pln) sda_pln(17,ip,isec) = i
        ENDDO            
C
C Store radius of each layer
        deltaR = cos(30.*degrad)*rsu(1,is,isec)*dc_dphi
        sda_pln(7,ip,isec) = rsu(1,is,isec) + deltaR*FLOAT(la-3)
C
C Store the offset coordinates for each layer
        sda_pln(1,ip,isec) = rsu(4,is,isec)
        sda_pln(2,ip,isec) = rsu(5,is,isec)
        sda_pln(3,ip,isec) = rsu(6,is,isec)
C
C Store ortvect to the plane part of the layer and distance from the origin
        IF(is.LT.3) THEN
          sda_pln(4,ip,isec) = COS(phi_pln)
          sda_pln(5,ip,isec) = SIN(phi_pln)
          sda_pln(6,ip,isec) = 0.
c          vp(1)=sda_pln(7,ip,isec)*sda_pln(4,ip,isec)+sda_pln(1,ip,isec)
c          vp(2)=sda_pln(7,ip,isec)*sda_pln(5,ip,isec)+sda_pln(2,ip,isec)
          vp(1)=sda_pln(7,ip,isec)*sda_pln(4,ip,isec)          
          vp(2)=sda_pln(7,ip,isec)*sda_pln(5,ip,isec)
          vp(3)=0.
          sda_pln(8,ip,isec) = VDOT(vp,sda_pln(4,ip,isec),3)
        ELSE
          sda_pln(4,ip,isec) = 0.
          sda_pln(5,ip,isec) = 0.
          sda_pln(6,ip,isec) = 0.
          sda_pln(8,ip,isec) = 0.
        ENDIF
C SB ???
*        sda_pln(9,ip,isec) = dccor(1,ireg,isec)
*        sda_pln(10,ip,isec)= dccor(2,ireg,isec)
*        sda_pln(11,ip,isec)= dccor(3,ireg,isec)
C
C Store the Stereo angle = 6 degrees (always)
        IF(is.LT.3) THEN 
          sda_pln(18,ip,isec) = 0.
          IF(MOD(is,2).EQ.1) sda_pln(18,ip,isec) = 6.*degrad
        ELSE
          sda_pln(18,ip,isec) = 6.*degrad
          IF(MOD(is,2).EQ.1) sda_pln(18,ip,isec) = 0.
        ENDIF
C Store half distance between wires (ca maximum drift distance)
        sda_pln(19,ip,isec)= .5*sda_pln(7,ip,isec)*dc_dphi
ccc      print *,'0:1=>',sda_pln(7,ip,isec),dc_dphi
        sda_pln(20,ip,isec) = dc_cutz(ireg)
      ENDDO                                  ! End DO over the layers of DC
C    
C
C *** Geometry of Cerenkov Counter (entering arc only)
C
      ip = ncc_min
C Store offset corrections (dX0, dY0, dZ0)
      sda_pln(9,ip,isec)  = 0.
      sda_pln(10,ip,isec) = 0.       
      sda_pln(11,ip,isec) = 0.               
C Center of curvature for arc or offset for plane
      sda_pln(1,ip,isec)  = x0_cc(1) + sda_pln( 9,ip,isec)
      sda_pln(2,ip,isec)  = x0_cc(2) + sda_pln(10,ip,isec)
      sda_pln(3,ip,isec)  = x0_cc(3) + sda_pln(11,ip,isec)
C Unit vector normal to the plane (irrelevent for arc)
      sda_pln(4,ip,isec)  = COS(phi_cc_ort*degrad)
      sda_pln(5,ip,isec)  = SIN(phi_cc_ort*degrad)
      sda_pln(6,ip,isec)  = 0.
C
      sda_pln(7,ip,isec)  = r_cc               ! Radius (irrelevent for plane)
      sda_pln(8,ip,isec)  = d_cc               ! Distance (irrelevent for arc)
      sda_pln(16,ip,isec) = phib_cc_min*degrad
      sda_pln(17,ip,isec) = phib_cc_max*degrad        
      sda_pln(12,ip,isec) = sda_pln(1,ip,isec)
     1                      + r_cc*COS(sda_pln(16,ip,isec))
      sda_pln(13,ip,isec) = sda_pln(2,ip,isec)
     1                      + r_cc*SIN(sda_pln(16,ip,isec))
      sda_pln(14,ip,isec) = sda_pln(1,ip,isec)
     1                      + r_cc*COS(sda_pln(17,ip,isec))
      sda_pln(15,ip,isec) = sda_pln(2,ip,isec)
     1                      + r_cc*SIN(sda_pln(17,ip,isec))        
      sda_pln(18,ip,isec) = 2.0
      sda_pln(19,ip,isec) = 18.
      sda_pln(20,ip,isec) = 0.
C
C
C *** Geometry of Scintilator Counter (entering planes only)
C
      DO i = 1,npl_sc
        ip = i + ncc_min
C Store offset corrections (dX0, dY0, dZ0)
        sda_pln(9,ip,isec)  = 0.      
        sda_pln(10,ip,isec) = 0.
        sda_pln(11,ip,isec) = 0.
C Center of curvature for arc or offset for plane
        sda_pln(1,ip,isec) = 0.
        sda_pln(2,ip,isec) = 0.
        sda_pln(3,ip,isec) = 0.
        IF(i.EQ.1) THEN
          sda_pln(1,ip,isec) = sda_pln(1,ip,isec) + sda_pln( 9,ip,isec)
          sda_pln(2,ip,isec) = sda_pln(2,ip,isec) + sda_pln(10,ip,isec)
          sda_pln(3,ip,isec) = sda_pln(3,ip,isec) + sda_pln(11,ip,isec)
        ENDIF
C Normal Vector to plane "ip"
        sda_pln(4,ip,isec) = COS(phi_ort(i)*degrad)
        sda_pln(5,ip,isec) = SIN(phi_ort(i)*degrad)
        sda_pln(6,ip,isec) = 0.
C Radius of the curvature
        sda_pln(7,ip,isec) = 0.
C Store the plane edges {x,y} (inner corners)
        j = 0
        DO k = 12,17
          j = j + 1
          sda_pln(k,ip,isec) = sc_xymid(j,i)
        ENDDO
C Calculate distance to the plane
        sda_pln(8,ip,isec) = sda_pln(12,ip,isec)*sda_pln(4,ip,isec)
     1                      +sda_pln(13,ip,isec)*sda_pln(5,ip,isec)
C Apply adjustments for displacement of SC along beam
        IF(i.EQ.1) THEN
          sda_pln(12,ip,isec) = sda_pln(12,ip,isec) +sda_pln( 9,ip,isec)
          sda_pln(14,ip,isec) = sda_pln(14,ip,isec) +sda_pln( 9,ip,isec)
          sda_pln(13,ip,isec) = sda_pln(13,ip,isec) +sda_pln(10,ip,isec)
          sda_pln(15,ip,isec) = sda_pln(15,ip,isec) +sda_pln(10,ip,isec)
        ENDIF
C Store number of slabs for each plane {Nsc1,Nsc2}         
        sda_pln(18,ip,isec) = nslab(1,i)
        sda_pln(19,ip,isec) = nslab(2,i)
C Store the thickness of a slab
        sda_pln(20,ip,isec) = 5.08
      ENDDO
C
C
C *** Geometry of Electromagnetic Counter (entering plane only)
C
      i = 0
      DO ip = nec_min,nec_max
        i = i + 1
C Store offset corrections (so far shift along the beam)
        sda_pln(9,ip,isec) = 0.
        sda_pln(10,ip,isec)= 0.
        sda_pln(11,ip,isec)= 0.
C Center of curvature for arc or offset for plane
        sda_pln(1,ip,isec) = x0_ec(1,i)
        sda_pln(2,ip,isec) = x0_ec(2,i)
        sda_pln(3,ip,isec) = x0_ec(3,i)
        IF(i.EQ.1) THEN
          sda_pln(1,ip,isec) = sda_pln(1,ip,isec) + sda_pln( 9,ip,isec)
          sda_pln(2,ip,isec) = sda_pln(2,ip,isec) + sda_pln(10,ip,isec)
          sda_pln(3,ip,isec) = sda_pln(3,ip,isec) + sda_pln(11,ip,isec)
        ENDIF
C Unit vector normal to the plane (irrelevent for arc)
        sda_pln(4,ip,isec) = COS(phi_ec_ort(i)*degrad)
        sda_pln(5,ip,isec) = SIN(phi_ec_ort(i)*degrad)
        sda_pln(6,ip,isec) = 0.
C
        sda_pln(7,ip,isec) = r_ec(i)           ! Radius (irrelevent for plane)
        sda_pln(8,ip,isec) = d_ec(i)           ! Distance (irrelevent for arc)
C Xright,Yright and Xleft,Yleft
        psi = (phi_ec_ort(i) - phi_ec_min(i))*degrad
        sda_pln(12,ip,isec) = COS(phi_ec_min(i)*degrad)
     1                 *sda_pln(8,ip,isec)/COS(psi) + sda_pln(1,ip,isec)
        sda_pln(13,ip,isec) = SIN(phi_ec_min(i)*degrad)
     1                 *sda_pln(8,ip,isec)/COS(psi) + sda_pln(2,ip,isec)
        psi = (phi_ec_ort(i) - phi_ec_max(i))*degrad
        sda_pln(14,ip,isec) = COS(phi_ec_max(i)*degrad)
     1                 *sda_pln(8,ip,isec)/COS(psi) + sda_pln(1,ip,isec)
        sda_pln(15,ip,isec) = SIN(phi_ec_max(i)*degrad)
     1                 *sda_pln(8,ip,isec)/COS(psi) + sda_pln(2,ip,isec)
        IF(ip.EQ.nec_min) THEN
          sda_pln(16,ip,isec) = 39.0         ! No of layers {SC + Pb}
          sda_pln(17,ip,isec) = 36.0         ! No of SC slabs in a layer 
          sda_pln(18,ip,isec) = 54.22*degrad ! Opening angle
          sda_pln(19,ip,isec) = 1.00         ! Tsc
          sda_pln(20,ip,isec) = 0.23         ! Tpb
        ENDIF
        IF(ip.EQ.nec_max) THEN
          sda_pln(16,ip,isec) = 33.0         ! No of layers {SC + Pb}
          sda_pln(17,ip,isec) = 24.0         ! No of SC slabs in X direction 
          sda_pln(18,ip,isec) = 40.0         ! No of SC slabs in Z direction
          sda_pln(19,ip,isec) = 1.50         ! Tsc 
          sda_pln(20,ip,isec) = 0.20         ! Tpb
        ENDIF
      ENDDO
C
C
C Write SDA Geometry into BOS bank
C
      IF(iucomp(4HWRIT,lgeom,10) .NE. 0) THEN
        IF(isec.EQ.1) THEN
          CALL ETNFORMAT(IW,'GHDR','8I')
          CALL ETNFORMAT(IW,'GSDA','F' )
          CALL ETLCTL(IW,'E=','GHDRGSDA')
          CALL FPARM('OPEN SDAGEOM UNIT=57 FILE="sdageom.bos"'//
     1      'RECL=36000 ACTION=WRITE ACCESS=SEQ STATUS=NEW FORM=BINARY')
C Write Header
          ind = ETNCREATE(IW,'GHDR',0,8,1)
          version = 2
          CALL GetUnixTime(utime)
          IW(ind+1) = version
          IW(ind+2) = utime
          DO i = 3,8
            IW(ind+i) = 0
          ENDDO
        ENDIF
C
C Write "sda_pln(20,46,isec)" for a sector
        Ncol = 20
        Nrow = 46
        ind = ETNCREATE(IW,'GSDA',isec,Ncol,Nrow)
        i = 0
        DO ip = 1,Nrow
          DO ic = 1,Ncol
            i = i + 1
            RW(ind+i) = sda_pln(ic,ip,isec)
          ENDDO
        ENDDO
C Write & Close
        IF(isec.EQ.6) THEN
          CALL FWBOS(IW,57,'E',ierr)
          CALL ETLDROP(IW,'E')
          CALL ETNGARB(IW)
          CALL FWBOS(IW,57,'0',ierr)
          CALL FPARM('CLOSE SDAGEOM')
        ENDIF
      ENDIF                            ! End IF(iucomp(4HWRIT,lgeom,10)
C
      ENDDO                            ! End of loop over Sectors
      ENDIF                            ! End IF(iucomp(4HINIT,lgeom,10)
C
C
C Read BOS Geometry file
C
      IF(iucomp(4HREAD,lgeom,10) .NE. 0) THEN
C Open & Read
        CALL getgbosforsda_ginit(fname)
        CALL FPARM('OPEN SDAGEOM UNIT=56 FILE="' // fname // '"')
        CALL FRBOS(IW,56,'E',ierr)
C Decode Header record
        ind = ETNLINK(IW,'GHDR',0)
        IF(ind.EQ.0) THEN
          WRITE(6,*) ' The bank "GHDR" is empty'
          GO TO 999
        ENDIF
        version = IW(ind+1)
        utime   = IW(ind+2)
        CALL GetASCIITime(utime,utimes)
        kname   = 'GSDA vs.'
c*      WRITE(6,201) kname,version,utimes
C Fill array "sda_pln(20,ip,isec)"
        DO isec = 1,6
          ind  = ETNLINK(IW,'GSDA',isec)
          IF(ind.EQ.0) THEN
            WRITE(6,*) ' The bank "GSDA" is empty'
            GO TO 999
          ENDIF
          Ncol = IW(ind-5)        ! Number of items for one plane [20]
          Nrow = IW(ind-4)        ! Number of planes [1 - 46]
          Nword = Ncol*Nrow
          IF(IW(ind).NE.Nword) THEN
            WRITE(6,*) ' IW(ind) .NE. Ncol*Nrow'
            GO TO 999
          ENDIF
          i = 0
          DO ip = 1,Nrow
            DO ic = 1,Ncol
              i = i + 1
              sda_pln(ic,ip,isec) = RW(ind+i)
            ENDDO
          ENDDO
        ENDDO                            ! End of loop over Sectors
C Drop & Close
        CALL ETLDROP(IW,'E')
        CALL ETNGARB(IW)
        CALL FPARM('CLOSE SDAGEOM')
      ENDIF                              ! End IF(iucomp(4HREAD,lgeom,10)
C              
C
      DO isec = 1,6
C Calculate: wire position "dc_wpmid(3,192,36,isec) at the middle plane,
C            wire direction (unit vector) "dc_wdir(3,192,36,isec)  and
C            half wire length "dc_wlen(2,192,36,isec) for +Z and -Z
      DO il = 1,npl_dc
        ireg = (il-1)/12 + 1                    ! Region #
        ip = il + nst_max                       ! Plane #
        phi_pln = 3.14159
        IF(il.LE.12)phi_pln=ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec))
        nw = sda_pln(14,ip,isec)
        DO i = 1,nw
          phiw = sda_pln(12,ip,isec) + (i-1)*sda_pln(13,ip,isec)
C Cylindrical part (up to "phi_pln")
          IF(phiw.LE.phi_pln) THEN
C Store position of the wire
            dc_wpmid(1,i,il,isec) = 
     1        sda_pln(7,ip,isec)*COS(phiw) + sda_pln(1,ip,isec)        
            dc_wpmid(2,i,il,isec) = 
     1        sda_pln(7,ip,isec)*SIN(phiw) + sda_pln(2,ip,isec)
            dc_wpmid(3,i,il,isec) = sda_pln(3,ip,isec)
C
C Store direction of the wire
            dc_wdir(1,i,il,isec) =  SIN(phiw)*SIN(sda_pln(18,ip,isec))
            dc_wdir(2,i,il,isec) = -COS(phiw)*SIN(sda_pln(18,ip,isec))
            dc_wdir(3,i,il,isec) =  COS(sda_pln(18,ip,isec))
          ELSE
C Plane part
            is   = (il-1)/6 + 1
            la3  = (is-1)/6 + 3 + nst_max
            dlen = sda_pln(7,la3,isec)*sda_pln(13,la3,isec) 
C Store position of the wire
            dc_wpmid(1,i,il,isec)=
     1         dc_wpmid(1,i-1,il,isec)-dlen*SIN(phi_pln)                
            dc_wpmid(2,i,il,isec)=
     1         dc_wpmid(2,i-1,il,isec)+dlen*COS(phi_pln)
            dc_wpmid(3,i,il,isec) = sda_pln(3,ip,isec)
C                    
C Store direction of the wire
            dc_wdir(1,i,il,isec) =
     1        SIN(phi_pln)*SIN(sda_pln(18,ip,isec))
            dc_wdir(2,i,il,isec) =
     1        -COS(phi_pln)*SIN(sda_pln(18,ip,isec))
            dc_wdir(3,i,il,isec) =   COS(sda_pln(18,ip,isec))
          ENDIF
C
          spac = - sda_pln(20,ip,isec)              ! Dist from endplate
          wxbywz = dc_wdir(1,i,il,isec)/dc_wdir(3,i,il,isec)
          wybywz = dc_wdir(2,i,il,isec)/dc_wdir(3,i,il,isec)
C Calculation of intersection of the wires with endplate (for +Z)
          dc_wpend(3) = (gbyb*spac-dc_wpmid(2,i,il,isec))/(gbyb+wybywz)
          dc_wpend(1) = dc_wpmid(1,i,il,isec)+wxbywz*dc_wpend(3)
          dc_wpend(2) = dc_wpmid(2,i,il,isec)+wybywz*dc_wpend(3)
C Calculation of intersection of the wires with endplate (for -Z)
          dc_wpend(6) =-(gbyb*spac-dc_wpmid(2,i,il,isec))/(gbyb-wybywz)
          dc_wpend(4) = dc_wpmid(1,i,il,isec)+wxbywz*dc_wpend(6)
          dc_wpend(5) = dc_wpmid(2,i,il,isec)+wybywz*dc_wpend(6)
C Calculate the sense wire half lengths (+z)
          dc_wlen(1,i,il,isec) =
     1       SQRT( (dc_wpmid(1,i,il,isec)-dc_wpend(1))**2
     2            +(dc_wpmid(2,i,il,isec)-dc_wpend(2))**2
     3            +(dc_wpmid(3,i,il,isec)-dc_wpend(3))**2 )
C Calculate the sense wire half lengths (-z)
          dc_wlen(2,i,il,isec) =
     1       SQRT( (dc_wpmid(1,i,il,isec)-dc_wpend(4))**2
     2            +(dc_wpmid(2,i,il,isec)-dc_wpend(5))**2
     3            +(dc_wpmid(3,i,il,isec)-dc_wpend(6))**2 )
C
        ENDDO                                            ! End DO over wires
      ENDDO                                              ! End DO over layers
C
C Calculate: Grid points {Xlow,Ylow,Xhigh,Yhigh} for each slab "id" in the
C            middle plane  "sc_grid(4,24,6,isec)"
C            and half slab length for each slab "sc_hleng(48,isec)".
      isc = 0
      DO ip = 1,6
        id = 1
        il = ip + ncc_min
        IF(ip.EQ.4) il = 3 + ncc_min
        IF(ip.GT.4) il = 4 + ncc_min
        IF(ip.EQ.4 .OR. ip.EQ.6) THEN
          nsl = sda_pln(19,il,isec) 
          sc_grid(1,id,ip,isec) = sda_pln(16,il,isec)
          sc_grid(2,id,ip,isec) = sda_pln(17,il,isec)
          sc_grid(3,id,ip,isec) =
     1     sda_pln(16,il,isec) +sda_pln(20,il,isec)*sda_pln(4,il,isec)
          sc_grid(4,id,ip,isec) =
     1     sda_pln(17,il,isec) +sda_pln(20,il,isec)*sda_pln(5,il,isec)
          DX = (sda_pln(14,il,isec)
     1        -sda_pln(16,il,isec))/sda_pln(19,il,isec)       
          DY = (sda_pln(15,il,isec)
     1        -sda_pln(17,il,isec))/sda_pln(19,il,isec)
        ELSE
          nsl = sda_pln(18,il,isec) 
          sc_grid(1,id,ip,isec) = sda_pln(12,il,isec)
          sc_grid(2,id,ip,isec) = sda_pln(13,il,isec)
          sc_grid(3,id,ip,isec) = sda_pln(12,il,isec)
     1      +sda_pln(20,il,isec)*sda_pln(4,il,isec)                    
          sc_grid(4,id,ip,isec) = sda_pln(13,il,isec)
     1      +sda_pln(20,il,isec)*sda_pln(5,il,isec)        
          IF(ip.EQ.3 .OR. ip.EQ.5) THEN        
            DX = (sda_pln(16,il,isec)
     1          - sda_pln(12,il,isec))/sda_pln(18,il,isec)
            DY = (sda_pln(17,il,isec) 
     1          - sda_pln(13,il,isec))/sda_pln(18,il,isec)
          ELSE
            DX = (sda_pln(14,il,isec) 
     1         - sda_pln(12,il,isec))/sda_pln(18,il,isec)
            DY = (sda_pln(15,il,isec)
     1          - sda_pln(13,il,isec))/sda_pln(18,il,isec)
          ENDIF
        ENDIF
        DO i = 1,nsl
          id = id + 1
          sc_grid(1,id,ip,isec) = sc_grid(1,1,ip,isec) +FLOAT(i)*DX
          sc_grid(2,id,ip,isec) = sc_grid(2,1,ip,isec) +FLOAT(i)*DY
          sc_grid(3,id,ip,isec) = sc_grid(1,id,ip,isec)
     1      +sda_pln(20,il,isec)*sda_pln(4,il,isec)
          sc_grid(4,id,ip,isec) = sc_grid(2,id,ip,isec)
     1      +sda_pln(20,il,isec)*sda_pln(5,il,isec)
          isc = isc + 1
C Store slab half length
          sc_hleng(isc,isec) = tan30*(sc_grid(2,id-1,ip,isec)
     1     + sc_grid(4,id-1,ip,isec)       
     2     +sc_grid(2,id,ip,isec) + sc_grid(4,id,ip,isec))/4. - PHT
        ENDDO                                     ! End of DO over nsl
      ENDDO                                       ! End of DO over ip
      ENDDO                                       ! End loop over Sectors
C
C
C Print Geometry Tables
C
      IF(lprnt(1).NE.0) THEN
**       isec = lprnt(1)
      print *,'------------------ginit ----------------------'
      isec=1
       WRITE(6,100)
       WRITE(6,111)
       WRITE(6,112)
       WRITE(6,111)
       WRITE(6,110)
       DO ip = 1,npln
        IF(ip.EQ.4 .OR. ip.EQ.16 .OR. ip.EQ.28 .OR. ip.EQ.40) WRITE(6,*)
        IF(ip.EQ.41 .OR. ip.EQ.45) WRITE(6,*)
        IF(ip.GT.3 .AND. ip.LT.40) THEN              
          WRITE(6,101) ip,DET(ip),(sda_pln(i,ip,isec),i=1,11)
        ELSE
          WRITE(6,114) ip,DET(ip),(sda_pln(i,ip,isec),i=1,11)
        ENDIF
       ENDDO
C
       WRITE(6,102)
       WRITE(6,113)
       WRITE(6,111)
       WRITE(6,107)
       DO ip = 1,npln
        IF(ip.EQ.4) THEN
          WRITE(6,*)
          WRITE(6,103)
        ENDIF
        IF(ip.EQ.16 .OR. ip.EQ.28 .OR. ip.EQ.40 .OR. ip.EQ.45)WRITE(6,*)
        IF(ip.EQ.40) WRITE(6,109)
        IF(ip.EQ.41) WRITE(6,*) 
        IF(ip.EQ.41) WRITE(6,106)
        IF(ip.EQ.45) WRITE(6,108)
        IF(ip.EQ.46) WRITE(6,115)
        IF(ip.GT.3 .AND. ip.LT.40) THEN
          WRITE(6,104) ip,DET(ip),(sda_pln(i,ip,isec),i=12,20)
        ELSE
          WRITE(6,105) ip,DET(ip),(sda_pln(i,ip,isec),i=12,20)
        ENDIF
       ENDDO
      ENDIF
C
 100  FORMAT(//' LEVEL=-1 (SDA Geometry, columns: 1 - 11)')
 110  FORMAT(
     1' ip',1X,'      Offset Center',1X,'    Normal Unit Vector'        
     2,1X,'     R      D     dX0  dY0   dZ0')
 111  FORMAT(' ---------------------------------------------------------
     2----------------------')
 113  FORMAT(' Col -->    12      13       14      15      16      17       
     1   18      19      20') 
 112  FORMAT(' Col -->   1     2   3     4      5      6          7        
     1 8    9     10    11') 
 101  FORMAT(I3,1X,A3,2F6.1,F4.1,2X,3F7.4,2X,2F7.2,3F6.2)
 114  FORMAT(I3,1X,A3,2F6.1,F4.1,2X,3F7.4,2X,2F7.2,F6.3,2F6.1)
 102  FORMAT(//' LEVEL=-1 (SDA Geometry, columns: 12 - 20)'/
     1' ----------------------------------------------------------------
     2---------------')
 103  FORMAT(
     1'   ',1X,'     PHBmin    DPHI       Nw   Nwmin   Nwmax   Nwcyl ASt
     2ereo    Dmax    Wcut')
 104  FORMAT(I3,1X,A3,2F8.4,1X,4F8.1,3F8.4)
 105  FORMAT(I3,1X,A3,2F8.2,1X,4F8.2,3F8.2)
 106  FORMAT('   ',1X,'     Xright  Yright    Xleft   Yleft    Xmid    Y
     1mid    Nsc1    Nsc2     Tsc')
 107  FORMAT(' ip',1X,'     Xright  Yright    Xleft   Yleft  Lplane
     1        Nsc    RadL     Tsc')
 108  FORMAT('   ',1X,'     Xright  Yright    Xleft   Yleft    Nlay
     1Nsc     Psi     Tsc     Tpb')
 115  FORMAT('   ',1X,'     Xright  Yright    Xleft   Yleft    Nlay    N
     1scX    NscZ     Tsc     Tpb')
 109  FORMAT('   ',1X,'     Xright  Yright    Xleft   Yleft  PHBmin  PHB
     1max    Nlay     Npt        ')
C
 201  FORMAT( ' Read "sdageom.bos" file for ',A8,I2,' created on  ',A24)
C
 999  CONTINUE
      RETURN
      END







E 1
