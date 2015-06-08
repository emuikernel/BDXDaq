
#ifdef VXWORKS

/* gen7120.c  ( for loading in VxWorks )
 *
 *   A set of functions to control the work of Phillips generator 7120.
 *
 *   Main of them :
 *   --------------
 *   gen7120_start (c,n,dt1,dt2,ddt,rate,N) - start gen7120 with different
 *                                            mode of generation:
 *                              a) continuous generation (N=-1),
 *                              b) N pulses for one Start-Stop delay,
 *                              c) bunch of delays with N pulses per each
 *   gen7120_stop (c,n)     - correct stop of gen7120
 *   gen7120_stat (c,n,prn) - get gen7120 status vs prn (Short,Full,Long)
 *   gen7120_Tset (c,n,T)   - set Start-to-Stop time delay
 *   gen7120_Qset (c,n,Q)   - set amplitude for Charge Output
 *   gen7120_getpos ()      - get positions of all gen7120 in polling roc
 *
 *   function inputs:
 *   ----------------
 *      c - CAMAC Crate Controller number (0-7)
 *      n - CAMAC slot number, position of gen7120 (1-24)
 *    dt1 - Start-to-Stop delay, MIN value or sole if ddt=0 (0-10000 nSec)
 *    dt2 - Start-to-Stop delay, MAX value (0-10000 nSec, dt2>dt1)
 *    ddt - step from MIN to MAX (in nSec, 0<=ddt<=dt2-dt1)
 *   rate - frequency of generated Start-Stop signals (1-10000 Hz)
 *      N - number of Start/Stop pairs (N>1 or N=-1 for continuous mode)
 *    prn - print flag: Short status (prn=0), Full(1), Long(2)
 *      T - Start-to-Stop time delay (in nSec)
 *      Q - amplitude of Q-signal (in mV)
 *
 *
 *   SP, 20-Sep-2002
 *
 */

#include <stdio.h>
#include <logLib.h>
#include <ca.h>
#include "genROCparm.h"
#include "gen7120.h"


/* global structure for gen7120 settings */
GEN7120  gen[8][25];


/* running flag of Tick timer3 on the MCChip (Memory controller)    */
/*    after pTickDisable():  pTickRunning = 0(d), 0(x), "(null)"(s) */ 
/*    after pTickEnable():   pTickRunning = 1(d), 1(x), ""(s)       */ 
extern BOOL pTickRunning;



/* take off inhibit in crate#c */
void
gen7120clearInh (int c, int *q)
{
  int ext, data=0;

  cdreg(&ext, BB, c, 30, 0);
  cfsa(17, ext, &data, q);
}

/* read CSR of gen7120 in crate#c & slot#n */
int
gen7120readCSR (int c, int n, int *q, int *x)
{     
  int ext, lstat, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(0, ext, &data, q);
  ctstatus(ext,&lstat);
  *x = (~lstat>>1)&1;
  return(data);
}

/* set 16-bit DAC: W1-W16 */
void
gen7120set16DAC (int c, int n, int data, int *q)
{
  int ext;

  cdreg(&ext, BB, c, n, 0);
  cfsa(16, ext, &data, q);
}

/* set 8-bit DAC: W1-W8 */
void
gen7120set8DAC (int c, int n, int data, int *q)
{
  int ext;

  cdreg(&ext, BB, c, n, 2);
  cfsa(16, ext, &data, q);
}

/***************************************************************
 * Get switches state of gen7120 in crate#c and slot#n:
 *   store CSR bits values in gen[c][n].R[1-8];
 *   return (0) if no errors occurred, or
 *   return (1) if can't read CSR or gen7120 state is NOT valid;
 *     prn=2: print gen7120 switches state and CSR bits values;
 *     prn=1: print only state of gen7120 switches;
 *     prn=0: print only Error messages.
 */
int
gen7120switchstat (int c, int n, int prn)
{
  int csr, q, x, i;
  char *ch[8] = {
    "",
    "Time Range    =",
    "Charge Range  =",
    "Time Mode     =",
    "Primary Out   =",
    "Charge Mode   =",
    "-DC Out full scale =",
    "+DC Out full scale ="};
  char *r[8][2] = {
    {"Not Valid","10 micro Seconds"},
    {"512 nano Seconds","1024 nano Seconds"},
    {"600 pC (4000 mV)","300 pC (2000 mV)"},
    {"CAMAC","Manual"},
    {"Charge Mode","Time Mode"},
    {"CAMAC","Manual"},
    {"5 V","10 V"},
    {"5 V","10 V"}};


/****** Read CSR & Check status of reading ******/
  /* gen7120clearInh(c,&q); */   /* take off inhibit */
  csr = gen7120readCSR (c, n, &q, &x);
  if ( q!=1 || x!=1 ) {
    printf("\n Error: CSR=0x%x, q=%d, x=%d  (should be q=1 and x=1)\n\n",csr,q,x);
    return (1); }

/****** Store CSR bits values in gen[c][n].R[1-8] ******/
  for (i=1; i<=8; i++)
    gen[c][n].R[i] = (csr>>(i-1))&1;

/****** Get Time Range & Check it's validity ******/
  if      ( gen[c][n].R[2]==1 && gen[c][n].R[1]==1 )  gen[c][n].TRange =  1024;
  else if ( gen[c][n].R[2]==1 && gen[c][n].R[1]==0 )  gen[c][n].TRange =   512;
  else if ( gen[c][n].R[2]==0 && gen[c][n].R[1]==1 )  gen[c][n].TRange = 10000;
  else {
    printf("\n Error: Time Range is Not Valid \n\n");
    return (1); }

/****** Get Charge Range ******/
  if ( gen[c][n].R[3]==1 )  gen[c][n].QRange = 2000;
  else                      gen[c][n].QRange = 4000;

/****** Get Primary DAC ******/
  gen[c][n].TDAC = 65535;
  gen[c][n].QDAC = 65535;
  if ( gen[c][n].R[5]==0 )  gen[c][n].TDAC = 255;
  else                      gen[c][n].QDAC = 255;

/****** Check gen7120 operating mode (if prn=0) ******/
  if ( prn==0 )
  for (i=4; i<=6; i+=2) {
    if ( gen[c][n].R[i]==1 ) {
      printf("\n Error:   %s %s \n\n", ch[i-1], r[i-1][1]);
      printf(" Change to CAMAC Mode and restart the program. \n\n");
      return (1); }
  }

/****** Print gen7120 state (prn=1, prn=2) ******/
  if ( prn==1 || prn==2 ) {
    printf("\n State of gen7120-%d-%d switches: \n", c, n);
    printf(" =============================== \n");
    printf(" %s %s \n", ch[1], r[gen[c][n].R[2]][gen[c][n].R[1]]); 
    for (i=2; i<=7; i++)
      printf(" %s %s \n", ch[i], r[i][gen[c][n].R[i+1]]); 
    printf("\n");
    if ( prn==2 ) {
      for (i=1; i<=8; i++)
	printf(" gen[%d][%2d].R[%d] = %d \n", c, n, i, gen[c][n].R[i]);
      printf("\n"); }
  }

  return (0);
}

/*************************************************************
 * Check inputed parameters for gen7120 in crate#c and slot#n:
 *   return (0) if no errors occurred, otherwise
 *   return (1) and printout:
 *     1) what parameter was incorrect and
 *     2) help message.
 */
int
gen7120checkinputs (int c, int n, int prn)
{
  int  checkresult=0;
  char *help = "\n"
    " Usage: gen7120_start ( c, n, dt1, dt2, ddt, rate, N ) \n"
    "        ---------------------------------------------- \n"
    " Where \"c\" - CAMAC Crate Controller number; \n"
    "       \"n\" - CAMAC slot number, position of gen7120; \n"
    "     \"dt1\" - MIN value of Start-to-Stop delay (0-10000 nSec); \n"
    "     \"dt2\" - MAX value of Start-to-Stop delay (0-10000 nSec); \n"
    "     \"ddt\" - STEP to go from MIN to MAX (in nSec), \n"
    "             if \"ddt\"=0, then only one step will be done \n"
    "             with delay=\"dt1\" and \"dt2\" is ignoring; \n"
    "    \"rate\" - frequency of Start/Stop signals (1-10000 Hz); \n"
    "       \"N\" - number of Start/Stop pairs per STEP (N>1), \n"
    "             if \"ddt\"=0 continuous mode can be set, put N=-1. \n";


  if ( gen[c][n].dt1<0 || 10000<gen[c][n].dt1 ) {
    printf("\n Error: dt1<0 OR 10000<dt1 \n\n");
    checkresult = 1;
  }
  else if ( gen[c][n].TRange<gen[c][n].dt1 ) {
    printf("\n Error:   TRange < dt1 \n\n");
    printf(" For current gen7120-%d-%d setting  TRange=%d nSec \n\n",
	   c, n, gen[c][n].TRange);
    checkresult = 1;
  }
  else if ( gen[c][n].dt2<gen[c][n].dt1 || 10000<gen[c][n].dt2 ) {
    if ( gen[c][n].ddt!=0 ) {
      printf("\n Error: dt2<dt1 OR 10000<dt2 \n\n");
      checkresult = 1;
    }
  }
  else if ( gen[c][n].TRange<gen[c][n].dt2 && gen[c][n].ddt!=0 ) {
    printf("\n Error:   TRange < dt2 \n\n");
    printf(" For current gen7120-%d-%d setting  TRange=%d nSec \n\n",
	   c, n, gen[c][n].TRange);
    checkresult = 1;
  }
  else if ( gen[c][n].ddt>(gen[c][n].dt2-gen[c][n].dt1) ||
	    gen[c][n].ddt<0) {
    printf("\n Error: dt1<0 OR (dt2-dt1)<ddt \n\n");
    checkresult = 1;
  }
  else if ( gen[c][n].rate<1 || 10000<gen[c][n].rate ) {
    printf("\n Error: rate<1 OR 10000<rate \n\n");
    checkresult = 1;
  }
  else if ( gen[c][n].N<1 ) {
    if ( gen[c][n].ddt!=0 ) {
      printf("\n Error: N<1 when ddt=%d \n\n", gen[c][n].ddt);
      checkresult = 1;
    }
    else if ( gen[c][n].N!=-1 ) {
      printf("\n Error: N<1  AND  N!=-1 \n\n");
      checkresult = 1;
    }
  }

  if ( checkresult!=0 && prn==1 ) printf("%s \n", help);

  return (checkresult);
}

/***********************************************************
 * Interrupt routine will be called at each timer interrupt:
 *   1) load 8-bit DAC, if necessary;
 *   2) load 16-bit DAC each time of calling,
 *      this executes one cycle of generation;
 *   3) check end-condition and disable interrupt.
 */
void
gen7120inthandler (int arg)
{
  int *cn, q, stop=0;
  int dac8=0, dac16=0;

  cn = (int *)arg;

  /* gen7120clearInh (cn[0],&q); */   /* take off inhibit */
  if ( gen[cn[0]][cn[1]].TDAC==65535 ) {
    dac16 = (gen[cn[0]][cn[1]].dt1*gen[cn[0]][cn[1]].TDAC)/
      gen[cn[0]][cn[1]].TRange;
    gen7120set16DAC (cn[0],cn[1],dac16,&q);
  }
  else {
    dac8  = (gen[cn[0]][cn[1]].dt1*gen[cn[0]][cn[1]].TDAC)/
      gen[cn[0]][cn[1]].TRange;
    dac16 = (gen[cn[0]][cn[1]].Q  *gen[cn[0]][cn[1]].QDAC)/
      gen[cn[0]][cn[1]].QRange;
    gen7120set8DAC  (cn[0],cn[1],dac8,&q);
    gen7120set16DAC (cn[0],cn[1],dac16,&q);
  }

  gen[cn[0]][cn[1]].genRunning++;
  if ( gen[cn[0]][cn[1]].genRunning>gen[cn[0]][cn[1]].N ) {
    if ( gen[cn[0]][cn[1]].ddt!=0) {
      if ( (gen[cn[0]][cn[1]].dt1+gen[cn[0]][cn[1]].ddt)>gen[cn[0]][cn[1]].dt2 )
	stop=1;
      else  {
	gen[cn[0]][cn[1]].dt1+=gen[cn[0]][cn[1]].ddt;
	gen[cn[0]][cn[1]].genRunning=1;
      }
    }
    else if ( gen[cn[0]][cn[1]].N>=1)  stop=1;
  }

  if ( stop ) {
    pTickDisable();
    logMsg("\n Gen7120-%d-%d stopped. \n\n",cn[0],cn[1],0,0,0,0);
    gen[cn[0]][cn[1]].genRunning=0;
  }
}

/***************************************************************
 * Start gen7120 with different mode of generation:
 *   a) continuous generation (N=-1, ddt=0), dt2 not essential;
 *   b) generate "N" pulses with "rate" frequency
 *      for "dt1" Start-Stop delay;
 *   c) if "ddt" not ZERO, generate "N" pulses per each 
 *      delay=(dt1+ddt*i), 0<=i<=(dt2-dt1)/ddt.
 */
int
gen7120_start (int c, int n, int dt1, int dt2, int ddt, int rate, int N)
{
  static int cn[2];

/****** Check if gen7120 is exist ******/
  if ( gen[c][n].genRunning==-1 ) {
    printf("\n Error: There is no gen7120 in crate #%d, slot #%d.\n\n", c, n);
    return (1); }

/****** Check if gen7120 is already in process ******/
  if ( gen[c][n].genRunning!=0 || pTickRunning!=0 ) {
    printf("\n Gen7120-%d-%d or Tick-timer is active.  Call later.\n\n", c, n);
    return (1); }
  else
    gen[c][n].genRunning=1;

/****** Check gen7120 state & get gen[c][n].R[1-8] ******/
  if ( gen7120switchstat(c,n,0)!=0 ) {
    printf(" Could be a problem with gen7120 or CAMAC.  Check hardware! \n\n");
    gen[c][n].genRunning=0;
    return (1); }

/****** Check correctness of input parameters ******/
  gen[c][n].dt1  = dt1;
  gen[c][n].dt2  = dt2;
  gen[c][n].ddt  = ddt;
  gen[c][n].rate = rate;
  gen[c][n].N    = N;
  if ( gen7120checkinputs(c,n,1)!=0 ) {
    gen[c][n].genRunning=0;
    return (1); }

/****** Preset Tick timer3 & interrupt handler ******/
  cn[0] = c;
  cn[1] = n;
  pTickPeriod(1000000/rate);
  pTickConnect((FUNCPTR)gen7120inthandler,(int)cn);
  pTickEnable();

  printf("\n Gen7120-%d-%d has been started. \n\n", c, n);

  return (0);
}

/************************************************
 * Correct stop of gen7120 in crate#c and slot#n
 */
void
gen7120_stop (int c, int n)
{
  if ( gen[c][n].genRunning==-1 )
    printf("\n Error: There is no gen7120 in crate #%d, slot #%d.\n\n", c, n);
  else {
    if ( gen[c][n].genRunning==0 ) {
      printf("\n Gen7120-%d-%d has been already stopped. \n\n", c, n);
      if ( pTickRunning!=0 ) {
	pTickDisable();
	printf(" But, Tick-timer was active. Now it stopped. \n\n");
      }
    }
    else {
      pTickDisable();
      gen[c][n].genRunning=0;
      printf("\n Gen7120-%d-%d stopped. \n\n", c, n);
    }
  }
}

/**********************************************
 * Get state of gen7120 in crate#c and slot#n:
 *   prn=0: print Short Status;
 *   prn=1: print Full Status;
 *   prn=2: print Long Status.
 */
int
gen7120_stat (int c, int n, int prn)
{
  if ( gen[c][n].genRunning==-1 ) {
    printf("\n Error: There is no gen7120 in crate #%d, slot #%d.\n\n", c, n);
    return (1); }

  if ( gen7120switchstat(c,n,prn)!=0 ) {
    printf(" Could be a problem with gen7120 or CAMAC.  Check hardware! \n");
    return (1); }

  if ( prn==2 ) {
    printf("\n");
    printf(" dt1  = %d ns \n", gen[c][n].dt1);
    printf(" dt2  = %d ns \n", gen[c][n].dt2);
    printf(" ddt  = %d ns \n", gen[c][n].ddt);
    printf(" rate = %d Hz \n", gen[c][n].rate);
    printf(" N    = %d    \n", gen[c][n].N);
    printf(" Q    = %d mV \n", gen[c][n].Q);
    printf(" genRunning   = %d   \n", gen[c][n].genRunning);
    printf(" pTickRunning = %d \n\n", pTickRunning);
  }

  if ( gen[c][n].genRunning!=0 ) {
    if ( gen[c][n].N==-1 ) {
      printf("\n Gen7120-%d-%d is in continuous mode. \n\n", c, n);
      if ( prn==1 || prn==2 )
	printf(" delta_time=%d ns,    rate=%d Hz \n\n",
	       gen[c][n].dt1, gen[c][n].rate);
    }
    else {
      printf("\n Gen7120-%d-%d is active. \n\n", c, n);
      if ( prn==1 || prn==2 )
	printf(" Done %d pulses for delta_time=%d ns and rate=%d Hz \n\n",
	       gen[c][n].genRunning, gen[c][n].dt1, gen[c][n].rate);
    }
  }
  else
    printf("\n Gen7120-%d-%d is not active. \n\n", c, n);

  return (0);
}

/**************************************************************
 * Set Start-to-Stop time delay & store it in global structure
 */
int
gen7120_Tset (int c, int n, int T)
{
  int q;

  if ( gen[c][n].genRunning==-1 ) {
    printf("\n Error: There is no gen7120 in crate #%d, slot #%d.\n\n", c, n);
    return (1); }

  if ( gen[c][n].TRange==0 || gen[c][n].QRange==0 )
    if ( gen7120switchstat(c,n,0)!=0 ) {
      printf(" Could be a problem with gen7120 or CAMAC.  Check hardware! \n\n");
      return (1); }

  if ( gen[c][n].TRange<T ) {
    printf("\n Error:   TRange < T \n\n");
    printf(" For current gen7120-%d-%d setting  TRange=%d nSec \n\n",
	   c, n, gen[c][n].TRange);
    return (1); }

  /* gen7120clearInh (c,&q); */   /* take off inhibit */
  if ( gen[c][n].TDAC==65535 )
    gen7120set16DAC (c,n,(T*gen[c][n].TDAC)/gen[c][n].TRange,&q);
  else
    gen7120set8DAC  (c,n,(T*gen[c][n].TDAC)/gen[c][n].TRange,&q);

  gen[c][n].dt1=T;
  printf("\n T=%d ns set for gen7120-%d-%d. \n\n", T, c, n);

  return (0);
}

/*****************************************************************
 * Set amplitude for Charge Output & store it in global structure
 */
int
gen7120_Qset (int c, int n, int Q)
{
  int q;

  if ( gen[c][n].genRunning==-1 ) {
    printf("\n Error: There is no gen7120 in crate #%d, slot #%d.\n\n", c, n);
    return (1); }

  if ( gen[c][n].TRange==0 || gen[c][n].QRange==0 )
    if ( gen7120switchstat(c,n,0)!=0 ) {
      printf(" Could be a problem with gen7120 or CAMAC.  Check hardware! \n\n");
      return (1); }

  if ( gen[c][n].QRange<Q ) {
    printf("\n Error:   QRange < Q \n\n");
    printf(" For current gen7120-%d-%d setting  QRange=%d mV \n\n",
	   c, n, gen[c][n].QRange);
    return (1); }

  /* gen7120clearInh (c,&q); */   /* take off inhibit */
  if ( gen[c][n].QDAC==65535 )
    gen7120set16DAC (c,n,(Q*gen[c][n].QDAC)/gen[c][n].QRange,&q);
  else
    gen7120set8DAC  (c,n,(Q*gen[c][n].QDAC)/gen[c][n].QRange,&q);

  gen[c][n].Q=Q;
  printf("\n Q=%d mV set for gen7120-%d-%d. \n\n", Q, c, n);

  return (0);
}

/**********************************************
 * Get positions of all gen7120 in polling roc
 */
void
gen7120_getpos ()
{
  int i, j;

  for(i=0; i<8; i++)
    for(j=0; j<25; j++)
      if ( gen[i][j].genRunning!=-1 )
	printf(" gen7120 exist in CAMAC crate #%d  slot #%d \n", i, j);
  printf("\n");
}

/* initialization of global structure */
void
gen7120initglob ()
{
  int i, j, k;

  for(i=0; i<8; i++)
    for(j=0; j<25; j++) {
      gen[i][j].dt1  = 0;
      gen[i][j].dt2  = 0;
      gen[i][j].ddt  = 0;
      gen[i][j].rate = 0;
      gen[i][j].N    = 0;
      for(k=0; k<=8; k++)  gen[i][j].R[k] = 0;
      gen[i][j].TDAC   = 0;
      gen[i][j].QDAC   = 0;
      gen[i][j].TRange = 0;
      gen[i][j].QRange = 0;
      gen[i][j].Q      = 0;
      gen[i][j].genRunning = -1;
    }
}

/* set any of global variable from global structure */
void
gen7120setglobVar (int c, int n, char *ch, int val, int i)
{
  if (strcmp(ch,"dt1")        == 0)  gen[c][n].dt1        = val;
  if (strcmp(ch,"dt2")        == 0)  gen[c][n].dt2        = val;
  if (strcmp(ch,"ddt")        == 0)  gen[c][n].ddt        = val;
  if (strcmp(ch,"rate")       == 0)  gen[c][n].rate       = val;
  if (strcmp(ch,"N")          == 0)  gen[c][n].N          = val;
  if (strcmp(ch,"R")          == 0)  gen[c][n].R[i]       = val;
  if (strcmp(ch,"TDAC")       == 0)  gen[c][n].TDAC       = val;
  if (strcmp(ch,"QDAC")       == 0)  gen[c][n].QDAC       = val;
  if (strcmp(ch,"TRange")     == 0)  gen[c][n].TRange     = val;
  if (strcmp(ch,"QRange")     == 0)  gen[c][n].QRange     = val;
  if (strcmp(ch,"Q")          == 0)  gen[c][n].Q          = val;
  if (strcmp(ch,"genRunning") == 0)  gen[c][n].genRunning = val;
}

/* get any of global variable from global structure */
int
gen7120getglobVar (int c, int n, char *ch, int i)
{
  int  val = -1;

  if (strcmp(ch,"dt1")        == 0)  val = gen[c][n].dt1;
  if (strcmp(ch,"dt2")        == 0)  val = gen[c][n].dt2;
  if (strcmp(ch,"ddt")        == 0)  val = gen[c][n].ddt;
  if (strcmp(ch,"rate")       == 0)  val = gen[c][n].rate;
  if (strcmp(ch,"N")          == 0)  val = gen[c][n].N;
  if (strcmp(ch,"TDAC")       == 0)  val = gen[c][n].TDAC;
  if (strcmp(ch,"QDAC")       == 0)  val = gen[c][n].QDAC;
  if (strcmp(ch,"TRange")     == 0)  val = gen[c][n].TRange;
  if (strcmp(ch,"QRange")     == 0)  val = gen[c][n].QRange;
  if (strcmp(ch,"Q")          == 0)  val = gen[c][n].Q;
  if (strcmp(ch,"genRunning") == 0)  val = gen[c][n].genRunning;
  if (strcmp(ch,"R")          == 0) {
    val = gen[c][n].R[i];
    printf(" gen[%d][%d].R[%d] = %d \n", c, n, i, gen[c][n].R[i]);
  }
  else
    printf(" gen[%d][%d].%s = %d \n", c, n, ch, val);

  return (val);
}

/* print set of global variables for "c" & "n" */
void
gen7120prnglobVar (int c, int n)
{
  int k;
  printf(" gen[%d][%2d].dt1  = %d \n", c, n, gen[c][n].dt1);
  printf(" gen[%d][%2d].dt2  = %d \n", c, n, gen[c][n].dt2);
  printf(" gen[%d][%2d].ddt  = %d \n", c, n, gen[c][n].ddt);
  printf(" gen[%d][%2d].rate = %d \n", c, n, gen[c][n].rate);
  printf(" gen[%d][%2d].N    = %d \n", c, n, gen[c][n].N);
  for(k=0; k<=8; k++)
    printf(" gen[%d][%2d].R[%d] = %d \n", c, n, k, gen[c][n].R[k]);
  printf(" gen[%d][%2d].TDAC       = %d \n", c, n, gen[c][n].TDAC);
  printf(" gen[%d][%2d].QDAC       = %d \n", c, n, gen[c][n].QDAC);
  printf(" gen[%d][%2d].TRange     = %d \n", c, n, gen[c][n].TRange);
  printf(" gen[%d][%2d].QRange     = %d \n", c, n, gen[c][n].QRange);
  printf(" gen[%d][%2d].Q          = %d \n", c, n, gen[c][n].Q);
  printf(" gen[%d][%2d].genRunning = %d \n", c, n, gen[c][n].genRunning);
}

#else

gen7120_dummy()
{
  return;
}

#endif
