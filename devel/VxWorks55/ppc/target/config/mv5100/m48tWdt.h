/* m48tWdt.h - STMicroelectronics RTC watchdog driver header */

/* Copyright 2000 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01d,05nov01,kab  SPR 69316: Corrected typo in func prototypes
01c,16aug00,dmw  Wind River coding standards review.
01b,26may00,dmw  Added M48T37V support.
01a,21mar00,rhv  written - Motorola.
*/

/*
This file contains the configuration parameters and control functions for the
SGS-Thomsom M48T37V and K48T559 Watchdog timer functions.
*/

#ifndef	INCm48tWdth
#define	INCm48tWdth

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* WDT resolutions */

#define M48TWDT_RESOLUTION_16THS    0	/* 16 ticks/Sec */
#define M48TWDT_RESOLUTION_4THS     1	/* 4  ticks/Sec */
#define M48TWDT_RESOLUTION_SECS     2	/* 1  tick /Sec */
#define M48TWDT_RESOLUTION_4_SECS   3	/* 1  tick/4 secs */
#define M48TWDT_RESOLUTION_MASK     3   /* resolution mask */

/* multiplier (WDT timer "ticks" until expiration */

#define M48TWDT_MULTIPLIER_MAX      31  /* maximum multiplier */
#define M48TWDT_MULTIPLIER_SHIFT    2   /* multiplier shift value */
#define M48TWDT_MULTIPLIER_MASK     (M48TWDT_MULTIPLIER_MAX \
                                         << M48TWDT_MULTIPLIER_SHIFT)

/* WDT expiration routing */

#define M48TWDT_WDS_RESET           1	/* generate board reset */
#define M48TWDT_WDS_INTERRUPT       0	/* generate interrupt */
#define M48TWDT_WDS_SHIFT           7   /* shift value */
#define M48TWDT_WDS_MASK            (1 << M48TWDT_WDS_SHIFT)

/* global functions */

STATUS sysM48tWdtSet  (UCHAR seconds, BOOL reset);
STATUS sysM48tWdtGet  (UCHAR * seconds, BOOL * reset);
STATUS sysM48tWdtShow (void);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCm48tWdth */
