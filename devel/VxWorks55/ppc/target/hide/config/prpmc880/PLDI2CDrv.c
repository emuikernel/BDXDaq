/* PLDI2CDrv.c - Low Level PLD I2C Routines */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon created.
*/

/* 
DESCRIPTION
This file contains the low-level routines which generate the following 
cycles on the I2C bus, via the PLD system register:

.CS
  Start
  Stop
  Read
  Write
.CE

*/

/* includes */
#include "vxWorks.h"
#include "config.h"
#include "PLDI2CDrv.h"
/* defines  */

/* typedefs */

/* locals   */

LOCAL void OutStartBit(void);
LOCAL void OutStoptBit(void);
LOCAL void I2CDelay(void);
LOCAL void ShiftOut(char OutChar, int OutNum);
LOCAL void ShiftIn(char *InChar);
LOCAL BOOL WaitAck(void);
LOCAL void OutPutAck(void);

/* extern */
void    sysUsDelay (UINT);
extern char ReadChar(char DevOffset);

/*******************************************************************************
*
* OutStartBit -output start bit
*
* This routine out a start bit from PLD I2C port
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void OutStartBit(void)
{
	char *SDA_RegPtr = NULL;
	char  RegValue;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	RegValue = *(char*)SDA_RegPtr;
	RegValue &= ~SDA_CLK_BIT;
	I2CDelay();
	*SDA_RegPtr = SDA_DIR_BIT | SDA_OUTPUT_BIT;
	I2CDelay();
	*SDA_RegPtr = SDA_DIR_BIT | SDA_OUTPUT_BIT |SDA_CLK_BIT;
	I2CDelay();
	*SDA_RegPtr = SDA_DIR_BIT | SDA_CLK_BIT;   /* High -> Low */
	I2CDelay();
	*SDA_RegPtr = SDA_DIR_BIT;
}

/*******************************************************************************
*
* OutStoptBit -output stop bit
*
* This routine out a stop bit from PLD I2C port
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void OutStoptBit(void)
{
	char *SDA_RegPtr = NULL;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	*SDA_RegPtr = SDA_DIR_BIT | SDA_CLK_BIT;   
	I2CDelay();
	*SDA_RegPtr = SDA_DIR_BIT | SDA_OUTPUT_BIT | SDA_CLK_BIT; /* low -> high */
	I2CDelay();
}

/*******************************************************************************
*
* ShiftOut -send out bits in I2C bus
*
* This routine send out OutChar's several bits in I2C bus
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ShiftOut(char OutChar, int OutNum)
{
	char *SDA_RegPtr = NULL;
	char ucOutBit, ucSDA_Reg_Data;
	int ucLoop = 0;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	*SDA_RegPtr = SDA_DIR_BIT;   
	for  (ucLoop = 0; ucLoop < OutNum; ucLoop++)
	{
		ucSDA_Reg_Data = *SDA_RegPtr;   
		ucSDA_Reg_Data &= (~SDA_CLK_BIT);
		*SDA_RegPtr = ucSDA_Reg_Data;
		I2CDelay();
		ucOutBit = (OutChar >> (7 - ucLoop)) & SDA_OUTPUT_BIT;
		*SDA_RegPtr = SDA_DIR_BIT | ucOutBit;   
		I2CDelay();
		*SDA_RegPtr = SDA_DIR_BIT | ucOutBit | SDA_CLK_BIT;   
		I2CDelay();
	}
}

/*******************************************************************************
*
* ShiftIn -read bits
*
* This routine receive one char from I2C bus
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ShiftIn(char *InChar)
{
	char *SDA_RegPtr = NULL;
	char ucInBit = 0, ucSDA_Reg_Data;
	int ucLoop = 0;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	ucSDA_Reg_Data = *SDA_RegPtr;
	ucSDA_Reg_Data &= ~SDA_DIR_BIT;
	*SDA_RegPtr = ucSDA_Reg_Data;   
	for  (ucLoop = 0; ucLoop < 8; ucLoop++)
	{
		ucSDA_Reg_Data = *SDA_RegPtr;   
		ucSDA_Reg_Data &= (~SDA_CLK_BIT);
		*SDA_RegPtr = ucSDA_Reg_Data;
		I2CDelay();
		ucSDA_Reg_Data |= (SDA_CLK_BIT);
		*SDA_RegPtr = ucSDA_Reg_Data;
		I2CDelay();
		ucInBit = ((ucInBit << 1) | (((*SDA_RegPtr) & SDA_INPUT_BIT) >> SDA_INPUTSHIFT_NUM)) ;
	}
	*InChar = ucInBit;
}

/*******************************************************************************
*
* WaitAck -wait acknowledge 
*
* This routine wait for acknowledge 
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL BOOL WaitAck(void)
{
	char *SDA_RegPtr = NULL;
	char ucSDA_Reg_Data;
	int ucLoop = 0;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	ucSDA_Reg_Data = *SDA_RegPtr;   
	ucSDA_Reg_Data &= (~SDA_CLK_BIT);
	ucSDA_Reg_Data &= (~SDA_DIR_BIT);
	*SDA_RegPtr = ucSDA_Reg_Data;
	I2CDelay();
	do
	{
		ucLoop++;
		ucSDA_Reg_Data |= SDA_CLK_BIT;
		*SDA_RegPtr = ucSDA_Reg_Data;
		I2CDelay();
		ucSDA_Reg_Data = *SDA_RegPtr;   
	}while((ucSDA_Reg_Data & SDA_INPUT_BIT) && (ucLoop < MAX_ACK_WAIT_TIME));
	if (ucSDA_Reg_Data & SDA_INPUT_BIT) return FALSE;
	else return TRUE;
}

/*******************************************************************************
*
* WaitAck -output acknowledge bit
*
* This routine output acknowledge 
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void OutPutAck(void)
{
	char *SDA_RegPtr = NULL;
	char ucSDA_Reg_Data;

	SDA_RegPtr = (char *)I2C_REG_ADDR;
	ucSDA_Reg_Data = *SDA_RegPtr;   
	ucSDA_Reg_Data &= (~SDA_CLK_BIT);
	*SDA_RegPtr = ucSDA_Reg_Data;
	I2CDelay();
	ucSDA_Reg_Data |= (SDA_DIR_BIT);
	ucSDA_Reg_Data &= (~SDA_OUTPUT_BIT);
	*SDA_RegPtr = ucSDA_Reg_Data;
	ucSDA_Reg_Data |= SDA_CLK_BIT;
	*SDA_RegPtr = ucSDA_Reg_Data;
	I2CDelay();
	ucSDA_Reg_Data &= (~SDA_CLK_BIT);
	*SDA_RegPtr = ucSDA_Reg_Data;
}

/*******************************************************************************
*
* PLDI2CRead - read a single byte to an i2c device 
*
* This function reads one or two 8-bit words from an I2C device specified by 
* the passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS PLDI2CRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to read */
    UCHAR  *pBfr      /* pointer to data byte */
    )
{
	WaitAck();
	OutStartBit();
	ShiftOut(devAddr,8);  /* send out Device address */
	WaitAck();
	ShiftOut((devOffset | 0x01),8);  /* send out address */
	WaitAck();
	OutStartBit();
	ShiftOut((devAddr | 0x01),8);  /* send out address */
	WaitAck();
	ShiftIn(pBfr);
	OutStoptBit();
	ShiftOut(0x00,8);  /* send out address */
	WaitAck();
	return (OK);
}

/******************************************************************************
*
* PLDI2CWrite - write a single byte to an i2c device 
*
* This function writes one or two bytes to an I2C device specified by the 
* passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS PLDI2CWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to write */
    UCHAR  *pBfr      /* pointer to data byte */
    )
{
	
	WaitAck();
	OutStartBit();
	ShiftOut(devAddr,8);  /* send out Device address */
	WaitAck();
	ShiftOut((devOffset & 0xfe),8);  /* send out address */
	WaitAck();
	ShiftOut(*pBfr,8);  /* send out address */
	WaitAck();
	OutStoptBit();
       return (OK);
}

/******************************************************************************
*
* PLDI2CBurstWrite - write bytes to an i2c device in burst mode
*
* This function writes bytes to an I2C device specified by the 
* passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS PLDI2CBurstWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR  *pBfr     /* pointer to data byte */
    )
{
	char ucLoop;
	
	WaitAck();
	OutStartBit();
	ShiftOut(devAddr,8);  /* send out Device address */
	WaitAck();
	ShiftOut(0xBE,8);  /* send out address */
	WaitAck();
	for (ucLoop = 0; ucLoop < 8; ucLoop++)
	{
		ShiftOut(*(pBfr+ucLoop),8);  /* send out address */
		WaitAck();
	}
	OutStoptBit();
	ShiftOut(0x00,8);  /* send out address */
	WaitAck();
	I2CDelay();
	return (OK);
}

/*******************************************************************************
*
* PLDI2CBurstRead - read bytes from an i2c device 
*
* This function reads 8-bit bytes from an I2C device specified by 
* the passed in parameters. 
*
* RETURNS: OK, or ERROR if transfer failed.
*/

STATUS PLDI2CBurstRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR  *pBfr      /* pointer to data byte */
    )
{
	char ucLoop;
	WaitAck();
	OutStartBit();
	ShiftOut(devAddr,8);  /* send out Device address */
	WaitAck();
	ShiftOut(0xBF,8);  /* send out address */
	WaitAck();
	OutStartBit();
	ShiftOut((devAddr | 0x01),8);  /* send out Device address */
	WaitAck();
	for (ucLoop = 0; ucLoop < 7; ucLoop++)
	{
		ShiftIn(pBfr+ucLoop);
		OutPutAck();
	}
	ShiftIn(pBfr+7);
	OutStoptBit();
	ShiftOut(0x00,8);  /* send out address */
	WaitAck();
	I2CDelay();
       return (OK);
}

/*******************************************************************************
*
* I2CDelay -delay some times depend on the I2C rate setting
*
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void I2CDelay(void)
{
    sysUsDelay (10);
}

