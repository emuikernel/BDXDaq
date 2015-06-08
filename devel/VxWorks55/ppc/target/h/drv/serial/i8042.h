/* i8042.h - header file for the Intel 8042 Keyboard Controller */

/* Copyright 1993-2001 Wind River System, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,06dec01,jlb  written.



DESCRIPTION
This file contains the data structures and defines used to manipulate the
Intel 8042 keyboard/mouse driver chip.

*/

#ifndef __INCi8042h
#define __INCi8042h

#include "tyLib.h"

#ifdef __cplusplus
extern "C" {
#endif


/* status register defines */

#define I8042_KBD_OBFULL      0x01  /* output buffer full (data from keyboard) */
#define I8042_KBD_IBFULL      0x02  /* input buffer full (data to keyboard) */
#define I8042_KBD_SYSFLAG     0x04  /* system flag (0 power on reset, 1 st ok) */
#define I8042_KBD_CD          0x08  /* cmd/data (0 data wt on 60, 1 cmd on 64 */
#define I8042_KBD_KEYL        0x10  /* lock status (0 locked, 1 free) */
#define I8042_KBD_AUXB        0x20  /* aux device (0 keyboard data, 1 aux data)*/
#define I8042_KBD_RXTO        0x40  /* General timeout */
#define I8042_KBD_PARERR      0x80  /* Parity error */

/* configuration defines */

#define I8042_KBD_INT         0x01  /* Enable keyboard interrupts */
#define I8042_KBD_AUX_INT     0x02  /* Enable aux device interrupts */
#define I8042_KBD_OFF         0x10  /* Turn keyboard off */
#define I8042_KBD_XT          0x20  /* XT Scan codes */
#define I8042_KBD_AT_TO_XT    0x40  /* Convert AT scan codes to XT scan codes */

/* 8042 command defines */

#define I8042_KBD_RD_CONFIG   0x20  /* Read the configuration byte */
#define I8042_KBD_WT_CONFIG   0x60  /* Write the configuration byte */
#define I8042_KBD_DISABLE_AUX 0xA7  /* Disable aux device */
#define I8042_KBD_ENABLE_AUX  0xA8  /* Enable aux device */
#define I8042_KBD_IF_AUX_TEST 0xA9  /* Perform aux interface test */
#define I8042_KBD_SELF_TEST   0xAA  /* Perform keyboard self test */
#define I8042_KBD_IF_TEST     0xAB  /* Perform keyboard interface test */
#define I8042_KBD_ENABLE      0xAE  /* Enable keyboard (sets bit 4 of config) */
#define I8042_KBD_DISABLE     0xAD  /* Disable I8042_KBD (resets bit 4 of config) */
#define I8042_KBD_WT_AUX      0xD4  /* Write next data to aux device */

/* 8042 command results */

#define I8042_KBD_IF_OK       0x00  /* Clock and data lines are ok */
#define I8042_KBD_IF_CLKL     0x01  /* Clock line stuck low */
#define I8042_KBD_IF_CLKH     0x02  /* Clock line stuck high */
#define I8042_KBD_IF_DATAL    0x03  /* Data line stuck low */
#define I8042_KBD_IF_DATAH    0x04  /* Data line stuck high */
#define I8042_KBD_SF_OK       0x55  /* Passed self test */
#define I8042_KBD_NO_DEV      0xFF  /* No aux device */

/* keyboard command defines */

#define I8042_KBD_LED_CMD     0xED  /* Set LED command */
#define I8042_KBD_ECHO_CMD    0xEE  /* Echo (returns 0xee) */
#define I8042_KBD_SC_CMD      0xF0  /* Set/Identify scan code */
#define I8042_KBD_ID_CMD      0xF2  /* Get keyboard id code */
#define I8042_KBD_RATE_CMD    0xF3  /* Set repetition and delay rates */
#define I8042_KBD_ENABLE_CMD  0xF4  /* Enable keyboard */
#define I8042_KBD_STDDIS_CMD  0xF5  /* Standard Disable  */
#define I8042_KBD_STDEN_CMD   0xF6  /* Standard Enable   */
#define I8042_KBD_RESEND_CMD  0xFE  /* Resend last character */
#define I8042_KBD_RESET_CMD   0xFF  /* Reset keyboard */

/* keyboard command returns */

#define I8042_KBD_BAT_OK      0xAA  /* BAT Complete code  */
#define I8042_KBD_CMD_ACK     0xFA  /* Command Acknowledge */
#define I8042_KBD_BAT_ERROR   0xFC  /* BAT Error code */

/* LED defines */

#define I8042_KBD_LED_SCROLL  0x01   /* Scroll LED */
#define I8042_KBD_LED_NUMLOCK 0x02   /* Num Lock LED */
#define I8042_KBD_LED_CAPLOCK 0x04   /* Caps Lock LED */

/* ps2 mouse command defines */

#define I8042_KBDM_SR_CMD     0xE8  /* Mouse set resolution */
#define I8042_KBDM_DS_CMD     0xE9  /* Mouse Determine status */
#define I8042_KBDM_SS_CMD     0xF3  /* Mouse set sample rate */
#define I8042_KBDM_ENABLE_CMD 0xF4  /* Mouse enable */
#define I8042_KBDM_DIS_CMD    0xF5  /* Mouse disable */
#define I8042_KBDM_SETS_CMD   0xF6  /* Mouse set standard */
#define I8042_KBDM_RESEND_CMD 0xFE  /* Mouse resend last packet */
#define I8042_KBDM_RESET_CMD  0xFF  /* Mouse reset */

/* ps2 mouse command returns */

#define I8042_KBDM_ID         0x00  /* Mouse identification */
#define I8042_KBDM_ACK        0xFA  /* Command Acknowledge */
#define I8042_KBDM_RESEND     0xFE  /* Resend Request */

/* musc definitions */
#define I8042_READ_DELAY       100  /* Maximum wait time for keyboard */
#define I8042_WAIT_SEC           2  /* Time to wait for command completion */


/*  Register definitions */         
#define I8042_DATA               0  /* Data regsiter offset */
#define I8042_STAT_CMD           1  /* Status/Command regsiter offset */


/* Register access method, these may be overriden within the BSP config.h file */
#ifndef I8042_KBD_OUT
#define I8042_KBD_OUT sysOutByte    /* Routine to call for output */
#define I8042_KBD_IN  sysInByte     /* Routine to call for input */
#endif /* KBD_OUT */


/* Mouse device control structure */
typedef struct
    {
    TY_DEV          ty_dev;
    WINDML_DEVICE * pDev;
    ULONG           dataReg;
    ULONG           statCmdReg;
    } I8042_MSE_DEVICE;


/* keyboard device descriptor */
typedef struct
    {
    TY_DEV          tyDev;
    WINDML_DEVICE * pDev;
    UINT16          kbdFlags;   
    ULONG           oldLedStat;
    ULONG           dataReg;
    ULONG           statCmdReg;
    } I8042_KBD_DEVICE;



#ifdef __cplusplus
}
#endif

#endif  /* __INCi8042h */
