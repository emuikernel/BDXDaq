c  ec.f

c  ejw, 13-may-98


c-----------------------------------------------------------
c

      subroutine ec_book
      
      character*80 title
      character*80 t102,t103,t1504
      character*8 fstring(6)
      integer i
      
      do i = 1,6
        write(fstring(i),20) 'SECTOR ',i
      enddo
        
      t102  = ' EC;TDC;(LAYER-1)*36+PMT'
      t103  = ' EC;ADC;(LAYER-1)*36+PMT'
      t1504 = ' EC;INNER ADCSUM;OUTER ADCSUM'
      
      call hmdir('//PAWC/EC','S')
      
      call hbook2(1,'EC;SECTOR;',6,1.,7.,2,9.,11.,0.)
      call hbook2(2,'EC TDC, Occupancy;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(3,'EC TDC, wt by TDC val;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(4,'EC ADC, Occupancy;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(5,'EC ADC, wt by ADC val;PMT;',36,1.,37.,36,1.,37.,0.)
      call hbook2(6,'EC;MULTIPLICITY;',37,0.,37.,36,1.,37.,0.)
      
      do i = 1,6
        title = fstring(i)//t102
        call hbook2(i*100+2,title,200,0.,8000.,216,1.,217.,0.)
        title = fstring(i)//t103
        call hbook2(i*100+3,title,100,0.,300.,216,1.,217.,0.)
        title = fstring(i)//t1504
        call hbook2(i*1000+504,title,50,0.,2000.,50,0.,2000.,0.)
      enddo
      
      call hcdir('//PAWC',' ')
     
20    format(a7,i1)
       
      end
      
     
c-----------------------------------------------------------

      subroutine ec_bor
      include 'eccal.inc'

      character*132 file_in,dumc
      integer str,lay,ped,id,sec,s,i,dumi
      real rms

      file_in='/usr/local/clas/parms/pedman/Tfiles/ec.tped'

      open(unit=12,file=file_in,status='old')

      read(12,*) dumc
      read(12,*) dumc
      read(12,*) dumi
      do s=1,6
        read(12,*) dumc
        do i=1,216
          read(12,*) id,sec,ped,rms
          str = mod(id,256)
          lay = id/256
          ecped(str,lay,sec+1) = ped
        enddo
      enddo

      close(12)

      return
      end

       
c-----------------------------------------------------------

      subroutine ec_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ec_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ec_fill
      
      include 'ECnt.inc'
      include 'eccal.inc'
     
      real layer,strip,superlayer,chan,tdc,adc,mul
      real echits(36,6,6),ecadc(36,6,6),ectdc(36,6,6)
      real eraw(2,6)
cSergey: 's' was described as 'real' - fixed
      integer s
c
      integer id1,id2,id3,j,mec(6,6)
      logical good
      integer i,n,lay,str,ii,ichan


c      print *,'ec: nEC=',nEC    
      if (nEC.gt.0) then
      call hcdir('//PAWC/EC',' ')
      
      do i = 1,6
        do j = 1,6
          mec(j,i) = 0
        enddo
        do j = 1,2
          eraw(j,i) = 0
        enddo
      enddo
      
      do n = 1,nec
        s   = secec(n)
        lay = layerec(n)
        str = stripec(n)
c      print *,'->',s,lay,str
        adcec(n) = adcec(n) - ecped(str,lay,s)
        if (adcec(n).gt.0) then
          mec(lay,s) = mec(lay,s) + 1
c      print *,'nec=',nec,' -> set mec(',lay,',',s,')=',mec(lay,s)
          ii = mec(lay,s)
          echits(ii,lay,s) = stripec(n)
          ectdc(ii,lay,s)  = tdcec(n)
          ecadc(ii,lay,s)  = adcec(n)
          if (lay.le.3) eraw(1,s) = eraw(1,s)+ecadc(ii,lay,s)
          if (lay.ge.4) eraw(2,s) = eraw(2,s)+ecadc(ii,lay,s)
        endif
      enddo

      do s = 1,6
        id1    = 10*s
        id2    = 100*s
        id3    = 1000*s
        do lay = 1,6
          superlayer = (s-1)*6+lay
          mul = float(mec(lay,s))
          layer = float(lay)
          call hf2(6,mul,superlayer,1.)
          do n = 1,mec(lay,s)

C sergey: they are float already
C            strip = float(echits(n,lay,s))
C            tdc	  = float(ectdc(n,lay,s))
C            adc   = float(ecadc(n,lay,s))
            strip = echits(n,lay,s)
            tdc	  = ectdc(n,lay,s)
            adc   = ecadc(n,lay,s)

            ichan = strip+36*(lay-1)
            chan  = float(ichan)
            call hf2(id2+2,tdc,chan,1.)
            call hf2(id2+3,adc,chan,1.)
            if (tdc.gt.200.and.tdc.lt.8000)then
              call hf2(1,s,9.,1.)
              call hf2(2,strip,superlayer,1.)
              call hf2(3,strip,superlayer,tdc)
            endif
            if (adc.gt.10.and.adc.lt.1024) then
              call hf2(1,s,10.,1.)
              call hf2(4,strip,superlayer,1.)
              call hf2(5,strip,superlayer,adc)
            endif            
          enddo
        enddo
        good = mec(1,s).gt.0.and.mec(2,s).gt.0.and.mec(3,s).gt.0
        good = good.and.mec(4,s).gt.0.and.mec(5,s).gt.0.and.mec(6,s).gt.0
        if (good.and.eraw(1,s).gt.0.and.eraw(2,s).gt.0) then
          call hf2(id3+504,eraw(1,s),eraw(2,s),1.)
        endif
      enddo

      call hcdir('//PAWC',' ')
      endif
      
      end
      
      
c-----------------------------------------------------------


      subroutine ec_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------


      return
      end


c-----------------------------------------------------------


      subroutine ecstore
      include 'bcs.inc'
      include 'ECnt.inc'

      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer ishft,iand,k16,j16
      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits
      integer*2 i8r/255/, i8l/32512/
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save ifirst,name_i
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'EC  ')
        if (name_i.le.0) then
          print *,' NO EC bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)
      nEC = 0

30    if (ind.ne.0) then
      
        nword 	= iw(ind)
        sector	= iw(ind-2)
        nrow  	= iw(ind-4)

cc      print *,'beg->',ind,nword,sector,nrow
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do

cc      do j=1,nword*2
cc        write(6,123) j,i16(j)
cc 123    format('->',I,' ->',Z)
cc      enddo

        k = 0

        do i = 1, nrow 
          nEC = nEC + 1
          SECEC(nEC)	= sector
          k = k + 1
          k16 = I16(k)
c
          LAYEREC(nEC)	= k16/256
          STRIPEC(nEC)	= mod(k16,256)
c
c          STRIPEC(nEC) = IAND(k16,i8R)
c          j16 = IAND(k16,i8L)
c          LAYEREC(nEC) = ISHFT(j16,-8)
c
          if(LAYEREC(nEC).GT.6.OR.STRIPEC(nEC).GT.36) then
            print *,'ec.F: ERROR: layer=',LAYEREC(nEC),' strip=',STRIPEC(nEC)
          endif
c
cc      print *,'k=',k,' nrow=',i,' -> ',LAYEREC(nEC),STRIPEC(nEC)


          k	= k + 1
          TDCEC(nEC) = min(8000,max(1,I16(k)))
          k	= k + 1
          ADCEC(nEC) = min(8191,max(1,I16(k))) 
        enddo
       
        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end


c-----------------------------------------------------------
