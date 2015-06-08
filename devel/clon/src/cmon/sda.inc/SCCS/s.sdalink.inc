h32040
s 00000/00000/00000
d R 1.2 01/11/19 19:00:46 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdalink.inc
e
s 00070/00000/00000
d D 1.1 01/11/19 19:00:45 boiarino 1 0
c date and time created 01/11/19 19:00:45 by boiarino
e
u
U
f e 0
t
T
I 1
C
C Dictionary (templates) needed to form the track segments in any SuperLayer
C and link the track segments to form the candidate trajectories in a Sector. 
C                                               Bogdan Niczyporuk, June 1992
C
C
C  *** Dictionary to build the track segments ***
C  ----------------------------------------------
C
C  --->   ((ipat(la,isg),la = 1,6),iseg = 1,npat)
C
C This array contains relative (with sespect to the layer 3) wire positions 
C in all remaining layers of the SuperLayer (SL).
C Thus, by definition "ipat(3,isg) = 0". In order to decide that a track
C segment (DATA) make sense one has to know only the wire number in layer 3.
C
C In case when SL consists of 3 or 4 layers then: 
C   
C  --->   ipat_flg3(isg) = 1  or  ipat_flg4(isg) = 1
C
C Thus those track segments must not be used for SL's with 3 or 4 layers.
C
C dtyp = 1 (TORU), = 2 (MINT), = 3 (TORU+MINT), = 4 (UNIF), = 5 (NONE)
C utime  - UnixTime when dictionary has been generated
C
C
C  *** Dictionary to build the track candidates (linked segments) ***
C  ------------------------------------------------------------------
C
C The concept adopted here is to use the absolute wire No of layer 3 in each SL
C with the road width of 2 cells for Reg.1 and 3 cells for Reg.2 and 3.
C Thus, the success of linking will depend on Geometry and B-field (DATA vs MC).
C
C  ---> nlink1(1,ilnk,iroadSL2)  - road for Axial SL of Reg.1
C  ---> nlink1(2,ilnk,iroadSL2)  - road for Axial SL of Reg.2
C  ---> nlink1(3,ilnk,iroadSL2)  - road for Axial SL of Reg.3
C  ---> nlink1(4,ilnk,iroadSL2)  - not used
C
C  ---> nlink2(1,ilnk,iroadSL2)  - road for Stereo SL of Reg.1
C  ---> nlink2(2,ilnk,iroadSL2)  - road for Stereo SL of Reg.2
C  ---> nlink2(3,ilnk,iroadSL2)  - road for Stereo SL of Reg.3
C  ---> nlink2(4,ilnk,iroadSL2)  - not used
C
C  ---> nlink3(1,ilnk,iroadSL2)  - No of simulated track for ilnk & iroadSL2
C  ---> nlink3(2,ilnk,iroadSL2)  - avarage mometum  <p>  for ilnk & iroadSL2
C
C Total number of linked trajectories is
C
C  ---> SUMi=1,ns2 { lnk(i) } 
C
      INTEGER max_npat, max_lnk, ns2
      PARAMETER (max_npat = 100, max_lnk = 1000, ns2 = 72)
C
      INTEGER npat, ipat(6,max_npat), dtyp, utime
     1       ,ipat_flg3(max_npat), ipat_flg4(max_npat)
C       
      INTEGER lnk(ns2) ,link1(max_lnk,ns2), link2(max_lnk,ns2)
     1                 ,link3(max_lnk,ns2)
C
      COMMON/sdalink/ npat, ipat, dtyp, utime, lnk
     1               ,link1, link2, link3, ipat_flg3,ipat_flg4
C
      BYTE      nlink1(4,max_lnk,ns2), nlink2(4,max_lnk,ns2)
      INTEGER*2 nlink3(2,max_lnk,ns2)
      EQUIVALENCE ( link1(1,1),nlink1(1,1,1) ) 
      EQUIVALENCE ( link2(1,1),nlink2(1,1,1) ) 
      EQUIVALENCE ( link3(1,1),nlink3(1,1,1) ) 
             
      save /sdalink/
C
E 1
