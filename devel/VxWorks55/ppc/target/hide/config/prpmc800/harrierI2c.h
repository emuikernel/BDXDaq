/* harrierI2c.h - Low-Level Harrier I2C header file */

/* Copyright 1998-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,06nov00,krp   Changed the define for I2C write mask.
01b,07sep00,dmw   Defined system I2c macros.
01a,31aug00,dmw   Written (from verison 01b of prpmc750/hawkI2c.h).
*/

/*
DESCRIPTION
This file contains the Harrier i2c definitions.
*/

#ifndef INCharrierI2ch
#define INCharrierI2ch

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */


/* miscellaneous defines */

#define I2C_TIMEOUT_COUNT 100


/* i2c byte 1 (slave address) control bits */

#define BYTE_WRITE_MASK 0xFE     /* AND to clear bit 0 */
#define BYTE_READ_MASK  1        /* OR to set bit 0  */


/* System I2C registers */

#define SYS_I2C_CLOCK_REG   (HARRIER_I2C_CONTROLLER0_REG + \
                             HARRIER_I2C_PRESCALER_OFFSET)
#define SYS_I2C_CONTROL_REG (HARRIER_I2C_CONTROLLER0_REG + \
                             HARRIER_I2C_CONTROL_OFFSET)
#define SYS_I2C_STATUS_REG  (HARRIER_I2C_CONTROLLER0_REG + \
                             HARRIER_I2C_STATUS_OFFSET)
#define SYS_I2C_TXD_REG     (HARRIER_I2C_CONTROLLER0_REG + \
                             HARRIER_I2C_TRANSMITTER_DATA_OFFSET)
#define SYS_I2C_RXD_REG     (HARRIER_I2C_CONTROLLER0_REG + \
                             HARRIER_I2C_RECEIVER_DATA_OFFSET)

/* SYS_I2C_CONTROL_REG bits */

#define SYS_I2C_START       HARRIER_I2CO_STA  /* i2c mstr controller start */
#define SYS_I2C_STOP        HARRIER_I2CO_STP  /* i2c mstr controller stop */
#define SYS_I2C_ACKOUT      HARRIER_I2CO_ACKO /* i2c mstr controller ack */
#define SYS_I2C_ENBL        HARRIER_I2CO_ENA  /* i2c mstr interface enable */


/* SYS_I2C_STATUS_REG bits */

#define SYS_I2C_DATIN       HARRIER_I2ST_DIN  /* data in from i2c slave */
#define SYS_I2C_ERR         HARRIER_I2ST_ERR  /* error: STA & STP both set */
#define SYS_I2C_ACKIN       HARRIER_I2ST_ACKI /* acknowledge in slave */
#define SYS_I2C_CMPLT       HARRIER_I2ST_CMP  /* i2c operation complete */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif  /* INCharrierI2ch */
