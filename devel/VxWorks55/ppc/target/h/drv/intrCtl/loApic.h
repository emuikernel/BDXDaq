/* loApic.h - Intel Pentium[234] Local APIC/xAPIC (Advanced PIC) */

/* Copyright 1984-2002 Wind River Systems, Inc. */ 

/*
modification history
--------------------
01d,08mar02,hdn  added prototypes for HTT (spr 73738)
01c,21jun01,hdn  added support for Pentium4 Local xAPIC
01b,25may98,hdn  re-written
01a,26jun97,sub  written
*/

#ifndef __INCloApich
#define __INCloApich

#ifdef __cplusplus
extern "C" {
#endif

/* Local APIC Register Offset */

#define LOAPIC_ID			0x020	/* Local APIC ID Reg */
#define LOAPIC_VER			0x030	/* Local APIC Version Reg */
#define LOAPIC_TPR			0x080	/* Task Priority Reg */
#define LOAPIC_APR			0x090	/* Arbitration Priority Reg */
#define LOAPIC_PPR			0x0a0	/* Processor Priority Reg */
#define LOAPIC_EOI			0x0b0	/* EOI Reg */
#define LOAPIC_LDR			0x0d0	/* Logical Destination Reg */
#define LOAPIC_DFR			0x0e0	/* Destination Format Reg */
#define LOAPIC_SVR			0x0f0	/* Spurious Interrupt Reg */
#define LOAPIC_ISR			0x100	/* In-service Reg */
#define LOAPIC_TMR			0x180	/* Trigger Mode Reg */
#define LOAPIC_IRR			0x200	/* Interrupt Request Reg */
#define LOAPIC_ESR			0x280	/* Error Status Reg */
#define LOAPIC_ICRLO			0x300	/* Interrupt Command Reg */
#define LOAPIC_ICRHI			0x310	/* Interrupt Command Reg */
#define LOAPIC_TIMER			0x320	/* LVT (Timer) */
#define LOAPIC_THERMAL			0x330	/* LVT (Thermal) */
#define LOAPIC_PMC			0x340	/* LVT (PMC) */
#define LOAPIC_LINT0			0x350	/* LVT (LINT0) */
#define LOAPIC_LINT1			0x360	/* LVT (LINT1) */
#define LOAPIC_ERROR			0x370	/* LVT (ERROR) */
#define LOAPIC_TIMER_ICR		0x380	/* Timer Initial Count Reg */
#define LOAPIC_TIMER_CCR		0x390	/* Timer Current Count Reg */
#define LOAPIC_TIMER_CONFIG		0x3e0	/* Timer Divide Config Reg */

/* IA32_APIC_BASE MSR Bits */

#define LOAPIC_BASE_MASK		0xfffff000 /* LO APIC Base Addr mask */
#define LOAPIC_GLOBAL_ENABLE		0x00000800 /* LO APIC Global Enable */
#define LOAPIC_BSP			0x00000100 /* LO APIC BSP */

/* Local APIC ID Register Bits */

#define LOAPIC_ID_MASK			0x0f000000 /* LO APIC ID mask */

/* Local APIC Version Register Bits */

#define LOAPIC_VERSION_MASK		0x000000ff /* LO APIC Version mask */
#define LOAPIC_MAXLVT_MASK		0x00ff0000 /* LO APIC Max LVT mask */
#define LOAPIC_PENTIUM4			0x00000014 /* LO APIC in Pentium4 */
#define LOAPIC_LVT_PENTIUM4		5 	   /* LO APIC LVT - Pentium4 */
#define LOAPIC_LVT_P6			4 	   /* LO APIC LVT - P6 */
#define LOAPIC_LVT_P5			3 	   /* LO APIC LVT - P5 */

/* Local APIC Vector Table Bits */

#define LOAPIC_VECTOR			0x000000ff /* vectorNo */
#define LOAPIC_MODE			0x00000700 /* delivery mode */
#define LOAPIC_FIXED			0x00000000 /* delivery mode: FIXED */
#define LOAPIC_SMI			0x00000200 /* delivery mode: SMI */
#define LOAPIC_NMI			0x00000400 /* delivery mode: NMI */
#define LOAPIC_EXT			0x00000700 /* delivery mode: ExtINT */
#define LOAPIC_IDLE			0x00000000 /* delivery status: Idle */
#define LOAPIC_PEND			0x00001000 /* delivery status: Pend */
#define LOAPIC_HIGH			0x00000000 /* polarity: High */
#define LOAPIC_LOW			0x00002000 /* polarity: Low */
#define LOAPIC_REMOTE			0x00004000 /* remote IRR */
#define LOAPIC_EDGE			0x00000000 /* trigger mode: Edge */
#define LOAPIC_LEVEL			0x00008000 /* trigger mode: Level */
#define LOAPIC_MASK			0x00010000 /* mask */

/* Local APIC Spurious-Interrupt Register Bits */

#define	LOAPIC_ENABLE			0x100	/* APIC Enabled */
#define	LOAPIC_FOCUS_DISABLE		0x200	/* Focus Processor Checking */

/* Local APIC Timer Bits */

#define LOAPIC_TIMER_DIVBY_2		0x0	/* Divide by 2 */
#define LOAPIC_TIMER_DIVBY_4		0x1	/* Divide by 4 */
#define LOAPIC_TIMER_DIVBY_8		0x2	/* Divide by 8 */
#define LOAPIC_TIMER_DIVBY_16		0x3	/* Divide by 16 */
#define LOAPIC_TIMER_DIVBY_32		0x8	/* Divide by 32 */
#define LOAPIC_TIMER_DIVBY_64		0x9	/* Divide by 64 */
#define LOAPIC_TIMER_DIVBY_128		0xa	/* Divide by 128 */
#define LOAPIC_TIMER_DIVBY_1		0xb	/* Divide by 1 */
#define LOAPIC_TIMER_DIVBY_MASK		0xf	/* mask bits */
#define LOAPIC_TIMER_PERIODIC		0x00020000 /* Timer Mode: Periodic */

/* Local Vector's lock-unlock macro used in loApicIntLock/Unlock */

#define	LOCKED_TIMER			0x01
#define	LOCKED_PMC			0x02
#define	LOCKED_LINT0			0x04
#define	LOCKED_LINT1			0x08
#define	LOCKED_ERROR			0x10
#define	LOCKED_THERMAL			0x20

/* Interrupt Command Register: delivery mode and status */

#define MODE_FIXED			0x0	/* delivery mode: Fixed */
#define MODE_LOWEST			0x1	/* delivery mode: Lowest */
#define MODE_SMI			0x2	/* delivery mode: SMI */
#define MODE_NMI			0x4	/* delivery mode: NMI */
#define MODE_INIT			0x5	/* delivery mode: INIT */
#define MODE_STARTUP			0x6	/* delivery mode: StartUp */
#define STATUS_PEND			0x1000	/* delivery status: Pend */

/* MP Configuration Table Entries */

#define	MP_ENTRY_CPU			0	/* Entry Type: CPU */
#define	MP_ENTRY_BUS			1	/* Entry Type: BUS */
#define	MP_ENTRY_IOAPIC			2	/* Entry Type: IO APIC */
#define	MP_ENTRY_IOINTERRUPT		3	/* Entry Type: IO INT */
#define	MP_ENTRY_LOINTERRUPT		4	/* Entry Type: LO INT */

/* MP Configuration Table CPU Flags */

#define MP_CPU_FLAGS_BP			0x02

/* IMCR related bits */

#define	IMCR_ADRS			0x22	/* IMCR addr reg */
#define	IMCR_DATA			0x23	/* IMCR data reg */
#define	IMCR_REG_SEL			0x70	/* IMCR reg select */
#define	IMCR_IOAPIC_ON			0x01	/* IMCR IOAPIC route enable */
#define	IMCR_IOAPIC_OFF			0x00	/* IMCR IOAPIC route disable */


#ifndef _ASMLANGUAGE

/* MultiProcessor Specification version 1.4 */

typedef struct			/* MP Floating Pointer Structure */
    {
    char    signature [4];	/* "_MP_" */
    UINT32  configTableAddr;	/* address of the MP configuration table */
    UINT8   length;		/* length of the floating pointer structure */
    UINT8   specRev;		/* version number of the MP specification */
    UINT8   cksum;		/* checksum of the pointer structure */
    UINT8   featureByte[5];	/* MP feature bytes 1 - 5 */
    } MP_FPS;

typedef struct			/* MP Configuration Table Header */
    {
    char    mpSignature[4];	/* "PCMP" */
    UINT16  tableLength;	/* length of the base configuration table */
    UINT8   specRevision;	/* version number of the MP specification */
    UINT8   cksum;		/* checksum of the base configuration table */
    char    oemId[8];		/* string that identifies the manufacturer */
    char    prodId[12];		/* string that identifies the product */
    UINT32  oemTablePtr;	/* address to an OEM configuration table */
    UINT16  oemTableSize;	/* size of the OEM configuration table */
    UINT16  entryCount;		/* number of entries in the variable table */
    UINT32  localApicBaseAddr;	/* address of the Local APIC */
    UINT16  extendedTableLength; /* length of the extended entries */
    UINT8   extendedTableCksum; /* checksum of the extended table entries */
    UINT8   reserved;            
    } MP_HEADER; 

typedef struct			/* MP Config Table Entry for CPU's */
    {
    UINT8   entryType;		/* 0 identifies a processor entry */
    UINT8   localApicId;	/* Local APIC ID number */
    UINT8   localApicVersion;	/* Local APIC's version number */
    UINT8   cpuFlags;		/* EN: usable, BP: boot-processor */
    UINT32  cpuSig;		/* stepping, model, family of the CPU */
    UINT32  featureFlags;	/* feature definition flag */
    UINT32  reserved[2];
    } MP_CPU;

typedef struct			/* MP Config Table Entry for IO APIC's */
    {
    UINT8  entryType;		/* 2 identifies an IO APIC entry */
    UINT8  ioApicId;		/* ID of this IO APIC */
    UINT8  ioApicVersion;	/* version of this IO APIC */
    UINT8  ioApicFlags;		/* usable or not */
    UINT32 ioApicBaseAddress;	/* address of this IO APIC */
    } MP_IOAPIC;

/* variable declarations */

IMPORT UINT32	loApicBase;		/* default Local APIC addr */
IMPORT UINT32	ioApicBase;		/* default IO APIC index addr */
IMPORT UINT32	ioApicData;		/* default IO APIC data addr */
IMPORT UINT32	loApicId;		/* local APIC Id */
IMPORT UINT32	loApicVersion;		/* local APIC Version */
IMPORT UINT32	loApicMaxLvt;		/* local APIC Max LVT */


#if	defined(__STDC__) || defined(__cplusplus)

IMPORT void	loApicInit	(void);
IMPORT void	loApicEnable	(BOOL enable);
IMPORT STATUS	loApicIpi	(INT32 apicId, INT32 shortHand, INT32 trigger, 
				 INT32 level, INT32 destMode, INT32 deliMode, 
				 INT32 vectorNo);

#else

IMPORT void	loApicInit	();
IMPORT void	loApicEnable	();
IMPORT STATUS	loApicIpi	();

#endif  /* __STDC__ */


#endif  /* _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif /* __INCloApich */
