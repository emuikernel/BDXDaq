/******************************************************************************
 *
 *  c1190Lib.h  -  Driver library header file for readout of a C.A.E.N. multiple
 *                 Model 775 TDCs using a VxWorks 5.2 or later based single board 
 *                 computer.
 *
 *  Author: David Abbott 
 *          Jefferson Lab Data Acquisition Group
 *          March 2002
 *
 * SVN: $Rev: 388 $
 *
 */


#define V1190_MAX_MODULES    20
#define V1190_MAX_CHANNELS   128
#define V1190_MAX_WORDS_PER_EVENT  34
#define V1190_MAX_WORDS_PER_BOARD  512/*3200*/ /* must be alligned to 16 !!! */

/* Define a Structure for access to TDC*/
typedef struct v1190_struct
{
  /*0x0000*/ volatile unsigned int   data[1024];
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
  /*0x101C*/ volatile unsigned int   evCount;
  /*0x1020*/ volatile unsigned short evStored;
  /*0x1022*/ volatile unsigned short almostFullLevel;
  /*0x1024*/ volatile unsigned short bltEventNumber;
  /*0x1026*/ volatile unsigned short firmwareRev;
  /*0x1028*/ volatile unsigned int   testReg;
  /*0x102C*/ volatile unsigned short outProgControl;
  /*0x102E*/ volatile unsigned short microReg;
  /*0x1030*/ volatile unsigned short microHandshake;

  /*0x1032*/ volatile unsigned short selflash;
  /*0x1034*/ volatile unsigned short flash;
  /*0x1036*/ volatile unsigned short compensationSRAMpage;

  /*0x1038*/ volatile unsigned int   fifo;
  /*0x103C*/ volatile unsigned short fifo_stored;
  /*0x103E*/ volatile unsigned short fifo_status;

} TDC1190;


struct v1190_ROM_struct {
  /*0x4000*/ volatile unsigned short checksum;
  unsigned short blank0;
  /*0x4004*/ volatile unsigned short checksum_length2;
  unsigned short blank1;
  /*0x4008*/ volatile unsigned short checksum_length1;
  unsigned short blank2;
  /*0x400C*/ volatile unsigned short checksum_length0;
  unsigned short blank3;
  /*0x4010*/ volatile unsigned short constant2;
  unsigned short blank4;
  /*0x4014*/ volatile unsigned short constant1;
  unsigned short blank5;
  /*0x4018*/ volatile unsigned short constant0;
  unsigned short blank6;
  /*0x401C*/ volatile unsigned short c_code;
  unsigned short blank7;
  /*0x4020*/ volatile unsigned short r_code;
  unsigned short blank8;
  /*0x4024*/ volatile unsigned short oui2;
  unsigned short blank9;
  /*0x4028*/ volatile unsigned short oui1;
  unsigned short blank10;
  /*0x402C*/ volatile unsigned short oui0;
  unsigned short blank11;
  /*0x4030*/ volatile unsigned short vers;
  unsigned short blank12;
  /*0x4034*/ volatile unsigned short board2;
  unsigned short blank13;
  /*0x4038*/ volatile unsigned short board1;
  unsigned short blank14;
  /*0x403C*/ volatile unsigned short board0;
  unsigned short blank15;
  /*0x4040*/ volatile unsigned short revis3;
  unsigned short blank16;
  /*0x4044*/ volatile unsigned short revis2;
  unsigned short blank17;
  /*0x4048*/ volatile unsigned short revis1;
  unsigned short blank18;
  /*0x404C*/ volatile unsigned short revis0;
  unsigned short blank19[13];
  /*0x4080*/ volatile unsigned short sernum1;
  unsigned short blank20;
  /*0x4084*/ volatile unsigned short sernum2;
};

#define V1190_BOARD_ID   0x000004A6
#define V1290_BOARD_ID   0x0000050A

#define V1190_FIRMWARE_REV   0x05 /* v1190core0.5.rbf or v1190core0.6.rbf */

/* address to be installed in A32 rotary switches,
   and to be used in CBLT */
#define V1190_A32_ADDR 0x08000000

/* Offset from V1190_A32_ADDR used for MCST */
#define V1190_A32_MCST_OFFSET 0x01000000

#define V1190_ROM_OFFSET    0x4000

/* Define default interrupt vector/level */
#define V1190_INT_VEC      0xbb
#define V1190_VME_INT_LEVEL   4

/* Status Register Bits */
#define V1190_STATUS_DATA_READY      0x1
#define V1190_STATUS_ALMOST_FULL     0x2
#define V1190_STATUS_FULL            0x4
#define V1190_STATUS_TRIG_MATCH      0x8
#define V1190_STATUS_HEADER_ENABLE  0x10
#define V1190_STATUS_TERM_ON        0x20
#define V1190_STATUS_ERROR_0        0x40
#define V1190_STATUS_ERROR_1        0x80
#define V1190_STATUS_ERROR_2       0x100
#define V1190_STATUS_ERROR_3       0x200
#define V1190_STATUS_BERR_FLAG     0x400
#define V1190_STATUS_PURG          0x800
#define V1190_STATUS_RES_0        0x1000
#define V1190_STATUS_RES_1        0x2000
#define V1190_STATUS_PAIR         0x4000
#define V1190_STATUS_TRIGGER_LOST 0x8000

/* Control Register Bits */
#define V1190_BUSERROR_ENABLE             0x1
#define V1190_TERMINATION                 0x2
#define V1190_TERMINATION_MODE            0x4
#define V1190_EMPTY_EVENT                 0x8
#define V1190_ALIGN64                    0x10
#define V1190_COMPENSATION_ENABLE        0x20 
#define V1190_TEST_FIFO_ENABLE           0x40
#define V1190_READ_COMP_SRAM_ENABLE      0x80
#define V1190_EVENT_FIFO_ENABLE         0x100
#define V1190_EXT_TRIG_TIME_TAG_ENABLE  0x200

/* FIXME: Dont need this definition??? */
#define V1190_TESTMODE                   0x2


#define V1190_ENABLE_ADER         0x1

#define V1190_DATA                0x00000000
#define V1190_GLOBAL_HEADER_DATA  0x40000000
#define V1190_GLOBAL_EOB_DATA     0x80000000
#define V1190_TDC_HEADER_DATA     0x08000000
#define V1190_TDC_EOB_DATA        0x18000000
#define V1190_TDC_ERROR_DATA      0x20000000
#define V1190_FILLER_DATA         0xc0000000


/* Register Masks */
#define V1190_DATA_TYPE_MASK  0xf8000000
#define V1190_TDC_MASK        0x06000000
#define V1190_CHANNEL_MASK    0x00f80000  /* Error in Manual */
#define V1190_EDGE_MASK       0x04000000
#define V1190_DATA_MASK       0x0007ffff

#define V1190_GHEAD_EVCOUNT_MASK     0x1fffffe0
#define V1190_GHEAD_GEOADDR_MASK     0x0000001f
#define V1190_GEOB_STATUS_MASK       0x07800000
#define V1190_GEOB_WORDCOUNT_MASK    0x000fffe0
#define V1190_GEOB_GEOADDR_MASK      0x0000001f
#define V1190_TDCHEAD_EVID_MASK      0x00fff000
#define V1190_TDCHEAD_BUNCHID_MASK   0x00000fff
#define V1190_TDCEOB_EVID_MASK       0x00fff000
#define V1190_TDCEOB_WORDCOUNT_MASK  0x00000fff


#define V1190_GEOADDR_MASK        0x001f
#define V1190_MCSTBASEADDR_MASK   0x00ff
#define V1190_INTLEVEL_MASK       0x0007
#define V1190_INTVECTOR_MASK      0x00ff
#define V1190_STATUS_MASK         0x03ff
#define V1190_CONTROL_MASK        0x000f
#define V1190_ADER32_MASK         0x00ff
#define V1190_ADER24_MASK         0x00ff
#define V1190_EVCOUNT_MASK        0xffff
#define V1190_BLTEVNUM_MASK       0x00ff
#define V1190_OUTPROG_MASK        0x0007

#define V1190_MICRO_WRITEOK       0x0001
#define V1190_MICRO_READOK        0x0002

#ifndef UINT32
#define UINT32 unsigned int
#endif
#ifndef INT32
#define INT32 int
#endif
#ifndef INT16
#define INT16 short
#endif
#ifndef UINT16
#define UINT16 unsigned short
#endif


/*sergey*/
#ifndef STATUS
#define STATUS int
#endif
#ifndef LOCAL
#define LOCAL static
#endif
#ifndef BOOL
#define BOOL int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif
/*
#ifndef VOIDFUNCPTR
typedef void (*VOIDFUNCPTR) ();
#endif
*/
/*sergey*/


/* Function Prototypes */
int    tdc1190Init(UINT32 addr, UINT32 addr_inc, int ntdc, int iFlag);
STATUS tdc1190InitList(UINT32 *addr_list, int ntdc, int iFlag);
STATUS tdc1190Status (int id);

int    tdc1190ReadMicro(int id, UINT16 *data, int nwords);
int    tdc1190WriteMicro(int id, UINT16 data);

int    tdc1190PrintEvent(int id, int pflag);
int    tdc1190StatusFull(int id);
int    tdc1190StatusAlmostFull(int id);

int    tdc1190Dready(int id);
int    tdc1190EventFifo(int id, UINT32 flag);
int    tdc1190ResetMCST(int id);
int    tdc1190BusError(int id, UINT32 flag);
int    tdc1190Align64(int id, UINT32 flag);

int    tdc1190ReadBlock(int id, volatile UINT32 *data, int nwrds, int rflag);
int    tdc1190ReadEvent(int id, UINT32 *tdata);
int    tdc1190ReadData(int id, UINT32 *tdata, int maxWords);

STATUS tdc1190SetTriggerMatchingMode(int id);
STATUS tdc1190SetContinuousStorageMode(int id);
int    tdc1190ReadAcquisitionMode(int id);
STATUS tdc1190SetKeepToken(int id);
STATUS tdc1190ClearKeepToken(int id);
STATUS tdc1190LoadDefaultConfiguration(int id);
STATUS tdc1190SaveUserConfiguration(int id);
STATUS tdc1190LoadUserConfiguration(int id);
STATUS tdc1190SetAutoLoadUserConfiguration(int id);
STATUS tdc1190SetAutoLoadDefaultConfiguration(int id);

STATUS tdc1190SetWindowWidth(int id, UINT32 window_width);
STATUS tdc1190SetWindowOffset(int id, INT32 window_offset);
STATUS tdc1190SetExtraSearchMargin(int id, UINT32 window_extra);
STATUS tdc1190SetRejectMargin(int id, UINT32 window_reject);
STATUS tdc1190EnableTriggerTimeSubtraction(int id);
STATUS tdc1190DisableTriggerTimeSubtraction(int id);
STATUS tdc1190ReadTriggerConfiguration(int id);

STATUS tdc1190SetEdgeDetectionConfig(int id, UINT16 setedge);
int    tdc1190ReadEdgeDetectionConfig(int id);
STATUS tdc1190SetEdgeResolution(int id, UINT16 edge_res);
int    tdc1190GetEdgeResolution(int id);
STATUS tdc1190SetPairResolution(int id, UINT32 leading_res, UINT32 pulse_width_res);
STATUS tdc1190GetPairResolution(int id, UINT32 *leading_res, UINT32 *pulse_width_res);
STATUS tdc1190SetDoubleHitResolution(int id, UINT16 double_hit_res);
int    tdc1190GetDoubleHitResolution(int id);

STATUS tdc1190EnableTDCHeaderAndTrailer(int id);
STATUS tdc1190DisableTDCHeaderAndTrailer(int id);
int    tdc1190GetTDCHeaderAndTrailer(int id);
STATUS tdc1190SetMaxNumberOfHitsPerEvent(int id, UINT32 nhits);
int    tdc1190GetMaxNumberOfHitsPerEvent(int id);
STATUS tdc1190EnableTDCErrorMark(int id);
STATUS tdc1190DisableTDCErrorMark(int id);
STATUS tdc1190EnableTDCErrorBypass(int id);
STATUS tdc1190DisableTDCErrorBypass(int id);
STATUS tdc1190SetTDCErrorType(int id, UINT32 mask);
int    tdc1190GetTDCErrorType(int id);
STATUS tdc1190SetEffectiveSizeOfReadoutFIFO(int id, UINT32 nwords);
int    tdc1190GetEffectiveSizeOfReadoutFIFO(int id);

STATUS tdc1190EnableChannel(int id, UINT16 channel);
STATUS tdc1190DisableChannel(int id, UINT16 channel);
STATUS tdc1190EnableAllChannels(int id);
STATUS tdc1190DisableAllChannels(int id);

/*sergey
STATUS tdc1190EnableChannels(int id, UINT16 channel_mask);
int    tdc1190GetChannels(int id);
*/
STATUS tdc1190EnableChannels(int id, UINT16 channels[8]);
STATUS tdc1190GetChannels(int id, UINT16 channels[8]);


int    tdc1190GetTDCError(int id, UINT16 ntdc);
int    tdc1190GetTDCDLLLock(int id, UINT16 ntdc);

STATUS tdc1190InitMCST(UINT32 *vmeaddress);

STATUS tdc1190Trig(int id);
STATUS tdc1190Clear(int id);
STATUS tdc1190Reset(int id);
int    tdc1190GetEventCounter(int id);
int    tdc1190GetEventStored(int id);
STATUS tdc1190SetAlmostFullLevel(int id, UINT16 nwords);
int    tdc1190GetAlmostFullLevel(int id);
STATUS tdc1190SetOutProg(int id, UINT16 code);
int    tdc1190GetOutProg(int id);

STATUS tdc1190TestMode(int id, int off);
STATUS tdc1190Test(int id, UINT16 tval);
STATUS tdc1190SetGeoAddress(int id, UINT16 geoaddr);
int    tdc1190GetGeoAddress(int id);
STATUS tdc1190SetBLTEventNumber(int id, UINT16 nevents);
int    tdc1190GetBLTEventNumber(int id);

/*LOCAL - rol does not like it !!??*/  void tdc1190Int (void);
STATUS tdc1190IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector);
STATUS tdc1190IntEnable (int id, UINT16 evCnt);
STATUS tdc1190IntDisable (int iflag);

int    v1190IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector);
int    v1190IntEnable (int id, UINT16 evCnt);
int    v1190IntDisable (int iflag);

/*sergey*/

STATUS tdc1190Config(int options);
int tdc1190ReadBoard(int itdc, UINT32 *tdata);
int tdc1190ReadBoardDmaStart(int ib, UINT32 *tdata);
int tdc1190ReadBoardDmaDone(int ib);
int tdc1190ReadStart(INT32 *tdcbuf, INT32 *rlenbuf);
int tdc1190ReadListStart(INT32 *tdcbuf, INT32 *rlenbuf);
int tdc1190ReadDone();
/*sergey*/

