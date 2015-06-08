h48579
s 00008/00008/01315
d D 1.18 08/03/31 21:04:42 boiarino 19 18
c fix target hist scale
c 
e
s 00035/00036/01288
d D 1.17 07/12/01 17:15:01 fklein 18 17
c add histogram subtraction
c 
e
s 00077/00000/01247
d D 1.16 07/12/01 11:29:21 fklein 17 16
c frost helicity hists
c 
e
s 00016/00016/01231
d D 1.15 07/11/04 14:35:22 boiarino 16 15
c *** empty log message ***
e
s 00001/00001/01246
d D 1.14 07/11/04 10:39:19 boiarino 15 14
c *** empty log message ***
e
s 00003/00000/01244
d D 1.13 07/11/04 09:48:31 boiarino 14 13
c *** empty log message ***
e
s 00009/00001/01235
d D 1.12 07/11/04 09:22:30 boiarino 13 12
c *** empty log message ***
e
s 00097/00100/01139
d D 1.11 07/11/03 14:11:23 boiarino 12 11
c *** empty log message ***
e
s 00013/00013/01226
d D 1.10 07/11/03 11:46:55 boiarino 11 10
c *** empty log message ***
e
s 00002/00002/01237
d D 1.9 07/11/02 20:29:31 fklein 10 9
c *** empty log message ***
e
s 00118/00040/01121
d D 1.8 07/11/02 18:04:40 boiarino 9 8
c *** empty log message ***
e
s 00010/00002/01151
d D 1.7 04/02/03 20:47:21 boiarino 8 7
c *** empty log message ***
e
s 00211/00054/00942
d D 1.6 03/12/16 22:19:23 boiarino 7 6
c y
e
s 00207/00012/00789
d D 1.5 03/04/17 16:54:23 boiarino 6 5
c *** empty log message ***
e
s 00020/00039/00781
d D 1.4 02/02/13 18:41:47 boiarino 5 4
c do not ci last time ...
c 
e
s 00191/00185/00629
d D 1.3 01/12/19 14:59:25 boiarino 4 3
c dchbook_(UThistf *histf) ( was *ana)
c 
e
s 00004/00003/00810
d D 1.2 01/12/09 23:18:42 boiarino 3 1
c add HIST 19 (prompt statistic)
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dchist.c
e
s 00813/00000/00000
d D 1.1 01/11/19 19:05:58 boiarino 1 0
c date and time created 01/11/19 19:05:58 by boiarino
e
u
U
f e 0
t
T
I 1

/* dchist.c - HBOOK-based histogram interface */
/*

Generic histos (all tracks):
11 - chi square
12 - momentum
13 - theta
 131-136 - theta vs momentum
14 - phi
15 - target(Z)
 151-156 - target(Z) per sector
 157 - target(Z) walls
I 14
 158 - target(Z) long range
E 14
16 - target(Y)
17 - target(X)
D 3
18 - statistic
19 - 
E 3
I 3
18 - statistic (total)
19 - statistic (prompt)
E 3

Electron histos:
21 - Q**2
 211-216 - Q**2 per sector
22 - W
 221-226 - W per sector
 227     - Q**2 vs W
 228-229 - W for helisity/helisity-bar

I 6
Special histos:
31 - beam polarization monitoring

I 17
261-276 helicity bit from TGBI
361-376 helicity bit from RC26
261-266 pi+n (hel+,-) for butanol,carbon,CH2 targets Eg=0.6-0.9GeV
271-276 pi+n (hel+,-) for butanol,carbon,CH2 targets Eg=0.9-1.2GeV
361-366 pi+n (hel+,-) for butanol,carbon,CH2 targets Eg=0.6-0.9GeV
371-376 pi+n (hel+,-) for butanol,carbon,CH2 targets Eg=0.6-0.9GeV

E 17
E 6
Occupancy plots (all tracks):
901-906 - DC region 1
911-916 - DC region 2
921-926 - DC region 3
931-936 - CC left
941-946 - CC right
951-956 - SC left
961-966 - SC right
971-976 - EC U
981-986 - EC V
991-996 - EC W
I 7
997     - IC
E 7

Residuals:
.....

*/

#include <stdio.h>
#include <math.h>
I 6

E 6
#include "dclib.h"
I 6
#include "uthbook.h"
E 6
#include "sdakeys.h"
SDAkeys sdakeys_;

#define vflt   29.97925
#define aMp    0.938272
#define aMk    0.493677
#define aMpi   0.140000
#define aMe    0.000511
#define raddeg 57.2957795130823209

#define ABS(x) ((x) < 0 ? -(x) : (x))
I 6
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
E 6

D 6

E 6
/*********************/
/* create histograms */
/*********************/

void
D 4
dchbook_(TRevent *ana)
E 4
I 4
dchbook_(UThistf *histf)
E 4
{
D 4
  dchbook(ana);
E 4
I 4
  dchbook(histf);
E 4
}

void
D 4
dchbook(TRevent *ana)
E 4
I 4
dchbook(UThistf *histf)
E 4
{
I 4
D 7
  int i;

E 7
I 7
  int i,j;
  float x,y,step=0.133;
E 7
E 4
  printf("dchbook reached\n"); fflush(stdout);
D 4
printf("&histf[0].nbinx=0x%08x\n",&ana->histf[0].nbinx);
E 4
I 4
  printf("&histf[0].nbinx=0x%08x\n",&histf[0].nbinx);
E 4

I 4
  /* cleanup all 'nbinx' to avoid complains */
  for(i=0; i<NHIST; i++) {histf[i].nbinx = 0; histf[i].nbiny = 0;}

E 4
  /* general histos */

D 4
  uthbook1(ana->histf,  11,"CHI square itrk0 ", 40,  0.0, 40.0);
  uthbook1(ana->histf,  12,"P (GeV) itrk0 ",120,  0.0,  6.0);
  uthbook1(ana->histf,  13,"THETA (deg) itrk0 ", 80,  0.0, 80.0);
  uthbook2(ana->histf, 131,"SEC1 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(ana->histf, 132,"SEC2 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(ana->histf, 133,"SEC3 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(ana->histf, 134,"SEC4 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(ana->histf, 135,"SEC5 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(ana->histf, 136,"SEC6 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook1(ana->histf,  14,"PHI(deg) itrk0 ",180,  0.0,360.0);
  uthbook1(ana->histf,  15,"Zv (cm) itrk0 ",        250, -25.0, 25.0);
  uthbook1(ana->histf, 151,"Zv (cm) itrk0 (sec 1)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 152,"Zv (cm) itrk0 (sec 2)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 153,"Zv (cm) itrk0 (sec 3)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 154,"Zv (cm) itrk0 (sec 4)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 155,"Zv (cm) itrk0 (sec 5)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 156,"Zv (cm) itrk0 (sec 6)", 250, -25.0, 25.0);
  uthbook1(ana->histf, 157,"Zv (cm) walls ",        250, -25.0, 25.0);
  uthbook1(ana->histf,  16,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(ana->histf,  17,"Xv (cm)  itrk0 ", 40,-10.0, 10.0);
D 3
  uthbook1(ana->histf,  18,"TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
E 3
I 3
  uthbook1(ana->histf,  18,"TOTAL:  TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(ana->histf,  19,"PROMPT: TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
E 4
I 4
  uthbook1(histf,  11,"CHI square itrk0 ", 40,  0.0, 40.0);
D 16
  uthbook1(histf,  12,"P (GeV) itrk0 ",120,  0.0,  6.0);
  uthbook1(histf,  13,"THETA (deg) itrk0 ", 80,  0.0, 80.0);
  uthbook2(histf, 131,"SEC1 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(histf, 132,"SEC2 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(histf, 133,"SEC3 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(histf, 134,"SEC4 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(histf, 135,"SEC5 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
  uthbook2(histf, 136,"SEC6 THETA vs P", 60, 0.0, 6.0, 40, 0.0, 80.0);
E 16
I 16
  uthbook1(histf,  12,"P (GeV) itrk0 ",120,  0.0,  3.0);
  uthbook1(histf,  13,"THETA (deg) itrk0 ", 140,  0.0, 140.0);
  uthbook2(histf, 131,"SEC1 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
  uthbook2(histf, 132,"SEC2 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
  uthbook2(histf, 133,"SEC3 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
  uthbook2(histf, 134,"SEC4 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
  uthbook2(histf, 135,"SEC5 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
  uthbook2(histf, 136,"SEC6 THETA vs P", 60, 0.0, 3.0, 40, 0.0, 140.0);
E 16
  uthbook1(histf,  14,"PHI(deg) itrk0 ",180,  0.0,360.0);
D 6
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        250, -25.0, 25.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 250, -25.0, 25.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 250, -25.0, 25.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 250, -25.0, 25.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 250, -25.0, 25.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 250, -25.0, 25.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 250, -25.0, 25.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        250, -25.0, 25.0);
E 6
I 6

I 7
// Ivan's Histograms
    uthbook1(histf, 1020,"P(+)",100,  0., 5.);
    uthbook1(histf, 1021,"Cos(+)",100,  0., 1.);
    uthbook1(histf, 1022,"Anlge(+)",100,  0., 180.);
    uthbook1(histf, 1023,"P(e)",100,  0., 6.);
    uthbook1(histf, 1024,"Angle(e)",100,  0., 180.);
    uthbook1(histf, 1025,"Cluster energy",100,  0., 4000.);
    uthbook1(histf, 1026,"Cluster energy",100,  0., 4000.);
    uthbook1(histf, 1027,"Cluster energy/E(e)",100,  0., 2000.);
    uthbook1(histf, 1028,"Cluster energy/E(e)",100,  0., 2000.);
    uthbook2(histf, 1029,"Cluster energy vs E(e)",50,0.,4000.,50,0.,5.0);     
    uthbook2(histf, 1030,"Cluster energy vs E(e)",50,0.,4000.,50,0.,5.0);     
    uthbook1(histf, 1031,"Phi",360,  0., 360.);  
    uthbook1(histf, 1032,"Phi",360,  0., 360.);  
    uthbook2(histf, 1033,"Phi2 vs phi",360,  0., 360.,360,  0., 360.);
    uthbook2(histf, 1034,"Phi2 vs phi",360,  0., 360.,360,  0., 360.);
    uthbook1(histf, 1040,"Cluster energy/E(e)",100,  0., 2000.);
   
    uthbook2(histf, 1041,"Cluster energy vs E(e)",50,0.,4000.,50,0.,5.0);     

    uthbook2(histf, 1042,"Occupancy plot, everything",22,0.,22.,22,0.,22.);
    uthbook2(histf, 1043,"Occupancy plot, big cluster",22,0.,22.,22,0.,22.);

// Additional Set of histograms 
   for(i=0; i<253; i++)
   {
     uthbook1(histf,1100+i,"Cluster energy (all) (MeV)",550,500.,6000.);
     uthbook1(histf,1400+i,"Cluster energy (>1cristal) (MeV)",550,500.,6000.);
     uthbook1(histf,1700+i,"Cluster energy (MIP) (MeV)",200,0.,600.);
   }
E 7
D 9
  /*
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        100, -5.0, 5.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 100, -5.0, 5.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 100, -5.0, 5.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 100, -5.0, 5.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 100, -5.0, 5.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 100, -5.0, 5.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 100, -5.0, 5.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        100, -5.0, 5.0);
  */
E 9
  
D 7
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        250, -50.0, 25.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 250, -50.0, 25.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 250, -50.0, 25.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 250, -50.0, 25.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 250, -50.0, 25.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 250, -50.0, 25.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 250, -50.0, 25.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        250, -50.0, 25.0);
E 7
I 7
D 9
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        320, -60.0, 20.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 320, -60.0, 20.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 320, -60.0, 20.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 320, -60.0, 20.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 320, -60.0, 20.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 320, -60.0, 20.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 320, -60.0, 20.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        320, -60.0, 20.0);
E 9
I 9
D 12
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        100, -600.0, 0.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 100, -600.0, 0.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 100, -600.0, 0.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 100, -600.0, 0.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 100, -600.0, 0.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 100, -600.0, 0.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 100, -600.0, 0.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        100, -600.0, 0.0);
E 12
I 12
D 15
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        100, -100.0, 100.0);
E 15
I 15
D 19
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        200,  -50.0,  50.0);
E 15
D 16
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 100, -100.0, 100.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 100, -100.0, 100.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 100, -100.0, 100.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 100, -100.0, 100.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 100, -100.0, 100.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 100, -100.0, 100.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        100, -100.0, 100.0);
E 16
I 16
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 200,  -50.0,  50.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 200,  -50.0,  50.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 200,  -50.0,  50.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 200,  -50.0,  50.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 200,  -50.0,  50.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 200,  -50.0,  50.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        200,  -50.0,  50.0);
E 19
I 19
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        200,  -150.0,  50.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 200,  -150.0,  50.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 200,  -150.0,  50.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 200,  -150.0,  50.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 200,  -150.0,  50.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 200,  -150.0,  50.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 200,  -150.0,  50.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        200,  -150.0,  50.0);
E 19
E 16
I 14
  uthbook1(histf, 158,"Zv (cm) long range ",   100, -1000.0, 1000.0);
E 14
E 12
E 9
E 7
  
I 9
  /*
  uthbook1(histf,  15,"Zv (cm) itrk0 ",        200, -200.0, 0.0);
  uthbook1(histf, 151,"Zv (cm) itrk0 (sec 1)", 200, -200.0, 0.0);
  uthbook1(histf, 152,"Zv (cm) itrk0 (sec 2)", 200, -200.0, 0.0);
  uthbook1(histf, 153,"Zv (cm) itrk0 (sec 3)", 200, -200.0, 0.0);
  uthbook1(histf, 154,"Zv (cm) itrk0 (sec 4)", 200, -200.0, 0.0);
  uthbook1(histf, 155,"Zv (cm) itrk0 (sec 5)", 200, -200.0, 0.0);
  uthbook1(histf, 156,"Zv (cm) itrk0 (sec 6)", 200, -200.0, 0.0);
  uthbook1(histf, 157,"Zv (cm) walls ",        200, -200.0, 0.0);
  */
E 9

E 6
  uthbook1(histf,  16,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
I 9
  uthbook1(histf, 111,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf, 112,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf, 113,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf, 114,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf, 115,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf, 116,"Yv (cm)  itrk0 ", 40,-10.0, 10.0);

E 9
  uthbook1(histf,  17,"Xv (cm)  itrk0 ", 40,-10.0, 10.0);
  uthbook1(histf,  18,"TOTAL:  TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(histf, 181,"TOTAL:  TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(histf, 182,"TOTAL:  TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(histf,  19,"PROMPT: TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(histf, 191,"PROMPT: TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
  uthbook1(histf, 192,"PROMPT: TO L3 PR HB PI DC TB AN", 10, 0.0, 10.0);
E 4
E 3

D 4
  uthbook2(ana->histf, 901,"SEC1 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(ana->histf, 902,"SEC2 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(ana->histf, 903,"SEC3 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(ana->histf, 904,"SEC4 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(ana->histf, 905,"SEC5 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(ana->histf, 906,"SEC6 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
E 4
I 4
  uthbook2(histf, 901,"SEC1 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(histf, 902,"SEC2 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(histf, 903,"SEC3 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(histf, 904,"SEC4 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(histf, 905,"SEC5 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
  uthbook2(histf, 906,"SEC6 REG1", 128, 0.0, 128.0, 12, 0.0, 12.0);
E 4

D 4
  uthbook2(ana->histf, 911,"SEC1 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(ana->histf, 912,"SEC2 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(ana->histf, 913,"SEC3 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(ana->histf, 914,"SEC4 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(ana->histf, 915,"SEC5 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(ana->histf, 916,"SEC6 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
E 4
I 4
  uthbook2(histf, 911,"SEC1 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(histf, 912,"SEC2 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(histf, 913,"SEC3 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(histf, 914,"SEC4 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(histf, 915,"SEC5 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
  uthbook2(histf, 916,"SEC6 REG2", 192, 0.0, 192.0, 12, 12.0, 24.0);
E 4

D 4
  uthbook2(ana->histf, 921,"SEC1 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(ana->histf, 922,"SEC2 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(ana->histf, 923,"SEC3 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(ana->histf, 924,"SEC4 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(ana->histf, 925,"SEC5 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(ana->histf, 926,"SEC6 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
E 4
I 4
  uthbook2(histf, 921,"SEC1 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(histf, 922,"SEC2 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(histf, 923,"SEC3 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(histf, 924,"SEC4 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(histf, 925,"SEC5 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
  uthbook2(histf, 926,"SEC6 REG3", 192, 0.0, 192.0, 12, 24.0, 36.0);
E 4

D 4
  uthbook1(ana->histf, 931,"SEC1 CC LEFT", 18,  0., 18.);
  uthbook1(ana->histf, 932,"SEC2 CC LEFT", 18,  0., 18.);
  uthbook1(ana->histf, 933,"SEC3 CC LEFT", 18,  0., 18.);
  uthbook1(ana->histf, 934,"SEC4 CC LEFT", 18,  0., 18.);
  uthbook1(ana->histf, 935,"SEC5 CC LEFT", 18,  0., 18.);
  uthbook1(ana->histf, 936,"SEC6 CC LEFT", 18,  0., 18.);
E 4
I 4
  uthbook1(histf, 931,"SEC1 CC LEFT", 18,  0., 18.);
  uthbook1(histf, 932,"SEC2 CC LEFT", 18,  0., 18.);
  uthbook1(histf, 933,"SEC3 CC LEFT", 18,  0., 18.);
  uthbook1(histf, 934,"SEC4 CC LEFT", 18,  0., 18.);
  uthbook1(histf, 935,"SEC5 CC LEFT", 18,  0., 18.);
  uthbook1(histf, 936,"SEC6 CC LEFT", 18,  0., 18.);
E 4

D 4
  uthbook1(ana->histf, 941,"SEC1 CC RIGHT", 18,  0., 18.);
  uthbook1(ana->histf, 942,"SEC2 CC RIGHT", 18,  0., 18.);
  uthbook1(ana->histf, 943,"SEC3 CC RIGHT", 18,  0., 18.);
  uthbook1(ana->histf, 944,"SEC4 CC RIGHT", 18,  0., 18.);
  uthbook1(ana->histf, 945,"SEC5 CC RIGHT", 18,  0., 18.);
  uthbook1(ana->histf, 946,"SEC6 CC RIGHT", 18,  0., 18.);
E 4
I 4
  uthbook1(histf, 941,"SEC1 CC RIGHT", 18,  0., 18.);
  uthbook1(histf, 942,"SEC2 CC RIGHT", 18,  0., 18.);
  uthbook1(histf, 943,"SEC3 CC RIGHT", 18,  0., 18.);
  uthbook1(histf, 944,"SEC4 CC RIGHT", 18,  0., 18.);
  uthbook1(histf, 945,"SEC5 CC RIGHT", 18,  0., 18.);
  uthbook1(histf, 946,"SEC6 CC RIGHT", 18,  0., 18.);
E 4

D 4
  uthbook1(ana->histf, 951,"SEC1 SC LEFT", 48,  0., 48.);
  uthbook1(ana->histf, 952,"SEC2 SC LEFT", 48,  0., 48.);
  uthbook1(ana->histf, 953,"SEC3 SC LEFT", 48,  0., 48.);
  uthbook1(ana->histf, 954,"SEC4 SC LEFT", 48,  0., 48.);
  uthbook1(ana->histf, 955,"SEC5 SC LEFT", 48,  0., 48.);
  uthbook1(ana->histf, 956,"SEC6 SC LEFT", 48,  0., 48.);
E 4
I 4
D 11
  uthbook1(histf, 951,"SEC1 SC LEFT", 48,  0., 48.);
  uthbook1(histf, 952,"SEC2 SC LEFT", 48,  0., 48.);
  uthbook1(histf, 953,"SEC3 SC LEFT", 48,  0., 48.);
  uthbook1(histf, 954,"SEC4 SC LEFT", 48,  0., 48.);
  uthbook1(histf, 955,"SEC5 SC LEFT", 48,  0., 48.);
  uthbook1(histf, 956,"SEC6 SC LEFT", 48,  0., 48.);
E 11
I 11
  uthbook1(histf, 951,"SEC1 SC LEFT", 57,  0., 57.);
  uthbook1(histf, 952,"SEC2 SC LEFT", 57,  0., 57.);
  uthbook1(histf, 953,"SEC3 SC LEFT", 57,  0., 57.);
  uthbook1(histf, 954,"SEC4 SC LEFT", 57,  0., 57.);
  uthbook1(histf, 955,"SEC5 SC LEFT", 57,  0., 57.);
  uthbook1(histf, 956,"SEC6 SC LEFT", 57,  0., 57.);
E 11
E 4

D 4
  uthbook1(ana->histf, 961,"SEC1 SC RIGHT", 48,  0., 48.);
  uthbook1(ana->histf, 962,"SEC2 SC RIGHT", 48,  0., 48.);
  uthbook1(ana->histf, 963,"SEC3 SC RIGHT", 48,  0., 48.);
  uthbook1(ana->histf, 964,"SEC4 SC RIGHT", 48,  0., 48.);
  uthbook1(ana->histf, 965,"SEC5 SC RIGHT", 48,  0., 48.);
  uthbook1(ana->histf, 966,"SEC6 SC RIGHT", 48,  0., 48.);
E 4
I 4
D 11
  uthbook1(histf, 961,"SEC1 SC RIGHT", 48,  0., 48.);
  uthbook1(histf, 962,"SEC2 SC RIGHT", 48,  0., 48.);
  uthbook1(histf, 963,"SEC3 SC RIGHT", 48,  0., 48.);
  uthbook1(histf, 964,"SEC4 SC RIGHT", 48,  0., 48.);
  uthbook1(histf, 965,"SEC5 SC RIGHT", 48,  0., 48.);
  uthbook1(histf, 966,"SEC6 SC RIGHT", 48,  0., 48.);
E 11
I 11
  uthbook1(histf, 961,"SEC1 SC RIGHT", 57,  0., 57.);
  uthbook1(histf, 962,"SEC2 SC RIGHT", 57,  0., 57.);
  uthbook1(histf, 963,"SEC3 SC RIGHT", 57,  0., 57.);
  uthbook1(histf, 964,"SEC4 SC RIGHT", 57,  0., 57.);
  uthbook1(histf, 965,"SEC5 SC RIGHT", 57,  0., 57.);
  uthbook1(histf, 966,"SEC6 SC RIGHT", 57,  0., 57.);
E 11
E 4

D 4
  uthbook1(ana->histf, 971,"SEC1 EC U", 36,  0., 36.);
  uthbook1(ana->histf, 972,"SEC2 EC U", 36,  0., 36.);
  uthbook1(ana->histf, 973,"SEC3 EC U", 36,  0., 36.);
  uthbook1(ana->histf, 974,"SEC4 EC U", 36,  0., 36.);
  uthbook1(ana->histf, 975,"SEC5 EC U", 36,  0., 36.);
  uthbook1(ana->histf, 976,"SEC6 EC U", 36,  0., 36.);
E 4
I 4
  uthbook1(histf, 971,"SEC1 EC U", 36,  0., 36.);
  uthbook1(histf, 972,"SEC2 EC U", 36,  0., 36.);
  uthbook1(histf, 973,"SEC3 EC U", 36,  0., 36.);
  uthbook1(histf, 974,"SEC4 EC U", 36,  0., 36.);
  uthbook1(histf, 975,"SEC5 EC U", 36,  0., 36.);
  uthbook1(histf, 976,"SEC6 EC U", 36,  0., 36.);
E 4

D 4
  uthbook1(ana->histf, 981,"SEC1 EC V", 36,  0., 36.);
  uthbook1(ana->histf, 982,"SEC2 EC V", 36,  0., 36.);
  uthbook1(ana->histf, 983,"SEC3 EC V", 36,  0., 36.);
  uthbook1(ana->histf, 984,"SEC4 EC V", 36,  0., 36.);
  uthbook1(ana->histf, 985,"SEC5 EC V", 36,  0., 36.);
  uthbook1(ana->histf, 986,"SEC6 EC V", 36,  0., 36.);
E 4
I 4
  uthbook1(histf, 981,"SEC1 EC V", 36,  0., 36.);
  uthbook1(histf, 982,"SEC2 EC V", 36,  0., 36.);
  uthbook1(histf, 983,"SEC3 EC V", 36,  0., 36.);
  uthbook1(histf, 984,"SEC4 EC V", 36,  0., 36.);
  uthbook1(histf, 985,"SEC5 EC V", 36,  0., 36.);
  uthbook1(histf, 986,"SEC6 EC V", 36,  0., 36.);
E 4

D 4
  uthbook1(ana->histf, 991,"SEC1 EC W", 36,  0., 36.);
  uthbook1(ana->histf, 992,"SEC2 EC W", 36,  0., 36.);
  uthbook1(ana->histf, 993,"SEC3 EC W", 36,  0., 36.);
  uthbook1(ana->histf, 994,"SEC4 EC W", 36,  0., 36.);
  uthbook1(ana->histf, 995,"SEC5 EC W", 36,  0., 36.);
  uthbook1(ana->histf, 996,"SEC6 EC W", 36,  0., 36.);
E 4
I 4
  uthbook1(histf, 991,"SEC1 EC W", 36,  0., 36.);
  uthbook1(histf, 992,"SEC2 EC W", 36,  0., 36.);
  uthbook1(histf, 993,"SEC3 EC W", 36,  0., 36.);
  uthbook1(histf, 994,"SEC4 EC W", 36,  0., 36.);
  uthbook1(histf, 995,"SEC5 EC W", 36,  0., 36.);
  uthbook1(histf, 996,"SEC6 EC W", 36,  0., 36.);
E 4

I 7
  uthbook1(histf, 997,"IC", 8192,  0., 8191.);

E 7
D 4
  uthbook1(ana->histf,  91,"sc1",100,  0., 100.);
  uthbook1(ana->histf,  92,"sc2",100,  0., 100.);
  uthbook1(ana->histf,  93,"sc3",100,  0., 100.);
  uthbook1(ana->histf,  94,"sc4",100,  0., 100.);
  uthbook1(ana->histf,  95,"sc5",100,  0., 100.);
E 4
I 4
  uthbook1(histf,  91,"sc1",100,  0., 100.);
  uthbook1(histf,  92,"sc2",100,  0., 100.);
  uthbook1(histf,  93,"sc3",100,  0., 100.);
  uthbook1(histf,  94,"sc4",100,  0., 100.);
  uthbook1(histf,  95,"sc5",100,  0., 100.);
E 4

I 9

E 9
  if(sdakeys_.zbeam[0] < -0.1) /* electroproduction */
  {
D 4
    uthbook1(ana->histf,  21,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 211,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 212,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 213,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 214,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 215,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf, 216,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(ana->histf,  22,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 221,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 222,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 223,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 224,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 225,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 226,"W itrk0  ", 200, 0.00, 4.00);
    uthbook2(ana->histf, 227,"Qsquare vs W", 80, 0.0, 4.0, 120, 0.0, 6.0);
    uthbook1(ana->histf, 228,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf, 229,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(ana->histf,  23,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(ana->histf, 231,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(ana->histf, 232,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(ana->histf,  24,"MMep?obs!  inel", 100, -1.00, 2.00);
    uthbook1(ana->histf, 241,"MM2ep?obs!  inel", 100, -1.00, 2.00);
    uthbook1(ana->histf,  25,"MMeK^+!?obs!", 110, 0.90, 2.00);
    uthbook1(ana->histf,  26,"MMe[p]^+!?obs!", 200, 0.40, 2.40);
    uthbook2(ana->histf,  27,"Beta vs p  h^+!", 50,0.,2.0, 50,0.,2.0);
E 4
I 4
    uthbook1(histf,  21,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 211,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 212,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 213,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 214,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 215,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf, 216,"Qsquare itrk0", 300, 0.00, 6.00);
    uthbook1(histf,  22,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 221,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 222,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 223,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 224,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 225,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 226,"W itrk0  ", 200, 0.00, 4.00);
    uthbook2(histf, 227,"Qsquare vs W", 80, 0.0, 4.0, 120, 0.0, 6.0);
D 9
    uthbook1(histf, 228,"W itrk0  ", 200, 0.00, 4.00);
    uthbook1(histf, 229,"W itrk0  ", 200, 0.00, 4.00);
E 9
I 9

    uthbook1(histf, 228,"W itrk0 L", 200, 0.00, 4.00);
    uthbook1(histf, 229,"W itrk0 R", 200, 0.00, 4.00);
    uthbook1(histf, 230,"W itrk0 D", 200, 0.00, 4.00);

    uthbook1(histf, 238,"W itrk0 L", 200, 0.00, 4.00);
    uthbook1(histf, 239,"W itrk0 R", 200, 0.00, 4.00);
    uthbook1(histf, 240,"W itrk0 D", 200, 0.00, 4.00);

    uthbook1(histf, 261,"W itrk0 S", 200, 0.00, 4.00);
    uthbook1(histf, 262,"W itrk0 S", 200, 0.00, 4.00);
    uthbook1(histf, 271,"W itrk0 N", 200, 0.00, 4.00);
    uthbook1(histf, 272,"W itrk0 N", 200, 0.00, 4.00);

E 9
    uthbook1(histf,  23,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(histf, 231,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(histf, 232,"MMep^2!?obs! el",  50, -0.025, 0.025);
    uthbook1(histf,  24,"MMep?obs!  inel", 100, -1.00, 2.00);
    uthbook1(histf, 241,"MM2ep?obs!  inel", 100, -1.00, 2.00);
    uthbook1(histf,  25,"MMeK^+!?obs!", 110, 0.90, 2.00);
D 7
    uthbook1(histf,  26,"MMe[p]^+!?obs!", 200, 0.40, 2.40);
E 7
I 7
    uthbook1(histf,  26,"MMe[p]^+!?obs!", 200, -0.1, 6.0);
E 7
    uthbook2(histf,  27,"Beta vs p  h^+!", 50,0.,2.0, 50,0.,2.0);
E 4

D 4
    uthbook2(ana->histf, 250,"RASTER YvsX",100,0.0,8200.0,100,0.0,8200.0);
    uthbook1(ana->histf, 251,"RASTER X",100, 0.0, 8200.0);
    uthbook1(ana->histf, 252,"RASTER Y",100, 0.0, 8200.0);
E 4
I 4
D 9
    uthbook2(histf, 250,"RASTER YvsX",100,0.0,8200.0,100,0.0,8200.0);
    uthbook1(histf, 251,"RASTER X",100, 0.0, 8200.0);
    uthbook1(histf, 252,"RASTER Y",100, 0.0, 8200.0);
E 9
I 9
    uthbook2(histf, 250,"RASTER YvsX",164,0.0,8200.0,164,0.0,8200.0);
    uthbook1(histf, 251,"RASTER X",164,0.0, 8200.0);
    uthbook1(histf, 252,"RASTER Y",164,0.0, 8200.0);
E 9
I 6

    uthbook1(histf,  31,"PHI-cm e-piplus", 180, -180., 180.);
E 6
E 4
  }
  else                         /* photoproduction */
  {
D 4
    uthbook1(ana->histf,  20,"Photon energy  ",300, 0.0, 6.0);
    uthbook1(ana->histf,  21,"T?tag! itrk0   ",200, -25.0, 25.0);
    uthbook1(ana->histf,  22,"T?tof! itrk0   ",200, -25.0, 25.0);
    uthbook1(ana->histf,  23,"W?obs! itrk0   ",200,   1.0,  3.0);
    uthbook1(ana->histf,  24,"MM[g]prot      ",150,  0.00, 3.00);
    uthbook1(ana->histf,  42,"MM[g]prot(pi+) ",150,  0.00, 1.50);
    uthbook1(ana->histf, 241,"MM2ep?obs! inel",100, -1.00, 2.00);
    uthbook1(ana->histf,  25,"MM[g]K^+!?obs! ",110,  0.90, 2.00);
    uthbook1(ana->histf,  26,"MM[g]pi+       ",250,  0.50, 5.50);
    uthbook2(ana->histf,  27,"Beta vs p  h^+!", 50,0.,2.0, 50,0.,2.0);
E 4
I 4
    uthbook1(histf,  20,"Photon energy  ",300, 0.0, 6.0);
    uthbook1(histf,  21,"T?tag! itrk0   ",200, -25.0, 25.0);
D 16
    uthbook1(histf,  22,"T?tof! itrk0   ",200, -25.0, 25.0);
E 16
I 16
    uthbook1(histf,  22,"T?tof! itrk0   ",200, 0.0, 50.0);
E 16
    uthbook1(histf,  23,"W?obs! itrk0   ",200,   1.0,  3.0);
    uthbook1(histf,  24,"MM[g]prot      ",150,  0.00, 3.00);
    uthbook1(histf,  42,"MM[g]prot(pi+) ",150,  0.00, 1.50);
    uthbook1(histf, 241,"MM2ep?obs! inel",100, -1.00, 2.00);
    uthbook1(histf,  25,"MM[g]K^+!?obs! ",110,  0.90, 2.00);
    uthbook1(histf,  26,"MM[g]pi+       ",250,  0.50, 5.50);
    uthbook2(histf,  27,"Beta vs p  h^+!", 50,0.,2.0, 50,0.,2.0);
E 4

I 17
    /*** FROST ***/
D 18
    /* use TGBI */
    uthbook1(histf, 261,"pi+n (h+) z-main E=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 262,"pi+n (h-) z-main E=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 263,"pi+n (h+) z-carbon E=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 264,"pi+n (h-) z-carbon E=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 265,"pi+n (h+) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 266,"pi+n (h-) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 271,"pi+n (h+) z-main E=0.9-1.2",  20, -1.0, 1.0);
    uthbook1(histf, 272,"pi+n (h-) z-main E=0.9-1.2",  20, -1.0, 1.0);
    uthbook1(histf, 273,"pi+n (h+) z-carbon E=0.9-1.2",20, -1.0, 1.0);
    uthbook1(histf, 274,"pi+n (h-) z-carbon E=0.9-1.2",20, -1.0, 1.0);
    uthbook1(histf, 275,"pi+n (h+) z-ch2 E=0.9-1.2",   20, -1.0, 1.0);
    uthbook1(histf, 276,"pi+n (h-) z-ch2 E=0.9-1.2",   20, -1.0, 1.0);
E 18
I 18
    /* asymmetry hists: (hel+)-(hel-))/(hel(+)+hel(-))=1-2*(hel-)/(sum)
    uthbook1(histf, 261,"Asym (pi+n) (butanol) Eg=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 262,"Asym (pi+n) (butanol) Eg=0.9-1.2",  20, -1.0, 1.0);
    uthbook1(histf, 263,"Asym (pi+n) (carbon) Eg=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 264,"Asym (pi+n) (carbon) Eg=0.9-1.2",20, -1.0, 1.0);
    uthbook1(histf, 265,"Asym (pi+n) (CH2) Eg=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 266,"Asym (pi+n) (CH2) Eg=0.9-1.2",   20, -1.0, 1.0);
E 18

    /* use RC26 */
D 18
    uthbook1(histf, 361,"pi+n (h+) z-main E=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 362,"pi+n (h-) z-main E=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 363,"pi+n (h+) z-carbon E=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 364,"pi+n (h-) z-carbon E=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 365,"pi+n (h+) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 366,"pi+n (h-) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 371,"pi+n (h+) z-main E=0.9-1.2",  20, -1.0, 1.0);
E 18
I 18
    uthbook1(histf, 361,"pi+n (sum) z-main E=0.6-0.9",  20, -1.0, 1.0);
    uthbook1(histf, 362,"pi+n (sum) z-main E=0.9-1.2",  20, -1.0, 1.0);
    uthbook1(histf, 363,"pi+n (sum) z-carbon E=0.6-0.9",20, -1.0, 1.0);
    uthbook1(histf, 364,"pi+n (sum) z-carbon E=0.9-1.2",20, -1.0, 1.0);
    uthbook1(histf, 365,"pi+n (sum) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
    uthbook1(histf, 366,"pi+n (sum) z-ch2 E=0.9-1.2",   20, -1.0, 1.0);
    uthbook1(histf, 371,"pi+n (h-) z-main E=0.6-0.9",  20, -1.0, 1.0);
E 18
    uthbook1(histf, 372,"pi+n (h-) z-main E=0.9-1.2",  20, -1.0, 1.0);
D 18
    uthbook1(histf, 373,"pi+n (h+) z-carbon E=0.9-1.2",20, -1.0, 1.0);
E 18
I 18
    uthbook1(histf, 373,"pi+n (h-) z-carbon E=0.6-0.9",20, -1.0, 1.0);
E 18
    uthbook1(histf, 374,"pi+n (h-) z-carbon E=0.9-1.2",20, -1.0, 1.0);
D 18
    uthbook1(histf, 375,"pi+n (h+) z-ch2 E=0.9-1.2",   20, -1.0, 1.0);
E 18
I 18
    uthbook1(histf, 375,"pi+n (h-) z-ch2 E=0.6-0.9",   20, -1.0, 1.0);
E 18
    uthbook1(histf, 376,"pi+n (h-) z-ch2 E=0.9-1.2",   20, -1.0, 1.0);

E 17
D 4
    uthbook1(ana->histf, 141,"Prot PHI(deg) coherent",180,0.0,360.0);
    uthbook1(ana->histf, 142,"Prot PHI(deg) noncoherent",180,0.0,360.0);
    uthbook1(ana->histf, 143,"Prot,pi+ PHI(deg) coherent",180,0.0,360.0);
    uthbook1(ana->histf, 144,"Prot,pi+ PHI(deg) noncoherent",180,0.0,360.0);
    uthbook1(ana->histf, 145,"pi+ PHI(deg) coherent",180,0.0,360.0);
    uthbook1(ana->histf, 146,"pi+ PHI(deg) noncoherent",180,0.0,360.0);
E 4
I 4
    uthbook1(histf, 141,"Prot PHI(deg) coherent",180,0.0,360.0);
    uthbook1(histf, 142,"Prot PHI(deg) noncoherent",180,0.0,360.0);
    uthbook1(histf, 143,"Prot,pi+ PHI(deg) coherent",180,0.0,360.0);
    uthbook1(histf, 144,"Prot,pi+ PHI(deg) noncoherent",180,0.0,360.0);
    uthbook1(histf, 145,"pi+ PHI(deg) coherent",180,0.0,360.0);
    uthbook1(histf, 146,"pi+ PHI(deg) noncoherent",180,0.0,360.0);
E 4

D 4
    uthbook1(ana->histf, 161,"slope L",100, 0., 0.1);
    uthbook1(ana->histf, 162,"slope R",100, 0., 0.1);
    uthbook1(ana->histf, 163,"peak L",100, 0., 100.);
    uthbook1(ana->histf, 164,"peak R",100, 0., 100.);
    uthbook1(ana->histf, 167,"raw 1 (T counters)",400, 10., 4010.);
    uthbook1(ana->histf, 169,"raw 2 (T counters)",400, 10., 4010.);
    uthbook1(ana->histf, 170,"xval1 -xval2 (T counters)",200, -10., 10.);
    uthbook1(ana->histf, 171,"xval1 (T counters)",       300, 20., 50.);
    uthbook1(ana->histf, 172,"xval2 (T counters)",       300, 20., 50.);
    uthbook1(ana->histf, 173,"xval average (T counters)",300, 20., 50.);
    uthbook1(ana->histf, 174,"xval1 (E counters)",       300, 20., 50.);
    uthbook1(ana->histf, 175,"xval1 (E counters)",400,  -4000., 4000.);
    uthbook1(ana->histf, 176,"rf[0]",100, 0., 100.);
    uthbook1(ana->histf, 177,"rf[1]",100, 0., 100.);
    uthbook1(ana->histf, 178,"rf[2]",100, 0., 100.);
    uthbook1(ana->histf, 179,"rf[0]-rf[1]",100, 30., 40.);
    uthbook1(ana->histf,  81,"Ntaghits",100,  0., 100.);
    uthbook1(ana->histf,  82,"chisquare",100,  0., 100.);
    uthbook1(ana->histf,  83,"x0",100,  0., 1000.);
    uthbook1(ana->histf,  84,"x0",100,  0., 1000.);
    uthbook1(ana->histf,  85,"x0",100,  0., 1000.);
    uthbook1(ana->histf,  86,"x0",100,  0., 1000.);
    uthbook1(ana->histf,  89,"x0",100,  0., 1000.);
    uthbook1(ana->histf,  87,"dif1",100,  -50., 50.);
    uthbook1(ana->histf,  88,"dif2",100,  -50., 50.);
    uthbook1(ana->histf, 181,"st1",100,  -100., 100.);
    uthbook1(ana->histf, 182,"st2",100,  -100., 100.);
    uthbook1(ana->histf, 183,"st3",100,  -100., 100.);
    uthbook1(ana->histf, 184,"st4",100,  -100., 100.);
    uthbook1(ana->histf, 185,"st5",100,  -100., 100.);
    uthbook1(ana->histf, 186,"st6",100,  -100., 100.);
    uthbook1(ana->histf, 191,"st1",100,  -100., 100.);
    uthbook1(ana->histf, 192,"st2",100,  -100., 100.);
    uthbook1(ana->histf, 193,"st3",100,  -100., 100.);
    uthbook1(ana->histf, 194,"st4",100,  -100., 100.);
    uthbook1(ana->histf, 195,"st5",100,  -100., 100.);
    uthbook1(ana->histf, 196,"st6",100,  -100., 100.);
    uthbook1(ana->histf, 187,"dif1",500,  -250., 250.);
    uthbook1(ana->histf, 188,"dif2",500,  -250., 250.);
    uthbook1(ana->histf, 201,"rf1",100,  -100., 100.);
    uthbook1(ana->histf, 202,"rf2",100,  -100., 100.);
    uthbook1(ana->histf, 211,"tof-st -1",100,  -100., 100.);
    uthbook1(ana->histf, 212,"tof-st -2",100,  -100., 100.);
    uthbook1(ana->histf, 213,"tof-st -3",100,  -100., 100.);
    uthbook1(ana->histf, 214,"tof-st -4",100,  -100., 100.);
    uthbook1(ana->histf, 215,"tof-st -5",100,  -100., 100.);
    uthbook1(ana->histf, 216,"tof-st -6",100,  -100., 100.);
E 4
I 4
    uthbook1(histf, 161,"slope L",100, 0., 0.1);
    uthbook1(histf, 162,"slope R",100, 0., 0.1);
    uthbook1(histf, 163,"peak L",100, 0., 100.);
    uthbook1(histf, 164,"peak R",100, 0., 100.);
D 9
    uthbook1(histf, 167,"raw 1 (T counters)",400, 10., 4010.);
    uthbook1(histf, 169,"raw 2 (T counters)",400, 10., 4010.);
    uthbook1(histf, 170,"xval1 -xval2 (T counters)",200, -10., 10.);
    uthbook1(histf, 171,"xval1 (T counters)",       300, 20., 50.);
    uthbook1(histf, 172,"xval2 (T counters)",       300, 20., 50.);
    uthbook1(histf, 173,"xval average (T counters)",300, 20., 50.);
    uthbook1(histf, 174,"xval1 (E counters)",       300, 20., 50.);
    uthbook1(histf, 175,"xval1 (E counters)",400,  -4000., 4000.);
E 9
I 9
    uthbook1(histf, 167,"T counters left (raw data)", 400,    10., 80010.);
    uthbook1(histf, 169,"T counters right (raw data)",400,    10., 80010.);
    uthbook1(histf, 170,"T counters left (ns)",       600, -5000.,  5000.);
    uthbook1(histf, 171,"T counters right (ns)",      600, -5000.,  5000.);
    uthbook1(histf, 172,"T counters left-right (ns)", 200,  -100.,   100.);
    uthbook1(histf, 173,"xval average (T counters)",  500,     0., 50000.);
    uthbook1(histf, 174,"E counters (raw data)",      400,    10.,  2000.);
    uthbook1(histf, 175,"E counters (ns)",            400,  -500.,   500.);
E 9
    uthbook1(histf, 176,"rf[0]",100, 0., 100.);
    uthbook1(histf, 177,"rf[1]",100, 0., 100.);
    uthbook1(histf, 178,"rf[2]",100, 0., 100.);
    uthbook1(histf, 179,"rf[0]-rf[1]",100, 30., 40.);
    uthbook1(histf,  81,"Ntaghits",100,  0., 100.);
    uthbook1(histf,  82,"chisquare",100,  0., 100.);
    uthbook1(histf,  83,"x0",100,  0., 1000.);
    uthbook1(histf,  84,"x0",100,  0., 1000.);
    uthbook1(histf,  85,"x0",100,  0., 1000.);
    uthbook1(histf,  86,"x0",100,  0., 1000.);
    uthbook1(histf,  89,"x0",100,  0., 1000.);
    uthbook1(histf,  87,"dif1",100,  -50., 50.);
    uthbook1(histf,  88,"dif2",100,  -50., 50.);
    uthbook1(histf, 181,"st1",100,  -100., 100.);
    uthbook1(histf, 182,"st2",100,  -100., 100.);
    uthbook1(histf, 183,"st3",100,  -100., 100.);
    uthbook1(histf, 184,"st4",100,  -100., 100.);
    uthbook1(histf, 185,"st5",100,  -100., 100.);
    uthbook1(histf, 186,"st6",100,  -100., 100.);
    uthbook1(histf, 191,"st1",100,  -100., 100.);
    uthbook1(histf, 192,"st2",100,  -100., 100.);
    uthbook1(histf, 193,"st3",100,  -100., 100.);
    uthbook1(histf, 194,"st4",100,  -100., 100.);
    uthbook1(histf, 195,"st5",100,  -100., 100.);
    uthbook1(histf, 196,"st6",100,  -100., 100.);
    uthbook1(histf, 187,"dif1",500,  -250., 250.);
    uthbook1(histf, 188,"dif2",500,  -250., 250.);
    uthbook1(histf, 201,"rf1",100,  -100., 100.);
    uthbook1(histf, 202,"rf2",100,  -100., 100.);
    uthbook1(histf, 211,"tof-st -1",100,  -100., 100.);
    uthbook1(histf, 212,"tof-st -2",100,  -100., 100.);
    uthbook1(histf, 213,"tof-st -3",100,  -100., 100.);
    uthbook1(histf, 214,"tof-st -4",100,  -100., 100.);
    uthbook1(histf, 215,"tof-st -5",100,  -100., 100.);
    uthbook1(histf, 216,"tof-st -6",100,  -100., 100.);
I 13

    uthbook1(histf, 221,"xxx sec1", 200, 0.00, 4.00);
    uthbook1(histf, 222,"xxx sec2", 200, 0.00, 4.00);
    uthbook1(histf, 223,"xxx sec3", 200, 0.00, 4.00);
    uthbook1(histf, 224,"xxx sec4", 200, 0.00, 4.00);
    uthbook1(histf, 225,"xxx sec5", 200, 0.00, 4.00);
    uthbook1(histf, 226,"xxx sec6", 200, 0.00, 4.00);
E 13
E 4
  }

D 4
  /* calibration histograms */
E 4

D 4
  tghbookcalib(ana);

E 4
  return;
}



/* dchfill.c - to be included in dchist.c and ROOT */

I 6
void polarization_monitor(float ki[3], float kf[3], float pf[3], float *phi);
E 6

void
dchfill_(int *jw, TRevent *ana, int *ntr_out, int *itag0,
D 10
         float ev_out[ntrmx][36], float tag_res[10][7])
E 10
I 10
         float ev_out[ntrmx][36], float tag_res[TAG_MAX_HITS][7])
E 10
{
  dchfill(jw,ana,*ntr_out,*itag0,ev_out,tag_res);
}

void
dchfill(int *jw, TRevent *ana, int ntr_out, int itag0,
D 10
        float ev_out[ntrmx][36], float tag_res[10][7])
E 10
I 10
        float ev_out[ntrmx][36], float tag_res[TAG_MAX_HITS][7])
E 10
{
D 5
  int i0, ip, ipp, it, i, isec, itmp, ihel, ind, ihsc, ipsc, idsc, iret, ipel;
E 5
I 5
D 7
  int i0, ip, ipp, it, i, isec, ihel, ind, ihsc, ipsc, idsc, iret, ipel;
E 7
I 7
  int i0, ip, ipp, it, i, j, k, isec, ihel, ind, ihsc, ipsc, idsc, iret, ipel;
E 7
E 5
  float x,y,z,th,phi,Ebeam,P4in[4],P4e[4],P4p[4],P4out[4],Qsqr,p,evout7,phi0;
  float MM2e,MMe,MMx,MM2x,dif,tmp,tmptot,*rw,t0sc,Wmass,twL,twR,tm,tsc,p0;
  float bklow,bkhi,d,d0;
I 6
  float ki[3], kf[3], pf[3], phipiplus;
E 6
D 11
  int scndig, scdigi[48][5]; /* SC data */
E 11
I 11
  int scndig, scdigi[57][5]; /* SC data */
E 11
D 7

E 7
I 7
  int ind2;/* Pointer for ICHB */
  float pe[3],pe_mod;
  float *fptr;
  int *iptr;
  int ncol,nrow;
  float diff;
  float cluster_energy,phi2;
E 7
  rw = (float *)jw;

I 8
/*
E 8
I 7
printf("dcfill reached\n");
I 8
*/

E 8
  /* ********************************************************* */
D 12
  if((ind2 = etNlink(jw,"ICHB",0))>0) 
D 8
	{// Here we process data from ICHB bank if it exists
E 8
I 8
	{/* Here we process data from ICHB bank if it exists */
E 8

      nrow = etNrow(jw,ind2); /* Number of rows */
      ncol = etNcol(jw,ind2);
	  for(i=0; i<nrow; i++)  
	  {
         fptr = (float *)&jw[ind2+i*ncol];
         iptr = (int *)&jw[ind2+i*ncol];
E 12
I 12
  if((ind2 = etNlink(jw,"ICHB",0))>0) /* Here we process data from ICHB bank if it exists */
  {
    nrow = etNrow(jw,ind2); /* Number of rows */
    ncol = etNcol(jw,ind2);
	for(i=0; i<nrow; i++)  
	{
      fptr = (float *)&jw[ind2+i*ncol];
      iptr = (int *)&jw[ind2+i*ncol];
E 12
         
D 12
		  // Actually now we fill only one hist
          cluster_energy=fptr[0];
          x=(float) iptr[10];
          y=(float) iptr[11];
          uthfill(ana->histf, 1042, x-0.5, y-0.5, 1.);
          if(iptr[11] < 14)
          {
            uthfill(ana->histf, 1100+(iptr[11]-1)*18+(iptr[10]-1),
E 12
I 12
	  // Actually now we fill only one hist
      cluster_energy=fptr[0];
      x=(float) iptr[10];
      y=(float) iptr[11];
      uthfill(ana->histf, 1042, x-0.5, y-0.5, 1.);
      if(iptr[11] < 14)
      {
        uthfill(ana->histf, 1100+(iptr[11]-1)*18+(iptr[10]-1),
E 12
                    cluster_energy, 0., 1.);
D 12
            uthfill(ana->histf, 1700+(iptr[11]-1)*18+(iptr[10]-1),
E 12
I 12
        uthfill(ana->histf, 1700+(iptr[11]-1)*18+(iptr[10]-1),
E 12
                    cluster_energy, 0., 1.);
D 12
          }
E 12
I 12
      }
E 12
		  if (iptr[13]>1) // We take into account only clusters of at least 2 channels
		  {
            j=(x+0.001);
            k=(y+0.001);
            printf("x=%f y=%f ix=%d iy=%d hist=%d\n",
              x,y,iptr[10],iptr[11],1100+(iptr[11]-1)*18+(iptr[10]-1));
            uthfill(ana->histf, 1043, x-0.5, y-0.5, 1.);
            if(iptr[11] < 14)
            {
              uthfill(ana->histf, 1400+(iptr[11]-1)*18+(iptr[10]-1),
                      cluster_energy, 0., 1.);
            }
		  }


		}
	  }
		/* ************************************************** */

E 7
/*
C Here we do histogramming for simulated single track in sector#1
      IF(ABS(zkine(1)).EQ.1.) THEN
        it = 1
        ihis = 1
C Calculate the simulated track parameters (MCIN bank)
        CALL CROSS(ez,evin(4,it),u)
        CALL VUNIT(u,u,3)
        d0  = VDOT(evin(1,it),u,3)
        z0  = evin(3,it)
        p0  = evin(7,it)
        th0 = ACOS(evin(6,it))
        phi0= ATAN2(evin(5,it),evin(4,it))
C Calculate the reconstracted track parameters
c        CALL CROSS(ez,trkp(4,1,it),u)
c        CALL VUNIT(u,u,3)
c        d   = VDOT(trkp(1,1,it),u,3)
c        z   = trkp(3,1,it)
c        p   = trkp(7,1,it)
c        th  = ACOS(trkp(6,1,it))
c        phi = ATAN2(trkp(5,1,it),trkp(4,1,it))
c        CALL uthfill(ana->histf, ihis, trkp(12,1,it),0., 1.)
        CALL CROSS(ez,trkp(4,1),u)
        CALL VUNIT(u,u,3)
        d   = VDOT(trkp(1,1),u,3)
        z   = trkp(3,1)
        p   = trkp(7,1)
        th  = ACOS(trkp(6,1))
        phi = ATAN2(trkp(5,1),trkp(4,1))
        CALL uthfill(ana->histf, ihis, trkp(12,1),0., 1.)
C Histogramming
        diff = (p - p0)/p0
        CALL uthfill(ana->histf, ihis+1, diff,0., 1.)
        diff = (th - th0)*1000.
        CALL uthfill(ana->histf, ihis+2, diff,0., 1.)
        diff = (phi - phi0)*1000.
        CALL uthfill(ana->histf, ihis+3, diff,0., 1.)
        diff = d - d0
        CALL uthfill(ana->histf, ihis+4, diff,0., 1.)
        diff = z - z0
        CALL uthfill(ana->histf, ihis+5, diff,0., 1.)
      ENDIF
*/

  /* Data Analysis part */

  /*if(ntr_out == 0) return; - we have to fill some hists even no tracks */
D 6
  i0  = 0;
E 6

I 8
/*
E 8
I 7
printf("ntr_out=%d\n",ntr_out);
I 8
*/
E 8

E 7
  /* Find Electron or track with beta using ST, ToF & TAG
     (i.e. "itrk0" <==> "i0") */
I 6
  i0  = 0;
E 6
  for(it=1; it<=ntr_out; it++)
  {
D 7
    if(ev_out[it-1][0] > 0.01)
E 7
I 7
D 12
    if(ev_out[it-1][0] > 0.01) /*boy: 1.0 - electron, 0.0 - any other */
E 12
I 12
	/*printf("ev_out[%d][0] = %f\n",it-1,ev_out[it-1][0]);*/
    if(ev_out[it-1][0] > 0.01) /*sergey: 1.0 - electron, 0.0 - any other */
E 12
E 7
    {
      i0 = it;
      break;
    }
  }

I 7
D 12
  if (ntr_out==1){
	if (ev_out[0][0]<0.01)  // Here we selct events with single positively charged particle
E 12
I 12

/* sergey: temporary for photons */
if(ntr_out==1) i0 = 1;


  if(ntr_out==1)
E 12
  {
D 12
   phi2 = atan2(ev_out[0][4],ev_out[0][3])*raddeg;
    if(phi2 < 0.) phi2 += 360.0;
  p= ev_out[0][6];
  //  printf("cx=%f cy=%f cz=%f\n",ev_out[0][3],ev_out[0][4],ev_out[0][5]);
  uthfill(ana->histf, 1020, p ,0., 1.);
  uthfill(ana->histf, 1021, ev_out[0][5] ,0., 1.);
  uthfill(ana->histf, 1022, acos(ev_out[0][5])*raddeg ,0., 1.);
E 12
I 12
	if(ev_out[0][0]<0.01)  /* Here we select events with single positively charged particle */
    {
      phi2 = atan2(ev_out[0][4],ev_out[0][3])*raddeg;
      if(phi2 < 0.) phi2 += 360.0;
      p= ev_out[0][6];
	  /*printf("cx=%f cy=%f cz=%f\n",ev_out[0][3],ev_out[0][4],ev_out[0][5]);*/
      uthfill(ana->histf, 1020, p ,0., 1.);
      uthfill(ana->histf, 1021, ev_out[0][5] ,0., 1.);
      uthfill(ana->histf, 1022, acos(ev_out[0][5])*raddeg ,0., 1.);
E 12
 
D 12
  Ebeam = sdakeys_.zbeam[1];
E 12
I 12
      Ebeam = sdakeys_.zbeam[1];
E 12

D 12
  pe[0]=-ev_out[0][6]*ev_out[0][3];
  pe[1]=-ev_out[0][6]*ev_out[0][4];
  pe[2]=Ebeam-ev_out[0][6]*ev_out[0][5];
  //  printf("Ebeam=%f\n",Ebeam);
  pe_mod=sqrt(pe[0]*pe[0]+pe[1]*pe[1]+pe[2]*pe[2]);// Modul of momemtum
  pe[4]=sqrt(0.000512*0.000512+pe_mod*pe_mod);
E 12
I 12
      pe[0]=-ev_out[0][6]*ev_out[0][3];
      pe[1]=-ev_out[0][6]*ev_out[0][4];
      pe[2]=Ebeam-ev_out[0][6]*ev_out[0][5];
	  /*printf("Ebeam=%f\n",Ebeam);*/
      pe_mod=sqrt(pe[0]*pe[0]+pe[1]*pe[1]+pe[2]*pe[2]);// Modul of momemtum
      pe[4]=sqrt(0.000512*0.000512+pe_mod*pe_mod);
E 12

D 12
  uthfill(ana->histf, 1023, pe_mod ,0., 1.);
  uthfill(ana->histf, 1024, acos(pe[2]/pe_mod)*raddeg ,0., 1.);
E 12
I 12
      uthfill(ana->histf, 1023, pe_mod ,0., 1.);
      uthfill(ana->histf, 1024, acos(pe[2]/pe_mod)*raddeg ,0., 1.);
E 12
  
D 12
  if((ind2 = etNlink(jw,"ICHB",0)) > 0 && ev_out[0][5]<0.7) 
	{// Here we process data from ICHB bank if it exists

      nrow = etNrow(jw,ind2); /* Number of rows */
      ncol = etNcol(jw,ind2);
	  for(i=0; i<nrow; i++)
		{/* here we loop through all entries */
         fptr = (float *)&jw[ind2+i*ncol];
         iptr = (int *)&jw[ind2+i*ncol];
E 12
I 12
      if((ind2 = etNlink(jw,"ICHB",0)) > 0 && ev_out[0][5]<0.7) /* Here we process data from ICHB bank if it exists */
      {
        nrow = etNrow(jw,ind2); /* Number of rows */
        ncol = etNcol(jw,ind2);
	    for(i=0; i<nrow; i++) /* here we loop through all entries */
	    {
          fptr = (float *)&jw[ind2+i*ncol];
          iptr = (int *)&jw[ind2+i*ncol];
E 12
         
		  // Actually now we fill only one hist
D 12
         cluster_energy=fptr[0];
         phi = atan2(fptr[5],fptr[4])*raddeg;
    if(phi < 0.) phi += 360.0;
         diff=ABS(phi2-phi);
         printf("phi=%f phi2=%f diff=%f\n",phi,phi2,diff);
         if (diff>150. && diff<210.)  
		   {
		 uthfill(ana->histf, 1040,fptr[0]/pe[4] ,0., 1.); // Cluster energy 
         uthfill(ana->histf, 1041,fptr[0],pe[4],1.);
		   }
         printf("phi=%f phi2=%f\n",phi,phi2);    
         uthfill(ana->histf, 1025,cluster_energy ,0., 1.);
         uthfill(ana->histf, 1027,fptr[0]/pe[4] ,0., 1.); // Cluster energy 
         uthfill(ana->histf, 1029,fptr[0],pe[4],1.);
         uthfill(ana->histf, 1031,phi,0,1.);
         uthfill(ana->histf, 1033,phi,phi2,1.);
        
       }
      if (nrow==1)
E 12
I 12
          cluster_energy=fptr[0];
          phi = atan2(fptr[5],fptr[4])*raddeg;
          if(phi < 0.) phi += 360.0;
          diff=ABS(phi2-phi);
          printf("phi=%f phi2=%f diff=%f\n",phi,phi2,diff);
          if (diff>150. && diff<210.)  
		  {
	        uthfill(ana->histf, 1040,fptr[0]/pe[4] ,0., 1.); // Cluster energy 
            uthfill(ana->histf, 1041,fptr[0],pe[4],1.);
		  }
          printf("phi=%f phi2=%f\n",phi,phi2);    
          uthfill(ana->histf, 1025,cluster_energy ,0., 1.);
          uthfill(ana->histf, 1027,fptr[0]/pe[4] ,0., 1.); // Cluster energy 
          uthfill(ana->histf, 1029,fptr[0],pe[4],1.);
          uthfill(ana->histf, 1031,phi,0,1.);
          uthfill(ana->histf, 1033,phi,phi2,1.);
        }
        if (nrow==1)
E 12
		/* In our quasi-elastic case we are interested in only one particle to hit DVCS */
		{
D 12
         fptr = (float *)&jw[ind2];
         iptr = (int *)&jw[ind2];
E 12
I 12
          fptr = (float *)&jw[ind2];
          iptr = (int *)&jw[ind2];
E 12
         
D 12
         uthfill(ana->histf, 1026,fptr[0] ,0., 1.); // Cluster energy 
         uthfill(ana->histf, 1028,fptr[0]/pe[4],0,1.);
         uthfill(ana->histf, 1030,fptr[0], pe[4] , 1.); // Cluster energy 
         uthfill(ana->histf, 1032,phi,0,1.);
         uthfill(ana->histf, 1034,phi,phi2,1.);





E 12
I 12
          uthfill(ana->histf, 1026,fptr[0] ,0., 1.); // Cluster energy 
          uthfill(ana->histf, 1028,fptr[0]/pe[4],0,1.);
          uthfill(ana->histf, 1030,fptr[0], pe[4] , 1.); // Cluster energy 
          uthfill(ana->histf, 1032,phi,0,1.);
          uthfill(ana->histf, 1034,phi,phi2,1.);
E 12
		}
D 12

    }
E 12
I 12
      }
E 12
	}
  }


I 12
  /*printf("i0=%d\n",i0);*/
E 12
E 7
  if(i0 > 0)
  {
    /* Plot the reconstracted track parameters */
    x   = 0.0; /* x = ev_outx[i0-1] ! ev_out[i0-1][0] */
    y   = ev_out[i0-1][1];
    z   = ev_out[i0-1][2];
    p   = ev_out[i0-1][6];
    th  = acos(ev_out[i0-1][5])*raddeg;
    phi = atan2(ev_out[i0-1][4],ev_out[i0-1][3])*raddeg;
    if(phi < 0.) phi += 360.0;
I 13
	/*
E 13
I 12
printf("x=%f y=%f z=%f p=%f theta=%f phi=%f\n",x,y,z,p,th,phi);
E 12
I 7
D 8
    printf("phi=%f\n",phi);
E 8
I 8
D 13

E 13
I 13
	*/
E 13
D 12
/*
printf("phi=%f\n",phi);
*/
E 12
E 8
E 7
    if(sdakeys_.zmagn[3] != 0.)
    {
      isec = (phi + 30.)/60. + 1.;
      if(isec > 6) isec -= 6;
    }
    else
    {
      isec = ev_out[i0-1][10]/10.;
    }
    uthfill(ana->histf, 11, ev_out[i0-1][11],0., 1.);
D 12
    uthfill(ana->histf, 12,  p,0., 1.);
    uthfill(ana->histf, 13, th,0., 1.);
    uthfill(ana->histf, 130+isec, p, th, 1.);
    uthfill(ana->histf, 14,phi,0., 1.);
    uthfill(ana->histf, 15,  z,0., 1.);
    uthfill(ana->histf, 150+isec,  z,0., 1.);
    uthfill(ana->histf, 16,  y,0., 1.);
I 9
    uthfill(ana->histf, 110+isec,  y,0., 1.);
E 9
    uthfill(ana->histf, 17,  x,0., 1.);
E 12
I 12
    uthfill(ana->histf, 12,       p,  0., 1.);
    uthfill(ana->histf, 13,      th,  0., 1.);
    uthfill(ana->histf, 130+isec, p, th,  1.);
    uthfill(ana->histf, 14,     phi,  0., 1.);
    uthfill(ana->histf, 15,       z,  0., 1.);
I 14
    uthfill(ana->histf, 158,      z,  0., 1.);
E 14
    uthfill(ana->histf, 150+isec, z,  0., 1.);
    uthfill(ana->histf, 16,       y,  0., 1.);
    uthfill(ana->histf, 110+isec, y,  0., 1.);
    uthfill(ana->histf, 17,       x,  0., 1.);
E 12
  }

  /*********************/
  /* Electroproduction */
  /*********************/

  if(sdakeys_.zbeam[0] < -0.1)
  {
    if(i0 <= 0) return;
    Ebeam = sdakeys_.zbeam[1];
    P4in[0] = 0.;
    P4in[1] = 0.;
    P4in[2] = sqrt(Ebeam*Ebeam - aMe*aMe);
    P4in[3] = Ebeam + aMp;

	/* massaging data 
    theta_ = acos(ev_out[i0-1][5]);
    phi_ = atan2(ev_out[i0-1][4],ev_out[i0-1][3]);
    evout7 = massage(ev_out[i0-1][6],isec,theta_,phi_);
    printf("massage: %f -> %f\n",ev_out[i0-1][6],evout7);
    */
    evout7 = ev_out[i0-1][6];

    /* Final electron 4-momentum */
    P4e[0] = evout7*ev_out[i0-1][3];
    P4e[1] = evout7*ev_out[i0-1][4];
    P4e[2] = evout7*ev_out[i0-1][5];
    P4e[3] = sqrt(evout7*evout7 + aMe*aMe);

    /* Qsqr observed */
    Qsqr = 2.*Ebeam*P4e[3]*(1. - ev_out[i0-1][5]);
    uthfill(ana->histf, 21, Qsqr,0., 1.);
    uthfill(ana->histf, 210+isec, Qsqr,0., 1.);

    /* W (or MM) if only electron is measured */
D 5
    itmp=4;
    vsub_(P4in,P4e,P4out,&itmp);
E 5
I 5
    vsub(P4in,P4e,P4out,4);
E 5
    /*MM2e = P4out(4)**2 - VDOT(P4out,P4out,3)*/
    MM2e = 2.*aMp*(Ebeam - P4e[3]) + aMp*aMp - Qsqr;
    if(MM2e > 0.)
    {
      MMe = sqrt(MM2e);
    }
    else
    {
      MMe = 0.;
    }

    /* SB select elastic events for DST */
    /*if(MMe .LT. 1.05)
    {*/
      uthfill(ana->histf, 22, MMe, 0., 1.);
      uthfill(ana->histf, 220+isec, MMe, 0., 1.);
      uthfill(ana->histf, 227, MMe, Qsqr, 1.);
    /*}
    else
    {
      etNdrop(jw,"TRAK",0);
    }*/

D 9
    /* fill raster hist */
E 9
I 9
    /* fill raster hist 
E 9
    ind = etNlink(jw,"FBPM",0);
    if(ind > 0)
    {
D 9
      short *jw16 = (short *)&jw[ind+1-1];
      uthfill(ana->histf, 250,(float)jw16[2],(float)jw16[5],1.);
      uthfill(ana->histf, 251,(float)jw16[2],0.,1.);
      uthfill(ana->histf, 252,(float)jw16[5],0.,1.);
E 9
I 9
      float fx, fy;
      int i, nrow, ncol;
      unsigned short *jw16;

      nrow = etNrow(jw,ind);
      ncol = etNcol(jw,ind);
      printf("FBPM: ncol=%d nrow=%d\n",ncol,nrow);

      jw16 = (unsigned short *)&jw[ind];
      fx = fy = 100000.0;
      for(i=0; i<nrow; i++)
      {
        if(jw16[0]==1) fx = (float)jw16[2];
        if(jw16[0]==2) fy = (float)jw16[2];
        jw16 += 3;
      }

      if(fx<10000.0 && fy<10000.0)
      {
        uthfill(ana->histf, 250,fx,fy,1.);
        uthfill(ana->histf, 251,fx,0.,1.);
        uthfill(ana->histf, 252,fy,0.,1.);
	  }
      else
	  {
        printf("FBPM: ERROR: fx=%f fy=%f\n",fx,fy);
	  }
E 9
    }
I 9
	*/
E 9

D 9
    /* update helicity info */
E 9
I 9
    /* update helicity info using TGBI bank */
E 9
    ind = etNlink(jw,"TGBI",0);
    if(ind > 0)
    {
D 9
      ihel = jw[ind+1-1] >> 15;
      ihel = ihel & 1;
E 9
I 9
      ihel = (jw[ind+1-1] >> 15) & 1;

E 9
      if(ihel == 1)
      {
        uthfill(ana->histf, 228, MMe, 0., 1.);
      }
      else if(ihel == 0)
      {
D 9
        uthfill(ana->histf, 229, MMe,0., 1.);
E 9
I 9
        uthfill(ana->histf, 229, MMe, 0., 1.);
E 9
      }
      else
      {
        printf("ERROR: helicity = %d - exit.\n",ihel);
        exit(0);
      }
      tmptot = tmptot + 1.;
    }

I 9


    /* use RC26 to get helicity, corrected for delayed reporting */
    ind = etNlink(jw,"RC26",0);
    if(ind > 0)
    {
      int flag;

      flag = (jw[ind+1-1] >> 31) & 1;
      ihel = (jw[ind+1-1] >> 15) & 1;

      if(flag)
	  {
        if(ihel == 1)
        {
          uthfill(ana->histf, 238, MMe, 0., 1.);
        }
        else if(ihel == 0)
        {
          uthfill(ana->histf, 239, MMe, 0., 1.);
        }
        else
        {
          printf("ERROR: helicity = %d - exit.\n",ihel);
          exit(0);
        }
        tmptot = tmptot + 1.;
	  }
    }




E 9
I 7
    /******************************/
    /* elastic scattering events: */
E 7
    /* check if there are at least two tracks and one is positive */
D 7

E 7
    if(ntr_out < 2) goto a999;
    ip = 0;
    for(it=1; it<=ntr_out; it++)
    {
D 7
      if(ev_out[it-1][8] < 1.0) continue;
      ip ++;
      ipel = it;
E 7
I 7
      /*printf("Q=%f\n",ev_out[it-1][8]);*/
      if(ev_out[it-1][8] < 1.0) continue; /* skip if negative */
      ip ++;     /* count positive tracks */
      ipel = it; /* positive track number */
E 7
    }

D 7
    /* elastic scattering events */
E 7

D 7
    if(MMe > 0.85 && MMe < 1.05 && ip == 1)
E 7
I 7
    if(MMe > 0.80 && MMe < 1.10 && ip == 1)
E 7
    {
I 7
      /*printf("MMe=%f ip=%d\n",MMe,ip);*/
E 7
      phi0 = atan2(ev_out[ipel-1][4],ev_out[ipel-1][3])*raddeg;
      if(phi0 < 0.) phi0 += 360.;
      d = ABS(phi0 - phi);
      if(d > 175. && d < 185.)
      {
D 5
        itmp=4;
        vsub_(P4in,P4e,P4out,&itmp);
E 5
I 5
        vsub(P4in,P4e,P4out,4);
E 5
        P4p[0] = ev_out[ipel-1][6]*ev_out[ipel-1][3];
        P4p[1] = ev_out[ipel-1][6]*ev_out[ipel-1][4];
        P4p[2] = ev_out[ipel-1][6]*ev_out[ipel-1][5];
        P4p[3] = sqrt(ev_out[ipel-1][6]*ev_out[ipel-1][6] + aMp*aMp);
D 5
        itmp=4;
        vsub_(P4out,P4p,P4out,&itmp);
        itmp=3;
        MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
        vsub(P4out,P4p,P4out,4);
        MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5
        uthfill(ana->histf, 23, MM2x,0., 1.);
        if(ihel == 1)
        {
          uthfill(ana->histf, 231, MM2x, 0., 1.);
        }
        else if(ihel == 0)
        {
          uthfill(ana->histf, 232, MM2x, 0., 1.);
        }
        else
        {
          printf("dchist: ERROR: helicity=%d - exit.\n",ihel);
          exit(1);
        }
      }
    }

I 7
    /****************************************/
E 7
    /* Electroproduction of pi0, pi+ and K+ */

D 7
    if(MMe < 1.10 || ip < 1) goto a999;
E 7
I 7
    /*boy if(MMe < 1.10 || ip < 1) goto a999;*/
E 7
    for(ip=1; ip<=ntr_out; ip++)
    {
D 7
	  /* Plot Beta for the positive hadrons only */
E 7
I 7
	  /* search for the positive tracks only */
E 7
      if(ev_out[ip-1][8] < 1.0) continue;
D 6

E 6
      uthfill(ana->histf, 27, ev_out[ip-1][6],ev_out[ip-1][7], 1.);
I 6

E 6
	  /* Kill elastic scattering */
      phi0 = atan2(ev_out[ip-1][4],ev_out[ip-1][3])*raddeg;
      if(phi0 < 0.) phi0 += 360.;
      d0 = phi0 - phi - 180.;
D 7
      if(d0 >= -5. && d0 <= 5.) continue;
E 7
I 7
      /*boy if(d0 >= -5. && d0 <= 5.) continue;*/
E 7
I 6

I 7

      /*****************************/
E 7
E 6
	  /* Particle's identification */
I 7

E 7
      p0  = ev_out[ip-1][6];
      bklow = p0/sqrt(p0*p0 + 0.716*0.716);
      bkhi  = p0/sqrt(p0*p0 + 0.316*0.316);
I 6

E 6
	  /* Final h+ 3-momentum */
      P4p[0] = p0*ev_out[ip-1][3];
      P4p[1] = p0*ev_out[ip-1][4];
      P4p[2] = p0*ev_out[ip-1][5];
I 6

E 6
      /* MM if proton is measured */
      if(ev_out[ip-1][7] < bklow)
      {
D 5
        itmp=4;
        vsub_(P4in,P4e,P4out,&itmp);
E 5
I 5
        vsub(P4in,P4e,P4out,4);
E 5
        P4p[3] = sqrt(ev_out[ip-1][6]*ev_out[ip-1][6] + aMp*aMp);
D 5
        itmp=4;
        vsub_(P4out,P4p,P4out,&itmp);
        itmp=3;
        MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
        vsub(P4out,P4p,P4out,4);
        MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5

        th = acos(ev_out[ip-1][5])*raddeg;
        uthfill(ana->histf, 241, MM2x,0., 1.);
        if(MM2x < 0. && th > 60. && th < 110.)
        {
          uthfill(ana->histf, 157, ev_out[ip-1][2],0., 1.);
		  /*
          if(ev_out[ip-1][2] < 3. && ev_out[ip-1][2] > 0.)
          {
            uthfill(ana->histf, 152, ev_out[i0-1][2], 0., 1.);
          }
          if(ev_out[ip-1][2] < -1. && ev_out[ip-1][2] > -4.)
          {
            uthfill(ana->histf, 153, ev_out[i0-1][2], 0., 1.);
          }
          dif = ABS(ev_out[ip-1][2] - ev_out[i0-1][2]);
          if(dif < 1.)
          {
            uthfill(ana->histf, 154, ev_out[ip-1][2], 0., 1.);
            uthfill(ana->histf, 155, ev_out[i0-1][2], 0., 1.);
          }
		  */
        }

        MMx = -0.1;
        if(MM2x > 0.) MMx = sqrt(MM2x);
        uthfill(ana->histf, 24, MMx, 0., 1.);
      }
D 7
      else
E 7
I 7
      else if(ev_out[ip-1][7] >= bkhi) /* MM if pion+ is measured */
E 7
      {
D 7
		/* MM if pion+ is measured */
        if(ev_out[it-1][7] >= bkhi)
        {
D 5
          itmp=4;
          vsub_(P4in,P4e,P4out,&itmp);
E 5
I 5
          vsub(P4in,P4e,P4out,4);
E 5
          P4p[3] = sqrt(ev_out[ip-1][6]*ev_out[ip-1][6] + aMpi*aMpi);
D 5
          itmp=4;
          vsub_(P4out,P4p,P4out,&itmp);
          itmp=3;
          MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
          vsub(P4out,P4p,P4out,4);
          MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5
          MMx = -0.1;
          if(MM2x > 0.) MMx = sqrt(MM2x);
          uthfill(ana->histf, 26, MMx,0., 1.);
E 7
I 7
        vsub(P4in,P4e,P4out,4);
        P4p[3] = sqrt(ev_out[ip-1][6]*ev_out[ip-1][6] + aMpi*aMpi);
        vsub(P4out,P4p,P4out,4);
        MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
        MMx = -0.01;
        if(MM2x > 0.) MMx = sqrt(MM2x);
        uthfill(ana->histf, 26, MMx,0., 1.);
E 7
I 6

/*** beam polarization monitoring attempt ***/

/* did not finished !!!
if(MMe>1.18 && MMe<1.30)
{
  for(i=0; i<3; i++)
  {
    ki[i] = P4in[i];
    kf[i] = P4e[i];
    pf[i] = P4p[i];
  }

  polarization_monitor(ki, kf, pf, &phipiplus);
  uthfill(ana->histf, 31, phipiplus, 0., 1.);
  printf("%7.3f %7.3f %7.3f - %7.3f %7.3f %7.3f - %7.3f %7.3f %7.3f -> %f\n",
  ki[0],ki[1],ki[2],kf[0],kf[1],kf[2],pf[0],pf[1],pf[2],phipiplus);
}
*/

/********************************************/

I 7
      }
      else		/* MM if kaon is measured */
      {
        if(MMe > 1.60)
        {                                       
          vsub(P4in,P4e,P4out,4);
          P4p[3] = sqrt(ev_out[ip-1][6]*ev_out[ip-1][6] + aMk*aMk);
          vsub(P4out,P4p,P4out,4);
          MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
          MMx = -0.1;
          if(MM2x > 0.) MMx = sqrt(MM2x);
          uthfill(ana->histf, 25, MMx, 0., 1.);
E 7
E 6
        }
D 7
        else
        {
		  /* MM if kaon is measured */
          if(MMe > 1.60)
          {                                       
D 5
            itmp=4;
            vsub_(P4in,P4e,P4out,&itmp);
E 5
I 5
            vsub(P4in,P4e,P4out,4);
E 5
            P4p[3] = sqrt(ev_out[ip-1][6]*ev_out[ip-1][6] + aMk*aMk);
D 5
            itmp=4;
            vsub_(P4out,P4p,P4out,&itmp);
            itmp=3;
            MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
            vsub(P4out,P4p,P4out,4);
            MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5
            MMx = -0.1;
            if(MM2x > 0.) MMx = sqrt(MM2x);
D 6
            uthfill(ana->histf, 25, MMx,0., 1.);
E 6
I 6
            uthfill(ana->histf, 25, MMx, 0., 1.);
E 6
          }
        }              /* IF(ev_out(8,it).GE.bkhi) */
      }                /* IF(ev_out(8,ip).LT.bklow) */
E 7
I 7
      }

E 7
    }               /* Loop over tracks */

  }


  /*********************/
  /*  Photoproduction  */
  /*********************/

  if(sdakeys_.zbeam[0] == 0.0)
  {
    if(i0 == 0) return;
    itag0 = ev_out[i0-1][0];

    /* Reconstructed photon energy "Ebeam" (if SIM one overwrites
       the simulated Ebeam) */

    Ebeam = tag_res[itag0-1][2] * sdakeys_.zbeam[1];
    uthfill(ana->histf, 20, Ebeam, 0., 1.);

    P4in[0] = 0.0;
    P4in[1] = 0.0;
    P4in[2] = Ebeam;
    P4in[3] = Ebeam + aMp;

    /* T-counter time (RF-cor "tag_res(7,itag0)"
       or RF-uncor "tag_res(6,itag0)") */
    uthfill(ana->histf, 21,tag_res[itag0-1][5],0.,1.);

    /* SC Time "t0sc" for itrk0 */
    ihsc = ev_out[i0-1][27]/10000.;
    ipsc = (ev_out[i0-1][27] - ihsc*10000.)/100.;
    idsc = ev_out[i0-1][27] - ihsc*10000. - ipsc*100.;

    GETSCDATA;

    sctwalk(ihsc,idsc,isec,scdigi, &twL,&twR,&tm,&tsc,&iret);
    t0sc = tsc - ev_out[i0-1][26]/vflt/ev_out[i0-1][7];

    uthfill(ana->histf, 22,t0sc,0.,1.);                    

    /* Center of Mass Energy (W) */
    Wmass = sqrt(aMp*aMp + 2.*Ebeam*aMp);
    uthfill(ana->histf, 23,Wmass,0.,1.);                    

/*
  Examine all tracks and identify: g + p --> pi+ + X   or
                                   g + p --> K+  + X   or
                                   g + p --> p   + X 
*/
I 17
D 18
    /* update helicity info using TGBI bank */
    ihel = -1;
    ind = etNlink(jw,"TGBI",0);
    if(ind > 0)
    {
      ihel = (jw[ind+1-1] >> 15) & 1;
    }
E 18
    /* use RC26 to get helicity (corrected) */
D 18
    int ihel2 = -1;
E 18
I 18
    int ihel = -1;
E 18
    ind = etNlink(jw,"RC26",0);
    if(ind > 0)
    {
      if( (jw[ind+1-1] >> 31) & 1 )
D 18
        ihel2 = (jw[ind+1-1] >> 15) & 1;
E 18
I 18
        ihel = (jw[ind+1-1] >> 15) & 1;
E 18
    }
E 17

    for(ip=1; ip<=ntr_out; ip++)
    {
      if(ev_out[ip-1][8] < 1.0) continue;

      /* Plot Beta for the positive hadrons */
      uthfill(ana->histf, 27, ev_out[ip-1][6],ev_out[ip-1][7], 1.);
      p0 = ev_out[ip-1][6];
      bklow = p0 / sqrt(p0*p0 + 0.716*0.716);
      bkhi  = p0 / sqrt(p0*p0 + 0.316*0.316);

      /* Final h+ 3-momentum */
      P4p[0] = p0*ev_out[ip-1][3];
      P4p[1] = p0*ev_out[ip-1][4];
      P4p[2] = p0*ev_out[ip-1][5];

      /* MM if proton is measured */
      if(ev_out[ip-1][7] < bklow)
      {
        P4p[3] = sqrt(p0*p0 + aMp*aMp);
D 5
        itmp=4;
        vsub_(P4in,P4p,P4out,&itmp);
        itmp=3;
        MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
        vsub(P4in,P4p,P4out,4);
        MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5

        th = acos(ev_out[ip-1][5])*raddeg;
        uthfill(ana->histf, 241, MM2x, 0., 1.);
        /*if(MM2x < 0 && th > 60 && th < 110)*/
        if(MM2x < 0)
        {
          uthfill(ana->histf, 157, ev_out[ip-1][2],0., 1.);
		  /*
          if(ev_out[ip-1][2] < 3 && ev_out[ip-1][2] > 0.) uthfill(ana->histf, 152, ev_out[i0-1][2],0., 1.);
          if(ev_out[ip-1][2] < -1 && ev_out[ip-1][2] > -4.) uthfill(ana->histf, 153, ev_out[i0-1][2],0., 1.);
          dif = ABS(ev_out[ip-1][2]-ev_out[i0-1][2]);
          if(dif < 1.)
          {
            uthfill(ana->histf, 154, ev_out[ip-1][2], 0., 1.);
            uthfill(ana->histf, 155, ev_out[i0-1][2], 0., 1.);
          }
		  */
        }

        MMx = -0.1;
        if(MM2x > 0.) MMx = sqrt(MM2x);
        uthfill(ana->histf, 24, MMx,0., 1.);
        /* do we have pi+ ? */
        for(ipp=1; ipp<=ntr_out; ipp++)
        {
          if(ev_out[ipp-1][7] >= bkhi)
          {
            uthfill(ana->histf, 42, MMx,0., 1.);
            break;
          }
        }
 
		/* phi dependence */
        if(MMx > 0.75 && MMx < 0.82)
        {
          phi = atan2(ev_out[ip-1][4],ev_out[ip-1][3])*raddeg;
          if(phi < 0.) phi += 360.0;
          if(Ebeam > 0.2 && Ebeam < 2.3) uthfill(ana->histf,141,phi,0., 1.);
          if(Ebeam > 2.4 && Ebeam < 3.5) uthfill(ana->histf,142,phi,0., 1.);

          /* do we have pi+ ? */
          for(ipp=1; ipp<=ntr_out; ipp++)
          {
            if(ev_out[ipp-1][7] >= bkhi)
            {
              phi = atan2(ev_out[ipp-1][4],ev_out[ipp-1][3])*raddeg;
              if(phi < 0.) phi += 360.0;
              if(Ebeam > 0.2 && Ebeam < 2.3) uthfill(ana->histf,143,phi,0., 1.);
              if(Ebeam > 2.4 && Ebeam < 3.5) uthfill(ana->histf,144,phi,0., 1.);
              break;
            }
          }
        }


      }
      else
      {
		/* MM if pion+ is measured */
        if(ev_out[ip-1][7] >= bkhi)
        {
          P4p[3] = sqrt(p0*p0 + aMpi*aMpi);
D 5
          itmp=4;
          vsub_(P4in,P4p,P4out,&itmp);
          itmp=3;
          MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
          vsub(P4in,P4p,P4out,4);
          MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5
          MMx = 0.;
          if(MM2x > 0.) MMx = sqrt(MM2x);
          uthfill(ana->histf, 26, MMx,0., 1.);

          /* phi dependence */
          if(MMx > 0.9 && MMx < 1.05)
          {
            phi = atan2(ev_out[ip-1][4],ev_out[ip-1][3])*raddeg;
            if(phi < 0.) phi += 360.0;

            if(Ebeam > 0.2 && Ebeam < 2.3) uthfill(ana->histf,145,phi,0., 1.);
            if(Ebeam > 2.4 && Ebeam < 3.5) uthfill(ana->histf,146,phi,0., 1.);
I 17

            /* helicity dependent rates */
            float z = ev_out[ip-1][2];
            int hid = 0;

            if(Ebeam >0.6 && Ebeam <1.2) {
D 18
              if(z > -2.4 && z < 2.4)  hid = 262;
              if(z >  5.5 && z < 6.5)  hid = 264;
              if(z > 15.5 && z < 16.5) hid = 266;
E 18
I 18
              if(z > -2.4 && z < 2.4)  hid = 362;
              if(z >  5.5 && z < 6.5)  hid = 364;
              if(z > 15.5 && z < 16.5) hid = 366;
E 18
              if(Ebeam > 0.9) hid+=10;

              if(hid > 10) {
                float beta  = Ebeam/(Ebeam+aMp);
                float gamma = (Ebeam+aMp)/Wmass;
                float Pcm_g = Ebeam*aMp/Wmass;
                float Ecm_pi = gamma*P4p[3] - gamma*beta*P4p[2];
                float PZcm_pi= gamma*P4p[2] - gamma*beta*P4p[3];
                float Pcm_pi = sqrt(P4p[0]*P4p[0]+P4p[1]*P4p[1]+PZcm_pi*PZcm_pi); 
                float cthcm_pi= Ecm_pi/Pcm_pi-Ebeam*(P4p[3]-p0*ev_out[ip-1][5])/Pcm_g/Pcm_pi;
                if(ihel >= 0) 
                  uthfill(ana->histf, hid-ihel, cthcm_pi,0.,1.); 
D 18
                if(ihel2 >=0)
                  uthfill(ana->histf, hid+100-ihel, cthcm_pi,0.,1.); 
E 18
              }
            }

E 17
          }

          uthfill(ana->histf, 220+isec, MMx,0., 1.);
        }
        else
        {
		  /* MM if kaon+ is measured */
          if(Wmass > 1.6)
          {                  
            P4p[3] = sqrt(p0*p0 + aMk*aMk);
D 5
            itmp=4;
            vsub_(P4in,P4p,P4out,&itmp);
            itmp=3;
            MM2x = P4out[3]*P4out[3] - vdot_(P4out,P4out,&itmp);
E 5
I 5
            vsub(P4in,P4p,P4out,4);
            MM2x = P4out[3]*P4out[3] - vdot(P4out,P4out,3);
E 5
            MMx = 0.;
            if(MM2x > 0.) MMx = sqrt(MM2x);
            uthfill(ana->histf, 25, MMx, 0., 1.);
          }
        }
      } /* IF(ev_out(8,it).LT.bklow) */

    } /* Loop over tracks */

I 18
    int id;
    float vsum[20], vneg[20];
    float xval, asym;
    for(id=361; id<=366; id++){
      if(ana->histf[id].entries>100){
	uthunpak(ana->histf,id,vsum," ",1);
	uthunpak(ana->histf,id+10,vneg," ",1);
        for(i=0; i<20; i++) { 
          asym= (vsum[i]>1.0)? 1.0-2*vneg[i]/vsum[i] : 0.0;
	  xval=i/11.-0.999;
          uthfill(ana->histf,id-100,xval,0.,asym);
	}
      }
    }
E 18
  }

a999:

  return;
}


I 6
/*************** service subroutines *****************/

/*
Hi Sergei,

 To monitor polarization we need to calculate 
 sin(phi*) moments at end of run, defined as:

 <sin(phi*)>(+) - <sin(phi*)>(-)

 positive helicity moment - negative helicity moment

 where <sin(phi*)>(+/-) = Sum over all N(+/-) events: sin(phi*) / N(+/-)

 So for each pi+ event between 1.18 < W < 1.30, we need phi*. The code 
 below will give this assuming you have detected electron 3-vector in kf(3) 
 and pion 3-vector in pf(3) and initial electron vector in ki(3).

 Cole
*/

/*
      subroutine extractang(th,thd,ph,phd,vec)
      implicit none

      real vec(3),vdot,th,thd,ph,phd,r2d,vec_mag,pi
      
      data r2d/57.2957795/
      data pi/3.141592654/
      
      vec_mag = sqrt(vdot(vec,vec,3))
      th  = acos(vec(3)/vec_mag)
      thd = th*r2d
      ph  = max(-pi,min(pi,atan2(vec(2),vec(1))))
      phd = max(-180.0,min(180.0,ph*r2d))
      
      return
      end

*/

#define r2d 57.2957795
#define pi  3.141592654

void
extractang_(float *th_, float *thd_, float *ph_, float *phd_, float vec[3])
{
  float th, thd, ph, phd;

  th  = acos( vec[2]/sqrt(vdot(vec,vec,3)) );  /* radians */
  thd = th*r2d;                                /* degrees */
  ph  = MAX(-pi,MIN(pi,atan2(vec[1],vec[0]))); /* radians */
  phd = MAX(-180.0,MIN(180.0,ph*r2d));         /* degrees */

  *th_  = th;
  *thd_ = thd;
  *ph_  = ph;
  *phd_ = phd;

  return;
}

/*
      call extractang(the_e,the_ed,phi_e,phi_ed,kf)
      call extractang(the_h,the_hd,phi_h,phi_hd,pf)

      do i=1,3
        qv(i)=ki(i)-kf(i)
      enddo

      ki_mag = sqrt(vdot(ki,ki,3))
      kf_mag = sqrt(vdot(kf,kf,3))
      qv_mag = sqrt(vdot(qv,qv,3))
      pf_mag = sqrt(vdot(pf,pf,3))

      E_h = sqrt(pf_mag**2+0.139**2)

      nu     = kf_mag - ki_mag

      beta    = qv_mag / (nu + 0.938)
      gamma   = 1./sqrt(1.-beta**2)

      call extractang(the_q,the_qd,phi_q,phi_qd,qv)
      call rotate(-1,the_q,phi_e,pf,pf_q)
      call boost(-beta,pf_q,E_h,pf_cm,E_h_cm)
      call extractang(the_h_cm,the_hd_cm,phi_h_cm,phi_hd_cm,pf_cm)      
*/


/*
  input params: ki[3] - initial electron 3-vector
                kf[3] - detected electron 3-vector
                pf[3] - detected pion 3-vector
  output:       phi - PHI angle in cm
*/

void
polarization_monitor(float ki[3], float kf[3], float pf[3], float *phi)
{
  float the_e, the_ed, phi_e, phi_ed;
  float the_h, the_hd, phi_h, phi_hd;
  float qv[3], bet, ki_mag, kf_mag, qv_mag, pf_mag, E_h, nu, beta, gamma;
  float the_q, the_qd, phi_q, phi_qd;
  float the_h_cm, the_hd_cm, phi_h_cm, phi_hd_cm;
  float pf_q[3], pf_cm[3], E_h_cm;
  int i, itmp;

  extractang_(&the_e, &the_ed, &phi_e, &phi_ed, kf);
printf("1: %f %f %f -> %f %f %f %f\n",kf[0],kf[1],kf[2],
the_e, the_ed, phi_e, phi_ed);
  extractang_(&the_h, &the_hd, &phi_h, &phi_hd, pf);
printf("2: %f %f %f -> %f %f %f %f\n",pf[0],pf[1],pf[2],
the_h, the_hd, phi_h, phi_hd);
  for(i=0; i<3; i++) qv[i] = ki[i] - kf[i];

  ki_mag = sqrt(vdot(ki,ki,3));
  kf_mag = sqrt(vdot(kf,kf,3));
  qv_mag = sqrt(vdot(qv,qv,3));
  pf_mag = sqrt(vdot(pf,pf,3));
  E_h = sqrt(pf_mag*pf_mag + 0.139*0.139);
printf("3: %f %f %f %f %f\n",ki_mag,kf_mag,qv_mag,pf_mag,E_h);
  nu = kf_mag - ki_mag;
  beta = qv_mag / (nu + 0.938);
printf("8: beta=%f (%f %f)\n",beta,qv_mag,nu);
  gamma = 1./sqrt(1.-beta*beta);

  extractang_(&the_q, &the_qd, &phi_q, &phi_qd, qv);
  itmp = -1;
  rotate_(&itmp, &the_q, &phi_e, pf, pf_q);
  bet = -beta;
  boost_(&bet, pf_q, &E_h, pf_cm, &E_h_cm);
  extractang_(&the_h_cm, &the_hd_cm, &phi_h_cm, &phi_hd_cm, pf_cm);      

printf("9: %f %f %f -> %f %f %f %f\n",pf_cm[0],pf_cm[1],pf_cm[2],
the_h_cm,the_hd_cm,phi_h_cm,phi_hd_cm);

  *phi = phi_hd_cm; /* ? */

  return;
}


D 7




E 6

E 7

E 1