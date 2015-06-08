/* pcic.h - Intel 82365SL PCMCIA Host Bus Adaptor chip header */

/* Copyright 1984-1996 Wind River Systems, Inc. */
/* Copyright (c) 1994 David A. Hinds -- All Rights Reserved */

/*
modification history
--------------------
01d,01apr98,hdn  added PCIC_ENA_MEMCS16	macro.
01c,19jan98,hdn  changed PCIC_VPP_XX macros.
01b,22feb96,hdn  cleaned up.
01a,19jan95,hdn  written based on David Hinds's version 2.2.3.
*/

#ifndef __INCpcich
#define __INCpcich

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


#define PCIC_MAX_SOCKS		8	/* number of sockets */
#define PCIC_MEM_WINDOWS	5	/* number of memory windows */
#define PCIC_IO_WINDOWS		2	/* number of io windows */

/* PCIC compatible chips */

#define PCIC_I82365		0x10	/* Intel 82365 */
#define PCIC_PD672X		0x20	/* Cirrus clone */
#define PCIC_PD6710		0x30	/* Cirrus clone */
#define PCIC_VLSI		0x40	/* VLSI clone */
#define PCIC_IBM		0x50	/* IBM clone */
#define PCIC_VG468		0x60	/* Vadem clone */
#define PCIC_RICOH		0x70	/* Ricoh clone */

/* Offsets for PCIC registers */

#define PCIC_IDENT		0x00	/* Identification and revision */
#define PCIC_STATUS		0x01	/* Interface status */
#define PCIC_POWER		0x02	/* Power and RESETDRV control */
#define PCIC_INTCTL		0x03	/* Interrupt and general control */
#define PCIC_CSC		0x04	/* Card status change */
#define PCIC_CSCINT		0x05	/* Card status change int control */
#define PCIC_ADDRWIN		0x06	/* Address window enable */
#define PCIC_IOCTL		0x07	/* I/O control */
#define PCIC_GENCTL		0x16	/* Card detect and general control */
#define PCIC_GBLCTL		0x1E	/* Global control register */

/* Offsets for I/O and memory window registers */

#define PCIC_IO(win)		(0x08+((win)<<2))
#define PCIC_MEM(win)		(0x10+((win)<<3))
#define PCIC_W_START		0
#define PCIC_W_STOP		2
#define PCIC_W_OFF		4

/* Flags for PCIC_STATUS */

#define PCIC_CS_BVD1		0x01
#define PCIC_CS_STSCHG		0x01
#define PCIC_CS_BVD2		0x02
#define PCIC_CS_SPKR		0x02
#define PCIC_CS_DETECT		0x0c
#define PCIC_CS_WRPROT		0x10
#define PCIC_CS_READY		0x20
#define PCIC_CS_POWERON		0x40
#define PCIC_CS_GPI		0x80

/* Flags for PCIC_POWER */

#define PCIC_PWR_OFF		0x00	/* Turn off the socket */
#define PCIC_VPP_5V		0x05	/* Vpp = 5.0v */
#define PCIC_VPP_12V		0x0a	/* Vpp = 12.0v */
#define PCIC_VPP_MASK		0x0f	/* Mask for turning off Vpp */
#define PCIC_VCC_5V		0x10	/* Vcc = 5.0v */
#define PCIC_VCC_3V		0x18	/* Vcc = 3.3v */
#define PCIC_VCC_MASK		0x18	/* Mask for turning off Vcc */
#define PCIC_PWR_AUTO		0x20	/* Auto pwr switch enable */
#define PCIC_PWR_NORESET	0x40	/* Disable RESETDRV on resume */
#define PCIC_PWR_OUT		0x80	/* Output enable */

/* Flags for PCIC_INTCTL */

#define PCIC_IRQ_MASK		0x0F
#define PCIC_INTR_ENA		0x10
#define PCIC_PC_IOCARD		0x20
#define PCIC_PC_RESET		0x40
#define PCIC_RING_ENA		0x80

/* Flags for PCIC_CSC and PCIC_CSCINT*/

#define PCIC_CSC_BVD1		0x01
#define PCIC_CSC_STSCHG		0x01
#define PCIC_CSC_BVD2		0x02
#define PCIC_CSC_READY		0x04
#define PCIC_CSC_DETECT		0x08
#define PCIC_CSC_ANY		0x0F
#define PCIC_CSC_GPI		0x10

/* Flags for PCIC_ADDRWIN */

#define PCIC_ENA_IO(win)	(0x40 << (win))
#define PCIC_ENA_MEM(win)	(0x01 << (win))
#define PCIC_ENA_ALL		0xdf
#define PCIC_ENA_MEMCS16	0x20

/* Flags for PCIC_IOCTL */

#define PCIC_IOCTL_MASK(win)	(0x0F << (win<<2))
#define PCIC_IOCTL_WAIT(win)	(0x08 << (win<<2))
#define PCIC_IOCTL_0WS(win)	(0x04 << (win<<2))
#define PCIC_IOCTL_CS16(win)	(0x02 << (win<<2))
#define PCIC_IOCTL_16BIT(win)	(0x01 << (win<<2))

/* Flags for PCIC_GENCTL */

#define PCIC_CTL_16DELAY	0x01
#define PCIC_CTL_RESET_ENA	0x02
#define PCIC_CTL_GPI_ENA	0x04
#define PCIC_CTL_GPI_CTL	0x08
#define PCIC_CTL_RESUME		0x10
#define PCIC_CTL_SW_IRQ		0x20

/* Flags for PCIC_GBLCTL */

#define PCIC_GBL_PWRDOWN	0x01
#define PCIC_GBL_CSC_LEV	0x02
#define PCIC_GBL_WRBACK		0x04
#define PCIC_GBL_IRQ_0_LEV	0x08
#define PCIC_GBL_IRQ_1_LEV	0x10

/* Flags for memory window registers */

#define PCIC_MEM_16BIT		0x8000	/* In memory start high byte */
#define PCIC_MEM_0WS		0x4000
#define PCIC_MEM_WS1		0x8000	/* In memory stop high byte */
#define PCIC_MEM_WS0		0x4000
#define PCIC_MEM_WRPROT		0x8000	/* In offset high byte */
#define PCIC_MEM_REG		0x4000


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern int	pcicInit	(int ioBase, int intVec, int intLevel, 
				 FUNCPTR showRtn);
extern void	pcicShow	(int sock);

#else

extern int	pcicInit	();
extern void	pcicShow	();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCpcich */
