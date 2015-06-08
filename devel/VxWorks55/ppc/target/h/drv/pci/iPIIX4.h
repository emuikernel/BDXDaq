/* iPIIX4.h - 82371AB PCI ISA IDE Xcelerator (PIIX4) header */

/*
modification history
--------------------
01a,17oct01,dmh  written.
*/

/*
This file contains IO address and related constants for the
PIIX4 (PCI to ISA/IDE bridge).  Use this instead of iPiix4Pci.h.
*/

#ifndef	_iPIIX4_h
#define	_iPIIX4_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */
    
/* PCI to ISA/IDE bridge */

#define IPIIX4_PCI_VENID         0x8086   /* PCI config spec for PIIX4 device */
  
#define IPIIX4_PCI_DEV0          0x7110                    
#define IPIIX4_PCI_DEV1          0x7111
#define IPIIX4_PCI_DEV2          0x7112              
#define IPIIX4_PCI_DEV3          0x7113

#define IPIIX4_PCI_FUNC0         0x0
#define IPIIX4_PCI_FUNC1         0x1              
#define IPIIX4_PCI_FUNC2         0x2
#define IPIIX4_PCI_FUNC3         0x3  

#define IPIIX4_PCI_INST0         0x0

#define IPIIX4_PCI_NOIDX         -1
#define IPIIX4_PCI_NOINT         0xFF

#define IPIIX4_PCI_XINT_MAX      0x4      /* Max # of interrupt to route */

/* PIRQ[A:D]- Suggested interrupt routing and not absolutely required */

#define IPIIX4_PCI_XINT1_LVL     0x9
#define IPIIX4_PCI_XINT2_LVL     0xA
#define IPIIX4_PCI_XINT3_LVL     0xB
#define IPIIX4_PCI_XINT4_LVL     0xC
    
#define IPIIX4_PCI_DEVRESD       0x50      /* PCI based PIIX4 registers */
#define IPIIX4_PCI_DEVRESA       0x5C
#define IPIIX4_PCI_DEVRESB       0x60
#define IPIIX4_PCI_PCICMD        0x04
#define IPIIX4_PCI_MLT           0x0D
#define IPIIX4_PCI_BMIBA         0x20
#define IPIIX4_PCI_IDETIM        0x40
#define IPIIX4_PCI_PIRQRCA       0x60
#define IPIIX4_PCI_PIRQRCB       0x61
#define IPIIX4_PCI_PIRQRCC       0x62
#define IPIIX4_PCI_PIRQRCD       0x63          
#define IPIIX4_PCI_ELCR1         0x4D0
#define IPIIX4_PCI_ELCR2         0x4D1          
          
#define IPIIX4_PCI_IRQ1ENDEV11   0x0800          /* bit #19 - DEVRESD */
#define IPIIX4_PCI_RESENDEV5     0x08            /* bit #11 - DEVRESD */          
#define IPIIX4_PCI_KBCENDEV11    0x10000000      /* bit #28 - DEVRESA */
#define IPIIX4_PCI_KBCEIOEN      0x40000000      /* bit #30 - DEVRESB */
#define IPIIX4_PCI_PCICMD_BME    0x04            /* bit #2  - PCICMD  */
#define IPIIX4_PCI_PCICMD_IOSE   0x01            /* bit #0  - PCICMD  */
#define IPIIX4_PCI_MLT_MLTCV     0x40            /* bit [7:4] - MLT   */
#define IPIIX4_PCI_BMIBA_RTE     0x1             /* bit #0  - BMIBA   */
#define IPIIX4_PCI_IDETIM_VAL    0xa307a307      /* Pri + Sec Chans   */

#define IPIIX4_PCI_FD_DCM               0xD6     /* DMA Channel Mode Register */
#define IPIIX4_PCI_FD_RWAMB             0xDE     /* Read/Write All Mask Bits */
#define IPIIX4_PCI_FD_DCM_CASCADE       0xC0     /* bits [7:6] of DCM */
#define IPIIX4_PCI_FD_DCM_AUTOINIT      0x10     /* bit [4] of DCM */    
#define IPIIX4_PCI_FD_RWAMB_MASKALL     0x00

/* function declarations */

extern STATUS iPIIX4Init ();
extern STATUS iPIIX4FdInit ();
extern STATUS iPIIX4KbdInit ();
extern STATUS iPIIX4AtaInit ();
extern STATUS iPIIX4IntrRoute (int pintx, char irq);
extern char   iPIIX4GetIntr (int pintx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _iPIIX4_h */
