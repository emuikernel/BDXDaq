C======================================================================
      SUBROUTINE sim_ecdigi
C----------------------------------------------------------------------
C-
C-    Purpose and Methods : Digitalization of hits recorded in EC planes for
C-                          photons and electrons (steering routine).
C-                          Actual Digitalization is donne in "elss_access".
C-
C-    Inputs  : swiming results
C-    Outputs : "sdadigi" include file
C-    Controls
C-
C-    Library belongs: libsim.a
C-
C-    Calls:  ranini, elss_init, elss_shower
C-
C-    Created     15-AUG-1995   Bogdan Niczyporuk
C-
C-
C-    Called by sim_main
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
c
**#include "sdacalib.inc"
      real vflt
      parameter (vflt=29.97925)
C
C The begin of ELSS common
      INTEGER iqi,iri, nevreq,nevsim,nevsav
      REAL wti,ei,tofi,xi,yi,zi,ui,vi,wi
      COMMON/elss_parini/iqi,iri,wti,ei,tofi,xi,yi,zi,ui,vi,wi,
     1                   nevreq,nevsim,nevsav
C The end of ELSS commons
C
C Local variables
      INTEGER it, isec, iacc, ifirst, isl
      INTEGER nsl, nreg
      PARAMETER (nreg = 88, nsl = 36)
      CHARACTER*4 mename(10)
      REAL phi,SINth,d,beta,dd,sml,tpsi,spsi,rat,trL,tan20,x,y,x0(3) 
C
      DATA mename/'PB  ','PS  ','FE  ','AIR ',6*'    '/
      DATA ifirst/1/
C dd = 15*1.23  is the 1-st SC plane of Backward EC
      DATA dd /18.45/
C
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        tan20  = 0.36397023
C Initialize RN needed for ELSS
        CALL RANINI(-123456787)
C Initialize the ELSS [ Energy in MeV]
C                        MaxE    CutP  CutE Nmed  MedArray
        CALL elss_init(6000.0,    0.2,  1.0,   4,   mename)
c
        nevreq = 1
        nevsim = 0
        nevsav = 0
      ENDIF
C
C Shower simulation for electron and gammas only (starts at the SC-plane)
C
      DO 1000 it = 1,ntrack
        iacc = 0
        itrin_acc(5,it) = iacc
        isec_sim = itrin_sec(it)
        isec     = isec_sim
C
C Check particle reached forward plane of SC (temporary stuff)
        IF(sc_trps(1,1,it).GT.999..OR.ec_trps(1,1,it).GT.999.) GOTO 1000
C Fill the rest of COMMON/ELSS_PARINI/
        iqi  = evin(9,it)
        iri  = 2
        wti  = 1.
        ei   = 1000.*evin(7,it)
        beta = vflt*evin(7,it)/SQRT(evin(7,it)**2 + evin(8,it)**2) 
        tofi = ec_trps(7,1,it)/beta
C
C Initial particle position {XI,YI,ZI} and direction (UI,VI,WI} are defined in 
C the local coordinate system {i,j,k}, where k is perpendicular to the EC-plane
C and j is parallel to Yd (or Zs) -axis. 
C The origin of the {i,j,k} system is at the SC-plane, where the normal vector
C to the EC-plane points.
C
C Particle position in the local system
        d = sda_pln(8,nsc_min,isec)
        x0(1) = sc_trps(1,1,it) - sda_pln(1,nec_min,isec)
        x0(2) = sc_trps(2,1,it) - sda_pln(2,nec_min,isec)
        x0(3) = sc_trps(3,1,it) - sda_pln(3,nec_min,isec)
        xi  = SQRT( (x0(1) - sda_pln(4,nsc_min,isec)*d)**2
     1             +(x0(2) - sda_pln(5,nsc_min,isec)*d)**2 )
        IF(x0(1).GT.sda_pln(4,nsc_min,isec)*d) xi = -xi
        yi  = x0(3)
        zi  = 0.
C Particle direction in the local system
        phi = ATAN2(sc_trps(5,1,it),sc_trps(4,1,it) )
     1       -ATAN2(sda_pln(5,nsc_min,isec),sda_pln(4,nsc_min,isec) )          
        SINth = SIN( ACOS(sc_trps(6,1,it)) )
        ui  = SINth*SIN(phi)
        vi  = sc_trps(6,1,it)
        wi  = SINth*COS(phi)
C
C Check acceptance for electron in forward EC
        trL = dd/wi
        x = xi + trL*ui
        y = yi + trL*vi
C Check boundaries of EC triangle at distance "d+dd" from origine
        tpsi = TAN(sda_pln(18,nec_min,isec)/2.)
      spsi = SIN(sda_pln(18,nec_min,isec)/2.)
        sml  = (sda_pln(8,nec_min,isec) + dd)*tan20
        IF(ABS(x).GE.sml) GO TO 1000
        rat = (sml + x)*tpsi
        IF(ABS(y).GE.rat) GO TO 1000
        IF( ABS(x).LE.(sml-10.) .AND. ABS(y).LE.(rat-10.) ) iacc = 1        
        itrin_acc(5,it) = iacc
C
C Shower simulation for electrons & gammas
        IF(evin(8,it).LE.-0.0001 .OR. evin(8,it).GE.0.00059) GOTO 1000

C Serguei      CALL elss_shower

C Serguei
C U - layer of EC
      IF(ABS(x).GE.190.) RETURN
      rat = (190.+x)*tpsi
      IF(ABS(y).GT.rat) RETURN
      isl = 37. - 18.*(1.-x/190.)
      IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
      ec_digi(1,1,1,isec) = isl
c      print *,'U=',isl
C V - layer of EC
      IF(ABS(x).GE.190.) RETURN
      rat = (190.+x)*tpsi
      IF(ABS(y).GT.rat) RETURN
      isl = 37. - 18.*spsi*(190. + x + y/tpsi)/190.
      IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
      ec_digi(1,1,2,isec) = isl
c      print *,'V=',isl
C W - layer of EC
      IF(ABS(x).GE.190.) RETURN
      rat = (190.+x)*tpsi
      IF(ABS(y).GT.rat) RETURN
      isl = 37. - 18.*spsi*(190. + x - y/tpsi)/190.
      IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
      ec_digi(1,1,3,isec) = isl
c      print *,'W=',isl
C found all 3 layers
      ec_ndig(1,isec) = 1
C Serguei


 1000 CONTINUE 
C
      RETURN
      END
C======================================================================
      SUBROUTINE elss_access(iflag)
C----------------------------------------------------------------------
C-
C-    The routine "elss_access" is called by ELSS
C-
C-    Created              6-JUN-1993   M. Kossov
C-    Interfaced to SDA   19-AUG-1994   B. Niczyporuk
C-    Modified & Debugged 28-AUG-1996   B. Niczyporuk
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdaevgen.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
c
**#include "sdacalib.inc"
      real cal_ec(6,36,6,6), Ethr_ec
C
C
C The begin of ELSS commons
      INTEGER nmed,name,med
      REAL rho,x0
      COMMON/elss_medium/nmed,name(10),rho(10),x0(10),med(1000)
C
      REAL elosi,elosc,elost,eold,enew
      COMMON/elss_energy/elosi,elosc,elost,eold,enew
C
      INTEGER irold,irnew,icros
      REAL stepr,stepu,stepa,stepc
      COMMON/elss_stpsiz/stepr,stepu,stepa,stepc,irold,irnew,icros
C
      INTEGER iqi,iri, nevreq,nevsim,nevsav
      REAL wti,ei,tofi,xi,yi,zi,ui,vi,wi
      COMMON/elss_parini/iqi,iri,wti,ei,tofi,xi,yi,zi,ui,vi,wi,
     1                   nevreq,nevsim,nevsav
C
      INTEGER iq,im,ir,idisc
      REAL wt,e,tof,x,y,z,u,v,w
      COMMON/elss_params/iq,im,ir,wt,e,tof,x,y,z,u,v,w,idisc
C
      REAL ae,ap,elm,te
      COMMON/elss_cutoff/ae,ap,elm,te
C
      INTEGER ilp,icr,icp,idp,imt
      COMMON/elss_syslun/ilp,icr,icp,idp,imt
C The end of ELSS commons
C
C Subroutine parameters
      INTEGER iflag
C
C Local parameters
C
C nsl    -No SC slabs in EC plane
C nreg   -No of layers (regions) along Z in SC & EC
      INTEGER nsl, nreg
      PARAMETER (nreg = 88, nsl = 36)
C
C Local variables
C
      INTEGER i,isl,ih,nh,ip, imin,imax, iedep,itdc, ifold,ifnew,isec             
      INTEGER npart(nsl,nreg)
      REAL rat, spsi, tpsi, rspsi, rcpsi, atl, mflt, tmin, tan20, adc
      REAL edep, zb(nreg), energy(nsl,nreg), tfull(nsl,nreg), sml(nreg)
C               
      INTEGER ifirst
      DATA ifirst/1/
C
      if(ifirst.EQ.1) then
        ifirst = 0

c get EC calibration
C 0,0,0 - MAP values, 1,1,1-defauls values
        call ecinitcalib(ltrig(1),0,0,0)
        call ecgetcalib(ltrig(1),Ethr_ec,cal_ec)
      endif
C
      GO TO(100,200,300,400,500,600),iflag
C
C*
C* Is called after the preparation routines and prior to simulation of the
C* first shower. May be used for any purposes.
C*
C       Here we initialize some constants and define the geometry.
C
 100  CONTINUE
C
C Initialize the media # for each region (along the Z-axis)
C
C im=0  -Vacuum (Vac)
C im=1  -Pb
C im=2  -plastic (PS)
C im=3  -Fe
C im=4  -Air
C
C Thick ->       0.005   5.08   5.260   0.156  7.620   0.156  1.00
C         1 Vac,  2 Pb,  3 PS,  4 Air,  5 Fe,  6 Air,  7 Fe,  8 PS,
C Thick ->      0.23  1.0
C         9-84 ( Pb,  Ps ),
C Thick ->0.156    7.620   0.156
C         85 Fe,  86 Air,  87 Fe,  88 Vac
C
      med(1) = 0
      med(2) = 1
      med(3) = 2
      med(4) = 4
      med(5) = 3
      med(6) = 4
      med(7) = 3
C
      DO 110 i = 8,nreg-4
	IF(MOD(i,2).EQ.0) med(i) = 2
	IF(MOD(i,2).EQ.1) med(i) = 1
 110  CONTINUE
C
      med(nreg-3) = 3
      med(nreg-2) = 4
      med(nreg-1) = 3
      med(nreg)   = 0
C
C Define the Boundaries along the Z-axis
C
      isec = 1       ! The constants used here are independent of Sector #
      zb(1)=0.
      DO 120 i = 2,nreg
	IF(MOD(i,2).EQ.1 .AND. i.GT.7 .AND. i.LT.85)
     1         zb(i) = zb(i-1) + sda_pln(20,nec_min,isec)    ! EC  Pb thickness
	IF(MOD(i,2).EQ.0 .AND. i.GT.7 .AND. i.LT.85)
     1         zb(i) = zb(i-1) + sda_pln(19,nec_min,isec)    ! EC  PS thickness
	IF(i.EQ.2)  zb(i) = zb(i-1) +0.005                   ! SC1 Pb thickness
	IF(i.EQ.3)  zb(i) = zb(i-1) +sda_pln(20,nsc_min,isec)! SC1 PS thickness
	IF(i.EQ.4)  zb(i) = zb(i-1) +5.26                    ! Air thickness
	IF(i.EQ.5)  zb(i) = zb(i-1) +0.156                   ! EC  Fe thickness
	IF(i.EQ.6)  zb(i) = zb(i-1) +7.62                    ! EC  Air thickness
	IF(i.EQ.7)  zb(i) = zb(i-1) +0.156                   ! EC  Fe thickness
	IF(i.EQ.85) zb(i) = zb(i-1) +0.156                   ! EC  Fe thickness
	IF(i.EQ.86) zb(i) = zb(i-1) +7.62                    ! EC  Air thickness
	IF(i.EQ.87) zb(i) = zb(i-1) +0.156                   ! EC  Fe thickness
	IF(i.EQ.88) zb(i) = zb(i-1)
 120  CONTINUE
C
      tpsi = TAN(sda_pln(18,nec_min,isec)/2.)                ! TAN(27.11deg)
      spsi = SIN(sda_pln(18,nec_min,isec)/2.)                ! SIN(27.11deg)
      rspsi= 1./SIN(sda_pln(18,nec_min,isec)/2.)             ! 1./SIN(27.11deg)
      rcpsi= 1./COS(sda_pln(18,nec_min,isec)/2.)             ! 1./COS(27.11deg)
      tan20= 0.36397023                                      ! TAN(20 deg)
C
      RETURN
C
C*
C* Is called prior to simulation of each shower. May be used for redefining:
C* IRI,EI,TOFI,XI,YI,ZI,UI,VI,WI or initializing user-defined arrays,constants.
C*
 200  CONTINUE
      isec = isec_sim       ! Sector # for current track # (see "sim_ecdigi")
      DO 210 i = 1,nreg
      DO 210 isl = 1,nsl
	 npart(isl,i) = 0
	energy(isl,i) = 0.
	 tfull(isl,i) = 0.
 210  CONTINUE
C
      DO 220 i = 8,84,2
	sml(i) = (sda_pln(8,nsc_min,isec)+(zb(i)+zb(i-1))/2.)*tan20
c      print *,'ir=',i,'  sml=',sml(i)
 220  CONTINUE
C	  
      RETURN
C
C*
C* Is called after the program has decided to transport the current particle
C* by distance STEPR. Here the user may define STEPU, or discard the particle
C* by setting: IDISC=1, or IDISC=-1, or determine IRNEW if it turns out that
C* STEPU.LT.STEPR, ... , or take no action at all (for certain problems).
C*
 300  CONTINUE
C
C Decision for the shower development inside an absorber
      IF(im.GT.0) GO TO 305
C If it is vacuum - scip any interactions
      idisc = 1
      RETURN
 305  CONTINUE
C
C Maximum STEP(U) definition for forward or backward motion
      IF(w.EQ.0.) RETURN
      IF(w.GT.0.) stepu = (zb(ir  )-z)/w
      IF(w.LT.0.) stepu = (zb(ir-1)-z)/w
      IF(stepu.LE.0.) GO TO 310
C
C If the step is inside the layer - do nothing
      IF(stepu.GT.stepr) RETURN
C
C Oterwise redefine the layer number (increment or decrement)
      IF(w.GT.0.) irnew = ir+1
      IF(w.LT.0.) irnew = ir-1
C
C If particle comes out of EC (forward or backward) - kill particle
      IF(irnew.EQ.   1) idisc = -1
      IF(irnew.EQ.nreg) idisc = -1
      RETURN
C
 310  CONTINUE
C
C Particle is in different layer now - correct layer # and try once more
      if(w.GT.0.) ir=ir+1
      if(w.LT.0.) ir=ir-1
      GO TO 300
C
C*
C* Is called after the complition of transport over distance STEPA and simula-
C* tion of interaction on it (STEPA). This call shell be used for book-keeping,
C* histogramming of quantities of interest.
C*
 400  CONTINUE
      isec = isec_sim       ! Sector # for current track # (see "sim_ecdigi")
C
C Fill energy deposition only for scintillator
C
      IF(im.NE.2) RETURN
C
C Scintillator counter layer 
C
      IF(ir.EQ.3) THEN
C
C Shower counter layers
C
      ELSEIF(MOD(ir,6).EQ.2) THEN
C U - layer of EC
	IF(ABS(x).GE.sml(ir)) RETURN
	rat = (sml(ir)+x)*tpsi
	IF(ABS(y).GT.rat) RETURN
	isl = 37. - 18.*(1.-x/sml(ir))
	IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
	atl = rat - y
	ip = 1
	IF(ir.GT.36) ip = 4
	energy(isl,ir) = energy(isl,ir)
     1                  +elost*exp(-atl/cal_ec(5,isl,ip,isec))       
	IF(elost.GT.0.) THEN
	  npart(isl,ir) = npart(isl,ir) + 1
	  tfull(isl,ir) = tfull(isl,ir) + tof +atl/cal_ec(4,isl,ip,isec)
	ENDIF
C
      ELSEIF(MOD(ir,6).EQ.4) THEN
C V - layer of EC
	IF(ABS(x).GE.sml(ir)) RETURN
	rat = (sml(ir)+x)*tpsi
	IF(ABS(y).GT.rat) RETURN
	isl = 37. - 18.*spsi*(sml(ir) + x + y/tpsi)/sml(ir)
	IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
	atl = (sml(ir)-x)*rcpsi
	ip = 2
	IF(ir.GT.36) ip = 5
	energy(isl,ir) = energy(isl,ir)
     1                  +elost*exp(-atl/cal_ec(5,isl,ip,isec))
	IF(elost.GT.0.) THEN
	  npart(isl,ir) = npart(isl,ir) + 1
	  tfull(isl,ir) = tfull(isl,ir) + tof +atl/cal_ec(4,isl,ip,isec)        
	ENDIF
C
      ELSEIF(MOD(ir,6).EQ.0) THEN
C W - layer of EC
	IF(ABS(x).GE.sml(ir)) RETURN
	rat = (sml(ir)+x)*tpsi
	IF(ABS(y).GT.rat) RETURN
	isl = 37. - 18.*spsi*(sml(ir) + x - y/tpsi)/sml(ir)
	IF(isl.LT.1 .OR. isl.GT.nsl) RETURN
	atl = 0.5*(rat+y)*rspsi
	ip = 3
	IF(ir.GT.36) ip = 6
	energy(isl,ir) = energy(isl,ir)
     1                  +elost*exp(-atl/cal_ec(5,isl,ip,isec))
	IF(elost.GT.0.) THEN
	  npart(isl,ir) = npart(isl,ir) + 1
	  tfull(isl,ir) = tfull(isl,ir) + tof +atl/cal_ec(4,isl,ip,isec)
	ENDIF
      ENDIF
C
      RETURN
C
C*
C* Is called after the simulation of each shower. May be used for savings.
C*
 500  CONTINUE
C
C Loop over 36 slabs in a layer for EC
C
      isec = isec_sim       ! Sector # for current track # (see "sim_ecdigi")
C
C Loop over six planes {u1,v1,w1,u2,v2,w2}
C u1
      DO ip = 1,6
        IF(ip.EQ.1) THEN
          imin = 8
          imax = 32
        ENDIF
C v1
        IF(ip.EQ.2) THEN
          imin = 10
          imax = 34
        ENDIF
C w1
        IF(ip.EQ.3) THEN
          imin = 12
          imax = 36
        ENDIF
C u2
        IF(ip.EQ.4) THEN
          imin = 38
          imax = 80
        ENDIF
C v2
        IF(ip.EQ.5) THEN
          imin = 40
          imax = 82
        ENDIF
C w2
        IF(ip.EQ.6) THEN
          imin = 42
          imax = 84
        ENDIF
C
C Loop over 36 slabs in a plane
	DO isl = 1,nsl
          edep = 0.
          tmin = 99.
	  DO i = imin,imax,6
	    edep = edep + energy(isl,i)
	    IF(npart(isl,i).GT.0) THEN
	      mflt = tfull(isl,i)/npart(isl,i)
	      IF(mflt.LT.tmin) tmin = mflt
	    ENDIF
 	  ENDDO      ! End of loop over Z-layers (regions)
C
          IF(edep.GT.Ethr_ec .AND. tmin.LT.99.) THEN
            adc   = cal_ec(3,isl,ip,isec)*edep + cal_ec(6,isl,ip,isec)
            iedep = NINT(adc)                     
 	    ifnew = NINT(cal_ec(2,isl,ip,isec)*tmin)
 	    itdc  = NINT(cal_ec(2,isl,ip,isec)
     1                   *(tmin + cal_ec(1,isl,ip,isec) - Ttrig))
 	    nh = ec_ndig(ip,isec)
 	    IF(nh.GT.0) THEN
 	      DO ih = 1,nh
 	        IF(ec_digi(1,ih,ip,isec).EQ.isl) THEN
 	          ifold = ec_digi(2,ih,ip,isec) + NINT(
     1            cal_ec(2,isl,ip,isec)*(Ttrig - cal_ec(1,isl,ip,isec)))
 	          IF(ifnew.LT.ifold) ec_digi(2,ih,ip,isec) = itdc
 	          ec_digi(3,ih,ip,isec) = ec_digi(3,ih,ip,isec) +iedep
 	          GO TO 510
 	        ENDIF
 	      ENDDO
 	    ENDIF
 	    ec_ndig(ip,isec) = ec_ndig(ip,isec) + 1
 	    ih = ec_ndig(ip,isec)
 	    ec_digi(1,ih,ip,isec) = isl
c      print *,'ec_digi(',ip,')=',ec_digi(1,ih,ip,isec)
 	    ec_digi(2,ih,ip,isec) = itdc
 	    ec_digi(3,ih,ip,isec) = iedep
          ENDIF
 510      CONTINUE
C
        ENDDO         ! End of loop over slabs
      ENDDO           ! End of loop over planes
      RETURN
C
C*
C* Is called after the simulation of all showers.
C*
 600  CONTINUE
      RETURN
C
      END
                           
