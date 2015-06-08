/******************************************************************************
*
*  c-rams.h  -  Driver library header file for readout of a CAEN C-RAM system 
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          January 2000
*/

/* Define a Structures for access to  c-ram and c-ram sequencer*/

/* v551B structure */
struct c_rams_s_struct {
  volatile unsigned short int_vec;
  volatile unsigned short int_level;
  volatile unsigned short clear;
  volatile unsigned short trigger;
  volatile unsigned short status;
  volatile unsigned short test;
  volatile unsigned short channels;
  volatile unsigned short T[5];
  volatile unsigned short iDAC;
  volatile unsigned short blank[112];
  volatile unsigned short code;
  volatile unsigned short type;
  volatile unsigned short id;
};

/* v550 structure */
struct c_rams_struct {
  volatile unsigned short int_reg;
  volatile unsigned short status;
  volatile unsigned short channels;
  volatile unsigned short clear;
  volatile unsigned long  fifo[2];
  volatile unsigned short wc[2];
  volatile unsigned short tpat[2];
  volatile unsigned short blank[113];
  volatile unsigned short code;
  volatile unsigned short type;
  volatile unsigned short id;
  volatile unsigned long blank2[1984];
  volatile unsigned long ch[2][2048];
};

/* Define Max and Minimum Detector Channel readout */
#define CRAMS_NCHAN_MAX   2048
#define CRAMS_NCHAN_MIN   32

/* Define default interrupt vector/level */
#define CRAMS_INT_VEC      0xca
#define CRAMS_VME_INT_LEVEL   3

/* Define default T values for Sequencer */
#define CRAMS_S_T1_VALUE    80
#define CRAMS_S_T2_VALUE    18   
#define CRAMS_S_T3_VALUE    6
#define CRAMS_S_T4_VALUE    18
#define CRAMS_S_T5_VALUE    10
#define CRAMS_S_T6_VALUE    3

/* Define Status bits */
#define CRAMS_STATUS_TEST_MODE                0x1
#define CRAMS_STATUS_MEMORY_OWNER             0x2
#define CRAMS_STATUS_CH0_DATA                 0x4
#define CRAMS_STATUS_CH1_DATA                 0x8
#define CRAMS_STATUS_FIFO0_EMPTY             0x10
#define CRAMS_STATUS_FIFO1_EMPTY             0x20
#define CRAMS_STATUS_FIFO0_HALF_FULL         0x40
#define CRAMS_STATUS_FIFO1_HALF_FULL         0x80
#define CRAMS_STATUS_FIFO0_FULL             0x100
#define CRAMS_STATUS_FIFO1_FULL             0x200

#define CRAMS_S_STATUS_ID                 0x01
#define CRAMS_S_STATUS_VETO               0x02
#define CRAMS_S_STATUS_AUTO               0x04
#define CRAMS_S_STATUS_DRDY               0x08
#define CRAMS_S_STATUS_BUSY               0x10
#define CRAMS_S_STATUS_ACTIVE             0x20

#define CRAMS_S_TEST_MODE            0x1
#define CRAMS_S_TEST_CLOCK           0x2
#define CRAMS_S_TEST_SHIFT           0x4
#define CRAMS_S_TEST_PULSE           0x8

#define CRAMS_S_TEST_MAX_CONV        64

/* Define TEST Macros */
#define CRAMS_TEST_MODE_ON         {crams_s->test = CRAMS_S_TEST_MODE;}
#define CRAMS_TEST_MODE_OFF        {crams_s->test = 0;}
#define CRAMS_TEST_SHIFT_ON        {crams_s->test |= CRAMS_S_TEST_SHIFT;}
#define CRAMS_TEST_SHIFT_OFF       {crams_s->test &= ~CRAMS_S_TEST_SHIFT;}
#define CRAMS_TEST_CLOCK_ON        {crams_s->test |= CRAMS_S_TEST_CLOCK;}
#define CRAMS_TEST_CLOCK_OFF       {crams_s->test &= ~CRAMS_S_TEST_CLOCK;}
#define CRAMS_TEST_PULSE_ON        {crams_s->test |= CRAMS_S_TEST_PULSE;}
#define CRAMS_TEST_PULSE_OFF       {crams_s->test &= ~CRAMS_S_TEST_PULSE;}

/* Define a delay Macro */
#define T_DELAY(c) {int kk; double b=1.0; for(kk=0;kk<c;kk++)b+=(1.0/b);}
#define TDELAY_COUNT               1000
#define TDELAY_COUNT_L             2000

#define CRAMS_GET_CRAMS_ID(id)     {id/nCRAMS;}
#define CRAMS_GET_ADC_ID(id)       {id%2;}

/* Define Masks */
#define CRAMS_S_TEST_MASK             0xf
#define CRAMS_S_STATUS_MASK           0x3f
#define CRAMS_STATUS_MASK             0x03ff
#define CRAMS_PEDESTAL_MASK           0x00fff000
#define CRAMS_THRESHOLD_MASK          0x00000fff
#define CRAMS_WC_MASK                 0x07ff
#define CRAMS_CHANNELS_MASK           0x0fff
#define CRAMS_ADC0_CHAN_MASK          0x003f
#define CRAMS_ADC1_CHAN_MASK          0x0fc0

/* Define Data masks */
#define CRAMS_DATA_HEADER             0xcd000000
#define CRAMS_DATA_OVER               0x80000000
#define CRAMS_DATA_VALID              0x40000000
#define CRAMS_DATA_CHANNEL_MASK       0x007ff000
#define CRAMS_DATA_MASK               0x00000fff


#ifndef STATUS
#define STATUS int
#endif

#ifndef UINT32
#define UINT32 unsigned int
#endif

/* Function prototypes */
STATUS cramsInit (UINT32 saddr, UINT32 addr_inc, int ncrams);
STATUS cramsTprog(unsigned short ch, unsigned short t1, 
               unsigned short t2, unsigned short t3,
               unsigned short t4, unsigned short t5);
void cramsTprint();
void cramsTrig();
void cramsAutoTrig(int on);
unsigned short cramsTest(unsigned short val);
void cramsTestPulse(int chanID, int pcount, int delay);
void cramsClear();
STATUS cramsWaitActive(int count);
void cramsStatus();
void cramsPrintData(int pflag, int detID, unsigned short maxWords);
int  cramsReadData(int detID, unsigned long *buf,  unsigned short maxWords);
STATUS cramsPTRead(int detID, int ch, unsigned short *ped_val, unsigned short *th_val);
STATUS cramsPTWrite(int detID, int ch, unsigned short ped_val, unsigned short th_val);
STATUS cramsPTZero(int detID);
STATUS cramsProg(int detID, int nChan);
STATUS cramsTestProg( int detID, unsigned short nConv, unsigned short dacVal);








 


