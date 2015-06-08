/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  INC - ECC OR ANY THIRD PARTY. MOTOROLA  INC - ECC RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* bFlashDrv.h - Header File for Boot Flash (AMD AM29LV008) Memory Driver.     *
*                                                                             *
* DESCRIPTION:                                                                *
*     This is a simple driver that allows the user to erase and program the   *
*     on-board boot flash AT49BV008AT.                                        *
*                                                                             *
* DEPENDENCIES:                                                               *
*       None.                                                                 *
*                                                                             *
******************************************************************************/


#ifndef _BFLASHDRV_H_
#define _BFLASHDRV_H_

/* Constants */
#define BFLASH_ADDRESS 0xFF800000
#define BFLASH_8BTOP_LASTSECADR (BFLASH_ADDRESS + 0x000FC000)
#define BFLASH_8BBOT_LASTSECADR (BFLASH_ADDRESS + 0x000F0000)

#define NVRAM_SIZE   0x2000
#define AMD_29LV008B_TOP         0x013e
#define AMD_29LV008B_BOTTOM      0x0137
#define S29AL_016D_TOP  0x01c4
#define S29AL_016D_BOTTOM  0x0149


#define SIZE_16K 0x4000
#define SIZE_64K 0x10000


#define AMD_29LV008_NUM_SECTORS	19
#define S_29LV008B_NUM_SECTORS 35

/* #define ERROR	-1 
#define OK	 0  */

/* Prototypes */

void bootFlashAutoSelect (unsigned int *Mnfct, unsigned int *DevCode);

short frcBootFlashProgram (volatile unsigned char *Address, unsigned char *data,
		   unsigned int size);

short frcBootFlashVerify (volatile unsigned char *Address, unsigned char *data,
		   unsigned int size);


short frcBootFlashSectorErase (volatile unsigned char *addr);

void frcbootFlashReadRst (unsigned long);

#endif /* _BFLASHDRV_H_ */

