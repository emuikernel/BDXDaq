/* pciLocalBus.h - pci bus configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,02nov01,tor  add class info
01i,27oct01,dat  Adding warnings about obsolete drivers
01h,05mar97,mas  made file C++ compliant (SPR 8117).
01g,21feb97,mas  PCI_DEV_IDSEL_MAX conditionally defined here so it can be
		 specified in config.h; NUM_PCI_DEVS_MAX set to 6 always
		 (SPR 8026).
01f,08nov96,mas  fixed NUM_PCI_DEVS_MAX and PCI_NUM_BAR for MV1300 (SPR 7446).
01e,14aug96,dat  changed PCI_READ macro to include address
01d,02aug96,dat  rewritten for portability, sprs 6490 and 6594
01c,24jul96,dds  SPR 6819: The PCI_CNFG_HDR structure has been 
                 modified for BIG_ENDIAN architectures.
01b,02feb95,vin  cleaned up.
01a,11nov94,vin  created.
*/

/*
This file contains type declarations for the PCI Header and the 
macros in regards to the PCI BUS. This header has been designed
with respect to PCI LOCAL BUS SPECIFICATION REVISION 2.1. Every
device on the PCI BUS has to support 256 byte of configuration
space of which the first 64 bytes is a predefined header portion
defined by the PCI commitee. 64 to 255 bytes is dedicated to the
device specific registers. PCI bus is inherently little endian. 
If a big endian processor is being interfaced with a PCI bus then
all the configuration registers have to be swapped and written.
*/


#ifndef	INCpciLocalBush
#define	INCpciLocalBush

/*
WARNING: The pciLocalBus driver is no longer supported. Please switch
all BSPs to use the pciConfigLib driver instead.
*/

#warning "h/drv/pci/pciLocalBus.h is being obsoleted, please use pciConfigLib"

#ifdef __cplusplus
extern "C" {
#endif

#if (_BYTE_ORDER == _BIG_ENDIAN)

#define PSWAP(x)	LONGSWAP(x)	/* swap the long word */

#else

#define PSWAP(x)	(x)		/* pass it as is */

#endif /* _BYTE_ORDER == _BIG_ENDIAN */

/* given a pci base address and longword index, return local address */

#ifndef PCI_ADDR
# define PCI_ADDR(base,index) (&((ULONG *)(base))[index])
#endif

/* Perform a longword pci read cycle */

#ifndef PCI_READ
#   define PCI_READ(base,index,addr) \
	    (*(ULONG *)addr = PSWAP(*PCI_ADDR(base,index)))
#endif

/* perform a longword pci write cycle */

#ifndef PCI_WRITE
#   define PCI_WRITE(base,index,data) (*PCI_ADDR(base,index) = PSWAP(data))
#endif

/*
 * Configuration space header offsets, all 32 bit accesses
 * Always use PSWAP() on the data value.  Use macros to
 * isolate sub fields within the 32 bit values.
 */

#define PCI_IDX_DEV_VNDR	0x0	/* dev id, vendor id */
#define PCI_IDX_ST_CMD		0x1	/* status, command */
#define PCI_IDX_CLS_REV		0x2	/* class, revision */
#define PCI_IDX_B_H_L_C		0x3	/* bist,hdr,lat,line_sz */
#define PCI_IDX_BAR_0		0x4	/* base addr reg 0 */
#define PCI_IDX_BAR_1		0x5	/* base addr reg 1 */
#define PCI_IDX_BAR_2		0x6	/* base addr reg 2 */
#define PCI_IDX_BAR_3		0x7	/* base addr reg 3 */
#define PCI_IDX_BAR_4		0x8	/* base addr reg 4 */
#define PCI_IDX_BAR_5		0x9	/* base addr reg 5 */
#define PCI_IDX_CIS_PTR		0xA	/* cardbus CIS ptr */
#define PCI_IDX_SUB_SVID	0xB	/* sub id, sub vendor id */
#define PCI_IDX_EXP_ROM		0xC	/* exp ROM Base Addr */
#define PCI_IDX_RES_1		0xD	/* reserved #1 */
#define PCI_IDX_RES_2		0xE	/* reserved #2 */
#define PCI_IDX_L_G_I_I		0xF	/* lat, grant, pin, lvl */

/* Macros to isolate Byte, word fields */

#define	PCI_DEVICE_ID(x)	(MSW(x))
#define	PCI_VENDOR_ID(x)	(LSW(x))

#define	PCI_STATUS(x)		(MSW(x))
#define	PCI_COMMAND(x)		(LSW(x))

#define	PCI_CLASS(x)		((x)>>8)
#define	PCI_REVISION(x)		(LLSB(x))

#define	PCI_BIST(x)		(LMSB(x))
#define	PCI_HDR(x)		(LNMSB(x))
#define	PCI_LAT_TMR(x)		(LNLSB(x))
#define	PCI_CACHE_LN_SZ(x)	(LLSB(x))
#define	PCI_MULTI_FUNC_HDR(x)	((x) & 0x00800000) /* if multi-func device */

#define	PCI_SUB_ID(x)		(MSW(x))
#define	PCI_SUB_VDR_ID(x)	(LSW(x))

#define	PCI_MAX_LAT(x)		(LMSB(x))
#define	PCI_MIN_GNT(x)		(LNMSB(x))
#define	PCI_INT_PIN(x)		(LNLSB(x))
#define	PCI_INT_LINE(x)		(LLSB(x))

/* misc defines */

#define PCI_CNFG_HDR_SIZE	0x40

#define PCI_CNFG_HDR_LONGS	0x40		/* nbr of longs in header */

/* define for ID_SEL of first PCI device on the PCI bus */

#define PCI_DEV_IDSEL		(1 << 11)
#ifndef PCI_DEV_IDSEL_MAX
#define PCI_DEV_IDSEL_MAX	(31 - 10)	/* max # of idsel lines */
#endif  /* PCI_DEV_IDSEL_MAX */

# define NUM_PCI_DEVS_MAX	(6)		/* #of dev idsel lines (slots)*/

#define	PCI_NUM_BAR		NUM_PCI_DEVS_MAX /* nbr of base addr reg's */

/* defines for configuration header */

#define INVALID_VENDOR_ID	0xffff		/* invalid vendor ID */

/* device class definitions */

/* device built before class code definitions were finalized */
#define CLASS_BEFORE_STD	0x00

#define CLASS_MASS_STORAGE	0x01		/* Mass storage controller */
#define CLASS_NET_CNTLR		0x02		/* Network controller */
#define CLASS_DISP_CNTLR	0x03		/* Display controller */
#define CLASS_MULTI_MEDIA	0x04		/* multimedia device */
#define CLASS_MEM_CNTLR		0x05		/* memory controller */
#define CLASS_BRIDGE_DEV	0x06		/* bridge device */
#define CLASS_COMM		0x07		/* simple communication controller */
#define CLASS_PERIPH		0x08		/* base system peripherals */
#define CLASS_INPUT		0x09		/* input devices */
#define CLASS_DOCK		0x0a		/* docking stations */
#define CLASS_PROCESSOR		0x0b		/* processors */
#define CLASS_SERIAL_BUS	0x0c		/* serial bus controllers */
#define CLASS_WIRELESS		0x0d		/* wireless controllers */
#define CLASS_INTELLIGENT_IO	0x0e		/* intelligent I/O controllers */
#define CLASS_SATELLITE		0x0f		/* satellite communications controllers */
#define CLASS_ENCRYPT		0x10		/* encryption/decryption controllers */
#define CLASS_DATA_ACQ		0x11		/* data acquisition controllers */
#define CLASS_UNDEFINED		0xff		/* not in any defined class */

/* device sub classes */
#define SUBCLASS_SCSI_CNTLR	0x00		/* SCSI bus controller */
#define	SUBCLASS_IDE_CNTLR	0x01		/* IDE controller */
#define	SUBCLASS_FLOPPY_CNTLR	0x02		/* Floppy disk Controller */
#define	SUBCLASS_IPI_CNTLR	0x03		/* IPI bus controller */

#define	SUBCLASS_ETHR_CNTLR	0x00		/* ethernet controller */
#define	SUBCLASS_TRNG_CNTLR	0x01		/* token ring controller */
#define	SUBCLASS_FDDI_CNTLR	0x02		/* FDDI controller */

#define	SUBCLASS_VGA_CNTLR	0x00		/* VGA compatible controller */
#define	SUBCLASS_XGA_CNTLR	0x01		/* XGA compatible controller */

#define	SUBCLASS_VIDEO		0x00		/* video */
#define	SUBCLASS_AUDIO		0x01		/* audio */

#define	SUBCLASS_RAM		0x00		/* ram device */
#define	SUBCLASS_FLASH		0x01		/* flash device */

#define	SUBCLASS_HST_BRDG	0x00		/* host bridge */
#define	SUBCLASS_ISA_BRDG	0x01		/* ISA bridge */
#define	SUBCLASS_EISA_BRDG	0x02		/* EISA bridge */
#define	SUBCLASS_MC_BRDG	0x03		/* MC bridge */
#define	SUBCLASS_PCI_PCI_BRDG	0x04		/* PCI to PCI bridge */
#define	SUBCLASS_PCMCIA_BRDG	0x05		/* PCMCIA bridge */
#define SUBCLASS_PCI_NUBUS_BRDG	0x06		/* NUBus bridge */
#define SUBCLASS_CARDBUS_BRDG	0x07		/* CardBus bridge */

#define	SUBCLASS_OTHER		0x80		/* other devices */

/* command and status register layout, bits 10 - 22 are reserved */

#define CSR_IO_EN		0x00000001 /* I/O address space enable */
#define CSR_MEM_EN		0x00000002 /* memory space enable */
#define CSR_BM_EN		0x00000004 /* bus master enable */
#define CSR_SP_CYCLS_EN		0x00000008 /* special cycles enable */
#define CSR_MEM_WRT_INV_EN	0x00000010 /* memry write & invalidate */
#define CSR_VGA_PLT_SN_EN	0x00000020 /* VGA Pallette snoop */
#define CSR_PARITY_EN		0x00000040 /* parity error response */
#define CSR_WAIT_EN		0x00000080 /* wait cycle control */
#define CSR_SERR_EN		0x00000100 /* SERR# enable */
#define CSR_FST_BTOB_EN		0x00000200 /* Fast back to back Enable */

/* status register layout */

/* writing a 1 to any bit will reset that bit, writing 0 has no effect */

#define CSR_66MHZ_CAP		0x00200000 /* 66 Mhz Capable */
#define CSR_UDF_SUPP		0x00400000 /* UDF supported */
#define CSR_FST_BTOB		0x00800000 /* Fast bk to bk Capable */
#define CSR_DATA_PTY_DTD	0x01000000 /* Data parity detected */
#define CSR_DEVSEL_FAST		0x00000000 /* DEVSEL timing fast */
#define CSR_DEVSEL_MED		0x02000000 /* DEVSEL timing medium */
#define CSR_DEVSEL_SLOW		0x04000000 /* DEVSEL timing slow */
#define CSR_SGD_TGT_ABRT 	0x08000000 /* signaled target abort */
#define CSR_RCV_TGT_ABRT 	0x10000000 /* received target abort */
#define CSR_RCV_MST_ABRT 	0x20000000 /* received master abort */
#define CSR_SGD_SYS_ERR		0x40000000 /* signaled system error */
#define CSR_DTD_PERR		0x80000000 /* detected parity error */

/* BIST register layout */

#define BIST_REG_BIST_CAPABLE	0x80	/* device supports BIST */
#define BIST_REG_BIST_START	0x40	/* start built in self test */

/* base address register for Memory */

#define INDEX_IO		0x00
#define INDEX_MEM		0x01
#define BASE_ADDR_IO_MASK	0xfffffffc /* mask for IO base addrs */
#define BASE_ADDR_MEM_MASK	0xfffffff0 /* mask for mem base addrs */
#define BASE_ADDR_REG_IO_SPACE	0x00000001 /* device mapped in I/O */
#define BASE_ADDR_REG_MEM_SPACE	0x00000000 /* device mapped in mem */
#define BASE_ADDR_REG_MEM_4GIG	0x00000000 /* mapped in A32 space */
#define BASE_ADDR_REG_MEM_1MEG	0x00000002 /* mapped in lower 1 Meg */
#define BASE_ADDR_REG_MEM_ANY	0x00000004 /* mapped in A64 space */
#define BASE_ADDR_REG_MEM_PFTCH	0x00000008 /* prefetechable bit */

/* pci function declerations */
#if defined(__STDC__) || defined(__cplusplus)

IMPORT	STATUS sysPciDevConfig (
				ULONG	devPciCnfgAdrs,
				ULONG	devIoBaseAdrs,
				ULONG	devMemBaseAdrs,
				ULONG	command,
				FUNCPTR	devPciRoutine,
				ULONG	devPciArg1,
				ULONG	devPCiArg2
				);
IMPORT	STATUS sysPciDevProbe (ULONG, ULONG, ULONG *);
IMPORT	STATUS sysPciDevShow (ULONG *);

#else

IMPORT	STATUS sysPciDevConfig ();
IMPORT	STATUS sysPciDevProbe ();
IMPORT	STATUS sysPciDevShow ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif	/* INCpciLocalBush */
