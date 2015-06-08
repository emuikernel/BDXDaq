/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#include "stdio.h"
#include "i2cDrv.h"
#include "ftpLib.h"
#include "vxWorks.h"
#include "ioLib.h"
#include "stdlib.h"

IMPORT int EEPROMDetect(UINT8);
IMPORT UINT8 ctohex(char c);
IMPORT bool protectPCI;
    
#define MAX_LEN 4*1024

unsigned int frcCheckSum(unsigned int *, unsigned int );

void frcPCIDataWrite(char * FTPSERVER_IP_ADRS,char * filename,UINT8 DevAdd)
{
  int j,count=0,realcount=0,ctrlSock,dataSock,i,m,k;
  int nBytes, image_size = 0,countf=0,p,q;
  char LOGIN_ID[30],PASSWORD[30];   
  char buf2[MAX_LEN],buf1[MAX_LEN],c,dump,dump1;
  UINT8 dat[20],dat1[20],c1,c2;
  UINT16 offset=0;
  int eepromsiz;
  char *buf;
  char *h;
  unsigned int checksum = 0;
 
  
  	printf("User:");
	scanf("%s",LOGIN_ID);
	printf("Password:");
	scanf("%s",PASSWORD);
 	buf = (char *)malloc(MAX_LEN);       
		if ( buf == NULL )
			return;

       if (ftpXfer (FTPSERVER_IP_ADRS, LOGIN_ID, PASSWORD, "",
                 "RETR %s", "", filename,
                  &ctrlSock, &dataSock) == ERROR)
                return;
	h=buf;
	while ((nBytes = read (dataSock, buf, 1024)) > 0)
         {
            printf("%d bytes transferred\n", nBytes);
            buf += nBytes;
            image_size += nBytes;
         }                                    

          if (nBytes < 0)             /* read error? */
              return;

          if (ftpReplyGet (ctrlSock, TRUE) != FTP_COMPLETE)
              return;

          if (ftpCommand (ctrlSock, "QUIT", 0, 0, 0, 0, 0, 0) != FTP_COMPLETE)
              return;
            close (dataSock);
            close (ctrlSock);

          printf("Image Size = 0x%08x bytes.\n", image_size);
          checksum = frcCheckSum((unsigned int *) h, 
                           image_size/4);    
          printf("Checksum is 0x%08x\n", checksum);
                    
     
         i=-1;                 
          do{              
   	      c=h[++i];
              if ((c==' ')||(c=='\t')||(c=='\v')||(c=='\b')||(c=='\n')||(c=='\r'))
                dump=c;
              else
                {                   
                   buf1[count]=c;
                   count++;
                }
              image_size--;
            }while(image_size>0);
         count--;
     
    realcount=0;
    if((buf1[1]=='x')||(buf1[1]=='X'))
      {                
         for(i=0;i<count;i=i+10)
          {
             j=i+2;
             m=i;
             dump1=buf1[m];
             dump1=buf1[++m];
             for (k=0;k<8;k++,j++)
             {
               buf2[realcount]=buf1[j];
               realcount++;
             }
          }               
        }
     else
       {         
         for(i=0;i<count;i++)
          {
            buf2[realcount]=buf1[i];
            realcount++;
          }
      }
             
       countf=0;
       p=0;
       q=0;
       do
         { p=q;              
           q=++p;           
            if(((buf2[p]=='F')||(buf2[p]=='f'))&&((buf2[q]=='F')||(buf2[q]=='f')))                       
                    countf++;                                           
            else
                    countf=0;
          }
        while(countf<16);
        realcount=++p;     
               
        eepromsiz=EEPROMDetect(DevAdd);
         
        for(i=0;i<realcount;i=i+2)
         {
            m=i;
            c=buf2[m];
            c1=ctohex(c);
            c=buf2[++m];
            c2=ctohex(c);
            dat[0]=c1*0x10+c2;
            printf("%4x",dat[0]); 
            if(eepromsiz==2)
               do
                {
                  frcEEPROMWrite8(DevAdd,(UINT8)offset,1,dat);
                  frcEEPROMRead8(DevAdd,(UINT8)offset,1,dat1);
                }
               while(dat[0]!=dat1[0]);
            else if(eepromsiz==64)
               do
                {
                  frcEEPROMWrite8(DevAdd,offset,1,dat);
                  frcEEPROMRead8(DevAdd,offset,1,dat1);
                }
               while(dat[0]!=dat1[0]);

            offset++;
         }       
                    
     printf("\n Successfully programmed PCI Boot data on the EEPROM %x\n",DevAdd); 
 }

unsigned int frcCheckSum(unsigned int *start, unsigned int size)
{
    unsigned int checksum = 0;
    while(size > 0)
    {
        checksum +=  *start;
#ifdef DEBUG
        printf("[0x%08x] %08x\n", start, *start);
#endif
        start++;
        size--;
    }
    return checksum;
}
