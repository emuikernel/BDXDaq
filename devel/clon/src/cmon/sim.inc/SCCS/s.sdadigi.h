h50842
s 00000/00000/00000
d R 1.2 01/11/19 19:04:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.inc/sdadigi.h
e
s 00111/00000/00000
d D 1.1 01/11/19 19:04:38 boiarino 1 0
c date and time created 01/11/19 19:04:38 by boiarino
e
u
U
f e 0
t
T
I 1
/*
C
C Results of the CALL,ST,TG,DC,CC,SC,EC,EC1 digitalization (data or simulation)
C                                                 Bogdan Niczyporuk, March 1991
C     
C
C  bm_ndig      - number of hits (digits)
C  bm_digi(1,7) - id ( = 6 & 7 are beam bunch markers)
C  bm_digi(2,7) - TDC
C  bm_digi(3,7) - ADC
C
C  st_ndig      - number of hits (= 2 or 4 or 6, since 1<-->2, 3<-->4 & 5<-->6)
C  st_digi(1,6) - id (= sector# of a track)
C  st_digi(2,6) - TDC
C  st_digi(3,6) - ADC
C
C  dc_ndig(ilayer,isect) - number of hits (digits) in a given layer & sector
C  dc_digi(1,idx,ilayer,isect) - wire number
C  dc_digi(2,idx,ilayer,isect) - TDC
C
C  cc_ndig(ilayer,isect) - number of hits (digits) in a given layer & sector
C  cc_digi(1,idx,ilayer,isect) - PT No (odd/even = left/right seen along beam)
C  cc_digi(2,idx,ilayer,isect) - TDC
C  cc_digi(3,idx,ilayer,isect) - ADC
C
C  sc_ndig(ilayer,isect) - number of hits (digits) in a given layer & sector
C  sc_digi(1,idx,ilayer,isect) - Slab number
C  sc_digi(2,idx,ilayer,isect) - TDCL (-y or left seen along beam)
C  sc_digi(3,idx,ilayer,isect) - ADCL (-y or left seen along beam)
C  sc_digi(4,idx,ilayer,isect) - TDCR (+y or right seen along beam)
C  sc_digi(5,idx,ilayer,isect) - ADCR (+y or right seen along beam)
C
C  ec_ndig(ilayer,isect) - number of hits (digits) in a given layer & sector
C  ec_digi(1,idx,ilayer,isect) - slab number (stack)
C  ec_digi(2,idx,ilayer,isect) - TDC
C  ec_digi(3,idx,ilayer,isect) - ADC
C
C  ec1_ndig(ilayer,isect) - number of hits (digits) in a given layer & sector
C  ec1_digi(1,idx,ilayer,isect) - slab number (stack)
C  ec1_digi(2,idx,ilayer,isect) - TDCL
C  ec1_digi(3,idx,ilayer,isect) - ADCL
C  ec1_digi(4,idx,ilayer,isect) - TDCR
C  ec1_digi(5,idx,ilayer,isect) - ADCR
C
C  tgt_ndig - number of hits (digits)
C  tgt_digi(1,idx) - slab number
C  tgt_digi(2,idx) - TDCL
C  tgt_digi(3,idx) - TDCR
C
C  tge_ndig - number of hits (digits)
C  tge_digi(1,idx) - slab number
C  tge_digi(2,idx) - TDCL
C
C  Ttrig   - Trigger Time [ns]
C
C nlay_max - maximum number of layers: DC0 nlay_max = 36
C                                      CC  nlay_max =  1
C                                      SC  nlay_max =  1
C                                      EC  nlay_max =  6 [ui,vi,wi,uo,vo,wo]
C                                      EC1 nlay_max =  4 [Xi,Zi,Xo,Zo]
C above X means strip along x-axis and Y means strip along z-axis in SDA C.S.
C 24 X-strips and 40 Z-strips. 33 layers in depth both X and Z for "i" & "o". 
C                                for  "bm,st,tgt,tge"  are not specified (=1)
C
C nsect  - maximum number of sectors:  nsect = 6
C
C Maximun "idx" see dimensions in COMMON/sdadigi/
C but actual size (i.e. number of hits) is defined as (idx = 1, xx_ndig)
C
C
      INTEGER bm_digi,bm_ndig, st_digi,st_ndig, dc_digi,dc_ndig
     1       ,cc_digi,cc_ndig, sc_digi,sc_ndig, ec_digi,ec_ndig
     2       ,ec1_digi,ec1_ndig, tgt_digi,tgt_ndig, tge_digi,tge_ndig 
      REAL Ttrig
C
      COMMON/sdadigi/ bm_digi(3,7),bm_ndig, st_digi(3,6),st_ndig 
     1      ,dc_digi(2,192,36,6), dc_ndig(36,6)
     2      ,cc_digi(3,36,  1,6), cc_ndig( 1,6)
     3      ,sc_digi(5,48,  1,6), sc_ndig( 1,6)
     4      ,ec_digi(3,36,  6,6), ec_ndig( 6,6)
     5      ,ec1_digi(5,40, 4,6), ec1_ndig(4,6)
     6      ,tgt_digi(3,61),tgt_ndig, tge_digi(2,384),tge_ndig, Ttrig
C
      save  /sdadigi/
C
*/

typedef struct sdadigi
{
  int   bm_digi[7][3];
  int   bm_ndig;
  int   st_digi[6][3];
  int   st_ndig;
  int   dc_digi[6][36][192][2];
  int   dc_ndig[6][36];
  int   cc_digi[6][1][36][3];
  int   cc_ndig[6][1];
  int   sc_digi[6][1][48][5];
  int   sc_ndig[6][1];
  int   ec_digi[6][6][36][3];
  int   ec_ndig[6][6];
  int   ec1_digi[6][4][40][5];
  int   ec1_ndig[6][4];
  int   tgt_digi[61][3];
  int   tgt_ndig;
  int   tge_digi[384][2];
  int   tge_ndig;
  float Ttrig;
} SDAdigi;

SDAdigi sdadigi_;
E 1
