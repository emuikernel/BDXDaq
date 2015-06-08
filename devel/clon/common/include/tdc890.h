/******************************************************************************
*
*  tdc890.h  -  Driver library header file for readout of a C.A.E.N. multiple
*               Model 775 TDCs using a VxWorks 5.2 or later based single board 
*               computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          March 2002
*/


#define V890_MAX_CHANNELS   128
#define V890_MAX_WORDS_PER_EVENT  34
#define V890_MAX_WORDS_PER_BOARD  512/*3200*/ /* must be alligned to 16 !!! */

/* Define a Structure for access to TDC*/
typedef struct v890_struct
{
  /*0x0000*/ volatile unsigned int  data[1024];
  /*0x1000*/ volatile unsigned short control;
  /*0x1002*/ volatile unsigned short status;
  /*0x1004*/ volatile unsigned short res1;
  /*0x1006*/ volatile unsigned short res2;
  /*0x1008*/ volatile unsigned short res3;
  /*0x100A*/ volatile unsigned short intLevel;
  /*0x100C*/ volatile unsigned short intVector;
  /*0x100E*/ volatile unsigned short geoAddr;
  /*0x1010*/ volatile unsigned short mcstBaseAddr;
  /*0x1012*/ volatile unsigned short mcstCtrl;
  /*0x1014*/ volatile unsigned short moduleReset;
  /*0x1016*/ volatile unsigned short clear;
  /*0x1018*/ volatile unsigned short countReset;
  /*0x101A*/ volatile unsigned short trigger;
  /*0x101C*/ volatile unsigned int  evCount;
  /*0x1020*/ volatile unsigned short evStored;
  /*0x1022*/ volatile unsigned short almostFullLevel;
  /*0x1024*/ volatile unsigned short bltEventNumber;
  /*0x1026*/ volatile unsigned short firmwareRev;
  /*0x1028*/ volatile unsigned int  testReg;
  /*0x102C*/ volatile unsigned short outProgControl;
  /*0x102E*/ volatile unsigned short microReg;
  /*0x1030*/ volatile unsigned short microHandshake;

  /*0x1032*/ volatile unsigned short selflash;
  /*0x1034*/ volatile unsigned short flash;
  /*0x1036*/ volatile unsigned short compensationSRAMpage;

  /*0x1038*/ volatile unsigned int  fifo;
  /*0x103C*/ volatile unsigned short fifo_stored;
  /*0x103E*/ volatile unsigned short fifo_status;

} TDC890;
/* old structure (see v890 manual)
typedef struct v890_struct
{
  volatile unsigned long  data[1024];
  volatile unsigned short control;
  volatile unsigned short status;
  volatile unsigned short ader32;
  volatile unsigned short ader24;
  volatile unsigned short enableAder;
  volatile unsigned short intLevel;
  volatile unsigned short intVector;
  volatile unsigned short geoAddr;
  volatile unsigned short mcstBaseAddr;
  volatile unsigned short mcstCtrl;
  volatile unsigned short moduleReset;
  volatile unsigned short countReset;
  volatile unsigned short clear;
  volatile unsigned short trigger;
  unsigned short blank;
  volatile unsigned short evCount;
  volatile unsigned short evStored;
  volatile unsigned short almostFullLevel;
  volatile unsigned short bltEventNumber;
  volatile unsigned short firmwareRev;
  volatile unsigned long  testReg;
  volatile unsigned short outProgControl;
  volatile unsigned short microReg;
  volatile unsigned short microHandshake;
  unsigned short blank1[103];
  volatile unsigned short TDC0;
  unsigned short blank2;
  volatile unsigned short TDC1;
  unsigned short blank3;
  volatile unsigned short TDC2;
  unsigned short blank4;
  volatile unsigned short TDC3;
  unsigned short blank5;
  unsigned short blank6[120];
  volatile unsigned long  dummy32;
  volatile unsigned short dummy16;
} TDC890;*/

#define V890_BOARD_ID   0x05 /* v1190core0.5.rbf or v1190core0.6.rbf */

/* address to be installed in A32 rotary switches,
   and to be used in CBLT */
#define V890_A32_ADDR 0x08000000

/* Define default interrupt vector/level */
#define V890_INT_VEC      0xbb
#define V890_VME_INT_LEVEL   4

/* Register Bits */
#define V890_STATUS_DATA_READY      0x1
#define V890_STATUS_ALMOST_FULL     0x2
#define V890_STATUS_FULL            0x4
#define V890_STATUS_TRIG_MATCH      0x8
#define V890_STATUS_HEADER_ENABLE  0x10
#define V890_STATUS_TERM_ON        0x20
#define V890_STATUS_ERROR_0        0x40
#define V890_STATUS_ERROR_1        0x80
#define V890_STATUS_ERROR_2       0x100
#define V890_STATUS_ERROR_3       0x200

#define V890_BUSERROR_ENABLE     0x1
#define V890_TESTMODE            0x2
#define V890_TERMINATION         0x4
#define V890_TERMINATION_MODE    0x8

#define V890_ENABLE_ADER         0x1

#define V890_DATA                0x00000000
#define V890_GLOBAL_HEADER_DATA  0x40000000
#define V890_GLOBAL_EOB_DATA     0x80000000
#define V890_TDC_HEADER_DATA     0x08000000
#define V890_TDC_EOB_DATA        0x18000000
#define V890_TDC_ERROR_DATA      0x20000000
#define V890_FILLER_DATA         0xc0000000


/* Register Masks */
#define V890_DATA_TYPE_MASK  0xf8000000
#define V890_TDC_MASK        0x06000000
#define V890_CHANNEL_MASK    0x00f80000  /* Error in Manual */
#define V890_EDGE_MASK       0x04000000
#define V890_DATA_MASK       0x0007ffff

#define V890_GHEAD_EVCOUNT_MASK     0x1fffffe0
#define V890_GHEAD_GEOADDR_MASK     0x0000001f
#define V890_GEOB_STATUS_MASK       0x07800000
#define V890_GEOB_WORDCOUNT_MASK    0x000fffe0
#define V890_GEOB_GEOADDR_MASK      0x0000001f
#define V890_TDCHEAD_EVID_MASK      0x00fff000
#define V890_TDCHEAD_BUNCHID_MASK   0x00000fff
#define V890_TDCEOB_EVID_MASK       0x00fff000
#define V890_TDCEOB_WORDCOUNT_MASK  0x00000fff


#define V890_GEOADDR_MASK        0x001f
#define V890_MCSTBASEADDR_MASK   0x00ff
#define V890_INTLEVEL_MASK       0x0007
#define V890_INTVECTOR_MASK      0x00ff
#define V890_STATUS_MASK         0x03ff
#define V890_CONTROL_MASK        0x000f
#define V890_ADER32_MASK         0x00ff
#define V890_ADER24_MASK         0x00ff
#define V890_EVCOUNT_MASK        0xffff
#define V890_BLTEVNUM_MASK       0x00ff
#define V890_OUTPROG_MASK        0x0007

#define V890_MICRO_WRITEOK       0x0001
#define V890_MICRO_READOK        0x0002


#define UINT32 unsigned int
#define INT32 int
#define UINT16 unsigned short

/* Function Prototypes */
int    tdc890Init (UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID);
void   tdc890Status (UINT32 addr);

int    v890IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector);
int    v890IntEnable (int id, UINT16 evCnt);
int    v890IntDisable (int iflag);

int    tdc890ReadMicro(UINT32 addr, UINT16 *data, int nwords);
int    tdc890ReadMicroGroup(int nboards, UINT32 *addr, UINT16 *data[21],
                            int nwords);
int    tdc890WriteMicro(UINT32 addr, UINT16 data);
int    tdc890WriteMicroGroup(int nboards, UINT32 *addr, UINT16 data);

int    tdc890ReadData(UINT32 addr, UINT32 *tdata, int maxWords);
int    tdc890GetResolution(UINT32 addr, UINT16 *data);
int    tdc890Dready(UINT32 addr);

void   tdc890TriggerMatch(UINT32 addr);
void   tdc890Trig(UINT32 addr);
void   tdc890Enable(UINT32 addr);
void   tdc890Disable(UINT32 addr);
void   tdc890Clear(UINT32 addr);
void   tdc890Reset(UINT32 addr);



/* CLAS specific options */

#define CLAS_A24_D32_BERR   0x00010100 /* A24 single word readout with VMEBUS error */
#define CLAS_A24_D32_FIFO   0x00010101 /* A24 single word readout with event fifo */

#define CLAS_A24_BLT_BERR   0x00010200 /* A24 D32 DMA readout with VMEBUS error */
#define CLAS_A24_BLT_FIFO   0x00010201 /* A24 D32 DMA readout with event fifo */

#define CLAS_A24_MBLT_BERR  0x00010300 /* A24 D64 DMA readout with VMEBUS error */
#define CLAS_A24_MBLT_FIFO  0x00010301 /* A24 D64 DMA readout with event fifo */


#define CLAS_A32_BLT        0x00020200 /* A32 D32 DMA readout with VMEBUS error */
#define CLAS_A32_BLT_FIFO   0x00020201 /* A32 D32 DMA readout with event fifo */

#define CLAS_A32_MBLT       0x00020300 /* A32 D64 DMA readout with VMEBUS error */
#define CLAS_A32_MBLT_FIFO  0x00020301 /* A32 D64 DMA readout with event fifo */

#define CLAS_A32_2eVME      0x00020400 /* A32 D64 2eVME DMA readout with VMEBUS error */
#define CLAS_A32_2eVME_FIFO 0x00020401 /* A32 D64 2eVME DMA readout with event fifo */

#define CLAS_A32_2eSST      0x00020500 /* A32 D64 2eSST DMA readout with VMEBUS error */
#define CLAS_A32_2eSST_FIFO 0x00020501 /* A32 D64 2eSST DMA readout with event fifo */


#define CLAS_A32_MBLT_CBLT  0x01020300 /* A32 D64 DMA chain-readout with VMEBUS error (former CLAS_A32_MBLT_BERR) */



/*NOTE: for A32 we do not reading fifo, so after 1023 events - 'not ready ..' */
/* this is why A32..FIFO commented out: do not use it ! */

/* CLAS specific functions */

void tdc1190ScanCLAS(int options, int *nboards, unsigned int *tdcadr);
void tdc1190SetGlobalWindow(int width, int offset);
void tdc1190InitCLAS(int nboards, unsigned int *tdcadr,
                     unsigned int *tdcbaseadr);
int tdc1190ReadEventCLAS(int nboards, UINT32 *tdcadr, INT32 *tdcbuf,
                         INT32 *rlenbuf);







 


