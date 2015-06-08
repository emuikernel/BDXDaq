/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#include"vxWorks.h"
#include"stdio.h"
#include"stdlib.h"
#include"sysLib.h"
#include"i2cDrv.h"
#include"frcBib.h"
#include"frcBibDrv.h"
#include "ftpLib.h"
#include "ioLib.h"

IMPORT int EEPROMDetect(UINT8);
IMPORT unsigned int frcCheckSum(unsigned int *start, unsigned int size);
IMPORT bool protectBIB; 

STATUS	bibReadIntf	(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pDstBuf);
STATUS	bibWriteIntf	(BIB_HDL handle,  UINT32 offset,
				UINT32  byteCnt, void  *pSrcBuf);
UINT8 ctohex(char);

STATUS	bibWriteIntf(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pDstBuf)
{  
   UINT8 handle1;
   UINT16 offset1;
   UINT8 bytecnt1;
   UINT8 *buffer;
   int eepromsiz;
     eepromsiz=EEPROMDetect(BIBEEPROM);
    handle1=(UINT8)handle;
    offset1=(UINT16)offset;
    bytecnt1=(unsigned int)byteCnt;
    
     buffer=(UINT8 *)pDstBuf;
    if(eepromsiz==2){
 
       			if (frcEEPROMWrite8(handle1,(UINT8)offset1,bytecnt1,buffer))
         			{
                                  printf("EEPROM WRITE SUCCESSFUL\n");
	   			  return(OK);
 				}
        		 else
         			{
                                 printf("EEPROM WRITE UNSUCCESSFUL\n");
           			 return(ERROR);
				} 
                    }
    else if (eepromsiz==64){
     			if (frcEEPROMWrite16(handle1,offset1,bytecnt1,buffer))
      				{
                                 printf("EEPROM WRITE SUCCESSFUL\n");
				 return(OK);
				}
     			else
      				{
                                 printf("EEPROM WRITE UNSUCCESSFUL\n");
       				 return(ERROR);
				}
  		          }
    else
       return(ERROR);
}

STATUS	bibReadIntf(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pSrcBuf)
{  UINT8 handle1;
   UINT16 offset1;
   unsigned int bytecnt1;
   UINT8 *buffer;
   int eepromsiz;
    eepromsiz=EEPROMDetect(BIBEEPROM);
    handle1=(UINT8)handle;
    offset1=(UINT16)offset;
    bytecnt1=(unsigned int)byteCnt;
    
     buffer=(UINT8 *)pSrcBuf;
     if (eepromsiz==2){
        	if (frcEEPROMRead8(handle1,(UINT8)offset1,bytecnt1,buffer))
          		{
			 printf("EEPROM READ SUCCESSFUL\n");
	   		 return(OK);
 			}
       		else
          		{
			 printf("EEPROM READ UNSUCCESSFUL\n");
          		 return(ERROR);
			}
                    }
      else if(eepromsiz==64){
              	if (frcEEPROMRead16(handle1,offset1,bytecnt1,buffer))
                    		{
		     		printf("EEPROM READ SUCCESSFUL\n");
	             		return(OK);
                    		}
             		    else
                   		{
		     		printf("EEPROM READ UNSUCCESSFUL\n");
                     		return(ERROR);
                		} 
                   	  }
      else
         return(ERROR); 
  }

/******************************************************************************
 frcBibDataWrite() - Function that writes BIB data into the EEPROM using 
                     a bib-file created using bib-tools like the mk_bib and 
                     the upd_bib 
******************************************************************************/
void frcBibDataWrite(char *FTPSERVER_IP_ADRS,char *directory,char *filename)
{  
  int j,count,realcount,ctrlSock,dataSock,i;
  int nBytes, image_size = 0;
  char cd,dump,LOGIN_ID[30],PASSWORD[30];
  UINT16 offset = 0;
  UINT8 c1,c2,c3,c4;
  UINT8 dat[45],buf1[45];
  int eepromsiz; 
  char *buf = (char *) malloc(1024*sizeof(char));
  char *h;
  unsigned int checksum = 0;

   
        eepromsiz=EEPROMDetect(BIBEEPROM); 
        protectBIB = false;
     	printf("User:");
	scanf("%s",LOGIN_ID);
	printf("Password:");
	scanf("%s",PASSWORD);
        
       if (ftpXfer (FTPSERVER_IP_ADRS, LOGIN_ID, PASSWORD, "",
                 "RETR %s", directory, filename,
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
              while(1)
                 {
                    dump=h[++i];                    
                    dump=h[++i];
                    
                    if(dump=='9')
	              break;          
          
                     cd=h[++i];                     
                     c1=ctohex(cd);
                     cd=h[++i];                     
                     c2=ctohex(cd);
                     count=c1*0x10+c2;
                     realcount=(count-3)*2;
                                          
                     cd=h[++i];
                     c1=ctohex(cd);
                     cd=h[++i];
                     c2=ctohex(cd);
                     cd=h[++i];
                     c3=ctohex(cd);
                     cd=h[++i];
                     c4=ctohex(cd);
       
                     offset=c1*4096+c2*256+c3*16+c4;                     

                       for(j=0;j<realcount;j=j+2)
	                 { 
	                     cd=h[++i];	  
	                     c1=ctohex(cd);
	                     cd=h[++i];
	                     c2=ctohex(cd);
                             dat[0] = c1*0x10+c2;
 	   
                             printf("%4x",dat[0]);
                             if(eepromsiz==2)          	              	   
                               do{      	   
                    		   frcEEPROMWrite8(BIBEEPROM,offset,1,dat);
                   		   frcEEPROMRead8(BIBEEPROM,offset,1,buf1);
                  	         } 
               		          while(dat[0]!=buf1[0]);
                             else if(eepromsiz==64)
                              do{      	   
                                  frcEEPROMWrite16(BIBEEPROM,offset,1,dat);
                                  frcEEPROMRead16(BIBEEPROM,offset,1,buf1);
                                 } 
                                  while(dat[0]!=buf1[0]);
                            offset++;		  	

	                 }        	 
	       dump=h[++i];
               dump=h[++i];
               dump=h[++i];
               dump=h[++i];
               printf("\n");
           }
       printf("\nSucessfully programmed BIB information in to BIB EEPROM\n");            
     protectBIB = true;
 }

UINT8 ctohex(char c)
{  UINT8 hex=0x00;

   if((c>0x2F)&&(c<0x3A))
    hex=(unsigned short)c-0x30;
    if((c=='A')||(c=='a'))
    hex=0x0a;
    if((c=='B')||(c=='b'))
    hex=0x0b;
    if((c=='C')||(c=='c'))
    hex=0x0c;
    if((c=='D')||(c=='d'))
    hex=0x0d;
    if((c=='E')||(c=='e'))
    hex=0x0e;
    if((c=='F')||(c=='f'))
    hex=0x0f;
    return(hex);

}      
