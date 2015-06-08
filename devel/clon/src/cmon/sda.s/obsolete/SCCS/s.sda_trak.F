h14051
s 00000/00000/00000
d R 1.2 01/11/19 19:02:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_trak.F
e
s 00091/00000/00000
d D 1.1 01/11/19 19:02:40 boiarino 1 0
c date and time created 01/11/19 19:02:40 by boiarino
e
u
U
f e 0
t
T
I 1
      SUBROUTINE sda_trak(iw,rw,lanal1,zcut5,zcut8)
C----------------------------------------------------------------------
C
C----------------------------------------------------------------------
      IMPLICIT NONE
      SAVE
#include "sdaanal.inc"
C temporary
      real lnk_vect
      common/lnkvect/ lnk_vect(nlnkvect,ntrmx)
C
      INTEGER iw(1000),lanal1
      REAL rw(1000),zcut5,zcut8
C
      INTEGER I,IFAIL
      integer ana(1000000)
C
      ifail = 0
C
C Fitting to the wire positions only (no timing involved)


cc      CALL ana_prfit(iw,rw,lnk_vect,ifail)

      call tmp2ana(ana,itrk0,beta0,itag0,ntagh,tzero,ntr_link,itr_level,
     &  itr_sect,lnk_clust,lnk_segm,itr_scid,itr_ecid,trk,trkp,segm,clust,
     &  nclust,nsegmc,tag_res,ntr_out,ev_out,lnk_vect)
      call dcfit(iw,ana,2,ntr_out,ev_out,ifail)




      IF(ifail.EQ.0) RETURN
C
c      IF(ifail.GE.zcut8) CALL dcstatb(4)
      do i=1,ifail
        call dcstatb(4)
      enddo
C
C Identify electron with CC,SC & EC  or  beta of any particle with ST & SC  
cc      CALL ana_finde(iw,rw,ifail)
      call dcfinde(iw,ana,ifail)
c      print *,'sda_anal: ',ifail,zcut5
      IF(ifail.GE.zcut5) CALL dcstatb(5)
      CALL usda_anal(iw,rw,2,trkp,ntr_out,itag0,ev_out,tag_res)
C
C
      IF(lanal1.LT.3) RETURN
C Check if electron  or beta any of particle (photoproduction) has been found 
      IF(ifail.LT.zcut5) RETURN
C
C Derive DCA and errors for hits on candidate tracks and resolve L-R Ambiguity
cc      CALL ana_dcam(ana,ifail)
      CALL dcdcam(ana,ifail)
c      call ana2tmp(ana,itrk0,beta0,itag0,ntagh,tzero,ntr_link,itr_level,
c     &  itr_sect,lnk_clust,lnk_segm,itr_scid,itr_ecid,trk,trkp,segm,clust,
c     &  nclust,nsegmc,tag_res,ntr_out,ev_out,lnk_vect)

      IF(ifail.EQ.0) RETURN
      CALL usda_anal(iw,rw,3,trkp,ntr_out,itag0,ev_out,tag_res)
      IF(ifail.GE.zcut8) CALL dcstatb(6)
C
C save HBT results to TRAK bank
      call sda_anal2trak(iw,rw)
C
C get HBT results from TRAK bank
***      call sda_trak2anal(iw,rw,0)
C
C                   *** LEVEL = 4 ***
C
C Make final fits to the measured: {DCA +/- SIGdca}i=1,N for each track
      IF(lanal1.LT.4) RETURN



cc      CALL ana_fit(iw,rw,ifail)
      call dcfit(iw,ana,4,ntr_out,ev_out,ifail)




      IF(ifail.EQ.0) RETURN
      CALL usda_anal(iw,rw,4,trkp,ntr_out,itag0,ev_out,tag_res)
      IF(ifail.GE.zcut8) CALL dcstatb(7)
C
C Number of reconstructed event
      IF(ntr_out.GE.zcut8) CALL dcstatb(8)
C
      ifail = 1
      RETURN
      END
E 1
