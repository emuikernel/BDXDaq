/* flash29.h - header for 29F0X0 FLASH memory devices */

/* Copyright 1994-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,13feb01,frf  added AM29LV040B definition.
01e,23mar00,zl   added 29LV160T and 29LV160B definitions
01d,06mar98,jpd  added 29LV1024 and 29C040A definitions.
01c,26jan96,dzb  added casts for CMD constants.
           +jpb  changed CAST to CAST_CH.
01b,24feb94,dzb  made device code 1 byte.
01a,10feb94,dzb  created.
*/

/*
DESCRIPTION
This file contains header information for 29F0X0 FLASH memory devices.
*/

#ifndef __INCflash29h
#define __INCflash29h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	_ASMLANGUAGE
#define CAST_CH
#else	/* _ASMLANGUAGE */
#define CAST_CH (char *)
#endif	/* _ASMLANGUAGE */

#ifndef FLASH29_REG_ADRS
#define FLASH29_REG_ADRS(reg) (CAST_CH FLASH_ADRS + (reg * FLASH_WIDTH))
#endif  /* FLASH29_REG_ADRS */

/* FLASH29 command register addresses */

#define FLASH29_REG_FIRST_CYCLE		FLASH29_REG_ADRS (0x5555)
#define FLASH29_REG_SECOND_CYCLE	FLASH29_REG_ADRS (0x2aaa)

/* FLASH29 command definitions */

#define	FLASH29_CMD_FIRST		(FLASH_DEF) 0xaaaaaaaa
#define	FLASH29_CMD_SECOND		(FLASH_DEF) 0x55555555
#define	FLASH29_CMD_FOURTH		(FLASH_DEF) 0xaaaaaaaa
#define	FLASH29_CMD_FIFTH		(FLASH_DEF) 0x55555555
#define	FLASH29_CMD_SIXTH		(FLASH_DEF) 0x10101010
#define	FLASH29_CMD_SECTOR		(FLASH_DEF) 0x30303030

#define	FLASH29_CMD_PROGRAM		(FLASH_DEF) 0xa0a0a0a0
#define	FLASH29_CMD_CHIP_ERASE		(FLASH_DEF) 0x80808080
#define	FLASH29_CMD_READ_RESET		(FLASH_DEF) 0xf0f0f0f0
#define	FLASH29_CMD_AUTOSELECT		(FLASH_DEF) 0x90909090
 
#define	FLASH_29F010			0x20	/* device code 29F010 */
#define	FLASH_29C040A			0xA4	/* device code 29C040A */
#define	FLASH_29LV1024			0x26	/* device code 29LV1024 */
#define	FLASH_29LV160T			0xC4	/* device code 29LV160T */
#define	FLASH_29LV160B			0x49	/* device code 29LV160B */
#define	FLASH_29LV040B			0x4f	/* device code 29LV040B */

#ifdef __cplusplus
}
#endif

#endif /* __INCflash29h */
