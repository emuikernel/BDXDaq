C======================================================================
      SUBROUTINE sim_anal(iw,rw,ifevb,inter,ievent,iwl3,istat)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : The main routine for an event reconstruction. 
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
C-   Calls: sda_brun, dcerun, sda_evin
C-          ana_segm, ana_link, ana_prfit, ana_finde, ana_dcam, ana_fit,
C-          dc_statb, dc_statp, usda_anal, ana_todst                      
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
#include "sdaparam.inc"
#include "sdakeys.inc"
#include "sdadraw.inc"
#include "sdadigi.inc"
#include "sdaevgen.inc"
C
C Subtoutine variables
      INTEGER iw(1000),ifevb,inter,ievent,iwl3,istat(6)
c      integer ntr_link1
      REAL rw(1000)
C
C Local variables
      integer segm(12,nsgmx,6,6),clust(nsgcmx,nclmx,6,6),
     &                nclust(6,6),nsegmc(nclmx,6,6)
c
c      integer it, iter
c      integer is, j, n
      INTEGER i, k, isec,isecm,nsect, ifail
      real vect1(6),pin
      integer ecdigi2(3)
      DATA nsect  /6/
C Set to zero the number of the track candidates, reconstructed tracks,
C and tracks to be drawn
c      ntr_link  = 0
c      ntr_link1 = 0
c      ntr_out   = 0
c      ntrdraw   = 0
c      ntrdraw0  = 0
c      itrk0     = 0
c      DO it = 1,ntrmx
c        itr_level(it) = 0
c      ENDDO 
C
      ievent = ievt
      do i=1,6
        istat(i) = 0
      enddo
C
      IF(inter.EQ.1) THEN
        Ndbg  = 999999
***        WRITE(6,*)
***        WRITE(6,*) '                  *** Event',ievt,' Triggered ***'
      ENDIF
C
c following call replaced by dchfill(); adjust if want histograms here
c      CALL usda_anal(iw,rw,0,trkp,ntr_out,itag0,ev_out,tag_res)
C
      CALL dc_statb(1)
      IF(lanal(1).EQ.0) GOTO 999
C
      IF(ltrig(4).EQ.0) THEN
        isecm = 1
        nsect = 6
      ELSE
        isecm = ltrig(4)
        nsect = ltrig(4)
      ENDIF
C ?????????????????????????????????
c      do k=1,6
c        do is=1,6
c          do j=1,nsgmx
c            do n=1,12
c              segm(n,j,is,k) = 0 !!!!!!!!!!!!!!!!!!
c            enddo
c          enddo
c          nclust(is,k) = 0
c        enddo
c      enddo
C ?????????????????????????????????
      DO isec = isecm,nsect 
        CALL sim_segm(isec,ifail,segm,clust,nclust,nsegmc)
c      print *,'after sim_segm ifail=',ifail
        IF(ifail.NE.0) THEN
          CALL dc_statb(2)
          DO k=1,6
            vect1(k) = dc_trps(k,1,1)
          ENDDO
c      print *,'sim_anal: vect1=',vect1

c          if(sc_ndig(1,isec).GT.0) then
c            print *,'ntof=',sc_ndig(1,isec),' -> ',sc_digi(1,1,1,isec)
c          endif

          if(ec_ndig(1,isec).GT.0) then
c            print *,'nu=',ec_ndig(1,isec),ec_digi(1,1,1,isec)
c            print *,'nv=',ec_ndig(2,isec),ec_digi(1,1,2,isec)
c            print *,'nw=',ec_ndig(3,isec),ec_digi(1,1,3,isec)
            ecdigi2(1) = ec_digi(1,1,1,isec)*2
            ecdigi2(2) = ec_digi(1,1,2,isec)*2
            ecdigi2(3) = ec_digi(1,1,3,isec)*2
          else
            ecdigi2(1) = 0
            ecdigi2(2) = 0
            ecdigi2(3) = 0
          endif
c          print *,'ecdigi2=',ecdigi2

c          print *,'charge=',evin(9,1)
          pin = evin(7,1) * evin(9,1)

          CALL sim_link(isec,vect1,pin,sc_digi(1,1,1,isec),ecdigi2,
     &                   ifail,iwl3,istat,segm,clust,nclust,nsegmc)

        ENDIF
      ENDDO
C
  999 CONTINUE
cC
cC Print statistics here if Interective
c      IF(inter.EQ.1) then
cc        CALL dc_statp
c        print *,'road: ',istat
c        CALL ICLRWK(0,1)
c        CALL sda_dsect(1,0.025,6.0,5.0,ntr_link,itr_sect,segm,lnk_segm)
c        CALL IUWK(0,1)
c        read *,iter
c      endif
C
C
 1001 FORMAT(/' BEGIN RUN#',I6,'  Type/Clas=',I4,'/',I2,' Date= ',A24/)
C
      RETURN
      END
