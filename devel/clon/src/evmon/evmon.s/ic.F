c  ic.f

c  Sergey, 19-oct-2008


c-----------------------------------------------------------


      subroutine ic_book
          
      implicit none
      save

      integer i,k
      real tminraw, tmaxraw, tmincal, tmaxcal
      real aminraw, amaxraw, amincal, amaxcal
      data tminraw,tmaxraw,tmincal,tmaxcal/0.,4095.,0.,4095./
      data aminraw,amaxraw,amincal,amaxcal/0.,8191.,0.,4000./
c      tminraw,..., are the hbook ranges for raw and calculated TDCs
c      aminraw,..., are the hbook ranges for raw and calculated ADCs

      
      call hmdir('//PAWC/IC','S')

      call hbook2(1,'IC TDC, Occupancy;PMT;',23,0.,23.,23,0.,23.,0.)
      call hbook2(2,'IC ADC, Occupancy;PMT;',23,0.,23.,23,0.,23.,0.)
      
      call hbpro(0,0.)
      call hcdir('//PAWC',' ')
     
20    format(a7,i2)
c     this is a test    
       
      end


c-----------------------------------------------------------


      subroutine ic_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save

      real IcCal_values(11,6),IcCal_uncertainties(11,6)

      integer i

      include "ic.inc"
      include "event_monitor.inc"


c  executable code:
c  ----------------

      print *, 'Read IC pedestals from Map - Run ',current_run
cc      call iccal_read_map(current_run,IcCal_values,IcCal_uncertainties)
      do i=1,MAXADC
cc         icped(i)=IcCal_values(11,i)
         icped(i)=700
      enddo

      return
      end


c-----------------------------------------------------------


      subroutine ic_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ic_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ic_fill  
      
      implicit none
      save

      include 'ic.inc'

      integer i
      real x,y

      call hcdir('//PAWC/IC',' ')

c fills ADC hists
      do i=1,nadchits
        x = (ixadc(i)-1)+0.5
        y = (iyadc(i)-1)+0.5
        if(x.gt.0.and.x.lt.23..and.y.gt.0.and.y.lt.23.) then
          if (iadc(i).gt.icped(i).and.iadc(i).lt.8000.) then
ccc            write(*,*) 'adc: ',i,ixadc(i),x,iyadc(i),y,iadc(i)
            call hf2(2,x,y,1.)
          endif
        endif
      enddo

c fills TDC hists
      do i=1,ntdchits
        x = (ixtdc(i)-1)+0.5
        y = (iytdc(i)-1)+0.5
        if(x.gt.0.and.x.lt.23..and.y.gt.0.and.y.lt.23.) then
          if (itdc(i).gt.200.and.itdc(i).lt.8000) then
ccc            write(*,*) 'tdc: ',i,ixtdc(i),x,iytdc(i),y,itdc(i)
            call hf2(1,x,y,1.)
          endif
        endif
      enddo

      call hcdir('//PAWC',' ')
c      endif
      
      end


c-----------------------------------------------------------


      subroutine ic_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine icstore 

      implicit none

      include "event_monitor.inc"
      include 'bcs.inc'
      include 'ic.inc'

      integer name_i,ind,nword,i,j,k,ifirst,ncol,nrow,mlink
      integer ichit, ptemp, size, ix, iy, itmp, tdcstat(23,23)

cc      write(*,*) 'IC store reached'


      nadchits = 0
      ntdchits = 0
      do i=1,MAXADC
        ixadc(i)=0
        iyadc(i)=0
        iadc(i)=0
      enddo
      do i=1,MAXTDC
        ixtdc(i)=0
        iytdc(i)=0
        itdc(i)=0
      enddo
      do i=1,23
        do j=1,23
          tdcstat(i,j) = 0
        enddo
      enddo

c get ADCs
      ind = mlink(iw,'IC  ',0)
      if (ind.ne.0) then
        nword   = iw(ind)
        nrow    = iw(ind-4)
        ncol    = iw(ind-5)
        if(nrow.gt.MAXADC) then
          write(*,*) 'ERROR: nrow=',nrow,', set it to MAXADC'
          nrow=MAXADC
        endif
        do i = 1, nrow
          k = (i-1)*3
          nadchits = nadchits + 1
          ptemp = iw16(2*ind+k+1)
          iyadc(nadchits)   = ptemp/256 ! high byte
          ixadc(nadchits)   = mod(ptemp,256) ! low byte
	      iadc(nadchits) = iw16(2*ind+k+3)
cc          write(*,*) 'IC hit number ',nadchits,'; x,y,adc ---> ',ixadc(nadchits),iyadc(nadchits),iadc(nadchits)
        enddo
      endif

c get TDCs
      ind = mlink(iw,'IC0 ',0)
      if (ind.ne.0) then
        nword   = iw(ind)
        nrow    = iw(ind-4)
        ncol    = iw(ind-5)
        if(nrow.gt.MAXTDC) then
          write(*,*) 'ERROR: nrow=',nrow,', set it to MAXTDC'
          nrow=MAXTDC
        endif
        do i = 1, nrow
          k = (i-1)*2
            ntdchits = ntdchits + 1
            ptemp = iw16(2*ind+k+1)
            iytdc(ntdchits)   = ptemp/256 ! high byte
            ixtdc(ntdchits)   = mod(ptemp,256) ! low byte
            itdc(ntdchits) = iw16(2*ind+k+2)
cc          write(*,*) 'IC hit number ',ntdchits,'; x,y,tdc ---> ',ixtdc(ntdchits),iytdc(ntdchits),itdc(ntdchits)

c ignore hit if that channel already in
          if( tdcstat(ixtdc(ntdchits),iytdc(ntdchits)).GT.0 ) then
            ntdchits = ntdchits - 1
          else
            tdcstat(ixtdc(ntdchits),iytdc(ntdchits)) = tdcstat(ixtdc(ntdchits),iytdc(ntdchits)) + 1
          endif

        enddo
      endif




      return
      end


c-----------------------------------------------------------
