c  photon.f

c  ejw, 13-may-98
c  dwl, 29-may-01  replaced pair counter and old pair spectrometer
c                  histograms with primex pair spectrometer histos


c-----------------------------------------------------------


      subroutine photon_book
      implicit none      
      character*80 title
      character*5 tc_label(23)
cSergey: BUG fixed (was 'cnum(16)')
      character*2 cnum(24)
      character*5 ps_occup_label(35)
      character*5 ps_scat_label_lf(8),ps_scat_label_rf(8),ps_scat_label_lb(8),ps_scat_label_rb(8)
      character*5 ps_front_label(16), ps_back_label(16)
      character*15 ps_ml_logic_label(8)
      character*5 cua_occup_label(4)
      integer i
      
      data TC_label /' ',' ','tdcTC',' ',' ',' '
     %     ,'adcLT',' ','adcRB',' ','adcLB',' ','adcRT'	
     %     ,' ',' ',' ','adcS1',' ','adcS2',' ','adcS3',' ',' '/

      data ps_occup_label/'PSF1',' ','PSF3',' ','PSF5',' ','PSF7',' '
     &     ,' '
     &     ,'PSF9',' ','PSF11',' ','PSF13',' ','PSF15',' '
     &     ,' '
     &     ,'PSB1',' ','PSB3',' ','PSB5',' ','PSB7',' '
     &     ,' '
     &     ,'PSB9',' ','PSB11',' ','PSB13',' ','PSB15',' '/
      
      data ps_scat_label_lf/'PSF1 ','PSF2 ','PSF3 ','PSF4 ','PSF5 ','PSF6 ','PSF7 ','PSF8 '/
      data ps_scat_label_rf/'PSF9 ','PSF10','PSF11','PSF12','PSF13','PSF14','PSF15','PSF16'/
      data ps_scat_label_lb/'PSB1 ','PSB2 ','PSB3 ','PSB4 ','PSB5 ','PSB6 ','PSB7 ','PSB8 '/
      data ps_scat_label_rb/'PSB9 ','PSB10','PSB11','PSB12','PSB13','PSB14','PSB15','PSB16'/
      
      data ps_front_label/'PSF1 ',' ','PSF3 ',' ','PSF5 ',' ','PSF7 ',' '
     &     ,'PSF9 ',' ','PSF11',' ','PSF13',' ','PSF15',' '/
      data ps_back_label/'PSB1 ',' ','PSB3 ',' ','PSB5 ',' ','PSB7 ',' '
     &     ,'PSB9 ',' ','PSB11',' ','PSB13',' ','PSB15',' '/

      data ps_ml_logic_label/'PS-master-true','PS-left-true', 'PS-right-true', 'CUA-master-true'
     &     ,'PS-spare-1', 'PS-spare-2', 'PS-spare-3', 'PS-spare-4'/
      data cua_occup_label/'CUA1','CUA2','CUA3','CUA4'/
      
cSergey: BUG fixed (was 'cnum(16)')
      data cnum/'01','02','03','04','05','06','07','08'
     &          ,'09','10','11','12','13','14','15','16'
     &          ,'17','18','19','20','21','22','23','24'/
      

      call hmdir('//PAWC/PHOTON','S')

c
c
c+ Booking Pair Spectrometer histos
c
c
c--------- Occupancy --------------
      call hbook1(100+1, 'PS Occupancy (TDC)',35,0.5,35.5,0.)
      call hlabel(100+1,35,ps_occup_label,'N')
      call hbook1(100+2, 'PS Occupancy (scaler)',35,0.5,35.5,0.)
      call hlabel(100+2,35,ps_occup_label,'N')
      
c------ Scatter plots correlating front to back counters --------------
      call hbook2(100+3, 'PS Back ID vs. Front ID (left)',8,0.5,8.5,8,0.5,8.5,0.)
      call hlabel(100+3,8,ps_scat_label_lf,'NX')		
      call hlabel(100+3,8,ps_scat_label_lb,'NY')		
      call hbook2(100+4, 'PS Back ID vs. Front ID (right)',8,8.5,16.5,8,8.5,16.5,0.)
      call hlabel(100+4,8,ps_scat_label_rf,'NX')		
      call hlabel(100+4,8,ps_scat_label_rb,'NY')		
c
c ---------- Mid-level logic -------------
      call hbook2(100+5, 'PS midlevel logic',256,0.,4096.,8,0.5,8.5,0.)
      call hlabel(100+5,8,ps_ml_logic_label,'NY')
                
c
c------------ ADC and TDC histoograms -------------
      call hbook2(100+10,'PS Front ID vs. TDC',256,0.,4096., 16,0.5,16.5,0.)
      call hlabel(100+10,16,ps_front_label,'NY')		
      call hbook2(100+11,'PS Back ID vs. TDC',256,0.,4096., 16,0.5,16.5,0.)
      call hlabel(100+11,16,ps_back_label,'NY')		
      
      call hbook2(100+20,'PS Front ID vs. ADC',256,0.,4096., 16,0.5,16.5,0.)
      call hlabel(100+20,16,ps_front_label,'NY')		
      call hbook2(100+21,'PS Back ID vs. ADC',256,0.,4096., 16,0.5,16.5,0.)
      call hlabel(100+21,16,ps_back_label,'NY')		
c
c
c+ Booking CUA/G8 Pair Spectrometer histos
c
c
c--------- Occupancy --------------
      call hbook1(200+1, 'CUA PS Occupancy (TDC)',4,0.5,4.5,0.)
      call hlabel(200+1,4,cua_occup_label,'N')
      call hbook1(200+2, 'CUA PS Occupancy (scaler)',4,0.5,4.5,0.)
      call hlabel(200+2,4,cua_occup_label,'N')
c
c------- Book ADC and TDC histoograms -------------
      call hbook2(200+10,'CUA PS ID vs. TDC',256,0.,4096., 4,0.5,4.5,0.)
      call hlabel(200+10,4,cua_occup_label,'NY')		
      
      call hbook2(200+20,'CUA PS ID vs. ADC',256,0.,4096., 4,0.5,4.5,0.)
      call hlabel(200+20,4,cua_occup_label,'NY')		

c
c

c+ Booking Total Absorption counter histos
c
c 12345678901234567890123
c --x---x-x-x-x---x-x-x--
c   T   A A A A   A A A
c   D   D D D D   D D D
c   C   C C C C   C C C
c   T   L R L R   S S S
c   A   T B B T   U U U
c   C             M M M
c                 1 2 3
      title = '; ;DSTC Population'
      call hbook1(1000+1, title,23,1.,23.,0.)
      call hlabel(1000+1,23,tc_label,'N')
      title = 'DSTC TDC TAC'
      call hbook1(1000+2,title,100,0.,5000.,0.)
      title = 'DSTC ADC Left Top'
      call hbook1(1000+3,title,100,0.,8192.,0.)
      title = 'DSTC ADC Right Top'
      call hbook1(1000+4,title,100,0.,8192.,0.)
      title = 'DSTC ADC Left Bottom'
      call hbook1(1000+5,title,100,0.,8192.,0.)
      title = 'DSTC ADC Right Bottom'
      call hbook1(1000+6,title,100,0.,8192.,0.)
      title = 'DSTC ADC Sum1'
      call hbook1(1000+7,title,100,0.,8192.,0.)
      title = 'DSTC ADC Sum2'
      call hbook1(1000+8,title,100,0.,8192.,0.)
      title = 'DSTC ADC Sum3'
      call hbook1(1000+9,title,100,0.,8192.,0.)
c-
c+ Booking Tagger  Histos
      call hbook1(301,'TAGE; ;Population',384,0.5,384.5,0.)
      call hbook2(302,'TAGE; ;channel',384,0.5,384.5,512,-0.5,2047.5,0.)
      call hbook1(303,'TAGT Left; ;Population',61,0.5,61.5,0.)
      call hbook1(304,'TAGT Right; ;Population',61,0.5,61.5,0.)
      call hbook2(305,'TAGT Left; ;channel',61,0.5,61.5,1000,0,40000.5,0.)
      call hbook2(306,'TAGT Right ; ;channel',61,0.5,61.5,1000,0,40000.5,0.)
      call hbook2(307,';TAGE;TAGT L*R',384,0.5,384.5,61,0.5,63.5,0.)
      call hbook2(308,'TC ID; ;Left-Right',61,0.5,61.5,300,-1000.,2000.,0.)
      call hbook2(309,'TC ID; ;Left-Right',61,0.5,61.5,400,-5.,5.,0.)


c+ CTOF test histograms

      call hbook1(901,'CTOF TDC1' ,1000, 0.,40000.,0.)
      call hbook1(902,'CTOF TDC2' ,1000, 0.,40000.,0.)
      call hbook1(911,'CTOF ADC1' ,512,0.,2047.,0.)
      call hbook1(912,'CTOF ADC2', 512,0.,2047.,0.)

c+ Booking new start counter histograms

      title = 'New Start Counter Population from TDC;ID'
      call hbook1(701, title,24,0.5,24.5,0.)
      title = 'New Start Counter Population from ADC;ID'
      call hbook1(702, title,24,0.5,24.5,0.)
      call hbook2(711,'New Start Counter;TDC;ID',400,2000.,10000.,24,0.5,24.5,0.)
      call hbook2(712,'New Start Counter;ADC;ID',128,0.,4095.,24,0.5,24.5,0.)
      call hbook1(713,'New ST multiplicity', 25, -0.5, 24.5,0.)
      call hbook1(715, 'STX-STY time difference', 2000, -1000., 1000.,0.)
      call hbook1(716, 'STXL-STYL time difference', 2000, -1000., 1000.,0.)
      call hbook1(717, 'STXS-STYS time difference', 2000, -1000., 1000.,0.)
      call hbook1(718, 'STXL-STYS time difference', 2000, -1000., 1000.,0.)

      do i=1,24
         title = 'New ST '//cnum(i)//';TDC'
         call hbook1(720+i,title,800,2000.,10000.,0.)
         title = 'New ST '//cnum(i)//';ADC'
         call hbook1(750+i,title,512,0.,2047.,0.)
         title = 'ST TDC '//cnum(i)//';ADC'
         call hbook2(800+i,title, 100, 0., 10000., 128, 0., 2047., 0.)
      end do


c     booking histos for T-counter multihit TDCs
      call hbook1(401,'Norm T Population',61,0.5, 61.5,0.)
      call hbook2(402,'Norm T TDCs',61,0.5,61.5,512,-0.5,2047.5,0.)
c booking "beam spectrum" histos
      call hbook1(410,'photons ALL rtriggers',384,0.5,384.5,0.)
      call hbook1(411,'photons from bit1',384,0.5,384.5,0.)
      call hbook1(412,'photons from bit2',384,0.5,384.5,0.)
      call hbook1(413,'photons from bit3',384,0.5,384.5,0.)
      call hbook1(414,'photons from bit4&5',384,0.5,384.5,0.)

      call HBoOK1(600,'E-hits per channel', 384, 0.5, 384.5, 0.)
      call HBoOK1(601,'T and PS hits per channel', 200, 0.5, 200.5, 0.)

      call hbook1(1301,'TAGE NEW; ;Population',32,0.5,32.5,0.)
      call hbook2(1302,'TAGE NEW; ;channel',32,0.5,32.5,512,-0.5,2047.5,0.)
      call HBoOK1(1600,'E-hits per channel NEW', 32, 0.5, 32.5, 0.)


c      call hbook1(311,'TAG E;  ;20',512,-0,5,2047.5,0.)
c      call hbook1(312,'TAG E;  ;40',512,-0,5,2047.5,0.)
c      call hbook1(313,'TAG E;  ;60',512,-0,5,2047.5,0.)
c      call hbook1(314,'TAG E;  ;120',512,-0,5,2047.5,0.)
c      call hbook1(315,'TAG E;  ;140',512,-0,5,2047.5,0.)
c      call hbook1(316,'TAG E;  ;160',512,-0,5,2047.5,0.)

c-
c+ Booking StartCounter histograms
      title = 'Start Counter Population;TDC'
      call hbook1(501, title,6,1.,7.,0.)
      title = 'Start Counter Population;ADC'
      call hbook1(502, title,6,1.,7.,0.)
      call hbook2(511,'Start Counter;TDC counts;ID',128,0.,4097.,6,1.,7.,0.)
      call hbook2(512,'Start Counter;ADC counts;ID',128,0.,8193.,6,1.,7.,0.)
      do i=1,6
         title = 'Start counter '//cnum(i)//';TDC channels'
         call hbook1(520+i,title,128,0.,4097.,0.)
         title = 'Start counter '//cnum(i)//';ADC channels'
         call hbook1(530+i,title,128,0.,8193.,0.)
      end do


      title = 'MOR-TLi; TLid'
      call hbook2(2001, title, 600, -170., -20., 61, 0.5, 61.5, 0.)
      title = 'MOR-TRi; TRid'
      call hbook2(2002, title, 600, -170., -20., 61, 0.5, 61.5, 0.)
      title = 'MOR-TLi; TLid single TAC'
      call hbook2(2003, title, 600, -170., -20., 61, 0.5, 61.5, 0.)
      title = 'MOR-TRi; TRid single TAC'
      call hbook2(2004, title, 600, -170., -20., 61, 0.5, 61.5, 0.)


      print *,' Photon histograms booked'
      return
      end


c-----------------------------------------------------------


      subroutine photon_bor
c  called at bor

c  vvsap, 5-Mar-98


      implicit none
      include 'PHOTONnt.inc'
      save
      firsttime = 0

c  executable code:
c  ----------------

c      print *,'Read PHOTON (DSD/TAG) pedestals from Map - Run ',current_run
c
c    read the pedestal from $CLAS_PARMS/PHOTON_PEDESTALS.map
c
c      call  photonreadpedmap(current_run)

c+ Reading ADCs pedestals (and others calibration constant) from file

      call photon_pedread


      return
      end



c-----------------------------------------------------------


      subroutine photon_done

c  called at done

c  ejw, 10-oct-96



      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine photon_eor

c  called at eor

c  vvsap, 4-mar-98


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------



      subroutine photon_fill
      implicit none
      include 'PHOTONnt.inc'
      integer i,j,k,m,id,offset,st_mult
      real temp0(384), temp1(200), temp2(32)
      real t_ref(61)
      real tac_mor_diff
      common/tc_ref/ t_ref

      call hcdir('//PAWC/PHOTON',' ')
      
c     -
c     + Filling Pair Spectrometer histos
c     
c     We want to record all hits in the PS multi-hit TDC's here
c     as well as all possible combinations in the scatter plots
c     D. L. 5/29/01
c     
      
c     
c-----Occupancy ------
      offset=0
      do j=1,16
         
c     offset accounts for the empty bins used to make the plot pretty
         IF (j.gt.8) THEN
            offset=1
         ENDIF
         IF (PSFT_hits(j).gt.0) THEN
            id = j + offset
            call hf1(100+1, 1.0*id, 1.0*PSFT_hits(j))
            do i=1,PSFT_hits(j)
               call hf2(100+10, 1.0*PSF_TDC(i,j), 1.0*j, 1.0)
            enddo
         ENDIF
         IF (PSBT_hits(j).gt.0) THEN
            id = j + (16+2) + offset
            call hf1(100+1, 1.0*id, 1.0*PSBT_hits(j))
            do i=1,PSBT_hits(j)
               call hf2(100+11, 1.0*PSB_TDC(i,j), 1.0*j, 1.0)
            enddo
         ENDIF
         
         IF (PSF_scaler(j).gt.0) THEN
            id = j + offset
            call hf1(100+2, 1.0*id, 1.0*PSF_scaler(j))
         ENDIF
         IF (PSB_scaler(j).gt.0) THEN
            id = j + (16+2) + offset
            call hf1(100+2, 1.0*id, 1.0*PSB_scaler(j))
         ENDIF
         
         IF (PSF_ADC(j).gt.0.0) THEN
            call hf2(100+20, 1.0*PSF_ADC(j), 1.0*j, 1.0)
         ENDIF
         IF (PSB_ADC(j).gt.0.0) THEN
            call hf2(100+21, 1.0*PSB_ADC(j), 1.0*j, 1.0)
         ENDIF
         IF (PS_logic_hits(j).gt.0) THEN
            do i=1,PS_logic_hits(j)
               call hf2(100+5, 1.0*PS_logic_TDC(i,j), 1.0*j, 1.0)
            enddo
         ENDIF
      enddo
      
c     Look for correlated front/back TDC hits. Plot all pairs
c     within 40 counts (~20ns) and do it separatley for left and right

c     -- LEFT --
      do i=1,8
         do j=1,PSFT_hits(i)
            do k=1,8
               do m=1,PSBT_hits(k)
                  if (abs(PSF_TDC(j,i)-PSB_TDC(m,k)).le.40) then
                     call hf2(100+3, 1.0*i, 1.0*k, 1.0)
                  endif
               enddo
            enddo
         enddo
      enddo
        
c     -- RIGHT -- 
      do i=9,16
         do j=1,PSFT_hits(i)
            do k=9,16
               do m=1,PSBT_hits(k)
                  if (abs(PSF_TDC(j,i)-PSB_TDC(m,k)).le.40) then
                     call hf2(100+4, 1.0*i, 1.0*k, 1.0)
                  endif
               enddo
            enddo
         enddo
      enddo
      
c---------CUA Pair Spectrometer ----------
      do j=1,4
         if (CUA_hits(j).gt.0) then
            call hf1(200+1, 1.0*j, 1.0*CUA_hits(j))
            do i=1,CUA_hits(j)
               call hf2(200+10, 1.0*CUA_TDC(i,j), 1.0*j, 1.0)
            enddo
         endif
         if (CUA_scaler(j).gt.0) then
            call hf1(200+2, 1.0*j, 1.0*CUA_scaler(j))
         endif
         if (CUA_ADC(j).gt.0.0) then
            call hf2(200+20, 1.0*CUA_ADC(j), 1.0*j, 1.0)
         endif
      enddo
c     - 
c+ Filling Total Absorption counter histos
c
c 12345678901234567890123
c --x---x-x-x-x---x-x-x--
c   T   A A A A   A A A
c   D   D D D D   D D D
c   C   C C C C   C C C
c   T   L R L R   S S S
c   A   T B B T   U U U
c   C             M M M
c                 1 2 3

      if(TDCTA_tc.gt.0)  then 
         call hf1(1000+1,3.,1.)
         call hf1(1000+2,1.*TDCTA_tc,1.)
      endif
      if(ADCLT_tc.gt.0)  then 
         call hf1(1000+1,7.,1.)
         call hf1(1000+3,1.*ADCLT_tc,1.)
      endif
      if(ADCRT_tc.gt.0)  then 
         call hf1(1000+1,9.,1.)
         call hf1(1000+4,1.*ADCRT_tc,1.)
      endif
      if(ADCLB_tc.gt.0)  then 
         call hf1(1000+1,11.,1.)
         call hf1(1000+5,1.*ADCLB_tc,1.)
      endif
      if(ADCRB_tc.gt.0)  then 
         call hf1(1000+1,13.,1.)
         call hf1(1000+6,1.*ADCRB_tc,1.)
      endif
      if(ADCS1_tc.gt.0)  then 
         call hf1(1000+1,17.,1.)
         call hf1(1000+7,1.*ADCS1_tc,1.)
      endif
      if(ADCS2_tc.gt.0)  then 
         call hf1(1000+1,19.,1.)
         call hf1(1000+8,1.*ADCS2_tc,1.)
      endif
      if(ADCS3_tc.gt.0)  then 
         call hf1(1000+1,21.,1.)
         call hf1(1000+9,1.*ADCS3_tc,1.)
      endif
c-
c+ Filling Tagger histos
c++ TAGE histos
      if(E_N .ne. 0) then
         do j=1,E_N
            if(E_TDC(j) .gt. 0 .AND. E_TDC(j).lt.4000)  then 
               call hf1(300+1,1.*E_ID(j),1.)
               call hf2(300+2,1.*E_ID(j),1.*E_TDC(j),1.)
c     if(ID_tage(j).eq.20)call hf1(311,1.*TDC_tage(j),1.)
c     if(ID_tage(j).eq.40)call hf1(312,1.*TDC_tage(j),1.)
c     if(ID_tage(j).eq.60)call hf1(313,1.*TDC_tage(j),1.)
c     if(ID_tage(j).eq.120)call hf1(314,1.*TDC_tage(j),1.)
c     if(ID_tage(j).eq.140)call hf1(315,1.*TDC_tage(j),1.)
c     if(ID_tage(j).eq.160)call hf1(316,1.*TDC_tage(j),1.)
            endif 
         enddo
      endif

      if(E_N_NEW .ne. 0) then
         do j=1,E_N_NEW
            if(E_TDC_NEW(j) .gt. 0 .AND. E_TDC_NEW(j).lt.4000)  then 
               call hf1(1300+1,1.*E_ID_NEW(j),1.)
               call hf2(1300+2,1.*E_ID_NEW(j),1.*E_TDC_NEW(j),1.)
            endif 
         enddo
      endif
c-
c+ Filling TAGE NEW histos 


c--   
      if(nTATL.ne.0) then
         do j= 1,nTATL
c            if(TDC_tatl(j).gt.0) then
               call hf1(300+3,1.*ID_tatl(j),1.)
               call hf2(300+5,1.*ID_tatl(j),1.*TDC_tatl(j),1.)
c            endif
         enddo
      endif

      if(nTATR.ne.0) then
         do j= 1,nTATR
c            if(TDC_tatr(j).gt.0) then
               call hf1(300+4,1.*ID_tatr(j),1.)
               call hf2(300+6,1.*ID_tatr(j),1.*TDC_tatr(j),1.)
c            endif
         enddo
      endif

c++ TL-TR monitoring (possible shifts due to beam bursts
c   use refernce offsets t_ref to allign them all at 0.

      if(nTATL.ne.0.and.nTATR.ne.0) then
         do j= 1,nTATL
            do i= 1,nTATR
               if(ID_tatl(j).eq.ID_tatr(i)) then
                  call hf2(300+8, 1.*ID_tatl(j), 
     +                 1.*(TDC_tatl(j)-TDC_tatr(i)),1.)
c                  dt = TDC_tatl(j)-TDC_tatr(i)-t_ref(ID_tatl(j)

                 call hf2(300+9, 1.*ID_tatl(j),0.025*
     +                 (TDC_tatl(j)-TDC_tatr(i)-t_ref(ID_tatl(j))),1.)
               endif
            enddo
         enddo
      endif

c deal with MOR timing

      if (nMOR .ne. 0 .and. TDCTA_tc .ne. 0) then
         do j=1, nMOR
            if (nTATL .ne. 0) then
               do i=1,nTATL
                  if (ID_tatl(i) .gt.0 .and. ID_tatl(i) .le. 61) then
                     tac_mor_diff = 
     +                    (390.-TDCTA_tc*0.098)-(700.-0.0244*TDC_MOR(i))
                     call hf2(2001, tac_mor_diff, 1.*ID_tatl(i), 1.)
                     if (nMOR .eq.1)
     +                    call hf2(2004, tac_mor_diff, 1.*ID_tatl(i), 1.)
                 endif
               enddo
            endif
            if (nTATR .ne. 0) then
               do i=1,nTATR
                  if (ID_tatr(i) .gt.0 .and. ID_tatr(i) .le. 61) then
                     tac_mor_diff = 
     +                    (390.-TDCTA_tc*0.098)-(700.-0.0244*TDC_MOR(i))
                     call hf2(2002, tac_mor_diff, 1.*ID_tatr(i), 1.)
                     if (nMOR .eq.1)
     +                    call hf2(2004, tac_mor_diff, 1.*ID_tatr(i), 1.)
                  endif
               enddo
            endif
         enddo
      endif
      
c++ TAGT histos
      if(nTAGT.ne.0) then
         do j=1,nTAGT
            if(TDCL_tagt(j).gt.0 .AND. TDCL_tagt(j).lt.4000)  then 
               call hf1(300+3,1.*ID_tagt(j),1.)
               call hf2(300+5,1.*ID_tagt(j),1.*TDCL_tagt(j),1.)
            endif
            if(TDCR_tagt(j).gt.0 .AND. TDCR_tagt(j).lt.4000)  then 
               call hf1(300+4,1.*ID_tagt(j),1.)
               call hf2(300+6,1.*ID_tagt(j),1.*TDCR_tagt(j),1.)
            endif
         enddo
      endif
c++   NTAGT histograms
      if(NT_N .ne. 0) then
         do j=1,NT_N
            if(NT_TDC(j) .gt. 0 .and. NT_TDC(j) .lt. 4000) then
               call hf1(401,1.*NT_ID(j),1.)
               call hf2(402,1.*NT_ID(j),1.*NT_TDC(j),1.)
            endif
         enddo
      endif
c--   
c++ Matching E and T counters ((TL.or.TR).and.E) 
c++ more than 1 hit allowed
      if(E_N.ne.0.and.nTAGT.ne.0) then
         do i=1,E_N  
            do j=1,nTAGT
               if(E_TDC(i) .gt. 750 .and. E_TDC(i) .lt. 1250  
     +              .and. (TDCL_tagt(j).gt.700 .and. TDCL_tagt(j).lt.3000)
     +              .and. (TDCR_tagt(j).gt.700 .and. TDCR_tagt(j).lt.3000))
     &              call hf2(300+7,1.*E_ID(i),1.*ID_tagt(j),1.)
            enddo
         enddo
         do i=1,E_N
            if(E_TDC(i) .gt. 750 .and. E_TDC(i) .lt. 1250) then
               do j=1,nTAGT
                  if(ID_tagt(j) .ge. ET_COIN(i,1) 
     &                 .and. ID_tagt(j) .le. ET_COIN(i,2) 
     +                 .and. (TDCL_tagt(j).gt.700 .and. TDCL_tagt(j).lt.3000)
     +                 .and. (TDCR_tagt(j).gt.700 .and. TDCR_tagt(j).lt.3000)) then
                     call hf1(410,1.*E_ID(i),1.)
                     if(iand(TGBI_latch,1) .ne. 0) call hf1(411,1.*E_ID(i),1.) !bit #1
                     if(iand(TGBI_latch,2) .ne. 0) call hf1(412,1.*E_ID(i),1.) !bit #2
                     if(iand(TGBI_latch,4) .ne. 0) call hf1(413,1.*E_ID(i),1.) !bit #3
                     if(iand(TGBI_latch,24) .ne. 0) call hf1(414,1.*E_ID(i),1.) !bits #4&5
                  endif
               enddo
            endif
         enddo
      endif

c-- now fill hit multiplicity  for 1877 in TAGE

      if(firsttime .eq. 0) then
         if(E_N .ne. 0) then
            call hpak(600,tage0)
         endif
         if(nTAGE1 .ne. 0) then
            call hpak(601,tage1)
         endif
cc
         if(E_N_NEW .ne. 0) then
            call hpak(1600,tage2)
         endif
cc
         firsttime = 1
      else
         if(E_N .ne. 0) then
            call hunpak(600,temp0,' ',0)
            do i = 1,384
               temp0(i) = 0.95*temp0(i) + 0.05*tage0(i)
            enddo
            call hpak(600,temp0)
         endif
         if(nTAGE1 .ne. 0) then
            call hunpak(601,temp1,' ',0)
            do i = 1,192
               temp1(i) = 0.95*temp1(i) + 0.05*tage1(i)
            enddo
             call hpak(601,temp1)
         endif
cc
         if(E_N_NEW .ne. 0) then
            call hunpak(1600,temp2,' ',0)
            do i = 1,32
               temp2(i) = 0.95*temp2(i) + 0.05*tage2(i)
            enddo
            call hpak(1600,temp2)
         endif

cc
      endif

c--
c++ Resetting DSD/TAGGER vectors
      do i = 1,nDSPS
         ID_ps (i) = 0
         TDC_ps(i) = 0
         ADC_ps(i) = 0
      enddo
      do j=1,nTAGE1
         ID_tage(j) = 0
         TDC_tage(j) = 0
      enddo
      do j=1,nTAGT
         ID_tagt(j) = 0
         TDCL_tagt(j) = 0
         TDCR_tagt(j) = 0
      enddo
c--

c++ CTOF
      if(nCTOF1 .ne. 0) then
         do j=1,nCTOF1
            call hf1(901,1.*TDC_CTOF1(j), 1.)
         enddo
      endif
      if(nCTOF2 .ne. 0) then
         do j=1,nCTOF2
            call hf1(902,1.*TDC_CTOF2(j), 1.)
         enddo
      endif

      if(ADC_CTOF1 .ne.0) call hf1(911, 1.*ADC_CTOF1, 1.)
      if(ADC_CTOF2 .ne.0) call hf1(912, 1.*ADC_CTOF2, 1.)


c++ New Start Counter histos
c-- TDCs
      st_mult = 0
      if(nSTn0.ne.0) then
         do j=1,nSTn0

C sergey: upper limit missing ?? if(TDC_stn0(j).gt.10 .AND. TDC_stn0(j))  then 
            if(TDC_stn0(j).gt.10 .AND. TDC_stn0(j).lt.100000)  then 
               st_mult = st_mult+1
               call hf1(701,1.*ID_stn0(j),1.)
               call hf2(711,1.*TDC_stn0(j),1.*ID_stn0(j),1.)
               call hf1(720+ID_stn0(j),1.*TDC_stn0(j),1.)
               do i=1, nSTn0
                  if(TDC_stn0(i).gt.10 .AND. TDC_stn0(i).lt.100000
     +                 .and.i.ne.j)  then
                     call hf1(715,1.*(TDC_stn0(i)-TDC_stn0(j)),1.)
                     
                  endif
               enddo
            endif
               
         enddo
         call hf1(713, 1.*st_mult, 1.)



      endif

c--   ADCs
      if(nSTn1.ne.0) then
         do j=1,nSTn1
            if(ADC_stn1(j).gt.0)  then 
               call hf1(702,1.*ID_stn1(j),1.)
            endif
            call hf2(712,1.*ADC_stn1(j),1.*ID_stn1(j),1.)
            call hf1(750+ID_stn1(j),1.*ADC_stn1(j),1.)
c     ADCxTDC
            if(nSTn0.ne.0) then
               do i=1,nSTn0
                  if(ID_stn1(j) .eq. ID_stn0(i)) then
                     call hf2(800+ID_stn1(j), 
     +                    1.*TDC_stn0(i), 1.*ADC_stn1(j), 1.)
                  endif
               enddo
            endif
         enddo
      endif
      




c++ Start Counter histos
      if(nST.ne.0) then
         do j=1,nST
            if(TDC_st(j).gt.0)  then 
               call hf1(501,1.*ID_st(j),1.)
               call hf2(511,1.*TDC_st(j),1.*ID_st(j),1.)
               call hf1(520+ID_st(j),1.*TDC_st(j),1.)
            endif
            if(ADC_st(j).gt.0)  then 
               call hf1(502,1.*ID_st(j),1.)
               call hf2(512,1.*ADC_st(j),1.*ID_st(j),1.)
               call hf1(530+ID_st(j),1.*ADC_st(j),1.)
            endif
         enddo
      endif
c-      
      call hcdir('//PAWC',' ')
      return
      end
       

c-----------------------------------------------------------


      subroutine photon_init

c  called at init

c  vvsap, 4-mar-98
      implicit none
      include 'PHOTONnt.inc'
      integer i,j,lenocc
      character*80 file_name,file_name1
      character*25 clon_parms
      real t_ref(61)
C sergey      data t_ref/61*0./
      common/tc_ref/ t_ref

      save
      
C sergey: add following loop
      do j=1,61
        t_ref(j) = 0.
      enddo

      file_name='/group/clas/parms/tagETcoinc.dat'
      
      call getenvf('CLON_PARMS',clon_parms)
      
      
c     executable code:
c     ----------------
      i=0
      open (unit=51,file=file_name,status='old',err=9)
      do i=1,384
         read(51,100,err=9) j,ET_coin(j,1),ET_coin(j,2)
      enddo
      close(51)
      print *,'Read file ',file_name

      i=0
 8    file_name1=clon_parms(1:lenocc(clon_parms))//'/evmon/tc_ref.dat'

c 8    file_name1 = '/usr/local/clas/parms/evmon/tc_ref.dat'

      open (unit=52,file=file_name1,status='old',err=10)
      do i=1,61
         read(52,101,err=10) t_ref(i) 
         print *,t_ref(i)
      enddo
      close(52)
      print *,'Read file ',file_name1
      return

 9    continue
      if(i.gt.0) then
      print *,'Error reading ',file_name,': line=',i 
      else
      print *,'Input file ',file_name,' does NOT exist'
      endif
      goto 8
 10    continue
      if(i.gt.0) then
      print *,'Error reading ',file_name1,': line=',i 
      else
      print *,'Input file ',file_name1,' does NOT exist'
      endif
      return
 100  format(i4,i3,i3)
 101  format(g12.0)
      end
      

c-----------------------------------------------------------


      subroutine photon_pedread
      
      include 'PHOTONnt.inc'
      
      CHARACTER *80 file_name
      character *100 comments
      real value
      integer i


c+ Checking existence of pedestal file
c        file_name='/home/anciant/PARAMETERS/dsdcal.dat'
c        file_name='/home/battagli/PARAMETERS/dsdcal.dat'
      file_name='/usr/local/clas/parms/dsdcal.dat'

      open (unit=50,file=file_name,status='old',err=2)
      goto 3
 2    continue
      print *,'Input file ',file_name,' does NOT exist'
      goto 105
 3    continue
      close (50)
      open(50,status='old',err=105,file=file_name)
 5    format(f8.2,a100)
	
c ---- PC

      read(50,5) value,comments
      pc_ped_ve = value
      read(50,5) value,comments
      pc_ped_mn = value
      read(50,5) value,comments
      pc_ped_lt = value
      read(50,5) value,comments
      pc_ped_rb = value
      read(50,5) value,comments
      pc_ped_lb = value
      read(50,5) value,comments
      pc_ped_rt = value
      
      read(50,5) value,comments
      pc_TDC_w = value
      read(50,5) value,comments
      posPCpeak = value
		
c ----- PS

      do i= 1,8
	 read(50,5) value,comments
	 ps_ped(i) = value
      enddo
	
      do i= 1,8
	 read(50,5) value,comments
	 ps_tdc_w(i) = value
      enddo
      
      do i= 1,8
         read(50,5) value,comments
         posPSpeak(i) = value
      enddo

      read(50,5) value,comments
      timwk1 = value
      read(50,5) value,comments
      timwk2 = value
      read(50,5) value,comments
      timwk3 = value
      read(50,5) value,comments
      PSthreshold = value
	


c ---- TAC
	
      read(50,5) value,comments
      tac_ped_lt = value
      read(50,5) value,comments
      tac_ped_rt = value
      read(50,5) value,comments
      tac_ped_lb = value
      read(50,5) value,comments
      tac_ped_rb = value
      read(50,5) value,comments
      tac_ped_sum1 = value
      read(50,5) value,comments
      tac_ped_sum2 = value
      read(50,5) value,comments
      tac_ped_sum3 = value
      
      read(50,5) value,comments
      tac_sca_lt = value
      read(50,5) value,comments
      tac_sca_rt = value
      read(50,5) value,comments
      tac_sca_lb = value
      read(50,5) value,comments
      tac_sca_rb = value
      read(50,5) value,comments
      tac_sca_sum1 = value
      read(50,5) value,comments
      tac_sca_sum2 = value
      read(50,5) value,comments
      tac_sca_sum3 = value
      
      read(50,5) value,comments
      tac_TDC_w = value
      read(50,5) value,comments
      posTACpeak = value
      
      close(50)
 105  continue
      return
      end


c-----------------------------------------------------------


      subroutine photonstore

c  vvsap, 4-mar-98
c Corvisiero & Battaglieri 23-3-98
c  vvsap, 28-mar-98
c  davidl, 2-june-01

      implicit none

      include 'bcs.inc'
      include 'PHOTONnt.inc'
      integer ttlid
      integer 
     &     name_tc,name_tage0,name_tage1,name_tagt,name_prim,name_st
     &     ,name_head
     &     ,ind_tc,ind_tage0,ind_tage1,ind_tagt,ind_prim,ind_st
     &     ,ind_head
     &     ,ifirst,k,i,mamind,id,tdc_val,nPRIM,ievtype
     &     ,name_tgbi,ind_tgbi,mlink, name_tatl, name_tatr, ind_tatl, 
     &     ind_tatr,name_stn0, name_stn1, ind_stn0, ind_stn1,st_mult
      data ifirst,nDSPS,nTAGE0,nTAGE1,nTAGT,nPRIM,nST/0,0,0,0,0,0,0/ 
      data nTATL, nTATR, nSTN0, nSTN1/0,0,0,0/
      save name_tc,name_tagt,name_st
     &     ,name_tatl,name_tatr,name_stn0, name_stn1

c reset all counters 

      nDSPS = 0
      nTAGE0 = 0 
      nTAGE1 = 0
      nTAGT = 0
      nPRIM = 0
      nST = 0
      nTATL = 0
      nTATR = 0
      nSTN0 = 0
      nSTN1 = 0
      nMOR = 0
      nCTOF1 = 0 
      nCTOF2 = 0
      ADC_CTOF1 = 0
      ADC_CTOF2 = 0
      
      if (ifirst.eq.0) then
         name_tc = mamind(iw,'DSTC')
         name_tage1=mamind(iw,'TAGE')
         name_tagt = mamind(iw,'TAGT')
         name_tatl = mamind(iw,'TGTL')
         name_tatr = mamind(iw,'TGTR')
         name_stn0 = mamind(iw,'STN0')
         name_stn1 = mamind(iw,'STN1')
         name_head = mamind(iw,'HEAD')

         name_prim = mamind(iw,'PRIM')
         name_st = mamind(iw,'ST  ')
         name_tgbi = mamind(iw,'TGBI')
         if (name_tc.eq.0) then
            print *,' NO DSTC bank registered'
c         else if (name_tage0.eq.0) then
c            print *,' NO TAGE0 bank registered'
         else if (name_tage1.eq.0) then
            print *,' NO TAGE1 bank registered'
         else if (name_tagt.eq.0) then
            print *,' NO TAGT bank registered'
         else if (name_tatl.eq.0) then
            print *,' NO TGTL bank registered'
         else if (name_tatr.eq.0) then
            print *,' NO TGTR bank registered'
         else if (name_st.eq.0) then
            print *,' NO ST bank registered'
         else if (name_tgbi .eq. 0) then
            print *,' NO TGBI bank registered'
         else if (name_stn0.eq.0) then
            print *,' NO STN0 bank registered'
         else if (name_stn1.eq.0) then
            print *,' NO STN1 bank registered'
         endif
         ifirst = 1
      endif

c check for HEAD bank
      if(name_head.NE.0) then
         ind_head = IW(name_head)
      else
         ind_head=0
      endif
      if (ind_head.eq.0) then
         print *,'ERROR: NO HEAD bank found',name_head
         return
      endif
c check for event type
      ievtype = IW(ind_head+5)
      if(ievtype.LT.1.OR.ievtype.GT.9) then
        return
      endif

! record no. changed for Dennis & the C++ crew
c     ind_tage0 = mlink(iw,'TAGE',1) ! old TAGE (E-counters)
c      ind_tage1 = mlink(iw,'TAGE',1) ! everything is in one bank
      if(name_tage1.NE.0) then
         ind_tage1 = IW(name_tage1)
      else
         ind_tage1=0
      endif
c      if (ind_tage1.eq.0) then
c         print *,' NO TAGE bank found',name_tage1
c      endif
      if ((name_tc+ind_tage1+name_tagt+name_st).eq.0) then
         print *,' NO PHOTON banks at all'
         return          
      endif

c read TGBI bank and get trigger level 1 latch word
      if(name_tgbi .ne. 0) then
         ind_tgbi = IW(name_tgbi)
         TGBI_latch = IW(ind_tgbi+1)
      else
         TGBI_latch = 0
      endif
      
c read DSTC bank
      if (name_tc.ne.0) then
         ind_tc = iw(name_tc)      
         if (ind_tc.ne.0) then
            k = 2*ind_tc        ! k is pointed before data words for iw16 array
            ID_tc    = IW16(k+1)
            TDCTA_tc = IW16(k+2)
            ADCLT_tc = IW16(k+3)-tac_ped_lt
            ADCRT_tc = IW16(k+4)-tac_ped_rt 
            ADCLB_tc = IW16(k+5)-tac_ped_lb
            ADCRB_tc = IW16(k+6)-tac_ped_rb
            ADCS1_tc = IW16(k+7)-tac_ped_sum1
            ADCS2_tc = IW16(k+8)-tac_ped_sum2
            ADCS3_tc = IW16(k+9)-tac_ped_sum3
         else
            TDCTA_tc = 0
            ADCLT_tc = 0
            ADCRT_tc = 0
            ADCLB_tc = 0
            ADCRB_tc = 0
            ADCS1_tc = 0
            ADCS2_tc = 0
            ADCS3_tc = 0
         endif
      endif

      do i=1,200
         tage1(i) = 0.
      enddo
      do i=1,384
         tage0(i) = 0.
      enddo
      do i=1,32
         tage2(i) = 0.
      enddo

c     read TAGE 1 bank (T-counter 1877 and PS) 
      if (ind_tage1 .ne. 0) then
c     ind_tage1 = iw(name_tage1)
c     if (ind_tage .ne. 0) then
         nTAGE1    = iwrow(ind_tage1)
c     get away if TAGE has more then 576*16 hits (6 boards of 1877)
         if (nTAGE1 .gt. 9216) then
            nTAGE1 = 0      
            nDSPS = 0    
            nTAGT = 0
            nST   = 0
            NT_N  = 0
            E_N   = 0
            E_N_NEW = 0
            return
         else
c  get E-counters     
            k = 2*ind_tage1     ! k is pointed before data words for iw16 array
            E_N  = 0
            do i = 1, nTAGE1
               ID_tage (i) = IW16(k+1)
               TDC_tage(i) = IW16(k+2)
               if(id_tage(i) .le. 384 .and. id_tage(i) .gt. 0) then
                  E_N        = E_N + 1
                  E_ID(E_N)  = id_tage(i)
                  E_TDC(E_N) = TDC_tage(i)
                  TAGE0(E_ID(E_N))= TAGE0(E_ID(E_N))+1 ! hit multiplicity for E-counters                  
               endif
               k = k + 2
            enddo

ccc TAGE NEW
            E_N_NEW  = 0
            do i = 1, nTAGE1
               if(ID_tage(i) .GE. 1281 .and. ID_tage(i) .LE. 1312) then
                  k= ID_tage(i) - 1280
                  E_N_NEW    = E_N_NEW + 1
                  E_ID_NEW(E_N_NEW)  = k
                  E_TDC_NEW(E_N_NEW) = TDC_tage(i)
                  TAGE2(E_ID_NEW(E_N_NEW))= TAGE2(E_ID_NEW(E_N_NEW))+1 ! hit multiplicity for NEW E-counters                  
               endif
            enddo


ccc


c     get T-counters           
            NT_N = 0
            DO i = 1, nTAGE1
               if(ID_tage(i)/256 .eq. 2) then
                  k = mod(ID_tage(i),256)
                  if(k .le. 61) then
                     NT_N         = NT_N + 1
                     NT_ID(NT_N)  = k
                     NT_TDC(NT_N) = TDC_tage(i)
                     TAGE1(k) = TAGE1(k) + 1 ! hit multiplicity for T-counters
                  endif
               endif
            enddo
         endif
      else
         nTAGE1 = 0
      endif




c now read TAGE 0 bank (old bank, E-counters)
c      E_N = 0
c      if (ind_tage0 .ne. 0) then
c         nTAGE0    = iwrow(ind_tage0)
c         k = 2*ind_tage0        ! k is pointed before data words for iw16 array
c         do i=1, nTAGE0
c            id = IW16(k+1)
c            tdc_val = IW16(k+2)
c            k=k+2
c         enddo
c      endif

c new tagt tdc

      if (name_tatl.ne.0) then
         ind_tatl = iw(name_tatl)
         if(ind_tatl.ne.0) then
            nTATL = iwrow(ind_tatl)
            k=2*ind_tatl
            do i=1,nTATL
               ID_tatl(i) = iw16(k+1)
               TDC_tatl(i) = iw16(k+2)
               k=k+2
c get CTOF TDCs
               if(ID_TATL(i) .eq. 257 .and. nCTOF1 .lt.16)then 
                  nCTOF1 = nCTOF1 + 1
                  TDC_CTOF1(nCTOF1) = TDC_tatl(i)
               endif
               if(ID_TATL(i) .eq. 258 .and. nCTOF2 .lt.16)then 
                  nCTOF2 = nCTOF2 + 1
                  TDC_CTOF2(nCTOF2) = TDC_tatl(i)
               endif

            enddo
         else
            nTATL = 0
         endif
      endif
      if (name_tatr.ne.0) then
         ind_tatr = iw(name_tatr)
         if(ind_tatr.ne.0) then
            nTATR = iwrow(ind_tatr)
            k=2*ind_tatr
            do i=1,nTATR
               ID_tatr(i) = iw16(k+1)
               TDC_tatr(i) = iw16(k+2)
               k=k+2
C get MOR TDC 
               if (ID_tatr(i) .eq. 62 .and. nMOR .lt. 256) then 
                  nMOR = nMOR + 1
                  TDC_MOR(nMOR) = TDC_tatr(i)
               endif   
            enddo
         else
            nTATR = 0
         endif
      endif

 
c read STN0 and STN1 (g11 start counter) banks
 
c get TDCs
      if (name_stn0.ne.0) then
         ind_stn0 = iw(name_stn0)
         if (ind_stn0.ne.0) then
            nSTN0    = iwrow(ind_stn0)
            k = 2*ind_stn0      ! k is pointed before data words for iw16 array
            do i = 1, nSTN0
               ID_stn0 (i) = IW16(k+1)
               TDC_stn0(i) = IW16(k+2)
               k     = k + 2
            enddo
         else
            nSTn0 = 0
         endif
      endif
      
c     get ADCs
      if (name_stn1.ne.0) then
         ind_stn1 = iw(name_stn1)
         if (ind_stn1.ne.0) then
            nSTN1    = iwrow(ind_stn1)
            k = 2*ind_stn1      ! k is pointed before data words for iw16 array
            do i = 1, nSTN1
               ID_stn1 (i) = IW16(k+1)
               ADC_stn1(i) = IW16(k+2)
               k     = k + 2
               if (ID_stn1 (i) .eq. 257) ADC_CTOF1 = ADC_stn1(i)
               if (ID_stn1 (i) .eq. 258) ADC_CTOF2 = ADC_stn1(i)
            enddo
         else
            nSTn1 = 0
         endif
      endif
      
      
c     read TAGT bank
      if (name_tagt.ne.0) then
         ind_tagt = iw(name_tagt)
         if (ind_tagt.ne.0) then
            nTAGT    = iwrow(ind_tagt)
            k = 2*ind_tagt      ! k is pointed before data words for iw16 array
            do i = 1, nTAGT
               ID_tagt (i) = IW16(k+1)
               TDCL_tagt(i) = IW16(k+2)
               TDCR_tagt(i) = IW16(k+3)
               k     = k + 3
            enddo
         else
            nTAGT = 0
         endif
      endif
c     
c PrimEx PS from TAGE bank. Parse info stored in ID_tage() and TDC_tage() above
c 
c PSF1-16  --  id= 1-16       PSF1-8=Beam Left   PSF9-16=Beam Right
c PSB1-16  --  id=17-32       PSB1-8=Beam Left   PSB9-16=Beam Right
c CUA1,2   --  id= 5, 6       Beam Left
c CUA3,4   --  id=11,12       Beam Right
c 
c First, initialize arrays stored in COMMON block
c 
      do id = 1, 4
         CUA_hits(id) = 0
         CUA_ADC(id)  = 0
         CUA_scaler(id) = 0
      enddo
      do id = 1, 16
         PSFT_hits(id) = 0
         PSF_ADC(id)   = 0
         PSBT_hits(id) = 0
         PSB_ADC(id)   = 0
         PS_logic_hits(id) = 0
         PSF_scaler(id) =0
         PSB_scaler(id) =0
      enddo

c     Loop over TAGE rows looking for ones with sd2=4 signifying PS
      do i = 1, nTAGE1
         if ((ID_tage(i)/256).eq.4) then
            
            id = mod(ID_tage(i),256)
            tdc_val = TDC_tage(i)
            TAGE1(id+70) = TAGE1(id+70) + 1 ! fill multiplicity for PS


c     Here, we look first for CUA paddles and put those hits in the
c     appropriate part of the COMMON block. This will exclude them
c     from the PrimEx PS arrays.
            if (id.eq.(5).or.id.eq.(6)) then
               id = id-1
               CUA_hits(id) = CUA_hits(id) + 1
               CUA_TDC(CUA_hits(id),id) = tdc_val
            else if (id.eq.(11).or.id.eq.(12)) then 
               id = id-8
               CUA_hits(id) = CUA_hits(id) + 1
               CUA_TDC(CUA_hits(id),id) = tdc_val
            else if ((id.ge.1).and.(id.le.16)) then 
               PSFT_hits(id) = PSFT_hits(id) + 1
               PSF_TDC(PSFT_hits(id),id) = tdc_val
            else if ((id.ge.17).and.(id.le.32)) then
               id = id-16
               PSBT_hits(id) = PSBT_hits(id) + 1
               PSB_TDC(PSBT_hits(id),id) = tdc_val
            else if ((id.ge.101).and.(id.le.116)) then
               id = id - 100
               PS_logic_hits(id) = PS_logic_hits(id) + 1
               PS_logic_TDC(PS_logic_hits(id),id) = tdc_val
            endif
         endif
      enddo
c     
c
c     PrimEx PS from PRIM bank (ADC info only)
      if (name_prim.ne.0) then
         ind_prim = iw(name_prim)
         if (ind_prim.ne.0) then
            nPRIM    = iwrow(ind_prim)
            k = 2*ind_prim      ! k is pointed before data words for iw16 array
            do i = 1, nPRIM
               id = IW16(k+1)
c     
c     First, fill for CUA paddles
c     
               if (id.ge.9.and.id.le.12) then
                  id = id - 8
                  CUA_ADC(id) = IW16(k+2)
c     then, for PrimEx paddles
               else if (id.le.16) then
                  PSF_ADC(id) = IW16(k+2)
               else
                  id = id - 16
                  PSB_ADC(id) = IW16(k+2)
               endif
               k = k + 2
            enddo
         else
            nPRIM = 0
         endif
      endif

c read ST (start counter) bank
      if (name_st.ne.0) then
         ind_st = iw(name_st)
         if (ind_st.ne.0) then
            nST    = iwrow(ind_st)
            k = 2*ind_st        ! k is pointed before data words for iw16 array
            do i = 1, nST
               ID_st (i) = IW16(k+1)
               TDC_st(i) = IW16(k+2)
               ADC_st(i) = IW16(k+3)
               k     = k +3
            enddo
         else
            nST = 0
         endif
      endif
      
      return
      end


c-----------------------------------------------------------







