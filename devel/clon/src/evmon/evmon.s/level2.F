c  level2.f

c  ejw, 19-jan-1999


c-----------------------------------------------------------


      subroutine level2_init

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine level2_book

      implicit none
      integer i
      save


c  executable code:
c  ----------------

      call hmdir('//PAWC/LEVEL2','S')
      call hcdir('//PAWC/LEVEL2',' ') 
      call hbook2(1011,'Segment TDC-1',140,0.,7000.,36,0.5,36.5,0.)
      call hbook2(1012,'Segment TDC-2',140,0.,7000.,36,0.5,36.5,0.)
      call hbook2(1021,'L2summary TDC-1',140,0.,7000.,9,0.5,10.5,0.)
      call hbook2(1022,'L2summary TDC-2',140,0.,7000.,9,0.5,10.5,0.)
      call hbook1(1101,'L2Pass Hardware',7,0.5,7.5,0.)
      call hbook1(1102,'L2Pass Software',7,0.5,7.5,0.)
      call hbook1(1111,'L2Pass Hard*Soft',7,0.5,7.5,0.)
      call hbook1(1112,'L2Pass Hard/Soft',7,0.5,7.5,0.)
      call hbook2(1310,'Nhits in Summary TDCs',10,0.,10.,10,0.,10.,0.)
      do i=1,6
         call hbook2(1300+i,'Nhits in Sector TDCs',10,0.,10.,10,0.,10.,0.)
      enddo
      print *,' Level2 histograms booked'
      return
      end


c------------------------------------------------------------------------------


      subroutine level2_bor

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine level2_fill

      implicit none
      include 'bcs.inc'
      include 'l2_hit.inc'
      integer mlink
      integer i, j, sec, k,kk
      integer nsl(6,6),nsk(9),l2_3_5(6)
      integer err,ind,nrow,id,edg,tdc,indx
      integer*2 I16(2)            ! 16 bits
      integer*4 I32(1)            ! 32 bits
      equivalence (I16(1), I32(1))
      integer head,evtyp
      real xax, yax
      save



c  executable code:
c  ----------------


c  ignore scaler events and events with no HEAD bank
      head=mlink(iw,'HEAD',0)
      if(head.le.0)then
         return
      elseif (iw(head+7).eq.0) then
         return
      endif
      evtyp=iw(head+5)
c get the Level 2 bank - TL2
c L2H stores information for on found segments in each superlayer for 
c given sector (total of 30 multihit TDC's). 
c Only first 4 hits for each TDC channel will be stored.
c
c Do the initialization.

      do j=1,6
         l2_3_5(j)=0
        do k=1,6
          nsl(k,j)=0
          do i=1,4
            sl2h(k,j,i)=0
          enddo
        enddo
      enddo
      do k=1,9
         nsk(k)=0
         do i=1,4
            l2sec(k,i)=0
         enddo
      enddo
      nsl0=0
      nsl1=0
c
      ind=mlink(iw,'TL2 ',0)
      if(ind.gt.0)then
        err=0
        nrow=iw(ind-4)
        indx=ind+1
        DO i=1,nrow
          i32(1)=iw(indx)
          indx=indx+1
          kk = I16(1)
          id  = mod(kk,256)
          edg = kk/256 +1
          sec = id/16
          id  = mod(id,16)
          tdc=i16(2)
          if(sec.gt.0 .AND. sec.LT.7) then
            if(id.lt.1 .or. id.gt.6) goto 100
            nsl(sec,id)=max(nsl(sec,id),edg)
            if(edg.gt.4)goto 100
            sl2h(sec,id,edg)=tdc
          ELSE
            if(id.lt.1 .or. id.gt.9) goto 100
            nsk(id)=max(nsk(id),edg)
            if(edg.gt.4)goto 100
            l2sec(id,edg)=tdc
          ENDIF
 100    continue
        ENDDO
      endif
      call hcdir('//PAWC/LEVEL2',' ')
      do j=1,6
        do i=1,6
           call hf2(1300+j,float(i),float(nsl(j,i)),1.)
          nsl0=max(nsl0,nsl(j,i))
        enddo
      enddo
      do j=1,9
         call hf2(1310,float(j),float(nsk(j)),1.)
         nsl1=max(nsl1,nsk(j))
      enddo
c  switch to level2 directory
      do i=1,6
        do j=1,6
          if(sl2h(i,j,1).gt.0)then
             l2_3_5(i)=l2_3_5(i)+1
            xax=float(sl2h(i,j,1))
            yax=float((i-1)*6+j)
            call hf2(1011,xax,yax,1.)
            xax=float(sl2h(i,j,2))
            call hf2(1012,xax,yax,1.)
          endif
        enddo
        if(l2_3_5(i).gt.2)call hf1(1102,float(i),1.)
      enddo
      do i=1,9
        if(l2sec(i,1).gt.0)then
          xax=float(l2sec(i,1))
          yax=float(i)
          call hf2(1021,xax,yax,1.)
          xax=float(l2sec(i,2))
          call hf2(1022,xax,yax,1.)
          if(i.lt.7)then
             call hf1(1101,float(i),1.)
             if(l2_3_5(i).gt.2)call hf1(1111,float(i),1.)
          endif
        endif
      enddo
c  switch back to main directory
      call hcdir('//PAWC',' ')

      return
      end


c------------------------------------------------------------------------------


      subroutine level2_eor

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine level2_done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------



