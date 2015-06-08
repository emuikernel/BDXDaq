h46112
s 00000/00000/00000
d R 1.2 01/11/19 19:00:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdaevgen.h
e
s 00129/00000/00000
d D 1.1 01/11/19 19:00:38 boiarino 1 0
c date and time created 01/11/19 19:00:38 by boiarino
e
u
U
f e 0
t
T
I 1
/*
C
C Simulated track parameters (starting values) in the event at target position
C                                                Bogdan Niczyporuk, March 1991
C                                                Modified by B.N. on Jan. 1996
C
C |--------------------------------------------------------------|
C | All tracks in Detector Coordinate System (beam along Z-axis) | 
C |--------------------------------------------------------------|
C
C  ntrack     - Number of tracks in the event (ntrack <= ntrmax}
C
C  ev_in(1,it) |          
C  ev_in(2,it) |-> Starting point (x,y,z) of the track (e.g. target pos.)  
C  ev_in(3,it) |          
C
C  ev_in(4,it) |          
C  ev_in(5,it) |-> Starting direction (unit vector) of the track  
C  ev_in(6,it) |          
C
C  ev_in(7,it)  -  Momentum of a particle [GeV]
C  ev_in(8,it)  -  Mass of a particle [GeV]
C  ev_in(9,it)  -  Charge of a particle [-1, 0, +1]
C
C |------------------------------------------------------------|
C | All tracks in Sector Coordinate System (beam along X-axis) | 
C |------------------------------------------------------------|
C
C  evin(1,it) |          
C  evin(2,it) |---> Starting point (x,y,z) of the track (e.g. target pos.)  
C  evin(3,it) |          
C
C  evin(4,it) |          
C  evin(5,it) |---> Starting direction (unit vector) of the track  
C  evin(6,it) |          
C
C  evin(7,it)      -  Momentum of a particle [GeV]
C  evin(8,it)      -  Mass of a particle [GeV]
C  evin(9,it)      -  Charge of a particle [-1, 0, +1]
C
C  itrin_sec(it)   - Sector number of track "it"
C  isec_sim        - Current track sector# { isec_sim = itrin_sec(it) }
C  itrin_acc(5,it) - Acceptance of track for {ST,DC,CC,SC,EC}  [> 0 accepted]
C  Nevin_prod      - Number of produced events for given Luminocity
C  Nevin_acc       - Number of accepted events with given multiplicity "zcut(7)"
C
C |-------------------------------------------------------|
C | A la LUND format of the generated event in Lab System | 
C |-------------------------------------------------------|
C
C  PL(1,it)  -  Momentum Px [GeV]
C  PL(2,it)  -  Momentum Py [GeV]
C  PL(3,it)  -  Momentum Pz [GeV]
C  PL(4,it)  -  Total energy of the particle E [GeV]
C  PL(5,it)  -  Mass of the particle M [GeV]
C
C |----------------------------------------------------------------------|
C | Kinematic variables describing the event (for 2,3 final tracks only) | 
C |----------------------------------------------------------------------|
C
C  Ebeam  -  Beam energy [GeV]
C  Qsqr   -  Mass squared (Q2) of the virtual photon [GeV2]
C  Wmass  -  Total energy (W) of photon and target nucleon [GeV]
C  Ttr    -  Four-momentum transfer to the target nucleon (t) [GeV2]
C  THstar -  Center of Mass polar angle [rad]
C  PHstar -  Center of Mass azimuthal angle [rad]
C  iFermi -  Fermi motion flag ( 0 - free nucleon, .NE.0 - bound nucleon)
C
C |----------------------------------------------------------------|
C | Results of swimming for Simulated tracks ( for digitalization) |                 
C |----------------------------------------------------------------|
C
C xx_trps(1,ip,itr) |
C xx_trps(2,ip,itr) |--> Track point {x,y,z} on a plane for track "itr"
C xx_trps(3,ip,itr) |
C
C xx_trps(4,ip,itr) |
C xx_trps(5,ip,itr) |--> Track direction at track point on a plane for "itr"
C xx_trps(6,ip,itr) |                 
C xx_trps(7,ip,itr)  --> Track length [cm] from the target to a plane for "itr"
C                    where "xx" stands for {st, dc, cc, sc, ec}
C
      INTEGER ntrack, itrin_sec,isec_sim,itrin_acc, Nevin_prod,Nevin_acc         
     1       ,ntrmax, iFermi
      REAL ev_in, evin, Ebeam, Qsqr,Wmass,Ttr,THstar,PHstar, PL
C
      PARAMETER (ntrmax = 10)
C
C The following stuff is used in "xx_digit.F" and "sim_trig.F" only 
      REAL st_trps(7,2,ntrmax),dc_trps(7,36,ntrmax), cc_trps(7,1,ntrmax)                
     1    ,sc_trps(7,4, ntrmax), ec_trps(7,2,ntrmax)
C
      COMMON/sdaevgen/ev_in(9,ntrmax), evin(9,ntrmax), itrin_sec(ntrmax)               
     1      ,isec_sim, itrin_acc(5,ntrmax), ntrack, Nevin_prod,Nevin_acc 
     2      ,st_trps, dc_trps, cc_trps, sc_trps, ec_trps        
     3      ,Ebeam, Qsqr,Wmass,Ttr,THstar,PHstar, PL(5,ntrmax), iFermi
C
      save  /sdaevgen/
C
*/

#define ntrmax 10

typedef struct sdaevgen
{
  float ev_in[ntrmax][9];
  float evin[ntrmax][9];
  int   itrin_sec[ntrmax];              
  int   isec_sim;
  int   itrin_acc[ntrmax][5];
  int   ntrack;
  int   Nevin_prod;
  int   Nevin_acc;
  float st_trps[ntrmax][2][7];
  float dc_trps[ntrmax][36][7];
  float cc_trps[ntrmax][1][7];
  float sc_trps[ntrmax][4][7];
  float ec_trps[ntrmax][2][7];
  float Ebeam;
  float Qsqr;
  float Wmass;
  float Ttr;
  float THstar;
  float PHstar;
  float PL[ntrmax][5];
  int   iFermi;
} SDAevgen;

SDAevgen sdaevgen_;
E 1
