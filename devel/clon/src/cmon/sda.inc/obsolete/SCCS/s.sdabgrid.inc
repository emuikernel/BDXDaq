h51825
s 00000/00000/00000
d R 1.2 01/11/19 19:01:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/obsolete/sdabgrid.inc
e
s 00033/00000/00000
d D 1.1 01/11/19 19:01:20 boiarino 1 0
c date and time created 01/11/19 19:01:20 by boiarino
e
u
U
f e 0
t
T
I 1
C Magnetic field Table calculated for toroidal magnet with six coils
C (Table is calculated in a box which cover only a half of sector)
C                                 Bogdan Niczyporuk, March 1991
C
C    NX  ,NY  ,NZ   - Number of bins for each coordinate in DST Lab system
C  Xmin,Ymin,Zmin   - Defines edges of the box in DST Lab system
C    DX,  DY,  DZ   - Number of bins for each coordinate in DST Lab system
C            Cmax   - Maximum current [A] in a loop for big or mini Torus 
C           dtime   - Unix Time                    
C
C  Ngr = 3*NX*NY*NZ - Number of grids 
C
C For details see routines: "sda_minit.F"  or/and  "sda_torus.F"
C
C
      INTEGER NX, NY, NZ, Cmax, dtime, Ngr, Ng, Imax_t, Imax_m, Imax_p          
      PARAMETER (Ng = 1000000)
      PARAMETER (Imax_t = 3860, Imax_m = 8000, Imax_p = 124)
      REAL Xmin, Ymin, Zmin, DX, DY, DZ, Q(Ng)
C
      COMMON/sdabgrid/ NX,NY,NZ,Xmin,Ymin,Zmin,DX,DY,DZ,Cmax,dtime,Ngr,Q 
C
C                                
C And corresponding "COMMON/poltg/" for the Polarized Target Magnetic Field
C
      INTEGER MX,MY,MZ,Mgr, Cpmax,dptime
      REAL Qm(Ng), Xpmin,Ypmin,Zpmin,DDX,DDY,DDZ
C
      COMMON/poltg/MX,MY,MZ,Xpmin,Ypmin,Zpmin,DDX,DDY,DDZ,
     1                                Cpmax,dptime,Mgr,Qm
      save  /sdabgrid/
      save  /poltg/
C
E 1
