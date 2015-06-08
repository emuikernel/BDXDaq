/* dec2155xCcpi.h - DEC 2155X (Drawbridge) PCI-to-PCI bridge header */

/* Copyright 1998-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,31aug00,dmw  Written (from version 01b of prpmc750/dec2155xCcpi.h).
*/

#ifndef INCdec2155xCcpih
#define INCdec2155xCcpih

#ifdef __cplusplus
    extern "C" {
#endif

#include "drv/pci/pciConfigLib.h"

/* DEC2155X */
/* Primary (host) configuration offsets viewed from primary side */

#define DEC2155X_CFG_PRI_CSR_AND_DS_MEM0_BAR    PCI_CFG_BASE_ADDRESS_0
#define DEC2155X_CFG_PRI_CSR_IO_BAR             PCI_CFG_BASE_ADDRESS_1
#define DEC2155X_CFG_PRI_DS_IO_OR_MEM1_BAR      PCI_CFG_BASE_ADDRESS_2
#define DEC2155X_CFG_PRI_DS_MEM2_BAR            PCI_CFG_BASE_ADDRESS_3
#define DEC2155X_CFG_PRI_DS_MEM3_BAR            PCI_CFG_BASE_ADDRESS_4
#define DEC2155X_CFG_PRI_UPR32_BITS_DS_MEM3_BAR PCI_CFG_BASE_ADDRESS_5

/* Secondary (local) configuration offsets viewed from secondary side */

#define DEC2155X_CFG_SEC_CSR_MEM_BAR       PCI_CFG_BASE_ADDRESS_0
#define DEC2155X_CFG_SEC_CSR_IO_BAR        PCI_CFG_BASE_ADDRESS_1
#define DEC2155X_CFG_SEC_US_IO_OR_MEM0_BAR PCI_CFG_BASE_ADDRESS_2
#define DEC2155X_CFG_SEC_US_MEM1_BAR       PCI_CFG_BASE_ADDRESS_3
#define DEC2155X_CFG_SEC_US_MEM2_BAR       PCI_CFG_BASE_ADDRESS_4

/* translation contants */

#define DEC2155X_PRI_FROM_SEC_OFFSET 0x40 /* primary regs seen from secondary */
#define DEC2155X_SEC_FROM_PRI_OFFSET 0x40 /* secondary regs seen from primary */
#define DEC2155X_DEFAULT_MEM_WINDOW_SZ 4096
#define DEC2155X_DEFAULT_IO_WINDOW_SZ   256

/* Shared configuration offsets viewed from primary or secondary side */

#define DEC2155X_CFG_DS_CONFIG_ADDRESS     0x80
#define DEC2155X_CFG_DS_CONFIG_DATA        0x84
#define DEC2155X_CFG_US_CONFIG_ADDRESS     0x88
#define DEC2155X_CFG_US_CONFIG_DATA        0x8c
#define DEC2155X_CFG_CFG_OWN_BITS          0x90
#define DEC2155X_CFG_CFG_CTRL_AND_STS      0x92
#define DEC2155X_CFG_DS_MEM0_TB            0x94
#define DEC2155X_CFG_DS_IO_OR_MEM1_TB      0x98
#define DEC2155X_CFG_DS_MEM2_TB            0x9c
#define DEC2155X_CFG_DS_MEM3_TB            0xa0
#define DEC2155X_CFG_US_IO_OR_MEM0_TB      0xa4
#define DEC2155X_CFG_US_MEM1_TB            0xa8
#define DEC2155X_CFG_DS_MEM0_SETUP         0xac
#define DEC2155X_CFG_DS_IO_OR_MEM1_SETUP   0xb0
#define DEC2155X_CFG_DS_MEM2_SETUP         0xb4
#define DEC2155X_CFG_DS_MEM3_SETUP         0xb8
#define DEC2155X_CFG_UPR32_BITS_DS_MEM3_SU 0xbc
#define DEC2155X_CFG_PRI_EXP_ROM_SETUP     0xc0
#define DEC2155X_CFG_US_IO_OR_MEM0_SETUP   0xc4
#define DEC2155X_CFG_US_MEM1_SETUP         0xc8
#define DEC2155X_CFG_CHP_CTRL0             0xcc
#define DEC2155X_CFG_CHP_CTRL1             0xce
#define DEC2155X_CFG_CHP_STATUS            0xd0
#define DEC2155X_CFG_ARB_CTRL              0xd2
#define DEC2155X_CFG_PRI_SERR_DISABLES     0xd4
#define DEC2155X_CFG_SEC_SERR_DISABLES     0xd5
#define DEC2155X_CFG_RESET_CONTROL         0xd8
#define DEC2155X_CFG_PWR_MGMT_CAP_ID       0xdc
#define DEC2155X_CFG_PWR_MGMT_NXT_PTR      0xdd
#define DEC2155X_CFG_PWR_MGMT_CAP          0xde
#define DEC2155X_CFG_PWR_MGMT_CSR          0xe0
#define DEC2155X_CFG_PMCSR_BSE             0xe2
#define DEC2155X_CFG_PM_DATA               0xe3
#define DEC2155X_CFG_VPD_CAP_ID            0xe4
#define DEC2155X_CFG_VPD_NXT_PTR           0xe5
#define DEC2155X_CFG_VPD_ADRS              0xe6
#define DEC2155X_CFG_VPD_DATA              0xe8
#define DEC2155X_CFG_HS_CTRL_CAP_ID        0xec
#define DEC2155X_CFG_HS_CTRL_NXT_PTR       0xed
#define DEC2155X_CFG_HS_CTRL               0xee

/* Control and Status Registers (CSR) */

/* Shared - Viewed from either primary or secondary */

#define DEC2155X_CSR_DS_CONFIG_ADDRESS    0x000
#define DEC2155X_CSR_DS_CONFIG_DATA       0x004
#define DEC2155X_CSR_USM_CONFIG_ADDRESS   0x008
#define DEC2155X_CSR_US_CONFIG_DATA       0x00c
#define DEC2155X_CSR_CFG_OWN_BITS         0x010
#define DEC2155X_CSR_CFG_CTRL_AND_STS     0x012
#define DEC2155X_CSR_DS_IO_ADDRESS        0x014
#define DEC2155X_CSR_DS_IO_DATA           0x018
#define DEC2155X_CSR_US_IO_ADDRESS        0x01c
#define DEC2155X_CSR_US_IO_DATA           0x020
#define DEC2155X_CSR_IO_OWN_BITS          0x024
#define DEC2155X_CSR_IOCTRL_AND_STS       0x026
#define DEC2155X_CSR_LKP_TBL_OFST         0x028
#define DEC2155X_CSR_LKP_TBL_DATA         0x02c
#define DEC2155X_CSR_I2O_OBND_PST_LST_STS 0x030
#define DEC2155X_CSR_I2O_OBND_PST_LST_MSK 0x034
#define DEC2155X_CSR_I2O_IBND_PST_LST_STS 0x038
#define DEC2155X_CSR_I2O_IBND_PST_LST_MSK 0x03c

/* I2O registers from 0x040 to 0x067 - Not Used */

#define DEC2155X_CSR_DS_MEM0_TB         0x068
#define DEC2155X_CSR_DS_IO_OR_MEM1_TB   0x06c
#define DEC2155X_CSR_DS_MEM2_TB         0x070
#define DEC2155X_CSR_DS_MEM3_TB         0x074
#define DEC2155X_CSR_US_IO_OR_MEM0_TB   0x078
#define DEC2155X_CSR_US_MEM1_TB         0x07c
#define DEC2155X_CSR_CHP_STS_CSR        0x082
#define DEC2155X_CSR_CHP_SET_IRQ_MSK    0x084
#define DEC2155X_CSR_CHP_CLR_IRQ_MSK    0x086
#define DEC2155X_CSR_US_PG_BND_IRQ0     0x088
#define DEC2155X_CSR_US_PG_BND_IRQ1     0x08c
#define DEC2155X_CSR_US_PG_BND_IRQ_MSK0 0x090
#define DEC2155X_CSR_US_PG_BND_IRQ_MSK1 0x094
#define DEC2155X_CSR_PRI_CLR_IRQ        0x098
#define DEC2155X_CSR_SEC_CLR_IRQ        0x09a
#define DEC2155X_CSR_PRI_SET_IRQ        0x09c
#define DEC2155X_CSR_SEC_SET_IRQ        0x09e
#define DEC2155X_CSR_PRI_CLR_IRQ_MSK    0x0a0
#define DEC2155X_CSR_SEC_CLR_IRQ_MSK    0x0a2
#define DEC2155X_CSR_PRI_SET_IRQ_MSK    0x0a4
#define DEC2155X_CSR_SEC_SET_IRQ_MSK    0x0a6
#define DEC2155X_CSR_SCRATCHPAD0        0x0a8
#define DEC2155X_CSR_SCRATCHPAD1        0x0ac
#define DEC2155X_CSR_SCRATCHPAD2        0x0b0
#define DEC2155X_CSR_SCRATCHPAD3        0x0b4
#define DEC2155X_CSR_SCRATCHPAD4        0x0b8
#define DEC2155X_CSR_SCRATCHPAD5        0x0bc
#define DEC2155X_CSR_SCRATCHPAD6        0x0c0
#define DEC2155X_CSR_SCRATCHPAD7        0x0c4
#define DEC2155X_CSR_ROM_SETUP          0x0c8
#define DEC2155X_CSR_ROM_DATA           0x0ca
#define DEC21554_CSR_ROM_ADRS           0x0cc
#define DEC2155X_CSR_ROM_CTRL           0x0cf
#define DEC2155X_CSR_US_MEM2_LKUP_TBL   0x100

/* Error bits in the standard PCI status register */

#define DEC2155X_CFG_STS_DATA_PAR_ERR_DET (1 << 8)
#define DEC2155X_CFG_STS_SIG_TGT_ABT      (1 << 11)
#define DEC2155X_CFG_STS_RCVD_TGT_ABT     (1 << 12)
#define DEC2155X_CFG_STS_RCVD_MSTR_ABT    (1 << 13)
#define DEC2155X_CFG_STS_SIG_SYS_ERR      (1 << 14)
#define DEC2155X_CFG_STS_DET_PAR_ERR      (1 << 15)

/* Reserved bits in the bist register */

#define DEC2155X_BIST_RSV 0x30

/* Reserved bits in the translated base registers. */

#define DEC2155X_MEM_TB_RSV_MSK       0xfff
#define DEC2155X_IO_OR_MEM_TB_RSV_MSK 0x3f

/* Setup register bits */

#define DEC2155X_MEM_SETUP_RSV_MSK 0xff0
#define DEC2155X_IO_OR_MEM_SETUP_RSV_MSK 0x30

#define DEC2155X_SETUP_REG_BAR_ENABLE      (1 << 31)

#define DEC2155X_MEM_SETUP_SZ_MSK \
          (~(DEC2155X_SETUP_REG_BAR_ENABLE | \
             DEC2155X_MEM_TB_RSV_MSK))

#define DEC2155X_IO_OR_MEM_SETUP_SZ_MSK \
          (~(DEC2155X_SETUP_REG_BAR_ENABLE | \
             DEC2155X_IO_OR_MEM_TB_RSV_MSK))

#define DEC2155X_UPR_32_BITS_DS_MEM3_SETUP_SZ_MSK \
          (~DEC2155X_SETUP_REG_BAR_ENABLE)

/* Configuration control and status bits */

#define DEC2155X_CCS_DS_CFG_OWN_STS	(1 << 0)
#define DEC2155X_CCS_DS_CFG_CTRL	(1 << 1)
#define DEC2155X_CCS_US_CFG_OWN_STS	(1 << 8)
#define DEC2155X_CCS_US_CFG_CTRL	(1 << 9)

/* Chip Control 0 bits */

#define DEC2155X_CC0_MSTR_ABRT_MD       (1 << 0)
#define DEC2155X_CC0_MEM_WRT_DISC_CTRL  (1 << 1)
#define DEC2155X_CC0_PRI_MSTR_TO        (1 << 2)
#define DEC2155X_CC0_SEC_MSTR_TO        (1 << 3)
#define DEC2155X_CC0_PRI_MSTR_TO_DIS    (1 << 4)
#define DEC2155X_CC0_SEC_MSTR_TO_DIS    (1 << 5)
#define DEC2155X_CC0_DLYD_TRNS_ORD_CTRL (1 << 6)
#define DEC2155X_CC0_SERR_FWD_ENA       (1 << 7)
#define DEC2155X_CC0_US_DAC_PRFTCH_DIS  (1 << 8)
#define DEC2155X_CC0_MULT_DEV_ENA       (1 << 9)
#define DEC2155X_CC0_PRI_ACCESS_LKOUT   (1 << 10)
#define DEC2155X_CC0_SEC_LOCK_DIS       (1 << 11)
#define DEC2155X_CC0_VGA_MODE_MSK       (3 << 14)
#define DEC2155X_CC0_VGA_IGN            (0x0 << 14)
#define DEC2155X_CC0_VGA_FWD_PRI_TO_SEC (0x1 << 14)
#define DEC2155X_CC0_VGA_FWD_SEC_TO_PRI (0x2 << 14)

/* Chip Control 1 bits */

#define DEC2155X_CC1_PRI_PSTED_WRT_THRSH       (1 << 0)
#define DEC2155X_CC1_SEC_PSTED_WRT_THRSH       (1 << 1)
#define DEC2155X_CC1_PRI_DLYD_RD_THRSH_MSK     (0x3 << 2)
#define DEC2155X_CC1_PRI_DLYD_RD_THRSH_8DW     (0x0 << 2)
#define DEC2155X_CC1_PRI_DLYD_RD_THRSH_1LN_MRL (0x2 << 2)
#define DEC2155X_CC1_PRI_DLYD_RD_THRSH_1LN     (0x3 << 2)
#define DEC2155X_CC1_SEC_DLYD_RD_THRSH_MSK     (0x3 << 4)
#define DEC2155X_CC1_SEC_DLYD_RD_THRSH_8DW     (0x0 << 4)
#define DEC2155X_CC1_SEC_DLYD_RD_THRSH_1LN_MRL (0x2 << 4)
#define DEC2155X_CC1_SEC_DLYD_RD_THRSH_1LN     (0x3 << 4)
#define DEC2155X_CC1_SUB_DCD_ENABLE_MSK        (0x3 << 6)
#define DEC2155X_CC1_SUB_DCD_DISABLE           (0 << 6)
#define DEC2155X_CC1_SUB_DCD_ENABLE_PRI        (0x1 << 6)
#define DEC2155X_CC1_SUB_DCD_ENABLE_SEC        (0x2 << 6)
#define DEC2155X_CC1_US_MEM2_PS_MSK            (0xf << 8)
#define DEC2155X_PSZ_OFST                      8
#define DEC2155X_PSZ_CONST                     128
#define DEC2155X_CC1_US_MEM2_PS_DIS            (0 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS256B            (0x1 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS512B            (0x2 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS1KB             (0x3 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS2KB             (0x4 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS4KB             (0x5 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS8KB             (0x6 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS16KB            (0x7 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS32KB            (0x8 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS64KB            (0x9 << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS128KB           (0xa << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS256KB           (0xb << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS512KB           (0xc << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS1MB             (0xd << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PZMB              (0xe << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_US_MEM2_PS4MB             (0xf << DEC2155X_PSZ_OFST)
#define DEC2155X_CC1_I2O_ENA                   (1 << 12)
#define DEC2155X_CC1_I2O_OFST                  13
#define DEC2155X_CC1_I2O_CONST                 256 
#define DEC2155X_CC1_I2O_SZ_MSK                (0x7 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ256                 (0x0 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ512                 (0x1 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ1K                  (0x2 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ2K                  (0x3 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ4K                  (0x4 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ8K                  (0x5 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ16K                 (0x6 << DEC2155X_CC1_I2O_OFST)
#define DEC2155X_CC1_I2O_SZ32K                 (0x7 << DEC2155X_CC1_I2O_OFST)

/* Reset Control Register bits */

#define DEC2155X_RCR_SEC_RST    (1 << 0)
#define DEC2155X_RCR_CHIP_RST   (1 << 1)
#define DEC2155X_RCR_SUB_STS    (1 << 2)
#define DEC2155X_RCR_L_STAT_STS (1 << 3)

/* Chip Status Bits */

#define DEC2155X_CHPSR_DS_DLYD_TRNS_MSTR_TO   (1 << 0)
#define DEC2155X_CHPSR_DS_DLYD_RD_TRNS_TO     (1 << 1)
#define DEC2155X_CHPSR_DS_DLYD_WRT_TRNS_TO    (1 << 2)
#define DEC2155X_CHPSR_DSM_PSTD_WRT_DATA_DISC (1 << 3)
#define DEC2155X_CHPSR_US_DLYD_TRNS_MSTR_TO   (1 << 8)
#define DEC2155X_CHPSR_US_DLYD_RD_TRNS_TO     (1 << 9)
#define DEC2155X_CHPSR_US_DLYD_WRT_TRNS_TO    (1 << 10)
#define DEC2155X_CHPSR_US_PSTD_WRT_DATA_DISC  (1 << 11)

/* Arbiter Control */

#define DEC2155X_ARB_CTRL_MSK ((1 << 10) - 1)

/* Primary and Secondary SERR# disable register bits */

#define DEC2155X_SERR_DIS_DLYD_TRNS_MSTR_ABRT (1 << 0)
#define DEC2155X_SERR_DIS_DLYD_RD_TRNS_TO     (1 << 1)
#define DEC2155X_SERR_DIS_DLYD_WRT_TRNS_DISC  (1 << 2)
#define DEC2155X_SERR_DIS_PSTD_WRT_DATA_DISC  (1 << 3)
#define DEC2155X_SERR_DIS_PSTD_WRT_TRGT_ABRT  (1 << 4)
#define DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT  (1 << 5)
#define DEC2155X_SERR_DIS_PSTD_WRT_PAR_ERROR  (1 << 6)
#define DEC2155X_SERR_RSV_MSK                 (1 << 7)

/* I2O Outbound and Inbound status and mask bits */

#define DEC2155X_I2O_PST_LST_STS     (1 << 3)
#define DEC2155X_I2O_PST_LST_MSK     (1 << 3)
#define DEC2155X_I2O_PST_LST_RSV_MSK (0xffff - I2O_PST_LST_STS)

/* Chip Status CSR, Chip Set IRQ Mask and Chip Clear IRQ Mask Registers */

#define DEC2155X_CHPCSR_PM_D0    (1 << 0)
#define DEC2155X_CHPCSR_SUB_EVNT (1 << 1)
#define DEC2155X_CHPCSR_RSV_MSK  0xfffc

#ifdef __cplusplus
    }
#endif

#endif  /* INCdec2155xCcpih */

