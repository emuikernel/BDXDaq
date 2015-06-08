/* dga001Vme.h - Densan DGA-001 VMEbus Interface Controller */

/* Copyright 1994-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01l,12jul97,hk   renamed as dga001Vme.h from drv/multi/dga001.h.
01k,15jan96,hk   added support for DVE-SH7043.
01j,10nov95,hk   changed CSR to DGA_CSR.
01i,09nov95,hk   fixed CSR1_1_RRS, but not used. did some code reviewing.
01h,04jul95,hk   modified some macro names.
01g,07jun95,hk   changed CSR20/CSR21 bit defs. copyright to '95.
01f,15dec94,sa   wrote CSR13_0, CSR15_0, CSR16_0, CSR17, IFR0, IFR1, IFR3.
01e,14nov94,hk   wrote CSR18_1.
01d,09nov94,sa   write CSR12, CSR14 and CSR23.
01c,19oct94,sa   write CSR19 to CSR21
01b,08sep94,sa   write to CSR7
01a,07sep94,sa   derived from 01e of vmechip.h.
*/

#ifdef  DOC
#define INCdga001Vmeh
#endif  /* DOC */

#ifndef __INCdga001Vmeh
#define __INCdga001Vmeh

#ifdef __cplusplus
extern "C" {
#endif

/*
This file contains constants for the Densan DGA-001 VMEbus Interface.
The macro DGA001_BASE_ADRS must be defined when using local bus addresses
defined in this header.  The macro IFR_BUS_BASE_ADRS must be defined when
using VMEbus addresses defined in this header.

The following standards have been adopted in the creation of this file.

Registers in the CSR have the prefix "DGA_CSR". And "DGA_CSR0_0" means most
low address of CSR0 register.

Registers in the IFR have the prefix "DGA_IFR" when addressed locally and
the prefix "IFR_BUS" when addressed from the VMEbus.  The bit definitions
are shared.

The registers are listed in ascending (numerical) order; the definitions
for each register start with a header, e.g.:

 Register                Register Register Register
 Mnemonic                Number   Address  Name
    |                        |       |       |
    v                        v       v       v
/@***********************************************************************
* CSR0_0                    0x00    0x00    local bus control and status*
***********************************************************************@/

The format of the definitions is as follows.  The define name always
starts with the register mnemonic it is associated with.  The [7-0]
number at the end of the comment indicates the bit position to which the
the define applies.  In the definition of PURSTF, for example, the 7 
indicates bit 7:
                                                                     |
                                                                     v
#define CSR1_0_PURSTF     0x80    /@ power-up status bit             7 @/

If the define applies to more than one bit, then the applicable bit
range is specified by two digits.  In the following example,
CSR0_LBT applies to the two bit field, bits 5-4:
                                                                     |
                                                                     v
#define CSR0_0_LBT        0x10    /@ local bus time-out (mask)      54 @/

If no bit field is given, then the define applies to the whole register.
*/


#define DGA001_LONG_ADRS(offset) \
    ((volatile UINT32 *)(DGA001_BASE_ADRS + offset))

#if defined(TARGET_DVE7604) || defined(TARGET_DVE7043)
#define DGA001_BYTE_ADRS(offset) \
    ((volatile UINT8 *)(DGA001_BASE_ADRS + DGA001_BYTE_ACCESS_OFFSET + offset))
#else
#define DGA001_BYTE_ADRS(offset) \
    ((volatile UINT8 *)(DGA001_BASE_ADRS + offset))
#endif /* TARGET_DVE7604 */

/*
 * Control and Status Register (CSR) definitions.
 *
 * only the local CPU can access the LCSR.
 */

#define DGA_CSR0		DGA001_LONG_ADRS(0x00)
#define DGA_CSR0_0		DGA001_BYTE_ADRS(0x00)
#define DGA_CSR0_1		DGA001_BYTE_ADRS(0x01)
#define DGA_CSR0_2		DGA001_BYTE_ADRS(0x02)
#define DGA_CSR0_3		DGA001_BYTE_ADRS(0x03)
#define DGA_CSR1		DGA001_LONG_ADRS(0x04)
#define DGA_CSR1_0		DGA001_BYTE_ADRS(0x04)
#define DGA_CSR1_1		DGA001_BYTE_ADRS(0x05)
#define DGA_CSR1_2		DGA001_BYTE_ADRS(0x06)
#define DGA_CSR1_3		DGA001_BYTE_ADRS(0x07)
#define DGA_CSR2		DGA001_LONG_ADRS(0x08)
#define DGA_CSR2_0		DGA001_BYTE_ADRS(0x08)
#define DGA_CSR2_1		DGA001_BYTE_ADRS(0x09)
#define DGA_CSR2_2		DGA001_BYTE_ADRS(0x0a)
#define DGA_CSR2_3		DGA001_BYTE_ADRS(0x0b)
#define DGA_CSR3		DGA001_LONG_ADRS(0x0c)
#define DGA_CSR3_0		DGA001_BYTE_ADRS(0x0c)
#define DGA_CSR3_1		DGA001_BYTE_ADRS(0x0d)
#define DGA_CSR3_2		DGA001_BYTE_ADRS(0x0e)
#define DGA_CSR3_3		DGA001_BYTE_ADRS(0x0f)
#define DGA_CSR4		DGA001_LONG_ADRS(0x10)
#define DGA_CSR4_0		DGA001_BYTE_ADRS(0x10)
#define DGA_CSR4_1		DGA001_BYTE_ADRS(0x11)
#define DGA_CSR4_2		DGA001_BYTE_ADRS(0x12)
#define DGA_CSR4_3		DGA001_BYTE_ADRS(0x13)
#define DGA_CSR5		DGA001_LONG_ADRS(0x14)
#define DGA_CSR5_0		DGA001_BYTE_ADRS(0x14)
#define DGA_CSR5_1		DGA001_BYTE_ADRS(0x15)
#define DGA_CSR5_2		DGA001_BYTE_ADRS(0x16)
#define DGA_CSR5_3		DGA001_BYTE_ADRS(0x17)
#define DGA_CSR6		DGA001_LONG_ADRS(0x18)
#define DGA_CSR6_0		DGA001_BYTE_ADRS(0x18)
#define DGA_CSR6_1		DGA001_BYTE_ADRS(0x19)
#define DGA_CSR6_2		DGA001_BYTE_ADRS(0x1a)
#define DGA_CSR6_3		DGA001_BYTE_ADRS(0x1b)
#define DGA_CSR7		DGA001_LONG_ADRS(0x1c)
#define DGA_CSR7_0		DGA001_BYTE_ADRS(0x1c)
#define DGA_CSR7_1		DGA001_BYTE_ADRS(0x1d)
#define DGA_CSR7_2		DGA001_BYTE_ADRS(0x1e)
#define DGA_CSR7_3		DGA001_BYTE_ADRS(0x1f)
#define DGA_CSR8		DGA001_LONG_ADRS(0x21)
#define DGA_CSR9		DGA001_LONG_ADRS(0x24)
#define DGA_CSR10		DGA001_LONG_ADRS(0x28)
#define DGA_CSR10_0		DGA001_BYTE_ADRS(0x28)
#define DGA_CSR10_1		DGA001_BYTE_ADRS(0x29)
#define DGA_CSR10_2		DGA001_BYTE_ADRS(0x2a)
#define DGA_CSR10_3		DGA001_BYTE_ADRS(0x2b)
#define DGA_CSR11		DGA001_LONG_ADRS(0x2c)
#define DGA_CSR11_0		DGA001_BYTE_ADRS(0x2c)
#define DGA_CSR11_1		DGA001_BYTE_ADRS(0x2d)
#define DGA_CSR11_2		DGA001_BYTE_ADRS(0x2e)
#define DGA_CSR11_3		DGA001_BYTE_ADRS(0x2f)
#define DGA_CSR12		DGA001_LONG_ADRS(0x30)
#define DGA_CSR13		DGA001_LONG_ADRS(0x34)
#define DGA_CSR14		DGA001_LONG_ADRS(0x38)
#define DGA_CSR15		DGA001_LONG_ADRS(0x3c)
#define DGA_CSR16		DGA001_LONG_ADRS(0x40)
#define DGA_CSR17		DGA001_LONG_ADRS(0x44)
#define DGA_CSR18		DGA001_LONG_ADRS(0x48)
#define DGA_CSR18_1		DGA001_BYTE_ADRS(0x49)
#define DGA_CSR18_2		DGA001_BYTE_ADRS(0x4a)
#define DGA_CSR18_3		DGA001_BYTE_ADRS(0x4b)
#define DGA_CSR19		DGA001_LONG_ADRS(0x4c)
#define DGA_CSR19_0		DGA001_BYTE_ADRS(0x4c)
#define DGA_CSR19_1		DGA001_BYTE_ADRS(0x4d)
#define DGA_CSR19_2		DGA001_BYTE_ADRS(0x4e)
#define DGA_CSR19_3		DGA001_BYTE_ADRS(0x4f)
#define DGA_CSR20		DGA001_LONG_ADRS(0x50)
#define DGA_CSR21		DGA001_LONG_ADRS(0x54)
#define DGA_CSR22		DGA001_LONG_ADRS(0x58)
#define DGA_CSR23		DGA001_LONG_ADRS(0x5c)
#define DGA_CSR24		DGA001_LONG_ADRS(0x60)
#define DGA_CSR25		DGA001_LONG_ADRS(0x64)
#define DGA_CSR26		DGA001_LONG_ADRS(0x68)
#define DGA_CSR27		DGA001_LONG_ADRS(0x6c)

/*
 * Interface Register (IFR) definitions.
 *
 * All registers of the IFR are accessible to both the local processor
 * and to other VMEbus masters.
 */

#define DGA_IFR0		DGA001_LONG_ADRS(0x70)
#define DGA_IFR1		DGA001_LONG_ADRS(0x74)
#define DGA_IFR2		DGA001_LONG_ADRS(0x78)
#define DGA_IFR3		DGA001_LONG_ADRS(0x7c)

/*************************************************************************
* CSR0_0                0x00    0x00     local bus control               *
*************************************************************************/

#define CSR0_0_LBT_DIS	0x30	/* local bus time-out disable	    5-4 */
#define CSR0_0_LBT_256	0x20	/* local bus time-out 256 clks 	    5-4 */
#define CSR0_0_LBT_128	0x10	/* local bus time-out 128 clks 	    5-4 */
#define CSR0_0_LBT_64	0x00	/* local bus time-out 64 clks 	    5-4 */

#define CSR0_0_TAOUTE	0x08	/* local bus TAOUT enable             3 */

/*************************************************************************
* CSR0_1                0x00    0x01    bus error control and status     *
*************************************************************************/

#define CSR0_1_BERDYO	0x80	/* activate RDYO# upon bus error      7 */

#define CSR0_1_LBTOUT	0x04	/* local bus time-out (status) 	      2 */
#define CSR0_1_RQTOUT	0x02	/* request time-out (status) 	      1 */
#define CSR0_1_VMEBERR	0x01	/* VMEbus bus error (status)          0 */

/*************************************************************************
* CSR0_2                0x00    0x02    general purpose output enable    *
*************************************************************************/

#define CSR0_2_GPOE7	0x80	/* general purpose output 7 enable    7 */
#define CSR0_2_GPOE6	0x40	/* general purpose output 6 enable    6 */
#define CSR0_2_GPOE5	0x20	/* general purpose output 5 enable    5 */
#define CSR0_2_GPOE4	0x10	/* general purpose output 4 enable    4 */
#define CSR0_2_GPOE3	0x08	/* general purpose output 3 enable    3 */
#define CSR0_2_GPOE2	0x04	/* general purpose output 2 enable    2 */
#define CSR0_2_GPOE1	0x02	/* general purpose output 1 enable    1 */
#define CSR0_2_GPOE0	0x01	/* general purpose output 0 enable    0 */

/*************************************************************************
* CSR0_3                0x00    0x03    general purpose I/O              *
*************************************************************************/

#define CSR0_3_GPIO7	0x80	/* general purpose I/O 7              7 */
#define CSR0_3_GPIO6	0x40	/* general purpose I/O 6              6 */
#define CSR0_3_GPIO5	0x20	/* general purpose I/O 5              5 */
#define CSR0_3_GPIO4	0x10	/* general purpose I/O 4              4 */
#define CSR0_3_GPIO3	0x08	/* general purpose I/O 3              3 */
#define CSR0_3_GPIO2	0x04	/* general purpose I/O 2              2 */
#define CSR0_3_GPIO1	0x02	/* general purpose I/O 1              1 */
#define CSR0_3_GPIO0	0x01	/* general purpose I/O 0              0 */

/*************************************************************************
* CSR1_0                0x01    0x04    reset control and status         *
*************************************************************************/

#define CSR1_0_PURSTF	0x80	/* power-up reset (status)            7 */

#define CSR1_0_RSWEN	0x40	/* reset switch enable                6 */
#define CSR1_0_RSWSYS	0x20	/* VMEbus SYSRESET enable             5 */

/*************************************************************************
* CSR1_1                0x01    0x05    system controller configuration  *
*************************************************************************/

#define CSR1_1_SYSCON	0x80	/* system controller (status)         7 */

#define CSR1_1_RRS	0x10	/* round robin arbitration	      4 */
#define CSR1_1_PRI	0x00	/* prioritized arbitration            4 */

#define CSR1_1_ARBT_DIS	0x0c	/* arbitration time-out disable	    3-2 */
#define CSR1_1_ARBT_63	0x08	/* arbitration time-out 63 usec     3-2 */
#define CSR1_1_ARBT_31	0x04	/* arbitration time-out 31 usec     3-2 */
#define CSR1_1_ARBT_15	0x00	/* arbitration time-out 15 usec     3-2 */

#define CSR1_1_SBT_DIS	0x03	/* VME bus time-out disable         1-0 */
#define CSR1_1_SBT_511	0x02	/* VME bus time-out 511 usec        1-0 */
#define CSR1_1_SBT_255	0x01	/* VME bus time-out 255 usec        1-0 */
#define CSR1_1_SBT_127	0x00	/* VME bus time-out 127 usec        1-0 */

/*************************************************************************
* CSR1_2                0x01    0x06    VMEbus requester control         *
*************************************************************************/

#define CSR1_2_DWB	0x80	/* VMEbus request                     7 */
#define CSR1_2_OWN	0x40	/* VMEbus request (status)            6 */

#define CSR1_2_RQT_DIS	0x30	/* request time-out disable         5-4 */
#define CSR1_2_RQT_1023	0x20	/* request time-out 1023 usec       5-4 */
#define CSR1_2_RQT_511	0x10	/* request time-out 511 usec        5-4 */
#define CSR1_2_RQT_255	0x00	/* request time-out 255 usec        5-4 */

#define CSR1_2_RNV	0x0c	/* release never                    3-2 */
#define CSR1_2_RBC	0x08	/* release when bus clear           3-2 */
#define CSR1_2_RWD	0x04	/* release when done                3-2 */
#define CSR1_2_ROR	0x00	/* release on request               3-2 */

#define CSR1_2_BR3	0x03	/* VMEbus request level 3           1-0 */
#define CSR1_2_BR2	0x02	/* VMEbus request level 2           1-0 */
#define CSR1_2_BR1	0x01	/* VMEbus request level 1           1-0 */
#define CSR1_2_BR0	0x00	/* VMEbus request level 0           1-0 */

/*************************************************************************
* CSR1_3                0x01    0x07    VMEbus master control            *
*************************************************************************/

#define CSR1_3_RMC	0x80	/* read-modify-write cycle            7 */

#define CSR1_3_AAMEN	0x40	/* addess modifier code enable        6 */
#define CSR1_3_AAMDIS	0x00	/* addess modifier code disable       6 */
#define CSR1_3_AAM	0x3f	/* addess modifier code (mask)      5-0 */

/*************************************************************************
* CSR2_0                0x02    0x08    unused                           *
* to                                                                     *
* CSR3_1                0x03    0x0d    unused                           *
*************************************************************************/

/*************************************************************************
* CSR3_2                0x03    0x0e    interface register address       *
*************************************************************************/

#define CSR3_2_IFRA_H	0xff	/* interface register addess (mask) 7-0 */

/*************************************************************************
* CSR3_3                0x03    0x0f    interface register address       *
*************************************************************************/

#define CSR3_3_IFRA_L	0xf0	/* interface register addess (mask) 7-4 */

#define CSR3_3_IFRSUP	0x02	/* respond to supervisor              1 */
#define CSR3_3_IFRUSR	0x01	/* respond to user                    0 */

/*************************************************************************
* CSR4_01               0x04    0x10    slave address 0                  *
*************************************************************************/

#define CSR4_01_SL0A	0xffff	/* slave addess 0 (mask)           15-0 */

/*************************************************************************
* CSR4_23               0x04    0x12    address mask 0                   *
*************************************************************************/

#define CSR4_23_AM0A	0xffff	/* address mask 0 (mask)           15-0 */

/*************************************************************************
* CSR5_01               0x05    0x14    mapping address 0                *
*************************************************************************/

#define CSR5_01_MP0A	0xffff	/* mapping address 0 (mask)        15-0 */

/*************************************************************************
* CSR5_23               0x05    0x16    slave control 0                  *
*************************************************************************/

#define CSR5_23_A32EN0	0x0020	/* respond to extended                5 */
#define CSR5_23_A24EN0	0x0010	/* respond to standard                4 */
#define CSR5_23_SUPER0	0x0008	/* respond to supervisor              3 */
#define CSR5_23_USER0	0x0004	/* respond to user                    2 */
#define CSR5_23_PRGRM0	0x0002	/* respond to program                 1 */
#define CSR5_23_DATA0	0x0001	/* respond to data                    0 */

/*************************************************************************
* CSR6_01               0x06    0x18    save address 1                   *
*************************************************************************/

#define CSR6_01_SL0A	0xffff	/* slave addess 1 (mask)           15-0 */

/*************************************************************************
* CSR6_23               0x06    0x1a    address mask 1                   *
*************************************************************************/

#define CSR6_23_AM0A	0xffff	/* address mask 1 (mask)           15-0 */

/*************************************************************************
* CSR7_01               0x07    0x1c    mapping addess 1                 *
*************************************************************************/

#define CSR7_01_MP0A	0xffff	/* mapping address 1 (mask)        15-0 */

/*************************************************************************
* CSR7_23               0x07    0x1e    slave control 1                  *
*************************************************************************/

#define CSR7_23_A32EN0	0x0020	/* respond to extended                5 */
#define CSR7_23_A24EN0	0x0010	/* respond to standard                4 */
#define CSR7_23_SUPER0	0x0008	/* respond to supervisor              3 */
#define CSR7_23_USER0	0x0004	/* respond to user                    2 */
#define CSR7_23_PRGRM0	0x0002	/* respond to program                 1 */
#define CSR7_23_DATA0	0x0001	/* respond to data                    0 */

/*************************************************************************
* CSR12                 0x0c    0x30    tick timer 0 control             *
*************************************************************************/

#define CSR12_TT0CEN	0x80000000	/* tick timer 0 enable          */

/*************************************************************************
* CSR13                 0x0d    0x34    tick timer 0 counter             *
*************************************************************************/

#define CSR13_TT0OCL	0x80000000	/* overflow counter clear       */

/*************************************************************************
* CSR14                 0x0e    0x38    tick timer 1 control             *
*************************************************************************/

#define CSR14_TT1CEN	0x80000000	/* tick timer 1 enable          */

/*************************************************************************
* CSR15                 0x0f    0x3c    tick timer 1 counter             *
*************************************************************************/

#define CSR15_TT1OCL	0x80000000	/* overflow counter clear       */

/*************************************************************************
* CSR16                 0x10    0x40    watchdog timer control           *
*************************************************************************/

#define CSR16_WDTCEN	0x80000000	/* watchdog timer enable        */
#define	CSR16_WDTSFE	0x40000000	/* watchdog timer SYSFAIL enable*/

/*************************************************************************
* CSR17                 0x11    0x44    watchdog timer counter           *
*************************************************************************/

#define CSR17_WDTCLR	0x80000000	/* watchdog timer counter clear */
#define	CSR17_WDTOUT	0x01000000	/* watchdog timeout (status)    */

/*************************************************************************
* CSR18_1               0x12	0x49	interrupt sense mode control	 *
*************************************************************************/

#define	CSR18_1_ACFLVL	0x80	/* ACFAIL  int level(1)/edge(0) sense 7 */
#define	CSR18_1_ABTLVL	0x40	/* ABORT   int level(1)/edge(0) sense 6 */
#define	CSR18_1_SFLVL	0x20	/* SYSFAIL int level(1)/edge(0) sense 5 */

/*************************************************************************
* CSR20                 0x14    0x50    interrupt status                 *
*************************************************************************/

#define CSR20_ACFIRQ	0x80000000	/* ACFAIL interrupt        31 */
#define CSR20_ABTIRQ	0x40000000	/* ABORT  interrupt        30 */
#define CSR20_SFIRQ	0x20000000	/* SYSFAIL interrupt       29 */
#define CSR20_BERIRQ	0x10000000	/* BERR interrupt          28 */
#define CSR20_IAKIRQ	0x08000000	/* IAK interrupt           27 */
#define CSR20_SRQIRQ	0x04000000	/* SRQ interrupt           26 */
#define CSR20_SAKIRQ	0x02000000	/* SAKF interrupt          25 */

#define CSR20_GP7IRQ	0x00800000	/* GP7 interrupt           23 */
#define CSR20_DMAIRQ	0x00400000	/* DMA interrupt           22 */
#define CSR20_TT1IRQ	0x00200000	/* TT1 interrupt           21 */
#define CSR20_TT0IRQ	0x00100000	/* TT0 interrupt           20 */
#define CSR20_GP3IRQ	0x00080000	/* GP3 interrupt           19 */
#define CSR20_GP2IRQ	0x00040000	/* GP2 interrupt           18 */
#define CSR20_GP1IRQ	0x00020000	/* GP1 interrupt           17 */
#define CSR20_GP0IRQ	0x00010000	/* GP0 interrupt           16 */

#define CSR20_SWIRQ7	0x00008000	/* SWIRQ7 interrupt        15 */
#define CSR20_SWIRQ6	0x00004000	/* SWIRQ6 interrupt        14 */
#define CSR20_SWIRQ5	0x00002000	/* SWIRQ5 interrupt        13 */
#define CSR20_SWIRQ4	0x00001000	/* SWIRQ4 interrupt        12 */
#define CSR20_SWIRQ3	0x00000800	/* SWIRQ3 interrupt        11 */
#define CSR20_SWIRQ2	0x00000400	/* SWIRQ2 interrupt        10 */
#define CSR20_SWIRQ1	0x00000200	/* SWIRQ1 interrupt        9 */
#define CSR20_SWIRQ0	0x00000100	/* SWIRQ0 interrupt        8 */

#define CSR20_VMIRQ7	0x00000080	/* VMIRQ7 interrupt        7 */
#define CSR20_VMIRQ6	0x00000040	/* VMIRQ7 interrupt        6 */
#define CSR20_VMIRQ5	0x00000020	/* VMIRQ7 interrupt        5 */
#define CSR20_VMIRQ4	0x00000010	/* VMIRQ4 interrupt        4 */
#define CSR20_VMIRQ3	0x00000008	/* VMIRQ3 interrupt        3 */
#define CSR20_VMIRQ2	0x00000004	/* VMIRQ2 interrupt        2 */
#define CSR20_VMIRQ1	0x00000002	/* VMIRQ1 interrupt        1 */

/*************************************************************************
* CSR21                 0x14    0x50    interrupt enable                 *
*************************************************************************/

#define CSR21_ACFIEN	0x80000000	/* ACFAIL interrupt           31 */
#define CSR21_ABTIEN	0x40000000	/* ABORT  interrupt           30 */
#define CSR21_SFIEN	0x20000000	/* SYSFAIL interrupt          29 */
#define CSR21_BERIEN	0x10000000	/* BERR interrupt             28 */
#define CSR21_IAKIEN	0x08000000	/* IAK interrupt              27 */
#define CSR21_SRQIEN	0x04000000	/* SRQ interrupt              26 */
#define CSR21_SAKIEN	0x02000000	/* SAKF interrupt             25 */

#define CSR21_GP7IEN	0x00800000	/* GP7 interrupt              23 */
#define CSR21_DMAIEN	0x00400000	/* DMA interrupt              22 */
#define CSR21_TT1IEN	0x00200000	/* TT1 interrupt              21 */
#define CSR21_TT0IEN	0x00100000	/* TT0 interrupt              20 */
#define CSR21_GP3IEN	0x00080000	/* GP3 interrupt              19 */
#define CSR21_GP2IEN	0x00040000	/* GP2 interrupt              18 */
#define CSR21_GP1IEN	0x00020000	/* GP1 interrupt              17 */
#define CSR21_GP0IEN	0x00010000	/* GP0 interrupt              16 */

#define CSR21_SWIEN7	0x00008000	/* SWIRQ7 interrupt           15 */
#define CSR21_SWIEN6	0x00004000	/* SWIRQ6 interrupt           14 */
#define CSR21_SWIEN5	0x00002000	/* SWIRQ5 interrupt           13 */
#define CSR21_SWIEN4	0x00001000	/* SWIRQ4 interrupt           12 */
#define CSR21_SWIEN3	0x00000800	/* SWIRQ3 interrupt           11 */
#define CSR21_SWIEN2	0x00000400	/* SWIRQ2 interrupt           10 */
#define CSR21_SWIEN1	0x00000200	/* SWIRQ1 interrupt           9 */
#define CSR21_SWIEN0	0x00000100	/* SWIRQ0 interrupt           8 */

#define CSR21_VMIEN7	0x00000080	/* VMIRQ7 interrupt           7 */
#define CSR21_VMIEN6	0x00000040	/* VMIRQ7 interrupt           6 */
#define CSR21_VMIEN5	0x00000020	/* VMIRQ7 interrupt           5 */
#define CSR21_VMIEN4	0x00000010	/* VMIRQ4 interrupt           4 */
#define CSR21_VMIEN3	0x00000008	/* VMIRQ3 interrupt           3 */
#define CSR21_VMIEN2	0x00000004	/* VMIRQ2 interrupt           2 */
#define CSR21_VMIEN1	0x00000002	/* VMIRQ1 interrupt           1 */

/*************************************************************************
* CSR23                 0x16    0x5c    interrupt clear                  *
*************************************************************************/

#define CSR23_ACFICL	0x80000000	/* ACFAIL clear              31 */
#define CSR23_ABTICL	0x40000000	/* ABORT  clear              30 */
#define CSR23_SFICL	0x20000000	/* SYSFAIL clear             29 */
#define CSR23_IAKICL	0x08000000	/* IAK clear                 27 */
#define CSR23_SRQICL	0x04000000	/* SRQ clear                 26 */
#define CSR23_SAKICL	0x02000000	/* SAK clear                 25 */
#define CSR23_DMAICL	0x00400000	/* DMA clear                 22 */
#define CSR23_TT1ICL	0x00200000	/* TT1 clear                 21 */
#define CSR23_TT0ICL	0x00100000	/* TT0 clear                 20 */

/*************************************************************************
* IFR0                  0x1c    0x70    interface register 0             *
*************************************************************************/

#define IFR0_SRQF    0x80000000	/* SRQ clear                         31 */

/*************************************************************************
* IFR1                  0x1d    0x74    interface register 1             *
*************************************************************************/

#define IFR1_SAKF    0x80000000	/* SAK clear                         31 */

/*************************************************************************
* IFR3                  0x1f    0x7c    interface register 3             *
*************************************************************************/

#define IFR3_SWREN   0x80000000	/* software reset enable bit         31 */

#ifdef __cplusplus
}
#endif

#endif /* INCdga001Vmeh */
