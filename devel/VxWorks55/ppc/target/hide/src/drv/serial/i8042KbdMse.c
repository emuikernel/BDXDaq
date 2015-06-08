/* i8042KbdMse.c - Intel 8042 keyboard/mouse driver routines */

/* Copyright 1993-2002 Wind River System, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02b,14jun02,gav  Fixed storage class warning from DIAB PPC compiler.
02a,17may02,rfm  Removed static declaration for i8042KbdStatInit
01a,06dec01,jlb  created
*/

/*
DESCRIPTION
This is the driver for the Intel 8042 Keyboard/Mouse Controller Chip. The
Intel 8042 is the most common device that is used to implement the keyboard
and mouse ports.  Although, this device is no longer available, it is emulated 
by various super I/O chips.  Any device specific needs of these super I/O devices
to enable the keyboard/mouse controller must be handled external to this driver. 
That is, this driver only pertains to the Intel 8042  specifics.  
 
This driver requires that the BSP implement the WindML BSP API (sysWindML.c)
to obtain the device addresses and to enable/disable interrupt handling.

Two devices are created, one for the keyboard and one for the pointer/mouse.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  i8042MseDevCreate() to
create the  mouse driver, and i8042KbdDevCreate() to create keyboard device.

IOCTL FUNCTIONS
This driver responds to all the same ioctl() codes as a normal tty driver;
for more information, see the manual entry for tyLib.  In addition, the keyboard
driver has ioctls to control the settings of the LEDs on the keyboard.


NOTES
The keyboard driver must be initialized prior to the initialization of the
mouse port.  The keyboard port may be handled by the pcConsole driver that is
present for the Pentium family of processors.  When the PC Console is included
the keyboard driver present within this file is not built, that is when 
INCLUDE_PC_CONSOLE is defined.

*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "errnoLib.h"
#include "wdLib.h"
#include "sysLib.h"
#include "intLib.h"
#include "taskLib.h"
#include "ugl/sysWindML.h"
#include "drv/serial/i8042.h"
#include "drv/serial/pcConsole.h"

LOCAL int       i8042TimeoutCnt;
LOCAL BOOL      i8042Timeout;
LOCAL WDOG_ID   i8042Wdid;

/* forward declarations */

#ifndef INCLUDE_PC_CONSOLE
LOCAL void      i8042KbdLedSet (I8042_KBD_DEVICE * pKbdDv, UCHAR led);
LOCAL void      i8042KbdReset (I8042_KBD_DEVICE * pKbdDv);
LOCAL int       i8042KbdWriteData (I8042_KBD_DEVICE * pKbdDv);
LOCAL void      i8042KbdInt (I8042_KBD_DEVICE * pKbdDv);
LOCAL int       i8042KbdOpen ();
LOCAL STATUS    i8042KbdIoctl (I8042_KBD_DEVICE * pKbdDv, int request, int arg);
LOCAL void      i8042KbdHrdInit (I8042_KBD_DEVICE * pKbdDv);
#endif /* INCLUDE_PC_CONSOLE */

LOCAL int       i8042MseOpen();
LOCAL int       i8042MseClose();
LOCAL void      i8042MseTxStart();
LOCAL void      i8042MseInt (I8042_MSE_DEVICE  *pDev);
LOCAL void      i8042MseHwInit (I8042_MSE_DEVICE  * pI8042MseDevice);

/*******************************************************************************
*
* i8042MseDevCreate - mouse driver initialization routine
*
* This routine creates a device for a mouse.  The 8042 hardware is
* initialized when the keyboard interface is initialized.  Therefore, the
* keyboard port must be initialized prior to the initialization of the mouse
* port.  This may be done either by the use of the keyboard driver contained
* within this file or by the inclusion of the keyboard driver used by the
* PC Console driver.
*
* RETURNS: device number, if successful; otherwise ERROR.
*
* SEE ALSO:
*/

int i8042MseDevCreate
    (
    char         *name            /* name to be associated with device   */
    )
    {

    int                 i8042MseDrvNum;
    I8042_MSE_DEVICE  * pI8042MseDevice;


    /* Install the driver and return installation status */

    i8042MseDrvNum = iosDrvInstall(i8042MseOpen, (FUNCPTR) NULL, i8042MseOpen,
                             (FUNCPTR) i8042MseClose, tyRead, tyWrite, tyIoctl);

    /* Allocate device structure */

    pI8042MseDevice = (I8042_MSE_DEVICE *)malloc (sizeof(I8042_MSE_DEVICE));

    /* Create a tty device  */

    if (tyDevInit(&pI8042MseDevice->ty_dev, 512, 512, (FUNCPTR)i8042MseTxStart) != OK)
        return (ERROR);

     /* Get the device access information */

    pI8042MseDevice->pDev = sysWindMLDevGet(WINDML_POINTER_DEVICE, 0, 0, 0);
    if (pI8042MseDevice->pDev == NULL)
        {
        free (pI8042MseDevice);
        return (ERROR);
        }

    /* Set up register access locations */

    pI8042MseDevice->statCmdReg =  (ULONG)pI8042MseDevice->pDev->pRegBase +
                       ((ULONG)pI8042MseDevice->pDev->regDelta * I8042_STAT_CMD);
    pI8042MseDevice->dataReg =  (ULONG)pI8042MseDevice->pDev->pRegBase +
                       ((ULONG)pI8042MseDevice->pDev->regDelta * I8042_DATA);

    /* Connect the interrupt handler */

    sysWindMLIntConnect(pI8042MseDevice->pDev, i8042MseInt, (int)pI8042MseDevice);

    /* initialize the interface */

    i8042MseHwInit (pI8042MseDevice);

    /* enable the pointer interrupt */

    sysWindMLIntEnable (pI8042MseDevice->pDev);

    /* add the device to the I/O system */

    if (iosDevAdd(&pI8042MseDevice->ty_dev.devHdr,name,i8042MseDrvNum) == ERROR)
        return (ERROR);

    return (i8042MseDrvNum);
    }


/*******************************************************************************
*
* i8042MseOpen - open file
*
* This routine opens the mouse port on the i8042 controller.
*
* RETURNS device structure
*
* SEE ALSO:
*/

LOCAL int i8042MseOpen
    (
    I8042_MSE_DEVICE  *dev,       /* device structure */ 
    char        *name,
    int         mode
    )
    {
    return ((int)dev);
    }


/*****************************************************************************
*
*  i8042MseTxStart - send data to mouse
*
* This routine transmits data to the mouse port on the i8042 controller
*
* RETURNS  N/A
*
* SEE ALSO:
*/

LOCAL void i8042MseTxStart
    (
    I8042_MSE_DEVICE  *dev
    )
    {
    char          out_char;

    while (tyITx(&dev->ty_dev,&out_char) == OK);
    }

/*****************************************************************************
*
* i8042MseClose - close file
*
*  This routine closes the mouse device
*
* RETURNS N/A
*
* SEE ALSO:
*/

LOCAL int i8042MseClose
    (
    I8042_MSE_DEVICE  *dev
    )
    {
    return((int)dev);
    }

/******************************************************************************
*
* i8042Wdog - I8042 driver watchdog handler.
*
* i8042 driver watchdog handler.
*
* RETURNS: N/A
*/

LOCAL void i8042Wdog (void)
    {
    i8042Timeout = TRUE;
    i8042TimeoutCnt++;
    }


/******************************************************************************
*
* i8042Read - read data from the i8042 keyboard/mouse controller.
*
* Read data from the i8042 keyboard/mouse controller.
*
* RETURNS: OK or ERROR if timed out
*/

LOCAL STATUS i8042Read
    (
    ULONG   statCmdReg,         /* location of the status/command register */
    ULONG   dataReg,            /* location of the data register */
    UCHAR * pData               /* location to return read data */
    )
    {

    /* Start watch dog to monitor device access */

    i8042Timeout = FALSE;
    wdStart (i8042Wdid, (sysClkRateGet() * I8042_WAIT_SEC), (FUNCPTR)i8042Wdog, 0);

    /* Wait for device to become ready */

    while (((I8042_KBD_IN (statCmdReg) & I8042_KBD_OBFULL) == 0) && !i8042Timeout)
        ;

    /* Cancel watch dog and delay for data to move up in device h/w queue */

    wdCancel (i8042Wdid);
    taskDelay (sysClkRateGet () >> 4);

    /* read the character from the device */

    *pData = I8042_KBD_IN (dataReg);

    return (i8042Timeout ? ERROR : OK);
    }


/******************************************************************************
*
* i8042Write - write data to the i8042 keyboard/mouse controller.
*
* Write data to the i8042 keyboard/mouse controller.
*
* RETURNS: OK or ERROR if timed out
*/

LOCAL STATUS i8042Write
    (
    ULONG   statCmdReg,       /* location of the status/command register */
    ULONG   dataReg,          /* location of the data register */
    UCHAR   data              /* data to write to data register */
    )
    {

    /* Start watch dog to monitor device access */

    i8042Timeout = FALSE;
    wdStart (i8042Wdid, (sysClkRateGet() * I8042_WAIT_SEC), (FUNCPTR)i8042Wdog, 0);

    /* Wait for device to become ready */

    while ((I8042_KBD_IN (statCmdReg) & I8042_KBD_IBFULL) && !i8042Timeout)
        ;

    /* Cancel watch dog */

    wdCancel (i8042Wdid);

    /* Send data */

    I8042_KBD_OUT (dataReg, data);

    return (i8042Timeout ? ERROR : OK);
    }


/******************************************************************************
*
* i8042Command - write command to the i8042 keyboard/mouse controller.
*
* Write command to the i8042 keyboard/mouse controller.
*
* RETURNS: OK or ERROR if timed out
*/

LOCAL STATUS i8042Command
    (
    ULONG   statCmdReg,     /* location of the status/command register */
    UCHAR   command         /* command to write to command register */
    )
    {
        
    /* Start watch dog to monitor device access */

    i8042Timeout = FALSE;
    wdStart (i8042Wdid, (sysClkRateGet() * I8042_WAIT_SEC), (FUNCPTR)i8042Wdog, 0);

    /* Wait for device to become ready */

    while ((I8042_KBD_IN (statCmdReg) & I8042_KBD_IBFULL) && !i8042Timeout)
        ;

    /* Issue command */

    I8042_KBD_OUT (statCmdReg, command);

    /* Wait for command to be completed */

    while ((I8042_KBD_IN (statCmdReg) & I8042_KBD_IBFULL) && !i8042Timeout)
        ;

    /* Cancel watch dog */

    wdCancel (i8042Wdid);

    return (i8042Timeout ? ERROR : OK);
    }


/*******************************************************************************
*
* i8042MseHwInit - initialize the 8042 for mouse support
*
* This routine initializes the 8042 mouse port.
*
* RETURNS N/A
*
* SEE ALSO:
*/

LOCAL void i8042MseHwInit 
    (
    I8042_MSE_DEVICE  * pI8042MseDevice
    )
    {
    unsigned char      cond;
    unsigned char      conf;


    /* Disable keyboard */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_DISABLE);

    /* Get current configuration */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_RD_CONFIG);
    i8042Read (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg, &conf);

    /* Disable interrupt for keyboard */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_WT_CONFIG);
    i8042Write (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg, conf & 0xFC);

    /* Enable auxiliary device */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_ENABLE_AUX);

    /* Check interface to device */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_IF_AUX_TEST);
    if (i8042Read (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg,
                    &cond) == ERROR)
        return;
    if (cond != I8042_KBD_IF_OK)
        return;

    /* Set Standard mode for mouse */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_WT_AUX);
    i8042Write (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg, 
                I8042_KBDM_SETS_CMD);
    if (i8042Read (pI8042MseDevice->statCmdReg, 
                   pI8042MseDevice->dataReg, &cond) == ERROR)
       return;
    if (cond == I8042_KBDM_ACK)
        {
        /* Enable mouse device */

        i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_WT_AUX);
        i8042Write (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg,  
                    I8042_KBDM_ENABLE_CMD);
        if (i8042Read (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg,  
                       &cond) == ERROR)
           return;
        if (cond == I8042_KBDM_ACK)
            conf |= I8042_KBD_AUX_INT;
        else
            return;
        }
    else
        return;

    /* reenable 8042 keyboard with  new configuration */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_WT_CONFIG);
    i8042Write (pI8042MseDevice->statCmdReg, pI8042MseDevice->dataReg,  conf);

    /* Enable keyboard */

    i8042Command (pI8042MseDevice->statCmdReg, I8042_KBD_ENABLE);

    return;
    }

/*****************************************************************************
*
* i8042MseInt - handle interrupts
*
* This routine is the mouse port interrupt handler
*
* RETURNS  NA
*
* SEE ALSO:
*/

LOCAL void i8042MseInt
    (
    I8042_MSE_DEVICE  *pI8042MseDevice       /* device control structure */
    )
    {
    UCHAR in_char;
    

    /* Check if character is ready */

    if (I8042_KBD_IN (pI8042MseDevice->statCmdReg) & 
        (I8042_KBD_OBFULL | I8042_KBD_AUXB))
        {

        /* Character present, so read it and place in read queue */

        in_char = I8042_KBD_IN (pI8042MseDevice->dataReg);

        tyIRd (&pI8042MseDevice->ty_dev,in_char);
        }
    }


/* If the PC Console is included then the keyboard controller is provided
 * by the PC console device.
 */

#ifndef INCLUDE_PC_CONSOLE       
/******************************************************************************
*
* i8042KbdDevCreate - create a device for the on-board ports
*
* This routine creates a device on one of the pcConsole ports.  Each port
* to be used should have only one device associated with it, by calling
* this routine.
*
* RETURNS: OK, or ERROR if there is no driver or one already exists for the
* specified port.
*/

STATUS i8042KbdDevCreate
    (
    char *  name       /* name to use for this device */
    )
    {
    int       i8042KbdDrvNum;               /* driver number  */
    I8042_KBD_DEVICE   *  pI8042KbdDevice;  /* device descriptors */

    /* Allocate and initialize driver control structure */

    pI8042KbdDevice = (I8042_KBD_DEVICE *)malloc (sizeof (I8042_KBD_DEVICE));

    /* Obtain the keyboard access info */

    pI8042KbdDevice->pDev = sysWindMLDevGet (WINDML_KEYBOARD_DEVICE, 0, 0, 0);

    /* Set up register access locations */

    pI8042KbdDevice->statCmdReg =  (ULONG)pI8042KbdDevice->pDev->pRegBase +
                       ((ULONG)pI8042KbdDevice->pDev->regDelta * I8042_STAT_CMD);
    pI8042KbdDevice->dataReg =  (ULONG)pI8042KbdDevice->pDev->pRegBase +
                       ((ULONG)pI8042KbdDevice->pDev->regDelta * I8042_DATA);

    /* Connect the keyboard interrupt */

    sysWindMLIntConnect (pI8042KbdDevice->pDev, i8042KbdInt, (int)pI8042KbdDevice);

    /* initialize the controller */

    i8042KbdHrdInit (pI8042KbdDevice);

    /* Install the driver */

    i8042KbdDrvNum = iosDrvInstall (i8042KbdOpen, (FUNCPTR) NULL, i8042KbdOpen,
                    (FUNCPTR) NULL, tyRead, tyWrite, i8042KbdIoctl);

    if (tyDevInit (&pI8042KbdDevice->tyDev, 20, 10, i8042KbdWriteData)
        != OK)
        return (ERROR);

    /* Enable the keyboard interrupt */

    sysWindMLIntEnable (pI8042KbdDevice->pDev);

    /* add the device to the I/O system */

    if (iosDevAdd (&pI8042KbdDevice->tyDev.devHdr, name, i8042KbdDrvNum) == ERROR)
        return (ERROR);

    return (i8042KbdDrvNum);
    }


/*******************************************************************************
*
* i8042KbdOpen - open keyboard device
*
*/

LOCAL int i8042KbdOpen
    (
    I8042_KBD_DEVICE *    pKbdDv,
    char *          name,
    int             mode
    )
    {
    return ((int) pKbdDv);
    }

/*******************************************************************************
*
* i8042KbdIoctl - special device control
*
* This routine handles LED change requests and passes all others to tyIoctl.
*
* RETURNS: OK or  whatever tyIoctl returns.
*/

LOCAL STATUS i8042KbdIoctl
    (
    I8042_KBD_DEVICE *    pKbdDv,    /* device to control */
    int             request,         /* request code */
    int             arg              /* some argument */
    )
    {
    int     status = OK;

    switch (request)
        {
        case CONIOLEDS:         /* change LEDs */
            i8042KbdLedSet (pKbdDv, arg);
            break;

        default:
            status = tyIoctl (&pKbdDv->tyDev, request, arg);
            break;
        }

    return (status);
    }

/*******************************************************************************
*
* i8042KbdWriteData - write data to device
*
* This routine the write function from the I/O subsystem.
*
* RETURNS:
*/

LOCAL int i8042KbdWriteData
    (
    I8042_KBD_DEVICE *    pKbdDv    /* device to control */
    )
    {

        return (0);
    }

#if (CPU_FAMILY != I80X86)
/*******************************************************************************
*
* i8042KbdHrdCheck - check the keyboard connection and verify operation
*
* This routine is called to verify the keyboard connection.  It is only
* on non X86 processor systems. The X86 processors execute the BIOS which
* performs the keyboard basic initialization.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL int i8042KbdHrdCheck
    (
    I8042_KBD_DEVICE   *  pI8042KbdDevice       /* Driver descriptor */
    )
    {
    UCHAR     temp;
    UCHAR     cond;

    if (i8042Command (pI8042KbdDevice->statCmdReg, I8042_KBD_WT_CONFIG) == ERROR) 
        return(ERROR);
    if (i8042Write (pI8042KbdDevice->statCmdReg, 
                    pI8042KbdDevice->dataReg, 0x44) == ERROR)   
        return(ERROR);

    /* 8042 Self Test and Initialize */

    cond = 0x00;
    for (temp=0; (cond != 0x55) && (temp < 5); temp++)
    {
        if (i8042Command (pI8042KbdDevice->statCmdReg, 0xAA) == ERROR) 
            return(ERROR);
        if (i8042Read (pI8042KbdDevice->statCmdReg, 
                       pI8042KbdDevice->dataReg, &cond) == ERROR)   
            return(ERROR);
        if (cond == 0xAA)
            break;
    }
    if (cond != 0x55)
        return(ERROR);

    /* Check interface to keyboard */

    if (i8042Command (pI8042KbdDevice->statCmdReg, I8042_KBD_IF_TEST) == ERROR) 
        return(ERROR);
    if (i8042Read (pI8042KbdDevice->statCmdReg, 
                   pI8042KbdDevice->dataReg, &cond) == ERROR) 
        return(ERROR);
    if (cond != 0x00)
        return(ERROR);

    if (i8042Command (pI8042KbdDevice->statCmdReg, I8042_KBD_WT_CONFIG) == ERROR) 
        return(ERROR);
    if (i8042Write (pI8042KbdDevice->statCmdReg, 
                    pI8042KbdDevice->dataReg, 0x45) == ERROR)   
        return(ERROR);

    /* Clean out the FIFO; typically has three return codes in it. */
    /* Must wait a moment for the data to work its way to the surface. */

    taskDelay (sysClkRateGet () >> 4);
    while (I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_OBFULL)
        {
        cond = I8042_KBD_IN (pI8042KbdDevice->dataReg);
        taskDelay (sysClkRateGet () >> 4);
        }
    return(OK);
}
#endif

/*******************************************************************************
*
* i8042KbdHrdInit - initialize the Keyboard
*
* This routine is called to do the key board initialization
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void i8042KbdHrdInit 
    (
    I8042_KBD_DEVICE   *  pI8042KbdDevice       /* Driver descriptor */
    )
    {
    UCHAR    temp;
    FAST int    oldlevel;   /* to hold the oldlevel of interrupt */

    oldlevel= intLock ();

    i8042Wdid = wdCreate ();

#if (CPU_FAMILY != I80X86)         /* BIOS initilizes for X86 */
    if (i8042KbdHrdCheck (pI8042KbdDevice) == ERROR)
        {
        intUnlock (oldlevel);    
        return;
        }
#endif

    /* Reset time outs */

    i8042TimeoutCnt = 0;
    i8042Timeout = FALSE;
        
    do
        {
        if (i8042TimeoutCnt > 3)  /* try 3 times then give up */
            break;

        if (i8042Timeout)         /* reset if we got timeout */
            i8042KbdReset (pI8042KbdDevice);

        if (i8042Command (pI8042KbdDevice->statCmdReg, I8042_KBD_WT_CONFIG) == ERROR)
            continue;
        if (i8042Write (pI8042KbdDevice->statCmdReg, 
                        pI8042KbdDevice->dataReg,0x44) == ERROR)
            continue;

        while (I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_OBFULL)
            if (i8042Read (pI8042KbdDevice->statCmdReg, 
                           pI8042KbdDevice->dataReg, &temp) == ERROR)
                break;

        if (i8042Command (pI8042KbdDevice->statCmdReg, I8042_KBD_WT_CONFIG) == ERROR)
            continue;
        if (i8042Write (pI8042KbdDevice->statCmdReg, 
                        pI8042KbdDevice->dataReg, 0x45) == ERROR)
            continue;

        if (i8042Command (pI8042KbdDevice->statCmdReg,  I8042_KBD_ENABLE) == ERROR)
            continue;
        } while (i8042Timeout);

    /* Set LEDS to off */
    pI8042KbdDevice->oldLedStat = 7;
    i8042KbdLedSet (pI8042KbdDevice, 0);

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* i8042KbdInt - interrupt level processing
*
* This routine handles the keyboard interrupts
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void i8042KbdInt 
    (
    I8042_KBD_DEVICE   *  pI8042KbdDevice       /* Driver descriptor */
    )
    {
    FAST UCHAR  scanCode;   /* to hold the scanCode */
    UCHAR stat = I8042_KBD_IN (pI8042KbdDevice->statCmdReg);


    if (stat & I8042_KBD_OBFULL)
        {
        scanCode = I8042_KBD_IN (pI8042KbdDevice->dataReg);

        /* keyboard acknowledge to any valid input, so just return */

        if (scanCode == I8042_KBD_CMD_ACK)
            return;
            
        /* queue scan code */

        tyIRd (&(pI8042KbdDevice->tyDev), scanCode);
        }
    }


/******************************************************************************
*
* i8042KbdLedSet - Keybord LED Set
*
* This routine sets the LEDs on the keyboard as they were set within the 
*
* RETURNS: N/A
*/

LOCAL void i8042KbdLedSet 
    (
    I8042_KBD_DEVICE   *  pI8042KbdDevice,       /* Driver descriptor */
    UCHAR                 led                    /* Value to set LEDs */
    )
    {
    int     ix;    

    led &=  0x07;         /* bits 0 1 2 for scroll numlock & capslock */

    /* Only set LEDs if they change from previous */
    if (pI8042KbdDevice->oldLedStat == led)
        return;
    pI8042KbdDevice->oldLedStat = led;

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->dataReg, 0xed);       /* SET LEDS command */


    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->dataReg, led);       /* set LEDs */
    }

/******************************************************************************
*
* i8042KbdReset - reset a keyboard
*
* This routine resets the keyboard.
*
* RETURNS: N/A
*/

LOCAL void i8042KbdReset 
    (
    I8042_KBD_DEVICE   *  pI8042KbdDevice       /* Driver descriptor */
    )
    {
    int     ix;

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->dataReg, 0xff);

    taskDelay (sysClkRateGet () >> 4);

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->dataReg, I8042_KBD_WT_CONFIG);

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->dataReg, 0x45);

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;

    I8042_KBD_OUT (pI8042KbdDevice->statCmdReg, I8042_KBD_ENABLE);

    for (ix = 0; ix < I8042_READ_DELAY; ix++)       /* wait for input buf empty */
        if ((I8042_KBD_IN (pI8042KbdDevice->statCmdReg) & I8042_KBD_IBFULL) == 0)
            break;
    }

#endif /* INCLUDE_PC_CONSOLE */
