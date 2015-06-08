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
* Filename    :variant.c                                                  *
*                                                                             *
* DESCRIPTION :	                                                              *
*       To handle PMC280 different variants.                            *
*                                                                             *                                           *
******************************************************************************/

/*****************************Includes ****************************************/
#include "frctesttool.h"
#include "frctestcases.h"
#include "frctestconfig.h"
#include "gtCore.h"
#include <stdlib.h>
#include <taskLib.h>
#include "flashDrv.h"

extern UINT32 flashBlocks;
IMPORT void frcPrintBoardInfo();
char * sysMemTop (void);
bool frcEEPROMRead16(UINT8 , UINT16 ,unsigned int ,UINT8 *);

 struct PMC280_VARIANT
{
/* Read from Board information*/
char variant;
int bi_mem_size; /*In MB*/
int bi_usrflash_size; /*In MB*/
int BOM_rev; 
char PCB_rev[3]; 
/*Detected */
int bank0_uflash, bank1_uflash;
int ram_size,bootflash_size; /*In MB*/
};

LOCAL struct PMC280_VARIANT pmc280variant;


/*******************************************************************************
*  DESCRIPTION:
*                Check whether PMC280 is dual CPU or not.
* INPUT:
*       None.
*
* RETURN:
*       OK- Dual CPU
*      ERROR- Single CPU
*******************************************************************************/
STATUS isPMC280DualCPU(void )
{
  unsigned int regVal = 0x0;
  GT_REG_READ (0x160, &regVal); /*CPU master control regitsre*/
#ifdef MONARCH_TEST_DBG
  printf ("\nValue read at the register 0x160 is:%x", regVal);
#endif /*#ifdef MONARCH_TEST_DBG */
  if (regVal & 0x200) /*Check MaskBR1 bit*/
  	return ERROR;

  else
  	return OK;
}
/*******************************************************************************
*  DESCRIPTION:
*                Check whether PMC280 has user flash or not.
* INPUT:
*       None.
*
* RETURN:
*       OK- Present
*      ERROR- Absent
*******************************************************************************/
STATUS isPMC280UsrFlashPresent(void )
{
if (( pmc280variant.bi_usrflash_size != 0) ||
	(pmc280variant.bank0_uflash !=0) ||
	( pmc280variant.bank1_uflash!=0))
return OK;
else
	return ERROR;
}
/*******************************************************************************
*  DESCRIPTION:
*                Check whether PMC280 has Boot flash or not.
* INPUT:
*       None.
*
* RETURN:
*       OK- Present
*      ERROR- Absent
*******************************************************************************/
STATUS isPMC280BootFlashPresent(void )
{
/*No way we could detect*/
return OK;
}
/*******************************************************************************
*  DESCRIPTION:
*                Check whether PMC280 has two Ethernet port or not.
* INPUT:
*       None.
*
* RETURN:
*       OK- Present
*      ERROR- Absent
*******************************************************************************/
STATUS isPMC280TwoOnBoardEth(void )
{

return OK;
}


/*******************************************************************************
*  DESCRIPTION:
*                Decode Board information EEPROM data.
* INPUT:
*       None.
*
* RETURN:
*       OK- Got board information data
*      ERROR- Absent
*******************************************************************************/
STATUS getPMC280BoardInfo(void )
{
UINT8 devAdd, buffer[2];
UINT32 data=0,temp;

devAdd = (UINT8)0xa4;

if ( frcEEPROMRead16(devAdd, 8160,2,buffer ) == false )
	{
	printf("\nError reading Board Information EEPROM.");
	return ERROR;
	}
/*As per PMC280 HES 004*/
data = buffer[0]| (((UINT32)buffer[1]) <<8);
printf("\n buf[0]= 0x%x  buf[1]=0x%x data = 0x%x",buffer[0],buffer[1],data);

sprintf(pmc280variant.PCB_rev,"E%d",data & 0x7); 

pmc280variant.BOM_rev = (data >> 3) & 0x7; 

switch (temp= ((data >>7 ) & 0x7) ) /*Determine Flash size*/
	{
	case 0:
		pmc280variant.bi_usrflash_size = 0;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		pmc280variant.bi_usrflash_size = 16 << (temp-1);
		break;
	case 5:
	case 6:
	case 7:
	default:
		pmc280variant.bi_usrflash_size = -1;
	}


switch ( temp =((data >>10 ) & 0x7 )) /*Determine RAM size*/
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		pmc280variant.bi_mem_size =128 << temp;
		break;
	case 5:
	case 6:
	case 7:
	default:
		pmc280variant.bi_mem_size = -1;
	}


pmc280variant.variant = 0;/*Unknown variant*/
temp = ((data >>13 ) & 0x7);
if (( temp >= 0) && (temp < 4))
	pmc280variant.variant = temp +'A'; 
return OK;
}





STATUS
isPMC280Monarch (void)
{
  unsigned int regVal = 0x0;
  GT_REG_READ (0xf104, &regVal);
#ifdef MONARCH_TEST_DBG
  printf ("Value read at the register 0xf104 is:%x", regVal);
#endif /*#ifdef MONARCH_TEST_DBG */
  if (regVal & 0x00000010)
  	return ERROR;

  else
  	return OK;

}


/*******************************************************************************
*  DESCRIPTION:
*                Detect User Flash
* INPUT:
*       None.
*
* RETURN:
*       OK- Got board information data
*      ERROR- Absent
*******************************************************************************/

void detectUserFlash ( void )
{
 int exc,dummy1,dummy2,size0,size1;

#ifdef DBG_USR_FLASH_TEST
          printf("\nDetecting BANK 0 at 0x%x..... ",FLASH_BANK0);
#endif
          exc = frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK0);
          if(exc == FLASH_SUCCESS)
          {
                size0 = (flashBlocks*SECSIZE) / 0x100000;        
#ifdef DBG_USR_FLASH_TEST
                printf("DONE,%d MB",size0);
#endif
          }        
          
          else
          	{
#ifdef DBG_USR_FLASH_TEST
                printf("FAILED");        
 #endif
                size0=0;
          	}               
#ifdef DBG_USR_FLASH_TEST
          printf("\nDetecting BANK 1 at 0x%x..... ",FLASH_BANK1);
#endif
          exc = frcFlashAutoSelect (&dummy1, &dummy2,FLASH_BANK1);
          if(exc == FLASH_SUCCESS)
          {
                size1 = (flashBlocks*SECSIZE) / 0x100000;
#ifdef DBG_USR_FLASH_TEST
                printf("DONE,%d MB",size1);
#endif
          }           
          else
          	{
#ifdef DBG_USR_FLASH_TEST
                printf("FAILED");        
 #endif
                size1=0;
          	}       
pmc280variant.bank0_uflash = size0;
pmc280variant.bank1_uflash = size1;
 
}

/*******************************************************************************
*  DESCRIPTION:
*                Detect actual memory size.
* INPUT:
*       None.
*
* RETURN:
*       OK- 
*      ERROR- Absent
*******************************************************************************/

void detectRAMsize ( void )
{
/*Convert size in MB*/
pmc280variant.ram_size=((UINT32)sysMemTop())/0x100000;

}
/*******************************************************************************
*  DESCRIPTION:
*                Display Board Information information
* INPUT:
*       None.
*
* RETURN:
*       OK- Got board information data
*      ERROR- Absent
*******************************************************************************/
STATUS frcDispBoardInfo ( void)
{
    frcPrintBoardInfo();
    return 0;
#if 0

if ( getPMC280BoardInfo()  != ERROR )
{
printf ("\n Could not read Board Information block.");
}
printf ("\n#######Board Information#######");
printf ("\nPMC280 Variant : ");
if ( pmc280variant.variant == 0 )
	printf ("Unknown.");
else
	printf ("%c" , pmc280variant.variant);
printf ("\nPCB rev            :%s", pmc280variant.PCB_rev);
printf ("\nBOM rev           :%d", pmc280variant.BOM_rev);
printf("\nMemory size       :%d MB", pmc280variant.bi_mem_size); 
printf("\nUser Flash size   :%d MB", pmc280variant.bi_usrflash_size); 
printf("\n##########################");
printf ("\nVerifying Baord Information data.");
/*Detect User Flash size*/
printf ("\n\nVerifying User Flash size.");
detectUserFlash();
printf ("\nBank0 :%d MB Bank1 :%d MB",pmc280variant.bank0_uflash,pmc280variant.bank1_uflash);
if ( pmc280variant.bi_usrflash_size != (pmc280variant.bank0_uflash + pmc280variant.bank1_uflash))
     printf ("\n*Wrong Board Information for User Flash size*");

/*Detect RAM size*/
printf ("\n\nVerifying Memory size.");
detectRAMsize();
if ( pmc280variant.bi_mem_size !=  pmc280variant.ram_size )
     printf ("\n*Wrong Board Information for Memory size*");

return OK;
#endif
}
