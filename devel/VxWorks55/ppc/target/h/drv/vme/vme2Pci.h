/* vme2Pci.h - vme2pci chip configuration header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,13nov96,mas  added VME2PCI_IO_OFFSET (SPR 7475).
01c,28feb95,caf  removed MSW() macro (now in vxWorks.h).
01b,20jan95,vin	 clean up & enhancements.
01a,09jan95,kvk  created.
*/

/*
This file contains type declarations for the vme2pci chip Header 
and the macros for the PCI BUS. This header has been designed
with respect to Genesis 1.5 Engineering Specification Version 1.0A.
Configuration registers have to be accessed as long words.
*/


#ifndef	INCvme2Pcih
#define	INCvme2Pcih


#ifdef  _ASMLANGUAGE
#define CASTB
#else
#define CASTB (char *)
#endif  /* _ASMLANGUAGE */


#define VME2PCI_REG_INTERVAL		4

/* to permit alternative board addressing */
#ifndef VME2PCI_IACK_ADRS		
#define VME2PCI_IACK_ADRS(reg)		(CASTB (VME2PCI_BASE_ADRS + \
					(0xF007) + \
					(reg * VME2PCI_REG_INTERVAL)))
#endif  /* VME2PCI_ADRS */

/*
 * Defines for the VME2PCI interrupt acknowledge registers
 */

#define VME2PCI_IACK_LEVEL1		VME2PCI_IACK_ADRS(0x00)
#define VME2PCI_IACK_LEVEL2		VME2PCI_IACK_ADRS(0x01)
#define VME2PCI_IACK_LEVEL3		VME2PCI_IACK_ADRS(0x02)
#define VME2PCI_IACK_LEVEL4		VME2PCI_IACK_ADRS(0x03)
#define VME2PCI_IACK_LEVEL5		VME2PCI_IACK_ADRS(0x04)
#define VME2PCI_IACK_LEVEL6		VME2PCI_IACK_ADRS(0x05)
#define VME2PCI_IACK_LEVEL7		VME2PCI_IACK_ADRS(0x06)


/* configuration space header */

typedef struct
    {
#if (_BYTE_ORDER == _BIG_ENDIAN)
    UINT16	pciSlvEndAddr1;		/* slave end addr1      0x40 */
    UINT16	pciSlvStrtAddr1; 	/* slave start addr1    0x42 */
    UINT16	pciSlvEnable1; 		/* pci slave enable 1   0x44 */
    UINT16	pciSlvAddrOffset1;	/* addr offset 1        0x46 */
    UINT16	pciSlvEndAddr2;		/* slave end addr2      0x48 */
    UINT16	pciSlvStrtAddr2; 	/* slave start addr2    0x4A */
    UINT16	pciSlvEnable2; 		/* pci slave enable 2   0x4C */
    UINT16	pciSlvAddrOffset2;	/* addr offset 2        0x4E */
#else
    UINT16	pciSlvStrtAddr1; 	/* slave start addr1    0x40 */
    UINT16	pciSlvEndAddr1;		/* slave end addr1      0x42 */
    UINT16	pciSlvAddrOffset1;	/* addr offset 1        0x44 */
    UINT16	pciSlvEnable1; 		/* pci slave enable 1   0x46 */
    UINT16	pciSlvStrtAddr2; 	/* slave start addr2    0x48 */
    UINT16	pciSlvEndAddr2;		/* slave end addr2      0x4A */
    UINT16	pciSlvAddrOffset2;	/* addr offset 2        0x4C */
    UINT16	pciSlvEnable2; 		/* pci slave enable 2   0x4E */
#endif /* _BYTE_ORDER == _BIG_ENDIAN */
    UINT32	pciIntEnable; 		/* pending intrpt enabl 0x50 */

    } VME2PCI_CNFG_HDR;


#define VME2PCI_CNFG_OFFSET	0x40	/* offset vme2Pci specific part */
#define VME2PCI_IO_OFFSET	0xF840	/* offset vme2Pci specific IO space */
	
/* vme2pci device specific masks */

#define VME2PCI_SLV_ADDR_MASK	0xffff0000	/* pci slave addr mask */
#define VME2PCI_SLV_EN_MASK	0x00010000	/* slave enable mask */
#define VME2PCI_INT_MASK        0x00000008	/* vme2PCI interrupt mask */
#define VME2PCI_INT_LVL_MASK	0x00000007	/* vme2PCI intr level mask */

#define VME2PCI_INT_LVL_GET(x)	(x & VME2PCI_INT_LVL_MASK)

#define VME2PCI_SLAVE_ENABLE	0x0001		/* slave enable flag */
#define VME2PCI_INT_ENABLE	0x00000008	/* Vme2PCI interrupt enable */

/* pci function declerations */
#if defined(__STDC__) || defined(__cplusplus)

extern int	sysVme2PciInit (VME2PCI_CNFG_HDR * pVme2PciCnfgHdr,
				ULONG  vme2PciCnfgBase); 

#else

extern int	sysVme2PciInit ();

#endif	/* __STDC__ */

#endif	/* INCvme2Pcih */


