/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
********************************************************************************/
#include "stdlib.h"
#include "stdio.h"
#include "i2cDrv.h"
#include "string.h"
#include "ctype.h" 
#include "frcBib.h"
#include "frcBibDrv.h"
#ifdef ROHS
IMPORT BIB_INTF bibgIntf;/* Global BIB data structure interface */

IMPORT bool bibInit(void );
#endif
IMPORT void    sysMsDelay      (UINT );
void frcEEPROMTestWrite(UINT8);
void frcEEPROMTestRead(UINT8);
void frcTestRTCWrite ();
void frcTestRTCRead ();
void frcRTCRead(UINT8 *);
void frcRTCWrite(UINT8 *);
bool frcMACRead(UINT8, UINT8 *);
bool frcMACWrite(UINT8, UINT8, UINT8, UINT8 , UINT8 , UINT8 ,UINT8);
bool frcMACWrite8(UINT8, UINT8, UINT8, UINT8 , UINT8 , UINT8 ,UINT8);
bool frcMACWrite16(UINT8, UINT8, UINT8, UINT8 , UINT8 , UINT8 ,UINT8);
int parse8(void);
int parse16(void);
UINT8 ctohex1(char);
int EEPROMDetect(UINT8);
void testEEPROMwork();
int frcTestI2C_EEPROMNew(void);
int  frcTestMACAddr1(void);
int atox1(char str[]);


void frcEEPROMTestWrite(UINT8 devAdd)
{
 unsigned int i,temp;
 unsigned int noBytes;
 UINT offset;
 UINT8 *buffer;
 int EEPROMSiz;	     
     
   EEPROMSiz=EEPROMDetect(devAdd);
   
   printf("\n The EEPROM SIZE IS : %d", EEPROMSiz); 
     
     if(EEPROMSiz==2)
        {
           printf("\nEnter the 8-bit offset,in hex:");          
           offset = parse8();	   
           if(((offset<0x80)||(offset>0xff))&&((devAdd==EEPROM1)||(devAdd==EEPROM2))){
             printf("The offset should be within the range 0x80 and 0xff\n");
             return;
           }
           if(devAdd==BIBEEPROM){
               printf("This device is used for the storage of BIB information\n");
               return;
           }
                         
           printf("\nEnter the number of bytes:");
           scanf("%d",&noBytes);
           if (noBytes > 256){
	     printf("Maximum number of bytes that can be written is 256\n");
	     return;
	   }
           if((0x100-offset)<noBytes){
              printf("Space not sufficient in the EEPROM device\n"); 
              return;
           } 
	   buffer = (UINT8 *)malloc(noBytes * sizeof(UINT8));
	   for(i=0;i<noBytes;i++) {
		printf("In Hex, the data byte %d - ",i);
		temp= parse8();
                buffer[i]=(UINT8)temp;
 	   }
	   frcEEPROMWrite8 (devAdd,(UINT8)offset,noBytes,(UINT8 *)buffer);
	   free(buffer);
         }
    else if(EEPROMSiz==64)
         {
            printf("\nEnter the 13-bit offset,in hex:");          
            offset = parse16(); 
            if(((offset<0x0080)||(offset>0x1fff))&&((devAdd==EEPROM1)||(devAdd==EEPROM2))){
             printf("The offset should be within the range 0x0080 and 0x1fff\n");
             return;
             }                 
	    if(((offset<0x200)||((0x1ff4<=offset)&&(offset<=0x1fff)))&&(devAdd==BIBEEPROM)){
              printf("The offset should be within the range 0x0200 and 0x1ff3\n");
              return;
            }           
            printf("\nEnter the number of bytes:");
            scanf("%d",&noBytes);
            if (noBytes > 8192){
	      printf("Maximum number of bytes that can be written is 8192\n");
	      return;
	    }
            if (devAdd==0xa8)
            {
	        if((0x1ff4-offset)<noBytes){
                        printf("Space not sufficient in the EEPROM device\n"); 
                        return;
                }
            }
            else
            {
                 if((0x2000-offset)<noBytes){
                        printf("Space not sufficient in the EEPROM device\n"); 
                        return;
                }
            }   
 	    buffer = (UINT8 *)malloc(noBytes * sizeof(UINT8));
	    for(i=0;i<noBytes;i++) {
		 printf("In Hex, the data byte %d - ",i);
	         temp= parse8();
                 buffer[i]=(UINT8) temp;
	    }        
	    frcEEPROMWrite8 (devAdd,(UINT16)offset,noBytes,(UINT8 *)buffer);
	    free(buffer);
         }
   else
     printf("\nEEPROM device absent\n");
}

void frcEEPROMTestRead(UINT8 devAdd)
{
 unsigned int i;
 unsigned noBytes; 
 UINT offset;
 UINT8  *regFile;
 int EEPROMSiz;	
     
     EEPROMSiz=EEPROMDetect(devAdd);
     if(EEPROMSiz==2)
        {
            printf("\nEnter the 8-bit offset, in hex:");         
            offset=parse8();	          
	    if((offset<0x00)||(offset>0xff)){
               printf("\nThe offset should be within the range 0x00 and 0xff\n");
               return;
            }                     
            printf("\nEnter the number of bytes:");
            scanf("%d",&noBytes);
            if (noBytes > 256){
	       printf("\nMaximum number of bytes that can be read is 256\n");
	       return;
	    }
	    if((0x100-offset)<noBytes){
                printf("\nCan't read from EEPROM device\n"); 
                return;
            } 
	    regFile = (UINT8 *) malloc(noBytes * sizeof(UINT8));

       	    if(frcEEPROMRead8 (devAdd,(UINT8)offset,noBytes, regFile)){
            for (i=0;i<noBytes;i++)
		  printf("\nThe Read data byte # %d : %x",i,regFile[i]);
	    free(regFile);
          }
      }
    else if(EEPROMSiz==64)
         {
              printf("\nEnter the 13-bit offset,in Hex:");                
              offset=parse16();                    		
	      if((offset<0x0000)||(offset>0x1fff)){
                  printf("The offset should be within the range 0x0000 and 0x1fff\n");
                  return;
              }                    
              printf("\nEnter the number of bytes:");
              scanf("%d",&noBytes);
              if (noBytes > 8192){
	         printf("Maximum number of bytes that can be read is 8192\n");
	         return;
	      }
	      if((0x2000-offset)<noBytes){
                 printf("\nCan't read from  EEPROM device"); 
                 return;
              } 
              regFile = (UINT8 *) malloc(noBytes * sizeof(UINT8));
              if(frcEEPROMRead8 (devAdd,(UINT16)offset,noBytes, regFile)){
                  for (i=0;i<noBytes;i++)
                     printf("The Read data byte # %d : %x\n",i,regFile[i]);
                  free(regFile);
              }
        }
     else
       printf("\nEEPROM device absent\n");
}

void frcTestRTCWrite () {
    UINT8 buffer[8];
    unsigned int temp;

    printf("Enter hour:");
    scanf("%x",&temp);
    buffer[0] = (UINT8) temp;

    printf("Enter minutes:");
    scanf("%x",&temp);
    buffer[1] = (UINT8) temp;

    printf("Enter seconds:");
    scanf("%x",&temp);
    buffer[2] = (UINT8) temp;

    printf("Enter date:");
    scanf("%x",&temp);
    buffer[3] = (UINT8) temp;

    printf("Enter month:");
    scanf("%x",&temp);
    buffer[4] = (UINT8) temp;

    printf("Enter year:");
    scanf("%x",&temp);
    buffer[5] = (UINT8) temp;

    printf("Enter day:");
    scanf("%x",&temp);
    buffer[6] = (UINT8) temp;
    
    printf("Enter century:");
    scanf("%x",&temp);
    buffer[7] = (UINT8) temp;

    mvRTCWrite (buffer);
}

void frcTestRTCRead () {
    UINT8 buffer[8];
    int i;

   mvRTCRead(buffer);
 
    for (i=0;i<8;i++) 
        printf("%02x:",buffer[i]);
    printf("\n");
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
  
    mvRTCRead (buffer);

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

void frcRTCWrite (UINT8 * buffer) {

    mvRTCWrite(buffer);    
}


int parse8()
{
  char temp[50];
  int offset=-1,flag=0,j;
  UINT8 c1,c2;

   scanf("%s",temp);
   if(strlen(temp)>4)
      return(offset);

   if((temp[1]=='x')||(temp[1]=='X'))
             j=2;
         else
             j=0;
       
     
      if(temp[1+j]=='\0') { 
             if(isxdigit((int)temp[0+j]))
                  flag=1;
             else
                  flag=0;
         }
         else {
             if (temp[2+j] == '\0') {
                 if((isxdigit((int)temp[0+j])) && (isxdigit((int)temp[1+j])))
                      flag=2;
                 else
                      flag =0;
             }
           else
              flag=0;
     }
          
    if (flag==1)
      {
         c1=ctohex1(temp[0+j]);                  
         offset=c1;
      }
     else if(flag==2){                   
                    	c1=ctohex1(temp[0+j]);
                    	c2=ctohex1(temp[1+j]);
                   	offset=c1*16+c2;
                    }
   
         return(offset);  	
     
       
 }

int parse16( )
{  
  char temp[50];
  int offset=-1,flag=0,j;
  UINT8 c1,c2,c3,c4;
 
       scanf("%s",temp);
       if(strlen(temp)>6)
         return(offset);
       
       if((temp[1]=='x')||(temp[1]=='X'))
             j=2;
         else
             j=0;
     
        if(temp[1+j]=='\0') { 
             if(isxdigit((int)temp[0+j]))
                  flag=1;
             else
                  flag=0;
         }
         else {
             if (temp[2+j] == '\0') {
                 if((isxdigit((int)temp[0+j])) && (isxdigit((int)temp[1+j])))
                      flag=2;
                 else
                      flag =0;
             }
             else {
                 if(temp[3+j] == '\0') {
                     if((isxdigit((int)temp[0+j])) &&(isxdigit((int)temp[1+j]))
                          && (isxdigit((int)temp[2+j])) )
                         flag=3;
                     else
                         flag =0;
                 }
                 else
                     {
                     if(temp[4+j] == '\0') {
                      	 if((isxdigit((int)temp[0+j])) && 
                            (isxdigit((int)temp[1+j])) &&
                            (isxdigit((int)temp[2+j])) && 
                            (isxdigit((int)temp[3+j]))) 
                        	flag=4;
                    	 else
                         	flag =0;
                 	}
               else
                
                    flag = 0;
             }
        }
       
    }
    if (flag==1)
      {
         c1=ctohex1(temp[0+j]);                  
         offset=c1;
      }
     else if(flag==2){                   
                       c1=ctohex1(temp[0+j]);
                       c2=ctohex1(temp[1+j]);
                       offset=c1*16+c2;
                     }
                    else if(flag==3){
                                       c1=ctohex1(temp[0+j]);
                                       c2=ctohex1(temp[1+j]);
                                       c3=ctohex1(temp[2+j]);
                                       offset=c1*256+c2*16+c3;
                                     }
                          else if(flag==4){
					     c1=ctohex1(temp[0+j]);
                                             c2=ctohex1(temp[1+j]);          
         				     c3=ctohex1(temp[2+j]);          
         				     c4=ctohex1(temp[3+j]); 
					     offset=c1*4096+c2*256+c3*16+c4;	        
                                          }                                                                     
             
  
         return(offset);  	
  } 
       
UINT8 ctohex1(char c)
{  
    UINT8 hex=0x00;

    if((c>0x2F)&&(c<0x3A))
    hex=(unsigned short)c-0x30;
    if(c=='a')
    hex=0x0a;
    if(c=='b')
    hex=0x0b;
    if(c=='c')
    hex=0x0c;
    if(c=='d')
    hex=0x0d;
    if(c=='e')
    hex=0x0e;
    if(c=='f')
    hex=0x0f;
    return(hex);

}


/******************************************************************************
* Function:     frcMACRead
*
* Description:  This function is to read the MAC addresses of the Wancom
*               ports. The address 0x00 to 0x11 is reserved in the EPROM for
*               the MAC addresses.
*
* Inputs:       portNo - Wancom port number 0,1,2.
*               buffer - Pointer to an array where the MACs will be copied
*
* Outputs:      true  - If successful.
*               false - If not successful
*
**************************************** *************************************/


bool frcMACRead (UINT8 portNo, UINT8 *buffer)
{

    
    
   

    char ret=false;	
#ifdef ROHS
char *bibData=NULL;
 int	j,k;

		if(bibInit()==OK)
		      {
			bibData = (char *)malloc(20);
			frcBibElemRead(BIBEEPROM,BIB_EthernetAdrs,0,bibData,14);
			switch(portNo){

					case 0:
							for(j=0;j<7;j++)
								*(buffer +j) = *(bibData + j +1);
						break;
						
					case 1:
							k=0;
							for(j=7;j<13;j++,k++)
								*(buffer +k) = *(bibData +j);
						break;
					default:
							printf("\nInvalid port\n");
							ret = false;
							return(false);
						
			}
			free(bibData);
			if(frcBibDetach(BIBEEPROM)== OK)
				{
					ret = true;
				}
				else 
				{
				ret = false;
					}
		}
#else 
		{
			UINT8 devadd;
			devadd=BIBEEPROM;
		    

		     
		        if(portNo == 0) {
		            if(frcEEPROMRead8 (devadd,(UINT16)0x1ff4,6,buffer))
		                ret=true;
		            else
		                 ret = false;
		        	}
		        else if(portNo == 1) {
		            if(frcEEPROMRead8 (devadd,(UINT16)0x1ffa,6,buffer))
		                 ret = true;
		           else
		                 ret = false;;
		        }
		        else {
		            printf("Invalid Port Number !!\n");
		            ret = false;;
		        }
		    }
#endif 	 
return ret;
		
}

/******************************************************************************
* Function:     frcMACWrite
*
* Description:  This function is to program the MAC addresses of the Wancom
*               ports. The address 0x00 to 0x11 is reserved in the EPROM for
*               the MAC addresses.
*
* Inputs:       portNo - Wancom port number 0,1,2.
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
bool  frcMACWrite (UINT8 portNo, UINT8 MAC0, UINT8 MAC1, UINT8 MAC2, UINT8 MAC3,
               UINT8 MAC4,UINT8 MAC5)

{ 
   
   
   char ret = false;
   
#ifdef ROHS
char *bibData=NULL;
if(bibInit()==OK)
{
	bibData = (char *)malloc(20);
	frcBibElemRead(BIBEEPROM,BIB_EthernetAdrs,0,bibData,14);
	switch(portNo){
			case 0:
					*(bibData +1) = MAC0;
					*(bibData +2) = MAC1;
					*(bibData +3) = MAC2;
					*(bibData +4) = MAC3;
					*(bibData +5) = MAC4;
					*(bibData +6) = MAC5;
				break;
			case 1:
					*(bibData +7) = MAC0;
					*(bibData +8) = MAC1;
					*(bibData +9) = MAC2;
					*(bibData +10) = MAC3;
					*(bibData +11) = MAC4;
					*(bibData +12) = MAC5;
			break;
			default:
					printf("\nInvalid port\n");
	}
	if(frcBibElemWrite(BIBEEPROM,BIB_EthernetAdrs,0,bibData)== OK)
		ret = true;
	else
		ret = false;
	free(bibData);
	if(frcBibDetach(BIBEEPROM)== OK)
		{
			ret =true;
		}
	else 
		{
			ret= false;
		}
	} 
#else
     
    UINT8 regFile[7];
   

    
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
         if(frcEEPROMWrite8(BIBEEPROM,0x1ff4,6,regFile) == true) {
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
        if(frcEEPROMWrite8(BIBEEPROM,0x1ffa,6,regFile) == true) {
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
    
    
#endif
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
   buffer[0]=0xdd;
  
   if (frcEEPROMWrite16 (devadd,0x1230,1,buffer))
   {
    if (frcEEPROMRead8 (devadd,0x12,2,buffer))
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

bool frcRTCDummyWrite1(UINT8 devAdd, unsigned int roffset) {

    unsigned int i2cStatus, TempData=0x0;
    bool retval= false;

    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,0x2c); /* M=5, N=4 so that SCL =69.27KHz                                                 for 133MHz TClk */

    GT_REG_WRITE(I2C_CONTROL,0x60); /* Start bit & enable bit*/
    while(!mvI2cReadIntFlag());
    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
    if(i2cStatus==0x08)    {
#ifdef DEBUG
        printf("I2C Dummy Start Condition is succuessful\n");
        printf("Writes the Dummy Slave Address as :%x\n",devAdd);
#endif /* K2_I2C_DEBUG */
        GT_REG_WRITE(I2C_DATA,devAdd | 0x00); /* Slave Address,LSB r/w bit is 
                                                 cleared for WRITE */
        mvI2cClearIntFlag();
        while(!mvI2cReadIntFlag());
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
        switch(i2cStatus) {
            case I2C_BUS_ERROR:
                printf("Bus error!!!\n");
                GT_REG_READ(I2C_CONTROL,&TempData);
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/
        mvI2cClearIntFlag();
        return retval;

            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
#ifdef K2_I2C_DEBUG
                printf("Dummy Slave Address + Write Bit is transmitted and ACK is received\n");
                printf("The Offset : %x is being written\n",roffset);
#endif /* K2_I2C_DEBUG */
                GT_REG_WRITE(I2C_DATA,roffset); /* Word Address */
                mvI2cClearIntFlag();
                while(!mvI2cReadIntFlag());
                GT_REG_READ(I2C_STATUS_BAUDE_RATE,&i2cStatus);
                switch(i2cStatus) {
                    case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
#ifdef K2_I2C_DEBUG
                        printf("Master transmitted word address and ACK is received\n");
#endif /* K2_I2C_DEBUG */
                        retval= true;
                        break;
                   case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
                        printf("Master transmitted word address and ACK is NOT received..\n");
                        retval = false;
                        break;
                   case I2C_MAS_LOST_ARB_DURING_ADDR_OR_DATA_TRA:
                        printf("Master lost arbitration during dummy write\n");
                        retval = false;
                        break;
                }
        return retval;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
                printf("Address + Dummy Write Bit is transmitted and ACK is NOT received....\n");
                GT_REG_WRITE(I2C_CONTROL,TempData & 0x10);/*generate stop bit*/
                mvI2cClearIntFlag();
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
    return retval;
}
