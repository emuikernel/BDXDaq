c  ec1.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine call_book


      implicit none
      save


c  executable code:
c  ----------------

      call hmdir('//PAWC/CALL','S')

      call hbook1(1,'RF TDC1',100,0.,200.,0)
      call hbook1(2,'RF TDC2',100,0.,200.,0)

      call hbook1(11,'RF nTDC1',1500,0.,1500.,0)
      call hbook1(12,'RF nTDC2',1500,0.,1500.,0)

      call hbook1(13,'delta RF nTDC1',1500,0.,1500.,0)
      call hbook1(14,'delta RF nTDC2',1500,0.,1500.,0)

      return
      end


c------------------------------------------------------------------------------


      subroutine call_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine call_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine call_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine call_fill

      implicit none
      save

      include 'bcs.inc'
      include 'CALLnt.inc'

      integer i,J,K
      integer ifirst
      integer name_r
      integer ind
      INTEGER mamind
      INTEGER NROW
      INTEGER NWORD

      real RFT1(50)
      real RFT2(50)
      integer nRFT1
      integer nRFT2
      real diff


      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits
      equivalence (I16(1), I32(1))
      data ifirst,name_r/0,0/
      



c  executable code:
c  ----------------
      call hcdir('//PAWC/CALL',' ')    

      if (nCALL.gt.0) then
         do i = 1,nCALL
            if (idCALL(i).eq.6) call hf1(1,0.04906*real(tdcCALL(i)),1.)
            if (idCALL(i).eq.7) call hf1(2,0.04906*real(tdcCALL(i)),1.)
         enddo         
      endif
C DEAL 
      if (ifirst.eq.0) then
        name_r = mamind(iw,'RFT ')
        if (name_r.eq.0) then
          print *,' NO RFT bank registered'
          return
        endif
        ifirst = 1
      endif  

      if(name_r .ne.0) then
         
         ind = iw(name_r)
         nRFT = 0
         if(ind .ne. 0) then
            nword   = iw(ind)
            nrow    = iw(ind-4)
            do j=1,nword
               I32(j)=IW(ind+j) ! bos stored in 32 bit
            end do
            k=0
            
            do i = 1, nrow 
               nRFT 	 = nRFT + 1
               k = k + 1
               idRFT(nRFT)  = I16( k ) 
               k = k + 1
               tdcRFT(nRFT) = I16( k )
            enddo
         endif
      endif
      if (nRFT.gt.0) then
         do i = 1,nRFT
            if (idRFT(i).eq.6) call hf1(11,0.098*real(tdcRFT(i)),1.)
            if (idRFT(i).eq.7) call hf1(12,0.098*real(tdcRFT(i)),1.)
         enddo         

c do time differences
         nRFT1 = 0
         nRFT2 = 0
         do i = 1,nRFT
            if(idRFT(i).eq.6) then
               nRFT1 = nRFT1 + 1
               RFT1(nRFT1) =  0.098*real(tdcRFT(i))
            else if(idRFT(i).eq.7) then
               nRFT2 = nRFT2 + 1
               RFT2(nRFT2) =  0.098*real(tdcRFT(i))
            endif
         enddo
         
         if (nRFT1 .gt. 1) then
            do i = 1, nRFT1
               do k = 1, nRFT1
                  diff = abs(RFT1(i)-RFT1(k))
                  call hf1(13, diff, 1.)
               enddo
            enddo
         endif

         if (nRFT2 .gt. 1) then
            do i = 1, nRFT2
               do k = 1, nRFT2
                  diff = abs(RFT2(i)-RFT2(k))
                  call hf1(14, diff, 1.)
               enddo
            enddo
         endif



      endif
      
      call hcdir('//PAWC',' ')
      
      return
      end


c------------------------------------------------------------------------------


      subroutine call_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine callstore
      include 'bcs.inc'
      include 'CALLnt.inc'      
      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer name_r
      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits
      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save name_i, name_r
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'CALL')
        if (name_i.eq.0) then
          print *,' NO CALL bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)
      nCALL = 0

30    if (ind.ne.0) then
      
        nword   = iw(ind)
        sector  = iw(ind-2)
        nrow    = iw(ind-4)
        
        do j=1,nword
          I32(j)=IW(ind+j)       ! bos stored in 32 bit
        end do
        
        k = 0
      
        do i = 1, nrow 
          nCALL 	 = nCALL + 1
          k 		 = k + 1
          idCALL(nCALL)  = min(48,max(1,I16( k ))) 
          k 		 = k + 1
          tdcCALL(nCALL) = min(4095,max(1,I16( k )))
          k 		 = k + 1
          adcCALL(nCALL) = min(8191,max(1,I16( k )))
        enddo
       
        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end


c-----------------------------------------------------------









