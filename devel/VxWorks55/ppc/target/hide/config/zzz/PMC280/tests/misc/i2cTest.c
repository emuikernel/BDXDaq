/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
*                                                                             *
*  File:        i2cTest.c                                                     *
*                                                                             *
*  Author:      Champak Nath                                                  *
*                                                                             *
*  Description: This is just a test file to test the I2C functionality on     *
*               PMC280.                                                           *
*                                                                             *
******************************************************************************/
#include "stdlib.h"
#include "stdio.h"
#include "./../../i2cDrv.h"
#include "string.h"
#include "ctype.h" 

void frcEEPROMTestWrite(UINT8);
void frcEEPROMTestRead(UINT8);
IMPORT int parse8(void);
IMPORT int parse16(void);
IMPORT UINT8 ctohex1(char);
IMPORT int EEPROMDetect(UINT8);

 #if 0 
void frcEEPROMTestWrite(UINT8 devAdd)
{
 unsigned int i,temp;
 unsigned int noBytes;
 UINT offset;
 UINT8 *buffer;
 int EEPROMSiz;	     
     
   EEPROMSiz=EEPROMDetect(devAdd);
     
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
        
	   frcEEPROMWrite8(devAdd,(UINT8)offset,noBytes,(UINT8 *)buffer);
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
	    frcEEPROMWrite16(devAdd,(UINT16)offset,noBytes,(UINT8 *)buffer);
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
       
	    if(frcEEPROMRead8(devAdd,(UINT8)offset,noBytes, regFile)){
	        for (i=0;i<noBytes;i++)
		  printf("\nThe Read data byte # %d : %x",i,regFile[i]);
               printf("\n");   
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
   	      if(frcEEPROMRead16(devAdd,(UINT16)offset,noBytes, regFile)){
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

    printf("Enter century:");
    scanf("%x",&temp);
    buffer[6] = (UINT8) temp;

    printf("Enter day:");
    scanf("%x",&temp);
    buffer[7] = (UINT8) temp;

    frcRTCWrite(buffer);
}

void frcTestRTCRead () {
    UINT8 buffer[8];
    int i;

    frcRTCRead(buffer);
 
    for (i=0;i<8;i++) 
        printf("%02x:",buffer[i]);
    printf("\n");
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
                     if((isxdigit((int)temp[0+j])) && (isxdigit((int)temp[1+j])) &&
                        (isxdigit((int)temp[2+j])) )
                         flag=3;
                     else
                         flag =0;
                 }
                 else
                     {
                     if(temp[4+j] == '\0') {
                      	 if((isxdigit((int)temp[0+j])) && (isxdigit((int)temp[1+j])) &&
                        	(isxdigit((int)temp[2+j]))  && (isxdigit((int)temp[3+j]))) 
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
#endif 