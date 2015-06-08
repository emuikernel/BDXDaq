C  include file with all tagger calibration constants
C
C  coincidence binning w.r.t E-counter (Eid=1...384)
      integer  etmin, etmax
C
C  coincidence binning w.r.t T-counter (Tid=1...61)
      integer  t1bin, t2bin
C
C  status for T counters (0=good, 1=left TDC missing, 2=right TDC missing)
      integer tagT_status

C  energy boundaries for T-counters (121: 61 counters + overlap)
C  energy mean, deviation for E-counters (767: 384 counters + overlap)
      real  Tbound, Ebound, Eaver, Edev
      
C  coincidence parameters 
      real  ET_window
      real  TAG_RF_MEAN,TAG_RF_10SIGMA,TAG_TCUT

C  RF parameters
      real  RF_OFFSET, RF_SLOPE

C map entries:
C peak, width, sum_offset for T-counters (left&right) and E-counters
      real  slopeTL, slopeTR, Tpeakl, Tpeakr
      real  TagTCi
      real  slopeE, Epeak

      COMMON /sdatagcal/ ET_window,TAG_RF_MEAN,TAG_RF_10SIGMA,
     &                   TAG_TCUT(2),RF_OFFSET,RF_SLOPE(2),
     &                   Tbound(122),Ebound(768),
     &                   Eaver(0:767),Edev(767),tagT_status(61),
     &                   etmin(384), etmax(384), t1bin(61), t2bin(61),
     &                   slopeTL(61),slopeTR(61),TpeakL(61),TpeakR(61),
     &                   TagTCi(121),slopeE,Epeak(384)
C
      SAVE /sdatagcal/
C

