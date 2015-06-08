/******************************************************************************
*
*  adc792.h  -  Driver library header file for readout of a C.A.E.N. multiple
*               Model 792 QDCs using a VxWorks 5.2 or later based single board 
*               computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          June 2000
*
* adc address: 0x00330000
* tdc address: 0x00220000
*/

#define UINT32 unsigned int
#define UINT16 unsigned short

/* Address Modifier */

#define AM24N      0x39    /* A24 non privileged data access */
#define AM24N_BLT  0x3B    /* A24 non privileged block transfer */
#define AM24N_MBLT 0x38    /* A24 non privileged 64 bit block transfer */

#define AM24S      0x3D    /* A24 supervisory data access */
#define AM24S_BLT  0x3F    /* A24 supervisory block transfer */
#define AM24S_MBLT 0x3C    /* A24 supervisory 64 bit block transfer */

#define AM24_GEO   0x2F    /* A24 Geographical access */

#define AM32N      0x09    /* A32 non privileged data access */
#define AM32N_BLT  0x0B    /* A32 non privileged block transfer */
#define AM32N_MBLT 0x08    /* A32 non privileged 64 bit block transfer */

#define AM32S      0x0D    /* A32 supervisory data access */
#define AM32S_BLT  0x0F    /* A32 supervisory block transfer */
#define AM32S_MBLT 0x0C    /* A32 supervisory 64 bit block transfer */

/* ROM memory offset */

#define ROM_OFFSET 0x8026

/* QDC Address Map */

typedef struct adc792_struct
{
/*0x0000*/  volatile unsigned long  data[512];  /* Output Buffer */
/*0x0000*/  unsigned long blank1[512];
/*0x1000*/  volatile unsigned short rev;        /* Firmware Revision */
/*0x1002*/  volatile unsigned short geoAddr;    /* Geo Address */
/*0x1004*/  volatile unsigned short cbltAddr;   /* MCST/CBLT Address */
/*0x1006*/  volatile unsigned short bitSet1;    /* Bit Set 1 */
/*0x1008*/  volatile unsigned short bitClear1;  /* Bit Clear 1 */
/*0x100A*/  volatile unsigned short intLevel;   /* Interrupt Level */
/*0x100C*/  volatile unsigned short intVector;  /* Interrupt Vector */
/*0x100E*/  volatile unsigned short status1;
/*0x1010*/  volatile unsigned short control1;
/*0x1012*/  volatile unsigned short aderHigh;
/*0x1014*/  volatile unsigned short aderLow;
/*0x1016*/  volatile unsigned short ssReset;
/*0x1018*/  unsigned short blank2;
/*0x101A*/  volatile unsigned short cbltControl;
/*0x101C*/  unsigned short blank3[2];
/*0x1020*/  volatile unsigned short evTrigger;
/*0x1022*/  volatile unsigned short status2;
/*0x1000*/  volatile unsigned short evCountL;
/*0x1000*/  volatile unsigned short evCountH;
/*0x1000*/  volatile unsigned short incEvent;
/*0x1000*/  volatile unsigned short incOffset;
/*0x1000*/  volatile unsigned short loadTest;
/*0x1000*/  volatile unsigned short fclrWindow;
/*0x1000*/  volatile unsigned short bitSet2;
/*0x1000*/  volatile unsigned short bitClear2;
/*0x1000*/  volatile unsigned short wMemTestAddr;
/*0x1000*/  volatile unsigned short memTestWordH;
/*0x1000*/  volatile unsigned short memTestWordL;
/*0x1000*/  volatile unsigned short crateSelect;
/*0x1000*/  volatile unsigned short testEvWrite;
/*0x1000*/  volatile unsigned short evCountReset;
/*0x1000*/  unsigned short blank4[15];
/*0x1000*/  volatile unsigned short iped;
/*0x1000*/  unsigned short blank5;
/*0x1000*/  volatile unsigned short rTestAddr;
/*0x1000*/  unsigned short blank6;
/*0x1000*/  volatile unsigned short swComm;
/*0x1000*/  unsigned short blank7[3];
/*0x1000*/  volatile unsigned short AAD;
/*0x1000*/  volatile unsigned short BAD;
/*0x1000*/  unsigned short blank8[6];
/*0x1000*/  volatile unsigned short threshold[32];
} ADC792;

typedef struct adc792rom_struct {
  volatile unsigned short OUI_3;
  unsigned short blank1;
  volatile unsigned short OUI_2;
  unsigned short blank2;
  volatile unsigned short OUI_1;
  unsigned short blank3;
  volatile unsigned short version;
  unsigned short blank4;
  volatile unsigned short ID_3;
  unsigned short blank5;
  volatile unsigned short ID_2;
  unsigned short blank6;
  volatile unsigned short ID_1;
  unsigned short blank7[7];
  volatile unsigned short revision;
  unsigned short blank8[1881];
  volatile unsigned short NUM_2;
  unsigned short blank9;
  volatile unsigned short NUM_1;
} ADC792ROM;


/* Define board ID */
#define C792_BOARD_ID   775

/* Define default interrupt vector/level */
#define C792_INT_VEC      0xaa
#define C792_VME_INT_LEVEL   4



/***********************/
/* low level functions */
/***********************/

/* Logic control */

#define BERR_FLAG     0x0008
#define SEL_ADDR      0x0010
#define SOFT_RESET    0x0080
#define BLKEND        0x0400 /* shifted byte ! */
#define PROG_RESET    0x1000 /* shifted byte ! */
#define BERR_ENABLE   0x2000 /* shifted byte ! */
int adc792setLogic  (UINT32 addr, UINT16 value);
int adc792getLogic  (UINT32 addr, UINT16 *value);
int adc792resetLogic(UINT32 addr, UINT16 value);

/* Geting status */

#define DREADY        0x0001
#define GLOBAL_DREADY 0x0002
#define BUSY          0x0004
#define GLOBAL_BUSY   0x0008
#define AMNESIA       0x0010
#define PURGED        0x0020
#define TERM_ON       0x0040
#define TERM_OFF      0x0080
#define EVRDY         0x0100
int adc792getStatus      (UINT32 addr, UINT16 *value);

#define BUFFER_EMPTY  0x0002
#define BUFFER_FULL   0x0004
int adc792getBufferStatus(UINT32 addr, UINT16 *value);
int adc792getBoardType   (UINT32 addr, UINT16 *value);


/* Control operations */

#define MEMORY_TEST   0x0001
#define OFFLINE       0x0002
#define CLEAR_DATA    0x0004
#define OVER_RANGE    0x0008
#define ZERO_SUPPRESS 0x0010
#define VALID_CONTROL 0x0020 /* TDC v775 only */
#define DAQ_TEST      0x0040
#define SLIDING_SCALE 0x0080
#define START_STOP    0x0400 /* TDC v775 only */
#define AUTOINCREMENT 0x0800
#define HEAD_SUPPRESS 0x1000
#define SLIDING_MODE  0x2000
#define ALL_TRIGGERS  0x4000


int adc792setOperationMode  (UINT32 addr, UINT16 value);
int adc792resetOperationMode(UINT32 addr, UINT16 value);
int adc792getOperationMode  (UINT32 addr, UINT16 *value);

/* Board addressing */

int adc792setVMEAddress(UINT32 addr, UINT32 value);
int adc792getVMEAddress(UINT32 addr, UINT32 *value);

int adc792getGEOAddress(UINT32 addr, UINT16 *value);

/* Interrupt operations */

int adc792setInterruptLevel(UINT32 addr, UINT16 value);
int adc792getInterruptLevel(UINT32 addr, UINT16 *value);
int adc792setInterruptVector(UINT32 addr, UINT16 value);
int adc792getInterruptVector(UINT32 addr, UINT16 *value);

int adc792setInterruptEventNumber(UINT32 addr, UINT16 value);
int adc792getInterruptEventNumber(UINT32 addr, UINT16 *value);

/* Output Buffer operations */

int adc792incrementWord32(UINT32 addr);
int adc792incrementEvent (UINT32 addr);

int adc792readWord (UINT32 addr, UINT32 *value);
int adc792readWord1(UINT32 addr, UINT32 i, UINT32 *value);
/*int adc792readLong(UINT32 addr, UINT64 *value);*/
int adc792readEvent(UINT32 addr, UINT32 array[32], UINT32 *nwords, UINT32 *nevent);

/* Fast Clear Window */

int adc792setFastClearWindow(UINT32 addr, UINT16 value);
int adc792getFastClearWindow(UINT32 addr, UINT16 *value);

/* define crate number */

int adc792setCrateNumber(UINT32 addr, UINT16 value);
int adc792getCrateNumber(UINT32 addr, UINT16 *value);

/* DAQ test */

int adc792writeTestEvent(UINT32 addr, UINT16 array[32]);

/* Event Counter operations */

int adc792resetEventCounter(UINT32 addr);
int adc792readEventCounter(UINT32 addr, UINT32 *value);

/* Pedestal operations (ADC only) */

int adc792setPedestal(UINT32 addr, UINT16 value);
int adc792getPedestal(UINT32 addr, UINT16 *value);

/* Scale Range operations (TDC only) */

int tdc775setResolution(UINT32 addr, UINT16 value);
int tdc775getResolution(UINT32 addr, UINT16 *value);

/* memory test */

int adc792setMemoryReadPointer (UINT32 addr, UINT16 value);
int adc792setMemoryWritePointer(UINT32 addr, UINT16 value);
int adc792writeMemoryWord32    (UINT32 addr, UINT32 value);

/* conversion for the test purposes */

int adc792startConversion(UINT32 addr);

/* sliding constant */

int adc792setSlideConstant(UINT32 addr, UINT16 value);
int adc792getSlideConstant(UINT32 addr, UINT16 *value);

/* access to the value converted by ADC of the block A and B */

int adc792getBlockA(UINT32 addr, UINT16 *value);
int adc792getBlockB(UINT32 addr, UINT16 *value);

/* Threshold operations */

int adc792setThreshold (UINT32 addr, UINT32 channel, UINT16 value);
int adc792getThreshold (UINT32 addr, UINT32 channel, UINT16 *value);
int adc792setThresholds(UINT32 addr, UINT16 *array);
int adc792getThresholds(UINT32 addr, UINT16 *array);

/* Firmware and ROM access */

int adc792getFirmwareRevision(UINT32 addr, UINT32 *value);

int adc792getOUI         (UINT32 addr, UINT32 *value);
int adc792getVersion     (UINT32 addr, UINT32 *value);
int adc792getBoardID     (UINT32 addr, UINT32 *value);
int adc792getRevision    (UINT32 addr, UINT32 *value);
int adc792getSerialNumber(UINT32 addr, UINT32 *value);


/************************/
/* high level functions */
/************************/

int adc792printROM   (UINT32 addr);
int adc792boardInit  (UINT32 addr);
int adc792printStatus(UINT32 addr);


 












