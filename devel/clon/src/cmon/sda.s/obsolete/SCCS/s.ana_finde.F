h04218
s 00000/00000/00000
d R 1.2 01/11/19 19:02:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/ana_finde.F
e
s 00212/00000/00000
d D 1.1 01/11/19 19:02:33 boiarino 1 0
c date and time created 01/11/19 19:02:33 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE ana_finde(iw,rw,ifail)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods :Identify Electron or other particle with Start Cntr.
C-
C-
C-   Input   :  include files
C-
C-   Outputs :  itrk0  - track number
C-              beta0  - velocity "beta0" of the track "itrk0" 
C-
C-   Controls:
C-
C-
C-   Library belongs    : libsda.a
C-
C-   Calls              : ana_ecid, dcscid, ana_tag
C-
C-   Created   28-FEB-1996  Bogdan Niczyporuk
C-
C-   Called by sda_anal
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdadigi.inc"
#include "sdaanal.inc"
#include "sdacalib.inc"
C
C Subroutine parameters
      INTEGER iw(1000),ifail
      REAL rw(1000)
C
C Local variables
      INTEGER i, it, ih, itag, itrk, iflg, iret
      INTEGER ip, idsc, ipsc, ihsc, isec
      REAL tp, tsc, tst, d1, xx, dif, beta, t0tag,t0tof, toffs
      REAL twL, twR, tm
      real x0(npl_sc),p
C
C
      itag0 = 0
      itrk0 = 0
C
C *** Photoproduction. Define the particle velocity beta0 using SC, ST & TG
C
      IF(zbeam(1).EQ.0.) THEN
        ifail = 0
        t0tag = zcut(9)
        t0tof = zcut(10)
        toffs = t0tof - t0tag
C
C If data analysis call "ana_tag"
        CALL ana_tag(iw,rw,ntagh,tag_res)
        IF(ntagh.LE.0) GO TO 999
C
C Find a track with hits in SC & ST Counters
        DO 10 itrk = 1,ntr_link
          IF(itr_level(itrk).NE.2) GO TO 10
C
C Check chisquare of the track fiting for Level = 2
          IF(trkp(12,1,itrk).GT.zcut(4)) GO TO 10
C
C Identify the track in ToF Counters (SC)
c          CALL ana_scid(itrk, iflg)
          do i=1,npl_sc
             x0(i)=trkp(1,nsc_min-1+i,itrk)
          enddo
          isec = itr_sect(itrk)
          CALL dcscid(isec, itr_scid(1,itrk), x0, iflg)
          IF(iflg.GT.0) GO TO 10          
C
C Get time of ST           
          IF(st_digi(1,isec).NE.isec .OR. st_digi(2,isec).LT.1) GO TO 10
          ip = 0
          xx = trkp(1,2,itrk)
          IF(xx.LT.999. .AND. xx.LE.sda_pln(12,2,isec)) ip = 2             
          xx = trkp(1,3,itrk)
          IF(xx.LT.999. .AND. xx.GT.sda_pln(14,3,isec)) ip = 3
          IF(ip.EQ.0) GO TO 10
C
          IF(ip.EQ.2) THEN
            d1 = ABS(sda_pln(14,ip,isec)) + trkp(1,ip,itrk)
          ELSE
            d1 = sda_pln(16,ip-1,isec)+SQRT((sda_pln(14,ip,isec)-trkp
     1         (1,ip,itrk))**2+(sda_pln(15,ip,isec)-trkp(2,ip,itrk))**2)                                                  
          ENDIF
          tst = st_digi(2,isec)/cal_st(2,isec)          
     1         -d1/cal_st(4,isec) - cal_st(1,isec)
C Check time difference: ST - TAG neglecting flight time to ST
          itag = 0
          DO 8 ih = 1,ntagh
            IF(tag_res(3,ih).LE.0.) GO TO 8
            dif = tst - tag_res(6,ih)
            IF(ifsim.EQ.1) dif = dif + t0tof
            IF(ABS(dif - toffs) .LT. 3.0) THEN
              itag = ih
              GO TO 9
            ENDIF 
 8        CONTINUE
 9        CONTINUE
          IF(itag.EQ.0) GO TO 10
C
C Get time of SC           
          idsc = itr_scid(1,itrk)
          ipsc = itr_scid(2,itrk)
          ihsc = itr_scid(3,itrk)
          IF(ifsim.EQ.1) THEN
            IF(sc_digi(3,ihsc,1,isec).GT.sc_digi(5,ihsc,1,isec)) THEN
              tp  =(sc_hleng(idsc,isec)+trkp(3,ipsc,itrk))
     1            /cal_sc(7,idsc,isec)             
              tsc =sc_digi(2,ihsc,1,isec)/cal_sc(3,idsc,isec) - tp
     1            -cal_sc(1,idsc,isec)
            ELSE
              tp  =(sc_hleng(idsc,isec)-trkp(3,ipsc,itrk))
     1            /cal_sc(7,idsc,isec)
              tsc =sc_digi(4,ihsc,1,isec)/cal_sc(4,idsc,isec) - tp
     1            -cal_sc(2,idsc,isec)
            ENDIF
          ELSE       
            CALL dctwalk(ihsc,idsc,isec,cal_sc,sc_digi, twL,twR,tm,iret)
            tsc  = tm + cal_sc(20,idsc,isec)
          ENDIF         
C Now calculate beta for the track# itrk
          beta = (trkp(10,ipsc,itrk)-trkp(10,ip,itrk))/(tsc-tst)/vflt
          IF(beta.LT.0.1 .OR. beta.GT.1.5) GO TO 10          
C Check time difference: SC - TAG using current beta
          dif = tsc - trkp(10,ipsc,itrk)/vflt/beta - tag_res(6,itag)
          IF(ifsim.EQ.1) dif = dif + t0tof
          IF(ABS(dif - toffs) .GT. 2.0) GO TO 10         
C
          itrk0 = itrk
          itag0 = itag
          beta0 = beta
          ifail = 1
          GO TO 999 
 10     CONTINUE
        GO TO 999
      ENDIF                           ! IF(zbeam(1).EQ.0.)
C
C
C *** Electroproduction. The goal is to identify the electron.
C
      IF(zbeam(1).LT.0.) THEN
        ifail = -1
        itrk0 =  0
C
        DO 20 itrk = 1,ntr_link
          IF(itr_level(itrk).NE.2) GO TO 20
C
C Check chisquare of the track fiting for Level = 2
          IF(trkp(12,1,itrk).GT.zcut(4)) GO TO 20
C
C Check charge sign of the current track
          IF(ABS(zkine(1)).NE.1. .AND. trkp(9,1,itrk).NE.-1.0) GOTO 20
C
C Identify the electron track in ToF Counters (SC)
c          CALL ana_scid(itrk, iflg)
          do i=1,npl_sc
             x0(i)=trkp(1,nsc_min-1+i,itrk)
          enddo
          isec = itr_sect(itrk)
          CALL dcscid(isec, itr_scid(1,itrk), x0, iflg)
          IF(iflg.GT.0) GOTO 20
C
C Identify the current track in Shower Counter (EC forward)
c          CALL ana_ecid(itrk, iflg)
          p = trkp(7,nec_min,itrk)
          IF(lanal(1).LT.2) p = ev_out(7,itrk)
          CALL dcecid(isec, itr_ecid(1,itrk), p, iflg)
          IF(iflg.EQ.2) GOTO 20            ! Track hit point outside EC volume
C Store the results
          IF(iflg.EQ.1 .AND. zcut(5).EQ.0.) THEN   ! Track hit inside EC volume
            itrk0 = itrk
            beta0 = 1.
            ifail = 0
            GO TO 999
          ENDIF
          IF(iflg.EQ.0) THEN   ! Track inside EC volume & complete EC analysis
            itrk0 = itrk
            beta0 = 1.
            ifail = 1
            GO TO 999
          ENDIF
 20     CONTINUE
      ENDIF
C
  999 CONTINUE
C
C Tagging electron track or track with Beta determined using ST-counter & ToF 
      IF(lanal(1).EQ.2 .AND. ifail.EQ.1) THEN
        DO it = 1,ntr_out
          ev_out(1,it) = 0.
          IF(ev_out(10,it).EQ.itrk0) THEN
            ev_out(1,it) = 1
            IF(zbeam(1).EQ.0.) ev_out(1,it) = itag0
            ev_out(8,it) = beta0
          ENDIF
        ENDDO
C Print DST bank 'EVTB'
        IF(lprnt(7).EQ.2 .AND. ntr_out.GT.0)
     &    CALL dcpdst(ntr_out, ev_out, ntagh, tag_res)
      ENDIF
C
      RETURN
      END
E 1
