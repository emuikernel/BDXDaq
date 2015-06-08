h21841
s 00003/00003/00641
d D 1.2 03/04/17 16:52:23 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:02:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/sda_ginit2.F
e
s 00644/00000/00000
d D 1.1 01/11/19 19:01:59 boiarino 1 0
c date and time created 01/11/19 19:01:59 by boiarino
e
u
U
f e 0
t
T
I 1
      SUBROUTINE sda_ginit2
C----------------------------------------------------------------------
c
c recalculate geometry in according to the new zgcor's
c
C======================================================================
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
C just for sda_ginit2
#include "wcs.inc"
C
#include "clas_offsets.inc"
#include "sdakeys.inc"
#include "sdageom.inc"
C
C External functions
D 3
      INTEGER iucomp, mlink
E 3
I 3
      INTEGER iucomp, etnlink
E 3
      REAL VDOT
C
C Local variables
      INTEGER i,j,k, ip,il, ind1, ind2, ncol1, ncol2, ipt_gm, ipt_gw
      INTEGER  Ncol, Nrow, utime, ierr, isc, nsl, id, ic
      INTEGER nw,is,la3,ireg,la, isec, version, Nword, isec_cor        
      REAL dc_cutz(3), phipln(6)     
      REAL phiw, dlen, spac, wxbywz, wybywz, phi_pln
      REAL degrad, psi, DX, DY, PHT, tan30
      CHARACTER*3 DET(npln)
C
C Vertex plane
      REAL r_vt,d_vt, x0_vt(3), phi_vt_ort,phi_vt_min,phi_vt_max
C Start Counter plane
      REAL r_st(2),d_st(2), x0_st(3,2), phi_st_ort(2)
     1                                , phi_st_min(2),phi_st_max(2)
C Drift Chambers
      REAL dc_cut(3)
C Cerenkov Counter
      REAL r_cc,d_cc, x0_cc(3), phi_cc_ort,phib_cc_min, phib_cc_max
C Scintilator counters
      INTEGER nslab(2,4)
      REAL sc_xymid(6,4), phi_ort(4)
C Electromagnetic Counter plane
      REAL r_ec(2),d_ec(2),dd_ec(2), x0_ec(3,2), phi_ec_ort(2)
     1                                , phi_ec_min(2),phi_ec_max(2)
C
C=================== DATA BLOCK ========================================
C
C Start Counter
      DATA r_st       / 0.00,  0.00 /
      DATA d_st       /11.00, 20.026/
      DATA x0_st      /0.,0.,0., 0.,0.,0./
      DATA phi_st_ort /90.00, 60.00/
      DATA phi_st_min / 27.646,  7.00 /
      DATA phi_st_max /152.354, 27.646/
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
C---------- former DATA ------------------------------------
C
C Vertex plane used in fitting
      r_vt = 0.00
      d_vt = 0.05
      phi_vt_ort = 90.0
      phi_vt_min = 0.1
      phi_vt_max = 179.9
      do i=1,3
        x0_vt(i) = 0.0
      enddo
C
C Drift Chambers 
      phipln(1)=90.  ! angle between cyl and plane part of DC
      phipln(2)=90.  !         90 degree for SL 1 & 2
      phipln(3)=180. !        180 for the rest - just big value for dclramb()
      phipln(4)=180. !
      phipln(5)=180. !
      phipln(6)=180. !
C Distances from endplate to mid-coil plane for 3 regions (wire cutoff)
      dc_cut(1)=3.213
      dc_cut(2)=11.5199
      dc_cut(3)=14.97
C==================== End of DATA BLOCK ================================
C
      degrad = ACOS(-1.)/180.
      dc_cutz(1) = dc_cut(1)/COS(30.*degrad)
      dc_cutz(2) = dc_cut(2)/COS(30.*degrad)
      dc_cutz(3) = dc_cut(3)/COS(30.*degrad)
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
C Initialization of CLAS Geometry
C
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
C ****************************************************
C *** Geometry of Drift Chamber ***
C
C get Drift Chamber geometry DCGM bank
C
D 3
      ind1 = mlink(jw,'DCGM',isec)
E 3
I 3
      ind1 = etnlink(jw,'DCGM',isec)
E 3
      if(ind1.LE.0) then
        print *,'sda_ginit ERROR: DCGM bank does not exist - exit.'
        stop
      endif
      ncol1 = jw(ind1-5)
C
C Loop over the layers of the Drift Chamber
      DO il = 1,npl_dc
        ip   = il + nst_max                     ! Plane #
        ireg = (il-1)/12 + 1                    ! Region #
        is   = (il-1)/6 + 1                     ! Superlayer #
        la   = il - (is-1)*6                    ! Layer # in superlayer
        ipt_gm = ind1 + (il-1)*ncol1
C
C Store "dphi"
        sda_pln(13,ip,isec) = rjw(ipt_gm+DCGM_D_THETA)
C Phi of 1-st logical wire in Bogdan Center Cordinate System
        sda_pln(12,ip,isec) = rjw(ipt_gm+DCGM_THETA_START)
C Total No of logical wires
        sda_pln(14,ip,isec) = jw(ipt_gm+DCGM_MAX_WIRE)-jw(ipt_gm+DCGM_MIN_WIRE)+1
C First physical wire #
        sda_pln(15,ip,isec) = jw(ipt_gm+DCGM_MIN_WIRE)
C Last physical wire #
        sda_pln(16,ip,isec) = jw(ipt_gm+DCGM_MAX_WIRE)
C
C Last wire # in cyl. part of DC (see "sda_ginit.F")        
        nw = sda_pln(14,ip,isec)
C Starting phi angle of the plane part (90 degree for Reg.1, 180 for others)
        phi_pln = phipln(is)*degrad
cc      print *,'?????? ',is, phipln(is),'->',phi_pln
        DO i = 1,nw
          phiw = sda_pln(12,ip,isec) + (i-1)*sda_pln(13,ip,isec)
cc      print *,'??? ',isec,ip,i,'->', sda_pln(12,ip,isec),sda_pln(13,ip,isec),'->',phiw
          IF(phiw.LE.phi_pln) then
            sda_pln(17,ip,isec) = i ! for dclramb() etc
          ENDIF
        ENDDO
cc      print *,'? ',sda_pln(17,ip,isec)
C
C Store radius of each layer
        sda_pln(7,ip,isec) = rjw(ipt_gm+DCGM_R_CURVE)
C
C Store the offset coordinates for each layer
        sda_pln(1,ip,isec) = rjw(ipt_gm+DCGM_Z_CURVE)
        sda_pln(2,ip,isec) = rjw(ipt_gm+DCGM_X_CURVE)
        sda_pln(3,ip,isec) = rjw(ipt_gm+DCGM_Y_CURVE)
C
C Store ortvect to the plane part of the layer and distance from the origin
        IF(is.LT.3) THEN
          sda_pln(4,ip,isec) = rjw(ipt_gm+DCGM_Z_NORM)
          sda_pln(5,ip,isec) = rjw(ipt_gm+DCGM_X_NORM)
          sda_pln(6,ip,isec) = rjw(ipt_gm+DCGM_Y_NORM)
          sda_pln(8,ip,isec) = rjw(ipt_gm+DCGM_R_CURVE) ! ????????????????
        ELSE
          sda_pln(4,ip,isec) = 0.
          sda_pln(5,ip,isec) = 0.
          sda_pln(6,ip,isec) = 0.
          sda_pln(8,ip,isec) = 0.
        ENDIF
C
C Store the Stereo angle = 6 degrees (always)
        sda_pln(18,ip,isec) = rjw(ipt_gm+DCGM_STEREO)*degrad
C Store half distance between wires (ca maximum drift distance)
        sda_pln(19,ip,isec) = rjw(ipt_gm+DCGM_CELL_SIZE)
C???
        sda_pln(20,ip,isec) = dc_cutz(ireg)
c???        sda_pln(20,ip,isec) = jw(ipt_gm+DCGM_ ! do not exist in DCGM
      ENDDO                                  ! End DO over the layers of DC
C
C *** End Of Drift Chamber Geometry ***
C *************************************
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

      ENDDO                            ! End of loop over Sectors
C
C
C
      DO isec = 1,6

***********************************
********** new scheme *************

C
C get Drift Chamber geometry DCGW bank
C
D 3
        ind2 = mlink(jw,'DCGW',isec)
E 3
I 3
        ind2 = etnlink(jw,'DCGW',isec)
E 3
        if(ind2.LE.0) then
          print *,'sda_ginit ERROR: DCGW bank number ',isec,' does not exist - exit.'
          stop
        endif
        ncol2 = jw(ind2-5)
C
C Calculate: wire position "dc_wpmid(3,192,36,isec) at the middle plane,
C            wire direction (unit vector) "dc_wdir(3,192,36,isec)  and
C            half wire length "dc_wlen(2,192,36,isec) for +Z and -Z
      DO il = 1,npl_dc
        ireg = (il-1)/12 + 1                    ! Region #
        ip = il + nst_max                       ! Plane #
        phi_pln = 3.14159
        IF(il.LE.12) phi_pln=ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec))
        nw = sda_pln(14,ip,isec)
        DO i = 1,nw

          ipt_gw = ind2 + ((il-1)*192+i-1)*ncol2

          phiw = sda_pln(12,ip,isec) + (i-1)*sda_pln(13,ip,isec)

C Store position of the wire
          dc_wpmid(1,i,il,isec) = rjw(ipt_gw+DCGW_Z_MID)
          dc_wpmid(2,i,il,isec) = rjw(ipt_gw+DCGW_X_MID)
          dc_wpmid(3,i,il,isec) = rjw(ipt_gw+DCGW_Y_MID)
C Store direction of the wire
          dc_wdir(1,i,il,isec)  = rjw(ipt_gw+DCGW_Z_DIR)
          dc_wdir(2,i,il,isec)  = rjw(ipt_gw+DCGW_X_DIR)
          dc_wdir(3,i,il,isec)  = rjw(ipt_gw+DCGW_Y_DIR)
C
          spac = - sda_pln(20,ip,isec)              ! Dist from endplate
          wxbywz = dc_wdir(1,i,il,isec)/dc_wdir(3,i,il,isec)
          wybywz = dc_wdir(2,i,il,isec)/dc_wdir(3,i,il,isec)

C sense wire half lengths (+z)
          dc_wlen(1,i,il,isec) = rjw(ipt_gw+DCGW_W_LEN)    ! ILI NAOBOROT ???
C sense wire half lengths (-z)
          dc_wlen(2,i,il,isec) = rjw(ipt_gw+DCGW_W_LEN_HV) !

C
        ENDDO                                            ! End DO over wires
      ENDDO                                              ! End DO over layers


********** end of DC  *************
***********************************

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
      print *,'------------------ginit2----------------------'
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
 999  CONTINUE
      return
      end







E 1
