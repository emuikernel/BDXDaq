h26897
s 00000/00000/00000
d R 1.2 01/11/19 19:01:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/obsolete/sdatagcal.h
e
s 00068/00000/00000
d D 1.1 01/11/19 19:01:20 boiarino 1 0
c date and time created 01/11/19 19:01:20 by boiarino
e
u
U
f e 0
t
T
I 1
/*
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
*/

typedef struct sdatagcal
{
  float ET_window;
  float TAG_RF_MEAN;
  float TAG_RF_10SIGMA;
  float TAG_TCUT[2];
  float RF_OFFSET;
  float RF_SLOPE[2];
  float Tbound[122];
  float Ebound[768];
  float Eaver[768]; /* !!!!!!!!! was Eaver(0:767) !!!!!!!!!! */
  float Edev[767];
  int   tagT_status[61];
  int   etmin[384];
  int   etmax[384];
  int   t1bin[61];
  int   t2bin[61];
  float slopeTL[61];
  float slopeTR[61];
  float TpeakL[61];
  float TpeakR[61];
  float TagTCi[121];
  float slopeE;
  float Epeak[384];
} SDAtagcal;

SDAtagcal sdatagcal_;
E 1
