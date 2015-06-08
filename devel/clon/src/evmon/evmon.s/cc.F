c  cc.f

c  ejw, 13-may-98
c  Correction Alex Vlassov, Nov,1,2001

c-----------------------------------------------------------

      subroutine cc_book
      
      implicit none
      save

      integer i
      character*80 title, t102,t103,t104,t105,t106
      character*8 fstring(6)

      do i = 1,6
        write(fstring(i),20) 'SECTOR ',i
      enddo
      
      call hmdir('//PAWC/CC','S')
      
      call hbook2(1,'CC;SECTOR;',6,0.5,6.5,4,0.5,4.5,0.)
      call hbook2(2,'CC TDC, Occupancy;PMT;',
     &            18,0.5,18.5,12,0.5,12.5,0.)
      call hbook2(3,'CC TDC, wt by TDC val;PMT;', 
     &            18,0.5,18.5,12,0.5,12.5,0.)
      call hbook2(4,'CC ADC, Occupancy;PMT;',
     &            18,0.5,18.5,12,0.5,12.5,0.)
      call hbook2(5,'CC ADC, wt by ADC val;PMT;',
     &            18,0.5,18.5,12,0.5,12.5,0.)
      call hbook2(6,'CC;MULTIPLICITY;',
     &            18,0.5,18.5,12,0.5,12.5,0.)
      call hbprof(7,' New TDC values',
     & 216, 0.5, 216.5, 4000., 4950.,'s')
      call hbook2(8,'New TDC - Number of hits per event;',
     &            10,0.5,10.5,216,0.5,216.5,0.)
     
      t102  = ' CC;TDCL;PMT'
      t103  = ' CC;ADCL;PMT'
      t104  = ' CC;TDCR;PMT'
      t105  = ' CC;ADCR;PMT'
      t106  = ' CC;TDC_NEW;PMT'
      
      do i = 1,6
        title = fstring(i)//t102
        call hbook2(i*100+2,title,200,3510.,5510.,18,0.5,18.5,0.)
        title = fstring(i)//t103 
        call hbook2(i*100+3,title,100,10.,5010.,18,0.5,18.5,0.)
        title = fstring(i)//t104
        call hbook2(i*100+4,title,200,3510.,5510.,18,0.5,18.5,0.)
        title = fstring(i)//t105
        call hbook2(i*100+5,title,100,10.,5010.,18,0.5,18.5,0.)
        title = fstring(i)//t106
        call hbook2(i*100+6,title,600,3000.,6000.,36,0.5,36.5,0.)
c
        title = 'CC amplitude - pedestal (channels) in '//fstring(i)
        call hbprof(100*i + 51,title, 36, 0.5, 36.5, 50., 4000.,'s')
        call hbook2(100*i + 53,title, 200, 0., 4000., 36, 0.5, 36.5, 0.) 
        title = 'mean TDC (channels) in '//fstring(i)
        call hbprof(100*i + 52,title, 36, 0.5, 36.5, 4000., 5000.,'s')
        call hbook2(100*i + 54,title,200,4000.,5000.,36,0.5,36.5, 0.) 

      end do
      
      call hbpro(0,0.)
      call hcdir('//PAWC',' ')
     
20    format(a7,i1)

      end
      

c-----------------------------------------------------------


      subroutine cc_bor
      include "ccrec.inc"

      character*132 file_in,dumc
      integer ped,id,sec,s,i,dumi
      real rms

      file_in='/usr/local/clas/parms/pedman/Tfiles/cc.tped'

      open(unit=12,file=file_in,status='old')

      read(12,*) dumc
      read(12,*) dumc
      read(12,*) dumi
      do s=1,6
        read(12,*) dumc
        do i=1,36
          read(12,*) id,sec,ped,rms
          ccpedes(sec+1,id) = ped
        enddo
      enddo

      close(12)
      
      return
      end


c-----------------------------------------------------------


      subroutine cc_done

c  called at done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine cc_eor

c  called at eor


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine cc_fill
      
      implicit none
      save
      
      include 'CCnt.inc'
      include 'ccrec.inc'
      
      real sector,chan,tdc,adc,adccor,superlayer,pmt
      integer id2,i,j,k,lr,isect,isl
Csergey: already in CCnt.inc      integer n_tdc(maxCC)
      
      if (nCC.gt.0) then
      
      call hcdir('//PAWC/CC',' ')
      
      do i = 1, nCC
        sector	= float(seccc(i))
        chan	= float(idcc(i))
        tdc	= float(tdccc(i))
        adc	= float(adccc(i))-ccpedes(sector,chan)
        adccor 	= adc/opheamp(sector,chan)
        
        id2	= 100*sector
        
        k 	= idcc(i)
        j	= mod(k,2)
        pmt	= float(k/2+j)
        lr	= 2-j

        isl = (sector-1)*2+lr
        superlayer = float(isl)
        
        if (lr.eq.1) then
          call hf2(id2+2,tdc,pmt,1.)            
          call hf2(id2+3,adc,pmt,1.)
          if (tdc.gt.200.and.tdc.lt.8000) then
            call hf2(2,pmt,superlayer,1.)
            call hf2(3,pmt,superlayer,tdc)
            call hf2(1,sector,1.,1.)
          endif
          if (adc.gt.100.and.adc.lt.10000.) then
            call hf2(4,pmt,superlayer,1.)
            call hf2(5,pmt,superlayer,adc)
            call hf2(1,sector,3.,1.)
          endif            
        elseif (lr.eq.2) then
          call hf2(id2+4,tdc,pmt,1.)
          call hf2(id2+5,adc,pmt,1.)
          if (tdc.gt.200.and.tdc.lt.8000) then
            call hf2(2,pmt,superlayer,1.)
            call hf2(3,pmt,superlayer,tdc)
            call hf2(1,sector,2.,1.)
          endif
          if (adc.gt.100.and.adc.lt.10000.) then
            call hf2(4,pmt,superlayer,1.)
            call hf2(5,pmt,superlayer,adc)
            call hf2(1,sector,4.,1.)
          endif            
        endif
        
      call hfill(id2+51,chan,adc,1.)
      call hfill(id2+52,chan,tdc,1.)
      call hf2(id2+53,adc,chan,1.)
      call hf2(id2+54,tdc,chan,1.)
      
      enddo
      
      
      endif
c
c    *** NEW TDC filling  ***
c
      do i = 1,maxCC
        if(TDCCC_NEW(i) .gt. 0) then
          tdc = float(TDCCC_NEW(i))
          call hfill(7,float(i),tdc,1.)
          call hf2(8,float(N_TDC(i)),float(i),1.)
c
          isect = int((i-1)/36. + 0.01) + 1
          id2 = 100*isect
          k   = mod((i-1),36) + 1
          pmt = float(k)
          tdc = float(TDCCC_NEW(i))

          call hf2(id2+6,tdc,pmt,1.)

         endif
      end do

      call hcdir('//PAWC',' ')
      return
      end


c-----------------------------------------------------------


      subroutine cc_init
c     ==================
c
c  called at initial stage
c 
c  Alexander Vlassov

      implicit none
      save


c  executable code:
c  ----------------

      END


c-----------------------------------------------------------


      subroutine ccstore

      include "bcs.inc"
      include "CCnt.inc"
      
      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer name_t, name_l, ind_l
      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save name_i
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'CC  ')
        if (name_i.eq.0) then
          print *,' NO CC bank registered'
          return
        endif
        name_t = mamind(iw,'CCT ')
        if (name_t.eq.0) then
          print *,' NO CCT bank registered'
          return
        endif
        name_l = mamind(iw,'TGBI')
        if (name_l.eq.0) then
          print *,' NO TGBI bank registered'
          return
        endif
        ifirst = 1
      endif
      ind_l = iw(name_l)
      if(ind_l .le. 0) then 
c        print *,' NO TGBI bank'
        return
      end if
c
      L1 = IW(ind_l+1)
      i = mod(L1,1024)
      if(i.ge.512) return
c
      ind = iw(name_i)
      nCC = 0

30    if (ind.ne.0) then
      
        nword   = iw(ind)
        sector  = iw(ind-2)
        nrow    = iw(ind-4)
        
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do
        
        k = 0
        
        do i = 1, nrow
          nCC 		= nCC +1
          secCC(nCC)	= sector
          k 		= k + 1
          idCC(nCC) 	= I16(k)
          k 		= k + 1
          TDCCC(nCC)	= I16(k)
          k 		= k + 1
          ADCCC(nCC)	= I16(k)
        enddo
       
        ind = iw(ind-1)
        goto 30
        
      endif
c
c     New TDC channels 
c  
      do j = 1,maxCC
        N_TDC(j) = 0
      end do
c
      ind = iw(name_t)

50    if (ind.ne.0) then
      
        nword   = iw(ind)
        sector  = iw(ind-2)
        nrow    = iw(ind-4)
        
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do
        
        k = 0
        
        do i = 1, nrow
c
          k = k + 1
          j = 36*(sector - 1) + I16(k)
          k = k + 1
          TDCCC_NEW(j)	= I16(k)
          N_TDC(j) = N_TDC(j) + 1
        end do
        ind = iw(ind-1)
        goto 50
        
      endif

      return
      end


c-----------------------------------------------------------

