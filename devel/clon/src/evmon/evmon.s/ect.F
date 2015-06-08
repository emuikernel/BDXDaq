c  ect.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine ect_book
      
      character*80 title
      character*80 t102,t103,t1504
      character*8 fstring(6)
      integer i
      
      do i = 1,6
        write(fstring(i),20) 'SECTOR ',i
      enddo
        
      t102  = ' ECT;TDC;(LAYER-1)*36+PMT'
      
      call hmdir('//PAWC/ECT','S')
      
      call hbook2(1,'ECT;SECTOR;',6,1.,7.,2,9.,11.,0.)
      call hbook2(2,'ECT TDC, Occupancy;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(3,'ECT TDC, wt by TDC val;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(6,'EC;MULTIPLICITY;',37,0.,37.,36,1.,37.,0.)
      
      do i = 1,6
        title = fstring(i)//t102
        call hbook2(i*100+2,title,200,2000.,6000.,216,1.,217.,0.)
      enddo
      
      call hcdir('//PAWC',' ')
     
20    format(a7,i1)
       
      end
      
     
c-----------------------------------------------------------

      subroutine ect_bor

      return
      end

       
c-----------------------------------------------------------

      subroutine ect_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ect_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ect_fill
      
      include 'ECTnt.inc'
      include 'eccal.inc'
     
      real layer,strip,superlayer,chan,tdc,adc,mul
cSergey: 's' was described as 'real' - fixed
      integer s
c
      real echits(36,6,6),ectdc(36,6,6)
      integer id1,id2,id3,j,mec(6,6)
      logical good
      integer i,n,lay,str,ii

            
c      print *,'ect: nECT=',nECT    
      if (nECT.gt.0) then
      call hcdir('//PAWC/ECT',' ')
      
      do i = 1,6
        do j = 1,6
          mec(j,i) = 0
        enddo
      enddo
      
      do n = 1,nect
        s   = secect(n)
        lay = layerect(n)
        str = stripect(n)
        mec(lay,s) = mec(lay,s) + 1
        ii = mec(lay,s)
        echits(ii,lay,s) = stripect(n)
        ectdc(ii,lay,s)  = tdcect(n)
      enddo
      
      do s = 1,6
        id2    = 100*s
        do lay = 1,6
          superlayer = (s-1)*6+lay
          mul        = float(mec(lay,s))
          layer      = float(lay)
          call hf2(6,mul,superlayer,1.)
          do n = 1,mec(lay,s)

C sergey: they are already float
C            strip = float(echits(n,lay,s))
C            tdc	  = float(ectdc(n,lay,s))
            strip = echits(n,lay,s)
            tdc	  = ectdc(n,lay,s)

            ichan = strip+36*(lay-1)
            chan  = float(ichan)
            call hf2(id2+2,tdc,chan,1.)
            if (tdc.gt.0.and.tdc.lt.8000) then
              call hf2(1,s,9.,1.)
              call hf2(2,strip,superlayer,1.)
              call hf2(3,strip,superlayer,tdc)
            endif            
          enddo
        enddo
      enddo   

      call hcdir('//PAWC',' ')
      endif
      
      end
      
      
c-----------------------------------------------------------


      subroutine ect_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------


      return
      end


c-----------------------------------------------------------


      subroutine ectstore
      include 'bcs.inc'
      include 'ECTnt.inc'

      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save name_i
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'ECT ')
        if (name_i.eq.0) then
          print *,' NO ECT bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)
      nECT = 0

30    if (ind.ne.0) then
      
        nword 	= iw(ind)
        sector	= iw(ind-2)
        nrow  	= iw(ind-4)
        
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do

        k = 0

        do i = 1, nrow 
          nECT		= nECT +1
          SECECT(nECT)	= sector
          k		= k + 1
          LAYERECT(nECT)= I16(k)/256
          STRIPECT(nECT)= mod(I16(k),256)
          if(LAYERECT(nECT).GT.6.OR.STRIPECT(nECT).GT.36) then
            print *,'ect.F: ERROR: layer=',LAYERECT(nECT),' strip=',STRIPECT(nECT)
          endif
          k		= k + 1
          TDCECT(nECT)	= min(65535,max(1,I16(k)))
        enddo
       
        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end


c-----------------------------------------------------------

