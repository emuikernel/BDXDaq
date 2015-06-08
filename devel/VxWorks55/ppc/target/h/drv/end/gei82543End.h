/* gei82543End.h - Intel 8254x network interface header */

/* Copyright 1990-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01g,28may02,jln add TX_RESTART_TRUE and TX_RESTART_NONE
01f,20apr02,jln support 82540/5/6 spr # 76739
01e,31jan02,jln change len of device name (spr#73010) 
01d,05dec01,jln add definitions for TBI compatibility workaround
01c,01oct01,jln move device ID definition to sysGei82543End.c
01b,01apr01,jln clean up after code review (partial spr#65326)
01a,08Jan01,jln written
*/

#ifndef __INCGEI82543Endh
#define __INCGEI82543Endh

/* includes */

#include "etherLib.h"
#include "miiLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1  /* tell gcc960 not to optimize alignments */
#endif    /* CPU_FAMILY==I960 */

/* define */

#define UNKNOWN -1
                 
#ifndef PHYS_TO_BUS_ADDR
#define PHYS_TO_BUS_ADDR(unit,physAddr)                                   \
    ((int) pDrvCtrl->adaptor.sysLocalToBus ?                                  \
    (*pDrvCtrl->adaptor.sysLocalToBus) (unit, physAddr) : physAddr)
#endif /* PHYS_TO_BUS_ADDR */

#ifndef BUS_TO_PHYS_ADDR
#define BUS_TO_PHYS_ADDR(unit,busAddr)                                     \
    ((int) pDrvCtrl->adaptor.sysBusToLocal ?                                  \
    (*pDrvCtrl->adaptor.sysBusToLocal)(unit, busAddr) : busAddr)
#endif /* BUS_TO_PHYS_ADDR */

#define TX_DESC_TYPE_CLEAN              0   /* TX descriptor type */
#define TX_DESC_TYPE_COPY               1      /* -- copy/chain/EOP */
#define TX_DESC_TYPE_CHAIN              2
#define TX_DESC_TYPE_EOP                3

/* TX descriptors structure */

#define TXDESC_BUFADRLOW_OFFSET         0   /* buf mem low offset */
#define TXDESC_BUFADRHIGH_OFFSET        4   /* buf mem high offset */
#define TXDESC_LENGTH_OFFSET            8   /* buf length offset */
#define TXDESC_CSO_OFFSET               10  /* checksum offset */
#define TXDESC_CMD_OFFSET               11  /* command offset */
#define TXDESC_STATUS_OFFSET            12  /* status offset */
#define TXDESC_CSS_OFFSET               13  /* cksum start */
#define TXDESC_SPECIAL_OFFSET           14  /* special field */
#define TXDESC_SIZE                     16  /* descriptor size */

#define TXDESC_CTX_IPCSS_OFFSET         0   /* CXT descriptor IPCSS */
#define TXDESC_CTX_TUCSS_OFFSET         4   /* CXT descriptor TUCSS */

/* RX descriptor structure */

#define RXDESC_BUFADRLOW_OFFSET         0   /* buf mem low offset */
#define RXDESC_BUFADRHIGH_OFFSET        4   /* buf mem high offset */
#define RXDESC_LENGTH_OFFSET            8   /* length offset */
#define RXDESC_CHKSUM_OFFSET            10  /* cksum offset */
#define RXDESC_STATUS_OFFSET            12  /* status offset */
#define RXDESC_ERROR_OFFSET             13  /* error offset */
#define RXDESC_SPECIAL_OFFSET           14  /* special offset */
#define RXDESC_SIZE                     16  /* descriptor size */

#define TX_ETHER_PHY_SIZE               (EH_SIZE + ETHERMTU + 2)

#define PRO1000_PCI_VENDOR_ID           0x8086 /* Intel vendor ID */
#define PRO1000_PCI_DEVICE_ID           0x1001 /* device ID */
#define PRO1000F_PCI_SUBSYSTEM_ID       0x1003 /* device subsystem ID */
#define PRO1000T_PCI_SUBSYSTEM_ID       0x1004
#define PRO1000T_PCI_DEVICE_ID          0x1004 /* bizard case */
#define PRO1000F_BOARD                  0x1003 /* backward compatible */
#define PRO1000T_BOARD                  0x1003 /* backward compatible */
#define INTEL_PCI_VENDOR_ID             0x8086

#define PRO1000_546_BOARD               0x100e /* 82540/82545/82546 MAC */
#define PRO1000_544_BOARD               0x1008 /* 82544 MAC */
#define PRO1000_543_BOARD               0x1003 /* 82543 MAC */

#define GEI_COPPER_MEDIA                1
#define GEI_FIBER_MEDIA                 2

#define GEI_PHY_GMII_TYPE               1

#define DEFAULT_RXRES_PROCESS_FACTOR    2
#define DEFAULT_TIMER_INTERVAL          2       /* 2 seconds */
#define DEFAULT_DRV_FLAGS               0
#define DEFAULT_LOAN_RXBUF_FACTOR       4
#define DEFAULT_MBLK_NUM_FACTOR         4
#define DEFAULT_MBUF_COPY_SIZE          256
#define DEFAULT_NUM_TXDES               24
#define DEFAULT_NUM_RXDES               24
#define DEFAULT_RXINT_DELAY             0
#define DEFAULT_MULTI_FILTER_TYPE       MULTI_FILTER_TYPE_47_36
#define DEFAULT_FLOW_CONTRL             FLOW_CONTRL_HW
#define DEFAULT_TX_REPORT               TX_REPORT_RS
#define DEFAULT_TIPG_IPGT_F             6
#define DEFAULT_TIPG_IPGT_T             8
#define DEFAULT_TIPG_IPGR1              8
#define DEFAULT_TIPG_IPGR2              6

#define GEI543_MAX_DEV_UNIT             4

/* flags available to config system */ 

#define GEI_END_SET_TIMER               0x0001 /* use a watchdog timer */   
#define GEI_END_SET_RX_PRIORITY         0x0002 /* RX has higher priority (543 only) */
#define GEI_END_FREE_RESOURCE_DELAY     0x0004 /* allow delay to free loaned TX cluster */ 
#define GEI_END_JUMBO_FRAME_SUPPORT     0x0008 /* Jumbo Frame allowed */
#define GEI_END_RX_IP_XSUM              0x0010 /* RX IP XSUM allowed, not supported */
#define GEI_END_RX_TCPUDP_XSUM          0x0020 /* RX TCP XSUM allowed, not supported */
#define GEI_END_TX_IP_XSUM              0x0040 /* TX IP XSUM allowed, not supported */
#define GEI_END_TX_TCPUDP_XSUM          0x0080 /* TX TCP XSUM allowed, not supported */
#define GEI_END_TX_TCP_SEGMENTATION     0x0100 /* TX TCP segmentation, not supported */
#define GEI_END_TBI_COMPATIBILITY       0x0200 /* TBI compatibility workaround (543 only) */
#define GEI_END_USER_MEM_FOR_DESC_ONLY  0x0400 /* cacheable user mem for RX descriptors only */ 

#define GEI_DEFAULT_RXDES_NUM           0x40
#define GEI_DEFAULT_TXDES_NUM           0x80
#define GEI_DEFAULT_USR_FLAG            (GEI_END_SET_TIMER | \
                                         GEI_END_SET_RX_PRIORITY | \
                                         GEI_END_FREE_RESOURCE_DELAY)

#define GEI_DEFAULT_ETHERHEADER         (SIZEOF_ETHERHEADER)
#define GEI_MAX_FRAME_SIZE              16288 /* based on default RX/TX 
                                               buffer configuration */

#define GEI_MAX_JUMBO_MTU_SIZE          (GEI_MAX_FRAME_SIZE - \
                                         GEI_DEFAULT_ETHERHEADER - \
                                         ETHER_CRC_LENGTH)   

#define GEI_DEFAULT_JUMBO_MTU_SIZE      9000      /* 9000 bytes */

#define AVAIL_TX_INT                    (INT_TXDW_BIT)
#define AVAIL_RX_INT                    (INT_RXO_BIT | INT_RXTO_BIT)
#define AVAIL_RX_TX_INT                 (AVAIL_TX_INT | AVAIL_RX_INT)
#define AVAIL_LINK_INT                  (INT_LSC_BIT)
#define INT_LINK_CHECK                  (AVAIL_LINK_INT | INT_RXCFG_BIT)
#define INTEL_82543GC_VALID_INT         (AVAIL_RX_TX_INT | INT_LINK_CHECK | INT_TXDLOW_BIT)
 
#define MAX_TXINT_DELAY                 65536
#define MAX_RXINT_DELAY                 65536
#define MIN_TXINT_DELAY                 1
#define TXINT_DELAY_LESS                5
#define TXINT_DELAY_MORE                512 

#define ETHER_CRC_LENGTH                4
#define RX_CRC_LEN                      ETHER_CRC_LENGTH
#define MAX_ETHER_PACKET_SIZE           1514 
#define MIN_ETHER_PACKET_SIZE           60
#define ETHER_ADDRESS_SIZE              6

#define CARRIER_EXTENSION_BYTE          0x0f

#define INTEL_82543GC_MTA_NUM           128

#define INTEL_82543GC_MULTIPLE_DES      8

#define TX_COLLISION_THRESHOLD          16
#define FDX_COLLISION_DISTANCE          64
#define HDX_COLLISION_DISTANCE          64
#define BIG_HDX_COLLISION_DISTANCE      512

#define NUM_RAR                         16
#define NUM_MTA                         128
#define NUM_VLAN                        128
#define MAX_NUM_MULTI                   (NUM_RAR + NUM_MTA - 1)

#define MULTI_FILTER_TYPE_47_36         0
#define MULTI_FILTER_TYPE_46_35         1
#define MULTI_FILTER_TYPE_45_34         2
#define MULTI_FILTER_TYPE_43_32         3

#define FLOW_CONTRL_NONE                0
#define FLOW_CONTRL_TRANSMIT            1
#define FLOW_CONTRL_RECEIVE             2
#define FLOW_CONTRL_ALL                 3
#define FLOW_CONTRL_HW                  0xf

#define TX_REPORT_RS                    1
#define TX_REPORT_RPS                   2

#define DMA_RX_PRIORITY                 1
#define DMA_FAIR_RX_TX                  2

#define FULL_DUPLEX_MODE                1
#define HALF_DUPLEX_MODE                2
#define DUPLEX_HW                       3
#define DEFAULT_DUPLEX_MODE             FULL_DUPLEX_MODE

#define END_SPEED_10M                   10000000    /* 10Mbs */
#define END_SPEED_100M                  100000000   /* 100Mbs */
#define END_SPEED_1000M                 1000000000  /* 1000Mbs */
#define END_SPEED                       END_SPEED_1000M

#define DEVICE_NAME                     "gei" 
#define DEVICE_NAME_LENGTH              4 

#define TYPE_PRO1000F_PCI               1
#define TYPE_PRO1000T_PCI               2
#define GEI82543_HW_AUTO                0x1
#define GEI82543_FORCE_LINK             0x2

/* general flags */

#define FLAG_POLLING_MODE               0x0001
#define FLAG_PROMISC_MODE               0x0002
#define FLAG_ALLMULTI_MODE              0x0004
#define FLAG_MULTICAST_MODE             0x0008
#define FLAG_BROADCAST_MODE             0x0010

/* misc */

#define TX_RESTART_NONE                 0x4000 /* muxTxRestart not scheduled */
#define TX_RESTART_TRUE                 0x8000 /* muxTxRestart is scheduled */
#define LINK_STATUS_OK                  0
#define LINK_STATUS_ERROR               1
#define LINK_STATUS_UNKNOWN             2
#define FREE_ALL_AUTO                   1
#define FREE_ALL_FORCE                  2
#define TX_LOAN_TRANSMIT                1
#define TX_COPY_TRANSMIT                2
#define LINK_TIMEOUT_IN_QUAR_SEC        12 /* 3s */

#define GEI_MII_PHY_CAP_FLAGS           (MII_PHY_10 | MII_PHY_100 | \
                                         MII_PHY_FD | MII_PHY_HD | \
                                         MII_PHY_1000T_FD)
/* register area */

/* TX registers */

#define INTEL_82543GC_TDBAL             0x3800  /* Tx Descriptor Base Low */
#define INTEL_82543GC_TDBAH             0x3804  /* Tx Descriptor Base High */
#define INTEL_82543GC_TDLEN             0x3808  /* Tx Descriptor Length */
#define INTEL_82543GC_TDH               0x3810  /* Tx Descriptor Head */
#define INTEL_82543GC_TDT               0x3818  /* Tx Descriptor Tail */

/* RX registers */

#define INTEL_82543GC_RDBAL             0x2800  /* Rx Descriptor Base Low */
#define INTEL_82543GC_RDBAH             0x2804  /* Rx Descriptor Base High */
#define INTEL_82543GC_RDLEN             0x2808  /* Rx Descriptor Length */
#define INTEL_82543GC_RDH               0x2810  /* Rx Descriptor Head */
#define INTEL_82543GC_RDT               0x2818  /* Rx Descriptor Tail */

/* Interrupt registers */

#define INTEL_82543GC_ICR               0xc0    /* Interrupt Cause Read */
#define INTEL_82543GC_ICS               0xc8    /* Interrupt Cause Set */
#define INTEL_82543GC_IMS               0xd0    /* Interrupt Mask Set/Read */
#define INTEL_82543GC_IMC               0xD8    /* Interrupt Mask Clear */

#define INTEL_82543GC_CTRL              0x0     /* Device Control */
#define INTEL_82543GC_STATUS            0x8	/* Device Status */
#define INTEL_82543GC_EECD              0x10    /* EEPROM/Flash Data */
#define INTEL_82543GC_CTRL_EXT          0x18    /* Extended Device Control */
#define INTEL_82543GC_MDI               0x20    /* MDI Control */
#define INTEL_82543GC_FCAL              0x28    /* Flow Control Adr Low */
#define INTEL_82543GC_FCAH              0x2c    /* Flow Control Adr High */
#define INTEL_82543GC_FCT               0x30    /* Flow Control Type */
#define INTEL_82543GC_VET               0x38    /* VLAN EtherType */
#define INTEL_82546EB_ITR               0xc4    /* register for 82540/5/6 MAC only, 
                                                   interrupt throttle register */
#define INTEL_82543GC_RCTL              0x100   /* Receive Control */
#define INTEL_82543GC_FCTTV             0x170   /* Flow Ctrl TX Timer Value */
#define INTEL_82543GC_TXCW              0x178   /* TX Configuration Word */
#define INTEL_82543GC_RXCW              0x180   /* RX Configuration Word */
#define INTEL_82543GC_TCTL              0x400   /* TX control */
#define INTEL_82543GC_TIPG              0x410   /* Transmit IPG */
#define INTEL_82543GC_PBA               0x1000  /* Packet Buffer Allocation */
#define INTEL_82543GC_FCRTL             0x2160  /* Flow ctrl RX Threshold Lo*/
#define INTEL_82543GC_FCRTH             0x2168  /* Flow ctrl RX Threshold hi*/
#define INTEL_82543GC_RDTR              0x2820  /* Rx Delay Timer Ring */
#define INTEL_82546EB_RADV              0x282C  /* register for 82540/5/6 MAC only,
                                                   absolute Rx Delay Timer register */
#define INTEL_82543GC_TIDV              0x3820  /* Tx Interrupt Delay Value */
#define INTEL_82543GC_TXDCTL            0x3828  /* Transmit descriptor control */
#define INTEL_82546EB_TADV              0x382C  /* register for 82540/5/6 MAC only,
                                                   absolute Tx Interrupt Delay register */
#define INTEL_82543GC_RXCSUM            0x5000  /* Receive Checksum control */
#define INTEL_82543GC_MTA               0x5200  /* Multicast Table Array */
#define INTEL_82543GC_RAL               0x5400  /* Rx Adr Low */
#define INTEL_82543GC_RAH               0x5404  /* Rx Adr High */
#define INTEL_82543GC_VLAN              0x5600  /* VLAN Filter Table Array */

/* Statistic Registers */

#define INTEL_82543GC_CRCERRS           0x4000
#define INTEL_82543GC_ALGNERRC          0x4004
#define INTEL_82543GC_SYMERRS           0x4008
#define INTEL_82543GC_RXERRC            0x400c
#define INTEL_82543GC_MPC               0x4010
#define INTEL_82543GC_SCC               0x4014
#define INTEL_82543GC_ECOL              0x4018
#define INTEL_82543GC_MCC               0x401c
#define INTEL_82543GC_LATECOL           0x4020
#define INTEL_82543GC_COLC              0x4028
#define INTEL_82543GC_TUC               0x402c
#define INTEL_82543GC_DC                0x4030
#define INTEL_82543GC_TNCRS             0x4034
#define INTEL_82543GC_SEC               0x4038
#define INTEL_82543GC_CEXTEER           0x403c
#define INTEL_82543GC_RLEC              0x4040
#define INTEL_82543GC_XONRXC            0x4048
#define INTEL_82543GC_XONTXC            0x404c
#define INTEL_82543GC_XOFFRXC           0x4050
#define INTEL_82543GC_XOFFTXC           0x4054
#define INTEL_82543GC_FCRUC             0x4058
#define INTEL_82543GC_PRC64             0x405c
#define INTEL_82543GC_PRC127            0x4060
#define INTEL_82543GC_PRC255            0x4064
#define INTEL_82543GC_PRC511            0x4068
#define INTEL_82543GC_PRC1023           0x406c
#define INTEL_82543GC_PRC1522           0x4070
#define INTEL_82543GC_GPRC              0x4074
#define INTEL_82543GC_BPRC              0x4078
#define INTEL_82543GC_MPRC              0x407c
#define INTEL_82543GC_GPTC              0x4080
#define INTEL_82543GC_GORL              0x4088
#define INTEL_82543GC_GORH              0x408c
#define INTEL_82543GC_GOTL              0x4090
#define INTEL_82543GC_GOTH              0x4094
#define INTEL_82543GC_RNBC              0x40a0
#define INTEL_82543GC_RUC               0x40a4
#define INTEL_82543GC_RFC               0x40a8
#define INTEL_82543GC_ROC               0x40ac
#define INTEL_82543GC_RJC               0x40b0
#define INTEL_82543GC_TORL              0x40c0
#define INTEL_82543GC_TORH              0x40c4
#define INTEL_82543GC_TOTL              0x40c8
#define INTEL_82543GC_TOTH              0x40cc
#define INTEL_82543GC_TPR               0x40d0
#define INTEL_82543GC_TPT               0x40d4
#define INTEL_82543GC_PTC64             0x40d8
#define INTEL_82543GC_PTC127            0x40dc
#define INTEL_82543GC_PTC255            0x40e0
#define INTEL_82543GC_PTC511            0x40e4
#define INTEL_82543GC_PTC1023           0x40e8
#define INTEL_82543GC_PTC1522           0x40ec
#define INTEL_82543GC_MPTC              0x40f0
#define INTEL_82543GC_BPTC              0x40f4
#define INTEL_82543GC_TSCTC             0x40f8
#define INTEL_82543GC_TSCTFC            0x40fc
#define INTEL_82543GC_RDFH              0x2410
#define INTEL_82543GC_RDFT              0x2418
#define INTEL_82543GC_RDFHS             0x2420
#define INTEL_82543GC_RDFTS             0x2428
#define INTEL_82543GC_RDFPC             0x2430
#define INTEL_82543GC_TDFH              0x3410
#define INTEL_82543GC_TDFT              0x3418
#define INTEL_82543GC_TDFHS             0x3420
#define INTEL_82543GC_TDFTS             0x3428
#define INTEL_82543GC_TDFPC             0x3430

/* Rx Configuration Word Field */

#define RXCW_C_BIT                      0x20000000

/* EEPROM Structure */

#define EEPROM_WORD_SIZE                0x40      /* 0-0x3f */
#define EEPROM_SUM                      0xBABA
#define EEPROM_INDEX_SIZE               0x40
#define EEPROM_INDEX_BITS               6
#define EEPROM_CMD_BITS                 3
#define EEPROM_DATA_BITS                16
#define EEPROM_READ_OPCODE              0x6
#define EEPROM_WRITE_OPCODE             0x5
#define EEPROM_ERASE_OPCODE             0x7

#define EEPROM_IA_ADDRESS               0x0
#define EEPROM_ICW1                     0xa
#define EEPROM_ICW1_SWDPIO_BITS         0x1e0
#define EEPROM_ICW1_SWDPIO_SHIFT        5
#define EEPROM_ICW1_ILOS_BIT            0x10
#define EEPROM_ICW1_FRCSPD_BIT          0x800
#define EEPROM_ICW1_ILOS_SHIFT          4

#define EEPROM_ICW2                     0xf
#define EEPROM_ICW2_PAUSE_BITS          0x3000
#define EEPROM_ICW2_ASM_DIR             0x2000
#define EEPROM_ICW2_SWDPIO_EXE_BITS     0xf0
#define EEPROM_ICW2_SWDPIO_EXE_SHIFT    4

#define BAR0_64_BIT                     0x04

/* TX Descriptor Command Fields */

#define TXD_CMD_EOP                     0x01
#define TXD_CMD_IFCS                    0x02
#define TXD_CMD_IC                      0x04
#define TXD_CMD_RS                      0x08
#define TXD_CMD_RPS                     0x10
#define TXD_CMD_DEXT                    0x20
#define TXD_CMD_VLE                     0x40
#define TXD_CMD_IDE                     0x80

/* for TX context descriptor only */

#define TXD_CMD_TCP                     0x01
#define TXD_CMD_IP 	                 	0x02
#define TXD_CMD_TSE                     0x04

/* TX Descriptor Status Fields */

#define TXD_STAT_DD                     0x01
#define TXD_STAT_EC                     0x02
#define TXD_STAT_LC                     0x04
#define TXD_STAT_TU                     0x08

/* TX Data Descriptor POPTS fields */

#define TXD_DATA_POPTS_IXSM_BIT         0x01
#define TXD_DATA_POPTS_TXSM_BIT         0x02

/* TX Data Descriptor DTYP Fileld */ 
#define TXD_DTYP_BIT                    0x01

/* RX Descriptor Status Field */

#define RXD_STAT_DD                     0x01
#define RXD_STAT_EOP                    0x02
#define RXD_STAT_IXSM                   0x04
#define RXD_STAT_VP                     0x08
#define RXD_STAT_RSV                    0x10
#define RXD_STAT_TCPCS                  0x20
#define RXD_STAT_IPCS                   0x40
#define RXD_STAT_PIF                    0x80

/* RX Descriptor Error Field */

#define RXD_ERROR_CE                    0x01        /* CRC or Align error */
#define RXD_ERROR_SE                    0x02        /* Symbol error */
#define RXD_ERROR_SEQ                   0x04        /* Sequence error */
#define RXD_ERROR_RSV                   0x08        /* reserved */
#define RXD_ERROR_CXE                   0x10        /* Carrier ext error */
#define RXD_ERROR_TCPE                  0x20        /* TCP/UDP CKSUM error */
#define RXD_ERROR_IPE                   0x40        /* IP CKSUM error */
#define RXD_ERROR_RXE                   0x80        /* RX data error */

/* Interrupt Register Fields */

#define INT_TXDW_BIT                    0x01        /* TX descriptor write-back */        
#define INT_TXQE_BIT                    0x02        /* TX ring empty */
#define INT_LSC_BIT                     0x04        /* link change interrupt */
#define INT_RXSEQ_BIT                   0x08        /* RX sequence error */
#define INT_RXDMT0_BIT                  0x10        /* RX descriptor Mini Threshold */
#define INT_RXO_BIT                     0x40        /* RX FIFO overrun */
#define INT_RXTO_BIT                    0x80        /* RX timer interrupt */
#define INT_MDAC_BIT                    0x200       /* MDIO complete interrupt */
#define INT_RXCFG_BIT                   0x400       /* Receiving /C/ ordered set */
#define INT_GPI0_BIT                    0x800       /* GPIO 0 interrupt (543 MAC) */
#define INT_GPI1_BIT                    0x1000      /* PHY (544 MAC) or GPIO1 (543) interrupt */ 
#define INT_GPI2_BIT                    0x2000      /* GPIO2 interrupt */
#define INT_GPI3_BIT                    0x4000      /* GPIO3 interrupt */
#define INT_TXDLOW_BIT                  0x8000      /* TX descriptor low threshold hit */

/* IMS register */ 

#define IMS_TXDW_BIT                    0x01
#define IMS_TXQE_BIT                    0x02
#define IMS_LSC_BIT                     0x04
#define IMS_RXSEQ_BIT                   0x08
#define IMS_RXDMT0_BIT                  0x10
#define IMS_RXO_BIT                     0x40
#define IMS_RXTO_BIT                    0x80
#define IMS_MDAC_BIT                    0x200
#define IMS_RXCFG_BIT                   0x400
#define IMS_TXDLOW_BIT                  0x8000

/* IMC register */

#define IMC_ALL_BITS                    0xffffffff
#define IMC_TXDW_BIT                    0x01
#define IMC_TXQE_BIT                    0x02
#define IMC_LSC_BIT                     0x04
#define IMC_RXSEQ_BIT                   0x08
#define IMC_RXDMT0_BIT                  0x10
#define IMC_RXO_BIT                     0x40
#define IMC_RXTO_BIT                    0x80
#define IMC_MDAC_BIT                    0x200
#define IMC_RXCFG_BIT                   0x400
#define IMC_TXDLOW_BIT                  0x8000

/* ICR register */

#define ICR_TXDW_BIT                    0x01
#define ICR_TXQE_BIT                    0x02
#define ICR_LSC_BIT                     0x04
#define ICR_RXSEQ_BIT                   0x08
#define ICR_RXDMT0_BIT                  0x10
#define ICR_RXO_BIT                     0x40
#define ICR_RXTO_BIT                    0x80
#define ICR_MDAC_BIT                    0x200
#define ICR_RXCFG_BIT                   0x400
#define ICR_TXDLOW_BIT                  0x8000

/* EEPROM Register Fields */

#define EECD_SK_BIT                     0x1
#define EECD_CS_BIT                     0x2
#define EECD_DI_BIT                     0x4
#define EECD_DO_BIT                     0x8
#define EECD_REQ_BIT                    0x40
#define EECD_GNT_BIT                    0x80
#define EECD_PRES_BIT                   0x100
#define EECD_SIZE_BIT                   0x200

/* RAT field */

#define RAH_AV_BIT                      0x80000000

/* Control Register Field */

#define CTRL_FD_BIT                     0x1
#define CTRL_PRIOR_BIT                  0x4
#define CTRL_ASDE_BIT                   0x20
#define CTRL_SLU_BIT                    0x40
#define CTRL_ILOS_BIT                   0x80
#define CTRL_SPD_100_BIT                0x100
#define CTRL_SPD_1000_BIT               0x200
#define CTRL_FRCSPD_BIT                 0x800
#define CTRL_FRCDPX_BIT                 0x1000
#define CTRL_SWDPIN0_BIT                0x40000
#define CTRL_SWDPIN1_BIT                0x80000
#define CTRL_SWDPIN2_BIT                0x100000
#define CTRL_SWDPIN3_BIT                0x200000
#define CTRL_SWDPIO0_BIT                0x400000
#define CTRL_SWDPIO1_BIT                0x800000
#define CTRL_SWDPIO2_BIT                0x1000000
#define CTRL_SWDPIO3_BIT                0x2000000
#define CTRL_RST_BIT                    0x4000000
#define CTRL_RFCE_BIT                   0x8000000
#define CTRL_TFCE_BIT                   0x10000000
#define CTRL_PHY_RST_BIT                0x80000000
#define CTRL_MDC_BIT                    CTRL_SWDPIN3_BIT
#define CTRL_MDIO_BIT                   CTRL_SWDPIN2_BIT
#define CTRL_MDC_DIR_BIT                CTRL_SWDPIO3_BIT
#define CTRL_MDIO_DIR_BIT               CTRL_SWDPIO2_BIT
#define CTRL_SPEED_MASK                 0x300
#define CTRL_SWDPIOLO_SHIFT             22
#define CTRL_ILOS_SHIFT                 7

/* Receive Control Register Fields */

#define RCTL_MO_SHIFT                   12
#define RCTL_BSIZE_2048                 0
#define RCTL_BSIZE_4096                 0x00030000
#define RCTL_BSIZE_8192                 0x00020000
#define RCTL_BSIZE_16384                0x00010000
#define RCTL_EN_BIT                     0x00000002
#define RCTL_SBP_BIT                    0x00000004
#define RCTL_UPE_BIT                    0x00000008
#define RCTL_MPE_BIT                    0x00000010
#define RCTL_LPE_BIT                    0x00000020
#define RCTL_BAM_BIT                    0x00008000
#define RCTL_BSEX_BIT                   0x02000000

/* MDI register fields */

#define MDI_WRITE_BIT                   0x4000000
#define MDI_READ_BIT                    0x8000000
#define MDI_READY_BIT                   0x10000000
#define MDI_ERR_BIT                     0x40000000
#define MDI_REG_SHIFT                   16
#define MDI_PHY_SHIFT                   21

/* Flow Control Field and Initial value */

#define FCRTL_XONE_BIT                  0x80000000
#define FLOW_CONTROL_LOW_ADR            0x00c28001
#define FLOW_CONTROL_HIGH_ADR           0x00000100
#define FLOW_CONTROL_TYPE               0x8808
#define FLOW_CONTROL_TIMER_VALUE        0x100
#define FLOW_CONTROL_LOW_THRESH         0x4000
#define FLOW_CONTROL_HIGH_THRESH        0x8000

/* Extended Control Register Fields */

#define CTRL_EXT_SWDPIN4_BIT            0x10
#define CTRL_EXT_SWDPIN5_BIT            0x20
#define CTRL_EXT_SWDPIN6_BIT            0x40
#define CTRL_EXT_SWDPIN7_BIT            0x80
#define CTRL_EXT_SWDPIO4_BIT            0x100
#define CTRL_EXT_SWDPIO5_BIT            0x200
#define CTRL_EXT_SWDPIO6_BIT            0x400
#define CTRL_EXT_SWDPIO7_BIT            0x800
#define CTRL_EXT_SWDPIOHI_SHIFT         8

#define CTRL_PHY_RESET_DIR4_BIT         CTRL_EXT_SWDPIO4_BIT
#define CTRL_PHY_RESET4_BIT             CTRL_EXT_SWDPIN4_BIT

/* Status Register Fields */

#define STATUS_FD_BIT                   0x1
#define STATUS_LU_BIT                   0x2
#define STATUS_TBIMODE_BIT              0x20
#define STATUS_SPEED_100_BIT            0x40
#define STATUS_SPEED_1000_BIT           0x80

/* TX control Regsiter Fields */

#define TCTL_EN_BIT                     0x2
#define TCTL_PSP_BIT                    0x8
#define TCTL_PBE_BIT                    0x800000
#define TCTL_COLD_BIT                   0x3ff000
#define TCTL_COLD_SHIFT                 12
#define TCLT_CT_SHIFT                   4

/* TIPG Register Fields */

#define TIPG_IPGR1_SHIFT                10
#define TIPG_IPGR2_SHIFT                20

/* RDTR Register Field */

#define RDTR_FPD_BIT                    0x80000000

/* TX Configuration Word fields */

#define TXCW_ANE_BIT                    0x80000000
#define TXCW_FD_BIT                     0x20
#define TXCW_ASM_DIR                    0x100
#define TXCW_PAUSE_BITS                 0x180

/* RXCSUM register field */

#define RXCSUM_IPOFL_BIT                0x0100
#define RXCSUM_TUOFL_BIT                0x0200

/* PHY's Registers And Initial Value*/

#define PHY_PREAMBLE                    0xFFFFFFFF
#define PHY_PREAMBLE_SIZE               32
#define PHY_WR_OP                       0x01
#define PHY_RD_OP                       0x02
#define PHY_TURNAR                      0x02
#define PHY_MARK                        0x01

#define CL_OVERHEAD                     4

/* Type define */

typedef struct txDescManager TX_DESCTL;

typedef TX_DESCTL * P_TX_DESCTL; 

struct txDescManager    
    {
    M_BLK_ID  mBlk;             /* location for mBlk */
    UINT32    txType;           /* TX desc type */
    }; 

typedef struct devDrv_stat
    {
    UINT32 rxtxHandlerNum;      /* num of rxtx handle routine was called per sec */ 
    UINT32 rxIntCount;		/* num of rx interrupt per sec */
    UINT32 txIntCount;		/* num of tx interrupt per sec */
    UINT32 rxORunIntCount;	/* num of rx overrun interrupt per sec */
    UINT32 rxPacketNum;         /* num of rx packet per sec */
    UINT32 txPacketNum;         /* num of tx packet per sec */
    UINT32 rxPacketDrvErr;	/* num of pkt rx error on driver per sec */
    } DEVDRV_STAT;

typedef struct dev_Timer
    {
    UINT32 rdtrVal;             /* RDTR, unit of 1.024ns */
    UINT32 radvVal;             /* RADV, unit of 1.024us */
    UINT32 itrVal;              /* ITR,  unit of 256 ns  */
    UINT32 tidvVal;             /* TIDV, unit of 1.024us */
    UINT32 tadvVal;             /* TADV, unit of 1.024us */
    UINT32 watchDogIntVal;      /* interval of watchdog, unit of second */
    } DEV_TIMER;

struct adapter_info 
    {
    int    vector;          /* interrupt vector */
    UINT32 regBaseLow;      /* register PCI base address - low  */
    UINT32 regBaseHigh;     /* register PCI base address - high */
    UINT32 flashBase;       /* flash PCI base address */

    BOOL   adr64;           /* indictor of 64 bit address */
    UINT32 boardType;       /* board type */
    UINT32 phyType;         /* PHY type (MII/GMII) */

    UINT32 delayUnit;       /* delay unit(in ns) for the delay function */
    FUNCPTR delayFunc;      /* BSP specified delay function */        

    STATUS (*intEnable)(int);    /* board specific interrupt enable routine */
    STATUS (*intDisable)(int);   /* board specific interrupt disable routine */
    STATUS (*intAck) (int);      /* interrupt ack */

    void   (*phySpecInit)(PHY_INFO *, UINT8); /* vendor specified PHY's init */
    UINT32 (*sysLocalToBus)(int,UINT32);
    UINT32 (*sysBusToLocal)(int,UINT32);

    FUNCPTR intConnect;     /* interrupt connect function */ 
    FUNCPTR intDisConnect;  /* interrupt disconnect function */

    UINT16  eeprom_icw1;    /* ICW1 in EEPROM */
    UINT16  eeprom_icw2;    /* ICW2 in EEPROM */

    UCHAR   enetAddr[6];    /* Ether address for this adaptor */ 
    UCHAR   reserved1[2];   /* reserved */

    FUNCPTR phyDelayRtn;    /* phy delay function */ 
    UINT32  phyMaxDelay;    /* max phy detection retry */
    UINT32  phyDelayParm;   /* delay parameter for phy delay function */
    UINT32  phyAddr;        /* phy Addr */

    BOOL   (*sysGeiDynaTimerSetup)(struct adapter_info *); /* adjust device's timer dynamically */
    BOOL   (*sysGeiInitTimerSetup)(struct adapter_info *); /* set the device's timer initially */

    DEVDRV_STAT devDrvStat;       /* statistic data for devices */
    DEV_TIMER   devTimerUpdate;   /* timer register value for update */
    };

typedef struct adapter_info ADAPTOR_INFO;

/* structure for Statistic registers */

typedef struct sta_reg 
    {
    UINT32    crcerrs;       /* CRC error count */
    UINT32    algnerrc;      /* alignment err count */
    UINT32    symerrs;       /* symbol err count */
    UINT32    rxerrc;        /* rx err count */
    UINT32    mpc;           /* missed packet count */
    UINT32    scc;           /* single collision count */
    UINT32    ecol;          /* excessive collision count */
    UINT32    mcc;           /* multi collision count */
    UINT32    latecol;       /* later collision count */
    UINT32    colc;          /* collision count */
    UINT32    tuc;           /* tx underun count */
    UINT32    dc;            /* defer count */
    UINT32    tncrs;         /* tx - no crs count */
    UINT32    sec;           /* sequence err count */
    UINT32    cexteer;       /* carrier extension count */
    UINT32    rlec;          /* rx length error count */
    UINT32    xonrxc;        /* XON receive count */
    UINT32    xontxc;        /* XON transmit count */
    UINT32    xoffrxc;       /* XOFF receive count */
    UINT32    xofftxc;       /* XFF transmit count */
    UINT32    fcruc;         /* FC received unsupported count */
    UINT32    prc64;         /* packet rx (64 byte) count */
    UINT32    prc127;        /* packet rx (65 - 127 byte) count */
    UINT32    prc255;        /* packet rx (128 - 255 byte) count */
    UINT32    prc511;        /* packet rx (256 - 511 byte) count */
    UINT32    prc1023;       /* packet rx (512 - 1023 byte) count */
    UINT32    prc1522;       /* packet rx (1024 - 1522 byte) count */
    UINT32    gprc;          /* good packet received count */
    UINT32    bprc;          /* broadcast packet received count */
    UINT32    mprc;          /* Multicast packet received count */
    UINT32    gptc;          /* good packet transmit count */
    UINT32    gorl;          /* good octets receive count (low) */
    UINT32    gorh;          /* good octets received count (high) */
    UINT32    gotl;          /* good octets transmit count (lo) */
    UINT32    goth;          /* good octets transmit count (hi) */
    UINT32    rnbc;          /* receive no buffer count */
    UINT32    ruc;           /* receive undersize count */
    UINT32    rfc;           /* receive fragment count */
    UINT32    roc;           /* receive oversize count */
    UINT32    rjc;           /* receive Jabber count */
    UINT32    torl;          /* total octets received (lo) */
    UINT32    torh;          /* total octets received (hi) */
    UINT32    totl;          /* total octets transmit (lo) */
    UINT32    toth;          /* total octets transmit (hi) */
    UINT32    tpr;           /* total packet received */
    UINT32    tpt;           /* total packet transmit */
    UINT32    ptc64;         /* packet transmit (64 byte) count */
    UINT32    ptc127;        /* packet transmit (65-127 byte) count */
    UINT32    ptc255;        /* packet transmit (128-255 byte) count */  
    UINT32    ptc511;        /* packet transmit (256-511 byte) count */
    UINT32    ptc1023;       /* packet transmit (512-1023 byte) count */
    UINT32    ptc1522;       /* packet transmit (1024-1522 byte) count */
    UINT32    mptc;          /* Multicast packet transmit count */
    UINT32    bptc;          /* Broadcast packet transmit count */
    UINT32    tsctc;         /* TCP segmentation context tx count */
    UINT32    tsctfc;        /* TCP segmentation context tx fail count */      
    UINT32    rdfh;          /* rx data FIFO head */
    UINT32    rdft;          /* rx data FIFO tail */
    UINT32    rdfhs;         /* rx data FIFO head saved register */
    UINT32    rdfts;         /* rx data FIFO tail saved register */
    UINT32    rdfpc;         /* rx data FIFO packet count */
    UINT32    tdfh;          /* tx data FIFO head */
    UINT32    tdft;          /* tx data FIFO tail */
    UINT32    tdfhs;         /* tx data FIFO head saved register */
    UINT32    tdfts;         /* tx data FIFO tail saved register */
    UINT32    tdfpc;         /* tx data FIFO packet count */
    } STA_REG;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#if defined(__STDC__) || defined(__cplusplus)
IMPORT END_OBJ * gei82543EndLoad (char * initString);
#else
IMPORT END_OBJ * gei82543EndLoad ();
#endif /* defined(__STDC__) || defined(__cplusplus) */

#ifdef __cplusplus
}
#endif

#endif /* __INCGEI82543Endh */


