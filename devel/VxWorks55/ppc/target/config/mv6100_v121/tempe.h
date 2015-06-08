/* tempe.h - Register definitions for Tempe PCI-X to VMEbus interface chip */

/* Copyright 2004, Motorola Inc. All Rights Reserved */

/*
modification history
--------------------
01g,10aug04,scb Add error handling module support.
01f,21apr04,scb Some busTas support to go along with RMW.
01e,06apr04,scb RMW support.
01d,17mar04,scb Tempe DMA support.
01c,27feb04,scb Fix diab compiler warnings.
01b,17sep03,scb Register init macros, minor tweek for register name changes.
01a,20aug03,scb Initial writing.
*/

/* 
This file contains all of the register definitions for the Tempe PCI-X to
VMEbus interface chip.  The register definitions are divided into sections
and specified with names that match the Engineering Specification for this
chip (version 0.6, 11 July 2003).  All register offsets are on 4-byte 
boundaries, even though some of the registers contain multiple embedded 
8-bit fields which are indepently named.

In addition the file contains chip access and initialization support macros .
*/

#ifndef __INCtempeh
#define __INCtempeh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* 
 * PCI Configuration:
 *   Header
 */

#define TEMPE_DEVI_VENI			0x000
#define TEMPE_STAT_CMMD			0x004
#define TEMPE_CLAS_REVI			0x008
#define TEMPE_HEAD_MLAT_CLSZ		0x00C
#define TEMPE_MBARL			0x010
#define TEMPE_MBARU			0x014
#define TEMPE_SUBI_SUBV			0x02C
#define TEMPE_CAPP			0x034
#define TEMPE_MXLA_MNGN_INTP_INTL	0x03C

/* 
 * PCI Configuration:
 *   PCI-X Capabilities
 */

#define TEMPE_PCIXCAP			0x040
#define TEMPE_PCIXSTAT			0x044

/* 
 * Outbound Functions:
 *   Outbound Translation 0
 */

#define TEMPE_OTSAU0			0x100
#define TEMPE_OTSAL0			0x104
#define TEMPE_OTEAU0			0x108
#define TEMPE_OTEAL0			0x10C
#define TEMPE_OTOFU0			0x110
#define TEMPE_OTOFL0			0x114
#define TEMPE_OTBS0			0x118
#define TEMPE_OTAT0			0x11C

/* 
 * Outbound Functions:
 *   Outbound Translation 1
 */

#define TEMPE_OTSAU1			0x120
#define TEMPE_OTSAL1			0x124
#define TEMPE_OTEAU1			0x128
#define TEMPE_OTEAL1			0x12C
#define TEMPE_OTOFU1			0x130
#define TEMPE_OTOFL1			0x134
#define TEMPE_OTBS1			0x138
#define TEMPE_OTAT1			0x13C

/* 
 * Outbound Functions:
 *   Outbound Translation 2
 */

#define TEMPE_OTSAU2			0x140
#define TEMPE_OTSAL2			0x144
#define TEMPE_OTEAU2			0x148
#define TEMPE_OTEAL2			0x14C
#define TEMPE_OTOFU2			0x150
#define TEMPE_OTOFL2			0x154
#define TEMPE_OTBS2			0x158
#define TEMPE_OTAT2			0x15C

/* 
 * Outbound Functions:
 *   Outbound Translation 3
 */

#define TEMPE_OTSAU3			0x160
#define TEMPE_OTSAL3			0x164
#define TEMPE_OTEAU3			0x168
#define TEMPE_OTEAL3			0x16C
#define TEMPE_OTOFU3			0x170
#define TEMPE_OTOFL3			0x174
#define TEMPE_OTBS3			0x178
#define TEMPE_OTAT3			0x17C

/* 
 * Outbound Functions:
 *   Outbound Translation 4
 */

#define TEMPE_OTSAU4			0x180
#define TEMPE_OTSAL4			0x184
#define TEMPE_OTEAU4			0x188
#define TEMPE_OTEAL4			0x18C
#define TEMPE_OTOFU4			0x190
#define TEMPE_OTOFL4			0x194
#define TEMPE_OTBS4			0x198
#define TEMPE_OTAT4			0x19C

/* 
 * Outbound Functions:
 *   Outbound Translation 5
 */

#define TEMPE_OTSAU5			0x1A0
#define TEMPE_OTSAL5			0x1A4
#define TEMPE_OTEAU5			0x1A8
#define TEMPE_OTEAL5			0x1AC
#define TEMPE_OTOFU5			0x1B0
#define TEMPE_OTOFL5			0x1B4
#define TEMPE_OTBS5			0x1B8
#define TEMPE_OTAT5			0x1BC

/* 
 * Outbound Functions:
 *   Outbound Translation 6
 */

#define TEMPE_OTSAU6			0x1C0
#define TEMPE_OTSAL6			0x1C4
#define TEMPE_OTEAU6			0x1C8
#define TEMPE_OTEAL6			0x1CC
#define TEMPE_OTOFU6			0x1D0
#define TEMPE_OTOFL6			0x1D4
#define TEMPE_OTBS6			0x1D8
#define TEMPE_OTAT6			0x1DC

/* 
 * Outbound Functions:
 *   Outbound Translation 7
 */

#define TEMPE_OTSAU7			0x1E0
#define TEMPE_OTSAL7			0x1E4
#define TEMPE_OTEAU7			0x1E8
#define TEMPE_OTEAL7			0x1EC
#define TEMPE_OTOFU7			0x1F0
#define TEMPE_OTOFL7			0x1F4
#define TEMPE_OTBS7			0x1F8
#define TEMPE_OTAT7			0x1FC

/* 
 * Outbound Functions:
 *   VMEbus Interrupt Acknowledge
 */

#define TEMPE_VIACK1			0x204
#define TEMPE_VIACK2			0x208
#define TEMPE_VIACK3			0x20C
#define TEMPE_VIACK4			0x210
#define TEMPE_VIACK5			0x214
#define TEMPE_VIACK6			0x218
#define TEMPE_VIACK7			0x21C

/* 
 * Outbound Functions:
 *   RMW
 */

#define TEMPE_RMWAU			0x220
#define TEMPE_RMWAL			0x224
#define TEMPE_RMWEN			0x228
#define TEMPE_RMWC			0x22C
#define TEMPE_RMWS			0x230

/* 
 * Outbound Functions:
 *   VMEbus Control
 */

#define TEMPE_VMCTRL			0x234
#define TEMPE_VCTRL			0x238
#define TEMPE_VSTAT			0x23C

/* 
 * Outbound Functions:
 *   PCI control/status
 */

#define TEMPE_PSTAT 			0x240

/* 
 * VME Filters:
 *   VME Filter
 */

#define TEMPE_VMEFL			0x250

/* 
 * VME Exception:
 *   VME Exception Status
 */

#define TEMPE_VEAU			0x260
#define TEMPE_VEAL			0x264
#define TEMPE_VEAT			0x268

/* 
 * PCI Error:
 *   PCI Error Status
 */

#define TEMPE_EDPAU			0x270
#define TEMPE_EDPAL			0x274
#define TEMPE_EDPXA			0x278
#define TEMPE_EDPXS			0x27C
#define TEMPE_EDPAT			0x280

/* 
 * Inbound Functions:
 *   Inbound Translation 0
 */

#define TEMPE_ITSAU0			0x300
#define TEMPE_ITSAL0			0x304
#define TEMPE_ITEAU0			0x308
#define TEMPE_ITEAL0			0x30C
#define TEMPE_ITOFU0			0x310
#define TEMPE_ITOFL0			0x314
#define TEMPE_ITAT0			0x318

/* 
 * Inbound Functions:
 *   Inbound Translation 1
 */

#define TEMPE_ITSAU1			0x320
#define TEMPE_ITSAL1			0x324
#define TEMPE_ITEAU1			0x328
#define TEMPE_ITEAL1			0x32C
#define TEMPE_ITOFU1			0x330
#define TEMPE_ITOFL1			0x334
#define TEMPE_ITAT1			0x338

/* 
 * Inbound Functions:
 *   Inbound Translation 2
 */

#define TEMPE_ITSAU2			0x340
#define TEMPE_ITSAL2			0x344
#define TEMPE_ITEAU2			0x348
#define TEMPE_ITEAL2			0x34C
#define TEMPE_ITOFU2			0x350
#define TEMPE_ITOFL2			0x354
#define TEMPE_ITAT2			0x358

/* 
 * Inbound Functions:
 *   Inbound Translation 3
 */

#define TEMPE_ITSAU3			0x360
#define TEMPE_ITSAL3			0x364
#define TEMPE_ITEAU3			0x368
#define TEMPE_ITEAL3			0x36C
#define TEMPE_ITOFU3			0x370
#define TEMPE_ITOFL3			0x374
#define TEMPE_ITAT3			0x378

/* 
 * Inbound Functions:
 *   Inbound Translation 4
 */

#define TEMPE_ITSAU4			0x380
#define TEMPE_ITSAL4			0x384
#define TEMPE_ITEAU4			0x388
#define TEMPE_ITEAL4			0x38C
#define TEMPE_ITOFU4			0x390
#define TEMPE_ITOFL4			0x394
#define TEMPE_ITAT4			0x398

/* 
 * Inbound Functions:
 *   Inbound Translation 5
 */

#define TEMPE_ITSAU5			0x3A0
#define TEMPE_ITSAL5			0x3A4
#define TEMPE_ITEAU5			0x3A8
#define TEMPE_ITEAL5			0x3AC
#define TEMPE_ITOFU5			0x3B0
#define TEMPE_ITOFL5			0x3B4
#define TEMPE_ITAT5			0x3B8

/* 
 * Inbound Functions:
 *   Inbound Translation 6
 */

#define TEMPE_ITSAU6			0x3C0
#define TEMPE_ITSAL6			0x3C4
#define TEMPE_ITEAU6			0x3C8
#define TEMPE_ITEAL6			0x3CC
#define TEMPE_ITOFU6			0x3D0
#define TEMPE_ITOFL6			0x3D4
#define TEMPE_ITAT6			0x3D8

/* 
 * Inbound Functions:
 *   Inbound Translation 7
 */

#define TEMPE_ITSAU7			0x3E0
#define TEMPE_ITSAL7			0x3E4
#define TEMPE_ITEAU7			0x3E8
#define TEMPE_ITEAL7			0x3EC
#define TEMPE_ITOFU7			0x3F0
#define TEMPE_ITOFL7			0x3F4
#define TEMPE_ITAT7			0x3F8

/* 
 * Inbound Functions:
 *   Inbound Translation GCSR
 */

#define TEMPE_GBAU			0x400
#define TEMPE_GBAL			0x404
#define TEMPE_GCSRAT			0x408

/* 
 * Inbound Functions:
 *   Inbound Translation CRG
 */

#define TEMPE_CBAU			0x40C
#define TEMPE_CBAL			0x410
#define TEMPE_CSRAT			0x414

/* 
 * Inbound Functions:
 *   Inbound Translation CR/CSR
 */

#define TEMPE_CROU			0x418
#define TEMPE_CROL			0x41C
#define TEMPE_CRAT			0x420

/* 
 * Inbound Functions:
 *   Inbound Translation Location Monitor
 */

#define TEMPE_LMBAU			0x424
#define TEMPE_LMBAL			0x428
#define TEMPE_LMAT			0x42C

/* 
 * Interrupt Functions:
 *   VMEbus Interrupt Control
 */

#define TEMPE_64BCU			0x430
#define TEMPE_64BCL			0x434
#define TEMPE_BPGTR			0x438
#define TEMPE_BPCTR			0x43C
#define TEMPE_VICR			0x440

/* 
 * Interrupt Functions:
 *   Local Bus Interrupt Control
 */

#define TEMPE_INTEN			0x448
#define TEMPE_INTEO			0x44C
#define TEMPE_INTS			0x450
#define TEMPE_INTC			0x454
#define TEMPE_INTM1			0x458
#define TEMPE_INTM2			0x45C

/* 
 * DMA Controller:
 *   DMA Controller 0
 */

#define TEMPE_DCTL0			0x500
#define TEMPE_DSTA0			0x504
#define TEMPE_DCSAU0			0x508
#define TEMPE_DCSAL0			0x50C
#define TEMPE_DCDAU0			0x510
#define TEMPE_DCDAL0			0x514
#define TEMPE_DCLAU0			0x518
#define TEMPE_DCLAL0			0x51C
#define TEMPE_DSAU0			0x520
#define TEMPE_DSAL0			0x524
#define TEMPE_DDAU0			0x528
#define TEMPE_DDAL0			0x52C
#define TEMPE_DSAT0			0x530
#define TEMPE_DDAT0			0x534
#define TEMPE_DNLAU0			0x538
#define TEMPE_DNLAL0			0x53C
#define TEMPE_DCNT0			0x540
#define TEMPE_DDBS0			0x544

/* 
 * DMA Controller:
 *   DMA Controller 1
 */

#define TEMPE_DCTL1			0x580
#define TEMPE_DSTA1			0x584
#define TEMPE_DCSAU1			0x588
#define TEMPE_DCSAL1			0x58C
#define TEMPE_DCDAU1			0x590
#define TEMPE_DCDAL1			0x594
#define TEMPE_DCLAU1			0x598
#define TEMPE_DCLAL1			0x59C
#define TEMPE_DSAU1			0x5A0
#define TEMPE_DSAL1			0x5A4
#define TEMPE_DDAU1			0x5A8
#define TEMPE_DDAL1			0x5AC
#define TEMPE_DSAT1			0x5B0
#define TEMPE_DDAT1			0x5B4
#define TEMPE_DNLAU1			0x5B8
#define TEMPE_DNLAL1			0x5BC
#define TEMPE_DCNT1			0x5C0
#define TEMPE_DDBS1			0x5C4

/*
 * GCSR Register Group
 */

#define TEMPE_GCSR_DEVI_VENI		0x600
#define TEMPE_GCSR_STAT_CMMD		0x604
#define TEMPE_GCSR_SRMAR0_3		0x608
#define TEMPE_GCSR_SRMAR4_7		0x60C
#define TEMPE_GCSR_MBOX0		0x610
#define TEMPE_GCSR_MBOX1		0x614
#define TEMPE_GCSR_MBOX2		0x618
#define TEMPE_GCSR_MBOX3		0x61C

/*
 * CR/CSR Register Group
 */

#define TEMPE_CSR_CSRBCR		0xFF4
#define TEMPE_CSR_CSRBSR		0xFF8
#define TEMPE_CSR_CBAR			0xFFC

/* Various bit fields in the Tempe PCI-VME bridge */

/* Error bits in the status and command register (004) */

#define TEMPE_STAT_CMMD_DPE_BIT		(31)
#define TEMPE_STAT_CMMD_DPE_MASK	(1<<TEMPE_STAT_CMMD_DPE_BIT)

#define TEMPE_STAT_CMMD_SIGSE_BIT	(30)
#define TEMPE_STAT_CMMD_SIGSE_MASK	(1<<TEMPE_STAT_CMMD_SIGSE_BIT)

#define TEMPE_STAT_CMMD_RCVMA_BIT	(29)
#define TEMPE_STAT_CMMD_RCVMA_MASK	(1<<TEMPE_STAT_CMMD_RCVMA_BIT)

#define TEMPE_STAT_CMMD_RCVTA_BIT	(28)
#define TEMPE_STAT_CMMD_RCVTA_MASK	(1<<TEMPE_STAT_CMMD_SIGTA_BIT)

#define TEMPE_STAT_CMND_DPED_BIT	(24)
#define TEMPE_STAT_CMND_DPED_MASK	(1<<TEMPE_STAT_CMND_DPED_BIT)

/* Error bits in the PCI-X status register (044) */

#define TEMPE_PCIXSTAT_RSCEM_BIT	(29)
#define TEMPE_PCIXSTAT_RSCEM_MASK	(1<<TEMPE_PCIXSTAT_RSCEM_BIT)

#define TEMPE_PCIXSTAT_USC_BIT		(19)
#define TEMPE_PCIXSTAT_USC_MASK		(1<<TEMPE_PCIXSTAT_USC_BIT)

#define TEMPE_PCIXSTAT_SCD_BIT		(18)
#define TEMPE_PCIXSTAT_SCD_MASK		(1<<TEMPE_PCIXSTAT_SCD_BIT)

/* Error bits in the PCI control/status register (240) */

#define TEMPE_PSTAT_SRTE_BIT		(10)
#define TEMPE_PSTAT_SRTE_MASK		(1<<TEMPE_PSTAT_SRTE_BIT)

#define TEMPE_PSTAT_DTTE_BIT		(9)
#define TEMPE_PSTAT_DTTE_MASK		(1<<TEMPE_PSTAT_DTTE_BIT)

#define TEMPE_PSTAT_MRCE_BIT		(8)
#define TEMPE_PSTAT_MRCE_MASK		(1<<TEMPE_PSTAT_MRCE_BIT)

/* Error bits in the VMEbus exception attributes register (268) */

#define TEMPE_VEAT_VES_BIT		(31)
#define TEMPE_VEAT_VES_MASK		(1<<TEMPE_VEAT_VES_BIT)

#define TEMPE_VEAT_VEOF_BIT		(30)
#define TEMPE_VEAT_VEOF_MASK		(1<<TEMPE_VEAT_VEOL_BIT)

#define TEMPE_VEAT_VESCL_BIT		(29)
#define TEMPE_VEAT_VESCL_MASK		(1<<TEMPE_VEAT_VESCL_BIT)

#define TEMPE_VEAT_2EOT_BIT		(21)
#define TEMPE_VEAT_2EOT_MASK		(1<<TEMPE_VEAT_2EOT_BIT)

#define TEMPE_VEAT_2EST_BIT		(20)
#define TEMPE_VEAT_2EST_MASK		(1<<TEMPE_VEAT_2EST_BIT)

#define TEMPE_VEAT_BERR_BIT		(19)
#define TEMPE_VEAT_BERR_MASK		(1<<TEMPE_VEAT_BERR_BIT)

/* Error bits in the Error Diagnostics PCI Attribute register (280) */

#define TEMPE_EDPAT_EDPST_BIT		(31)
#define TEMPE_EDPAT_EDPST_MASK		(1<<TEMPE_EDPAT_EDPST_BIT)

#define TEMPE_EDPAT_EDPOF_BIT		(30)
#define TEMPE_EDPAT_EDPOF_MASK		(1<<TEMPE_EDPAT_EDPOF_BIT)

#define TEMPE_EDPAT_EDPCL_BIT		(29)
#define TEMPE_EDPAT_EDPCL_MASK		(1<<TEMPE_EDPAT_EDPCL_BIT)

#define TEMPE_EDPAT_SCD_BIT		(17)
#define TEMPE_EDPAT_SCD_MASK		(1<<TEMPE_EDPAT_SCD_BIT )

#define TEMPE_EDPAT_USC_BIT		(16)
#define TEMPE_EDPAT_USC_MASK		(1<<TEMPE_EDPAT_USC_BIT)

#define TEMPE_EDPAT_SRT_BIT		(15)
#define TEMPE_EDPAT_SRT_MASK		(1<<TEMPE_EDPAT_SRT_BIT)

#define TEMPE_EDPAT_SCEM_BIT		(14)
#define TEMPE_EDPAT_SCEM_MASK		(1<<TEMPE_EDPAT_SCEM_BIT)

#define TEMPE_EDPAT_DPED_BIT		(13)
#define TEMPE_EDPAT_DPED_MASK		(1<<TEMPE_EDPAT_DPED_BIT)

#define TEMPE_EDPAT_DPE_BIT		(12)
#define TEMPE_EDPAT_DPE_MASK		(1<<TEMPE_EDPAT_DPE_BIT)

#define TEMPE_EDPAT_MRC_BIT		(11)
#define TEMPE_EDPAT_MRC_MASK		(1<<TEMPE_EDPAT_MRC_BIT)

#define TEMPE_EDPAT_RMA_BIT		(10)
#define TEMPE_EDPAT_RMA_MASK		(1<<TEMPE_EDPAT_RMA_BIT)

#define TEMPE_EDPAT_RTA_BIT		(9)
#define TEMPE_EDPAT_RTA_MASK		(1<<TEMPE_EDPAT_RTA_BIT)

#define TEMPE_EDPAT_DTT_BIT		(8)
#define TEMPE_EDPAT_DTT_MASK		(1<<TEMPE_EDPAT_DTT_BIT)

/* Error bits in the Interrupt Status register (450) */

#define TEMPE_INTS_PERRS_BIT		(13)
#define TEMPE_INTS_PERRS_MASK		(1<<TEMPE_INTS_PERRS_BIT)

#define TEMPE_INTS_VERRS_BIT		(12)
#define TEMPE_INTS_VERRS_MASK		(1<<TEMPE_INTS_VERRS_BIT)

/* Error bits in the Interrupt Clear register (454) */

#define TEMPE_INTC_PERRC_BIT		(13)
#define TEMPE_INTC_PERRC_MASK		(1<<TEMPE_INTC_PERRC_BIT)

#define TEMPE_INTC_VERRC_BIT		(12)
#define TEMPE_INTC_VERRC_MASK		(1<<TEMPE_INTC_VERRC_BIT)

/* Outbound windowing related */

#define TEMPE_OTATLx_STAL_MASK		(0xffff0000)
#define TEMPE_OTEALx_ENDL_MASK		(0xffff0000)
#define TEMPE_OTOFLx_OFFL_MASK		(0xffff0000)
#define TEMPE_OTOFLx_2eBS_MASK		(0x000fffff)

#define TEMPE_OTATx_EN_BIT		(31)
#define TEMPE_OTATx_EN_MASK		(1<<TEMPE_OTATx_EN_BIT)
#define TEMPE_OTATx_EN_VAL_ENABLED	(1<<TEMPE_OTATx_EN_BIT)

#define TEMPE_OTATx_MRPFD_BIT		(18)
#define TEMPE_OTATx_MRPFD_MASK		(1<<18)

#define TEMPE_OTATx_PFSx_BIT		(16)
#define TEMPE_OTATx_PFSx_MASK		(3<<TEMPE_OTATx_PFS1_BIT)

#define TEMPE_OTATx_2eSSTMx_BIT		(11)
#define TEMPE_OTATx_2eSSTMx_MASK	(7<<TEMPE_OTATx_2eSSTMx_BIT)

#define TEMPE_OTATx_TMx_BIT		(8)
#define TEMPE_OTATx_TMx_MASK		(7<<TEMPE_OTATx_TMx_BIT)

#define TEMPE_OTATx_DBWx_BIT		(6)
#define TEMPE_OTATx_DBWx_MASK		(3<<TEMPE_OTATx_DBWx_BIT)

#define TEMPE_OTATx_SUP_BIT		(5)
#define TEMPE_OTATx_SUP_MASK		(1<<TEMPE_OTATx_SUP_BIT)

#define TEMPE_OTATx_PGM_BIT		(4)
#define TEMPE_OTATx_PGM_MASK		(1<<TEMPE_OTATx_PGM_BIT)

#define TEMPE_OTATx_AMODEx_BIT		(0)
#define TEMPE_OTATx_AMODEx_MASK		(0xf<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_A16	(0x0<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_A24	(0x1<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_A32	(0x2<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_A64	(0x4<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_CSR	(0x5<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_USER1	(0x8<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_USER2	(0x9<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_USER3	(0xa<<TEMPE_OTATx_AMODEx_BIT)
#define TEMPE_OTATx_AMODE_VAL_USER4	(0xb<<TEMPE_OTATx_AMODEx_BIT)

/* Inbound windowing related */

#define TEMPE_ITATLx_STAL_MASK		(0xfffffff0)
#define TEMPE_ITEALx_ENDL_MASK		(0xfffffff0)
#define TEMPE_ITOFLx_OFFL_MASK		(0xfffffff0)

#define TEMPE_ITATx_EN_BIT		(31)
#define TEMPE_ITATx_EN_MASK		(1<<TEMPE_ITATx_EN_BIT)
#define TEMPE_ITATx_EN_VAL_ENABLED	(1<<TEMPE_ITATx_EN_BIT)

#define TEMPE_ITATx_TH_BIT		(18)
#define TEMPE_ITATx_TH_MASK		(1<<TEMPE_ITATx_TH_MASK)

#define TEMPE_ITATx_VFSx_BIT		(16)
#define TEMPE_ITATx_VFSx_MASK		(3<<TEMPE_ITATx_VFSx_MASK)

#define TEMPE_ITATx_2eSSTMx_BIT		(12)
#define TEMPE_ITATx_2eSSTMx_MASK	(7<<TEMPE_ITATx_2eSSTMx_MASK)

#define TEMPE_ITATx_PRTCL_BIT		(7)
#define TEMPE_ITATx_PRTCL_MASK	        (0x1f<<TEMPE_ITATx_PRTCL_MASK)

/* Breakdown of TEMPE_ITATx_PRTCL follows (5 bits) */

#define TEMPE_ITATx_2eSSTB_BIT		(11)
#define TEMPE_ITATx_2eSSTB_MASK	        (1<<TEMPE_ITATx_2eSSTB_MASK)

#define TEMPE_ITATx_2eSST_BIT		(10)
#define TEMPE_ITATx_2eSST_MASK	        (1<<TEMPE_ITATx_2eSST_MASK)

#define TEMPE_ITATx_2eVME_BIT		(9)
#define TEMPE_ITATx_2eVME_MASK	        (1<<TEMPE_ITATx_2eVME_MASK)

#define TEMPE_ITATx_MBLT_BIT		(8)
#define TEMPE_ITATx_MBLT_MASK	        (1<<TEMPE_ITATx_MBLT_MASK)

#define TEMPE_ITATx_BLT_BIT		(7)
#define TEMPE_ITATx_BLT_MASK	        (1<<TEMPE_ITATx_2eSSTB_MASK)

#define TEMPE_ITATx_ASx_BIT		(4)
#define TEMPE_ITATx_ASx_MASK	        (7<<TEMPE_ITATx_ASx_BIT)
#define TEMPE_ITATx_ASx_VAL_A16		(0<<TEMPE_ITATx_ASx_BIT)
#define TEMPE_ITATx_ASx_VAL_A24		(1<<TEMPE_ITATx_ASx_BIT)
#define TEMPE_ITATx_ASx_VAL_A32		(2<<TEMPE_ITATx_ASx_BIT)
#define TEMPE_ITATx_ASx_VAL_A64		(4<<TEMPE_ITATx_ASx_BIT)

#define TEMPE_ITATx_SUPR_BIT		(3)
#define TEMPE_ITATx_SUPR_MASK	        (1<<TEMPE_ITATx_SUPR_MASK)

#define TEMPE_ITATx_NPRIV_BIT		(2)
#define TEMPE_ITATx_NPRIV_MASK	        (1<<TEMPE_ITATx_NPRIV_MASK)

#define TEMPE_ITATx_PGM_BIT		(1)
#define TEMPE_ITATx_PGM_MASK	        (1<<TEMPE_ITATx_PGM_MASK)

#define TEMPE_ITATx_DATA_BIT		(0)
#define TEMPE_ITATx_DATA_MASK	        (1<<TEMPE_ITATx_DATA_MASK)

#define TEMPE_VMCTRL_RMWEN_BIT		(20)
#define TEMPE_VMCTRL_RMWEN_MASK		(1<<TEMPE_VMCTRL_RMWEN_BIT)		

#define TEMPE_VMCTRL_DWB_BIT		(24)
#define TEMPE_VMCTRL_DHB_MASK		(1<<TEMPE_VMCTRL_DHB_BIT)		

#define TEMPE_VMCTRL_DHB_BIT		(25)
#define TEMPE_VMCTRL_DWB_MASK		(1<<TEMPE_VMCTRL_DWB_BIT)		

#define TEMPE_VMCTRL_VS_BIT		(26)
#define TEMPE_VMCTRL_VS_MASK		(1<<TEMPE_VMCTRL_VS_BIT)		

#define TEMPE_VMCTRL_VSA_BIT		(27)
#define TEMPE_VMCTRL_VSA_MASK		(1<<TEMPE_VMCTRL_VSA_BIT)		


/* Define Tempe Interrupt related bit positions */

#define TEMPE_INT_NUM_IRQ1_BIT		1
#define TEMPE_INT_NUM_IRQ2_BIT		2
#define TEMPE_INT_NUM_IRQ3_BIT		3
#define TEMPE_INT_NUM_IRQ4_BIT		4
#define TEMPE_INT_NUM_IRQ5_BIT		5
#define TEMPE_INT_NUM_IRQ6_BIT		6
#define TEMPE_INT_NUM_IRQ7_BIT		7
#define TEMPE_INT_NUM_ACFL_BIT		8
#define TEMPE_INT_NUM_SYSFL_BIT		9
#define TEMPE_INT_NUM_IACK_BIT		10
#define TEMPE_INT_NUM_VIEEN_BIT		11
#define TEMPE_INT_NUM_VERR_BIT		12
#define TEMPE_INT_NUM_PERR_BIT		13
#define TEMPE_INT_NUM_MB0_BIT		16
#define TEMPE_INT_NUM_MB1_BIT		17
#define TEMPE_INT_NUM_MB2_BIT		18
#define TEMPE_INT_NUM_MB3_BIT		19
#define TEMPE_INT_NUM_LM0_BIT		20
#define TEMPE_INT_NUM_LM1_BIT		21
#define TEMPE_INT_NUM_LM2_BIT		22
#define TEMPE_INT_NUM_LM3_BIT		23
#define TEMPE_INT_NUM_DMA0_BIT		24
#define TEMPE_INT_NUM_DMA1_BIT		25

#define TEMPE_INTEN_INIT_VAL	( (1 << TEMPE_INT_NUM_IRQ1_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ2_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ3_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ4_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ5_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ6_BIT) | \
                                  (1 << TEMPE_INT_NUM_IRQ7_BIT) | \
                                  (1 << TEMPE_INT_NUM_ACFL_BIT) | \
                                  (1 << TEMPE_INT_NUM_SYSFL_BIT) | \
                                  (1 << TEMPE_INT_NUM_IACK_BIT) | \
                                  (1 << TEMPE_INT_NUM_VIEEN_BIT) | \
                                  (1 << TEMPE_INT_NUM_VERR_BIT) | \
                                  (1 << TEMPE_INT_NUM_PERR_BIT) | \
                                  (1 << TEMPE_INT_NUM_MB0_BIT) | \
                                  (1 << TEMPE_INT_NUM_MB1_BIT) | \
                                  (1 << TEMPE_INT_NUM_MB2_BIT) | \
                                  (1 << TEMPE_INT_NUM_MB3_BIT) | \
                                  (1 << TEMPE_INT_NUM_LM0_BIT) | \
                                  (1 << TEMPE_INT_NUM_LM1_BIT) | \
                                  (1 << TEMPE_INT_NUM_LM2_BIT) | \
                                  (1 << TEMPE_INT_NUM_LM3_BIT) | \
                                  (1 << TEMPE_INT_NUM_DMA0_BIT) | \
                                  (1 << TEMPE_INT_NUM_DMA1_BIT) )

#define TEMPE_VICR_STID_BIT	(0)
#define TEMPE_VICR_STID_MASK	(0xff<<TEMPE_VICR_STID_BIT)

#define TEMPE_VICR_IRQS_BIT	(11)
#define TEMPE_VICR_IRQS_MASK	(1<<TEMPE_VICR_IRQS_BIT)
#define TEMPE_VICR_IRQL_BIT	(8)

#define TEMPE_VEAT_VES_BIT	(31)
#define TEMPE_VEAT_VES_MASK	(1<<TEMPE_VEAT_VES_BIT)

#define TEMPE_VEAT_VESCL_BIT	(29)
#define TEMPE_VEAT_VESCL_MASK	(1<<TEMPE_VEAT_VESCL_BIT)

#define TEMPE_VEAT_IACK_BIT	(16)
#define TEMPE_VEAT_IACK_MASK	(1<<TEMPE_VEAT_IACK_BIT)

/* DMA control register bits */

#define TEMPE_DCTLx_ABT_BIT	(27)
#define TEMPE_DCTLx_ABT_MASK	(1<<TEMPE_DCTLx_ABT_BIT)

#define TEMPE_DCTLx_PAU_BIT	(26)
#define TEMPE_DCTLx_PAU_MASK	(1<<TEMPE_DCTLx_PAU_BIT)

#define TEMPE_DCTLx_DGO_BIT	(25)
#define TEMPE_DCTLx_DGO_MASK	(1<<TEMPE_DCTLx_DGO_BIT)

#define TEMPE_DCTLx_MOD_BIT	(23)
#define TEMPE_DCTLx_MOD_MASK	(1<<TEMPE_DCTLx_MOD_BIT)

#define TEMPE_DCTLx_VFAR_BIT	(17)
#define TEMPE_DCTLx_VFAR_MASK	(1<<TEMPE_DCTLx_VFAR_BIT)

#define TEMPE_DCTLx_PFAR_BIT	(16)
#define TEMPE_DCTLx_PFAR_MASK	(1<<TEMPE_DCTLx_PFAR_BIT)

#define TEMPE_DCTLx_VBKS_BIT	(12)
#define TEMPE_DCTLx_VBKS_MASK	(7<<TEMPE_DCTLx_VBKS_BIT)

#define TEMPE_DCTLx_VBOT_BIT	(8)
#define TEMPE_DCTLx_VBOT_MASK	(7<<TEMPE_DCTLx_VBOT_BIT)

#define TEMPE_DCTLx_PBKS_BIT	(4)
#define TEMPE_DCTLx_PBKS_MASK	(7<<TEMPE_DCTLx_PBKS_BIT)

#define TEMPE_DCTLx_PBOT_BIT	(0)
#define TEMPE_DCTLx_PBOT_MASK	(7<<TEMPE_DCTLx_PBOT_BIT)

/* DMA status bits */

#define TEMPE_DSTAx_ERR_BIT	(28)
#define TEMPE_DSTAx_ERR_MASK	(1<<TEMPE_DSTAx_ERR_BIT)

#define TEMPE_DSTAx_ABT_BIT	(27)
#define TEMPE_DSTAx_ABT_MASK	(1<<TEMPE_DSTAx_ABT_BIT)

#define TEMPE_DSTAx_PAU_BIT	(26)
#define TEMPE_DSTAx_PAU_MASK	(1<<TEMPE_DSTAx_PAU_BIT)

#define TEMPE_DSTAx_DON_BIT	(25)
#define TEMPE_DSTAx_DON_MASK	(1<<TEMPE_DSTAx_DON_BIT)

#define TEMPE_DSTAx_BSY_BIT	(24)
#define TEMPE_DSTAx_BSY_MASK	(1<<TEMPE_DSTAx_BSY_BIT)

#define TEMPE_DSTAx_ERRS_BIT	(20)
#define TEMPE_DSTAx_ERRS_MASK	(1<<TEMPE_DSTAx_ERRS_BIT)

#define TEMPE_DSTAx_ERT_BIT	(16)
#define TEMPE_DSTAx_ERT_MASK	(3<<TEMPE_DSTAx_ERT_BIT)

/* DMA source attributes bits */

#define TEMPE_DSATx_2eSSTMx_BIT	(11)
#define TEMPE_DSATx_2eSSTMx_MASK (3<<TEMPE_DSATx_2eSSTMx_BIT)

#define TEMPE_DSATx_TMx_BIT	(8)
#define TEMPE_DSATx_TMx_MASK	(7<<TEMPE_DSATx_TMx_BIT)

#define TEMPE_DSATx_DBWx_BIT	(6)
#define TEMPE_DSATx_DBWx_MASK	(3<<TEMPE_DSATx_DBWx_BIT)

#define TEMPE_DSATx_SUP_BIT	(5)
#define TEMPE_DSATx_SUP_MASK	(1<<TEMPE_DSATx_SUP_BIT)

#define TEMPE_DSATx_PGM_BIT	(4)
#define TEMPE_DSATx_PGM_MASK	(1<<TEMPE_DSATx_PGM_BIT)

/* DMA destination attributes bits */

#define TEMPE_DDATx_2eSSTMx_BIT	(11)
#define TEMPE_DDATx_2eSSTMx_MASK (3<<TEMPE_DDATx_2eSSTMx_BIT)

#define TEMPE_DDATx_TMx_BIT	(8)
#define TEMPE_DDATx_TMx_MASK	(7<<TEMPE_DDATx_TMx_BIT)

#define TEMPE_DDATx_DBWx_BIT	(6)
#define TEMPE_DDATx_DBWx_MASK	(3<<TEMPE_DDATx_DBWx_BIT)

#define TEMPE_DDATx_SUP_BIT	(5)
#define TEMPE_DDATx_SUP_MASK	(1<<TEMPE_DDATx_SUP_BIT)

#define TEMPE_DDATx_PGM_BIT	(4)
#define TEMPE_DDATx_PGM_MASK	(1<<TEMPE_DDATx_PGM_BIT)

/* DMA Next Link Address (lower) bits */

#define TEMPE_DNLALx_LLA_BIT	(0)
#define TEMPE_DNLALx_LLA_MASK	(1<<TEMPE_DNLALx_LLA_BIT)

/* Error handling support */

#define TEMPE_ERROR_DOMAIN_CODE	   ERROR_PCI_T0_VME_TYPE
#define TEMPE_ERROR_ASCII_DOMAIN   "Tsi148"
#define TEMPE_ERROR_REGISTER       (* tempeErrorUtils.errorRegister)
#define TEMPE_ERROR_LOG            (* tempeErrorUtils.errorLog)

/*
 * Tempe (Tsi148) error IDs are constructed as follows:
 * High order byte contains 0x03 - the Tempe (Tsi148) error domain
 * identifier.  The next-to-highest byte contains the bit number
 * of a bit which is associated with the error.  The low order
 * two bytes contain the Tempe (Tsi148) register offset of the
 * associated register.  
 *
 * Example:
 *
 * PCI Master Abort is associated the Tempe EDPAT "Error Diagnostic
 * PCI Attributes" register, whose offset is 0x280.  The bit within
 * this register which specifically identifies "PCI Target Abort" is
 * the RMA (Received Master Abort), bit number is 10 (0xa).  So the error
 * code associated with Tsi148 PCI "received master abort" is:
 *
 *  0x03_0A_0280 (0x030A280)
 *     ^  ^    ^
 *     |  |    |
 *     |  |    +--- Offset (0x280) of EDPAT register
 *     |  +--- Bit number of RMA bit inside of EDPAT register
 *     +--- Domain number (0x03) of PCI-VME bridge errors
 */

#define TEMPE_ERROR_ID(offset,bit) (TEMPE_ERROR_DOMAIN_CODE | \
				   (offset & 0xffff) | ((bit & 0xff) << 16))

/* VEAT (VMB Bus Exception Attributes Register) related errors */

#define TEMPE_ERR_VEAT_BERR        TEMPE_ERROR_ID (TEMPE_VEAT,\
					           TEMPE_VEAT_BERR_BIT)

#define TEMPE_ERR_VEAT_2EOT	   TEMPE_ERROR_ID (TEMPE_VEAT,\
						   TEMPE_VEAT_2EOT_BIT)

#define TEMPE_ERR_VEAT_2EST	   TEMPE_ERROR_ID (TEMPE_VEAT,\
						   TEMPE_VEAT_2EST_BIT)

/*
 * If we get a VME Bus Exception Attribute error which we cannot
 * deciper, we will log a huh? error with the following ID.
 */

#define TEMPE_ERR_VEAT_VERR_HUH	   TEMPE_ERROR_ID (TEMPE_VEAT,0xff)

/* EDPAT (Error Diagnostic PCI Attributes Register) related errors */

#define TEMPE_ERR_EDPAT_SCD	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_SCD_BIT) 

#define TEMPE_ERR_EDPAT_USC	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_USC_BIT) 
						   
#define TEMPE_ERR_EDPAT_SRT	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_SRT_BIT) 

#define TEMPE_ERR_EDPAT_SCEM	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_SCEM_BIT) 

#define TEMPE_ERR_EDPAT_DPED	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_DPED_BIT) 

#define TEMPE_ERR_EDPAT_DPE	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_DPE_BIT) 

#define TEMPE_ERR_EDPAT_MRC	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_MRC_BIT) 

#define TEMPE_ERR_EDPAT_RMA	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_RMA_BIT) 

#define TEMPE_ERR_EDPAT_RTA	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_RTA_BIT) 

#define TEMPE_ERR_EDPAT_DTT	   TEMPE_ERROR_ID (TEMPE_EDPAT,\
						   TEMPE_EDPAT_DTT_BIT) 

/*
 * If we get a Diagnostic PCI Exception Attribute error which we cannot
 * deciper, we will log a huh? error with the following ID.
 */

#define TEMPE_ERR_EDPAT_PERR_HUH   TEMPE_ERROR_ID (TEMPE_EDPAT,0xff)

/* For select bus tas HW assist */

#define VME_BUS_LOCK	1
#define VME_BUS_RMW	2
#define VME_TAS_ASSIST	VME_RMW

/* Generic error handling support */

#define TEMPE_ERROR_DOMAIN_ID	(0x03000000)

/* Structure relates interrupt level to bit position in Tempe register */

#ifndef _ASMLANGUAGE

typedef struct 
    {
    int intLvl;			/* intConnect() interrupt level */
    int bit;			/* bit position in register */
    } TEMPE_INTLVL_BIT;

#endif /* _ASMLANGUAGE */

/* FSET32 - set field into a 32-bit word */

#define FSET32(img,val,bitPos) img |= (val << bitPos)

/*
 * Tempe bridge base address is not known until after pciAutoConfig
 * has completed.  The registers are grouped together into the
 * CGR (Combined Register Group), a 4K-byte region which ultimately
 * gets configured into PCI memory space.  The #define below refers
 * to the global variable which will hold this address.  
 */

#define TEMPE_REG_BASE	sysTempeBaseAdrsGet()

/*
 * Macros for reading from and writing to Tempe Internal Registers
 *
 * The Tempe Internal Registers must be read from and written to
 * with endian swapping.  The CPU bus is big endian and the internal
 * registers are little endian.
 *
 * After we write a 32-bit quantity to an internal register we "confirm"
 * (push out to chip) by reading back from the register.
 */

#define TEMPE_READ32(base,reg)  *(UINT32 *)(base+reg)
#define TEMPE_WRITE32_PUSH(base,reg,val) \
	 { *(UINT32 *)(base+reg)=val; \
	   (void)sysInLong(base+reg); }

/*
 * Many of the registers in the Tempe are initialized statically
 * via a table-driven procedure.  The following macros deal with
 * direct register writes to the Tempe chip. 
 */

#define TEMPE_INTRNL            0 /* Mark register as direct "write" access */
#define YES_INIT_TEMPE_REG(offset,val) (TEMPE_INTRNL),(offset),(val),
#define TEMPE_INTRNL_INIT_SZ	3 /* # of elements in above macro */
#define NOT_INIT_TEMPE_REG(offset, desc)         /* No code for this macro */

/* Internal register initialization table entry offsets */

#define TEMPE_REG_OFFSET(x)	*(x+1)
#define TEMPE_REG_VALUE(x)      *(x+2)

/* End of register initialization table marker */

#define END_INIT_TEMPE_REG        0xffffffff

#define TEMPE_OUTBOUND_WINDOW_COUNT 8
#define TEMPE_INBOUND_WINDOW_COUNT 8

#ifndef _ASMLANGUAGE
typedef struct 
    {
    UINT32 otsauImg;        /* Outbound translation start address upper */
    UINT32 otsalImg;        /* Outbound translation start address lower */
    UINT32 oteauImg;        /* Outbound translation end address upper */
    UINT32 otealImg;        /* Outbound translation end address lower */
    UINT32 otofuImg;        /* Outbound translation offset address upper */
    UINT32 otoflImg;        /* Outbound translation offset address lower */
    UINT32 otbsImg;         /* Outbound translation 2eSST broadcast sel */
    UINT32 otatImg;         /* Outbound translation attribute */
    } VME_OUT_MAP;

typedef struct 
    {
    UINT32 itsauImg;        /* Inbound translation start address upper */
    UINT32 itsalImg;        /* Inbound translation start address lower */
    UINT32 iteauImg;        /* Inbound translation end address upper */
    UINT32 itealImg;        /* Inbound translation end address lower */
    UINT32 itofuImg;        /* Inbound translation offset address upper */
    UINT32 itoflImg;        /* Inbound translation offset address lower */
    UINT32 itatImg;         /* Inbound translation attribute */
    } VME_IN_MAP;

/* VME inbound Read Prefetch size */

typedef enum
    {
    VME_RD_PREFETCH_2_CACHE_LINES  = 0,
    VME_RD_PREFETCH_4_CACHE_LINES  = 1,
    VME_RD_PREFETCH_8_CACHE_LINES  = 2,
    VME_RD_PREFETCH_16_CACHE_LINES = 3
    } rdPrefetchSize_t;

/* VME inbound or outbound 2eSST Data Transfer Rate Codes (3 bits) */

typedef enum 
    {
    VME_SST160 = 0x0,
    VME_SST267 = 0x1,
    VME_SST320 = 0x2 /* Note: 0x3 thru 0x7 are reserved */
    } vme2esstRate_t;

/* VME outbound address type definitions (4 bits) */

typedef enum 
    { 
    VME_MODE_A16   = 0x0,
    VME_MODE_A24   = 0x1,
    VME_MODE_A32   = 0x2,
    VME_MODE_A64   = 0x4,
    VME_MODE_CRCSR = 0x5,
    VME_MODE_USER1 = 0x8, /* AM: %0100xx */
    VME_MODE_USER2 = 0x9, /* AM: %0101xx */
    VME_MODE_USER3 = 0xa, /* AM: %0110xx */
    VME_MODE_USER4 = 0xb  /* AM: %0111xx */
    /* Note: 0x3, 0x6, 0x7, 0xc, 0xd 0xe, 0xf are reserved */
    } addressMode_t;

/*  VME outbound Data Widths (2 bits) */

typedef enum 
    {
    VME_D16 = 0x0, /* Note: 0x2 and 0x3 are reserved */
    VME_D32 = 0x1  /* Note: MBLT, 2eVME, 2eSST transfers are always 64-bit */
    } dataWidth_t;

/* VME outbound transfer mode (3 bits) */

typedef enum
    {
    VME_SCT_OUT 	 = 0,
    VME_BLT_OUT		 = 1,
    VME_MBLT_OUT	 = 2,
    VME_2eVME_OUT        = 3,
    VME_2eSST_OUT        = 4,
    VME_2eSST_BCAST_OUT	 = 5 
    } xferProtocolOut_t;


/* 
 * VMEbus OutBound Window Argument Structure 
 * Maps the following way onto the Tempe outbound register set:
 *
 *    Field name		Reg    bits[]    Field name
 *
 *    windowEnable		OTATx  [31]	 EN
 *    pciBusAddrU		OTSAUx [31:0]    STAU
 *    pciBusAddrL		OTSALx [31:0]	 STAL
 *    windowSizeU		OTEAUx [31:0]    ENDU
 *    windowSizeL		OTEALx [31:16]   ENDL
 *    vmeAddrU		        OTOFUx [31:0]    OFFU
 *    vmeAddrL		        OTOFLx [31:16]   OFFL
 *    bcastSelect2esst		OTBSOx [20:0]    2eBS
 *    unused			--- (Residual for old Universe chip)
 *    rdPrefetchEnable		OTATx  [18]      MRPFD
 *    rdPrefetchSize		OTATx  [17:16]   PFS[0-1]
 *    vme2esstRate		OTATx  [13:11]   2eSSTM[0-2]
 *    xferProtocolOut		OTATx  [10:8]    TM[0-2]
 *    dataWidth			OTATx  [7:6]     DBW[0-1]
 *    superAccessType		OTATx  [5]       SUP
 *    pgmAccessType		OTATx  [4]       PGM
 *    addressMode		OTATx  [3:0]     AMODE[0-3]
 */

typedef struct  vmeOutWindowCfg
    {
    BOOL              windowEnable;        /* State of Window */
    UINT32            pciBusAddrU;         /* Start Address on PCI Bus */
    UINT32            pciBusAddrL;         /* Start Address on PCI Bus */
    UINT32            windowSizeU;         /* Window Size */
    UINT32            windowSizeL;         /* Window Size */
    UINT32            vmeBusAddrU;         /* Starting Address on VMEbus */
    UINT32            vmeBusAddrL;         /* Starting Address on VMEbus */
    int               bcastSelect2esst;    /* 2eSST Broadcast Select */
    char              unused;              /* Write Post State (Universe) */
    BOOL              rdPrefetchEnable;    /* Prefetch Read Enable State */
    rdPrefetchSize_t  rdPrefetchSize;      /* Prefetch Read Size (in 
					      Cache Lines) */
    vme2esstRate_t    vme2esstRate;	   /* 2eSST Transfer Rate */
    xferProtocolOut_t xferProtocolOut;     /* Transfer Protocol */
    dataWidth_t       dataWidth;           /* Maximum Data Width */
    BOOL              superAccessType;     /* TRUE = supervisor, FALSE = non */
    BOOL              pgmAccessType;       /* TRUE = pgm, FALSE = data */
    addressMode_t     addressMode;	   /* Address Space */
    int               reserved;            /* For future use */
    } VME_OUT_WINDOW_CFG;

/* VME Inbound transfer mode (5 bits)  - can be ORed together */

typedef enum
    {
    VME_BLT_IN		= 1,
    VME_MBLT_IN	        = 2,
    VME_2eVME_IN        = 4,
    VME_2eSST_IN        = 8,
    VME_2eSST_BCAST_IN	= 16 
    } xferProtocolIn_t;

/* VME inbound Read-Ahead Threshold (1 bit) */

typedef enum
    {
    VME_RD_THRESHOLD_EMPTY      = 0,
    VME_RD_THRESHOLD_HALF	= 1
    } rdThreshold_t;

/* VME inbound Virtual FIFO size for prefetch cycles (2 bits) */

typedef enum
    {
    VME_VFS_FIFO_64 	= 0,
    VME_VFS_FIFO_128	= 1,
    VME_VFS_FIFO_256	= 2,
    VME_VFS_FIFO_512	= 3
    } vfsFifoSize_t;

/* VME inbound address space definitions (3 bits) */

typedef enum 
    { 
    VME_SPACE_A16   = 0x0,
    VME_SPACE_A24   = 0x1,
    VME_SPACE_A32   = 0x2,
    VME_SPACE_A64   = 0x4
    /* Note: 0x3, 0x5, 0x6, 0x7 are reserved */
    } addressSpace_t;

/* 
 * VMEbus InBound Window Argument Structure 
 * Maps the following way onto the Tempe outbound register set:
 *
 *    Field name		Reg    bits[]    Field name
 *
 *    windowEnable		ITATx  [31]	 EN
 *    vmeBusAddrU		ITSAUx [31:0]    STAU
 *    vmeBusAddrL		ITSALx [31:0]	 STAL
 *    windowSizeU		ITEAUx [31:0]    ENDU
 *    windowSizeL		ITEALx [31:16]   ENDL
 *    pciAddrU		        ITOFUx [31:0]    OFFU
 *    pciAddrL		        ITOFLx [31:16]   OFFL
 *    rdThreshold		ITATx  [18]      TH
 *    vfsFifoSize		ITATx  [17:16]   VFS[0-1]
 *    vme2esstRate		ITATx  [14:12]   2eSSTM[0-2]
 *    xferProtocolIn		ITATx  [11:7]    2eSSTB,2eSST,2eVME,MBLT,BLT
 *    addressSpace		ITATx  [6:4]     AS[0-2]
 *    superAccessSlave		ITATx  [3]       SUPER
 *    nonSuperAccessSlave	ITATx  [2]       NPRIV
 *    pgmAccessSlave		ITATx  [1]       PGM
 *    dataAccessSlave		ITATx  [0]       DATA
 */

typedef struct vmeInWindowCfg
    {
    BOOL              windowEnable;        /* State of Window */
    UINT32	      vmeBusAddrU;	   /* Start Address on VME bus */
    UINT32	      vmeBusAddrL;	   /* Start Address on VME bus */
    UINT32            windowSizeU;         /* Window Size */
    UINT32            windowSizeL;         /* Window Size */
    UINT32	      pciBusAddrU;	   /* Starting Address on PCI bus */
    UINT32	      pciBusAddrL;	   /* Starting Address on PCI bus */
    rdThreshold_t     rdThreshold;	   /* Read Prefetch Threshold state */
    vfsFifoSize_t     vfsFifoSize;	   /* Read FIFO prefetch size */
    vme2esstRate_t    vme2esstRate;	   /* 2eSST Transfer Rate */
    xferProtocolIn_t  xferProtocolIn;	   /* Transfer protocol */
    addressSpace_t    addressSpace;	   /* Address space */
    BOOL	      superAccessSlave;	   /* Respond to super access ? */
    BOOL              nonSuperAccessSlave; /* Respond to nonSuper access ? */
    BOOL	      pgmAccessSlave;	   /* Respond to pgm access ? */
    BOOL	      dataAccessSlave;	   /* Respond to data access ? */
    int 	      reserved;		   /* For future use */
    } VME_IN_WINDOW_CFG;

/* Use the following #define string(s) to disable a window(s) */

#define VME_OUTx_CFG_DISABLED  FALSE,0,0,0,0x10000,0,0,0,0,0,0,0,0,0,0,0,0,0
#define VME_INx_CFG_DISABLED   FALSE,0,0,0,0x10000,0,0,0,0,0,0,0,0,0,0,0,0

/* Tempe DMA support */

#define TEMPE_DMA_CHANNEL_COUNT	(2)

typedef enum
    {
    VME_DMA_PATTERN_BYTE_NOINCREMENT	= 0x23000000, /* src */
    VME_DMA_PATTERN_BYTE_INCREMENT	= 0x22000000, /* src */
    VME_DMA_PATTERN_WORD_NOINCREMENT	= 0x21000000, /* src */
    VME_DMA_PATTERN_WORD_INCREMENT	= 0x20000000, /* src */
    VME_DMA_PCI				= 0x00000000, /* src, dst */
    VME_DMA_VME				= 0x10000000  /* src, dst */
    } VME_DMA_TYPE;

/*
 * Template for link-list parameter block - the layout of this block
 * is the way the Tempe chip wants to see it for chain mode DMA.
 */

typedef struct vmeLLdesc	/* Linked list block */
    {
    UINT32 dsaUx;	/* Src upper */
    UINT32 dsaLx;	/* Src lower */
    UINT32 ddaUx;	/* Dst upper */
    UINT32 ddaLx;	/* Dst lower */
    UINT32 dsatx;	/* Src attrib */
    UINT32 ddatx;	/* Dst attrib */
    UINT32 dnlaUx;	/* Next lnk upper */
    UINT32 dnlaLx;	/* Next lnk lower */
    UINT32 dcntx;	/* Count */
    UINT32 ddbsx;	/* Bcst select */
    } VME_LL_DESC;

/* DMA control register information component of TEMPE_DMA_ATTRIBUTES */

typedef struct vmeBusUsg
    {
    int	 maxPciBlockSize;        /* PCI Bus Maximum Block Size */
    int	 pciBackOffTimer;        /* PCI Bus Back-Off Timer (usec) */
    int	 maxVmeBlockSize;        /* VMEbus Maximum Block Size */
    int	 vmeBackOffTimer;        /* VMEbus Back-Off Timer (usec) */
    BOOL vmeFlshOnAbtRead;	 /* TRUE = VME flush on aborted read */
    BOOL pciFlshOnAbtRead;	 /* TRUE = PCI flush on aborted read */
    } VME_BUS_USAGE; 

/* 
 * TEMPE_DMA_ATTRIBUTES is one of the parameters passed in to 
 * sysTempeDmaStart().  The information in this structure will be used to 
 * program the appropriate channel's DMA control register.  Note that the 
 * control register is set once per transfer, even if the transfer spans 
 * multiple chained descriptors.  This structure also contains the
 * optional userHandler function pointer which is optionally used to invoke a
 * user-specified routine after DMA completes.
 */

typedef void (*INT_HANDLER)(UINT32 chan);
typedef struct sysTempeDmaAttributes
    {
    VME_BUS_USAGE       busUsg;         /* VME bus usage */
    INT_HANDLER         userHandler;    /* User defined int compl routine */
    } TEMPE_DMA_ATTRIBUTES;

/* Information for DMA source and destination attribute register fields */

typedef struct vmeAttr
    {
    VME_DMA_TYPE	bus;		   /* Bus specification */
    vme2esstRate_t	vme2esstRate;
    addressMode_t 	addressMode;
    dataWidth_t		dataWidth;
    xferProtocolOut_t	xferProtocolOut;
    BOOL   		superAccessType;   /* TRUE = supervisor access */
    BOOL   		pgmAccessType;	   /* TRUE = user access */
    } VME_ATTR;

/* 
 * TEMPE_DMA_DESCRIPTOR is one of the parameters passed in to 
 * sysTempeDmaStart().  The information in this structure will be used 
 * set up a link-list parameter block which will in turn be used to
 * program the DMA controller's registers if this is a direct mode
 * transfer or to stand in memory as part of a linked list parameter
 * chain for the requested DMA operation.
 */

typedef struct sysTempeDmaDesc
    {

    /* User input parameters */

    UINT64 sourceAddr;     /* 64-bit starting address of source data */
    UINT64 destAddr ;      /* 64-bit starting address of destination */
    UINT32 byteCount;      /* Number of bytes to transfer */
    UINT32 bcastSelect2esst; /* Destination broadcast select */
    VME_ATTR srcVmeAttr;   /* Source VME bus attributes */
    VME_ATTR dstVmeAttr;   /* Destination VME bus attributes */
    struct sysTempeDmaDesc * next;  /* Linked descriptor (or null) */

    /* 
     * Computed values used for actual DMA linked list.  vmeLLDesc
     * is the actual machine parameter block which can be used in
     * a linked list transfer or be used to program the actual
     * DMA engine registers for a direct mode (non linked) transfer.
     * It must be 8-byte aligned, thus we allow a reserved word to
     * give us the ability to align.
     */

    UINT32      linkDesc2 [1]; 			   /* Allow for 4-byte align */
    UINT32      linkDesc  [sizeof(VME_LL_DESC)];   /* Allow for 4-byte align */

    } TEMPE_DMA_DESCRIPTOR;

/* Template for status of DMA */

typedef struct sysTempeDmaStatus
    {
    BOOL   dmaDone;     /* TRUE when DMA is done, successfully or with error */
    UINT32 dstax;	/* Offset to Tempe status register */
    UINT32 dcsaUx;      /* Offset to Tempe Current Source Addr (upper) */
    UINT32 dcsaLx;      /* Offset to Tempe Current Source Addr (lower) */
    UINT32 dcdaUx;      /* Offset to Tempe Current Destination Addr (upper) */
    UINT32 dcdaLx;      /* Offset to Tempe Current Destination Addr (lower) */
    UINT32 dclaUx;	/* Offset to Tempe Current Link Addr (upper) */
    UINT32 dclaLx;	/* Offset to Tempe Current Link Addr (lower) */
    UINT32 dcntx;	/* Offset to Tempe Count (byte) */
    UINT32 timeBaseU;	/* Timebase upper register */
    UINT32 timeBaseL;	/* Timebase lower register */
    } TEMPE_DMA_STATUS;

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCtempeh */
