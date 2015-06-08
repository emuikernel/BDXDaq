C======================================================================
      SUBROUTINE ana_prfit(iw,rw,ifail)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Performs track fitting using the stored wire
C-                         positions and momentum from the pattern
C-                         recognition templates.
C-
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : ana_trfit, dcrotsd, sda_vrotsd, sda_ptrk,
C-                        dcpdst
C-
C-   Created   JUN-10-1992  Bogdan Niczyporuk
C-   Modified  JUN-11-1992  Billy Leung
C-
C-   Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#define CED 1
C
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdadraw.inc"
#include "sdaanal.inc"
C
C
C Subroutine variable
      INTEGER iw(1000),ifail
      REAL rw(1000)
C
C External function
      INTEGER iucomp
C
C Lolal variables
      INTEGER nlay, ndc, isec, Level
      PARAMETER (nlay = 6, ndc = 36)
C
      INTEGER i,il,ip,is,ist,isg,ilnk,ilmin,la,iw_,iw9,iw15,iw27,n_trk
      INTEGER k, it, ifirst, inter, if_draw, nsuper, iflg, iwm,ilm 
C
      REAL Dang, fit_iter
      REAL dx21, dx32, dy21, dy32, defl
      REAL svin(9),svout(9), chifit, sphi, STmin, phi, Bsign
      REAL svinn(9)
      REAL d, dd, x,y, u,v,w, x0(3), spsi, tpsi, sml, rat, tan20
      real x00(npl_sc)
#ifdef CED
      integer ncol_hbla, ncol_dpl, ncol_hbtr
      parameter (ncol_hbla=16, ncol_dpl=8, ncol_hbtr=9)
      integer ETNCREATE, ind0, ind1, ind2, n_dclay, n_planes, itrk
      integer itmp, ipl
      integer ced_trk(6), ced_lnk(ntrmx,6), ced_itr(ntrmx,6)
      real tmp, vertex(10)

      real nu,Q2,W2,E0,E1,E2,costheta1,costheta2,mp
      real theta2,tantheta2,sintheta1,raddeg
      integer sec1,sec2
      data mp/0.938/, E0/4.0451/
#endif
C
      DATA ifirst /1/
      DATA dd /18.45/      ! dd = 15*1.23 (1-st SC plane of Backward EC)
      DATA nsuper /6/
C
C
C SB taken from pattern recognition now
***      Bsign = zmagn(2)/ABS(zmagn(2))
C
      IF(ifirst.EQ.1) THEN
        ifirst = 0
        inter  = 0
        IF(iucomp(4HINT ,lmode,1).NE.0) inter = 1
        tan20  = 0.36397023
        raddeg = 180./ACOS(-1.)
      ENDIF
C
C Save the original simulation parameters and replace with the new ones
C
      fit_iter = ztfit(1)
        ztfit(1) = fit_iter - 1.
      STmin    = zstep(1)
        zstep(1) = STmin
      Dang     = zstep(3)
        zstep(3) = 2.*Dang
C
C Loop over linked track candidates
C
      Level = 2
      n_trk = 0
      IF(ntr_link.EQ.0) GO TO 999
C
C Set flag "if_draw = 1" if interactive job and PR fitting selected
      if_draw = 0
      IF(lanal(1).LT.4 .AND. inter.EQ.1) if_draw = inter
C
C
      ifail = 0
      DO 100 ilnk = 1,ntr_link
        itr_ecid(1,ilnk) = 0
        itr_ecid(2,ilnk) = 0
        itr_ecid(3,ilnk) = 0
C
        IF(itr_level(ilnk).NE.1) GO TO 100
        isec = itr_sect(ilnk)
C
C Store wire numbers for fitting without drift times
        DO is = 1,nsuper
          ilmin = (is-1)*nlay
          isg = lnk_segm(is,ilnk)
          k = 1
          DO la = 1,nlay
            il = la + ilmin
            ip = il + nst_max
            trk(1,il,ilnk)  = segm(k  ,isg,is,isec)      ! Wire number
            trk(2,il,ilnk)  = segm(k+1,isg,is,isec)      ! Raw TDC [cnts]
            trk(3,il,ilnk)  = 0.                         ! Tdrift [ns]
            trk(4,il,ilnk)  = 0.                         ! Dmeas. [cm]
            trk(5,il,ilnk)  = sda_pln(19,ip,isec)/1.732  ! SIGm [cm]
            trk(6,il,ilnk)  = 0.                         ! Velocity beta [c]
            trk(7,il,ilnk)  = 0.                         ! Dfit [cm]
            trk(8,il,ilnk)  = 0.                         ! TrkLen [cm]
            trk(9,il,ilnk)  = 0.                         ! WireLen [cm]
            trk(10,il,ilnk) = 0.                         ! Alpha [rad]
            trk(11,il,ilnk) = 0.                         ! Analysis Level & Sect
            trk(12,il,ilnk) = 0.                         ! Status OK
            IF(trk(1,il,ilnk).LE.0.) trk(12,il,ilnk) = 1.! Status notOK
            k = k + 2
          ENDDO
        ENDDO
C
C Create initial parameters for fitting
C
        ip = ztfit(5)                                 ! Key "TFIT" card
C One may start the fitting at Plane#1 (target  plane) or Plane#4 (DC layer#1)
        IF(ip.GE.4) THEN
          il = ip - 3
          ilm= il + 11
          iw_ = ABS(trk(1,il ,ilnk))
          iwm= ABS(trk(1,ilm,ilnk))

c          sphi = ATAN2(dc_wpmid(2,iwm,ilm,isec)-dc_wpmid(2,iw_,il,isec),          
c     1                 dc_wpmid(1,iwm,ilm,isec)-dc_wpmid(1,iw_,il,isec))           
c          svin(1) = dc_wpmid(1,iw_,1,isec) - svin(4)  ! x
c          svin(2) = dc_wpmid(2,iw_,1,isec) - svin(5)  ! y
c          svin(3) = 0.                                ! z
c          svin(4) = COS(sphi)                         ! Cx
c          svin(5) = SIN(sphi)                         ! Cy
c          svin(6) = 0.                                ! Cz


*          print *,'OLDVECT=',svin(1),svin(2),svin(3),svin(4),svin(5),svin(6)
*          print *,'NEWVECT=',lnk_vect(1,ilnk),lnk_vect(2,ilnk),lnk_vect(3,ilnk)
*     &                  ,lnk_vect(4,ilnk),lnk_vect(5,ilnk),lnk_vect(6,ilnk)

          svin(1) = lnk_vect(1,ilnk)
          svin(2) = lnk_vect(2,ilnk)
          svin(3) = lnk_vect(3,ilnk)
          svin(4) = lnk_vect(4,ilnk)
          svin(5) = lnk_vect(5,ilnk)
          svin(6) = lnk_vect(6,ilnk)


        ELSE
          il = 1
          svin(1) = ztarg(3)                          ! x
          svin(2) = 0.                                ! y
          svin(3) = 0.                                ! z
          iw_ = ABS(trk(1,il,ilnk))
          sphi = ATAN2(dc_wpmid(2,iw_,il,isec),dc_wpmid(1,iw_,il,isec)
     1          -svin(1))
          svin(4) = COS(sphi)                         ! Cx
          svin(5) = SIN(sphi)                         ! Cy
          svin(6) = 0.                                ! Cz
        ENDIF
C
        svin(7) = lnk_vect(7,ilnk)                    ! Initial momentum
        IF(svin(7).LT.0.02) svin(7) = 0.02
        svin(8) = 0.0                                 ! Velocity of a particle
C
C Guess charge of a particle from track candidate sagitta
c        IF(mgtyp.EQ.5 .OR. ABS(zmagn(2)).LT.1.) THEN
c          svin(9) = 1.
c        ELSE
c          iw9  = ABS(trk(1,9,ilnk))
c          iw15 = ABS(trk(1,15,ilnk))
c          iw27 = ABS(trk(1,27,ilnk))
c          dx21 = dc_wpmid(1,iw15,15,isec) - dc_wpmid(1,iw9,  9,isec)
c          dy21 = dc_wpmid(2,iw15,15,isec) - dc_wpmid(2,iw9,  9,isec)
c          dx32 = dc_wpmid(1,iw27,27,isec) - dc_wpmid(1,iw15,15,isec)
c          dy32 = dc_wpmid(2,iw27,27,isec) - dc_wpmid(2,iw15,15,isec)
c          defl = ATAN2(dy32,dx32) - ATAN2(dy21,dx21)
c          svin(9) = -1.
c          IF(defl.GE.0.) svin(9) = 1.
c          svin(9) = svin(9)*Bsign                     ! Charge of a particle
c        ENDIF

        svin(9) = lnk_vect(12,ilnk)


ccboyc      print *,'svin(link)=',svin

C
C Fit to wire positon only (drift times not used)
C
*      print *,'svin0=',svin
        IF(ztfit(1).GT.0.)
*     &    CALL ana_trfit(iw,isec,trk(1,1,ilnk),Level,svin,chifit)
     & call dctrfit(iw,isec,Level,sda_trp,trk(1,1,ilnk),svin,chifit)

c      print *,'after trfit:'
c      print *,'svin1=',svin
c      print *,'ana_prfit 2: ',chifit,zcut(4)
C
C Chi-square (for Fit to wire position) cut
        IF(chifit.GT.zcut(4)) THEN
*          print *,'chi2 too big : ',chifit,' > ',zcut(4)
          GOTO 100
        ENDIF
        n_trk = n_trk + 1
c      print *,'ana_prfit 3: get track #',n_trk
C
C Swimming to the target plane#1
        IF(ztfit(5).EQ.4.) THEN

          DO i = 1,6
            svinn(i) = dc_trp(i,1)
          ENDDO
          DO i = 7,9
            svinn(i) = svin(i)
          ENDDO
*          CALL sda_swim_vt(iw, isec, svinn, step, level, if_draw, svout, vertex)
          CALL dcswimvt(iw, sda_pln(1,1,isec), isec, svinn, step, level, if_draw,
     &                  svout, vertex, sda_trp, nstep_xyz, xyz)

C If photoproduction get Track Point for Start Counter plane#2 or 3 (no PTF)
          IF(zbeam(1).EQ.0.) THEN
**            CALL sda_swim(isec,      1,      3,     0,      0, svout)
            CALL dcswim(sda_pln(1,1,isec),1,3,0,0,svout,
     &              sda_trp,sda_tmat,nstep_xyz,xyz)
          ENDIF
        ENDIF
*      print *,'svout1=',svout(7)
C
C Store the results of PR fit, which was not rejected from the above cuts
        itr_level(ilnk) = Level
        itr_sect(ilnk)  = isec
        DO il = 1,ndc
          ip = il + nst_max
          iw_ = ABS(trk(1,il,ilnk))
          trk( 8,il,ilnk) = dc_trp(7,il)                               ! Tlen
          IF(il.LT.13) THEN
            trk( 9,il,ilnk) = dc_wlen(2,iw_,il,isec) - dc_trp(3,il)     ! Wlen
          ELSE
            trk( 9,il,ilnk) = dc_wlen(1,iw_,il,isec) - dc_trp(3,il)     ! Wlen
          ENDIF
          phi = ATAN2(dc_trp(5,il),dc_trp(4,il))
          IF(iw_ .GT. sda_pln(17,ip,isec)) THEN
            trk(10,il,ilnk) =
     1          phi - ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec))     ! alfa
          ELSE
            trk(10,il,ilnk) = phi - sda_pln(12,ip,isec)-(iw_-1)
     1                       *sda_pln(13,ip,isec)                      ! alfa        
          ENDIF
          trk(11,il,ilnk)  = 10*isec + Level                           ! sec/L
        ENDDO
        DO i = 1,9
          trkp(i,1,ilnk) = svin(i)
        ENDDO
*       print *,'prfit2: ',  trkp( 7,1,ilnk),1,ilnk
        trkp(10,1,ilnk) = 0.                  ! s (track length)
        trkp(11,1,ilnk) = 10*isec + Level     ! Analysis Level & Sector No
        trkp(12,1,ilnk) = chifit              ! Reduced Chi-Square
        DO ip = 2,npln
          DO i = 1,6
            trkp(i,ip,ilnk) = sda_trp(i,ip)    ! x,y,z,Cx,Cy,Cz
          ENDDO
          trkp( 7,ip,ilnk) = svin(7)           ! p
*       print *,'prfit1: ',  trkp( 7,ip,ilnk),ip,ilnk
          trkp( 8,ip,ilnk) = svin(8)           ! velocity of a particle 
          trkp( 9,ip,ilnk) = svin(9)           ! Q 
          trkp(10,ip,ilnk) = sda_trp(7,ip)     ! s (track length)
          trkp(11,ip,ilnk) = 10*isec + Level   ! Analysis Level & Sector
          trkp(12,ip,ilnk) = chifit            ! Reduced Chi-Square
        ENDDO
C
C Find the {u,v,w} slabs in forward EC where the current track points to 
C
C Check if track has reached the EC plane?
        ip    = nec_min
        IF(trkp(1,ip,ilnk).GT.999.) GO TO 20
C Define "x,y,z,u,v,w" in local EC C.S. {i,j,k} whwre k perp. to plane
        d     = sda_pln(8,ip,isec)
        x0(1) = trkp(1,ip,ilnk) - sda_pln(1,ip,isec)
        x0(2) = trkp(2,ip,ilnk) - sda_pln(2,ip,isec)
        x0(3) = trkp(3,ip,ilnk) - sda_pln(3,ip,isec)
        x = SQRT( (x0(1) - sda_pln(4,ip,isec)*d)**2
     1          + (x0(2) - sda_pln(5,ip,isec)*d)**2 )
        IF(x0(1) .GT. sda_pln(4,ip,isec)*d) x = -x
        y = x0(3)
C Particle direction in the local system
        phi = ATAN2(trkp(5,ip,ilnk),trkp(4,ip,ilnk))
     1       -ATAN2( sda_pln(5,ip,isec),sda_pln(4,ip,isec) )
        u = SIN(ACOS(trkp(6,ip,ilnk)))*SIN(phi)
        v = trkp(6,ip,ilnk)
        w = SIN(ACOS(trkp(6,ip,ilnk)))*COS(phi)
        x = x + u*dd/w
        y = y + v*dd/w
        spsi = SIN(sda_pln(18,ip,isec)/2.)
        tpsi = TAN(sda_pln(18,ip,isec)/2.)
        sml  = (sda_pln(8,ip,isec) + dd)*tan20
        rat = (sml + x)*tpsi
C Check boundaries of EC triangle at distance "d+dd" from origine
        IF(ABS(x).GE.sml .OR. ABS(y).GE.rat) GO TO 20
        itr_ecid(1,ilnk) = 37. - 18.*(1. - x/sml)
        itr_ecid(2,ilnk) = 37. - 18.*spsi*(sml + x + y/tpsi)/sml
        itr_ecid(3,ilnk) = 37. - 18.*spsi*(sml + x - y/tpsi)/sml
 20     CONTINUE
C
C Fill array "ev_out(36,it)" i.e. DST bank 'EVTB'
C
***        IF(lanal(1).LT.3) THEN
*** then have to cleanup ntr_out in ana_fit !!!!!!!!!!!!!!!
          IF(ntr_out.GE.ntrmx) GOTO 30
c          CALL ana_scid(ilnk, iflg)
          do i=1,npl_sc
             x00(i)=trkp(1,nsc_min-1+i,ilnk)
          enddo
          CALL dcscid(isec, itr_scid(1,ilnk), x00, iflg)
cboy          IF(iflg.EQ.1) GOTO 30
          ntr_out = ntr_out + 1
          it = ntr_out         
          CALL dcrotsd(ev_out(1,it), svin,isec)
          ev_out(10,it) = ilnk
          ev_out(11,it) = 10*isec + Level
          ev_out(12,it) = chifit
C Tagging electron track or track with Beta determined using ST-counter & ToF 
          ev_out(1,it)  = 0.
C* The "itrk0" is not yet derived (it will be done later in "ana_finde.F")
C*        IF(ilnk.EQ.itrk0) ev_out(1,it) = 1.    for electroproduction
C*        IF(ilnk.EQ.itrk0) ev_out(1,it) = itag0 for photoproduction
C Add the trajectory prediction at plane of CC
          ip = ncc_min
          il = 13
          DO i = 1,6
            ev_out(il,it) = trkp(i,ip,ilnk)
            il = il + 1
          ENDDO
          ev_out(19,it) = trkp(10,ip,ilnk)
          ev_out(20,it) = 0.
C Add the trajectory prediction at plane#1 of SC
          ip = itr_scid(2,ilnk)
          il = 21
          DO i = 1,6
            ev_out(il,it) = trkp(i,ip,ilnk)
            il = il + 1
          ENDDO
          ev_out(27,it) = trkp(10,ip,ilnk)
          ev_out(28,it) = itr_scid(1,ilnk) +  100*itr_scid(2,ilnk)
     1                                     +10000*itr_scid(3,ilnk)
C Add the trajectory prediction at plane of EC
          ip = nec_min
          il = 29
          DO i = 1,6
            ev_out(il,it) = trkp(i,ip,ilnk)
            il = il + 1
          ENDDO
          ev_out(35,it) = trkp(10,ip,ilnk)
          ev_out(36,it) = itr_ecid(1,ilnk) +  100*itr_ecid(2,ilnk)
     1                                     +10000*itr_ecid(3,ilnk)
***        ENDIF





ccboyc      print *,'svin(fit )=',svin

        call hf1(17010,lnk_vect(1,ilnk)-svin(1),1.)
        call hf1(17020,lnk_vect(2,ilnk)-svin(2),1.)
        call hf1(17030,lnk_vect(3,ilnk)-svin(3),1.)
        call hf1(17040,lnk_vect(4,ilnk)-svin(4),1.)
        call hf1(17050,lnk_vect(5,ilnk)-svin(5),1.)
        call hf1(17060,lnk_vect(6,ilnk)-svin(6),1.)



c        th0 = ACOS(evin(6,it))
c        phi0= ATAN2(evin(5,it),evin(4,it))

        call hf1(17061,(ACOS(lnk_vect(5,ilnk))-ACOS(svin(5)))*raddeg,1.)
        call hf1(17062,(ATAN2(lnk_vect(6,ilnk),lnk_vect(4,ilnk))-
     &                 ATAN2(svin(6),svin(4)))*raddeg,1.)



        call hf1(17070,((lnk_vect(7,ilnk)-svin(7))/svin(7)*100.),1.)
        if(svin(7).GT.1.)
     &    call hf1(17071,((lnk_vect(7,ilnk)-svin(7))/svin(7))*100.,1.)
        if(svin(7).LE.1.0 .AND. svin(7).GT.0.3)
     &    call hf1(17072,((lnk_vect(7,ilnk)-svin(7))/svin(7))*100.,1.)
        if(svin(7).LE.0.3)
     &    call hf1(17073,((lnk_vect(7,ilnk)-svin(7))/svin(7))*100.,1.)
        call hf1(17080,lnk_vect(8,ilnk)-itr_scid(1,ilnk),1.)
        call hf1(17100,lnk_vect(12,ilnk)-svin(9),1.)

c        print *,'ana_prfit: u=',lnk_ec(9,ilnk),' v=',lnk_ec(10,ilnk),
c     &     ' w=',lnk_ec(11,ilnk),' e=',lnk_ec(12,ilnk)

        if(lnk_ec(12,ilnk).GT.0.0.AND.itr_ecid(1,ilnk).GT.0) then
          call hf1(17091,lnk_ec(9,ilnk)-FLOAT(itr_ecid(1,ilnk)),1.)
          call hf1(17092,lnk_ec(10,ilnk)-FLOAT(itr_ecid(2,ilnk)),1.)
          call hf1(17093,lnk_ec(11,ilnk)-FLOAT(itr_ecid(3,ilnk)),1.)
c      print *,'+++ 17 +++++++++++++++++++++++',lnk_ec(12,ilnk),svin(7)
          call hf1(17094,svin(7)-lnk_ec(12,ilnk),1.)
          call hf1(17095,((svin(7)-lnk_ec(12,ilnk))/svin(7))*100.,1.)
          call hf1(17096,lnk_vect(7,ilnk)-lnk_ec(12,ilnk),1.)
          call hf1(17097,((lnk_vect(7,ilnk)-lnk_ec(12,ilnk))/
     &                        lnk_vect(7,ilnk))*100.,1.)

      if(((lnk_vect(7,ilnk)-lnk_ec(12,ilnk))/lnk_vect(7,ilnk))*100.0
     &     .LT.30.0.AND.lnk_vect(12,ilnk).LT.0.) itrk0=ilnk


          call hf2(17098,lnk_ec(4,ilnk)/lnk_vect(7,ilnk),
     &             lnk_ec(8,ilnk)/lnk_vect(7,ilnk),1.)
          call hf2(17099,lnk_ec(4,ilnk)/svin(7),lnk_ec(8,ilnk)
     &             /svin(7),1.)
         if(svin(9).LT.0)
     &    call hf2(17101,lnk_ec(4,ilnk)/svin(7),lnk_ec(8,ilnk)/svin(7),
     &             1.)
         if(svin(9).GT.0)
     &    call hf2(17102,lnk_ec(4,ilnk)/svin(7),lnk_ec(8,ilnk)/svin(7),
     &             1.)

        endif

C
C Store trajectories for drawing
*        IF(if_draw.EQ.1) THEN
        IF(inter.EQ.1) THEN
          IF(ntrdraw0.GE.max_draw) GOTO 30
          ntrdraw0 = ntrdraw0 + 1
          draw0_isec(ntrdraw0)  = isec
          draw0_nstep(ntrdraw0) = nstep_xyz
          DO ist = 1,nstep_xyz
            draw0_xyz(1,ist,ntrdraw0) = xyz(1,ist)
            draw0_xyz(2,ist,ntrdraw0) = xyz(2,ist)
            draw0_xyz(3,ist,ntrdraw0) = xyz(3,ist)
          ENDDO
C Store ST hits
          k = 0
          DO il = nst_min,nst_max
            k = k + 1
            draw0_hit(1,k,ntrdraw0) = sda_trp(1,il)
            draw0_hit(2,k,ntrdraw0) = sda_trp(2,il)
            draw0_hit(3,k,ntrdraw0) = sda_trp(3,il)
          ENDDO
C Store CC,SC and EC hits
          k = 2
          DO il = ncc_min,nec_max
            k = k + 1
            draw0_hit(1,k,ntrdraw0) = sda_trp(1,il)
            draw0_hit(2,k,ntrdraw0) = sda_trp(2,il)
            draw0_hit(3,k,ntrdraw0) = sda_trp(3,il)
          ENDDO
        ENDIF
 30     CONTINUE
C Print track info for PR fit (included in dcfit.c)
***        IF(lprnt(4).EQ.1) CALL sda_ptrk(ilnk,isec)
  100 CONTINUE                  ! End of loop over linked clusters
      ifail = n_trk
C
C Print DST bank 'EVTB'
      IF(lprnt(7).EQ.1 .AND. ntr_out.GT.0)
     &  CALL dcpdst(ntr_out, ev_out, ntagh, tag_res)
C
C elastic peak
C
        if(itrk0.EQ.0) goto 500
        E1 = lnk_vect(7,itrk0)
        costheta1 = lnk_vect(4,itrk0)-0.005

c      E1 = E0*mp/(E0*(1.-costheta1)+mp)

        nu = E0 - E1
        Q2 = 2.*E0*E1*(1.-costheta1)
        W2 = mp*mp + 2.*mp*nu - Q2
c        print *,'costheta1=',costheta1,' Ebeam=',zbeam(2)
c        print *,'E1=',E1,'  nu=',nu,'  Q**2=',Q2,'  W**2 = ',W2

        sintheta1 = sqrt(1.-costheta1*costheta1)
        tantheta2 = E1*sintheta1/(E0-E1*costheta1)
        theta2 = ATAN(tantheta2)
c        print *,'tantheta2=',tantheta2,'  theta2=',theta2*raddeg
        costheta2 = COS(theta2)

        call hf1(17201,W2,1.)
        call hf1(17203,Q2,1.)

        sec1=trkp(11,1,itrk0)/10.
        if(sec1.EQ.1) sec2=4
        if(sec1.EQ.2) sec2=5
        if(sec1.EQ.3) sec2=6
        if(sec1.EQ.4) sec2=1
        if(sec1.EQ.5) sec2=2
        if(sec1.EQ.6) sec2=3

        do i=1,ntr_link
          if(i.NE.itrk0.AND.trkp(9,1,i).GT.0.0.AND.
     &  int(trkp(11,1,i)/10).EQ.sec2) then
            call hf1(17202,W2,1.)
            if(lnk_vect(4,i).LT.costheta2+0.15.AND.
     &         lnk_vect(4,i).GT.costheta2-0.15) call hf1(17204,W2,1.)
            if(lnk_vect(4,i).LT.costheta2+0.1.AND.
     &         lnk_vect(4,i).GT.costheta2-0.1) call hf1(17205,W2,1.)
            if(lnk_vect(4,i).LT.costheta2+0.05.AND.
     &         lnk_vect(4,i).GT.costheta2-0.05) call hf1(17206,W2,1.)
            goto 500
          endif
        enddo

 500    continue



#ifdef CED

c      print *,'CED stuff, ntr_out=',ntr_out

      ind0 = ETNCREATE(iw,'HBTR', 0, ncol_hbtr, ntr_out)

C count tracks in each sector
      do i=1,6
        ced_trk(i) = 0
      enddo
      do it=1,ntr_out
        ilnk = ev_out(10,it)                     ! ilnk for track it
        isec = itr_sect(ilnk)                    ! sector# for ilnk
        ced_trk(isec) = ced_trk(isec) + 1        ! track# in current sector
        itmp = ced_trk(isec)
        ced_lnk(itmp,isec) = ilnk
        ced_itr(itmp,isec) = it                 ! absolute track#
      enddo
      do i=1,6
c        print *,'ced_trk(',i,')=',ced_trk(i)
      enddo

C creates and fills banks for CED
      do isec=1,6
        if(ced_trk(isec).NE.0) then
          n_dclay = ced_trk(isec) * (npl_dc - 2)
          n_planes = ced_trk(isec) * (1 + npl_st + npl_cc + npl_sc + 
     &               npl_ec)
          itmp = ncol_hbla
          ind1 = ETNCREATE(iw,'HBLA', isec, itmp, n_dclay)
          itmp = ncol_dpl
          ind2 = ETNCREATE(iw,'HDPL', isec, itmp, n_planes)

          do it=1,ced_trk(isec)
            ilnk = ced_lnk(it,isec)
c            print *,'ilnk=',ilnk

            do il=1,npl_dc
              if(il.LT.5 .OR. il.GT.6) then
                ipl = nst_max + il                             ! layer for trkp[][][]
c                print *,'isec=',isec,' it=',ced_itr(it,isec)
                iw(ind1+1) = ced_itr(it,isec)*100+ipl          ! 100*track# + plane#
                rw(ind1+2) = trkp(2,ipl,ilnk)                  ! position
                rw(ind1+3) = trkp(3,ipl,ilnk)
                rw(ind1+4) = trkp(1,ipl,ilnk)
c                print *,'x,y,z =',rw(ind1+2),rw(ind1+3),rw(ind1+4)
                rw(ind1+5) = trkp(5,ipl,ilnk)                  ! direction
                rw(ind1+6) = trkp(6,ipl,ilnk)
                rw(ind1+7) = trkp(4,ipl,ilnk)
                rw(ind1+8) = trk(8,il,ilnk)                    ! track length
                iw(ind1+9) = 0                                 ! trk_pt[ilnk-1][il-1] - ptr to DC1
                iw(ind1+10)= int( trk(12,il,ilnk) )            ! Status
                iw(ind1+11)= int( trk(1,il,ilnk) )             ! wire#
                rw(ind1+12)= trk(3,il,ilnk)                    ! drift time
                rw(ind1+13)= trk(10,il,ilnk)                   ! track angle in Layer
                rw(ind1+14)= trk(9,il,ilnk)                    ! wire length to preamp.
                rw(ind1+15)= trk(5,il,ilnk)                    ! Sigma doca
                rw(ind1+16)= trk(7,il,ilnk)                    ! fitted doca
                ind1 = ind1 + ncol_hbla
              endif
            enddo

            do ipl=1,npln
              if(ipl.LE.nst_max .OR.
     &          (ipl.GE.ncc_min .AND. ipl.LE.ncc_max) .OR.
     &          (ipl.GE.nsc_min .AND. ipl.LE.nsc_max) .OR.
     &          (ipl.GE.nec_min .AND. ipl.LE.nec_max)) then
                iw(ind2+1) = ced_itr(it,isec)*100+ipl     ! 100*track# + plane#
                rw(ind2+2) = trkp(2,ipl,ilnk)             ! position
                rw(ind2+3) = trkp(3,ipl,ilnk)
                rw(ind2+4) = trkp(1,ipl,ilnk)
                rw(ind2+5) = trkp(5,ipl,ilnk)             ! direction
                rw(ind2+6) = trkp(6,ipl,ilnk)
                rw(ind2+7) = trkp(4,ipl,ilnk)
                rw(ind2+8) = trkp(10,ipl,ilnk)            ! track length
                ind2 = ind2 + ncol_dpl
              endif
            enddo

            itmp = ced_itr(it,isec)
            rw(ind0+1) = ev_out(1,itmp)
            rw(ind0+2) = ev_out(2,itmp)           ! Vertex {x,y,z}
            rw(ind0+3) = ev_out(3,itmp)
            tmp = ev_out(7,itmp)                  ! Momentum (GeV)
            rw(ind0+4) = ev_out(4,itmp) * tmp
            rw(ind0+5) = ev_out(5,itmp) * tmp     ! Momentum {Px,Py,Pz}
            rw(ind0+6) = ev_out(6,itmp) * tmp
            rw(ind0+7) = ev_out(8,itmp)           ! Charge
c            print *,'charge=',rw(ind0+7)
            rw(ind0+8) = ev_out(12,itmp)          ! Chisquare
            iw(ind0+9) = isec*100 + it
            ind0 = ind0 + ncol_hbtr

          enddo
        endif
      enddo


#endif


C
  999 CONTINUE
C
C Restore the original simulation parameters
      ztfit(1) = fit_iter
      zstep(1) = STmin
      zstep(3) = Dang
C
      RETURN
      END
