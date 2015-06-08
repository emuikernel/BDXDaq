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
* i2cDrv.h - Header file for the i2cDrv.c .
*
* DESCRIPTION:
*       i2cDrv.h defines for i2cDrv.c
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/


#ifndef _I2CDRV_H_
#define _I2CDRV_H_


/* includes */

#include "./coreDrv/gtCore.h"


/* defines */
#define EEPROM1  0xa4
#define EEPROM2  0xa6
#define BIBEEPROM 0xa8

#define I2CBUS_ERROR							0x00
#define I2C_START_TRA							0x08
#define I2C_REP_START_TRA						0x08
#define ADDR_WB_TRA_ACK							0x18
#define ADDR_WB_TRA_NACK						0x20
#define MASTER_DATA_TRA_ACK						0x28
#define MASTER_DATA_TRA_NACK						0x30 
#define MASTER_LOST_ARBTR_DURING_ADR_DATA				0x38
#define ADDR_RB_TRA_ACK							0x40
#define ADDR_RB_TRA_NACK						0x48
#define MASTER_READ_ACK_TRA						0x50
#define MASTER_READ_NACK_TRA						0x58
#define SLAVE_RECD_ADDR_ACK						0x60
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_SLAVEW_ACK_TRA			0x68
#define GEN_CALL_RECD_ACK_TRA						0x70
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_GCA_RECD_ACK_TRA		0x78
#define SLAVE_RECD_WRT_DATA_AFTER_ADR_ACK_TRA				0x80
#define SLAVE_RECD_WRT_DATA_AFTER_ADR_ACK_NTRA				0x88
#define SLAVE_RECD_WRT_DATA_AFTER_GC_ACK_TRA				0x90
#define SLAVE_RECD_WRT_DATA_AFTER_GC_ACK_NTRA				0x98
#define SLAVE_RECD_STOP_REP_START					0xA0
#define SLAVE_RECD_ADDR_RB_ACK_TRA					0xA8
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_SLAVER_ACK_TRA			0xB0
#define SLAVE_TRA_READ_DATA_ACK_RECD					0xB8
#define SLAVE_TRA_READ_DATA_ACK_NOT_RECD				0xC0
#define	SLAVE_TRA_LOST_READ_BYTE_ACK_RECD				0xC8


/* Prototypes */
bool frcEEPROMWrite16(UINT8, UINT16,unsigned int, UINT8 *);
bool frcEEPROMRead16(UINT8, UINT16, unsigned int, UINT8 *);
bool frcEEPROMWrite8(UINT8, UINT8,unsigned int, UINT8 *);
bool frcEEPROMRead8(UINT8, UINT8, unsigned int, UINT8 *);
void frcEEPROMTestRead(UINT8);
void frcEEPROMTestWrite(UINT8);
void frcI2cReset(void);
bool frcI2CWrite(UINT8,UINT8*, unsigned int);
bool frcI2CRead(UINT8, UINT8* , unsigned int );
bool frcEEPROMDummyWrite16(UINT8 , UINT16);
bool frcEEPROMDummyWrite(UINT8 , unsigned int);
bool frcRTCDummyWrite(UINT8 , unsigned int);
void frcRTCRead(UINT8 * ); 
void frcRTCWrite(UINT8 * );
bool frcMACWrite(UINT8,UINT8,UINT8,UINT8,UINT8,UINT8,UINT8);
bool frcMACRead(UINT8, UINT8 *);
 
#endif /* _I2CDRV_H_ */

