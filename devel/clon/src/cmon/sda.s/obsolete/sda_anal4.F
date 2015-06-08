C======================================================================
      SUBROUTINE sda_anal4(iw,rw,ifevb,inter,ievent)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : The main routine for TBT tracking only
C-                         
C-
C-   Inputs  :  REVB in BOS/FPACK format
C-
C-   Outputs :  DST (or MiniDST)
C-
C-   Controls:
C-
C-
C-   Library belongs: libana.a
C-
C-
C-   Calls: sda_brun, sda_erun, sda_evin
C-          ana_segm, ana_link, ana_prfit, ana_finde, ana_dcam, ana_fit,
C-          dcstatb, dcstatp, usda_anal, ana_todst                      
C-
C-   Created   JUNE-9-1992  Bogdan Niczyporuk
C-
C-   Called by sda_main (or any other package)
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
c#include "sdaparam.inc"
#include "sdaanal.inc"

#include "sdakeys.inc"
#include "sdadraw.inc"
#include "sdadigi.inc"
#include "sdaevgen.inc"
C
C
C External
      INTEGER iucomp, ETNLINK
C
C Subtoutine variables
      INTEGER iw(1000),ifevb,inter,ievent
      integer n,is
      REAL rw(1000)
C
C Local variables
C temporary
      real lnk_vect
      common/lnkvect/ lnk_vect(nlnkvect,ntrmx)
      integer ana(1000000)
C
      integer i, j, k, iwl3, istat(6)
      INTEGER isec,ifail,ind,ifdst,iret,ifirst
      real vect1(6),pin
      integer ecdigi2(3)
      real threshold(3)
      integer option(4)
      CHARACTER*24 dtimes
      DATA ifirst /1/
C
C Set to zero the number of the track candidates, reconstructed tracks,
C and tracks to be drawn
      call sda_cleananal()
      ntrdraw   = 0
      ntrdraw0  = 0
C
      ind = ETNLINK(IW,'HEAD',0)
      ievt = IW(ind+3)
C Begin of RUN
      IF(IW(ind+7).EQ.17 .OR. IW(ind+7).EQ.18) THEN
        ievent = ievt
        IF(ifirst.EQ.1) THEN
          ifirst = 0

          ifdst = 0
          IF(iucomp(4HWDST,levb,10) .NE. 0) ifdst = 1
C-EC
          print *,'EC: sending parameters ...'
          threshold(1) = 0.001 ! EcFitEdge_strip_threshold
          threshold(2) = 0.001 ! EcFitEdge_peak_threshold
          threshold(3) = 0.3   ! EcFitEdge_hit_threshold
          option(1) = 0 ! 1-default, 0-from database
          option(2) = 0 ! 1-default, 0-from database
          option(3) = 0 ! 1-default, 0-from database
          option(4) = 0 ! 1-default, 0-from database
C-EC
          CALL sda_brun(iw)
        ENDIF
        IF(IW(ind+7).EQ.18) THEN
          CALL GetASCIITime(IW(ind+4),dtimes)
          WRITE(6,1001) IW(ind+2),IW(ind+5),IW(ind+7),dtimes
 1001     FORMAT(/' BEGIN RUN#',I6,'  Type/Clas=',I4,'/',I2,' Date= ',A24/)
        ENDIF
        IF(ifdst.EQ.1) THEN
          CALL ETLCTL(IW,'E=','HEAD')
          CALL FWBOS(IW,2,'E',iret)
          CALL ETLDROP(IW,'E')
          CALL ETNGARB(IW)
        ENDIF
        RETURN
C End of RUN
      ELSE IF(IW(ind+7).EQ.20) THEN
        RETURN
C Data, unpack an event
      ELSE IF(IW(ind+7).GT.0 .AND. IW(ind+7).LT.16) THEN 
        ievent = ievt
        IF(ifirst.EQ.1) THEN
          ifirst = 0
          CALL sda_brun(iw)
        ENDIF
      ELSE
C Scaler's bank
        ievent = 0
        RETURN
      ENDIF                                 ! IF(IW(ind+7).EQ.17 .OR.
C
      CALL usda_anal(iw,rw,0,trkp,ntr_out,itag0,ev_out,tag_res)
      CALL dcstatb(1)
      IF(lanal(1).EQ.0) GO TO 999
C
C get HBT results from TRAK bank
      isec = zgcor(10)
      call sda_trak2anal(iw,rw,isec)
C
C Make final fits to the measured: {DCA +/- SIGdca}i=1,N for each track

cc      CALL ana_fit(iw,rw,ifail)
      call tmp2ana(ana,itrk0,beta0,itag0,ntagh,tzero,ntr_link,itr_level,
     &  itr_sect,lnk_clust,lnk_segm,itr_scid,itr_ecid,trk,trkp,segm,clust,
     &  nclust,nsegmc,tag_res,ntr_out,ev_out)
      call dcfit(iw,ana,4,ntr_out,ev_out,ifail,lnk_vect)

c      print *,'ifail=',ifail
      IF(ifail.EQ.0) RETURN
      CALL usda_anal(iw,rw,4,trkp,ntr_out,itag0,ev_out,tag_res)
      IF(ifail.GE.zcut(8)) CALL dcstatb(7)
C
C Number of reconstructed event
      IF(ntr_out.GE.zcut(8)) CALL dcstatb(8)
C
      ifail = 1

  999 CONTINUE
C
C At present write the complete input event as DST (testing)
      IF(ifdst.EQ.1) CALL ana_todst(iw,rw,ntr_out,ev_out,ntagh,tag_res)
C
C Print statistics here if Interective
      IF(inter.EQ.1) then
        CALL dcstatp
        CALL ICLRWK(0,1)
        CALL sda_dsect(1,  .008,  4.0, 10.6,ntr_link,itr_sect,segm,lnk_segm)
        CALL sda_dsect(4, -.008,  4.0, 10.6,ntr_link,itr_sect,segm,lnk_segm)
        CALL sda_dsect(2,  .008, 14.0, 10.6,ntr_link,itr_sect,segm,lnk_segm)
        CALL sda_dsect(5, -.008, 14.0, 10.6,ntr_link,itr_sect,segm,lnk_segm)
        CALL sda_dsect(3,  .008,  9.0,  4.0,ntr_link,itr_sect,segm,lnk_segm)
        CALL sda_dsect(6, -.008,  9.0,  4.0,ntr_link,itr_sect,segm,lnk_segm)
      ENDIF
C
      RETURN
      END







