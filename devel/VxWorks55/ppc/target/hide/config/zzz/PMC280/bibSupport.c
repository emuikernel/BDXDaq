/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  INC - ECC OR ANY THIRD PARTY. MOTOROLA  INC - ECC RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
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

#undef DEBUG
#define UINT88 unsigned char

IMPORT int EEPROMDetect(UINT8);
IMPORT unsigned int frcCheckSum(unsigned int *start, unsigned int size);
IMPORT bool protectBIB; 
IMPORT void *memset();
IMPORT void sysMsDelay();
bool bibInit();
BIB_INTF bibgIntf;/* Global BIB data structure interface */
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
   int ret = ERROR;
     eepromsiz=EEPROMDetect(BIBEEPROM);
    handle1=(UINT8)handle;
    offset1=(UINT16)offset;
    bytecnt1=(unsigned int)byteCnt;
    
     buffer=(UINT8 *)pDstBuf;
    if(eepromsiz==2){
 
       			if (frcEEPROMWrite8(handle1,(UINT8)offset1,bytecnt1,buffer))
         			{
#ifdef DEBUG
                                  printf("\nEEPROM WRITE 8 SUCCESSFUL"); 
#endif
	   			  ret = OK;
 				}
        		 else
         			{
                                 printf("\nEEPROM WRITE8 UNSUCCESSFUL"); 
           			 ret = ERROR;
				} 
                    }
    else if (eepromsiz==64){
				
					
						
     			if (frcEEPROMWrite8(handle1,(UINT16)offset1,bytecnt1,buffer))
      				{

					
					
						

				 ret = OK;
				}
     			else
      				{
                                 printf("EEPROM WRITE 16 UNSUCCESSFUL\n");
       				ret = ERROR;
				}
  		          }
    
       return(ret);
}

STATUS	bibReadIntf(BIB_HDL handle,  UINT32 offset,
				 UINT32  byteCnt, void  *pSrcBuf)
{  UINT8 handle1;
   UINT16 offset1;
   unsigned int bytecnt1;
   UINT8 *buffer;
  static  int eepromsiz;
   static char eepromSizeTrue =0;
   if (eepromSizeTrue ==0)
   	{
    eepromsiz=EEPROMDetect(BIBEEPROM);
	eepromSizeTrue =1;
   	}
    handle1=(UINT8)handle;
    offset1=(UINT16)offset;
    bytecnt1=(unsigned int)byteCnt;
    
     buffer=(UINT8 *)pSrcBuf;
     if (eepromsiz==2){
        	if (frcEEPROMRead8(handle1,(UINT8)offset1,bytecnt1,buffer))
          		{
#ifdef DEBUG
                                  printf("EEPROM READ 8 SUCCESSFUL\n");
#endif
	   		 return(OK);
 			}
       		else
          		{
			 printf("EEPROM READ 8 UNSUCCESSFUL\n");
          		 return(ERROR);
			}
                    }
      else if(eepromsiz==64){
              	if (frcEEPROMRead8(handle1,offset1,bytecnt1,buffer))
                    		{
 #ifdef DEBUG
                                  printf("EEPROM READ 8 SUCCESSFUL\n");
#endif
	             		return(OK);
                    		}
             		    else
                   		{
		     		printf("EEPROM READ 8 UNSUCCESSFUL\n");
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
  

   printf ("\nWARNING : You are about to Change the Board Specific DATA\n");
  
   
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
                    
                    if(dump=='7')
	              break;          
          
                     cd=h[++i];                     
                     c1=ctohex(cd);
                     cd=h[++i];                     
                     c2=ctohex(cd);
                     count=c1*0x10+c2;
                     realcount=32;
                                          
                     cd=h[++i];
                     c1=ctohex(cd);
                     cd=h[++i];
                     c2=ctohex(cd);
                     cd=h[++i];
                     c3=ctohex(cd);
                     cd=h[++i];
                     c4=ctohex(cd);


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
                                  frcEEPROMWrite8(BIBEEPROM,offset,1,dat);
                                  frcEEPROMRead8(BIBEEPROM,offset,1,buf1);
                                 } 
                                  while(dat[0]!=buf1[0]);
                            offset++;		  	

	                 }        	 
	       dump=h[++i];
               dump=h[++i];
               dump=h[++i];
               dump=h[++i];
              count  = 0;
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
bool bibInit (){

      bibgIntf.bibReadIntfRtn  = bibReadIntf;
      bibgIntf.bibWriteIntfRtn =bibWriteIntf;
      bibgIntf.pMemPool = NULL;
      bibgIntf.memPoolSize = 0;

	if(frcBibAttach(BIBEEPROM,&bibgIntf)!=OK){
		printf("\nBib Attached failed\n");
		return ERROR;
	}

	return OK;
}

	


void frcBibDataWriteBin(char *FTPSERVER_IP_ADRS,char *directory,char *filename){


	int j,ctrlSock,dataSock;
	int nBytes, image_size = 0;
	char LOGIN_ID[30],PASSWORD[30];
	UINT16 offset = 0;

	UINT8 dat[45],buf1[45];

	char *buf = (char *) malloc(1024*sizeof(char));
	char *h;
	unsigned int checksum = 0;
	int eepromsiz=0;

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

	   for(j=0;j<nBytes;j++){

		    dat[0] = *(buf+j);
		    printf("%4x",dat[0]);
                 if(eepromsiz==2)          	              	   
                               do{      	   
                    		   frcEEPROMWrite8(BIBEEPROM,offset,1,dat);
                   		   frcEEPROMRead8(BIBEEPROM,offset,1,buf1);
                  	         } 
               		          while(dat[0]!=buf1[0]);
                             else if(eepromsiz==64)
                              do{      	   
                                  frcEEPROMWrite8(BIBEEPROM,offset,1,dat);
                                  frcEEPROMRead8(BIBEEPROM,offset,1,buf1);
                                 } 
                                  while(dat[0]!=buf1[0]);
		    offset++;

	    }
			
	
}















