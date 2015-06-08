/* mpc107.h - MPC107 General Configuration registers   */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,15sep01,dat  Gnu-diab, use of WRS_ASM macro
01b,11sep00,rcs  Add MCP107_DELTA macro
01a,23may00,bri written
*/

#ifndef	__INCmpc107h
#define	__INCmpc107h

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

/* MPC107 device and vendor ID's */

#define MPC107_VEN_ID		0x1057	     /* Motorola Vendor ID */
#define MPC107_DEV_ID		0x0003	     /* MPC107 Device ID */
#define MPC107_DEV_VEN_ID   	0x00031057   /* (DevId << 16) + VenId */

/* MPC107 configuration adrs and data register for map A & B */

#define MPC107_CFG_ADDR_PREP	0x80000cf8   /* MPC107 cfg reg map A */
#define MPC107_CFG_DATA_PREP	0x80000cfc   /* MPC107 cfg data map A */
#define MPC107_CFG_ADDR_CHRP	0xfec00000   /* MPC107 cfg reg map B */
#define MPC107_CFG_DATA_CHRP	0xfee00000   /* MPC107 cfg data map B */
#define MPC107_PICR1_MAPB	0x0  	     /* PICR1 bit 16 map B */
#define MPC107_PICR1_MAPA	0x1          /* PICR1 bit 16 map A */

/* MPC107 indirect configuration registers */

#define MPC107_CFG_BASE		0x80000000   /* Base adrs of cfg regs */
#define MPC107_CFG_BASE_16 	0x8000	     /* MPC107_CFG_BASE >> 16 */
#define MPC107_PCICMD_ADRS	(MPC107_CFG_BASE + 0x04) /* PCI Cmd reg */
#define MPC107_PCISTAT_ADRS	(MPC107_CFG_BASE + 0x06) /* PCI status reg */
#define MPC107_PCIREVID_ADRS	(MPC107_CFG_BASE + 0x08) /* PCI RevId reg */
#define MPC107_PCISTDPROG_ADRS	(MPC107_CFG_BASE + 0x09) /* PCI Standard */
                                                         /* Program Interface */
#define MPC107_PCISUBCLAS_ADRS	(MPC107_CFG_BASE + 0x0a) /* PCI SubClass reg */
#define MPC107_PCIBASECLAS_ADRS (MPC107_CFG_BASE + 0x0b) /* PCI BaseClas reg */
#define MPC107_PCICACHLINE_ADRS	(MPC107_CFG_BASE + 0x0c) /* PCI Cache Line */
                                                         /* Size reg */
#define MPC107_PCIHEADER_ADRS	(MPC107_CFG_BASE + 0x0d) /* PCI Head Typ reg */
#define MPC107_PCIBIST_ADRS	(MPC107_CFG_BASE + 0x0e) /* PCI BIST reg */
#define MPC107_LOCMEMBAR_ADRS	(MPC107_CFG_BASE + 0x10) /* Local Memory Base */
                                                         /* Address Register */
#define MPC107_PCSRBAR_ADRS	(MPC107_CFG_BASE + 0x14) /* Peripheral Cont */
                                                         /* & Status Register */
                                                         /* Base Address Reg */
#define MPC107_PCIINTLINE_ADRS	(MPC107_CFG_BASE + 0x3c) /* PCI Int Line reg */
#define MPC107_PCIINTPIN_ADRS	(MPC107_CFG_BASE + 0x3d) /* PCI Int Pin reg */
#define MPC107_PCIMINGNT_ADRS	(MPC107_CFG_BASE + 0x3e) /* PCI Min Gran reg */
#define MPC107_PCIMAXLAT_ADRS	(MPC107_CFG_BASE + 0x3f) /* PCI Max Lat reg */
#define MPC107_PCIARBCONT_ADRS	(MPC107_CFG_BASE + 0x46) /* PCI Arb Cont reg */
#define MPC107_PMCR1_ADRS	(MPC107_CFG_BASE + 0x70) /* Power Management */
                                                         /* Config Reg-1 */
#define MPC107_PMCR2_ADRS	(MPC107_CFG_BASE + 0x72) /* Power Management */
                                                         /* Config Reg-2 */
#define MPC107_ODCR_ADRS	(MPC107_CFG_BASE + 0x73) /* out driver reg */
#define MPC107_CDCR_ADRS	(MPC107_CFG_BASE + 0x74) /* Clk Driv Cnt reg */
#define MPC107_MIOCR_ADRS	(MPC107_CFG_BASE + 0x76) /* Misc i/o Cnt reg */
#define MPC107_EUMBBAR_ADRS	(MPC107_CFG_BASE + 0x78) /* Embedded Utilitie */
                                                         /* Memory Block Base */
                                                         /* address */
#define MPC107_MSAR1_ADRS	(MPC107_CFG_BASE + 0x80) /* MSAR1 */
#define MPC107_XMSAR1_ADRS	(MPC107_CFG_BASE + 0x88) /* XMSAR1 */
#define MPC107_MEAR1_ADRS	(MPC107_CFG_BASE + 0x90) /* MEAR1 */
#define MPC107_XMEAR1_ADRS	(MPC107_CFG_BASE + 0x98) /* XMEAR1 */
#define MPC107_MSAR2_ADRS  	(MPC107_CFG_BASE + 0x84) /* MSAR2 */
#define MPC107_XMSAR2_ADRS	(MPC107_CFG_BASE + 0x8c) /* XMSAR2 */
#define MPC107_MEAR2_ADRS	(MPC107_CFG_BASE + 0x94) /* MEAR2 */
#define MPC107_XMEAR2_ADRS	(MPC107_CFG_BASE + 0x9c) /* XMEAR2 */
#define MPC107_MBER_ADRS	(MPC107_CFG_BASE + 0xa0) /* MBER */
#define MPC107_MPMR_ADRS	(MPC107_CFG_BASE + 0xa3) /* Page mode reg */
#define MPC107_PICR1_ADRS   	(MPC107_CFG_BASE + 0xa8) /* PICR1 adrs */
#define MPC107_PICR2_ADRS   	(MPC107_CFG_BASE + 0xac) /* PICR2 adrs */

/* MPC107 Diagnostic Registers */

#define MPC107_DIAGS_BASE	(EUMB_BASE + 0x80000)
#define MCP107_MDP_ERR_INJ_MASK_DH 	0xF_F000 /* Data High Err Inject Msk */
#define MCP107_MDP_ERR_INJ_MASK_DL 	0xF_F004 /* Data Low Err Inject Msk */
#define MCP107_MDP_ERR_INJ_MASK_PAR	0xF_F008 /* Data Par Err Inject Msk */
#define MCP107_MDP_ERR_CAP_MON_DH 	0xF_F00c /* Data High Err Capture Mon */
#define MCP107_MDP_ERR_CAP_MON_DL 	0xF_F010 /* Data Low Err Capture Mon */
#define MCP107_MDP_ERR_CAP_MON_PAR	0xF_F014 /* Data Par Err Capture Mon */


#define MPC107_PICR1_ADRSMAP_MSK 0x00010000 /* Mask to determine address Map */
					    /* PREP or CHRP */
#define MPC107_ECCSBEC_ADRS  (MPC107_CFG_BASE + 0xb8) /* ECC single Bit */
                                                      /* error count Reg */
#define MPC107_ECCSBETR_ADRS (MPC107_CFG_BASE + 0xb9) /* ECC single Bit */
                                                      /* error trig Reg */
#define MPC107_ERRENR1_ADRS  (MPC107_CFG_BASE + 0xc0) /* Err Enable Reg-1 */
#define MPC107_ERRDR1_ADRS   (MPC107_CFG_BASE + 0xc1) /* Err Detect Reg-1 */
#define MPC107_PBSES_ADRS    (MPC107_CFG_BASE + 0xc3) /* Processor Err Bus */
#define MPC107_ERRENR2_ADRS  (MPC107_CFG_BASE + 0xc4) /* Err Enable reg-2 */
#define MPC107_ERRDR2_ADRS   (MPC107_CFG_BASE + 0xc5) /* Err Detect Reg-2 */
#define MPC107_PCIBES_ADRS   (MPC107_CFG_BASE + 0xc7) /* PCI Bus/Stat Err */
#define MPC107_PPCIEAR_ADRS  (MPC107_CFG_BASE + 0xc8) /* Processor/PCI Bus */
                                                      /* Error address Reg */
#define MPC107_ADMBO_ADRS    (MPC107_CFG_BASE + 0xe0) /* Addr Map B Opt */
#define MPC107_MCCR1_ADRS    (MPC107_CFG_BASE + 0xf0) /* MCCR1 */
#define MPC107_MCCR2_ADRS    (MPC107_CFG_BASE + 0xf4) /* MCCR2 */
#define MPC107_MCCR3_ADRS    (MPC107_CFG_BASE + 0xf8) /* MCCR3 */
#define MPC107_MCCR4_ADRS    (MPC107_CFG_BASE + 0xfc) /* MCCR4 */


/* MPC107 Latency Timer register */

#define MPC107_LAT_MAX_HOLD	0xf8	     /* max PCI clocks for bus hold */
#define MPC107_LAT_MIN_LAT	0x07	     /* min latency timer value */

/* MPC107 - PCI arbiter control Register */

#define MPC107_PAC_IAE		0x00008000 /* internal arbiter enabled */
#define MPC107_PAC_PARK_MASK    0x00006000 /* park mode control mask */
#define MPC107_PAC_PARK_LAST    0x00000000 /* park mode cntrl, last device */
#define MPC107_PAC_PARK_REQ0    0x00002000 /* park mode using REQ0/GNT0) */
#define MPC107_PAC_PARK_MPC107  0x00004000 /* park mode control (MPC107) */
#define MPC107_PAC_RPCC         0x00000400 /* retry PCI configuration cycle */
#define MPC107_PAC_PPRI_LVL     0x00000080 /* MPC107 priority level: 1=high */
#define MPC107_PAC_EDPL_MASK    0x0000000f /* ext dev priority lvl mask */
#define MPC107_PAC_EDPL_REQ0    0x00000008 /* ext dev pty lvl REQ3/GNT3 */
#define MPC107_PAC_EDPL_REQ1    0x00000004 /* ext dev pty lvl REQ2/GNT2 */
#define MPC107_PAC_EDPL_REQ2    0x00000002 /* ext dev pty lvl REQ1/GNT1 */
#define MPC107_PAC_EDPL_REQ3    0x00000001 /* ext dev pty lvl REQ0/GNT0 */

/* MPC107 - power management configuration #1 Register */

#define MPC107_PMC1_NO_NAP_MSG    0x00008000 /* no message before nap */
#define MPC107_PMC1_NO_SLEEP_MSG  0x00004000 /* no message before sleep */
#define MPC107_PMC1_LP_REF_EN     0x00001000 /* rfrsh enbl in low pwr mode */
#define MPC107_PMC1_SUSP_QACK     0x00000400 /* QACK_ enable */
#define MPC107_PMC1_PM            0x00000080 /* power mananagement enable */
#define MPC107_PMC1_DOZE          0x00000020 /* doze mode */
#define MPC107_PMC1_NAP           0x00000010 /* nap mode */
#define MPC107_PMC1_SLEEP         0x00000008 /* sleep mode */
#define MPC107_PMC1_CKO_MODE_MASK 0x00000006 /* clock output mode - mask */
#define MPC107_PMC1_CKO_MODE_D    0x00000000 /* clk output mode - disable */
#define MPC107_PMC1_CKO_MODE_S    0x00000002 /* clk out mode - system clk */
#define MPC107_PMC1_CKO_MODE_1H   0x00000004 /* clk out mode - 1/2 PCI rate */
#define MPC107_PMC1_CKO_MODE_P    0x00000006 /* clk out mode - PCI rate */
#define MPC107_PMC1_CKO_SEL       0x00000001 /* clk out mode select */

/* MPC107 - power management configuration #2 Register */

#define MPC107_PMC2_DLL_EXTEND    0x00000080 /* extend DLL by a half clock */
#define MPC107_PMC2_PCI_OHD_MASK  0x00000070 /* PCI out hold delay (mask) */
#define MPC107_PMC2_PCI_OHD_SHIFT 4          /* PCI out hold delay (shift) */
#define MPC107_PMC2_SLEEP         0x00000004 /* get PLL when exiting sleep */
#define MPC107_PMC2_SUSPEND       0x00000002 /* get PLL when exiting suspend*/
#define MPC107_PMC2_SHARED_MCP    0x00000001 /* disable MCP assertion */

/* MPC107 - output driver control Register */

#define MPC107_ODC_DRV_PCI           0x00000080 /* PCI drive: 0=hi, 1=med */
#define MPC107_ODC_DRV_STD           0x00000040 /* STD drive: 0=hi, 1=med */
#define MPC107_ODC_DRV_MEM_CTRL_MASK 0x00000030 /* mem ctrl drive: mask */
#define MPC107_ODC_DRV_MEM_CTRL_40   0x00000000 /* mem ctrl drive: 40-ohms */
#define MPC107_ODC_DRV_MEM_CTRL_20   0x00000010 /* mem ctrl drive: 20-ohms */
#define MPC107_ODC_DRV_MEM_CTRL_13_3 0x00000020 /* mem ctrl drive: 13.3-ohms */
#define MPC107_ODC_DRV_MEM_CTRL_8    0x00000030 /* mem ctrl drive: 8-ohms */
#define MPC107_ODC_DRV_PCI_CLK_MASK  0x0000000c /* PCI clock drive: mask */
#define MPC107_ODC_DRV_PCI_CLK_40    0x00000000 /* PCI clock drive: 40-ohms */
#define MPC107_ODC_DRV_PCI_CLK_20    0x00000004 /* PCI clock drive: 20-ohms */
#define MPC107_ODC_DRV_PCI_CLK_13_3  0x00000008 /* PCI clock drive: 13.3-ohms */
#define MPC107_ODC_DRV_PCI_CLK_8     0x0000000c /* PCI clock drive: 8-ohms */
#define MPC107_ODC_DRV_MEM_CLK_MASK  0x00000003 /* mem clock drive: mask */
#define MPC107_ODC_DRV_MEM_CLK_40    0x00000000 /* mem clock drive: 40-ohms */
#define MPC107_ODC_DRV_MEM_CLK_20    0x00000001 /* mem clock drive: 20-ohms */
#define MPC107_ODC_DRV_MEM_CLK_13_3  0x00000002 /* mem clock drive: 13.3-ohms */
#define MPC107_ODC_DRV_MEM_CLK_8     0x00000003 /* mem clock drive: 8-ohms */

/* MPC107 - clock driver control Register */

#define MPC107_CDC_PCI_CLK_0        0x00004000  /* PCI_CLK(0) disable */
#define MPC107_CDC_PCI_CLK_1        0x00002000  /* PCI_CLK(1) disable */
#define MPC107_CDC_PCI_CLK_2        0x00001000  /* PCI_CLK(2) disable */
#define MPC107_CDC_PCI_CLK_3        0x00000800  /* PCI_CLK(3) disable */
#define MPC107_CDC_PCI_CLK_4        0x00000400  /* PCI_CLK(4) disable */
#define MPC107_CDC_SDRAM_CLK_0      0x00000040  /* SDRAM_CLK(0) disable */
#define MPC107_CDC_SDRAM_CLK_1      0x00000020  /* SDRAM_CLK(1) disable */
#define MPC107_CDC_SDRAM_CLK_2      0x00000010  /* SDRAM_CLK(2) disable */
#define MPC107_CDC_SDRAM_CLK_3      0x00000008  /* SDRAM_CLK(3) disable */

/* MPC107 - processor interface configuration #1 Register */

#define MPC107_PIC1_CF_BREAD_WS_MASK  0x00c00000 /* wait states mask */
#define MPC107_PIC1_CF_BREAD_WS_SHIFT 22         /* wait states shift */
#define MPC107_PIC1_RCS0              0x00100000 /* ROM location */
#define MPC107_PIC1_PROC_TYPE_MASK    0x00060000 /* processor type mask */
#define MPC107_PIC1_PROC_TYPE_SHIFT   17         /* processor type shift */
#define MPC107_PIC1_ADDRESS_MAP       0x00010000 /* address map */
#define MPC107_PIC1_FLASH_WR_EN       0x00001000 /* FLASH write enable */
#define MPC107_PIC1_MCP_EN            0x00000800 /* machine check enable */
#define MPC107_PIC1_CF_DPARK          0x00000200 /* processor data bus park */
#define MPC107_PIC1_STORE_GATHER      0x00000040 /* store gathering enable */
#define MPC107_PIC1_ENDIAN_MODE       0x00000020 /* endian mode */
#define MPC107_PIC1_CF_LOOP_SNOOP     0x00000010 /* PCI-to-mem snoop loop en */
#define MPC107_PIC1_CF_APARK          0x00000008 /* processor addr bus park */
#define MPC107_PIC1_SPECULATIVE       0x00000004 /* speculative PCI from */
                                           	 /* memory read enable */

/* MPC107 - processor interface configuration #2 Register */

#define MPC107_PIC2_NO_SER_ON_CFG  0x20000000 /* disable PCI serialization */
#define MPC107_PIC2_NO_SNOOP_EN    0x08000000 /* disable PCI snoop */
#define MPC107_PIC2_CF_FF0_LOCAL   0x04000000 /* ROM PCI address map */
#define MPC107_PIC2_FLSH_WR_LCK_EN 0x02000000 /* disable FLASH writes */
#define MPC107_PIC2_CF_SNOOP_WS_M  0x00c00000 /* snoop addr phase wait state*/
#define MPC107_PIC2_CF_SNOOP_WS_S  18         /* snoop addr wait shift */
#define MPC107_PIC2_CF_APHASE_WS_M 0x0000000c /* proc addr phase wait states*/
#define MPC107_PIC2_CF_APHASE_WS_S 2          /* proc addr phase wait shift */

/* MPC107 - emulation support */

#define MPC107_ES_CPU_FD_ALIAS_EN  0x00000080 /* forward FDxxxxxx to PCI */
#define MPC107_ES_PCI_FD_ALIAS_EN  0x00000040 /* forward FDxxxxxx to CPU */
#define MPC107_ES_DLL_RESET	   0x00000020 /* reset the DLL */
#define MPC107_ES_PCI_COMPAT_HOLE  0x00000008 /* PCI compatibil hole enable */
#define MPC107_ES_PROC_COMPAT_HOLE 0x00000004 /* proc compatibility hole en */

/* MPC107 - error enable #1 Register */

#define MPC107_EE1_PCI_TARG_ABORT   0x00000080  /* PCI target abort */
#define MPC107_EE1_PCI_PERR_SLAVE   0x00000040  /* PCI slace PERR */
#define MPC107_EE1_MEM_SELECT       0x00000020  /* memory select */
#define MPC107_EE1_MEM_REFRESH      0x00000010  /* memory refresh overflow */
#define MPC107_EE1_PCI_PERR_MSTR    0x00000008  /* PCI master PERR */
#define MPC107_EE1_MEM_READ_PARITY  0x00000004  /* memory read parity */
#define MPC107_EE1_PCI_MSTR_ABORT   0x00000002  /* PCI master abort */
#define MPC107_EE1_LOCAL_BUS_ERROR  0x00000001  /* local bus error */

/* MPC107 - error detection #1 Register */

#define MPC107_ED1_SERR            0x00000080 /* SERR_ received */
#define MPC107_ED1_PCI_PERR_SLAVE  0x00000040 /* PCI slace PERR */
#define MPC107_ED1_MEM_SELECT      0x00000020 /* memory select */
#define MPC107_ED1_MEM_REFRESH     0x00000010 /* memory refresh overflow */
#define MPC107_ED1_CYCLE_SPACE     0x00000008 /* cycle type: 0=local, 1=PCI */
#define MPC107_ED1_MEM_READ_PARITY 0x00000004 /* memory read parity */
#define MPC107_ED1_ULBC_MASK       0x00000003 /* unsupported local bus */
					      /* cycle mask */
#define MPC107_ED1_ULBC_NO_ERROR   0x00000000 /* no error detected */
#define MPC107_ED1_ULBC_UTA        0x00000001 /* unsupported transfer */
					      /* attributes */

/* MPC107 - CPU Bus Error Status Register */

#define MPC107_CPU_BUS_ERR_TT_MASK    0x000000f8
#define MPC107_CPU_BUS_ERR_TSIZ_MASK  0x00000007

/* MPC107 - error enable #2 */

#define MPC107_EE2_PCI_ADRS_PARITY  0x00000080 /* PCI address parity error */
#define MPC107_EE2_ECC_MULTIBIT     0x00000008 /* ECC multi-bit error */
#define MPC107_EE2_60X_MEM_WRITE_P  0x00000004 /* 60X mem write parity error */
#define MPC107_EE2_FLASH_ROM_WRITE  0x00000001 /* Flash ROM write error */

/* MPC107 - error detection #2 Register */

#define MPC107_ED2_IEA              0x00000080 /* invalid error address */
#define MPC107_ED2_ECC_MULTIBIT     0x00000008 /* ECC multi-bit error */
#define MPC107_ED2_60X_MEM_WRITE_P  0x00000004 /* 60X mem write parity error */
#define MPC107_ED2_FLASH_ROM_WRITE  0x00000001 /* Flash ROM write error */

/* MCP107 - PCI Bus Error Status Register */

#define MPC107_CPU_BUS_TARGET	     0x00000010	/* 1=bus target,0=bus master */
#define MPC107_CPU_BUS_ERR_C_BE_MASK 0x0000000f /* Bus Error Status mask */


/* Address MAP B Options register */

#define MPC107_ADMBO_PROC_HOLE   0x00000004 /* Processor compatability hole */
#define MPC107_ADMBO_PCI_HOLE  	0x00000008 /* PCI compatabilty hole */
#define MPC107_ADMBO_PCI_ALIAS   0x00000040 /* PCI alias */
#define MPC107_ADMBO_CPU_ALIAS   0x00000080 /* CPU alias */

#define MPC107_ADMBO_DEFAULT  0x0


/* Default Settings & values  for MPC107 Registers */

#define MPC107_PCICMD_DEFAULT	0x00000006  /* PCI COMMAND Default value */
#define MPC107_PICR1_DEFAULT   	0xff041a18  /* PICR1  setting Flash */
                                            /* writing Enabled */
#define MPC107_PICR2_DEFAULT    0x04040000  /* PICR2  setting with Write */
                                            /* Operations to Flash Enabled */
#define MPC107_FLASH_WRITE_BIT	(0x1 << 12) /* Flash Write Enable Bit */

/* PPC Decrementer - used as vxWorks system clock */

#define MPC107_DELTA(a,b)              (abs((int)a - (int)b))

/* High and low words of bit-mask of read-only bits in PICR1 */

#define MPC107_PICR1_ROBITS_16    0x0011    /* Mask of Readonly bits */
#define MPC107_PICR1_ROBITS_00    0x0000    /* Mask of Readonly bits */
#define MPC107_EUMBBAR_VAL	0x80500000  /* Base address of EUMBBAR Reg */

#define MPC107_PCSRBAR_VAL	0x80500000  /* Base addr of PCRBARVAL Reg */

#define MPC107_ODCR_DATA 	0xff        /* default ODCR data */

#define MPC107_DEFAULT_BUS_CLOCK	33000000    /* 33 Mhz */
#define MPC107_DEFAULT_TIMER_CLOCK	33000000/8  /* 33Mhz /8 */

#ifndef SYNC
# define SYNC  		WRS_ASM ("sync") 
#endif  /* SYNC */


/* Swap of four bytes : ABCD becomes DCBA */

#define MPC107LONGSWAP(x) LONGSWAP(x)

/* Swap of two bytes : AB becomes BA */

#define MPC107WORDSWAP(x) WORDSWAP(x)

/* Macro to read Data from a register in EUMBBAR space */

#define MPC107EUMBBARREAD(regNum)    MPC107LONGSWAP(*(ULONG *) \
                                          (MPC107_EUMBBAR_VAL + regNum))

/* Macro to write Data to a register in EUMBBAR space */

#define MPC107EUMBBARWRITE(regNum,regVal) \
             *(ULONG *)(MPC107_EUMBBAR_VAL + regNum) = MPC107LONGSWAP(regVal)

/* Macro to read Data from a register in PCSRBAR space */

#define  MPC107EUMBBARREAD(regNum)    MPC107LONGSWAP(*(ULONG *) \
                                                 (MPC107_EUMBBAR_VAL + regNum))

/* Macro to write Data to a register in PCSRBAR space */

#define MPC107PCSRBARWRITE(regNum,regVal)  \
             *(ULONG *) (PCSRBAR_VAL + regNum) = MPC107LONGSWAP(regVal)

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107h */

