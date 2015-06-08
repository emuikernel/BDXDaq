/* mv64360EnetUnit.h - MV64360 Gigabit Ethernet Unit Header File */

/* Copyright 2002-2004 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,12mar04,cak  Added bit definitions for Integrated SRAM. 
01b,31jan03,efb  Moved default value define to mv64360EnetReg.h
01a,29may02,efb  New file.
*/

/*

DESCRIPTION

This file contains the MV64360 ethernet unit register bit definitions, 
masks, and typical values. Register offsets are defined in the 
main MV64360 header file. Registers that are not listed here, have 
single meanings and do not contain addressable subfields. Default
(initial reset) values for subfields are assumed to be zero unless 
a default #define (..._DFLT suffixed) exists for the subfield.

*/

#ifndef	INCmv64360EnetUnith
#define	INCmv64360EnetUnith

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

/* Includes */

#include "vxWorks.h"

/* Main Interrupt Cause Definition for the Ethernet Unit */

#define GE_INT_VEC_ENET_UNIT_ERR      ICI_MICL_INT_NUM_14

/* GE Phy Address Register */

#define GE_PHY_ADDR_MASK              0x1F

#define GE_PHY_ADDR_0_BIT             0
#define GE_PHY_ADDR_0_MASK            GE_PHY_ADDR_MASK
#define GE_PHY_ADDR_0_DFLT            0x8

#define GE_PHY_ADDR_1_BIT             5
#define GE_PHY_ADDR_1_MASK            (GE_PHY_ADDR_MASK << GE_PHY_ADDR_1_BIT)
#define GE_PHY_ADDR_1_DFLT            0x9

#define GE_PHY_ADDR_2_BIT             10
#define GE_PHY_ADDR_2_MASK            (GE_PHY_ADDR_MASK << GE_PHY_ADDR_2_BIT)
#define GE_PHY_ADDR_2_DFLT            0xA

/* GE SMI Register */

#define GE_SMI_DATA_MASK              0xFFFF

#define GE_SMI_PHY_ADDR_BIT           16
#define GE_SMI_PHY_ADDR_MASK          (0x1F << GE_SMI_PHY_ADDR_BIT)

#define GE_SMI_REG_ADDR_BIT           21
#define GE_SMI_REG_ADDR_MASK          (0x1F << GE_SMI_REG_ADDR_BIT)

#define GE_SMI_OPCODE_BIT             26
#define GE_SMI_OPCODE_READ            (0x1 << GE_SMI_OPCODE_BIT)
#define GE_SMI_OPCODE_WRITE           0x0
#define GE_SMI_OPCODE_DFLT            GE_SMI_OPCODE_READ

#define GE_SMI_READ_VALID_BIT         27
#define GE_SMI_READ_VALID_MASK        (0x1 << GE_SMI_READ_VALID_BIT)

#define GE_SMI_BUSY_BIT               28
#define GE_SMI_BUSY_MASK              (0x1 << GE_SMI_BUSY_BIT)

/* 
 * GE Unit Default ID Register
 *
 * Note that default ID values and attribute values are the same as those 
 * defined for the base address target encoding and attribute fields below.
 */

#define GE_UNIT_DEFAULT_ID_BIT        0
#define GE_UNIT_DEFAULT_ID_MASK       0xF

#define GE_UNIT_DEFAULT_ATTR_BIT      4
#define GE_UNIT_DEFAULT_ATTR_MASK     (0xFF << GE_UNIT_DEFAULT_ATTR_BIT)

/* GE Unit Interrupt Cause and Mask Registers */

#define GE_UNIT_INT_SUMMARY_BIT          0
#define GE_UNIT_INT_SUMMARY_MASK         0x1
 
#define GE_UNIT_INT_PARITY_BIT           1
#define GE_UNIT_INT_PARITY_MASK          (0x1 << GE_UNIT_INT_PARITY_BIT)

#define GE_UNIT_INT_ADDR_VIOL_BIT        2
#define GE_UNIT_INT_ADDR_VIOL_MASK       (0x1 << GE_UNIT_INT_ADDR_VIOL_BIT)

#define GE_UNIT_INT_ADDR_NO_MATCH_BIT    3
#define GE_UNIT_INT_ADDR_NO_MATCH_MASK   (0x1 << GE_UNIT_INT_ADDR_NO_MATCH_BIT)

#define GE_UNIT_INT_SMI_DONE_BIT         4
#define GE_UNIT_INT_SMI_DONE_MASK        (0x1 << GE_UNIT_INT_SMI_DONE_BIT)

#define GE_UNIT_INT_CNTR_WRAP_BIT        5
#define GE_UNIT_INT_CNTR_WRAP_MASK       (0x1 << GE_UNIT_INT_CNTR_WRAP_BIT)

#define GE_UNIT_INT_INTRNL_ADDR_ERR_BIT  7
#define GE_UNIT_INT_INTRNL_ADDR_ERR_MASK                          \
        (0x1 << GE_UNIT_INT_INTRNL_ADDR_ERR_BIT)

#define GE_UNIT_INT_MASK      (GE_UNIT_INT_SUMMARY_MASK |             \
                               GE_UNIT_INT_PARITY_MASK  |             \
                               GE_UNIT_INT_ADDR_VIOL_MASK |           \
                               GE_UNIT_INT_ADDR_NO_MATCH_MASK |       \
                               GE_UNIT_INT_SMI_DONE_MASK |            \
                               GE_UNIT_INT_CNTR_WRAP_MASK |           \
                               GE_UNIT_INT_INTRNL_ADDR_ERR_MASK)

#define GE_UNIT_INT_DISAB_MASK GE_UNIT_INT_MASK

#define GE_UNIT_INT_ENB_MASK  (GE_UNIT_INT_SUMMARY_MASK |             \
                               GE_UNIT_INT_PARITY_MASK  |             \
                               GE_UNIT_INT_ADDR_VIOL_MASK |           \
                               GE_UNIT_INT_ADDR_NO_MATCH_MASK |       \
                               GE_UNIT_INT_CNTR_WRAP_MASK |           \
                               GE_UNIT_INT_INTRNL_ADDR_ERR_MASK)

/* GE Unit Internal Address Error Register */

#define GE_UNIT_INTRNL_ERR_ADDR_MASK     0x000000F

/* 
 * Gigabit Ethernet (GE) Base Address Register Bit Definitions -
 *
 * Note that the same attribute bits have different meanings based
 * on the target bit values.
 */

#define GE_BASE_ADDR_TARGET_BIT       0x0
#define GE_BASE_ADDR_TARGET_MASK      0x000000F

#define GE_BASE_ADDR_TARGET_DRAM      0x0
#define GE_BASE_ADDR_TARGET_DEV       0x1
#define GE_BASE_ADDR_TARGET_SRAM      0x2
#define GE_BASE_ADDR_TARGET_60X       0x2
#define GE_BASE_ADDR_TARGET_PCI0      0x3
#define GE_BASE_ADDR_TARGET_PCI1      0x4
#define GE_BASE_ADDR_TARGET_C_UNIT    0x5
#define GE_BASE_ADDR_TARGET_A_UNIT    0x6
#define GE_BASE_ADDR_TARGET_G_UNIT    0x7

#define GE_BASE_ADDR_ATTRIBUTE_BIT    8
#define GE_BASE_ADDR_ATTRIBUTE_MASK   (0xFF << GE_BASE_ADDR_ATTRIBUTE_BIT)

#define GE_BASE_ADDR_DRAM_BANK_BIT    GE_BASE_ADDR_ATTRIBUTE_BIT
#define GE_BASE_ADDR_DRAM_BANK_MASK   (0xF << GE_BASE_ADDR_DRAM_BANK_BIT)

#define GE_BASE_ADDR_DRAM_BANK_CS0    (0xE << GE_BASE_ADDR_DRAM_BANK_BIT)
#define GE_BASE_ADDR_DRAM_BANK_CS1    (0xD << GE_BASE_ADDR_DRAM_BANK_BIT)
#define GE_BASE_ADDR_DRAM_BANK_CS2    (0xB << GE_BASE_ADDR_DRAM_BANK_BIT)
#define GE_BASE_ADDR_DRAM_BANK_CS3    (0x7 << GE_BASE_ADDR_DRAM_BANK_BIT)

#define GE_BASE_ADDR_SRAM_BLOCK_0     0
#define GE_BASE_ADDR_SRAM_BLOCK_1     1
#define GE_BASE_ADDR_SRAM_BLOCK_BIT   8

#define GE_BASE_ADDR_COHERENCY_BIT    12
#define GE_BASE_ADDR_COHERENCY_MASK   (0x3 << GE_BASE_ADDR_COHERENCY_BIT)

#define GE_BASE_ADDR_COHERENCY_WT     (0x1 << GE_BASE_ADDR_COHERENCY_BIT)
#define GE_BASE_ADDR_COHERENCY_WB     (0x2 << GE_BASE_ADDR_COHERENCY_BIT)
#define GE_BASE_ADDR_COHERENCY_NONE   (~GE_BASE_ADDR_COHERENCY_MASK)

#define GE_BASE_ADDR_DEV_BUS_BIT      GE_BASE_ADDR_ATTRIBUTE_BIT
#define GE_BASE_ADDR_DEV_BUS_MASK     (0x1F << GE_BASE_ADDR_DEV_BUS_BIT)

#define GE_BASE_ADDR_DEV_CS0          (0x1E << GE_BASE_ADDR_DEV_BUS_BIT)
#define GE_BASE_ADDR_DEV_CS1          (0x1D << GE_BASE_ADDR_DEV_BUS_BIT)
#define GE_BASE_ADDR_DEV_CS2          (0x1B << GE_BASE_ADDR_DEV_BUS_BIT)
#define GE_BASE_ADDR_DEV_CS3          (0x17 << GE_BASE_ADDR_DEV_BUS_BIT)
#define GE_BASE_ADDR_BOOT_CS          (0xF << GE_BASE_ADDR_DEV_BUS_BIT)

#define GE_BASE_ADDR_PCI_SWAP_BIT     GE_BASE_ADDR_ATTRIBUTE_BIT
#define GE_BASE_ADDR_PCI_SWAP_MASK    (0x3 << GE_BASE_ADDR_PCI_SWAP_BIT)

#define GE_BASE_ADDR_PCI_BYTE_SWAP    (~GE_BASE_ADDR_PCI_SWAP_MASK)
#define GE_BASE_ADDR_PCI_NO_SWAP      (0x1 << GE_BASE_ADDR_PCI_SWAP_BIT)
#define GE_BASE_ADDR_PCI_ALL_SWAP     (0x2 << GE_BASE_ADDR_PCI_SWAP_BIT)
#define GE_BASE_ADDR_PCI_WORD_SWAP    (0x3 << GE_BASE_ADDR_PCI_SWAP_BIT)

#define GE_BASE_ADDR_PCIX_NS_BIT      10
#define GE_BASE_ADDR_PCIX_NO_SNOOP    (0x1 << GE_BASE_ADDR_PCIX_NS_BIT)

#define GE_BASE_ADDR_PCI_IO_MEM_BIT   11
#define GE_BASE_ADDR_PCI_MEM_SEL      (0x1 << GE_BASE_ADDR_PCI_IO_MEM_BIT)

#define GE_BASE_ADDR_PCI_REQ64_BIT    12
#define GE_BASE_ADDR_PCI_REQ64_SIZE   (0x1 << GE_BASE_ADDR_PCI_REQ64_BIT)

#define GE_BASE_ADDR_SRAM_60X_BIT     11
#define GE_BASE_ADDR_60X_SEL          (0x1 << GE_BASE_ADDR_SRAM_60X_BIT)

#define GE_BASE_ADDR_BASE_BIT         16
#define GE_BASE_ADDR_BASE_MASK        (0xFFFF << GE_BASE_ADDR_BASE_BIT)
#define GE_CPUIF_ADDR_SHIFT_INDEX     GE_BASE_ADDR_BASE_BIT

/* GE Window Size Register Encoding Values */

#define GE_WINDOW_SIZE_BIT            16
#define GE_WINDOW_SIZE_MASK           (0xFFFF << GE_WINDOW_SIZE_BIT)

#define GE_WINDOW_SIZE_64KB           (~GE_WINDOW_SIZE_MASK)
#define GE_WINDOW_SIZE_128KB          (0x1 << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_256KB          (0x3 << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_512KB          (0x7 << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_1MB            (0xF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_2MB            (0x1F << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_4MB            (0x3F << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_8MB            (0x7F << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_16MB           (0xFF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_32MB           (0x1FF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_64MB           (0x3FF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_128MB          (0x7FF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_256MB          (0xFFF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_512MB          (0x1FFF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_1GB            (0x3FFF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_2GB            (0x7FFF << GE_WINDOW_SIZE_BIT)
#define GE_WINDOW_SIZE_4GB            (0xFFFF << GE_WINDOW_SIZE_BIT)

/* GE Header Retarget Base */

#define GE_HDR_RETARGET_BASE_BIT      16
#define GE_HDR_RETARGET_BASE_MASK     (0xFFFF << GE_HDR_RETARGET_BASE_BIT)

/* GE Headers Retarget Control Register */

#define GE_HDR_RETARGET_ENABLE        0x1

#define GE_HDR_RETARGET_BUF_BIT       1
#define GE_HDR_RETARGET_BUF_MASK      (0x7 << GE_HDR_RETARGET_BUF_BIT)

#define GE_HDR_RETARGET_BUF_256B      (~GE_HDR_RETARGET_BUF_MASK)
#define GE_HDR_RETARGET_BUF_512B      (0x1 << GE_HDR_RETARGET_BUF_BIT)
#define GE_HDR_RETARGET_BUF_1KB       (0x2 << GE_HDR_RETARGET_BUF_BIT)
#define GE_HDR_RETARGET_BUF_2KB       (0x3 << GE_HDR_RETARGET_BUF_BIT)
#define GE_HDR_RETARGET_BUF_4KB       (0x4 << GE_HDR_RETARGET_BUF_BIT)
#define GE_HDR_RETARGET_BUF_8KB       (0x5 << GE_HDR_RETARGET_BUF_BIT)

#define GE_HDR_RETARGET_MASK_BIT      16
#define GE_HDR_RETARGET_MASK1         (0xFFFF << GE_HDR_RETARGET_MASK1_BIT)

#define GE_RETARGET_TOTAL_64KB        (~GE_HDR_RETARGET_MASK1)
#define GE_RETARGET_TOTAL_128KB       (0x1 << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_256KB       (0x3 << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_512KB       (0x7 << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_1MB         (0xF << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_2MB         (0x1F << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_4MB         (0x3F << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_8MB         (0x7F << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_16MB        (0xFF << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_32MB        (0x1FF << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_64MB        (0x3FF << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_128MB       (0x7FF << GE_HDR_RETARGET_MASK1_BIT)
#define GE_RETARGET_TOTAL_256MB       (0xFFF << GE_HDR_RETARGET_MASK1_BIT)

/* GE Base Address Window Enable Register */

#define GE_BASE_ADDR_ENABLE_WIN_MASK  0x3F
#define GE_BASE_ADDR_ENABLE_WIN_DFLT  0x3F

#define GE_BASE_ADDR_ENABLE_WIN0      0x1
#define GE_BASE_ADDR_ENABLE_WIN1      0x2
#define GE_BASE_ADDR_ENABLE_WIN2      0x4
#define GE_BASE_ADDR_ENABLE_WIN3      0x8
#define GE_BASE_ADDR_ENABLE_WIN4      0x10
#define GE_BASE_ADDR_ENABLE_WIN5      0x20
 
/* GE Port Access Protect Register */

#define GE_PORT_ACCESS_PROT_MASK      0x3
#define GE_PORT_ACCESS_PROT_DFLT      0x3

#define GE_PORT_ACCESS_PROT_WIN0_BIT  0x0
#define GE_PORT_ACCESS_PROT_WIN1_BIT  0x2
#define GE_PORT_ACCESS_PROT_WIN2_BIT  0x4
#define GE_PORT_ACCESS_PROT_WIN3_BIT  0x6
#define GE_PORT_ACCESS_PROT_WIN4_BIT  0x8
#define GE_PORT_ACCESS_PROT_WIN5_BIT  0x10

#define GE_PORT_ACCESS_NOT_ALLOWED    0x0
#define GE_PORT_ACCESS_READ_ONLY      0x1
#define GE_PORT_ACCESS_FULL           0x3

#define GE_PORT_ACCESS_NONE_WIN_ALL   0x0000000
#define GE_PORT_ACCESS_READ_WIN_ALL   0x0000555
#define GE_PORT_ACCESS_FULL_WIN_ALL   0x0000FFF
#define GE_PORT_ACCESS_WIN_ALL_DFLT   GE_PORT_ACCESS_FULL_WIN_ALL

/* PHY Register defines */

#define PHY_MII_CTRL_REG                0x00

#define PHY_MII_CTRL_RESET_BIT          15
#define PHY_MII_CTRL_RESET_MASK         (0x1 << PHY_MII_CTRL_RESET_BIT)

#define PHY_MII_CTRL_INTRNL_LOOPBK_BIT  14
#define PHY_MII_CTRL_INTRNL_LOOPBK_MASK (0x1 << PHY_MII_CTRL_INTRNL_LOOPBK_BIT)

#define PHY_MII_CTRL_SPEED_SEL_LSB_BIT  13
#define PHY_MII_CTRL_SPEED_SEL_LSB_MASK (0x1 << PHY_MII_CTRL_SPEED_SEL_LSB_BIT)

#define PHY_MII_CTRL_AUTONEG_ENB_BIT    12
#define PHY_MII_CTRL_AUTONEG_ENB_MASK   (0x1 << PHY_MII_CTRL_AUTONEG_ENB_BIT)

#define PHY_MII_CTRL_STANDBY_BIT        11
#define PHY_MII_CTRL_STANDBY_MASK       (0x1 << PHY_MII_CTRL_STANDBY_BIT)

#define PHY_MII_CTRL_ISOLATE_BIT        10
#define PHY_MII_CTRL_ISOLATE_MASK       (0x1 << PHY_MII_CTRL_ISOLATE_BIT)

#define PHY_MII_CTRL_RE_AUTONEG_BIT     9
#define PHY_MII_CTRL_RE_AUTONEG_MASK    (0x1 << PHY_MII_CTRL_RE_AUTONEG_BIT)

#define PHY_MII_CTRL_DUPLEX_BIT         8
#define PHY_MII_CTRL_DUPLEX_MASK        (0x1 << PHY_MII_CTRL_DUPLEX_BIT)

#define PHY_MII_CTRL_CLSN_TEST_BIT      7
#define PHY_MII_CTRL_CLSN_TEST_MASK     (0x1 << PHY_MII_CTRL_CLSN_TEST_BIT)

#define PHY_MII_CTRL_SPEED_SEL_MSB_BIT  6
#define PHY_MII_CTRL_SPEED_SEL_MSB_MASK (0x1 << PHY_MII_CTRL_SPEED_SEL_MSB_BIT)

#define PHY_MII_STATUS_REG              0x01

#define PHY_MII_STATUS_LINK_STATE_BIT   2
#define PHY_MII_STATUS_LINK_STATE_MASK  (0x1 << PHY_MII_STATUS_LINK_STATE_BIT)

#define PHY_MII_STATUS_AUTONEG_CMPL_BIT 5
#define PHY_MII_STATUS_AUTONEG_CMPL_MASK                                   \
        (0x1 << PHY_MII_STATUS_AUTONEG_CMPL_BIT)

#define PHY_AUTONEG_LINK_ABILITY_REG    0x05

#define PHY_AUTONEG_LINK_10_HD_BIT      5
#define PHY_AUTONEG_LINK_10_HD_MASK     (0x1 << PHY_AUTONEG_LINK_10_HD_BIT)

#define PHY_AUTONEG_LINK_10_FD_BIT      6
#define PHY_AUTONEG_LINK_10_FD_MASK     (0x1 << PHY_AUTONEG_LINK_10_FD_BIT)

#define PHY_AUTONEG_LINK_100_HD_BIT     7
#define PHY_AUTONEG_LINK_100_HD_MASK    (0x1 << PHY_AUTONEG_LINK_100_HD_BIT)

#define PHY_AUTONEG_LINK_100_FD_BIT     8
#define PHY_AUTONEG_LINK_100_FD_MASK    (0x1 << PHY_AUTONEG_LINK_100_FD_BIT)

#define PHY_1000BASE_T_STATUS_REG       0x0A

#define PHY_1000_STATUS_LINK_HD_BIT     10
#define PHY_1000_STATUS_LINK_HD_MASK    (0x1 << PHY_1000_STATUS_LINK_HD_BIT)

#define PHY_1000_STATUS_LINK_FD_BIT     11
#define PHY_1000_STATUS_LINK_FD_MASK    (0x1 << PHY_1000_STATUS_LINK_FD_BIT)

#define PHY_EXT_CTRL_REG                0x10

#define PHY_EXT_CTRL_INT_FORCE_BIT      11
#define PHY_EXT_CTRL_INT_FORCE_MASK     (0x1 << PHY_EXT_CTRL_INT_FORCE_BIT)

#define PHY_EXT_CTRL_INT_DISAB_BIT      12
#define PHY_EXT_CTRL_INT_DISAB_MASK     (0x1 << PHY_EXT_CTRL_INT_DISAB_BIT)

#define PHY_EXT_STATUS_REG              0x11

#define PHY_EXT_STATUS_LINK_STATUS_BIT  8
#define PHY_EXT_STATUS_LINK_STATUS_MASK (0x1 << PHY_EXT_STATUS_LINK_STATUS_BIT)

#define PHY_EXT_STATUS_INT_STATUS_BIT   12
#define PHY_EXT_STATUS_INT_STATUS_MASK  (0x1 << PHY_EXT_STATUS_INT_STATUS_BIT)

#define PHY_AUX_STATUS_SUMMARY_REG      0x19

#define PHY_AUX_STATUS_AUTONEG_DONE_BIT 15
#define PHY_AUX_STATUS_AUTONEG_DONE_MASK                                   \
        (0x1 << PHY_AUX_STATUS_AUTONEG_DONE_BIT)

#define PHY_AUX_STATUS_AUTONEG_HCD_BIT  8
#define PHY_AUX_STATUS_AUTONEG_HCD_MASK (0x7 << PHY_AUX_STATUS_AUTONEG_HCD_BIT)

#define PHY_AUX_STATUS_AUTONEG_1000_FD  0x7  /* 1000Base-T Full-duplex */
#define PHY_AUX_STATUS_AUTONEG_1000_HD  0x6  /* 1000Base-T Half-duplex */
#define PHY_AUX_STATUS_AUTONEG_100_FD   0x5  /* 100Base-TX Full-duplex */
#define PHY_AUX_STATUS_AUTONEG_100_T4   0x4  /* 100Base-T4 */
#define PHY_AUX_STATUS_AUTONEG_100_HD   0x3  /* 100Base-TX Half-duplex */
#define PHY_AUX_STATUS_AUTONEG_10_FD    0x2  /* 10Base-T Full-duplex */
#define PHY_AUX_STATUS_AUTONEG_10_HD    0x1  /* 10Base-T Half_duplex */
#define PHY_AUX_STATUS_AUTONEG_INCOMPLT 0x0  /* Autonegotiation in progress */

#define PHY_AUX_STATUS_LINK_STATUS_BIT  2
#define PHY_AUX_STATUS_LINK_STATUS_MASK (0x1 << PHY_AUX_STATUS_LINK_STATUS_BIT)

#define PHY_INT_STATUS_REG              0x1A

#define PHY_INT_HCD_NOT_LINK_BIT        9
#define PHY_INT_HCD_NOT_LINK_MASK       (0x1 << PHY_INT_HCD_NOT_LINK_BIT)

#define PHY_INT_DUPLEX_CHANGE_BIT       3
#define PHY_INT_DUPLEX_CHANGE_MASK      (0x1 << PHY_INT_DUPLEX_CHANGE_BIT)

#define PHY_INT_SPEED_CHANGE_BIT        2
#define PHY_INT_SPEED_CHANGE_MASK       (0x1 << PHY_INT_SPEED_CHANGE_BIT)

#define PHY_INT_LINK_CHANGE_BIT         1
#define PHY_INT_LINK_CHANGE_MASK        (0x1 << PHY_INT_LINK_CHANGE_BIT)

#define PHY_INT_MASK_REG                0x1B

#define PHY_INT_LINK_STATE_MASK         (PHY_INT_DUPLEX_CHANGE_MASK |   \
                                         PHY_INT_SPEED_CHANGE_MASK  |   \
                                         PHY_INT_LINK_CHANGE_MASK)

#define PHY_INT_MASK_ENB                (0xFFFF & ~PHY_INT_LINK_STATE_MASK)

#define GE_PHY_ADDR_MAX                 0x1F
#define GE_SMI_SEM_WAIT                 120    /* Wait in ticks */

void mv64360EnetUnitInit (void);

STATUS mv64360EnetUnitPhyInit (UINT32);

STATUS mv64360EnetUnitPhyIntEnable (UINT32);

BOOL mv64360EnetUnitPhyInterrupted (UINT32);

STATUS mv64360EnetUnitGetLinkState (UINT32, UINT32 *, BOOL *, BOOL *);

void mv64360EnetUnitIntEnable (UINT32);

void mv64360EnetUnitIntDisable (UINT32);

void mv64360EnetUnitHandleInts (void);

void mv64360EnetUnitSmiWrite (UINT32, UINT32, UINT32);
 
UINT32 mv64360EnetUnitSmiRead (UINT32, UINT32);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* INCmv64360EnetUnith */
