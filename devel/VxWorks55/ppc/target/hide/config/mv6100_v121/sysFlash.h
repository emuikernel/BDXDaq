/* sysFlash.h - flash header file */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,15apr04,cak Modified for MVME6100.
01b,30jan04,nrv modified for mv5500 bsp
01a,07may02,gtf modified for wrSbc7410 bsp.
*/

/*
This file contains various definitions for the Intel Strataflash
driver for the mv6100 processor board.
*/

#ifndef _sysFlash_H_
#define _sysFlash_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Flash types */

#define INTEL28F320_SECTOR_SZ   (UINT32)0x20000*2 /* 128KB x 2 in x16 mode */
#define INTEL28F320_NUM_SECTORS (32-4)
#define INTEL28F320_SIZE	(UINT32)(INTEL28F320_SECTOR_SZ * INTEL28F320_NUM_SECTORS)

#define INTEL28F640_SECTOR_SZ   (UINT32)0x20000*2 /* 128KB x 2 in x16 mode */
#define INTEL28F640_NUM_SECTORS (64-4)
#define INTEL28F640_SIZE	(UINT32)(INTEL28F640_SECTOR_SZ * INTEL28F640_NUM_SECTORS)

#define INTEL28F128_SECTOR_SZ   (UINT32)0x20000*2 /* 128KB x 2 in x16 mode */
#define INTEL28F128_NUM_SECTORS (128-4)
#define INTEL28F128_SIZE	(UINT32)((UINT32)INTEL28F128_SECTOR_SZ * (UINT32)INTEL28F128_NUM_SECTORS)

#define INTEL28F256_SECTOR_SZ   (UINT32)0x20000*2 /* 128KB x 2 in x16 mode */
#define INTEL28F256_NUM_SECTORS (256-4)
#define INTEL28F256_SIZE	(UINT32)((UINT32)INTEL28F256_SECTOR_SZ * (UINT32)INTEL28F256_NUM_SECTORS)

/* 128KB * 2 chips = 256KB per logical sector */

#define INTEL_MFGR_ID		0x00890089
#define INTEL28F320_DEVICE_ID   0x00160016
#define INTEL28F640_DEVICE_ID 	0x00170017
#define INTEL28F128_DEVICE_ID   0x00180018
#define INTEL28F256_DEVICE_ID   0x88038803

#ifndef _ASMLANGUAGE
typedef union _FlashWord
    {
	unsigned char  c[4] ;
	unsigned short s[2] ;
	unsigned long  l ;
	} FlashWord ;
#endif

IMPORT int checkStatusIntel(UINT32 status1) ;
IMPORT STATUS sysFlashReadIdIntel(volatile FlashWord *adrs) ;
IMPORT STATUS sysBlockLockStatusIntel(FlashWord * adrs) ;
IMPORT STATUS sysFlashPollDataIntel(FlashWord * adrs) ;
IMPORT STATUS sysFlashSectorEraseIntel(char *sectorAdrs) ;
IMPORT STATUS sysFlashChipEraseIntel(UINT32 adrs) ;
IMPORT STATUS sysFlashSectorWriteBufferIntel(FlashWord *adrs, char * buf, int len) ;
IMPORT STATUS sysFlashSectorLockIntel(char *adrs) ;
IMPORT STATUS sysFlashSectorUnlockIntel(char *adrs) ;
IMPORT STATUS sysFlashSectorWriteIntel(char * adrs, char * buf, int len) ;
IMPORT STATUS sysFlashEraseStrataflash(UINT8 * sectorAdrs) ;
IMPORT STATUS sysFlashWriteStrataflash( UINT8 * sectorAdrs, UINT8 * buffer, int length) ;
IMPORT STATUS sysFlashWriteBufferStrataflash( UINT8 * sectorAdrs, UINT8 * buffer, int length) ;

#ifdef __cplusplus
}
#endif
#endif /* _sysFlash_H_ */
