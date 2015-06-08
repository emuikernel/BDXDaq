/* PLDI2CDrv.h - Low Level PLD I2C Header File */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon created.
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

/* includes */

#include "vxWorks.h"
#include "config.h"

/* defines  */

#define SDA_INPUT_BIT  0x08
#define SDA_INPUTSHIFT_NUM  0x03
#define SDA_DIR_BIT  0x04
#define SDA_CLK_BIT  0x02
#define SDA_OUTPUT_BIT 0x01
#define I2C_REG_ADDR MV64360_DEVCS2_BASE_ADDR+0x06

#define DEV_ADDR 0xA0
#define MAX_ACK_WAIT_TIME 0x20

/* typedefs */

STATUS PLDI2CRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to read */
    UCHAR  *pBfr      /* pointer to data byte */
    );
STATUS PLDI2CWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    USHORT  devOffset, /* offset within target device to write */
    UCHAR  *pBfr      /* pointer to data byte */
    );
STATUS PLDI2CBurstWrite
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR  *pBfr     /* pointer to data byte */
    );
STATUS PLDI2CBurstRead
    (
    UCHAR   devAddr,   /* i2c address of target device */
    UCHAR  *pBfr      /* pointer to data byte */
    );

#ifdef __cplusplus
    }
#endif

#endif /* PLDI2CDrvh */

