c  st.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine st_book
          
      implicit none
      save

      character*80 title,title0,title1,title2 
      character*80 title3,title4,title5
      character*80 title6,title7,title8,title9 
      character*10 fstring(9)
      integer i,k
      real tminraw, tmaxraw, tmincal, tmaxcal
      real aminraw, amaxraw, amincal, amaxcal
      data tminraw,tmaxraw,tmincal,tmaxcal/0.,4095.,0.,4095./
      data aminraw,amaxraw,amincal,amaxcal/0.,8191.,0.,4000./
c      tminraw,..., are the hbook ranges for raw and calculated TDCs
c      aminraw,..., are the hbook ranges for raw and calculated ADCs

      
      do k = 1,6
        write(fstring(k),20)   'SECTOR ',k
      enddo
      do k = 1,3
        write(fstring(k+6),20) 'PAIR   ',k
      enddo  
      
      call hmdir('//PAWC/ST','S')
      title0 = ' ST;SECTOR'
      title1 = ' ST;ADC'
      title2 = ' ST;TDC'
      title3 = ' ST;TDC,ADC'
      title4 = ' ST;ln(ADCj/ADCi)'
      title5 = ' ST;sqrt(ADCi*ADCj)'
      title6 = ' ST;(TDCi-TDCj);sqrt(ADCi*ADCj)'
      title7 = ' ST;(TDCi-TDCj);ln(ADCj/ADCi)'
      title8 = ' ST;ln(ADCj/ADCi);sqrt(ADCi*ADCj)'  
      title9 = ' ST;TDCl-TDCr'
      
      call hbook1(1,title0,6,1.,7.,0.)
      do i = 1,6   
        title = fstring(i)//title1
        call hbook1(i*100+1,title,400,0.,4000.,0.)
        title = fstring(i)//title2
        call hbook1(i*100+2,title,400,0.,4095.,0.)
        title=fstring(i)//title3
        call hbook2(i*100+3,title,400,0.,4095.,400,0.,8192.,0.)
      enddo
 
      do i = 1,3
        title = fstring(i+6)//title9
        call hbook1(i*1000,title,200,-4095.,4095.,0.)  
        title = fstring(i+6)//title4
        call hbook1(i*1000+1,title,200,-7.,7.,0.) 
        title = fstring(i+6)//title5
        call hbook1(i*1000+2,title,200,0.,8191.,0.)
        title = fstring(i+6)//title6 
        call hbook2(i*1000+3,title,100,-4095.,4095.,100,0.,8191.,0.) 
        title = fstring(i+6)//title7 
        call hbook2(i*1000+4,title,100,-4095.,4095.,100,-7.,7.,0.)
        title = fstring(i+6)//title8
        call hbook2(i*1000+5,title,100,-7.,7.,100,0.,8191.,0.) 
      enddo
      
      call hcdir('//PAWC',' ')
     
20    format(a7,i2)
c     this is a test    
       
      end


c-----------------------------------------------------------


      subroutine st_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save

      real StCal_values(11,6),StCal_uncertainties(11,6)

      integer i

      include "stcal.inc"
      include "event_monitor.inc"


c  executable code:
c  ----------------

      print *, 'Read ST pedestals from Map - Run ',current_run
      call stcal_read_map(current_run,StCal_values,StCal_uncertainties)
      do i=1,6
         stped(i)=StCal_values(11,i)
      enddo

      return
      end


c-----------------------------------------------------------


      subroutine st_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine st_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine st_fill  
      
      implicit none
      save

      include 'STnt.inc'
      include 'stcal.inc'
      
      real sector,tdcl,adcl,tdcr,adcr 
      real tdifij,ampmean,amprtji  
      integer id1,j

      real tmincut, tmaxcut, amincut, amaxcut
      data tmincut,tmaxcut,amincut,amaxcut/0.,4095.,15.,8191./
            
c      if (nST.gt.0) then 
      call hcdir('//PAWC/ST',' ')

      
      
      do j = 1,6
         if(mod(j,2).ne.0) then
            tdcl=0.
            adcl=0.
            tdcr=0.
            adcr=0.
         endif   
         if(dat(j).eq.1) then
            sector = float(j)
            if(mod(j,2).eq.0) then
               tdcr   = float(tdcst(j)) 
               adcr   = float(adcst(j)) - stped(j)
               if(tdcr.GT.tmincut.AND.tdcr.LT.tmaxcut.AND.
     &     adcr.GT.amincut.AND.adcr.LT.amaxcut) then
                  call hf1(1,sector,1.)
                  id1    = 100*sector
                  call hf1(id1+1,adcr,1.)
                  call hf1(id1+2,tdcr,1.)
                  call hf2(id1+3,tdcr,adcr,1)
               endif
            else
               tdcl   = float(tdcst(j))
               adcl   = float(adcst(j))-stped(j)
               if(tdcl.GT.tmincut.AND.tdcl.LT.tmaxcut.AND.
     &     adcl.GT.amincut.AND.adcl.LT.amaxcut) then  
                  call hf1(1,sector,1.)
                  id1    = 100*sector
                  call hf1(id1+1,adcl,1.)
                  call hf1(id1+2,tdcl,1.)
                  call hf2(id1+3,tdcl,adcl,1)
               endif
            endif
         endif
     
c       added histograms from calculated quantities
         if(mod(j,2).eq.0) then

            if(tdcl.GT.tmincut.AND.tdcl.LT.tmaxcut.AND.
     &         tdcr.GT.tmincut.AND.tdcr.LT.tmaxcut) then
               tdifij = tdcl-tdcr  
               call hf1((j/2)*1000,tdifij,1.) 

               if(adcl.GT.amincut.AND.adcl.LT.amaxcut.AND.
     &              adcr.GT.amincut.AND.adcr.LT.amaxcut) then
                  ampmean = SQRT(adcl*adcr)
                  amprtji= LOG(adcr/adcl)
                  call hf1((j/2)*1000+1,amprtji,1.)
                  call hf1((j/2)*1000+2,ampmean,1.)
                  call hf2((j/2)*1000+3,tdifij,ampmean,1.)
                  call hf2((j/2)*1000+4,tdifij,amprtji,1.)
                  call hf2((j/2)*1000+5,amprtji,ampmean,1.)  
               end if
  
            end if
         endif   

      enddo 
      
      call hcdir('//PAWC',' ')
c      endif
      
      end


c-----------------------------------------------------------


      subroutine st_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ststore 

      implicit none

      integer StData_BOS_size/3/

      include "event_monitor.inc"
      include 'bcs.inc'
      include 'STnt.inc'      

      integer name_i,ind,nword,i,k,ifirst,ncol,mamind
      integer sthit, ptemp, size
c      integer*2 I16(2000)            ! 16 bits
c      integer*4 I32(1000)            ! 32 bits
      data ifirst/0/ 
c      equivalence (I16(1), I32(1))
      save name_i
      
      sthit = 0
      ptemp = 0
      do i=1,6
         dat(i)=0
      enddo   

      if (ifirst.eq.0) then
        name_i = mamind(iw,'ST  ')
        if (name_i.eq.0) then
          print *,' NO ST bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)
      
30    if (ind.ne.0) then
      
        nword   = iw(ind)
        size = 2*nword
        ncol    = iw(ind-5)

        IF( MOD(size-1,StData_BOS_size).EQ.0 ) THEN
          size= size-1                      !BOS was forced to round up
        ENDIF

        k = 0
      
        do i = 1, size/3
          k 		= (i-1)*3
          secST(i)        = iw16(2*ind+k+1)
          tdcST(secST(i))   = iw16(2*ind+k+2)
	  adcST(secST(i))   = iw16(2*ind+k+3)
	  dat(secST(i))   = 1
c          write(*,*)secST(i),tdcST(secST(i)),adcST(secST(i))
        enddo

        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end


c-----------------------------------------------------------
