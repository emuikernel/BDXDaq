/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* mpsc.h - Header File for : MPSC module
*
* DESCRIPTION:
*		This file include the definition of the MPSC channel struct as well 
*		as other MPSC register bit definitions.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/
#ifndef __INCmpsch
#define __INCmpsch

/* includes */
#include "gtCore.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* defines */

/* Set default MPSC Main Configuration Register Low (MMCRLx) */
#define MPSC_MMCRL_UART_VALUE              \
            MMCRL_MODE_UART   		   |   \
            MMCRL_ET_ENABLED           |   \
            MMCRL_ER_ENABLED           |   \
            MMCRL_NLM_NULL_MODEM       |   \
            MMCRL_TIDL_DATA_TX         |   \
            MMCRL_RTSM_IDLE            |   \
            MMCRL_CTSS_ASYNCHRONOUS    |   \
            MMCRL_CDS_ASYNCHRONOUS     |   \
            MMCRL_CRCM_CRC16
    
/* Set default MPSC Main Configuration Register High (MMCRHx) */
#define MPSC_MMCRH_UART_VALUE	       	                                	  \
            MMCRH_TPL_16                                            	 	 |\
            ((DEFAULT_MMCRH_UART_TPPT << MMCRH_TPPT_SHIFT) & MMCRH_TPPT_MASK)|\
            MMCRH_TCDV_X8                                           	 	 |\
            MMCRH_RDW_LOW_LATENCY                                   	 	 |\
            MMCRH_RCDV_X8                                       
            
#define DEFAULT_MMCRH_UART_TPPT   0xF 
    
/* Set default MPSC Protocol Configuration Register (MPCRx) for UART Mode */
#define MPSC_MPCR_UART_VALUE                \
			MPCR_ISO_ASYNCHRONOUS_MODE |	\
			MPCR_CL_8_DATA_BITS        |	\
			MPCR_SBL_ONE_STOP_BIT

    
/* Set default MPSC CHR1 - UART Break/Stuff Register (UBSR) */
#define MPSC_CHAR1_UART_VALUE                  								  \
            ((UART_DEFAULT_CHAR1_TCS << CHAR1_TCS_SHIFT) & CHAR1_TCS_MASK) |  \
            ((UART_DEFAULT_CHAR1_BRK << CHAR1_BRK_SHIFT) & CHAR1_BRK_MASK)

#define UART_DEFAULT_CHAR1_TCS	0x33
#define UART_DEFAULT_CHAR1_BRK	0x22

/* Set default MPSC CHR2 - UART Break/Stuff Register (UBSR) */
#define MPSC_CHAR2_UART_VALUE                    \
            CHAR2_TPM_ODD               |   \
            CHAR2_RPM_ODD               |   \
            CHAR2_EH_ENABLED
    

/* MPSC port Routing Register (MRR) bit description */
#define MRR_MR_BIT_GAP      6		/* Bit gap between MR0 and MR1 in MRR */
#define MRR_MR_MASK         0x0007
#define MRR_MR_UNCONNECTED  0x0007
#define MRR_MR_SERIAL_PORT	0x0000

/* MPSC Clock Routing Register (CRR) definition */
#define CRR_BIT_GAP		8
#define CRR_MASK    	0x000F
#define CRR_BRG0    	0x0000
#define CRR_BRG1    	0x0001
#define CRR_SCLK0    	0x0008
#define CRR_TSCLK0    	0x0009


/* MPSC Main Configuration Register Low (MMCRLx) bit descriptor */
#define MMCRL_MODE_MASK             0x00000007
#define MMCRL_MODE_HDLC             0x00000000
#define MMCRL_MODE_UART             0x00000004
#define MMCRL_MODE_BISYNC           0x00000005
#define MMCRL_TTX_TRANSPARENT       0x00000008
#define MMCRL_TRX_TRANSPARENT       0x00000010
#define MMCRL_ET_ENABLED            0x00000040
#define MMCRL_ER_ENABLED            0x00000080
#define MMCRL_LPBK_LOOP_BACK        0x00000100
#define MMCRL_LPBK_ECHO             0x00000200
#define MMCRL_LPBK_LOOP_BACK_ECHO   0x00000300
#define MMCRL_NLM_NULL_MODEM        0x00000400
#define MMCRL_TSYN_TX_SYNC_RX       0x00001000
#define MMCRL_TIDL_DATA_TX          0x00000000
#define MMCRL_TIDL_CONTINUES        0x00010000
#define MMCRL_RTSM_IDLE             0x00000000
#define MMCRL_RTSM_FLAG_SYNC        0x00020000
#define MMCRL_CTSS_ASYNCHRONOUS     0x00000000
#define MMCRL_CTSS_SYNCHRONOUS      0x00080000
#define MMCRL_CDS_ASYNCHRONOUS      0x00000000
#define MMCRL_CDS_SYNCHRONOUS       0x00100000
#define MMCRL_CTSM_PULSE_MODE       0x00200000
#define MMCRL_CDM_PULSE_MODE        0x00400000
#define MMCRL_CRCM_CRC16_CCITT      0x00000000
#define MMCRL_CRCM_CRC16            0x00800000
#define MMCRL_CRCM_CRC32_CCITT      0x01000000
#define MMCRL_TRVD_REVERSE          0x20000000
#define MMCRL_RRVD_REVERSE          0X40000000
#define MMCRL_GDE_GLITCH_DETECT     0X80000000
                   

/* MPSC Main Configuration Register High (MMCRHx) bit descriptor */
#define MMCRH_TCI_ENABLE            0x00000001
#define MMCRH_TINV_ENABLE           0x00000002
#define MMCRH_TPL_NONE              0x00000000
#define MMCRH_TPL_1                 0x00000004
#define MMCRH_TPL_2                 0x00000008
#define MMCRH_TPL_4                 0x0000000C
#define MMCRH_TPL_6                 0x00000010
#define MMCRH_TPL_8                 0x00000014
#define MMCRH_TPL_16                0x00000018
#define MMCRH_TPPT_MASK             0x000001E0
#define MMCRH_TPPT_SHIFT            5
#define MMCRH_TCDV_MASK             0x00000600
#define MMCRH_TCDV_X1               0x00000000
#define MMCRH_TCDV_X8               0x00000200
#define MMCRH_TCDV_X16              0x00000400
#define MMCRH_TCDV_X32              0x00000600
#define MMCRH_TDEC_NRZ              0x00000000
#define MMCRH_TDEC_NRZI             0x00000800
#define MMCRH_TDEC_FM0              0x00001000
#define MMCRH_TDEC_MANCHESTER       0x00002000
#define MMCRH_TDEC_DIFF_MANCHESTER  0x00003000
#define MMCRH_RINV_ENABLE           0x00010000
#define MMCRH_RDW_LOW_LATENCY       0x00400000
#define MMCRH_RSYL_EXT_SYNC         0x00000000
#define MMCRH_RSYL_4_BIT_SYNC       0x00800000
#define MMCRH_RSYL_8_BIT_SYNC       0x01000000
#define MMCRH_RSYL_16_BIT_SYNC      0x01800000
#define MMCRH_RCDV_MASK             0x06000000
#define MMCRH_RCDV_X1               0x00000000
#define MMCRH_RCDV_X8               0x02000000
#define MMCRH_RCDV_X16              0x04000000
#define MMCRH_RCDV_X32              0x06000000
#define MMCRH_RENC_NRZ              0x00000000
#define MMCRH_RENC_NRZI             0x08000000
#define MMCRH_RENC_FM0              0x10000000
#define MMCRH_RENC_MANCHESTER       0x20000000
#define MMCRH_RENC_DIFF_MANCHESTER  0x30000000
#define MMCRH_SEDG_BOTH_RISE_FALL   0x00000000
#define MMCRH_SEDG_RISING           0x40000000
#define MMCRH_SEDG_FALLING          0x80000000
#define MMCRH_SEDG_NO_ADJUST        0xC0000000


/* MPSC Protocol Configuration Register (MPCRx) for UART Mode */
#define MPCR_DRT_ENABLED            0x00000040
#define MPCR_ISO_ASYNCHRONOUS_MODE  0x00000000
#define MPCR_ISO_SYNCHRONOUS_MODE   0x00000080
#define MPCR_RZS_ZERO_STOP_BIT      0x00000100
#define MPCR_FRZ_FREEZ_TX           0x00000200
#define MPCR_UM_MULTI_DROP          0x00000400
#define MPCR_CL_5_DATA_BITS         0x00000000
#define MPCR_CL_6_DATA_BITS         0x00001000
#define MPCR_CL_7_DATA_BITS         0x00002000
#define MPCR_CL_8_DATA_BITS         0x00003000
#define MPCR_SBL_ONE_STOP_BIT       0x00000000
#define MPCR_SBL_TWO_STOP_BITS      0x00004000
#define MPCR_FLC_ASYNCHRONOUS_MODE  0x00008000

/* MPSC Channel Registers (CHxRx) for UART Mode */

/* MPSC CHR1 - UART Break/Stuff Register (UBSR) */
#define CHAR1_TCS_MASK              0x000000FF
#define CHAR1_TCS_SHIFT             0
#define CHAR1_BRK_MASK              0x00FF0000
#define CHAR1_BRK_SHIFT             16

/* MPSC CHR2 - UART Command Register (CR) */
#define CHAR2_TEV_ENABLED           0x00000002
#define CHAR2_TPM_ODD               0x00000000
#define CHAR2_TPM_LOW               0x00000004
#define CHAR2_TPM_EVEN              0x00000008
#define CHAR2_TPM_HIGH              0x0000000C
#define CHAR2_AT_TX_ABORT           0x00000080
#define CHAR2_TTCS_TX_CTS_CHAR      0x00000200
#define CHAR2_REV_ENABLED           0x00020000
#define CHAR2_RPM_ODD               0x00000000
#define CHAR2_RPM_LOW               0x00040000
#define CHAR2_RPM_EVEN              0x00080000
#define CHAR2_RPM_HIGH              0x000C0000
#define CHAR2_AR_RX_ABORT           0x00800000
#define CHAR2_CRD_ENABLED           0x02000000
#define CHAR2_EH_ENABLED            0x80000000

/* MPSC CHR8 - UART Control Character Register (CCR) */
#define CHAR8_CHAR_MASK             0x000000FF
#define CHAR8_INT_ENABLED           0x00001000
#define CHAR8_CO                    0x00002000
#define CHAR8_REJECT_CHAR           0x00004000
#define CHAR8_V_ENTRY_VALID         0x00008000

/* MPSC CHR10 - UART Event Status Register (ESR) */
#define CHAR10_CLEAR_TO_SEND_SIGNAL  0x00000001
#define CHAR10_CARRIER_DETECT_SIGNAL 0x00000002
#define CHAR10_TX_IN_IDLE_STATE      0x00000008
#define CHAR10_RX_OUT_OF_HUNT_STATE  0x00000020
#define CHAR10_RX_IN_HUNT_STATE      0x00000000
#define CHAR10_RX_LINE_STATUS        0x00000080
#define CHAR10_RX_IDLE_LINE          0x00000800
#define CHAR10_RX_CTRL_CHAR_MASK     0x00FF0000

/* TCDV and RCDV options macros */
#define MPSC_CDV_X1		1
#define MPSC_CDV_X8		8
#define MPSC_CDV_X16	16
#define MPSC_CDV_X32	32

/* typedefs */

typedef enum _mpscChan 
{	
	MPSC_0,
    MPSC_1
}MPSC_NUM;
	
typedef struct _mpscChannel
{
    MPSC_NUM		mpscNum;    /* MPSC channel number                      */
	unsigned int    mrr;        /* MPSC port Routing Register (MRR) 		*/
    unsigned int    rcrr;       /* Rx Clock Routing Register (RCRR) 		*/
    unsigned int    tcrr;       /* Tx Clock Routing Register (TCRR) 		*/
    unsigned int    mmcrl;      /* Main Configuration Register Low (MMCRLx) */
    unsigned int    mmcrh;      /* Main Configuration Register High (MMCRHx)*/
    unsigned int    mpcr;       /* Protocol Configuration Register (MPCRx)  */
    unsigned int    char1;      /* Channel register 1                       */
    unsigned int    char2;      /* Channel register 2                       */
    unsigned int    char3;      /* Channel register 3                       */
    unsigned int    char4;      /* Channel register 4                       */
    unsigned int    char5;      /* Channel register 5                       */
    unsigned int    char6;      /* Channel register 6                       */
    unsigned int    char7;      /* Channel register 7                       */
    unsigned int    char8;      /* Channel register 8                       */
    unsigned int    char9;      /* Channel register 9                       */
    unsigned int    char10;     /* Channel register 10                      */
} MPSC_CHANNEL;


/* mpsc.h API list */
void mpscChanInit  (MPSC_CHANNEL *mpscChan);
bool mpscChanStart (MPSC_CHANNEL *mpscChan);
void mpscChanStopTx(MPSC_CHANNEL *mpscChan);
void mpscChanStopRx(MPSC_CHANNEL *mpscChan);
void mpscChanStopTxRx(MPSC_CHANNEL *mpscChan);
bool mpscChanSetCdv(MPSC_CHANNEL *mpscChan, int mpscCdv);

#ifdef __cplusplus
}
#endif

#endif /* __INCmpsch */

