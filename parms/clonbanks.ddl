!-----------------------------------------------------------------------
 TABLE  PTRN     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  PSYN     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC00     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC01     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC02     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC03     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC04     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC05     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC06     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC07     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC08     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC09     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC10     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC11     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC12     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC13     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC14     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC15     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC16     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC17     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC18     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC19     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC20     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC21     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC22     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC23     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC24     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC25     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC26     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC27     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC28     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC29     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC30     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RC31     B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  RWM      B32 ! create  ! Raw data bank
  1  WORD     I   0  2000000000  ! raw data
 END TABLE
!-----------------------------------------------------------------------
 TABLE  INFO     A   ! create  ! Raw data bank
  1  TEXT     A   0  2000000000  ! message
 END TABLE
!-----------------------------------------------------------------------
 TABLE  WARN     A   ! create  ! Raw data bank
  1  TEXT     A   0  2000000000  ! message
 END TABLE
!-----------------------------------------------------------------------
 TABLE  ERRO     A   ! create  ! Raw data bank
  1  TEXT     A   0  2000000000  ! message
 END TABLE
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CC       B16 ! create    ! Forward Cerenkov counters event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      36  ! the address of the hit detector element
  2  TDC      I     0  100000  ! tdc information (channels)
  3  ADC      I     0  100000  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CCT      B16 ! create    ! Forward Cerenkov counters event bank (TDC)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      36  ! the address of the hit detector element
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CC1      B16   ! create    ! Large angle Cerenkov counters event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      36  ! the address of the hit detector element
  2  TDC      I     0  100000  ! tdc information (channels)
  3  ADC      I     0  100000  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DC0       B16  ! create    ! Drift chamber event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    9408  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  EC       B16   !     ! Forward calorimeter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ECT      B16   !       ! Forward calorimeter event bank (TDCs only)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  MS1      B16   !     ! Microstrip detector event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     0    1572  ! layer number & strip number
  2  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  IC       B16   !     ! Forward calorimeter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  IC0       B16  ! create    ! Drift chamber event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    9408  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  EC1      B16   !     ! Large angle calorimeter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1064  ! the address of the hit detector element
  2  TDCL     I     0  100000  ! tdc information (channels)
  3  ADCL     I     0  100000  ! adc information (channels)
  4  TDCR     I     0  100000  ! tdc information (channels)
  5  ADCR     I     0  100000  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  EC1T     B16   !     ! Large angle calorimeter event bank (TDC)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    2128  ! the address of the hit detector element
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      !Comments
 TABLE   HEAD      ! create write display delete ! Bank HEAD
!
!COL ATT-name FMT Min    Max   !Comments
   1 VERSION  I    0      2    ! Version Number
   2 NRUN     I    1      1000 ! Run Number (monotonically increasing)
   3 NEVENT   I    1      1000 ! Event Number (starting with 1 at run begin,
                               !   counting separately for physics and scalers events)
   4 TIME     I  100 100000000 ! Event Time (UNIX time = seconds as of January 1,1970)
   5 TYPE     I    0      1000 ! Event Type (Defined by on-line system or MC run:
                               !            = 1 - 9    - physics events:
                               !            =   1          - regular event
                               !            =   2          - physics sync
                               !            =   4          - level2 late fail
                               !            = 10       - scaler events
                               !           >= 100      - database record events
                               !            = 0        - control events
                               !            < 0        - Monte Carlo Events:
                               !            =   -1          - SDA
                               !            =   -2          - GEANT
                               !            =   -3          - ClasSim
   6 ROC      I    0  20000000 !            = 0        - sync status is OK
                               !            > 0        - bit pattern of offending ROC's
   7 EVTCLASS I    1      20   ! Event Classification from DAQ:
                               !        0 Special Events (scaler, parameter, database, etc.)
                               !     1-15 Physics Events
                               !       16 Sync Event
                               !       17 Prestart Event
                               !       18 Go Event
                               !       19 Pause Event
                               !       20 End Event
   8 TRIGBITS I    0   9999999 ! Level 1 Trigger Latch Word (16 bits)
                               !    (see LATCH1 in tgbi.ddl)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype                ! Comments
 TABLE  TGBI   B32  ! create write display delete ! Trigger bank
!
!   ATTributes:
!   -----------
!COL ATT-name 	    FMT  Min     Max     ! Comments
!     	      	    
  1 latch1          I      0    9999999  ! level1 trigger latch word (16 bits)
                       			 !   bits    descr
                       			 !   ----    -----
                       			 !    1-12   Level 1 input bits to trigger supervisor
                       			 !     13    not used
                       			 !     14    not used
                       			 !     15    Helicity clock
                       			 !     16    Helicity state
  2 helicity_scaler I      0    9999999  ! helicity interval count
  3 interrupt_time  I      0    9999999  ! interrupt time from microsec clock
  4 latch2          I      0    9999999  ! level2 trigger latch word (16 bits)
                                         !   bits    descr
                                         !   ----    -----
                                         !    1-6    level 2 sector bits
                                         !    7-12   not used
                                         !     13    level 2 fail
                                         !     14    level 2 pass
                                         !     15    level 2 fail
                                         !     16    level 2 pass
  5 level3          I      0    9999999  ! level3 trigger word (32 bits)
                                         !   bits    descr
                                         !   ----    -----
                                         !    1-6    level 3 sector bits
                                         !    7-24   not used
                                         !    25-32  version number
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype                ! Comments
 TABLE  +TRG   B32  ! create write display delete ! copy of TGBI bank
!
!   ATTributes:
!   -----------
!COL ATT-name 	    FMT  Min     Max     ! Comments
!     	      	    
  1 latch1          I      0    9999999  ! level1 trigger latch word (16 bits)
                       			 !   bits    descr
                       			 !   ----    -----
                       			 !    1-12   Level 1 input bits to trigger supervisor
                       			 !     13    not used
                       			 !     14    not used
                       			 !     15    Helicity clock
                       			 !     16    Helicity state
  2 helicity_scaler I      0    9999999  ! helicity interval count
  3 interrupt_time  I      0    9999999  ! interrupt time from microsec clock
  4 latch2          I      0    9999999  ! level2 trigger latch word (16 bits)
                                         !   bits    descr
                                         !   ----    -----
                                         !    1-6    level 2 sector bits
                                         !    7-12   not used
                                         !     13    level 2 fail
                                         !     14    level 2 pass
                                         !     15    level 2 fail
                                         !     16    level 2 pass
  5 level3          I      0    9999999  ! level3 trigger word (32 bits)
                                         !   bits    descr
                                         !   ----    -----
                                         !    1-6    level 3 sector bits
                                         !    7-32   not used
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SC       B16   ! create    ! Scintillation counter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      48  ! the address of the hit detector element
  2  TDCL     I     0   65536  ! tdc information (channels)
  3  ADCL     I     0   65536  ! adc information (channels)
  4  TDCR     I     0   65536  ! tdc information (channels)
  5  ADCR     I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SCT      B16   ! create    ! Scintillation counter event bank (TDC)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      96  ! the address of the hit detector element
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  MPTA     B16   ! create    ! Moller tdc-adc bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      48  ! the address of the hit detector element
  2  TDCL     I     0   65536  ! tdc information (channels)
  3  ADCL     I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  MPS      B32   ! create    ! Moller scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! 
  2  S2       I     1  9000000 ! 
  3  S3       I     1  9000000 ! 
  4  S4       I     1  9000000 ! 
  5  S5       I     1  9000000 ! 
  6  S6       I     1  9000000 ! 
  7  S7       I     1  9000000 ! 
  8  S8       I     1  9000000 ! 
  9  S9       I     1  9000000 ! 
  10 S10      I     1  9000000 ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  MPS+     B32   ! create    ! Moller scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! 
  2  S2       I     1  9000000 ! 
  3  S3       I     1  9000000 ! 
  4  S4       I     1  9000000 ! 
  5  S5       I     1  9000000 ! 
  6  S6       I     1  9000000 ! 
  7  S7       I     1  9000000 ! 
  8  S8       I     1  9000000 ! 
  9  S9       I     1  9000000 ! 
 10  S10      I     1  9000000 ! 
 11  U1       I     1  9000000 ! 
 12  U2       I     1  9000000 ! 
 13  U3       I     1  9000000 ! 
 14  U4       I     1  9000000 ! 
 15  U5       I     1  9000000 ! 
 16  U6       I     1  9000000 ! 
 17  U7       I     1  9000000 ! 
 18  U8       I     1  9000000 ! 
 19  U9       I     1  9000000 ! 
 20  U10      I     1  9000000 ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  FBPM     B16   ! create    ! Fast BPM bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     0       7  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  HLS      B32   ! create    ! Helicity scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! 10MHz clock
  2  S2       I     1  9000000 ! OTR-1
  3  S3       I     1  9000000 ! OTR-2
  4  S4       I     1  9000000 ! SLM
  5  S5       I     1  9000000 ! lvl-1 trigger rate
  6  S6       I     1  9000000 ! L.R Moller coincidences
  7  S7       I     1  9000000 ! L.R Moller accidentals
  8  S8       I     1  9000000 ! F-CUP
  9  S9       I     1  9000000 ! pmt-1
  10 S10      I     1  9000000 ! pmt-2
  11 S11      I     1  9000000 ! pmt-3
  12 S12      I     1  9000000 ! pmt-4
  13 S13      I     1  9000000 ! reserve
  14 S14      I     1  9000000 ! reserve
  15 S15      I     1  9000000 ! Helicity states accumulating counter
  16 S16      I     1  9000000 ! HLS banks accumulating counter
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  HLS+     B32   ! create    ! Helicity scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! 
  2  S2       I     1  9000000 ! 
  3  S3       I     1  9000000 ! 
  4  S4       I     1  9000000 ! 
  5  S5       I     1  9000000 ! 
  6  S6       I     1  9000000 ! 
  7  S7       I     1  9000000 ! 
  8  S8       I     1  9000000 ! 
  9  S9       I     1  9000000 ! 
 10  S10      I     1  9000000 ! 
 11  S11      I     1  9000000 ! 
 12  S12      I     1  9000000 ! 
 13  S13      I     1  9000000 ! 
 14  S14      I     1  9000000 ! 
 15  S15      I     1  9000000 ! 
 16  S16      I     1  9000000 ! 
 17  U1       I     1  9000000 ! 
 18  U2       I     1  9000000 ! 
 19  U3       I     1  9000000 ! 
 20  U4       I     1  9000000 ! 
 21  U5       I     1  9000000 ! 
 22  U6       I     1  9000000 ! 
 23  U7       I     1  9000000 ! 
 24  U8       I     1  9000000 ! 
 25  U9       I     1  9000000 ! 
 26  U10      I     1  9000000 ! 
 27  U11      I     1  9000000 ! 
 28  U12      I     1  9000000 ! 
 29  U13      I     1  9000000 ! 
 30  U14      I     1  9000000 ! 
 31  U15      I     1  9000000 ! 
 32  U16      I     1  9000000 ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  L2H       B16  ! create    ! Level 2 hit bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    9408  ! superlayer + 256 * hit#
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!
!       BANKname BANKtype      ! Comments
 TABLE  L2S       B16  ! create    ! Level 2 report bank
!
! id = 1 - TL1 - reported TDC for Level 1
! id = 2 - L2S1 - reported 2 segments out of 4 from sector 1
! id = 3 - L2S1 - reported 2 segments out of 4 from sector 2
! id = 4 - L2S1 - reported 2 segments out of 4 from sector 3
! id = 5 - L2S1 - reported 2 segments out of 4 from sector 4
! id = 6 - L2S1 - reported 2 segments out of 4 from sector 5
! id = 7 - L2S1 - reported 2 segments out of 4 from sector 6
! id = 8 - L2OR - Level 2 final report (OR from all sectors)
! id = 9 - L2NOR - Level 2 fault
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    9408  ! id + 256 * hit#
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SYNC     B16   ! create    ! Physics sync event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     0      25  ! slot number starting from 0
  2  MISSING  I     0   65536  ! count of missing responses
  3  EXTRA    I     0   65536  ! count of extra buffers
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  PRIM      B16  ! create    ! extra ADC at CC crate at slot #0
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      64  ! the address of the hit detector element
  2  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DSTC       B16  ! create write display delete   ! Down Stream Total Absorption Shower Counter event bank
!		
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  TACID    I     0    1     ! ID compelled by DAQ (always 1)
  2  TDCTAC   I     0    4096  ! tdc information (on sum)
  3  ADCLT    I     0   65536  ! adc information (left top)
  4  ADCRT    I     0   65536  ! adc information (right top)
  5  ADCLB    I     0   65536  ! adc information (left bottom)
  6  ADCRB    I     0   65536  ! adc information (right bottom)
  7  ADCSUM1  I     0   65536  ! adc information (sum scale1)
  8  ADCSUM2  I     0   65536  ! adc information (sum scale2)
  9  ADCSUM3  I     0   65536  ! adc information (sum scale3)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DSPC       B16  ! create write display delete ! Down Stream Pair Counter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  PCID     I     0       1  ! Id compelled by DAQ (always 1)
  2  TDCPC    I     0    4096  ! tdc information ( scintillator)
  3  ADCMN    I     0   65536  ! adc information (main)
  4  ADCLT    I     0   65536  ! adc information (left top)
  5  ADCRB    I     0   65536  ! adc information (right bottom)
  6  ADCLB    I     0   65536  ! adc information (left bottom)
  7  ADCRT    I     0   65536  ! adc information (right top)
  8  ADCVE    I     0   65536  ! adc information (veto)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DSPS       B16  ! create write display delete ! Down Stream Pair Spectrometer event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1    8     ! paddle ID (left = 1 to 4)( right = 5 to 8)
  2  TDC      I    0    4096  ! tdc information
  3  ADC      I    0   65536  ! adc information 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TAGT       B16  ! create write display delete ! Tagger T countert event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     62    ! the address of the hit detector element
  2  TDCL     I    0    4096   ! tdc information (Left counters channels)
  3  TDCR     I    0    4096   ! tdc information (Right counters channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TGTL       B16  ! create write display delete ! Tagger T countert event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     63    ! the address of the hit detector element
  2  TDC      I    0   65535   ! tdc information
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TGTR       B16  ! create write display delete ! Tagger T countert event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     63    ! the address of the hit detector element
  2  TDC      I    0   65535   ! tdc information
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TATL       B32  ! create write display delete ! Tagger T countert event bank (long words)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     63    ! the address of the hit detector element
  2  TDC      I    0 1048575   ! tdc information
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TATR       B32  ! create write display delete ! Tagger T countert event bank (long words)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     63    ! the address of the hit detector element
  2  TDC      I    0 1048575   ! tdc information
!
 END TABLE
!
!-----------------------------------------------------------------------

!       BANKname BANKtype      ! Comments
 TABLE  TRGT       B16  ! create write display delete ! Trigger bits timing
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1      64   ! the trigger bit number
  2  TDC      I    0    4096   ! tdc information: start from trigger, stop from bit
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TAGE       B16  ! create write display delete ! Tagger E-counter data event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I    1     384   ! the address of the hit detector element
  2  TDC      I    0    4096   ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  +SYN       B16  ! create write display delete ! Sync bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ROCID    I    1     31    ! ROC number
  2  RES1     I    0    4096   ! 0
  3  RES2     I    0    4096   ! 0
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TLV1       B16  ! create write display delete ! Level1 trigger bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    3000  ! id = Detector * 256 + channel
                               ! Detector = 1 for the CC   (16 input channels)
                               ! Detector = 2 for the TOF  (32 input channels doubles and triples)
                               ! Detector = 3 for the EC   (16 input channels)
                               ! Detector = 4 for the LAC  (16 input channels)
							   ! Detector = 5 for TS inputs
							   ! channel 1 Async 1
                               ! channel 2 Async 2
                               ! channel 3 MOR 1
                               ! channel 4 Start Counter 1
                               ! channels 5 -- 16 not used
                               ! channel 17 Prescale 1
                               ! channel 18 Prescale 2
                               ! channel 19 Prescale 3
                               ! channel 20 Prescale 4
                               ! channel 21 Prescale 5
                               ! channel 22 Prescale 6
                               ! channel 23 Prescale 7
                               ! channel 24 Prescale 8
                               ! channel 25 Prescale 9
                               ! channel 26 Prescale 10
                               ! channel 27 Prescale 11
                               ! channel 28 Prescale 12
                               ! channel 29 Start Counter 2
                               ! channel 30 MOR 2
                               ! channel 31 Helicity bit
                               ! channel 32 Helicity bit
   2  time     I     0   65536 ! time(ns) 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TL1        B16  ! create write display delete ! Level1 trigger bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
   1  ID       I   257    3000 ! id = Detector * 256 + channel
   2  time     I     0   65536 ! time(ns) 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TL2        B16  ! create write display delete ! Level1 trigger bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
   1  ID       I   257    3000 ! id = Detector * 256 + channel
   2  time     I     0   65536 ! time(ns) 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ECS     B32  ! create write display delete ! Calorimeter scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   ECS1      I     0    9999999  ! scaler sector 1
  2   ECS2      I     0    9999999  ! scaler sector 1
  3   ECS3      I     0    9999999  ! scaler sector 1
  4   ECS4      I     0    9999999  ! scaler sector 1
  5   ECS5      I     0    9999999  ! scaler sector 1
  6   ECS6      I     0    9999999  ! scaler sector 1
  7   ECS7      I     0    9999999  ! scaler sector 1
  8   ECS8      I     0    9999999  ! scaler sector 1
  9   ECS9      I     0    9999999  ! scaler sector 1
  10  ECS10     I     0    9999999  ! scaler sector 1
  11  ECS11     I     0    9999999  ! scaler sector 1
  12  ECS12     I     0    9999999  ! scaler sector 1
  13  ECS13     I     0    9999999  ! scaler sector 1
  14  ECS14     I     0    9999999  ! scaler sector 1
  15  ECS15     I     0    9999999  ! scaler sector 1
  16  ECS16     I     0    9999999  ! scaler sector 1
  17  ECS17     I     0    9999999  ! scaler sector 2
  18  ECS18     I     0    9999999  ! scaler sector 2
  19  ECS19     I     0    9999999  ! scaler sector 2
  20  ECS20     I     0    9999999  ! scaler sector 2
  21  ECS21     I     0    9999999  ! scaler sector 2
  22  ECS22     I     0    9999999  ! scaler sector 2
  23  ECS23     I     0    9999999  ! scaler sector 2
  24  ECS24     I     0    9999999  ! scaler sector 2
  25  ECS25     I     0    9999999  ! scaler sector 2
  26  ECS26     I     0    9999999  ! scaler sector 2
  27  ECS27     I     0    9999999  ! scaler sector 2
  28  ECS28     I     0    9999999  ! scaler sector 2
  29  ECS29     I     0    9999999  ! scaler sector 2
  30  ECS30     I     0    9999999  ! scaler sector 2
  31  ECS31     I     0    9999999  ! scaler sector 2
  32  ECS32     I     0    9999999  ! scaler sector 2
  33  ECS33     I     0    9999999  ! scaler sector 3
  34  ECS34     I     0    9999999  ! scaler sector 3
  35  ECS35     I     0    9999999  ! scaler sector 3
  36  ECS36     I     0    9999999  ! scaler sector 3
  37  ECS37     I     0    9999999  ! scaler sector 3
  38  ECS38     I     0    9999999  ! scaler sector 3
  39  ECS39     I     0    9999999  ! scaler sector 3
  40  ECS40     I     0    9999999  ! scaler sector 3
  41  ECS41     I     0    9999999  ! scaler sector 3
  42  ECS42     I     0    9999999  ! scaler sector 3
  43  ECS43     I     0    9999999  ! scaler sector 3
  44  ECS44     I     0    9999999  ! scaler sector 3
  45  ECS45     I     0    9999999  ! scaler sector 3
  46  ECS46     I     0    9999999  ! scaler sector 3
  47  ECS47     I     0    9999999  ! scaler sector 3
  48  ECS48     I     0    9999999  ! scaler sector 3
  49  ECS49     I     0    9999999  ! scaler sector 4
  50  ECS50     I     0    9999999  ! scaler sector 4
  51  ECS51     I     0    9999999  ! scaler sector 4
  52  ECS52     I     0    9999999  ! scaler sector 4
  53  ECS53     I     0    9999999  ! scaler sector 4
  54  ECS54     I     0    9999999  ! scaler sector 4
  55  ECS55     I     0    9999999  ! scaler sector 4
  56  ECS56     I     0    9999999  ! scaler sector 4
  57  ECS57     I     0    9999999  ! scaler sector 4
  58  ECS58     I     0    9999999  ! scaler sector 4
  59  ECS59     I     0    9999999  ! scaler sector 4
  60  ECS60     I     0    9999999  ! scaler sector 4
  61  ECS61     I     0    9999999  ! scaler sector 4
  62  ECS62     I     0    9999999  ! scaler sector 4
  63  ECS63     I     0    9999999  ! scaler sector 4
  64  ECS64     I     0    9999999  ! scaler sector 4
  65  ECS65     I     0    9999999  ! scaler sector 5
  66  ECS66     I     0    9999999  ! scaler sector 5
  67  ECS67     I     0    9999999  ! scaler sector 5
  68  ECS68     I     0    9999999  ! scaler sector 5
  69  ECS69     I     0    9999999  ! scaler sector 5
  70  ECS70     I     0    9999999  ! scaler sector 5
  71  ECS71     I     0    9999999  ! scaler sector 5
  72  ECS72     I     0    9999999  ! scaler sector 5
  73  ECS73     I     0    9999999  ! scaler sector 5
  74  ECS74     I     0    9999999  ! scaler sector 5
  75  ECS75     I     0    9999999  ! scaler sector 5
  76  ECS76     I     0    9999999  ! scaler sector 5
  77  ECS77     I     0    9999999  ! scaler sector 5
  78  ECS78     I     0    9999999  ! scaler sector 5
  79  ECS79     I     0    9999999  ! scaler sector 5
  80  ECS80     I     0    9999999  ! scaler sector 5
  81  ECS81     I     0    9999999  ! scaler sector 6
  82  ECS82     I     0    9999999  ! scaler sector 6
  83  ECS83     I     0    9999999  ! scaler sector 6
  84  ECS84     I     0    9999999  ! scaler sector 6
  85  ECS85     I     0    9999999  ! scaler sector 6
  86  ECS86     I     0    9999999  ! scaler sector 6
  87  ECS87     I     0    9999999  ! scaler sector 6
  88  ECS88     I     0    9999999  ! scaler sector 6
  89  ECS89     I     0    9999999  ! scaler sector 6
  90  ECS90     I     0    9999999  ! scaler sector 6
  91  ECS91     I     0    9999999  ! scaler sector 6
  92  ECS92     I     0    9999999  ! scaler sector 6
  93  ECS93     I     0    9999999  ! scaler sector 6
  94  ECS94     I     0    9999999  ! scaler sector 6
  95  ECS95     I     0    9999999  ! scaler sector 6
  96  ECS96     I     0    9999999  ! scaler sector 6
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ICS     B32  ! create write display delete ! Inner Calorimeter scalers
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   ICS1      I     0    9999999  ! scaler
  2   ICS2      I     0    9999999  ! scaler
  3   ICS3      I     0    9999999  ! scaler
  4   ICS4      I     0    9999999  ! scaler
  5   ICS5      I     0    9999999  ! scaler
  6   ICS6      I     0    9999999  ! scaler
  7   ICS7      I     0    9999999  ! scaler
  8   ICS8      I     0    9999999  ! scaler
  9   ICS9      I     0    9999999  ! scaler
  10  ICS10     I     0    9999999  ! scaler
  11  ICS11     I     0    9999999  ! scaler
  12  ICS12     I     0    9999999  ! scaler
  13  ICS13     I     0    9999999  ! scaler
  14  ICS14     I     0    9999999  ! scaler
  15  ICS15     I     0    9999999  ! scaler
  16  ICS16     I     0    9999999  ! scaler
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ICT     B32  ! create write display delete ! Inner Calorimeter temps
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   ICT1     I     0    9999999  ! termoprobe readout
  2   ICT2     I     0    9999999  ! termoprobe readout
  3   ICT3     I     0    9999999  ! termoprobe readout
  4   ICT4     I     0    9999999  ! termoprobe readout
  5   ICT5     I     0    9999999  ! termoprobe readout
  6   ICT6     I     0    9999999  ! termoprobe readout
  7   ICT7     I     0    9999999  ! termoprobe readout
  8   ICT8     I     0    9999999  ! termoprobe readout
  9   ICT9     I     0    9999999  ! termoprobe readout
  10  ICT10    I     0    9999999  ! termoprobe readout
  11  ICT11    I     0    9999999  ! termoprobe readout
  12  ICT12    I     0    9999999  ! termoprobe readout
  13  ICT13    I     0    9999999  ! termoprobe readout
  14  ICT14    I     0    9999999  ! termoprobe readout
  15  ICT15    I     0    9999999  ! termoprobe readout
  16  ICT16    I     0    9999999  ! termoprobe readout
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SCS     B32   ! create write display delete ! Time of Flight scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   SCS1      I     0    9999999  ! scaler sector 1
  2   SCS2      I     0    9999999  ! scaler sector 1
  3   SCS3      I     0    9999999  ! scaler sector 1
  4   SCS4      I     0    9999999  ! scaler sector 1
  5   SCS5      I     0    9999999  ! scaler sector 1
  6   SCS6      I     0    9999999  ! scaler sector 1
  7   SCS7      I     0    9999999  ! scaler sector 1
  8   SCS8      I     0    9999999  ! scaler sector 1
  9   SCS9      I     0    9999999  ! scaler sector 1
  10  SCS10     I     0    9999999  ! scaler sector 1
  11  SCS11     I     0    9999999  ! scaler sector 1
  12  SCS12     I     0    9999999  ! scaler sector 1
  13  SCS13     I     0    9999999  ! scaler sector 1
  14  SCS14     I     0    9999999  ! scaler sector 1
  15  SCS15     I     0    9999999  ! scaler sector 1
  16  SCS16     I     0    9999999  ! scaler sector 1
  17  SCS17     I     0    9999999  ! scaler sector 1
  18  SCS18     I     0    9999999  ! scaler sector 1
  19  SCS19     I     0    9999999  ! scaler sector 1
  20  SCS20     I     0    9999999  ! scaler sector 1
  21  SCS21     I     0    9999999  ! scaler sector 1
  22  SCS22     I     0    9999999  ! scaler sector 1
  23  SCS23     I     0    9999999  ! scaler sector 1
  24  SCS24     I     0    9999999  ! scaler sector 1
  25  SCS25     I     0    9999999  ! scaler sector 1
  26  SCS26     I     0    9999999  ! scaler sector 1
  27  SCS27     I     0    9999999  ! scaler sector 1
  28  SCS28     I     0    9999999  ! scaler sector 1
  29  SCS29     I     0    9999999  ! scaler sector 1
  30  SCS30     I     0    9999999  ! scaler sector 1
  31  SCS31     I     0    9999999  ! scaler sector 1
  32  SCS32     I     0    9999999  ! scaler sector 1
  33  SCS33     I     0    9999999  ! scaler sector 2
  34  SCS34     I     0    9999999  ! scaler sector 2
  35  SCS35     I     0    9999999  ! scaler sector 2
  36  SCS36     I     0    9999999  ! scaler sector 2
  37  SCS37     I     0    9999999  ! scaler sector 2
  38  SCS38     I     0    9999999  ! scaler sector 2
  39  SCS39     I     0    9999999  ! scaler sector 2
  40  SCS40     I     0    9999999  ! scaler sector 2
  41  SCS41     I     0    9999999  ! scaler sector 2
  42  SCS42     I     0    9999999  ! scaler sector 2
  43  SCS43     I     0    9999999  ! scaler sector 2
  44  SCS44     I     0    9999999  ! scaler sector 2
  45  SCS45     I     0    9999999  ! scaler sector 2
  46  SCS46     I     0    9999999  ! scaler sector 2
  47  SCS47     I     0    9999999  ! scaler sector 2
  48  SCS48     I     0    9999999  ! scaler sector 2
  49  SCS49     I     0    9999999  ! scaler sector 2
  50  SCS50     I     0    9999999  ! scaler sector 2
  51  SCS51     I     0    9999999  ! scaler sector 2
  52  SCS52     I     0    9999999  ! scaler sector 2
  53  SCS53     I     0    9999999  ! scaler sector 2
  54  SCS54     I     0    9999999  ! scaler sector 2
  55  SCS55     I     0    9999999  ! scaler sector 2
  56  SCS56     I     0    9999999  ! scaler sector 2
  57  SCS57     I     0    9999999  ! scaler sector 2
  58  SCS58     I     0    9999999  ! scaler sector 2
  59  SCS59     I     0    9999999  ! scaler sector 2
  60  SCS60     I     0    9999999  ! scaler sector 2
  61  SCS61     I     0    9999999  ! scaler sector 2
  62  SCS62     I     0    9999999  ! scaler sector 2
  63  SCS63     I     0    9999999  ! scaler sector 2
  64  SCS64     I     0    9999999  ! scaler sector 2
  65  SCS65     I     0    9999999  ! scaler sector 3
  66  SCS66     I     0    9999999  ! scaler sector 3
  67  SCS67     I     0    9999999  ! scaler sector 3
  68  SCS68     I     0    9999999  ! scaler sector 3
  69  SCS69     I     0    9999999  ! scaler sector 3
  70  SCS70     I     0    9999999  ! scaler sector 3
  71  SCS71     I     0    9999999  ! scaler sector 3
  72  SCS72     I     0    9999999  ! scaler sector 3
  73  SCS73     I     0    9999999  ! scaler sector 3
  74  SCS74     I     0    9999999  ! scaler sector 3
  75  SCS75     I     0    9999999  ! scaler sector 3
  76  SCS76     I     0    9999999  ! scaler sector 3
  77  SCS77     I     0    9999999  ! scaler sector 3
  78  SCS78     I     0    9999999  ! scaler sector 3
  79  SCS79     I     0    9999999  ! scaler sector 3
  80  SCS80     I     0    9999999  ! scaler sector 3
  81  SCS81     I     0    9999999  ! scaler sector 3
  82  SCS82     I     0    9999999  ! scaler sector 3
  83  SCS83     I     0    9999999  ! scaler sector 3
  84  SCS84     I     0    9999999  ! scaler sector 3
  85  SCS85     I     0    9999999  ! scaler sector 3
  86  SCS86     I     0    9999999  ! scaler sector 3
  87  SCS87     I     0    9999999  ! scaler sector 3
  88  SCS88     I     0    9999999  ! scaler sector 3
  89  SCS89     I     0    9999999  ! scaler sector 3
  90  SCS90     I     0    9999999  ! scaler sector 3
  91  SCS91     I     0    9999999  ! scaler sector 3
  92  SCS92     I     0    9999999  ! scaler sector 3
  93  SCS93     I     0    9999999  ! scaler sector 3
  94  SCS94     I     0    9999999  ! scaler sector 3
  95  SCS95     I     0    9999999  ! scaler sector 3
  96  SCS96     I     0    9999999  ! scaler sector 3
  97  SCS97     I     0    9999999  ! scaler sector 4
  98  SCS98     I     0    9999999  ! scaler sector 4
  99  SCS99     I     0    9999999  ! scaler sector 4
  100  SCS100     I     0    9999999  ! scaler sector 4
  101   SCS101      I     0    9999999  ! scaler sector 4
  102   SCS102      I     0    9999999  ! scaler sector 4
  103   SCS103      I     0    9999999  ! scaler sector 4
  104   SCS104      I     0    9999999  ! scaler sector 4
  105   SCS105      I     0    9999999  ! scaler sector 4
  106   SCS106      I     0    9999999  ! scaler sector 4
  107   SCS107      I     0    9999999  ! scaler sector 4
  108   SCS108      I     0    9999999  ! scaler sector 4
  109   SCS109      I     0    9999999  ! scaler sector 4
  110  SCS110     I     0    9999999  ! scaler sector 4
  111  SCS111     I     0    9999999  ! scaler sector 4
  112  SCS112     I     0    9999999  ! scaler sector 4
  113  SCS113     I     0    9999999  ! scaler sector 4
  114  SCS114     I     0    9999999  ! scaler sector 4
  115  SCS115     I     0    9999999  ! scaler sector 4
  116  SCS116     I     0    9999999  ! scaler sector 4
  117  SCS117     I     0    9999999  ! scaler sector 4
  118  SCS118     I     0    9999999  ! scaler sector 4
  119  SCS119     I     0    9999999  ! scaler sector 4
  120  SCS120     I     0    9999999  ! scaler sector 4
  121  SCS121     I     0    9999999  ! scaler sector 4
  122  SCS122     I     0    9999999  ! scaler sector 4
  123  SCS123     I     0    9999999  ! scaler sector 4
  124  SCS124     I     0    9999999  ! scaler sector 4
  125  SCS125     I     0    9999999  ! scaler sector 4
  126  SCS126     I     0    9999999  ! scaler sector 4
  127  SCS127     I     0    9999999  ! scaler sector 4
  128  SCS128     I     0    9999999  ! scaler sector 4
  129  SCS129     I     0    9999999  ! scaler sector 5
  130  SCS130     I     0    9999999  ! scaler sector 5
  131  SCS131     I     0    9999999  ! scaler sector 5
  132  SCS132     I     0    9999999  ! scaler sector 5
  133  SCS133     I     0    9999999  ! scaler sector 5
  134  SCS134     I     0    9999999  ! scaler sector 5
  135  SCS135     I     0    9999999  ! scaler sector 5
  136  SCS136     I     0    9999999  ! scaler sector 5
  137  SCS137     I     0    9999999  ! scaler sector 5
  138  SCS138     I     0    9999999  ! scaler sector 5
  139  SCS139     I     0    9999999  ! scaler sector 5
  140  SCS140     I     0    9999999  ! scaler sector 5
  141  SCS141     I     0    9999999  ! scaler sector 5
  142  SCS142     I     0    9999999  ! scaler sector 5
  143  SCS143     I     0    9999999  ! scaler sector 5
  144  SCS144     I     0    9999999  ! scaler sector 5
  145  SCS145     I     0    9999999  ! scaler sector 5
  146  SCS146     I     0    9999999  ! scaler sector 5
  147  SCS147     I     0    9999999  ! scaler sector 5
  148  SCS148     I     0    9999999  ! scaler sector 5
  149  SCS149     I     0    9999999  ! scaler sector 5
  150  SCS150     I     0    9999999  ! scaler sector 5
  151  SCS151     I     0    9999999  ! scaler sector 5
  152  SCS152     I     0    9999999  ! scaler sector 5
  153  SCS153     I     0    9999999  ! scaler sector 5
  154  SCS154     I     0    9999999  ! scaler sector 5
  155  SCS155     I     0    9999999  ! scaler sector 5
  156  SCS156     I     0    9999999  ! scaler sector 5
  157  SCS157     I     0    9999999  ! scaler sector 5
  158  SCS158     I     0    9999999  ! scaler sector 5
  159  SCS159     I     0    9999999  ! scaler sector 5
  160  SCS160     I     0    9999999  ! scaler sector 5
  161  SCS161     I     0    9999999  ! scaler sector 6
  162  SCS162     I     0    9999999  ! scaler sector 6
  163  SCS163     I     0    9999999  ! scaler sector 6
  164  SCS164     I     0    9999999  ! scaler sector 6
  165  SCS165     I     0    9999999  ! scaler sector 6
  166  SCS166     I     0    9999999  ! scaler sector 6
  167  SCS167     I     0    9999999  ! scaler sector 6
  168  SCS168     I     0    9999999  ! scaler sector 6
  169  SCS169     I     0    9999999  ! scaler sector 6
  170  SCS170     I     0    9999999  ! scaler sector 6
  171  SCS171     I     0    9999999  ! scaler sector 6
  172  SCS172     I     0    9999999  ! scaler sector 6
  173  SCS173     I     0    9999999  ! scaler sector 6
  174  SCS174     I     0    9999999  ! scaler sector 6
  175  SCS175     I     0    9999999  ! scaler sector 6
  176  SCS176     I     0    9999999  ! scaler sector 6
  177  SCS177     I     0    9999999  ! scaler sector 6
  178  SCS178     I     0    9999999  ! scaler sector 6
  179  SCS179     I     0    9999999  ! scaler sector 6
  180  SCS180     I     0    9999999  ! scaler sector 6
  181  SCS181     I     0    9999999  ! scaler sector 6
  182  SCS182     I     0    9999999  ! scaler sector 6
  183  SCS183     I     0    9999999  ! scaler sector 6
  184  SCS184     I     0    9999999  ! scaler sector 6
  185  SCS185     I     0    9999999  ! scaler sector 6
  186  SCS186     I     0    9999999  ! scaler sector 6
  187  SCS187     I     0    9999999  ! scaler sector 6
  188  SCS188     I     0    9999999  ! scaler sector 6
  189  SCS189     I     0    9999999  ! scaler sector 6
  190  SCS190     I     0    9999999  ! scaler sector 6
  191  SCS191     I     0    9999999  ! scaler sector 6
  192  SCS192     I     0    9999999  ! scaler sector 6
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  L2H       B16  ! create write display delete ! Level 2 hit bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    9408  ! Superlayer + 256*(hit# in TDC)
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  L2S       B16  ! create write display delete ! Level 2 report bank
!
! id = 1 - TL1 - reported TDC for Level 1
! id = 2 - L2S1 - reported 2 segments out of 4 from sector 1
! id = 3 - L2S2 - reported 2 segments out of 4 from sector 2
! id = 4 - L2S3 - reported 2 segments out of 4 from sector 3
! id = 5 - L2S4 - reported 2 segments out of 4 from sector 4
! id = 6 - L2S5 - reported 2 segments out of 4 from sector 5
! id = 7 - L2S6 - reported 2 segments out of 4 from sector 6
! id = 8 - L2OR - Level 2 final report (OR from all sectors)
! id = 9 - L2NOR - Level 2 fault
!   Attributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID      I   256    9408  ! ID+256*hit#
  2  TDC     I     0   65536  ! TDC value
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DCST     B32   ! create    ! Drift chamber status bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! 
  2  S2       I     1  9000000 ! 
  3  S3       I     1  9000000 ! 
  4  S4       I     1  9000000 ! 
  5  S5       I     1  9000000 ! 
  6  S6       I     1  9000000 ! 
  7  S7       I     1  9000000 ! 
  8  S8       I     1  9000000 ! 
  9  S9       I     1  9000000 ! 
  10 S10      I     1  9000000 ! 
  11 S11      I     1  9000000 ! 
  12 S12      I     1  9000000 ! 
  13 S13      I     1  9000000 ! 
  14 S14      I     1  9000000 ! 
  15 S15      I     1  9000000 ! 
  16 S16      I     1  9000000 ! 
  17 S17      I     1  9000000 ! 
  18 S18      I     1  9000000 ! 
  19 S19      I     1  9000000 ! 
  20 S20      I     1  9000000 ! 
  21 S21      I     1  9000000 ! 
  22 S22      I     1  9000000 ! 
  23 S23      I     1  9000000 ! 
  24 S24      I     1  9000000 ! 
  25 S25      I     1  9000000 ! 
  26 S26      I     1  9000000 ! 
  27 S27      I     1  9000000 ! 
  28 S28      I     1  9000000 ! 
  29 S29      I     1  9000000 ! 
  30 S30      I     1  9000000 ! 
  31 S31      I     1  9000000 ! 
  32 S32      I     1  9000000 ! 
  33 S33      I     1  9000000 ! 
  34 S34      I     1  9000000 ! 
  35 S35      I     1  9000000 ! 
  36 S36      I     1  9000000 ! 
  37 S37      I     1  9000000 ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TRGS    ! create write display delete! Scaler bank 
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1  CLOCK_UG   I     0    9999999  ! Ungated Clock (10 KHz)
  2  FCUP_UG    I     0    9999999  ! FCUP UNGATED
  3  Microsec   I     0    9999999  ! Ungated Clock (1 MHz)
  4  NotUsed4   I     0    9999999  ! Currently not used
  5  MOR_ST     I     0    9999999  ! MOR.ST
  6  MOR_PC     I     0    9999999  ! MOR.PC (unpluged ?)
  7  MOR_PS     I     0    9999999  ! MOR.PS (unpluged ?)
  8  MOR_TAC    I     0    9999999  ! MOR.TAC ( ??? )
  9  MOR        I     0    9999999  ! TAGGER Master OR
 10  PC         I     0    9999999  ! Pair Counter
 11  PS         I     0    9999999  ! Pair Spectrometer
 12  TAC        I     0    9999999  ! Total Absorption Counter
 13  ST         I     0    9999999  ! ST
 14  NotUsed14  I     0    9999999  ! Helicity strob (30 Hz or any)
 15  clock_ug_2 I     0    9999999  ! duplicate of channel 1
 16  fcup_ug_2  I     0    9999999  ! duplicate of channel 2
 17  CLOCK_G1   I     0    9999999  ! Clock with run gatei ----
 18  FCUP_G1    I     0    9999999  ! FCUP with Run Gate ====
 19  NotUsed19  I     0    9999999  ! Currently not used
 20  NotUsed20  I     0    9999999  ! Currently not used
 21  MOR_ST_rg  I     0    9999999  ! MOR.ST  with run gate
 22  MOR_PC_rg  I     0    9999999  ! MOR.PC with run gate
 23  MOR_PS_rg  I     0    9999999  ! MOR.PS with run gate
 24  MOR_TAC_rg I     0    9999999  ! MOR.TAC with run gate
 25  MOR_rg     I     0    9999999  ! MASTER_OR with run gate
 26  PC_rg      I     0    9999999  ! PC with run gate
 27  PS_rg      I     0    9999999  ! PS with run gate
 28  TAC_rg     I     0    9999999  ! TAC with run gate
 29  ST_rg      I     0    9999999  ! ST  with run gate
 30  NotUsed30  I     0    9999999  ! Currently not used
 31  clock_g1_2 I     0    9999999  ! duplicate of channel 17
 32  fcup_g1_2  I     0    9999999  ! duplicate of channel 18
 33  CLOCK_G2   I     0    9999999  ! CLOCK with Live gate ----
 34  FCUP_G2    I     0    9999999  ! FCUP with Live gate ====
 35  trig_or_g2 I     0    9999999  ! Trigger OR of 1-12 with Live gate
 36  NotUsed36  I     0    9999999  ! Currently not used
 37  NotUsed37  I     0    9999999  ! Currently not used
 38  NotUsed38  I     0    9999999  ! Currently not used
 39  NotUsed39  I     0    9999999  ! Currently not used
 40  NotUsed40  I     0    9999999  ! Currently not used
 41  MOR_lg     I     0    9999999  ! MASTER_OR with Live gate
 42  NotUsed42  I     0    9999999  ! Currently not used
 43  NotUsed43  I     0    9999999  ! Currently not used
 44  NotUsed44  I     0    9999999  ! Currently not used
 45  NotUsed45  I     0    9999999  ! Currently not used
 46  NotUsed46  I     0    9999999  ! Currently not used
 47  clock_g2_2 I     0    9999999  ! duplicate of channel 33
 48  fcup_g2_2  I     0    9999999  ! duplicate of channel 34
 49  trig1_ug   I     0    9999999  ! Trigger 1 ungated, prescaled
 50  trig2_ug   I     0    9999999  ! Trigger 2 ungated, prescaled
 51  trig3_ug   I     0    9999999  ! Trigger 3 ungated, prescaled
 52  trig4_ug   I     0    9999999  ! Trigger 4 ungated, prescaled
 53  trig5_ug   I     0    9999999  ! Trigger 5 ungated, prescaled
 54  trig6_ug   I     0    9999999  ! Trigger 6 ungated, prescaled
 55  trig7_ug   I     0    9999999  ! Trigger 7 ungated, prescaled
 56  trig8_ug   I     0    9999999  ! Trigger 8 ungated, prescaled
 57  trig9_ug   I     0    9999999  ! Trigger 9 ungated, prescaled
 58  trig10_ug  I     0    9999999  ! Trigger 10 ungated, prescaled
 59  trig11_ug  I     0    9999999  ! Trigger 11 ungated, prescaled
 60  trig12_ug  I     0    9999999  ! Trigger 12 ungated, prescaled
 61  trig_or_ug I     0    9999999  ! Trigger OR of 1-12 ungated,
 62  l1accept   I     0    9999999  ! Level 1 accept
 63  notused63  I     0    9999999  ! Currently not used
 64  notused64  I     0    9999999  ! Currently not used
 65  l2fail     I     0    9999999  ! Level2 fail
 66  l2pass     I     0    9999999  ! Level2 pass
 67  l2start    I     0    9999999  ! Level2 start
 68  l2clear    I     0    9999999  ! Level2 clear
 69  l2accept   I     0    9999999  ! Level2 accept
 70  l3accept   I     0    9999999  ! Level3 accept
 71  notused71  I     0    9999999  ! Currently not used
 72  notused72  I     0    9999999  ! Currently not used
 73  l2sec1_g  	I     0    9999999  ! Level2 sec1 gated
 74  l2sec2_g  	I     0    9999999  ! Level2 sec2 gated
 75  l2sec3_g  	I     0    9999999  ! Level2 sec3 gated
 76  l2sec4_g  	I     0    9999999  ! Level2 sec4 gated
 77  l2sec5_g  	I     0    9999999  ! Level2 sec5 gated
 78  l2sec6_g  	I     0    9999999  ! Level2 sec6 gated
 79  l2_or_g    I     0    9999999  ! OR level2 gated
 80  l2_ok_g    I     0    9999999  ! Level 2 OK gated
 81  trig1_lg   I     0    9999999  ! Trigger 1 live gated
 82  trig2_lg   I     0    9999999  ! Trigger 2 live gated
 83  trig3_lg  	I     0    9999999  ! Trigger 3 live gated
 84  trig4_lg  	I     0    9999999  ! Trigger 4 live gated
 85  trig5_lg  	I     0    9999999  ! Trigger 5 live gated
 86  trig6_lg  	I     0    9999999  ! Trigger 6 live gated
 87  trig7_lg  	I     0    9999999  ! Trigger 7 live gated
 88  trig8_lg  	I     0    9999999  ! Trigger 8 live gated
 89  trig9_lg   I     0    9999999  ! Trigger 9 live gated
 90  trig10_lg  I     0    9999999  ! Trigger 10 live gated
 91  trig11_lg  I     0    9999999  ! Trigger 11 live gated
 92  trig12_lg  I     0    9999999  ! Trigger 12 live gated
 93  notused93	I     0    9999999  ! not used
 94  notused94  I     0    9999999  ! not used
 95  ignore95   I     0    9999999  ! ignore
 96  ignore96  	I     0    9999999  ! ignore
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  RCST    ! create write display delete ! Roc status bank
!
!   ATTributes:
!   -----------
!COL ATT-name 	   	    FMT  Min     Max     ! Comments
!     	      
  1 down_count                I    0    9999999  ! download count
  2 prestart_count            I    0    9999999  ! prestart count
  3 go_count                  I    0    9999999  ! go count
  4 trig_count                I    0    9999999  ! total trigger count
  5 event_count               I    0    9999999  ! phys event count
  6 sync_count                I    0    9999999  ! force-sync event count
  7 run_trig_count            I    0    9999999  ! trigger count current run
  8 run_event_count           I    0    9999999  ! phys event count current run
  9 run_sync_count            I    0    9999999  ! force-sync event count current run
 10 pause_count               I    0    9999999  ! coda pause count
 11 end_count                 I    0    9999999  ! end count
 12 illegal_count             I    0    9999999  ! illegal count
 13 run_illegal_count         I    0    9999999  ! illegal count current run
 14 phys_sync_count           I    0    9999999  ! physics-sync event count
 15 run_phys_sync_count       I    0    9999999  ! physics-sync event count current run
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  S1ST    ! create write display delete ! Scaler bank 
!
!   ATTributes:
!   -----------
!COL ATT-name 	   	    FMT  Min     Max     ! Comments
!     	      
  1 latch1_bit1_count       I     0    9999999  ! Count trigger bit 1  latched events
  2 latch1_bit2_count       I     0    9999999  ! Count trigger bit 2  latched events
  3 latch1_bit3_count       I     0    9999999  ! Count trigger bit 3  latched events
  4 latch1_bit4_count       I     0    9999999  ! Count trigger bit 4  latched events
  5 latch1_bit5_count       I     0    9999999  ! Count trigger bit 5  latched events
  6 latch1_bit6_count       I     0    9999999  ! Count trigger bit 6  latched events
  7 latch1_bit7_count       I     0    9999999  ! Count trigger bit 7  latched events
  8 latch1_bit8_count       I     0    9999999  ! Count trigger bit 8  latched events
  9 latch1_bit9_count	    I     0    9999999  ! Count trigger bit 9  latched events
 10 latch1_bit10_count	    I     0    9999999  ! Count trigger bit 10 latched events
 11 latch1_bit11_count	    I     0    9999999  ! Count trigger bit 11 latched events
 12 latch1_bit12_count	    I     0    9999999  ! Count trigger bit 12 latched events
 13 event_count 	    I     0    9999999  ! Latched event count this run
 14 latch1_zero_count	    I     0    9999999  ! Latch1 zero count (illegal)
 15 latch1_empty_count	    I     0    9999999  ! Latch1 empty count (illegal)
 16 latch1_not_empty_count  I     0    9999999  ! Latch1 not empty on sync event (illegal)
 17 latch1_ok_count         I     0    9999999  ! Latch1 ok
 18 level2_sector1          I     0    9999999  ! Level2 sector1 count
 19 level2_sector2          I     0    9999999  ! Level2 sector2 count
 20 level2_sector3          I     0    9999999  ! Level2 sector3 count
 21 level2_sector4          I     0    9999999  ! Level2 sector4 count
 22 level2_sector5          I     0    9999999  ! Level2 sector5 count
 23 level2_sector6          I     0    9999999  ! Level2 sector6 count
 24 level2_pass             I     0    9999999  ! Level2 pass count
 25 level2_fail             I     0    9999999  ! Level2 fail count
 26 latch2_zero_count	    I     0    9999999  ! Latch2 zero count (illegal)
 27 latch2_empty_count	    I     0    9999999  ! Latch2 empty count (illegal)
 28 latch2_not_empty_count  I     0    9999999  ! Latch2 not empty on sync event (illegal)
 29 latch2_ok_count         I     0    9999999  ! Latch2 ok
 30 roc_13_count            I     0    9999999  ! Roc code 13 count (zero latch)
 31 roc_15_count            I     0    9999999  ! Roc code 15 count (illegal)
 32 l1l2_zero_count         I     0    9999999  ! (latch1==0)&&(latch2==0)
 33 l1zero_13_count         I     0    9999999  ! (latch1==0)&&(roc_code==13)
 34 l2zero_13_count         I     0    9999999  ! (latch2==0)&&(roc_code==13)
 35 l1l2zero_13_count       I     0    9999999  ! (latch1==0)&&(latch2==0)&&(roc_code==13)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ST       B16   ! create write display delete ! Start counter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      6  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  STN0     B16   ! create write display delete ! Start counter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      24  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  STN1     B16   ! create write display delete ! Start counter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      24  ! the address of the hit detector element
  2  ADC      I     0   65536  ! adc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  ELSR     B16   ! create write display delete ! ???????? (ROC14)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      6   ! 
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CALL      B16   ! create write display delete ! Catch-ALL event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      10  ! catch-all element(RF,laser diode,etc)
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CL       B16   ! create write display delete ! ????????? (ROC13)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      6   ! 
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
!    RELations:
!    ----------
!COL RELname  RELtype INTbank  ! Comments
!                     (COL)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  STS     ! create write display delete ! Start Counter scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   STS1      I     0    9999999  ! trigger rate sector 1-2
  2   STS2      I     0    9999999  ! trigger rate sector 3-4
  3   STS3      I     0    9999999  ! trigger rate sector 5-6
  4   STS4      I     0    9999999  ! 
  5   STS5      I     0    9999999  ! 
  6   STS6      I     0    9999999  ! 
  7   STS7      I     0    9999999  ! 
  8   STS8      I     0    9999999  ! 
  9   STS9      I     0    9999999  ! 
  10  STS10     I     0    9999999  ! 
  11  STS11     I     0    9999999  ! 
  12  STS12     I     0    9999999  ! 
  13  STS13     I     0    9999999  ! 
  14  STS14     I     0    9999999  ! 
  15  STS15     I     0    9999999  ! 
  16  STS16     I     0    9999999  ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  STSN    ! create write display delete ! Start Counter scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   STSN1      I     0    9999999  !
  2   STSN2      I     0    9999999  !
  3   STSN3      I     0    9999999  !
  4   STSN4      I     0    9999999  ! 
  5   STSN5      I     0    9999999  ! 
  6   STSN6      I     0    9999999  ! 
  7   STSN7      I     0    9999999  ! 
  8   STSN8      I     0    9999999  ! 
  9   STSN9      I     0    9999999  ! 
  10  STSN10     I     0    9999999  ! 
  11  STSN11     I     0    9999999  ! 
  12  STSN12     I     0    9999999  ! 
  13  RES1       I     0    9999999  ! 
  14  RES2       I     0    9999999  ! 
  15  RES3       I     0    9999999  ! 
  16  RES4       I     0    9999999  ! 
  17  STSN13     I     0    9999999  ! 
  18  STSN14     I     0    9999999  ! 
  19  STSN15     I     0    9999999  ! 
  20  STSN16     I     0    9999999  ! 
  21  STSN17     I     0    9999999  ! 
  22  STSN18     I     0    9999999  ! 
  23  STSN19     I     0    9999999  ! 
  24  STSN20     I     0    9999999  ! 
  25  STSN21     I     0    9999999  ! 
  26  STSN22     I     0    9999999  ! 
  27  STSN23     I     0    9999999  ! 
  28  STSN24     I     0    9999999  ! 
  29  STOR       I     0    9999999  ! 
  30  STMULT     I     0    9999999  ! 
  31  STANDMOR   I     0    9999999  ! 
  32  MULTANDMOR I     0    9999999  ! 
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TGS     ! create write display delete ! Photon Flux Scaler bank 
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!
  1   RAWT1      I     0    9999999  ! 
  2   RAWT2      I     0    9999999  ! 
  3   RAWT3      I     0    9999999  ! 
  4   RAWT4      I     0    9999999  ! 
  5   RAWT5      I     0    9999999  ! 
  6   RAWT6      I     0    9999999  ! 
  7   RAWT7      I     0    9999999  ! 
  8   RAWT8      I     0    9999999  ! 
  9   RAWT9      I     0    9999999  ! 
  10  RAWT10     I     0    9999999  ! 
  11  RAWT11     I     0    9999999  ! 
  12  RAWT12     I     0    9999999  ! 
  13  RAWT13     I     0    9999999  ! 
  14  RAWT14     I     0    9999999  ! 
  15  RAWT15     I     0    9999999  ! 
  16  RAWT16     I     0    9999999  ! 
  17  RAWT17     I     0    9999999  ! 
  18  RAWT18     I     0    9999999  ! 
  19  RAWT19     I     0    9999999  ! 
  20  RAWT20     I     0    9999999  ! 
  21  RAWT21     I     0    9999999  ! 
  22  RAWT22     I     0    9999999  ! 
  23  RAWT23     I     0    9999999  ! 
  24  RAWT24     I     0    9999999  ! 
  25  RAWT25     I     0    9999999  ! 
  26  RAWT26     I     0    9999999  ! 
  27  RAWT27     I     0    9999999  ! 
  28  RAWT28     I     0    9999999  ! 
  29  RAWT29     I     0    9999999  ! 
  30  RAWT30     I     0    9999999  ! 
  31  RAWT31     I     0    9999999  ! 
  32  RAWT32     I     0    9999999  ! 
  33  RAWT33     I     0    9999999  ! 
  34  RAWT34     I     0    9999999  ! 
  35  RAWT35     I     0    9999999  ! 
  36  RAWT36     I     0    9999999  ! 
  37  RAWT37     I     0    9999999  ! 
  38  RAWT38     I     0    9999999  ! 
  39  RAWT39     I     0    9999999  ! 
  40  RAWT40     I     0    9999999  ! 
  41  RAWT41     I     0    9999999  ! 
  42  RAWT42     I     0    9999999  ! 
  43  RAWT43     I     0    9999999  ! 
  44  RAWT44     I     0    9999999  ! 
  45  RAWT45     I     0    9999999  ! 
  46  RAWT46     I     0    9999999  ! 
  47  RAWT47     I     0    9999999  ! 
  48  RAWT48     I     0    9999999  ! 
  49  RAWT49     I     0    9999999  ! 
  50  RAWT50     I     0    9999999  ! 
  51  RAWT51     I     0    9999999  ! 
  52  RAWT52     I     0    9999999  ! 
  53  RAWT53     I     0    9999999  ! 
  54  RAWT54     I     0    9999999  ! 
  55  RAWT55     I     0    9999999  ! 
  56  RAWT56     I     0    9999999  ! 
  57  RAWT57     I     0    9999999  ! 
  58  RAWT58     I     0    9999999  ! 
  59  RAWT59     I     0    9999999  ! 
  60  RAWT60     I     0    9999999  ! 
  61  RAWT61     I     0    9999999  ! 
  62  notused62  I     0    9999999  ! 
  63  notused63  I     0    9999999  ! 
  64  notused64  I     0    9999999  ! 
  65  BNK1T1     I     0    9999999  !
  66  BNK1T2     I     0    9999999  !
  67  BNK1T3     I     0    9999999  !
  68  BNK1T4     I     0    9999999  !
  69  BNK1T5     I     0    9999999  !
  70  BNK1T6     I     0    9999999  !
  71  BNK1T7     I     0    9999999  !
  72  BNK1T8     I     0    9999999  !
  73  BNK1T9     I     0    9999999  !
  74  BNK1T10    I     0    9999999  !
  75  BNK1T11    I     0    9999999  !
  76  BNK1T12    I     0    9999999  !
  77  BNK1T13    I     0    9999999  !
  78  BNK1T14    I     0    9999999  !
  79  BNK1T15    I     0    9999999  !
  80  BNK1T16    I     0    9999999  !
  81  BNK1T17    I     0    9999999  !
  82  BNK1T18    I     0    9999999  !
  83  BNK1T19    I     0    9999999  !
  84  BNK1T20    I     0    9999999  !
  85  BNK1T21    I     0    9999999  !
  86  BNK1T22    I     0    9999999  !
  87  BNK1T23    I     0    9999999  !
  88  BNK1T24    I     0    9999999  !
  89  BNK1T25    I     0    9999999  !
  90  BNK1T26    I     0    9999999  !
  91  BNK1T27    I     0    9999999  !
  92  BNK1T28    I     0    9999999  !
  93  BNK1T29    I     0    9999999  !
  94  BNK1T30    I     0    9999999  !
  95  BNK1T31    I     0    9999999  !
  96  BNK1T32    I     0    9999999  !
  97  BNK1T33    I     0    9999999  !
  98  BNK1T34    I     0    9999999  !
  99  BNK1T35    I     0    9999999  !
 100  BNK1T36    I     0    9999999  !
 101  BNK1T37    I     0    9999999  !
 102  BNK1T38    I     0    9999999  !
 103  BNK1T39    I     0    9999999  !
 104  BNK1T40    I     0    9999999  !
 105  BNK1T41    I     0    9999999  !
 106  BNK1T42    I     0    9999999  !
 107  BNK1T43    I     0    9999999  !
 108  BNK1T44    I     0    9999999  !
 109  BNK1T45    I     0    9999999  !
 110  BNK1T46    I     0    9999999  !
 111  BNK1T47    I     0    9999999  !
 112  BNK1T48    I     0    9999999  !
 113  BNK1T49    I     0    9999999  !
 114  BNK1T50    I     0    9999999  !
 115  BNK1T51    I     0    9999999  !
 116  BNK1T52    I     0    9999999  !
 117  BNK1T53    I     0    9999999  !
 118  BNK1T54    I     0    9999999  !
 119  BNK1T55    I     0    9999999  !
 120  BNK1T56    I     0    9999999  !
 121  BNK1T57    I     0    9999999  !
 122  BNK1T58    I     0    9999999  !
 123  BNK1T59    I     0    9999999  !
 124  BNK1T60    I     0    9999999  !
 125  BNK1T61    I     0    9999999  !
 126  notused126 I     0    9999999  !
 127  notused127 I     0    9999999  !
 128  notused128 I     0    9999999  !
 129  BNK2T1     I     0    9999999  !
 130  BNK2T2     I     0    9999999  !
 131  BNK2T3     I     0    9999999  !
 132  BNK2T4     I     0    9999999  !
 133  BNK2T5     I     0    9999999  !
 134  BNK2T6     I     0    9999999  !
 135  BNK2T7     I     0    9999999  !
 136  BNK2T8     I     0    9999999  !
 137  BNK2T9     I     0    9999999  !
 138  BNK2T10    I     0    9999999  !
 139  BNK2T11    I     0    9999999  !
 140  BNK2T12    I     0    9999999  !
 141  BNK2T13    I     0    9999999  !
 142  BNK2T14    I     0    9999999  !
 143  BNK2T15    I     0    9999999  !
 144  BNK2T16    I     0    9999999  !
 145  BNK2T17    I     0    9999999  !
 146  BNK2T18    I     0    9999999  !
 147  BNK2T19    I     0    9999999  !
 148  BNK2T20    I     0    9999999  !
 149  BNK2T21    I     0    9999999  !
 150  BNK2T22    I     0    9999999  !
 151  BNK2T23    I     0    9999999  !
 152  BNK2T24    I     0    9999999  !
 153  BNK2T25    I     0    9999999  !
 154  BNK2T26    I     0    9999999  !
 155  BNK2T27    I     0    9999999  !
 156  BNK2T28    I     0    9999999  !
 157  BNK2T29    I     0    9999999  !
 158  BNK2T30    I     0    9999999  !
 159  BNK2T31    I     0    9999999  !
 160  BNK2T32    I     0    9999999  !
 161  BNK2T33    I     0    9999999  !
 162  BNK2T34    I     0    9999999  !
 163  BNK2T35    I     0    9999999  !
 164  BNK2T36    I     0    9999999  !
 165  BNK2T37    I     0    9999999  !
 166  BNK2T38    I     0    9999999  !
 167  BNK2T39    I     0    9999999  !
 168  BNK2T40    I     0    9999999  !
 169  BNK2T41    I     0    9999999  !
 170  BNK2T42    I     0    9999999  !
 171  BNK2T43    I     0    9999999  !
 172  BNK2T44    I     0    9999999  !
 173  BNK2T45    I     0    9999999  !
 174  BNK2T46    I     0    9999999  !
 175  BNK2T47    I     0    9999999  !
 176  BNK2T48    I     0    9999999  !
 177  BNK2T49    I     0    9999999  !
 178  BNK2T50    I     0    9999999  !
 179  BNK2T51    I     0    9999999  !
 180  BNK2T52    I     0    9999999  !
 181  BNK2T53    I     0    9999999  !
 182  BNK2T54    I     0    9999999  !
 183  BNK2T55    I     0    9999999  !
 184  BNK2T56    I     0    9999999  !
 185  BNK2T57    I     0    9999999  !
 186  BNK2T58    I     0    9999999  !
 187  BNK2T59    I     0    9999999  !
 188  BNK2T60    I     0    9999999  !
 189  BNK2T61    I     0    9999999  !
 190  notused190 I     0    9999999  !
 191  notused191 I     0    9999999  !
 192  notused192 I     0    9999999  !
!
 END TABLE
!
!-----------------------------------------------------------------------
!
!       BANKname BANKtype      !Comments
 TABLE   +DST      ! create write display delete ! Bank HEAD
!
!COL ATT-name FMT Min    Max   !Comments
   1 TEST     I    0      2    !test
!
 END TABLE
!
!-----------------------------------------------------------------------
!
!       BANKname BANKtype      !Comments
 TABLE   HISS    B16  ! create write display delete ! Short (2 byte) histograms
!
!COL ATT-name FMT Min    Max   !Comments
   1 DATA     I    0   1000000 !test
!
 END TABLE
!
!-----------------------------------------------------------------------
!
!       BANKname BANKtype      !Comments
 TABLE   HISI      ! create write display delete ! Integer (4 byte) histograms
!
!COL ATT-name FMT Min    Max   !Comments
   1 DATA     I    0   1000000 !test
!
 END TABLE
!
!-----------------------------------------------------------------------
!
!       BANKname BANKtype      !Comments
 TABLE   HISF      ! create write display delete ! Float histograms
!
!COL ATT-name FMT Min    Max   !Comments
   1 DATA     F    0   1000000 !test
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TESC          ! create write display delete 
!
!   TAG E-counter SCaler bank 
!   record no.=0 :  array of 384 values (count rate per E-counter channel)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  value    F   0.0   999999999.0  ! count rate per E-counter channel
!
 END TABLE
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  EPIC          ! create write display delete 
!
!   Online epics channel values
!   record no.=0 :  read out of lots of epics channels (every 20 sec)
!   record no.=1 :  read out of BPMs and Active Colemator (every 2 sec)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  value    F   -999999.0   999999.0 ! value of the epics channel
  2  char1    A    0          255      ! char[32] name;
  3  char2    A    0          255      ! 
  4  char3    A    0          255      ! 
  5  char4    A    0          255      ! 
  6  char5    A    0          255      ! 
  7  char6    A    0          255      ! 
  8  char7    A    0          255      ! 
  9  char8    A    0          255      ! 
!
 END TABLE
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  RNLG    ! create write display delete ! Database archive bank 
!
!   ATTributes:
!   -----------
!COL ATT-name FMT  Min     Max     ! Comments
!     	      
  1 sql       A    0    9999999    ! Run log entry SQL statement
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  REF      B16   !       ! Reference signals for pipeline TDCs
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! roc id (high byte) & slot number (low byte)
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TCT      B16   !       ! total absorbtion counter TDC (pipeline TDCs)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! id
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  RFT      B16   !       ! RF signal TDC (pipeline TDCs)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! id
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPC      B16   !       ! BoNuS TPC event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information
  3  ADC      I     0   65536  ! adc information
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPCH     B32   ! create    ! BoNuS TPC head bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  NEVENT   I     0  9000000 ! event number
  2  TIME     I     0  9000000 ! time stamp
  3  NHITS    I     0  9000000 ! the number of hits
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  HLB      B32   ! create    ! Helicity scaler bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  S1       I     1  9000000 ! reserve
  2  S2       I     1  9000000 ! reserve
  3  S3       I     1  9000000 ! reserve
  4  S4       I     1  9000000 ! reserve
  5  S5       I     1  9000000 ! reserve
  6  S6       I     1  9000000 ! reserve
  7  S7       I     1  9000000 ! reserve
  8  S8       I     1  9000000 ! reserve
  9  S9       I     1  9000000 ! reserve
  10 S10      I     1  9000000 ! reserve
  11 S11      I     1  9000000 ! reserve
  12 S12      I     1  9000000 ! reserve
  13 S13      I     1  9000000 ! reserve
  14 S14      I     1  9000000 ! reserve
  15 S15      I     1  9000000 ! reserve
  16 S16      I     1  9000000 ! reserve
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  LSRT     B16   ! create    ! TOF laser reference signal (TDCs)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I     1      96  ! the address of the hit detector element
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  EC2      B16   !       ! Preshower calorimeter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SH       B16   !       ! Scintillator hodoscope event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  SHT      B16   !       ! Scintillator hodoscope (multihit TDCs)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  CT       B16   !       ! Central time-of-flight event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
  3  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPC0     B32   !       ! eg6 TPC event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  IDTDCADC    I   0   65000 ! id, tdc, adc packed
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPC6     B16   !       ! eg6 TPC event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  DATA        I   0   65000 ! id, tdc, adc packed
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPE1     B16   !       ! TPE calorimeter event bank
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  ADC      I     0   65536  ! adc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  TPE0     B16   !       ! TPE calorimeter pipeline TDCs
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  DFM0     B16   !       ! Dense fiber monitor pipeline TDCs
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the hit detector element
  2  TDC      I     0   65536  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
!       BANKname BANKtype      ! Comments
 TABLE  GAT0      B16   !      ! Gate event bank (TDCs only)
!
!   ATTributes:
!   -----------
!COL ATT-name FMT Min    Max   ! Comments
!
  1  ID       I   257    1572  ! the address of the gate
  2  TDC      I     0   65535  ! tdc information (channels)
!
 END TABLE
!
!-----------------------------------------------------------------------
