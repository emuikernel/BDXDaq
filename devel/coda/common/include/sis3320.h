/* sis3320.h */

/* for David J Abbott board
#define SIS3320_BOARD_ID 0x33200105
*/
#define SIS3320_BOARD_ID 0x33200102

#define SIS3320_ADC12           0x02000000 /* start ADC12 area */
#define SIS3320_ADC34           0x02800000 /* start ADC34 area */
#define SIS3320_ADC56           0x03000000 /* start ADC56 area */
#define SIS3320_ADC78           0x03800000 /* start ADC78 area */

#define TRIGGER_PEAK       0 /*???*/
#define TRIGGER_GAP        0 /*???*/
#define TRIGGER_PULSE_LEN  0 /*???*/
#define TRIGGER_THRESHOLD  0 /*???*/
#define RING_TRIGGER       0 /*???*/


/* */
typedef struct sis3320ctrl_struct
{
  /*0x0000*/ volatile unsigned int control;               /* read/write; D32 */
  /*0x0004*/ volatile unsigned int firmware;              /* read only; D32 */
  /*0x0008*/ volatile unsigned int intConfig;             /* read/write; D32 */
  /*0x000c*/ volatile unsigned int intControl;            /* read/write; D32 */
  /*0x0010*/ volatile unsigned int acquistionControl;     /* read/write; D32 */
  /*0x0014*/ volatile unsigned int startDelay;            /* read/write; D32 */
  /*0x0018*/ volatile unsigned int stopDelay;             /* read/write; D32 */
  /*0x001c*/ volatile unsigned int res1;
  /*0x0020*/ volatile unsigned int maxNevents;            /* read/write; D32 */
  /*0x0024*/ volatile unsigned int eventCount;            /* read; D32 */
             volatile unsigned int res2[2];
  /*0x0030*/ volatile unsigned int cbltBroadcastSetup;    /* read/write; D32 */
  /*0x0034*/ volatile unsigned int ADCMemoryPage;         /* read/write; D32 */
             volatile unsigned int res3[6];
  /*0x0050*/ volatile unsigned int dacCSR;                /* read/write; D32 */
  /*0x0054*/ volatile unsigned int dacData;               /* read/write; D32 */
  /*0x0058*/ volatile unsigned int ADCGainControl;        /* read/write; D32 */
  /*0x005c*/ volatile unsigned int res4;
  /*0x006x*/ volatile unsigned int res5[4];
  /*0x007x*/ volatile unsigned int res6[4];
  /*0x008x*/ volatile unsigned int res7[32];
  /*0x01xx*/ volatile unsigned int res8[64];
  /*0x02xx*/ volatile unsigned int res9[64];
  /*0x03xx*/ volatile unsigned int res10[64];
  /*0x0400*/ volatile unsigned int keyReset;              /* write only; D32 */
             volatile unsigned int res11[3];
  /*0x0410*/ volatile unsigned int keyArm;                /* write only; D32 */
  /*0x0414*/ volatile unsigned int keyDisarm;             /* write only; D32 */
  /*0x0418*/ volatile unsigned int keyStart;              /* write only; D32 */
  /*0x041c*/ volatile unsigned int keyStop;               /* write only; D32 */
             volatile unsigned int res12[2];
  /*0x0428*/ volatile unsigned int keyResetDDR2;          /* write only; D32 */

} SIS3320CTRL;


/* */
typedef struct sis3320cnfgtot_struct
{
  /*0x01000000*/ volatile unsigned int eventConfigAllADC;           
  /*0x01000004*/ volatile unsigned int sampleLengthAllADC;          
  /*0x01000008*/ volatile unsigned int sampleStartAddressAllADC;   
  /*0x0100000C*/ volatile unsigned int ADCInputModeAllADC;         
                 volatile unsigned int res13[7];
  /*0x0100002C*/ volatile unsigned int triggerFlagClrCntAllADC;

} SIS3320CNFGTOT;


/* structure to control ADC pairs: ADC12, ADC34, ADC56 or ADC78 */
typedef struct sis3320cnfg_struct
{
  /*0x02000000*/ volatile unsigned int eventConfig;
  /*0x02000004*/ volatile unsigned int sampleLength;
  /*0x02000008*/ volatile unsigned int sampleStart;
  /*0x0200000C*/ volatile unsigned int adcInputMode;
  /*0x02000010*/ volatile unsigned int nextSampAdr[2];
                 volatile unsigned int res2[2];
  /*0x02000020*/ volatile unsigned int actualSampleValue;
  /*0x02000024*/ volatile unsigned int testReg;
  /*0x02000028*/ volatile unsigned int ddr2MemLogVerify;
  /*0x0200002C*/ volatile unsigned int trigFlagClearCtr;
  /*0x02000030*/ volatile unsigned int trigReg[2][2];
                 volatile unsigned int res4[16368];
  /*0x02010000*/ volatile unsigned int eventDir[2][512];
                 volatile unsigned int res5[2079744];

} SIS3320CNFG;


/* */
typedef struct sis3320_struct
{
  volatile SIS3320CTRL    *ctrl;
  volatile SIS3320CNFGTOT *cnfgtot;
  volatile SIS3320CNFG    *cnfg[4];
  volatile unsigned int   *data[8];

} SIS3320;



#define SIS3320_CONTROL_STATUS                 0x0    /* read/write; D32 */
#define SIS3320_MODID                          0x4    /* read only; D32 */
#define SIS3320_IRQ_CONFIG                     0x8    /* read/write; D32 */
#define SIS3320_IRQ_CONTROL                    0xC    /* read/write; D32 */
#define SIS3320_ACQUISTION_CONTROL             0x10   /* read/write; D32 */
#define SIS3320_START_DELAY                    0x14   /* read/write; D32 */
#define SIS3320_STOP_DELAY                     0x18   /* read/write; D32 */
#define SIS3320_MAX_NOF_EVENT                  0x20   /* read/write; D32 */
#define SIS3320_ACTUAL_EVENT_COUNTER           0x24   /* read; D32 */
#define SIS3320_CBLT_BROADCAST_SETUP           0x30   /* read/write; D32 */
#define SIS3320_ADC_MEMORY_PAGE_REGISTER       0x34   /* read/write; D32 */
#define SIS3320_DAC_CONTROL_STATUS             0x50   /* read/write; D32 */
#define SIS3320_DAC_DATA                       0x54   /* read/write; D32 */
#define SIS3320_ADC_GAIN_CONTROL               0x58   /* read/write; D32 */
#define SIS3320_KEY_RESET                      0x400  /* write only; D32 */
#define SIS3320_KEY_ARM                        0x410  /* write only; D32 */
#define SIS3320_KEY_DISARM                     0x414  /* write only; D32 */
#define SIS3320_KEY_START                      0x418  /* write only; D32 */
#define SIS3320_KEY_STOP                       0x41C  /* write only; D32 */
#define SIS3320_KEY_RESET_DDR2_LOGIC           0x428  /* write only; D32 */


#define SIS3320_EVENT_CONFIG_ALL_ADC           0x01000000
#define SIS3320_SAMPLE_LENGTH_ALL_ADC          0x01000004
#define SIS3320_SAMPLE_START_ADDRESS_ALL_ADC   0x01000008
#define SIS3320_ADC_INPUT_MODE_ALL_ADC         0x0100000C
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ALL_ADC   0x0100002C


#define SIS3320_ADC_INPUT_MODE_ADC12           0x0200000C
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC1     0x02000010
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC2     0x02000014
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC12      0x02000020
#define SIS3320_DDR2_TEST_REGISTER_ADC12	   0x02000028
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC12	   0x0200002C
#define SIS3320_TRIGGER_SETUP_ADC1             0x02000030
#define SIS3320_TRIGGER_THRESHOLD_ADC1         0x02000034
#define SIS3320_TRIGGER_SETUP_ADC2             0x02000038
#define SIS3320_TRIGGER_THRESHOLD_ADC2         0x0200003C
#define SIS3320_EVENT_DIRECTORY_ADC1		   0x02010000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC2		   0x02018000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_ADC_INPUT_MODE_ADC34           0x0280000C
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC3     0x02800010
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC4     0x02800014
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC34      0x02800020
#define SIS3320_DDR2_TEST_REGISTER_ADC34	   0x02800028
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC34	   0x0280002C
#define SIS3320_TRIGGER_SETUP_ADC3             0x02800030
#define SIS3320_TRIGGER_THRESHOLD_ADC3         0x02800034
#define SIS3320_TRIGGER_SETUP_ADC4             0x02800038
#define SIS3320_TRIGGER_THRESHOLD_ADC4         0x0280003C
#define SIS3320_EVENT_DIRECTORY_ADC3		   0x02810000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC4		   0x02818000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_ADC_INPUT_MODE_ADC56           0x0300000C
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC5     0x03000010
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC6     0x03000014
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC56      0x03000020
#define SIS3320_DDR2_TEST_REGISTER_ADC56	   0x03000028
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC56	   0x0300002C
#define SIS3320_TRIGGER_SETUP_ADC5             0x03000030
#define SIS3320_TRIGGER_THRESHOLD_ADC5         0x03000034
#define SIS3320_TRIGGER_SETUP_ADC6             0x03000038
#define SIS3320_TRIGGER_THRESHOLD_ADC6         0x0300003C
#define SIS3320_EVENT_DIRECTORY_ADC5		   0x03010000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC6		   0x03018000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_ADC_INPUT_MODE_ADC78           0x0380000C
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC7     0x03800010
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC8     0x03800014
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC78      0x03800020
#define SIS3320_DDR2_TEST_REGISTER_ADC78	   0x03800028
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC78	   0x0380002C
#define SIS3320_TRIGGER_SETUP_ADC7             0x03800030
#define SIS3320_TRIGGER_THRESHOLD_ADC7         0x03800034
#define SIS3320_TRIGGER_SETUP_ADC8             0x03800038
#define SIS3320_TRIGGER_THRESHOLD_ADC8         0x0380003C
#define SIS3320_EVENT_DIRECTORY_ADC7		   0x03810000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC8		   0x03818000 /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_ADC1_OFFSET                    0x04000000
#define SIS3320_ADC2_OFFSET                    0x04800000
#define SIS3320_ADC3_OFFSET                    0x05000000
#define SIS3320_ADC4_OFFSET                    0x05800000
#define SIS3320_ADC5_OFFSET                    0x06000000
#define SIS3320_ADC6_OFFSET                    0x06800000
#define SIS3320_ADC7_OFFSET                    0x07000000
#define SIS3320_ADC8_OFFSET                    0x07800000

#define SIS3320_NEXT_ADC_OFFSET                0x00800000



/*******************/
/* bit definitions */

/* define sample clock */
#define SIS3320_ACQ_SET_CLOCK_TO_200MHZ        0x70000000 /* default after Reset */
#define SIS3320_ACQ_SET_CLOCK_TO_100MHZ        0x60001000
#define SIS3320_ACQ_SET_CLOCK_TO_50MHZ         0x50002000
#define SIS3320_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN 0x10006000
#define SIS3320_ACQ_SET_CLOCK_TO_P2_CLOCK_IN   0x00007000

#define SIS3320_ACQ_DISABLE_LEMO_START_STOP    0x01000000
#define SIS3320_ACQ_ENABLE_LEMO_START_STOP     0x00000100

#define SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER   0x00400000
#define SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER    0x00000040

#define SIS3320_ACQ_DISABLE_MULTIEVENT         0x00200000
#define SIS3320_ACQ_ENABLE_MULTIEVENT          0x00000020

#define SIS3320_ACQ_DISABLE_AUTOSTART          0x00100000
#define SIS3320_ACQ_ENABLE_AUTOSTART           0x00000010



/* bits of SIS3320_Event Configuration register ; D-register*/
#define SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP       0x20
#define SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE	   0x10

#define SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP		   0x0
#define SIS3320_EVENT_CONF_PAGE_SIZE_4M_WRAP		   0x1
#define SIS3320_EVENT_CONF_PAGE_SIZE_1M_WRAP		   0x2
#define SIS3320_EVENT_CONF_PAGE_SIZE_256K_WRAP		   0x3

#define SIS3320_EVENT_CONF_PAGE_SIZE_64K_WRAP		   0x4
#define SIS3320_EVENT_CONF_PAGE_SIZE_16K_WRAP		   0x5
#define SIS3320_EVENT_CONF_PAGE_SIZE_4K_WRAP		   0x6
#define SIS3320_EVENT_CONF_PAGE_SIZE_1K_WRAP		   0x7

#define SIS3320_EVENT_CONF_PAGE_SIZE_512_WRAP		   0x8
#define SIS3320_EVENT_CONF_PAGE_SIZE_256_WRAP		   0x9
#define SIS3320_EVENT_CONF_PAGE_SIZE_128_WRAP		   0xA
#define SIS3320_EVENT_CONF_PAGE_SIZE_64_WRAP		   0xB


/* Define Bit Masks */

#define SIS3320_INT_VEC_MASK            0x000000ff
#define SIS3320_INT_LEVEL_MASK          0x00000700
#define SIS3320_INT_ENABLE_MASK         0x00000800
#define SIS3320_INT_MODE_MASK           0x00001000
#define SIS3320_INT_SOURCE_ENABLE_MASK  0x0000000f

#define SIS3320_PAGESIZE_MASK     0x0000000f
const sis3320_pagesize[16] = {0x1000000, 0x400000, 0x100000, 0x40000,
			        0x10000,   0x4000,   0x1000,   0x400,
			          0x200,    0x100,     0x80,    0x40,
                                    0x0,      0x0,      0x0,     0x0};

#define SIS3320_CLOCKSOURCE_MASK  0x00007000
const int   sis3320_clocksource[] = {200000000, 100000000, 50000000, -1,
				     -1, -1, -1, -1};
const char *sis3320_clksrc_string[] = {"200 MHz", "100 MHz", "50 MHz", "Reserved",
				       "External with internal doubling", "Reserved", "External", "P2"};


#define SIS3320_DAC_CYCLE_BUSY            0x00008000
#define SIS3320_ADC_SAMPLING_BUSY         0x00020000
#define SIS3320_ADC_SAMPLING_ARMED        0x00010000

#define SIS3320_ADC_SAMPLE_LENGTH_MASK    0x01fffffc
#define SIS3320_ADC_SAMPLE_OFFSET_MASK    0x00000003
#define SIS3320_ADC_ADDR_OFFSET_MASK      0x01fffffc

#define SIS3320_ADC_HIDATA_MASK           0x7fff0000
#define SIS3320_ADC_LODATA_MASK           0x00007fff
#define SIS3320_ADC_BOTHDATA_MASK         0x0fff0fff
#define SIS3320_ADC_LOWORD_MASK           0x0000ffff

#define SIS3320_EDIR_END_ADDR_MASK        0x01ffffff
#define SIS3320_EDIR_WRAP                 0x10000000
#define SIS3320_EDIR_TRIG                 0x20000000

#define SIS3320_TDIR_END_ADDR_MASK        0x0001ffff
#define SIS3320_TDIR_WRAP                 0x00080000
#define SIS3320_TDIR_TMASK                0xff000000



/* function prototypes */

STATUS sis3320Init               (UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID);
int    sis3320Status             (UINT32 addr);
int    sis3320ReadEvent          (UINT32 addr, INT32 *lenbuf, INT32 *adcbuf);
int    sis3320ReadEventMultiMode (UINT32 addr, INT32 *lenbuf, INT32 *adcbuf);

void sis3320Reset  ();
void sis3320Arm    ();
void sis3320Disarm ();
void sis3320Start  ();
void sis3320Stop   ();

void sis3320DefaultSetup    ();
void sis3320SetCommonStart  ();
void sis3320SetCommonStop   ();
void sis3320SetGlobalWindow (int width, int delay);
void sis3320GetGlobalWindow ();


