/* smcFdc37b78x.h - a super i/o initialization include module */

/* Copyright 1984-1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,28mar00,mks written.
*/

/*
DESCRIPTION
This file contains mnemonics for SUPER I/0 (fdc37b78x) chip initialization.
*/

#ifndef INCsmcfdc37b78xh
#define INCsmcfdc37b78xh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */
    
/* wiring-dependent SMCFDC37B78X variables */    
#ifndef SMCFDC37B78X_CONFIG_PORT
#define SMCFDC37B78X_CONFIG_PORT	0x0370 
#endif

#ifndef SMCFDC37B78X_INDEX_PORT
#define SMCFDC37B78X_INDEX_PORT	        0x0370
#endif

#ifndef SMCFDC37B78X_DATA_PORT
#define SMCFDC37B78X_DATA_PORT	        (SMCFDC37B78X_INDEX_PORT + 1)
#endif

#define SMCFDC37B78XWRT(ioPort, value) sysOutByte (ioPort, value)
#define SMCFDC37B78XRD(ioPort)         sysInByte (ioPort)

/* configuration mode begin command */    
#define SMCFDC37B78X_CONFIG_START       0x55
/* configuration mode end command */    
#define SMCFDC37B78X_CONFIG_END         0xAA   

/* chip level registers */
    
/* logical device configuration register */
#define SMCFDC37B7X_LOGDEVCFGREG  0x07  

/* logical device level registers */
#define SMCFDC37B78X_LOGDEV_ACTIVATE    0x30    /* activation register */
#define SMCFDC37B78X_IOBASE_HIGH	0x60    /* i/o base - high byte */
#define SMCFDC37B78X_IOBASE_LOW		0x61    /* i/o base - low byte */
#define SMCFDC37B78X_INTERRUPT		0x70    /* interrupt config */
#define SMCFDC37B78X_MODE		0xF0    /* mode register */

/* defined logical devices */
#define SMCFDC37B78X_FDD	0x00    /* floppy disk drive */
#define SMCFDC37B78X_LPT1       0x03    /* parallel port */
#define SMCFDC37B78X_COM1	0x04    /* serial device - port 1 */
#define SMCFDC37B78X_COM2	0x05    /* serial device - port 2 */
#define SMCFDC37B78X_KBD        0x07    /* keyboard */
#define SMCFDC37B78X_AUX	0x08    /* auxiliary i/o */
#define SMCFDC37B78X_ACPI       0x0A    /* ACPI */

/* use these defines for enabling specific device. DON'T USE ONE ABOVE. */
#define SMCFDC37B78X_FDD_EN	0x01    /* floppy disk drive */
#define SMCFDC37B78X_LPT1_EN    0x02    /* parallel port */
#define SMCFDC37B78X_COM1_EN    0x04    /* serial device - port 1 */
#define SMCFDC37B78X_COM2_EN	0x08    /* serial device - port 2 */
#define SMCFDC37B78X_KBD_EN     0x10    /* keyboard */
    
/* device specific intialization values */

#define SMCFDC37B78X_ACT_DEV    0x01    /* activate device */
#define SMCFDC37B78X_COM_HSM	0x02    /* enable high speed services on COM */

#define SMCFDC37B78X_INTR_KBD   0x01    /* irq line # 1 tied to KBD */
#define SMCFDC37B78X_INTR_COM1	0x04    /* irq line # 4 tied to COM 1 */
#define SMCFDC37B78X_INTR_COM2	0x03    /* irq line # 5 tied to COM 2 */
#define SMCFDC37B78X_INTR_LPT1	0x07    /* irq line # 7 tied to LPT */

#define SMCFDC37B78X_COM_MCR	        0x04
#define SMCFDC37B78X_COM_MCROUT2	0x08    
#define SMCFDC37B78X_COM_MCROUT2_MASK   0x08

#define SMCFDC37B78X_COM1_IOHIGH	0x03    /* COM1, i/o high byte */
#define SMCFDC37B78X_COM1_IOLOW		0xF8    /* COM1, i/o low  byte */
#define SMCFDC37B78X_COM1_IOBASE	0x03F8  /* COM1, i/o address */
#define SMCFDC37B78X_COM1_MCR           (SMCFDC37B78X_COM1_IOBASE + \
                                        SMCFDC37B78X_COM_MCR)

#define SMCFDC37B78X_COM2_IOHIGH	0x02    /* COM2, i/o high byte */
#define SMCFDC37B78X_COM2_IOLOW		0xF8    /* COM2, i/o low  byte */
#define SMCFDC37B78X_COM2_IOBASE	0x02F8  /* COM2, i/o address */
#define SMCFDC37B78X_COM2_MCR           (SMCFDC37B78X_COM2_IOBASE + \
                                        SMCFDC37B78X_COM_MCR)

#define SMCFDC37B78X_LPT1_IOHIGH	0x03    /* LPT1, i/o high byte */
#define SMCFDC37B78X_LPT1_IOLOW		0xBC    /* LPT1, i/o low  byte */
#define SMCFDC37B78X_LPT1_IOBASE	0x03BC  /* LPT1, i/o address */

/* GPIO Stuff */

#define SMCFDC37B78X_IRQMUX_IDX		0xC0    /* IRQ Mux register */
#define SMCFDC37B78X_IRQ_PARALLEL	0x07

#define SMCFDC37B78X_GP53_INDEX		0xD4    /* IRQ 2 pin */
#define SMCFDC37B78X_GP53_IRQ11_OUT		0x08
#define SMCFDC37B78X_GP60_INDEX		0xD0    /* IRQ 1 pin */
#define SMCFDC37B78X_GP60_IRQ1_OUT		0x08
#define SMCFDC37B78X_GP61_INDEX		0xD1    /* IRQ 3 pin */
#define SMCFDC37B78X_GP61_IRQ3_OUT		0x08
#define SMCFDC37B78X_GP62_INDEX		0xD2    /* IRQ 4 pin */
#define SMCFDC37B78X_GP62_IRQ4_OUT		0x08
#define SMCFDC37B78X_GP64_INDEX		0xD4    /* IRQ 6 pin */
#define SMCFDC37B78X_GP64_IRQ6_OUT		0x08
#define SMCFDC37B78X_GP65_INDEX		0xD5    /* IRQ 7 pin */
#define SMCFDC37B78X_GP65_IRQ7_OUT		0x08

/* defined masks */
#define SMCFDC37B78X_COM_HSM_MASK	0x00
#define SMCFDC37B78X_GP_FUNC_MASK	0xBB
#define SMCFDC37B78X_IRQ_PARALLEL_MASK	0x07
#define SMCFDC37B78X_MASK_ALL   	0xFF

/* keyboard specific defines */
#define SMCFDC37B78X_KBD_DATA           0x60  /* ISA port addresses */
#define SMCFDC37B78X_KBD_CTLSTS         0x64

#define SMCFDC37B78X_KBD_CCMD_SELFTEST  0xAA  /* kbd controller commands */
#define SMCFDC37B78X_KBD_CCMD_IFCTEST   0xAB
#define SMCFDC37B78X_KBD_CCMD_WRTPORT2  0xD1

#define SMCFDC37B78X_KBD_CTLSTS_IBF     0x2   /* kbd status bit [1:0] */
#define SMCFDC37B78X_KBD_CTLSTS_OBF     0x1

#define SMCFDC37B78X_KBD_SELFTEST_PASS  0x55     
#define SMCFDC37B78X_KBD_IFCTEST_PASS   0x00
#define SMCFDC37B78X_KBD_IRQ_KSC_EN     0xD3

#define SMCFDC37B78X_KBD_DELAY()        sysWait()

#define SMCFDC37B78X_KBD_RD_DATA()      (SMCFDC37B78X_KBD_DELAY(), \
                                        sysInByte (SMCFDC37B78X_KBD_DATA))
#define SMCFDC37B78X_KBD_RD_CTLSTS()    (SMCFDC37B78X_KBD_DELAY(), \
                                        sysInByte (SMCFDC37B78X_KBD_CTLSTS))
#define SMCFDC37B78X_KBD_WRT_DATA(x)    (SMCFDC37B78X_KBD_DELAY(), \
                                        sysOutByte (SMCFDC37B78X_KBD_DATA, x))
#define SMCFDC37B78X_KBD_WRT_CTLSTS(x)  (SMCFDC37B78X_KBD_DELAY(), \
                                        sysOutByte (SMCFDC37B78X_KBD_CTLSTS, x))
    
/* typedefs */

typedef struct smcFdc37b78x_IoPorts
    {
    int    config;
    int    index;
    int    data;
    } SMCFDC37B78X_IOPORTS;

/* function declarations */

extern VOID   smcFdc37b78xDevCreate (SMCFDC37B78X_IOPORTS *fdc37b78x_iop);        
extern VOID   smcFdc37b78xInit (int devInitMask);
extern STATUS smcFdc37b78xKbdInit (VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* INCsmcfdc37b78xh */

















