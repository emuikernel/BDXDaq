/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* Filename    :                                                *
*                                                                             *
* DESCRIPTION :	                                                              *
*       Configuration information related to tests is defined here.           *
*                                                                             *
* DEPENDENCIES:	                                                              *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#ifndef __FRC_TESTCONFIG_H
#define __FRC_TESTCONFIG_H

#include <vxWorks.h>
#include <stdio.h>

#define TEST_SUCCESS 0
#define TEST_ERROR   -1

#define SECSIZE 0x40000

#endif /*__FRC_TESTCONFIG_H*/


/********************Macros to enable/disabletests****************************/
/* #define TEST_DEBUG*/

#ifdef PMC280_VARIANT_B
#define BIG_ENDIAN_SUPPORT
#endif
#if 0
#ifndef PMC280_PCI_BOOT
#define BOOT_FLASH_TEST
#endif
#endif
#define BOOT_FLASH_TEST /*Irrespective of PCI boot, boot flash test is included*/
#define DISP_FREQ_TEST
#define SERIAL_PORT_TEST
#define ECC_TEST
#define IDMA_SCRUB_TEST

#define MEMORY_PERF_TEST
#define MEM_PERF_TEST

#define LMBENCH_TEST
#define PCI_TEST
/*#define PCI_PERF_TEST*/
#define ETHERNET_TEST
#define SUB_ID_VEND_ID_TEST
#define CFG_LAN_TEST
#define DISP_LAN_TEST
#define REG_PARSE_TEST
#undef DISABLE_PCI_INTR_MONARCH

/*#define DMA_STRESS*/
#undef AUX_CLK_TEST
#define I2C_EEPROM_TEST
#define RTC_TEST
#define BIB_TEST
#undef TAU_TEST
#define BAUD_TEST
#define MONARCH_TEST
#define MAC_TEST
#define DMA_TEST
#define INT_LATENCY_TEST
#define USER_FLASH_TEST 
#define WATCHDOG_TEST

#ifdef PMC280_DUAL_CPU
#define DUAL_CPU_TEST
#define MV_SEMAPHORE_TEST
#define WHO_AMI_TEST
#endif

#define FRC_TEST_ETH
#undef FRC_TEST_ETH_DBG


STATUS isPMC280TwoOnBoardEth(void );
STATUS isPMC280BootFlashPresent(void );
STATUS isPMC280UsrFlashPresent(void );
STATUS isPMC280DualCPU(void );
STATUS isPMC280Monarch (void);

