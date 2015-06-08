/* mv64360.h - Register definitions for mv64360 system Controller */

/* Copyright 2002-2004, Motorola Inc. All Rights Reserved */

/*
modification history
--------------------
01u,06dec04,cak Changed DDR_SDRAM_TWTR_DFLT to 1 as per Marvell's 
		Technical Bulletin 92.
01t,03jun04,cak Added bit and mask definitions for the watchdog timer
		registers.
01s,05may04,scb specialMem() support.
01r,07apr04,cak Modified a few of the default DRAM timing parameters. 
01q,20mar04,cak Modified default values for tWTR, tRTR, and tRTW (sdram
		timing high and low registers) used by sysMv64360Smc.c. 
01p,12mar04,cak Added Integrated SRAM definitions.
01o,03nov03,cak BSP update.
01n,04mar03,cak Added NOT_CNFG_MV64360_REG and NOT_CNFG_MV64360_REG_ALT
		macros.
01m,10jan03,efb Corrected some ethernet register offsets.
01l,30sep02,cak Added MV64360_PCICNFG_ALT and YES_CNFG_MV64360_REG_ALT macros.
01k,17sep02,cak Added definitions for the DUnit Control (Low), DFCDL
                Configuration 0, and SDRAM Address/Control Pads Calibration
                registers.
01j,03sep02,scb Move base address and size encoding/decoding macros out of
                mv6100A.h and into the this file.
01i,23jul02,scb Fix minor typo in commentary.
01h,18jul02,yyz Added bit #defines for registers related to timer/counter 0.
01g,10jul02,cak More name changes.
01f,26jun02,scb Register mnemonic name changes.
01e,26jun02,scb Fix register mnemonic faux pas.
01d,25jun02,scb Added some bit shift definitions.
01c,14jun02,scb Modified some register names for consistency.
01b,10apr02,sdb Add macros to support Mv64360 chip initialization.
01a,06mar02,scb Initial creation, reg defs only - not bit defs.
*/

/*
This file contains all of the register definitions for the Marvell MV64360
system Controller.  The registers are divided into subfunctions with a unique
mnemonic preface to identify the subfunction (e.g. CPUIF_ == CPU Interface
registers).  The register definitions are given in the same order as
presented in Appendix A through Appendix Q of the MV64360 specification
(rev 0.98).  Naming conventions used in the Marvell specification were 
used subject to abbreviations (e.g. _ATTR_ == Attribute etc.)

NOTE:
The following line of code appears later in this file
#define MUIF_OUTBND_MESSAGE1_PCI0                              0x1C1C

even though the Atlantis spec lists the value as 0x1C1 which 
would appear to be an error in the Atlantis spec (rev 0.98).
*/

#ifndef __INCmv64360h
#define __INCmv64360h

/* includes */

#ifndef _ASMLANGUAGE
#   include "vmLib.h"
#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
   extern "C" {
#endif

/* CPU Interface Registers */

#define CPUIF_CS0_BASE_ADDR                                     0x008
#define CPUIF_CS0_SIZE                                          0x010
#define CPUIF_CS1_BASE_ADDR                                     0x208
#define CPUIF_CS1_SIZE                                          0x210
#define CPUIF_CS2_BASE_ADDR                                     0x018
#define CPUIF_CS2_SIZE                                          0x020
#define CPUIF_CS3_BASE_ADDR                                     0x218
#define CPUIF_CS3_SIZE                                          0x220
#define CPUIF_DEVCS0_BASE_ADDR                                  0x028
#define CPUIF_DEVCS0_SIZE                                       0x030
#define CPUIF_DEVCS1_BASE_ADDR                                  0x228
#define CPUIF_DEVCS1_SIZE                                       0x230
#define CPUIF_DEVCS2_BASE_ADDR                                  0x248
#define CPUIF_DEVCS2_SIZE                                       0x250
#define CPUIF_DEVCS3_BASE_ADDR                                  0x038
#define CPUIF_DEVCS3_SIZE                                       0x040
#define CPUIF_BOOTCS_BASE_ADDR                                  0x238
#define CPUIF_BOOTCS_SIZE                                       0x240
#define CPUIF_PCI0_IO_BASE_ADDR                                 0x048
#define CPUIF_PCI0_IO_SIZE                                      0x050
#define CPUIF_PCI0_MEM0_BASE_ADDR                               0x058
#define CPUIF_PCI0_MEM0_SIZE                                    0x060
#define CPUIF_PCI0_MEM1_BASE_ADDR                               0x080
#define CPUIF_PCI0_MEM1_SIZE                                    0x088
#define CPUIF_PCI0_MEM2_BASE_ADDR                               0x258
#define CPUIF_PCI0_MEM2_SIZE                                    0x260
#define CPUIF_PCI0_MEM3_BASE_ADDR                               0x280
#define CPUIF_PCI0_MEM3_SIZE                                    0x288
#define CPUIF_PCI1_IO_BASE_ADDR                                 0x090
#define CPUIF_PCI1_IO_SIZE                                      0x098
#define CPUIF_PCI1_MEM0_BASE_ADDR                               0x0A0
#define CPUIF_PCI1_MEM0_SIZE                                    0x0A8
#define CPUIF_PCI1_MEM1_BASE_ADDR                               0x0B0
#define CPUIF_PCI1_MEM1_SIZE                                    0x0B8
#define CPUIF_PCI1_MEM2_BASE_ADDR                               0x2A0
#define CPUIF_PCI1_MEM2_SIZE                                    0x2A8
#define CPUIF_PCI1_MEM3_BASE_ADDR                               0x2B0
#define CPUIF_PCI1_MEM3_SIZE                                    0x2B8
#define CPUIF_INTEGRATED_SRAM_BASE_ADDR                         0x268
#define CPUIF_INTERNAL_SPACE_BASE_ADDR                          0x068
#define CPUIF_BASE_ADDR_ENABLE                                  0x278
#define CPUIF_PCI0_IO_ADDR_REMAP                                0x0F0
#define CPUIF_PCI0_MEM0_ADDR_REMAP_LOW                          0x0F8
#define CPUIF_PCI0_MEM0_ADDR_REMAP_HI                           0x320
#define CPUIF_PCI0_MEM1_ADDR_REMAP_LOW                          0x100
#define CPUIF_PCI0_MEM1_ADDR_REMAP_HI                           0x328
#define CPUIF_PCI0_MEM2_ADDR_REMAP_LOW                          0x2F8
#define CPUIF_PCI0_MEM2_ADDR_REMAP_HI                           0x330
#define CPUIF_PCI0_MEM3_ADDR_REMAP_LOW                          0x300
#define CPUIF_PCI0_MEM3_ADDR_REMAP_HI                           0x338
#define CPUIF_PCI1_IO_ADDR_REMAP                                0x108
#define CPUIF_PCI1_MEM0_ADDR_REMAP_LOW                          0x110
#define CPUIF_PCI1_MEM0_ADDR_REMAP_HI                           0x340
#define CPUIF_PCI1_MEM1_ADDR_REMAP_LOW                          0x118
#define CPUIF_PCI1_MEM1_ADDR_REMAP_HI                           0x348
#define CPUIF_PCI1_MEM2_ADDR_REMAP_LOW                          0x310
#define CPUIF_PCI1_MEM2_ADDR_REMAP_HI                           0x350
#define CPUIF_PCI1_MEM3_ADDR_REMAP_LOW                          0x318
#define CPUIF_PCI1_MEM3_ADDR_REMAP_HI                           0x358
#define CPUIF_PCI0_HDRS_RETARG_CTRL                             0x3B0
#define CPUIF_PCI0_HDRS_RETARG_BASE                             0x3B8
#define CPUIF_PCI1_HDRS_RETARG_CTRL                             0x3C0
#define CPUIF_PCI1_HDRS_RETARG_BASE                             0x3C8
#define CPUIF_CPU_CFG                                           0x000
#define CPUIF_CPU_GE_HDRS_RETARG_CTRL                           0x3D0
#define CPUIF_CPU_GE_HDRS_RETARG_BASE                           0x3D8
#define CPUIF_CPU_IDMA_HDRS_RETARG_CTRL                         0x3E0
#define CPUIF_CPU_IDMA_HDRS_RETARG_BASE                         0x3E8
#define CPUIF_CPU_MODE                                          0x120
#define CPUIF_CPU_PADS_CALIBRATION                              0x3B4
#define CPUIF_RST_SAMPLE_LO                                     0x3C4
#define CPUIF_RST_SAMPLE_HI                                     0x3D4
#define CPUIF_CPU_MSTR_CTRL                                     0x160
#define CPUIF_CPU_INTERFACE_CROSSBAR_CTRL_LOW                   0x150
#define CPUIF_CPU_INTERFACE_CROSSBAR_CTRL_HI                    0x158
#define CPUIF_WHO_AM_I                                          0x200
#define CPUIF_CPU0_DOORBELL                                     0x214
#define CPUIF_CPU0_DOORBELL_CLEAR                               0x21C
#define CPUIF_CPU_INTERFACE_CROSSBAR_TIME_OUT                   0x168
#define CPUIF_CPU1_DOORBELL                                     0x224
#define CPUIF_CPU1_DOORBELL_CLEAR                               0x22C
#define CPUIF_CPU0_DOORBELL_MASK                                0x234
#define CPUIF_CPU1_DOORBELL_MASK                                0x23C
#define CPUIF_SEMAPHORE0                                        0x244
#define CPUIF_SEMAPHORE1                                        0x24C
#define CPUIF_SEMAPHOER2                                        0x254
#define CPUIF_SEMAPHORE3                                        0x25C
#define CPUIF_SEMAPHORE4                                        0x264
#define CPUIF_CPU0_SYNC_BARRIER_TRIGGER                         0x0C0
#define CPUIF_CPU0_SYNC_BARRIER_VIRTUAL                         0x0C8
#define CPUIF_SEMAPHORE5                                        0x26C
#define CPUIF_SEMAPHORE6                                        0x274
#define CPUIF_SEMAPHORE7                                        0x27C
#define CPUIF_CPU_PROTECT_WINDOW0_BASE_ADDR                     0x180
#define CPUIF_CPU_PROTECT_WINDOW0_SIZE                          0x188
#define CPUIF_CPU1_SYNC_BARRIER_TRIGGER                         0x0D0
#define CPUIF_CPU1_SYNC_BARRIER_VIRTUAL                         0x0D8
#define CPUIF_CPU_PROTECT_WINDOW1_BASE_ADDR                     0x190
#define CPUIF_CPU_PROTECT_WINDOW1_SIZE                          0x198
#define CPUIF_CPU_PROTECT_WINDOW2_BASE_ADDR                     0x1A0
#define CPUIF_CPU_PROTECT_WINDOW2_SIZE                          0x1A8
#define CPUIF_CPU_PROTECT_WINDOW3_BASE_ADDR                     0x1B0
#define CPUIF_CPU_PROTECT_WINDOW3_SIZE                          0x1B8
#define CPUIF_CPU_ERR_ADDR_LOW                                  0x070
#define CPUIF_CPU_ERR_ADDR_HI                                   0x078
#define CPUIF_CPU_ERR_DATA_LOW                                  0x128
#define CPUIF_CPU_ERR_DATA_HI                                   0x130
#define CPUIF_CPU_ERR_PARITY                                    0x138
#define CPUIF_CPU_ERR_CAUSE                                     0x140
#define CPUIF_CPU0_ERR_MASK                                     0x148

/* Integrated SRAM Registers */

#define ITG_SRAM_CFG                                            0x380
#define ITG_SRAM_ERR_CAUSE                                      0x388
#define ITG_SRAM_TEST_MODE                                      0x3F4
#define ITG_SRAM_ERR_ADDR                                       0x390
#define ITG_SRAM_ERR_ADDR_HI                                    0x3F8
#define ITG_SRAM_ERR_DATA_LOW                                   0x398
#define ITG_SRAM_ERR_DATA_HI                                    0x3A0
#define ITG_SRAM_ERR_PARITY                                     0x3A8

/* DDR SDRAM Controller Registers */

#define DDR_SDRAM_CFG 	                                        0x1400
#define DDR_DUNIT_CTRL_LO                                       0x1404
#define DDR_DUNIT_CTRL_HI                                       0x1424
#define DDR_SDRAM_TMNG_LO                                       0x1408
#define DDR_SDRAM_TMNG_HI                                       0x140C
#define DDR_SDRAM_ADDR_CTRL                                     0x1410
#define DDR_SDRAM_OPEN_PAGES_CTRL                               0x1414
#define DDR_SDRAM_OPER                                          0x1418
#define DDR_SDRAM_MODE                                          0x141C
#define DDR_SDRAM_EXT_DRAM_MODE                                 0x1420
#define DDR_SDRAM_IF_XBAR_CTRL_LO                               0x1430
#define DDR_SDRAM_IF_XBAR_CTRL_HI                               0x1434
#define DDR_SDRAM_IF_XBAR_TMOUT                                 0x1438
#define DDR_SDRAM_ADDR_CTRL_PAD_CALIB                           0x14C0
#define DDR_SDRAM_DATA_PAD_CALIB                                0x14C4
#define DDR_SDRAM_ERR_ADDR                                      0x1450
#define DDR_SDRAM_ERR_DATA_LO                                   0x1444
#define DDR_SDRAM_ERR_DATA_HI                                   0x1440
#define DDR_SDRAM_RCVD_ECC                                      0x1448
#define DDR_SDRAM_CALC_ECC                                      0x144C
#define DDR_SDRAM_ECC_CTRL                                      0x1454
#define DDR_SDRAM_ECC_CNTR                                      0x1458
#define DDR_DFCDL_CFG0                                          0x1480
#define DDR_DFCDL_CFG1                                          0x1484
#define DDR_SRAM_ADDR                                           0x1490
#define DDR_SRAM_DATA0                                          0x1494
#define DDR_DFCDL_PROBE                                         0x14A0

/* Device Controller Registers*/

#define DVCTL_DEVICE_BANK0_PARAMS                               0x45C
#define DVCTL_DEVICE_BANK1_PARAMS                               0x460
#define DVCTL_DEVICE_BANK2_PARAMS                               0x464
#define DVCTL_DEVICE_BANK3_PARAMS                               0x468
#define DVCTL_DEVICE_BOOT_BANK_PARAMS                           0x46C
#define DVCTL_DEVICE_INTERFACE_CTRL                             0x4C0
#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW                0x4C8
#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI                 0x4CC
#define DVCTL_DEVICE_INTERFACE_CROSSBAR_TIMEOUT                 0x4C4
#define DVCTL_DEVICE_INTERRUPT_CAUSE                            0x4D0
#define DVCTL_DEVICE_INTERRUPT_MASK                             0x4D4
#define DVCTL_DEVICE_ERR_ADDR                                   0x4D8
#define DVCTL_DEVICE_ERR_DATA                                   0x4DC
#define DVCTL_DEVICE_ERR_PARITY                                 0x4E0

/* PCI Interface Registers */

#define PCIIF_CS0_BAR_SIZE_PCI0                                 0xC08
#define PCIIF_CS0_BAR_SIZE_PCI1                                 0xC88
#define PCIIF_CS1_BAR_SIZE_PCI0                                 0xD08
#define PCIIF_CS1_BAR_SIZE_PCI1                                 0xD88
#define PCIIF_CS2_BAR_SIZE_PCI0                                 0xC0C
#define PCIIF_CS2_BAR_SIZE_PCI1                                 0xC8C
#define PCIIF_CS3_BAR_SIZE_PCI0                                 0xD0C
#define PCIIF_CS3_BAR_SIZE_PCI1                                 0xD8C
#define PCIIF_DEVCS0_BAR_SIZE_PCI0                              0xC10
#define PCIIF_DEVCS0_BAR_SIZE_PCI1                              0xC90
#define PCIIF_DEVCS1_BAR_SIZE_PCI0                              0xD10
#define PCIIF_DEVCS1_BAR_SIZE_PCI1                              0xD90
#define PCIIF_DEVCS2_BAR_SIZE_PCI0                              0xD18
#define PCIIF_DEVCS2_BAR_SIZE_PCI1                              0xD98
#define PCIIF_DEVCS3_BAR_SIZE_PCI0                              0xC14
#define PCIIF_DEVCS3_BAR_SIZE_PCI1                              0xC94
#define PCIIF_BOOTCS_BAR_SIZE_PCI0                              0xD14
#define PCIIF_BOOTCS_BAR_SIZE_PCI1                              0xD94
#define PCIIF_P2P_MEM0_BAR_SIZE_PCI0                            0xD1C
#define PCIIF_P2P_MEM0_BAR_SIZE_PCI1                            0xD9C
#define PCIIF_P2P_MEM1_BAR_SIZE_PCI0                            0xD20
#define PCIIF_P2P_MEM1_BAR_SIZE_PCI1                            0xDA0
#define PCIIF_P2P_IO_BAR_SIZE_PCI0                              0xD24
#define PCIIF_P2P_IO_BAR_SIZE_PCI1                              0xDA4
#define PCIIF_CPU_BAR_SIZE_PCI0                                 0xD28
#define PCIIF_CPU_BAR_SIZE_PCI1                                 0xDA8
#define PCIIF_INTEGRATED_SRAM_BAR_SIZE_PCI0                     0xE00
#define PCIIF_INTEGRATED_SRAM_BAR_SIZE_PCI1                     0xE80
#define PCIIF_EXPANSION_ROM_BAR_SIZE_PCI0                       0xD2C
#define PCIIF_EXPANSION_ROM_BAR_SIZE_PCI1                       0xDAC
#define PCIIF_BASE_ADDR_ENABLE_PCI0                             0xC3C
#define PCIIF_BASE_ADDR_ENABLE_PCI1                             0xCBC
#define PCIIF_CS0_BASE_ADDR_REMAP_PCI0                          0xC48
#define PCIIF_CS0_BASE_ADDR_REMAP_PCI1                          0xCC8
#define PCIIF_CS1_BASE_ADDR_REMAP_PCI0                          0xD48
#define PCIIF_CS1_BASE_ADDR_REMAP_PCI1                          0xDC8
#define PCIIF_CS2_BASE_ADDR_REMAP_PCI0                          0xC4C
#define PCIIF_CS2_BASE_ADDR_REMAP_PCI1                          0xCCC
#define PCIIF_CS3_BASE_ADDR_REMAP_PCI0                          0xD4C
#define PCIIF_CS3_BASE_ADDR_REMAP_PCI1                          0xDCC
#define PCIIF_DEVCS0_BASE_ADDR_REMAP_PCI0                       0xC50
#define PCIIF_DEVCS0_BASE_ADDR_REMAP_PCI1                       0xCD0
#define PCIIF_DEVCS1_BASE_ADDR_REMAP_PCI0                       0xD50
#define PCIIF_DEVCS1_BASE_ADDR_REMAP_PCI1                       0xDD0
#define PCIIF_DEVCS2_BASE_ADDR_REMAP_PCI0                       0xD58
#define PCIIF_DEVCS2_BASE_ADDR_REMAP_PCI1                       0xDD8
#define PCIIF_DEVCS3_BASE_ADDR_REMAP_PCI0                       0xC54
#define PCIIF_DEVCS3_BASE_ADDR_REMAP_PCI1                       0xCD4
#define PCIIF_BOOTCS_BASE_ADDR_REMAP_PCI0                       0xD54
#define PCIIF_BOOTCS_BASE_ADDR_REMAP_PCI1                       0xDD4
#define PCIIF_P2P_MEM0_BASE_ADDR_REMAP_LOW_PCI0                 0xD5C
#define PCIIF_P2P_MEM0_BASE_ADDR_REMAP_LOW_PCI1                 0xDDC
#define PCIIF_P2P_MEM0_BASE_ADDR_REMAP_HI_PCI0                  0xD60
#define PCIIF_P2P_MEM0_BASE_ADDR_REMAP_HI_PCI1                  0xDE0
#define PCIIF_P2P_MEM1_BASE_ADDR_REMAP_LOW_PCI0                 0xD64
#define PCIIF_P2P_MEM1_BASE_ADDR_REMAP_LOW_PCI1                 0xDE4
#define PCIIF_P2P_MEM1_BASE_ADDR_REMAP_HI_PCI0                  0xD68
#define PCIIF_P2P_MEM1_BASE_ADDR_REMAP_HI_PCI1                  0xDE8
#define PCIIF_P2P_IO_BASE_ADDR_REMAP_PCI0                       0xD6C
#define PCIIF_P2P_IO_BASE_ADDR_REMAP_PCI1                       0xDEC
#define PCIIF_CPU_BASE_ADDR_REMAP_LOW_PCI0                      0xD70
#define PCIIF_CPU_BASE_ADDR_REMAP_LOW_PCI1                      0xDF0
#define PCIIF_CPU_BASE_ADDR_REMAP_HI_PCI0                       0xD74
#define PCIIF_CPU_BASE_ADDR_REMAP_HI_PCI1                       0xDF4
#define PCIIF_HDRS_RETARG_CTRL_PCI0                             0xF40
#define PCIIF_HDRS_RETARG_CTRL_PCI1                             0xFC0
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_REMAP_PCI0              0xF00
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_REMAP_PCI1              0xF80
#define PCIIF_EXPANSION_ROM_BASE_ADDR_REMAP_PCI0                0xF38
#define PCIIF_EXPANSION_ROM_BASE_ADDR_REMAP_PCI1                0xFB8
#define PCIIF_ADDR_DECODE_CTRL_PCI0                             0xD3C
#define PCIIF_ADDR_DECODE_CTRL_PCI1                             0xDBC
#define PCIIF_HDRS_RETARG_BASE_PCI0                             0xF44
#define PCIIF_HDRS_RETARG_BASE_PCI1                             0xFC4
#define PCIIF_HDRS_RETARG_BASE_HI_PCI0                          0xF48
#define PCIIF_HDRS_RETARG_BASE_HI_PCI1                          0xFC8
#define PCIIF_DLL_STATUS_AND_CTRL_PCI0                          0x1D20
#define PCIIF_DLL_STATUS_AND_CTRL_PCI1                          0x1DA0
#define PCIIF_MPP_PADS_CALIBRATION_PCI0                         0x1D1C
#define PCIIF_MPP_PADS_CALIBRATION_PCI1                         0x1D9C
#define PCIIF_PCI_CMD_PCI0                                      0xC00
#define PCIIF_PCI_CMD_PCI1                                      0xC80
#define PCIIF_MODE_PCI0                                         0xD00
#define PCIIF_MODE_PCI1                                         0xD80
#define PCIIF_RETRY_PCI0                                        0xC04
#define PCIIF_RETRY_PCI1                                        0xC84
#define PCIIF_DISCARD_TIMER_PCI0                                0xD04
#define PCIIF_DISCARD_TIMER_PCI1                                0xD84
#define PCIIF_MSI_TRIGGER_TIMER_PCI0                            0xC38
#define PCIIF_MSI_TRIGGER_TIMER_PCI1                            0xCB8
#define PCIIF_ARB_CTRL_PCI0                                     0x1D00
#define PCIIF_ARB_CTRL_PCI1                                     0x1D80
#define PCIIF_INTERFACE_CROSSBAR_CTRL_LOW_PCI0                  0x1D08
#define PCIIF_INTERFACE_CROSSBAR_CTRL_LOW_PCI1                  0x1D88
#define PCIIF_INTERFACE_CROSSBAR_CTRL_HI_PCI0                   0x1D0C
#define PCIIF_INTERFACE_CROSSBAR_CTRL_HI_PCI1                   0x1D8C
#define PCIIF_INTERFACE_CROSSBAR_TIMEOUT_PCI0                   0x1D04
#define PCIIF_INTERFACE_CROSSBAR_TIMEOUT_PCI1                   0x1D84
#define PCIIF_SYNC_BARRIER_TRIGGER_PCI0                         0x1D18
#define PCIIF_SYNC_BARRIER_TRIGGER_PCI1                         0x1D98
#define PCIIF_SYNC_BARRIER_VIRTUAL_PCI0                         0x1D10
#define PCIIF_SYNC_BARRIER_VIRTUAL_PCI1                         0x1D90
#define PCIIF_P2P_CFG_PCI0                                      0x1D14
#define PCIIF_P2P_CFG_PCI1                                      0x1D94
#define PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0                        0x1E00
#define PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1                        0x1E80
#define PCIIF_ACCESS_CTRL_BASE0_HI_PCI0                         0x1E04
#define PCIIF_ACCESS_CTRL_BASE0_HI_PCI1                         0x1E84
#define PCIIF_ACCESS_CTRL_SIZE0_PCI0                            0x1E08
#define PCIIF_ACCESS_CTRL_SIZE0_PCI1                            0x1E88
#define PCIIF_ACCESS_CTRL_BASE1_LOW_PCI0                        0x1E10
#define PCIIF_ACCESS_CTRL_BASE1_LOW_PCI1                        0x1E90
#define PCIIF_ACCESS_CTRL_BASE1_HI_PCI0                         0x1E14
#define PCIIF_ACCESS_CTRL_BASE1_HI_PCI1                         0x1E94
#define PCIIF_ACCESS_CTRL_SIZE1_PCI0                            0x1E18
#define PCIIF_ACCESS_CTRL_SIZE1_PCI1                            0x1E98
#define PCIIF_ACCESS_CTRL_BASE2_LOW_PCI0                        0x1E20
#define PCIIF_ACCESS_CTRL_BASE2_LOW_PCI1                        0x1EA0
#define PCIIF_ACCESS_CTRL_BASE2_HI_PCI0                         0x1E24
#define PCIIF_ACCESS_CTRL_BASE2_HI_PCI1                         0x1EA4
#define PCIIF_ACCESS_CTRL_SIZE2_PCI0                            0x1E28
#define PCIIF_ACCESS_CTRL_SIZE2_PCI1                            0x1EA8
#define PCIIF_ACCESS_CTRL_BASE3_LOW_PCI0                        0x1E30
#define PCIIF_ACCESS_CTRL_BASE3_LOW_PCI1                        0x1EB0
#define PCIIF_ACCESS_CTRL_BASE3_HI_PCI0                         0x1E34
#define PCIIF_ACCESS_CTRL_BASE3_HI_PCI1                         0x1EB4
#define PCIIF_ACCESS_CTRL_SIZE3_PCI0                            0x1E38
#define PCIIF_ACCESS_CTRL_SIZE3_PCI1                            0x1EB8
#define PCIIF_ACCESS_CTRL_BASE4_LOW_PCI0                        0x1E40
#define PCIIF_ACCESS_CTRL_BASE4_LOW_PCI1                        0x1EC0
#define PCIIF_ACCESS_CTRL_BASE4_HI_PCI0                         0x1E44
#define PCIIF_ACCESS_CTRL_BASE4_HI_PCI1                         0x1EC4
#define PCIIF_ACCESS_CTRL_SIZE4_PCI0                            0x1E48
#define PCIIF_ACCESS_CTRL_SIZE4_PCI1                            0x1EC8
#define PCIIF_ACCESS_CTRL_BASE5_LOW_PCI0                        0x1E50
#define PCIIF_ACCESS_CTRL_BASE5_LOW_PCI1                        0x1ED0
#define PCIIF_ACCESS_CTRL_BASE5_HI_PCI0                         0x1E54
#define PCIIF_ACCESS_CTRL_BASE5_HI_PCI1                         0x1ED4
#define PCIIF_ACCESS_CTRL_SIZE5_PCI0                            0x1E58
#define PCIIF_ACCESS_CTRL_SIZE5_PCI1                            0x1ED8
#define PCIIF_SERR_MASK_PCI0                                    0xC28
#define PCIIF_SERR_MASK_PCI1                                    0xCA8
#define PCIIF_CFG_ADDR_PCI0                                     0xCF8
#define PCIIF_CFG_ADDR_PCI1                                     0xC78
#define PCIIF_CFG_DATA_PCI0                                     0xCFC
#define PCIIF_CFG_DATA_PCI1                                     0xC7C
#define PCIIF_INTERRUPT_ACK_PCI0                                0xC34
#define PCIIF_INTERRUPT_ACK_PCI1                                0xCB4
#define PCIIF_INTERRUPT_CAUSE_PCI0                              0x1D58
#define PCIIF_INTERRUPT_CAUSE_PCI1                              0x1DD8
#define PCIIF_INTERRUPT_MASK_PCI0                               0x1D5C
#define PCIIF_INTERRUPT_MASK_PCI1                               0x1DDC
#define PCIIF_ERR_ADDR_LOW_PCI0                                 0x1D40
#define PCIIF_ERR_ADDR_LOW_PCI1                                 0x1DC0
#define PCIIF_ERR_ADDR_HI_PCI0                                  0x1D44
#define PCIIF_ERR_ADDR_HI_PCI1                                  0x1DC4
#define PCIIF_ERR_ATTR_PCI0                                     0x1D48
#define PCIIF_ERR_ATTR_PCI1                                     0x1DC8
#define PCIIF_ERR_CMD_PCI0                                      0x1D50
#define PCIIF_ERR_CMD_PCI1                                      0x1DD0

/*
 * The following registers are in the function 0 header space of the
 * MV64360 and can be modified via PCI config cycles.  Following the
 * definitions for function 0 registers are the definitions for
 * function 1, function 2, function 3, and function 4.  Any register
 * whose mnemonic ends in "PCICFG" or "PCICFG_ALT"  represents a
 * register that must be accessed via PCI configuration cycles.  In
 * general the register whose mnemonic ends with "...PCICFG",
 * represents an access on the same PCI bus (e.g.  accessing PCI1
 * registers with the PCI1 configuration accesses or accessing PCI0 
 * registers with the PCI0 config registers.  If the register mnemonic 
 * ends with PCICFG_ALT, the access is to the alternate PCI bus (PCI0 
 * accesses of PCI1 registers and vice versa).
 */

#define PCIIF_DEVICE_AND_VENDOR_ID_PCICFG                       0x00
#define PCIIF_DEVICE_AND_VENDOR_ID_PCICFG_ALT                   0x80
#define PCIIF_STATUS_AND_CMD_PCICFG                             0x04
#define PCIIF_STATUS_AND_CMD_PCICFG_ALT                         0x84
#define PCIIF_CLASS_CODE_AND_REVISION_ID_PCICFG                 0x08
#define PCIIF_CLASS_CODE_AND_REVISION_ID_PCICFG_ALT             0x88
#define PCIIF_BIST_LATENCY_CACHE_PCICFG                         0x0C
#define PCIIF_BIST_LATENCY_CACHE_PCICFG_ALT                     0x8C
#define PCIIF_CS0_BASE_ADDR_LOW_PCICFG                          0x10
#define PCIIF_CS0_BASE_ADDR_LOW_PCICFG_ALT                      0x90
#define PCIIF_CS0_BASE_ADDR_HI_PCICFG                           0x14
#define PCIIF_CS0_BASE_ADDR_HI_PCICFG_ALT                       0x94
#define PCIIF_CS1_BASE_ADDR_LOW_PCICFG                          0x18
#define PCIIF_CS1_BASE_ADDR_LOW_PCICFG_ALT                      0x98
#define PCIIF_CS1_BASE_ADDR_HI_PCICFG                           0x1C
#define PCIIF_CS1_BASE_ADDR_HI_PCICFG_ALT                       0x9C
#define PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG          0x20
#define PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG_ALT      0xA0
#define PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG           0x24
#define PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG_ALT       0xA4
#define PCIIF_SUBSYSTEM_DEVICE_AND_VENDOR_ID_PCICFG             0x2C
#define PCIIF_SUBSYSTEM_DEVICE_AND_VENDOR_ID_PCICFG_ALT         0xAC
#define PCIIF_EXPANSION_ROM_BASE_ADDR_PCICFG                    0x30
#define PCIIF_EXPANSION_ROM_BASE_ADDR_PCICFG_ALT                0xB0
#define PCIIF_CAPABILITY_LIST_PTR_PCICFG                        0x34
#define PCIIF_CAPABILITY_LIST_PTR_PCICFG_ALT                    0xB4
#define PCIIF_INTERRUPT_PIN_AND_LINE_PCICFG                     0x3C
#define PCIIF_INTERRUPT_PIN_AND_LINE_PCICFG_ALT                 0xBC
#define PCIIF_POWER_MGMT_PCICFG                                 0x40
#define PCIIF_POWER_MGMT_PCICFG_ALT                             0xC0
#define PCIIF_POWER_MGMT_CTRL_AND_STATUS_PCICFG                 0x44
#define PCIIF_POWER_MGMT_CTRL_AND_STATUS_PCICFG_ALT             0xC4
#define PCIIF_VPD_ADDR_PCICFG                                   0x48
#define PCIIF_VPD_DATA_PCICFG                                   0x4C
#define PCIIF_VPD_ADDR_PCICFG_ALT                               0xC8
#define PCIIF_VPD_DATA_PCICFG_ALT                               0xCC
#define PCIIF_MSI_MSG_CTRL_PCICFG                               0x50
#define PCIIF_MSI_MSG_CTRL_PCICFG_ALT                           0xD0
#define PCIIF_MSI_MSG_ADDR_PCICFG                               0x54
#define PCIIF_MSI_MSG_ADDR_PCICFG_ALT                           0xD4
#define PCIIF_MSI_MSG_UPPER_ADDR_PCICFG                         0x58
#define PCIIF_MSI_MSG_UPPER_ADDR_PCICFG_ALT                     0xD8
#define PCIIF_MSG_DATA_PCICFG                                   0x5C
#define PCIIF_MSG_DATA_PCICFG_ALT                               0xDC
#define PCIIF_PCI_X_CMD_PCICFG                                  0x60
#define PCIIF_PCI_X_CMD_PCICFG_ALT                              0xE0
#define PCIIF_PCI_X_STATUS_PCICFG                               0x64
#define PCIIF_PCI_X_STATUS_PCICFG_ALT                           0xE4
#define PCIIF_COMPACTPCI_HOTSWAP_PCICFG                         0x68
#define PCIIF_COMPACTPCI_HOTSWAP_PCICFG_ALT                     0xE8

/*
 * The following registers are in the function 1 header space of the
 * MV64360 and can be modified via PCI config cycles.  
 */

#define PCIIF_CS2_BASE_ADDR_LOW_PCICFG                          0x10
#define PCIIF_CS2_BASE_ADDR_LOW_PCICFG_ALT                      0x90
#define PCIIF_CS2_BASE_ADDR_HI_PCICFG                           0x14
#define PCIIF_CS2_BASE_ADDR_HI_PCICFG_ALT                       0x94
#define PCIIF_CS3_BASE_ADDR_LOW_PCICFG                          0x18
#define PCIIF_CS3_BASE_ADDR_LOW_PCICFG_ALT                      0x98
#define PCIIF_CS3_BASE_ADDR_HI_PCICFG                           0x1C
#define PCIIF_CS3_BASE_ADDR_HI_PCICFG_ALT                       0x9C
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_LOW_PCICFG              0x20
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_LOW_PCICFG_ALT          0xA0
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_HI_PCICFG               0x24
#define PCIIF_INTEGRATED_SRAM_BASE_ADDR_HI_PCICFG_ALT           0xA4

/*
 * The following registers are in the function 2 header space of the
 * MV64360 and can be modified via PCI config cycles.  
 */

#define PCIIF_DEVCS0_BASE_ADDR_LOW_PCICFG                       0x10
#define PCIIF_DEVCS0_BASE_ADDR_HI_PCICFG                        0x14
#define PCIIF_DEVCS0_BASE_ADDR_LOW_PCICFG_ALT                   0x90
#define PCIIF_DEVCS0_BASE_ADDR_HI_PCICFG_ALT                    0x94
#define PCIIF_DEVCS1_BASE_ADDR_LOW_PCICFG                       0x18
#define PCIIF_DEVCS1_BASE_ADDR_HI_PCICFG                        0x1C
#define PCIIF_DEVCS1_BASE_ADDR_LOW_PCICFG_ALT                   0x98
#define PCIIF_DEVCS1_BASE_ADDR_HI_PCICFG_ALT                    0x9C
#define PCIIF_DEVCS2_BASE_ADDR_LOW_PCICFG                       0x20
#define PCIIF_DEVCS2_BASE_ADDR_HI_PCICFG                        0x24
#define PCIIF_DEVCS2_BASE_ADDR_LOW_PCICFG_ALT                   0xA0
#define PCIIF_DEVCS2_BASE_ADDR_HI_PCICFG_ALT                    0xA4

/*
 * The following registers are in the function 3 header space of the
 * MV64360 and can be modified via PCI config cycles.  
 */

#define PCIIF_DEVCS3_BASE_ADDR_LOW_PCICFG                       0x10
#define PCIIF_DEVCS3_BASE_ADDR_HI_PCICFG                        0x14
#define PCIIF_DEVCS3_BASE_ADDR_LOW_PCICFG_ALT                   0x90
#define PCIIF_DEVCS3_BASE_ADDR_HI_PCICFG_ALT                    0x94
#define PCIIF_BOOTCS_BASE_ADDR_LOW_PCICFG                       0x18
#define PCIIF_BOOTCS_BASE_ADDR_HI_PCICFG                        0x1C
#define PCIIF_BOOTCS_BASE_ADDR_LOW_PCICFG_ALT                   0x98
#define PCIIF_BOOTCS_BASE_ADDR_HI_PCICFG_ALT                    0x9C
#define PCIIF_CPU_BASE_ADDR_LOW_PCICFG                          0x20
#define PCIIF_CPU_BASE_ADDR_LOW_PCICFG_ALT                      0xA0

/*
 * The following registers are in the function 4 header space of the
 * MV64360 and can be modified via PCI config cycles.  
 */


#define PCIIF_P2P_IO_BASE_ADDR_PCICFG                           0x20
#define PCIIF_P2P_IO_BASE_ADDR_PCICFG_ALT                       0xA0
#define PCIIF_P2P_MEM0_BASE_ADDR_LOW_PCICFG                     0x10
#define PCIIF_P2P_MEM0_BASE_ADDR_HI_PCICFG                      0x14
#define PCIIF_P2P_MEM0_BASE_ADDR_LOW_PCICFG_ALT                 0x90
#define PCIIF_P2P_MEM0_BASE_ADDR_HI_PCICFG_ALT                  0x94
#define PCIIF_P2P_MEM1_BASE_ADDR_LOW_PCICFG                     0x18
#define PCIIF_P2P_MEM1_BASE_ADDR_HI_PCICFG                      0x1C
#define PCIIF_P2P_MEM1_BASE_ADDR_LOW_PCICFG_ALT                 0x98
#define PCIIF_P2P_MEM1_BASE_ADDR_HI_PCICFG_ALT                  0x9C
#define PCIIF_CPU_BASE_ADDR_HI_PCICFG                           0x24
#define PCIIF_CPU_BASE_ADDR_HI_PCICFG_ALT                       0xA4
#define PCIIF_INTERNAL_IO_MAPPED_BASE_ADDR_PCICFG               0x24
#define PCIIF_INTERNAL_IO_MAPPED_BASE_ADDR_PCICFG_ALT           0xA4

/* Messaging Unit Interface Registers */

#define MUIF_INBND_MSG0_PCI0                                    0x1C10
#define MUIF_INBND_MSG0_PCI1                                    0x1C90
#define MUIF_INBND_MSG1_PCI0                                    0x1C14
#define MUIF_INBND_MSG1_PCI1                                    0x1C94
#define MUIF_OUTBND_MSG0_PCI0                                   0x1C18
#define MUIF_OUTBND_MSG0_PCI1                                   0x1C98
#define MUIF_OUTBND_MSG1_PCI0                                   0x1C1C
#define MUIF_OUTBND_MSG1_PCI1                                   0x1C9C
#define MUIF_INBND_DOORBELL_PCI0                                0x1C20
#define MUIF_INBND_DOORBELL_PCI1                                0x1CA0
#define MUIF_INBND_INTERRUPT_CAUSE_PCI0                         0x1C24
#define MUIF_INBND_INTERRUPT_CAUSE_PCI1                         0x1CA4
#define MUIF_INBND_INTERRUPT_MASK_PCI0                          0x1C28
#define MUIF_INBND_INTERRUPT_MASK_PCI1                          0x1CA8
#define MUIF_OUTBND_DOORBELL_PCI0                               0x1C2C
#define MUIF_OUTBND_DOORBELL_PCI1                               0x1CAC
#define MUIF_OUTBND_INTERRUPT_CAUSE_PCI0                        0x1C30
#define MUIF_OUTBND_INTERRUPT_CAUSE_PCI1                        0x1CB0
#define MUIF_OUTBND_INTERRUPT_MASK_PCI0                         0x1C34
#define MUIF_OUTBND_INTERRUPT_MASK_PCI1                         0x1CB4
#define MUIF_INBND_Q_PORT_VIRTUAL_PCI0                          0x1C40
#define MUIF_INBND_Q_PORT_VIRTUAL_PCI1                          0x1CC0
#define MUIF_OUTBND_Q_PORT_VIRTUAL_PCI0                         0x1C44
#define MUIF_OUTBND_Q_PORT_VIRTUAL_PCI1                         0x1CC4
#define MUIF_Q_CTRL_PCI0                                        0x1C50
#define MUIF_Q_CTRL_PCI1                                        0x1CD0
#define MUIF_INBND_POST_HEAD_PTR_PCI0                           0x1C68
#define MUIF_INBND_POST_HEAD_PTR_PCI1                           0x1CE8
#define MUIF_Q_BASE_ADDR_PCI0                                   0x1C54
#define MUIF_Q_BASE_ADDR_PCI1                                   0x1CD4
#define MUIF_INBND_FREE_HEAD_PTR_PCI0                           0x1C60
#define MUIF_INBND_FREE_HEAD_PTR_PCI1                           0x1CE0
#define MUIF_INBND_FREE_TAIL_PTR_PCI0                           0x1C64
#define MUIF_INBND_FREE_TAIL_PTR_PCI1                           0x1CE4
#define MUIF_INBND_POST_TAIL_PTR_PCI0                           0x1C6C
#define MUIF_INBND_POST_TAIL_PTR_PCI1                           0x1CEC
#define MUIF_OUTBND_FREE_HEAD_PTR_PCI0                          0x1C70
#define MUIF_OUTBND_FREE_HEAD_PTR_PCI1                          0x1CF0
#define MUIF_OUTBND_FREE_TAIL_PTR_PCI0                          0x1C74
#define MUIF_OUTBND_FREE_TAIL_PTR_PCI1                          0x1CF4
#define MUIF_OUTBND_POST_HEAD_PTR_PCI0                          0x1C78
#define MUIF_OUTBND_POST_HEAD_PTR_PCI1                          0x1CF8
#define MUIF_OUTBND_POST_TAIL_PTR_PCI0                          0x1C7C
#define MUIF_OUTBND_POST_TAIL_PTR_PCI1                          0x1CFC

/* Gigabit Ethernet Controller Interface Registers */

#define GE_PHY_ADDR_REG                                         0x2000
#define GE_SMI_REG                                              0x2004
#define GE_ENET_UNIT_DEFAULT_ADDR_REG                           0x2008
#define GE_ENET_UNIT_DEFAULT_ID_REG                             0x200C
#define GE_ENET_UNIT_INTERRUPT_CAUSE_REG                        0x2080
#define GE_ENET_UNIT_INTERRUPT_MASK_REG                         0x2084
#define GE_ENET_UNIT_ERR_ADDR_REG                               0x2094
#define GE_ENET_UNIT_INTERNAL_ADDR_ERR_REG                      0x2098
#define GE_BASE_ADDR_BA0                                        0x2200
#define GE_BASE_ADDR_BA1                                        0x2208
#define GE_BASE_ADDR_BA2                                        0x2210
#define GE_BASE_ADDR_BA3                                        0x2218
#define GE_BASE_ADDR_BA4                                        0x2220
#define GE_BASE_ADDR_BA5                                        0x2228
#define GE_SIZE_SR0                                             0x2204
#define GE_SIZE_SR1                                             0x220C
#define GE_SIZE_SR2                                             0x2214
#define GE_SIZE_SR3                                             0x221C
#define GE_SIZE_SR4                                             0x2224
#define GE_SIZE_SR5                                             0x222C
#define GE_HDR_RETARG_BASE_REG                                  0x2230
#define GE_HDR_RETARG_CTRL_REG                                  0x2234
#define GE_HI_ADDR_REMAP_HARR0                                  0x2280
#define GE_HI_ADDR_REMAP_HARR1                                  0x2284
#define GE_HI_ADDR_REMAP_HARR2                                  0x2288
#define GE_HI_ADDR_REMAP_HARR3                                  0x228C
#define GE_BASE_ADDR_ENABLE_REG                                 0x2290
#define GE_ENET_PORT_ACCESS_PROTECT_PORT0                       0x2294
#define GE_ENET_PORT_ACCESS_PROTECT_PORT1                       0x2298
#define GE_ENET_PORT_ACCESS_PROTECT_PORT2                       0x229C
#define GE_CFG_PORT0                                            0x2400
#define GE_CFG_PORT1                                            0x2800
#define GE_CFG_PORT2                                            0x2C00
#define GE_CFG_EXTEND_PORT0                                     0x2404
#define GE_CFG_EXTEND_PORT1                                     0x2804
#define GE_CFG_EXTEND_PORT2                                     0x2C04
#define GE_MII_SERIAL_PARAMS_PORT0                              0x2408
#define GE_MII_SERIAL_PARAMS_PORT1                              0x2808
#define GE_MII_SERIAL_PARAMS_PORT2                              0x2C08
#define GE_GMII_SERIAL_PARAMS_PORT0                             0x240C
#define GE_GMII_SERIAL_PARAMS_PORT1                             0x280C
#define GE_GMII_SERIAL_PARAMS_PORT2                             0x2C0C
#define GE_VLAN_ETHERTYPE_PORT0                                 0x2410
#define GE_VLAN_ETHERTYPE_PORT1                                 0x2810
#define GE_VLAN_ETHERTYPE_PORT2                                 0x2C10
#define GE_MAC_ADDR_LOW_PORT0                                   0x2414
#define GE_MAC_ADDR_LOW_PORT1                                   0x2814
#define GE_MAC_ADDR_LOW_PORT2                                   0x2C14
#define GE_MAC_ADDR_HI_PORT0                                    0x2418
#define GE_MAC_ADDR_HI_PORT1                                    0x2818
#define GE_MAC_ADDR_HI_PORT2                                    0x2C18
#define GE_SDMA_CFG_PORT0                                       0x241C
#define GE_SDMA_CFG_PORT1                                       0x281C
#define GE_SDMA_CFG_PORT2                                       0x2C1C
#define GE_IP_DIFF_SVCS_CODEPT_0_TO_PRIORITY_PORT0              0x2420
#define GE_IP_DIFF_SVCS_CODEPT_0_TO_PRIORITY_PORT1              0x2820
#define GE_IP_DIFF_SVCS_CODEPT_0_TO_PRIORITY_PORT2              0x2C20
#define GE_IP_DIFF_SVCS_CODEPT_1_TO_PRIORITY_PORT0              0x2424
#define GE_IP_DIFF_SVCS_CODEPT_1_TO_PRIORITY_PORT1              0x2824
#define GE_IP_DIFF_SVCS_CODEPT_1_TO_PRIORITY_PORT2              0x2C24
#define GE_IP_DIFF_SVCS_CODEPT_2_TO_PRIORITY_PORT0              0x2428
#define GE_IP_DIFF_SVCS_CODEPT_3_TO_PRIORITY_PORT0              0x242C
#define GE_IP_DIFF_SVCS_CODEPT_4_TO_PRIORITY_PORT0              0x2430
#define GE_IP_DIFF_SVCS_CODEPT_5_TO_PRIORITY_PORT0              0x2434
#define GE_IP_DIFF_SVCS_CODEPT_2_TO_PRIORITY_PORT1              0x2828
#define GE_IP_DIFF_SVCS_CODEPT_3_TO_PRIORITY_PORT1              0x282C
#define GE_IP_DIFF_SVCS_CODEPT_4_TO_PRIORITY_PORT1              0x2830
#define GE_IP_DIFF_SVCS_CODEPT_5_TO_PRIORITY_PORT1              0x2834
#define GE_IP_DIFF_SVCS_CODEPT_2_TO_PRIORITY_PORT2              0x2228
#define GE_IP_DIFF_SVCS_CODEPT_3_TO_PRIORITY_PORT2              0x2C2C
#define GE_IP_DIFF_SVCS_CODEPT_4_TO_PRIORITY_PORT2              0x2C30
#define GE_IP_DIFF_SVCS_CODEPT_5_TO_PRIORITY_PORT2              0x2C34
#define GE_IP_DIFF_SVCS_CODEPT_6_TO_PRIORITY_PORT0              0x2438
#define GE_IP_DIFF_SVCS_CODEPT_6_TO_PRIORITY_PORT1              0x2838
#define GE_IP_DIFF_SVCS_CODEPT_6_TO_PRIORITY_PORT2              0x2C38
#define GE_SERIAL_CTRL_PSCR_PORT0                               0x243C
#define GE_SERIAL_CTRL_PSCR_PORT1                               0x283C
#define GE_SERIAL_CTRL_PSCR_PORT2                               0x2C3C
#define GE_VLAN_PRIORITY_TAG_TO_PRIORITY_VPT2P_PORT0            0x2440
#define GE_VLAN_PRIORITY_TAG_TO_PRIORITY_VPT2P_PORT1            0x2840
#define GE_VLAN_PRIORITY_TAG_TO_PRIORITY_VPT2P_PORT2            0x2C40
#define GE_ENET_PORT_STATUS_PSR_PORT0                           0x2444
#define GE_ENET_PORT_STATUS_PSR_PORT1                           0x2844
#define GE_ENET_PORT_STATUS_PSR_PORT2                           0x2C44
#define GE_XMIT_Q_CMD_PORT0                                     0x2448
#define GE_XMIT_Q_CMD_PORT1                                     0x2848
#define GE_XMIT_Q_CMD_PORT2                                     0x2C48
#define GE_XMIT_Q_FIXED_PRIORITY_CFG_PORT0                     0x244C
#define GE_XMIT_Q_FIXED_PRIORITY_CFG_PORT1                     0x284C
#define GE_XMIT_Q_FIXED_PRIORITY_CFG_PORT2                     0x2C4C
#define GE_XMIT_TKN_BKT_RATE_CFG_PORT0                         0x2450
#define GE_XMIT_TKN_BKT_RATE_CFG_PORT1                         0x2850
#define GE_XMIT_TKN_BKT_RATE_CFG_PORT2                         0x2C50
#define GE_MAXIMUM_XMIT_UNIT_PORT0                              0x2458
#define GE_MAXIMUM_XMIT_UNIT_PORT1                              0x2858
#define GE_MAXIMUM_XMIT_UNIT_PORT2                              0x2C58
#define GE_MAXIMUM_TKN_BKT_SIZE_PORT0                           0x245C
#define GE_MAXIMUM_TKN_BKT_SIZE_PORT1                           0x285C
#define GE_MAXIMUM_TKN_BKT_SIZE_PORT2                           0x2C5C
#define GE_INTERRUPT_CAUSE_PORT0                                0x2460
#define GE_INTERRUPT_CAUSE_PORT1                                0x2860
#define GE_INTERRUPT_CAUSE_PORT2                                0x2C60
#define GE_INTERRUPT_CAUSE_EXTEND_PORT0                         0x2464
#define GE_INTERRUPT_CAUSE_EXTEND_PORT1                         0x2864
#define GE_INTERRUPT_CAUSE_EXTEND_PORT2                         0x2C64
#define GE_INTERRUPT_MASK_PIMR_PORT0                            0x2468
#define GE_INTERRUPT_MASK_PIMR_PORT1                            0x2868
#define GE_INTERRUPT_MASK_PIMR_PORT2                            0x2C68
#define GE_EXTEND_INTERRUPT_MASK_PORT0                          0x246C
#define GE_EXTEND_INTERRUPT_MASK_PORT1                          0x286C
#define GE_EXTEND_INTERRUPT_MASK_PORT2                          0x2C6C
#define GE_RX_FIFO_URGENT_THRESHOLD_PORT0                       0x2470
#define GE_RX_FIFO_URGENT_THRESHOLD_PORT1                       0x2870
#define GE_RX_FIFO_URGENT_THRESHOLD_PORT2                       0x2C70
#define GE_TX_FIFO_URGENT_THRESHOLD_PORT0                       0x2474
#define GE_TX_FIFO_URGENT_THRESHOLD_PORT1                       0x2874
#define GE_TX_FIFO_URGENT_THRESHOLD_PORT2                       0x2C74
#define GE_RX_MINIMAL_FRAME_SIZE_PXMFSR_PORT0                   0x247C
#define GE_RX_MINIMAL_FRAME_SIZE_PXMFSR_PORT1                   0x287C
#define GE_RX_MINIMAL_FRAME_SIZE_PXMFSR_PORT2                   0x2C7C
#define GE_RX_DISCARD_FRAME_CTR_PORT0                           0x2484
#define GE_RX_DISCARD_FRAME_CTR_PORT1                           0x2884
#define GE_RX_DISCARD_FRAME_CTR_PORT2                           0x2C84
#define GE_OVERRUN_FRAME_CTR_PORT0                              0x2488
#define GE_OVERRUN_FRAME_CTR_PORT1                              0x2888
#define GE_OVERRUN_FRAME_CTR_PORT2                              0x2C88
#define GE_INTERNAL_ADDR_ERR_PORT0                              0x2494
#define GE_INTERNAL_ADDR_ERR_PORT1                              0x2894
#define GE_INTERNAL_ADDR_ERR_PORT2                              0x2C94
#define GE_ENET_CURR_RCV_DESC_PTR_Q0_PORT0                      0x260C
#define GE_ENET_CURR_RCV_DESC_PTR_Q1_PORT0                      0x261C
#define GE_ENET_CURR_RCV_DESC_PTR_Q2_PORT0                      0x262C
#define GE_ENET_CURR_RCV_DESC_PTR_Q3_PORT0                      0x263C
#define GE_ENET_CURR_RCV_DESC_PTR_Q4_PORT0                      0x264C
#define GE_ENET_CURR_RCV_DESC_PTR_Q5_PORT0                      0x265C
#define GE_ENET_CURR_RCV_DESC_PTR_Q6_PORT0                      0x266C
#define GE_ENET_CURR_RCV_DESC_PTR_Q7_PORT0                      0x267C
#define GE_ENET_CURR_RCV_DESC_PTR_Q0_PORT1                      0x2A0C
#define GE_ENET_CURR_RCV_DESC_PTR_Q1_PORT1                      0x2A1C
#define GE_ENET_CURR_RCV_DESC_PTR_Q2_PORT1                      0x2A2C
#define GE_ENET_CURR_RCV_DESC_PTR_Q3_PORT1                      0x2A3C
#define GE_ENET_CURR_RCV_DESC_PTR_Q4_PORT1                      0x2A4C
#define GE_ENET_CURR_RCV_DESC_PTR_Q5_PORT1                      0x2A5C
#define GE_ENET_CURR_RCV_DESC_PTR_Q6_PORT1                      0x2A6C
#define GE_ENET_CURR_RCV_DESC_PTR_Q7_PORT1                      0x2A7C
#define GE_ENET_CURR_RCV_DESC_PTR_Q0_PORT2                      0x2E0C
#define GE_ENET_CURR_RCV_DESC_PTR_Q1_PORT2                      0x2E1C
#define GE_ENET_CURR_RCV_DESC_PTR_Q2_PORT2                      0x2E2C
#define GE_ENET_CURR_RCV_DESC_PTR_Q3_PORT2                      0x2E3C
#define GE_ENET_CURR_RCV_DESC_PTR_Q4_PORT2                      0x2E4C
#define GE_ENET_CURR_RCV_DESC_PTR_Q5_PORT2                      0x2E5C
#define GE_ENET_CURR_RCV_DESC_PTR_Q6_PORT2                      0x2E6C
#define GE_ENET_CURR_RCV_DESC_PTR_Q7_PORT2                      0x2E7C
#define GE_RCV_Q_CMD_PORT0                                      0x2680
#define GE_RCV_Q_CMD_PORT1                                      0x2A80
#define GE_RCV_Q_CMD_PORT2                                      0x2E80
#define GE_XMIT_CURR_SERVED_DESC_PTR_PORT0                      0x2684
#define GE_XMIT_CURR_SERVED_DESC_PTR_PORT1                      0x2A84
#define GE_XMIT_CURR_SERVED_DESC_PTR_PORT2                      0x2E84
#define GE_XMIT_CURR_Q_DESC_PTR_Q0_PORT0                        0x26C0
#define GE_XMIT_CURR_Q_DESC_PTR_Q1_PORT0                        0x26C4
#define GE_XMIT_CURR_Q_DESC_PTR_Q2_PORT0                        0x26C8
#define GE_XMIT_CURR_Q_DESC_PTR_Q3_PORT0                        0x26CC
#define GE_XMIT_CURR_Q_DESC_PTR_Q4_PORT0                        0x26D0
#define GE_XMIT_CURR_Q_DESC_PTR_Q5_PORT0                        0x26D4
#define GE_XMIT_CURR_Q_DESC_PTR_Q6_PORT0                        0x26D8
#define GE_XMIT_CURR_Q_DESC_PTR_Q7_PORT0                        0x26DC
#define GE_XMIT_CURR_Q_DESC_PTR_Q0_PORT1                        0x2AC0
#define GE_XMIT_CURR_Q_DESC_PTR_Q1_PORT1                        0x2AC4
#define GE_XMIT_CURR_Q_DESC_PTR_Q2_PORT1                        0x2AC8
#define GE_XMIT_CURR_Q_DESC_PTR_Q3_PORT1                        0x2ACC
#define GE_XMIT_CURR_Q_DESC_PTR_Q4_PORT1                        0x2AD0
#define GE_XMIT_CURR_Q_DESC_PTR_Q5_PORT1                        0x2AD4
#define GE_XMIT_CURR_Q_DESC_PTR_Q6_PORT1                        0x2AD8
#define GE_XMIT_CURR_Q_DESC_PTR_Q7_PORT1                        0x2ADC
#define GE_XMIT_CURR_Q_DESC_PTR_Q0_PORT2                        0x2EC0
#define GE_XMIT_CURR_Q_DESC_PTR_Q1_PORT2                        0x2EC4
#define GE_XMIT_CURR_Q_DESC_PTR_Q2_PORT2                        0x2EC8
#define GE_XMIT_CURR_Q_DESC_PTR_Q3_PORT2                        0x2ECC
#define GE_XMIT_CURR_Q_DESC_PTR_Q4_PORT2                        0x2ED0
#define GE_XMIT_CURR_Q_DESC_PTR_Q5_PORT2                        0x2ED4
#define GE_XMIT_CURR_Q_DESC_PTR_Q6_PORT2                        0x2ED8
#define GE_XMIT_CURR_Q_DESC_PTR_Q7_PORT2                        0x2EDC
#define GE_XMIT_Q_TKN_BKT_CTR_Q0_PORT0                          0x2700
#define GE_XMIT_Q_TKN_BKT_CTR_Q1_PORT0                          0x2710
#define GE_XMIT_Q_TKN_BKT_CTR_Q2_PORT0                          0x2720
#define GE_XMIT_Q_TKN_BKT_CTR_Q3_PORT0                          0x2730
#define GE_XMIT_Q_TKN_BKT_CTR_Q4_PORT0                          0x2740
#define GE_XMIT_Q_TKN_BKT_CTR_Q5_PORT0                          0x2750
#define GE_XMIT_Q_TKN_BKT_CTR_Q6_PORT0                          0x2760
#define GE_XMIT_Q_TKN_BKT_CTR_Q7_PORT0                          0x2770
#define GE_XMIT_Q_TKN_BKT_CTR_Q0_PORT1                          0x2B00
#define GE_XMIT_Q_TKN_BKT_CTR_Q1_PORT1                          0x2B10
#define GE_XMIT_Q_TKN_BKT_CTR_Q2_PORT1                          0x2B20
#define GE_XMIT_Q_TKN_BKT_CTR_Q3_PORT1                          0x2B30
#define GE_XMIT_Q_TKN_BKT_CTR_Q4_PORT1                          0x2B40
#define GE_XMIT_Q_TKN_BKT_CTR_Q5_PORT1                          0x2B50
#define GE_XMIT_Q_TKN_BKT_CTR_Q6_PORT1                          0x2B60
#define GE_XMIT_Q_TKN_BKT_CTR_Q7_PORT1                          0x2B70
#define GE_XMIT_Q_TKN_BKT_CTR_Q0_PORT2                          0x2F00
#define GE_XMIT_Q_TKN_BKT_CTR_Q1_PORT2                          0x2F10
#define GE_XMIT_Q_TKN_BKT_CTR_Q2_PORT2                          0x2F20
#define GE_XMIT_Q_TKN_BKT_CTR_Q3_PORT2                          0x2F30
#define GE_XMIT_Q_TKN_BKT_CTR_Q4_PORT2                          0x2F40
#define GE_XMIT_Q_TKN_BKT_CTR_Q5_PORT2                          0x2F50
#define GE_XMIT_Q_TKN_BKT_CTR_Q6_PORT2                          0x2F60
#define GE_XMIT_Q_TKN_BKT_CTR_Q7_PORT2                          0x2F70
#define GE_XMIT_Q_TKN_BKT_CFG_Q0_PORT0                         0x2704
#define GE_XMIT_Q_TKN_BKT_CFG_Q1_PORT0                         0x2714
#define GE_XMIT_Q_TKN_BKT_CFG_Q2_PORT0                         0x2724
#define GE_XMIT_Q_TKN_BKT_CFG_Q3_PORT0                         0x2734
#define GE_XMIT_Q_TKN_BKT_CFG_Q4_PORT0                         0x2744
#define GE_XMIT_Q_TKN_BKT_CFG_Q5_PORT0                         0x2754
#define GE_XMIT_Q_TKN_BKT_CFG_Q6_PORT0                         0x2764
#define GE_XMIT_Q_TKN_BKT_CFG_Q7_PORT0                         0x2774
#define GE_XMIT_Q_TKN_BKT_CFG_Q0_PORT1                         0x2B04
#define GE_XMIT_Q_TKN_BKT_CFG_Q1_PORT1                         0x2B14
#define GE_XMIT_Q_TKN_BKT_CFG_Q2_PORT1                         0x2B24
#define GE_XMIT_Q_TKN_BKT_CFG_Q3_PORT1                         0x2B34
#define GE_XMIT_Q_TKN_BKT_CFG_Q4_PORT1                         0x2B44
#define GE_XMIT_Q_TKN_BKT_CFG_Q5_PORT1                         0x2B54
#define GE_XMIT_Q_TKN_BKT_CFG_Q6_PORT1                         0x2B64
#define GE_XMIT_Q_TKN_BKT_CFG_Q7_PORT1                         0x2B74
#define GE_XMIT_Q_TKN_BKT_CFG_Q0_PORT2                         0x2F04
#define GE_XMIT_Q_TKN_BKT_CFG_Q1_PORT2                         0x2F14
#define GE_XMIT_Q_TKN_BKT_CFG_Q2_PORT2                         0x2F24
#define GE_XMIT_Q_TKN_BKT_CFG_Q3_PORT2                         0x2F34
#define GE_XMIT_Q_TKN_BKT_CFG_Q4_PORT2                         0x2F44
#define GE_XMIT_Q_TKN_BKT_CFG_Q5_PORT2                         0x2F54
#define GE_XMIT_Q_TKN_BKT_CFG_Q6_PORT2                         0x2F64
#define GE_XMIT_Q_TKN_BKT_CFG_Q7_PORT2                         0x2F74
#define GE_XMIT_Q_ARB_CFG_Q0_PORT0                             0x2708
#define GE_XMIT_Q_ARB_CFG_Q1_PORT0                             0x2718
#define GE_XMIT_Q_ARB_CFG_Q2_PORT0                             0x2728
#define GE_XMIT_Q_ARB_CFG_Q3_PORT0                             0x2738
#define GE_XMIT_Q_ARB_CFG_Q4_PORT0                             0x2748
#define GE_XMIT_Q_ARB_CFG_Q5_PORT0                             0x2758
#define GE_XMIT_Q_ARB_CFG_Q6_PORT0                             0x2768
#define GE_XMIT_Q_ARB_CFG_Q7_PORT0                             0x2778
#define GE_XMIT_Q_ARB_CFG_Q0_PORT1                             0x2B08
#define GE_XMIT_Q_ARB_CFG_Q1_PORT1                             0x2B18
#define GE_XMIT_Q_ARB_CFG_Q2_PORT1                             0x2B28
#define GE_XMIT_Q_ARB_CFG_Q3_PORT1                             0x2B38
#define GE_XMIT_Q_ARB_CFG_Q4_PORT1                             0x2B48
#define GE_XMIT_Q_ARB_CFG_Q5_PORT1                             0x2B58
#define GE_XMIT_Q_ARB_CFG_Q6_PORT1                             0x2B68
#define GE_XMIT_Q_ARB_CFG_Q7_PORT1                             0x2B78
#define GE_XMIT_Q_ARB_CFG_Q0_PORT2                             0x2F08
#define GE_XMIT_Q_ARB_CFG_Q1_PORT2                             0x2F18
#define GE_XMIT_Q_ARB_CFG_Q2_PORT2                             0x2F28
#define GE_XMIT_Q_ARB_CFG_Q3_PORT2                             0x2F38
#define GE_XMIT_Q_ARB_CFG_Q4_PORT2                             0x2F48
#define GE_XMIT_Q_ARB_CFG_Q5_PORT2                             0x2F58
#define GE_XMIT_Q_ARB_CFG_Q6_PORT2                             0x2F68
#define GE_XMIT_Q_ARB_CFG_Q7_PORT2                             0x2F78
#define GE_XMIT_TKN_BKT_CTR_PORT0                              0x2780
#define GE_XMIT_TKN_BKT_CTR_PORT1                              0x2B80
#define GE_XMIT_TKN_BKT_CTR_PORT2                              0x2F80
#define GE_DEST_ADDR_FILTER_SPCL_MCAST_TBL_PORT0               0x3400
#define GE_DEST_ADDR_FILTER_SPCL_MCAST_TBL_PORT1               0x3800
#define GE_DEST_ADDR_FILTER_SPCL_MCAST_TBL_PORT2               0x3C00
#define GE_DEST_ADDR_FILTER_OTHER_MCAST_TBL_PORT0              0x3500
#define GE_DEST_ADDR_FILTER_OTHER_MCAST_TBL_PORT1              0x3900
#define GE_DEST_ADDR_FILTER_OTHER_MCAST_TBL_PORT2              0x3D00
#define GE_DEST_ADDR_FILTER_UNICAST_TBL_PORT0                  0x3600
#define GE_DEST_ADDR_FILTER_UNICAST_TBL_PORT1                  0x3A00
#define GE_DEST_ADDR_FILTER_UNICAST_TBL_PORT2                  0x3E00
#define GE_MAC_MIB_CTRS_PORT0                                  0x3000
#define GE_MAC_MIB_CTRS_PORT1                                  0x3080
#define GE_MAC_MIB_CTRS_PORT2                                  0x3100

/* Communication Unit Interface Registers */

#define CUI_BASE_ADDR0                                         0xF200
#define CUI_BASE_ADDR1                                         0xF208
#define CUI_BASE_ADDR2                                         0xF210
#define CUI_BASE_ADDR3                                         0xF218
#define CUI_SZ_REG0                                            0xF204
#define CUI_SZ_REG1                                            0xF20C
#define CUI_SZ_REG2                                            0xF214
#define CUI_SZ_REG3                                            0xF21C
#define CUI_HI_ADDR_RMAP0                                      0xFD40
#define CUI_HI_ADDR_RMAP1                                      0xFD44
#define CUI_BASE_ADDR_ENB                                      0xF250
#define CUI_ARB_CTRL                                           0xF300
#define CUI_MPSC0                                              0xF254
#define CUI_MPSC1                                              0xF258
#define CUI_INTRNL_SPACE_BASE_ADDR                             0xF25C
#define CUI_INT_CAUSE                                          0xF310
#define CUI_CFG                                                0xB40C
#define CUI_XBAR_TMOUT                                         0xF304
#define CUI_INT_MASK                                           0xF314
#define CUI_MPSC_ROUTG                                         0xB400
#define CUI_RX_CLK_ROUTG                                       0xB404
#define CUI_ERR_ADDR                                           0xF318
#define CUI_MPSC_CAUSE0                                        0xB804
#define CUI_MPSC_CAUSE1                                        0xB80C
#define CUI_TX_CLK_ROUTG                                       0xB408
#define CUI_MPSC_MASK0                                         0xB884
#define CUI_MPSC_MASK1                                         0xB88C
#define CUI_MPSCX_MAIN_CFG0_LO                                 0x8000
#define CUI_MPSCX_MAIN_CFG1_LO                                 0x9000
#define CUI_MPSCX_MAIN_CFG0_HI                                 0x8004
#define CUI_MPSCX_MAIN_CFG1_HI                                 0x9004
#define CUI_SDMA_CFG_CHAN0                                     0x4000
#define CUI_SDMA_CFG_CHAN1                                     0x6000
#define CUI_SDMA_CMD_CHAN0                                     0x4008
#define CUI_SDMA_CMD_CHAN1                                     0x6008
#define CUI_SDMA_CAUSE                                         0xB800
#define CUI_SDMA_MASK                                          0xB880

/* Baud Rate Generators (BRG) Registers */

#define BRG_0_CFG_REG                                          0xB200
#define BRG_1_CFG_REG                                          0xB208
#define BRG_0_BAUD_TUNING_REG                                  0xB204
#define BRG_1_BAUD_TUNING_REG                                  0xB20C
#define BRG_CAUSE_REG                                          0xB834
#define BRG_MASK_REG                                           0xB8B4

/* IDMA Controller Interface Registers  */

#define IDMA_BYTE_COUNT_CHAN0                                  0x800
#define IDMA_BYTE_COUNT_CHAN1                                  0x804
#define IDMA_BYTE_COUNT_CHAN2                                  0x808
#define IDMA_BYTE_COUNT_CHAN3                                  0x80C
#define IDMA_SRC_ADDR_CHAN0                                    0x810
#define IDMA_SRC_ADDR_CHAN1                                    0x814
#define IDMA_SRC_ADDR_CHAN2                                    0x818
#define IDMA_SRC_ADDR_CHAN3                                    0x81C
#define IDMA_DEST_ADDR_CHAN0                                   0x820
#define IDMA_DEST_ADDR_CHAN1                                   0x824
#define IDMA_DEST_ADDR_CHAN2                                   0x828
#define IDMA_DEST_ADDR_CHAN3                                   0x82C
#define IDMA_NEXT_DESC_PTR_CHAN0                               0x830
#define IDMA_NEXT_DESC_PTR_CHAN1                               0x834
#define IDMA_NEXT_DESC_PTR_CHAN2                               0x838
#define IDMA_NEXT_DESC_PTR_CHAN3                               0x83C
#define IDMA_CURR_DESC_PTR_CHAN0                               0x870
#define IDMA_CURR_DESC_PTR_CHAN1                               0x874
#define IDMA_CURR_DESC_PTR_CHAN2                               0x878
#define IDMA_CURR_DESC_PTR_CHAN3                               0x87C
#define IDMA_BASE_ADDR_REG0                                    0xA00
#define IDMA_BASE_ADDR_REG1                                    0xA08
#define IDMA_BASE_ADDR_REG2                                    0xA10
#define IDMA_BASE_ADDR_REG3                                    0xA18
#define IDMA_BASE_ADDR_REG4                                    0xA20
#define IDMA_BASE_ADDR_REG5                                    0xA28
#define IDMA_BASE_ADDR_REG6                                    0xA30
#define IDMA_BASE_ADDR_REG7                                    0xA38
#define IDMA_SIZE_REG0                                         0xA04
#define IDMA_SIZE_REG1                                         0xA0C
#define IDMA_SIZE_REG2                                         0xA14
#define IDMA_SIZE_REG3                                         0xA1C
#define IDMA_SIZE_REG4                                         0xA24
#define IDMA_SIZE_REG5                                         0xA2C
#define IDMA_SIZE_REG6                                         0xA34
#define IDMA_SIZE_REG7                                         0xA3C
#define IDMA_HI_ADDR_REMAP0                                    0xA60
#define IDMA_HI_ADDR_REMAP1                                    0xA64
#define IDMA_HI_ADDR_REMAP2                                    0xA68
#define IDMA_HI_ADDR_REMAP3                                    0xA6C
#define IDMA_BASE_ADDR_ENABLE                                  0xA80
#define IDMA_CHAN0_ACCESS_PROTECT                              0xA70
#define IDMA_CHAN1_ACCESS_PROTECT                              0xA74
#define IDMA_CHAN2_ACCESS_PROTECT                              0xA78
#define IDMA_CHAN3_ACCESS_PROTECT                              0xA7C
#define IDMA_HDRS_RETARG_CTRL                                  0xA84
#define IDMA_CHAN0_CTRL_LOW                                    0x840
#define IDMA_CHAN1_CTRL_LOW                                    0x844
#define IDMA_CHAN2_CTRL_LOW                                    0x848
#define IDMA_CHAN3_CTRL_LOW                                    0x84C
#define IDMA_HDR_RETARG_BASE                                   0xA88
#define IDMA_CHAN0_CTRL_HI                                     0x880
#define IDMA_CHAN1_CTRL_HI                                     0x884
#define IDMA_CHAN2_CTRL_HI                                     0x888
#define IDMA_CHAN3_CTRL_HI                                     0x88C
#define IDMA_ARB_CTRL                                          0x860
#define IDMA_INTERRUPT_CAUSE                                   0x8C0
#define IDMA_INTERRUPT_MASK                                    0x8C4
#define IDMA_CROSSBAR_TIMEOUT                                  0x8D0
#define IDMA_ERR_ADDR                                          0x8C8
#define IDMA_ERR_SELECT                                        0x8CC

/* Timer/Counters Registers */

#define TMR_CNTR0                                              0x850
#define TMR_CNTR1                                              0x854
#define TMR_CNTR2                                              0x858
#define TMR_CNTR3                                              0x85C
#define TMR_CNTR_CTRL                                          0x864
#define TMR_CNTR_INT_CAUSE                                     0x868
#define TMR_CNTR_INT_MASK                                      0x86C

/* Watchdog Timer */

#define WDOG_CFG_REG                                           0xB410
#define WDOG_VAL_REG                                           0xB414

/* I2C Interface Registers */

#define I2C_SLV_ADDR                                           0xC000
#define I2C_EXTENDED_SLV_ADDR                                  0xC010
#define I2C_DATA                                               0xC004
#define I2C_CTRL                                               0xC008
#define I2C_STATUS                                             0xC00C
#define I2C_BAUDE_RATE                                         0xC00C
#define I2C_SOFT_RESET                                         0xC01C

/* General Purpose Port Interface Registers */

#define GPP_IO_CTRL                                            0xF100
#define GPP_LEVEL_CTRL                                         0xF110
#define GPP_VAL                                                0xF104
#define GPP_INTERRUPT_CAUSE                                    0xF108
#define GPP_INTERRUPT_MASK0                                    0xF10C
#define GPP_INTERRUPT_MASK1                                    0xF114
#define GPP_VAL_SET                                            0xF118
#define GPP_VAL_CLEAR                                          0xF11C

/* Interrupt Controller Registers Interface */

#define ICI_MAIN_INTERRUPT_CAUSE_LOW                           0x004
#define ICI_MAIN_INTERRUPT_CAUSE_HI                            0x00C
#define ICI_CPU_INT0_MASK_LOW                                  0x014
#define ICI_CPU_INT0_MASK_HI                                   0x01C
#define ICI_CPU_INT0_SELECT_CAUSE                              0x024
#define ICI_CPU_INT1_MASK_LOW                                  0x034
#define ICI_CPU_INT1_MASK_HI                                   0x03C
#define ICI_CPU_INT1_SELECT_CAUSE                              0x044
#define ICI_INT0_MASK_LOW                                      0x054
#define ICI_INT0_MASK_HI                                       0x05C
#define ICI_INT0_SELECT_CAUSE                                  0x064
#define ICI_INT1_MASK_LOW                                      0x074
#define ICI_INT1_MASK_HI                                       0x07C
#define ICI_INT1_SELECT_CAUSE                                  0x084

/* Pins Multiplexing Interface Registers */

#define MPP_CTRL0                                              0xF000
#define MPP_CTRL1                                              0xF004
#define MPP_CTRL2                                              0xF008
#define MPP_CTRL3                                              0xF00C

/* Serial Initialization Interface Registers */

#define SII_SERIAL_INIT_LAST_DATA                              0xF324
#define SII_SERIAL_INIT_CTRL                                   0xF328
#define SII_SERIAL_INIT_STATUS                                 0xF32C

/*
 * Following are 'shift' positions for various fields or bits within
 * some of the mv64360 registers.
 */

/* CPU to PCI - base address enable bit postions & masks */

#define CPUIF_BASE_ADDR_ENABLE_CS0_BIT		0
#define CPUIF_BASE_ADDR_ENABLE_CS1_BIT		1
#define CPUIF_BASE_ADDR_ENABLE_CS2_BIT		2
#define CPUIF_BASE_ADDR_ENABLE_CS3_BIT		3

#define CPUIF_BASE_ADDR_ENABLE_CS_MASK          0x0000000F
#define CPUIF_BASE_ADDR_ENABLE_CS_0_MASK        (1 << \
						 CPUIF_BASE_ADDR_ENABLE_CS0_BIT)
#define CPUIF_BASE_ADDR_ENABLE_CS_1_MASK        (1 << \
						 CPUIF_BASE_ADDR_ENABLE_CS1_BIT)
#define CPUIF_BASE_ADDR_ENABLE_CS_2_MASK        (1 << \
						 CPUIF_BASE_ADDR_ENABLE_CS2_BIT)
#define CPUIF_BASE_ADDR_ENABLE_CS_3_MASK        (1 << \
						 CPUIF_BASE_ADDR_ENABLE_CS3_BIT)

#define CPUIF_BASE_ADDR_ENABLE_PCI0_IO_BIT	9
#define CPUIF_BASE_ADDR_ENABLE_PCI0_MEM0_BIT	10
#define CPUIF_BASE_ADDR_ENABLE_PCI0_MEM1_BIT	11
#define CPUIF_BASE_ADDR_ENABLE_PCI0_MEM2_BIT	12
#define CPUIF_BASE_ADDR_ENABLE_PCI0_MEM3_BIT	13
#define CPUIF_BASE_ADDR_ENABLE_PCI1_IO_BIT	14
#define CPUIF_BASE_ADDR_ENABLE_PCI1_MEM0_BIT	15
#define CPUIF_BASE_ADDR_ENABLE_PCI1_MEM1_BIT	16
#define CPUIF_BASE_ADDR_ENABLE_PCI1_MEM2_BIT	17
#define CPUIF_BASE_ADDR_ENABLE_PCI1_MEM3_BIT	18
#define CPUIF_BASE_ADDR_ENABLE_INTEGRATED_SRAM_BIT 19
#define CPUIF_BASE_ADDR_ENABLE_INTRNL_SPACE_BIT 20

/* CPU to DEV bus 0 base address enable bit positions */

#define CPUIF_BASE_ADDR_ENABLE_DEVCS0_BIT	4
#define CPUIF_BASE_ADDR_ENABLE_DEVCS1_BIT	5
#define CPUIF_BASE_ADDR_ENABLE_DEVCS2_BIT	6
#define CPUIF_BASE_ADDR_ENABLE_DEVCS3_BIT	7
#define CPUIF_BASE_ADDR_ENABLE_BOOTCS_BIT	8

/* PCI to CSx - base address enable bit positions */

#define PCIIF_BASE_ADDR_ENABLE_CS0_BIT	0
#define PCIIF_BASE_ADDR_ENABLE_CS1_BIT	1
#define PCIIF_BASE_ADDR_ENABLE_CS2_BIT	2
#define PCIIF_BASE_ADDR_ENABLE_CS3_BIT	3
#define PCIIF_BASE_ADDR_ENABLE_INTMEM_BIT 9

/* SDRAM Configuration Register Definitions */

#define DDR_SDRAM_CFG_REGISTERED_BIT    17
#define DDR_SDRAM_CFG_REGISTERED_MASK   (1 << \
                                         DDR_SDRAM_CFG_REGISTERED_BIT)
#define DDR_SDRAM_CFG_ECC_BIT           18
#define DDR_SDRAM_CFG_ECC_MASK          (1 << \
                                         DDR_SDRAM_CFG_ECC_BIT)
#define DDR_SDRAM_CFG_DQS_MASK          0x00300000
#define DDR_SDRAM_CFG_DQS_X32           0x00000000
#define DDR_SDRAM_CFG_DQS_X8_X16        0x00200000
#define DDR_SDRAM_CFG_DQS_X4            0x00300000

/* DUnit Control (Low) Register Definitions */

#define DDR_DUNIT_CTRL_LO_CLKSYNC_BIT		0
#define DDR_DUNIT_CTRL_LO_CLKSYNC_MASK		(1 << \
						 DDR_DUNIT_CTRL_LO_CLKSYNC_BIT)
#define DDR_DUNIT_CTRL_LO_RDPIPE_BIT		7
#define DDR_DUNIT_CTRL_LO_RDSYNCEN_BIT		8
#define DDR_DUNIT_CTRL_LO_RMWSYNCEN_BIT		9
#define DDR_DUNIT_CTRL_LO_STBURSTDEL_BIT	24
#define DDR_DUNIT_CTRL_LO_STBURSTNEG_BIT	27
#define DDR_DUNIT_CTRL_LO_STBURSTSCR_BIT	28
#define DDR_DUNIT_CTRL_LO_RDDATANEG_BIT		29
#define DDR_DUNIT_CTRL_LO_RST_SAMPLE_MASK	0x0000007F
#define DDR_DUNIT_CTRL_LO_MASK			0x0000007F
#define DDR_DUNIT_CTRL_LO_STATIC_BITS		0x0011FC00

/* DFCDL Configuration 0 Register Definitions */

#define DDR_DFCDL_CFG0_VAL		0x00300000

/* SDRAM Address Control Pads Calibration Register Definitions */

#define DDR_SDRAM_ADDR_CTRL_PAD_CALIB_MASK		0xFFFFFC00
#define DDR_SDRAM_ADDR_CTRL_PAD_CALIB_DRV_STRNGTH	0x000000A5

/* SDRAM Timing (Low) Register Definitions */

#define DDR_SDRAM_TMNG_LO_TDQSS_BIT     0
#define DDR_SDRAM_TMNG_LO_TRCD_BIT      4
#define DDR_SDRAM_TMNG_LO_TRP_BIT       8
#define DDR_SDRAM_TMNG_LO_TWR_BIT       12
#define DDR_SDRAM_TMNG_LO_TWTR_BIT      16
#define DDR_SDRAM_TMNG_LO_TRAS_BIT      20
#define DDR_SDRAM_TMNG_LO_TRRD_BIT      24

/* SDRAM Timing (High) Register Definitions */

#define DDR_SDRAM_TMNG_HI_TRFC_BIT      0
#define DDR_SDRAM_TMNG_HI_TRTR_BIT      4
#define DDR_SDRAM_TMNG_HI_TRTW_BIT      6

/* SDRAM Address Control Register Definitions */

#define DDR_SDRAM_ADDR_CTRL_DCFG_64_128_MB      0x00000000
#define DDR_SDRAM_ADDR_CTRL_DCFG_256_512_MB     0x00000010
#define DDR_SDRAM_ADDR_CTRL_DCFG_1_2_GB         0x00000020

/* SDRAM Operation Register Definitions */

#define DDR_SDRAM_OPER_CMD_NORMAL                  0x00000000
#define DDR_SDRAM_OPER_CMD_PRECHARGE               0x00000001
#define DDR_SDRAM_OPER_CMD_REFRESH                 0x00000002
#define DDR_SDRAM_OPER_CMD_MODE_REG_SET            0x00000003
#define DDR_SDRAM_OPER_CMD_EXT_MODE_REG_SET        0x00000004
#define DDR_SDRAM_OPER_CMD_NOP                     0x00000005

/* SDRAM Mode Register Definitions */

#define DDR_SDRAM_MODE_BL_BIT         0
#define DDR_SDRAM_MODE_BT_BIT         3
#define DDR_SDRAM_MODE_CL_BIT         4

/* Default Timing Parameters */

#define DDR_SDRAM_TRAS_DFLT          8       /* tRAS default - 9 cycles */
#define DDR_SDRAM_TRP_DFLT           3       /* tRP default - 4 cycles */
#define DDR_SDRAM_TRCD_DFLT          3       /* tRCD default - 4 cycles */
#define DDR_SDRAM_TRRD_DFLT          1       /* tRRD default - 2 cycles */
#define DDR_SDRAM_TCL_DFLT           3       /* tCL default - 3 cycles */
#define DDR_SDRAM_TDQSS_DFLT         0       /* tDQSS default - 1 cycle */
#define DDR_SDRAM_TWR_DFLT           1       /* tWR default - 2 cycles */
#define DDR_SDRAM_TWTR_DFLT          1       /* tWTR default - 2 cycle */
#define DDR_SDRAM_TRFC_DFLT          0xF     /* tRFC default - 16 cycles */
#define DDR_SDRAM_TRTR_DFLT          0       /* tRTR default - 1 cycle */
#define DDR_SDRAM_TRTW_DFLT          0       /* tRTW default - 1 cycle */

/* Default Register Definitions */

#define DDR_SDRAM_MODE_DFLT          0x00000002

/* Reset Sample (High) Register Definitions */

#define CPUIF_RST_SAMPLE_HI_N_MASK_HI 0x00000180      /* bits 8-7 */
#define CPUIF_RST_SAMPLE_HI_N_MASK_LO 0x0000003F      /* bits 5-0 */
#define CPUIF_RST_SAMPLE_HI_M_MASK    0x0007E000      /* bits 18-13 */

/* Reset Sample (Low) Register Definitions */

#define CPUIF_RST_SAMPLE_LO_DEVAD18_BIT	  18
#define CPUIF_RST_SAMPLE_LO_DEVAD18_MASK  (1 << \
					   CPUIF_RST_SAMPLE_LO_DEVAD18_BIT)

/*
 * Non-Configurable Bit Definitions
 *
 * In config.h there are several "DEFAULT" definitions provided to allow
 * the user to optimize the SDRAM controller by setting certain configurable
 * bits in the registers:
 * SDRAM Configuration
 * SDRAM Address Control
 * SDRAM Open Pages Control
 * SDRAM Interface Crossbar Control (Low)
 * SDRAM Interface Crossbar Control (High)
 * SDRAM Interface Crossbar Timeout
 *
 * The following definitions are used to generate compiler warnings if
 * the user attempts to alter bits within these registers which are not
 * configurable.
 */

#define MV64360_DDR_SDRAM_CFG_NON_CFG_BITS                     0x03FF3FFF
#define MV64360_DDR_SDRAM_ADDR_CTRL_NON_CFG_BITS               0xFFFFFFF0
#define MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_NON_CFG_BITS         0xFFFF0000
#define MV64360_DDR_SDRAM_IF_XBAR_TMOUT_NON_CFG_BITS           0xFFFEFF00

/* Bit definitions for Watchdog Timer */

#define WDOG_CFG_EN_BIT			31
#define WDOG_CFG_EN_MASK		(1 << WDOG_CFG_EN_BIT)
#define WDOG_CFG_CTL1_BIT		24
#define WDOG_CFG_CTL1_MASK		(3 << WDOG_CFG_CTL1_BIT)
#define WDOG_CFG_CTL2_BIT		26
#define WDOG_CFG_CTL2_MASK		(3 << WDOG_CFG_CTL2_BIT)
#define WDOG_CFG_PRESET_MASK		0x00FFFFFF
#define WDOG_VAL_NMI_MASK		0x00FFFFFF

/* Bit definitions for Timer/Counter Registers */

/* Timer/Counter Control Register bit and mask for timer 0 */

#define TMR_CNTR_CTRL_TC0EN_BIT		0
#define TMR_CNTR_CTRL_TC0EN_MASK	(1 << TMR_CNTR_CTRL_TC0EN_BIT)

#define TMR_CNTR_CTRL_TC0MOD_BIT	1
#define TMR_CNTR_CTRL_TC0MOD_MASK	(1 << TMR_CNTR_CTRL_TC0MOD_BIT)

/* Timer/Counter Interrupt Cause Register bit and mask for timer 0 */

#define TMR_CNTR_INT_CAUSE_TC0_BIT	0
#define TMR_CNTR_INT_CAUSE_TC0_MASK	(1 << TMR_CNTR_INT_CAUSE_TC0_BIT)

/* Timer/Counter Interrupt Mask Register shift bit and mask for timer 0 */

#define TMR_CNTR_INT_MASK_TC0_BIT	0
#define TMR_CNTR_INT_MASK_TC0_MASK	(1 << TMR_CNTR_INT_MASK_TC0_BIT)

/* PCI Interrupt cause bits and masks of interest */

#define PCIIF_INTERRUPT_CAUSE_PCIx_MMABORT_BIT		8
#define PCIIF_INTERRUPT_CAUSE_PCIx_MMABORT_MASK (1 << \
				 PCIIF_INTERRUPT_CAUSE_PCIx_MMABORT_BIT)

#define PCIIF_INTERRUPT_CAUSE_PCIx_MTABORT_BIT		9
#define PCIIF_INTERRUPT_CAUSE_PCIx_MTABORT_MASK (1 << \
				PCIIF_INTERRUPT_CAUSE_PCIx_MTABORT_BIT)

/*
 * CPU Address Decoding Window Macros.
 *
 * CPU address decoding is performed via 'base' and 'size' registers.
 * The following macros will assist in tranlating actual base and size
 * values into their register encoding and vice versa.
 * 
 * MV64360_BASE_DECODE_ADDR(x) converts an actual size in bytes into
 * the value that must be placed into a size register such as
 * CPUIF_DEVCS1_BASE_ADDR.
 * 
 * MV64360_BASE_DECODE_ADDR_INV(x) converts a value read from the base
 * address register (such as from CPUIF_DEVCS1_BASE_ADDR) into an
 * actual size in bytes.
 *
 * CPU address windows are defined in 64 KB granularity.  The size register
 * must be programmed as a set of '1's (starting from the LSB) followed by
 * a set of '0's.  The set of '1's defines the size.  For example, if 
 * size[15:0] is set to 0x03FF, it defines a size of 64MB (number of '1's
 * is 10, 2^10 * 64KB = 64MB).  The following macro is designed to determine
 * the value to be programmed in the size register based on the window size
 * in whatever granularity is desired.  For instance, if you wish to have 
 * a window of size 64MB (MB is the granularity) then you would use the 
 * following command:
 *
 * MV64360_SIZE_FIELD(SIZE_64MB)
 * 
 * This will return the value 0x3FF, which you would then program in the
 * window's size register.  Definitions can be created and used for other 
 * granularities as needed.
 *
 * The MV64360_SIZE_FIELD_INV macro will perform the operation in the
 * opposite direction, that is it will take size encoded field and convert
 * it to the actual size in bytes which is being encoded.
 */
 

#define MV64360_BASE_DECODE_ADDR(x)	((x >> 16) & 0x0000ffff)
#define MV64360_BASE_DECODE_ADDR_INV(x)	(x << 16)

#define MV64360_SIZE_FIELD(x)		((x >> 16) - 1)	
#define MV64360_SIZE_FIELD_INV(x)	((x + 1) << 16)

/* PCI access control register related */

#define MV64360_PCI_SIZE_ENCODE(x) ((x - 1) & 0xFFFFF000)
#define MV64360_PCI_SIZE_DECODE(x) (x + 0x1000)
#define MV64360_PCI_LO_ADDR(b)	   (b & 0xfffff000)	
#define MV64360_PCI_HI_ADDR(b,s)   (MV64360_PCI_LO_ADDR(b) + \
				    MV64360_PCI_SIZE_DECODE(s))
                                               
/* Read MV64360 32 bit register with Endian swapping */

#define MV64360_READ32(base,reg)  sysInLong(reg + base)

/* Output 32 bit quantity and "confirm" (push out to chip) by reading back */

#define MV64360_WRITE32_PUSH(base,reg,val) \
         { sysOutLong((reg + base), val); \
	   (void)MV64360_READ32(reg,base); \
	 }

/* Size of an MV64360 register */

#define MV64360_REG_SIZE	4

/* 
 * Many of the registers in the MV64360 are initialized statically via
 * a table-driven procedure.  There are two kinds of initialization
 * required - direct register write access or PCI configuration
 * access.  Each type of access requires a different kind of table
 * entry.  The initialization procedure will query the table entry for
 * the type and then handle the initialization in the prescribed
 * manner.  The following macros assist in the table building.
 *
 * First let's define the table entry discriminators and end marker:
 */

#define MV64360_INTERNAL	0	/* Mark register as direct access */
#define MV64360_PCICNFG		1	/* Mark register as PCI config Bus 0 */
#define MV64360_PCICNFG_ALT	2	/* Mark register as PCI config Bus 1 */
#define END_INIT_MV64360_REG    0xffffffff   /* End of register list */

/* Now define the essentials for direct register write access: */

#define YES_INIT_MV64360_REG(offset,val)  (MV64360_INTERNAL),(offset),(val),
#define MV64360_INTERNAL_INIT_SIZE	  3  /* # of elements in above macro */

/* Now let's define the essentials for the PCI config access: */

#define YES_CNFG_MV64360_REG(pciFunc,offset,val) (MV64360_PCICNFG),(pciFunc),\
						 (offset),(val),
#define YES_CNFG_MV64360_REG_ALT(pciFunc,offset,val) (MV64360_PCICNFG_ALT),\
						      (pciFunc),(offset),(val),
#define MV64360_PCICNFG_INIT_SIZE	  4  /* # of elements in above macro */

/* 
 * Some of these MV64360 registers are not initialized at all via the
 * static table-driven method but we still want to put a place holder
 * in the table which names the register and identifies it as one which
 * is not statically initialized.  The following macros generate no code
 * but allow us to make these place holder table entries.
 */

#define NOT_INIT_MV64360_REG(offset,x)    /* No code for this macro */
#define NOT_CNFG_MV64360_REG(pciFunc,offset,val)     /* No code */
#define NOT_CNFG_MV64360_REG_ALT(pciFunc,offset,val) /* No code */

/* Define the "all mask" used by many MV64360 registers (mask bit = 0) */

#define MASK_ALL	0

#ifndef _ASMLANGUAGE

/* Support for "specialMem()" */

typedef enum
    {
    snoopNo = 0,        /* No snooping */
    snoopWT = 1,        /* Snoop to "write thru" region */
    snoopWB = 2         /* Snoop to "write back" region */
    } _snoopAttr;

typedef enum
    {
    mburst_32  = 0,     /* 32 bytes */
    mburst_64  = 1,     /* 64 bytes */
    mburst_128 = 2      /* 128 bytes */
    } _mburstVal;

typedef enum
    {
    rdSize_32  = 0,     /* 32 bytes */
    rdSize_64  = 1,     /* 64 bytes */
    rdSize_128 = 2,     /* 128 bytes */
    rdSize_256 = 3      /* 256 bytes */
    } _rdSize;

typedef enum
    {
    cacheCopyBack  = VM_STATE_CACHEABLE,
    cacheWriteThru = VM_STATE_CACHEABLE_WRITETHROUGH,
    cacheNot       = VM_STATE_CACHEABLE_NOT,
    cacheIgnore    = (-1)
    } _cacheAttribute;

typedef struct _SPECIAL_ATTR
    {
    BOOL            enabled;            /* TRUE = enabled/ FALSE = disabled */
    _snoopAttr      snoopAttr;          /* PCI snoop ctrl lo bits 3:2 */
    _mburstVal      mburstVal;          /* PCI access ctrl lo bits 9:8 */
    _rdSize         rdSize;             /* PCI access ctrl lo bit 12:11 */
    _cacheAttribute cacheable;          /* TRUE caceable, FALSE not cacheable */
    } SPECIAL_ATTR;

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif

#endif /* __INCmv64360h */
