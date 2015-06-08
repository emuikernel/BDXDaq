
/* dcrb1.c - first readout list for RCDB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

#define ROL_NAME__ "DCRB1"

/* ts control */
#ifndef STANDALONE
#define TRIG_SUPV
#endif

/* polling mode if needed 
#define POLLING_MODE
*/

/* name used by loader */
#ifdef STANDALONE
#define INIT_NAME dcrb1_standalone__init
#else
#define INIT_NAME dcrb1__init
#endif

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "dcrb.h"


/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG


/* main TI board */
#define TIRADR   0x0ed0

static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];

/* pointer to TTS structures for current roc */
static/*extern*/ TTSPtr ttp1;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 0;

/* histogram area */
static UThisti histi[NHIST];
static int neventh = 0;


/*lookup table for dcrb tests

LAST ADB board:

#bank	sector#	sd1#	sd2#	ind#	tag#	stat#	roc#	slot#	type	channel#
DC0 	2	31	181	0	23	0	3	23	1877	48
DC0 	2	33	181	0	23	0	3	23	1877	49
DC0 	2	35	181	0	23	0	3	23	1877	50
DC0 	2	36	181	0	23	0	3	23	1877	51
DC0 	2	34	181	0	23	0	3	23	1877	52
DC0 	2	32	181	0	23	0	3	23	1877	53
DC0 	2	31	182	0	23	0	3	23	1877	54
DC0 	2	33	182	0	23	0	3	23	1877	55
DC0 	2	35	178	0	23	0	3	23	1877	56
DC0 	2	36	178	0	23	0	3	23	1877	57
DC0 	2	34	178	0	23	0	3	23	1877	58
DC0 	2	32	178	0	23	0	3	23	1877	59
DC0 	2	31	179	0	23	0	3	23	1877	60
DC0 	2	33	179	0	23	0	3	23	1877	61
DC0 	2	35	179	0	23	0	3	23	1877	62
DC0 	2	36	179	0	23	0	3	23	1877	63

DC0 	2	34	183	0	23	0	3	23	1877	64
DC0 	2	32	183	0	23	0	3	23	1877	65
DC0 	2	31	184	0	23	0	3	23	1877	66
DC0 	2	33	184	0	23	0	3	23	1877	67
DC0 	2	35	184	0	23	0	3	23	1877	68
DC0 	2	36	184	0	23	0	3	23	1877	69
DC0 	2	34	184	0	23	0	3	23	1877	70
DC0 	2	32	184	0	23	0	3	23	1877	71
DC0 	2	31	189	0	23	0	3	23	1877	72
DC0 	2	33	189	0	23	0	3	23	1877	73
DC0 	2	35	189	0	23	0	3	23	1877	74
DC0 	2	36	189	0	23	0	3	23	1877	75
DC0 	2	34	189	0	23	0	3	23	1877	76
DC0 	2	32	189	0	23	0	3	23	1877	77
DC0 	2	31	190	0	23	0	3	23	1877	78
DC0 	2	33	190	0	23	0	3	23	1877	79

DC0 	2	35	186	0	23	0	3	23	1877	80
DC0 	2	36	186	0	23	0	3	23	1877	81
DC0 	2	34	186	0	23	0	3	23	1877	82
DC0 	2	32	186	0	23	0	3	23	1877	83
DC0 	2	31	187	0	23	0	3	23	1877	84
DC0 	2	33	187	0	23	0	3	23	1877	85
DC0 	2	35	187	0	23	0	3	23	1877	86
DC0 	2	36	187	0	23	0	3	23	1877	87
DC0 	2	34	191	0	23	0	3	23	1877	88
DC0 	2	32	191	0	23	0	3	23	1877	89
DC0 	2	31	192	0	23	0	3	23	1877	90
DC0 	2	33	192	0	23	0	3	23	1877	91
DC0 	2	35	192	0	23	0	3	23	1877	92
DC0 	2	36	192	0	23	0	3	23	1877	93
DC0 	2	34	192	0	23	0	3	23	1877	94
DC0 	2	32	192	0	23	0	3	23	1877	95

DC0 	2	31	177	1	23	0	3	23	1877	48
DC0 	2	33	177	1	23	0	3	23	1877	49
DC0 	2	35	177	1	23	0	3	23	1877	50
DC0 	2	36	177	1	23	0	3	23	1877	51
DC0 	2	34	177	1	23	0	3	23	1877	52
DC0 	2	32	177	1	23	0	3	23	1877	53
DC0 	2	31	178	1	23	0	3	23	1877	54
DC0 	2	33	178	1	23	0	3	23	1877	55
DC0 	2	35	182	1	23	0	3	23	1877	56
DC0 	2	36	182	1	23	0	3	23	1877	57
DC0 	2	34	182	1	23	0	3	23	1877	58
DC0 	2	32	182	1	23	0	3	23	1877	59
DC0 	2	31	183	1	23	0	3	23	1877	60
DC0 	2	33	183	1	23	0	3	23	1877	61
DC0 	2	35	183	1	23	0	3	23	1877	62
DC0 	2	36	183	1	23	0	3	23	1877	63

DC0 	2	34	179	1	23	0	3	23	1877	64
DC0 	2	32	179	1	23	0	3	23	1877	65
DC0 	2	31	180	1	23	0	3	23	1877	66
DC0 	2	33	180	1	23	0	3	23	1877	67
DC0 	2	35	180	1	23	0	3	23	1877	68
DC0 	2	36	180	1	23	0	3	23	1877	69
DC0 	2	34	180	1	23	0	3	23	1877	70
DC0 	2	32	180	1	23	0	3	23	1877	71
DC0 	2	31	185	1	23	0	3	23	1877	72
DC0 	2	33	185	1	23	0	3	23	1877	73
DC0 	2	35	185	1	23	0	3	23	1877	74
DC0 	2	36	185	1	23	0	3	23	1877	75
DC0 	2	34	185	1	23	0	3	23	1877	76
DC0 	2	32	185	1	23	0	3	23	1877	77
DC0 	2	31	186	1	23	0	3	23	1877	78
DC0 	2	33	186	1	23	0	3	23	1877	79

DC0 	2	35	190	1	23	0	3	23	1877	80
DC0 	2	36	190	1	23	0	3	23	1877	81
DC0 	2	34	190	1	23	0	3	23	1877	82
DC0 	2	32	190	1	23	0	3	23	1877	83
DC0 	2	31	191	1	23	0	3	23	1877	84
DC0 	2	33	191	1	23	0	3	23	1877	85
DC0 	2	35	191	1	23	0	3	23	1877	86
DC0 	2	36	191	1	23	0	3	23	1877	87
DC0 	2	34	187	1	23	0	3	23	1877	88
DC0 	2	32	187	1	23	0	3	23	1877	89
DC0 	2	31	188	1	23	0	3	23	1877	90
DC0 	2	33	188	1	23	0	3	23	1877	91
DC0 	2	35	188	1	23	0	3	23	1877	92
DC0 	2	36	188	1	23	0	3	23	1877	93
DC0 	2	34	188	1	23	0	3	23	1877	94
DC0 	2	32	188	1	23	0	3	23	1877	95

PRE_LAST ADB board:

#bank	sector#	sd1#	sd2#	ind#	tag#	stat#	roc#	slot#	type	channel#
DC0 	2	31	165	0	23	0	3	23	1877	0
DC0 	2	33	165	0	23	0	3	23	1877	1
DC0 	2	35	165	0	23	0	3	23	1877	2
DC0 	2	36	165	0	23	0	3	23	1877	3
DC0 	2	34	165	0	23	0	3	23	1877	4
DC0 	2	32	165	0	23	0	3	23	1877	5
DC0 	2	31	166	0	23	0	3	23	1877	6
DC0 	2	33	166	0	23	0	3	23	1877	7
DC0 	2	35	162	0	23	0	3	23	1877	8
DC0 	2	36	162	0	23	0	3	23	1877	9
DC0 	2	34	162	0	23	0	3	23	1877	10
DC0 	2	32	162	0	23	0	3	23	1877	11
DC0 	2	31	163	0	23	0	3	23	1877	12
DC0 	2	33	163	0	23	0	3	23	1877	13
DC0 	2	35	163	0	23	0	3	23	1877	14
DC0 	2	36	163	0	23	0	3	23	1877	15
DC0 	2	34	167	0	23	0	3	23	1877	16
DC0 	2	32	167	0	23	0	3	23	1877	17
DC0 	2	31	168	0	23	0	3	23	1877	18
DC0 	2	33	168	0	23	0	3	23	1877	19
DC0 	2	35	168	0	23	0	3	23	1877	20
DC0 	2	36	168	0	23	0	3	23	1877	21
DC0 	2	34	168	0	23	0	3	23	1877	22
DC0 	2	32	168	0	23	0	3	23	1877	23
DC0 	2	31	173	0	23	0	3	23	1877	24
DC0 	2	33	173	0	23	0	3	23	1877	25
DC0 	2	35	173	0	23	0	3	23	1877	26
DC0 	2	36	173	0	23	0	3	23	1877	27
DC0 	2	34	173	0	23	0	3	23	1877	28
DC0 	2	32	173	0	23	0	3	23	1877	29
DC0 	2	31	174	0	23	0	3	23	1877	30
DC0 	2	33	174	0	23	0	3	23	1877	31
DC0 	2	35	170	0	23	0	3	23	1877	32
DC0 	2	36	170	0	23	0	3	23	1877	33
DC0 	2	34	170	0	23	0	3	23	1877	34
DC0 	2	32	170	0	23	0	3	23	1877	35
DC0 	2	31	171	0	23	0	3	23	1877	36
DC0 	2	33	171	0	23	0	3	23	1877	37
DC0 	2	35	171	0	23	0	3	23	1877	38
DC0 	2	36	171	0	23	0	3	23	1877	39
DC0 	2	34	175	0	23	0	3	23	1877	40
DC0 	2	32	175	0	23	0	3	23	1877	41
DC0 	2	31	176	0	23	0	3	23	1877	42
DC0 	2	33	176	0	23	0	3	23	1877	43
DC0 	2	35	176	0	23	0	3	23	1877	44
DC0 	2	36	176	0	23	0	3	23	1877	45
DC0 	2	34	176	0	23	0	3	23	1877	46
DC0 	2	32	176	0	23	0	3	23	1877	47

#bank	sector#	sd1#	sd2#	ind#	tag#	stat#	roc#	slot#	type	channel#
DC0 	2	31	161	1	23	0	3	23	1877	0
DC0 	2	33	161	1	23	0	3	23	1877	1
DC0 	2	35	161	1	23	0	3	23	1877	2
DC0 	2	36	161	1	23	0	3	23	1877	3
DC0 	2	34	161	1	23	0	3	23	1877	4
DC0 	2	32	161	1	23	0	3	23	1877	5
DC0 	2	31	162	1	23	0	3	23	1877	6
DC0 	2	33	162	1	23	0	3	23	1877	7
DC0 	2	35	166	1	23	0	3	23	1877	8
DC0 	2	36	166	1	23	0	3	23	1877	9
DC0 	2	34	166	1	23	0	3	23	1877	10
DC0 	2	32	166	1	23	0	3	23	1877	11
DC0 	2	31	167	1	23	0	3	23	1877	12
DC0 	2	33	167	1	23	0	3	23	1877	13
DC0 	2	35	167	1	23	0	3	23	1877	14
DC0 	2	36	167	1	23	0	3	23	1877	15
DC0 	2	34	163	1	23	0	3	23	1877	16
DC0 	2	32	163	1	23	0	3	23	1877	17
DC0 	2	31	164	1	23	0	3	23	1877	18
DC0 	2	33	164	1	23	0	3	23	1877	19
DC0 	2	35	164	1	23	0	3	23	1877	20
DC0 	2	36	164	1	23	0	3	23	1877	21
DC0 	2	34	164	1	23	0	3	23	1877	22
DC0 	2	32	164	1	23	0	3	23	1877	23
DC0 	2	31	169	1	23	0	3	23	1877	24
DC0 	2	33	169	1	23	0	3	23	1877	25
DC0 	2	35	169	1	23	0	3	23	1877	26
DC0 	2	36	169	1	23	0	3	23	1877	27
DC0 	2	34	169	1	23	0	3	23	1877	28
DC0 	2	32	169	1	23	0	3	23	1877	29
DC0 	2	31	170	1	23	0	3	23	1877	30
DC0 	2	33	170	1	23	0	3	23	1877	31
DC0 	2	35	174	1	23	0	3	23	1877	32
DC0 	2	36	174	1	23	0	3	23	1877	33
DC0 	2	34	174	1	23	0	3	23	1877	34
DC0 	2	32	174	1	23	0	3	23	1877	35
DC0 	2	31	175	1	23	0	3	23	1877	36
DC0 	2	33	175	1	23	0	3	23	1877	37
DC0 	2	35	175	1	23	0	3	23	1877	38
DC0 	2	36	175	1	23	0	3	23	1877	39
DC0 	2	34	171	1	23	0	3	23	1877	40
DC0 	2	32	171	1	23	0	3	23	1877	41
DC0 	2	31	172	1	23	0	3	23	1877	42
DC0 	2	33	172	1	23	0	3	23	1877	43
DC0 	2	35	172	1	23	0	3	23	1877	44
DC0 	2	36	172	1	23	0	3	23	1877	45
DC0 	2	34	172	1	23	0	3	23	1877	46
DC0 	2	32	172	1	23	0	3	23	1877	47



*/



static int lookup[96][2] = {

  /*PRE-LAST ADB, tdc slot 23 channels 0-47, 0 */

/*ADB connector 1, pins 1-8*/
 31,	161,
 33,	161,
 35,	161,
 36,	161,
 34,	161,
 32,	161,
 31,	162,
 33,	162,

/*ADB connector 1, pins 9-16*/
 35,	162,
 36,	162,
 34,	162,
 32,	162,
 31,	163,
 33,	163,
 35,	163,
 36,	163,

/*ADB connector 2, pins 1-8*/
 34,	163,
 32,	163,
 31,	164,
 33,	164,
 35,	164,
 36,	164,
 34,	164,
 32,	164,

/*ADB connector 2, pins 9-16*/
 31,	165,
 33,	165,
 35,	165,
 36,	165,
 34,	165,
 32,	165,
 31,	166,
 33,	166,

/*ADB connector 3, pins 1-8*/
 35,	166,
 36,	166,
 34,	166,
 32,	166,
 31,	167,
 33,	167,
 35,	167,
 36,	167,

/*ADB connector 3, pins 9-16*/
 34,	167,
 32,	167,
 31,	168,
 33,	168,
 35,	168,
 36,	168,
 34,	168,
 32,	168,

/*ADB connector 4, pins 1-8*/
 31,	169,
 33,	169,
 35,	169,
 36,	169,
 34,	169,
 32,	169,
 31,	170,
 33,	170,

/*ADB connector 4, pins 9-16*/
 35,	170,
 36,	170,
 34,	170,
 32,	170,
 31,	171,
 33,	171,
 35,	171,
 36,	171,

/*ADB connector 5, pins 1-8*/
 34,	171,
 32,	171,
 31,	172,
 33,	172,
 35,	172,
 36,	172,
 34,	172,
 32,	172,

/*ADB connector 5, pins 9-16*/
 31,	173,
 33,	173,
 35,	173,
 36,	173,
 34,	173,
 32,	173,
 31,	174,
 33,	174,

/*ADB connector 6, pins 1-8*/
 35,	174,
 36,	174,
 34,	174,
 32,	174,
 31,	175,
 33,	175,
 35,	175,
 36,	175,

/*ADB connector 6, pins 9-16*/
 34,	175,
 32,	175,
 31,	176,
 33,	176,
 35,	176,
 36,	176,
 34,	176,
 32,	176





/*LAST ADB, tdc slot 23 channels 48-95, 0

  31,	181,
  33,	181,
  35,	181,
  36,	181,
  34,	181,
  32,	181,
  31,	182,
  33,	182,

  35,	178,
  36,	178,
  34,	178,
  32,	178,
  31,	179,
  33,	179,
  35,   179,
  36,	179,

  34,	183,
  32,	183,
  31,	184,
  33,	184,
  35,   184,
  36,	184,
  34,	184,
  32,	184,

  31,	189,
  33,	189,
  35,	189,
  36,	189,
  34,	189,
  32,	189,
  31,	190,
  33,	190,

  35,	186,
  36,	186,
  34,	186,
  32,	186,
  31,	187,
  33,	187,
  35,	187,
  36,	187,

  34,	191,
  32,	191,
  31,	192,
  33,	192,
  35,	192,
  36,	192,
  34,	192,
  32,	192,
*/

/*LAST ADB, tdc slot 23 channels 48-95, 1

  31,	177,
  33,	177,
  35,	177,
  36,	177,
  34,	177,
  32,	177,
  31,	178,
  33,	178,

  35,	182,
  36,	182,
  34,	182,
  32,	182,
  31,	183,
  33,	183,
  35,	183,
  36,	183,

  34,	179,
  32,	179,
  31,	180,
  33,	180,
  35,	180,
  36,	180,
  34,	180,
  32,	180,

  31,	185,
  33,	185,
  35,	185,
  36,	185,
  34,	185,
  32,	185,
  31,	186,
  33,	186,

  35,	190,
  36,	190,
  34,	190,
  32,	190,
  31,	191,
  33,	191,
  35,	191,
  36,	191,

  34,	187,
  32,	187,
  31,	188,
  33,	188,
  35,	188,
  36,	188,
  34,	188,
  32,	188
*/

};



#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */


/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000/*3200*/ /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards, nadcs;
static int maxbytes = 0;
static int error_flag[NBOARDS];
#ifdef SLOTWORKAROUND
static int slotnums[NBOARDS];
#endif
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;

static unsigned int NBsubtract = 9; /* # words to subtract including errors (5 for v1290N, 9 for others) */
/*
static int tmpgood[MY_MAX_EVENT_LENGTH];
static int tmpbad[MY_MAX_EVENT_LENGTH];
*/
#define NTICKS 1000 /* the number of ticks per second */

void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}

/* v1190 end */








/* standart functions */


static void
__download()
{
  unsigned long offset, ii;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  printf("rol1: downloading translation table for roc=%d (ttp1=0x%08x)\n",rol->pid,ttp1);
  ttp1 = TT_LoadROCTT_(rol->pid, ttp1);
  printf("rol1: ... done, ttp1=0x%08x\n",ttp1);

  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,&offset);
  printf("A16 offset = 0x%08x\n",offset);

  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  printf("A24 offset = 0x%08x\n",offset);


  /* DMA setup */
  usrVmeDmaInit();
  usrVmeDmaSetConfig(2,2,0); /* A32, BLT */


/* RCDB start */

  /* check 'tdcbuftmp' alignment and set 'tdcbuf' to 16-byte boundary */
  tdcbuf = &tdcbuftmp[0];
  printf("alignment0: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  if( (((int)tdcbuf)&7) == 0xc )
  {
    tdcbuf += 1;
    printf("alignment1: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x8 )
  {
    tdcbuf += 2;
    printf("alignment2: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x4 )
  {
    tdcbuf += 3;
    printf("alignment3: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  printf("alignment: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);


  /* 900-11mV, 2900-30mV, 4200-45.0mV, 4400-47.2mV*/
  DCRBInit(4200);

/* RCDB end */


  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}


static void
__prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16, iflag;
  int i, ii, jj, ifull, istat;
  short buf;
  unsigned short slot, channel, pattern[8];

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  VME_INIT; /* init trig source VME */

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
#ifdef POLLING_MODE
  CTRIGRSS(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#else
  CTRIGRSA(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s< (ttp1=0x%08x)\n",rcname,ttp1);


  if( (env=getenv("PROFILE")) != NULL )
  {
    if(*env == 'T')
    {
      timeprofile = 1;
      logMsg("rol1: time profiling is ON\n",1,2,3,4,5,6);
    }
    else
    {
      timeprofile = 0;
      logMsg("rol1: time profiling is OFF\n",1,2,3,4,5,6);
    }
  }

  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",rol->pid);
    uthbook1(histi, 1000+rol->pid, histname, 200, 0, 200);
  }
/*
  printf("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad);
*/
  printf("INFO: User Prestart Executed\n");

  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(VME,TIR_SOURCE,0);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
  int extra;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  CDOENABLE(VME,TIR_SOURCE,0);

  return;
}


void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned int tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2;
  TIMER_VAR;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nevent, nbcount, buff[32];
  unsigned short level;
  int previousIndex, kk, type;

  int ii, iii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned int res, datascan, mymask=0xfff0;
  unsigned int tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned int tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf, nbsubtract, word;
  int nheaders, ntrailers;

  rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;
  /*
logMsg("EVTYPE=%d\n",EVTYPE,2,3,4,5,6);
  */

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;

  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */

  /* for sync events do nothing          */
  /* for physics events send HLS scalers */

/* if want physics_sync events in external mode
if( (*(rol->nevents))%1000)
{
  syncFlag=0;
}
else
{
  syncFlag=1;
  rol->dabufp += bosMmsg_(jw,"INFO",rol->pid,"physics_sync event");
}
*/

  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    logMsg("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    logMsg("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/


    /* report histograms
    if(timeprofile)
    {
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
	*/


    /*logMsg("Force Sync: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);*/
    /*
    ncol=32;
    nrow=1;
    if( (ind = bosMopen_(jw,"HLS+",0,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
    if( (ind = bosMopen_(jw,"HLS+",1,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
    */
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else           /* physics and physics_sync events */
  {

	/*
goto skip;
	*/

    rlenbuf[0] = NBOARDS*MY_MAX_EVENT_LENGTH;
    res = DCRBReadoutFIFO(tdcbuf, rlenbuf);

    if(res<0 || rlenbuf[0]<=0) error_flag[6/*slot*/] = 1;

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;
	
    rlen = rlenbuf[0];
#ifdef DEBUG
    logMsg("rlen=%d\n",rlen,2,3,4,5,6);
#endif
    for(i=0; i<rlen; i++)
    {
      word = tdcbuf[i];                        
      if(word & 0x80000000)
      {
        type = (word>>27)&0xF;
        switch(type)
        {
          case DATA_TYPE_BLKHDR:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {BLKHDR} SLOTID: %d NEVENTS: %d BLOCK: %d\n",i,word,(word>>22)&0x1f,(word>>11)&0x7ff,(word>>0)&0x7ff,6);
#endif
            tdcslot = (word>>22)&0x1f;
            tdchead = rol->dabufp++; /*remember pointer to the slot header*/
            nwords = 1;
            break;

          case DATA_TYPE_BLKTLR:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {BLKTLR} SLOTID: %d NWORDS: %d\n",i,word,(word>>22)&0x1f,(word>>0)&0x3fffff,5,6);
#endif
			*tdchead = ((tdcslot<<27) + nwords);
#ifdef DEBUG
			logMsg("-----> slot=%d nwords=%d -> output=0x%08x\n",tdcslot,nwords,*tdchead,4,5,6);
#endif
            break;

          case DATA_TYPE_EVTHDR:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {EVTHDR} EVENT: %d\n",i,word,(word>>0)&0x7ffffff,4,5,6);
#endif
            break;

          case DATA_TYPE_TRGTIME:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {TRGTIME}\n",i,word,3,4,5,6);
#endif
            break;

          case DATA_TYPE_TDCEVT:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {TDCEVT} CH: %d TIME: %d ns\n",i,word, (word>>16)&0x7f,(word>>0)&0xFFFF,5,6);
#endif
            tdcchan = (word>>16)&0x7f;
            tdcval = word&0xFFFF;
			*rol->dabufp++ = ((tdcslot<<27) + (tdcchan<<17) + tdcval);
            nwords ++;
#ifdef DEBUG
			logMsg("-----> slot=%d chan=%d tdc=%d -> output=0x%08x\n",tdcslot,tdcchan,tdcval,*(rol->dabufp-1),5,6);
#endif
            break;

          case DATA_TYPE_DNV:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {***DNV***}\n",i,word,3,4,5,6);
#endif
            break;

          case DATA_TYPE_FILLER:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {FILLER}\n",i,word,3,4,5,6);
#endif
            break;

          default:
#ifdef DEBUG
            logMsg("[%3d] 0x%08X {***DATATYPE ERROR***}\n",i,word,3,4,5,6);
#endif
            break;
        }
      }
      else
	  {
#ifdef DEBUG
        logMsg("[%3d]      0x%08X\n",i,word,3,4,5,6);
#endif
	  }
    }


	/*
	*rol->dabufp++ = 0x12345;
	*rol->dabufp++ = 0x12345;
	*rol->dabufp++ = 0x12345;
	*rol->dabufp++ = 0x12345;
	*rol->dabufp++ = 0x12345;
	*/



    blen = rol->dabufp - (int *)&jw[ind+1];
	/*logMsg("blen=%d\n",blen,2,3,4,5,6);*/
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      logMsg("1ERROR: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
      logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
      tsleep(1); /* 1 = 0.01 sec */
    }
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }






	/*DC0 bank*/
    len = blen - 1; /*will drop header*/
	if(len>0)
	{
      ind2 = bosMopen_(jw,"DC0 ",2,2,len);
      for(i=0; i<len; i++)
      {
        word = jw[ind+2+i];
        tdcslot = (word>>27)&0x1F;
        tdcchan = (word>>17)&0x7F;
        tdcval = (word&0x7FFF)*2;
        jw[ind2+1+i] = (lookup[tdcchan][0]<<24) + (lookup[tdcchan][1]<<16) + tdcval;
      }
      rol->dabufp += bosMclose_(jw,ind2,2,len);
	}






    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot = 6;

      scan_flag = 0;
      nboards = 0;
      for(ii=0; ii<nboards; ii++)
      {
        if(nboards>0)
		{
          nev = DCRBFifoEventCount();
          if(nev <= 0) /* clear board if extra event */
          {
            logMsg("SYNC: ERROR: [%2d] slot=%2d nev=%d - clear\n",
              ii,slot,nev,4,5,6);
            DCRBSync();
            scan_flag |= (1<<slot);
          }
          if(error_flag[slot] == 1) /* clear board if error flag was set */
          {
            logMsg("SYNC: ERROR: [%2d] slot=%2d error_flag=%d - clear\n",
              ii,slot,error_flag[slot],4,5,6);
            DCRBSync();
            error_flag[slot] = 0;
            scan_flag |= (1<<slot);
          }
		}

      }
	  /*
	  if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  */
	  /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
    }	

skip:
	;
	/*
	logMsg("TRIG\n",1,2,3,4,5,6);
	*/
  }

  /* close event */
  CECLOSE;

  return;
}

void
usrtrig_done()
{
  return;
}

static void
__end()
{
  int ii;

  CDODISABLE(VME,TIR_SOURCE,0);

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(VME,TIR_SOURCE,0);

  return;
}  

static void
__status()
{
  return;
}  
