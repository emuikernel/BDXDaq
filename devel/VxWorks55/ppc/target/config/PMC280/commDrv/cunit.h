/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* cunit.h - Header File for : C UNIT
*
* DESCRIPTION:
*		This file include function declaration and macros for the C Unit.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCcunith
#define __INCcunith

/* includes */

#include "gtCore.h"
#include "mpsc.h"


#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* defines  */

/* CUNIT Arbiter Control Register bits */
#define CACR_GPP_INT_LEVEL		BIT10

/* CUNIT Configuration Register bits */
#define CCR_DESC_END_NO_SWAP	BIT31

/* CUNIT Crossbar Time out Register bits */
#define CCTR_DEFAULT_TIMEOUT	0x000000FF
#define CCTR_TIMEOUT_DISABLE	BIT16

/* CUNIT Base Address Register bits */
#define CBAR_TARGET_DRAM					0x00000000
#define CBAR_TARGET_DEVICE					0x00000001
#define CBAR_TARGET_CBS						0x00000002
#define CBAR_TARGET_PCI0					0x00000003
#define CBAR_TARGET_PCI1					0x00000004
	
/* Window attributes */
#define CBAR_ATTR_DRAM_CS0					0x00000E00
#define CBAR_ATTR_DRAM_CS1					0x00000D00
#define CBAR_ATTR_DRAM_CS2					0x00000B00
#define CBAR_ATTR_DRAM_CS3					0x00000700

/* DRAM Target interface */
#define CBAR_ATTR_DRAM_NO_CACHE_COHERENCY	0x00000000
#define CBAR_ATTR_DRAM_CACHE_COHERENCY_WT	0x00001000
#define CBAR_ATTR_DRAM_CACHE_COHERENCY_WB	0x00002000

/* Device Bus Target interface */
#define CBAR_ATTR_DEVICE_DEVCS0				0x00001E00
#define CBAR_ATTR_DEVICE_DEVCS1				0x00001D00
#define CBAR_ATTR_DEVICE_DEVCS2				0x00001B00
#define CBAR_ATTR_DEVICE_DEVCS3				0x00001700
#define CBAR_ATTR_DEVICE_BOOTCS3			0x00000F00

/* PCI Target interface */
#define CBAR_ATTR_PCI_BYTE_SWAP				0x00000000
#define CBAR_ATTR_PCI_NO_SWAP				0x00000100
#define CBAR_ATTR_PCI_BYTE_WORD_SWAP		0x00000200
#define CBAR_ATTR_PCI_WORD_SWAP				0x00000300
#define CBAR_ATTR_PCI_NO_SNOOP_NOT_ASSERT	0x00000000
#define CBAR_ATTR_PCI_NO_SNOOP_ASSERT		0x00000400
#define CBAR_ATTR_PCI_IO_SPACE				0x00000000
#define CBAR_ATTR_PCI_MEMORY_SPACE			0x00000800
#define CBAR_ATTR_PCI_REQ64_FORCE			0x00000000
#define CBAR_ATTR_PCI_REQ64_SIZE			0x00001000

/* CPU 60x bus or internal SRAM interface */
#define CBAR_ATTR_CBS_SRAM					0x00000000
#define CBAR_ATTR_CBS_CPU_BUS				0x00000800

/* Window access control */
#define WIN_ACCESS_NOT_ALLOWED	0
#define WIN_ACCESS_READ_ONLY    BIT0
#define WIN_ACCESS_FULL         (BIT1 | BIT0)
#define WIN0_ACCESS_MASK		0x0003
#define WIN1_ACCESS_MASK		0x000C
#define WIN2_ACCESS_MASK		0x0030
#define WIN3_ACCESS_MASK		0x00C0


/* typedefs */

typedef enum _cunitAddrWin 
{
	CUNIT_WIN0, 
	CUNIT_WIN1, 
	CUNIT_WIN2, 
	CUNIT_WIN3, 
} CUNIT_ADDR_WIN;

typedef enum _cunitTarget
{
	TARGET_DRAM  ,
	TARGET_DEVICE,
	TARGET_CBS   ,
	TARGET_PCI0  ,
	TARGET_PCI1  
}CUNIT_TARGET;

typedef struct _cunitWinParam
{
    CUNIT_ADDR_WIN 	win;		/* Window number. See CUNIT_ADDR_WIN enum   */
    CUNIT_TARGET 	target;     /* System targets. See CUNIT_TARGET enum	*/
    unsigned short 	attributes;	/* BAR attributes. See above macros.		*/
    unsigned int 	baseAddr;	/* Window base address in unsigned int form */
    unsigned int 	highAddr;   /* Window high address in unsigned int form */
    unsigned int 	size; 		/* Size in MBytes. Must be % 64Kbyte. 		*/
    bool 			enable;     /* Enable/disable access to the window.     */
    unsigned short 	accessCtrl; /* Access ctrl register. see above macros 	*/
} CUNIT_WIN_PARAM;

/* cunit.h API list */
void cunitInit 			  ();
void cunitSetAccessControl(MPSC_NUM mpscNum, CUNIT_WIN_PARAM *param);


#ifdef __cplusplus
}
#endif

#endif /* __INCcunith */

