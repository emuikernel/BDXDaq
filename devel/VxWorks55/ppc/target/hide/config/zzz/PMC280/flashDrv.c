/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************
*                                                                             *
* flashDrv.c - Source File for User Flash Memory Driver.                      *
*                                                                             *
* DESCRIPTION:                                                                *
*     This driver supports different flash memory devices for the user flash. *
*     It assumes there are 1, 2, or 4 flash chips and they are each accessed  *
*     as 16 bits (no 8 bit modes). Therefore, a single access  might be a     *
*     total of 16 bits, 32 bits, or 64 bits.                                  *
*                                                                             *
* DEPENDENCIES:                                                               *
*       None.                                                                 *
*                                                                             *
******************************************************************************/

#include "vxWorks.h"
#include "ioLib.h"
#include "stdio.h"
#include "config.h"
#include "flashDrv.h"
#include "stdlib.h"
#include "ctype.h" 
#include "gtCore.h"
#include "frcBoardId.h"

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */


#undef  DEBUG        /* - debug Compile */

#undef DBG_FW        /* Debug Flash Whole TEst XXXX */

IMPORT void    sysMsDelay  (UINT );
IMPORT UINT32  vxMsrGet    (void);
IMPORT void    vxMsrSet    (UINT32);
UINT8   option  (UINT8, UINT8);
UINT8   option1  (void);
IMPORT UINT8 frcGetBoardRev (void);
IMPORT int gtFlashUnlock(unsigned int sectorBaseAddr);

struct ID {
  unsigned short VenId;
  unsigned short DevId;
  UINT32     blocks;
} id [] =

{
  { 0x00b0, 0x00d0, FLASH_BLOCKS_SHARP320 },        /* SHARP LH28F320SKTD */
  { 0x0089, 0x0016, FLASH_BLOCKS_INTELF320},        /* Intel F320/ 32Mbit  */

  { 0x0089, 0x0017, FLASH_BLOCKS_INTELF640},        /* Intel F640/ 64Mbit  */
  { 0x0089, 0x0018, FLASH_BLOCKS_INTELF128},        /* Intel F128/ 128Mbit  */
  { 0x0089, 0x0002, FLASH_BLOCKS_INTELF128},        /* Intel F128K3/ 128Mbit  */
  { 0x0089, 0x001c, FLASH_BLOCKS_INTELP30 } 		/*Intel p30 */
};

unsigned int         num_flash_dev = sizeof (id) / sizeof (struct ID);
unsigned int         flash_index;
unsigned int         flash_width = 32;


int             debug_flash_width = -1;

void             (*flash_write)(unsigned int, unsigned int) = NULL;
void             (*flash_read)(unsigned int, unsigned int) = NULL;
UINT32            flashBlocks = 0;


extern void         __WRITE_FLASH64 (unsigned int, unsigned int);
extern void         __READ_FLASH64 (unsigned int, unsigned int);

void             __WRITE_FLASH32 (unsigned int fAddr, unsigned int fData)
{
  *((unsigned int *) fAddr) = *((unsigned int *) fData);
}

void             __READ_FLASH32 (unsigned int fAddr, unsigned int fData)
{
  *((unsigned int *) fData) = *((unsigned int *) fAddr);
}

void             __WRITE_FLASH16 (unsigned int fAddr, unsigned int fData)
{
  *((unsigned short *) fAddr) = *((unsigned short *) fData);
}

void             __READ_FLASH16 (unsigned int fAddr, unsigned int fData)
{
  *((unsigned short *) fData) = *((unsigned short *) fAddr);
}

unsigned int getFlashBase(unsigned int blockNo,unsigned int Flashbase);
    /* Pattern maker for local buffer */
#define LOCALBUF_PAT(val)    (val + (chipBlk >> 8))
#define USERFBUF_SIZE    0x00010000  
unsigned char usrflashBuf[USERFBUF_SIZE];



/****************************************************************************
*                                                                           *
* Function        : frcFlashReadRst                                         *
*                                                                           *
* Description    : This routine resets the flash into Read Mode             *
*                                                                           *
* Inputs        : Base Address of the Flash Device                          *
*                                                                           *
* RETURNS        : None                                                     * 
****************************************************************************/


void frcFlashReadRst (unsigned int Flashbase)
{
  unsigned short FlashCmdC [4] = 
    { FLASH_CMD_CLEARSTATAW, FLASH_CMD_CLEARSTATAW,
      FLASH_CMD_CLEARSTATAW, FLASH_CMD_CLEARSTATAW
    };
  unsigned short FlashCmdR[4] = 
    { FLASH_CMD_RESET, FLASH_CMD_RESET,
      FLASH_CMD_RESET, FLASH_CMD_RESET
    };
  unsigned int flashAdr;
  unsigned int flashDat;
 
  if (flash_width == 0)
    return;

  /* clear status register */
  flashAdr = Flashbase;
  flashDat = (UINT) &FlashCmdC[0];
  (*flash_write) (flashAdr, flashDat);

 EIEIO_SYNC;

  /* reset */
  flashAdr = Flashbase;
  flashDat = (UINT) &FlashCmdR[0];
  (*flash_write) (flashAdr, flashDat);     

  EIEIO_SYNC;

}


/****************************************************************************
*                                                                           *
* Function        : frcFlashAutoSelect                                      *     
*                                                                           *
* Description    : This routine returns a Manufactor code and Device code   *
*                                                                           *
* Inputs        : pointers to return the Manufactor and Device codes        * 
*                                                                           *
* Output        : flash codes at input pointers.                            *
*                                                                           *        
* RETURNS        : Success / Fail                                           *
****************************************************************************/

int frcFlashAutoSelect 
    (
    unsigned * Mnfct, 
    unsigned * DevCode,
    unsigned int Flashbase    
    )
    {
    volatile unsigned char *FlashAddress;
    unsigned short FlashCmd [4] = 
      { FLASH_CMD_DATA90, FLASH_CMD_DATA90, 
        FLASH_CMD_DATA90, FLASH_CMD_DATA90
      };
    unsigned int flashAdr;
    unsigned int flashDat;
    unsigned short data [4];
    
 
    FlashAddress = (UCHAR *) Flashbase;

               
    /* Send Autoselect command */
    flashAdr = Flashbase;
    flashDat = (UINT) &FlashCmd[0];
    __WRITE_FLASH64(flashAdr,flashDat) ;     

    /* Read Manufacture and device code */

    flashDat = (UINT) &data [0];
    __READ_FLASH64((unsigned) &FlashAddress[0],flashDat);
    *Mnfct = (unsigned) data [0] & 0xff;
    
    /* detect data width */
    flash_width = 16;
    if (data [0] == data [1])
      flash_width = 32;
    if (data [0] == data [2])
      flash_width = 64;

   if(flash_width == 16)
     __READ_FLASH16((unsigned) &FlashAddress[2],flashDat);

   if(flash_width == 32)
     __READ_FLASH32((unsigned) &FlashAddress[4],flashDat);    

   if(flash_width == 64)
     __READ_FLASH64((unsigned) &FlashAddress[8],flashDat);
    
    *DevCode = (unsigned) data [0] & 0xff;    
    

#ifdef DEBUG 
    if(frcWhoAmI()) {
        dbg_printf1 ("Mfr ID = %02x\n", *Mnfct);
        dbg_printf1 ("Device ID = %02x\n", *DevCode);
    }
    else {
        dbg_printf0 ("Mfr ID = %02x\n", *Mnfct);
        dbg_printf0 ("Device ID = %02x\n", *DevCode);
    }
#endif 

    for (flash_index = 0; flash_index < num_flash_dev; flash_index++)
     {
      if (*Mnfct == id [flash_index].VenId && 
              *DevCode == id [flash_index].DevId)
       {        
        flashBlocks = id [ flash_index].blocks;
        break;
       }
     }


    if (flash_index == num_flash_dev) /* device not found */
    {
#ifdef DEBUG
    if(frcWhoAmI()) {
      dbg_puts1 ("Device not found or there is no flash device installed\n");
      dbg_puts1 ("Trying anyway ...... ");
    }
    else {
      dbg_puts0 ("Device not found or there is no flash device installed\n");
      dbg_puts0 ("Trying anyway ...... ");
    }
#endif
      return(FLASH_TIMEOUT);    

    }

#ifdef DEBUG    
    if (debug_flash_width == -1)
      printf ("Bus width is auto detected = %d bits\n", flash_width);
    else
    {
      printf ("Bus width is configured to = %d bits\n", debug_flash_width);
      flash_width = (unsigned int) debug_flash_width;
    }
#endif

    if (flash_width == 64)
    {
      flash_write = __WRITE_FLASH64;
      flash_read = __READ_FLASH64;
    }
    else if (flash_width == 32)
    {
      flash_write = __WRITE_FLASH32;
      flash_read = __READ_FLASH32;
    }
    else if (flash_width == 16)
    {
      flash_write = __WRITE_FLASH16;
      flash_read = __READ_FLASH16;
    }

    /* Back to read mode */
    frcFlashReadRst (Flashbase);
    return(FLASH_SUCCESS);    

    }


/****************************************************************************
* Function        : compare_error                                           *
*                                                                           *        
* Description    : This routine compares the two data passed to it and      *
*                  returns 1 if there is an error, otherwise 0              * 
*                                                                           *
*                                                                           *
* Inputs        : Start addresses of two data to be compared                * 
*                                                                           *
* RETURNS        : Success / Fail                                           *
*                                                                           *    
****************************************************************************/

int compare_error (unsigned short *flash, unsigned short *mem)
{
  switch (flash_width)
  {
    case 16:
      if (flash [0] != mem [0])
        return 1;
      break;
    case 32:
      if (flash [0] != mem [0] || flash [1] != mem [1])
        return 1;
      break;
     default:
      break;
  }
  return 0;
}

/****************************************************************************
* Function        : operation_complete                                      *
*                                                                           *        
* Description    : This routine gives the status of operation of a Flash    *
                  Device                                                    *
*                                                                           * 
*                                                                           *
*                                                                           *
* Inputs        : Start addresses of the polled status, Device Value        *
*                  to be compared                                           * 
*                                                                           *
* RETURNS        : 1 in case of complete otherwise 0                        *
*                                                                           *    
****************************************************************************/

int operation_complete (unsigned short *status, unsigned short value)
{
  switch (flash_width)
  {
    case 16:
      if (status [0] == value)
        return 1;
      else
        return 0;
      break;
    case 32:
      if (status [0] == value && status [1] == value)
        return 1;
      else
        return 0;
      break;
    case 64:
      if (status [0] == value && status [1] == value &&
          status [2] == value && status [3] == value)
        return 1;
      else
        return 0;
      break;
    default:
      return 0;
      break;
  }
}

/****************************************************************************
* Function        : operation_fail                                            *
*                                                                            *        
* Description    : This routine gives the status of operation of a Flash     *
                  Device                                                    *
*                                                                            * 
*                                                                            *
*                                                                            *
* Inputs        : Start addresses of the polled status, Device Value        *
*                  to be compared                                            * 
*                                                                            *
* RETURNS        : 1 in case of fail, otherwise 0                            *
*                                                                            *    
****************************************************************************/


int operation_fail (unsigned short *status, unsigned short value)
{
  switch (flash_width)
  {
    case 16:
      if ((status [0] & value) == value)
        return 1;
      else
        return 0;
      break;
    case 32:
      if ((status [0] & value) == value)
        return 1;
      if ((status [1] & value) == value)
        return 1;
      return 0;
      break;
    case 64:
      if ((status [0] & value) == value)
        return 1;
      if ((status [1] & value) == value)
        return 1;
      if ((status [2] & value) == value)
        return 1;
      if ((status [3] & value) == value)
        return 1;
      return 0;
      break;
    default:
      return 1;
      break;
  }
}

/****************************************************************************
* Function        : print_staus                                               *
*                                                                            *        
* Description    : This routine prints the status messages for Debugging     *
*                                                                            *
*                                                                            * 
*                                                                            *
*                                                                            *
* Inputs        : pointer to message string and pointer of status to be     *
*                  displayed                                                 * 
*                                                                            *
* RETURNS        : None                                                      *
*                                                                            *    
****************************************************************************/


void print_status (char *mss, unsigned short *status)
{
  printf ("%s", mss);
  switch (flash_width)
  	
  {
    case 16:
      printf ("%04x\n", status [0]);
      break;
    case 32:
      printf ("%04x %04x\n", status [0], status [1]);
      break;
    case 64:
      printf ("%04x %04x ", status [0], status [1]);
      printf ("%04x %04x\n", status [2], status [3]);
      break;
    default:
      printf ("\n");
      break;
  }
}

/****************************************************************************
* Function        : frcFlashProgram                                            *
*                                                                            *        
* Description    : This routine writes flash as double words (32 bits).        *
*                  Therefore, it will round down all addresses to boundary    *
*                  of  4.                                                    * 
*                                                                            *
* Inputs        : Starting addr of flash to be programmed,                    *
*                  Start addr of source data,                                *
*                  Word Count.                                                *    
*                                                                            *
* RETURNS        : FLASH_SUCCESS if OK;                                        *
*                                                                            *    
****************************************************************************/

short frcFlashProgram
    (
    volatile unsigned *Address_32,    /* Addr of Flash to start */ 
    unsigned *data_32,            /* Addr of source data */
    unsigned int size,            /* # bytes to program */
    unsigned int Flashbase
    )
    {
 	
	
   
    UINT32 lpCt;
    unsigned int done;
    unsigned short poll [4];
    short exitval;
    unsigned int flashDat, times_16, num_16;
    unsigned short *data, *Address;

    unsigned short writeSetup [4] = 
      { FLASH_CMD_SETUPPROW, FLASH_CMD_SETUPPROW,
        FLASH_CMD_SETUPPROW, FLASH_CMD_SETUPPROW
      };
    unsigned short readStat [4] = 
      { FLASH_CMD_READSTATW, FLASH_CMD_READSTATW,
        FLASH_CMD_READSTATW, FLASH_CMD_READSTATW
      };
    
    exitval = FLASH_SUCCESS;
    done = 0;

	
    /* Round down,4byte boundary */
    if ( (unsigned) Address_32 & 0x03)
    Address_32 = (unsigned *) ( (unsigned) Address_32 & 0xfffffffC ) ;

    Address = (unsigned short *) Address_32;


    data = (unsigned short *) data_32;

#ifdef DEBUG
   printf ("flashProgram: faddr 0x%x  src addr 0x%x ct = 0x%x",
        (UINT32) Address , (UINT32) data , size);
#endif

    if (flash_width == 0)
      return FLASH_TIMEOUT;

    else if (flash_width == 64)
    /* Flash width = 64 bits, so 8 bytes per access */
      times_16 = size / 8;
    else if (flash_width == 32)
    /* Flash width = 32 bits, so 4 bytes per access */ 
     times_16 = size / 4;
    else
    /* Flash width = 16 bits, 2 bytes/access */
     times_16 = size / 2;

    /* # of 16 bits in width = num_16 */
    num_16 = flash_width / 16;




#ifdef DEBUG
   printf ("flashProgram: times16  0x%x  num_16  0x%x   flash_width = 0x%x",
        (UINT32)times_16, (UINT32) num_16 , flash_width );
#endif

   /* -------program flash loop -------------------------- */


    /* Write data */
    for (lpCt = 0; lpCt < times_16; lpCt++)
    {

        /* Write program command */

       (*flash_write) ((unsigned) &Address[lpCt << 1], (unsigned) &writeSetup[0]); /* 32 bit data width*/    
       EIEIO_SYNC;

               
       (*flash_write) ((unsigned) &Address[lpCt << 1], (unsigned) &data[lpCt * num_16]);    
        EIEIO_SYNC;

       if ((lpCt & 0xFFFFF) == 0)
          printf (".");

    
      /* Data polling */
       while (1)
     {
/*#ifdef NOTNEEDED*/
      (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
          flashDat = (UINT) &poll[0];
          EIEIO_SYNC;
/*#endif*/


         (*flash_read) (Flashbase, (unsigned) &poll[0]) ;    /* Read status register */
         EIEIO_SYNC;
              

#ifdef DEBUG		 
          print_status("\nProgram Status Reg =  ", poll);
#endif


        if (operation_complete (poll, FLASH_CMD_DATA80))
        break;
        
    
    }


   } /* End for write data */


 /* Last time - before exit - check full status */


       

           (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
           flashDat = (UINT) &poll[0];

       EIEIO_SYNC;

           (*flash_read) (Flashbase, (unsigned) &poll[0]);  /* Read status register */

       EIEIO_SYNC;

              if (operation_fail (poll, FLASH_CMD_DATA10))
       {
            printf("\nERROR Flash Program unsuccessful");
            exitval = FLASH_TIMEOUT;
       }
       if (operation_fail (poll, FLASH_CMD_DATA02))
       {
               printf("\nERROR Flash Program unsuccessful");
               exitval = FLASH_TIMEOUT;
       }
       if (operation_fail (poll, FLASH_CMD_DATA08))
       {
               printf("\nERROR Flash Program unsuccessful");
               exitval = FLASH_TIMEOUT;
       }    



#ifdef DEBUG
   printf ("flashProgram: Exit   lpct = 0x%x ", lpCt);
#endif

    
  /* Reset Flash */
  frcFlashReadRst (Flashbase);
#ifdef DEBUG
printf("\n Flash Program Complete");
#endif
  return (exitval);
}



unsigned short FlashCmdS [4] = 
  { FLASH_CMD_SETUPERAW, FLASH_CMD_SETUPERAW,
    FLASH_CMD_SETUPERAW, FLASH_CMD_SETUPERAW
  };
unsigned short FlashCmdE [4]= 
  { FLASH_CMD_ERASCONFALL, FLASH_CMD_ERASCONFALL,
    FLASH_CMD_ERASCONFALL, FLASH_CMD_ERASCONFALL
  };



/****************************************************************************
* Function        : frcFlashErase  
* 
* Description    : This routine erases all of the flash main blocks. 
* 
* 
* Inputs        : Sector No, Base Address 
* 
* RETURNS        : FLASH_SUCCESS if OK; 
* 
****************************************************************************/

short frcFlashErase (short blocks,unsigned int Flashbase)
{
  volatile unsigned *FlashAddress;
  unsigned dummy1, dummy2;
  unsigned short poll [4];
  unsigned once ;
  unsigned chipBlk =0;
  unsigned ix ;
  short done;
  short exit ;
  char index=0;

  unsigned int flashDat=0;
#ifdef ROHS
 unsigned int  retry = 5;
#endif 
  unsigned short readStat [4] = 
    { FLASH_CMD_READSTATW, FLASH_CMD_READSTATW,
      FLASH_CMD_READSTATW, FLASH_CMD_READSTATW
    };
 
  unsigned int BlockNo,Flashaddress,BlockLockcode,Mnfct,DevCode;
  UINT32 flashSize;

  exit = FLASH_SUCCESS;

#ifdef DEBUG
 printf ("Erasing flash. . .\n");
 printf("In frcFlashErase Flash base address = %x\n",Flashbase);
#endif

  chipBlk =  0;        /* current offset in flash */

  /*  Make sure we have correct flash selected */
  frcFlashAutoSelect (&dummy1, &dummy2,Flashbase);

  flashSize = flashBlocks *  FLASH_MAX_BLOCK_SIZE ;     /* Total addressable bytes in this flash */

#ifdef ROHS
 	flashBlocks = flashBlocks ;
	flashSize = ((flashBlocks-3) *  FLASH_MAX_BLOCK_SIZE );
#endif
	printf (" \n flash size = %x",flashSize);
	printf (" \n Please Wait for around 18 Mins\n");
	for (BlockNo=0;BlockNo<flashBlocks;BlockNo++)
	{
#ifdef ROHS   
		Flashaddress = getFlashBase(BlockNo, Flashbase);
		
	
#else
    	Flashaddress = Flashbase + BlockNo * 0x40000;
#endif
    		
    	
 	   	frcFlashReadIdCommand (&Mnfct,&DevCode,&BlockLockcode, Flashaddress);
  	  	if(BlockLockcode == 0x1) 
		{
	        	printf("Erase unsuccesful, block is locked at address %x!!\n",Flashaddress);
	        	return(FLASH_TIMEOUT);
    	  	}

	}
#ifdef ROHS
  frcFlashAutoSelect (&dummy1, &dummy2,Flashbase);
#endif

 for (ix=0;chipBlk < flashSize;ix++)
 {
  /* Set Flash Addr for this set of chips */

#ifdef ROHS 
RETRY:  
#endif 
  FlashAddress =  (unsigned *)(Flashbase + chipBlk) ;

  /*printf ("\n flash base add inside the for loop = %x retry = %d ",FlashAddress,retry);*/
  flashDat = (UINT) &FlashCmdS[0];
  (*flash_write) ((unsigned) FlashAddress, flashDat);

   EIEIO_SYNC;
  
  flashDat = (UINT) &FlashCmdE[0];
  (*flash_write) ((unsigned) FlashAddress, flashDat);
  EIEIO_SYNC;

 /* Data polling */
  done = 0;
  once = 0;
  while (!done)
    {
      (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
      EIEIO_SYNC;
      flashDat = (UINT) &poll[0];
      (*flash_read) (Flashbase, flashDat) ;    /* Read status register */
   
      EIEIO_SYNC;
	  
 	/*printf(" \ninsde the while loop");*/
     #ifdef DEBUG
          print_status ("Erase Status Reg = ", poll);
      #endif

      if (operation_complete (poll, FLASH_CMD_DATA80))
     {

	   done  =1;

          (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
          EIEIO_SYNC;  
          flashDat = (UINT) &poll[0];
          (*flash_read) ((unsigned) FlashAddress,flashDat) ;     /* Read status register */
          EIEIO_SYNC;

      /* Check if Erase was successful */
	   if (operation_fail (poll, FLASH_CMD_DATA20))
	   {
		      printf("\nERROR Flash Erase unsuccessful");
#ifdef DEBUG
		      printf("\nERROR Flash Erase unsuccessful");
#endif
#ifdef ROHS
			retry--;
			if(retry > 0)
			   	goto RETRY;
			else{
#endif 
				exit = FLASH_TIMEOUT;
				
			       break;
#ifdef ROHS
			 }
#endif 
	      }

			          if (operation_fail (poll, FLASH_CMD_DATA02))
			      {
			      printf("\nERROR Flash Erase unsuccessful FLASH_CMD_DATA02");
#ifdef DEBUG
			            printf("\nERROR Flash Erase unsuccessful FLASH_CMD_DATA02");
#endif
#ifdef ROHS
			retry--;
			if(retry > 0)
			   	goto RETRY;
			else{
#endif 
				exit = FLASH_TIMEOUT;
				
			       break;
#ifdef ROHS
			 }
#endif 
				   }

	index++;
	if(index == 5)
		{
    		printf ("-"); 
			index=0;
		}
      }
    } /* End while polling data */


  /* To next Flash physical address - this is fixed as it is always on 64 bit accesses*/
#ifdef ROHS
  chipBlk += 0x10000;/*FLASH_INC_ADDR;        */	
#else 
  chipBlk += FLASH_INC_ADDR;        

#endif 


 } /* End for all sectors */

  printf ("\nErase flash Done: last offset 0x%x \n", (chipBlk - FLASH_INC_ADDR) );    

  /* Reset Flash */
  frcFlashReadRst (Flashbase);

  return (exit);
}


/* buffer for one sector, max size */

UINT16 uflashBuf [FLASH_MAX_BLOCK_SIZE / (sizeof (UINT16)) + 16];

#define SECTOR_TEST_SZ    16        /* Test this many 16 bit words in each sector */
UINT16  flocalBuf[SECTOR_TEST_SZ];

/****************************************************************************
* Function        : frcFlashGetInfo                                         *
*                                                                           *        
* Description    : This routine gets the size and Flash info.               *
*                                                                           *
*                                                                           *
* Inputs        : Pointers to return Manufacture and Device Codes, bits     * 
                  width, total blocks, total bytes and Address of Flash     *
*                                                                           *
* RETURNS        : ERROR if problem reading size, else OK.                  *
*                                                                           *    
****************************************************************************/


STATUS  frcFlashGetInfo ( 
            UINT32 * mCode,            /* Returned Manufact code*/
            UINT32 * dCode,            /*          device code */
            UINT32 * fWidth,        /*          bits in width */
            UINT32 * nBlocks,        /*        total blocks */
            UINT32 * bSize,            /*          total bytes */
            unsigned int Flashbase
              )
{

    UINT32  mnFct;
    UINT32  devCode;
    UINT32  num16;
    UINT32  count;



   frcFlashAutoSelect (&mnFct, &devCode,Flashbase);
 
   if (flash_width == 0)
   {
    return ERROR;
   }
   
   if (flashBlocks == 0)
   {
    return ERROR;
   }

   *mCode = mnFct;
   *dCode = devCode;
   *fWidth = flash_width;
   *nBlocks = flashBlocks;

  
  /* set count to number bytes in each sector */
 

   num16 = flash_width / 16;
   count = num16 * FLASH_BLOCK_SZ_16BITS;
   
  /* bytes per sector * # sectors = total size */
   count = count * flashBlocks;

   *bSize = count;
 
   return OK;
 }



 
 

/****************************************************************************
* Function    : frcFlashRead
*
* Description : This routine reads specified number of bytes from a specified
*               flash device.
*
* Inputs      : StartAdd - The starting address of the flash where to read
*                          from.
*               noLongs  - Number of Longs.
*               buffer   - Destination address for the read data. 
*
* RETURNS     : None
****************************************************************************/

void frcFlashRead(UINT32 StartAdd, UINT32 noLongs, UINT32 *buffer)
{
    unsigned char *StartAddress,*DestAddr;
	unsigned int i;

        frcFlashReadRst (StartAdd);
        DestAddr = (unsigned char *)buffer;
    StartAddress = (unsigned char *)StartAdd;
    for(i=0;i< (noLongs);i++)
    {
        *DestAddr++ = *StartAddress++;
    }
         return;
}

/****************************************************************************
* Function    :  frcFlashBlockWrite                                         *
*                                                                           *
* Description : This routine writes into the specified block                *
*                                                                           *
* Inputs      : BlockNo - Number of the block to be written.                *
*               Flashbase- Base address of the flash device                 *
*               buffer -   Array of data that has to be written. This array *
*                          should be 256KB in size.                         *
*                                                                           *
* RETURNS     : FLASH_SUCCESS - If successful                               *
*               FLASH_TIMEOUT - If not successful                           *
*                                                                           *    
****************************************************************************/

int frcFlashBlockWrite(unsigned int BlockNo,unsigned int Flashbase, 
                       unsigned int *buffer)
{
	
    unsigned short writeSetup [4] = 
    { FLASH_CMD_SETUPPROW, FLASH_CMD_SETUPPROW,
     FLASH_CMD_SETUPPROW, FLASH_CMD_SETUPPROW
    };
    unsigned short readStat [4] = 
    { FLASH_CMD_READSTATW, FLASH_CMD_READSTATW,
     FLASH_CMD_READSTATW, FLASH_CMD_READSTATW
    };
    unsigned int exitval,times_16,num_16,lpCt,flashDat,size,*pData;
    unsigned int FlashAddress, DevCode, BlockLockcode, Mnfct;
    unsigned short *Address;    
    unsigned short poll [4];    

/*  Check whether the block is locked */
#ifdef ROHS
	    FlashAddress = getFlashBase(BlockNo,Flashbase);  
#ifdef DEBUG
	printf(" flash Add = %x ",FlashAddress);  
#endif
#else		
    FlashAddress = Flashbase + BlockNo * 0x40000;    
#endif

    frcFlashReadIdCommand (&Mnfct,&DevCode,&BlockLockcode, FlashAddress);
    if(BlockLockcode == 0x1) {
        printf("Write unsuccessful, block is locked!!\n");
        return(FLASH_TIMEOUT);
    }
    exitval = FLASH_SUCCESS;
#ifdef DEBUG	
	printf("\ncontent of the Buffer = %d ",*buffer);
#endif
    pData = buffer;

#ifdef ROHS
	Address = (unsigned short *)getFlashBase(BlockNo,Flashbase);
	if (BlockNo<4)
		{
		size =0x10000;
	}
	else
		{
	size=0x40000;
	}
#else
    Address = (unsigned short *)(Flashbase + BlockNo * 0x40000);
    size = 0x40000;
#endif

    if (flash_width == 0)
      return FLASH_TIMEOUT;

    else if (flash_width == 64)
    /* Flash width = 64 bits, so 8 bytes per access */
      times_16 = size / 8;
    else if (flash_width == 32)
    /* Flash width = 32 bits, so 4 bytes per access */ 
     times_16 = size / 4;
    else
    /* Flash width = 16 bits, 2 bytes/access */
      times_16 = size / 2;

    /* # of 16 bits in width = num_16 */
    num_16 = flash_width / 16;


   /* -------program flash loop -------------------------- */

    printf("\nPlease wait .....");    

    /* Write data */
    for (lpCt = 0; lpCt < times_16; lpCt++)
    {
	

        (*flash_write) ((unsigned) &Address[lpCt * num_16], (unsigned) &writeSetup[0]); /* 32 bit data width*/    
        EIEIO_SYNC;

               
        (*flash_write) ((unsigned) &Address[lpCt * num_16], (unsigned) pData);    
        EIEIO_SYNC;

    pData++;

    
       while (1)
     {
	
      (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
          flashDat = (unsigned int) &poll[0];
          EIEIO_SYNC;

          (*flash_read) (Flashbase, (unsigned) &poll[0]) ;    /* Read status register */
          EIEIO_SYNC;
	
          if (operation_complete (poll, FLASH_CMD_DATA80))
		
        break;
    }
#ifdef DEBUG
ix++;
if(ix==10)
{
printf("-");
ix=0;
}
#endif
    } /* End for write data */

 /* Last time - before exit - check full status */

  (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
  flashDat = (UINT) &poll[0];
  EIEIO_SYNC;
  (*flash_read) (Flashbase, (unsigned) &poll[0]);  /* Read status register */
  EIEIO_SYNC;
  if (operation_fail (poll, FLASH_CMD_DATA10))
  {
/*    printf("\nERROR Flash BANK Program unsuccessful");*/
    exitval = FLASH_TIMEOUT;
  }
  if (operation_fail (poll, FLASH_CMD_DATA02))
  {
/*    printf("\nERROR Flash BANK %d Program unsuccessful",bank);*/
    exitval = FLASH_TIMEOUT;
  }
  if (operation_fail (poll, FLASH_CMD_DATA08))
  {
/*    printf("\nERROR Flash BANK %d Program unsuccessful",bank);*/
    exitval = FLASH_TIMEOUT;
  }
  /* Reset Flash */
  frcFlashReadRst (Flashbase);
  return (exitval);
}


/****************************************************************************
* Function    : frcFlashBlockErase
*
* Description : This routine Erases the specified block    
*
* Inputs      : Block Number, Address of the Flash Device 
*
* RETURNS     : Success / Fail
****************************************************************************/

int frcFlashBlockErase(unsigned int blockNum,unsigned int Flashbase)
{

    int exit = FLASH_SUCCESS;
  unsigned short poll [4];
 #ifdef ROHS
  short done=0;
 #endif 
  unsigned short readStat [4] = 
    { FLASH_CMD_READSTATW, FLASH_CMD_READSTATW,
      FLASH_CMD_READSTATW, FLASH_CMD_READSTATW
    };
  unsigned int FlashAddress,BlockLockcode=0,Mnfct,DevCode, flashDat;



  /*  Make sure we have correct flash selected */

#ifdef ROHS      
	 FlashAddress = getFlashBase(blockNum, Flashbase);
#else
    FlashAddress = Flashbase + blockNum * 0x40000;
#endif

#ifdef DEBUG
	printf ("flash add = %x",FlashAddress);
#endif 
	frcFlashReadIdCommand (&Mnfct,&DevCode,&BlockLockcode, FlashAddress);
    	if(BlockLockcode == 0x1) 
			{
        	printf("Erase unsuccesful, block is locked at address %x!!\n",FlashAddress);
        	return(FLASH_TIMEOUT);
    		}

    

		  flashDat = (UINT) &FlashCmdS[0];
		  (*flash_write) ((unsigned) FlashAddress, flashDat);

		   EIEIO_SYNC;
		  
		  flashDat = (UINT) &FlashCmdE[0];
		  (*flash_write) ((unsigned) FlashAddress, flashDat);
		  EIEIO_SYNC;

		  (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
		  EIEIO_SYNC;
		  flashDat = (UINT) &poll[0];
		  (*flash_read) (Flashbase, flashDat) ;    /* Read status register */
		  EIEIO_SYNC;

#ifdef ROHS
 while (!done){
      (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
      EIEIO_SYNC;
      flashDat = (UINT) &poll[0];
      (*flash_read) (Flashbase, flashDat) ;    /* Read status register */
   
      EIEIO_SYNC;
	  
 	/*printf(" \ninsde the while loop");*/
     #ifdef DEBUG
          print_status ("Erase Status Reg = ", poll);
      #endif

      if (operation_complete (poll, FLASH_CMD_DATA80))
     {

	   done  =1;

          (*flash_write) (Flashbase, (unsigned) &readStat[0]);  /* To read Status */
          EIEIO_SYNC;  
          flashDat = (UINT) &poll[0];
          (*flash_read) ((unsigned) FlashAddress,flashDat) ;     /* Read status register */
          EIEIO_SYNC;

      /* Check if Erase was successful */
	   if (operation_fail (poll, FLASH_CMD_DATA20))
	   {
		      printf("\nERROR Flash Erase unsuccessful");
#ifdef DEBUG
		      printf("\nERROR Flash Erase unsuccessful");
#endif
		
				exit = FLASH_TIMEOUT;
			       break;
			 
	      }

			          if (operation_fail (poll, FLASH_CMD_DATA02))
			      {
			      printf("\nERROR Flash Erase unsuccessful FLASH_CMD_DATA02");
#ifdef DEBUG
			            printf("\nERROR Flash Erase unsuccessful FLASH_CMD_DATA02");
#endif
				
					   exit = FLASH_TIMEOUT;
			       	   break;
				   
				   }

	
      }
    }		  
#else 
    if (operation_complete (poll, FLASH_CMD_DATA80))
    {
  
        (*flash_write) (Flashbase, (unsigned) &readStat[0]);  
        EIEIO_SYNC;  
        flashDat = (UINT) &poll[0];
        (*flash_read) ((unsigned) FlashAddress,flashDat) ;     
        EIEIO_SYNC;


        if (operation_fail (poll, FLASH_CMD_DATA20))
        {
/*          printf("\nERROR Flash Erase unsuccessful");*/
            exit = FLASH_TIMEOUT;
        
        }

        if (operation_fail (poll, FLASH_CMD_DATA02))
        {
/*          printf("\nERROR Flash Erase unsuccessful");*/
            exit = FLASH_TIMEOUT;
        
        }

    }

#endif 
/*sysMsDelay(1500);*/
#ifdef DEBUG
printf("\n Erased flash block");
#endif 
  
  /* Reset Flash */
  frcFlashReadRst (FlashAddress);

  return (exit);
}


 

 
 

/******************************************************************************
*
* Function:    frcFlashUnlock.
*
* Description: This functions clears the lock-bit of a Flash device.
*
* Inputs:      flashBase -> Base address of the flash to be unlocked.
*
* Output:      1 ->  if flash erase was successful.
*              0 -> if flash erase was unsuccessful.
*
******************************************************************************/

int frcFlashUnlock (unsigned int * flashBase) {

    UINT32 temp;

    *(flashBase) = 0x60606060;
    *(flashBase) = 0xd0d0d0d0;

    /* Read the Status register and wait until SR.7  is set */
    sysMsDelay(100);
	
    while(1) {
    if (( (temp = *( (UINT32 *)flashBase)) & 0x80) == 0x80)
		
            break;
    }

    /* Full staus check procedure */

    if ((temp & 0x08) == 0x08) 
        return(0); /* Voltage renge error */

    if ((temp & 0x30) == 0x30)
        return(0); /* Command sequesnce error */

    if ((temp & 0x20) == 0x20)
        return(0); /* Clear block Loc-bits error */

frcFlashReadRst ((unsigned int )flashBase);

    return(1); /* Unlock successful */

}
/******************************************************************************
*
* Function:    frcFlashlock.
*
* Description: This functions locks a block of the Flash device.
*
* Inputs:      flashBase -> Base address of the flash to be Locked.
*
* Output:      1 ->  if flash lock was successful.
*              0 -> if flash lock was unsuccessful.
*
******************************************************************************/

int frcFlashLock (unsigned int * flashBase) {

    UINT32 temp;

    *(flashBase) = 0x60606060;
    *(flashBase) = 0x01010101;

    /* Read the Status register and wait until SR.7  is set */
    sysMsDelay(1000);
    while(1) {
    if (( (temp = *( (UINT32 *)flashBase)) & 0x80) == 0x80)
		sysMsDelay(1000);
            break;
    }

    /* Full staus check procedure */

    if ((temp & 0x08) == 0x08) 
#ifdef DEBUG
		printf("\n Voltage range error ");
#endif 
        return(0); /* Voltage range error */

    if ((temp & 0x30) == 0x30)
#ifdef DEBUG
		printf(" \nCommand sequence error");
#endif 
        return(0); /* Command sequence error */

    if ((temp & 0x20) == 0x20)
#ifdef DEBUG
		printf("\n Clear block Loc-bits error  ");
#endif 
        return(0); /* Clear block Loc-bits error */

    return(1); /* Lock successful */

}
int frcFlashReadIdCommand ( unsigned int * Mnfct , unsigned int * DevCode,
                          unsigned int * BlockLockcode, unsigned int Flashbase)
{
  volatile unsigned char *FlashAddress;
  unsigned short FlashCmd [4] = 
  { FLASH_CMD_DATA90, FLASH_CMD_DATA90, 
    FLASH_CMD_DATA90, FLASH_CMD_DATA90
   };
    unsigned int flashAdr;
    unsigned int flashDat;
    unsigned short data [4];
    
 
    FlashAddress = (UCHAR *) Flashbase;

               
    /* Send Autoselect command */
    flashAdr = Flashbase;
    flashDat = (UINT) &FlashCmd[0];
    __WRITE_FLASH64(flashAdr,flashDat) ;     

    /* Read Manufacture and device code */

    flashDat = (UINT) &data [0];
    __READ_FLASH64((unsigned) &FlashAddress[0],flashDat);
    *Mnfct = (unsigned) data [0] & 0xff;
    
    /* detect data width */
    flash_width = 16;
    if (data [0] == data [1])
      flash_width = 32;
    if (data [0] == data [2])
      flash_width = 64;

   /* __READ_FLASH64((unsigned) &FlashAddress[8],flashDat);*/
   if(flash_width == 16)
     __READ_FLASH16((unsigned) &FlashAddress[2],flashDat);

   if(flash_width == 32)
     __READ_FLASH32((unsigned) &FlashAddress[4],flashDat);    

   if(flash_width == 64)
     __READ_FLASH64((unsigned) &FlashAddress[8],flashDat);
    
    *DevCode = (unsigned) data [0] & 0xff;    
    
   /* __READ_FLASH64((unsigned) &FlashAddress[8],flashDat);*/
   if(flash_width == 16)
     __READ_FLASH16((unsigned) &FlashAddress[4],flashDat);

   if(flash_width == 32)
     __READ_FLASH32((unsigned) &FlashAddress[8],flashDat);    

   if(flash_width == 64)
     __READ_FLASH64((unsigned) &FlashAddress[16],flashDat);

    *BlockLockcode = (unsigned) data[0] & 0xff;
   
    /* Back to read mode */
    frcFlashReadRst (Flashbase);
    return(FLASH_SUCCESS);    

} 
/****************************************************************
  option-function: takes care that the option entered by the user 
                    is within the specified range
*****************************************************************/

UINT8 option(UINT8 low,UINT8 high)

{    
     UINT8 temp[20];
     bool flag=1;

  
     while(flag) 
    { 
     printf("Enter the choice:");
     scanf("%s",temp);    
     if(temp[1]=='\0')
        { 
          if((temp[0]>=low)&&(temp[0]<=high))
          {       
           flag=0;
           continue;  
           }  
	 else
            {
             printf("\nInvalid  Number..........\n");
	     printf("\nEnter the number from %c to %c:",low,high);                     
            }
            
           }
      else           
            { printf("do not enter a string\n"); 
              
              
             }
            
      }
                    	
     return(temp[0]);
  
}


UINT8 option1(void)

{    
     UINT8 temp[20];
     bool flag=1;
     int dat=0;

  
     while(flag) { 
         scanf("%s",temp);    
         if(temp[1]=='\0') { 
             if(isdigit(temp[0]) == 0)
                  flag=1;
             else
                  flag=0;
         }
         else {
             if (temp[2] == '\0') {
                 if((isdigit(temp[0]) == 0) || (isdigit(temp[1]) ==0))
                      flag=1;
                 else
                      flag =0;
             }
             else {
                 if(temp[3] == '\0') {
                     if((isdigit(temp[0]) == 0) || (isdigit(temp[1]) ==0) ||
                        (isdigit(temp[2]) == 0) )
                         flag=1;
                     else
                         flag =0;
                 }
                 else
                    flag = 1;
             }
        }
        if (flag == 0) {
            dat = atoi((char *)temp);
#ifdef ROHS			
            if ( (dat < 0) || (dat > 259))
                flag=1;
#else		
            if ( (dat < 0) || (dat > 127))
                flag=1;
#endif			

        }

        if (flag == 1) {
            printf("Invalid input\n");
            printf("Enter the block Number:");
        }
    }
    return((UINT8)dat);
}
  
/******************************************************************************
*
* Function:    flashAutoDetect.
*
* Description: This functions detects the size of each  of the Flash device.
*
* Inputs:      N/A
*
* Output:      N/A
*
******************************************************************************/

void flashAutoDetect()
{
    UINT32 size=0;
    UINT32 dummy1, dummy2;

    IMPORT void gtMemoryMapDevice0Space(unsigned int ,unsigned int );
    IMPORT void gtMemoryMapDevice1Space(unsigned int ,unsigned int );
    IMPORT UINT8 frcGetUserFlashSize();    
    IMPORT UINT8 frcGetFlashOrg();    
    IMPORT UINT8 frcWhoAmI();    

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering flashAutoDetect!!!\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering flashAutoDetect!!!\n");
    }
#else
    dbg_puts0("Entering flashAutoDetect!!!\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI()) {
#endif /* PMC280_DUAL_CPU */

        switch(frcGetUserFlashSize()) {
            case FLASH_SIZE_16MB:  size =  _16M;break;
            case FLASH_SIZE_32MB:  size =  _32M;break;
            case FLASH_SIZE_64MB:  size =  _64M;break;
            case FLASH_SIZE_128MB: size =  _128M;break;
            default:               size =  0;break;
        }

        /* Disable both initially */
#ifdef ROHS
        gtMemoryMapDevice0Space(0 , 0);
#else 
	gtMemoryMapDevice0Space(0 , 0);
        gtMemoryMapDevice1Space(0 , 0);
#endif

        switch(frcGetFlashOrg()) {
            case FLASH_ORG_EQ:
                
#ifdef ROHS
		gtMemoryMapDevice0Space(CS0_BASE_ADRS ,size);
#else	
		gtMemoryMapDevice0Space(CS0_BASE_ADRS ,size/2);
		gtMemoryMapDevice1Space(CS1_BASE_ADRS , size/2);
#endif
                break;
            case FLASH_ORG_B0:
                gtMemoryMapDevice0Space(CS0_BASE_ADRS , size);
                break;
            case FLASH_ORG_B1:
                gtMemoryMapDevice1Space(CS1_BASE_ADRS , size);
                break;
        }

#ifdef PMC280_DUAL_CPU
    }
#endif /* PMC280_DUAL_CPU */

    vxMsrSet( vxMsrGet() | 0x2000 );
#ifdef ROHS
    frcFlashAutoSelect( &dummy1, &dummy2, FLASH_BANK0 );
   
#else
    frcFlashAutoSelect( &dummy1, &dummy2, FLASH_BANK0 );
    frcFlashAutoSelect( &dummy1, &dummy2, FLASH_BANK1 );

#endif 
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting flashAutoDetect!!!\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting flashAutoDetect!!!\n");
    }
#else
    dbg_puts0("Exiting flashAutoDetect!!!\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

}




int frcFlashUnlockComplete  (){

	
	unsigned int BlockNo,*tempAddr = NULL;
 unsigned dummy1, dummy2;
unsigned int  Mnfct,DevCode,BlockLockcode,Flashaddress;
int status= ERROR;
int index=0;

frcFlashUnlock((unsigned int *)0xa0000000);
frcFlashUnlock((unsigned int *)0xa0010000);
frcFlashUnlock((unsigned int *)0xa0020000);
frcFlashUnlock((unsigned int *)0xa0030000);
tempAddr = (unsigned int *) 0xa0040000;
	for(BlockNo = 0;BlockNo <= 1024;BlockNo++){

		
		

		*(tempAddr) = 0x60606060;
		*(tempAddr) = 0xd0d0d0d0;
		
		
		/*sysMsDelay(10);*/	
		
	/*i = frcFlashUnlock((unsigned int *)tempAddr); */	
		

		Flashaddress =(unsigned int )tempAddr; 
		frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK0);
		frcFlashReadIdCommand (&Mnfct,&DevCode,&BlockLockcode, Flashaddress);
    		if(BlockLockcode == 0x1) 
			{
        	printf("unsuccesful, block is locked at address %x!!\n",Flashaddress);
			status = ERROR;
        	
    		}
		else {
		/*printf("\nunlocked block %d return value = %d Flash Base = %x\n ",BlockNo,i,tempAddr);*/
			
			status= OK;
			}
		tempAddr = (unsigned int *)(FLASH_BANK0 + (BlockNo * 0x10000));
	index ++;
	if(index == 10)
	{
		printf("-");
		index =0;

	}
	}




	frcFlashReadRst((unsigned int )CS0_BASE_ADRS);
	return status;	
}


/***************************************************************************/
/*to get the flash base address*/
unsigned int getFlashBase(unsigned int blockNo,unsigned int Flashbase) {

	unsigned int  FlashAddress=0;

	if (blockNo < 4){
                FlashAddress = Flashbase + (blockNo * 0x10000);
        }else if (blockNo >= 4 && blockNo <= 258){
            FlashAddress =      Flashbase + (((blockNo + 1) - 4) * 0x40000);
        }
	return (FlashAddress);

}



