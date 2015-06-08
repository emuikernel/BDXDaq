/* mv64360I2c.h - Low Level MV64360 I2C Header File */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,31mar03,simon  Ported. (from ver 01a, mcpm905/mv64360I2c.h)
*/

/* 
DESCRIPTION
This file contains the MV64360 I2C definitions.
*/

#ifndef INCmv64360I2ch
#define INCmv64360I2ch

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* I2C Control Register Bits */

#define I2C_CTRL_ACK_SHIFT		2
#define I2C_CTRL_INTFLG_SHIFT		3
#define I2C_CTRL_STOP_SHIFT		4
#define I2C_CTRL_START_SHIFT		5
#define I2C_CTRL_ENABLE_SHIFT		6
#define I2C_CTRL_INTENABLE_SHIFT	7

#define I2C_CTRL_ACK_MASK		(1 << I2C_CTRL_ACK_SHIFT)
#define I2C_CTRL_INTFLG_MASK		(1 << I2C_CTRL_INTFLG_SHIFT)
#define I2C_CTRL_STOP_MASK		(1 << I2C_CTRL_STOP_SHIFT)
#define I2C_CTRL_START_MASK		(1 << I2C_CTRL_START_SHIFT)
#define I2C_CTRL_ENABLE_MASK		(1 << I2C_CTRL_ENABLE_SHIFT)
#define I2C_CTRL_INTENABLE_MASK		(1 << I2C_CTRL_INTENABLE_SHIFT)

/* I2C Status Register Codes */

#define I2C_STATUS_BUSERR			0x00
#define I2C_STATUS_STARTTX			0x08
#define I2C_STATUS_REPEATSTARTX			0x10
#define I2C_STATUS_ADDRWRITEACK			0x18
#define I2C_STATUS_ADDRWRITENACK		0x20
#define I2C_STATUS_MSTDATAWRITEACK		0x28
#define I2C_STATUS_MSTDATAWRITENACK		0x30
#define I2C_STATUS_MSTLOSTARB			0x38
#define I2C_STATUS_ADDRREADACK			0x40
#define I2C_STATUS_ADDRREADNACK			0x48
#define I2C_STATUS_MSTDATARECVACK		0x50
#define I2C_STATUS_MSTDATARECVNACK		0x58
#define I2C_STATUS_SLVADDRRECACK		0x60
#define I2C_STATUS_MSTLOSTARBADDR		0x68
#define I2C_STATUS_GENCALLRECVACK		0x70
#define I2C_STATUS_MSTLOSTARBADDRGC		0x78
#define I2C_STATUS_SLVRECWDATAACK		0x80
#define I2C_STATUS_SLVRECWDATANACK		0x88
#define I2C_STATUS_SLVRECWDATAACKGC		0x90
#define I2C_STATUS_SLVRECWDATANACKGC		0x98
#define I2C_STATUS_SLVRECSTOP			0xA0
#define I2C_STATUS_SLVRECADDRACK		0xA8
#define I2C_STATUS_MSTLOSTARBADDRTX		0xB0
#define I2C_STATUS_SLVTXRDATAACK		0xB8
#define I2C_STATUS_SLVTXRDATANACK		0xC0
#define I2C_STATUS_SLVTXLASTDATAACK		0xC8
#define I2C_STATUS_SECADDRWTXACK		0xD0
#define I2C_STATUS_SECADDRWTXNACK		0xD8
#define I2C_STATUS_SECADDRRTXACK		0xE0
#define I2C_STATUS_SECADDRRTXNACK		0xE8
#define I2C_STATUS_NOSTATUS			0xF8

/* forward declarations */

STATUS sysMv64360I2cAckIn (UINT32);
STATUS sysMv64360I2cAckOut (UINT32);
STATUS sysMv64360I2cStop (UINT32);
STATUS sysMv64360I2cStart (UINT32);
STATUS sysMv64360I2cRead (UINT32, UCHAR *, BOOL);
STATUS sysMv64360I2cWrite (UINT32, UCHAR *);
void sysMv64360UsDelay (UINT32);

#ifdef __cplusplus
    }
#endif

#endif /* INCmv64360I2ch */
