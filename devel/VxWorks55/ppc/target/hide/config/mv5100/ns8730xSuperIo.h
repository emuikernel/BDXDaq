/* ns8730xSuperIo.h - National Semiconductor 8730X chip header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996-1998,2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,25oct00,cak  Added support for any revision of 87307 Super I/O chip.
01c,16aug00,dmw  Wind River coding standards review.
01b,10apr00,dmw  added 97307 id and expanded id mask.
01a,15dec97,rhk  written.
*/

/*
This file contains defines specific to the National Semiconductor pc8730x
series chips.
*/

#ifndef INCns8730xSuperIoh
#define INCns8730xSuperIoh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* defines */

#define SIO_LUNINDEX	0x07       /* SIO LUN index register */
#define SIO_CNFG1	0x21       /* SIO configuration #1 register */
#define SIO_CNFG2	0x22       /* SIO configuration #2 register */
#define SIO_PCSCI	0x23       /* SIO PCS configuration index reg */
#define SIO_PCSCD	0x24       /* SIO PCS configuration data reg */
#define SIO_SID		0x08       /* SIO identifier register */
#define SIO_SIDPNP	0x20       /* SIO identifier register - PnP */
#define SIO_CNFG1_MASK  0xFB	   /* keep all but PS/2-AT mode bit */

#define SIO_SIDMASK	0xF8       /* SIO identifier mask */
#define SIO_SID87303	0x30       /* SIO identifier - 87303 */
#define SIO_SID87323	0x20       /* SIO identifier - 87323 */
#define SIO_SID87307	0xC0       /* SIO identifier - 87307 */
#define SIO_SID87308	0xA0       /* SIO identifier - 87308 */
#define SIO_SID97307	0xC8       /* SIO identifier - 97307 */

#define SIO_ACTIVATE	0x30       /* SIO activate register */
#define SIO_IOBASEHI	0x60       /* SIO I/O port base address, 15:8 */
#define SIO_IOBASELO	0x61       /* SIO I/O port base address,  7:0 */
#define SIO_DBASEHI	0x60       /* SIO KBC data base address, 15:8 */
#define SIO_DBASELO	0x61       /* SIO KBC data base address,  7:0 */
#define SIO_CBASEHI	0x62       /* SIO KBC command base addr, 15:8 */
#define SIO_CBASELO	0x63       /* SIO KBC command base addr,  7:0 */
#define SIO_IRQREQ	0x71       /* SIO interrupt req register */
#define SIO_LUNCNFGR	0xF0       /* SIO LUN configuration register */

#define SIO_LUNENABLE	0x01	   /* SIO LUN enable */
#define SIO_LUNDISABLE	0x00       /* SIO LUN disable */

#define PID_MASK	0xF0       /* processor identifier mask */
#define PID_CLARIION	0xC0       /* processor identifier, CLARIION */
#define PID_VIPER	0xD0       /* processor identifier, VIPER */
#define PID_GENESIS2	0xE0       /* processor identifier, GENESIS2 */

#define SIO_LUN_KBC	0x00	   /* Keyboard LUN */
#define SIO_LUN_MOU     0x01	   /* Mouse LUN */
#define SIO_LUN_RTC	0x02	   /* RTC and APC LUN */
#define SIO_LUN_FDC	0x03	   /* FDC LUN */
#define SIO_LUN_PP	0x04	   /* parallel port LUN */
#define SIO_LUN_COM2	0x05	   /* COM2 LUN */
#define SIO_LUN_COM1	0x06	   /* COM1 LUN */
#define SIO_LUN_GPIO	0x07	   /* GPIO port LUN */
#define SIO_LUN_PWR	0x08	   /* power management LUN */

/* typedefs */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* INCns8730xSuperIoh */
