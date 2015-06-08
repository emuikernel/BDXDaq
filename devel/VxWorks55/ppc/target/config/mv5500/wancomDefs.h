/* wancomDefs.h - MV64260 Ethernet Driver To BSP Defines Header File */

/* Copyright 2002,2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,23jul03,scb add push to GT64260_REG_WR - prevent interrupt with no cause. 
01c,19nov02,scb Undid (01b) change - fix for gei buffs in cacheable mem is in.
01b,18nov02,scb Changed #defines for "ETHER_..." to "WANCON_...".
01a,28oct02,scb Copied from hxeb100 base version 01a.
*/

/*

DESCRIPTION

This file connects the Wind River WANCOM driver #defines to the Motorola
MV64260 defines to allow the Wind River driver to compile with as little
modification as possible in the Motorola MV64260 BSP environment.

*/

#ifndef INCwancomDefsh
#define INCwandcomDefsh

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

/* includes */

#include "mv64260.h"

/* defines */

#define GT64260_BASE_ADRS          MV64260_REG_BASE

#define ETH_PHY_ADRS_PORT0         PHY_ADDR_ENET_PORT0
#define ETH_PHY_ADRS_PORT1         PHY_ADDR_ENET_PORT1

#define ETH_PORT_MEM_SIZE          ETHER_PORT_MEM_SIZE
#define ETH_MEM_START_ADRS(unit)   \
        (char *)(ETHER_MEM_START_ADDR + (ETHER_PORT_MEM_SIZE * unit))

#define GT64260_REG_RD(x,val)   \
        { \
                UINT32 tmp ; \
                tmp = *((UINT32*)(GT64260_BASE_ADRS+(x))); \
                EIEIO ; \
                (*val) = LONGSWAP(tmp); \
                EIEIO ; \
        }
#define GT64260_REG_WR(x,val)   \
        {       \
                UINT32 tmp ; \
        	tmp = LONGSWAP(val); \
                EIEIO ; \
                *((UINT32*)(GT64260_BASE_ADRS+(x))) = tmp ; \
                EIEIO ; \
                tmp = *((UINT32*)(GT64260_BASE_ADRS+(x))); \
        }

/* Main interrupt cause high */

#define INT_BIT_ETH0            0x00000001
#define INT_BIT_ETH1            0x00000002
#define INT_BIT_ETH2            0x00000004

#define INT_PRI_ETH0            ICI_MICH_INT_NUM_0
#define INT_PRI_ETH1            ICI_MICH_INT_NUM_1
#define INT_PRI_ETH2            ICI_MICH_INT_NUM_2

#define INT_LVL_ETH0            INT_PRI_ETH0
#define INT_LVL_ETH1            INT_PRI_ETH1
#define INT_LVL_ETH2            INT_PRI_ETH2

#define INT_VEC_ETH0            INT_LVL_ETH0
#define INT_VEC_ETH1            INT_LVL_ETH1
#define INT_VEC_ETH2            INT_LVL_ETH2

/*
 * Descriptor size in bytes as seen by the MV64260
 */

#define DEV_DESC_SIZE           16  

/*      Wind River Defines      Motorola Defines (mv64260.h)   Offsets     */

/* Communications Unit */

#define CU_ETH0_ADRS_CTRL_L     CU_ENET0_ADDR_CTRL_LO          /* (0xf200) */
#define CU_ETH0_ADRS_CTRL_H     CU_ENET0_ADDR_CTRL_HI          /* (0xf204) */
#define CU_ETH0_RX_BUFF_PCI_H   CU_ENET0_RCV_BUF_PCI_HI_ADDR   /* (0xf208) */
#define CU_ETH0_TX_BUFF_PCI_H   CU_ENET0_XMIT_BUF_PCI_HI_ADDR  /* (0xf20c) */
#define CU_ETH0_RX_DESC_PCI_H   CU_ENET0_RCV_DESC_PCI_HI_ADDR  /* (0xf210) */
#define CU_ETH0_TX_DESC_PCI_H   CU_ENET0_XMIT_DESC_PCI_HI_ADDR /* (0xf214) */
#define CU_ETH0_HASH_TBL_PCI_H  CU_ENET0_HASH_TBL_PCI_HI_ADDR  /* (0xf218) */

#define CU_ETH1_ADRS_CTRL_L     CU_ENET1_ADDR_CTRL_LO          /* (0xf220) */
#define CU_ETH1_ADRS_CTRL_H     CU_ENET1_ADDR_CTRL_HI          /* (0xf224) */
#define CU_ETH1_RX_BUFF_PCI_H   CU_ENET1_RCV_BUF_PCI_HI_ADDR   /* (0xf228) */
#define CU_ETH1_TX_BUFF_PCI_H   CU_ENET1_XMIT_BUF_PCI_HI_ADDR  /* (0xf22c) */
#define CU_ETH1_RX_DESC_PCI_H   CU_ENET1_RCV_DESC_PCI_HI_ADDR  /* (0xf230) */
#define CU_ETH1_TX_DESC_PCI_H   CU_ENET1_XMIT_DESC_PCI_HI_ADDR /* (0xf234) */
#define CU_ETH1_HASH_TBL_PCI_H  CU_ENET1_HASH_TBL_PCI_HI_ADDR  /* (0xf238) */

#define CU_ETH2_ADRS_CTRL_L     CU_ENET2_ADDR_CTRL_LO          /* (0xf240) */
#define CU_ETH2_ADRS_CTRL_H     CU_ENET2_ADDR_CTRL_HI          /* (0xf244) */
#define CU_ETH2_RX_BUFF_PCI_H   CU_ENET2_RCV_BUF_PCI_HI_ADDR   /* (0xf248) */
#define CU_ETH2_TX_BUFF_PCI_H   CU_ENET2_XMIT_BUF_PCI_HI_ADDR  /* (0xf24c) */
#define CU_ETH2_RX_DESC_PCI_H   CU_ENET2_RCV_DESC_PCI_HI_ADDR  /* (0xf250) */
#define CU_ETH2_TX_DESC_PCI_H   CU_ENET2_XMIT_DESC_PCI_HI_ADDR /* (0xf254) */
#define CU_ETH2_HASH_TBL_PCI_H  CU_ENET2_HASH_TBL_PCI_HI_ADDR  /* (0xf258) */

/* Ethernet Registers */

#define ETH_PHY_ADRS            FE_ENET_PHY_ADDR               /* (0x2000) */

#define ETH_SMIR                FE_ENET_SMI                    /* (0x2010) */

#define ETH0_PCR                FE_ENET0_PORT_CFG              /* (0x2400) */
#define ETH1_PCR                FE_ENET1_PORT_CFG              /* (0x2800) */
#define ETH2_PCR                FE_ENET2_PORT_CFG              /* (0x2c00) */

#define ETH0_PCXR               FE_ENET0_PORT_CFG_EXT          /* (0x2408) */
#define ETH1_PCXR               FE_ENET1_PORT_CFG_EXT          /* (0x2808) */
#define ETH2_PCXR               FE_ENET2_PORT_CFG_EXT          /* (0x2c08) */

#define ETH0_PCMR               FE_ENET0_PORT_CMD              /* (0x2410) */
#define ETH1_PCMR               FE_ENET1_PORT_CMD              /* (0x2810) */
#define ETH2_PCMR               FE_ENET2_PORT_CMD              /* (0x2c10) */

#define ETH0_PSR                FE_ENET0_PORT_STATUS           /* (0x2418) */
#define ETH1_PSR                FE_ENET1_PORT_STATUS           /* (0x2818) */
#define ETH2_PSR                FE_ENET2_PORT_STATUS           /* (0x2c18) */

#define ETH0_SPR                FE_ENET0_SERIAL_PARAMS         /* (0x2420) */
#define ETH1_SPR                FE_ENET1_SERIAL_PARAMS         /* (0x2820) */
#define ETH2_SPR                FE_ENET2_SERIAL_PARAMS         /* (0x2c20) */

#define ETH0_HTPR               FE_ENET0_HASH_TBL_PTR          /* (0x2428) */
#define ETH1_HTPR               FE_ENET1_HASH_TBL_PTR          /* (0x2828) */
#define ETH2_HTPR               FE_ENET2_HASH_TBL_PTR          /* (0x2c28) */

#define ETH0_FCSAL              FE_ENET0_FLOW_CTRL_SRC_ADDR_LO /* (0x2430) */
#define ETH1_FCSAL              FE_ENET1_FLOW_CTRL_SRC_ADDR_LO /* (0x2830) */
#define ETH2_FCSAL              FE_ENET2_FLOW_CTRL_SRC_ADDR_LO /* (0x2c30) */

#define ETH0_FCSAH              FE_ENET0_FLOW_CTRL_SRC_ADDR_HI /* (0x2438) */
#define ETH1_FCSAH              FE_ENET1_FLOW_CTRL_SRC_ADDR_HI /* (0x2838) */
#define ETH2_FCSAH              FE_ENET2_FLOW_CTRL_SRC_ADDR_HI /* (0x2c38) */

#define ETH0_SDCR               FE_ENET0_SDMA_CFG              /* (0x2440) */
#define ETH1_SDCR               FE_ENET1_SDMA_CFG              /* (0x2840) */
#define ETH2_SDCR               FE_ENET2_SDMA_CFG              /* (0x2c40) */

#define ETH0_SDCMR              FE_ENET0_SDMA_CMD              /* (0x2448) */
#define ETH1_SDCMR              FE_ENET1_SDMA_CMD              /* (0x2848) */
#define ETH2_SDCMR              FE_ENET2_SDMA_CMD              /* (0x2c48) */

#define ETH0_ICR                FE_ENET0_INT_CAUSE             /* (0x2450) */
#define ETH1_ICR                FE_ENET1_INT_CAUSE             /* (0x2850) */
#define ETH2_ICR                FE_ENET2_INT_CAUSE             /* (0x2c50) */

#define ETH0_IMR                FE_ENET0_INT_MASK              /* (0x2458) */
#define ETH1_IMR                FE_ENET0_INT_MASK              /* (0x2858) */
#define ETH2_IMR                FE_ENET0_INT_MASK              /* (0x2c58) */

#ifdef __cplusplus

    }
#endif /* __cplusplus */

#endif /* INCwancomDefsh */
