/* i28F640j3Mem.c - Intel 28F640J3 Flash Memory library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,16apr01,gh   written
*/

/*
DESCRIPTION
This library contains routines to manipulate Intel 28F640J3 Flash memory.
Read and write routines are included.

The macros NV_RAM_ADRS and NV_RAM_SIZE must be defined to indicate the
address and size of the Flash memory.

This module use the visionWare flash algorithm.

The Intel 28F640J3 access is BITS:8 BUS:8 ACCESS:INTEGER.
*/

/* includes */
#include "drv/mem/i28f640j3mem.h"
#include "memLib.h"
#include "stdlib.h"

/* LOCALs */
LOCAL void   IOSync(void){ }
LOCAL UINT32 FlashWrite(volatile UINT8 *pDest, UINT8 *pSource, UINT32 Length);
LOCAL UINT32 FlashEraseSector(volatile UINT8 *pSector);

LOCAL const tFLASH Erase1 = 		{ 0x20 };
LOCAL const tFLASH Erase2 = 		{ 0xD0 };
LOCAL const tFLASH Program =		{ 0x40 };
LOCAL const tFLASH ReadArray =		{ 0xFF };
LOCAL const tFLASH ClearStatus =	{ 0x50 };
LOCAL const tFLASH Erased =    { (UINT8)-1 };
LOCAL const tFLASH ReadyBits =		{ 0x80 };
LOCAL const tFLASH EraseBits =		{ 0x20 };
LOCAL const tFLASH ProgramBits =	{ 0x10 };
LOCAL const tFLASH BufferWrite =	{ 0xE8 };
LOCAL const tFLASH ConfirmWrite =	{ 0xD0 };
LOCAL const tFLASH BufferCount =	{ 0x1F };

/*****************************************************************************
*
* sysNvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string will be terminated with an EOS.
*
* INTERNAL
* The board has no NVRAM, so we've implemented some pseudo-NVRAM
* in the flash.
*
* RETURNS: OK or ERROR
*
* SEE ALSO: sysNvRamSet()
*/

STATUS sysNvRamGet
    (
    char *	string,		/* where to copy flash memory      */
    int		strLen,		/* maximum number of bytes to copy */
    int		offset		/* byte offset into flash memory   */
    )
{
	char	*pNvRam,*pString;
	int	i;

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    /* Check that the string is entirely within the NVRAM */
    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return ERROR;       

    pNvRam  = (char *)(NV_RAM_ADRS + offset);
    pString =  (char *)string;
    for (i = 0; i < strLen; i++)
        *pString++ = *pNvRam++;

    *pString = EOS;       /* force EOS onto nvram string */

    /* The generic boot-line code expects this function to check the contents
	 * and report any errors. The default boot line is only used if this 
	 * function returns ERROR. So, we return ERROR if the first character
     * copied is 0xFF (= erased flash).
     */

    return ((*string == (char)0xff) ? ERROR : OK);
}

/******************************************************************************
*
* sysNvRamSet - write data to non-volatile RAM
*
* This routine copies the provided string to the non-volatile memory.
*
* INTERNAL
* The board has no NVRAM, so we've implemented some pseudo-NVRAM
* in the flash.
* The "NVRAM" is a small region at the very top of flash memory, and can
* thus be read using normal memory accesses. However, to write the data we
* have to read the entire top sector, modify its contents, erase the top
* sector, and then reprogram it.
*
* RETURNS: OK or ERROR
*
* SEE ALSO: sysNvRamGet()
*/

STATUS sysNvRamSet
    (
    char *	string,		/* string to be copied into flash memory */
    int		strLen,		/* maximum number of bytes to copy       */
    int		offset		/* byte offset into flash memory         */
    )
{
	UINT32	blockOffs;
	char	*pBlock, *pTmpBuf;

    offset += NV_BOOT_OFFSET;   /* boot line begins at <offset> = 0 */

    /* Check that the string will fit entirely into the NVRAM */
    if ((offset < 0) || (strLen < 0) || ((offset + strLen) > NV_RAM_SIZE))
        return ERROR;

    /* see if contents are actually changing */
    if (bcmp ((char *) (NV_RAM_ADRS + offset), string, strLen) == 0)
	return OK;

    /* Allocate a temporary buffer for the operation */
    pTmpBuf = (char *)malloc(FLASH_SECTOR_SIZE);
    if ( pTmpBuf == NULL )
        return ERROR;

    pBlock = (UINT8 *)((UINT32)(NV_RAM_ADRS + offset) & ~(FLASH_SECTOR_SIZE-1));

    /* Copy current contents of flash (entire sector) to the buffer */
    memcpy(pTmpBuf,(void*)pBlock,FLASH_SECTOR_SIZE);

    /* Now erase the flash sector */
    FlashEraseSector((volatile UINT8 *)pBlock);

    blockOffs = (NV_RAM_ADRS + offset) & (FLASH_SECTOR_SIZE-1);

    /* Overwrite buffer contents with specified string */
    memcpy((void*)(pTmpBuf + blockOffs),string,strLen);

    /* Now program the flash sector */
    FlashWrite((volatile UINT8 *)pBlock, pTmpBuf, FLASH_SECTOR_SIZE);

    free (pTmpBuf);
    return OK;
}

/******************************************************************************
*
* FlashWrite - write data to the Intel 29F640J3
*
* RETURNS: Number of byte writen
*
* NOMANUAL
*/

LOCAL UINT32 FlashWrite(volatile UINT8 *pDest, UINT8 *pSource, UINT32 Length)
{
	unsigned int OriginalLength = Length;
	tFLASH Data;
	unsigned int n;
	volatile UINT8 *pBlock = (UINT8 *)((UINT32)pDest & ~0x1FFFF);
	UINT8 TempBuffer[0x20];

	while(Length)
	{
		*(volatile UINT8 *)pDest = ReadArray.Float;

		if ((((UINT32)pDest & 0x1F) == 0) && (Length >= 0x20))
		{
			memcpy((char *)&TempBuffer, pSource, 0x20);
			*pBlock = BufferWrite.Float;
			IOSync();
			if (*pBlock == ReadyBits.Integer)
			{
				*pBlock = BufferCount.Float;

				for (n = 0; n < 0x20; ++n)
					((volatile UINT8 *)pDest)[n] = TempBuffer[n];

				*pBlock = ConfirmWrite.Float;

				while (*(volatile UINT8 *)pDest != ReadyBits.Integer)
					;

				pDest += 0x20;
				pSource += 0x20;
				Length -= 0x20;

				continue;
			}

			*(volatile UINT8 *)pDest = ReadArray.Float;
		}

		Data.Integer = *(UINT8 *)pSource++;

		*(volatile UINT8 *)pDest = ClearStatus.Float;
		IOSync();
		*(volatile UINT8 *)pDest = Program.Float;
		IOSync();
		*(volatile UINT8 *)pDest = Data.Float;
		IOSync();

		while (*(volatile UINT8 *)pDest != ReadyBits.Integer)
			;

		++pDest;
		--Length;
	}

	*(volatile UINT8 *)(pDest - 1) = ReadArray.Float;
	return OriginalLength - Length;
}

/******************************************************************************
*
* FlashEraseSector - erase sector in the Intel 29F640J3
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL UINT32 FlashEraseSector(volatile UINT8 *pSector)
{
	*(volatile UINT8 *)pSector = ClearStatus.Float;
	IOSync();
	*(volatile UINT8 *)pSector = Erase1.Float;
	IOSync();
	*(volatile UINT8 *)pSector = Erase2.Float;
	IOSync();

	while (*(volatile UINT8 *)pSector != ReadyBits.Integer);

	*(volatile UINT8 *)pSector = ReadArray.Float;

	return 0;
}
