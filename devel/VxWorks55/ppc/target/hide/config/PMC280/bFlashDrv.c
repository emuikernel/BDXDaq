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
*                                                                             *
* bFlashDrv.c - Source File for Boot Flash (AMD AM29LV008) Memory Driver.     *
*                                                                             *
* DESCRIPTION:	                                                              *
*     This is a simple driver that allows the user to erase and program the   *
*     on-board boot flash AT49BV008AT.                                        *
*                                                                             *
* DEPENDENCIES:	None.                                                         *
*                                                                             *
******************************************************************************/
#include "vxWorks.h"
#include "bFlashDrv.h"
#include "flashDrv.h"
#include "ftpLib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <logLib.h>
#include <stdio.h>
#include <unistd.h>

/*#define DBGF		*- debug Compile */
IMPORT void sysDelay (void);

static unsigned int chip_ID;    /* Manu_ID | Device_ID */
#define FBUF_SIZE	0x00010000  
unsigned char flashBuf[FBUF_SIZE];
UINT8         flashBufNV [ NVRAM_SIZE + 0x100];
unsigned int correctaddr(unsigned int addr);  
void frcBootFlashFile 
     ( 
       char * FTPSERVER_IP_ADRS ,char * DIRECTORY,char * filename 
     );
void frcBootFlashFileV 
     ( 
	char * filename 
     );
short bootFlashParamsProgram
 (
   volatile unsigned char *Address,	/* Addr of Flash to start */ 
   unsigned char *data,			/* Addr of source data */
   unsigned int size			/* # chars to program */
 );
short bootFlashProgram1byte 
  ( 
    volatile unsigned char *dest,
    unsigned char data
  );

/****************************************************************************
*									    
* Function		: bootFlashAutoSelect					 	
*										
* Description	: This routine returns a Manufactor code and Device code 
*									
* Inputs		: pointers to return the Manufactor and Device codes
*								
* Output		: flash codes at input pointers.		
*									
* RETURNS		: None							
****************************************************************************/

void bootFlashAutoSelect(unsigned int *Mnfct, unsigned int *DevCode)
{
  /* Send Autoselect command */
   
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) ) = 0xaa;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x2aa)) )= 0x55;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) )= 0x90;

	sysDelay();
  /* Read Manufacture and device code */
  *Mnfct = (unsigned int) (*((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x0)) ));
  *DevCode = (unsigned int) (*((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x1)) ));

#ifdef DBGF 
  printf ("Mfr ID = %x ",Mnfct[0]);
  printf ("Device ID = %x \n",DevCode[0]);
#endif

  chip_ID = (unsigned short) ((Mnfct [0] << 8) | DevCode [0]);

  /* Back to read mode */
  frcbootFlashReadRst ((unsigned) BFLASH_ADDRESS);
}



/****************************************************************************
*																			
* Function		: bootFlashGetProtect										
*																			
* Description	: This routine returns  the protect code for a sector ...   
*																			
* Inputs		: Sector Offset												
*																			
* Outputs		: None														
*																			
* RETURNS		: protect code (0 or 1) of a sector							
****************************************************************************/

int bootFlashGetProtect (unsigned int sectorOff )

{
   int code;


  /* Send Autoselect command */
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) ) = 0xaa;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x2aa)) ) = 0x55;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) )= 0x90;
 
  /* Read Protect code */
  code = (int)  (*((volatile unsigned char *) correctaddr(((BFLASH_ADDRESS + sectorOff) | 0x2)) ));




  /* Back to read mode */
  frcbootFlashReadRst ((unsigned) BFLASH_ADDRESS);

  return (code);
}


/****************************************************************************
*																			*
* Function		: bootEraseOrProgramOk										*
*																			*
* Description	: This routine Erases or programs the Flash at a specified  *
*				  location												    *
*																			*
* Inputs		: Destination Address										*				
*																			*
* Outputs		: None														*
*																			*
* RETURNS		: The Flash Status  code (Success / Failure)				*
****************************************************************************/

short bootEraseOrProgramOk (volatile unsigned char *dest)
{
  volatile unsigned char poll1, poll2;

 /* Data polling  */

 poll1 = *((char *)correctaddr((int)dest));
 while (1)
 {
   poll2 = *((char *)correctaddr((int)dest));

   if ((poll1 ^ poll2) & 0x40)   /* toggle */ 
   {
     if (poll2 & 0x20)     /* DQ5 = 1 */
     {
       /* read twice */
       poll1 = *((char *)correctaddr((int)dest));;
       poll2 = *((char *)correctaddr((int)dest));;
       if ((poll1 ^ poll2) & 0x40)   /* toggle */ 
         return FLASH_TIMEOUT;
       else
         break;   /* program completed */
     }
     else
     {
       poll1 = poll2;
       continue;
     }
   }
   else
     break;   /* program completed */
 }

 return FLASH_SUCCESS;
}


/********************************************************************************
* Read Boot Flash -- Starting at the last sector where boot Parameters are saved 
* Note: This routine may be called early in the boot process, so
*  no printf's !!! (no prints here and callees )
*/

short bootFlashParamRead ( UINT8 * data, unsigned int size)
{
  volatile UINT8  * fAddr;
  unsigned int i;

  if (!chip_ID)
  {
    unsigned x, y;
    bootFlashAutoSelect (&x, &y);
  }

  /* set  last block  addr */
  switch (chip_ID)
  {
    case AMD_29LV008B_TOP:
      if (size > SIZE_16K)
      {
        logMsg("Data size is bigger than 16 KBytes, can not read boot parameter\n",
			1,2,3,4,5,6 );
        return ERROR;
      }
      fAddr = (volatile unsigned char *) BFLASH_8BTOP_LASTSECADR ;
      break;
    case AMD_29LV008B_BOTTOM:
      if (size > SIZE_64K)
      {
        logMsg ("Data size is bigger than 64 KBytes, can not write boot parameter\n",
		1,2,3,4,5,6 );
        return ERROR;
      }
      fAddr = (volatile unsigned char *) BFLASH_8BBOT_LASTSECADR ;
      break;
    default:
      logMsg ("Unsupported device, chip ID = %04x\n", chip_ID, 2,3,4,5,6 );
      return ERROR; 
      break;
  }

  /* Flash data to caller's buffer  */
  if ((chip_ID == AMD_29LV008B_TOP) || (chip_ID == AMD_29LV008B_BOTTOM))
  {
    for (i = 0; i < size; i++)
    { 	
      *data = *((volatile unsigned char *)correctaddr((unsigned int)fAddr));
       fAddr++;
       data++;	 	
    }
    return OK;
  }
  else
    return ERROR;
}


short bootFlashParamWrite (unsigned char *data, unsigned int size)
{
  volatile unsigned char *fAddr;

  if (!chip_ID)
  {
    unsigned i, j;
    bootFlashAutoSelect (&i, &j);
  }

  /* erase last block */
  switch (chip_ID)
  {
    case AMD_29LV008B_TOP:
      if (size > SIZE_16K)
      {
        printf ("Data size is bigger than 16 KBytes, can not write boot parameter\n");
        return ERROR;
      }


      fAddr = (volatile unsigned char *) BFLASH_8BTOP_LASTSECADR;
/*      if (bootFlashSectorErase (fAddr) == FLASH_TIMEOUT) 
      {
        printf ("Failed to erase sector 18, can not write boot parameter\n");
        return ERROR;
      }  */
      break;
    case AMD_29LV008B_BOTTOM:
      if (size > SIZE_64K)
      {
        printf ("Data size is bigger than 64 KBytes, can not write boot parameter\n");
        return ERROR;
      }

      fAddr = (volatile unsigned char *) BFLASH_8BBOT_LASTSECADR;
/*      if (bootFlashSectorErase (fAddr) == FLASH_TIMEOUT)
      {
        printf ("Failed to erase sector 18, can not write boot parameter\n");
        return ERROR;
      }  */
      break;
    default:
      printf ("Unsupported device, chip ID = %04x\n", chip_ID);
      return ERROR; 
      break;
  }

  /* write boot parameter to flash */
  if ((chip_ID == AMD_29LV008B_TOP) || (chip_ID == AMD_29LV008B_BOTTOM))
  {
    if (bootFlashParamsProgram (fAddr, data, size) != FLASH_SUCCESS)
    {
      printf ("Failed to program boot parameter\n");
      return ERROR;
    } 
    else
    {
	  sysDelay();
      printf("Verify...\n");
      if (frcBootFlashVerify (fAddr, data, size) == OK)
      {
        printf("OK!\n");
        return (OK);
      }
      else
      {
        printf ("FAILED!\n");
        return ERROR;
      }
    }
  }
  else
    return ERROR;
}



/**************************************************************************
* bootFlashParamsProgram - program flash
*
* Inputs:  Start addr of flash
*	   Start addr of source data
*	   byte count
*
* Outputs: FLASH_SUCCESS if OK;
*
************************************************************************/

short bootFlashParamsProgram
 (
   volatile unsigned char *Address,	/* Addr of Flash to start */ 
   unsigned char *data,			/* Addr of source data */
   unsigned int size			/* # chars to program */
 )
{
  unsigned i;
  unsigned j;

  
  if (!chip_ID)
    bootFlashAutoSelect (&i, &j);
   printf("\nChipID = %x",chip_ID);

  j = 0;
  if ((chip_ID == AMD_29LV008B_TOP) || (chip_ID == AMD_29LV008B_BOTTOM))
  {
     /*printf("\nbootFlashSectorErase....");*/
     frcBootFlashSectorErase(Address);	
    /* Write data */
    printf("\nProgram BOOTFLASH at 0xFFFF0000 %d bytes....",size);	
    for (i = 0; i < size; i++)
    {
       if (bootFlashProgram1byte (&Address[i], data[i]) == FLASH_TIMEOUT)
       {
         /* program error */
         printf("Program Error\n");
	 return FLASH_TIMEOUT;
       }
    } /* end for size */
    
    /* Reset Flash */
    /*printf("\nBootFlashReadRst....");	*/
    frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );
 
    printf("Done\n");
    return FLASH_SUCCESS;
  }
  else
    return FLASH_TIMEOUT;
}




/******************************************************************************
*									      *
* Function		: bootFlashProgram1byte 			      *
*									      *
* Description	: Programs the Flash device with 1 byte of data		      *
*									      *
* Inputs		: Address Location for the data to be written and the *
*				  1 byte data				      *
*									      *
* RETURNS		: The Flash Status  code (Success / Failure)	      *
*******************************************************************************/


short bootFlashProgram1byte 
  ( 
    volatile unsigned char *dest,
    unsigned char data
  )
{
 int i;
 /* Write program command sequence  */
 *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) ) = 0xaa;
 *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x2aa)) )= 0x55;
 *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) )= 0xa0;
 *((char *)correctaddr((int)dest)) = data;

 /* Data polling  */

 for(i = 0;i < 10000;i++);
 return bootEraseOrProgramOk (dest);
} 

/****************************************************************************
* Function		: frcBootFlashVerify				    *
*									    *
* Description	: Verifies the Flash Device				    *
*									    *		
* Inputs		: Starting addr of flash for verification,	    *
*				  Start addr of source data,		    *
*				  No. of bytes.				    *
*									    *
* RETURNS		: SUCCESS if OK					    *
*									    *	
****************************************************************************/
short frcBootFlashVerify
 (
   volatile unsigned char *Address,	/* Addr of Flash to start */ 
   unsigned char *data,			/* Addr of source data */
   unsigned int size			/* # chars to program */
 )
{
  unsigned int i;

  for (i = 0; i < size; i++)
  {
    if(*((volatile unsigned char *)correctaddr((UINT32)Address + i)) != data[i])
    {
      printf ("Error in programming.  Wrote: %x", data[i]);
      printf ("Read: %x", Address[i]);
      printf (" at address %x", (unsigned int) Address + i); 
      return (ERROR);
    }
  }
  return (OK);
}




/****************************************************************************
*																			*
* Function		: frcbootFlashReadRst										*
*																			*
* Description	: This routine resets the flash into Read Mode				*
*																			*
* Inputs		: Address of the Flash Device								*
*																			*
* RETURNS		: None														* 
****************************************************************************/

void frcbootFlashReadRst( unsigned fAddr)
{
  if (!chip_ID)
  {
    unsigned i, j;
    bootFlashAutoSelect (&i, &j);
  }

  if (chip_ID == AMD_29LV008B_TOP || chip_ID == AMD_29LV008B_BOTTOM)
    *((volatile unsigned char *) correctaddr(fAddr)) = 0xf0;
}



 /* AM29LV008BT sector offset indexed by sector # */
unsigned int am29TopOffset [ AMD_29LV008_NUM_SECTORS] =
 {
	0x00000,
	0x10000,
	0x20000,
	0x30000,
	0x40000,
	0x50000,
	0x60000,
	0x70000,
	0x80000,
	0x90000,
	0xA0000,
	0xB0000,
	0xC0000,
	0xD0000,
	0xE0000,
	0xF0000,
	0xF8000,
	0xFA000,
	0xFC000

 };



/* AM29LV008BB sector offset indexed by sector # */
unsigned int am29BotOffset [ AMD_29LV008_NUM_SECTORS] =
 {
	0x00000,
	0x04000,
	0x06000,
	0x08000,
	0x10000,
	0x20000,
	0x30000,
	0x40000,
	0x50000,
	0x60000,
	0x70000,
	0x80000,
	0x90000,
	0xA0000,
	0xB0000,
	0xC0000,
	0xD0000,
	0xE0000,
	0xF0000

 };



/****************************************************************************
*																			*	
* Function		: bootFlashGetSectorOff										*
*																			*
* Description	: Returns the sector offset for a given sector number		* 
*																			*
* Inputs		: sector No.												*
*																			*	
* RETURNS		: sector offset.											*	
****************************************************************************/

unsigned int  bootFlashGetSectorOff ( unsigned sectorNumb )
{
  unsigned int offset;
  unsigned int i, j;


 if (!chip_ID)
  {
   
    bootFlashAutoSelect (&i, &j);
  }


  if (chip_ID == AMD_29LV008B_TOP)
  {
     offset =  am29TopOffset [sectorNumb]; 
  }
  else
    offset =  am29BotOffset [sectorNumb]; 


  return ( offset);
}




/****************************************************************************
* Function		: frcBootFlashSectorErase			    *
*									    *	
* Description		: Erases one sector of flash			    *
*									    *
* Inputs		: sector address				    *
*									    *
* RETURNS		: FLASH_SUCCESS if OK;				    *	
*									    *	
****************************************************************************/
short frcBootFlashSectorErase (volatile unsigned char *addr)
{
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) ) = 0xaa;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x2aa)) )= 0x55;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) )= 0x80;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x555)) )= 0xaa;
  *((volatile unsigned char *) correctaddr((BFLASH_ADDRESS | 0x2aa)) )= 0x55;
  *((volatile unsigned char *) correctaddr((int)addr)) = 0x30;

 return bootEraseOrProgramOk (addr);
}

/**************************************************************************
* frcBootFlashProgram - program flash
*
* Inputs:  Start addr of flash
*	   Start addr of source data
*	   byte count
*
* Outputs: FLASH_SUCCESS if OK;
*
************************************************************************/

short frcBootFlashProgram
 (
   volatile unsigned char *Address,	/* Addr of Flash to start */ 
   unsigned char *data,			/* Addr of source data */
   unsigned int size			/* # chars to program */
 )
{
  unsigned i;
  unsigned j;
  
  if (!chip_ID)
    bootFlashAutoSelect (&i, &j);

  j = 0;
  if (chip_ID == AMD_29LV008B_TOP || chip_ID == AMD_29LV008B_BOTTOM)
  {
    /* Write data */
    for (i = 0; i < size; i++)
    {
#if 0	
       if ((i & 0xffff0000) == i)
       {
         printf("Programming at address: %x ", (unsigned int)&Address[i]);
         printf(" For length: %x", (size - i) );
         printf(" From address: %x\n", (unsigned int)&data[i]);
       }
#endif

       if (bootFlashProgram1byte (&Address[i], data[i]) == FLASH_TIMEOUT)
       {
         /* program error */
         printf("Program Error\n");
	 return FLASH_TIMEOUT;
       }
    } /* end for size */
    
    /* Reset Flash */
    frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );
 
	sysDelay();

	if(frcBootFlashVerify( Address, data, size) == OK )
		return FLASH_SUCCESS;
	else
		return FLASH_TIMEOUT;
    /*printf("Program Done\n");*/
    return FLASH_SUCCESS;
  }
  else
    return FLASH_TIMEOUT;
}



/**************************************************************************
* bootFlashErase - Erase flash
*
*
*
* Note: This program always erases all of flash.
*
*
* Inputs:  sector (ignored)
*
* Outputs: FLASH_SUCCESS if OK;
*
**************************************************************************/

short bootFlashErase(
			short sector			/* Ignored */
		    )
{
  if (!chip_ID)
  {
    unsigned i, j;
    bootFlashAutoSelect (&i, &j);
  }

  if ((chip_ID == AMD_29LV008B_TOP) || (chip_ID == AMD_29LV008B_BOTTOM))
  {
	UCHAR status;
    /* do chip erase - 
       command sequence is in table 5
       polling for completion is in Figure 5
    */
    /* perform chip erase */
	printf ("Write chip erase sequence\n");
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x555)) = 0xaa;
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x2aa)) = 0x55;
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x555)) = 0x80;
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x555)) = 0xaa;
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x2aa)) = 0x55;
	*((volatile unsigned char *) (BFLASH_ADDRESS | 0x555)) = 0x10;
    /* polling for completion */
	printf ("Poll for completion: ");
	while (1)
	{
		status = *((volatile unsigned char *) BFLASH_ADDRESS);
		if (status & 0x80)
		{
			printf ("done\n");
			/* Reset Flash */
			frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );
			return FLASH_SUCCESS;
		}
		else
		{
			if(status & 0x20)
			{
				printf ("error\n");
				frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );	
				return FLASH_TIMEOUT;
			}
		}
        }
  } 
  else 
  {
	volatile unsigned int *FlashAddress;
	unsigned int poll;
	unsigned once ;
	unsigned chipBlk ;
	unsigned ix ;
	short done;
	short exit ;
	exit = FLASH_SUCCESS;
  	/* Reset Flash */
	frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );

  	chipBlk =  0;		/* current offset in flash */

 	for (ix = 0; chipBlk < (unsigned) 4  ; ix++)
 	{
		/* Set Flash Addr for this set of chips */
		FlashAddress = (unsigned int *)( (unsigned) BFLASH_ADDRESS | chipBlk );
    		/* Write Chip Erase command */
		FlashAddress[ 0 ] = FLASH_CMD_SETUPERAW;
		FlashAddress[ 0  ] = FLASH_CMD_ERASCONFALL;
  		sysDelay( );
		  /* Data polling */
		printf("\n\rPolling data ");
		done = 0;
		once = 0;
		while (!done)
		{
      			poll = FlashAddress[0];
      			if ((poll & FLASH_CMD_DATA80) == FLASH_CMD_DATA80)
			{
	  			done = 1;
				break;
			}
			if ((poll & FLASH_CMD_DATA20) == FLASH_CMD_DATA20)
			{
				poll = FlashAddress[0];
				if ((poll & FLASH_CMD_DATA80) == FLASH_CMD_DATA80)
				break;
				printf("\nERROR Flash TIMEOUT Erase ");
				exit = FLASH_TIMEOUT;
				break;
			}
		} /* End while polling data */

 	} /* End for all sectors */
  
  	/* Reset Flash */
  	frcbootFlashReadRst( (unsigned) BFLASH_ADDRESS );
   	return (exit);
  }
}



/**************************************************************************
*
* frcBootFlashFile - program a binary file to flash 
*
* RETURNS: NA
*
* NOMANUAL
**************************************************************************/

#define FBUF_SIZE	0x00010000  
unsigned char flashBuf[FBUF_SIZE];

UINT8	flashBufNV [ NVRAM_SIZE + 0x100];


void frcBootFlashFile 
     ( 
       char * FTPSERVER_IP_ADRS,char * DIRECTORY, char * filename
     )
{       char LOGIN_ID[30],PASSWORD[30];
	unsigned rdCt ;	
	unsigned char *fAddr ;
	int ctrlSock;
	int dataSock;
        if(filename == NULL)
   	{
		printf("\nCommand Usage frcBootFlashFile(<\"ftpserver_ip_address\",\"directory\",\"filename\">)");
		return;
   	}
	fAddr = (unsigned char *) BFLASH_ADDRESS ;

	/* Save NV RAM sector in boot flash */
	bootFlashParamRead (flashBufNV, NVRAM_SIZE);

	printf("User:");
	scanf("%s",LOGIN_ID);
	printf("Password:");
	scanf("%s",PASSWORD);     



	if (ftpXfer (FTPSERVER_IP_ADRS, LOGIN_ID, PASSWORD, "",
		 "RETR %s", DIRECTORY, filename,
                  &ctrlSock, &dataSock) == ERROR)
	   {
                printf("\n Unable to transfer file through  ftp\n");
                return;
           }  
         
        if ( bootFlashErase(0)  != FLASH_SUCCESS)
	   {
		printf ("\nERROR: cannot Erase Flash ");
		goto eExit;
	   }	 

	while (1)
	{
		if ( (rdCt = read ( dataSock, &flashBuf[0], FBUF_SIZE )) == ERROR)
		{
			printf ("\nERROR: cannot read %s ", filename);
			goto eExit;
		}
		if (rdCt == 0)
		{
			printf ("\nFile read done %s ", filename);
			break; 
		}
		if ( (frcBootFlashProgram ( fAddr, &flashBuf[0], rdCt ) ) != FLASH_SUCCESS )
		{
			printf ("\nERROR: Flash program ");
			goto eExit;
		}
		fAddr = fAddr + rdCt; /* Bytes in count += flash adr */
	}
eExit:

	close (dataSock);


       if (ftpReplyGet (ctrlSock, TRUE) != FTP_COMPLETE)
		return;

       if (ftpCommand (ctrlSock, "QUIT", 0, 0, 0, 0, 0, 0) != FTP_COMPLETE)
	       return;

	close (ctrlSock);
	/* reprogram boot parameter sector with saved data */
	bootFlashParamWrite (flashBufNV, NVRAM_SIZE); 

}




/**************************************************************************
*									  *
* frcBootFlashFileV - verify file data vs Flash                              *
*									  *
* RETURNS: NA								  * 										
*									  *
* NOMANUAL								  *	
***************************************************************************/


void frcBootFlashFileV 
     ( 
     char * filename 
     )
    {
    unsigned i ;
    unsigned rdCt ;
    unsigned fd ;
    unsigned char *fAddr ;
    unsigned flagOK ;


    if(filename == NULL)
    {
	printf("\nCommand Usage frcBootFlashFileV(<\"filename\">)");
	return;
    }
    flagOK = 1;
    fAddr = (unsigned char *) BFLASH_ADDRESS ;

    if ( (fd = open( filename, O_RDONLY, 0664) ) == ERROR)
    {
	 printf ("\nERROR: cannot open %s ", filename);
	 return;
    }

    while (1)
	{
		if ( (rdCt = read ( fd, &flashBuf[0], FBUF_SIZE )) == ERROR) 
		{
	 		printf ("\nERROR: cannot read %s ", filename);
    			flagOK = 0;
	 		goto eExit;
		}

		if (rdCt == 0)
		{
	 		printf ("\nFile read done %s ", filename);
	 		break; 
		}

		/* Verify */
 		{
		 volatile unsigned char *pFlash = fAddr ;
		 volatile unsigned char *pBuf = &flashBuf[0];

		 for (i = 0; i < rdCt; i++)
		 {
		   if ( *pFlash != *pBuf )
		    {
	 		printf ("\nERROR: Flash Verify addr %08x data %02x ",
			    (unsigned int) pFlash, *pFlash );
	 		printf ("bufaddr %08x data %02x ",
			    (unsigned int) pBuf, *pBuf );
    			flagOK = 0;
	 		break;
		    }
		   pFlash++;
		   pBuf++;

	         } /* end for */
	        } /* end verify */
 	   fAddr = fAddr + rdCt; /* Bytes in count += flash adr */
	} /* end while */

eExit:
    	if (flagOK == 1) 
	 printf ("Verify OK ");
	close (fd);
}




/****************************************************************************
* Function    :  correctaddr
*
* Description : Generates the correct address (for MV-64360) for 
*                 specified location in the Boot Flash
*
* Inputs      : Address of the location.
*
* RETURNS     : Correct Address
*
****************************************************************************/


unsigned int correctaddr(unsigned int addr)
{
/*	unsigned int temp;
        temp = addr << 1;
	temp = temp  & 0x000FFFFF;
	temp = temp | ((addr >> 19) & 0x1) ;
	addr = 0xFFF00000 | temp;*/
	return(addr);
}
