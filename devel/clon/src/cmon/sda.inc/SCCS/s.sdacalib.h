h22277
s 00012/00007/00119
d D 1.2 07/11/01 15:00:55 boiarino 3 1
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdacalib.h
e
s 00126/00000/00000
d D 1.1 01/11/19 19:00:37 boiarino 1 0
c date and time created 01/11/19 19:00:37 by boiarino
e
u
U
f e 0
t
T
I 1
/*
C
C Calibration Constants currently used in SDA for Simulation & Reconstruction
C                                                Bogdan Niczyporuk, June 1996
C
C
C             *** Constants related to TRIG ***
C
C Tdel                  - delay [ns] from SC P.T. to Pre-trigger Black Box
C Tjit                  - trigger jitter [ns] 
C DL1                   - time delay [ns] for DC common TDC stop
C vflt                  - velocity of relativistic particle [cm/ns]
C
C             *** Constants for ST ***
C
D 3
C cal_st(1,isec)  - cable length [ns] 
C cal_st(2,isec)  - slope [cnts/ns]
C cal_st(3,isec)  - ADC conversion
C cal_st(4,isec)  - velocity [cm/ns] of signal propagation along a slab  
C cal_st(5,isec)  - attenuation length [cm] in a slab
E 3
I 3
C cal_st(1,slab)  - cable length [ns] (=T0_value+delta_T_pd2pd-delta_T_st2tof)
C cal_st(2,slab)  - slope [cnts/ns]   (=1/T1_value)
C cal_st(3,slab)  - ADC conversion
C cal_st(4,slab)  - velocity [cm/ns] of signal propagation along a slab (leg)
C cal_st(5,slab)  - attenuation length [cm] in a slab
C cal_st(6,slab)  - timewalk: adc_max
C cal_st(7,slab)  - timewalk: W1_value
C cal_st(8,slab)  - timewalk: W2_value
C cal_st(9,slab)  - propagation velocity in nose (=veff_nose)
C cal_st(10,slab) - quadratic correction for velocity in nose (=veff_nose2)
E 3
C
C Elos_st  - energy loss [Mev/cm] in SC slab for minimum ionizig particle
C Ethr_st  - energy threshold [MeV] before attenuation (used in SIM)
C
C             *** Constants for DC ***
C
C tc_dc  (iw,il,isec)   - cable length [ns] for each DC wire
C dc_stat(iw,il,isec)   - status of the wire (0 OK, 1 Bad)
C vprp_dc(SLayer)       - velocity of signal propagation along a wire [cm/ns]
C Tsl_dc                - slope [cnts/ns] (tdc --> counts conversion)
C vgas                  - drift velocity in a drift cell [cm/ns]
C td_h(3,iLayer)        - ngrid, Tmax [ns], Dmax [cm]
C td_f(2,igrid,iLayer)  - Td [ns], Dm [cm]
C
C             *** Constants for SC ***
C
C cal_sc(1,islab,isec)  - cable length [ns] for  left P.T. 
C cal_sc(2,islab,isec)  - cable length [ns] for right P.T.
C cal_sc(3,islab,isec)  - slope [cnts/ns] for  left P.T.
C cal_sc(4,islab,isec)  - slope [cnts/ns] for right P.T.
C cal_sc(5,islab,isec)  - ADC conversion [cnts/Mev] for  left P.T.
C cal_sc(6,islab,isec)  - ADC conversion [cnts/Mev] for right P.T.
C cal_sc(7,islab,isec)  - velocity [cm/ns] of signal propagation along a slab  
C cal_sc(8,islab,isec)  - attenuation length [cm] in a slab
C cal_sc(9,islab,isec)  - spare
C cal_sc(10,islab,isec) - spare
C cal_sc(11,islab,isec) - pedestals for left  P.T.
C cal_sc(12,islab,isec) - pedestals for right P.T.
C cal_sc(13,islab,isec) - time walk constant "w0" left
C cal_sc(14,islab,isec) - time walk constant "w0" right
C cal_sc(15,islab,isec) - time walk constant "w1" left
C cal_sc(16,islab,isec) - time walk constant "w1" right
C cal_sc(17,islab,isec) - time walk constant "w2" left
C cal_sc(18,islab,isec) - time walk constant "w2" right
C cal_sc(19,islab,isec) - (twL - twR)/2  Laser
C cal_sc(20,islab,isec) - (twL + twR)/2  Data or Cosmic
C cal_sc(21,islab,isec) - T0_TDC -offset to slope, left
C cal_sc(22,islab,isec) - T0_TDC -offset to slope, right
C cal_sc(23,islab,isec) - T2_TDC -quadratic term to slope, left
C cal_sc(24,islab,isec) - T2_TDC -quadratic term to slope, right
C
C Elos_sc  - energy loss [Mev/cm] in SC slab for minimum ionizig particle
C Ethr_sc  - energy threshold [MeV] before attenuation (used in SIM)
C
C             *** Constants for EC ***
C
C cal_ec(1,islab,iview,isec)  - cable length [ns] 
C cal_ec(2,islab,iview,isec)  - slope [cnts/ns]
C cal_ec(3,islab,iview,isec)  - ADC conversion [cnts/Mev]
C cal_ec(4,islab,iview,isec)  - velocity of signal propagation [cm/ns] 
C cal_ec(5,islab,iview,isec)  - attenuation length [cm]
C cal_ec(6,islab,iview,isec)  - pedestals [cnts]
C
C Ethr_ec  - visible energy threshold [MeV]
C
C
      INTEGER dc_stat
      REAL Tdel, Tjit, DL1, vflt
      REAL cal_st, Elos_st, Ethr_st 
      REAL tc_dc, vprp_dc, Tsl_dc, vgas, td_h, td_f
      REAL cal_sc, Elos_sc, Ethr_sc
      REAL cal_ec, Ethr_ec 
C
      COMMON/sdacalib/ Tdel, Tjit, DL1, vflt
D 3
     1      ,cal_st(5,6), Elos_st, Ethr_st
E 3
I 3
     1      ,cal_st(10,24), Elos_st, Ethr_st
E 3
     2      ,tc_dc(192,36,6), dc_stat(192,36,6), vprp_dc(6)
     3      ,Tsl_dc,vgas, td_h(3,36), td_f(2,200,36)
     4      ,cal_sc(24,48,6), Elos_sc, Ethr_sc
     5      ,cal_ec(6,36,6,6), Ethr_ec
      save  /sdacalib/
C
*/

typedef struct sdacalib
{
  float Tdel;
  float Tjit;
  float DL1;
  float vflt;

D 3
  float cal_st[6][5];
E 3
I 3
  float cal_st[24][10];
E 3
  float Elos_st;
  float Ethr_st;

  float tc_dc[6][36][192];
  int   dc_stat[6][36][192];
  float vprp_dc[6];
  float Tsl_dc,vgas;
  float td_h[36][3];
  float td_f[36][200][2] ;                       

  float cal_sc[6][48][24];
  float Elos_sc;
  float Ethr_sc;

  /* float cal_ec[6][6][36][6]; */
  /* float Ethr_ec;             */

} SDAcalib;

SDAcalib sdacalib_;





E 1
