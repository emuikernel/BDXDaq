h07054
s 00000/00000/00000
d R 1.2 01/11/19 19:02:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/obsolete/sda_io.F
e
s 00127/00000/00000
d D 1.1 01/11/19 19:02:38 boiarino 1 0
c date and time created 01/11/19 19:02:38 by boiarino
e
u
U
f e 0
t
T
I 1
C============================================================
      subroutine sda_path2anal(iw,isecm,nsect,lnk_vect)
c
      implicit none
      SAVE
#include "sdaanal.inc"
      integer iw(1000),isec,isecm,nsect
      real lnk_vect(nlnkvect,ntrmx)
c
      DO isec = isecm,nsect
        call proutput(iw,isec,segm(1,1,1,isec),
     &    clust(1,1,1,isec),nsegmc(1,1,isec),nclust(1,isec),
     &    ntr_link,itr_level,itr_sect,lnk_segm,lnk_clust,lnk_vect)
      ENDDO
c
      return
      end
C============================================================
      subroutine sda_anal2trak(iw,rw)
      implicit none
      SAVE
#include "sdaanal.inc"
      integer iw(1000)
      real rw(1000)
      integer ETNCREATE,i,j,k,l,ind,nw
      nw = 3 + ntr_link*(1+1+3+12*36+12*nplane)
      ind = ETNCREATE(iw,'TRAK',0,1,nw)
      if(ind.GT.0) then
        k=1
        iw(ind+k) = itrk0
        k=k+1
        rw(ind+k) = beta0
        k=k+1
        iw(ind+k) = ntr_link
        k=k+1
        do i=1,ntr_link
          iw(ind+k) = itr_sect(i)
          k=k+1
          iw(ind+k) = itr_level(i)
          k=k+1
          do j=1,3
            rw(ind+k) = itr_scid(j,i)
            k=k+1
          enddo
          do j=1,36
            do l=1,12
              rw(ind+k) = trk(l,j,i)
              k=k+1
            enddo
          enddo
          do j=1,nplane
            do l=1,12
              rw(ind+k) = trkp(l,j,i)
              k=k+1
            enddo
          enddo
        enddo
      endif
      return
      end
C============================================================
      subroutine sda_trak2anal(iw,rw,isec)
c
      implicit none
      SAVE
#include "sdaanal.inc"
c
      integer iw(1000),isec
      real rw(1000)
      integer ETNLINK,i,j,k,l,ind,nw
c
      ind = ETNLINK(iw,'TRAK',0)
      if(ind.GT.0) then
        k=1
        itrk0 = iw(ind+k)
c
      if(iw(ind+4).ne.isec) return
c
        k=k+1
        beta0 = rw(ind+k)
        k=k+1
        ntr_link = iw(ind+k)
        k=k+1
        do i=1,ntr_link
          itr_sect(i) = iw(ind+k)
          k=k+1
          itr_level(i) = iw(ind+k)
          k=k+1
          do j=1,3
            itr_scid(j,i) = rw(ind+k)
            k=k+1
          enddo
          do j=1,36
            do l=1,12
              trk(l,j,i) = rw(ind+k)
              k=k+1
            enddo
          enddo
          do j=1,nplane
            do l=1,12
              trkp(l,j,i) = rw(ind+k)
              k=k+1
            enddo
          enddo
        enddo
      endif
ccc      print *,'k=',k
c
      return
      end
C============================================================
      subroutine sda_cleananal()
c
      implicit none
      SAVE
#include "sdaanal.inc"
      integer it
c
      ntr_link  = 0
      ntr_out   = 0
      itrk0     = 0
      DO it = 1,ntrmx
        itr_level(it) = 0
      ENDDO 
c
      return
      end
E 1
