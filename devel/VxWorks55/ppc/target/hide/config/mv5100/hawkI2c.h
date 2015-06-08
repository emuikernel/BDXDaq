/* hawkI2c.h - Low-Level Hawk I2C header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1998-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,07mar01,rhk  WRS coding standards cleanup.
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from verison 01a of mcpn765/hawkI2c.h).
*/

/*
DESCRIPTION
This file contains the falcon/hawk i2c definitions.
*/

#ifndef INChawkI2ch
#define INChawkI2ch

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */


/* miscellaneous defines */

#define I2C_TIMEOUT_COUNT 100

/* i2c byte 1 (slave address) control bits */

#define BYTE_WRITE_MASK 0xFFFE   /* AND to clear bit 0 */
#define BYTE_READ_MASK  1        /* OR to set bit 0  */

/* Hawk I2C registers */

#define I2C_HAWK_CLOCK_REG   0xFEF80090
#define I2C_HAWK_CONTROL_REG 0xFEF80098
#define I2C_HAWK_STATUS_REG  0xFEF800A0
#define I2C_HAWK_TXD_REG     0xFEF800A8
#define I2C_HAWK_RXD_REG     0xFEF800B0

/* I2C_CONTROL_REG bits */

#define I2C_HAWK_START  (1<<3)
#define I2C_HAWK_STOP   (1<<2)
#define I2C_HAWK_ACKOUT (1<<1)
#define I2C_HAWK_ENBL   (1<<0)

/* I2C_STATUS_REG bits */

#define I2C_HAWK_DATIN  (1<<3)
#define I2C_HAWK_ERR    (1<<2)
#define I2C_HAWK_ACKIN  (1<<1)
#define I2C_HAWK_CMPLT  (1<<0)

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif  /* INChawkI2ch */
