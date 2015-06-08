/* wancomEnd.h - Marvel/Galileo GT64260 network interface header */

/* Copyright 1990-2002 Wind River Systems, Inc. */
/* Copyright 2002 Motorola, Inc. All Rights Reserved */

/*

modification history
--------------------
01a,28oct02,scb Copied from hxeb100 base version 01a.

*/

/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED *
* OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.           *
********************************************************************************
* wancomEnd.h - Header File for : GT-642xx network interface header 
*
* DESCRIPTION:
*       This file defines the network devices that can be attached to the MUX.
*
* DEPENDENCIES:
*       WRS endLib library.
*
*******************************************************************************/

#ifndef __INCwancomEndh
#define __INCwancomEndh

/* includes */

#include "wancomDefs.h"
#include "etherLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* defines */

#define NUMBER_OF_ETHERNET_PORTS                2
#define NUMBER_OF_MPSC_PORTS                    2
#define MRR_REG_VALUE                           0x7ffe38
#define ETHERNET_DOWNLOADING_PORT               ETHERNET_PORT0

#define SHADOW_OWNER_BY_GT          1
#define SHADOW_OWNER_BY_CPU         0
#define OWNER_BY_GT                 (1<<31)
#define FIRST                       (1<<17)
#define LAST                        (1<<16)
#define ENABLE_INTERRUPT            (1<<23)
#define PADDING                     (1<<18)
#define GENERATE_CRC                (1<<22)
#define ERROR_SUMMARY               (1<<15)


/* this macros are used to enable access to MPSC_SDCMR */
#define ABORT_RECEIVE                   (1<<15)
#define STOP_TRANSMIT                   (1<<16)
#define ENABLE_RX_DMA                   (1<<7)
#define TX_DEMAND                       (1<<23)
#define ABORT_TRANSMIT                  (1<<31)

/* Serial second layer cause registers bits definition */
/* Ethernet mask bits */
#define RX_BUFFER_RETURN           0x1
#define TX_BUFFER_HIGH_PRIO         0x4
#define TX_BUFFER_LOW_PRIO          0x8
#define TX_END_HIGH_PRIO            0x40
#define TX_END_LOW_PRIO             0x80
#define RX_RESOURCE_ERR             0x100
#define TX_RESOURCE_ERR_HIGH_PRIO   0x400
#define TX_RESOURCE_ERR_LOW_PRIO    0x800
#define RX_OVERRUN                  0x1000
#define TX_UNDERRUN                 0x2000
#define RX_BUFFER_QUEUE_0           (1<<16)
#define RX_BUFFER_QUEUE_1           (1<<17)
#define RX_BUFFER_QUEUE_2           (1<<18)
#define RX_BUFFER_QUEUE_3           (1<<19)
#define RX_ERROR_QUEUE_0            (1<<20)
#define RX_ERROR_QUEUE_1            (1<<21)
#define RX_ERROR_QUEUE_2            (1<<22)
#define RX_ERROR_QUEUE_3            (1<<23)
#define MII_PHYS_STATUS_CHANGE      0x10000000
#define SMI_COMMAND_DONE            0x20000000
#define ETHER_INT_SUM               0x80000000


#define SERIAL_PORT_MULTIPLEX                               0xf010

#define ETHERNET_PORTS_DIFFERENCE_OFFSETS       0x400 

#define NIBBLE_SWAPPING_16_BIT(X) (((X&0xf)<<4) | ((X&0xf0)>>4) |\
        ((X&0xf00)<<4) | ((X&0xf000)>>4))

#define NIBBLE_SWAPPING_32_BIT(X) (((X&0xf)<<4) | ((X&0xf0)>>4) |\
        ((X&0xf00)<<4) | ((X&0xf000)>>4) | ((X&0xf0000)<<4) | ((X&0xf00000)>>4)\
         | ((X&0xf000000)<<4) | ((X&0xf0000000)>>4))

#define GT_NIBBLE(X) ((X&0x1)<<3)+((X&0x2)<<1)+((X&0x4)>>1)+((X&0x8)>>3)

#define FLIP_6_BITS(X) (((X&0x1)<<5)+((X&0x2)<<3)+((X&0x4)<<1)+\
        ((X&0x8)>>1)+((X&0x10)>>3)+((X&0x20)>>5))

#define FLIP_9_BITS(X) (((X&0x1)<<8)+((X&0x2)<<6)+((X&0x4)<<4)+\
        ((X&0x8)<<2)+((X&0x10)<<0)+((X&0x20)>>2)+((X&0x40)>>4)+\
        ((X&0x80)>>6)+((X&0x100)>>8))

#define _8K_TABLE                           0

#define MAC_ENTRY_SIZE 8
#define SKIP_BIT 1
#define SKIP (1<<1)
#define VALID 1
#define HOP_NUMBER 12

#define EIGHT_K 0x8000
#define HALF_K  0x8000/16
#define HASH_DEFUALT_MODE                   14
#define HASH_MODE                           13
#define HASH_SIZE                           12
#define PROMISCUOUS_MODE                    0

#define WANCOMEND_MAX_UNITS			2

#define RX_DESC_ALIGNED_SIZE		0x20
#define TX_DESC_ALIGNED_SIZE		0x20

#define RX_BUFFER_DEFAULT_SIZE      0x604	
#define TX_BUFFER_DEFAULT_SIZE      0x604
#define TX_BUF_OFFSET_IN_DESC       0x18	/* For buffers less than 8 Bytes */

#define PORT_CONFIG_VALUE			0x00000081	/* Ethernet config value  	  */
#define PORT_CONFIG_EXTEND_VALUE	0x00018800	/* Ethernet config ext. value */
#define PORT_SDMA_CONFIG_VALUE		0x00002000	/* SDMA config value		  */

#define GT_SERIAL_PORT_MUX			0x00000001	/* Serial Ports Multiplex Reg */
#define PORT_SDMA_START_TX_LOW		0x01000000	/* SDMA start Tx low command  */

#define END_RX_SEM_TAKE(semId,tmout) \
	    (semTake (semId,tmout))

#define END_RX_SEM_GIVE(semId) \
	    (semGive (semId))

#ifndef WANCOM_SYS_INT_CONNECT
#define WANCOM_SYS_INT_CONNECT(pDrvCtrl, pFunc, arg, pRet)                         \
	{                                                                           \
	*pRet = ERROR ;                                                             \
	if(wancomEndIntConnect != NULL) \
		{ \
        *pRet = (wancomEndIntConnect) ((VOIDFUNCPTR*)                          \
                             INUM_TO_IVEC (pDrvCtrl->port.level),          \
                             (pFunc), (int) (arg));                         \
        }                                                                   \
	}


#endif /* SYS_INT_CONNECT */

#ifndef WANCOM_SYS_INT_DISCONNECT
#define WANCOM_SYS_INT_DISCONNECT(pDrvCtrl, pFunc, arg, pRet)                      \
		{                                                                           \
		*pRet = ERROR ;                                                             \
		if(wancomEndIntDisconnect != NULL) \
			{ \
			*pRet = (wancomEndIntDisconnect) ((VOIDFUNCPTR*)                          \
								 INUM_TO_IVEC (pDrvCtrl->port.level),          \
								 (pFunc), (int) (arg));                         \
			}                                                                   \
		}



#endif /* SYS_INT_DISCONNECT */

#ifndef WANCOM_SYS_INT_ENABLE
#define WANCOM_SYS_INT_ENABLE(pDrvCtrl)                                            \
if (WANCOM_INT_ENABLE (pDrvCtrl))                                              \
    ((*(FUNCPTR) (WANCOM_INT_ENABLE (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_ENABLE*/

#ifndef WANCOM_SYS_INT_DISABLE
#define WANCOM_SYS_INT_DISABLE(pDrvCtrl)                                           \
if (WANCOM_INT_DISABLE (pDrvCtrl))                                              \
    ((*(FUNCPTR) (WANCOM_INT_DISABLE (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_DISABLE*/

#ifndef WANCOM_SYS_INT_ACK
#define WANCOM_SYS_INT_ACK(pDrvCtrl)                                               \
if (WANCOM_INT_ACK (pDrvCtrl))                                              \
    ((*(FUNCPTR) (WANCOM_INT_ACK (pDrvCtrl))) (pDrvCtrl->unit))
#endif /*SYS_INT_ACK*/

#ifndef LOCAL_TO_SYS_ADDR
#define LOCAL_TO_SYS_ADDR(unit,localAddr)                                   \
    ((int) pDrvCtrl->port.sysLocalToBus ?                                  \
    (*pDrvCtrl->port.sysLocalToBus) (unit, localAddr) : localAddr)
#endif /* LOCAL_TO_SYS_ADDR */

#ifndef SYS_TO_LOCAL_ADDR
#define SYS_TO_LOCAL_ADDR(unit,sysAddr)                                     \
    ((int) pDrvCtrl->port.sysBusToLocal ?                                  \
    (*pDrvCtrl->port.sysBusToLocal)(unit, sysAddr) : sysAddr)
#endif /* SYS_TO_LOCAL_ADDR */


#define WANCOM_DEV_NAME        "wancom"
#define WANCOM_DEV_NAME_LEN		7
#define DEF_NUM_CFDS    		32                  /* default number of CFDs (Hex) */
#define DEF_NUM_RFDS    		32                  /* default number of RFDs (Hex) */
#define WANCOM_100MBS      		100000000        /* bits per sec */
#define WANCOM_10MBS       		10000000         /* bits per sec */
#define EADDR_LEN       		6                   /* ethernet address length */
#define WANCOM_ADDR_LEN    		EADDR_LEN        /* ethernet address length */

	/* constants needed within this file */

#define EH_SIZE		14  		/* avoid structure padding issues */
#define N_MCAST		12

#define PORT0_BASE_OFFSET					0x2400

/* Ethernet port register offsets from port base address */
#define PORT_CONFIG_REG               		0x000
#define PORT_CONFIG_EXTEND_REG        		0x008
#define PORT_COMMAND_REG                    0x010
#define PORT_STATUS_REG                     0x018
#define SERIAL_PARAMETERS_REG               0x020
#define HASH_TABLE_POINTER_REG              0x028
#define FLOW_CONTROL_SOURCE_ADDRESS_LOW     0x030
#define FLOW_CONTROL_SOURCE_ADDRESS_HIGH	0x038
#define SDMA_CONFIGURATION_REG              0x040
#define SDMA_COMMAND_REG                    0x048
#define INTERRUPT_CAUSE_REG                 0x050
#define INTERRUPT_MASK_REG                  0x058
#define FIRST_RX_DESC_PTR0_OFFSET           0x080
#define FIRST_RX_DESC_PTR1_OFFSET           0x084
#define FIRST_RX_DESC_PTR2_OFFSET           0x088
#define FIRST_RX_DESC_PTR3_OFFSET           0x08c
#define CURRENT_RX_DESC_PTR0_OFFSET         0x0a0
#define CURRENT_RX_DESC_PTR1_OFFSET         0x0a4
#define CURRENT_RX_DESC_PTR2_OFFSET         0x0a8
#define CURRENT_RX_DESC_PTR3_OFFSET         0x0ac
#define CURRENT_TX_DESC_PTR0_OFFSET         0x0e0
#define CURRENT_TX_DESC_PTR1_OFFSET         0x0e4
#define MIB_COUNTER_BASE                    0x100 
#define MIB_COUNTER_END                     0x200

/* Port status register bits offset in register */
#define LINK_UP_BIT				(1<<3)

/* Port Config register bits offset in register */
#define	PROMISCUOUS_MODE_BIT	(1<<0)


/* typedefs */
/* board specific infomation */

	typedef struct
	{
		UINT32 vector ;				/* interrupt vector */
		UINT32 level ;				/* interrupt level */
		UINT32 baseAddr;			/* Port registers base offset */
		UCHAR  enetAddr[6];		 /* ethernet address. */
		int (*intEnable)(int unit);	 /* port specific interrupt enable routine */
		int (*intDisable)(int unit); /* port specific interrupt disable routine */
		int (*intAck) (int unit);	 /* interrupt ack */

		UINT32  portConfig;
		UINT32  portConfigExtend;
		UINT32  portCommand;
		UINT32  portSdmaConfig;
	} WANCOM_PORT_INFO;


#ifdef __cplusplus
}
#endif

#endif /* __INCwancomEndh */

