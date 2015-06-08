C======================================================================
      SUBROUTINE sim_segm(isec,ifail,segm,clust,nclust,nsegmc)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : Classify hits into segments and clusters
C-
C-
C-   Inputs  : isec  - the sector to be analyzed
C-             ifail - control flag
C-
C-    Controls: lanal(2)      - patr. switch 2 must be on for writing
C-                              new templates and no background must
C-                              be ensured (see the data-card).
C-
C-
C-   Library belongs    : libana.a
C-
C-   Calls              : none
C-
C-   Created   JUNE-8-1992  Bogdan Niczyporuk
C-   Modified   JAN-9-1999  Serguei Boiarinov
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
#include "sdaparam.inc"
C lanal(), ifsim, zcut(), lprnt()
#include "sdakeys.inc"
C dc_ndig(,), dc_digi(,,,)
#include "sdadigi.inc"
C dc_stat(,,)
#include "sdacalib.inc"
C
C Subroutine variables
      INTEGER isec, ifail
      integer segm(12,nsgmx,6,6),clust(nsgcmx,nclmx,6,6),
     &                nclust(6,6),nsegmc(nclmx,6,6)
C
      INTEGER no_hit, nlay, nwmx, nfound
      PARAMETER (no_hit = 0, nlay = 6, nwmx = 192, nfound = 200)
C
C Local variables
      INTEGER i, j, k, m, n, Level, nhit_in_sl
      INTEGER icl,ncl, nsgm, isgc, nsgc, ifoundold, ifound
      INTEGER ih, il, ip, ilmin, is, iw, iw3, iwc, its, isg, isum              
      INTEGER la, nd, nd3, max_hit_seg, min_hit_seg
      INTEGER nw(nlay)
      INTEGER NLinSL(6), NWL(6)
      INTEGER*2 idtime(6,192)
C
      integer nsegm(6,6)
C
      DATA NLinSL / 4, 6, 6, 6, 6, 6 /
      DATA NWL / 130, 142, 192, 192, 192, 192 /
c
c cleanup
c
      do k=1,6
        do is=1,6
          nsegm(is,k) = 0
        enddo
      enddo
C
C Loop over the superlayers in a Sector
C
      ifail = 0
      DO 20 is = 1,6
C
C Initialize accounting for segment and cluster information
C
        nsgm = 0
        nclust(is,isec) = 0
        DO j = 1,nclmx
          nsegmc(j,is,isec) = 0
        ENDDO
C
C
        ilmin = (is-1)*nlay
        max_hit_seg = NLinSL(is)
        min_hit_seg = nlay
        max_hit_seg = nlay
        nd3 = dc_ndig(ilmin+3,isec)
        IF(nd3.LE.0) GOTO 20
        nd3 = 1
C
C Initialization of idtime(6,192) for given SuperLayer
C
        CALL MEMSET(idtime(1,1),no_hit,nwmx*nlay/2)
C
C Fill "idtime" array with hits for a given superlayer "is"
C
        nhit_in_sl = 0
        DO 8 la = 1,nlay
          il = ilmin + la
          nd = dc_ndig(il,isec)
          IF (la .GT. max_hit_seg) nd = 0
          IF(nd.EQ.0) GOTO 8
          nhit_in_sl = nhit_in_sl + 1
          DO ih = 1,nd
            iw = dc_digi(1,ih,il,isec)
            idtime(la,iw) = dc_digi(2,ih,il,isec)
cc      print *,'bla1: la=',la,' iw=',iw,' idtime=', idtime(la,iw)
            IF(ifsim.EQ.1.AND.idtime(la,iw).LT.0) idtime(la,iw) = 2000
          ENDDO
 8      CONTINUE
C
C Check if at least there are "min_hit_seg" in a SuperLayer 
        IF(nhit_in_sl .LT. min_hit_seg) THEN
cc      print *,'goto20'
          GOTO 20
        ENDIF
C
C Loop over the hits(training) or all wires(analysis) in layers #2 & #3
C
        ifound = 0
        ifoundold = 0
        DO 14 ih = 1,nd3
          iw3 = dc_digi(1,ih,ilmin+3,isec)
          IF(idtime(3,iw3).EQ.no_hit) GOTO 14
C
C get list of segments
C
cobsolete
          call prsegmlist(iw3,idtime,ifound,min_hit_seg)

C
C Store new segment template
C
        IF(ifound.EQ.0) THEN
C First check if there are hits in all layers of SL
          do la = 1,nlay
            il = ilmin + la
cc      print *,'bla2: la=',la,' dc_ndig(il,isec)=',dc_ndig(il,isec)
            IF(dc_ndig(il,isec).EQ.0) GOTO 20
          enddo
C Assuming no background when training, therefore use the 1st hit
          do la = 1,nlay
            il = ilmin + la
            nw(la) = dc_digi(1,1,il,isec)
          enddo
cc      print *,'bla3: nw=',nw

cobsolete
          call prupdatesegm(nw)
cobsolete
          call prsegmlist(iw3,idtime,ifound,min_hit_seg)

        ENDIF








   14   CONTINUE
C
C Cleaning segments by setting flag "nwflag(ifound)=1 for good segments
C and Search for Segment Clusters

cc      print *,'ifound=',ifound
        IF(ifound .GT. 0) THEN
          ifoundold = 0
cobsolete
          call cleansegmlist(max_hit_seg,min_hit_seg,
     &             idtime,ifoundold,ifound,
     &             nsgm,segm(1,1,is,isec))

C
C Cluster finding. Store results cluster arrays (1-st cluster).
C
          DO 12 isg = 1,nsgm
            IF(isg .EQ. 1) THEN
              nclust(is,isec)    = 1
              nsegmc(1,is,isec)  = 1
              clust(1,1,is,isec) = isg
cc      print *,'1'
              GOTO 12
            ENDIF
C Loop over existing clusters
            ncl = nclust(is,isec)
            DO icl = 1,ncl
C Loop over segments in each found cluster
              nsgc = nsegmc(icl,is,isec)
cc      print *,'2: nsgc=',nsgc
              DO isgc = 1,nsgc
                m = clust(isgc,icl,is,isec)
                k = 1
                DO la = 1,nlay
                  iwc = segm(k,m,is,isec)
                  iw  = segm(k,isg,is,isec)
                  IF(iw.GT.0 .AND. iw.EQ.iwc) THEN
                    IF(nsegmc(icl,is,isec).LT.nsgcmx) THEN       
                      nsegmc(icl,is,isec) = nsegmc(icl,is,isec) + 1
                      clust(nsegmc(icl,is,isec),icl,is,isec) = isg
                      GOTO 12
                    ENDIF
                  ENDIF
                  k = k + 2
                ENDDO
              ENDDO       ! End of loop over isgc
            ENDDO         ! End of loop over icl
C Create a new cluster
cc      print *,'nclust=',nclust,' nclmx=',nclmx
            IF(nclust(is,isec).LT.nclmx) THEN
              nclust(is,isec) = nclust(is,isec) + 1
              n = nclust(is,isec)
              nsegmc(n,is,isec) = 1
              clust(1,n,is,isec) = isg
            ENDIF
 12       CONTINUE      ! End of DO over nsgm

        ENDIF           ! End of IF (ifound.GT.0)
ccC
ccC Store new segment template
ccC
cc        IF(ifound.EQ.0) THEN
ccC First check if there are hits in all layers of SL
cc          do la = 1,nlay
cc            il = ilmin + la
cc            IF(dc_ndig(il,isec).EQ.0) GOTO 20
cc          enddo
ccC
ccC Assuming no background when training, therefore use the 1st hit
cc          do la = 1,nlay
cc            il = ilmin + la
cc            nw(la) = dc_digi(1,1,il,isec)
cc          enddo
cc      print *,'update ...'
cc          call prupdatesegm(nw)
cc          GOTO 20                  ! an early exit point
cc        ENDIF

        nsegm(is,isec) = nsgm

   20 CONTINUE                      ! End of loop over superlayers
C
C Require clusters in all 6 super layers, if yes set ifail = 1
C
      isum  = 0
      DO i = 1,6
        IF(nclust(i,isec).GT.0 ) isum  = isum  + 1
      ENDDO
      IF(isum .GE. IFIX(zcut(2))) ifail = 1
cc      print *,'bla4: isum=',isum,' ifail=',ifail
C
C Report clusters/segments information for each Superlayer
C
      IF(lprnt(3).EQ.1) THEN
        Level = 1
        IF(isum.GT.0) WRITE(6,31) Level,isec
        DO 30 is = 1,6
          IF(nclust(is,isec).EQ.0) GOTO 30
          WRITE(6,32) is,nsegm(is,isec),nclust(is,isec)
          WRITE(6,33)
          DO isg = 1,nsegm(is,isec)
            DO icl = 1,nclust(is,isec)
              nsgc = nsegmc(icl,is,isec)
              DO i = 1,nsgc
                IF(isg.EQ.clust(i,icl,is,isec)) THEN
                  k = icl
                  GOTO 29
                ENDIF
              ENDDO                 
            ENDDO
 29         CONTINUE
            WRITE(6,34) isg,(segm(i,isg,is,isec),i=1,12,2),k
            WRITE(6,35) isg,
     & (IABS(segm(i,isg,is,isec)),i=1,12,2),k
          ENDDO
 30     CONTINUE
      ENDIF
C
 31   FORMAT(//' LEVEL=',I2,'  Sector=',I2,'  (DC Segments & Clusters)')
 32   FORMAT(/' SLayer=',I2,'  Nsegm=',I2,'  Nclus=',I2)
 33   FORMAT(' isgm    La1  La2  La3  La4  La5  La6   iclus')
 34   FORMAT(I5,2X,6I5,4X,I4)
 35   FORMAT(I5,3X,6('(',I3,')'),3X,I4)
C
  999 CONTINUE
C
      RETURN
      END




