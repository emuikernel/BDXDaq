/* fec2rorc_RCU.h */

/* Global registers and commands */
#define RCU_AFL          0x00008000
#define RCU_DCS_ON       0x0000E000
#define RCU_DDL_ON       0x0000F000
#define RCU_L1_TTC       0x0000E800
#define RCU_L1_I2C       0x0000F800
#define RCU_L1_CMD       0x0000D800
#define RCU_L1           0x0000D000
#define RCU_GLB_RESET    0x00002000
#define RCU_FEC_RESET    0x00002001
#define RCU_RESET        0x00002002
#define RCU_HEADER       0x00004001
#define RCU_WRD_RD       0x00005000
#define RCU_FIRMWARE_VER 0x00008008

/* ALTRO interface addressable memories */
#define RCU_INSTRUCTION_MEM  0x7000
#define RCU_PATTERN_MEM      0x6800
#define RCU_RESULT_MEM       0x6000
#define RCU_ACL              0x6400
#define RCU_DATA_MEM_1_LOW   0x7400
#define RCU_DATA_MEM_1_HIGH  0x7500
#define RCU_DATA_MEM_2_LOW   0x7C00
#define RCU_DATA_MEM_2_HIGH  0x7D00
#define RCU_ERRST            0x7800

/* ALTRO interface memory sizes */
#define RCU_INSTRUCTION_MEM_SZ  256
#define RCU_PATTERN_MEM_SZ     1024
#define RCU_RESULT_MEM_SZ       128
#define RCU_ACL_SZ              256
#define RCU_ERRST_SZ              1
#define RCU_AFL_SZ                1
#define RCU_DATA_MEM_1_LOW_SZ   256
#define RCU_DATA_MEM_1_HIGH_SZ  256
#define RCU_DATA_MEM_2_LOW_SZ   256
#define RCU_DATA_MEM_2_HIGH_SZ  256

#define AL_BCAST  0x1
#define AL_BRANCH 0x1
#define AL_FEC    0xF
#define AL_CHIP   0x7
#define AL_CHAN   0x1F
#define AL_REG    0x1F
#define AL_DATA   0x7FFFF

/********************************
 * Extracted from the u2f_lib.h *
 *******************************/

/* registers */
#define O_ERRST   0
#define O_TRCFG1  1
#define O_TRCNT   2
#define O_LWADD   3
#define O_IRADD   4
#define O_IRDAT   5
#define O_EVWORD  6
#define O_ACTFEC  7
#define O_FMIREG  8
#define O_FMOREG  9
#define O_TRCFG2 10
#define O_RESREG 11
#define O_ERRREG 12
#define O_INTMOD 13
#define O_INTREG 14

/* sizes */
#define S_IMEM  256
#define S_PMEM 1024
#define B_PMEM  256  
#define S_RMEM  128
#define S_ACL   256
#define S_DM    256

/* commands */
#define C_RS_STATUS 0x6c01
#define C_RS_TRCFG  0x6c02
#define C_RS_TRCNT  0x6c03
#define C_EXEC      0x0000
#define C_ABORT     0x0800
#define C_FECRST    0x2001
#define C_SWTRG	    0xD000
#define C_RS_DMEM1  0xD001
#define C_RS_DMEM2  0xD002
#define C_TRG_CLR   0xD003
#define C_WRFM	    0xD004
#define C_RDFM      0xD005
#define C_RDABORT   0xD006
#define C_CLR_EVTAG 0xD007 
#define C_SCCOMMAND 0xC000

/* read out modes */
#define M_FIRST   1  /* start a new read-out   */
#define M_LAST    2  /* terminate the read out */
#define M_TRIGGER 4  /* Send a S/W trigger */

#define USE_MINOR -1
