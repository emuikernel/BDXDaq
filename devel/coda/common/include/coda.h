#ifndef CODA_H
#define CODA_H

/*
 * Discription: coda.h - header file for CLAS-CODA environment
 *
 * Author of sources :      Serguei Boiarinov
 *                          CLAS Detector Software Group
 *
 * Date of last revision :  5 November 1996
 *
 */

#ifdef VXWORKSPPC
#ifndef VXWORKS
#define VXWORKS
#endif
#endif

/*
#ifndef _ROLDEFINED
#define _ROLDEFINED
#ifndef VXWORKS
#ifndef _CODA_DA_H
typedef void (*VOIDFUNCPTR) ();
#endif
#endif
#endif
*/

#ifndef VXWORKS
#include "bosio.h"
#define NBCS 70000
#endif


#define MAXPLACE    10       /* max places number                           */
#define MAXID       16000    /* max id number for non-DC detectors          */
#define MAXIDSIM    10000    /* max id number for simulation                */
#define MAXFILLEN   256      /* max file name length (with path)            */
#define MAXSTRLEN   80       /* max string length in *.tab files            */
#define MAXSTATWD   50       /* max number of statistic words in RCST bank  */
#define MAXCHANNEL  192      /* max channel number                          */
#define NUMCHANNELS	96       /* the number of channels in board             */

#define  FIRST_SLOT 0        /* Number of the first slot in a crate         */
#define  LAST_SLOT 25        /* Number of the last slot in a crate          */
#define  NUM_SLOTS 26        /* Number of slots in a crate                  */

#define NUSERBUFFERS 100     /* Number of pointers to the user buffers */
                             /* (see tttrans.c) */

/*  DC_REGION 1 */
#define R1_dpres       20       /* double pulse resolution */
#define R1_jitter      80       /* generator pulse width jitter */
#define R1_width1      700      /* first XORed wire pulse width (width sub 0) */
#define R1_width2      900      /* second XORed wire pulse width (width sub 1) */
/*  DC_REGION 2 & 3 */
#define R23_dpres       20     /* double pulse resolution */
#define R23_jitter      160    /* generator pulse width jitter */
#define R23_width1      2000   /* first XORed wire pulse width (width sub 0) */
#define R23_width2      2400   /* second XORed wire pulse width (width sub 1) */

/* BOS data structures */

typedef struct dcdata *DCDataPtr;
typedef struct dcdata
{
  unsigned short id;		/* layer number * 256 + wire number */
  unsigned short tdc;		/* tdc */
  unsigned short adc;		/* adc */
} DCData;

typedef struct ccdata *CCDataPtr;
typedef struct ccdata
{
  unsigned short id;		/* section number */
  unsigned short tdc;		/* tdc */
  unsigned short adc;		/* adc */
} CCData;

typedef struct scdata *SCDataPtr;
typedef struct scdata
{
  unsigned short id;		/* slab number */
  unsigned short tdc1;		/* "left" tdc */
  unsigned short adc1;		/* "left" adc */
  unsigned short tdc2;		/* "right" tdc */
  unsigned short adc2;		/* "right" adc */
} SCData;

typedef struct ecdata *ECDataPtr;
typedef struct ecdata
{
  unsigned short id;		/* layer number * 256 + strip number */
  unsigned short tdc;		/* tdc */
  unsigned short adc;		/* adc */
} ECData;




/* board data structures */
typedef struct CAEN1190Head *PTDCHeadPtr;
typedef struct CAEN1190Head
{
#ifdef VXWORKS
  unsigned zero    :  5;
  unsigned count   : 22;
  unsigned slot    :  5;
#else
  unsigned slot    :  5;
  unsigned count   : 22;
  unsigned zero    :  5;
#endif
} PTDCHead;

/*---------------------------------------------------------------
                      1190 MODIFIED HEADER WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| |_________________________________________| |________|
     0                     word count                  geographic
                                                        address */

typedef struct CAEN1190 *PTDCPtr;
typedef struct CAEN1190
{
#ifdef VXWORKS
  unsigned slot    :  5;
  unsigned edge    :  1;
  unsigned channel :  7;
  unsigned data    : 19;
#else
  unsigned data    : 19;
  unsigned channel :  7;
  unsigned edge    :  1;
  unsigned slot    :  5;
#endif
} PTDC;

/*---------------------------------------------------------------
                       1190 MODIFIED DATA WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| | |___________| |___________________________________|
geographic |   channel #                     data
 address   |   (0..127)
           |
      sign of the edge
     (0-leader, 1-trailer)		*/


typedef struct CAEN1290 *PTDC1Ptr;
typedef struct CAEN1290
{
#ifdef VXWORKS
  unsigned slot    :  5;
  unsigned edge    :  1;
  unsigned channel :  5;
  unsigned data    : 21;
#else
  unsigned data    : 21;
  unsigned channel :  5;
  unsigned edge    :  1;
  unsigned slot    :  5;
#endif
} PTDC1;

/*---------------------------------------------------------------
                       1290 MODIFIED DATA WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| | |_______| |_______________________________________|
geographic |   channel #                     data
 address   |   (0..31)
           |
      sign of the edge
     (0-leader, 1-trailer)		*/




typedef struct LeCroy1877Head *MTDCHeadPtr;
typedef struct LeCroy1877Head
{
  unsigned slot    :  5;
  unsigned empty0  :  3;
  unsigned empty1  :  8;
  unsigned empty   :  5;
  unsigned count   : 11;
} MTDCHead;

/*---------------------------------------------------------------
                      1877 HEADER WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| |___| |_____________| |   |___| |___________________|
geographic   0          1      | |  buffer      word count
 address                       | |
 |            OR               | parity
 |_____________________________|
       logical address						*/

typedef struct LeCroy1877 *MTDCPtr;
typedef struct LeCroy1877
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned edge    :  1;
  unsigned data    : 16;
} MTDC;

/*---------------------------------------------------------------
                       1877 DATA WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| | | | |___________| | |_____________________________|
geographic | | |channel number |               data
 address   | |_|    (0..95)    |
           | buffer            |
           |                  sign of the edge
          parity            (0-leader,1-trailer)		*/

typedef struct LeCroy1872A *TDCPtr;
typedef struct LeCroy1872A
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned empty   :  1;
  unsigned channel :  7;
  unsigned data    : 16;
} TDC;

/*---------------------------------------------------------------
                         1872A DATA WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| |   | | |___________| |___| | |_____________________|
geographic |   | |channel number   0   |          data
 address   |___| |    (0..63)         W(?)      (0..4095)
           event |
          number |
                 |
                 range						*/

typedef struct LeCroy1881MHead *ADCHeadPtr;
typedef struct LeCroy1881MHead
{
  unsigned slot    :  5;
  unsigned empty   : 20;
  unsigned count   :  7;
} ADCHead;

/*---------------------------------------------------------------
                        1881M HEADER WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| |___________________| | |_| |_________| |___________|
geographic           0         | |  0 buffer number  word count
 address                       | |
 |  OR                         | word parity
 |_____________________________|
       logical address						*/

typedef struct LeCroy1881M *ADCPtr;
typedef struct LeCroy1881M
{
  unsigned slot    :  5;
  unsigned type    :  3;
  unsigned channel :  7;
  unsigned empty   :  3;
  unsigned data    : 14;
} ADC;

/*---------------------------------------------------------------
                         1881M DATA WORD FORMAT
-----------------------------------------------------------------
 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1           
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
-----------------------------------------------------------------
 |_______| | | | |___________| |___| |_________________________|
geographic | | |channel number   0              data
 address   | |_|    (0..63)
           | buffer
           | number
           |(MODULO 4)
           |
           even word parity					*/

typedef struct LeCroy *INDPtr;
typedef struct LeCroy
{
  unsigned slot    :  5;
  unsigned empty   :  3;
  unsigned channel :  8;
  unsigned empty2  :  2;
  unsigned data    : 14;
} IND;

/* pre structure */

#define NPREBANKS 100
#define NPREFRAGS 100

typedef struct preStruct *preS;
typedef struct preStruct
{
  unsigned long  name;                  /* name of BOS bank */
  unsigned short nr;                    /* bank number */
  unsigned long  idnr;                  /* BOS bank id */
  long           n;                     /* the number of pieces */
  INDPtr         begin[NPREFRAGS];      /* piece begins */
  INDPtr         end[NPREFRAGS];        /* piece ends */
} preStructS;

/* some useful structures */

typedef struct intchar *IntcharPtr;
typedef struct intchar
{
  unsigned char ch0;
  unsigned char ch1;
  unsigned char ch2;
  unsigned char ch3;
} Intchar;

typedef struct Rcst *RCSTPtr;
typedef struct Rcst
{
  unsigned long slot;             /* slot number */
  unsigned long event;            /* how many events from this slot */
  unsigned long overflow;         /* how many overflow events in this slot */
  unsigned long stat[MAXSTATWD];  /* performance statistic */
} RCST;


#define NHC     2 /* CODA header length (2 CODA words) */
#define	NHWRES 10 /* reserve space */

/* compressed banks definitions */
#define ILEN   -2       /* position of the exclusive length, used as bank index */
#define NHEAD   2       /* the number of header words in compressed format */
#define IDNR    1       /* position of the ID + NR in compressed format */
#define IDATA   0       /* position of NROW in compressed format */


/*                   BOS FRAGMENT FORMAT

    HIGH BYTE                                      LOW BYTE
----------------------------------------------------------------
                        fragment length
----------------------------------------------------------------
 (code&status) |    roc_id     |      1        |   counter
----------------------------------------------------------------
                             NHEAD
----------------------------------------------------------------
                             FMT[0]
----------------------------------------------------------------
                             FMT[1]
----------------------------------------------------------------
                             ......
----------------------------------------------------------------
                              CODE
----------------------------------------------------------------
                             NAMEXT
----------------------------------------------------------------
                              NCOL
----------------------------------------------------------------
                              NROW
----------------------------------------------------------------
                              NAME
----------------------------------------------------------------
                              NR
----------------------------------------------------------------
                              NXT
----------------------------------------------------------------
                             NDATA
----------------------------------------------------------------
                          data word 1
----------------------------------------------------------------
                          data word 2
----------------------------------------------------------------
                          ...........
----------------------------------------------------------------
                          data word NDATA
----------------------------------------------------------------
 ..............................................................
 ..............................................................
 ..............................................................
----------------------------------------------------------------*/




#define SLOT64 31

#define ABS(x)      ((x) < 0 ? -(x) : (x))

#define TIMER_VAR \
  unsigned long startTim, stopTim, dTim; \
  static int nTim, Tim, rmsTim, minTim=10000000, maxTim, normTim=1

#define TIMER_NORMALIZE \
if(normTim==1) \
{ \
  normTim = sysGetBusSpd() / 4000000; \
  printf("normTim=%d\n",normTim); \
}

#define TIMER_START \
{ \
  startTim = sysTimeBaseLGet(); \
}

/* use (..,-1) if want to disable histos */
#define TIMER_STOP(whentoprint_macros,histid_macros) \
{ \
  stopTim = sysTimeBaseLGet(); \
  if(stopTim > startTim) \
  { \
    nTim ++; \
    dTim = stopTim - startTim; \
    if(histid_macros >= 0) \
    { \
      uthfill(histi, histid_macros, (int)(dTim/normTim), 0, 1); \
    } \
    Tim += dTim; \
    rmsTim += dTim*dTim; \
    minTim = minTim < dTim ? minTim : dTim; \
    maxTim = maxTim > dTim ? maxTim : dTim; \
    /*logMsg("good: %d %ud %ud -> %d\n",nTim,startTim,stopTim,Tim,5,6);*/ \
    if(nTim == whentoprint_macros) \
    { \
      logMsg("timer: %d microsec (min=%d max=%d rms**2=%d)\n", \
                Tim/nTim/normTim,minTim/normTim,maxTim/normTim, \
                ABS(rmsTim/nTim-Tim*Tim/nTim/nTim)/normTim/normTim,5,6); \
      nTim = Tim = 0; \
    } \
  } \
  else \
  { \
    /*logMsg("bad:  %d %ud %ud -> %d\n",nTim,startTim,stopTim,Tim,5,6);*/ \
  } \
}



#define PROFILE_VAR \
  unsigned long startPTim, stopPTim, dPTim, dPTim1; \
  static int dtPTim, nPTim, PTim, minPTim=10000000, maxPTim; \
  static int nPTim1, PTim1, minPTim1=10000000, maxPTim1, normPTim=1

#define PROFILE_NORMALIZE \
if(normPTim==1) \
{ \
  normPTim = sysGetBusSpd() / 4000000; \
  printf("normPTim=%d\n",normTim); \
}

#define PROFILE_START \
{ \
  startPTim = sysTimeBaseLGet(); \
}

#define PROFILE_STOP(whentoprint_macros) \
{ \
  stopPTim = sysTimeBaseLGet(); \
  if(stopPTim > startPTim) \
  { \
    nPTim ++; \
    dPTim1 = dPTim = stopPTim - startPTim; \
    /* if execution time so far smaller then totalPTim1, run some more */ \
    if(dPTim1 < totalPTim1) nPTim1 ++; \
    while(dPTim1 < totalPTim1) \
    { \
      dtPTim = sysTimeBaseLGet() - startPTim; \
      if(dtPTim > 0) \
      { \
        dPTim1 = dtPTim; \
      } \
      else \
      { \
        break; \
      } \
    } \
    PTim += dPTim; \
    minPTim = minPTim < dPTim ? minPTim : dPTim; \
    maxPTim = maxPTim > dPTim ? maxPTim : dPTim; \
    PTim1 += dPTim1; \
    minPTim1 = minPTim1 < dPTim1 ? minPTim1 : dPTim1; \
    maxPTim1 = maxPTim1 > dPTim1 ? maxPTim1 : dPTim1; \
    if(nPTim == whentoprint_macros) \
    { \
      logMsg("execution time (microsec): self=%d (min=%d max=%d)\n", \
                PTim/nPTim/normPTim,minPTim/normPTim,maxPTim/normPTim,4,5,6); \
      nPTim = PTim = 0; \
    } \
    if(nPTim1 == whentoprint_macros) \
    { \
      logMsg("execution time (microsec): total=%d (min=%d max=%d)\n", \
                PTim1/nPTim1/normPTim,minPTim1/normPTim,maxPTim1/normPTim,4,5,6); \
      nPTim1 = PTim1 = 0; \
    } \
  } \
}

#ifdef VXWORKSPPC
/* work around PPC problem */
#define EIEIO    __asm__ volatile ("eieio")
#define SynC     __asm__ volatile ("sync")
#endif



#endif
