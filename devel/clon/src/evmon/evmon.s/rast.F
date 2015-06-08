c  raster historgamms
c  Alexander Vlassov, Feb 2006
c  ----------------------------
c
      subroutine rast_book
c     ====================
c
      implicit none
      save
c
      call hmdir('//PAWC/RAST','S')
c
      call hbook1(11,' Raster X', 400, 0., 8000.,0.)
      call hbook1(12,' Raster Y', 400, 0., 8000.,0.)
      call hbook2(21,' Raster XY', 
     & 160, 0., 8000., 160, 0., 8000., 0.)
c
      call hcdir('//PAWC',' ')
      return
      end     

      subroutine rast_bor

c  called at done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end

      subroutine rast_done

c  called at done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


      subroutine rast_eor

c  called at eor


      implicit none
      save


c  executable code:
c  ----------------

      return
      end
c
      subroutine rast_fill
      
      implicit none
      save
      
      include "rast.inc"
c    --------------------

      if(irast_ok.eq.1) then    ! Raster signal
c
        call hcdir('//PAWC/RAST',' ')
c
        call hf1(11, float(ADC_rast(1)),1.)
        call hf1(12, float(ADC_rast(2)),1.)
        call hf2(21, float(ADC_rast(1)),float(ADC_rast(2)),1.)
c
        call hcdir('//PAWC',' ')
      end if
c
      return
      end
c-----------------------------------------------------------


      subroutine rast_init
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


      subroutine raststore
c     =====================
c
      implicit none
      save
c
      include "bcs.inc"
      include "rast.inc"
c      
      integer name_r,ind,nrow,nword,i,j,k, ifirst, mamind
      integer*2 I16(20)            ! 16 bits
      integer*4 I32(10)            ! 32 bits
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      
      if (ifirst.eq.0) then
        name_r = mamind(iw,'FBPM')
        if (name_r.eq.0) then
          print *,' NO FBPM bank registered'
          return
        endif
        ifirst = 1
      endif
c
      ind =iw(name_r) 
      n_rast = 0 
      IRAST_OK = 0

 10   continue
      if(ind.ne.0) then

        nword   = iw(ind)
        nrow    = iw(ind-4)

        do j = 1,nword
          I32(j) = iw(ind + j)
        end do

        k = 0
        do i = 1, nrow
          n_rast = n_rast + 1
          if(n_rast.gt.max_rast) then
            print *,'BAD RASTER number :',k
            stop
          end if
          k = k+1
          id_rast(n_rast) = I16(k)
          k = k+1
          TDC_rast(n_rast) = I16(k)
          k = k+1
          ADC_rast(n_rast) = I16(k)
        end do
        ind = iw(ind-1)
        go to 10
      end if
c
      if(n_rast.eq.2 .and.
     &  ADC_rast(1).le.8000 .and. ADC_rast(2).le.8000) irast_ok = 1

      return
      end




