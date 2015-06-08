
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
* mvI2c.c - I2C unit c file. 
*
* DESCRIPTION
*       Functions to control the I2C.
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/*includes*/
#include "stdio.h"
#include "i2cDrv.h"
#include "logLib.h"
#include "gt64360r.h"

#ifdef __cplusplus
extern "C" { 
#endif

unsigned int mvI2cTclock;
bool isIntFlagSet(void);
void clearIntFlag(void);
IMPORT void    sysMsDelay      (UINT );

bool protectMAC = true; /* true - protect flase - allow write */
bool protectBIB = true; /* true - protect flase - allow write */
bool protectPCI = true; /* true - protect flase - allow write */

/*******************************************************************************
* mvI2cSlaveInit - Initializes the I2C slave address and the general call enable
*                  register.
*
* DESCRIPTION:
*       This function initialize the MV as a slave, with the given address. 
*       If the desired device address is a 10 bit address, add (logical OR)
*       to the deviceAddress parameter the value I2C_10BIT_ADDR defined in 
*       mvI2c.h. The parameter generalCallStatus (defind in mvI2c.h) indicates
*       if the slave will answer general call or not. If set to 1 the I2C slave 
*       interface responds to general call accesses. if set to 0 the I2C slave 
*       interface will not responds to general call accesses.The general call, 
*       if enabled causes the MV (while acting as a slave) to respond to any 
*       access regardless of the targeted address.
*     NOTE:
*       before using this function you must call 'i2cMasterInit' function to 
*       initialize the I2C mechanism. 
*
* INPUT:
*       deviceAddress - the desired address to assigned to the MV slave address
*                       register.
*       generalCallStatus - inidicates whether the slave will respond to general
*                           call or not (values defind in mvI2c.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cSlaveInit(unsigned int deviceAddress,I2C_GENERAL_CALL_STATUS 
                    generalCallStatus)
{
    unsigned int tempAddress; 

    /* 10 Bit deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR)
    {
        /* writing the 2 most significant bits of the 10 bit address*/
        GT_REG_WRITE(I2C_SLAVE_ADDRESS,(((deviceAddress & 0X300) >> 7)|0Xf0) |
                      generalCallStatus);
        /* writing the 8 least significant bits of the 10 bit address*/
        tempAddress = (deviceAddress & 0xff);  
        GT_REG_WRITE(I2C_EXTENDED_SLAVE_ADDRESS,tempAddress);
    }
    /*7 bit address*/
    else
    {
        GT_REG_WRITE(I2C_EXTENDED_SLAVE_ADDRESS,0x0);
        GT_REG_WRITE(I2C_SLAVE_ADDRESS,(deviceAddress<<1) | generalCallStatus);
    }
}

/*******************************************************************************
* mvI2cMasterInit - Initializes the I2C mechanism integrated in the MV.
*
* DESCRIPTION:
*       This function initialize the I2C mechanism and must be called before 
*       any attemp to use the I2C bus ( use this function on both master or 
*       slave mode to initialize the I2C mechanism).The function calculates the
*       parameters needed for the I2C's freuency registers, resets the I2C and 
*       then enables it. 
*
* INPUT:
*       i2cFreq - the desired frequency , values defind in mvI2c.h and can be 
*                 either _100KHZ or _400KHZ.
*       tclk    - The system's Tclock.
*
* OUTPUT:
*       I2C mechanism is enabled.
*       mvI2cTclock is set to tclk value.
*
* RETURN:
*       the actual frequancy calculated and assigned to the I2C baude - rate 
*       register (for more details please see the I2C section in the MV 
*       datasheet). 
*
*******************************************************************************/
unsigned int mvI2cMasterInit(I2C_FREQ i2cFreq,unsigned int tclk)
{
    unsigned int    n,m,freq,margin,minMargin = 0xffffffff,power;
    unsigned int    actualFreq = 0,actualN = 0,actualM = 0;
    
    for(n = 0 ; n < 8 ; n++)
    {
        for(m = 0 ; m < 16 ; m++)
        {
            power = 2<<n; /* power = 2^(n+1) */
            freq = tclk/(10*(m+1)*power);
            margin = i2cFreq - freq;
            if(margin < minMargin)
            {
                minMargin   = margin;
                actualFreq  = freq;
                actualN     = n;
                actualM     = m;
            }
        }
    }

	
    /* Reset the I2C logic */
    mvI2cReset();
	
    /* Set the baud-rate */
	printf (" value of the actualIM = %x",actualM);
    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,(actualM<<3) | actualN);
	
    /* Enable the I2C and slave */
    GT_REG_WRITE(I2C_CONTROL,I2C_ENABLE|I2C_ACK);  
	
    mvI2cTclock = tclk;
    return actualFreq;
}

/*******************************************************************************
* mvI2cIntEnable - Inables the I2C's interrupt generation.
*
* DESCRIPTION:
*      This function enables the MV interrupt mode. After calling  this function
*      an interrupt will be generated each time the interrupt flag is set . If 
*      any of the status codes other than 0xf8 are set , the I2C hardware sets 
*      the interrupt flag and ( if  interrupt  is enabled ) an interrupt will be
*      generated .When interrupt enable bit is cleared to zero, the interrupt 
*      line will always remain low.
*     NOTE:
*      It is highly recommended to use I2C interrupt to interface the I2C 
*      module, rather than using register polling method. 
*
* INPUT:
*      None.
*
* OUTPUT:
*      Set I2C_INT_ENABLE bit in I2C_CONTROL register.
*
* RETURN:
*      None.
*
*******************************************************************************/
void mvI2cIntEnable(void)
{
    unsigned int temp;
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_INT_ENABLE);
}

/*******************************************************************************
* mvI2cIntDisable - Disables the I2C's interrupt generation.
*
* DESCRIPTION:
*       This function disables the I2C's interrupt generation described in 
*       'mvI2cIntEnable'.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Unset I2C_INT_ENABLE bit in I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cIntDisable(void)
{
    unsigned int temp;
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & ~I2C_INT_ENABLE);
}
/*******************************************************************************
* mvI2cReadStatus - Reads the I2C's status register
*
* DESCRIPTION:
*       This function reads the status register and return the status code 
*       generated in the last operation.There are 31 aveliable codes (defind 
*       in mvI2c.h ) that can be assigned to the status register (also called 
*       baude - rate register) each time one of the statuses occur.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The status code.
*
*******************************************************************************/
unsigned int mvI2cReadStatus(void)
{
    unsigned int status;
    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
    return status;
}

/*******************************************************************************
* mvI2cReadIntFlag - Reads the I2C's interrupt flag status.
*
* DESCRIPTION:
*       This function reads the I2C's interrupt flag status .The interrupt flag
*       is assigned each time when one of the status codes other than 0xf8 are 
*       set.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 if the interrupt flag of the I2C is set
*       0 otherwise.
*
*******************************************************************************/
unsigned int mvI2cReadIntFlag(void)
{
    unsigned int temp;
    GT_REG_READ(I2C_CONTROL,&temp);
    if(temp & I2C_INT_FLAG)
        return 1;
    else
        return 0;
}

/*******************************************************************************
* mvI2cClearIntFlag - Clears the interrupt flag bit in the control register.
*
* DESCRIPTION:
*       This function clears the interrupt flag bit.The interrupt flag bit is 
*       set to '1' automatically when any of 30 of the possible 31 statuses is 
*       entered. The only status that does not set IFLG is status F8h. When 
*       interrupt flag is set , the low period of the I2C bus clock line (SCL) 
*       is stretched and the data transfer is suspended. After calling this 
*       function, zero is written to interrupt flag, the interrupt line goes 
*       low (in case interrupt is enable, see function 'i2cIntEnable') and the 
*       I2C clock line is released . It is necessary to use this function each 
*       time one of the possible 31 statuses is entered to release the bus and 
*       allow data transfer to continue. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       Clear bit I2C_INT_FLAG in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cClearIntFlag(void)
{
    unsigned int temp;
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & (~I2C_INT_FLAG));
}

/*******************************************************************************
* mvI2cReset - Reset the I2C's logic and set its registers values to their 
*              defaults.
*
* DESCRIPTION:
*       This function resets the I2C mechanism . After calling this function ,a
*       software reset is performed  by setting the soft reset register to 0,it
*       sets the I2C back to idle and the stop it.The start bit and interrupt
*       bits of the I2C control register will also be set to zero.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_SOFT_RESET register to 0x0.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cReset(void)
{

	GT_REG_WRITE(I2C_SOFT_RESET,0x0);
}
	
/*******************************************************************************
* mvI2cGenerateStartBit - Generate start bit to begin a transaction.
*
* DESCRIPTION:
*       This function (mostly used by other functions) initiates a start 
*       condition on the bus (to indicate a master transaction request) when the
*       bus is free or a repeated start condition, if the master already drives
*       the bus. After a start bit has been transmited successfuly the next 
*       action should be generating the target slave address with a read/write 
*       bit. Note that these action are already handeld by the upcoming 
*       functions.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_START_BIT in the I2C_CONTROL register.
*
* RETURN:
*       I2c data struct(defined in mvI2c.h) with status and error code.
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cGenerateStartBit(void)
{
    unsigned int    timeOut,status;
    I2C_DATA_STRUCT mvI2cData;
setI2cBusy();


    /* Generate the start bit */
    /*GT_REG_READ(I2C_CONTROL,&temp);*/
    /*GT_REG_WRITE(I2C_CONTROL,temp | I2C_START_BIT);*/ 
    
    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE , 0x2c); 
    GT_REG_WRITE(I2C_CONTROL , 0x60); 
    
    /* Wait for it to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);

        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_START_CONDITION_TRA:
        case I2C_REPEATED_START_CONDITION_TRA:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cData.data = 0xffffffff;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    mvI2cData.errorCode = I2C_NO_ERROR;
    mvI2cData.status = status;
    return mvI2cData;
}

/*******************************************************************************
* mvI2cGenerateStopBit - Generate's stop bit to end a transaction.
*
* DESCRIPTION:
*       This function (mostly used by other function) generate stop bit on the
*       bus to indicate end of  transaction by the master.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_STOP_BIT in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cGenerateStopBit(void)
{
    unsigned int    temp;
    /* Generate stop bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_STOP_BIT);
setI2cReady(); 

}
/*******************************************************************************
* mvI2cSetAckBit - Sets the acknoledge bit on.
*
* DESCRIPTION:
*       This function ( mostly used by other functions ) sets the acknowledge 
*       bit to 1. When set to 1,  the I2C master drives the acknowledge bit in 
*       response to received read data from the slave. If working in a slave 
*       mode , an acknowledge bit will be driven on the bus in response to 
*       received address or write data. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_ACK in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cSetAckBit(void)
{
    unsigned int    temp;
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_ACK);
}
/*******************************************************************************
* mvI2cMasterRead - Reads data from an I2C device.
*
* DESCRIPTION:
*       This function reads data from an I2C device with a 7 or 10 bit address.
*       For example, this function can be used to read data by one MV (master) 
*       from another MV (slave , on the same board or on differant boards) or 
*       read from any device (with the exception of EEPROM) that support I2C.
*       The data struct returnd (defind in mvI2c.h) contain the read data , 
*       status and error code incase of an error. 
*
* INPUT:
*       deviceAddress - 7 or 10 bit address of the target device. If the target 
*                       device has a 10 bit address, add to deviceAddress the 
*                       value I2C_10BIT_ADDR (defind in mvI2c.h).    
*
* OUTPUT:
*       None.
*
* RETURN:
*       I2C_DATA_STRUCT containing the read data error-code and status.
*      NOTE: 
*       Data is valid only if the errorCode field contains the value 
*       I2C_NO_ERROR ( defind in i2c.h ). 
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cMasterRead(unsigned int deviceAddress)
{
    unsigned int    temp,data,timeOut,status, intFlagTimeOut = 0;
    I2C_DATA_STRUCT mvI2cData;

    /* set the acknoledge bit on (incase it was turned off) */
    mvI2cSetAckBit();
    mvI2cData = mvI2cGenerateStartBit();
    if(mvI2cData.errorCode!=I2C_NO_ERROR)
    {
        return mvI2cData; 
    }
    /* Transmit the deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR) 
        /* 10 Bit deviceAddress */
    {
        /* Transmiting the 2 most significant bits of the 10 bit address  */
        GT_REG_WRITE( I2C_DATA,((((deviceAddress & 0X300)>>7)|0Xf0))|I2C_READ);

        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                mvI2cData.data = 0xffffffff;
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cGenerateStopBit();
                return mvI2cData;
            }
        }

        mvI2cClearIntFlag();
        /* Wait for the deviceAddress(1st byte) to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                mvI2cData.data = 0xffffffff;
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return mvI2cData;
            case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;   
                break;
            case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
                mvI2cData.data = 0xffffffff;
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return mvI2cData;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        }
        /* Transmiting the 8 least significant bits of the 10 bit address */
        GT_REG_WRITE(I2C_DATA,(deviceAddress & 0x000000ff));
    }
    else /* 7 Bit deviceAddress */
    {
        GT_REG_WRITE(I2C_DATA,(deviceAddress<<1) | I2C_READ);
    }
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }

    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            break;
        case I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    /* clear interupt bit and acknowledge bit */
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    GT_REG_READ(I2C_CONTROL,&temp);
    temp = temp & ~( I2C_ACK | I2C_INT_FLAG);
    GT_REG_WRITE(I2C_CONTROL,temp);    
    /* Wait for the data */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_MAS_REC_READ_DATA_ACK_NOT_TRA:
            GT_REG_READ(I2C_DATA,&data);
            mvI2cData.data = data;
            mvI2cData.status = status;
            timeOut = I2C_TIMEOUT_VALUE + 1;
            /* now generate stop bit */  
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }    
    mvI2cData.errorCode = I2C_NO_ERROR;
    mvI2cData.status = status;
    return mvI2cData;
}
/*******************************************************************************
* mvI2cMasterWrite - write 8-bit data to an I2c device.
*
* DESCRIPTION:
*       This function writes the data within the data struct ‘i2cData’ to an i2c 
*       slave with 7 or 10 bit address. For example, this function can be used 
*       for writing from one MV (master) to another MV (slave ,on the same board
*       or on differant board) or writing to any device (with the exception of 
*       an EEPROM) that support I2C .The data struct ‘i2cData’ deliverd is 
*       defind in mvI2c.h and contain the data to be written.
*
* INPUT:
*       deviceAddress - 7 or 10 bit address of the target device . If the device
*                       has a 10 bit address , add to deviceAddress the value 
*                       I2C_10BIT_ADDR (defind in mvI2c.h).    
*       *pI2cData     - struct containing the data to be written.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cMasterWrite(unsigned int deviceAddress, I2C_DATA_STRUCT *pI2cData)
{
    unsigned int    timeOut,status, intFlagTimeOut = 0;
    I2C_DATA_STRUCT I2cResult;

    mvI2cSetAckBit();
    I2cResult = mvI2cGenerateStartBit();
    if(I2cResult.errorCode!=I2C_NO_ERROR)
    {
        pI2cData->errorCode = I2cResult.errorCode;
        pI2cData->status = I2cResult.status;
        return; 
    }
    /* Transmit the deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR) 
        /* 10 Bit deviceAddress */
    {
        /* Transmiting the 2 most significant bits of the 10 bit address */
        GT_REG_WRITE( I2C_DATA,((((deviceAddress & 0X300)>>7)|0Xf0)));
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                pI2cData->data = 0xffffffff;
                pI2cData->errorCode = I2C_GENERAL_ERROR;
                pI2cData->status = status;
                mvI2cGenerateStopBit();
                return;
            }
        }
        mvI2cClearIntFlag();
        /* Wait for the deviceAddress(1st byte) to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                pI2cData->errorCode = I2C_GENERAL_ERROR;
                pI2cData->status = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return ;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;   
                break;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
                pI2cData->errorCode = I2C_GENERAL_ERROR;
                pI2cData->status = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            pI2cData->errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return ;
        }
        /* Transmiting the 8 least significant bits of the 10 bit address */
        GT_REG_WRITE(I2C_DATA,deviceAddress & 0x000000ff);
    }
    else /* 7 Bit deviceAddress */
    {
        GT_REG_WRITE(I2C_DATA,deviceAddress<<1);
    }
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            pI2cData->data = 0xffffffff;
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            return;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return ;
        case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            break;
        case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        pI2cData->errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
        pI2cData->status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return;
    }
    /* Write the data */
    GT_REG_WRITE(I2C_DATA,pI2cData->data);
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            pI2cData->data = 0xffffffff;
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            return;
        }
    }
    mvI2cClearIntFlag();
    /* wait for the data to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1; 
            /* Generate stop bit */
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        pI2cData->errorCode = I2C_TIME_OUT_ERROR;
        pI2cData->status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return;
    }
    pI2cData->errorCode = I2C_NO_ERROR;
    pI2cData->status = status;
    return;
}

/*******************************************************************************
* mvI2cMasterEEPROMtransmitAddress - Transmit the device address and offset 
*                                    address of an EEPROM device 
*
* DESCRIPTION:
*       This function transmit the EEPROM address on the bus .Whether a read 
*       transaction or write transactoin towards an EEPROM  is intended , this 
*       function transmit the target device address on the bus and return the 
*       result in a I2C_DATA_STRUCT .Only if the field 'errorCode' within the 
*       structure contain the value I2C_NO_ERROR  the read/write transaction can
*       be continued.
*
* INPUT:
*       deviceAdress - The EEPROM device address ( 7 bit only).
*       address      - The target offset within the EEPROM to be written or read.
*       addrRange    - The address range within the EEPROM ( values defined in 
*                      mvI2c.h ).
*
* OUTPUT:
*       None.
*
* RETURN:
*       I2C_DATA_STRUCT struct (defined in mvI2c.h) and contain the result.
*       any value other than I2C_NO_ERROR means that an error occured during 
*       the address transmition.
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cMasterEEPROMtransmitAddress(unsigned int deviceAddress,
                                                 unsigned int address,
                                                 I2C_ADDR_RANGE addrRange)
{    
    unsigned int    temp,timeOut,status = 0, intFlagTimeOut = 0;
    I2C_DATA_STRUCT mvI2cData;
    
    mvI2cSetAckBit();
     /* Clear the stop bit */
    GT_RESET_REG_BITS(I2C_CONTROL,I2C_STOP_BIT);       
    mvI2cData = mvI2cGenerateStartBit();
    if(mvI2cData.errorCode!=I2C_NO_ERROR)
    {
       /* logMsg("\nError Return  \n",1,2,3,4,5,6); 
        logMsg("\nError Code : %u\n",mvI2cData.errorCode,2,3,4,5,6);*/ 
        return mvI2cData; 
    }
    /* writing the address (7-bit)*/    
    GT_REG_WRITE(I2C_DATA,deviceAddress<<1);      
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
	    logMsg("\n TRANSMIT ADDRESS: Writing address timed out \n",1,2,3,4,5,6);
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_CONTROL,&temp);
        if((temp & I2C_INT_FLAG) == I2C_INT_FLAG)
            break;
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.errorCode = I2C_INT_FLAG_STUCK_AT_0;
        mvI2cData.status = status;
        mvI2cData.data = 0xffffffff;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
	logMsg("\n TRANSMIT ADDRESS: I2C timed out \n",1,2,3,4,5,6);
        return mvI2cData;
    }
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {   
	/*mvI2cDelay(I2C_EEPROM_DELAY,0x1);*/
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
	/*mvI2cDelay(I2C_EEPROM_DELAY,0x1);*/
        /*logMsg("\n The BAUD RATE STATUS IS : %d", status, 2,3,4,5,6);*/        
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\n TRANSMIT ADDRESS: Bus ERROR\n",1,2,3,4,5,6);
            return mvI2cData;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;	      	    
            break;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\n TRANSMIT ADDRESS: Address plus write bit transmitted but ACK not Recieved\n",1,2,3,4,5,6); 
            return mvI2cData; 
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
        mvI2cData.status = status;
        mvI2cData.data = 0xffffffff;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
	logMsg("\n TRANSMIT ADDRESS:I2C TIMED OUT2\n",1,2,3,4,5,6);
        return mvI2cData;
    }
    if(addrRange == MORE_THAN_256BYTES)
    {
        /* Write the 1st data - MSB of address */
        temp = (address>>8) & 0x000000ff;
        GT_REG_WRITE(I2C_DATA,temp);
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                mvI2cData.data = 0xffffffff;
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cGenerateStopBit();
		logMsg("\n MORE THAN 256 Bytes: Address transmit Time out\n",1,2,3,4,5,6);
                return mvI2cData;
            }
        }
        mvI2cClearIntFlag();
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_CONTROL,&temp);
            if((temp & I2C_INT_FLAG) == I2C_INT_FLAG)
                break;
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.errorCode = I2C_INT_FLAG_STUCK_AT_0;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\n MORE THAN 256 Bytes: I2C Address transmit Time out\n",1,2,3,4,5,6);
            return mvI2cData;
        }
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cData.data = 0xffffffff;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
		logMsg("\n MORE THAN 256 Bytes: Bus Error\n",1,2,3,4,5,6);
                return mvI2cData;
            case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;   
                break;
            case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
                mvI2cData.errorCode = I2C_GENERAL_ERROR;
                mvI2cData.status = status;
                mvI2cData.data = 0xffffffff;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
		logMsg("\n MORE THAN 256 Bytes: I2C Address ACK Not Recieved\n",1,2,3,4,5,6);
                return mvI2cData;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\n MORE THAN 256 Bytes:Time out2\n",1,2,3,4,5,6);
            return mvI2cData;
        }
    }
    /* Write the 2nd data (or 1st in case of 256 bytes chip)
       - LSB of Address */
    temp = address & 0x000000ff;
    GT_REG_WRITE(I2C_DATA,temp);
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
	    logMsg("\n Time out3\n",1,2,3,4,5,6);
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_CONTROL,&temp);
        if((temp & I2C_INT_FLAG) == I2C_INT_FLAG)
            break;
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.errorCode = I2C_INT_FLAG_STUCK_AT_0;
        mvI2cData.status = status;
        mvI2cData.data = 0xffffffff;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
	logMsg("\n Time out4\n",1,2,3,4,5,6);
        return mvI2cData;
    }
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\n BUS ERROR\n",1,2,3,4,5,6);
            return mvI2cData;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cData.data = 0xffffffff;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    logMsg("\nI2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC \n",1,2,3,4,5,6);
            return mvI2cData;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cData.data = 0xffffffff;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        logMsg("\n TIMEOUT\n",1,2,3,4,5,6);
        return mvI2cData;
    }
    /* address transmited ok */
    mvI2cData.errorCode = I2C_NO_ERROR;
    return mvI2cData;
}
/*******************************************************************************
* mvI2cMasterEEPROMread - Read data from an EEPROM device
*
* DESCRIPTION:
*       This function reads data ( byte size ) from an EEPROM device and return 
*       with the 'I2C_DATA_STRUCT' structure.The data is valid only if the 
*       structure's field 'errorCode' has the value 'I2C_NO_ERROR'.
*
* INPUT:
*       deviceAddress - The target EEPROM device address(7 bit only) .
*       address - The desired offset to be read from ( can be 8 bit or more 
*                 depending on the EEPROM device).
*       addrRange - I2C_ADDR_RANGE value ( defined in i2c.h ) to indicate the 
*                   EEPROM address range. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       I2C_DATA_STRUCT struct containing the read data and the error-code.
*      NOTE:
*       Data is valid only if the errorCode is I2C_NO_ERROR.
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cMasterEEPROMread(unsigned int deviceAddress,
                                      unsigned int address,
                                      I2C_ADDR_RANGE addrRange)
{
    unsigned int    temp,timeOut,status,data, intFlagTimeOut = 0;
    I2C_DATA_STRUCT mvI2cResult , mvI2cData;

    mvI2cSetAckBit();
    mvI2cResult = mvI2cMasterEEPROMtransmitAddress(deviceAddress,address,
                                                   addrRange);                                                      
    if(mvI2cResult.errorCode != I2C_NO_ERROR)
    {
        return mvI2cResult;
    }
    /* generate a repeated start bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_START_BIT);
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = 0;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for it to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_REPEATED_START_CONDITION_TRA:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    /* Transmit the deviceAddress */
    GT_REG_WRITE(I2C_DATA,(deviceAddress<<1) | I2C_READ);
    /*mvI2cDelay(I2C_EEPROM_DELAY, 0x1);*/
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;    
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    /* Clear the interrupt flag and signal no-acknowledge */
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    GT_REG_READ(I2C_CONTROL,&temp);
    /*mvI2cDelay(I2C_EEPROM_DELAY, 0x1);*/
    GT_REG_WRITE(I2C_CONTROL,temp & 0xfffffff0);
    /*mvI2cDelay(I2C_EEPROM_DELAY, 0x1);*/
    /* Wait for the data */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_MAS_REC_READ_DATA_ACK_NOT_TRA:
            GT_REG_READ(I2C_DATA,&data);
            mvI2cData.data = data;
            mvI2cData.status = status;
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    mvI2cData.errorCode = I2C_NO_ERROR;
    mvI2cData.status = status;
    return mvI2cData;
}
/*******************************************************************************
* mvI2cMasterEEPROMread - Read data from an EEPROM device
*
* DESCRIPTION:
*       This function reads data ( byte size ) from an EEPROM device and return 
*       with the 'I2C_DATA_STRUCT' structure.The data is valid only if the 
*       structure's field 'errorCode' has the value 'I2C_NO_ERROR'.
*
* INPUT:
*       deviceAddress - The target EEPROM device address(7 bit only) .
*       address - The desired offset to be read from ( can be 8 bit or more 
*                 depending on the EEPROM device).
*       addrRange - I2C_ADDR_RANGE value ( defined in i2c.h ) to indicate the 
*                   EEPROM address range. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       I2C_DATA_STRUCT struct containing the read data and the error-code.
*      NOTE:
*       Data is valid only if the errorCode is I2C_NO_ERROR.
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cMasterEEPROMBurstread(unsigned int deviceAddress,
                                      unsigned int address,int count,
                                      I2C_ADDR_RANGE addrRange)
{
    unsigned int    temp,timeOut,status,data, intFlagTimeOut = 0;
    I2C_DATA_STRUCT mvI2cResult , mvI2cData;

    mvI2cSetAckBit();
    mvI2cResult = mvI2cMasterEEPROMtransmitAddress(deviceAddress,address,
                                                   addrRange);                                                      
    if(mvI2cResult.errorCode != I2C_NO_ERROR)
    {
        return mvI2cResult;
    }
    /* generate a repeated start bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_START_BIT);
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for it to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_REPEATED_START_CONDITION_TRA:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    /* Transmit the deviceAddress */
    GT_REG_WRITE(I2C_DATA,(deviceAddress<<1) | I2C_READ);
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;    
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    /* Clear the interrupt flag and signal no-acknowledge */
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            return mvI2cData;
        }
    }
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & 0xfffffff0);
    /* Wait for the data */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            mvI2cData.data = 0xffffffff;
            mvI2cData.errorCode = I2C_GENERAL_ERROR;
            mvI2cData.status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return mvI2cData;
        case I2C_MAS_REC_READ_DATA_ACK_NOT_TRA:
            GT_REG_READ(I2C_DATA,&data);
            mvI2cData.data = data;
            mvI2cData.status = status;
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        mvI2cData.data = 0xffffffff;
        mvI2cData.errorCode = I2C_TIME_OUT_ERROR;
        mvI2cData.status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return mvI2cData;
    }
    mvI2cData.errorCode = I2C_NO_ERROR;
    mvI2cData.status = status;
    return mvI2cData;
}
/*******************************************************************************
* mvI2cDelay - cause a delay for desired mili-second.
*
* DESCRIPTION:
*       This function causes a time delay equals to the parameter miliSec with 
*       the counter-timer countNum deliverd. This function is neccesary  when 
*       writing to an EEPROM. A time delay of 10 mili-seconds is needed between
*       every write transaction according to the EEPROM data sheet .
*
* INPUT:
*       miliSec  - the desired delay in mili-seconds.
*       countNum -  the counter-timer number (defind in mvCntmr.h ) .
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cDelay(unsigned int miliSec,CNTMR_NUM countNum)
{
    unsigned int oneMiliSec;

    oneMiliSec = mvI2cTclock/1000;
    gtCntmrDisable(countNum);	            
    gtCntmrSetMode(countNum,COUNTER);
    gtCntmrLoad(countNum,miliSec * oneMiliSec);	
    gtCntmrEnable(countNum);	            
    while(gtCntmrRead(countNum));
}

/*******************************************************************************
* mvI2cMasterEEPROMwrite - write data to an EEPROM device.
*
* DESCRIPTION:
*       This function writes to an EEPROM device the data ( byte size ) within 
*       the i2cData struct .
*
* INPUT:
*       deviceAddress - The target device address to be written.
*       *pI2cData     - struct containing the data to be written.
*       address       - The desired offset to be written within the EEPROM.
*       addrRange     - The address range of the target EEPROM ( values defined
*                       in mvI2c.h).
*       counterNum    - The counter-timer number (one out of 8 possible values 
*                       defined in mvCntmr.h) needed for the delay between each 
*                       write transaction.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cMasterEEPROMwrite(unsigned int deviceAddress,
                            I2C_DATA_STRUCT *pI2cData, unsigned int address,
                            I2C_ADDR_RANGE addrRange, CNTMR_NUM counterNum)
{
  
    unsigned int    temp,timeOut,status = 0, intFlagTimeOut = 0;
    I2C_DATA_STRUCT mvI2cResult;

    mvI2cSetAckBit();
    mvI2cDelay(I2C_EEPROM_DELAY,counterNum);
   /* logMsg ("\n Before EEPROM Trasmit Address Inside WriteEEPROM\n",1,2,3,4,5,6);*/
    mvI2cResult = mvI2cMasterEEPROMtransmitAddress(deviceAddress,address,
                                               addrRange);
    if(mvI2cResult.errorCode != I2C_NO_ERROR)
    {
        pI2cData->errorCode = mvI2cResult.errorCode;
        pI2cData->status    = mvI2cResult.status;
       /* logMsg ("\n Error During Transmit of Device Address and offset\n",1,2,3,4,5,6);
        logMsg ("\n Error Code : %u Status : %u \n", pI2cData->errorCode, pI2cData->status,3,4,5,6);*/
        return ;
    }
    /* Write the data */
    GT_REG_WRITE(I2C_DATA,pI2cData->data);
    /*mvI2cDelay(I2C_EEPROM_DELAY,0x1);*/
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            pI2cData->data = 0xffffffff;
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
	    printf("Error during wait for Interrupt flag to be set after data Transmission\n");
            return;
        }
    }
    mvI2cClearIntFlag();
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_CONTROL,&temp);
        if((temp & I2C_INT_FLAG) == I2C_INT_FLAG)
            break;
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        pI2cData->errorCode = I2C_INT_FLAG_STUCK_AT_0;
        pI2cData->status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
	printf ("Time out value reached\n");
        return;
    }
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        mvI2cDelay(I2C_EEPROM_DELAY,0x1);
        /*status = GTREGREAD(I2C_STATUS_BAUDE_RATE);*/
        GT_REG_READ(I2C_STATUS_BAUDE_RATE, &status);

        switch(status)
        {
        case I2C_BUS_ERROR:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            printf("I2c Bus Error\n"); 
            return;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            /* Generate stop bit */
            mvI2cGenerateStopBit();
            /* Wait for the interrupt bit to be set */
            while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
            {   
                if(mvI2cReadIntFlag() == 1)
                    break;
                intFlagTimeOut++;
                if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
                {
                    pI2cData->data = 0xffffffff;
                    pI2cData->errorCode = I2C_GENERAL_ERROR;
                    pI2cData->status = status;
                    mvI2cGenerateStopBit();
		    printf("Master Transmitted data Byte but timed out before ACK Recieved\n");	 
                    return;
                }
            }
            mvI2cClearIntFlag();
            break;
        case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
            pI2cData->errorCode = I2C_GENERAL_ERROR;
            pI2cData->status = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
	    printf("Transmitted Byte but ACK not Recieved\n");
            return ;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        pI2cData->errorCode = I2C_TIME_OUT_ERROR;
        pI2cData->status = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
	printf("I2c ERROR : timed Out\n");
        return;
    }
    pI2cData->errorCode = I2C_NO_ERROR;
    pI2cData->status = status;
    return;
}
/*******************************************************************************
* mvI2cMasterEEPROMwriteWord - write word to an EEPROM device.
*
* DESCRIPTION:
*       This function writes a word to an EEPROM device.When writing a word to
*       an EEPROM the address must be aligned to 4 (i.e 0, 4, 8 .... ).
*       The LSB byte is written first.
*
* INPUT:
*       deviceAdress - The EEPROM device address ( 7 bit only).
*       *pI2cData    - struct containing the data(word) to be written and the
*                      result at the of the transaction.
*       address      - The target offset within the EEPROM to be written.
*       addrRange    - The address range within the EEPROM ( values defined in 
*                      mvI2c.h ).
*       counterNum   - The counter-timer number (one out of 8 possible values 
*                      defined in cntmr.h) needed for the delay between each
*                      write transaction.
*
* OUTPUT:
*       i2cData struct containing the result of the write action ,any value 
*       other than I2C_NO_ERROR  means that an error occured during the 
*       transaction .
*
* RETURN:
*       None.
*
*******************************************************************************/
void mvI2cMasterEEPROMwriteWord(unsigned int deviceAddress,
                                I2C_DATA_STRUCT *pI2cData,unsigned int address,
                                I2C_ADDR_RANGE addrRange,CNTMR_NUM counterNum)
{
    unsigned int    tempAddress;
    I2C_DATA_STRUCT mvI2cTempData;

    /* must be aligned to 4 */ 
    if(address%4!=0)
    {
        pI2cData->errorCode  = I2C_GENERAL_ERROR;
        return;
    }
    /* Writing the word */
    for(tempAddress = address ; tempAddress < (address + 4) ;tempAddress++)
    {
        mvI2cTempData.data = 
            (pI2cData->data >> (8*(tempAddress - address))) & 0x000000ff;
        mvI2cMasterEEPROMwrite(deviceAddress,&mvI2cTempData,tempAddress,
                             addrRange,counterNum);        
        
        if(mvI2cTempData.errorCode != I2C_NO_ERROR)
        {
            pI2cData->errorCode  = mvI2cTempData.errorCode;
            pI2cData->status     = mvI2cTempData.status;
            return; 
        }
    }
    pI2cData->errorCode  = mvI2cTempData.errorCode;
    pI2cData->status     = mvI2cTempData.status;
    return;
}
/*******************************************************************************
* mvI2cMasterEEPROMreadWord - Read word from an EEPROM device
*
* DESCRIPTION:
*       This function reads a word  from an EEPROM device.The read address must 
*       be aligned to 4.
*
* INPUT:
*       deviceAdress - The EEPROM device address ( 7 bit only).
*       address      - The target offset within the EEPROM to be read.
*       addrRange    - The address range within the EEPROM ( values defined in 
*                      mvI2c.h ).
*
* OUTPUT:
*       None.
*
* RETURN:
*       I2C_DATA_STRUCT struct containing the read data ( word ) and the 
*       error-code .
*       Data is valid only if the errorCode is I2C_NO_ERROR.
*
*******************************************************************************/
I2C_DATA_STRUCT mvI2cMasterEEPROMreadWord(unsigned int deviceAddress,
                                          unsigned int address,
                                          I2C_ADDR_RANGE addrRange)
{
    unsigned int    wordData=0x0,tempAddress;
    I2C_DATA_STRUCT mvI2cData;

    /* must be aligned to 4 */ 
    if(address%4!=0)
    {
        mvI2cData.errorCode  = I2C_GENERAL_ERROR;
        return mvI2cData;
    }
    /* reading the word */
    for(tempAddress = address ; tempAddress < (address + 4) ;tempAddress++)
    {
        mvI2cData = mvI2cMasterEEPROMread(deviceAddress,tempAddress,
                                      addrRange);  
        if(mvI2cData.errorCode == I2C_NO_ERROR)
        {
            wordData =  wordData  | 
                        ((0x000000ff & mvI2cData.data)<<((8*(tempAddress -
                                                            address))));
        }
        else
        {
            return mvI2cData;
        }
    }
    mvI2cData.data = wordData;
    return mvI2cData;
}

/*******************************************************************************
* mvI2cMasterWriteBlock - write a block of data to an I2c device.
*
* DESCRIPTION:
*       This function writes the data pointed by the pI2cData paramter to an i2c 
*       slave with 7 or 10 bit address. For example, this function can be used 
*       for writing from one MV (master) to another MV (slave ,on the same board
*       or on differant board) or writing to any device (with the exception of 
*       an EEPROM) that support I2C . The write operation can be teminated 
*       either with a stop bit or without a stop bit depending on the 
*       'generateStopBit' parameter value.
*
* INPUT:
*       deviceAddress   - 7 or 10 bit address of the target device . If the 
*                         device has a 10 bit address , add to deviceAddress the
*                         value I2C_10BIT_ADDR (defind in mvI2c.h).    
*       *pI2cData       - Pointer to a data buffer containing the data to be 
*                         written.
*       i2cDataLen      - The data buffer length (actual number of data bytes).
*       generateStopBit - Flag indicating whether a stop bit should be generated
*                         at the end of data transfer or not.
*       *pI2cStatus     - Pointer to a variable that will contain the I2C status
*                         value at the end of this function operation.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void   mvI2cMasterWriteBlock(unsigned short  deviceAddress,                 
                             unsigned char  *pI2cData,                    
                             unsigned int    i2cDataLen,                  
                             bool            generateStopBit,
                             unsigned int   *pI2cStatus)
{
    unsigned int     timeOut, status,i, intFlagTimeOut = 0;
    I2C_DATA_STRUCT  I2cResult;

    mvI2cSetAckBit();
    /* Clear the stop bit */
    GT_RESET_REG_BITS(I2C_CONTROL,I2C_STOP_BIT);
    /* Generate start bit */
    I2cResult = mvI2cGenerateStartBit();
    if(I2cResult.errorCode != I2C_NO_ERROR)
    {
        *pI2cStatus = I2cResult.status;
        return;
    }
    /* Transmit the deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR) 
        /* 10 Bit deviceAddress */
    {
        /* Transmiting the 2 most significant bits of the 10 bit address */
        GT_REG_WRITE( I2C_DATA,((((deviceAddress & 0X300)>>7)|0Xf0)));
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                return;
            }
        }
        mvI2cClearIntFlag();
        /* Wait for the deviceAddress(1st byte) to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return ;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;   
                break;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return ;
        }
        /* Transmiting the 8 least significant bits of the 10 bit address */
        GT_REG_WRITE(I2C_DATA,deviceAddress & 0x000000ff);
    }
    else /* 7 Bit deviceAddress */
    {
        GT_REG_WRITE(I2C_DATA,deviceAddress<<1);
    }
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            return;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            break;
        case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return ;
        case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            break;
        case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        default:
            break;
        }
    }

    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        *pI2cStatus = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return;
    }
    
    for(i = 0 ; i < i2cDataLen ; i++)
    {
        GT_REG_WRITE(I2C_DATA,pI2cData[i] & 0xff);
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                return;
            }
        }
        mvI2cClearIntFlag();
        /* wait for the data to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            if (mvI2cReadIntFlag() != 1) 
                continue;
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        }
    }
    if (generateStopBit == true)
    {
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
    }
    *pI2cStatus = status;
}

/*******************************************************************************
* mvI2cMasterReadBlock - Read a block of data from an I2C device.
*
* DESCRIPTION:
*       This function reads a block of data from an I2C device with a 7 or 10 
*       bit address. For example, this function can be used to read data by one 
*       MV (master) from another MV (slave ,on the same board or on differant 
*       board) or read from any device (with the exception of EEPROM) that 
*       support I2C. The read operation can be teminated either with a stop bit 
*       or without a stop bit depending on the 'generateStopBit' parameter value.
*
* INPUT:
*       deviceAddress   - 7 or 10 bit address of the target device . If the 
*                         device has a 10 bit address , add to deviceAddress the
*                         value I2C_10BIT_ADDR (defind in mvI2c.h).    
*       *pI2cData       - Pointer to a data buffer that will contain the read 
*                         data.
*       i2cDataLen      - The data buffer length (actual number of data bytes 
*                         be read).
*       generateStopBit - Flag indicating whether a stop bit should be generated
*                         at the end of data transfer or not.
*       *pI2cStatus     - Pointer to a variable that will contain the I2C status
*                         value at the end of this function operation.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void  mvI2cMasterReadBlock (unsigned short  deviceAddress,
                            unsigned char  *pI2cData,                    
                            unsigned int    i2cDataLen,                  
                            bool            generateStopBit,
                            unsigned int   *pI2cStatus)
{
    I2C_DATA_STRUCT  I2cResult;
    unsigned int     timeOut, status, data, intFlagTimeOut = 0;
    unsigned char    temp; 
    
    mvI2cSetAckBit();
    /* Clear the stop bit */
    GT_RESET_REG_BITS(I2C_CONTROL, I2C_STOP_BIT);
    /* Generate start bit */
    I2cResult = mvI2cGenerateStartBit();
    if(I2cResult.errorCode != I2C_NO_ERROR)
    {
        *pI2cStatus = I2cResult.status;
        return;
    }
    /* Transmit the deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR) 
        /* 10 Bit deviceAddress */
    {
        /* Transmiting the 2 most significant bits of the 10 bit address  */
        GT_REG_WRITE( I2C_DATA,((((deviceAddress & 0X300)>>7)|0Xf0))|I2C_READ);
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                return;
            }
        }
        mvI2cClearIntFlag();
        /* Wait for the deviceAddress(1st byte) to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;   
                *pI2cStatus = status;
                break;
            case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        }
        /* Transmiting the 8 least significant bits of the 10 bit address */
        GT_REG_WRITE(I2C_DATA,(deviceAddress & 0x000000ff));
    }
    else /* 7 Bit deviceAddress */
    {
        GT_REG_WRITE(I2C_DATA,(deviceAddress<<1) | I2C_READ);
    }
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            return;
        }
    }
    mvI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;   
            *pI2cStatus = status;
            break;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        case I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            *pI2cStatus = status;
            break;
        case I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        *pI2cStatus = status;
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
        return;
    }
    /* Wait for interrupt to be set as a result of generating the address */
    /* Wait for the interrupt bit to be set */
    while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
    {   
        if(mvI2cReadIntFlag() == 1)
            break;
        intFlagTimeOut++;
        if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            return;
        }
    }
    mvI2cClearIntFlag();

    /* Start reading the data */
    for (data = 0 ;data < i2cDataLen ; data++)
    {                
        /* Signal no-acknowledge if it's the last data */ 
        if((data + 1) == i2cDataLen)
        {
            GT_RESET_REG_BITS(I2C_CONTROL, I2C_ACK);
        }
        /* Wait for the data */
        /* Wait for the interrupt bit to be set */
        while(intFlagTimeOut < I2C_TIMEOUT_VALUE)    
        {   
            if(mvI2cReadIntFlag() == 1)
                break;
            intFlagTimeOut++;
            if(intFlagTimeOut == I2C_TIMEOUT_VALUE)
            {
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                return;
            }
        }
        /* Wait of the right status */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                *pI2cStatus = status;
                mvI2cGenerateStopBit();
                mvI2cClearIntFlag();
                return;
            case I2C_MAS_REC_READ_DATA_ACK_NOT_TRA:  /* Last data */
                *pI2cStatus = status;
                GT_REG_READ(I2C_DATA,&temp);
                pI2cData[data] = 0x000000ff & temp;
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            case I2C_MAS_REC_READ_DATA_ACK_TRA:
                *pI2cStatus = status;
                GT_REG_READ(I2C_DATA,&temp);
                pI2cData[data] = 0x000000ff & temp;
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            *pI2cStatus = status;
            mvI2cGenerateStopBit();
            mvI2cClearIntFlag();
            return;
        }
        mvI2cClearIntFlag();
    }
    if(generateStopBit == true)
    {
        mvI2cGenerateStopBit();
        mvI2cClearIntFlag();
    }
}


#ifdef __cplusplus
} 
#endif

bool frcEEPROMWrite8 (unsigned int devAdd, unsigned int offset, int noofBytes, UINT8 *buffer)
{

    I2C_DATA_STRUCT  *I2cbuffer=malloc(sizeof(I2C_DATA_STRUCT));
    int i=0;	
    bool ret = true;
    
    
   
    for (i=0;i<noofBytes;i++)
    {
	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[i];	
	mvI2cMasterEEPROMwrite ( devAdd,I2cbuffer,offset+i,MORE_THAN_256BYTES, CNTMR_FIRST);
	
        if ( I2cbuffer->errorCode != I2C_NO_ERROR)
        {
            printf(" Error during write of byte no:%d\t status:%x\n",i,I2cbuffer->status);
            ret = false;
            return (ret);
        }
	/* Clear the stop bit */
    	GT_RESET_REG_BITS(I2C_CONTROL,I2C_STOP_BIT);
        sysMsDelay(10);
   }
        free(I2cbuffer);
        return (ret);
}

bool mvEEPROMRTCWrite (unsigned int devAdd, unsigned int offset, int noofBytes, UINT8 *buffer)
{

    I2C_DATA_STRUCT  *I2cbuffer=malloc(sizeof(I2C_DATA_STRUCT));
    int i=0;
    bool ret = true;
   
   
  
    for (i=0;i<noofBytes;i++)
    {
        I2cbuffer->errorCode = I2C_NO_ERROR;
        I2cbuffer->data = buffer[i];
        mvI2cMasterEEPROMwrite ( devAdd,I2cbuffer,offset+i, _256BYTES, CNTMR_FIRST);

        if ( I2cbuffer->errorCode != I2C_NO_ERROR)
        {
            printf(" Error during write of byte no:%d\t status:%x\n",i,I2cbuffer->status);
            ret = false;
            return (ret);
        }
        /* Clear the stop bit */
        GT_RESET_REG_BITS(I2C_CONTROL,I2C_STOP_BIT);
	/*mvI2cDelay(I2C_EEPROM_DELAY,0x1);*/
   }
        free(I2cbuffer);
        return (ret);
}

bool frcEEPROMWrite16 (unsigned int devAdd, unsigned int offset, int noofBytes, UINT8 *buffer)
{

    I2C_DATA_STRUCT  *I2cbuffer=malloc(sizeof(I2C_DATA_STRUCT));
    int i=0;
    bool ret = true;



    for (i=0;i<noofBytes;i++)
    {

        I2cbuffer->errorCode = I2C_NO_ERROR;
        I2cbuffer->data = buffer[i];
        mvI2cMasterEEPROMwriteWord ( devAdd,I2cbuffer,offset+i,MORE_THAN_256BYTES, CNTMR_FIRST);

        if ( I2cbuffer->errorCode != I2C_NO_ERROR)
        {
            printf(" Error during write of byte no:%d\t status:%x\n",i,I2cbuffer->status);
            ret = false; 
            return (ret);
        }
        /* Clear the stop bit */
        GT_RESET_REG_BITS(I2C_CONTROL,I2C_STOP_BIT);
	mvI2cDelay(I2C_EEPROM_DELAY,0x1);
   }
        free(I2cbuffer);
        return (ret);
}

bool frcEEPROMRead8 (UINT8 devAdd, UINT16 offset, int noofBytes, UINT8 *buffer)
{
   I2C_DATA_STRUCT  I2cbuffer;
   int i;
   bool ret = true;

   for (i=0;i<noofBytes;i++)
   {
        
	I2cbuffer = mvI2cMasterEEPROMread (devAdd, offset+i , MORE_THAN_256BYTES);        
        buffer[i]=I2cbuffer.data;

        if (I2cbuffer.errorCode != I2C_NO_ERROR)
        {
            printf(" Error during read of byte no:%d\t status:%x\n",i,I2cbuffer.status);
            ret = false;
            return (ret);
        }
           
   }
   
   return (ret);
} 
   

bool frcEEPROMRead16 (UINT8 devAdd, UINT16 offset, int noofBytes, UINT8 *buffer)
{
   I2C_DATA_STRUCT  I2cbuffer;
   int i,j;
    bool ret = true;

   for (i=0;i<noofBytes;i++)
   {

	
        I2cbuffer = mvI2cMasterEEPROMreadWord (devAdd, offset+i , MORE_THAN_256BYTES);
        buffer[i]=I2cbuffer.data;

        if (I2cbuffer.errorCode != I2C_NO_ERROR)
        {
            printf(" Error during read of byte no:%d\t status:%x\n",i,I2cbuffer.status);
            ret = false;
            return (ret);
        }

   }

   for (j=0;j<noofBytes;j++)
   {
        printf("The Byte Read %d is: %x\n",j,buffer[j]);

   }
  return (ret);
}
 
void mvRTCRead(UINT8 *buffer)
{
   	I2C_DATA_STRUCT  I2cbuffer;

	
        I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x85 , _256BYTES); /* The Device Address of the RTC is 0xa0 */
	buffer[0] = I2cbuffer.data;  /* Hours */

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x83 , _256BYTES); /* The Device Address of the RTC is 0xa0 */
    	buffer[1] = I2cbuffer.data;  /*min*/

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x81 , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[2] = I2cbuffer.data;  /*seconds*/

        I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x87 , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[3] = I2cbuffer.data;  /*date*/

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x89 , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[4] = I2cbuffer.data;  /*month*/

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x8d , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[5] = I2cbuffer.data;  /*year*/

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x8b , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[6] = I2cbuffer.data;  /*day*/

	I2cbuffer = mvI2cMasterEEPROMread (0x50, 0x93 , _256BYTES); /* The Device Address of the RTC is 0xa0 */        buffer[7] = I2cbuffer.data;  /*century*/
	
}

void mvRTCWrite(UINT8 *buffer)   
{
   	I2C_DATA_STRUCT  *I2cbuffer=malloc(sizeof(I2C_DATA_STRUCT));

	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[0]; /* Hours */	
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x84 , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */
	
        I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[1]; /* Minutes */	
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x82 , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[2]; /* Seconds*/
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x80 , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[3]; /* Date*/
        mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x86 , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[4]; /* Month*/
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer,  0x88 ,_256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[5]; /* Year*/
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x8c , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[6]; /* Day*/
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x8a , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
    	I2cbuffer->errorCode = I2C_NO_ERROR;        	
 	I2cbuffer->data = buffer[7]; /* Century */
	mvI2cMasterEEPROMwrite (0x50,I2cbuffer, 0x92 , _256BYTES, CNTMR_FIRST); /* The Device Address of the RTC is 0xa0 */                    
}   

/* :: MV64360 I2C Fix :: 04 FEB 2005 - Start */

/************************************************************/
/*Set I2c_ready*/
/************************************************************/
void setI2cReady()
{

    unsigned int temp3;
    /* Gpp[9] = 1 */
    GT_REG_READ( GPP_VALUE, &temp3 );
    temp3 |= 0x00000200;
    GT_REG_WRITE(GPP_VALUE, temp3);
}
/************************************************************/
/*Set I2c_busy*/
/************************************************************/


void setI2cBusy()
{

    unsigned int temp3;
    /* Gpp[9] = 0 */
    GT_REG_READ( GPP_VALUE, &temp3 );
    temp3 &= 0xfffffdff;
    GT_REG_WRITE(GPP_VALUE, temp3);

}


/******************************************************************************
* Function    : frcI2CWrite                                                   *
*                                                                             *
* Description : This is the lowest level driver to write to a slave I2C device*
*                                                                             *
* Inputs      : devAdd  - Slave device hardware address.                      *
*               regFile - Array of data to be sent to the slave device        *
*               noBytes - Number of bytes to be sent.(present in the array)   *
*                                                                             *
* Output      : true  - if successful                                         *
*               false - if not successful                                     *
*                                                                             *
******************************************************************************/

bool frcI2CWrite(UINT8 devAdd,UINT8* regFile, unsigned int noBytes) {
    unsigned int i2cstatus ,temp=0x0, count=0;

#ifdef PMC280_I2C_DEBUG
    unsigned int i;
#endif /* PMC280_I2C_DEBUG */
    bool ret= false;

/* Debug prints */
#ifdef PMC280_I2C_DEBUG
    printf("Slave device hardware address = %x\n",devAdd);
    printf("Number of bytes = %x\nAnd the data are\n",noBytes);
    for (i=0;i<noBytes;i++)
    printf("%x ",regFile[i]);
#endif /* PMC280_I2C_DEBUG */

    sysMsDelay(50);
    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL=69.27KHz
                                                 for 133MHz TClk */

	 setI2cBusy();/*Aravind*/
    GT_REG_WRITE(I2C_CONTROL,0x60);           /* Start bit and Enable bit */
    while(!isIntFlagSet());                   /* Wait for it to start */

    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cstatus);
    if((i2cstatus == 0x08) || (i2cstatus == 0x10)) {

#ifdef PMC280_I2C_DEBUG 
        printf("I2C Start/Re-start Condition is succuessful\n");
        printf("Writes the Slave Address as :%x\n",devAdd);
#endif /* PMC280_I2C_DEBUG */

        sysMsDelay(50);
        GT_REG_WRITE(I2C_DATA,devAdd | 0x00); /* Slave Address,LSB r/w bit is
                                                 cleared(0) for WRITE */
        clearIntFlag();
        while(!isIntFlagSet());    /* Wait for it to drive the slave address */

        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cstatus);
        switch(i2cstatus) {
            case I2C_BUS_ERROR:
                printf("Bus Error!!!\n");
                GT_REG_READ(I2C_CONTROL,&temp);
                GT_REG_WRITE(I2C_CONTROL,temp | 0x10); /* generate stop bit */

		   setI2cReady();
                sysMsDelay(20);
                clearIntFlag();
                return ret;

            case  I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:

#ifdef PMC280_I2C_DEBUG 
                printf("Address+Write Bit is transmitted and ACK is received\n");
                printf("Data being written to Slave.....\n");
#endif /* PMC280_I2C_DEBUG */

                sysMsDelay(50);

                /***** Data Write ******/
                for (count=0;count<noBytes;count++) {
                    GT_REG_WRITE(I2C_DATA,regFile[count]); /*data to be written
                                                             to the slave */
                    clearIntFlag();
                    while(!isIntFlagSet());

                    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cstatus);
                    switch(i2cstatus) {

                        case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
#ifdef PMC280_I2C_DEBUG 
                            printf("Master transmitted data byte #%d : %x"
                                " and ACK is received\n",count,regFile[count]);
#endif /* PMC280_I2C_DEBUG */
                            sysMsDelay(50);
                            ret= true;
                            break;

                        case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
                            printf("Master transmitted data byte # %d : %x"
                            "and ACK is NOT received.\n",count,regFile[count]);
                            ret = false;
                            break;

                        case I2C_MAS_LOST_ARB_DURING_ADDR_OR_DATA_TRA:
                            printf("Master lost arbitration during data byte # %d: %x  transfer\n",count,regFile[count]);
                ret = false;
                break;
                        default:
                            printf("Error, I2C status %x\n",i2cstatus);
                            ret=false;
                            break;
                    }
                    if (ret == false)
                        break;
                }
#ifdef PMC280_I2C_DEBUG  
                printf("\n\nStop the Data Transfer\n");
#endif /* PMC280_I2C_DEBUG */

                sysMsDelay(50);
                GT_REG_READ(I2C_CONTROL,&temp);
                GT_REG_WRITE(I2C_CONTROL, temp | 0x10);
		setI2cReady();/*Aravind*/
                clearIntFlag();
        return ret;

            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
#ifdef PMC280_I2C_DEBUG                
        printf("Address+Write Bit is transmitted and ACK is NOT received....\n");
#endif /* PMC280_I2C_DEBUG */
                GT_REG_READ(I2C_CONTROL,&temp);
                GT_REG_WRITE(I2C_CONTROL, temp | 0x10);
			setI2cReady();/*Aravind*/
                clearIntFlag();
                GT_REG_WRITE(I2C_CONTROL,0x0);
                return ret;
        }
    }
    else {
        printf("I2C Start Condition Fails!!!\nI2C Status = %x\n",i2cstatus);
        GT_REG_READ(I2C_CONTROL,&temp);
        GT_REG_WRITE(I2C_CONTROL, temp | 0x10);
	setI2cReady();/*Aravind*/
        clearIntFlag();
        GT_REG_WRITE(I2C_CONTROL,0x00);
        return ret;
    }
	setI2cReady();/*Aravind*/
    return ret;        
}




/******************************************************************************    
* Function:     isIntFlagSet.
*
* Description:  Checks wether the interrupt flag bit of the Control Register
*               is set or not.
* Inputs:       None.
*
* Outputs:      True if set else false
*
******************************************************************************/

bool isIntFlagSet() {

    unsigned int temp;

#ifdef PMC280_I2C_DEBUG
    printf("Waiting for the Interrupt flag to be set .....\n");
#endif /* PMC280_I2C_DEBUG */

    GT_REG_READ(I2C_CONTROL, &temp);
    if ((temp & 0x8) == 0x8)
        return true;
    else
        return false;
}

void clearIntFlag() {
    unsigned int temp;

    GT_REG_READ(I2C_CONTROL, &temp);
    GT_REG_WRITE(I2C_CONTROL, temp & 0xfffffff7);
}




         
/***********************************************************/

/*  :: MV64360 I2C Fix :: 04 FEB 2005 - End */
