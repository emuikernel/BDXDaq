c  dc.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine dc0_book
      character*80 title,title0,title1
      character*8 fstring(6)
      integer i
      
      do i = 1,6
        write(fstring(i),20) 'SECTOR ',i
      enddo
        
      call hmdir('//PAWC/DC0','S')
      title0 =  'DC0;SECTOR;'
      title1 = ' DC0;WIRE;LAYER'
      
      call hbook1(1,title0,6,1.,7.,0.)
      do i = 1,6
        title = fstring(i)//title1
        call hbook2(i*10+1,title,192,1.,193.,36,1.,37.,0.)
      enddo
      
      call hcdir('//PAWC',' ')
     
20    format(a7,i1)
       
      end


c-----------------------------------------------------------


      subroutine dc0_fill
      
      include 'DC0nt.inc'
      
      real sector,layer,wire,tdc
      integer id1,id2,j
      
      call hcdir('//PAWC',' ')
      
      if (nDC0.gt.0) then
      call hcdir('//PAWC/DC0',' ')

      do j = 1, nDC0
        sector = float(secdc0(j))
        layer  = float(layerdc0(j))
        wire   = float(wiredc0(j))
        tdc    = float(tdcdc0(j))
        id1    =  10*sector
        id2    = 100*sector
        call hf1(1,sector,1.)
        call hf2(id1+1,wire,layer,1.)
        call hf2(id2+2,wire,layer,tdc)
      enddo
      
      call hcdir('//PAWC',' ')
      endif
      
      end
      

c-----------------------------------------------------------


      subroutine dc0store
      include "bcs.inc"
      include "DC0nt.inc"
      
      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer*2 I16(20000)            ! 16 bits
      integer*4 I32(10000)            ! 32 bits
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save name_i
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'DC0 ')
        if (name_i.eq.0) then
          print *,' NO DC0 bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)
      nDC0 = 0

30    if (ind.ne.0) then
      
        nword   = iw(ind)
        sector  = iw(ind-2)
        nrow    = iw(ind-4)
        
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do

        k = 0
        
        do i = 1, nrow 
          nDC0 			= nDC0 + 1
          secDC0(nDC0)		= sector
          k 			= k + 1
          layerDC0(nDC0)        = max(1,min(36,I16(k)/256))
          WireDC0(nDC0)         = max(1,min(193,mod(I16(k),256)))
          k                     = k + 1
          TDCDC0(nDC0 )         = max(1,min(8191,I16(k)))
        enddo
        
        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end


c-----------------------------------------------------------


      subroutine dc_book

c  Francois and Burin's DC histograms

c  modified by EJW, 3-oct-96
c   modified by SAD feb-97


      implicit none
      save

      integer isec,ireg,nhists
      parameter (nhists=6)               !update for no. of hists
      character*21 title(nhists)         !hist title
      character*13 fstring(3,6)          !contains sector and region
      character*80 atitle                !complete title


c  executable code:
c  ----------------

c  name hists
      title(1)= 'Hit spectrum'
      title(2)= 'TDC spectrum'
      title(3)= 'Hits, layer vs. wire '
      title(4)= 'Ti-Tj '
      title(5)= 'Hits/layer '
      title(6)= 'Multiplicity/sector '
      do isec=1,6
        do ireg=1,3
          write(fstring(ireg,isec),1000) 'reg= ',ireg,' sec= ',isec
 1000     format(a5,i1,a6,i1)
c          write(6,1010) fstring(ireg,isec)
        enddo
      enddo
c  create DC sub-dir (n.b. Cole uses DC0)
      call hcdir('//PAWC',' ')
      call hmdir('DC','S')

c  book histograms  
      call hbook1(100,'Status ',7,0.5,7.5,0.)
      do isec = 1, 6
         do ireg = 1, 3
            atitle= title(1)//fstring(ireg,isec)
c            if(isec.eq.5 .and. ireg.eq.3) write(6,1010) atitle
c 1010       format(1x,a)
            call hbook1(100*ireg+10*isec+1,atitle,193,0.,192.,0.)
            atitle= title(2)//fstring(ireg,isec)	
            call hbook1(100*ireg+10*isec+2,atitle,4000,1.,8000.,0.)	
            atitle= title(3)//fstring(ireg,isec)
            if (ireg.eq.1) call hbook2(100*ireg+10*isec+3,atitle
     -           ,142,0.5,142.5,12,0.5,12.5,0.)
            if (ireg.eq.2) call hbook2(100*ireg+10*isec+3,atitle
     -           ,192,0.5,192.5,12,12.5,24.5,0.)
            if (ireg.eq.3) call hbook2(100*ireg+10*isec+3,atitle
     -           ,192,0.5,192.5,12,24.5,36.5,0.)
            atitle= title(4)//fstring(ireg,isec)
            call hbook1(100*ireg+10*isec+4,atitle,2000,-2000.,2000.,0.)
            atitle= title(5)//fstring(ireg,isec)
            call hbook1(100*ireg+10*isec+5,atitle,36,0.5,36.5,0.)
            atitle= title(6)//fstring(ireg,isec)
            call hbook1(100*ireg+10*isec+6,atitle,61,-0.5,60.5,0)
         enddo
      enddo
      
c  change back to top directory
      call hcdir('//PAWC',' ')
      
      return
      end


c------------------------------------------------------------------------------


      subroutine dc_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------
      return
      end


c-----------------------------------------------------------


      subroutine dc_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine dc_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine dc_fill

c  Francois and Burin's histogram filler

c  modified by EJW, 3-oct-96
c  modified by SAD feb-97

      implicit none
      save


      include "bcs.inc"

      integer mamind, ishft, iand

      integer*2 i16(20000)
      integer*4 i32(10000)
      equivalence ( i16(1),i32(1) )

      integer ind, i,j, il, ih, nd, nd2, isec, id, ireg, ipart, ik, iz
      integer j16
      integer ip(8)         !labels channels (max of 7) in error hist to update
      integer ievent        !event counter
      real err_bit          !channels to be incremented for Error hist
      integer nsect/6/, nl/36/, npt/36/, nstr/36/, ndx/60/
      integer*2 i8r/255/, i8l/32512/

      integer dc_nhit(36,6), p_nhit(13,3,6)
      integer dc_hit(2,60,36,6), p_hit(2,10000,13,3,6), diff(36,3)
      integer dc_mult(3,6)  !multiplicity per sector/region

      integer nam_dc/0/  


c  executable code:
c  ----------------

c  First change PAWC directories
      call hcdir('//PAWC/DC',' ')

c  update event counter
      ievent=ievent+1
c        if (ievent.eq.1 .or. ievent.eq.2) write(6,2000) 'E',ievent

c  zero error bits
      ip(1)=1
      do i= 2,7
         ip(i)=0
      enddo

c  zero counters
      do isec = 1,nsect
         dc_mult(1,isec)= 0
         dc_mult(2,isec)= 0
         dc_mult(3,isec)= 0
         do il = 1,nl
            dc_nhit(il,isec) = 0
         enddo
      enddo
      
      do isec = 1,nsect
         do ireg =1,3
            do ipart = 1,13
               p_nhit(ipart,ireg,isec) = 0
            enddo
         enddo
      enddo       
      

C  Loop over sectors to store info in "dc_hit" array


C  At first entry define name-indices
      nam_dc = MAMIND(IW,'DC0 ')
      ind = nam_dc + 1


 20   CONTINUE
        ind  = IW(ind-1)
        IF(ind.EQ.0) GO TO 22         
        isec = IW(ind-2)
        IF(isec.LT.1 .OR. isec.GT.nsect) then
           ip(7)=1
           GO TO 20
        endif
        nd   = IW(ind)
        IF(nd.EQ.0) GO TO 20

        DO j = 1,nd
          I32(j) = IW(ind+j)
        ENDDO

c  Unpack and store in "dc_hit( , , , )"
        nd2 = 2*nd
        DO 21 i = 1,nd2,2
          id  = IAND(I16(i),i8R)
          j16 = IAND(I16(i),i8L)
          il  = ISHFT(j16,-8)

          if(il.ge.1.and.il.le.12) ireg = 1
	  if(il.ge.13.and.il.le.24) ireg = 2
	  if(il.ge.25.and.il.le.36) ireg = 3

          if (ireg.eq.1 .and. (id.lt.1 .or. id.gt.142)) then
             ip(2)=1
c             type *,'reg 1 - bad id= ',id
             endif
          if (ireg.eq.2 .and. (id.lt.1 .or. id.gt.192)) then
             ip(3)=1
c             type *,'reg 2 - bad id= ',id
             endif
          if (ireg.eq.3 .and. (id.lt.1 .or. id.gt.192)) then
             ip(4)=1
c             type *,'reg 3 - bad id= ',id
             endif

          if ((id/16*16).ne.(id*16/16)) then
             ipart = id/16+1
          else
             ipart = id/16
          endif 

          IF(il.LT.1 .OR. il.GT.nl .or. il.eq.5 .or. il.eq.6) then
             ip(5)=1                 !if bad layer, update cntr and continue
             GO TO 21
          endif

          IF(dc_nhit(il,isec).GE.ndx) then  !if too many hits, update and go on
             ip(6)=1
             GO TO 21
          endif

c          if(il.eq.13 .and. (id.eq.57 .or. id.eq.61)) goto 21
c          if(il.eq.16 .and. (id.eq.132)) goto 21
c          if(il.eq.8 .and. (id.eq.78 .or. id.eq.82)) goto 21
c          if(il.eq.10 .and. (id.eq.26)) goto 21
c          if(il.eq.35 .and. (id.eq.155)) goto 21
c          if(il.eq.31 .and. (id.eq.36 .or. id.eq.40)) goto 21
c          if(il.eq.34 .and. (id.eq.36 .or. id.eq.40)) goto 21

          dc_nhit(il,isec) = dc_nhit(il,isec) + 1
          p_nhit(ipart,ireg,isec) = p_nhit(ipart,ireg,isec) + 1
          ih = dc_nhit(il,isec)
          ik = p_nhit(ipart,ireg,isec)
          dc_hit(1,ih,il,isec) = id  
          dc_hit(2,ih,il,isec) = I16(i+1)
          p_hit(1,ik,ipart,ireg,isec) = id
          p_hit(2,ik,ipart,ireg,isec) = I16(i+1)

 21     CONTINUE

c  sum over layers to get multiplicity for this sector (isec)
        do i= 1, 12
           dc_mult(1,isec)= dc_mult(1,isec)+ dc_nhit(i,isec)
           dc_mult(2,isec)= dc_mult(2,isec)+ dc_nhit(i+12,isec)
           dc_mult(3,isec)= dc_mult(3,isec)+ dc_nhit(i+24,isec)
        enddo

C Go to next sector
        GO TO 20

c  ******** fill histograms here ********
 22   CONTINUE
c        if (ievent.eq.1 .or. ievent.eq.2) write(6,2000) ' EVENT',ievent
 2000   format(a,i3)
      do i= 1, 7
         err_bit= float(i)
         if(ip(i).ne.0) call hf1(100,err_bit,1.)
      enddo
      do isec = 1, nsect
         do il = 1, nl
            if(il.ge.1.and.il.le.12) ireg = 1
            if(il.ge.13.and.il.le.24) ireg = 2
            if(il.ge.25.and.il.le.36) ireg = 3
            do ih = 1, dc_nhit(il,isec)
               call hf1(100*ireg+10*isec+1,float(dc_hit(1,ih,il,isec)),1.)
               call hf1(100*ireg+10*isec+2,0.5*float(dc_hit(2,ih,il,isec)),1.)
               call hf2(100*ireg+10*isec+3,float(dc_hit(1,ih,il,isec)),
     -              float(il),1.)
               call hf1(100*ireg+10*isec+5,float(il),1.)
            enddo
         enddo		
      enddo
      
      do ireg= 1, 3
c         if(ievent.lt.20) write(6,1020) (dc_mult(ireg,i),i=1,6)
c 1020    format(1x, 6(i6))
         do isec= 1, nsect
            call hf1(100*ireg+10*isec+6,float(dc_mult(ireg,isec)),1.)
         enddo
      enddo

      do isec = 1, nsect
         do ireg = 1, 3
            do ipart = 1, 12
               diff(isec,ireg) = 0
               do ik = 1, p_nhit(ipart,ireg,isec)-1
                  do iz = ik+1, p_nhit(ipart,ireg,isec)
                     diff(isec,ireg) = (p_hit(2,ik,ipart,ireg,isec) 
     -                    - p_hit(2,iz,ipart,ireg,isec))/2.
                     call hf1(100*ireg+10*isec+4,float(diff(isec,ireg)),1.)
                  enddo
               enddo
	    enddo		
         enddo 
      enddo
      
 100  continue
c  return to the top directory and exit
      call hcdir('//PAWC',' ')

      return
      end


c------------------------------------------------------------------------------
      subroutine dc_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


