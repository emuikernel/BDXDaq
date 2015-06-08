/* cisLib.h - PCMCIA CIS library header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,02may01,dat  added WRS_PACK_ALIGN struct. spr 67168
01c,16jan97,hdn  added CIS_MAX_TUPLES.
01b,22feb96,hdn  cleaned up.
01a,10feb95,hdn  written
*/

#ifndef __INCcislibh
#define __INCcislibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/*
 * The following packing attribute is needed by code that reads the CIS tuples
 * to ensure the structure sizes can be used to calculate the proper offsets.
 */
#ifndef WRS_PACK_ALIGN
#   define WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))
#endif


typedef struct cisTupleA
    {
    u_char code;
    u_char dummy0;
    u_char link;
    u_char dummy1;
    } CIS_TUPLE_A;

typedef struct cisTupleC
    {
    u_char code;
    u_char link;
    } WRS_PACK_ALIGN(2) CIS_TUPLE_C;


typedef CIS_TUPLE_C CIS_TUPLE;

typedef union cisByte2
    {
    u_char	c[2];
    u_short	s;
    } CIS_BYTE2;

typedef union cisByte4
    {
    u_char	c[4];
    u_int	l;
    } CIS_BYTE4;

typedef struct cisIo
    {
    u_int	start;
    u_int	stop;
    } CIS_IO;

typedef struct cisMem
    {
    u_int	length;
    u_int	cAddr;
    u_int	hAddr;
    } CIS_MEM;

typedef struct cisConfig
    {
    DL_NODE	node;			/* Double Link Node */
    u_char	index;			/* configuration index */
    u_char	interfaceType;		/* interface type */
    u_char	vcc[3];			/* vcc */
    u_char	vpp1[3];		/* vpp1 */
    u_char	vpp2[3];		/* vpp2 */
    u_char	ioBuswidth;		/* io bus width */
    u_char	ioAddrlines;		/* io address lines */
    u_char	ioRanges;		/* io ranges */
    CIS_IO	io[16];			/* io window info */
    u_char	irqMode;		/* IRQ mode */
    u_char	irqMask;		/* IRQ mask */
    u_char	irqLevel;		/* IRQ level */
    u_char	irqSpecial;		/* IRQ special */
    CIS_BYTE2	irqBit;			/* IRQ bit */
    CIS_MEM	mem[8];			/* memory window info */
    u_char	twins;			/* twin */
    u_char	audio;			/* audio */
    u_char	readonly;		/* read only */
    u_char	pwrdown;		/* power down */
    } CIS_CONFIG;


/* assume max number of tuples in attribute or common memory */

#define	CIS_MAX_TUPLES		512

/* memory window for Tuple and Configuration-register */

#define	CIS_MEM_TUPLE		0
#define	CIS_MEM_CONFIG		1

/* Tuple codes */

#define CISTPL_NULL		0x00
#define CISTPL_DEVICE		0x01
#define CISTPL_CHECKSUM		0x10
#define CISTPL_LONGLINK_A	0x11
#define CISTPL_LONGLINK_C	0x12
#define CISTPL_LINKTARGET	0x13
#define CISTPL_NO_LINK		0x14
#define CISTPL_VERS_1		0x15
#define CISTPL_ALTSTR		0x16
#define CISTPL_DEVICE_A		0x17
#define CISTPL_JEDEC_C		0x18
#define CISTPL_JEDEC_A		0x19
#define CISTPL_CONFIG		0x1a
#define CISTPL_CFTABLE_ENTRY	0x1b
#define CISTPL_DEVICE_OC	0x1c
#define CISTPL_DEVICE_OA	0x1d
#define CISTPL_DEVICE_GEO	0x1e
#define CISTPL_DEVICE_GEO_A	0x1f
#define CISTPL_MANFID		0x20
#define CISTPL_FUNCID		0x21
#define CISTPL_FUNCE		0x22
#define CISTPL_SWIL		0x23
#define CISTPL_VERS_2		0x40
#define CISTPL_FORMAT		0x41
#define CISTPL_GEOMETRY		0x42
#define CISTPL_BYTEORDER	0x43
#define CISTPL_DATE		0x44
#define CISTPL_BATTERY		0x45
#define CISTPL_ORG		0x46
#define CISTPL_END		0xff

/* device speed codes */

#define DSPEED_NULL		0
#define DSPEED_250NS		1
#define DSPEED_200NS		2
#define DSPEED_150NS		3
#define DSPEED_100NS		4
#define DSPEED_EXT		7

/* device type codes */

#define DTYPE_NULL		0x00
#define DTYPE_ROM		0x10
#define DTYPE_OTPROM		0x20
#define DTYPE_EPROM		0x30
#define DTYPE_EEPROM		0x40
#define DTYPE_FLASH		0x50
#define DTYPE_SRAM		0x60
#define DTYPE_DRAM		0x70
#define DTYPE_FUNCSPEC		0xd0
#define DTYPE_EXTEND		0xe0

/* function ID */

#define FUNC_MULTI		0x0
#define FUNC_MEMORY		0x1
#define FUNC_SERIAL		0x2
#define FUNC_PARALLEL		0x3
#define FUNC_FIXEDDISK		0x4
#define FUNC_VIDEO		0x5
#define FUNC_LAN		0x6
#define FUNC_AIMS		0x7

/* function extension type and data */

#define FUNCE_TYPE_DISK		0x01
#define FUNCE_DATA_ATA		0x01

/* configuration register: configuration option register */

#define COR_SRESET		0x80
#define COR_LEVIREQ		0x40

/* configuration register: card configuration and status register */

#define CCSR_CHANGED		0x80
#define CCSR_SIGCHG		0x40
#define CCSR_IOIS8		0x20
#define CCSR_AUDIO		0x08
#define CCSR_PWRDWN		0x04
#define CCSR_INTR		0x02

/* configuration register: pin replacement register */

#define PRR_CBVD1		0x80
#define PRR_CBVD2		0x40
#define PRR_CRDY		0x20
#define PRR_CWPROT		0x10
#define PRR_RBVD1		0x08
#define PRR_RBVD2		0x04
#define PRR_RRDY		0x02
#define PRR_RWPROT		0x01


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	cisGet		(int sock);
extern void	cisFree		(int sock);
extern STATUS	cisConfigregGet	(int sock, int reg, int *pValue);
extern STATUS	cisConfigregSet	(int sock, int reg, int value);
extern void	cisShow		(int sock);

#else

extern STATUS	cisGet		();
extern void	cisFree		();
extern STATUS	cisConfigregGet	();
extern STATUS	cisConfigregSet	();
extern void	cisShow		();

#endif  /* __STDC__ */


#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCcislibh */
