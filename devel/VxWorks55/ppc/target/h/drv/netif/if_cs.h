/* if_cs.h - Crystal Semiconductor CS8900 network interface header */

/*
 * Copyright 1996 Crystal Semiconductor Corp.
 * Copyright 1990-1996 Wind River Systems, Inc.
 */

/*
modification history
--------------------
01a,16dec96,hdn  adapted and cleaned up.
*/


#ifndef __INCif_csh
#define __INCif_csh

#ifdef __cplusplus
extern "C" {
#endif


/* Individual Address (Ethernet Address) */

typedef struct cs_ia /* CS_IA */
    {
    USHORT word[3];
    } CS_IA;

/* Receive Frame Buffer */

typedef struct cs_rxbuf /* CS_RXBUF */
    {
    struct cs_rxbuf *pNext;
    UCHAR	status;
    UCHAR	refCount;
    USHORT	length;
    UCHAR	data[SIZEOF_ETHERHEADER + ETHERMTU];
    } CS_RXBUF;

/* Instance global variables */

typedef struct cs_softc /* CS_SOFTC */
    {
    struct arpcom arpCom;
    USHORT	ioAddr;
    USHORT	intLevel;
    USHORT	intVector;
    USHORT	*pPacketPage;
    USHORT	mediaType;
    USHORT	configFlags;
    CS_RXBUF	*pFreeRxBuff;
    BOOL	inMemoryMode;
    BOOL	txInProgress;
    BOOL	resetting;
    USHORT	rxDepth;
    USHORT	maxRxDepth;
    USHORT	maxTxDepth;
    UINT	loanCount;
    } CS_SOFTC;

/* Receive buffer status */

#define CS_RXBUF_FREE          0x0000
#define CS_RXBUF_ALLOCATED     0x0001
#define CS_RXBUF_LOANED        0x0002

/* Config Flags in cs_softc */

#define CS_CFGFLG_MEM_MODE     0x0001
#define CS_CFGFLG_USE_SA       0x0002
#define CS_CFGFLG_IOCHRDY      0x0004
#define CS_CFGFLG_DCDC_POL     0x0008
#define CS_CFGFLG_FDX          0x0010
#define CS_CFGFLG_NOT_EEPROM   0x8000

/* Media Type in cs_softc */

#define CS_MEDIA_AUI           0x0001
#define CS_MEDIA_10BASE2       0x0002
#define CS_MEDIA_10BASET       0x0003

/* Chip Identification */

#define CS_EISA_NUM_CRYSTAL    0x630E
#define CS_PROD_ID_MASK        0xE000
#define CS_PROD_ID_CS8900      0x0000
#define CS_PROD_ID_CS8920      0x4000
#define CS_PROD_ID_CS892X      0x6000
#define CS_PROD_REV_MASK       0x1F00

/* IO Port Offsets */

#define CS_PORT_RXTX_DATA     0x0000
#define CS_PORT_RXTX_DATA_1   0x0002
#define CS_PORT_TX_CMD        0x0004
#define CS_PORT_TX_LENGTH     0x0006
#define CS_PORT_ISQ           0x0008
#define CS_PORT_PKTPG_PTR     0x000A
#define CS_PORT_PKTPG_DATA    0x000C
#define CS_PORT_PKTPG_DATA_1  0x000E

/* PacketPage Offsets */

#define CS_PKTPG_EISA_NUM     0x0000
#define CS_PKTPG_PRODUCT_ID   0x0002
#define CS_PKTPG_IO_BASE      0x0020
#define CS_PKTPG_INT_NUM      0x0022
#define CS_PKTPG_MEM_BASE     0x002C
#define CS_PKTPG_EEPROM_CMD   0x0040
#define CS_PKTPG_EEPROM_DATA  0x0042
#define CS_PKTPG_RX_CFG       0x0102
#define CS_PKTPG_RX_CTL       0x0104
#define CS_PKTPG_TX_CFG       0x0106
#define CS_PKTPG_BUF_CFG      0x010A
#define CS_PKTPG_LINE_CTL     0x0112
#define CS_PKTPG_SELF_CTL     0x0114
#define CS_PKTPG_BUS_CTL      0x0116
#define CS_PKTPG_TEST_CTL     0x0118
#define CS_PKTPG_ISQ          0x0120
#define CS_PKTPG_RX_EVENT     0x0124
#define CS_PKTPG_TX_EVENT     0x0128
#define CS_PKTPG_BUF_EVENT    0x012C
#define CS_PKTPG_RX_MISS      0x0130
#define CS_PKTPG_TX_COL       0x0132
#define CS_PKTPG_LINE_ST      0x0134
#define CS_PKTPG_SELF_ST      0x0136
#define CS_PKTPG_BUS_ST       0x0138
#define CS_PKTPG_TX_CMD       0x0144
#define CS_PKTPG_TX_LENGTH    0x0146
#define CS_PKTPG_IND_ADDR     0x0158
#define CS_PKTPG_RX_STATUS    0x0400
#define CS_PKTPG_RX_LENGTH    0x0402
#define CS_PKTPG_RX_FRAME     0x0404
#define CS_PKTPG_TX_FRAME     0x0A00

/* EEPROM Offsets */

#define CS_EEPROM_IND_ADDR_H  0x001C
#define CS_EEPROM_IND_ADDR_M  0x001D
#define CS_EEPROM_IND_ADDR_L  0x001E
#define CS_EEPROM_ISA_CFG     0x001F
#define CS_EEPROM_MEM_BASE    0x0020
#define CS_EEPROM_XMIT_CTL    0x0023
#define CS_EEPROM_ADPTR_CFG   0x0024

/* Register Numbers */

#define CS_REG_NUM_MASK       0x003F
#define CS_REG_NUM_RX_EVENT   0x0004
#define CS_REG_NUM_TX_EVENT   0x0008
#define CS_REG_NUM_BUF_EVENT  0x000C
#define CS_REG_NUM_RX_MISS    0x0010
#define CS_REG_NUM_TX_COL     0x0012

/* Self Control Register */

#define CS_SELF_CTL_RESET     0x0040
#define CS_SELF_CTL_HC1E      0x2000
#define CS_SELF_CTL_HCB1      0x8000

/* Self Status Register */

#define CS_SELF_ST_INIT_DONE  0x0080
#define CS_SELF_ST_SI_BUSY    0x0100
#define CS_SELF_ST_EEP_PRES   0x0200
#define CS_SELF_ST_EEP_OK     0x0400
#define CS_SELF_ST_EL_PRES    0x0800

/* EEPROM Command Register */

#define CS_EEPROM_CMD_READ    0x0200
#define CS_EEPROM_CMD_ELSEL   0x0400

/* Bus Control Register */

#define CS_BUS_CTL_USE_SA     0x0200
#define CS_BUS_CTL_MEM_MODE   0x0400
#define CS_BUS_CTL_IOCHRDY    0x1000
#define CS_BUS_CTL_INT_ENBL   0x8000

/* Bus Status Register */

#define CS_BUS_ST_TX_BID_ERR  0x0080
#define CS_BUS_ST_RDY4TXNOW   0x0100

/* Line Control Register */

#define CS_LINE_CTL_RX_ON     0x0040
#define CS_LINE_CTL_TX_ON     0x0080
#define CS_LINE_CTL_AUI_ONLY  0x0100
#define CS_LINE_CTL_10BASET   0x0000

/* Test Control Register */

#define CS_TEST_CTL_DIS_LT    0x0080
#define CS_TEST_CTL_ENDEC_LP  0x0200
#define CS_TEST_CTL_AUI_LOOP  0x0400
#define CS_TEST_CTL_DIS_BKOFF 0x0800
#define CS_TEST_CTL_FDX       0x4000

/* Receiver Configuration Register */

#define CS_RX_CFG_SKIP        0x0040
#define CS_RX_CFG_RX_OK_IE    0x0100
#define CS_RX_CFG_CRC_ERR_IE  0x1000
#define CS_RX_CFG_RUNT_IE     0x2000
#define CS_RX_CFG_X_DATA_IE   0x4000
#define CS_RX_CFG_ALL_IE      0x7100

/* Receiver Event Register */

#define CS_RX_EVENT_DRIBBLE   0x0080
#define CS_RX_EVENT_RX_OK     0x0100
#define CS_RX_EVENT_IND_ADDR  0x0400
#define CS_RX_EVENT_BCAST     0x0800
#define CS_RX_EVENT_CRC_ERR   0x1000
#define CS_RX_EVENT_RUNT      0x2000
#define CS_RX_EVENT_X_DATA    0x4000

/* Receiver Control Register */

#define CS_RX_CTL_RX_OK_A     0x0100
#define CS_RX_CTL_MCAST_A     0x0200
#define CS_RX_CTL_IND_A       0x0400
#define CS_RX_CTL_BCAST_A     0x0800
#define CS_RX_CTL_CRC_ERR_A   0x1000
#define CS_RX_CTL_RUNT_A      0x2000
#define CS_RX_CTL_X_DATA_A    0x4000

/* Transmit Configuration Register */

#define CS_TX_CFG_LOSS_CRS_IE 0x0040
#define CS_TX_CFG_SQE_ERR_IE  0x0080
#define CS_TX_CFG_TX_OK_IE    0x0100
#define CS_TX_CFG_OUT_WIN_IE  0x0200
#define CS_TX_CFG_JABBER_IE   0x0400
#define CS_TX_CFG_16_COLL_IE  0x8000
#define CS_TX_CFG_ALL_IE      0x8FC0

/* Transmit Event Register */

#define CS_TX_EVENT_LOSS_CRS  0x0040
#define CS_TX_EVENT_SQE_ERR   0x0080
#define CS_TX_EVENT_TX_OK     0x0100
#define CS_TX_EVENT_OUT_WIN   0x0200
#define CS_TX_EVENT_JABBER    0x0400
#define CS_TX_EVENT_COLL_MASK 0x7800
#define CS_TX_EVENT_16_COLL   0x8000

/* Transmit Command Register */

#define CS_TX_CMD_START_5     0x0000
#define CS_TX_CMD_START_381   0x0080
#define CS_TX_CMD_START_1021  0x0040
#define CS_TX_CMD_START_ALL   0x00C0
#define CS_TX_CMD_FORCE       0x0100
#define CS_TX_CMD_ONE_COLL    0x0200
#define CS_TX_CMD_NO_CRC      0x1000
#define CS_TX_CMD_NO_PAD      0x2000

/* Buffer Configuration Register */

#define CS_BUF_CFG_SW_INT     0x0040
#define CS_BUF_CFG_RDY4TX_IE  0x0100
#define CS_BUF_CFG_TX_UNDR_IE 0x0200
#define CS_BUF_CFG_RX_MISS_IE 0x0400

/* Buffer Event Register */

#define CS_BUF_EVENT_SW_INT   0x0040
#define CS_BUF_EVENT_RDY4TX   0x0100
#define CS_BUF_EVENT_TX_UNDR  0x0200
#define CS_BUF_EVENT_RX_MISS  0x0400

/* ISA Configuration from EEPROM */

#define CS_ISA_CFG_IRQ_MASK   0x000F
#define CS_ISA_CFG_USE_SA     0x0080
#define CS_ISA_CFG_IOCHRDY    0x0100
#define CS_ISA_CFG_MEM_MODE   0x8000

/* Memory Base from EEPROM */

#define CS_MEM_BASE_MASK      0xFFF0

/* Adpater Configuration from EEPROM */

#define CS_ADPTR_CFG_MEDIA    0x0060
#define CS_ADPTR_CFG_10BASET  0x0020
#define CS_ADPTR_CFG_AUI      0x0040
#define CS_ADPTR_CFG_10BASE2  0x0060
#define CS_ADPTR_CFG_DCDC_POL 0x0080

/* Transmission Control from EEPROM */

#define CS_XMIT_CTL_FDX       0x8000

/* Miscellaneous definitions */

#define CS_MAXLOOP            0x8888
#define CS_RXBUFCOUNT         16
#define CS_MC_LOANED          5


#ifdef __cplusplus
}
#endif

#endif  /* __INCif_csh */
