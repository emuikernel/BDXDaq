C
C Geometry of detector planes used in SDA and results of swimming
C                                 Bogdan Niczyporuk, October 1993
C                                          modified November 1996
C                
C                 Detector Planes
C sda_pln( 1,ip,isec) |
C sda_pln( 2,ip,isec) |--> Offset Center {X0,Y0,Z0}
C sda_pln( 3,ip,isec) |
C
C sda_pln( 4,ip,isec) |
C sda_pln( 5,ip,isec) |--> Unit Vector perpendicular to the plane "ip"
C sda_pln( 6,ip,isec) |    or (0,0,0) if plane is a cylinder
C
C sda_pln( 7,ip,isec) --> Radius of arc (DC and CC)
C sda_pln( 8,ip,isec) --> Distance to the plane (Reg.1 DC, SC and EC)
C 
C sda_pln( 9,ip,isec) --> Correction {dPHIBmin} to PHIBmin (i.e. 1st sense wire)
C sda_pln(10,ip,isec) --> Correction {dX0} to X0 (see Offset Center)
C sda_pln(11,ip,isec) --> Correction {dY0} to Y0 (see Offset Center) 
C
C                 Drift Chambers (for planes ip: 4 - 39)
C sda_pln(12,ip,isec) --> Phi angle of the 1-st logical sense wire in BCS
C sda_pln(13,ip,isec) --> DeltaPhi (between adjacent sense wires in cyl. part)
C sda_pln(14,ip,isec) --> Number of sense wires (logical)
C sda_pln(15,ip,isec) --> First physical sense wire 
C sda_pln(16,ip,isec) --> Last physical sense wire
C sda_pln(17,ip,isec) --> Last sense wire number in cylindrical part
C sda_pln(18,ip,isec) --> Stereo angle of a wire (6 degrees)
C sda_pln(19,ip,isec) --> Radiius of a cell (= DeltaPhi*R/2)
C sda_pln(20,ip,isec) --> Sense wire cut (from end of a wire to coil plane)
C
C                 Locations: 12 - 20 for all other sub-systems (exept DC)
C sda_pln(12,ip,isec) --> Xright |
C sda_pln(13,ip,isec) --> Yright | Boundaries of planes
C sda_pln(14,ip,isec) --> Xleft  | in the middle of a sector
C sda_pln(15,ip,isec) --> Yleft  |
C
C                       VTX(ip=1) ST(ip:2-3) CC(ip=40) SC(ip:41-44) EC(ip:45-46)
C sda_pln(16,ip,isec) --> 0.      LenPln     PHBmin    Xmid         Nlay[39,33]
C sda_pln(17,ip,isec) --> 0.      0.         PHBmax    Ymid         Nslab[36] 
C sda_pln(18,ip,isec) --> 0.      Nslab[1]   Nlay[2]   Nslab1       Psi=[.95,0.] 
C sda_pln(19,ip,isec) --> 0.      RadL[42.4] Npt[18]   Nslab2       Tslab[1.,1.5] 
C sda_pln(20,ip,isec) --> 0.      Tslab[0.3] 0.        Tslab[5.08]  Tpb[.23,.2]
C
C
C                 DC Wire position, Wire Direction & WireLen for layers: 1 - 36
C dc_wpmid(1,iw,il,isec) |
C dc_wpmid(2,iw,il,isec) |-> Wire position in the middle plane
C dc_wpmid(3,iw,il,isec) |
C
C dc_wdir(1,iw,il,isec)  |
C dc_wdir(2,iw,il,isec)  |-> Wire direction (unit vector)
C dc_wdir(3,iw,il,isec)  |
C
C dc_wlen(1,iw,il,isec)  --> Half wire length (+z or +y AMPL,Reg.1 opposit)
C dc_wlen(2,iw,il,isec)  --> Half wire length (-z or -y HV, ,Reg.1 opposit)
C
C                 Scintillator Slab Positions (SC)
C 
C sc_grid(1,igrid,ip,isec)  - xlow    | "ip" runs from 1 to 6 planes
C sc_grid(2,igrid,ip,isec)  - ylow    | Low means closer to target
C sc_grid(3,igrid,ip,isec)  - xhigh   | igrid runs  from 1 to nslab(ip)+1
C sc_grid(4,igrid,ip,isec)  - yhigh   |
C sc_hleng(isc,isec)        - Half Length of slab ["isc" runs from 1 to 57]
C
C *** Here is an example of "sc_grid" structure as it is for plane #6 ***
C
C       {x,y}igrid=3       {x,y}igrid=2       {x,y}igrid=1    <--- High
C         *------------------*------------------*
C         |                  |                  |
C         |    Slab #48      |   Slab #47       |
C         |                  |                  |
C         *------------------*------------------*
C       {x,y}igrid=3       {x,y}igrid=2       {x,y}igrid=1    <--- Low
C
C                 
C                 Results of swimming
C sda_trp(1,ip) |
C sda_trp(2,ip) |--> Track point {x,y,z} on a plane 
C sda_trp(3,ip) |
C
C sda_trp(4,ip) |
C sda_trp(5,ip) |--> Track direction at track point on a plane
C sda_trp(6,ip) |
C                 
C sda_trp(7,ip)  --> Track length from the target to a layer [cm]
C
C sda_tmat(5,5,ip)-> Transport matrix at track point on a plane
C
C                 Store the coordinates for a trajectory drawing
C nstep_xyz      --> Number of steps for current track
C xyz(3,istep)   --> {x,y,z} for a given step 
C
      INTEGER npln, nst_min, nst_max, 
     1              ndc_min, ncc_min, nsc_min, nec_min,
     2              ndc_max, ncc_max, nsc_max, nec_max,
     3              npl_st, npl_dc ,  npl_cc,  npl_sc,  npl_ec,
     4              mxstep ,nstep_xyz
      REAL sda_pln, sda_trp_obsolete, sda_tmat_obsolete, xyz, dc_wpmid
      REAL dc_wdir, dc_wlen, sc_grid, sc_hleng
C
      PARAMETER (npln =46)
      PARAMETER (npl_st =2, npl_dc =36, npl_cc =1, npl_sc =4, npl_ec =2)
C
      PARAMETER ( nst_min =2        , nst_max =nst_min+npl_st-1 )
      PARAMETER ( ndc_min =nst_max+1, ndc_max =ndc_min+npl_dc-1 )
      PARAMETER ( ncc_min =ndc_max+1, ncc_max =ncc_min+npl_cc-1 )
      PARAMETER ( nsc_min =ncc_max+1, nsc_max =nsc_min+npl_sc-1 )
      PARAMETER ( nec_min =nsc_max+1, nec_max =nec_min+npl_ec-1 )
C
      PARAMETER ( mxstep = 10000 )
C
      COMMON/sdageom/ sda_pln(20,npln,6) ,dc_wpmid(3,192,npl_dc,6)
     &               ,dc_wdir(3,192,npl_dc,6),dc_wlen(2,192,npl_dc,6)
     &               ,sc_grid(4,24,6,6), sc_hleng(57,6)       
     &               ,nstep_xyz, xyz(3,mxstep)      
     &               ,sda_trp_obsolete(7,npln), sda_tmat_obsolete(5,5,npln)
C
c      REAL dc_trp(7,npl_dc), cc_trp(7,npl_cc), sc_trp(7,npl_sc),
c     1     ec_trp(7,npl_ec)
c      EQUIVALENCE ( sda_trp(1,ndc_min),dc_trp(1,1) )
c      EQUIVALENCE ( sda_trp(1,ncc_min),cc_trp(1,1) )
c      EQUIVALENCE ( sda_trp(1,nsc_min),sc_trp(1,1) )
c      EQUIVALENCE ( sda_trp(1,nec_min),ec_trp(1,1) )
C
c      REAL dc_tmat(5,5,npl_dc)
c      EQUIVALENCE ( sda_tmat(1,1,ndc_min),dc_tmat(1,1,1) )
C
      save /sdageom/
C
