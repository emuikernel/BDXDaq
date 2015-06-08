c  ec1.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine tagger_book
      implicit none
      save

      integer i
      character*3 id3
      integer dsd_tdc_slot,dsd_adc_slot
      integer dsd_tdc_h(0:63),dsd_adc_h(0:63)       ! DSD channel - histo map
      integer d(0:31)                               ! DSD histogram ID's
      character*2 ps_label(8),pc_label(6),tac_label(4)

C     Mapping between slot.channel and DSD histograms below
      common /map/dsd_tdc_slot,dsd_adc_slot,dsd_tdc_h,dsd_adc_h,d

C     This mapping was true during 1997 photon runs
C      data dsd_tdc_slot /7/
C      data dsd_tdc_h /22*0, 24,17,1,2,3,4,5,6,7,8, 32*0/
C      data dsd_adc_slot /3/
C      data dsd_adc_h /32*0, 
C     &     25,26,27,28,29, 18,19,22,20,21, 9,10,11,12,13,14,15,16, 23, 13*0/

C     This mapping holds from March 98 photon commissioning
      data dsd_tdc_slot /15/
      data dsd_tdc_h 
     & /22*0,                   !  0 - 21
     & 24,                      ! 22            TDC TAC
     & 17,                      ! 23            TDC PC
     & 1,2,3,4,5,6,7,8,         ! 24 - 31       TDC PS
     & 32*0/                    ! 32 - 63
      data dsd_adc_slot /1/
      data dsd_adc_h 
     & /32*0, 			!  0 - 31
     & 25,26,27,28,29, 		! 32 - 36	ADC TAC 
     & 18,19,22,20,21, 		! 37 - 41	ADC PC 
     & 9,10,11,12,13,14,15,16, 	! 42 - 49	ADC PS 
     & 23, 			! 50		ADC PC VE
     & 30,31, 			! 51 - 52	ADC TAC SUM2 and 3
     & 11*0/			! 53 - 63	

*     These lines are used in offline analysis
*      call hmdir('TAG','S')
*      call hcdir('//PAWC',' ')
*      call hmdir('TAG','S')
*      call hcdir('//PAWC/TAG',' ')

      call hmdir('//PAWC/TAG','S')

C     Tagger

      call hbook1(100,'E distribution',384,0.5,384.5,0.)
      call hbook1(101,'T Left distribution',61,0.5,61.5,0.)
      call hbook1(102,'T Right distribution',61,0.5,61.5,0.)
      call hbook2(200,'E timing',384,0.5,384.5,512,-0.5,2047.5,0.)
      call hbook2(201,'T Left timing',61,0.5,61.5,512,-0.5,4095.5,0.)
      call hbook2(202,'T Right timing',61,0.5,61.5,512,-0.5,4095.5,0.)
      call hbook2(210,'T vs E',384,0.5,384.5,61,0.5,61.5,0.)

C     Downstream Devices (DSD)

      do i=0,31
         d(i)=1000+i
      end do

      call hbook1(d(0),'DSD Garbage',1,0.5,1.5,0.)

      call hbook1(d(1),'TDC PS L1',512,-0.5,4095.5,0.)
      call hbook1(d(2),'TDC PS L2',512,-0.5,4095.5,0.)
      call hbook1(d(3),'TDC PS L3',512,-0.5,4095.5,0.)
      call hbook1(d(4),'TDC PS L4',512,-0.5,4095.5,0.)
      call hbook1(d(5),'TDC PS R1',512,-0.5,4095.5,0.)
      call hbook1(d(6),'TDC PS R2',512,-0.5,4095.5,0.)
      call hbook1(d(7),'TDC PS R3',512,-0.5,4095.5,0.)
      call hbook1(d(8),'TDC PS R4',512,-0.5,4095.5,0.)
      call hbook1(d(9), 'ADC PS L1',512,-0.5,8191.5,0.)
      call hbook1(d(10),'ADC PS L2',512,-0.5,8191.5,0.)
      call hbook1(d(11),'ADC PS L3',512,-0.5,8191.5,0.)
      call hbook1(d(12),'ADC PS L4',512,-0.5,8191.5,0.)
      call hbook1(d(13),'ADC PS R1',512,-0.5,8191.5,0.)
      call hbook1(d(14),'ADC PS R2',512,-0.5,8191.5,0.)
      call hbook1(d(15),'ADC PS R3',512,-0.5,8191.5,0.)
      call hbook1(d(16),'ADC PS R4',512,-0.5,8191.5,0.)

      call hbook1(d(17),'TDC PC',512,-0.5,4095.5,0.)
      call hbook1(d(18),'ADC PC MN',512,-0.5,8191.5,0.)
      call hbook1(d(19),'ADC PC LT',512,-0.5,8191.5,0.)
      call hbook1(d(20),'ADC PC RT',512,-0.5,8191.5,0.)
      call hbook1(d(21),'ADC PC LB',512,-0.5,8191.5,0.)
      call hbook1(d(22),'ADC PC RB',512,-0.5,8191.5,0.)
      call hbook1(d(23),'ADC PC VE',512,-0.5,8191.5,0.)

      call hbook1(d(24),'TDC TAC',512,-0.5,4095.5,0.)
      call hbook1(d(25),'ADC TAC LT',512,-0.5,8191.5,0.)
      call hbook1(d(26),'ADC TAC RT',512,-0.5,8191.5,0.)
      call hbook1(d(27),'ADC TAC LB',512,-0.5,8191.5,0.)
      call hbook1(d(28),'ADC TAC RB',512,-0.5,8191.5,0.)
      call hbook1(d(29),'ADC TAC',512,-0.5,8191.5,0.)
      call hbook1(d(30),'ADC TAC 2',512,-0.5,8191.5,0.)
      call hbook1(d(31),'ADC TAC 3',512,-0.5,8191.5,0.)

C     DSD occupancy plots
      
      call hbook1(1101,'PS',8,0.5,8.5,0.)
      data ps_label /'L4','L3','L2','L1','R1','R2','R3','R4'/
      call hlabel(1101,8,ps_label,'n')
      
      call hbook1(1102,'PC',6,0.5,6.5,0.)
      data pc_label /'MN','LB','LT','RT','RB','VE'/
      call hlabel(1102,6,pc_label,'n')
      
      call hbook1(1103,'TAC',4,0.5,4.5,0.)
      data tac_label /'LB','LT','RT','RB'/
      call hlabel(1103,4,tac_label,'n')

C     Tagger and TAC

      do i=1,61,5
         write(id3,'(I3)') i
         call hbook1(2000+i,'Timed ADC TAC if T'//id3,128,-0.5,8191.5,0.)
         call hbook1(2100+i,'All ADC TAC if T'//id3,128,-0.5,8191.5,0.)
      enddo

      call hbook1(2201,'Tagged photons',61,0.5,61.5,0.)
      call hbook1(2202,'Total photons',61,0.5,61.5,0.)

      call hcdir('//PAWC',' ')

      return
      end


c-----------------------------------------------------------


      subroutine tagger_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine tagger_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine tagger_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine tagger_fill
      implicit none
      save

      include "bcs.inc"

      integer mamind
      external mamind
      integer*4 word
      integer nami,ind,nd,i,j
      integer slot,channel,prev_slot
      integer val_e,edge,id_e,hits_e,hit_e(500,2)
      logical t_right,t_left,dsd_tdc,dsd_adc,header
      logical e_good,t_good,tac_good    ! good means on time
      integer val_t,id_t,hit_t(61,2)
      integer adc_tac,tdc_tac
      logical occ(0:31)                             ! DSD occupancy
      integer dsd_tdc_slot,dsd_adc_slot
      integer dsd_tdc_h(0:63),dsd_adc_h(0:63)       ! DSD channel - histo map
      integer d(0:31)                               ! DSD histogram ID's

C     Mapping between slot.channel and DSD histograms
      common /map/dsd_tdc_slot,dsd_adc_slot,dsd_tdc_h,dsd_adc_h,d

      call hcdir('//PAWC/TAG',' ')                ! get into /TAG paw subdir.

c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     Reading E counters from RC17 raw data bank
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      nami = mamind(iw,'RC17')
      ind  = iw(nami)

      nd = 0
      if (ind.ne.0) nd = iw(ind)                       ! number of words

      prev_slot = 0
      hits_e = 0
      do i = 1,nd

         word    = iw(ind+i)
         slot	 = ishft(word,-27)  
         channel = ishft(iand(word,'00FE0000'x),-17)
         val_e	 = iand(word,'0000FFFF'x)              ! TDC1877, 16 bit
         edge	 = ishft(iand(word,'00010000'x),-16)   ! 0=leader 1=trailer
		
         if (slot.ne.prev_slot) then
            id_e = 0
         else if (slot.eq.6) then
            id_e = 1 + channel
         else if (slot.eq.10) then
            id_e = 97 + channel
         else if (slot.eq.14) then
            id_e = 193 + channel
         else if (slot.eq.18) then
            id_e = 289 + channel
         else
            id_e = 0                                   ! error
         end if

         e_good = id_e.gt.0.and.val_e.gt.0

         if (e_good.and.edge.eq.0) then
            hits_e = hits_e + 1
            hit_e(hits_e,1) = id_e
            hit_e(hits_e,2) = val_e
            call hf1(100,float(id_e),1.)
            call hf2(200,float(id_e),float(val_e),1.)
         end if
         prev_slot = slot
           
      end do
	
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     Reading T counters and Downstream Devices from RC12 raw data bank
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      nami = mamind(iw,'RC12')
      ind  = iw(nami)
      
      nd = 0
      if (ind.ne.0) nd = iw(ind)                         ! number of words

      do i=1,61
      do j=1,2
      hit_t(i,j) = 0
      end do
      end do
      prev_slot = 0
      adc_tac = 0
      tdc_tac = 0
      do i=0,31
         occ(i) = .false.
      end do

      do i=1,nd		

         word = iw(ind+i)
         slot = ishft(word,-27)
         
         if (slot.eq.dsd_adc_slot) then                  ! ADC1881, 14 bit
            channel = ishft(iand(word,'00FE0000'x),-17)
            val_t   = iand(word,'00003FFF'x)
         else                                            ! TDC1872, 12 bit
            channel = ishft(iand(word,'007F0000'x),-16)
            val_t   = iand(word,'00000FFF'x)
         end if

C        t_left  = slot.eq.22.and.channel.le.60          ! before run 5119
         t_left  = slot.eq.23.and.channel.le.60
         t_right = slot.eq.20.and.channel.le.60
         dsd_tdc = slot.eq.dsd_tdc_slot
         dsd_adc = slot.eq.dsd_adc_slot
         header  = dsd_adc.and.prev_slot.ne.slot
         t_good  = val_t.gt.0.and.val_t.lt.4000  ! cut out TDC overflow 4093 ?!

         if (t_left.and.t_good) then
            id_t = channel + 1
            call hf1(101,float(id_t),1.)
            call hf2(201,float(id_t),float(val_t),1.)
            hit_t(id_t,1) = val_t
              
         else if (t_right.and.t_good) then
            id_t = channel + 1
            call hf1(102,float(id_t),1.)
            call hf2(202,float(id_t),float(val_t),1.)
            hit_t(id_t,2) = val_t

         else if (header) then
              
         else if (dsd_tdc.and.t_good) then
            id_t = dsd_tdc_h(channel)
            call hf1(d(id_t),float(val_t),1.)
            occ(id_t) = .true.
            if (id_t.eq.24) tdc_tac = val_t

         else if (dsd_adc) then
            id_t = dsd_adc_h(channel)
            call hf1(d(id_t),float(val_t),1.)
            occ(id_t) = .true.
            if (id_t.eq.29) adc_tac = val_t

         else
			
         end if
         prev_slot = slot
			
      end do

c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     Matching T counters with E counters
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      do i=1,hits_e
         e_good = hit_e(i,2).gt.750.and.hit_e(i,2).lt.1220
         if (e_good) then
            do j=1,61
               if (hit_t(j,1).ne.0.or.hit_t(j,2).ne.0) then
                  call hf2(210,float(hit_e(i,1)),float(j),1.)
               end if
            end do
         end if
      end do

c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     Matching T counters with TAC
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      tac_good = tdc_tac.gt.500.and.tdc_tac.lt.4000

      do j=1,61
         if (hit_t(j,1).ne.0.or.hit_t(j,2).ne.0) then
            call hf1(2100+j,float(adc_tac),1.)
            call hf1(2202,float(j),1.)
            if (tac_good) then
               call hf1(2000+j,float(adc_tac),1.)
               call hf1(2201,float(j),1.)
            end if
         end if
      end do

c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     DSD occupancy plots
c++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if (occ(1).and.occ(9)) call hf1(1101,4.,1.)
      if (occ(2).and.occ(10)) call hf1(1101,3.,1.)
      if (occ(3).and.occ(11)) call hf1(1101,2.,1.)
      if (occ(4).and.occ(12)) call hf1(1101,1.,1.)
      if (occ(5).and.occ(13)) call hf1(1101,5.,1.)
      if (occ(6).and.occ(14)) call hf1(1101,6.,1.)
      if (occ(7).and.occ(15)) call hf1(1101,7.,1.)
      if (occ(8).and.occ(16)) call hf1(1101,8.,1.)

      if (occ(17).and.occ(18)) call hf1(1102,1.,1.)
      if (occ(17).and.occ(19)) call hf1(1102,3.,1.)
      if (occ(17).and.occ(20)) call hf1(1102,4.,1.)
      if (occ(17).and.occ(21)) call hf1(1102,2.,1.)
      if (occ(17).and.occ(22)) call hf1(1102,5.,1.)
      if (.not.occ(17).and.occ(23)) call hf1(1102,6.,1.)

      if (occ(24).and.occ(25)) call hf1(1103,2.,1.)
      if (occ(24).and.occ(26)) call hf1(1103,3.,1.)
      if (occ(24).and.occ(27)) call hf1(1103,1.,1.)
      if (occ(24).and.occ(28)) call hf1(1103,4.,1.)

c change back to top directory

      call hcdir('//PAWC',' ')
      return
      end


c-----------------------------------------------------------


      subroutine tagger_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


