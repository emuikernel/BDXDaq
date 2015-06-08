/* sioLib.h - header file for binary interface serial drivers */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,09may02,wef adding keyboard led set and get ioctls.
01h,29sep01,dat added SIO_ERRROR_NONE, new MODEM ctrl values.
01g,26apr00,dat added error callback
01f,04feb00,db  added support for handling modem control signals and 
		multiple keyboard modes(SPR #29348).
01e,14jan97,db  added SIO_OPEN and SIO_HUP for modem control(SPR #7637).
01d,24oct95,ms  removed "static __inline__" (SPR #4500)
01c,15jun95,ms  Added SIO_MODE_SET, SIO_MODE_GET, SIO_AVAIL_MODES_GET iotcl's
		Renamed SIO_CHAN to SIO_CHAN
		Changed prototype and name of callbackInstall()
		Added documentation.
01b,22may95,ms  removed unneeded include file.
01a,21feb95,ms  written.
*/

#ifndef __INCsioLibh
#define __INCsioLibh

#include "types/vxTypes.h"
#include "types/vxTypesOld.h"

#ifdef __cplusplus
extern "C" {
#endif

/* serial device I/O controls */

#define SIO_BAUD_SET		0x1003
#define SIO_BAUD_GET		0x1004

#define SIO_HW_OPTS_SET		0x1005
#define SIO_HW_OPTS_GET		0x1006

#define SIO_MODE_SET		0x1007
#define SIO_MODE_GET		0x1008
#define SIO_AVAIL_MODES_GET	0x1009

/* These are used to communicate open/close between layers */

#define SIO_OPEN		0x100A 
#define SIO_HUP			0x100B

/*
 * The ioctl commands listed below provide a way for reading and
 * setting the modem lines. 
 * 
 * SIO_MSTAT_GET: returns status of all input and output modem signals.
 * SIO_MCTRL_BITS_SET: sets modem signals specified in argument.
 * SIO_MCTRL_BITS_CLR: clears modem signal(s) specified in argument.
 * SIO_MCTRL_ISIG_MASK: returns mask of all input modem signals.
 * SIO_MCTRL_OSIG_MASK: returns mask of all output(writable) modem signals.
 */

#define SIO_MSTAT_GET           0x100C  /* get modem status lines */
#define SIO_MCTRL_BITS_SET      0x100D  /* set selected modem lines */
#define SIO_MCTRL_BITS_CLR      0x100E  /* clear selected modem lines  */
#define SIO_MCTRL_ISIG_MASK     0x100F  /* mask of lines that can be read */
#define SIO_MCTRL_OSIG_MASK     0x1010  /* mask of lines that can be set */

/* Ioctl cmds for reading/setting keyboard mode */

#define SIO_KYBD_MODE_SET       0x1011
#define SIO_KYBD_MODE_GET       0x1012


/* Ioctl cmds for reading/setting keyboard led state */

#define SIO_KYBD_LED_SET       0x1013
#define SIO_KYBD_LED_GET       0x1014

/* callback type codes */

#define SIO_CALLBACK_GET_TX_CHAR	1
#define SIO_CALLBACK_PUT_RCV_CHAR	2
#define SIO_CALLBACK_ERROR		3

/* Get Tx Chara callback (getTxCallbk) (callbkArg, pChar) */

/* Put Rx Chara callback (putRxCallbk) (callbkArg, char) */

/* Error callback codes: (errorCallbk) (callbkArg, code, pData, size) */

#define SIO_ERROR_NONE			(-1)
#define SIO_ERROR_FRAMING		0
#define SIO_ERROR_PARITY		1
#define SIO_ERROR_OFLOW			2
#define SIO_ERROR_UFLOW			3
#define SIO_ERROR_CONNECT		4
#define SIO_ERROR_DISCONNECT		5
#define SIO_ERROR_NO_CLK		6
#define SIO_ERROR_UNKNWN		7

/* options to SIO_MODE_SET */

#define	SIO_MODE_POLL	1
#define	SIO_MODE_INT	2

/* options to SIOBAUDSET */
/* use the number, not a macro */

/* options to SIO_HW_OPTS_SET (ala POSIX), bitwise or'ed together */

#define CLOCAL		0x1	/* ignore modem status lines */
#define CREAD		0x2	/* enable device reciever */

#define CSIZE		0xc	/* bits 3 and 4 encode the character size */
#define CS5		0x0	/* 5 bits */
#define CS6		0x4	/* 6 bits */
#define CS7		0x8	/* 7 bits */
#define CS8		0xc	/* 8 bits */

#define HUPCL		0x10	/* hang up on last close */
#define STOPB		0x20	/* send two stop bits (else one) */
#define PARENB		0x40	/* parity detection enabled (else disabled) */
#define PARODD		0x80	/* odd parity  (else even) */

/* 
 * Modem signals definitions 
 *
 * The following six macros define the different modem signals. They
 * are used as arguments to the modem ioctls commands to specify 
 * the signals to read(set) and also to parse the returned value. They
 * provide hardware independence, as modem signals bits vary from one 
 * chip to another.      
 */

#define SIO_MODEM_DTR   0x01    /* data terminal ready */
#define SIO_MODEM_RTS   0x02    /* request to send */
#define SIO_MODEM_CTS   0x04    /* clear to send */
#define SIO_MODEM_CD    0x08    /* carrier detect */
#define SIO_MODEM_RI    0x10    /* ring */
#define SIO_MODEM_DSR   0x20    /* data set ready */

/*
 * options to SIO_KYBD_MODE_SET 
 *
 * These macros are used as arguments by the keyboard ioctl comands.
 * When used with SIO_KYBD_MODE_SET they mean the following:
 * 
 * SIO_KYBD_MODE_RAW:
 *    Requests the keyboard driver to return raw key values as read
 *    by the keyboard controller.
 * 
 * SIO_KYBD_MODE_ASCII:
 *    Requests the keyboard driver to return ASCII codes read from a
 *    known table that maps raw key values to ASCII.
 *
 * SIO_KYBD_MODE_UNICODE:
 *    Requests the keyboard driver to return 16 bit UNICODE values for
 *    multiple languages support.
 */

#define SIO_KYBD_MODE_RAW     1
#define SIO_KYBD_MODE_ASCII   2
#define SIO_KYBD_MODE_UNICODE 3

/*
 * options to SIO_KYBD_LED_SET 
 *
 * These macros are used as arguments by the keyboard ioctl comands.
 * When used with SIO_KYBD_LED_SET they mean the following:
 * 
 * SIO_KYBD_LED_NUM:
 *    Sets the Num Lock LED on the keyboard - bit 1
 * 
 * SIO_KYBD_LED_CAP:
 *    Sets the Caps Lock LED on the keyboard - bit 2
 *
 * SIO_KYBD_LED_SRC:
 *    Sets the Scroll Lock LED on the keyboard - bit 4
 */

#define SIO_KYBD_LED_NUM	1
#define SIO_KYBD_LED_CAP	2
#define SIO_KYBD_LED_SCR	4

/* serial device data structures */

typedef struct sio_drv_funcs SIO_DRV_FUNCS;

typedef struct sio_chan				/* a serial channel */
    {
    SIO_DRV_FUNCS * pDrvFuncs;
    /* device data */
    } SIO_CHAN;

struct sio_drv_funcs				/* driver functions */
    {
    int		(*ioctl)
			(
			SIO_CHAN *	pSioChan,
			int		cmd,
			void *		arg
			);

    int		(*txStartup)
			(
			SIO_CHAN *	pSioChan
			);

    int		(*callbackInstall)
			(
			SIO_CHAN *	pSioChan,
			int		callbackType,
			STATUS		(*callback)(void *, ...),
			void *		callbackArg
			);

    int		(*pollInput)
			(
			SIO_CHAN *	pSioChan,
			char *		inChar
			);

    int		(*pollOutput)
			(
			SIO_CHAN *	pSioChan,
			char 		outChar
			);
    };


/* in-line macros */

#define sioIoctl(pSioChan, cmd, arg) \
        ((pSioChan)->pDrvFuncs->ioctl (pSioChan, cmd, arg))
#define sioTxStartup(pSioChan) \
        ((pSioChan)->pDrvFuncs->txStartup (pSioChan))
#define sioCallbackInstall(pSioChan, callbackType, callback, callbackArg) \
	((pSioChan)->pDrvFuncs->callbackInstall (pSioChan, callbackType, \
			callback, callbackArg))
#define sioPollInput(pSioChan, inChar) \
        ((pSioChan)->pDrvFuncs->pollInput (pSioChan, inChar))
#define sioPollOutput(pSioChan, thisChar) \
        ((pSioChan)->pDrvFuncs->pollOutput (pSioChan, thisChar))

#ifdef __cplusplus
}
#endif

#endif  /* __INCsioLibh */
