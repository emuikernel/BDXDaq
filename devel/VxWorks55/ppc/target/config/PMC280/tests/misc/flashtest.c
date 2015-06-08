/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                                                    *
*                                                                                                                                       *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.                  *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT   *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION          *
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.                 *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,  *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.    *
*******************************************************************************
*                                                                                                                                       *
* Test utilities for User Flash                                                                                         */
#include <stdio.h>
#include <vxWorks.h>
#include "../main/frctestconfig.h"
#include "./../../flashDrv.h"

extern UINT32 flashBlocks;

int frcFlashBlockErase();
int getOption (void);
void frcFlashRead();
int frcFlashBlockWrite();
void *malloc();
void *memset();
void *free();


/****************************************************************************
* Function     : frcFlashCheck
*
* Description  : Flash test routine
*
* Inputs       : None  
*
* RETURNS      : None
****************************************************************************/

#if 0
STATUS
frcTestUserFlash (void)
{
  int   BlockNo,i,exc,dummy1,dummy2;
  UINT8 choice;

  UINT32  size,initData,  *arr, *arrT;



exc = frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK0);

if(exc != FLASH_SUCCESS)
                {
                printf(" frcFlashAutoSelect error");
                return ERROR;
                }
size = (flashBlocks*0x40000) / 0x100000;        
printf("\n Flash size:%dMB",size);
                  


  while (1)
      {
      printf ("\n0 - Exit\n");
        printf ("\n1 - Block Erase Verify");
        printf ("\n2 - Block Write Verify");

        
        choice = getOption();

        printf ("\nEnter Flash Block No.(0-%d) : ",(size*4-1));
      BlockNo = getOption();
      arrT = arr = (UINT32 *) malloc (SECSIZE);

        switch (choice)
            {
            case 1:
                     	memset( arrT, 0xff,SECSIZE);
                     	verifyBlock (FLASH_BANK0 + BlockNo * SECSIZE,arrT);
                     	break;
            case 2:
              	printf ("Enter the initial data : ");
	             initData = getOption();

             	 for (i = 0; i <SECSIZE; i++)
                  		{
                    		*(arrT) = initData;
                    		arrT++;
                    		initData = initData + 0x01010101;
                  		}
              	exc = frcFlashBlockWrite (BlockNo, FLASH_BANK0, arr);
              	free (arr);
              	printf("\nWrite");
              
              	if (exc != OK)
                        		printf (" failed");
	              else
             	             {
                        		printf ("successeful");
                        		printf("\nVerifying write");
                        		verifyBlock(FLASH_BANK0 + BlockNo * SECSIZE,arrT);
                              	}

	                break;
                default:
                        printf("\nUnknown option.");
            }

      }

}

#endif
/***************************************************
startAdd - Start address of block in User Flash
cmp - Pointer to a memory location
*************************************************/
#if 0
LOCAL STATUS verifyBlock (unsigned int startAdd, UINT32 *cmp)
{
  UINT32 *mem;
  UINT32 no,flag;
  
  mem = (UINT32 *) malloc (SECSIZE);
  frcFlashRead (startAdd, SECSIZE / 4, (UINT32 *) mem);
  printf(" read done."); 
  flag = OK;
  for (no = 0; no < SECSIZE / 4; no++)
      {
        if (*(mem + no) != *(cmp+no))
                  {
                  printf ("E");
                  flag = ERROR;
                }
        else;
      }

if (flag == ERROR)
        printf(" successful.");
else
        printf("error.");
return flag;

}
#endif


#undef USER_FLASH_DBG

#ifdef USER_FLASH_TEST
extern UINT32 flashBlocks;
STATUS  frcTestUserFlash(void)
{
          int exit = 0,exc,bank,Flashbase,dummy1,dummy2,BlockNo,size,i;
          UINT8 choice;
          unsigned int initData,startAdd, *arr, *arrT,FlashAddress;
          char choice1[30];  
          int blkNo=0, status1=0, numBlks=0,numBlks1=0; /*added by vijay*/
          printf("\nDetecting BANK 0 at 0x%x..... ",FLASH_BANK0);
          exc = frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK0);
          if(exc == FLASH_SUCCESS)
          {
                size = (flashBlocks*SECSIZE) / 0x100000;        
                printf("DONE,%d MB",size);
          }        
          else
                printf("FAILED");        
          printf("\nDetecting BANK 1 at 0x%x..... ",FLASH_BANK1);
          exc = frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK1);
          if(exc == FLASH_SUCCESS)
          {
                size = (flashBlocks*SECSIZE) / 0x100000;
                printf("DONE,%d MB",size);
          }           
          else
                printf("FAILED");        
        


          while(!exit)
          {

          printf("\nChoose FLASH BANK to test (0 or 1), ");
          bank = getOption();
          if ( bank > 2  || bank < 0)
          		{
          		printf("\nIncorrect bank number");
          		continue;
          		}
          printf("\nEntered selection for FLASH BANK %d",bank); 
          if(bank == 0)
                  Flashbase = FLASH_BANK0;
          else
                Flashbase = FLASH_BANK1;

                printf("\n   =====Choose the option =====");
                printf("\n1 - Erase Flash Block");
                printf("\n2 - Read Flash Block");
                printf("\n3 - Write Flash Block");
                printf("\n4 - Erase complete Flash Bank");
                printf("\n5 - Lock Flash Block");
                printf("\n6 - Unlock complete Flash Bank"); 
                printf("\n7 - Unlock Flash Block");
                printf("\n8 - Write data to more than one Flash Block"); 
                printf("\n9 - Exit\n");           
                choice = getOption();
                printf("\n");
                switch(choice)
                {
                        case 1:
                                printf("\nEnter Flash Block No.(0-127) : ");
                                BlockNo = getOption();
		 if ( BlockNo < 0 || BlockNo> 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                                exc = frcFlashBlockErase(BlockNo,Flashbase);
                                if(exc == FLASH_SUCCESS)
                                        printf("FLASH BANK %d BLOCK %d ERASE ----- PASSED",bank,BlockNo);                                                        
                                else
                                        printf("FLASH BANK %d BLOCK %d ERASE ----- FAILED",bank,BlockNo);                                
        
                                break;
                        case 2:
                            printf("\nEnter Flash Block No.(0-127) : ");
                            BlockNo = getOption();
                             if ( BlockNo < 0 || BlockNo > 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                            startAdd = Flashbase + BlockNo * 0x40000; 
                            frcFlashRead(startAdd,0x40000/4,(UINT32*)0xa00000);
                            printf("FLASH BANK %d BLOCK %d READ ----- DONE AND COPIED AT 0xa00000",bank,BlockNo);                                                        
                            break;
                        case 3:
                                printf("\nEnter Flash Block No.(0-127) : ");
                                BlockNo = getOption();
		      if ( BlockNo < 0 ||BlockNo > 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                                printf("Enter the initial data, in Hex : ");
                                scanf("%x",&initData);
                                arrT= arr = (UINT32 *)malloc(256 * 1024);
                                for(i=0;i<0x10000;i++) {
                                    *(arrT) = initData;
                                    arrT++;                                    
                                }                       
                                exc = frcFlashBlockWrite(BlockNo,Flashbase,arr);
                                free(arr);
                                if(exc == FLASH_SUCCESS)
                                        printf("\nFLASH BANK %d BLOCK %d WRITE ----- PASSED",bank,BlockNo);                                                        
                                else
                                        printf("\nFLASH BANK %d BLOCK %d WRITE ----- FAILED",bank,BlockNo);

                                break;
                        case 4:
                                
                                printf("Are you sure(y\\n)\n");
                                scanf("%s",choice1);
                                                                                             
                                if(choice1[0]=='n')
                                       break;
                                else if(choice1[0]=='y'){  
                                /* Unlock the flash, before erasing */
                                if (frcFlashUnlock((unsigned int *)Flashbase) == 0) {
                                    printf("Could not unlock the flash\nExiting Block erase\n");
                                    break;
                                } 
                                #if 0
                                sysMsDelay(1000); /* Bug Fix */
                                #endif
                                exc = frcFlashErase(0,Flashbase);
                                if(exc == FLASH_SUCCESS)
                                        printf("\nCOMPLETE FLASH BANK %d ERASE ----- PASSED",bank);                                                        
                                else
                                        printf("\nCOMPLETE FLASH BANK %d ERASE ----- FAILED",bank);
                                }                
                                break;
                        case 5:                                 
                                printf("\nEnter Flash Block No.(0-127) : ");
                                BlockNo = getOption();
                                if ( BlockNo < 0 || BlockNo > 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                                FlashAddress = Flashbase + BlockNo * 0x40000;
                                exc =  frcFlashLock ((unsigned int *) FlashAddress);
                                if(exc == 1)
                                        printf("\nFLASH BANK %d BLOCK %d LOCK ----- PASSED",bank,BlockNo);                                                        
                                else
                                        printf("\nFLASH BANK %d BLOCK %d LOCK ----- FAILED",bank,BlockNo);
                                break;
                       case 6:
                       	numBlks = flashBlocks;
#ifdef USER_FLASH_DBG
                            printf("\nTotal number of blocks:%d \n",numBlks);
#endif /*USER_FLASH_DBG*/
                       	
                       	for(blkNo =0; blkNo < numBlks;blkNo++)
                       	{
                       	    FlashAddress = Flashbase + blkNo * 0x40000;
                                exc =  frcFlashUnlock ((unsigned int *) FlashAddress);
                                if(exc == 1)
                                {
#ifdef USER_FLASH_DBG
                                        printf("\nFLASH BANK %d UNLOCK ----- PASSED, at block:%d",bank,blkNo);
#else
                                        printf("-");
#endif /*USER_FLASH_DBG*/
                                }
                                else
                                {
                                        status1 = -1;
                                        printf("\nFLASH BANK %d UNLOCK ----- FAILED, at block:%d",bank,blkNo);
                                                                                
                                }
                       	}
                       	if( -1 == status1)
                       	{
                                printf("\nUNLOCK COMPLETE  FLASH BANK %d  is FAILED",bank);
                       	}
                       	else if(0 == status1)
                       	{
                                printf("\nUNLOCK COMPLETE  FLASH BANK %d  is SUCCESS ",bank);
                       	}
                       	else
                       		printf("Test result is in unknown state \n");
                       	
                            break;            
                      case 7:
                      	    printf("\nEnter Flash Block No.(0-127) : ");
                                BlockNo = getOption();
                                if ( BlockNo < 0 || BlockNo > 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                                FlashAddress = Flashbase + BlockNo * 0x40000;
                                exc =  frcFlashUnlock ((unsigned int *) FlashAddress);
                                if(exc == 1)
                                        printf("\nFLASH BANK %d BLOCK %d UNLOCK ----- PASSED",bank,BlockNo);                                                        
                                else
                                        printf("\nFLASH BANK %d BLOCK %d UNLOCK ----- FAILED",bank,BlockNo);
                                break;
                        case 8:
                                printf("\nEnter starting Flash Block No.(0-127) : ");
                                BlockNo = getOption();
                 	        if ( BlockNo < 0 ||BlockNo > 127 )
                                	{
                                	printf("\nInvalid block number %d",BlockNo);
                                	continue;
                                	}
                                printf("Enter number of Blocks, data to be written:");
                                numBlks1 = getOption();
                                printf("Enter the initial data, in Hex : ");
                                scanf("%x",&initData);
                                arrT= arr = (UINT32 *)malloc(256 * 1024);
                                for(i=0;i<0x10000;i++) {
                                    *(arrT) = initData;
                                    arrT++;                                    
                                }                      
                                for(i=0;i<numBlks1;i++)
                                {
                                    exc = frcFlashBlockWrite(BlockNo+i,Flashbase,arr);
                                    if(exc == FLASH_SUCCESS)
                                        printf("\nFLASH BANK %d BLOCK %d WRITE ----- PASSED",bank,BlockNo+i);                                                        
                                    else
                                        printf("\nFLASH BANK %d BLOCK %d WRITE ----- FAILED",bank,BlockNo+i);
                                }
                                free(arr);

                                break;
                                                            
                      case 9:          
                                exit = 1;
                                break;
                       default:
                       		printf("\nInvalid choice:%d",choice);
                                                         
                }
                
          }

return OK;                
}

#else
 #warning "User Flash test not included."

#endif /*USER_FLASH_TEST */

