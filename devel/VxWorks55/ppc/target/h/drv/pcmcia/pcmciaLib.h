/* pcmciaLib.h - PCMCIA library header */

/* Copyright 1984-1996 Wind River Systems, Inc. */
/* Copyright (c) 1994 David A. Hinds -- All Rights Reserved */

/*
modification history
--------------------
01c,16jan97,hdn  added two new members to PCMCIA_CTRL.
01b,22feb96,hdn  cleaned up.
01a,19jan95,hdn  written based on David Hinds's version 2.2.3.
*/

#ifndef __INCpcmciah
#define __INCpcmciah

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"


/* pcmciaLib status codes */

#define S_pcmciaLib_BATTERY_DEAD	(M_pcmciaLib | 1)
#define S_pcmciaLib_BATTERY_WARNING	(M_pcmciaLib | 2)


#ifndef _ASMLANGUAGE

#include "dllLib.h"
#include "blkIo.h"
#include "sysLib.h"
#include "dosFsLib.h"
#include "drv/pcmcia/pccardLib.h"

#define DOS_ID_OFFSET                   3
#define FIRST_PARTITION_SECTOR_OFFSET   (0x1be + 8)
#define VXDOS                           "VXDOS"
#define VXEXT                           "VXEXT"

#define PCMCIA_MAX_ARGS		6	/* max args to task level call */
#define PCMCIA_MAX_MSGS		10	/* max number of PCMCIA event msgs */
#define MAX_SOCKETS		8	/* XXX */
#define PCMCIA_CIS_WINDOW	0	/* memory window for CIS */


typedef struct pcmciaCard		/* for PC card */
    {
    int		type;			/* card type */
    int		sock;			/* socket no. */
    int		ctrl;			/* controller no. */
    BOOL	detected;		/* TRUE if a card is detected */
    BOOL	installed;		/* TRUE if a driver is installed */
    BOOL	changed;		/* TRUE if a card is changed */
    int		regBase;		/* config register base */
    int		regMask;		/* config register mask */
    int		cardStatus;		/* card status */
    int		initStatus;		/* init status */
    FUNCPTR	cscIntr;		/* CSC interrupt handler */
    FUNCPTR	showRtn;		/* show routine */
    DL_LIST	cisTupleList;		/* CIS tuple link list */
    DL_LIST	cisConfigList;		/* CIS config tuple link list */
    PCCARD_RESOURCE *pResource;		/* resources for the card */
    BLK_DEV	*pBlkDev;		/* booting for block device */
    NETIF	*pNetIf;		/* booting for ethernet device */
    DOS_VOL_DESC *pDos;			/* DOS file system volume descriptor */
    } PCMCIA_CARD;

typedef struct pcmciaChip		/* for PCMCIA host adapter chip */
    {
    int		type;			/* type of PCMCIA adapter chip */
    char	*name;			/* name of PCMCIA adapter chip */
    BOOL	installed;		/* TRUE if it is installed */
    int		socks;			/* number of sockets supported */
    int		flags;			/* */
    int		intLevel;		/* CSC interrupt level */
    int		memWindows;		/* number of memory windows */
    int		ioWindows;		/* number of io windows */
    FUNCPTR	reset;			/* reset routine */
    FUNCPTR	status;			/* status routine */
    FUNCPTR	flagGet;		/* flag get routine */
    FUNCPTR	flagSet;		/* flag set routine */
    FUNCPTR	cscOn;			/* CSC on routine */
    FUNCPTR	cscOff;			/* CSC off routine */
    FUNCPTR	cscPoll;		/* CSC poll routine */
    FUNCPTR	irqGet;			/* IRQ get routine */
    FUNCPTR	irqSet;			/* IRQ set routine */
    FUNCPTR	iowinGet;		/* io window get routine */
    FUNCPTR	iowinSet;		/* io window set routine */
    FUNCPTR	memwinGet;		/* memory window get routine */
    FUNCPTR	memwinSet;		/* memory window set routine */
    FUNCPTR	showRtn;		/* show routine */
    } PCMCIA_CHIP;

typedef struct pcmciaCtrl
    {
    int		socks;			/* number of sockets */
    int		memBase;		/* mapping memory base addr */
    PCMCIA_CHIP chip;			/* chip structure */
    PCMCIA_CARD card[MAX_SOCKETS];	/* card structure */
    } PCMCIA_CTRL;

typedef struct pcmciaIowin
    {
    int		window;
    int		flags;
    int		extraws;
    int		start;
    int		stop;
    } PCMCIA_IOWIN;

typedef struct pcmciaMemwin
    {
    int		window;
    int		flags;
    int		extraws;
    int		start;
    int		stop;
    int		cardstart;
    } PCMCIA_MEMWIN;

typedef struct pcmciaAdapter
    {
    int		type;			/* type of a PCMCIA adapter chip */
    int		ioBase;			/* IO base address */
    int		intVec;			/* CSC interrupt vector */
    int		intLevel;		/* CSC interrupt level */
    FUNCPTR	initRtn;		/* initialize routine */
    FUNCPTR	showRtn;		/* show routine */
    } PCMCIA_ADAPTER;

typedef struct pcmciaMsg
    {
    VOIDFUNCPTR	func;			/* pointer to function to invoke */
    int		arg [PCMCIA_MAX_ARGS];	/* args for function */
    } PCMCIA_MSG;

/* supported PCMCIA host adapter chip type */

#define PCMCIA_PCIC		0x01
#define PCMCIA_TCIC		0x02

/* supported PC card type */

#define PCCARD_SRAM		0x01
#define PCCARD_ATA		0x02
#define PCCARD_LAN_ELT		0x03

/* configuration register number */

#define CONFIG_OPTION_REG	0
#define CONFIG_STATUS_REG	1
#define PIN_REPLACEMENT_REG	2
#define SOCKET_COPY_REG		3

/* bits in configuration option register */

#define COPTION_SRESET		0x80
#define COPTION_LEVIRQ		0x40

/* bits in configuration status register */

#define CSTATUS_CHANGED		0x80
#define CSTATUS_SIGCHG		0x40
#define CSTATUS_IOIS8		0x20
#define CSTATUS_AUDIO		0x08
#define CSTATUS_PWRDWN		0x04
#define CSTATUS_INTR		0x02

/* bits in pin replacement register */

#define PINREP_CBVD1		0x80
#define PINREP_CBVD2		0x40
#define PINREP_CRDYBSY		0x20
#define PINREP_CWPROT		0x10
#define PINREP_RBVD1		0x08
#define PINREP_RBVD2		0x04
#define PINREP_RRDYBSY		0x02
#define PINREP_RWPROT		0x01


/* definitions for card status flags */

#define PC_WRPROT		0x001
#define PC_CARDLOCK		0x002
#define PC_NO_CARD		0x004
#define PC_IS_CARD		0x008
#define PC_BATDEAD		0x010
#define PC_BATWARN		0x020
#define PC_READY		0x040
#define PC_DETECT		0x080
#define PC_POWERON		0x100
#define PC_GPI			0x200
#define PC_STSCHG		0x400

/* various card configuration flags */

#define PC_PWR_OFF		0x00
#define PC_VCC_MASK		0x03
#define PC_VCC_3V		0x01
#define PC_VCC_5V		0x02
#define PC_VPP_MASK		0x0C
#define PC_VPP_5V		0x04
#define PC_VPP_12V		0x08
#define PC_PWR_AUTO		0x10
#define PC_IOCARD		0x20
#define PC_RESET		0x40

/* flags for I/O port and memory windows */

#define MAP_ACTIVE		0x01
#define MAP_16BIT		0x02
#define MAP_0WS			0x04
#define MAP_CS16		0x08
#define MAP_WRPROT		0x10
#define MAP_ATTRIB		0x20


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	pcmciaInit	(void);
extern STATUS 	pcmciaJobAdd	(VOIDFUNCPTR func, int arg1, int arg2, 
				 int arg3, int arg4, int arg5, int arg6);
extern void 	pcmciad		(void);
extern void 	pcmciaShowInit	(void);
extern void 	pcmciaShow	(int sock);

#else

extern STATUS 	pcmciaInit	();
extern STATUS 	pcmciaJobAdd	();
extern void 	pcmciad		();
extern void 	pcmciaShowInit	();
extern void 	pcmciaShow	();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCpcmciah */
