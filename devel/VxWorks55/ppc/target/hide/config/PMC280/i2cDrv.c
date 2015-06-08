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
* i2cDrv.c - I2C Driver support Source File for the on-board ATMEL AT24C02A
*             serial EEPROM device.
*
* DESCRIPTION:
*       This is a simple driver that allows the user to read and write to the 
*       serial EEPROM device through I2C.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/
  

#include "i2cDrv.h"
#include "vxWorks.h"
#include "stdio.h"
#include "intLib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "config.h"
#include "ftpLib.h"
#include "ioLib.h"
  
/* Declarations */
IMPORT void    sysMsDelay      (UINT);  /* delay N millisecond(s) */
bool isIntFlagSet(void);
void clearIntFlag(void);
int EEPROMDetect(UINT8);
void setI2cReady();
void setI2cBusy();

bool protectMAC = true; /* true - protect flase - allow write */
bool protectBIB = true; /* true - protect flase - allow write */
bool protectPCI = true; /* true - protect flase - allow write */
    
/******************************************************************************
* Function     : frcEEPROMWrite                                               *
*                                                                             *
* Description  : This routine does page write in to AT24C02A serial EEPROM at *
*                a specified offset                                           *
*                                                                             *
* Inputs       : devAdd      - EEPROM hardware address                        *
*                writeoffset - page offset                                    *
*                noBytes     - Number of bytes to be written                  *
*                regFile1    - Array of data to be written                    *
*                                                                             *
* Outputs      : true  - If successful                                        *
*                false - If not successful                                    *
*                                                                             *
******************************************************************************/ 

bool frcEEPROMWrite8(UINT8 devAdd, UINT8 writeoffset, unsigned int noBytes, 
                    UINT8 *regFile1) {

    UINT8  regFile2[9];
    unsigned int i=0,j=0;
    bool ret = true;

    if((noBytes < 0) || (noBytes > 256)) {
    printf("Only 0-256 Bytes can be written.\n");
    return(false);
    }    
   
    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
    sysMsDelay(40);   /* Restart the I2C controller */
    for (i=0,j=1;i<noBytes;i++) {
    regFile2[j]=regFile1[i];
    if ((j == 8-(writeoffset%8)) || (i == (noBytes-1))) { /* either 8 or less
                                                             than eight 
                                                         only sould be burnt */
        sysMsDelay(200);      
        regFile2[0]=writeoffset;
        GT_REG_WRITE(I2C_SOFT_RESET,0x0);
        sysMsDelay(40);   /* Restart the I2C controller */
        if(frcI2CWrite(devAdd,regFile2,j+1)){
#ifdef PMC280_I2C_DEBUG 
        printf("Burning of a Page over\n");
#endif
            }
        else {
        printf("Page burning Failed...\n");
                ret = false;
        break;
        }
        j=0;
        writeoffset=writeoffset + 8;
        writeoffset = writeoffset & 0xfffffff8; /* To go to the next page*/
    }
    j++;
    }
    return (ret);
}


/******************************************************************************
* Function     : frcEEPROMWrite                                               *
*                                                                             *
* Description  : This routine does page write in to AT24C02A serial EEPROM at *
*                a specified offset                                           *
*                                                                             *
* Inputs       : devAdd      - EEPROM hardware address                        *
*                writeoffset - page offset                                    *
*                noBytes     - Number of bytes to be written                  *
*                regFile1    - Array of data to be written                    *
*                                                                             *
* Outputs      : true  - If successful                                        *
*                false - If not successful                                    *
*                                                                             *
******************************************************************************/ 

bool frcEEPROMWrite16(UINT8 devAdd, UINT16 writeoffset, unsigned int noBytes, 
                    UINT8 *regFile1) {

    UINT8  regFile2[34];
    unsigned int i=0,j=0;
    bool ret = true;
        
    writeoffset=writeoffset&0x1fff;      /* 13-bit address for 64k EEPROM*/ 
                                     
    if((noBytes < 0) || (noBytes > 8192)) {
    printf("Only 0-8192 Bytes can be written.\n");
    return(false);
    }    
    

    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
    sysMsDelay(40);   /* Restart the I2C controller */
    for (i=0,j=2;i<noBytes;i++) {
    regFile2[j]=regFile1[i];
    if ((j == (33-(writeoffset%32))) || (i == (noBytes-1))) { /* either 32 or less than thirty two
                                                 only sould be burnt */
        sysMsDelay(200);        
        regFile2[0]=(writeoffset&0xff00)>>8;
            regFile2[1]=writeoffset&0x00ff;
        GT_REG_WRITE(I2C_SOFT_RESET,0x0);
        sysMsDelay(40);   /* Restart the I2C controller */
        if(frcI2CWrite(devAdd,regFile2,j+1)){ 
#ifdef PMC280_I2C_DEBUG
        printf("Burning of a Page over\n");
#endif       
            }
        else {
        printf("Page burning Failed...\n");
                ret = false;
        break;
        }
        j=1;             
        writeoffset=writeoffset + 0x0020;                  
            writeoffset=writeoffset & 0xffffffe0; /* To go to the next page*/
    }
    j++;
    }
    return (ret);
}
/******************************************************************************
* Function      : frcEEPROMRead                                               *
*                                                                             *
* Description   : This routine reads from AT24C02A serial EEPROM at           *
*                 a specified offset                                          *
*                                                                             *
* Inputs        : devAdd     - EEPROM hardware device address                 *
*                 readoffset - page offset                                    *
*                 noBytes    - Number of bytes to be read                     *
*                 regFile    - Array where the read data has to be stored     *
*                                                                             *
* Outputs       : true  - If successful                                       *
*                 false - If not successful                                   *
*                                                                             *
******************************************************************************/ 

bool frcEEPROMRead8(UINT8 devAdd, UINT8 readoffset,unsigned int noBytes, 
                    UINT8 *regFile)
{ 
    bool ret;

    if((noBytes < 0) || (noBytes > 256)) {
        printf("Only 0-256 Bytes can be read.\n");
        return(false);
    }    
    sysMsDelay(40);
    GT_REG_WRITE(I2C_SOFT_RESET,0x0); 
    sysMsDelay(40);   /* Restart the I2C controller */

    if(frcEEPROMDummyWrite(devAdd,readoffset)) {
        
#ifdef PMC280_I2C_DEBUG
        printf("Dummy write successful\n");
#endif /* PMC280_I2C_DEBUG */

        if(frcI2CRead(devAdd, regFile, noBytes))
            ret = true;
        else
            ret = false;
    }
    else { 
        printf("Dummy write NOT successful!!!\n");
        ret = false;
    }
    return(ret);
}    

/******************************************************************************
* Function      : frcEEPROMRead                                               *
*                                                                             *
* Description   : This routine reads from AT24C02A serial EEPROM at           *
*                 a specified offset                                          *
*                                                                             *
* Inputs        : devAdd     - EEPROM hardware device address                 *
*                 readoffset - page offset                                    *
*                 noBytes    - Number of bytes to be read                     *
*                 regFile    - Array where the read data has to be stored     *
*                                                                             *
* Outputs       : true  - If successful                                       *
*                 false - If not successful                                   *
*                                                                             *
******************************************************************************/ 

bool frcEEPROMRead16(UINT8 devAdd, UINT16 readoffset,unsigned int noBytes, 
                    UINT8 *regFile)
{ 
    bool ret;
    readoffset=readoffset&0x1fff;   /* 64k EEPROM requires 13-bit addrress */

    if((noBytes < 0) || (noBytes > 8192)) {
        printf("Only 0-8192 Bytes can be read.\n");
        return(false);
    }    
    sysMsDelay(40);
    GT_REG_WRITE(I2C_SOFT_RESET,0x0); 
    sysMsDelay(40);   /* Restart the I2C controller */

    if(frcEEPROMDummyWrite16(devAdd,readoffset)) {
        
#ifdef PMC280_I2C_DEBUG
        printf("Dummy write successful\n");
#endif /* PMC280_I2C_DEBUG */

        if(frcI2CRead(devAdd, regFile, noBytes))
            ret = true;
        else
            ret = false;
    }
    else { 
        printf("Dummy write NOT successful!!!\n");
        ret = false;
    }
    return(ret);
}    
    
/******************************************************************************
* Function    : frcI2cReset                                                   *
*                                                                             *
* Description : This routine resets the I2C logic of MV-64360 system          *
*               controller                                                    *
*                                                                             *
* Inputs      : None                                                          *
*                                                                             *
* Outputs     : None                                                          *
*                                                                             *
* Returns     : None                                                          *
******************************************************************************/ 

void frcI2cReset()
{
    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
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
            case I2CBUS_ERROR:
                printf("Bus Error!!!\n");
                GT_REG_READ(I2C_CONTROL,&temp);
                GT_REG_WRITE(I2C_CONTROL,temp | 0x10); /* generate stop bit */

		   setI2cReady();
                sysMsDelay(20);
                clearIntFlag();
                return ret;

            case ADDR_WB_TRA_ACK:

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

                        case MASTER_DATA_TRA_ACK:
#ifdef PMC280_I2C_DEBUG 
                            printf("Master transmitted data byte #%d : %x"
                                " and ACK is received\n",count,regFile[count]);
#endif /* PMC280_I2C_DEBUG */
                            sysMsDelay(50);
                            ret= true;
                            break;

                        case MASTER_DATA_TRA_NACK:
                            printf("Master transmitted data byte # %d : %x"
                            "and ACK is NOT received.\n",count,regFile[count]);
                            ret = false;
                            break;

                        case MASTER_LOST_ARBTR_DURING_ADR_DATA:
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

            case ADDR_WB_TRA_NACK:
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
* Function    : frcI2CRead 
*
* Description : This is the lowest driver to read from the slave I2C device.
*
* Inputs      : devAdd  - Slave I2c Device hardware address
*               regFile - Array to store read data from slave
*               index   - Number of bytes to read from slave
*  
* Output      : true  - If successful   
*               false - If no successful
*
******************************************************************************/

bool frcI2CRead(UINT8 devAdd, UINT8* regFile, unsigned int index) {

    unsigned int Status ,Temp=0x0, Count=0;
    bool Ret= false;

    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL =69.27KHz
                                                 for 133MHz TClk */
    			setI2cBusy();/*Aravind*/
 
    GT_REG_WRITE(I2C_CONTROL,0x64);           /* START,EN & ACK BITs are Set */
    while(!isIntFlagSet());                   /* wait for start condition */

    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&Status);
#ifdef PMC280_I2C_DEBUG 
    printf("The Status: %x\n",Status);
#endif /* PMC280_I2C_DEBUG */

    if((Status == 0x10) || (Status == 0x08)) {

#ifdef PMC280_I2C_DEBUG
        printf("I2C Start/Re-start Condition is succuessful\n");
        printf("Writes the Slave Address as :%x\n",devAdd);
#endif /* PMC280_I2C_DEBUG */

        GT_REG_WRITE(I2C_DATA,devAdd | 0x01); /* Slave Address,LSB r/w bit is set for 
                                          READ */
        clearIntFlag();
        while(!(isIntFlagSet()));

        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&Status);
        switch(Status) {
            case I2CBUS_ERROR:
                printf("Bus Error!!!\n");
                GT_REG_READ(I2C_CONTROL,&Temp);
                GT_REG_WRITE(I2C_CONTROL,Temp & 0x10);  /* generate stop bit */

			setI2cReady();/*Aravind*/
                sysMsDelay(20);
                clearIntFlag();                 /* Interrupt flag is cleared */
                return Ret;

            case ADDR_RB_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
               printf("Address+Read Bit is transmitted and ACK is received\n");
               printf("Data being read from slave.....\n");
#endif /* PMC280_I2C_DEBUG */

               /*while(!(isIntFlagSet()));*/
               clearIntFlag(); 
               sysMsDelay(20);

               for (Count=0;Count<index;Count++) {
                    
                   GT_REG_READ(I2C_STATUS_BAUDE_RATE,&Status);
                   switch(Status) {
                       case MASTER_READ_ACK_TRA:
#ifdef PMC280_I2C_DEBUG
                           printf("Master received read Data #%d+ACK is transmitted\n",Count+1);
#endif /* PMC280_I2C_DEBUG */
                        GT_REG_READ(I2C_DATA,&regFile[Count]);                   
                                              /*data read 
                                                                  from slave */
#ifdef PMC280_I2C_DEBUG 
                           printf("Byte %d: %x is read by CPU\n",Count,regFile[Count]);
#endif /* PMC280_I2C_DEBUG */
                           clearIntFlag();  /* Interrupt flag is cleared */
                           sysMsDelay(100);
                           
                           Ret = true;
                           break;

                      case MASTER_READ_NACK_TRA:
                           printf("Master Received read Data #%d + ACK is NOT transmitted\n",Count);
                           Ret = false;
                           clearIntFlag(); /* Interrupt flag is cleared */
                           sysMsDelay(20);
                           break;

                      default:
                           printf("Master Failed to receive Data #%d from EEPROM\n",Count);
                           printf("I2C Status = %x\n",Status);
                           Ret = false;
                           clearIntFlag(); /* Interrupt flag is cleared */
                           sysMsDelay(20);
                           break;
                   }
               } /*End of For loop */

               GT_REG_READ(I2C_CONTROL,&Temp);
               GT_REG_WRITE(I2C_CONTROL,Temp & 0xfff0);   /*Interrupt flag and 
                                                            ACK is cleared */
               sysMsDelay(20);
#ifdef PMC280_I2C_DEBUG 
               printf("\n\nStop the Read Data Transfer\n");
#endif /* PMC280_I2C_DEBUG */
               GT_REG_WRITE(I2C_CONTROL,Temp & 0x10); /* generate stop bit */
		setI2cReady();/*Aravind*/
               clearIntFlag();
               sysMsDelay(20);
               GT_REG_WRITE(I2C_CONTROL,0x0);
               return Ret;

            case ADDR_RB_TRA_NACK:
               printf("Address+Read Bit is transmitted and ACK is NOT received\n");
               GT_REG_WRITE(I2C_CONTROL,Temp & 0x10); /* generate stop bit */
			setI2cReady();/*Aravind*/
               clearIntFlag();
               sysMsDelay(20);
               GT_REG_WRITE(I2C_CONTROL,0x0);
               return Ret;

        }
    }
    else  {
        printf("I2C Start/Re-start Condition Fails.....\n");
        GT_REG_WRITE(I2C_CONTROL,Temp & 0x10); /* generate stop bit */
setI2cReady();/*Aravind*/
        clearIntFlag();
        sysMsDelay(20);
        GT_REG_WRITE(I2C_CONTROL,0x0);
        return Ret;
    }
	setI2cReady();/*Aravind*/
    return Ret;
}

/******************************************************************************
* Function    : frcEEPROMDummyWrite 
*   
* Description : This routine does "Dummy write" to the EEPROM device
*               specifically, for random read and sequencial read  
*  
* Inputs      : devAdd  - EEPROM device hardware address
*               roffset - Offset from where the next read command would 
*                         like to read from.
*
* Output      : true - If successful
*               false - If not successful in issuing the "dummy write"
* 
******************************************************************************/
bool frcEEPROMDummyWrite(UINT8 devAdd, unsigned int roffset) {

    unsigned int i2cStatus, TempData=0x0;
    bool retval= false;

    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL =69.27KHz                                                 for 133MHz TClk */
setI2cBusy();/*Aravind*/
    GT_REG_WRITE(I2C_CONTROL,0x60); /* Start bit & enable bit*/
    while(!isIntFlagSet());
    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
    if(i2cStatus==0x08)    {
#ifdef PMC280_I2C_DEBUG
        printf("I2C Dummy Start Condition is succuessful\n");
        printf("Writes the Dummy Slave Address as :%x\n",devAdd);
#endif /* PMC280_I2C_DEBUG */
        GT_REG_WRITE(I2C_DATA,devAdd | 0x00); /* Slave Address,LSB r/w bit is 
                                                 cleared for WRITE */
        clearIntFlag();
        while(!isIntFlagSet());
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
        switch(i2cStatus) {
            case I2CBUS_ERROR:
                printf("Bus error!!!\n");
                GT_REG_READ(I2C_CONTROL,&TempData);
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/
 setI2cReady();/*Aravind*/

        clearIntFlag();
        return retval;

            case ADDR_WB_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                printf("Dummy Slave Address + Write Bit is transmitted and ACK is received\n");
                printf("The Offset : %x is being written\n",roffset);
#endif /* PMC280_I2C_DEBUG */
                GT_REG_WRITE(I2C_DATA,roffset); /* Word Address */
                clearIntFlag();
                while(!isIntFlagSet());
                GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
                switch(i2cStatus) {
                    case MASTER_DATA_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                        printf("Master transmitted word address and ACK is received\n");
#endif /* PMC280_I2C_DEBUG */
                        retval= true;
                        break;
                   case MASTER_DATA_TRA_NACK:
                        printf("Master transmitted word address and ACK is NOT received..\n");
                        retval = false;
                        break;
                   case MASTER_LOST_ARBTR_DURING_ADR_DATA:
                        printf("Master lost arbitration during dummy write\n");
                        retval = false;
                        break;
                }
        return retval;
            case ADDR_WB_TRA_NACK:
                printf("Address + Dummy Write Bit is transmitted and ACK is NOT received....\n");
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/

		 setI2cReady();/*Aravind*/
                clearIntFlag();
                sysMsDelay(20);
                GT_REG_WRITE(I2C_CONTROL,0x0);
                return retval;
        }
    }
    else {
    printf("I2C Re-Start Condition Fails.....\n");
    GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);    /* generate stop bit */
    sysMsDelay(20);
    GT_REG_WRITE(I2C_CONTROL,0x0);
    return retval;
    }
 setI2cReady();/*Aravind*/
    return retval;
}
/******************************************************************************
* Function    : frcEEPROMDummyWrite 
*   
* Description : This routine does "Dummy write" to the EEPROM device
*               specifically, for random read and sequencial read  
*  
* Inputs      : devAdd  - EEPROM device hardware address
*               roffset - Offset from where the next read command would 
*                         like to read from.
*
* Output      : true - If successful
*               false - If not successful in issuing the "dummy write"
* 
******************************************************************************/
bool frcEEPROMDummyWrite16(UINT8 devAdd, UINT16 roffset) {

    unsigned int i2cStatus,TempData=0x0;
    bool retval= false;
    UINT16 roffset1=roffset;

    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL =69.27KHz                                                 for 133MHz TClk */


setI2cBusy();/*Aravind*/

    GT_REG_WRITE(I2C_CONTROL,0x60); /* Start bit & enable bit*/
    while(!isIntFlagSet());
    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
    if(i2cStatus==0x08)    {
#ifdef PMC280_I2C_DEBUG
        printf("I2C Dummy Start Condition is succuessful\n");
        printf("Writes the Dummy Slave Address as :%x\n",devAdd);
#endif /* PMC280_I2C_DEBUG */
        GT_REG_WRITE(I2C_DATA,devAdd | 0x00); /* Slave Address,LSB r/w bit is 
                                                 cleared for WRITE */
        clearIntFlag();
        while(!isIntFlagSet());
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
        switch(i2cStatus) {
            case I2CBUS_ERROR:
                printf("Bus error!!!\n");
                GT_REG_READ(I2C_CONTROL,&TempData);
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/

 setI2cReady();/*Aravind*/

                clearIntFlag();
                return retval;

            case ADDR_WB_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                printf("Dummy Slave Address + Write Bit is transmitted and ACK is received\n");
                printf("The Offset : %x is being written\n",roffset);
#endif /* PMC280_I2C_DEBUG */
                roffset=(roffset&0xff00)>>8;
                GT_REG_WRITE(I2C_DATA,roffset); /* Word Address */
                clearIntFlag();
                while(!isIntFlagSet());
                GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
                switch(i2cStatus) {
                    case MASTER_DATA_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                        printf("Master transmitted Ist word address %x and ACK is received\n",roffset);
#endif /* PMC280_I2C_DEBUG */
                        roffset1=roffset1&0x00ff;
                        GT_REG_WRITE(I2C_DATA,roffset1);
                        clearIntFlag();
                        while(!isIntFlagSet());
                        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
                        switch(i2cStatus){
                            case MASTER_DATA_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                                printf("Master transmitted 2nd word address %x and ACK received\n",roffset1);
#endif/*PMC280_I2C_DEBUG*/
                                retval=true;
                                break;  
                            case MASTER_DATA_TRA_NACK:
                                printf("Master transmitted word address and ACK is NOT received..\n");
                                retval=false;
                                break;
                            case MASTER_LOST_ARBTR_DURING_ADR_DATA:
                                printf("Master lost arbritration during dummy write\n");
                                retval=false;
                                break; 
                    
                        }
				 setI2cReady();/*Aravind*/
                        return retval;
                    case MASTER_DATA_TRA_NACK:
                        printf("Master transmitted word address and ACK is NOT received..\n");
                        retval = false;
                        break;
                    case MASTER_LOST_ARBTR_DURING_ADR_DATA:
                        printf("Master lost arbitration during dummy write\n");
                        retval = false;
                        break;
                }
			 setI2cReady();/*Aravind*/
                return retval;
            case ADDR_WB_TRA_NACK:
                printf("Address + Dummy Write Bit is transmitted and ACK is NOT received....\n");
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/

		    setI2cReady();/*Aravind*/
                clearIntFlag();
                sysMsDelay(20);
                GT_REG_WRITE(I2C_CONTROL,0x0);
                return retval;
        }
    }
    else {
        printf("I2C Re-Start Condition Fails.....\n");
        GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);    /* generate stop bit */
	 setI2cReady();/*Aravind*/
        sysMsDelay(20);
        GT_REG_WRITE(I2C_CONTROL,0x0);
        return retval;
    }
     setI2cReady();/*Aravind*/
    return retval;
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

/******************************************************************************
*
* Function    : frcRTCRead
*
* Description : This function is to get the time and date from the RTC device. The 
*               slave address of RTC is always fixed(by the vendor) to 0xa0.The 
*               format of date/time is "hh:mm:ss:dd:mm:yy:ce:da".In this format the
*               data would be read and stored in the buffer passed to the routine.
*
* Inputs      : buffer - array where the date/time has to be stored.
*
* Output      : true  - If successful.
*               false - If not successsful.
*
*******************************************************************************/

void frcRTCRead(UINT8 * buffer) {

    UINT8 regfile[20],regFile1[4];
    int flag=0;

    do{
          flag=0;
          sysMsDelay(100);
          GT_REG_WRITE(I2C_SOFT_RESET,0x0); 
          sysMsDelay(40);   /* Restart the I2C controller */
 
          /* Do a Burst read */ 
          if(frcRTCDummyWrite(0xa0,0xbf))
          {                         
             frcI2CRead(0xa0,regfile,8);         
            buffer[0]=regfile[2]; /*hour*/
             buffer[1]=regfile[1]; /*min*/
             buffer[2]=regfile[0]; /*seconds*/
             buffer[3]=regfile[3]; /*date*/
              buffer[4]=regfile[4]; /*month*/
             buffer[5]=regfile[6]; /*year*/
             buffer[6]=regfile[5]; /*day*/

         }

         else 
         {         
              printf("RTCdummy write unsuccessful\n");   
         }
    
        
       sysMsDelay(40);   /* Restart the I2C controller */
       GT_REG_WRITE(I2C_SOFT_RESET,0x0); 
       sysMsDelay(40);   /* Restart the I2C controller */

       /* Read the century field */
       if(frcRTCDummyWrite(0xa0,0x93))
       {                        
          frcI2CRead(0xa0,regFile1,1);
          buffer[7]=regFile1[0];        
       }
       else 
       {
          printf("RTCdummy write unsuccessful\n");  
       }
       
       
       GT_REG_WRITE(I2C_SOFT_RESET,0x0); 
       sysMsDelay(40);   /* Restart the I2C controller */

      if(frcRTCDummyWrite(0xa0,0xbf))
      {                         
         frcI2CRead(0xa0,regfile,8);            
         if (buffer[5]!=regfile[6]) /*year*/
               flag=1;
   
      }
      else 
      {
         printf("RTCdummy write unsuccessful\n");
  
      } 
   }/*end do*/
  while(flag==1); 
                 
}

/*******************************************************************************
*
* Function    : frcRTCWrite
*
* Description : This function is to set the time and date of the RTC device. The
*               slave address of RTC is always fixed(by the vendor) to 0xa0.The
*               format of date/time is "hh:mm:ss:dd:mm:yy:ce:da".In this format the
*               data would be sent and stored in the slave.
*
* Inputs      : buffer - array where the date/time to be set is stored.
*
* Output      : true  - If successful.
*               false - If not successsful.
*
*******************************************************************************/

void frcRTCWrite(UINT8 * buffer) {
    UINT8 regfile[20]; 
    
    sysMsDelay(100);  
    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
    sysMsDelay(40);   /* Restart the I2C controller */

    regfile[0]=0x93;                     /* century   */
    regfile[1]=buffer[7];
    if (!frcI2CWrite(0xa0,regfile,2)) {
    printf("Century write failed\n");
    } 

    regfile[3]=buffer[0]; /*hour*/
    regfile[2]=buffer[1]; /*minutes*/ 
    
    regfile[1]=buffer[2]; /*seconds*/
    regfile[4]=buffer[3];  /*date*/
    regfile[5]=buffer[4]; /*month*/
    regfile[7]=buffer[5]; /*year*/
    regfile[6]=buffer[6]; /*day*/
    regfile[8]=0x00;      /*control register-write protect bit disabled*/
   
    
    regfile[0]=0xbe;

    sysMsDelay(100); 
    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
    sysMsDelay(40);   /* Restart the I2C controller */
   
    if (!frcI2CWrite(0xa0,regfile,9)) {
    printf("Burst write failed\n");
    } 
}

/******************************************************************************
* Function:     frcMACRead
*
* Description:  This function is to read the MAC addresses of the ethernet
*               ports.The address 0x1ff4 to 0x1fff is reserved in the EPROM for
*               the MAC addresses.
*
* Inputs:       portNo - Port number 0,1.
*               buffer - Pointer to an array where the MACs will be copied
*
* Outputs:      true  - If successful.
*               false - If not successful
*
**************************************** *************************************/


bool frcMACRead(UINT8 portNo, UINT8 *buffer)
{

    UINT8 devadd;
    int EEPROMSiz;

    devadd=BIBEEPROM;
    EEPROMSiz=EEPROMDetect(BIBEEPROM);
    if(EEPROMSiz != 64) {
        printf("64K BIB EEPROM not present !!\n");
        return(false);
    }

    if(portNo == 0) {
        if(frcEEPROMRead16(devadd,(UINT16)0x1ff4,6,buffer))
            return(true);
        else
            return(false);
    }
    else if(portNo == 1) {
        if(frcEEPROMRead16(devadd,(UINT16)0x1ffa,6,buffer))
            return(true);
        else
            return(false);
    }
    else {
        printf("Invalid Port Number !!\n");
        return(false);
    }
}

/******************************************************************************
* Function:     frcMACWrite
*
* Description:  This function is to program the MAC addresses of the ethernet
*               ports.The address 0x1ff4 to 0x1fff is reserved in the EPROM for
*               the MAC addresses.
*
* Inputs:       portNo - Port number 0,1,2.
*               MAC0   - Byte 0 of the MAC address.
*               MAC1   - Byte 1 of the MAC address.
*               MAC2   - Byte 2 of the MAC address.
*               MAC3   - Byte 3 of the MAC address.
*               MAC4   - Byte 4 of the MAC address.
*               MAC5   - Byte 5 of the MAC address.
*
* Outputs:      true  - If successful.
*               false - If not successful
*
**************************************** *************************************/

bool frcMACWrite(UINT8 portNo, UINT8 MAC0, UINT8 MAC1, UINT8 MAC2, 
                   UINT8 MAC3, UINT8 MAC4,UINT8 MAC5) {

    UINT8 regFile[7];
    bool ret = false;

    
    switch (portNo) {
    case 0:

        regFile[0]= MAC0;
        regFile[1]= MAC1;
        regFile[2]= MAC2;
        regFile[3]= MAC3;
        regFile[4]= MAC4;
        regFile[5]= MAC5;

        GT_REG_WRITE(I2C_SOFT_RESET,0x0);
        sysMsDelay(500);
         if(frcEEPROMWrite16(BIBEEPROM,0x1ff4,6,regFile) == true) {
#ifdef PMC280_I2C_DEBUG
            printf("Programmed MAC address %x-%x-%x-%x-%x-%x of Mgi Port 0\n",
                    MAC0,MAC1,MAC2,MAC3,MAC4,MAC5);
#endif /* PMC280_I2C_DEBUG */
                ret = true;
            }
        else
            printf("Could not program the MAC address\n"); 

        break;
    case 1:
        regFile[0]= MAC0;
        regFile[1]= MAC1;
            regFile[2]= MAC2;
        regFile[3]= MAC3;
        regFile[4]= MAC4;
        regFile[5]= MAC5;   

        GT_REG_WRITE(I2C_SOFT_RESET,0x0);
        sysMsDelay(500);
        if(frcEEPROMWrite16(BIBEEPROM,0x1ffa,6,regFile) == true) {
#ifdef PMC280_I2C_DEBUG
            printf("Programmed MAC address %x-%x-%x-%x-%x-%x of Mgi Port 1\n",
                    MAC0,MAC1,MAC2,MAC3,MAC4,MAC5);
#endif /* PMC280_I2C_DEBUG */
                    ret = true;
                }
        else
            printf("Could not program the MAC address\n");        
        break;    
    default:
        printf("Invalid Port Number\n");
    }
    
    return (ret);
}
/***************************************************************************** 
* Function:    EEPROMDetect
* 
* Description: Detects dynamically the size of the EEPROM using a simple 
*              technique of using the 16-bit EEPROMWrite and the 8-bit EEPROM
*              Write routines 
*
* Inputs:      devAdd - Hardware address of the EEPROM.
* 
* Returns:     Size of the EEPROM. 
******************************************************************************/
int EEPROMDetect(UINT8 devadd)
{
  
   UINT8 dat,val;
   UINT8 buffer[2];
   int EEPROMSiz=-1;     
   
   dat=0xdd;
   buffer[0]=dat;
   
   if (frcEEPROMWrite16(devadd,0x1230,1,buffer)) {
       if (frcEEPROMRead8(devadd,0x12,2,buffer))
       {
           val=buffer[1];
           if(val==dat)
               EEPROMSiz=2;
           else
               EEPROMSiz=64; 
       } 
       else
           printf("\nUnable to detect EEPROM Device\n");
   }
   
   return(EEPROMSiz);
}

bool frcRTCDummyWrite(UINT8 devAdd, unsigned int roffset) {

    unsigned int i2cStatus, TempData=0x0;
    bool retval= false;

    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL =69.27KHz                                                 for 133MHz TClk */

setI2cBusy();/*Aravind*/
    GT_REG_WRITE(I2C_CONTROL,0x60); /* Start bit & enable bit*/
    while(!isIntFlagSet());
    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
    if(i2cStatus==0x08)    {
#ifdef PMC280_I2C_DEBUG
        printf("I2C Dummy Start Condition is succuessful\n");
        printf("Writes the Dummy Slave Address as :%x\n",devAdd);
#endif /* PMC280_I2C_DEBUG */
        GT_REG_WRITE(I2C_DATA,devAdd | 0x00); /* Slave Address,LSB r/w bit is 
                                                 cleared for WRITE */
        clearIntFlag();
        while(!isIntFlagSet());
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
        switch(i2cStatus) {
            case I2CBUS_ERROR:
                printf("Bus error!!!\n");
                GT_REG_READ(I2C_CONTROL,&TempData);
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/


 setI2cReady();/*Aravind*/
        clearIntFlag();
        return retval;

            case ADDR_WB_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                printf("Dummy Slave Address + Write Bit is transmitted and ACK is received\n");
                printf("The Offset : %x is being written\n",roffset);
#endif /* PMC280_I2C_DEBUG */
                GT_REG_WRITE(I2C_DATA,roffset); /* Word Address */
                clearIntFlag();
                while(!isIntFlagSet());
                GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
                switch(i2cStatus) {
                    case MASTER_DATA_TRA_ACK:
#ifdef PMC280_I2C_DEBUG
                        printf("Master transmitted word address and ACK is received\n");
#endif /* PMC280_I2C_DEBUG */
                        retval= true;
                        break;
                   case MASTER_DATA_TRA_NACK:
                        printf("Master transmitted word address and ACK is NOT received..\n");
                        retval = false;
                        break;
                   case MASTER_LOST_ARBTR_DURING_ADR_DATA:
                        printf("Master lost arbitration during dummy write\n");
                        retval = false;
                        break;
                }
		
 		setI2cReady();/*Aravind*/
        return retval;
            case ADDR_WB_TRA_NACK:
                printf("Address + Dummy Write Bit is transmitted and ACK is NOT received....\n");
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/
		
 		setI2cReady();/*Aravind*/
                clearIntFlag();
                sysMsDelay(20);
                GT_REG_WRITE(I2C_CONTROL,0x0);
                return retval;
        }
    }
    else {
    printf("I2C Re-Start Condition Fails.....\n");
    GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);    /* generate stop bit */
    
 setI2cReady();/*Aravind*/
    sysMsDelay(20);
    GT_REG_WRITE(I2C_CONTROL,0x0);
    return retval;
    }
	
 setI2cReady();/*Aravind*/
    return retval;
}
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

