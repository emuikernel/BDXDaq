/* w83601Gpio.h - Winbond General Purpose Input/Output driver header */

/* Copyright 2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,03aug01,srr  Written.
*/

/*
This file contains the configuration parameters for the
Winbond W83601R/602R General Purpose Input/Output ICs with SMBus.
*/

#ifndef	INCw83601Gpioh
#define	INCw83601Gpioh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* GPIO Registers */

#define W83601_GP1_DATA_IN		0x0	/* GP1 Input Data Register */
#define W83601_GP1_DATA_OUT		0x1	/* GP1 Output Data Register */
#define W83601_GP1_POLARITY		0x2	/* GP1 Polarity Inversion Reg */
#define W83601_GP1_IO_CFG		0x3	/* GP1 I/O Config Reg */
#define W83601_GP1_OUT_CTRL		0x4	/* GP1 Output Control Reg */
#define W83601_GP1_LATCHED_IN		0x5	/* GP1 Latched Data Reg */

#define W83601_GP2_DATA_IN		0x8	/* GP2 Input Data Register */
#define W83601_GP2_DATA_OUT		0x9	/* GP2 Output Data Register */
#define W83601_GP2_POLARITY		0xa	/* GP2 Polarity Inversion Reg */
#define W83601_GP2_IO_CFG		0xb	/* GP2 I/O Config Reg */
#define W83601_GP2_OUT_CTRL		0xc	/* GP2 Output Control Reg */
#define W83601_GP2_LATCHED_IN		0xd	/* GP1 Latched Data Reg */

#define W83601_GP1_INT_STAT		0x10	/* GP1 Interrupt Status Reg */
#define W83601_GP2_INT_STAT		0x11	/* GP2 Interrupt Status Reg */
#define W83601_GP1_INT_EN		0x12	/* GP1 Interrupt Enable Reg */
#define W83601_GP2_INT_EN		0x13	/* GP2 Interrupt Enable Reg */

#define W83601_MODE_CFG			0x14	/* Mode Configuration Reg */
#define W83601_LED_CFG			0x15	/* LED Configuration Reg */

#define W83601_ID_HIGH			0x20	/* Chip ID High Byte Reg */
#define W83601_ID_LOW			0x21	/* Chip ID Low Byte Reg */
#define W83602_ID_LOW			0x22	/* 83602 Chip ID Low Byte Reg */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCw83601Gpioh */
