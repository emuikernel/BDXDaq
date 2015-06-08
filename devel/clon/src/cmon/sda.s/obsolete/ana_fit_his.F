C======================================================================
      SUBROUTINE ana_fit(iw,rw,ifail)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Final fitting with corrected drift times.
C-
C-
C-
C-   Controls:
C-
C-
C-   Library belongs    : libana.a
C-
C-   Calls              : ana_trfit, dcrotsd, sda_vrotsd, sda_ptrk,
C-                        dcpdst 
C-
C-   Created   JUNE-15-1992  Bogdan Niczyporuk
C-   Modified  JUNE-16-1992  Billy Leung
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
C== parameters, constants, control keys, ... ==
C
C lmode,zstep,lanal,ztfit,zcut,step,zbeam,lprnt,ifsim
#include "sdakeys.inc"
C a lot ...
#include "sdageom.inc"
C cal_sc(filled in sda_calib at initialization), vflt=29.97925
#include "sdacalib.inc"
C drawing stuff ...
#include "sdadraw.inc"
C
C== input/output ==
C
C sc_digi(filled in sda_evin: raw data)
#include "sdadigi.inc"
C
C
C ntrmx=500 - constant parameter
C
C INPUT:
C ntr_link - the number of linked tracks (after PRLIB)
C itr_sect() - sector number for track
C itr_scid(3,) - prediction: slab number in SC (filled by ana_scid())
C beta0 - track velocity (filled by ana_finde())
C
C OUTPUT:
C ntr_out - the number of tracks found (second index in ev_out(,))
C ev_out(,) - the list of tracks found
C itr_ecid(3,) - prediction: strip numbers in EC
C
C INPUT/OUTPUT:
C itr_level() - input have to be =3 for good tracks, output =4 for good tracks
C trkp
C trk

C lnk_vect - just for histogramming
C lnk_ec - just for histogramming

#include "sdaanal.inc"
C
C
C External function
      INTEGER iucomp
C
C Subroutine variables
      INTEGER iw(1000),ifail
      REAL rw(1000)
C
C Local variables
      INTEGER ndc
      PARAMETER (ndc = 36)
#ifdef CED
      integer ncol_tbla, ncol_tdpl, ncol_tbtr
      parameter (ncol_tbla=17, ncol_tdpl=8, ncol_tbtr=10)
      integer ETNCREATE, ind0, ind1, ind2, n_dclay, n_planes
      integer itmp, ipl, ind, etnlink
      integer ced_trk(6), ced_lnk(ntrmx,6), ced_itr(ntrmx,6)
      real tmp, time1, timeold, vertex(10)
#endif
C
      INTEGER i, k, il, it, ip, iw_, ilnk, ist, n_trk, isec, Level
      INTEGER ifirst, inter, if_draw
      REAL svin(9),svout(9), chifit, STmin, Dang, phi, pi
      REAL svinn(9)
      REAL d, dd, x,y, u,v,w, x0(3), spsi, tpsi, sml, rat, tan20, tan30
      real temp(9), svin1(9)
C
      real nu,Q2,W2,E0,E1,costheta1,costheta2,mp
      real theta2,tantheta2,sintheta1,raddeg
      integer sec1,sec2
      data mp/0.938/, E0/4.0451/
C
      INTEGER ie, itr, idsc, ipsc, ihsc, iret
      REAL    tPR, tFLe, tFLm, Se, Sm, t0, twL,twR,tm
C 
      DATA ifirst /1/
      DATA dd /18.45/      ! dd = 15*1.23 (1-st SC plane of Backward EC)
C
      ntr_out = 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
C
      IF(ifirst.EQ.1) THEN
        ifirst= 0
        timeold=0
        call hbook1(267,'GOOD events vs TIME $',4000, 0., 4000., 0.)
        inter = 0
        IF(iucomp(4HINT ,lmode,1).NE.0) inter = 1
        tan20 = 0.36397023
        tan30 = 0.57735027
        pi    = 3.14159265
        raddeg = 180./ACOS(-1.)
      ENDIF
C
C Save the initial parameters and replace them with the new ones
C
      STmin = zstep(1)
      zstep(1) = STmin
      Dang = zstep(3)
      zstep(3) = 2.*Dang
C
      Level = 4
      n_trk = 0
C
C Loop over all track candidates prepared by Pattern Recognition routines
C
      IF(ntr_link.EQ.0) GO TO 999
C
C Set flag "if_draw = 1" if interactive job and FF is selected (with DT)
      if_draw = 0
      IF(lanal(1).EQ.4 .AND. inter.EQ.1) if_draw = inter
C
C
      ifail = 0
      DO 100 ilnk = 1,ntr_link
        itr_ecid(1,ilnk) = 0
        itr_ecid(2,ilnk) = 0
        itr_ecid(3,ilnk) = 0
C
C Check if fit is successful
        IF(itr_level(ilnk).NE.3) GOTO 100
        isec = itr_sect(ilnk)
        DO i = 1,9
          svin(i) = trkp(i,1,ilnk)
        ENDDO
C
C Fit using the reconstructed drift times
C

        do i = 1,9
          temp(i) = svin(i)
        enddo


c no HBT ???
cboy        do i = 1,7
cboy          svin(i) = lnk_vect(i,ilnk)
cboy        enddo


        IF(ztfit(1).GT.0)
c     &    CALL ana_trfit(iw,isec,trk(1,1,ilnk),Level,svin,chifit)
     & call dctrfit(iw,isec,Level,sda_trp,trk(1,1,ilnk),svin,chifit)
        do i = 1,9
          svin1(i) = svin(i)
        enddo
C
C Check if fit was successful
        IF(chifit.GT.zcut(7)) THEN
          chifit = 19.5
          GOTO 100
        ENDIF
        n_trk = n_trk + 1
C
C Swimming back to the target plane#1
        IF(ztfit(5).EQ.4.) THEN

          DO i = 1,6
            svinn(i) = dc_trp(i,1)
          ENDDO
          DO i = 7,9
            svinn(i) = svin(i)
          ENDDO
*          DO i = 1,9
*            svinn(i) = svin(i)
*          ENDDO

*          CALL sda_swim_vt(iw,isec,svinn,step,level,if_draw,svout,vertex)
          CALL dcswimvt(iw,sda_pln(1,1,isec),isec,svinn,step,level,if_draw,
     &                  svout,vertex,sda_trp,nstep_xyz,xyz)

**      if(chifit.LT.1.2.AND.chifit.GT.0.8) then
        call hfill(290,vertex(2),vertex(1),1.)
        call hfill(280,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
        call hfill(281,vertex(1)-vertex(8),0.,1.)
        call hfill(282,vertex(2)-vertex(9),0.,1.)
        call hfill(283,vertex(3)-vertex(10),0.,1.)
        call hfill(280*10+isec,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
        call hfill(281*10+isec,vertex(1)-vertex(8),0.,1.)
        call hfill(282*10+isec,vertex(2)-vertex(9),0.,1.)
        call hfill(283*10+isec,vertex(3)-vertex(10),0.,1.)
        if(svin(6).GT.-0.01 .AND. svin(6).LT. 0.01) then
          call hfill(480,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
          call hfill(481,vertex(1)-vertex(8),0.,1.)
          call hfill(482,vertex(2)-vertex(9),0.,1.)
          call hfill(483,vertex(3)-vertex(10),0.,1.)
          call hfill(480*10+isec,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
          call hfill(481*10+isec,vertex(1)-vertex(8),0.,1.)
          call hfill(482*10+isec,vertex(2)-vertex(9),0.,1.)
          call hfill(483*10+isec,vertex(3)-vertex(10),0.,1.)
          call hfill(28011,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
        else
          call hfill(580,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
          call hfill(581,vertex(1)-vertex(8),0.,1.)
          call hfill(582,vertex(2)-vertex(9),0.,1.)
          call hfill(583,vertex(3)-vertex(10),0.,1.)
          call hfill(580*10+isec,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
          call hfill(581*10+isec,vertex(1)-vertex(8),0.,1.)
          call hfill(582*10+isec,vertex(2)-vertex(9),0.,1.)
          call hfill(583*10+isec,vertex(3)-vertex(10),0.,1.)
          call hfill(28012,vertex(2)-vertex(9),vertex(3)-vertex(10),1.)
        endif
**      endif

C
C convert to (Another Sector?) Coordinate System
**??          IF(ABS(svout(5)).LT.0.001) GOTO 100
c          tanphi = svout(6)/ABS(svout(5))
c          isecp = isec
c          asec  = 0.
c          IF(tanphi.GT.tan30) THEN
c            asec = pi/3.
c            isecp = isec + 1
c            IF(isecp.GT.6) isecp = 1
c          ELSE IF(tanphi.LT.-tan30) THEN
c            asec =-pi/3.
c            isecp = isec - 1
c            IF(isecp.LT.1) isecp = 6
c          ENDIF
c          IF(asec.NE.0.) THEN
c            svin(1) = svout(1)
c            svin(2) = svout(2)*COS(asec) + svout(3)*SIN(asec)          
c            svin(3) =-svout(2)*SIN(asec) + svout(3)*COS(asec)
c            svin(4) = svout(4)
c            svin(5) = svout(5)*COS(asec) + svout(6)*SIN(asec)
c            svin(6) =-svout(5)*SIN(asec) + svout(6)*COS(asec)
c            svin(7) = svout(7)
c            svin(8) = svout(8)
c            svin(9) = svout(9)
c          ELSE
            DO i = 1,9
              svin(i) = svout(i)
            ENDDO
c          ENDIF
C If photoproduction get Track Point for Start Counter plane#2 or 3 (no PTF)
          IF(zbeam(1).EQ.0.) THEN
      print *,'STOP in ana_fit'
      stop
**            CALL sda_swim(isec,      1,      3,     0,      0, svin)
            CALL dcswim(sda_pln(1,1,isec),1,3,0,0,svin,
     &                  sda_trp,sda_tmat,nstep_xyz,xyz)
          ENDIF
        ENDIF
C
C Store the results of FF fit, which was not rejected from the above cuts
C
        itr_level(ilnk) = Level
        itr_sect(ilnk)  = isec
        DO il = 1,ndc
          ip = il + nst_max
          iw_ = ABS(trk(1,il,ilnk))
          trk(8,il,ilnk)  = dc_trp(7,il)                           ! Tlen
          IF(il.LT.13) THEN
            trk( 9,il,ilnk) = dc_wlen(2,iw_,il,isec) - dc_trp(3,il) ! Wlen
          ELSE
            trk( 9,il,ilnk) = dc_wlen(1,iw_,il,isec) - dc_trp(3,il) ! Wlen
          ENDIF
          phi = ATAN2(dc_trp(5,il),dc_trp(4,il))
          IF(iw_.GT.sda_pln(17,ip,isec)) THEN
            trk(10,il,ilnk) =
     1          phi - ATAN2(sda_pln(5,ip,isec),sda_pln(4,ip,isec)) ! alfa
          ELSE
            trk(10,il,ilnk) = phi-sda_pln(12,ip,isec)
     1                       -(iw_-1)*sda_pln(13,ip,isec)           ! alfa        
          ENDIF
          trk(11,il,ilnk)  = 10*isec + Level                       ! Level & Sect
        ENDDO
        svin(8) = trk(6,1,ilnk)               ! velocity of a particle (Level>2)
C First plane is 
        DO i = 1,9
          trkp(i,1,ilnk) = svin(i)
        ENDDO
        trkp(10,1,ilnk) = 0.                  ! s (track length)
        trkp(11,1,ilnk) = 10*isec + Level     ! Analysis Level & Sector No
        trkp(12,1,ilnk) = chifit              ! Reduced Chi-Square
        DO ip = 2,npln
          DO i = 1,6
            trkp(i,ip,ilnk) = sda_trp(i,ip)
          ENDDO
          trkp(7,ip,ilnk)  = svin(7)           ! p 
          trkp(8,ip,ilnk)  = svin(8)           ! velocity of a particle 
          trkp(9,ip,ilnk)  = svin(9)           ! Q 
          trkp(10,ip,ilnk) = sda_trp(7,ip)     ! s (track length)
          trkp(11,ip,ilnk) = 10*isec + Level   ! Analysis Level & Sector No
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
C Fill the array "ev_out(36,it)" i.e. DST bank 'EVTB'
C
        IF(lanal(1).EQ.4) THEN
          IF(ntr_out.GE.ntrmx) GO TO 30
          ntr_out = ntr_out + 1
          it = ntr_out
C
C ztfit(5) ?????
c          IF(ztfit(5).EQ.4.) THEN
c            CALL dcrotsd(ev_out(1,it), svin,isecp)
c          ELSE
            CALL dcrotsd(ev_out(1,it), svin,isec)
c          ENDIF
          ev_out(10,it) = ilnk
          ev_out(11,it) = 10*isec + Level
          ev_out(12,it) = chifit
C Tagging electron track or track with Beta determined using ST-counter & ToF 
          ev_outx(it) = ev_out(1,it) ! save X vertex
          ev_out(1,it) = 0.
          IF(ilnk.EQ.itrk0) THEN
            ev_out(1,it) = 1.
            IF(zbeam(1).EQ.0.) ev_out(1,it) = itag0
          ENDIF
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
        ENDIF


C PR-TBT


        call hf1(27010,lnk_vect(1,ilnk)-svin1(1),1.)
        call hf1(27020,lnk_vect(2,ilnk)-svin1(2),1.)
        call hf1(27030,lnk_vect(3,ilnk)-svin1(3),1.)
        call hf1(27040,lnk_vect(4,ilnk)-svin1(4),1.)
        call hf1(27050,lnk_vect(5,ilnk)-svin1(5),1.)
        call hf1(27060,lnk_vect(6,ilnk)-svin1(6),1.)


      if(svin1(9).LT.0.) then

        call hf1(27070,((lnk_vect(7,ilnk)-svin1(7))/svin1(7)*100.),1.)
        if(svin1(7).GT.1.)
     &    call hf1(27071,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.1.0 .AND. svin1(7).GT.0.3)
     &    call hf1(27072,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.0.3 .AND. svin1(7).GT.0.1)
     &    call hf1(27073,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.0.1)
     &    call hf1(27074,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)

      else

        call hf1(27075,((lnk_vect(7,ilnk)-svin1(7))/svin1(7)*100.),1.)
        if(svin1(7).GT.1.)
     &    call hf1(27076,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.1.0 .AND. svin1(7).GT.0.3)
     &    call hf1(27077,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.0.3 .AND. svin1(7).GT.0.1)
     &    call hf1(27078,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.0.1)
     &    call hf1(27079,((lnk_vect(7,ilnk)-svin1(7))/svin1(7))*100.,1.)

      endif


        call hf1(27080,lnk_vect(8,ilnk)-itr_scid(1,ilnk),1.)
c        if(itr_ecid(1,ilnk).GT.0.) then
c          call hf1(27091,lnk_vect(9,ilnk)-itr_ecid(1,ilnk),1.)
c          call hf1(27092,lnk_vect(10,ilnk)-itr_ecid(2,ilnk),1.)
c          call hf1(27093,lnk_vect(11,ilnk)-itr_ecid(3,ilnk),1.)
c        endif
        call hf1(27100,lnk_vect(12,ilnk)-svin1(9),1.)

c        if(lnk_ec(12,ilnk).GT.0.0.AND.itr_ecid(1,ilnk).GT.0.AND.ilnk.EQ.itrk0) then
        if(lnk_ec(12,ilnk).GT.0.0.AND.itr_ecid(1,ilnk).GT.0) then
          call hf1(27091,lnk_ec(9,ilnk)-FLOAT(itr_ecid(1,ilnk)),1.)
          call hf1(27092,lnk_ec(10,ilnk)-FLOAT(itr_ecid(2,ilnk)),1.)
          call hf1(27093,lnk_ec(11,ilnk)-FLOAT(itr_ecid(3,ilnk)),1.)
c      print *,'+++ 27 +++++++++++++++++++++++',lnk_ec(12,ilnk),svin1(7)
          call hf1(27094,svin1(7)-lnk_ec(12,ilnk),1.)
          call hf1(27095,((svin1(7)-lnk_ec(12,ilnk))/svin1(7))*100.,1.)

          call hf2(27099,lnk_ec(4,ilnk)/svin1(7),lnk_ec(8,ilnk)/svin1(7),
     &             1.)
         if(svin1(9).LT.0)
     &    call hf2(27101,lnk_ec(4,ilnk)/svin1(7),lnk_ec(8,ilnk)/svin1(7),
     &             1.)
         if(svin1(9).GT.0)
     &    call hf2(27102,lnk_ec(4,ilnk)/svin1(7),lnk_ec(8,ilnk)/svin1(7),
     &             1.)

        endif

C HBT-TBT


        call hf1(37010,temp(1)-svin1(1),1.)
        call hf1(37020,temp(2)-svin1(2),1.)
        call hf1(37030,temp(3)-svin1(3),1.)
        call hf1(37040,temp(4)-svin1(4),1.)
        call hf1(37050,temp(5)-svin1(5),1.)
        call hf1(37060,temp(6)-svin1(6),1.)
        call hf1(37070,((temp(7)-svin1(7))/svin1(7)*100.),1.)
        if(svin1(7).GT.1.)
     &    call hf1(37071,((temp(7)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.1.0 .AND. svin1(7).GT.0.3)
     &    call hf1(37072,((temp(7)-svin1(7))/svin1(7))*100.,1.)
        if(svin1(7).LE.0.3)
     &    call hf1(37073,((temp(7)-svin1(7))/svin1(7))*100.,1.)
        call hf1(37080,lnk_vect(8,ilnk)-itr_scid(1,ilnk),1.)
        if(itr_ecid(1,ilnk).GT.0.) then
          call hf1(37091,lnk_vect(9,ilnk)-itr_ecid(1,ilnk),1.)
          call hf1(37092,lnk_vect(10,ilnk)-itr_ecid(2,ilnk),1.)
          call hf1(37093,lnk_vect(11,ilnk)-itr_ecid(3,ilnk),1.)
        endif
        call hf1(37100,temp(9)-svin1(9),1.)


c#indef INTER
C
C Store trajectories for drawing
        IF(if_draw.EQ.1) THEN
          IF(ntrdraw.GE.max_draw) GO TO 30
          ntrdraw = ntrdraw + 1
          draw_isec(ntrdraw)  = isec
          draw_nstep(ntrdraw) = nstep_xyz
          DO ist = 1,nstep_xyz
            draw_xyz(1,ist,ntrdraw) = xyz(1,ist)
            draw_xyz(2,ist,ntrdraw) = xyz(2,ist)
            draw_xyz(3,ist,ntrdraw) = xyz(3,ist)
          ENDDO
C Store ST hits
          k = 0
          DO il = nst_min,nst_max
            k = k + 1
            draw_hit(1,k,ntrdraw) = sda_trp(1,il)
            draw_hit(2,k,ntrdraw) = sda_trp(2,il)
            draw_hit(3,k,ntrdraw) = sda_trp(3,il)
          ENDDO
C Store CC,SC and EC hits
          k = 2
          DO il = ncc_min,nec_max
            k = k + 1
            draw_hit(1,k,ntrdraw) = sda_trp(1,il)
            draw_hit(2,k,ntrdraw) = sda_trp(2,il)
            draw_hit(3,k,ntrdraw) = sda_trp(3,il)
          ENDDO
        ENDIF
 30     CONTINUE
c#endif

C
C Print track info for FF fit (included in dcfit.c)
***        IF(lprnt(6).EQ.1) CALL sda_ptrk(ilnk,isec)
 100  CONTINUE                  ! End of loop over linked clusters
      ifail = n_trk
C
C Recalculate the particle's velocity
C
        IF(ntr_out.EQ.0) GOTO 999
c#ifdef SIM
        IF(ifsim.EQ.1) THEN
          ie = 0
          DO it = 1,ntr_out
            IF(ev_out(10,it).EQ.itrk0) THEN
              ie = it
              idsc = itr_scid(1,itrk0)
              ipsc = itr_scid(2,itrk0)
              ihsc = itr_scid(3,itrk0)
              isec = itr_sect(itrk0)
              IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
                tPR =(sc_hleng(idsc,isec)+trkp(3,ipsc,itrk0))
     1              /cal_sc(7,idsc,isec)
                tFLe=sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec)
     1              -tPR -cal_sc(1,idsc,isec)               
              ELSE
                tPR =(sc_hleng(idsc,isec)-trkp(3,ipsc,itrk0))
     1              /cal_sc(7,idsc,isec)       
                tFLe=sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec)
     1              -tPR -cal_sc(2,idsc,isec)        
              ENDIF       
              ev_out(8,it) = beta0
              Se = trkp(10,ipsc,itrk0)
            ENDIF
          ENDDO
          IF(ie.EQ.0) GO TO 999
          itr = 0
          DO it = 1,ntr_out
            IF(ev_out(10,it).NE.itrk0) THEN
              itr = ev_out(10,it)
              idsc = itr_scid(1,itr)
              ipsc = itr_scid(2,itr)
              ihsc = itr_scid(3,itr)
              isec = itr_sect(itr)
              IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
                tPR = (sc_hleng(idsc,isec)+trkp(3,ipsc,itr))
     1               /cal_sc(7,idsc,isec)
                tFLm= sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec)
     1               -tPR -cal_sc(1,idsc,isec)               
              ELSE
                tPR = (sc_hleng(idsc,isec)-trkp(3,ipsc,itr))
     1               /cal_sc(7,idsc,isec)       
                tFLm= sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec)
     1               -tPR -cal_sc(2,idsc,isec)        
              ENDIF       
              Sm   = trkp(10,ipsc,itr)
              ev_out(8,it) = Sm*beta0/(vflt*beta0*(tFLm - tFLe) + Se)
            ENDIF
          ENDDO
c#else
        ELSE
          ie = 0
          DO it = 1,ntr_out
            IF(ev_out(10,it).EQ.itrk0) THEN
              ie = it
              idsc = itr_scid(1,itrk0)
              ipsc = itr_scid(2,itrk0)
              ihsc = itr_scid(3,itrk0)
              isec = itr_sect(itrk0)
              CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
              Se = trkp(10,ipsc,itrk0)
              t0 = tm - Se/vflt/beta0 + cal_sc(20,idsc,isec)          
              ev_out(8,it) = beta0
            ENDIF
          ENDDO
          IF(ie.EQ.0) GO TO 999
          DO it = 1,ntr_out
            IF(ev_out(10,it).NE.itrk0) THEN
              itr = ev_out(10,it)
              idsc = itr_scid(1,itr)
              ipsc = itr_scid(2,itr)
              ihsc = itr_scid(3,itr)
              isec = itr_sect(itr)
              Sm   = trkp(10,ipsc,itr)
              CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
              Sm = trkp(10,ipsc,itr)
              tFLm  = tm + cal_sc(20,idsc,isec) - t0         
              ev_out(8,it) = Sm/(vflt*tFLm)
            ENDIF
          ENDDO
        ENDIF
c#endif
C
C Print DST bank 'EVTB'
        IF(lprnt(7).EQ.1 .AND. ntr_out.GT.0)
     &    CALL dcpdst(ntr_out, ev_out, ntagh, tag_res)
C
C elastic peak
C
        E1 = trkp(7,1,itrk0)
        costheta1 = trkp(4,1,itrk0)

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

        call hf1(27201,W2,1.)
        call hf1(27203,Q2,1.)

        sec1=trkp(11,1,itrk0)/10
        if(sec1.EQ.1) sec2=4
        if(sec1.EQ.2) sec2=5
        if(sec1.EQ.3) sec2=6
        if(sec1.EQ.4) sec2=1
        if(sec1.EQ.5) sec2=2
        if(sec1.EQ.6) sec2=3

        do i=1,ntr_link
          if(i.NE.itrk0.AND.trkp(9,1,i).GT.0.0.AND.
     &  int(trkp(11,1,i)/10).EQ.sec2) then
c            print *,'YES'
            call hf1(27202,W2,1.)
            if(trkp(4,1,i).LT.costheta2+0.05.AND.
     &         trkp(4,1,i).GT.costheta2-0.05) call hf1(27204,W2,1.)
            goto 500
          endif
        enddo

 500    continue


      if(ntr_out.GT.0) then
        ind = etnlink(iw,'TGBI',0)
        if(ind.GT.0) then
          time1 = iw(ind+3) - 6049918
          if(time1.GT.timeold) then
            timeold = time1
            call hfill(267,time1/100.,0.,1.)
          endif
        endif
      endif



#ifdef CED

c      print *,'CED stuff, ntr_out=',ntr_out

      ind0 = ETNCREATE(iw,'TBTR', 0, ncol_tbtr, ntr_out)

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
          n_planes = ced_trk(isec) * (1 + npl_st + npl_cc + npl_sc
     &             + npl_ec)
          itmp = ncol_tbla
          ind1 = ETNCREATE(iw,'TBLA', isec, itmp, n_dclay)
          itmp = ncol_tdpl
          ind2 = ETNCREATE(iw,'TDPL', isec, itmp, n_planes)

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
                rw(ind1+17)= trk(4,il,ilnk)                    ! calculated doca SHOULD BE HERE
                ind1 = ind1 + ncol_tbla
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
                ind2 = ind2 + ncol_tdpl
              endif
            enddo

            itmp = ced_itr(it,isec)
            rw(ind0+1) = ev_outx(itmp) ! ev_out(1,itmp)
            rw(ind0+2) = ev_out(2,itmp)           ! Vertex {x,y,z}
            rw(ind0+3) = ev_out(3,itmp)
c      print *,'itmp=',itmp,'  ilnk=',ilnk,'  x=',rw(ind0+1),'  y=',rw(ind0+2),'  z=',rw(ind0+3)
            tmp = ev_out(7,itmp)                  ! Momentum (GeV)
            rw(ind0+4) = ev_out(4,itmp) * tmp
            rw(ind0+5) = ev_out(5,itmp) * tmp     ! Momentum {Px,Py,Pz}
            rw(ind0+6) = ev_out(6,itmp) * tmp
            rw(ind0+7) = ev_out(8,itmp)           ! Charge
c            print *,'charge=',rw(ind0+7)
            rw(ind0+8) = ev_out(12,itmp)          ! Chisquare
            iw(ind0+9) = isec*100 + it
            iw(ind0+10)= 0                        ! SHOULD BE track# for this track in the Hit Based Tracking Banks
            ind0 = ind0 + ncol_tbtr

          enddo
        endif
      enddo


#endif





C
 999  CONTINUE
C
C Restore the initial parameters
      zstep(1) = STmin
      zstep(3) = Dang
C
      RETURN
      END

