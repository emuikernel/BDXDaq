/******************************************************************************
*
*  fadcLib.h  - Driver library header file for readout of the JLAB 250MHz FLASH
*                ADC using a VxWorks 5.5 or later based single board 
*                computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          June 2007
*
*  Revision  1.0 - Initial Revision
*                    - Supports up to 20 FADC boards in a Crate
*                    - Programmed I/O and Block reads
*
*  Revision  1.1 - Updated for new firmware (0x0111 or later)
*                  Supports FADC Signal Distribution Module
*
*  Revision  1.2 - Updated to support Internal (HITSUM) triggering
*                  and Moller firmware revisions of the FADC
*
*  Revision  2.0 - Updated to support FADC V2.
*
*  SVN: $Rev: 517 $
*
*/

#ifndef __FADCLIB__
#define __FADCLIB__

#define FA_BOARD_ID       0xfadc0000
#define FA_MAX_BOARDS             20
#define FA_MAX_ADC_CHANNELS       16
#define FA_MAX_DATA_PER_CHANNEL  251
#define FA_MAX_A32_MEM      0x800000   /* 8 Meg */
#define FA_MAX_A32MB_SIZE   0x800000  /*  8 MB */
#define FA_VME_INT_LEVEL           3     
#define FA_VME_INT_VEC          0xFA


struct fadc_struct {
  volatile unsigned int version;		/* (0x000 - 0x0FC) - CTRL */
  volatile unsigned int csr;
  volatile unsigned int ctrl1;
  volatile unsigned int ctrl2;
  volatile unsigned int blk_level;              /* 0x10 */
  volatile unsigned int intr;
  volatile unsigned int adr32;
  volatile unsigned int adr_mb;
  volatile unsigned int s_adr;                  /* 0x20 */
  volatile unsigned int delay;
  volatile unsigned int itrig_cfg;
  volatile unsigned int reset;
  volatile unsigned int trig_scal;              /* 0x30 */
  volatile unsigned int ev_count;
  volatile unsigned int blk_count;
  volatile unsigned int blk_fifo_count;
  volatile unsigned int blk_wrdcnt_fifo;        /* 0x40 */
  volatile unsigned int internal_trig_scal;
  volatile unsigned int ram_word_count;
  volatile unsigned int dataflow_status;
  volatile unsigned short dac[16];              /* 0x50 */
  volatile unsigned int status[4];              /* 0x70 */
  volatile unsigned int aux[3];                 /* 0x80 */
  volatile unsigned int mem_adr;
  volatile unsigned int mem1_data;		/* 0x090 */
  volatile unsigned int mem2_data;
  volatile unsigned int prom_reg1;		
  volatile unsigned int prom_reg2;
  volatile unsigned int berr_module_scal;       /* 0xA0 */
  volatile unsigned int berr_crate_scal;
  volatile unsigned int aux_scal[6];
  volatile unsigned int busy_level;             /* 0xC0 */
  volatile unsigned int gen_evt_header;
  volatile unsigned int gen_evt_data;
  volatile unsigned int gen_evt_trailer;
  volatile unsigned int mgt_status;             /* 0xD0 */
  volatile unsigned int mgt_ctrl;
  volatile unsigned int reserved_ctrl[2];
  volatile unsigned int scaler_ctrl;
  volatile unsigned int spare_ctrl[(0x100-0xE4)>>2];
  
  volatile unsigned int adc_status[3];          /* (0x100 - 0x1FC) - ADC processing*/
  volatile unsigned int adc_config[4];
  volatile unsigned int adc_ptw;
  volatile unsigned int adc_pl;                 /* 0x120 */
  volatile unsigned int adc_nsb;
  volatile unsigned int adc_nsa;
  volatile unsigned short adc_thres[16];
  volatile unsigned int ptw_last_adr;
  volatile unsigned int ptw_max_buf;            /* 0x150 */
  volatile unsigned int adc_test_data;
  volatile unsigned int adc_pedestal[16];
#ifdef HPS
  volatile unsigned int config6;
  volatile unsigned int config7;
#endif
  volatile unsigned int spare_adc[(0x200-0x198)>>2];

  volatile unsigned int hitsum_status;	        /* (0x200 - 0x2FC) - HITSUM processing */
  volatile unsigned int hitsum_cfg;
  volatile unsigned int hitsum_hit_width;	/*  secondary address (0x0 - 0xf) */
  volatile unsigned int hitsum_trig_delay;
  volatile unsigned int hitsum_trig_width;      /* 0x210 */
  volatile unsigned int hitsum_window_bits;
  volatile unsigned int hitsum_window_width;
  volatile unsigned int hitsum_coin_bits;       
  volatile unsigned int hitsum_pattern;	        /* 0x220 */
  volatile unsigned int hitsum_fifo;
  volatile unsigned int hitsum_sum_thresh;
  volatile unsigned int spare_hitsum[(0x300-0x22C)>>2];

  volatile unsigned int scaler[16];             /* (0x300 - 0x3FC) - Scalers */
  volatile unsigned int time_count;             /* 0x340 */
  volatile unsigned int spare_scaler[(0x400-0x344)>>2];
};

struct fadc_data_struct {
  unsigned int new_type;	
  unsigned int type;	
  unsigned int slot_id_hd;
  unsigned int slot_id_tr;
  unsigned int n_evts;
  unsigned int blk_num;
  unsigned int n_words;
  unsigned int evt_num_1;
  unsigned int evt_num_2;
  unsigned int time_now;
  unsigned int time_1;
  unsigned int time_2;
  unsigned int time_3;
  unsigned int time_4;
  unsigned int chan;
  unsigned int width;
  unsigned int valid_1;
  unsigned int adc_1;
  unsigned int valid_2;
  unsigned int adc_2;
  unsigned int over;
  unsigned int adc_sum;
  unsigned int pulse_num;
  unsigned int thres_bin;
  unsigned int quality;
  unsigned int integral;
  unsigned int time;
  unsigned int chan_a;
  unsigned int source_a;
  unsigned int chan_b;
  unsigned int source_b;
  unsigned int group;
  unsigned int time_coarse;
  unsigned int time_fine;
  unsigned int vmin;
  unsigned int vpeak;
  unsigned int trig_type_int;	/* next 4 for internal trigger data */
  unsigned int trig_state_int;	/* e.g. helicity */
  unsigned int evt_num_int;
  unsigned int err_status_int;
};


struct fadc_sdc_struct {
  volatile unsigned short csr;
  volatile unsigned short ctrl;
  volatile unsigned short busy_enable;
  volatile unsigned short busy_status;
};


/* FADC Special Board IDs */

/* Define CSR Bits */
#define  FA_CSR_EVENT_AVAILABLE           0x1
#define  FA_CSR_BLOCK_LEVEL_FLAG          0x2
#define  FA_CSR_BLOCK_READY               0x4
#define  FA_CSR_BERR_STATUS               0x8
#define  FA_CSR_TOKEN_STATUS             0x10
#define  FA_CSR_FIFO1_EMPTY             0x800
#define  FA_CSR_FIFO1_ALMOST_EMPTY     0x1000
#define  FA_CSR_FIFO1_HALF_FULL        0x2000
#define  FA_CSR_FIFO1_ALMOST_FULL      0x4000
#define  FA_CSR_FIFO1_FULL             0x8000
#define  FA_CSR_ERROR_MASK         0x0c000000
#define  FA_CSR_ERROR_CLEAR        0x08000000
#define  FA_CSR_SYNC               0x10000000
#define  FA_CSR_TRIGGER            0x20000000
#define  FA_CSR_SOFT_RESET         0x40000000
#define  FA_CSR_HARD_RESET         0x80000000

/* Define Init Flag bits for Clock Source */
#define FA_SOURCE_INT         0x00
#define FA_SOURCE_SDC         0x10
#define FA_SOURCE_VXS         0x20
#define FA_SOURCE_MASK        0x30

/* Define Control2 Bits */
#define FA_CTRL_GO               0x1
#define FA_CTRL_ENABLE_TRIG      0x2
#define FA_CTRL_ENABLE_SRESET    0x4
#define FA_CTRL_ENABLE_INT_TRIG  0x8
#define FA_CTRL_ENABLE_MASK      0x7
#define FA_CTRL_ENABLED          0x7

/* Define CTRL1 Bits */
#define FA_REF_CLK_INTERNAL        0x0
#define FA_REF_CLK_FP              0x1
#define FA_REF_CLK_P0              0x2
#define FA_REF_CLK_MASK            0x3
#define FA_ENABLE_INTERNAL_CLK     0x8

#define FA_TRIG_FP                0x00
#define FA_TRIG_FP_ISYNC          0x10
#define FA_TRIG_P0                0x20
#define FA_TRIG_P0_ISYNC          0x30
#define FA_TRIG_VME               0x60
#define FA_TRIG_INTERNAL          0x70
#define FA_TRIG_MASK              0x70
#define FA_ENABLE_SOFT_TRIG       0x80

#define FA_SRESET_FP             0x000
#define FA_SRESET_FP_ISYNC       0x100
#define FA_SRESET_P0             0x200
#define FA_SRESET_P0_ISYNC       0x300
#define FA_SRESET_VME            0x600
#define FA_SRESET_NONE           0x700
#define FA_SRESET_MASK           0x700
#define FA_ENABLE_SOFT_SRESET    0x800

#define FA_ENABLE_LIVE_TRIG_OUT  0x1000
#define FA_ENABLE_TRIG_OUT_FP    0x2000
#define FA_ENABLE_TRIG_OUT_P0    0x4000
#define FA_TRIGOUT_MASK          0x7000

#define FA_ENABLE_BLKLVL_INT      0x40000
#define FA_ENABLE_BERR           0x100000
#define FA_ENABLE_MULTIBLOCK     0x200000
#define FA_FIRST_BOARD           0x400000
#define FA_LAST_BOARD            0x800000
#define FA_ENABLE_DEBUG         0x2000000
#define FA_MB_TOKEN_VIA_P0     0x10000000
#define FA_MB_TOKEN_VIA_P2     0x20000000

/* Define Control 2 Bits */
#define FA_GO                         0x1
#define FA_ENABLE_EXT_TRIGGER         0x2
#define FA_ENABLE_EXT_SRESET          0x4
#define FA_ENABLE_INT_TRIGGER         0x8
#define FA_ENABLE_STREAM_MODE        0x10

#define FA_DEBUG_XFER_INFIFO_BFIFO       0x1000        
#define FA_DEBUG_XFER_BFIFO_OFIFO        0x2000        

/* Define Reset Bits */
#define FA_RESET_CNTL_FPGA              0x1
#define FA_RESET_ADC_FPGA1              0x2
#define FA_RESET_ADC_FIFO1            0x100
#define FA_RESET_DAC                  0x800
#define FA_RESET_EXT_RAM_PT          0x1000
#define FA_RESET_TOKEN              0x10000
#define FA_RESET_ALL                 0xFFFF

/* Define trig_scal bits */
#define FA_TRIG_SCAL_RESET          (1<<31)

/* Define MGT Control bits */
#define FA_MGT_RESET                    0x0
#define FA_RELEASE_MGT_RESET            0x1

#define FA_INT_ACTIVE             0x1000000
#define FA_INT_IACKSTAT_WAIT      0x2000000
#define FA_INT_IACKSTAT_ONBT      0x4000000


/* Define RAM Bits */
#define FA_RAM_DATA_MASK       0x000fffff
#define FA_RAM_FULL            0x00100000
#define FA_RAM_EMPTY           0x00200000
#define FA_RAM_ADR_INCREMENT   0x00100000

/* Define Bit Masks */
#define FA_VERSION_MASK        0x000000ff
#define FA_BOARD_MASK          0xffff0000
#define FA_CSR_MASK            0x0fffffff
#define FA_CONTROL_MASK        0xffffffff
#define FA_CONTROL2_MASK       0xc000303f
#define FA_EVENT_COUNT_MASK    0xffffff
#define FA_BLOCK_COUNT_MASK    0xfffff
#define FA_BLOCK_LEVEL_MASK    0xffff
#define FA_INT_ENABLE_MASK     0xc0000
#define FA_INT_VEC_MASK        0xff
#define FA_INT_LEVEL_MASK      0x700
#define FA_SLOT_ID_MASK        0x1f0000
#define FA_DAC_VALUE_MASK      0x0fff

#define FA_REF_CLK_SEL_MASK        0x7
#define FA_TRIG_SEL_MASK          0x70
#define FA_SRESET_SEL_MASK       0x700

#define FA_BUSY_LEVEL_MASK     0xfffff
#define FA_FORCE_BUSY       0x80000000

#define FA_A32_ENABLE        0x1
#define FA_AMB_ENABLE        0x1
#define FA_A32_ADDR_MASK     0xff80   /* 8 MB chunks */
#define FA_AMB_MIN_MASK      0xff80
#define FA_AMB_MAX_MASK      0xff800000

#define FA_SADR_AUTO_INCREMENT   0x10000
#define FA_PPG_WRITE_VALUE       0x8000
#define FA_PPG_SAMPLE_MASK       0x1fff
#define FA_PPG_MAX_SAMPLES       512
#define FA_PPG_ENABLE            0x80

/* Define MGT Status Bits/Masks */
#define FA_MGT_GTX1_LANE1_UP      0x1
#define FA_MGT_GTX1_LANE2_UP      0x2
#define FA_MGT_GTX1_CHANNEL_UP    0x4
#define FA_MGT_GTX1_HARD_ERROR    0x8
#define FA_MGT_GTX1_SOFT_ERROR    0x10
#define FA_MGT_GTX2_LANE1_UP      0x20
#define FA_MGT_GTX2_LANE2_UP      0x40
#define FA_MGT_GTX2_CHANNEL_UP    0x80
#define FA_MGT_GTX2_HARD_ERROR    0x100
#define FA_MGT_GTX2_SOFT_ERROR    0x200
#define FA_MGT_SUM_DATA_VALID     0x400
#define FA_MGT_RESET_ASSERTED     0x800


/* Define default ADC Processing prarmeters 
   values are in clock cycles 4ns/cycle */

#define FA_ADC_NS_PER_CLK      4

#define FA_ADC_PROC_MASK        0x7
#define FA_ADC_PROC_ENABLE      0x8
#define FA_ADC_PROC_MODE_WINDOW   1
#define FA_ADC_PROC_MODE_PEAK     2
#define FA_ADC_PROC_MODE_SUM      3
#define FA_ADC_PROC_MODE_TIME     7

#define FA_ADC_VERSION_MASK  0x7fff
#define FA_ADC_PLAYBACK_MODE 0x0080

#define FA_ADC_PEAK_MASK     0x0070
#define FA_ADC_CHAN_MASK     0xffff

#define FA_ADC_DEFAULT_PL     50
#define FA_ADC_DEFAULT_PTW    50
#define FA_ADC_DEFAULT_NSB     5
#define FA_ADC_DEFAULT_NSA    10
#define FA_ADC_DEFAULT_NP      4
#define FA_ADC_DEFAULT_THRESH  0

#define FA_ADC_MAX_PL       2000/*1000*/
#define FA_ADC_MAX_PTW       512
#define FA_ADC_MAX_NSB       500
#define FA_ADC_MAX_NSA       500
#define FA_ADC_MAX_NP          1
#define FA_ADC_MAX_THRESH   1023

#define FA_ADC_MIN_PL          1
#define FA_ADC_MIN_PTW         1
#define FA_ADC_MIN_NSB         1
#define FA_ADC_MIN_NSA         1
#define FA_ADC_MIN_NP          1
#define FA_ADC_MIN_THRESH      0

#define FA_ADC_PEDESTAL_MASK   0xffff

/* Define parameters for HITSUM (Trigger) FPGA */
#define FA_ITRIG_VERSION_MASK     0x1fff
#define FA_ITRIG_CONFIG_MASK      0x001F
#define FA_ITRIG_ENABLE_MASK      0x0010
#define FA_ITRIG_P2OUT_MASK       0x0008
#define FA_ITRIG_MODE_MASK        0x0007

#define FA_ITRIG_TABLE_MODE       1
#define FA_ITRIG_WINDOW_MODE      2
#define FA_ITRIG_COIN_MODE        3
#define FA_ITRIG_SUM_MODE         4

#define FA_ITRIG_DISABLED         0x0010
#define FA_ITRIG_HB_P2OUT         0x0008

#define FA_ITRIG_MIN_WIDTH        0x01  /* 4 nsec */
#define FA_ITRIG_MAX_WIDTH        0x100  /* 1.024 microsec */
#define FA_ITRIG_MAX_DELAY        0xf00  /* 15.360 microsec */  

#define FA_ITRIG_WINDOW_MAX_WIDTH 0x200  /* 2.048 microsec */


/* Define ADC Data Types and Masks */

#define FA_DATA_TYPE_DEFINE       0x80000000
#define FA_DATA_TYPE_MASK         0x78000000
#define FA_DATA_SLOT_MASK         0x07c00000

#define FA_DATA_BLOCK_HEADER      0x00000000
#define FA_DATA_BLOCK_TRAILER     0x08000000
#define FA_DATA_EVENT_HEADER      0x10000000
#define FA_DATA_TRIGGER_TIME      0x18000000
#define FA_DATA_WINDOW_RAW        0x20000000
#define FA_DATA_WINDOW_SUM        0x28000000
#define FA_DATA_PULSE_RAW         0x30000000
#define FA_DATA_PULSE_INTEGRAL    0x38000000
#define FA_DATA_PULSE_TIME        0x40000000
#define FA_DATA_STREAM            0x48000000
#define FA_DATA_INVALID           0x70000000
#define FA_DATA_FILLER            0x78000000
#define FA_DUMMY_DATA             0xf800fafa

#define FA_DATA_BLKNUM_MASK       0x0000003f
#define FA_DATA_WRDCNT_MASK       0x003fffff
#define FA_DATA_TRIGNUM_MASK      0x07ffffff

/* Define Firmware registers Data types and Masks */
#define FA_PROMREG1_SRAM_TO_PROM1    0x0
#define FA_PROMREG1_SRAM_TO_PROM2    0x1
#define FA_PROMREG1_PROM1_TO_SRAM    0x3
#define FA_PROMREG1_PROM2_TO_SRAM    0x4
#define FA_PROMREG1_GET_ID1          0x6
#define FA_PROMREG1_GET_ID2          0x7
#define FA_PROMREG1_ERASE_PROM1      0x9
#define FA_PROMREG1_ERASE_PROM2      0xA
#define FA_PROMREG1_REBOOT_FPGA1     0xC
#define FA_PROMREG1_REBOOT_FPGA2     0xD
#define FA_PROMREG1_READY            (1<<31)

#define FA_MEM_ADR_INCR_MEM1      (1<<31)
#define FA_MEM_ADR_INCR_MEM2      (1<<30)

/* Define Scaler Control bits */
#define FA_SCALER_CTRL_ENABLE     (1<<0)
#define FA_SCALER_CTRL_LATCH      (1<<1)
#define FA_SCALER_CTRL_RESET      (1<<2)
#define FA_SCALER_CTRL_MASK        (0x7)

/* Define FADC Signal Distribution card bits */

#define FA_SDC_BOARD_ID    0x0200
#define FA_SDC_BOARD_MASK  0xfffe
#define FA_SDC_ADR_MASK    0xffc0

#define FASDC_BUSY_MASK   0x00ff

#define FASDC_CSR_BUSY           0x1
#define FASDC_CSR_TRIG          0x20
#define FASDC_CSR_SRESET        0x40
#define FASDC_CSR_INIT          0x80
#define FASDC_CSR_MASK        0x00e1

#define FASDC_CTRL_CLK_EXT               0x1
#define FASDC_CTRL_NOSYNC_TRIG          0x10
#define FASDC_CTRL_ENABLE_SOFT_TRIG     0x20
#define FASDC_CTRL_NOSYNC_SRESET       0x100
#define FASDC_CTRL_ENABLE_SOFT_SRESET  0x200
#define FASDC_CTRL_ASSERT_SOFT_BUSY   0x8000
#define FASDC_CTRL_MASK               0x8331

/* Definitions for FADC Firmware Tools */
#define        FADC_FIRMWARE_LX110    0
#define        FADC_FIRMWARE_FX70T    1


/* Function Prototypes */
STATUS faInit (UINT32 addr, UINT32 addr_inc, int nadc, int iFlag);
int faSetClockSource(int id, int clkSrc);
void faStatus(int id, int sflag);
void faGStatus(int sflag);
int faSetProcMode(int id, int pmode, unsigned int PL, unsigned int PTW, 
	          unsigned int NSB, unsigned int NSA, unsigned int NP, int bank);
void faGSetProcMode(int pmode, unsigned int PL, unsigned int PTW, 
	          unsigned int NSB, unsigned int NSA, unsigned int NP, int bank);
void faSetNormalMode(int id, int opt);
int faSetPPG(int id, int pmode, unsigned short *sdata, int nsamples);
void faPPGEnable(int id);
void faPPGDisable(int id);
int faItrigBurstConfig(int id, unsigned int ntrig, 
		   unsigned int burst_window, unsigned int busy_period);
unsigned int faItrigControl(int id, unsigned short itrig_width, unsigned short itrig_dt);
int faReadBlock(int id, volatile UINT32 *data, int nwrds, int rflag);
int faPrintBlock(int id, int rflag);
void faClear(int id);
void faClearError(int id);
unsigned int faReadCSR(int id);
void faReset(int id, int iFlag);
void faSoftReset(int id);
void faResetToken(int id);
void faSetCalib(int id, unsigned short sdelay, unsigned short tdelay);
void faChanDisable(int id, unsigned short cmask);
void faEnable(int id, int eflag, int bank);
void faDisable(int id, int eflag);
void faTrig(int id);
void faSync(int id);
int faDready(int id,int dflag);
int faBready(int id);
unsigned int faGBready();
unsigned int faScanMask();
int faBusyLevel(int id, unsigned int val, int bflag);
int faBusy(int id);
void faEnableSoftTrig(int id);
void faDisableSoftTrig(int id);
void faEnableSoftSync(int id);
void faDisableSoftSync(int id);
void faEnableClk(int id);
void faDisableClk(int id);
void faEnableTriggerOut(int id, int output);
void faEnableBusError(int id);
void faDisableBusError(int id);
void faEnableMultiBlock(int tflag);
void faDisableMultiBlock();
int faSetBlockLevel(int id, int level);
int faSetClkSource(int id, int source);
int faSetTrigSource(int id, int source);
int faSetSyncSource(int id, int source);
void faEnableFP(int id);
int faSetTrigOut(int id, int trigout);
int faResetTriggerCount(int id);
int faSetThreshold(int id, unsigned short tvalue, unsigned short chmask);
int faPrintThreshold(int id);
int faSetDAC(int id, unsigned short dvalue, unsigned short chmask);
void faPrintDAC(int id);
int faLive(int id, int sflag);
void faDataDecode(unsigned int data);

int faSetMGTTestMode(int id, unsigned int mode);

/* FADC scaler routine prototypes */
int faReadScalers(int id, volatile unsigned int *data, unsigned int chmask, int rflag);
int faPrintScalers(int id, int rflag);
int faClearScalers(int id);
int faLatchScalers(int id);
int faEnableScalers(int id);
int faDisableScalers(int id);

/* FADC Internal Trigger Routine prototypes */
unsigned int faItrigStatus(int id, int sFlag);
int faItrigSetMode(int id, int tmode, unsigned int wMask, unsigned int wWidth,
	           unsigned int cMask, unsigned int sumThresh, unsigned int *tTable);
int faItrigInitTable(int id, unsigned int *table);
int faItrigSetHBwidth(int id, unsigned short hbWidth, unsigned short hbMask);
unsigned int faItrigGetHBwidth(int id, unsigned int chan);
void faItrigPrintHBwidth(int id);
unsigned int faItrigOutConfig(int id, unsigned short itrigDelay, unsigned short itrigWidth);
void faItrigEnable(int id);
void faItrigDisable(int id);
int faItrigGetTableVal (int id, unsigned short pMask);

/* FADC Firmware Tools Prototypes */
int  fadcFirmwareLoad(int id, int chip, int pFlag);
int  fadcFirmwareGLoad(int chip, int pFlag);
void fadcFirmwareDownloadConfigData(int id);
int  fadcFirmwareVerifyDownload (int id);
int  fadcFirmwareTestReady(int id, int n_try, int pFlag);
int  fadcFirmwareZeroSRAM (int id);
void fadcFirmwareSetFilename(char *filename, int chip);
int  fadcFirmwareReadFile(char *filename);
int  fadcFirmwareReadMcsFile(char *filename);

#ifdef HPS
void faWriteHPSConfig(int id, unsigned int config6, unsigned int config7);
int  faSetHPSParameters(int id, unsigned int tot, 
			unsigned int maxIntTime, unsigned int sumScaleFactor);
#endif

/* FLASH SDC prototypes */
int faSDC_Config(unsigned short cFlag, unsigned short bMask);
void faSDC_Status(int sFlag);
void faSDC_Enable(int nsync);
void faSDC_Disable();
void faSDC_Sync();
void faSDC_Trig();
int faSDC_Busy();

#endif /* __FADCLIB__ */
