h12747
s 00007/00001/01726
d D 1.17 10/09/29 12:33:25 boiarino 17 16
c *** empty log message ***
e
s 00001/00001/01726
d D 1.16 10/09/28 20:05:52 boiarino 16 15
c *** empty log message ***
e
s 00003/00003/01724
d D 1.15 10/09/28 20:02:30 boiarino 15 14
c *** empty log message ***
e
s 00000/00000/01727
d D 1.14 10/09/27 11:06:12 boiarino 14 13
c *** empty log message ***
e
s 00744/00083/00983
d D 1.13 10/09/27 09:58:05 boiarino 13 12
c *** empty log message ***
e
s 00009/00001/01057
d D 1.12 09/09/30 21:51:48 boiarino 12 11
c *** empty log message ***
e
s 00003/00003/01055
d D 1.11 09/09/30 20:33:24 boiarino 11 10
c *** empty log message ***
e
s 00001/00001/01057
d D 1.10 09/09/15 13:18:29 boiarino 10 9
c *** empty log message ***
e
s 00005/00003/01053
d D 1.9 09/06/19 10:02:09 boiarino 9 8
c *** empty log message ***
e
s 00002/00002/01054
d D 1.8 08/11/24 10:11:37 boiarino 8 7
c *** empty log message ***
e
s 00107/00027/00949
d D 1.7 08/09/15 15:44:14 boiarino 7 6
c add dvcs2: rocid=27, SF south (branch 1, id=7)
c 
e
s 00006/00006/00970
d D 1.6 07/11/09 17:49:31 boiarino 6 5
c *** empty log message ***
e
s 00011/00000/00965
d D 1.5 06/10/27 22:32:21 boiarino 5 4
c add l2setlatefail() and l2setreject()
c 
e
s 00002/00002/00963
d D 1.4 06/10/09 17:36:03 boiarino 4 3
c fix level2
c 
e
s 00008/00002/00957
d D 1.3 06/05/26 16:13:33 boiarino 3 2
c add croctest5: test setup rocid=1 branchid=1
e
s 00001/00001/00958
d D 1.2 06/04/26 14:57:49 sergpozd 2 1
c Added '1' in id=28 in roc_map for TEST_SETUP
e
s 00959/00000/00000
d D 1.1 06/04/26 14:51:56 boiarino 1 0
c date and time created 06/04/26 14:51:56 by boiarino
e
u
U
f e 0
t
T
I 13
#ifndef PRIMEX
#define FROMSCALER1
#define USEPROGRAM1
#endif
E 13
I 1

I 13
#ifdef PRIMEX
D 15
#define DO_PHASES
E 15
I 15
D 16
#undef DO_PHASES
E 16
I 16
#define DO_PHASES
E 16
E 15
#endif


E 13
/* ts2.c - first readout list for Trigger Supervisor crate */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list TS2 */
#define ROL_NAME__ "TS2"

/* maximum 131072,100 
#define MAX_EVENT_LENGTH 131072
#define MAX_EVENT_POOL 200
*/

/* ts control */
#define TRIG_SUPV

/* name used by loader */
#ifdef TEST_SETUP
I 3

#ifdef LOCK_ROC
#define INIT_NAME ts2_testsetup_lockroc__init
#else
E 3
#define INIT_NAME ts2_testsetup__init
I 3
#endif

E 3
#else

I 13
#ifdef PRIMEX

E 13
#ifdef LOCK_ROC
I 13
#define INIT_NAME ts2_primex_lockroc__init
#else
#define INIT_NAME ts2_primex__init
#endif

#else

#ifdef LOCK_ROC
E 13
#define INIT_NAME ts2_lockroc__init
#else
#define INIT_NAME ts2__init
#endif

#endif

I 13
#endif



E 13
#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "coda.h"

I 7
/*sergey*/
#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

E 7
/* TS addresses */
#define TSBASE 0xed0000
#define TSPROG 0xed4000

/* OR module address */
#define ORBASE 0xdd0000

I 13
#ifdef FROMSCALER1
/* trigger latch board local addresses */
#define LATCH1ADDR 0x000ed0
#define LATCH2ADDR 0x000ee0
#endif

E 13
/* force sync interval (seconds) */
#define SYNCINTERVAL 10

static char *rcname = "RC00";
D 7
unsigned long ppc_offset; /* global PPC board offset */
E 7
I 7
D 13
static unsigned long ppc_offset; /* global PPC board offset */
E 13
I 13
static unsigned int ppc_offset; /* global PPC board offset */
E 13
E 7
extern int rocMask; /* defined in roc_component.c */

I 13











/* from scalrol1.c */

/* sync status */
static unsigned long sync_status=0;

/* extra software scalers (S1ST), run only */
static unsigned long run_l1_count[12];
static unsigned long run_latch1_zero_count;
static unsigned long run_latch1_empty_count;
static unsigned long run_latch1_not_empty_count;
static unsigned long run_latch1_ok_count;
static unsigned long run_latch2_zero_count;
static unsigned long run_latch2_empty_count;
static unsigned long run_latch2_not_empty_count;
static unsigned long run_latch2_ok_count;
static unsigned long run_l2pass;
static unsigned long run_l2fail;
static unsigned long run_l2s1;
static unsigned long run_l2s2;
static unsigned long run_l2s3;
static unsigned long run_l2s4;
static unsigned long run_l2s5;
static unsigned long run_l2s6;
static unsigned long run_roc13_count;
static unsigned long run_roc15_count;
static unsigned long run_l1l2_zero_count;
static unsigned long run_13_l1_zero_count;
static unsigned long run_13_l2_zero_count;
static unsigned long run_13_l1l2_zero_count;













E 13
/* set branch mask - different for TS in the hall and TS in test setup */
/*
 map correlates coda roc id (0-31) and roc branch bit number
   branch 1 has bit numbers 0-7, branch 2 has 8-15 etc
 e.g. roc 1 uses branch bit 11, roc 8 uses bit 9, etc. 

 so position in the table is roc id starting from 0,
 and values are branch bit number

 branch 1 - SF south
 branch 2 - SF north
 branch 3 - FC 
 branch 4 - FC 
*/

#ifdef TEST_SETUP
D 6
/* croctest2 - put '2' in id=0, croctest5 - put '2' in id=5 */
D 3
int roc_map[32] = { 2, -1, -1, -1,    -1, -1, -1, -1,
E 3
I 3
int roc_map[32] = { 2,  1, -1, -1,    -1, -1, -1, -1,
E 6
I 6
D 9
/* croctest2 - put '2' in first place, croctest5 - put '2' in second place */
E 9
I 9
/* croctest2 - put '2' in first place (ID=0) */
/* croctest1 - put '4' in last place (ID=31) */
/* croctest5 - put '0' in pre-last place (ID=30) */
I 13
/* croctest3 - put '1' in pre-pre-last place (ID=29) */
E 13
E 9
int roc_map[32] = { 2, -1, -1, -1,    -1, -1, -1, -1,
E 6
E 3
                   -1, -1, -1, -1,    -1, -1, -1, -1,
                   -1, -1, -1, -1,    -1, -1, -1, -1,
D 2
                   -1, -1, -1, -1,    -1, -1, -1,  4};
E 2
I 2
D 3
                   -1, -1, -1, -1,     1, -1, -1,  4};
E 3
I 3
D 9
                   -1, -1, -1, -1,    -1, -1, -1,  4};
E 9
I 9
D 13
                   -1, -1, -1, -1,    -1, -1,  0,  4};
E 13
I 13
                   -1, -1, -1, -1,    -1,  1,  0, 4};
E 13
E 9
E 3
E 2
#else
I 13

#ifdef PRIMEX

/* primexroc4 - ID=1, branch 1, jumper 4 (4) */
/* primexroc5 - ID=2, branch 2, jumper 5 (13) */
/* primexroc6 - ID=3, branch 3, jumper 6 (22) */
/* tage       - ID=15, branch 4, jumper 4 (28) */
/* tage2      - ID=16, branch 4, jumper 5 (29) */
/* tage3      - ID=17, branch 4, jumper 6 (30) */
D 15
/* primextdc1 - ID=18, branch 4, jumper 7 (31) */
E 15
I 15
/* primextdc1 - ID=18, branch 4, jumper 0 (24) */
E 15
/* primexts2 - ID=5, branch 5 */
int roc_map[32] = {-1,  4, 13, 22,    -1, -1, -1, -1,
                   -1, -1, -1, -1,    -1, -1, -1, 28,
D 15
                   29, 30, 31, -1,    -1, -1, -1, -1,
E 15
I 15
                   29, 30, 24, -1,    -1, -1, -1, -1,
E 15
                   -1, -1, -1, -1,    -1, -1, -1, -1};

#else

E 13
/*
      ROC_ID    ROC_NAME      BRANCH#      BRANCH_ID
------------------------------------------------------
       0       ========
       1          dc1             2            3
       2          dc2             2            0
       3          dc3           1            1
       4          dc4           1            2
       5          dc5             2            2
       6          dc6             2            5
       7          dc7             2            4
       8          dc8             2            1
       9          dc9           1            0
      10          dc10          1            3
      11          dc11            2            6
      12          cc1               3            1
      13          sc1               3            0
      14          ec1               3            2
      15          ec2               3            3
      16          lac1                4            3
      17          tage          1            4
      18          scaler2             4            7
      19          scaler4             4            4
      20          ec3               3            4
      21          ec4               3            5
      22          sc2               3            6
      23          scaler3             4            5
      24          lac2                4            2
      25          polar           2            7
      26          clastrig2             5            -
D 6
      27         bonuspc2       1            6
E 6
I 6
      27          bonuspc2      1            6
E 6
      28        ==tage3==
      29          tage2         1            5
      30          scaler1             4            6
      31         bonuspc1       1            7

int roc_map[32] = {-1, 11,  8,  1,     2, 10, 13, 12,
                    9,  0,  3, 14,    17, 16, 18, 19, 
                   27,  4, 31, 28,    20, 21, 22, 29, 
                   26, 15, -1,  6,    -1,  5, 30,  7};
*/




/*
      ROC_ID    ROC_NAME      BRANCH#      BRANCH_ID
------------------------------------------------------
       0       croctest2
       1          dc1             2            3
       2          dc2             2            0
       3          dc3           1            1
       4          dc4           1            2
       5          dc5             2            2
       6          dc6             2            5
       7          dc7             2            4
       8          dc8             2            1
       9          dc9           1            0
      10          dc10          1            3
      11          dc11            2            6
      12          cc1               3            1
      13          sc1               3            0
      14          ec1               3            2
      15          ec2               3            3
      16          lac1                4            3
D 11
      17          tage          1            4
E 11
I 11
      17          tage          1            4       ---> bonuspc1
E 11
      18          scaler2             4            7
      19          scaler4             4            4
      20          ec3               3            4
      21          ec4               3            5
      22          sc2               3            6
      23          scaler3             4            5
      24          lac2                4            2
      25          polar           2            7
      26          clastrig2             5            -
D 7
      27       croctest10
E 7
I 7
      27          dvcs2         1            7
E 7
D 6
      28          tage3         1            6  ----------> use for ms1(id=27)
      29          tage2         1            5  ----------> use for ROC31
E 6
I 6
D 11
      28          tage3         1            6
      29          tage2         1            5
E 11
I 11
      28          tage3         1            6       ---> bonuspc3
      29          tage2         1            5       ---> bonuspc2
E 11
E 6
      30          scaler1             4            6
      31       croctest1
I 13

      xx          bonuspc0            4            0

E 13
*/
I 12
D 13
/*
E 13
I 13

E 13
E 12
int roc_map[32] = {-1, 11,  8,  1,     2, 10, 13, 12,
                    9,  0,  3, 14,    17, 16, 18, 19, 
                   27,  4, 31, 28,    20, 21, 22, 29, 
D 7
				   26, 15, -1, -1,     6,  5, 30, -1};
E 7
I 7
				   26, 15, -1,  7,     6,  5, 30, -1};
I 12
D 13
*/
E 13
E 12
E 7

I 12
D 13
/* bonuspc1 instead of tage */
int roc_map[32] = {-1, 11,  8,  1,     2, 10, 13, 12,
E 13
I 13

/* bonuspc1(rocid=31) instead of tage, bonuspc2(rocid=0) instead of tage2
int roc_map[32] = { 5, 11,  8,  1,     2, 10, 13, 12,
E 13
                    9,  0,  3, 14,    17, 16, 18, 19, 
                   27, -1, 31, 28,    20, 21, 22, 29, 
D 13
				   26, 15, -1,  7,     6,  5, 30, 4};
E 13
I 13
				   26, 15, -1,  7,     6, 24, 30, 4};
*/
E 13
E 12

I 12
D 13

E 13
E 12
/* ms1 instead of tage3 
int roc_map[32] = {-1, 11,  8,  1,     2, 10, 13, 12,
                    9,  0,  3, 14,    17, 16, 18, 19, 
                   27,  4, 31, 28,    20, 21, 22, 29, 
				   26, 15, -1,  6,    -1,  5, 30, -1};
*/
#endif

I 13
#endif
E 13

I 13

E 13
/*BONUS*/
int roc_map_bonus[32] = {-1, 11,  8,  1,     2, 10, 13, 12,
                          9,  0,  3, 14,    17, 16, 18, 19, 
                         27,  4, 31, 28,    20, 21, 22, 29, 
			             26, 15, -1, -1,     6, -1, 30, 5}; /* remove tage2, add ROC31 */
/*BONUS*/


I 7

E 7
/* update UPD message 'sts:'; parameters:
    livetime - TS2-measured livetime (percent)
    trigbits - statistics for 12 trigger bits
 */
int
ts2UpdateStatistics(unsigned int livetime, unsigned int trigbits[12])
{
  char tmp[1000];

  sprintf(tmp,"ts2: %d %d %d %d %d %d %d %d %d %d %d %d %d ",livetime,
    trigbits[0],trigbits[1],trigbits[2],trigbits[3],trigbits[4],trigbits[5],
    trigbits[6],trigbits[7],trigbits[8],trigbits[9],trigbits[10],trigbits[11]);

  UDP_request(tmp);

  return(0);
}



/* some debug functions */
void
printTSstatus()
{
  printf("ts->csr:  0x%08x\n",ts->csr);
  printf("ts->csr2: 0x%08x\n",ts->csr2);
  printf("ts->trig: 0x%08x\n",ts->trig);
  printf("ts->roc:  0x%08x\n",ts->roc);
  printf("ts->trigCount: 0x%08x\n",ts->trigCount);
  printf("ts->trigData: 0x%08x\n",ts->trigData);
  printf("ts->lrocData: 0x%08x\n",ts->lrocData);
  printf("ts->rocBufStatus: 0x%08x\n",ts->rocBufStatus);
  printf("ts->lrocBufStatus: 0x%08x\n",ts->lrocBufStatus);
  printf("ts->rocAckStatus: 0x%08x\n",ts->rocAckStatus);
  printf("ts->state: 0x%08x\n",ts->state);
  return;
}

I 5
void
l2setlatefail()
{
  ts->timer[0] = 0;
}
E 5

void
I 5
l2setreject()
{
  ts->timer[0] = 80;
}

void
E 5
ts2status()
{
  int branch;
  unsigned int status;
  int i;
  unsigned int rocenabled;

  /* CSR1 */
  status = ts->csr;
  printf("CSR 1 (0x0):\n");
  printf("\t                              Go : %d\n", (status>>0)&1);
  printf("\t    Pause on Next scheduled Sync : %d\n", (status>>1)&1);
  printf("\t                  Sync and Pause : %d\n", (status>>2)&1);
  printf("\t             Initiate Sync Event : %d\n", (status>>3)&1);
  printf("\t        Initiate Program 1 Event : %d\n", (status>>4)&1);
  printf("\t        Initiate Program 2 Event : %d\n", (status>>5)&1);
  printf("\t Enable Level 1 (drives outputs) : %d\n", (status>>6)&1);
  printf("\t                Override Inhibit : %d\n", (status>>7)&1);
  printf("\t                       Test Mode : %d\n", (status>>8)&1);
  printf("\t                        Reserved : %d\n", (status>>9)&1);
  printf("\t                           Reset : %d\n", (status>>14)&1);
  printf("\t                      Initialize : %d\n", (status>>15)&1);
  printf("\n");
  printf("\t             Sync Event occurred : %d\n", (status>>16)&1);
  printf("\t        Program 1 Event occurred : %d\n", (status>>17)&1);
  printf("\t        Program 2 Event occurred : %d\n", (status>>18)&1);
  printf("\t              Late Fail occurred : %d\n", (status>>19)&1);
  printf("\t                Inhibit occurred : %d\n", (status>>20)&1);
  printf("\t       Write FIFO error occurred : %d\n", (status>>21)&1);
  printf("\t       Read FIFO error occurreds : %d\n", (status>>22)&1);


  /* CSR2 */
  status = ts->csr2;
  printf("CSR 2 (0x4):\n");
  printf("\t              Enable Scheduled Sync : %d\n", (status>>0)&1);
  printf("\t             Use Clear Permit Timer : %d\n", (status>>1)&1);
  printf("\t               Use Front Busy Timer : %d\n", (status>>2)&1);
  printf("\t               Use Clear Hold Timer : %d\n", (status>>3)&1);
  printf("\t            Use External Front Busy : %d\n", (status>>4)&1);
  printf("\t                  Lock ROC Branch 1 : %d\n", (status>>5)&1);
  printf("\t                  Lock ROC Branch 2 : %d\n", (status>>6)&1);
  printf("\t                  Lock ROC Branch 3 : %d\n", (status>>7)&1);
  printf("\t                  Lock ROC Branch 4 : %d\n", (status>>8)&1);
  printf("\t                  Lock ROC Branch 5 : %d\n", (status>>9)&1);
  printf("\t Enable Program 1 front panel input : %d\n", (status>>10)&1);
  printf("\t                   Enable Interrupt : %d\n", (status>>11)&1);
  printf("\t        Enable local ROC (branch 5) : %d\n", (status>>12)&1);
  printf("\n");


  status = ts->trig;
  printf("Trigger Control Register (0x8): 0x%08x\n",status&0xFFFF);
  printf("\n");


  rocenabled = ts->roc;
  printf("ROC Enable Register (0xc) val=0x%8x:\n",rocenabled);
  for(branch=1; branch<=4; branch++)
  {
	printf("Branch %d: 0x%2x  bits: ", branch, rocenabled&0xFF);
	for(i=0; i<=7; i++) printf("%d",(rocenabled>>(7-i))&0x01);
	printf("\n");
	rocenabled >>= 8;
  }
  printf("\n");


  status = ts->sync;
  printf("Synchronization Interval Register (0x10): %d\n",status&0xFFFF);
  printf("\n");


  status = ts->trigCount;
  printf("Trigger Word Count Register (0x14): %d\n",status&0xFFFF);
  printf("\n");


  status = ts->trigData;
  printf("Trigger Data Register (0x18): %d\n",status&0xFFF);
  printf("\n");


  status = ts->lrocData;
  printf("Local ROC (Branch 5) Data Register (0x1c): %d\n",status&0xFF);
  printf("\t               Synchronization Flag : %d\n", (status>>0)&1);
  printf("\t                     Late Fail Flag : %d\n", (status>>1)&1);
  printf("\t                           ROC code : %d\n", (status>>2)&0x3F);
  printf("\n");


  printf("Input Trigger Prescale Registers:\n");
  printf("\t         Input 1 Prescale Factor : %d\n",ts->prescale[0]&0xFFFFFF);
  printf("\t         Input 2 Prescale Factor : %d\n",ts->prescale[1]&0xFFFFFF);
  printf("\t         Input 3 Prescale Factor : %d\n",ts->prescale[2]&0xFFFFFF);
  printf("\t         Input 4 Prescale Factor : %d\n",ts->prescale[3]&0xFFFFFF);
  printf("\t         Input 5 Prescale Factor : %d\n",ts->prescale[4]&0xFFFF);
  printf("\t         Input 6 Prescale Factor : %d\n",ts->prescale[5]&0xFFFF);
  printf("\t         Input 7 Prescale Factor : %d\n",ts->prescale[6]&0xFFFF);
  printf("\t         Input 8 Prescale Factor : %d\n",ts->prescale[7]&0xFFFF);
  printf("\n");


  printf("Clear Permit Timer Register  = %d\n",(int)(ts->timer[0]&0xFFFF));
  printf("Level2 Accept Timer Register = %d\n",(int)(ts->timer[1]&0xFFFF));
  printf("Level3 Accept Timer Register = %d\n",(int)(ts->timer[2]&0xFFFF));
  printf("Front Busy Timer Register    = %d\n",(int)(ts->timer[3]&0xFFFF));
  printf("Clear Hold Timer Register    = %d\n",(int)(ts->timer[4]&0xFF));
  printf("\n");


  status = ts->rocBufStatus;
  printf("Branch (1-4) ROC Buffer Status Register (0x58): 0x%08x\n",status);
  for(branch=1; branch<=4; branch++)
  {
	printf("Branch %d: Buffer Count = %d, Empty Flag = %d, Full Flag = %d\n",
      branch,status&0xF,(status>>6)&0x1,(status>>7)&0x1);
	status >>=8;
  }
  printf("\n");


  status = ts->lrocBufStatus;
  printf("Local ROC (Branch 5) Buffer Status Register (0x5c): 0x%08x\n",status);
  branch = 5;
  printf("Branch %d: Buffer Count = %d, Empty Flag = %d, Full Flag = %d,",
      branch,status&0xF,(status>>6)&0x1,(status>>7)&0x1);
  printf("Local Acknowledge = %d, Local Event Strob Status = %d\n",
      (status>>8)&0x1,(status>>15)&0x1);
  printf("\n");


  status = ts->rocAckStatus;
  rocenabled = ts->roc;
  printf("ROC Acknowledge Status Register (0x60): val=0x%x\n",status);
  for(branch=1; branch<=4; branch++)
  {
	printf("Branch %d: 0x%2x  bits: ", branch, status&0xFF);
	for(i=0; i<=7; i++) printf("%d",(status>>(7-i))&0x01);
	printf(" (enabled: ");
	for(i=0; i<=7; i++) printf("%d",(rocenabled>>(7-i))&0x01);
	printf(")\n");
	status >>= 8;
    rocenabled >>= 8;
  }
  printf("\n");


  status = ts->state;
  printf("State Register (0x6C):\n");
  printf("\t               Level 1 Accept     : %d\n", (status>>0)&1);
  printf("\t            Start Level 2 Trigger : %d\n", (status>>1)&1);
  printf("\t             Level 2 Pass Latched : %d\n", (status>>2)&1);
  printf("\t             Level 2 Fail Latched : %d\n", (status>>3)&1);
  printf("\t                   Level 2 Accept : %d\n", (status>>4)&1);
  printf("\t            Start Level 3 Trigger : %d\n", (status>>5)&1);
  printf("\t             Level 3 Pass Latched : %d\n", (status>>6)&1);
  printf("\t             Level 3 Fail Latched : %d\n", (status>>7)&1);
  printf("\t                   Level 3 Accept : %d\n", (status>>8)&1);
  printf("\t                            Clear : %d\n", (status>>9)&1);
  printf("\t        Front End Busy (external) : %d\n", (status>>10)&1);
  printf("\t                 External Inhibit : %d\n", (status>>11)&1);
  printf("\t                  Latched Trigger : %d\n", (status>>12)&1);
  printf("\t                          TS Busy : %d\n", (status>>13)&1);
  printf("\t                        TS Active : %d\n", (status>>14)&1);
  printf("\t                         TS Ready : %d\n", (status>>15)&1);
  printf("\t            Main Sequencer Active : %d\n", (status>>16)&1);
  printf("\t Synchronization Sequencer Active : %d\n", (status>>17)&1);
  printf("\t Program 1 Event Sequencer Active : %d\n", (status>>18)&1);
  printf("\t Program 2 Event Sequencer Active : %d\n", (status>>19)&1);
  printf("\n\n");


  status = ts->scalEvent;
  printf("Event Count (0xc8): %d\n",status);
  status = ts->scalLive1;
  printf("Live 1 Count (0xcc): %d\n",status);
  status = ts->scalLive2;
  printf("Live 2 Count (0xd0): %d\n",status);
  printf("\n");

}




D 13
unsigned int correct_counter(unsigned int counter)
E 13
I 13
unsigned int
correct_counter(unsigned int counter)
E 13
/* obtain corrected value of either TS live time counter - EJ - 6/05 */
{
	unsigned int count0, count1, count2, count3, actual;
	unsigned int actual0, actual1, actual2, actual3;
	
	count0 = counter & 0xFF;
	count1 = (counter & 0xFF00) >> 8;
	count2 = (counter & 0xFF0000) >> 16;
	count3 = (counter & 0xFF000000) >> 24;
	
	actual0 = count0;
	actual1 = count1;
	
	if( (count1 == 0xFF) && (count2 != 0) )
	    actual2 = count2 - 1;
	else if( (count1 == 0xFF) && (count2 == 0) )
	    actual2 = 0xFF;  
	else    
	    actual2 = count2;
	        
	if( (count2 == 0xFF) || ( (count2 == 0) && (count1 == 0xFF) ) )
	    actual3 = count3 - 1;
	else    
	    actual3 = count3;
	    	    
	actual = (actual3 << 24) | (actual2 << 16) | (actual1 << 8) | actual0;
	
	return actual;
}



static void
__download()
{
  int i;

D 13
  /* sync mode (ROC send data) */
E 13
I 13
  /* NOT IN USE ANY MORE !!!??? 
E 13
  *(rol->async_roc) = 0;
I 13
  */
E 13

I 13
#ifdef PRIMEX
#ifdef DO_PHASES
  set_current_phase(-1); /*in ts2config.c*/
#endif
#endif

E 13
  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  sysBusToLocalAdrs(0x39,0,&ppc_offset);

  /* set up TS data structures */
  ts    = (volatile struct vme_ts *)(ppc_offset+TSBASE);
D 13
  tsmem = (volatile long *)(ppc_offset+TSPROG);
E 13
I 13
  tsmem = (volatile int *)(ppc_offset+TSPROG);
E 13

I 13
  /* initialize TS */
  ts->csr = 0x8000;

E 13
  ts->trig = 0x1FFF;

D 7
  logMsg("ts address=0x%08x, tsmem address=0x%08x\n",
    (int)ts,(int)tsmem,3,4,5,6);
E 7
I 7
D 13
  logMsg("ppc_offset=0x%08x, ts address=0x%08x, tsmem address=0x%08x\n",
    ppc_offset,(int)ts,(int)tsmem,3,4,5,6);
E 13
I 13
  printf("ppc_offset=0x%08x, ts address=0x%08x, tsmem address=0x%08x\n",
    ppc_offset,(int)ts,(int)tsmem);
E 13
E 7

I 13
#ifndef TEST_SETUP
#ifdef FROMSCALER1
  download_l1_mlu();  /* program L1 MLU */
#endif

#endif

E 13
  return;
}

I 7

void
ts2testfast()
{
  ts->timer[1] = 83;
  ts->timer[2] = 0;
  ts->timer[3] = 25;
  ts->timer[4] = 100;
}
void
ts2testslow()
{
  ts->timer[1] = 83;
EIEIO;
SYNC;
  ts->timer[2] = 0;
EIEIO;
SYNC;
  ts->timer[3] = 25;
EIEIO;
SYNC;
  ts->timer[4] = 100;
}
void
ts2testprint()
{
  printf("%d %d %d %d\n",ts->timer[1]&0xFFFF,ts->timer[2]&0xFFFF,
                         ts->timer[3]&0xFFFF,ts->timer[4]&0xFF);
}
void
ts2testreset()
{
  ts->timer[1] = 0;
  ts->timer[2] = 0;
  ts->timer[3] = 0;
  ts->timer[4] = 0;
}

/*
-> ts2testprint
0 0 0 0
value = 8 = 0x8
-> ts2testfast
value = 0 = 0x0
-> ts2testprint
83 0 0 100
value = 11 = 0xb
-> ts2testfast
value = 0 = 0x0
-> ts2testprint
83 0 0 100
value = 11 = 0xb
-> ts2testfast
value = 0 = 0x0
-> ts2testprint
83 0 0 100
value = 11 = 0xb
-> ts2testslow
value = 0 = 0x0
-> ts2testprint
83 0 25 100
value = 12 = 0xc
-> 
 */


E 7
static void
__prestart()
{
  int i;
  char *p;
D 13
  unsigned long roc_enable_mask, roc_enable_mask_readback, l1mask;
  unsigned long roc_enable_mask_bonus;
  unsigned long myprescale[8];
  volatile unsigned long *ores, *orset;
E 13
I 13
  unsigned int roc_enable_mask, roc_enable_mask_readback, l1mask;
  unsigned int roc_enable_mask_bonus;
  unsigned int myprescale[8];
  volatile unsigned int *ores, *orset;
E 13

I 13
#ifdef PRIMEX
#ifdef DO_PHASES
  TSInit();
  set_current_phase(-1); /*in ts2config.c*/
#endif
#endif

E 13
  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source VME */
  VME_INIT;

  /* Register an async trigger source (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 2 to ts_trig and ts_trig_done */
D 13
  CTRIGRSA(VME, 2, ts_trig, ts_trig_done); /* second arg=2 - what is that ? */
E 13
I 13
  CTRIGRSA(VME, TS_SOURCE, ts_trig, ts_trig_done); /* second arg=2 - for TS, for TI it must be 1 !!! */
E 13

D 13
  /* Register a physics event type (up to 16 types) */
  /* CRTTYPE(type,source,code) */
  /* event type 1 then read VME 2 */
  CRTTYPE(1, VME, 2);
  /* evMasks[ 1 ] |= (1<<( VME_handlers ));*/
E 13

D 13

E 13
  /* user code */

  /* disable dummy trigger, etc. */
  /*rol->poll = 0;TS only*/
  VMEflag = 0;
  VME_prescale = 1;

  VMEflag = 1;
  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);/*.................DEBUG*/

  /* read l1 mask */
  l1mask = ts->trig & 0x1FFE;;
  printf("l1mask=0x%04x\n",l1mask);

  for(i=0; i<8; i++) myprescale[i] = ts->prescale[i];

  /* initialize TS */
  ts->csr = 0x8000;

  /* reset TS state machines - done by 0x8000 */
  /* ts->csr = 0x4000;*/

  /* reset TS go, pause-on-next-sync, sync-and-pause,
     and force-sync bits just in case */
  ts->csr = 0xf0000;

I 7

E 7
  /* default all L1 trigger enables on (12 total), non-strobe mode,
    no open prescale  */
  ts->trig = l1mask | 0x1;/*0x1FFF;*/ /* do not change L1 enables */
  printf("trig=0x%04x\n",ts->trig);

  /* prescales */
  for(i=0; i<8; i++) ts->prescale[i] = myprescale[i];

  /* set timers (40 ns/count) */
#ifdef TEST_SETUP
  ts->timer[1] = 83;  /* L2 */
  ts->timer[2] = 0;   /* L3 */
D 6
  ts->timer[3] = 325;  /* 25 - front end busy set to 1 us */
E 6
I 6
D 9
  ts->timer[3] = 25;  /* 25 - front end busy set to 1 us */
E 9
I 9
D 10
  ts->timer[3] = 325;  /* 25 - front end busy set to 1 us */
E 10
I 10
D 13
  ts->timer[3] = 500;  /* 25 - front end busy set to 1 us */
E 10
E 9
E 6
  ts->timer[4] = 100; /* clear hold set to 4000 ns - length of clear signal */
E 13
I 13
  ts->timer[3] = 325;  /* 25 - front end busy set to 1 us */
  ts->timer[4] = 50; /* clear hold set to 4000 ns - length of clear signal */
E 13
#else
  ts->timer[1] = 83;  /* L2 */ /* was 4 */ /* set to 3320 ns (PTDC trigger) */
  ts->timer[2] = 0;   /* L3 */
D 4
  ts->timer[3] = 325; /* front end busy set to 13 us */
E 4
I 4
D 12
  ts->timer[3] = 325; /* 325 - front end busy set to 13 us */
E 12
I 12
D 13
  ts->timer[3] = 500/*increase for bonus 325*/; /* 325 - front end busy set to 13 us */
E 12
E 4
  ts->timer[4] = 100; /* clear hold set to 4000 ns - length of clear signal */
E 13
I 13
  ts->timer[3] = 325;/*500*//*increase for bonus 325*/; /* 325 - TS busy set to 13 us */
  ts->timer[4] = 50/*100*/; /* clear hold set to 4000 ns - length of clear signal */
E 13
#endif

  /* 12500 - 500us */

D 7

E 7
  ts->intVec = 0xec; /* Set Interrupt vector */

D 7
  /* enable scheduled sync, clear permit timer, frond busy timer,
  clear hold timer and external front busy */
E 7
I 7
D 13
  /* csr2 bits:
       bit 0: enable scheduled sync,
       bit 1: use clear permit timer, 
       bit 2: use frond busy timer,
       bit 3: use clear hold timer,
       bit 4: use external front busy;
       5 following bits: lock roc for 5 branches */
E 13
E 7

I 7

I 13

  /* csr2 bits:
       bit 0: enable scheduled sync
       bit 1: use clear permit timer 
       bit 2: use frond busy timer
       bit 3: use clear hold timer
       bit 4: use external front busy
       bit 5: lock roc branch 1
       bit 6: lock roc branch 2
       bit 7: lock roc branch 3
       bit 8: lock roc branch 4
       bit 9: lock roc branch 5
       bit 10: enable program 1 front panel input
 */
E 13
E 7
#ifdef LOCK_ROC
I 17

E 17
D 7
  /*ts->csr2 = 0x000003F5;*/
  ts->csr2 = 0x000003FF;
E 7
I 7
  ts->csr2 = 0x03FF;
I 13
#ifdef PRIMEX
  /*ts->csr2 = 0x03EF; no FEB */
D 17
  ts->csr2 = 0x03FF;
E 17
I 17
  ts->csr2 = 0x83FF;
E 17
#endif
I 17

E 17
E 13
E 7
#else
I 17

E 17
D 7
  ts->csr2 = 0x0000001F;
E 7
I 7
  ts->csr2 = 0x001F;
I 17
#ifdef PRIMEX
  ts->csr2 = 0x801F;
#endif
E 17
I 13

E 13
E 7
#endif

D 7

E 7
D 13
  /* override inhibit (was called for latef only !!! - why ???) */
  /*ts->csr  = 0x800;*/
E 13

I 13


E 13
  /************************************************/
  /**************** level2 section ****************/
  /************************************************/

I 7

E 7
  /* set 'clear' timer (40 ns/count); it has following meaning: */
  /* during that time we will generate 'clear', after that we will not */
  if(get_ts_level2())
  {
D 7
    logMsg("Level2 is ON\n",1,2,3,4,5,6);
E 7
I 7
    printf("Level2 is ON\n");
E 7
D 4
    ts->timer[0] = 0/*80*/;  /* clear permit set to 3200 ns */
E 4
I 4
    ts->timer[0] = /*0*/80;  /* clear permit set to 3200 ns */
E 4
D 7
    logMsg("download Level2 memory program\n",1,2,3,4,5,6);
E 7
I 7
    printf("download Level2 memory program\n");
E 7
    ts2_load_default_l2_program();
  }
  else
  {
D 7
    logMsg("Level2 is OFF\n",1,2,3,4,5,6);
E 7
I 7
    printf("Level2 is OFF\n");
E 7
    ts->timer[0] = 0;   /* clear permit set to 0 ns */
D 7
    logMsg("download Level1 memory program\n",1,2,3,4,5,6);
E 7
I 7
    printf("download Level1 memory program\n");
E 7
    ts2_load_default_l1_program();
  }

  /************************************************/
  /************************************************/

  /* the number of events between scheduled syncs */
#ifdef TEST_SETUP
  ts->sync = 10000;
#else
  ts->sync = 1000;
#endif

  /* set TS roc enable register using CODA-generated rocMask */
  roc_enable_mask = roc_enable_mask_readback = 0;
  roc_enable_mask_bonus = 0;
  for(i=0; i<32; i++)
  {
    if(roc_map[i]>=0) roc_enable_mask |= ((rocMask>>i)&0x1) << roc_map[i];

/*BONUS*/
    if(roc_map_bonus[i]>=0) roc_enable_mask_bonus |= ((rocMask>>i)&0x1) << roc_map_bonus[i];
/*BONUS*/

  }
I 7

E 7
  ts->roc = roc_enable_mask;

  /* set OR module mask */
D 7
  orset = (volatile long *)(ppc_offset+ORBASE);
E 7
I 7
D 13
  orset = (volatile unsigned long *)(ppc_offset+ORBASE);
E 7


E 13
I 13
  orset = (volatile unsigned int *)(ppc_offset+ORBASE);
E 13
  *orset = roc_enable_mask;
I 13

E 13
I 7
EIEIO;
SYNC;

E 7
/* BONUS
  *orset = roc_enable_mask_bonus;
BONUS*/

D 7

  ores = (volatile long *)(ppc_offset+ORBASE);
E 7
I 7
D 13
  ores = (volatile unsigned long *)(ppc_offset+ORBASE);
E 13
I 13
  ores = (volatile unsigned int *)(ppc_offset+ORBASE);
E 13
E 7
  roc_enable_mask_readback = *ores;
  if(roc_enable_mask != roc_enable_mask_readback)
  {
D 7
    logMsg("ERROR: OR module mask not set properly\n",1,2,3,4,5,6);
    logMsg("ERROR: write 0x%08x , read 0x%08x\n",
             roc_enable_mask,roc_enable_mask_readback,3,4,5,6);
E 7
I 7
    printf("ERROR: OR module mask not set properly\n");
    printf("ERROR: write 0x%08x , read 0x%08x\n",
             roc_enable_mask,roc_enable_mask_readback);
E 7
  }
D 7
  logMsg("csr,roc,trig,or: 0x%08x 0x%08x 0x%08x 0x%08x\n",
    ts->csr,ts->roc,ts->trig,*ores,5,6);
  logMsg("roc_enable_mask = 0x%08x, rocMask = 0x%08x\n",
    roc_enable_mask,rocMask,3,4,5,6);
  logMsg("timers: %8d %8d %8d %8d %8d, sync=%8d\n",
E 7
I 7

I 13



E 13
  printf("csr,roc,trig,or (or ards): 0x%08x 0x%08x 0x%08x 0x%08x (0x%08x)\n",
    ts->csr,ts->roc,ts->trig,*ores,ores);
D 13
  printf("roc_enable_mask = 0x%08x, rocMask = 0x%08x\n",
    roc_enable_mask,rocMask);
E 13
I 13

  printf("roc_enable_mask = 0x%08x\n",roc_enable_mask);
  printf("roc_enable_mask = 0x%08x\n",roc_enable_mask);
  printf("roc_enable_mask = 0x%08x\n",roc_enable_mask);
  printf("roc_enable_mask = 0x%08x\n",roc_enable_mask);
  printf("roc_enable_mask = 0x%08x\n",roc_enable_mask);

  printf("rocMask = 0x%08x\n",rocMask);
  printf("rocMask = 0x%08x\n",rocMask);
  printf("rocMask = 0x%08x\n",rocMask);
  printf("rocMask = 0x%08x\n",rocMask);
  printf("rocMask = 0x%08x\n",rocMask);

E 13
  printf("timers: %8d %8d %8d %8d %8d, sync=%8d\n",
E 7
    ts->timer[0]&0xFFFF,ts->timer[1]&0xFFFF,ts->timer[2]&0xFFFF,
    ts->timer[3]&0xFFFF,ts->timer[4]&0xFFFF,ts->sync&0xFFFF);

  printf("INFO: User Prestart Executed\n");

  /* from parser */
  if(__the_event__) WRITE_EVENT_;
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{

  ts->csr = 0x410000; /* reset go (before CDODISABLE() !!!) and EN1 */
I 13

#ifdef USEPROGRAM1
  ts->csr2 &= ~(0x0400);
#endif

E 13
  CDODISABLE(VME,2,0);

  /* disable dummy triggers */
  VMEflag = 0;

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}  

static void
__end()
{
D 13
  int ct;
E 13
I 13
  int ct, i;
  unsigned int tmp;
E 13

I 13
#ifdef PRIMEX
#ifdef DO_PHASES
  set_current_phase(-1); /*in ts2config.c*/
#endif
#endif

E 13
  /* disable dummy triggers */
  VMEflag = 0;

  /* set sync-and-pause to generate sync event and reset go bit */
  /* we want to force force_sync event and stop on it */
  ts->csr = 0x4;

  /* wait for go bit to clear max 3 secs */
  ct=0;
  while((ct<3)&&((ts->csr)&0x1)!=0)
  {
    ct++;
    taskDelay(sysClkRateGet());
  }

  /* check if go cleared */
  if(((ts->csr)&0x1)!=0)
  {
    logMsg("TS GO bit not clear after 3 sec in end transition\n",1,2,3,4,5,6);
    logMsg("TS csr,state registers are:   0x%08lx,  0x%08lx\n",
      ts->csr,ts->state,3,4,5,6);
  }
I 13
  else
  {
    logMsg("TS GO bit cleared\n",1,2,3,4,5,6);
  }
E 13

I 13
  /* check state register 0x6C from TS Active down */
  /* must wait until not active ??? */
  /* 0x14, 0x18 */
E 13

I 13
  /* disable force sync events */
  setForceSyncInterval(0);

E 13
  ts->csr = 0x410000; /* reset go (before CDODISABLE() !!!) and EN1 */
I 13
#ifdef USEPROGRAM1
  ts->csr2 &= ~(0x0400);
  /* wait for bit 14 of state to become 0 */
  ct=0;
  for(i=0; i<10; i++)
  {
    SYNC;
    EIEIO;
    tmp = ts->state;
    SYNC;
    EIEIO;
    taskDelay(sysClkRateGet());
	/*
  logMsg("TS state:  0x%08x\n",tmp,2,3,4,5,6);
	*/
    if(tmp & 0x4000)
	{
	  ct++;
	}
    else
	{
      break;
	}
  }
  ts->csr2 &= ~(0x0400);
  logMsg("Nloops=%d\n",ct,2,3,4,5,6);

#endif


E 13
  CDODISABLE(VME,2,0);


I 13
  logMsg("TS before csr:  0x%08x\n",ts->csr,2,3,4,5,6);
  logMsg("TS before csr2:  0x%08x\n",ts->csr2,2,3,4,5,6);
  logMsg("TS before state:  0x%08x\n",ts->state,2,3,4,5,6);


E 13
  /* reset TS go, pause-on-next-sync, sync-and-pause, and force-sync bits
  just in case */
  ts->csr = 0xf0000;

I 13
  logMsg("TS after1 csr:  0x%08x\n",ts->csr,2,3,4,5,6);
  logMsg("TS after1 csr2:  0x%08x\n",ts->csr2,2,3,4,5,6);

E 13
  /* reset TS state machines just in case */
  ts->csr = 0x4000;

D 13
  logMsg("TS csr:  0x%08x\n",ts->csr,2,3,4,5,6);
E 13
I 13
  logMsg("TS after2 csr:  0x%08x\n",ts->csr,2,3,4,5,6);
  logMsg("TS after2 csr2:  0x%08x\n",ts->csr2,2,3,4,5,6);
E 13

D 13
  /* disable force sync events */
  setForceSyncInterval(0);
E 13



  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

static void
__go()
{
I 13
  int i;
  volatile unsigned short *p;

E 13
  /* disable dummy triggers */
  VMEflag = 0;

I 13
#ifdef USEPROGRAM1
/*#ifndef TEST_SETUP*/
  /* set force sync for helicity strobs */
  ts2SetProgram1Event(15, 0);
/*#endif*/
#endif

E 13
  /* set force sync params: event type = 0, sync flag = 1,
  interval = SYNCINTERVAL sec */
D 13
  ts2SetProgram1Event(0, 1);
E 13
I 13
  
/*#ifndef TEST_SETUP*/
  ts2SetProgram2Event(0, 1);
E 13
  setForceSyncInterval(SYNCINTERVAL);
I 13
/*#endif*/
E 13

  /* reset TS scalers */
  ts->scalControl = 0xFFFFF;

  /* assign scalers */
D 13
  /*ts->scalAssign = ... */
E 13
I 13
  ts->scalAssign = 0x0000FE; /*scaler12=Program1Events, scaler14=Program2Events*/
E 13

  /* enable L1 */
  ts->csr = 0x40;

D 13
  logMsg("TS csr: 0x%08x\n",ts->csr,2,3,4,5,6);
E 13
I 13
#ifndef TEST_SETUP
#ifdef FROMSCALER1
E 13

I 13
  /* setup software scalers on first go transition only 
  if(run_go_count==1)*/
  {
    for(i=0; i<12; i++) run_l1_count[i]=0;
    run_latch1_zero_count=0;
    run_latch1_empty_count=0;
    run_latch1_not_empty_count=0;
    run_latch1_ok_count=0;
    run_latch2_zero_count=0;
    run_latch2_empty_count=0;
    run_latch2_not_empty_count=0;
    run_latch2_ok_count=0;
    run_l2pass=0;
    run_l2fail=0;
    run_l2s1=0;
    run_l2s2=0;
    run_l2s3=0;
    run_l2s4=0;
    run_l2s5=0;
    run_l2s6=0;
    run_roc13_count=0;
    run_roc15_count=0;
    run_l1l2_zero_count=0;
    run_13_l1_zero_count=0;
    run_13_l2_zero_count=0;
    run_13_l1l2_zero_count=0;
  }


  /*latch1*/
  p = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x0);
  EIEIO;
  SYNC;  
  *p=0x80;                      /* reset */
  EIEIO;
  SYNC;  
  *p=0x6;                       /* enable front panel, use buffer signal */
  EIEIO;
  SYNC;  

  /*latch2*/
  p = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x0);
  EIEIO;
  SYNC;  
  *p=0x80;                      /* reset */
  EIEIO;
  SYNC;  
  *p=0x6;                       /* enable front panel, use buffer signal */
  EIEIO;
  SYNC;  
#endif
#endif

  printf("TS csr: 0x%08x\n",ts->csr);

E 13
  CDOENABLE(VME,2,0);

D 13
/* disable branch 5
#ifdef USE_TS2
ts->csr2 &= ~(0x1000);
E 13
I 13
#ifdef USEPROGRAM1
  /*enable external Program1 */
  ts->csr2 |= 0x0400;
E 13
#endif
D 13
*/
E 13

I 13
  /* SYNC/EIEIO ???*/

#ifdef PRIMEX
#ifdef DO_PHASES
  set_current_phase(0); /*in ts2config.c*/
#else
  ts->csr = 0x1;
#endif
#else

E 13
  /* set 'go' bit; must be LAST operation, after that for example
     'ts->csr2' will be unaccessible, so CDOENABLE() will not work */
  ts->csr = 0x1;
I 13
#endif
E 13


  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

void
D 13
ts_trig(unsigned long EVTYPE, unsigned long EVSOURCE)
E 13
I 13
ts_trig(unsigned int EVTYPE, unsigned int EVSOURCE)
E 13
{
D 13
  long EVENT_LENGTH;
  int i, ind, ncol, nrow, blen;
  unsigned int livetime, livetime_corr, live1, live2, live1_corr, live2_corr;
  unsigned long *secondword, *jw, scalbuf1[16], scalbuf2[16];
E 13
I 13
  int EVENT_LENGTH;
  int i, ind, ind2, ncol, nrow, blen;
  unsigned int livetime, livetime_corr, live1, live2, live1_corr, live2_corr, tgbi1;
  unsigned int *secondword, *jw, scalbuf1[18], *buf, psync;
  volatile unsigned short *p,*p1,*p2;
  unsigned short temp;
  unsigned int latch1=0;
  unsigned int latch2=0;
E 13

I 13
  /*logMsg("ts_trig reached\n",1,2,3,4,5,6);*/
E 13

D 13
  rol->dabufp = (long *) 0;
E 13
I 13
  rol->dabufp = (int *) 0;
E 13

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

D 13
  /*logMsg("EVTYPE=%d syncFlag=%d\n",EVTYPE,syncFlag,3,4,5,6);*/
E 13
I 13
  /*
  logMsg("EVTYPE=%d syncFlag=%d\n",EVTYPE,syncFlag,3,4,5,6);
  */
E 13


D 13

E 13
  jw = rol->dabufp;
  /*jw[ILEN] = 1;*/ jw[-2] = 1;
  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */

I 13
  psync = 0;
E 13
  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    logMsg("ERROR: syncFlag<0 || syncFlag>1\n",0,0,0,0,0,0);
  }
D 13
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
E 13
I 13
  else if((syncFlag==0)&&(EVTYPE==0))
E 13
  {
    logMsg("ERROR: syncFlag==0 && EVTYPE==0\n",0,0,0,0,0,0);
  }
D 13
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync events */
E 13
I 13
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
E 13
  {
D 13
goto a11;
    /*logMsg("3\n",0,0,0,0,0,0);*/
E 13

I 13
#ifndef TEST_SETUP
#ifdef FROMSCALER1
    /* trigger latch banks...set latch words to 0 for force_sync */

    ind2 = bosMopen_(jw,"+TRG",0,5,1);
    buf = (int *)&jw[ind2+1];

    *buf++=0;     
    *buf++=0; 
    *buf++=0; 
    *buf++=0; 
    *buf++=0; 

    rol->dabufp += bosMclose_(jw,ind2,5,1);


    /* check latch card buffers are empty */

    p1 = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x6);
    EIEIO;
    SYNC;  
    if(*p1!=0)
    { 
      run_latch1_not_empty_count++; 
      sync_status=1;
      if((run_latch1_not_empty_count%1000)==1)
      {
        logMsg("ERROR: Latch1 buffer not empty for sync event, count:  %10d, Run %10d, Event %10d\n",
			   *p1, RUN_NUMBER, EVENT_NUMBER,4,5,6);
      }
      p2 = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x0);
      EIEIO;
      SYNC;  
      temp=*p2&0xF;
      EIEIO;
      SYNC;  
      *p2=0x80;
      EIEIO;
      SYNC;  
      *p2=temp;
      EIEIO;
      SYNC;  
    }

    p1 = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x6);
    EIEIO;
    SYNC;  
    if(*p1!=0)
    {
      run_latch2_not_empty_count++; 
      sync_status=1;
      if((run_latch2_not_empty_count%1000)==1)
      {
        logMsg("ERROR: Latch2 buffer not empty for sync event, count:  %10d, Run %10d, Event %10d\n",
          *p1,RUN_NUMBER, EVENT_NUMBER,4,5,6);
      }
      p2 = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x0);
      EIEIO;
      SYNC;  
      temp=*p2&0xF;
      EIEIO;
      SYNC;  
      *p2=0x80;
      EIEIO;
      SYNC;  
      *p2=temp;
      EIEIO;
      SYNC;  
    }  



    /* software scaler bank */
    ind2 = bosMopen_(jw,"S1ST",0,35,1);
    buf = (int *)&jw[ind2+1];

    for(i=0; i<12; i++) *buf++=run_l1_count[i];
    *buf++=0/*run_phys_count+run_phys_sync_count*/;
    *buf++=run_latch1_zero_count;
    *buf++=run_latch1_empty_count;
    *buf++=run_latch1_not_empty_count;
    *buf++=run_latch1_ok_count;
    *buf++=run_l2s1;
    *buf++=run_l2s2;
    *buf++=run_l2s3;
    *buf++=run_l2s4;
    *buf++=run_l2s5;
    *buf++=run_l2s6;
    *buf++=run_l2pass;
    *buf++=run_l2fail;
    *buf++=run_latch2_zero_count;
    *buf++=run_latch2_empty_count;
    *buf++=run_latch2_not_empty_count;
    *buf++=run_latch2_ok_count;
    *buf++=run_roc13_count;
    *buf++=run_roc15_count;
    *buf++=run_l1l2_zero_count;
    *buf++=run_13_l1_zero_count;
    *buf++=run_13_l2_zero_count;
    *buf++=run_13_l1l2_zero_count;

    rol->dabufp += bosMclose_(jw,ind2,35,1);


#endif
#endif






#ifdef PRIMEX_
E 13
    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;
D 13
a11:
E 13
I 13
#endif
E 13
D 8
    ts->scalControl = 0x800000; /* latch all TS scalers (including live ???); they will keep count */
E 8
I 8
    ts->scalControl = 0x800000; /* latch all TS scalers (including live ???); they will keep counting */
E 8

D 13
/* temp */
	for(i=0; i<18; i++) scalbuf1[i] = ts->scaler[i]//*6*/SYNCINTERVAL; /* SYNCINTERVAL sec interval */
/* temp */
E 13
I 13
	for(i=0; i<18; i++) scalbuf1[i] = ts->scaler[i]/SYNCINTERVAL; /* SYNCINTERVAL sec interval */
E 13

D 13
goto a12;
E 13
I 13
#ifdef PRIMEX_
E 13
    *rol->dabufp++ = ts->scalAssign;
    for(i=0; i<18; i++) *rol->dabufp++ = ts->scaler[i];
    *rol->dabufp++ = ts->scalEvent;
D 13
a12:
E 13
I 13
#endif
E 13
    live1 = ts->scalLive1; /* reading scalLive1 latch scalLive2 automatically */
    live2 = ts->scalLive2; /* live time = scalLive1 / scalLive2 */
D 13
goto a13;
E 13
I 13
#ifdef PRIMEX_
E 13
    *rol->dabufp++ = live1;
    *rol->dabufp++ = live2;
D 13
a13:
D 8
    ts->scalControl = 0xFFFFF; /* reset TS scalers *//*error: do not want to reset event scaler
E 8
I 8
    ts->scalControl = 0xBFFFF/*0xFFFFF*/; /* reset TS scalers *//*error: do not want to reset event scaler
E 8
								  19 - should do 0xBFFFF */
E 13
I 13
#endif
    ts->scalControl = 0xBFFFF; /* reset TS scalers except event scaler */
E 13

    /* some prints 
    logMsg("\nscaler assign -> %08x\n",secondword[3],0,0,0,0,0);
    logMsg("inputs  1-6: %8u %8u %8u %8u %8u %8u\n",
      secondword[4],secondword[5],secondword[6],
      secondword[7],secondword[8],secondword[9]);
    logMsg("inputs 7-12: %8u %8u %8u %8u %8u %8u\n",
      secondword[10],secondword[11],secondword[12],
      secondword[13],secondword[14],secondword[15]);
	*/
/*temp*/
    logMsg("inputs  1-6: %8u %8u %8u %8u %8u %8u\n",
      scalbuf1[0],scalbuf1[1],scalbuf1[2],
      scalbuf1[3],scalbuf1[4],scalbuf1[5]);
    logMsg("inputs 7-12: %8u %8u %8u %8u %8u %8u\n",
      scalbuf1[6],scalbuf1[7],scalbuf1[8],
      scalbuf1[9],scalbuf1[10],scalbuf1[11]);
I 13
    logMsg("Program Events 1-2: %8u %8u\n",scalbuf1[12],scalbuf1[13]);
E 13
/*temp*/

    if(live2 > 0) livetime = (live1*100)/live2;
    else          livetime = -1;

    live1_corr = correct_counter(live1);
    live2_corr = correct_counter(live2);

    if(live2_corr > 0) livetime_corr = (live1_corr*100)/live2_corr;
    else               livetime_corr = -1;

    logMsg("live time = %d percent (gated=%d ungated=%d)\n\n",
            livetime,live1,live2,0,0,0);

    logMsg("live_corr = %d percent (gated=%d ungated=%d)\n\n",
            livetime_corr,live1_corr,live2_corr,0,0,0);

    ts2UpdateStatistics(livetime, scalbuf1);


D 13
goto a14;
E 13
I 13
#ifdef PRIMEX_
E 13
    /* close data bank */
D 13
    blen = rol->dabufp - (long *)&jw[ind+1];
E 13
I 13
    blen = rol->dabufp - (int *)&jw[ind+1];
E 13
    bosMclose_(jw,ind,1,blen);
D 13
a14:
    ;
E 13
I 13
#endif


E 13
  }
I 13
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    /* create empty +TRG bank */
    ind2 = bosMopen_(jw,"+TRG",0,5,1);
    buf = (int *)&jw[ind2+1];
    *buf++=0;
    *buf++=0;
    *buf++=0;
    *buf++=0;
    *buf++=0;
    rol->dabufp += bosMclose_(jw,ind2,5,1);
  }
E 13
  else           /* physics and physics_sync events */
  {

I 13

#ifndef TEST_SETUP
#ifdef FROMSCALER1

    /* trigger latch bank...check latch buffers have data for normal events */
    /* count l1 bits, check for zero latch word, etc. */

    ind2 = bosMopen_(jw,"+TRG",0,5,1);
    buf = (unsigned int *)&jw[ind2+1];


    /* latch 1 */

    p = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x6);
    EIEIO;
    SYNC;
    if(*p>0)
    {
      EIEIO;
      SYNC;  
      p2 = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x8);
      EIEIO;
      SYNC;
      latch1 = *p2;
      EIEIO;
      SYNC;
      /*if(latch1==0) logMsg("latch1=%d\n",latch1,2,3,4,5,6);*/

      /*form first word of the bank: bits 0-11 from TS2 latch, bits 12-15 from the same bits of latch1,
        bits 16-27 from bits 0-11 of latch1*/
      tgbi1 = ((latch1 & 0xFFF)<<16) + (latch1 & 0xF000) + (ts->trigData & 0xFFF);

	  *buf++= tgbi1; /* 1st word in bank */
      if((tgbi1&0xFFF)==0)
      {
        run_latch1_zero_count++;
	    if(EVTYPE==13)
        {
          run_13_l1_zero_count++;
	    }
        else
        {
          if(((run_latch1_zero_count-run_13_l1_zero_count)%100)==1)
          {
            logMsg("ERROR: Latch1 zero for physics event, Run %7d, Event %7d type %d\n",
              RUN_NUMBER,EVENT_NUMBER,EVTYPE,4,5,6);
          }
        }
      }
      else
      {
        run_latch1_ok_count++;
        for (i=0; i<12; i++) run_l1_count[i]+=(latch1>>i)&0x1;
      }
    }
    else
    {
      *buf++=0; /* 1st word in bank */
      latch1=-1;
      run_latch1_empty_count++;
      sync_status=1;
      psync|=1;
      /*if((run_latch1_empty_count%1000)==1)*/
      {
		/*sergey
        logMsg("ERROR: Latch1 buffer empty for physics event for Run %7d, Event %7d\n",
		RUN_NUMBER, EVENT_NUMBER,3,4,5,6)*/;
      }
    }


    /* helicity synch scaler */
    /* *buf++=*((unsigned int*)(ppc_offset+trscal[0]+0x10)+13);*/ /* 2nd word in bank */
    /* 1 MHz clock */
    /* *buf++=*((unsigned int*)(ppc_offset+trscal[0]+0x10)+2);*/ /* 3rd word in bank */
    *buf ++= 0;
    *buf ++= 0;


    /* latch 2 */

    p = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x6);
    EIEIO;
    SYNC;
    if(*p>0)
    {
      EIEIO;
      SYNC;  
      p2 = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x8);
      EIEIO;
      SYNC;
      latch2 = *p2;
      EIEIO;
      SYNC;
      /*if(latch2==0) logMsg("latch2=%d\n",latch2,2,3,4,5,6);*/

      *buf++ = latch2;  /* 4th word in bank */
      if((latch2&0xffff)==0)
      {
        run_latch2_zero_count++;
        if(EVTYPE==13)
        {
          run_13_l2_zero_count++;
	    }
        else
        {
/*
          if(((run_latch2_zero_count-run_13_l2_zero_count)%1000)==1)
          {
            logMsg("ERROR: Latch2 zero for physics event, Run %10d, Event %10d\n",
              RUN_NUMBER,EVENT_NUMBER,3,4,5,6);
          }
*/
        }
      }
      else
      {
        run_latch2_ok_count++;
  	    if(latch2 & 0x2000) run_l2pass++;
  	    if(latch2 & 0x1000) run_l2fail++;
  	    if(latch2 & 0x0001) run_l2s1++;
  	    if(latch2 & 0x0002) run_l2s2++;
  	    if(latch2 & 0x0004) run_l2s3++;
  	    if(latch2 & 0x0008) run_l2s4++;
  	    if(latch2 & 0x0010) run_l2s5++;
  	    if(latch2 & 0x0020) run_l2s6++;
      }
    }
    else
    {
      *buf++=0;  /* 4th word in bank */
      latch2 = -1;
      run_latch2_empty_count++;
      sync_status=1;
      psync|=1;
      if((run_latch2_empty_count%1000)==1)
      {
        logMsg("ERROR: Latch2 buffer empty for physics event for Run %10d, Event %10d\n",
          RUN_NUMBER, EVENT_NUMBER,3,4,5,6);
      }
    }

    *buf++=0;   /* 5th word in bank - just reserve space for level3 */
    rol->dabufp += bosMclose_(jw,ind2,5,1);



    /* count roc code 13,15 events (zero trigger word, illegal trigger) */
    if(EVTYPE==13) run_roc13_count++;
    if(EVTYPE==15) run_roc15_count++;

    /* check correlations between zero latch and roc 13 */
    if(((latch1&0xfff)==0)&&((latch2&0xffff)==0))
    {
      run_l1l2_zero_count++;
      if(EVTYPE==13) run_13_l1l2_zero_count++;
    }
#endif
#endif

E 13
	/*
	{
      unsigned int tmp, c1, c2;
      c1 = ts->trigCount&0xFFFF;
      tmp = ts->trigData&0xFFF;
      c2 = ts->trigCount&0xFFFF;
      if(tmp == 0)
        logMsg("ts2(phys): ERROR: trigData=0x%08x befor=%d after=%d (%d %d %d)\n",
          tmp,c1,c2,0,0,0);
	}
	*/

D 13
/*(sergey: until latch1 does not work) goto a456;*/
E 13
I 13

E 13
    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;

    *rol->dabufp++ = (ts->trigData & 0xFFF); /* trigger bits 1-12 *//*that decrement trigCount??*/
    *rol->dabufp++ = ts->scalLive1;
    *rol->dabufp++ = ts->scalLive2; /* live time = scalLive1 / scalLive2 */
    if(syncFlag==1)
    {
      *rol->dabufp++ = time(NULL);
      /*logMsg("5\n",0,0,0,0,0,0);*/
    }
    else
    {
      *rol->dabufp++ = 0;
    }
    /* close data bank */
D 13
    blen = rol->dabufp - (long *)&jw[ind+1];
E 13
I 13
    blen = rol->dabufp - (int *)&jw[ind+1];
E 13
    bosMclose_(jw,ind,1,blen);
D 13
a456:
E 13

I 13













E 13
    /* for phys sync events trigCount must be zero*/
    if(syncFlag==1)
	{
      unsigned int c1;
      c1 = ts->trigCount&0xFFFF;
      if(c1 != 0)
        logMsg("ts2(sync): ERROR: trigCount=%d (%d %d %d %d %d)\n",
          c1,0,0,0,0,0);
I 13

#ifndef TEST_SETUP
#ifdef FROMSCALER1
      p1 = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x6);
      EIEIO;
      SYNC;  
      if(*p1!=0)
      { 
	    run_latch1_not_empty_count++; 
	    sync_status=1;
	    if((run_latch1_not_empty_count%1000)==1)
        {
	      logMsg("ERROR: Latch1 buffer not empty for sync event, count:  %10d for Run %10d, Event %10d\n",
            *p1,RUN_NUMBER, EVENT_NUMBER, 4,5,6);
        }
	    p2 = (volatile unsigned short *)(ppc_offset+LATCH1ADDR+0x0);
        EIEIO;
        SYNC;  
	    temp=*p2&0xF;
        EIEIO;
        SYNC;  
	    *p2=0x80;
        EIEIO;
        SYNC;  
	    *p2=temp;
        EIEIO;
        SYNC;  
      }
      p1 = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x6);
      EIEIO;
      SYNC;  
      if(*p1!=0)
      {
	    run_latch2_not_empty_count++; 
	    sync_status=1;
	    if((run_latch2_not_empty_count%1000)==1)
        {
          logMsg("ERROR: Latch2 buffer not empty for sync event, count:  %10d for Run %10d, Event %10d\n",
            *p1,RUN_NUMBER, EVENT_NUMBER,4,5,6);
        }
	    p2 = (volatile unsigned short *)(ppc_offset+LATCH2ADDR+0x0);
        EIEIO;
        SYNC;  
	    temp=*p2&0xF;
	    *p2=0x80;
        EIEIO;
        SYNC;  
        EIEIO;
        SYNC;  
	    *p2=temp;
        EIEIO;
        SYNC;  
      }
#endif
#endif

      psync|=sync_status;
      sync_status=0;

      /* create PTRN bank */
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000 + psync;
      rol->dabufp += bosMclose_(jw,ind2,1,1);

E 13
	}

I 13
#ifdef PRIMEX
#ifdef DO_PHASES
    check_phase_limit();
#endif
#endif
E 13

  }


  /*
  {
    int lll;
lll = bosMmsg_(jw,"INFO",rol->pid,"qwertyuiopasdfgh");
logMsg("message---> lll=%d\n",lll,2,3,4,5,6);
rol->dabufp += lll;
  }
  */

  /* close event */
  CECLOSE;

I 13
  /*logMsg("ts_trig done\n",1,2,3,4,5,6);*/

E 13
  return;
}

void
ts_trig_done()
{
I 13
  /*logMsg("ts_trig_done reached\n",1,2,3,4,5,6);*/
E 13
  return;
}  

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  CDOACK(VME,2,0);

  return;
}  

static void
__status()
{
  return;
}  
E 1
