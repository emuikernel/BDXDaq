C======================================================================
      SUBROUTINE ana_tag(iw,rw,ntagh,tag_res)
C----------------------------------------------------------------------
C-
C-   Purpose and Methods : short analysis of tagger data
C-
C-   Inputs  : none
C-   Outputs :
C-
C-   fills tag_res array:
C-                       ntagh :  number of valid tagger hits
C-                                (T & E counters successfully matched)
C-                       tag_res(1,n) :  binid for E-counter (1...767)
C-                       tag_res(2,n) :  binid for T-counter (1...121) 
C-                       tag_res(3,n) :  energy for E-counter (0.209 ... 0.953)
C-                       tag_res(4,n) :  energy for T-counter (0.206 ... 0.954)
C-                       tag_res(5,n) :  time for E-counter hit 
C-                       tag_res(6,n) :  time for T-counter hit (mean value)
C-                       tag_res(7,n) :  time for T-counter hit (RF corrected) 
C-
C-                     binids:    (2*i-1) for hit in single counter (i) 
C-                                (2*i) for hit in adjacent counters (i,i+1)
C-                                real counter id : (binid+1)/2
C-                     energies : tagger energy information 
C-                                as ratios of primary electron energy 
C-                     times :    tagger time information (w.r.t. CLAS-trigger)
C-   Controls:
C-
C-   Library belongs: 
C-
C-   Calls: 
C-
C-   Created   12-JUL-1998   Franz Klein
C-
C-   Called by ana_finde
C-
C--------1---------2---------3---------4---------5---------6---------7--
      IMPLICIT NONE
C-----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdadigi.inc"
#include "sdatagcal.inc"

      INTEGER IW(1000),ntagh
      REAL rw(1000),tag_res(7,10)

C  local variables

      INTEGER  TAG_MAX_HITS
      PARAMETER ( TAG_MAX_HITS = 10 )
      INTEGER  hitno(TAG_MAX_HITS)

      INTEGER  MAX_hitE, MAX_hit
      PARAMETER (MAX_hitE=200, MAX_HIT=20)

      INTEGER  nhit_T, nhit_E, nhits1
      INTEGER  tag_Tbin(MAX_hit), tag_Ebin(MAX_hit)
      REAL  tag_Tmean(61), tag_Etime(MAX_hit) 
      REAL  tag_Tdiff(61), tag_Ttime(MAX_hit)
      INTEGER  iptr_Eid(4,384), nhit_Eid(384)
      INTEGER  tag_hitno(61), tag_nhT(0:61)
      REAL  tag_Etdc(MAX_hitE)

      REAL  xval1, xval2, rf(3)
      INTEGER  id, id1, id2, i, j, k, k1, k2, ih
      integer ind, etNcreate, ncol_tagr
      data ncol_tagr/6/

c-------------------------------------------------------------
c
c  initialize output variables
c
      ntagh   = 0
c
c  initialize local arrays
c  
      nhits1   = 0
      nhit_T   = 0
      nhit_E   = 0
      do j=1,384
         nhit_Eid(j) = 0
      enddo
      do j=1,61
         tag_Tmean(j) = 0.0
         tag_nhT(j)   = 0
      enddo
      tag_nhT(0) = 0
c
c  T counters
c
c  (a) cut on reasonable TDC values (converted to time)
c         for left and right PMT
c
      do j=1,tgt_ndig
         id   = tgt_digi(1,j)
         xval1 = tgt_digi(2,j)*slopeTL(id) - TpeakL(id) ! left
         xval2 = tgt_digi(3,j)*slopeTR(id) - TpeakR(id) ! right
c
c  if one PMT always missing set it here
         if (tagT_status(id).EQ.1) xval1 = xval2
         if (tagT_status(id).EQ.2) xval2 = xval1
         if (xval1.GT.TAG_TCUT(1) .AND. xval1.LT.TAG_TCUT(2) .AND.
     &       xval2.GT.TAG_TCUT(1) .AND. xval2.LT.TAG_TCUT(2)) then
            nhit_T = nhit_T +1
            tag_Tmean(id) = (xval1+xval2)/2. 
         endif
      enddo
c
c  (b) take T counters with single hits if no 'good' T-counter 
c
      if (nhit_T.EQ.0) then
         do j=1,tgt_ndig
            id = tgt_digi(1,j)
            xval1 = tgt_digi(2,j)*slopeTL(id) - TpeakL(id) ! left
            xval2 = tgt_digi(3,j)*slopeTR(id) - TpeakR(id) ! right
            if (xval1.GT.TAG_TCUT(1) .AND. xval1.LT.TAG_TCUT(2)) then
               nhit_T = nhit_T +1
               tag_Tmean(id) = xval1
            elseif (xval2.GT.TAG_TCUT(1).AND.xval2.LT.TAG_TCUT(2)) then
               nhit_T = nhit_T +1
               tag_Tmean(id) = xval2
            endif
         enddo
c
c  (c) exit if no valid T-counter entry  
c
         if (nhit_T.EQ.0) goto 900
      endif
c
c  E counters:
c
      do j=1,tge_ndig
         id    = tge_digi(1,j)
         xval1 = tge_digi(2,j) * slopeE - Epeak(id)
c ----- cut for reasonable time window
c ----- regard up to 4 hits per counter
         if (nhit_E.LT.MAX_hitE .AND. nhit_Eid(id).LT.4 .AND.
     &       xval1.GT.TAG_TCUT(1).AND. xval1.LT.TAG_TCUT(2)) then 
            nhit_E = nhit_E +1
            tag_Etdc(nhit_E) = xval1
            nhit_Eid(id) = nhit_Eid(id) +1
            iptr_Eid(nhit_Eid(id),id) = nhit_E
         endif
      enddo
c
c   save only those E-counter entries which match to T-counters
c
      if (nhit_E.EQ.0) goto 120
c
c  (1) for individual T-counters
c
      do j=1,61
         if (tag_Tmean(j).EQ.0.0) goto 100
         k1 = t1bin(j)
         k2 = t2bin(j)
         id1 = -1
         do k=k1,k2
            ih = 0
            xval2 = ET_window
            do i=1,nhit_Eid(k)
               xval1 = ABS(tag_Etdc(iptr_Eid(i,k))-tag_Tmean(j))
               if (xval1.LT.xval2) then
                  xval2 = xval1
                  ih    = i
               endif
            enddo
            if (ih.GT.0) then
               if (id1.EQ.k-1) then
                  tag_Ebin(nhits1)  = 2*id1
                  if (xval2.LT.tag_tdiff(nhits1)) then
                     tag_tdiff(nhits1) = xval2
                     tag_Etime(nhits1) = tag_Etdc(iptr_Eid(ih,k))
                  endif
               else
                  nhits1 = nhits1 +1
                  tag_nhT(j) = tag_nhT(j) +1
                  tag_hitno(j) = nhits1
                  tag_Tbin(nhits1)  = j*2-1
                  tag_Ebin(nhits1)  = 2*k-1
                  tag_tdiff(nhits1) = xval2
                  tag_Ttime(nhits1) = tag_Tmean(j)
                  tag_Etime(nhits1) = tag_Etdc(iptr_Eid(ih,k))
                  id1 = k
               endif
            endif
         enddo
c
c  (2) for hits in neighbouring T-counters
c      (except T-counters 18-20: overlap up to 2 counters)
c
         if (j.EQ.20 .AND. tag_nhT(18).GT.0) then
            tag_hitno(19) = tag_hitno(18) + tag_nhT(19)
            tag_nhT(19) = tag_nhT(18) + tag_nhT(19)
         endif

         if (tag_nhT(j)*tag_nhT(j-1).EQ.0) goto 100
         do i=1,tag_nhT(j)
            k1=tag_hitno(j) -i +1
            id = MOD(tag_Ebin(k1),2)
            do k=1,tag_nhT(j-1)
               k2=tag_hitno(j-1) -k +1
               if (IABS(Tag_Ebin(k1)-tag_Ebin(k2)).LT.2) then
                  if (id.EQ.1) then
                     id1 = k2
                     id2 = k1
                  else
                     id1 = k1
                     id2 = k2
                  endif
                  tag_Ebin(id1) = (tag_Ebin(id1)+tag_Ebin(id2))/2
                  tag_Ebin(id2) = -tag_Ebin(id2)
                  tag_Tbin(id1) = (j-1)*2
                  if (tag_tdiff(id2).LT.tag_tdiff(id1)) then
                     tag_Ttime(id1) = tag_Ttime(id2)
                     tag_Etime(id1) = tag_Etime(id2)
                     tag_tdiff(id1) = tag_tdiff(id2)
                  endif
               endif
            enddo
         enddo
 100     continue
      enddo
c
c  (3) no coincidence ? take only T-counters
c
 120  continue
      if (nhits1.EQ.0) then
         j=1
         do while (j.LE.61)
            if (tag_Tmean(j).NE.0.0) then
               k1 = 0  
               if (j.EQ.61 .OR. tag_Tmean(j+1).EQ.0.0) then
                  if (ABS(tag_Tmean(j)-TAG_RF_MEAN).LT.
     &                                         TAG_RF_10SIGMA) then
                     k  = j
                     k1 = 2*j-1
                  endif
               else
                  if (ABS(tag_Tmean(j)-TAG_RF_MEAN).LT.
     &                                           TAG_RF_10SIGMA) then
                     k = j
                     k1= 2*j
                     j = j+1
                  elseif (ABS(tag_Tmean(j+1)-TAG_RF_MEAN).LT.
     &                                           TAG_RF_10SIGMA) then
                     k = j+1
                     k1= 2*j
                     j = j+1
                  endif
               endif
               if (k1.GT.0) then
                  nhits1 = nhits1 +1
                  tag_Tbin(nhits1) = k1
                  tag_Ebin(nhits1) = 0
                  tag_Etime(nhits1) = 0.0
                  tag_Ttime(nhits1) = tag_Tmean(k)
                  tag_Tdiff(nhits1) = ABS(tag_Tmean(k)-TAG_RF_MEAN)
               endif
            endif
            j = j+1
         enddo
      endif
c
c  (4)  get RF times
c
      rf(1) = bm_digi(2,6) * RF_SLOPE(1)
      rf(2) = bm_digi(2,7) * RF_SLOPE(2)
      if (rf(1).GT.15. .AND. rf(1).LT.78.) then
         rf(3) = rf(1)
      else
         rf(3) = rf(2) + RF_OFFSET
      endif
c
c  (4)  correct timing for RF and copy to output
c
      k1=nhits1
      do while (k1.GT.0 .AND. (tag_Ebin(k1).LE.0 .OR.
     &        ABS(tag_Tmean(k1)-TAG_RF_MEAN).GT.TAG_RF_10SIGMA) )
         k1=k1-1
      enddo
      if (k1.GT.0) then
         ntagh = 1
         hitno(1) = k1
      endif
      do j=1,nhits1
         if (j.NE.k1 .AND. tag_Ebin(j).GE.0 .AND. 
     &                                ntagh.LT.TAG_MAX_HITS) then
            ntagh = ntagh +1
            hitno(ntagh) = j
         endif
      enddo

      do k=1,ntagh
         j=hitno(k)
         tag_res(1,k) = tag_Ebin(j)
         tag_res(2,k) = tag_Tbin(j)
         tag_res(3,k) = Eaver(tag_Ebin(j))
         tag_res(4,k) = (Tbound(tag_tbin(j))+Tbound(tag_tbin(j)+1))/2.
         tag_res(5,k) = tag_Etime(j)
         tag_res(6,k) = Tag_Ttime(j) -TagTCi(tag_Tbin(j))
         xval1 = (tag_res(6,k) - rf(3)  -1.002)/2.004
         tag_res(7,k) = rf(3) + IFIX(xval1)*2.004
      enddo

C boy: fill TAGR bank

      ind = etNcreate(iw,'TAGR', 0, ncol_tagr, ntagh)
      do k=1,ntagh
        rw(ind+1) = tag_res(3,k)
c      print *,'tag_res(3,',k,')=',tag_res(3,k)
        rw(ind+2) = 0.0
        rw(ind+3) = 0.0
        iw(ind+4) = 0
        iw(ind+5) = 0
        iw(ind+6) = 0
        ind = ind + ncol_tagr
      enddo

C boy


 900  continue

c      print *,'Edev(1)=',Edev(1)

      IF (lprnt(3).GT.0) THEN
         write(6,*)'  Ebin, Tbin, Een, Ten, Etime, Ttime, rf_corr'
         do i=1,ntagh
            write(6,*) (tag_res(j,i),j=1,7)
         enddo
         do i=1,nhits1
            write(6,*)'hit',i,tag_tbin(i),tag_ebin(i),
     &               tag_ttime(i),tag_etime(i),tag_tdiff(i)
         enddo
      ENDIF
      END

c--------------------------------------------------------------------
