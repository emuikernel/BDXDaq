
#ifdef VXWORKS

/* pulserint.c   ( for loading in VxWorks )
 *
 *  Use INTERRUPT SERVICE to operate tasks where
 *  several steps of delta_time should be used.
 *  Based on LAM request from clk.gen3660
 *
 *  Here is a set of functions to control work
 *  of  Phillips Charge/Time Generator 7120
 *  and KineticSystem Programmable Clock Generator 3660
 *  as a ONE COMBINED DEVICE - named "pulser".
 *
 *  Main of them :
 *  --------------
 *  pulser_start (int dt1, int dt2, int ddt, int rate, int N) -
 *
 *   - provide two modes of hardware work:
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   1) when ddt not ZERO, chosen INTERRUPT SERVICE;
 *     set MINIMUM ("dt1") and MAXIMUM ("dt2") values of a delay
 *     between Start and Stop signals; set STEP ("ddt") to change
 *     a delay through the whole region from MINIMUM to  MAXIMUM;
 *     "N" and "rate" are a number of Start-Stop pairs and
 *     a frequency of their followings.
 *   2) when ddt=0, hardware works without Interrupts;
 *     set a delay ("delta_time") between Start and Stop signals;
 *     "N" and "rate" are a number of Start-Stop pairs and
 *     a frequency of their followings;
 *     continuous mode of generation can be set, put N=-1.
 *
 *
 *  pulser_stop ()        - correct pulser stop.
 *  pulser_stat (int prn) - get pulser status vs prn
 *                               0: NO prints, return(stat);
 *                               1: print Short Status;
 *                               2: print Full Status;
 *                               3: print Long Status.
 *
 *
 *   SP, 28-Sep-2002
 *
 */

#include <stdio.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include <iv.h>
#include <ca.h>
#include "genROCparm.h"
#include "gen7120.h"
#include "clockgen3660.h"


/* base address */
#ifdef VXWORKSPPC
#define ks2917_BADR  0xfbffff00
#else
#define ks2917_BADR  0xffffff00
#endif

/* interrupt level */
#define MYINT_LEVEL   6

/* Read ks2917_BADR+0x48 for LAM interrupt vector */


/****** Clk.gen3660 parameters ******/
unsigned short fsd_1, fsd_2, fsd_3, fsd_4, csr;

/****** Gen7120 global structure ******/
extern GEN7120 gen[8][25];


/****** Libruary functions ******/
int  pulser_start (int dt1, int dt2, int ddt, int rate, int N);
void pulser_stop  ();
int  pulser_stat  (int prn);

int    gen7120prestart  ();
void   gen3660prestart  ();
STATUS ks2917inthandler ();
void   ks2917intinit    ();
void   ks2917intenable  ();
void   ks2917intdisable ();


/***************************************************************
 * Start pulser with different mode of generation:
 *   a) continuous generation (N=-1, ddt=0), dt2 not essential;
 *   b) generate "N" pulses with "rate" frequency
 *      for "dt1" Start-Stop delay;
 *   c) if "ddt" not ZERO, generate "N" pulses per each 
 *      delay=(dt1+ddt*i), 0<=i<=(dt2-dt1)/ddt.
 *
 * Provide two modes of hardware work:
 *  - when ddt not ZERO, chosen INTERRUPT SERVICE;
 *  - when ddt=0, hardware works without Interrupts.
 */
int
pulser_start (int dt1, int dt2, int ddt, int rate, int N)
{
  int q, res;

/****** Check if gen3660 or gen7120 is already in process ******/
  res =  pulser_stat(0);
  if ( res==1 || res==2 ) {
    printf("\n Pulser is already active.  Call later.\n\n");
    return (1); }
  else if ( res==-1 )  return (1);
  else if ( gen[CC][NN7120].genRunning!=0 ) {
    printf("\n Gen7120-%d-%d is active.  Call later.\n\n", CC, NN7120);
    return (1); }

/****** Store input parameters in global structure ******/
  gen[CC][NN7120].dt1  = dt1;
  gen[CC][NN7120].dt2  = dt2;
  gen[CC][NN7120].ddt  = ddt;
  gen[CC][NN7120].rate = rate;
  gen[CC][NN7120].N    = N;

/****** Check & Prestart gen7120 ******/
  if ( gen7120prestart()!=0 )  return (1);

/****** Prestart gen3660 ******/
  gen3660prestart();

/****** Preset and Activate interrupt ******/
  if ( gen[CC][NN7120].ddt!=0 ) {
    ks2917intinit ();
    ks2917intenable ();
  }

/****** Activate gen3660 ******/
  csr   = csr | (1<<7);
  clkgen3660writeCSR (CC,NN3660,csr,&q);  /* 3660 to Active mode */

  printf("\n Pulser has been started. \n\n");

  return (0);
}

/***********************
 * Correct pulser stop
 */
void
pulser_stop ()
{
  int d, q, ii;

  d = clkgen3660readCSR (CC, NN3660, &q);
  d = (d & 0x80) >> 7;

  if ( d != 1 )
    printf("\n Pulser has been already stopped. \n\n");
  else {
    clkgen3660resetALL (CC, NN3660, &q);  /* put 3660 in Inactive state */
    for (ii=0; ii<=10; ii++) {
      d = clkgen3660readCSR (CC, NN3660, &q);
      d = (d & 0x80) >> 7;
      if ( (d==1) && (ii<10) )
	clkgen3660resetALL (CC, NN3660, &q);  /* next try to stop 3660 */
      else if ( d != 1 ) {
	ks2917intdisable ();
	gen[CC][NN7120].genRunning=0;  /* unhold gen7120 for others */
	printf("\n Pulser stopped. \n\n");
	ii=11; }
      else
	printf("\n Error: Pulser did not stop correctly. \n\n");
    }
  }
}

/************************************
 * Get pulser status:
 *   prn=0: NO prints, return(stat);
 *   prn=1: print Short Status;
 *   prn=2: print Full Status;
 *   prn=3: print Long Status.

 */
int
pulser_stat (int prn)
{
  int d, d1, d2, cpc, q;
  int stat=0;

  if ( prn==2 || prn==3 )
    if ( gen7120switchstat(CC,NN7120,(prn-1))!=0 ) {
      printf(" Could be a problem with gen7120-%d-%d or CAMAC.  \
Check hardware! \n\n", CC, NN7120);
      return (-1); }

  if ( prn==3 ) {
    printf("\n");
    printf(" dt1  = %d ns \n", gen[CC][NN7120].dt1);
    printf(" dt2  = %d ns \n", gen[CC][NN7120].dt2);
    printf(" ddt  = %d ns \n", gen[CC][NN7120].ddt);
    printf(" rate = %d Hz \n", gen[CC][NN7120].rate);
    printf(" N    = %d    \n", gen[CC][NN7120].N);
    printf(" Q    = %d mV \n", gen[CC][NN7120].Q);
    printf(" genRunning   = %d   \n", gen[CC][NN7120].genRunning);
  }

  cpc = clkgen3660readCPC (CC, NN3660, &q);
  d   = clkgen3660readCSR (CC, NN3660, &q);
  d1  = (d & 0x40) >> 6;
  d2  = (d & 0x80) >> 7;

  if ( d1 == 1 ) {
    stat=2;
    if ( prn!=0 )  printf("\n Pulser is in continuous mode. \n\n");
    if ( prn==2 || prn==3 )
      printf(" delta_time=%d ns,    rate=%d Hz \n\n",
	     gen[CC][NN7120].dt1, gen[CC][NN7120].rate);
  }
  else if ( d2 == 1 ) {
    stat=1;
    if ( prn==1 )  printf("%d\n", cpc);
    if ( prn==2 || prn==3 ) {
      printf("\n Pulser is active. \n\n");
      printf(" Done %d pulses for delta_time=%d ns and rate=%d Hz\n\n",
	     cpc, gen[CC][NN7120].dt1, gen[CC][NN7120].rate); }
  }
  else {
    stat=0;
    if ( prn==1 )  printf("\n");
    if ( prn==2 || prn==3 )
      printf("\n Pulser is not active. \n\n");
  }

  return (stat);
}

/**********************************************
 * Check & Prestart Generator 7120:
 *   1) check gen7120 state;
 *   2) check correctness of input parameter;
 *   3) load converted delta_time.
 */
int
gen7120prestart ()
{
  int t, q;

/****** Check gen7120 state & get gen[CC][NN7120].R[1-8] ******/
  if ( gen7120switchstat(CC,NN7120,0)!=0 ) {
    printf(" Could be a problem with gen7120-%d-%d or CAMAC.  \
Check hardware! \n\n", CC, NN7120);
    return (1); }

/****** Check correctness of input parameters ******/
  if ( gen7120checkinputs(CC,NN7120,0)!=0 )  return (1);

/****** Convert delta_time for loading to gen7120  ******/
  t=(gen[CC][NN7120].dt1*gen[CC][NN7120].TDAC)/gen[CC][NN7120].TRange;

  gen7120clearInh (CC,&q);              /* take off inhibit   */
  if ( gen[CC][NN7120].TDAC==65535 )    /* load converted dt1 */
    gen7120set16DAC (CC,NN7120,t,&q);   /*   in 16-bit DAC    */
  else                                  /*       OR           */
    gen7120set8DAC  (CC,NN7120,t,&q);   /*   in 8-bit DAC     */

  return (0);
}

/**********************************************************
 * Prestart Clock Generator 3660:
 *   1) based on "rate" and "N" preset gen3660 registers;
 *   2) write program to gen3660 RAM;
 *   3) initialize 3660 to the mode of "Ready to Start".
 */
void
gen3660prestart ()
{
  int q;

/****** Initialization of clk.gen3660 parameters ******/
  fsd_1 = fsd_2 = fsd_3 = fsd_4 = csr = 0x0;

/****** Presetting for Step=1, Word=1 and CSR ******/
  if ( gen[CC][NN7120].rate<160 ) {
    csr   = 0x1;  /* choose 10.24 MHz internal clock */
    csr   = csr | (1<<2);  /* divide Base Clock by 256 */
    fsd_1 = (unsigned short) (40000/gen[CC][NN7120].rate);
  }
  else {
    csr   = 0x0; /* choose 10.00 MHz internal clock */
    fsd_1 = (unsigned short) (10000000/gen[CC][NN7120].rate);
  }

/****** Presetting for Step=1; Words=2,3,4 and CSR ******/
  if ( gen[CC][NN7120].N==-1 || gen[CC][NN7120].N>16000000 ) {
    /* mode of continuous generation will be set */
    gen[CC][NN7120].genRunning=1;  /* set gen7120 busy for others */
    fsd_2 = 0x81; /* set EOL and TS=1*/
    fsd_3 = 0x2;
    csr   = csr | (1<<6); /* set Recycle Mode */
  }
  else {
    fsd_2 = 0x81; /* set EOL, not LAM at the end of step */
    fsd_3 = (unsigned short) (gen[CC][NN7120].N &  0xffff);
    fsd_4 = (unsigned short) (gen[CC][NN7120].N >> 16);
  }

/****** Special bit setting (if ddt!=0) ******/
  if ( 0<gen[CC][NN7120].ddt ) {
    fsd_2 = fsd_2 | (1<<4); /* enable LAM at the end of step */
    gen[CC][NN7120].genRunning=1;  /* set gen7120 busy for others */
  }

/******   Programming of the Clock Generator 3660      ******/
/*   to provide external pulse for Phillips generator 7120  */

  clkgen3660resetALL (CC, NN3660, &q);    /* 3660 to Inactive state */
  clkgen3660writeRAP (CC, NN3660, 0, &q); /* set RAP to zero */

/****** Write program to RAM (1 step = 4 words) ******/
  clkgen3660writeFSD (CC, NN3660, fsd_1, &q);
  clkgen3660writeFSD (CC, NN3660, fsd_2, &q);
  clkgen3660writeFSD (CC, NN3660, fsd_3, &q);
  clkgen3660writeFSD (CC, NN3660, fsd_4, &q);

/***** Initialize 3660 RAM Address Pointer to first step ******/
  clkgen3660writeRAP (CC, NN3660, 0, &q);

}

/*************************
 *   Interrupt handler
 */
STATUS
ks2917inthandler ()
{
  int t, q;

  clkgen3660clearLS (CC,NN3660,&q);  /* Clear LAM */
return(0);

/****** Check end of steps ******/
  if ( (gen[CC][NN7120].dt1+gen[CC][NN7120].ddt)>gen[CC][NN7120].dt2 ) {
    logMsg("ks2917inthandler:  dt1=%dns, ddt=%dns, dt2=%dns\n",
	   gen[CC][NN7120].dt1,gen[CC][NN7120].ddt,gen[CC][NN7120].dt2,4,5,6);
    logMsg("ks2917inthandler:  (dt1+ddt)>dt2, no more steps.\n",1,2,3,4,5,6);
    ks2917intdisable ();
    logMsg("ks2917inthandler:  interrupt disable\n",1,2,3,4,5,6);
    gen[CC][NN7120].genRunning=0;  /* unhold gen7120 for others */
  }
  else {
    gen[CC][NN7120].genRunning++;  /* increment step-counter    */
    gen[CC][NN7120].dt1+=gen[CC][NN7120].ddt;
    logMsg("ks2917inthandler:  dt1=%dns\n",gen[CC][NN7120].dt1,2,3,4,5,6);

/****** Convert delta_time for loading to gen7120  ******/
    t=(gen[CC][NN7120].dt1*gen[CC][NN7120].TDAC)/gen[CC][NN7120].TRange;

    /* gen7120clearInh (CC,&q); */        /* take off inhibit   */
    if ( gen[CC][NN7120].TDAC==65535 )    /* load converted dt1 */
      gen7120set16DAC (CC,NN7120,t,&q);   /*   in 16-bit DAC    */
    else                                  /*       OR           */
      gen7120set8DAC  (CC,NN7120,t,&q);   /*   in 8-bit DAC     */

    ks2917intenable ();                   /* reanable interrupt  */
    csr   = csr | (1<<7);                    /* refresh gen3660 CSR */
    clkgen3660writeCSR (CC,NN3660,csr,&q);   /* 3660 to Active mode */
  }

  logMsg("ks2917inthandler:  done\n",1,2,3,4,5,6);

  return(0);
}

/***************************************************/
void
ks2917intinit ()
{
  int            ext, q;
  int            lam_mask;
  unsigned char  *ks2917_ivr;
  VOIDFUNCPTR    *vector;

/****** Set LAM Mask Register ******/
  cdreg(&ext, BB, CC, 30, 13);
  lam_mask = 0 | (1<<(NN3660-1));
  cfsa(17, ext, &lam_mask, &q);

  ks2917_ivr = (unsigned char *) (ks2917_BADR+0x49);
  vector     = (VOIDFUNCPTR *) INUM_TO_IVEC(*ks2917_ivr);

  logMsg("\nks2917intinit:  INUM=0x%x(%d) -> IVEC=0x%x(%d)\n",
	 (*ks2917_ivr),(*ks2917_ivr),(int)vector,(int)vector,5,6);

#ifdef VXWORKS
  intDisconnect(vector);
#endif
  intConnect(vector,(VOIDFUNCPTR)ks2917inthandler,0);
  sysIntEnable(MYINT_LEVEL);

  logMsg("ks2917intinit:  done\n",1,2,3,4,5,6);
}

/***************************************************/
void
ks2917intenable ()
{
  int            ext, q;
  int            en_s_req;
  unsigned short *ks2917_icr;

/******  Enable LAM in clk.gen3660 ******/
  clkgen3660enbLAM (CC, NN3660, &q);

/****** Set Enable Service Request in 3922 Cr.Controller ******/
  cdreg(&ext, BB, CC, 30, 0);
  en_s_req = 0 | (1<<8);
  cfsa(17, ext, &en_s_req, &q);

/****** Enable interrupt in ks_2917 ******/
  ks2917_icr = (unsigned short *) (ks2917_BADR+0x40);
  *ks2917_icr = (0x0) | (1<<4) | (1<<3) | (MYINT_LEVEL);
}

/***************************************************/
void
ks2917intdisable ()
{
  unsigned short *ks2917_icr;

  ks2917_icr = (unsigned short *) (ks2917_BADR+0x40);
  *ks2917_icr = 0x0;
}

#else

pulserint_dummy()
{
  return;
}

#endif
