/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
*
* flashdrv.h - Header File for User Flash Memory Driver.
*
* DESCRIPTION:	
*     	      flashdrv.h  defines for flashdrv.c
*
* DEPENDENCIES:	
*       None.
*
******************************************************************************/

#ifndef _FLASHDRV_H_
#define _FLASHDRV_H_

/* Constants */
#define FLASH_SUCCESS 0x0
#define FLASH_TIMEOUT 0x100
#define FLASH_BANK0   0xA0000000
#define FLASH_BANK1   0xA2000000

/* Prototypes */
int   frcFlashAutoSelect(unsigned *Mnfct,unsigned *DevCode,unsigned int Flashbase);
int   frcFlashReadIdCommand ( unsigned int * , unsigned int * , unsigned int * , unsigned int );
void  frcFlashRead(UINT32 StartAdd, UINT32 noLongs, UINT32 *buffer);
short frcFlashErase (short sector,unsigned int Flashbase);
void  frcFlashReadRst (unsigned int);
int   frcFlashUnlock (unsigned int *);
int   frcFlashLock (unsigned int *);
int frcFlashBlockErase(unsigned int BlockNo,unsigned int Flashbase);
int frcFlashBlockWrite(unsigned int BlockNo,unsigned int Flashbase, unsigned int *buffer);
int frcFlashUnlockComplete  ();

#define FLASH_CMD_RESET       (unsigned short) 0xffff  /* Reset */
#define FLASH_CMD_CLEARSTATAW (unsigned short) 0x5050  /* Clear status reg */
#define FLASH_CMD_DATA02      (unsigned short) 0x0002  /* Device protect error*/
#define FLASH_CMD_DATA08      (unsigned short) 0x0008  /* Voltage range error*/	
#define FLASH_CMD_DATA10      (unsigned short) 0x0010  /* Programming error*/
#define FLASH_CMD_DATA20      (unsigned short) 0x0020
#define FLASH_CMD_DATA80      (unsigned short) 0x0080
#define FLASH_CMD_DATA90      (unsigned short) 0x0090  /* For autoselect */
#define FLASH_CMD_SETUPPROW   (unsigned short) 0x4040  /* Setup/ Program */
#define FLASH_CMD_READSTATW   (unsigned short) 0x7070  /* Read status reg */
#define FLASH_CMD_SETUPERAW   (unsigned short) 0x2020  /* Setup /Erase all 4 */
#define FLASH_CMD_ERASCONFALL (unsigned short) 0xD0D0  /* Erase/Verify */


#define FLASH_ADDRESS		USER_FLASH_ADR 	


/*
 * Support Flashes:
 *	Sharp LH28F320	2MB x 8, 128KB/block, 32 blocks 
 *	Intel     F320  32Mbit x 16, 128KB/block, 32 blocks
 *	Intel     F640  32Mbit x 16, 128KB/block, 32 blocks
 *	Intel     F320  64Mbit x 16, 128KB/block, 64 blocks
 *	Intel     F128  128Mbit x 16, 128KB/block, 128 blocks
 *
 *
 * This board can have 1, 2, or 4 devices. Each device is always 16 bits.
 * Therefore, the width of the data can be 16 bits, 32 bits, or 64 bits.
 * However, the bus width is always 64 bits, so the software does adjust.
 *
 */

#define FLASH_BLOCK_SZ_8BITS	(0x00010000 )        /* Bytes/8bit block(sector) */

#define FLASH_BLOCK_SZ_16BITS	(0x00010000 * 2)        /* Bytes/16bit block(sector) */
#define FLASH_BLOCKS_SHARP320	32			/* Blocks for Sharp 320 */
#define FLASH_BLOCKS_INTELF320	32			/* Blocks for Intel F320 */
#define FLASH_BLOCKS_INTELF640	64			/* Blocks for Intel F640 */

#define FLASH_BLOCKS_INTELF128	128			/* Blocks for Intel F128 */
#define FLASH_BLOCKS_INTELF256 256			/* Blocks for Intel F256 */
#define FLASH_BLOCKS_INTELP30 259	/* Number of blocks of size 128K(Actual no of blocks is
											    239(2*255 *128K +4*2 *32K = 64MB)) */

#ifdef ROHS

#define FLASH_MAX_BLOCK_SIZE	0x40000    /* Max bytes per one block (sector)*/
#else 
#define FLASH_MAX_BLOCK_SIZE	(FLASH_BLOCK_SZ_16BITS * 2) /* Max bytes per one block (sector)*/
#endif

#define FLASH_INC_ADDR           ( FLASH_MAX_BLOCK_SIZE ) /* To next Flash block */  

/* General purpose macro */

#ifndef EIEIO_SYNC
    #define EIEIO_SYNC __asm__("eieio;sync");
#endif


/* Macros to write to & read from 64-bit Flash memory */ 
/*
__asm__ ("
.align 2
.global flashAdr
flashAdr:
	.long0
.align 2
.global flashDat
flashDat:
	.long0
")

#define WRITE_FLASH 18

#define READ_FLASH 19
*/

#endif /* _FLASHDRV_H_ */

