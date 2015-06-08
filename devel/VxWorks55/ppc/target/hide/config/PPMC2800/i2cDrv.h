
/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (MVL) AND GALILEO TECHNOLOGY, INC. (MVI).    *
********************************************************************************
* mvI2c.h - I2C unit header file 
*
* DESCRIPTION:
*       Function to control the I2C unit.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/


#ifndef __INCmvI2ch 
#define __INCmvI2ch

/* includes */
#include <stdlib.h>
#include "./coreDrv/gtCore.h"  
#include "./coreDrv/gtCntmr.h" 

#ifdef __cplusplus
extern "C" { 
#endif

/*#define EEPROM1  0xa4
#define EEPROM2  0xa6
#define BIBEEPROM 0xa8*/

#define EEPROM1  0x52
#define EEPROM2  0x53
#define BIBEEPROM 0x54

/* defines */
#define I2C_TIMEOUT_VALUE               0x4000000 /* 2 Seconds at 500 Mhz */
#define I2C_ENABLE                      BIT6
#define I2C_INT_ENABLE                  BIT7
#define I2C_ACK                         BIT2
#define I2C_INT_FLAG                    BIT3
#define I2C_STOP_BIT                    BIT4
#define I2C_START_BIT                   BIT5 
#define I2C_READ                        BIT0
#define I2C_EEPROM_DELAY                10 /* Mili sec */
#define I2C_10BIT_ADDR                  BIT31

/* Error codes */
#define I2C_TIME_OUT_ERROR              0xFF
#define I2C_NO_DEVICE_WITH_SUCH_ADDR    0x01
#define I2C_GENERAL_ERROR               0x02
#define I2C_NO_ERROR                    0x03
#define I2C_INT_FLAG_STUCK_AT_0         0x04

#define I2C_SLAVE_ADDRESS                                   0xc000
#define I2C_EXTENDED_SLAVE_ADDRESS                          0xc010

/* I2C status codes */
/*  ShomvRtcuts-
    RECEIVED    -> REC
    TRANSMITED  -> TRA
    MASTER      -> MAS
    SLAVE       -> SLA
    ACKNOWLEDGE -> ACK
    ARBITRATION -> ARB
    ADDR        -> ADDR
*/

#define I2C_BUS_ERROR                                                       0X00
#define I2C_START_CONDITION_TRA                                             0X08
#define I2C_REPEATED_START_CONDITION_TRA                                    0X10
#define I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC                                 0X18
#define I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC                             0X20
#define I2C_MAS_TRAN_DATA_BYTE_ACK_REC                                      0X28
#define I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC                                  0X30
#define I2C_MAS_LOST_ARB_DURING_ADDR_OR_DATA_TRA                            0X38
#define I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC                                  0X40
#define I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC                              0X48
#define I2C_MAS_REC_READ_DATA_ACK_TRA                                       0X50
#define I2C_MAS_REC_READ_DATA_ACK_NOT_TRA                                   0X58
#define I2C_SLA_REC_ADDR_PLUS_WRITE_BIT_ACK_TRA                             0X60
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_ADDR_IS_TARGETED_TO_SLA_ACK_TRA_W  0X68
#define I2C_GENERAL_CALL_REC_ACK_TRA                                        0X70
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_GENERAL_CALL_ADDR_REC_ACK_TRA      0X78
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_TRAN                  0X80
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_NOT_TRAN              0X88
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_TRAN              0X90
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_NOT_TRAN          0X98
#define I2C_SLA_REC_STOP_OR_REPEATED_START_CONDITION                        0XA0
#define I2C_SLA_REC_ADDR_PLUS_READ_BIT_ACK_TRA                              0XA8
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_ADDR_IS_TARGETED_TO_SLA_ACK_TRA_R  0XB0
#define I2C_SLA_TRA_READ_DATA_ACK_REC                                       0XB8
#define I2C_SLA_TRA_READ_DATA_ACK_NOT_REC                                   0XC0
#define I2C_SLA_TRA_LAST_READ_DATA_ACK_REC                                  0XC8
#define I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC                          0XD0
#define I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC                      0XD8
#define I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_REC                           0XE0
#define I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC                       0XE8
#define I2C_NO_RELEVANT_STATUS_INTERRUPT_FLAG_IS_KEPT_0                     0XF8

/* typedefs */
typedef enum _i2cfreq      {_100KHZ = 100000,_400KHZ = 400000} I2C_FREQ;
typedef enum _i2cAddrRange {_256BYTES, MORE_THAN_256BYTES    } I2C_ADDR_RANGE;
typedef enum _i2cGenaralCallStatus {I2C_GENERAL_CALL_DISABLE,
                                    I2C_GENERAL_CALL_ENABLE
                                   } I2C_GENERAL_CALL_STATUS;

typedef struct _i2cDataStruct
{
    unsigned int data; /* For read transaction, data is valid only if errorCode
                          = I2C_NO_ERROR, For write transaction, it is a
                          guarantee for a successful operation */
    unsigned int errorCode;
    unsigned int status; /*I2C's status register when updating the error code*/
} I2C_DATA_STRUCT;

unsigned int    mvI2cMasterInit(I2C_FREQ freq,unsigned int tclk);
void            mvI2cSlaveInit (unsigned int deviceAddress,
                                I2C_GENERAL_CALL_STATUS generalCallStatus);
I2C_DATA_STRUCT mvI2cGenerateStartBit(void);
void            mvI2cGenerateStopBit (void);
void            mvI2cIntEnable       (void);
void            mvI2cIntDisable      (void);
void            mvI2cSetAckBit       (void);
unsigned int    mvI2cReadStatus      (void);
unsigned int    mvI2cReadIntFlag     (void);
void            mvI2cClearIntFlag    (void);
void            mvI2cReset           (void);
void            mvI2cDelay       (unsigned int miliSec,
                                  CNTMR_NUM countNum);

void            mvI2cMasterWrite (unsigned int deviceAddress,
                                  I2C_DATA_STRUCT *pI2cData);

I2C_DATA_STRUCT mvI2cMasterRead  (unsigned int deviceAddress);

void            mvI2cMasterWriteBlock(unsigned short  deviceAddress,                 
                                      unsigned char  *pI2cData,                    
                                      unsigned int    i2cDataLen,                  
                                      bool            generateStopBit,
                                      unsigned int   *pI2cStatus);

void            mvI2cMasterReadBlock (unsigned short deviceAddress,
                                      unsigned char  *pI2cData,                    
                                      unsigned int    i2cDataLen,                  
                                      bool            generateStopBit,
                                      unsigned int   *pI2cStatus);

I2C_DATA_STRUCT mvI2cMasterEEPROMread (unsigned int    deviceAddress,
                                       unsigned int    address,
                                       I2C_ADDR_RANGE  addrRange);
void            mvI2cMasterEEPROMwrite(unsigned int    deviceAddress,
                                       I2C_DATA_STRUCT *pI2cData,
                                       unsigned int    address,
                                       I2C_ADDR_RANGE  addrRange,
                                       CNTMR_NUM counterNum);
I2C_DATA_STRUCT mvI2cMasterEEPROMtransmitAddress(unsigned int deviceAddress,
                                                 unsigned int address,
                                                 I2C_ADDR_RANGE addrRange);
void mvI2cMasterEEPROMwriteWord(unsigned int    deviceAddress,
                                I2C_DATA_STRUCT *pI2cData,
                                unsigned int    address,
                                I2C_ADDR_RANGE  addrRange,
                                CNTMR_NUM       counterNum);
I2C_DATA_STRUCT mvI2cMasterEEPROMreadWord(unsigned int deviceAddress,
                                          unsigned int address,
                                          I2C_ADDR_RANGE addrRange);





int parse8(void);
int parse16(void);


void testEEPROMwork();

bool frcEEPROMWrite8 (unsigned int, unsigned int, int, UINT8 *);
bool mvEEPROMRTCWrite (unsigned int ,unsigned int ,int ,UINT8 *);
bool frcEEPROMWrite16 (unsigned int ,unsigned int ,int , UINT8 *);
bool frcEEPROMRead8 (UINT8 ,UINT16, int ,UINT8 *);
bool frcEEPROMRead16 (UINT8 ,UINT16,int ,UINT8 *);
void mvRTCRead(UINT8 *);
void mvRTCWrite(UINT8 *);
void setI2cReady();

void setI2cBusy();



#ifdef __cplusplus
}
#endif

#endif /* __INCmvI2ch */

