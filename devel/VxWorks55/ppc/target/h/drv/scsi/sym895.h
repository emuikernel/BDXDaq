/* sym895.h - SYM 895 Script SCSI Controller header file */

/* Copyright 1989-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23jun00,bsp  added description for register access mode macro.
01a,06feb00,bsp  Created ; adapted from ncr810.h.
*/

#ifndef __INCsym895h
#define __INCsym895h


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "semLib.h"
#include "scsiLib.h"

/* Bit register definitions for the SYM895 */

/* SCSI Control Zero - SCNTL0 */

#define SYM895_SCNTL0_ARB1     0x80  /* Arbitration Control Bits. */
#define SYM895_SCNTL0_ARB0     0x40  /* define Simple or Full arbitration. */
#define SYM895_SCNTL0_START    0x20  /* Start arbitration sequence */
#define SYM895_SCNTL0_WATN     0x10  /* SATN/signal ctrl on Start sequence */
#define SYM895_SCNTL0_EPC      0x08  /* Parity Check enable */
#define SYM895_SCNTL0_AAP      0x02  /* SATN/ signal control on Parity Error */
#define SYM895_SCNTL0_TRG      0x01  /* Initator/Target mode set */

/* SCSI Control One - SCNTL1 */

#define SYM895_SCNTL1_EXC      0x80  /* Extra Clock Period for data setup */
#define SYM895_SCNTL1_ADB      0x40  /* Asset SCSI Data Bus */
#define SYM895_SCNTL1_DHP      0x20  /* Disable Halt on Parity/ATN (tgt only) */
#define SYM895_SCNTL1_CON      0x10  /* Bus Connection status */
#define SYM895_SCNTL1_RST      0x08  /* Assert RST/ */
#define SYM895_SCNTL1_AESP     0x04  /* Force parity error on SCSI Bus */
#define SYM895_SCNTL1_IARB     0x02  /* Immediate arbitration */
#define SYM895_SCNTL1_SST      0x01  /* Start SCSI transfer */

/* SCNTL2 */

#define SYM895_SCNTL2_SDU      0x80  /* SCSI Disconnect Unexpected */
#define SYM895_SCNTL2_CHM      0x40  /* Chained Move (automatically set/reset)*/
#define SYM895_SCNTL2_SLPMD    0x20  /* Longitudianal parity mode related */
#define SYM895_SCNTL2_SLPHBEN  0x10  /* Longitudianal parity mode related */
#define SYM895_SCNTL2_WSS      0x08  /* Wide SCSI Send */
#define SYM895_SCNTL2_VUE0     0x04  /* Vendor Unique Enhansements related */
#define SYM895_SCNTL2_VUE1     0x02  /* Vendor Unique Enhansements related */
#define SYM895_SCNTL2_WSR      0x01  /* Wide SCSI Receive */
 
/* SCNTL3 */

#define SYM895_SCNTL3_ULTRA    0x80  /* Ultra Enable */
#define SYM895_SCNTL3_SCF_MASK 0x70  /* Syncronous Clock Conv. Factor bits */
#define SYM895_SCNTL3_EWS      0x08  /* Enable WIDE SCSI */
#define SYM895_SCNTL3_CCF_MASK 0x07  /* Clock Conversion Factor bits */

/* SCID */

#define SYM895_SCID_RRE        0x40  /* Enable Response to Reselection */
#define SYM895_SCID_SRE        0x20  /* Enable Response to Selection   */
#define SYM895_SCID_ENC_MASK   0x0f  /* The Encoded SCSI ID mask */

/* SXFER */

#define SYM895_SXFER_XFERP_MASK   0xE0 /* Synchronous transfer period */
#define SYM895_SXFER_OFF_MASK     0x1f /* Sync. Transfer Max. Offset period */

/* SDID */

#define SYM895_SDID_ENC_MASK      0x0f /* The Encoded SCSI ID mask */

/* GPREG */

#define SYM895_GPREG_MASK         0x1f /* Mask for IO pins */
#define SYM895_GPREG_GPIO4        0x10 /* Programmable IO Pin */
#define SYM895_GPREG_GPIO3        0x08 /* Programmable IO Pin */
#define SYM895_GPREG_GPIO2        0x04 /* Programmable IO Pin */
#define SYM895_GPREG_GPIO1        0x02 /* Programmable IO Pin */
#define SYM895_GPREG_GPIO0        0x01 /* Programmable IO Pin */

/* SOCL */

#define SYM895_SOCL_REQ           0x80 /* Assert SCSI REQ/ signal */
#define SYM895_SOCL_ACK           0x40 /* Assert SCSI ACK/ signal */
#define SYM895_SOCL_BSY           0x20 /* Assert SCSI BSY/ signal */
#define SYM895_SOCL_SEL           0x10 /* Assert SCSI SEL/ signal */
#define SYM895_SOCL_ATN           0x08 /* Assert SCSI ATN/ signal */
#define SYM895_SOCL_MSG           0x04 /* Assert SCSI MSG/ signal */
#define SYM895_SOCL_CD            0x02 /* Assert SCSI CD/ signal */
#define SYM895_SOCL_IO            0x01 /* Assert SCSI IO/ signal */

/* SSID */

#define SYM895_SSID_VAL           0x80	 /* SCSI Valid Bit.*/
#define SYM895_SSID_ENC_MASK      0x0f   /* Binary encoded SCSI ID mask 0-15 */

/* SBCL */

#define SYM895_SBCL_REQ           0x80   /* Get  SCSI REQ/ Status */
#define SYM895_SBCL_ACK           0x40   /* Get  SCSI ACK/ Status */
#define SYM895_SBCL_BSY           0x20   /* Get  SCSI BSY/ Status */
#define SYM895_SBCL_SEL           0x10   /* Get  SCSI SEL/ Status */
#define SYM895_SBCL_ATN           0x08   /* Get  SCSI ATN/ Status */
#define SYM895_SBCL_MSG           0x04   /* Get  SCSI MSG/ Status */
#define SYM895_SBCL_CD            0x02   /* Get  SCSI CD/ Status  */
#define SYM895_SBCL_IO            0x01   /* Get  SCSI IO/ Status  */

/* DSTAT (RO) and DIEN (RW enable intr) */

#define SYM895_B_DFE              0x80   /* Dma fifo empty */
#define SYM895_B_MDPE             0x40   /* Master Data Parity Error */
#define SYM895_B_BF               0x20	 /* Bus Fault */
#define SYM895_B_ABT              0x10	 /* Abort condition */
#define SYM895_B_SSI              0x08	 /* Scsi Step Interrupt */
#define SYM895_B_SIR              0x04	 /* Script Interrupt received */
#define SYM895_B_IID              0x01	 /* Illegal Instruction Detected */

/* SSTAT0 */

#define SYM895_SSTAT0_ILF         0x80   /* SIDL Input Latch Full */
#define SYM895_SSTAT0_ORF         0x40   /* SODR Output Register Full */
#define SYM895_SSTAT0_OLF         0x20   /* SODL Output Latch Full */
#define SYM895_SSTAT0_AIP         0x10   /* Arbitration In Progress */ 
#define SYM895_SSTAT0_LOA         0x08   /* LOst Arbitration */
#define SYM895_SSTAT0_WOA         0x04   /* WOn Arbitration */
#define SYM895_SSTAT0_RST         0x02   /* SCSI ReSeT signal in ISTAT reg */
#define SYM895_SSTAT0_SDP0        0x01   /* SCSI SDP/0 parity signal */

/* SSTAT1 RO register */

#define SYM895_SSTAT1_FIFO_MASK   0xf0   /* Also..see the SSTAT2 register */
#define SYM895_SSTAT1_SDP0L       0x08   /* Parity for latched scsi i/p data */
#define SYM895_SSTAT1_MSG         0x04   /* SCSI:MSG Signal, latched */		
#define SYM895_SSTAT1_CD          0x04	 /* SCSI:CD Signal,Latched */		
#define SYM895_SSTAT1_IO          0x04   /* SCSI:IO Signal, Latched */		

/* SSTAT2 */

#define SYM895_SSTAT2_ILF1        0x80   /* SIDL MSB full */
#define SYM895_SSTAT2_ORF1        0x40   /* SODR MSB full */
#define SYM895_SSTAT2_OLF1        0x20   /* SODL MSB full */
#define SYM895_SSTAT2_FF4         0x10   /* FIFO flags bit 4 */
#define SYM895_SSTAT2_SPL1        0x08   /* Latched SCSI Parity for SIDL */
#define SYM895_SSTAT2_DM          0x04   /* DIFFSENS Mismatch */
#define SYM895_SSTAT2_LDSC        0x02   /* Last Disconnect;used in */
                                         /* conjunction with CON bit in SCNTL1*/
#define SYM895_SSTAT2_SDP1        0x01   /* SCSI SDP1/ parity signal */

/* ISTAT RW register */

#define SYM895_ISTAT_ABRT         0x80	 /* Abort operation */
#define SYM895_ISTAT_SOFTRST      0x40	 /* Soft chip reset */
#define SYM895_ISTAT_SIGP         0x20	 /* signal process */
#define SYM895_ISTAT_SEM          0x10   /* Semaphore */
#define SYM895_ISTAT_CON          0x08	 /* stat connected (reset doesn't */
                                         /* disconnect) */
#define SYM895_ISTAT_INTF         0x04   /* set by INTFLY script instruction.*/
                                         /* Signals ISRs while scripts are   */    
                                         /* still running */
#define SYM895_ISTAT_SIP          0x02   /* SCSI Interrupt Pending */
#define SYM895_ISTAT_DIP          0x01   /* Dma Interrupt Pending */

/* CTEST0 */

#define SYM895_CTEST1_FMT_MASK	  0xf0   /* Byte Empty in DMA Fifo */
#define SYM895_CTEST1_FFL_MASK	  0x0f   /* Byte Full in DMA Fifo */

/* CTEST2 */

#define SYM895_CTEST2_DDIR        0x80   /* Data Direction. 1 => From */
                                         /* SCSI to board */
#define SYM895_CTEST2_SIGP        0x40	 /* Signal Process. Same as in ISTAT.*/
                                         /* When this register is read SIGP  */
                                         /* is cleared in ISTAT */
#define SYM895_CTEST2_CIO         0x20   /* Configured as I/O  */
#define SYM895_CTEST2_CM          0x10	 /* Configured as Memory */
#define SYM895_CTEST2_SRTCH       0x08   /* Configure Scratch regs */
#define SYM895_CTEST2_TEOP        0x04   /* Scsi True end of process */
#define SYM895_CTEST2_DREQ        0x02   /* Data Request Status */
#define SYM895_CTEST2_DACK        0x01	 /* Data Acknowledge Status */

/* CTEST3 */

#define SYM895_CTEST3_REV_MASK    0xf0	 /* Chip Revision Mask */
#define SYM895_CTEST3_FLF         0x08	 /* Flush DMA FIFO */
#define SYM895_CTEST3_CLF         0x04	 /* Clear DMA FIFO */
#define SYM895_CTEST3_FM          0x02	 /* Fetch Pin Mode */
#define SYM895_CTEST3_WRIE        0x01	 /* Write and Invalidate Enable */

/* DFIFO */

#define SYM895_FIFO_112_MASK      0x7f   /* 7 bits of Byte Offset counter */
                                         /* between  SCSI core and DMA core */
#define SYM895_FIFO_816_MASK      0x3ff  /* 10 bits (8 - DFIFO, 2 - CTEST5 ) */
                                         /* of SCSI Byte Offset counter */
                                         /* between  SCSI core and DMA core */

/* CTEST4 */

#define SYM895_CTEST4_BDIS        0x80   /* Burst transfer DISable */
#define SYM895_CTEST4_ZMOD        0x40	 /* High Impedance Mode */
#define SYM895_CTEST4_ZSD         0x20	 /* SCSI data high impedance */
#define SYM895_CTEST4_SRTM        0x10	 /* Shadow Register Test Mode */
#define SYM895_CTEST4_MPEE        0x08   /* Master Parity Error Enable */
#define SYM895_CTEST4_FBL_MASK    0x07   /* Fifo byte control */

/* CTEST5 */

#define SYM895_CTEST5_ADCK        0x80   /* Increment DNAD register */
#define SYM895_CTEST5_BBCK        0x40   /* Decrement DBC register */
#define SYM895_CTEST5_DFS         0x20   /* DMA FIFO Size: 0=112, 1=816 bytes */
#define SYM895_CTEST5_MASR        0x10   /* Master Ctrl for Set/Reset Pulses */
#define SYM895_CTEST5_BL2         0x04   /* Burst Length bit 2 */
#define SYM895_CTEST5_BO9         0x02   /* b9 of DMA Fifo byte offset counter*/
#define SYM895_CTEST5_BO8         0x01   /* b8 of DMA Fifo byte offset counter*/

/* CTEST6 */

#define SYM895_CTEST6_DF_MASK     0xff   /* Dma Fifo bits - mask */

/* DMODE RW register */

#define SYM895_DMODE_BL1          0x80   /* Burst Length transfer bits; */
                                         /* determine the number of transfers */
                                         /* per bus ownership */
#define SYM895_DMODE_BL0          0x40	 /* 00=2,01=4,10=8,11=16 */
#define SYM895_DMODE_SIOM         0x20	 /* Source I/O Memory Enable; */
                                         /* 1 => I/O */
#define SYM895_DMODE_DIOM         0x10   /* Destination I/O Memory Enable; */
#define SYM895_DMODE_ERL          0x08   /* Enable PCI Read Line command */
#define SYM895_DMODE_ERMP         0x04   /* Enable PCI Read Multiple command */
#define SYM895_DMODE_BOF          0x02   /* Burst Opcode Fetch enable; */
                                         /* instrs fetched in burst mode */
#define SYM895_DMODE_MAN          0x01   /* Manual start mode ,when set   */
                                         /* disable autostart script when */ 
                                         /* writting in DSP */

/* DCNTL - DMA Control bits */

#define SYM895_DCNTL_CLSE         0x80   /* Cache Line Size Enable */
#define SYM895_DCNTL_PFF          0x40   /* Pre-Fetch Flush */
#define SYM895_DCNTL_PFEN         0x20   /* Pre-Fetch ENable */ 
#define SYM895_DCNTL_SSM          0x10   /* Single-Step Mode */
#define SYM895_DCNTL_IRQM         0x08   /* IRQ Mode */
#define SYM895_DCNTL_STD          0x04   /* Start Dma operation; */
#define SYM895_DCNTL_IRQD         0x02   /* IRQ Disable */
#define SYM895_DCNTL_COM          0x01   /* Compatability 1 => ncr700 mode  */

/* 
 * SIEN0 (enable intr)and SIST0 (RO Status) 
 * Enable interrupt 
 */

#define SYM895_B_MA            0x80  /* Phase Mismatch / ATN interrupt*/
#define SYM895_B_CMP           0x40  /* Funtion CoMPlete; interrupt when full */ 
                                     /* arbitration and selection is complete */
#define SYM895_B_SEL           0x20  /* Selected; intr when selected as tgt */
#define SYM895_B_RSL           0x10  /* Reselected; intr when reselected */
#define SYM895_B_SGE           0x08  /* SCSI Gross Error; intr enable */
#define SYM895_B_UDC           0x04  /* Enable Unexpected Disconnect intr */
#define SYM895_B_RST           0x02  /* Enable Reset intr */
#define SYM895_B_PAR           0x01  /* Enable parity intr */

/* SIEN1 (enable intr) and SIST1 (RO status) */

#define SYM895_B_SBMC          0x10  /* SCSI Bus Mode Change Interrupt */
#define SYM895_B_STO           0x04  /* Selection/Reselection Timeout */
#define SYM895_B_GEN           0x02  /* GENeral purpose timer expired */
#define SYM895_B_HTH           0x01  /* Handshake to Handshake timer expired */

/* MACNTL - Memory Access Control (RW) */

#define SYM895_MACNTL_TYP_MASK 0xf0  /* Identify the chip type */
#define SYM895_MACNTL_DWR      0x08  /* Data WRite is local memeory access */
#define SYM895_MACNTL_DRD      0x04  /* Data ReaD is local memory access */
#define SYM895_MACNTL_PSCPT    0x02  /* Pointer to SCRIPTs is local mem access*/
#define SYM895_MACNTL_SCPTS    0x01  /* SCRIPTS fetch is local mem access */

/* 
 * GPCNTL - General Purpose Pin Control (RW) 
 * This reg is used to determine if GPREG pins are inputs or outputs 
 */

#define SYM895_GPCNTL_ME       0x80  /* Master Enable;internal busmaster:GPIO1*/
#define SYM895_GPCNTL_FE       0x40  /* Fetch Enable; */ 
                                     /* internal op-code fetch GPIO0*/
#define SYM895_GPCNTL_GPIO4    0x10  /* GPIO4: set - input and reset - output */
#define SYM895_GPCNTL_GPIO3    0x08  /* GPIO3: set - input and reset - output */
#define SYM895_GPCNTL_GPIO2    0x04  /* GPIO2: set - input and reset - output */
#define SYM895_GPCNTL_GPIO1    0x02  /* GPIO1: set - input and reset - output */
#define SYM895_GPCNTL_GPIO0    0x01  /* GPIO0: set - input and reset - output */

/* STIME0  SCSI Timer Zero (RW) */

/* These bits select various timeout values.e.g 0000 => disable; 1111 => 1.6s */

#define SYM895_STIME0_HTH_MASK 0xf0  /* Handshake-To-Handshake period mask*/
#define SYM895_STIME0_SEL_MASK 0x0f  /* SELection timeout mask */

/* STIME1 SCSI Timer One (RW) */

#define SYM895_STIME1_HTHBA    0x40  /* HTH Timer Bus Activity Enable */
#define SYM895_STIME1_GENSF    0x20  /* General Purpose Timer Scale Factor */
#define SYM895_STIME1_HTHSF    0x10  /* HTH Timer Scale Factor */
#define SYM895_STIME1_GEN_MASK 0x0f  /* GENeral timer timeout mask */

/* STEST0 SCSI Test Zero (RO) */

#define SYM895_STEST0_SSAID_MASK    0xf0 /* SCSI Selected As ID */
#define SYM895_STEST0_SLT           0x08 /* Selection response logic test */
#define SYM895_STEST0_ART           0x04 /* Arbitation Priority encoder Test */
#define SYM895_STEST0_SOZ           0x02 /* SCSI Synchronous Offset Zero */
#define SYM895_STEST0_SOM           0x01 /* SCSI Synchronous Offset Maximum */

/* STEST1 SCSI Test One (RW) */

#define SYM895_STEST1_SCLK          0x80 /* Disables external SCLK and uses */
                                         /* PCI internal SCSI clock */
#define SYM895_STEST1_SISO          0x40 /* SCSI Isolation Mode; inputs  */
                                         /* isolated from the SCSI bus */
#define SYM895_STEST1_QEN           0x08 /* Enable: Power on clock Quadrupler */
#define SYM895_STEST1_QSEL          0x04 /* Select: Increase clock to 160 MHz */

/* STEST2 SCSI Test Two (RW) */

#define SYM895_STEST2_SCE           0x80 /* SCSI Control Enable */
#define SYM895_STEST2_ROF           0x40 /* Reset SCSI OFfset */
#define SYM895_STEST2_DIF           0x20 /* SCSI Differential Mode */
#define SYM895_STEST2_SLB           0x10 /* SCSI Loopback Mode */
#define SYM895_STEST2_SZM           0x08 /* SCSI High-Impedance Mode */
#define SYM895_STEST2_AWS           0x04 /* Always Wide SCSI */
#define SYM895_STEST2_EXT           0x02 /* Extend SREQ/SACK filtering -  */
                                         /* Should not be set during FAST */
                                         /* SCSI> 5MB/s */
#define SYM895_STEST2_LOW           0x01 /* SCSI LOW level mode; no DMA  */
                                         /* operation occur and no SCRIPTS */ 
                                         /* execute */

/* STEST3 SCSI Test Three (RW) */

#define SYM895_STEST3_TE            0x80 /* Tolerant Enable */ 
#define SYM895_STEST3_STR           0x40 /* SCSI FIFO Test Read */
#define SYM895_STEST3_HSC           0x20 /* Halt Scsi Clock */
#define SYM895_STEST3_DSI           0x10 /* Disable Single Initiator */
                                         /* response (SCSI-1) */
#define SYM895_STEST3_S16           0x08 /* 16-bit System */
#define SYM895_STEST3_TTM           0x04 /* Timer Test Mode */
#define SYM895_STEST3_CSF           0x02 /* Clear Scsi Fifo */
#define SYM895_STEST3_STW           0x01 /* SCSI Fifo Test Write */

/* STEST4 SCSI Test Four (R) */

#define SYM895_STEST4_SMODE_MASK    0xc0 /* SCSI Mode Detect */
#define SYM895_STEST4_LOCK          0x20 /* Detect Clock Quadrupler Lockup */

/* Byte Offsets of On-Chip Registers from the base address */

/* 
 * The registers can be accessed in two ways.One is using Memory IO base addr
 * plus the offset and the other by using PCI config base addr plus offset. 
 * These registers are part of PCI configuration space for the device.
 */

#define MEMIO_REG_BASE      (0x00)
#define PCI_CONFIG_REG_BASE (0x80)
#define BASE                MEMIO_REG_BASE

/* Little Endian offsets */

#define SYM895_OFF_SCNTL0  (BASE)          /* SCTNL0 SCSI control register 0 */
#define SYM895_OFF_SCNTL1  (BASE + 0x01)   /* SCTNL1 SCSI control register 1 */
#define SYM895_OFF_SCNTL2  (BASE + 0x02)   /* SCTNL2 SCSI control register 2 */
#define SYM895_OFF_SCNTL3  (BASE + 0x03)   /* SCTNL3 SCSI control register 3 */
#define SYM895_OFF_SCID    (BASE + 0x04)   /* SCID SCSI chip ID register */
#define SYM895_OFF_SXFER   (BASE + 0x05)   /* SXFER SCSI chip ID register */
#define SYM895_OFF_SDID    (BASE + 0x06)   /* SDID SCSI destination ID  */
#define SYM895_OFF_GPREG   (BASE + 0x07)   /* GPREG General Purpose register */
#define SYM895_OFF_SFBR    (BASE + 0x08)   /* SFBR SCSI first byte received  */
#define SYM895_OFF_SOCL    (BASE + 0x09)   /* SOCL SCSI output control latch */
#define SYM895_OFF_SSID    (BASE + 0x0a)   /* SSID SCSI selector ID */
#define SYM895_OFF_SBCL    (BASE + 0x0b)   /* SBCL SCSI bus control lines */
#define SYM895_OFF_DSTAT   (BASE + 0x0c)   /* DSTAT DMA status register */
#define SYM895_OFF_SSTAT0  (BASE + 0x0d)   /* SSTAT0 SCSI status register 0 */
#define SYM895_OFF_SSTAT1  (BASE + 0x0e)   /* SSTAT1 SCSI status register 0 */
#define SYM895_OFF_SSTAT2  (BASE + 0x0f)   /* SSTAT2 SCSI status register 0 */
#define SYM895_OFF_DSA     (BASE + 0x10)   /* 32-bit DSA data structure */
                                           /* address */
#define SYM895_OFF_ISTAT   (BASE + 0x14)   /* 8-bit ISTAT interrupt status  */
#define SYM895_OFF_CTEST0  (BASE + 0x18)   /* CTEST0 chip test register 0 */
#define SYM895_OFF_CTEST1  (BASE + 0x19)   /* CTEST1 chip test register 1 */
#define SYM895_OFF_CTEST2  (BASE + 0x1a)   /* CTEST2 chip test register 2 */
#define SYM895_OFF_CTEST3  (BASE + 0x1b)   /* CTEST3 chip test register 3 */
#define SYM895_OFF_TEMP    (BASE + 0x1c)   /* 32-bit TEMP temporary holding */
                                           /* register */
#define SYM895_OFF_DFIFO   (BASE + 0x20)   /* DFIFO DMA FIFO control register */
#define SYM895_OFF_CTEST4  (BASE + 0x21)   /* CTEST4 chip test register 4 */
#define SYM895_OFF_CTEST5  (BASE + 0x22)   /* CTEST5 chip test register 5 */
#define SYM895_OFF_CTEST6  (BASE + 0x23)   /* CTEST6 chip test register 6 */
#define SYM895_OFF_DBC     (BASE + 0x24)   /* 24-bit DBC SIOP command reg.*/
#define SYM895_OFF_DCMD    (BASE + 0x27)   /* 8-bit DCMD SIOP command register */
#define SYM895_OFF_DNAD    (BASE + 0x28)   /* 32-bit DNAD DMA buffer ptr */
                                           /* (next addr) */
#define SYM895_OFF_DSP     (BASE + 0x2c)   /* 32-bit DSP SIOP scripts pointer */
                                           /* register */
#define SYM895_OFF_DSPS    (BASE + 0x30)   /* 32-bit DSPS SIOP scripts ptr */
                                           /* save register */
#define SYM895_OFF_DMODE   (BASE + 0x38)   /* DMODE DMA operation mode */
                                           /* register */
#define SYM895_OFF_DIEN    (BASE + 0x39)   /* DIEN DMA interrupt enable */
#define SYM895_OFF_DWT     (BASE + 0x3a)   /* DWT DMA watchdog timer register */
#define SYM895_OFF_DCNTL   (BASE + 0x3b)   /* DCTNL DMA control register */	
#define SYM895_OFF_ADDER   (BASE + 0x3c)   /* 32-bit ADDER Adder output */
                                           /* Register */	
#define SYM895_OFF_SIEN0   (BASE + 0x40)   /* SIEN0 SCSI interrupt enable 0 */
                                           /* register */
#define SYM895_OFF_SIEN1   (BASE + 0x41)   /* SIEN1 SCSI interrupt enable 1 */
                                           /* register */
#define SYM895_OFF_SIST0   (BASE + 0x42)   /* SIST0 SCSI interrupt status 0 */
                                           /* register */
#define SYM895_OFF_SIST1   (BASE + 0x43)   /* SIST1 SCSI interrupt status 1 */
                                           /* register */
#define SYM895_OFF_SLPAR   (BASE + 0x44)   /* SLPAR SCSI longitudinal Parity*/
                                           /* register */
#define SYM895_OFF_SWIDE   (BASE + 0x45)   /* SWIDE SCSI Wide Residue */
#define SYM895_OFF_MACNTL  (BASE + 0x46)   /* MACNTL Memory Access Control */
                                           /* register */
#define SYM895_OFF_GPCNTL  (BASE + 0x47)   /* GPCNTL General Purpose Pin */
                                           /* Control */
#define SYM895_OFF_STIME0  (BASE + 0x48)   /* STIME0 SCSI Timer Zero reg */
#define SYM895_OFF_STIME1  (BASE + 0x49)   /* STIME1 SCSI Timer One reg */
#define SYM895_OFF_RESPID0 (BASE + 0x4a)   /* RESPID Response ID register 0 */
#define SYM895_OFF_RESPID1 (BASE + 0x4b)   /* RESPID Response ID register 1 */
#define SYM895_OFF_STEST0  (BASE + 0x4c)   /* STEST0 SCSI Test 0 register */
#define SYM895_OFF_STEST1  (BASE + 0x4d)   /* STEST1 SCSI Test 1 register */
#define SYM895_OFF_STEST2  (BASE + 0x4e)   /* STEST2 SCSI Test 2 register */
#define SYM895_OFF_STEST3  (BASE + 0x4f)   /* STEST3 SCSI Test 3 register */
#define SYM895_OFF_SIDL    (BASE + 0x50)   /* 16-bit SIDL SCSI input data */
                                           /* latch register */
#define SYM895_OFF_STEST4  (BASE + 0x52)   /* STEST4 SCSI Test 4 register */
#define SYM895_OFF_SODL    (BASE + 0x54)   /* 16-bit SCSI output data latch */
                                           /* register */
#define SYM895_OFF_SBDL    (BASE + 0x58)   /* 16-bit SBDL SCSI bus data lines */
                                           /* register */

#define SYM895_OFF_SCRATCHA0	(BASE + 0x34)	/* SCRATCHA0 gen purpose */
                                                /* scratch register A0 */
#define SYM895_OFF_SCRATCHA1	(BASE + 0x35)	/* SCRATCHA1 gen purpose */ 
                                                /* scratch reg A1 */
#define SYM895_OFF_SCRATCHA2	(BASE + 0x36)	/* SCRATCHA2 gen purpose */
                                                /* scratch reg A2 */
#define SYM895_OFF_SCRATCHA3	(BASE + 0x37)	/* SCRATCHA3 gen purpose */
                                                /* scratch reg A3 */
#define SYM895_OFF_SCRATCHB0	(BASE + 0x5c)	/* SCRATCHB0 gen purpose */
                                                /* scratch reg B0 */
#define SYM895_OFF_SCRATCHB	(BASE + 0x5c)	/* SCRATCHB Scratch Register B */
#define SYM895_OFF_SCRATCHC0	(BASE + 0x60)	/* SCRATCHC Scratch Register C */
#define SYM895_OFF_SCRATCHC1	(BASE + 0x61)	/* SCRATCHC Scratch Register C */
#define SYM895_OFF_SCRATCHC2	(BASE + 0x62)	/* SCRATCHC Scratch Register C */
#define SYM895_OFF_SCRATCHC	(BASE + 0x60)	/* SCRATCHC Scratch Register C */
#define SYM895_OFF_SCRATCHD	(BASE + 0x64)	/* SCRATCHD Scratch Register D */
#define SYM895_OFF_SCRATCHE	(BASE + 0x68)	/* SCRATCHE Scratch Register E */
#define SYM895_OFF_SCRATCHF	(BASE + 0x6c)	/* SCRATCHF Scratch Register F */
#define SYM895_OFF_SCRATCHG	(BASE + 0x70)	/* SCRATCHG Scratch Register G */
#define SYM895_OFF_SCRATCHH	(BASE + 0x74)	/* SCRATCHH Scratch Register H */
#define SYM895_OFF_SCRATCHI	(BASE + 0x78)	/* SCRATCHI Scratch Register I */
#define SYM895_OFF_SCRATCHJ	(BASE + 0x7C)	/* SCRATCHJ Scratch Register J */

/* 
 * The Device Id's (required for finding the device on PCI Bus)
 * for the SYM895 Chip. Note that NCR895 and SYM895 are both the same.
 * the SCSI division of NCR is now Symbios, which is taken over by LSI logic
 */

#define NCR810_DEVICE_ID    0x0001
#define NCR825_DEVICE_ID    0x0003
#define NCR875_DEVICE_ID    0x000f
#define NCR895_DEVICE_ID    0x000c
#define NCR895A_DEVICE_ID   0x0012
#define SYM895_DEVICE_ID    NCR895_DEVICE_ID
#define PCI_ID_SYMBIOS      0x1000

/* 
 * The Block Move Counter Mask.
 * The DBC register in sym895 holds the number of bytes transferred in 
 * a Block Move instruction. It is also used to hold the least significant 
 * 24 bits of the first dword of a SCRIPTS fetch. For more details, please 
 * refer to chap 5, of the 895 data manual.
 * The maximum value that it can store is 0x00ffffff (24 bits only). 
 */

#define SYM895_COUNT_MASK   ((UINT)0x00ffffff)

/* 
 * As a part of the SCSI Thread, the target parameters are to be set
 * in the thread context. These parameters are  encoded. Device (target)id 
 * and a copy of the sxfer register are to be encoded in a single 32 bit word. 
 * Bits 23-16 specify the target bus id(mapped to SDID register) and bits 15-08 
 * specify the copy of sxfer register. Refer to 8-8 of PCI-SCSI Programming 
 * guide.
 */

#define SYM895_TARGET_BUS_ID_SHIFT  16
#define SYM895_XFER_PARAMS_SHIFT    8

/* 
 * Allowable SCSI Synchronous Offset and Transfer period values.
 * Please refer to chap 2, the Functional description and chap 5, Operating 
 * registers of sym895 data manual, for more details.
 */

#define SYM895_MIN_SYNC_OFFSET      1
#define SYM895_MAX_SYNC_OFFSET      31
#define SYM895_ASYNC_OFFSET         0   /* Sync. Offset should be 0x00 */
                                        /* for asynchronous xfers */

/* Allowable values for SCSI Sync. transfer period in XFERP register */

#define SYM895_MIN_XFERP            4
#define SYM895_MAX_XFERP            11  /* Not recommended.see the manual */
#define SYM895_IDEAL_XFERP          4

#define SYM895_SYNC_XFER_PERIOD_SHIFT   5 /* PERIOD is Bits 7-5 of XFER reg */

/* The Value of sxfer register for asynchronous transfers */

#define SYM895_ASYNC_SXFER          0x00

/* 
 * Synchronous Transfer : Clock division factor 
 * Refer to chap 5, Operating Registers, SCNTL3 register bits SCF2-0
 */

#define SYM895_SYNC_MIN_CLK_DIV	    1
#define SYM895_SYNC_MAX_CLK_DIV     7

#define SYM895_SYNC_CLK_DIV_1       1       /* SCLK / 1 */
#define SYM895_SYNC_CLK_DIV_1_5     2       /* SCLK / 1.5 */
#define SYM895_SYNC_CLK_DIV_2       3       /* SCLK / 2 */ 
#define SYM895_SYNC_CLK_DIV_3       4       /* SCLK / 3 */
#define SYM895_SYNC_CLK_DIV_4       5       /* SCLK / 4 */
#define SYM895_SYNC_CLK_DIV_6       6       /* SCLK / 6 */
#define SYM895_SYNC_CLK_DIV_8       7       /* SCLK / 8 */

/* 
 * Asynchronous Transfer : Clock conversion factor 
 * Refer to chap 5, Operating Registers, SCNTL3 register
 * Bits CCF2-0.
 */

#define SYM895_ASYNC_MIN_CLK_DIV    1       /* 16.67 - 25 MHz */
#define SYM895_ASYNC_MAX_CLK_DIV    7       /* 160 MHz : The clock quadrupler  */
                                            /* bit must be set */

/* prescale factor for asynchronous scsi core (scntl3) */

#define SYM895_16MHZ_ASYNC_DIV      0x01    /* 16.67-25.00Mhz input clock */
#define SYM895_25MHZ_ASYNC_DIV      0x02    /* 25.01-37.50Mhz input clock */
#define SYM895_3750MHZ_ASYNC_DIV    0x03    /* 37.51-50.00Mhz input clock */
#define SYM895_50MHZ_ASYNC_DIV      0x04    /* 50.01-75.00Mhz input clock */

/* 
 * Note: 70MHz & 160MHz value has Ultra Enable bit set (b7).
 * When Ultra enable bit is set, the Tolerant Enable bit must also be set . 
 * It is STEST3:TE (bit 7).
 */

#define SYM895_75MHZ_ASYNC_DIV      0x85    /* 75.01-80.00Mhz input clock */

/* 160.00 Mhz input clock with clock quadrupler and 40 Mhz input clock */

#define SYM895_160MHZ_ASYNC_DIV	    0x87    

/* Nano Seconds x 100 clock period */

#define SYM895_1667MHZ      6000    /* 16.67Mhz chip */
#define SYM895_20MHZ        5000    /* 20Mhz chip */
#define SYM895_25MHZ        4000    /* 25Mhz chip */
#define SYM895_3750MHZ      2667    /* 37.50Mhz chip */
#define SYM895_40MHZ        2500    /* 40Mhz chip */
#define SYM895_50MHZ        2000    /* 50Mhz chip */
#define SYM895_66MHZ        1515    /* 66Mhz chip */
#define SYM895_6666MHZ      1500    /* 66Mhz chip */
#define SYM895_75MHZ        1333    /* 75Mhz chip */
#define SYM895_80MHZ        1250    /* 80Mhz chip */
#define SYM895_160MHZ        625    /* 40Mhz chip with Quadrupler */

/* Timer values for General Purpose Timer (STIME1) */

#define SYM895_GEN_DISABLED 0x00    /* Disabled */
#define SYM895_GEN_128MS    0x0B    /* 128 MS */
#define SYM895_GEN_64MS	    0x0A    /* 64 MS */

/* Various Bus Modes supported by 895 */

#define SYM895_BUSMODE_LVD     3
#define SYM895_BUSMODE_SE      2
#define SYM895_BUSMODE_HVD     1

#define SYM895_BUSMODE_SHIFT   6

#define SYM895_MAX_XFER_WIDTH  1    /* in transfer width exponent units.*/
                                    /* 16bits. */

#define SYM895_TYPE     0x895       /* Chip Type; */

/* sym895 PCI Resources  */

#define SYM895_MEMBASE         0xf5200000	
#define SYM895_MEMSIZE         0x00001000      /* memory size for Int.RAM, 4KB */
#define SYM895_RAM_ADR         (SYM895_MEMBASE)
#define SYM895_MEM_ADR         (SYM895_MEMBASE + SYM895_MEMSIZE)
#define SYM895_IO_ADR          0xf800	
#define SYM895_INT_LVL         0x0a
#define SYM895_INIT_STATE_MASK (VM_STATE_MASK_FOR_ALL)
#define SYM895_INIT_STATE      (VM_STATE_FOR_IO)

#define SYM895_DEV_MMU_MSK      (~(VM_PAGE_SIZE - 1))   /* Mask MMU page */
#define SYM895_DEV_ADRS_SIZE    VM_PAGE_SIZE * 2        /* two pages */

/* Controller Options, while creating the Controller Structure */

#define SYM895_ENABLE_PARITY_CHECK      0x01
#define SYM895_ENABLE_SINGLE_STEP       0x02
#define SYM895_COPY_SCRIPTS             0x04

/* SCRIPT ram Size for various controllers */

#define SYM895_SCRIPT_RAM_SIZE 4*1024	/* bytes */

/* 
 * Chip register access mode (memory mapped / IO). If SYM895_IO_MAPPED is 
 * defined, then IO Base Address (PCI BAR 0) is used for all register access. 
 * If not defined, then Memory Base Address(PCI BAR 1) is used.
 */

#define SYM895_IO_MAPPED   /* IO mapped access by default */

/* Loopback diagnostics */

#define LOOPBACK_MSG_BYTE  0xa5
#define LOOPBACK_DATA      0x5a5a

/*
 *  Structure , used as an interface between the C Code and the Scripts.
 *  used for storing data in Block Move instructions.
 */

typedef struct moveParams
    {

    ULONG  size;              /* # bytes to move to/from SCSI */

    UINT8 *addr;              /* where in memory they go/come */

    } MOVE_PARAMS;


/*
 * This has to be defined to include support for SCATTER-GATHER memory
 * operations. 
 */

/* #define SCATTER_GATHER  */

/* Maximum number of scattered data elements. */

#define MAX_NO_OF_SCAT_ELEMENTS 255

/*
 * Structure to hold the scattered data, to be gathered by the SIOP while
 * in scripts. Gathered Data is transferred using chained block moves, one 
 * after the other (block wise) with out the intervention of the host.
 */

typedef struct scatGather
    {

    ULONG         noElems;   /* No of places to gather the scattered the data */

    MOVE_PARAMS   Elements[MAX_NO_OF_SCAT_ELEMENTS]; 
 
    } SCAT_DATA;

/*
 *  Shared structure defining all information about a SCSI connection
 *  visible to the script.  
 *  For more details, please refer to PCI-SCSI programmers guide (8)
 *  Every element in this structure _must_ be aligned on
 *  a 4-byte boundary, otherwise the script will fail (illegal instruction).
 */

typedef struct sym895Shared
    {

    ULONG       device;	      /* target bus ID, sync xfer params */

    MOVE_PARAMS command;      /* command bytes sent to target    */

    MOVE_PARAMS dataIn;       /* data bytes from target          */
   
    MOVE_PARAMS dataOut;      /* data bytes to target            */	

    MOVE_PARAMS status;       /* command status byte from target */

    MOVE_PARAMS identOut;     /* outgoing identification msg     */

    MOVE_PARAMS identIn;      /* incoming identification msg     */

    MOVE_PARAMS msgOut;	      /* message bytes sent to target    */

    MOVE_PARAMS msgIn;        /* message bytes recvd from target */

    MOVE_PARAMS msgInSecond;  /* second byte of multi-byte msg   */

    MOVE_PARAMS msgInRest;    /* remainder of multi-byte message */

#ifdef SCATTER_GATHER

    SCAT_DATA	scatData;     /* scatter - gather memory moves */

#endif

    } SYM895_SHARED;

/* The Typedefs and the structure definitions */

/* 
 * Shared structure exclusively used by the diagnostic scripts for 
 * loopback mode. Every element of this structure must be 4-byte aligned.
 */

typedef struct sym895Loopback
{

    ULONG       device    ;  /* target bus ID, sync xfer params */

    MOVE_PARAMS identOut  ;  /* Identify buffer for loopback mode */
 
    MOVE_PARAMS dataOut   ;  /* data buffer for loopback mode */

} SYM895_LOOPBACK;


/* Sym895 Thread strcuture. */

typedef struct sym895Thread 
    {

    SCSI_THREAD             scsiThread  ;  /* generic SCSI Thread */

    struct sym895Shared *    pShMem     ; /* Pointer to SIOP shared */
                                          /* data area */
													 
    struct sym895Shared *    pShMemDuplicate ; /* Pointer to SIOP */
                                               /* shared data area */

    /* 
     * Every thread has its own context, which is the image of some of the 
     * SIOP registers. Whenever the execution comes out of thread, the
     * current values of those registers are saved in these variables, and 
     * retreived, when the execution is back to the thread.
     */

    UINT8   nHostFlags;     /* Scratch A register byte 0 */
    UINT8   msgOutStatus;   /* Scratch A register byte 1 */
    UINT8   msgInStatus;    /* Scratch A register byte 2 */
    UINT8   targetId;       /* Scratch A register byte 3 */
    UINT8   busPhase;       /* Scratch B register byte 0 */

    UINT8   sxfer;          /* SXFER register */
    UINT8   scntl3;         /* SCNTL3 registers */

    /* scatter gather memory support */

    UINT8   isScatTransfer;    /* Scratch C register byte 0 */
    UINT8   totalScatElements; /* Scratch C register byte 1 */
    UINT8   noRemaining;       /* Scratch C register byte 2 */

    }SYM895_THREAD;

/* Sym895 Event Structure. */

typedef struct sym895Event 
    {

    SCSI_EVENT          scsiEvent  ;      /* generic SCSI Event */
    UINT                remCount   ;      /* Remaining Byte Count */
                                          /* (Phase Mismatch) */
    }SYM895_EVENT;

/* 
 * Script Entry Point Identifiers. These are the 4 Scripts that VxWorks
 * requires. If some more are to be added, they have to be added here and in 
 * sym895StartScript() to ensure that the script starts when required.
 */

typedef enum sym895ScriptEntries
    {

    SYM895_SCRIPT_WAIT          = 0,    /* Wait for ReSelect or Host Command */
    SYM895_SCRIPT_INIT_START,           /* Start an Initiator Thread */
    SYM895_SCRIPT_INIT_CONTINUE,        /* Continue an Initiator Thread */
    SYM895_SCRIPT_TGT_DISCONNECT,       /* Disconnect a Target Thread */
    SYM895_SCRIPT_DIAG

    }SYM895_SCRIPT_ENTRY;

/* SIOP operating States */

typedef enum sym895State
    {

    SYM895_STATE_IDLE           = 0,    /* Not running any script */
    SYM895_STATE_PASSIVE,               /* Wait for ReSelect or Host Command */
    SYM895_STATE_ACTIVE                 /* Running a script */

    }SYM895_STATE;

/* 
 * SIOP hardware configuration structure. This is used to set/reset various 
 * options available in SYM895. This is used as a parameter for the
 * sym895SetHWOptions() functions. This is a  bit-field structure and has 
 * to be operated with caution. 
 *
 * Note : AWS bit affects all the SCSI phases while EWS bit affects only the 
 * data transfers. EWS, unlike AWS, doesn't affect the command, status and 
 * message phases.
 */

typedef struct sym895HWOptions
    {

    UINT    SCLK    : 1;     /* STEST1:b7,if false,uses PCI Clock for SCSI*/
    UINT    QEN     : 1;     /* STEST1:b3, Clock Quadrapler Enable */
    UINT    SCE     : 1;     /* STEST2:b7,enable assertion of SCSI thro SOCL */
                             /* and SODL registers */
    UINT    DIF     : 1;     /* STEST2:b5, enable differential SCSI */
    UINT    AWS     : 1;     /* STEST2:b2, Always Wide SCSI */
    UINT    EWS     : 1;     /* SCNTL3:b3, Enable Wide SCSI */
    UINT    EXTEND  : 1;     /* STEST2:b1, Extend SREQ/SACK filtering */
    UINT    TE      : 1;     /* STEST3:b7, TolerANT Enable */
    UINT    BL      : 3;     /* DMODE:b7,b6, CTEST5:b2 : Burst length */
                             /* when set to any of 32/64/128 burst length */
                             /* transfers, requires the DMA Fifo size to be */
                             /* 816 bytes (ctest5:b5 = 1). */
    UINT    SIOM    : 1;     /* DMODE:b5, Source I/O Memory Enable */
    UINT    DIOM    : 1;     /* DMODE:b4, Destination I/O Memory Enable */
    UINT    EXC     : 1;     /* SCNTL1:b7, Slow Cable Mode */
    UINT    ULTRA   : 1;     /* SCNTL3:b7, Ultra Enable */
    UINT    DFS     : 1;     /* CTEST5:b5, DMA Fifo size 112/816 bytes */

    }SYM895_HW_OPTIONS;

/* 
 * The Default HW Options as the driver initialies the Sym 895 Chip 
 * These options SHOULD correspond to the above mentioned structure.
 * 
 * Use SCSI Clock and not PCI clock
 * Enable Clock Quadrapler
 * SCSI signal assertion through regsiters - disable 
 * Disable High Voltage differential SCSI  
 * Not always WIDE SCSI
 * Enable Wide SCSI 
 * No filtering via SREQ/ SACK
 * TolerANT Enable
 * 128 burst transfer
 * Disable IO mode for both source and destination
 * No slow cable mode
 * Ultra SCSI  
 * DMA Fifo size - 816 bytes 
 *
 * NOTE:  These options can be overriden by sym895SetHwOptions() call in 
 *        "sysScsi.c".
 */

#define  SYM895_ULTRA2_SCSI_OPTIONS         { 0,1,0,0,0,1,0,1,6,0,0,0,1,1 }

#define  SYM895_FAST_SCSI_OPTIONS           { 0,0,0,0,0,1,0,1,6,0,0,0,0,1 }

/* 
 * SCSI Controller Structure
 *
 * This the structure describing the SCSI controller chip and the SCSI 
 * parameters used on the SCSI Bus. This structure also contains the 
 * shared memory areas and the Thread information like what threads is
 * being executed by the SCRIPT processor currently and what script is 
 * scheduled to run next etc.
 *
 * As this structure contains shared memory areas which are accessed 
 * by both the SCSI controller and the CPU, this entire structure and
 * its memory buffers must be located in the cache-coherent memory, safe for
 * the dma transfers.
 */

typedef struct sym895ScsiCtrl
    {

    SCSI_CTRL       scsiCtrl;           /* generic SCSI Controller info. */
                                        /* used by both the driver and SCSI */ 
                                        /* Manager.*/

    SEM_ID          singleStepSem;      /* used to debug script */

    SYM895_STATE    state;              /* Current state of the controller */

    UINT            clkPeriod;          /* Clock Period; nSec*100 */

    UINT            clkDiv;             /* SYNC/ASYNC clock divisor */

    UINT            devType;            /* Currently 895 */

    BOOL            isParityCheck;      /* Enable/Disable Paritychecks on Bus*/
 
    BOOL            isScriptsCopy;      /* Copy SCRIPTS to on-chip RAM */      

    BOOL            isCmdPending;       /* Is there a new command to start */

    BOOL            isSingleStep;       /* is Single Step Debugging enabled */

    /* 
     * The following identMsg buffer is used for initialising the pointer
     * for the current thread's IdentOut Message buffer. This buffer is also
     * defined in the SCSI_THREAD structure and was previously used for the
     * IdentOut Message buffer. The additional 5 bytes are for wide/sync SCSI
     */

    UINT8           identMsg[SCSI_MAX_IDENT_MSG_LENGTH + 5]; 

    UINT            identMsgLength;

    SYM895_THREAD * pCurThread;        /* Thread corresponding to current */
                                       /* running script */
										 
    SYM895_THREAD * pNewThread;        /* Thread to be activated next */

    SYM895_SHARED * pIdentShMem;       /* Shared Data Area for Identification */
                                       /* Thread */

    SYM895_SHARED * pClientShMem;      /* Shared Data Area for Client thread*/


    volatile UINT8 * baseAddress;      /* Address where the Chip is mapped */
                                       /* in memory */

    volatile UINT8 * siopRamBaseAddress; /* Base Address for internal Ram */

    SYM895_HW_OPTIONS  hwOptions;      /* 895 Options used/not being used */

    }SYM895_SCSI_CTRL;

/* Function Declarations */

IMPORT SYM895_SCSI_CTRL * sym895CtrlCreate (
                                            UINT8 *  siopBaseAdrs,
                                            UINT     clkPeriod,
                                            UINT16   devType,
                                            UINT8 *  siopRamBaseAdrs,
                                            UINT16   flags
                                           );

IMPORT STATUS sym895CtrlInit (
                              SYM895_SCSI_CTRL * pSiop,
                              UINT scsiCtrlBusId
                             );

IMPORT STATUS sym895SetHwOptions (
                                  SYM895_SCSI_CTRL  * pSiop,
                                  SYM895_HW_OPTIONS * pHwRegs
                                 );

IMPORT void sym895Intr (SYM895_SCSI_CTRL * pSiop);

IMPORT STATUS sym895Show (SYM895_SCSI_CTRL * pSiop);

IMPORT void sym895StepEnable (SYM895_SCSI_CTRL * pSiop, BOOL enable);

IMPORT void sym895SingleStep (SYM895_SCSI_CTRL * pSiop, BOOL verbose);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus

}

#endif /* __cplusplus */

#endif /* __INCsym895h */
