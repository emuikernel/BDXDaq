h01393
s 00015/00000/00111
d D 1.2 02/03/26 15:15:26 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:00:39 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/sdakeys.h
e
s 00111/00000/00000
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
C SDA keys as initialized reading the DATA CARDS (see Config. File "ftn33")
C                                     Bogdan Niczyporuk, March 1991
C
C   Run    - Carrent run number (TRIG card)
C   Nevt   - Number of events to process (TRIG card)
C   Ndbg   - Number of events to debug (TRIG card)
C   ievt   - Current event number
C   iseed  - Initialized random number (RNDM card)
C   mgtyp  - Selected type of the magnetic field (MAGN card)
C              mgtyp =0 for TABL - table of magnetic fild
C              mgtyp =1 for TORU - calculates field for torus
C              mgtyp =2 for MINT - calculates field for mini torus
C              mgtyp =3 for TOPM - calculates field for torus + mini torus
C              mgtyp =4 for UNIF - uniform field (along wire) of -3.0 kG
C              mgtyp =5 for NONE - No magnetic field
C   step   - Current step size for trajectory propagation
C   lmode  - Interactive or Batch version (MODE card)
C   isim   - Flag when set to 1 means data simulation (=0 analysis:fit,...)
C   ifsim  - Flag is set to 1 when reading REVB simulated by SDA (IW(ind+5)=-1)
C
C   ltrig  - Contains parameters of TRIG card
C   lprnt  - Contains parameters of PRNT card
C    time  - Contains parameters of TIME card
C     isw  - Contains parameters of SWIT card
C   lgeom  - Contains parameters of GEOM card
C   lcali  - Contains parameters of CALI card
C   zkine  - Contains parameters of KINE card
C   zderr  - Contains parameters of DERR card
C   zmagn  - Contains parameters of MAGN card
C   lmatx  - Contains parameters of MATX card
C   zgrid  - Contains parameters of WTAB card
C   ztfit  - Contains parameters of TFIT card
C   levb   - Contains parameters of EVBU card
C   zbgrd  - Contains parameters of BGRD card
C    lout  - Contains parameters of LOUT card
C   lanal  - Contains parameters of ANAL card
C   zstep  - Contains parameters of STEP card
C   zcut   - Contains parameters of CUTS card
C   ztarg  - Contains parameters of TARG card
C   zbeam  - Contains parameters of BEAM card
C   zgcor  - Contains parameters of GCOR card
C
      INTEGER Run, Nevt, Ndbg, ievt, iseed, mgtyp, lmode, isim, ifsim
      REAL step
C Arrays
      INTEGER ltrig, lprnt, isw, lgeom, lcali, lmatx, levb, lout, lanal 
      REAL    time, zkine, zderr, zmagn, zgrid, ztfit, zbgrd, zstep,zcut
      REAL    ztarg, zbeam, zgcor
C
      COMMON/sdakeys/ Run,Nevt,Ndbg,ievt,iseed,mgtyp,lmode,isim,ifsim,
     2      step,ltrig(10),lprnt(10),time(10),isw(10),lgeom(10),zkine(20),
     3      zderr(10),zmagn(10),lmatx(10),zgrid(10),ztfit(10),
     4      levb(10),zbgrd(14),lout(36),lanal(10),zstep(10),zcut(10),
     5      ztarg(10),zbeam(10),lcali(10),zgcor(11)
C
      save  /sdakeys/
C
*/

typedef struct sdakeys
{
  int   Run;
  int   Nevt;
  int   Ndbg;
  int   ievt;
  int   iseed;
  int   mgtyp;
  int   lmode;
  int   isim;
  int   ifsim;
  float step;      
  int   ltrig[10];
  int   lprnt[10];
  float time[10];
  int   isw[10];
  int   lgeom[10];
  float zkine[20];      
  float zderr[10];
  float zmagn[10];
  int   lmatx[10];
  float zgrid[10];
  float ztfit[10];                     
  int   levb[10];
  float zbgrd[14];
  int   lout[36];
  int   lanal[10];
  float zstep[10];
  float zcut[10];
  float ztarg[10];
  float zbeam[10];
  int   lcali[10];
  float zgcor[11];
I 3

  /**********************************************/
E 3
  /* following part does not exist in *.inc !!! */
I 3
  /**********************************************/

  /* l3lib stuff */
  int   l3mode; /* 'TAGG' or 'TRIG' */
  int   l3prescale;
  int   l3a1;
  int   l3a2;
  int   l3a3;
  float l3p1[10];
  float l3p2[10];
  float l3p3[10];

  /* */
E 3
  int   ndebu;
  char  debu[10][6];
  char  geomfile[256];
  char  dictfile[256];
  char  btorfile[256];
  char  bminfile[256];
  char  bpolfile[256];
  char  cbosfile[256];
  char  inputfile[256];
  char  outputfile[256];
} SDAkeys;


/* function prtotypes */

void trgetconfig_(SDAkeys *keys);
void trgetconfig(SDAkeys *keys);
E 1
