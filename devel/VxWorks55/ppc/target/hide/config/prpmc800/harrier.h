/* harrier.h - Harrier chip header file */

/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,10oct01,scb  Add #define's for shared memory interrupt support.
01f,09aug01,scb  Add #defines for PPC arbiter register (XARB).
01g,17may01,blk  Add support for harrier II.
01f,14nov00,dmw  Added defines for Harrier offsets used in romInit.
01e,17oct00,krp  display of SMC SDRAM addressing & control settings.
01d,16oct00,dmw  Added PCI bridge defines.
01c,08oct00,dmw  corrected I2C device offsets.
01b,01sep00,dmw  added inbound translation defines.
01a,11jul00,krp  created for the prpmc800 BSP.
*/

/*
This file contains Base address defines, register offsets and bit
definitions for the the Harrier chip
*/


#ifndef	INCharrierh
#define	INCharrierh

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef HARRIER_XCSR_BASE
#   define HARRIER_XCSR_BASE	0xFEFF0000
#endif

#ifndef HARRIER_XCSR_SIZE
#   define HARRIER_XCSR_SIZE	0x00001000
#endif

/*
 * SDRAM block descriptor registers. these are for setting the base
 * address, size, and enable status for a block of SDRAM. there are
 * eight sets of these registers for up to eight blocks of SDRAM.
 */

#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_A (HARRIER_XCSR_BASE + 0x110)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_B (HARRIER_XCSR_BASE + 0x114)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_C (HARRIER_XCSR_BASE + 0x118)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_D (HARRIER_XCSR_BASE + 0x11C)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_E (HARRIER_XCSR_BASE + 0x120)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_F (HARRIER_XCSR_BASE + 0x124)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_G (HARRIER_XCSR_BASE + 0x128)
#define HARRIER_REG_SDRAM_BLOCK_ADDRESSING_H (HARRIER_XCSR_BASE + 0x12C)

#define HARRIER_SDRAM_BLOCK_BASEADDR_OFFSET  0
#define HARRIER_SDRAM_BLOCK_SIZE_OFFSET      1
#define HARRIER_SDRAM_BLOCK_ENABLE_OFFSET    2

/*
 * registers for X-port channels. these registers are used for setting the
 * starting address, ending address, and attributes for an X-port channel.
 * the attributes register also has read-only bit fields of information on
 * the X-port channel.
 *
 * There are FOUR available X-port channels.
 */

#define HARRIER_XPORT0_ADDR_RANGE_REG	(HARRIER_XCSR_BASE + 0x150)
#define HARRIER_XPORT1_ADDR_RANGE_REG	(HARRIER_XCSR_BASE + 0x158)
#define HARRIER_XPORT2_ADDR_RANGE_REG	(HARRIER_XCSR_BASE + 0x160)
#define HARRIER_XPORT3_ADDR_RANGE_REG	(HARRIER_XCSR_BASE + 0x168)

#define HARRIER_XPORT0_ATTR_REG 	(HARRIER_XCSR_BASE + 0x154)
#define HARRIER_XPORT1_ATTR_REG 	(HARRIER_XCSR_BASE + 0x15C)
#define HARRIER_XPORT2_ATTR_REG 	(HARRIER_XCSR_BASE + 0x164)
#define HARRIER_XPORT3_ATTR_REG 	(HARRIER_XCSR_BASE + 0x16C)

#define HARRIER_XPORT_STARTADDR_OFFSET	0
#define HARRIER_XPORT_ENDADDR_OFFSET	2
#define HARRIER_XPORT_ATTR_OFFSET	4

#define HARRIER_XPORT_GENERAL_CONTROL_REG   (HARRIER_XCSR_BASE + 0x170)

/*
 * registers for I2C serial bus interface. the Harrier has two seperate
 * I2C interfaces for up to two I2C serial buses.
 */

#define HARRIER_I2C_CONTROLLER0_REG  (HARRIER_XCSR_BASE + 0x180)
#define HARRIER_I2C_CONTROLLER1_REG  (HARRIER_XCSR_BASE + 0x1A0)

#define HARRIER_I2C_PRESCALER_OFFSET	      0x00
#define HARRIER_I2C_CONTROL_OFFSET	      0x04
#define HARRIER_I2C_TRANSMITTER_DATA_OFFSET   0x0C
#define HARRIER_I2C_STATUS_OFFSET	      0x14
#define HARRIER_I2C_RECEIVER_DATA_OFFSET      0x1C

/*
 * registers descriptors for mapping PCI memory or I/O space to
 * PowerPC memory space. there are four sets of these registers.
 */


/*
 * offset +00 - outbound translation range starting address register
 * offset +02 - outbound translation range ending address register
 * offset +04 - outbound translation address offset register
 * offset +06 - outbound translation attributes register
 */
#define HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG  (HARRIER_XCSR_BASE + 0x220)
#define HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG  (HARRIER_XCSR_BASE + 0x228)
#define HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG  (HARRIER_XCSR_BASE + 0x230)
#define HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG  (HARRIER_XCSR_BASE + 0x238)

#define HARRIER_OUTBOUND_TRANSLATION_STARTADDRESS_OFFSET  0
#define HARRIER_OUTBOUND_TRANSLATION_ENDADDRESS_OFFSET	  2
#define HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET	  4
#define HARRIER_OUTBOUND_TRANSLATION_ATTRIBUTE_OFFSET	  6

#define HARRIER_VENDOR_ID_REG	(HARRIER_XCSR_BASE + 0x000) /* Motorola: 0x1057 */
#define HARRIER_DEVICE_ID_REG	(HARRIER_XCSR_BASE + 0x002) /* Harrier:  0x480B */
#define HARRIER_VERSION_ID_REG	(HARRIER_XCSR_BASE + 0x005)

/*
 * XCSR+$010 - global control and status register
 * XCSR+$014 - PowerPC clock frequency register
 * XCSR+$018 - count 32-bit register
 * XCSR+$01C - miscellaneous control and status register
 */

#define H_PPCCLKFRQ_OFST   (0x014)

#define HARRIER_GLOBAL_CONTROL_STATUS_REG (HARRIER_XCSR_BASE + 0x010)
#define HARRIER_PPC_CLOCK_FREQUENCY_REG   (HARRIER_XCSR_BASE + H_PPCCLKFRQ_OFST)
#define HARRIER_COUNTER_32BIT_REG	  (HARRIER_XCSR_BASE + 0x018)
#define HARRIER_MISC_CONTROL_STATUS_REG   (HARRIER_XCSR_BASE + 0x01C)

#define HARRIER_GP0_OFFSET   (0x020)
#define HARRIER_GP1_OFFSET   (0x024)
#define HARRIER_GP2_OFFSET   (0x028)
#define HARRIER_GP3_OFFSET   (0x02C)
#define HARRIER_GP4_OFFSET   (0x030)
#define HARRIER_GP5_OFFSET   (0x034)

#define HARRIER_GENERAL_PURPOSE0_REG   (HARRIER_XCSR_BASE + HARRIER_GP0_OFFSET)
#define HARRIER_GENERAL_PURPOSE1_REG   (HARRIER_XCSR_BASE + HARRIER_GP1_OFFSET)
#define HARRIER_GENERAL_PURPOSE2_REG   (HARRIER_XCSR_BASE + HARRIER_GP2_OFFSET)
#define HARRIER_GENERAL_PURPOSE3_REG   (HARRIER_XCSR_BASE + HARRIER_GP3_OFFSET)
#define HARRIER_GENERAL_PURPOSE4_REG   (HARRIER_XCSR_BASE + HARRIER_GP4_OFFSET)
#define HARRIER_GENERAL_PURPOSE5_REG   (HARRIER_XCSR_BASE + HARRIER_GP5_OFFSET)

/*
 * XCSR+$040 - functional exception enable register
 * XCSR+$044 - functional exception status register
 * XCSR+$048 - functional exception mask register
 * XCSR+$04C - functional exception clear register
 */

#define HARRIER_EXCEPTION_ENABLE_REG  (HARRIER_XCSR_BASE + 0x040)
#define HARRIER_EXCEPTION_STATUS_REG  (HARRIER_XCSR_BASE + 0x044)
#define HARRIER_EXCEPTION_MASK_REG    (HARRIER_XCSR_BASE + 0x048)
#define HARRIER_EXCEPTION_CLEAR_REG   (HARRIER_XCSR_BASE + 0x04C)

#define HARRIER_ERROR_EXCEPTION_ENABLE_REG	 (HARRIER_XCSR_BASE + 0x050)
#define HARRIER_ERROR_EXCEPTION_STATUS_REG	 (HARRIER_XCSR_BASE + 0x054)
#define HARRIER_ERROR_EXCEPTION_CLEAR_REG	 (HARRIER_XCSR_BASE + 0x058)
#define HARRIER_ERROR_EXCEPTION_INT_ENABLE_REG	 (HARRIER_XCSR_BASE + 0x05C)

#define HARRIER_ERROR_EXCMACHINECHECK0_ENABLE (HARRIER_XCSR_BASE + 0x060)
#define HARRIER_ERROR_EXCMACHINECHECK1_ENABLE (HARRIER_XCSR_BASE + 0x064)

/*
 * XCSR+$06C - error diagnostics parity error injection register
 * XCSR+$070 - error diagnostics PowerPC address register
 * XCSR+$074 - error diagnostics PowerPC attribute register
 * XCSR+$078 - error diagnostics PCI address register
 * XCSR+$07C - error diagnostics PCI attribute register
 */

#define HARRIER_ERR_DIAGS_PARITYERRORINJECT_REG (HARRIER_XCSR_BASE + 0x06C)
#define HARRIER_ERR_DIAGS_PPCADDRESS_REG	(HARRIER_XCSR_BASE + 0x070)
#define HARRIER_ERR_DIAGS_PPCATTRIBUTE_REG	(HARRIER_XCSR_BASE + 0x074)
#define HARRIER_ERR_DIAGS_PCIADDRESS_REG	(HARRIER_XCSR_BASE + 0x078)
#define HARRIER_ERR_DIAGS_PCIATTRIBUTE_REG	(HARRIER_XCSR_BASE + 0x07C)

/*
 * XCSR+$080 - watchdog timer #0 control register
 * XCSR+$084 - watchdog timer #0 status register
 * XCSR+$088 - watchdog timer #1 control register
 * XCSR+$08C - watchdog timer #1 status register
 */

#define HARRIER_WATCHDOGTIMER_CONTROL0_REG    (HARRIER_XCSR_BASE + 0x080)
#define HARRIER_WATCHDOGTIMER_STATUS0_REG     (HARRIER_XCSR_BASE + 0x084)
#define HARRIER_WATCHDOGTIMER_CONTROL1_REG    (HARRIER_XCSR_BASE + 0x088)
#define HARRIER_WATCHDOGTIMER_STATUS1_REG     (HARRIER_XCSR_BASE + 0x08C)

#define HARRIER_WTCHDG_CNTR0_OFFSET           (0x080)
#define HARRIER_WTCHDG_CNTR1_OFFSET           (0x088)

/*
 * XCSR+$090 - PCI arbiter register
 * XCSR+$094 - PowerPC arbiter register
 */

#define HARRIER_PCI_ARBITER_REG  (HARRIER_XCSR_BASE + 0x090)
#define HARRIER_PPC_ARBITER_REG  (HARRIER_XCSR_BASE + 0x094)

/*
 * XCSR+$0C0 - UART #0 receiver buffer / transmitter holding /
 *		       divisor latch low register
 * XCSR+$0C1 - UART #0 interrupt enable / divisor latch high register
 * XCSR+$0C2 - UART #0 interrupt identification / FIFO control register
 * XCSR+$0C3 - UART #0 line control register
 * XCSR+$0C4 - UART #0 modem control register
 * XCSR+$0C5 - UART #0 line status register
 * XCSR+$0C6 - UART #0 modem status register
 * XCSR+$0C7 - UART #0 scratch register
 */

#define  HARRIER_UART_0_RTDL_REG  (HARRIER_XCSR_BASE + 0x0C0)
#define  HARRIER_UART_0_IEDH_REG  (HARRIER_XCSR_BASE + 0x0C1)
#define  HARRIER_UART_0_IDFC_REG  (HARRIER_XCSR_BASE + 0x0C2)
#define  HARRIER_UART_0_LCTL_REG  (HARRIER_XCSR_BASE + 0x0C3)
#define  HARRIER_UART_0_MCTL_REG  (HARRIER_XCSR_BASE + 0x0C4)
#define  HARRIER_UART_0_LSTA_REG  (HARRIER_XCSR_BASE + 0x0C5)
#define  HARRIER_UART_0_MSTA_REG  (HARRIER_XCSR_BASE + 0x0C6)
#define  HARRIER_UART_0_SCRT_REG  (HARRIER_XCSR_BASE + 0x0C7)

/*
 * XCSR+$0C8 - UART #1 receiver buffer / transmitter holding /
 *		       divisor latch low register
 * XCSR+$0C9 - UART #1 interrupt enable / divisor latch high register
 * XCSR+$0CA - UART #1 interrupt identification / FIFO control register
 * XCSR+$0CB - UART #1 line control register
 * XCSR+$0CC - UART #1 modem control register
 * XCSR+$0CD - UART #1 line status register
 * XCSR+$0CE - UART #1 modem status register
 * XCSR+$0CF - UART #1 scratch register
 */

#define  HARRIER_UART_1_RTDL_REG  (HARRIER_XCSR_BASE + 0x0C8)
#define  HARRIER_UART_1_IEDH_REG  (HARRIER_XCSR_BASE + 0x0C9)
#define  HARRIER_UART_1_IDFC_REG  (HARRIER_XCSR_BASE + 0x0CA)
#define  HARRIER_UART_1_LCTL_REG  (HARRIER_XCSR_BASE + 0x0CB)
#define  HARRIER_UART_1_MCTL_REG  (HARRIER_XCSR_BASE + 0x0CC)
#define  HARRIER_UART_1_LSTA_REG  (HARRIER_XCSR_BASE + 0x0CD)
#define  HARRIER_UART_1_MSTA_REG  (HARRIER_XCSR_BASE + 0x0CE)
#define  HARRIER_UART_1_SCRT_REG  (HARRIER_XCSR_BASE + 0x0CF)


/*
 * XCSR+$0D0 - UART control register
 * XCSR+$0D3 - UART clock prescaler register
 */

#define HARRIER_UART_CONTROL_REG	  (HARRIER_XCSR_BASE + 0x0D0)
#define HARRIER_UART_CLOCK_PRESCALER_REG  (HARRIER_XCSR_BASE + 0x0D3)

/*
 * XCSR+$0E0 - MPIC base address register
 * XCSR+$0E4 - MPIC control and status register
 * XCSR+$0E6 - MPIC interrupt request sample register
 */

#define HARRIER_MPIC_BASEADDRESS_REG	   (HARRIER_XCSR_BASE + 0x0E0)
#define HARRIER_MPIC_CONTROL_STATUS_REG    (HARRIER_XCSR_BASE + 0x0E4)
#define HARRIER_MPIC_INT_REQUESTSAMPLE_REG (HARRIER_XCSR_BASE + 0x0E6)

/*
 * XCSR+$100 - SDRAM general control register
 * XCSR+$104 - SDRAM timing control register
 */

#define HARRIER_SDRAM_GENERAL_CONTROL_REG  (HARRIER_XCSR_BASE + 0x100)
#define HARRIER_SDRAM_TIMING_CONTROL_REG   (HARRIER_XCSR_BASE + 0x104)


/*
 * XCSR+$130 - SDRAM scrub control register
 * XCSR+$134 - SDRAM scrub address counter
 */

#define HARRIER_SDRAM_SCRUB_CONTROL_REG        (HARRIER_XCSR_BASE + 0x130)
#define HARRIER_SDRAM_SCRUB_ADDRESSCOUNTER_REG (HARRIER_XCSR_BASE + 0x134)


/*
 * XCSR+$140 - SDRAM single-bit error syndrome register
 * XCSR+$141 - SDRAM single-bit error scrub block register
 * XCSR+$142 - SDRAM single-bit error count register
 * XCSR+$144 - SDRAM single-bit error address register
 */

#define HARRIER_SDRAM_SBE_SYNDROME_REG	  (HARRIER_XCSR_BASE + 0x140)
#define HARRIER_SDRAM_SBE_SCRUBBLOCK_REG  (HARRIER_XCSR_BASE + 0x141)
#define HARRIER_SDRAM_SBE_COUNT_REG	  (HARRIER_XCSR_BASE + 0x142)
#define HARRIER_SDRAM_SBE_ADDRESS_REG	  (HARRIER_XCSR_BASE + 0x144)

/*
 * XCSR+$149 - SDRAM multi-bit error scrub block register
 * XCSR+$14C - SDRAM multi-bit error address register
 */

#define HARRIER_SDRAM_MBE_SCRUBBLOCK_REG  (HARRIER_XCSR_BASE + 0x149)
#define HARRIER_SDRAM_MBE_ADDRESS_REG	  (HARRIER_XCSR_BASE + 0x14C)



/*
 * XCSR+$200 - bridge PCI control and status register
 * XCSR+$204 - bridge PowerPC control and status register
 */

#define HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG (HARRIER_XCSR_BASE + 0x200)
#define HARRIER_BRIDGE_PPC_CONTROLSTATUS_REG (HARRIER_XCSR_BASE + 0x204)

#define HARRIER_PCI_CS_REG_OFFSET 0x200 /* offset to control status reg */


/* XCSR+$210 - PCI interrupt acknowledge register */

#define HARRIER_PCI_INTERRUPT_ACK_REG (HARRIER_XCSR_BASE + 0x210)

/*
 * XCSR+$248 - passive slave address register
 * XCSR+$24C - passive slave offset register
 * XCSR+$24E - passive slave size register
 * XCSR+$24F - passive slave attribute register
 */

#define HARRIER_PASSIVE_SLAVEADDRESS_REG    (HARRIER_XCSR_BASE + 0x248)
#define HARRIER_PASSIVE_SLAVEOFFSET_REG     (HARRIER_XCSR_BASE + 0x24C)
#define HARRIER_PASSIVE_SLAVESIZE_REG	    (HARRIER_XCSR_BASE + 0x24E)
#define HARRIER_PASSIVE_SLAVEATTRIBUTE_REG  (HARRIER_XCSR_BASE + 0x24F)

/*
 * XCSR+$250 - DMA control register
 * XCSR+$254 - DMA status register
 * XCSR+$260 - DMA source address register
 * XCSR+$264 - DMA source attribute register
 * XCSR+$268 - DMA destination address register
 * XCSR+$26C - DMA destination attribute register
 * XCSR+$270 - DMA next link address register
 * XCSR+$274 - DMA count register
 * XCSR+$280 - DMA current source address register
 * XCSR+$284 - DMA current destination address register
 * XCSR+$288 - DMA current link address register
 */

#define HARRIER_DMA_CONTROL_REG 	       (HARRIER_XCSR_BASE + 0x250)
#define HARRIER_DMA_STATUS_REG		       (HARRIER_XCSR_BASE + 0x254)
#define HARRIER_DMA_SOURCEADDR_REG	       (HARRIER_XCSR_BASE + 0x260)
#define HARRIER_DMA_SOURCEATTR_REG	       (HARRIER_XCSR_BASE + 0x264)
#define HARRIER_DMA_DESTINATIONADDR_REG        (HARRIER_XCSR_BASE + 0x268)
#define HARRIER_DMA_DESTINATIONATTR_REG        (HARRIER_XCSR_BASE + 0x26C)
#define HARRIER_DMA_NEXTLINKADDR_REG	       (HARRIER_XCSR_BASE + 0x270)
#define HARRIER_DMA_COUNT_REG		       (HARRIER_XCSR_BASE + 0x274)
#define HARRIER_DMA_CURRENTSOURCEADDRREG       (HARRIER_XCSR_BASE + 0x280)
#define HARRIER_DMA_CURRENTDESTINATIONADDR_REG (HARRIER_XCSR_BASE + 0x284)
#define HARRIER_DMA_CURRENTLINKADDR_REG        (HARRIER_XCSR_BASE + 0x288)


/*
 * XCSR+$290 - MP generic outbound message 0 register
 * XCSR+$294 - MP generic outbound message 1 register
 * XCSR+$298 - MP generic outbound doorbell register
 * XCSR+$2A0 - MP generic inbound message 0 register
 * XCSR+$2A4 - MP generic inbound message 1 register
 * XCSR+$2A8 - MP generic inbound doorbell register
 * XCSR+$2B0 - MP generic inbound doorbell mask register
 */

#define HARRIER_MP_GENERIC_OUTBOUNDMESSAGE0_REG     (HARRIER_XCSR_BASE + 0x290)
#define HARRIER_MP_GENERIC_OUTBOUNDMESSAGE1_REG     (HARRIER_XCSR_BASE + 0x294)
#define HARRIER_MP_GENERIC_OUTBOUNDDOORBELL_REG     (HARRIER_XCSR_BASE + 0x298)
#define HARRIER_MP_GENERIC_INBOUNDMESSAGE0_REG	    (HARRIER_XCSR_BASE + 0x2A0)
#define HARRIER_MP_GENERIC_INBOUNDMESSAGE1_REG	    (HARRIER_XCSR_BASE + 0x2A4)
#define HARRIER_MP_GENERIC_INBOUNDDOORBELL_REG	    (HARRIER_XCSR_BASE + 0x2A8)
#define HARRIER_MP_GENERIC_INBOUNDDOORBELLMASK_REG  (HARRIER_XCSR_BASE + 0x2B0)

/*
 * XCSR+$2C0 - MP I2O outbound free_list head register
 * XCSR+$2C4 - MP I2O outbound free_list tail register
 * XCSR+$2C8 - MP I2O outbound post_list head register
 * XCSR+$2CC - MP I2O outbound post_list tail register
 * XCSR+$2D0 - MP I2O inbound free_list head register
 * XCSR+$2D4 - MP I2O inbound free_list tail register
 * XCSR+$2D8 - MP I2O inbound post_list head register
 * XCSR+$2DC - MP I2O inbound post_list tail register
 * XCSR+$2E0 - MP I2O control register
 * XCSR+$2E4 - MP I2O queue base register
 */

#define HARRIER_MP_I2O_OUTBOUNDFREELISTHEAD_REG (HARRIER_XCSR_BASE + 0x2C0)
#define HARRIER_MP_I2O_OUTBOUNDFREELISTTAIL_REG (HARRIER_XCSR_BASE + 0x2C4)
#define HARRIER_MP_I2O_OUTBOUNDPOSTLISTHEAD_REG (HARRIER_XCSR_BASE + 0x2C8)
#define HARRIER_MP_I2O_OUTBOUNDPOSTLISTTAIL_REG (HARRIER_XCSR_BASE + 0x2CC)
#define HARRIER_MP_I2O_INBOUNDFREELISTHEAD_REG	(HARRIER_XCSR_BASE + 0x2D0)
#define HARRIER_MP_I2O_INBOUNDFREELISTTAIL_REG	(HARRIER_XCSR_BASE + 0x2D4)
#define HARRIER_MP_I2O_INBOUNDPOSTLISTHEAD_REG	(HARRIER_XCSR_BASE + 0x2D8)
#define HARRIER_MP_I2O_INBOUNDPOSTLISTTAIL_REG	(HARRIER_XCSR_BASE + 0x2DC)
#define HARRIER_MP_I2O_CONTROL_REG		(HARRIER_XCSR_BASE + 0x2E0)
#define HARRIER_MP_I2O_QUEUEBASE_REG		(HARRIER_XCSR_BASE + 0x2E4)


/* HARRIER PHB (PCI Host Bridge)*/

/*
 * XCSR+$300 - vendor ID register
 * XCSR+$302 - device ID register
 * XCSR+$304 - command register
 * XCSR+$306 - status register
 * XCSR+$308 - revision ID / class code registers
 */

#define HARRIER_PHB_VENDORID_REG	 (HARRIER_XCSR_BASE + 0x300)
#define HARRIER_PHB_DEVICEID_REG	 (HARRIER_XCSR_BASE + 0x302)
#define HARRIER_PHB_COMMAND_REG 	 (HARRIER_XCSR_BASE + 0x304)
#define HARRIER_PHB_STATUS_REG		 (HARRIER_XCSR_BASE + 0x306)
#define HARRIER_PHB_REVISIONANDCLASS_REG (HARRIER_XCSR_BASE + 0x308)

/*
 * XCSR+$30C - cache line size register
 * XCSR+$30D - master latency timer register
 * XCSR+$30E - header type register (always reads as 0x00)
 */

#define HARRIER_CACHE_LINESIZE_REG	  (HARRIER_XCSR_BASE + 0x30C)
#define HARRIER_MASTER_LATENCY_TIMER_REG  (HARRIER_XCSR_BASE + 0x30D)
#define HARRIER_HEADERTYPE_REG		  (HARRIER_XCSR_BASE + 0x30E)

#define HARRIER_CACHE_LINESIZE            0x08000000
#define HARRIER_READLATENCY               0x00400000

/* XCSR+$310 - message passing register group base address register */

#define HARRIER_MSG_PASSING_REGGROUPBASEADDR_REG (HARRIER_XCSR_BASE + 0x310)
#define HARRIER_XCSR_MPBAR HARRIER_MSG_PASSING_REGGROUPBASEADDR_REG

/* XCSR+$314 - inbound translation base address registers (4) */

#define HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0 (HARRIER_XCSR_BASE + 0x314)
#define HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_1 (HARRIER_XCSR_BASE + 0x318)
#define HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_2 (HARRIER_XCSR_BASE + 0x31C)
#define HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_3 (HARRIER_XCSR_BASE + 0x320)

/*
 * XCSR+$32C - subsystem ID register
 * XCSR+$32E - subsystem vendor ID register
 */

#define HARRIER_SUBSYSTEM_ID_REG       (HARRIER_XCSR_BASE + 0x32C)
#define HARRIER_SUBSYSTEM_VENDORID_REG (HARRIER_XCSR_BASE + 0x32E)

/*
 * XCSR+$33C - interrupt line register
 * XCSR+$33D - interrupt pin register
 * XCSR+$33E - minimum grant register
 * XCSR+$33F - maximum latency register
 */

#define HARRIER_INTERRUPT_LINE_REG  (HARRIER_XCSR_BASE + 0x33C)
#define HARRIER_INTERRUPT_PIN_REG   (HARRIER_XCSR_BASE + 0x33D)
#define HARRIER_MINIMUM_GRANT_REG   (HARRIER_XCSR_BASE + 0x33E)
#define HARRIER_MAXIMUM_LATENCY_REG (HARRIER_XCSR_BASE + 0x33F)

/* XCSR+$344 - message passing attribute register */

#define HARRIER_MSG_PASSINGATTR_REG (HARRIER_XCSR_BASE + 0x344)

/*
 * XCSR+$348 - inbound translation register 0
 * XCSR+$350 - inbound translation register 0
 * XCSR+$358 - inbound translation register 0
 * XCSR+$360 - inbound translation register 0
 */

#define HARRIER_INBOUND_TRANSLATION_SIZE_0_REG	(HARRIER_XCSR_BASE + 0x348)
#define HARRIER_INBOUND_TRANSLATION_SIZE_1_REG	(HARRIER_XCSR_BASE + 0x350)
#define HARRIER_INBOUND_TRANSLATION_SIZE_2_REG	(HARRIER_XCSR_BASE + 0x358)
#define HARRIER_INBOUND_TRANSLATION_SIZE_3_REG	(HARRIER_XCSR_BASE + 0x360)

#define HARRIER_INBOUND_TRANSLATION_OFFSET_0_REG	(HARRIER_XCSR_BASE + 0x34A)
#define HARRIER_INBOUND_TRANSLATION_OFFSET_1_REG	(HARRIER_XCSR_BASE + 0x352)
#define HARRIER_INBOUND_TRANSLATION_OFFSET_2_REG	(HARRIER_XCSR_BASE + 0x35A)
#define HARRIER_INBOUND_TRANSLATION_OFFSET_3_REG	(HARRIER_XCSR_BASE + 0x362)

#define HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_0 (HARRIER_XCSR_BASE + 0x34C)
#define HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_1	(HARRIER_XCSR_BASE + 0x354)
#define HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_2	(HARRIER_XCSR_BASE + 0x35C)
#define HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_3	(HARRIER_XCSR_BASE + 0x364)

#define HARRIER_PCI_STATUS_REG		(HARRIER_XCSR_BASE + 0x383)
#define HARRIER_PCI_GENERAL_PURPOSE_REG (HARRIER_XCSR_BASE + 0x384)

/* XCSR+$400 - general purpose memory, 2Kilobytes */

#define HARRIER_GENERAL_PURPOSE_MEMORY_STARTADDR (HARRIER_XCSR_BASE + 0x400)


/*
 * HARRIER_REGISTER_VENDOR_ID (VENI) XCSR+$000
 * HARRIER_REGISTER_DEVICE_ID (DEVI) XCSR+$002
 */

#define HARRIER_VENI			0x1057	/* vendor ID */
#define HARRIER_DEVI			0x480B	/* device ID */
#define HARRIER_IDS			((HARRIER_DEVI<<16)|HARRIER_VENI)

/*
 * HARRIER_REGISTER_REVISION_ID (REVI) XCSR+$004 
 */

#define HARRIER_REGISTER_REVISION_ID   (HARRIER_XCSR_BASE + 0x004)
#define HARRIER_REVISION_1             0x0001
#define HARRIER_REVISION_ID_SHIFT_MASK 16

/* HARRIER_GLOBAL_CONTROL_STATUS_REG (GCSR) XCSR+$010 */

#define HARRIER_GCSR_PUR	    (1<<31) /* power-up reset */
#define HARRIER_GCSR_PUST_MASK	   (15<<24) /* power-up status mask */
#define HARRIER_GCSR_AOAO	    (1<<20) /* address only ack other */

#define HARRIER_GCSR_XBS_MASK	    (3<<18) /* burst size/mode mask */
#define HARRIER_GCSR_XBS_64	    (0<<18) /* burst size: 64 */
#define HARRIER_GCSR_XBS_128	    (1<<18) /* burst size: 128 */
#define HARRIER_GCSR_XBS_256	    (2<<18) /* burst size: 256 */
#define HARRIER_GCSR_XBS_CON	    (3<<18) /* burst mode continuous */

#define HARRIER_GCSR_BTO_MASK	    (3<<16) /* bus time-out mask */
#define HARRIER_GCSR_BTO_256	    (0<<16) /* bus time-out 256 usec */
#define HARRIER_GCSR_BTO_64	    (1<<16) /* bus time-out 64 usec */
#define HARRIER_GCSR_BTO_8	    (2<<16) /* bus time-out 8 usec */
#define HARRIER_GCSR_BTO_DIS	    (3<<16) /* bus time-out disabled */

#define HARRIER_GCSR_MID_MASK	    (3<<12) /* bus master ID mask */
#define HARRIER_GCSR_MID_CPU0	    (0<<12) /* bus master ID: CPU0 */
#define HARRIER_GCSR_MID_CPU1	    (1<<12) /* bus master ID: CPU1 */
#define HARRIER_GCSR_MID_EXTL	    (2<<12) /* bus master ID: EXTL */
#define HARRIER_GCSR_MID_HARRIER    (3<<12) /* bus master ID: Harrier */

#define HARRIER_GCSR_RAT_MASK	    (7<<8)  /* PowerPC/PCI clk ratio mask */
#define HARRIER_GCSR_RAT_1to1	    (1<<8)  /* PowerPC/PCI clk ratio 1:1 */
#define HARRIER_GCSR_RAT_2to1	    (2<<8)  /* PowerPC/PCI clk ratio 2:1 */
#define HARRIER_GCSR_RAT_3to1	    (3<<8)  /* PowerPC/PCI clk ratio 3:1 */
#define HARRIER_GCSR_RAT_3to2	    (4<<8)  /* PowerPC/PCI clk ratio 3:2 */
#define HARRIER_GCSR_RAT_5to2	    (6<<8)  /* PowerPC/PCI clk ratio 5:2 */



/* HARRIER_MISC_CONTROL_STATUS_REG   (MCSR) XCSR+$01C */

#define HARRIER_MCSR_BRDFLS	    (1<<31) /* board fail status */
#define HARRIER_MCSR_BRDFL	    (1<<30) /* board fail */
#define HARRIER_MCSR_EREADYS	    (1<<29) /* PCI bus enum ready status */
#define HARRIER_MCSR_EREADY	    (1<<28) /* PCI bus enumeration ready */
#define HARRIER_MCSR_SCON	    (1<<27) /* system controller */
#define HARRIER_MCSR_TBEN0	    (1<<26) /* time base enable 0 */
#define HARRIER_MCSR_TBEN1	    (1<<25) /* time base enable 1 */
#define HARRIER_MCSR_RSTOUT	    (1<<24) /* reset out */
#define HARRIER_MCSR_RSTINH	    (1<<16) /* reset inhibit */


/* HARRIER_EXCEPTION_ENABLE_REG	(HARRIER_XCSR_BASE + 0x40) */
#define HARRIER_XCSR_FEEN 	HARRIER_EXCEPTION_ENABLE_REG

#define HARRIER_FEEN_DMA	    (1<<15) /* enable DMA exception */
#define HARRIER_FEEN_IMDB	    (1<<14) /* enable IB doorbell exc */
#define HARRIER_FEEN_IMM0	    (1<<13) /* enable IB MP 0 exc */
#define HARRIER_FEEN_IMM1	    (1<<12) /* enable IB MP 1 exc */
#define HARRIER_FEEN_MIP	    (1<<11) /* enable MIP exception */
#define HARRIER_FEEN_UA0	    (1<<10) /* enable UART #0 exception */
#define HARRIER_FEEN_UA1	    (1<<9)  /* enable UART #1 exception */
#define HARRIER_FEEN_ABT	    (1<<8)  /* enable abort exception */


/* HARRIER_REGISTER_FUNCTIONAL_EXCEPTION_STATUS (FEST) XCSR+$044 */

#define HARRIER_FEST_DMA	    (1<<15) /* DMA transaction complete */
#define HARRIER_FEST_IMDB	    (1<<14) /* IB doorbell bit written */
#define HARRIER_FEST_IMM0	    (1<<13) /* IB message 0 written */
#define HARRIER_FEST_IMM1	    (1<<12) /* IB message 1 written */
#define HARRIER_FEST_MIP	    (1<<11) /* msg written to post_list */
#define HARRIER_FEST_UA0	    (1<<10) /* UART #0 requesting service */
#define HARRIER_FEST_UA1	    (1<<9)  /* UART #1 requesting service */
#define HARRIER_FEST_ABT	    (1<<8)  /* abort was asserted */


/* HARRIER_EXCEPTION_MASK_REG	(HARRIER_XCSR_BASE + 0x48) */
#define HARRIER_XCSR_FEMA 	HARRIER_EXCEPTION_MASK_REG

#define HARRIER_FEMA_DMA	    (1<<15) /* mask DMA exception */
#define HARRIER_FEMA_IMDB	    (1<<14) /* mask IB doorbell exc */
#define HARRIER_FEMA_IMM0	    (1<<13) /* mask IB msg passing 0 exc */
#define HARRIER_FEMA_IMM1	    (1<<12) /* mask IB msg passing 1 exc */
#define HARRIER_FEMA_MIP	    (1<<11) /* mask MIP exception */
#define HARRIER_FEMA_UA0	    (1<<10) /* mask UART #0 exception */
#define HARRIER_FEMA_UA1	    (1<<9)  /* mask UART #1 exception */
#define HARRIER_FEMA_ABT	    (1<<8)  /* mask abort exception */


/* HARRIER_EXCEPTION_CLEAR_REG	   (FECL) XCSR+$04C */

#define HARRIER_FECL_DMA	    (1<<15) /* clear DMA exception */
#define HARRIER_FECL_IMM0	    (1<<13) /* clear IB msg passing 0 exc */
#define HARRIER_FECL_IMM1	    (1<<12) /* clear IB msg passing 1 exc */
#define HARRIER_FECL_ABT	    (1<<8)  /* clear abort exception */



/* HARRIER_ERROR_EXCEPTION_ENABLE_REG	 (EEEN) XCSR+$050 */

#define HARRIER_EEEN_PMA	    (1<<31) /* enable PCI mstr abort exc */
#define HARRIER_EEEN_PTA	    (1<<30) /* enable PCI trgt abort exc */
#define HARRIER_EEEN_PAP	    (1<<29) /* enable PCI addr parity exc */
#define HARRIER_EEEN_PDP	    (1<<28) /* enable PCI data parity exc */
#define HARRIER_EEEN_PDT	    (1<<27) /* enable PCI bus delay exc */
#define HARRIER_EEEN_PSE	    (1<<26) /* enable PCI SERR exception */
#define HARRIER_EEEN_PPE	    (1<<25) /* enable PCI PERR exception */
#define HARRIER_EEEN_SSE	    (1<<23) /* enable SDRAM SBE exception */
#define HARRIER_EEEN_SSC	    (1<<22) /* enable SBE cnt overflow ex */
#define HARRIER_EEEN_SMX	    (1<<21) /* enable SDRAM MBE exception */
#define HARRIER_EEEN_SMS	    (1<<20) /* enable SDRAM MBE scrub exc */
#define HARRIER_EEEN_XBT	    (1<<15) /* enable PPC bus timeout exc */
#define HARRIER_EEEN_XAP	    (1<<14) /* enable PPC addr parity exc */
#define HARRIER_EEEN_XDP	    (1<<13) /* enable PPC data parity exc */
#define HARRIER_EEEN_XDT	    (1<<12) /* enable PPC del timeout exc */



/* HARRIER_ERROR_EXCEPTION_STATUS_REG	 (EEST) XCSR+$054 */

#define HARRIER_EEST_PMA	    (1<<31) /* PCI master abort exc */
#define HARRIER_EEST_PTA	    (1<<30) /* PCI target abort exc */
#define HARRIER_EEST_PAP	    (1<<29) /* PCI address parity exc */
#define HARRIER_EEST_PDP	    (1<<28) /* PCI data parity exc */
#define HARRIER_EEST_PDT	    (1<<27) /* PCI bus delay exc */
#define HARRIER_EEST_PSE	    (1<<26) /* PCI SERR exception */
#define HARRIER_EEST_PPE	    (1<<25) /* PCI PERR exception */
#define HARRIER_EEST_POF	    (1<<24) /* PCI error overflow */
#define HARRIER_EEST_SSE	    (1<<23) /* SDRAM SBE exception */
#define HARRIER_EEST_SSC	    (1<<22) /* SDRAM SBE cnt overflow exc */
#define HARRIER_EEST_SMX	    (1<<21) /* SDRAM MBE exception */
#define HARRIER_EEST_SMS	    (1<<20) /* SDRAM MBE scrub exception */
#define HARRIER_EEST_SSOF	    (1<<17) /* SDRAM SBE overflow */
#define HARRIER_EEST_SMOF	    (1<<16) /* SDRAM MBE overflow */
#define HARRIER_EEST_XBT	    (1<<15) /* PPC bus timeout exc */
#define HARRIER_EEST_XAP	    (1<<14) /* PPC addr parity exc */
#define HARRIER_EEST_XDP	    (1<<13) /* PPC data parity exc */
#define HARRIER_EEST_XDT	    (1<<12) /* PPC del timeout exc */
#define HARRIER_EEST_XOF	    (1<<8)  /* PPC error overflow exc */


/* HARRIER_ERROR_EXCEPTION_CLEAR_REG	 (EECL) XCSR+$058 */

#define HARRIER_EECL_PMA	    (1<<31) /* clear PCI master abort exc */
#define HARRIER_EECL_PTA	    (1<<30) /* clear PCI target abort exc */
#define HARRIER_EECL_PAP	    (1<<29) /* clear PCI addr parity exc */
#define HARRIER_EECL_PDP	    (1<<28) /* clear PCI data parity exc */
#define HARRIER_EECL_PDT	    (1<<27) /* clear PCI bus delay exc */
#define HARRIER_EECL_PSE	    (1<<26) /* clear PCI SERR exception */
#define HARRIER_EECL_PPE	    (1<<25) /* clear PCI PERR exception */
#define HARRIER_EECL_POF	    (1<<24) /* clear PCI error overflow */
#define HARRIER_EECL_SSE	    (1<<23) /* clear SDRAM SBE exception */
#define HARRIER_EECL_SSC	    (1<<22) /* clear SBE cnt overflow exc */
#define HARRIER_EECL_SMX	    (1<<21) /* clear SDRAM MBE exception */
#define HARRIER_EECL_SMS	    (1<<20) /* clear SDRAM MBE scrub exc */
#define HARRIER_EECL_SSOF	    (1<<17) /* clear SBE overflow exc */
#define HARRIER_EECL_SMOF	    (1<<16) /* clear MBE overflow exc */
#define HARRIER_EECL_XBT	    (1<<15) /* clear PPC bus timeout exc */
#define HARRIER_EECL_XAP	    (1<<14) /* clear PPC addr parity exc */
#define HARRIER_EECL_XDP	    (1<<13) /* clear PPC data parity exc */
#define HARRIER_EECL_XDT	    (1<<12) /* clear PPC del timeout exc */
#define HARRIER_EECL_XOF	    (1<<8)  /* clear PPC err overflow exc */



/* HARRIER_ERROR_EXCEPTION_INT_ENABLE_REG   (EEINT) XCSR+$05C */

#define HARRIER_EEINT_PMA	    (1<<31) /* enable PCI mstr abort int */
#define HARRIER_EEINT_PTA	    (1<<30) /* enable PCI trgt abort int */
#define HARRIER_EEINT_PAP	    (1<<29) /* enable PCI addr parity int */
#define HARRIER_EEINT_PDP	    (1<<28) /* enable PCI data parity int */
#define HARRIER_EEINT_PDT	    (1<<27) /* enable PCI bus delay int */
#define HARRIER_EEINT_PSE	    (1<<26) /* enable PCI SERR interrupt */
#define HARRIER_EEINT_PPE	    (1<<25) /* enable PCI PERR interrupt */
#define HARRIER_EEINT_SSE	    (1<<23) /* enable SDRAM SBE interrupt */
#define HARRIER_EEINT_SSC	    (1<<22) /* enable SBE cnt overflo int */
#define HARRIER_EEINT_SMX	    (1<<21) /* enable SDRAM MBE interrupt */
#define HARRIER_EEINT_SMS	    (1<<20) /* enable SDRAM MBE scrub int */
#define HARRIER_EEINT_XBT	    (1<<15) /* enable PPC bus timeout int */
#define HARRIER_EEINT_XAP	    (1<<14) /* enable PPC addr parity int */
#define HARRIER_EEINT_XDP	    (1<<13) /* enable PPC data parity int */
#define HARRIER_EEINT_XDT	    (1<<12) /* enable PPC del timeout int */


/*
 * HARRIER_ERROR_EXCMACHINECHECK0_ENABLE (EEMCKx)
 * HARRIER_ERROR_EXCMACHINECHECK1_ENABLE (EEMCKx)
 * XCSR+$060 & XCSR+$064
 */

#define HARRIER_EEMCK_PMA	    (1<<31) /* enable PCI mstr abort MC */
#define HARRIER_EEMCK_PTA	    (1<<30) /* enable PCI trgt abort MC */
#define HARRIER_EEMCK_PAP	    (1<<29) /* enable PCI addr parity MC */
#define HARRIER_EEMCK_PDP	    (1<<28) /* enable PCI data parity MC */
#define HARRIER_EEMCK_PDT	    (1<<27) /* enable PCI bus delay MC */
#define HARRIER_EEMCK_PSE	    (1<<26) /* enable PCI SERR MC */
#define HARRIER_EEMCK_PPE	    (1<<25) /* enable PCI PERR MC */
#define HARRIER_EEMCK_SSE	    (1<<23) /* enable SDRAM SBE MC */
#define HARRIER_EEMCK_SSC	    (1<<22) /* enable SBE cnt overflow MC */
#define HARRIER_EEMCK_SMX	    (1<<21) /* enable SDRAM MBE MC */
#define HARRIER_EEMCK_SMS	    (1<<20) /* enable SDRAM MBE scrub MC */
#define HARRIER_EEMCK_XBT	    (1<<15) /* enable PPC bus timeout MC */
#define HARRIER_EEMCK_XAP	    (1<<14) /* enable PPC addr parity MC */
#define HARRIER_EEMCK_XDP	    (1<<13) /* enable PPC data parity MC */
#define HARRIER_EEMCK_XDT	    (1<<12) /* enable PPC del timeout MC */


/* HARRIER_ERR_DIAGS_PPCATTRIBUTE_REG  (EXAT) XCSR+$074 */

#define HARRIER_EXAT_MID_MASK		(3<<28)	/* PPC bus master ID mask */
#define HARRIER_EXAT_TBST		(1<<24)	/* transfer burst */
#define HARRIER_EXAT_TSIZ_MASK		(7<<21)	/* transfer size mask */
#define HARRIER_EXAT_TT_MASK		(31<<16) /* transfer type mask */


/* HARRIER_ERR_DIAGS_PCIATTRIBUTE_REG	 (EPAT) XCSR+$07C */

#define HARRIER_EPAT_MID_MASK		(15<<28) /* PCI command mask */


/*
 * HARRIER_REGISTER_WATCHDOG_TIMER_CONTROL (WTxC)
 * XCSR+$080 & XCSR+$088
 */

#define HARRIER_WTxC_KEY1		(0x55<<24) /* key pattern #1 */
#define HARRIER_WTxC_KEY2		(0xAA<<24) /* key pattern #2 */
#define HARRIER_WTxC_ENA		(1<<23)	/* watchdog timer enable */
#define HARRIER_WTxC_ARM		(1<<22)	/* register armed for write */

#define HARRIER_WTxC_RES_MASK		(15<<16) /* watchdog timer resolution */
#define HARRIER_WTxC_RES_1U		(0<<16)	/* 1 usec resolution */
#define HARRIER_WTxC_RES_2U		(1<<16)	/* 2 usec resolution */
#define HARRIER_WTxC_RES_4U		(2<<16)	/* 4 usec resolution */
#define HARRIER_WTxC_RES_8U		(3<<16)	/* 8 usec resolution */
#define HARRIER_WTxC_RES_16U		(4<<16)	/* 16 usec resolution */
#define HARRIER_WTxC_RES_32U		(5<<16)	/* 32 usec resolution */
#define HARRIER_WTxC_RES_64U		(6<<16)	/* 64 usec resolution */
#define HARRIER_WTxC_RES_128U		(7<<16)	/*  usec resolution */
#define HARRIER_WTxC_RES_256U		(8<<16)	/*  usec resolution */
#define HARRIER_WTxC_RES_512U		(9<<16)	/*  usec resolution */
#define HARRIER_WTxC_RES_1024U		(10<<16) /*  usec resolution */
#define HARRIER_WTxC_RES_2048U		(11<<16) /*  usec resolution */
#define HARRIER_WTxC_RES_4096U		(12<<16) /*  usec resolution */
#define HARRIER_WTxC_RES_8192U		(13<<16) /*  usec resolution */
#define HARRIER_WTxC_RES_16384U		(14<<16) /*  usec resolution */
#define HARRIER_WTxC_RES_32768U		(15<<16) /*  usec resolution */



/* HARRIER_PCI_ARBITER_REG  (PARB) XCSR+$090 */

#define HARRIER_PARB_PRI_MASK		(3<<14)	/* PCI arbiter priority mask */
#define HARRIER_PARB_PRI_FIXED		(0<<14)	/* fixed priority */
#define HARRIER_PARB_PRI_ROUNDROBIN	(1<<14) /* round robin prioity */
#define HARRIER_PARB_PRI_MIXED		(2<<14)	/* mixed priority scheme */

#define HARRIER_PARB_PRK_MASK		(15<<10) /* PCI arbiter parking mask */
#define HARRIER_PARB_PRK_LASTMASTER	(0<<10)	/* park on last master */
#define HARRIER_PARB_PRK_PARB6		(1<<10)	/* park on PARB6 */
#define HARRIER_PARB_PRK_PAR5B		(2<<10)	/* park on PARB5 */
#define HARRIER_PARB_PRK_PARB4		(3<<10)	/* park on PARB4 */
#define HARRIER_PARB_PRK_PARB3		(4<<10)	/* park on PARB3 */
#define HARRIER_PARB_PRK_PARB2		(5<<10)	/* park on PARB2 */
#define HARRIER_PARB_PRK_PARB1		(6<<10)	/* park on PARB1 */
#define HARRIER_PARB_PRK_PARB0		(7<<10)	/* park on PARB0 */
#define HARRIER_PARB_PRK_HARRIER	(8<<10)	/* park on Harrier */
#define HARRIER_PARB_PRK_NONE		(15<<10) /* none */
#define HARRIER_PARB_HIE_MASK		(7<<7)	/* PCI arbiter hierarchy */
#define HARRIER_PARB_POL		(1<<0)	/* park on lock */

#define HARRIER_PARB_ENABLE		(1<<0) /* enable arbiter */

/* HARRIER_PPC_ARBITER_REG (XARB) - XCSR+$094 */

#define HARRIER_XARB_PIPE_FBR_NONE	(0<<14) /* None */
#define HARRIER_XARB_PIPE_FBR_FLTA	(2<<14) /* Flatten always */
#define HARRIER_XARB_PIPE_FBR_FLTS	(3<<14) /* Flatten on mstr. switch */

#define HARRIER_XARB_PIPE_FSR_NONE	(0<<12) /* None */
#define HARRIER_XARB_PIPE_FSR_FLTA	(2<<12) /* Flatten always */
#define HARRIER_XARB_PIPE_FSR_FLTS	(3<<12) /* Flatten on mstr. switch */

#define HARRIER_XARB_PIPE_FBW_NONE	(0<<10) /* None */
#define HARRIER_XARB_PIPE_FBW_FLTA	(2<<10) /* Flatten always */
#define HARRIER_XARB_PIPE_FBW_FLTS	(3<<10) /* Flatten on mstr. switch */

#define HARRIER_XARB_PIPE_FSW_NONE	(0<<8)  /* None */
#define HARRIER_XARB_PIPE_FSW_FLTA	(2<<8)  /* Flatten always */
#define HARRIER_XARB_PIPE_FSW_FLTS	(3<<8)  /* Flatten on mstr. switch */

#define HARRIER_XARB_PRK_NONE		(0<<4)	 /* None */
#define HARRIER_XARB_PRK_LASTCPU	(1<<4)	 /* Park on last CPU */
#define HARRIER_XARB_PRK_CPU0		(2<<4)	 /* Park always on CPU0 */
#define HARRIER_XARB_PRK_CPU1		(3<<4)	 /* Park always on CPU1 */

#define HARRIER_XARB_ENABLE		(1<<0)   /* Enable PowerPC Arbiter */

/*
 * HARRIER_UART_x_RTDL_REG  (RTDLx) XCSR+$0C0 & XCSR+$0C8
 * HARRIER_UART_x_IEDH_REG  (IEDHx) XCSR+$0C1 & XCSR+$0C9
 * HARRIER_UART_x_IDFC_REG  (IDFCx) XCSR+$0C2 & XCSR+$0CA
 * HARRIER_UART_x_LCTL_REG  (LCTLx) XCSR+$0C3 & XCSR+$0CB
 */

#define HARRIER_IEDH_EDSSI		(1<<3)	/* enable modem status int */
#define HARRIER_IEDH_ELSI		(1<<2)	/* enable rec line stat int */
#define HARRIER_IEDH_ETBEI		(1<<1)	/* enable xmit hold reg int */
#define HARRIER_IEDH_ERBFI		(1<<0)	/* enable data available int */

#define	HARRIER_IDFC_FENS_MASK		(3<<6)	/* FIFOs enabled status mask */
#define	HARRIER_IDFC_FENS_DISABLED	(0<<6)	/* FIFOs disabled */
#define	HARRIER_IDFC_FENS_ENABLED	(3<<6)	/* FIFOs enabled */
#define	HARRIER_IDFC_IID_MASK		(7<<1)	/* interrupt ID mask */
#define	HARRIER_IDFC_IPEN		(1<<0)	/* interrupt pending */

#define	HARRIER_IDFC_RFTL_MASK		(3<<6)	/* receiver trigger lvl mask */
#define	HARRIER_IDFC_DMAS		(1<<3)	/* DMA mode select */
#define	HARRIER_IDFC_TFR		(1<<2)	/* transmitter FIFO reset */
#define	HARRIER_IDFC_RFR		(1<<1)	/* receiver FIFO reset */
#define	HARRIER_IDFC_FIFOEN		(1<<0)	/* FIFO enable */

#define	HARRIER_LCTL_DLAB		(1<<7)	/* divisor latch access bit */
#define	HARRIER_LCTL_SB			(1<<6)	/* set break */
#define	HARRIER_LCTL_SP			(1<<5)	/* stick parity */
#define	HARRIER_LCTL_EPS		(1<<4)	/* even parity select */
#define	HARRIER_LCTL_PEN		(1<<3)	/* parity enable */
#define	HARRIER_LCTL_STB		(1<<2)	/* number of stop bits */
#define	HARRIER_LCTL_WLS_MASK		(3<<0)	/* word length select mask */
#define	HARRIER_LCTL_WLS_5BIT		(0<<0)	/* 5 bits per character */
#define	HARRIER_LCTL_WLS_6BIT		(1<<0)	/* 6 bits per character */
#define	HARRIER_LCTL_WLS_7BIT		(2<<0)	/* 7 bits per character */
#define	HARRIER_LCTL_WLS_8BIT		(3<<0)	/* 8 bits per character */


/*
 * HARRIER_UART_x_MCTL_REG  (MCTLx) XCSR+$0C4 & XCSR+$0CC
 * HARRIER_UART_x_LSTA_REG  (LSTAx) XCSR+$0C5 & XCSR+$0CD
 * HARRIER_UART_x_MSTA_REG  (MSTAx) XCSR+$0C6 & XCSR+$0CE
 * HARRIER_UART_x_SCRT_REG  (SCRTx) XCSR+$0C7 & XCSR+$0CF
 */

#define	HARRIER_MCTL_LOOP		(1<<4)	/* loopback mode */
#define	HARRIER_MCTL_OUT2		(1<<3)	/* output 2 signal */
#define	HARRIER_MCTL_OUT1		(1<<2)	/* output 1 signal */
#define	HARRIER_MCTL_RTS		(1<<1)	/* request-to-send output */
#define	HARRIER_MCTL_DTR		(1<<0)	/* data-terminal-ready output */

#define	HARRIER_LSTA_FERR		(1<<7)	/* error in receiver FIFO */
#define	HARRIER_LSTA_TEMT		(1<<6)	/* xmitter empty */
#define	HARRIER_LSTA_THRE		(1<<5)	/* xmiterr holding reg empty */
#define	HARRIER_LSTA_BI			(1<<4)	/* break interrupt */
#define	HARRIER_LSTA_FE			(1<<3)	/* framing error */
#define	HARRIER_LSTA_PE			(1<<2)	/* parity error */
#define	HARRIER_LSTA_OE			(1<<1)	/* overrun error */
#define	HARRIER_LSTA_DR			(1<<0)	/* data ready */

#define	HARRIER_MSTA_DCD		(1<<7)	/* data carrier detect */
#define	HARRIER_MSTA_RI			(1<<6)	/* ring indicator */
#define	HARRIER_MSTA_DSR		(1<<5)	/* data set ready */
#define	HARRIER_MSTA_CTS		(1<<4)	/* clear to send */
#define	HARRIER_MSTA_DDCD		(1<<3)	/* delta data carrier detect */
#define	HARRIER_MSTA_TERI		(1<<2)	/* trailing edge ring ind */
#define	HARRIER_MSTA_DDSR		(1<<1)	/* delta data set ready */
#define	HARRIER_MSTA_DCTS		(1<<0)	/* delta clear to send */


/*
 * HARRIER_UART_CONTROL_REG	     (UCTL) XCSR+$0D0
 * HARRIER_UART_CLOCK_PRESCALER_REG  (UPS)  XCSR+$0D3
 */

#define	HARRIER_UCTL_U0BO		(1<<6)	/* UART 0 baudout */
#define	HARRIER_UCTL_U1BO		(1<<5)	/* UART 1 baudout */
#define	HARRIER_UCTL_U0GRTS		(1<<4)  /* UART 0 gated RTS */
#define	HARRIER_UCTL_U1GRTS		(1<<3)  /* UART 1 gated RTS */
#define	HARRIER_UCTL_U0TXP		(1<<2)  /* UART 0 transmitter pause */
#define	HARRIER_UCTL_U1TXP		(1<<1)  /* UART 1 transmitter pause */
#define	HARRIER_UCTL_UCOS		(1<<0)	/* UART clock select */



/* HARRIER_MPIC_BASEADDRESS_REG     (MBAR) XCSR+$0E0 */

#define	HARRIER_MBAR_MBA_MASK		0xFFFC0000 /* MPIC base address mask */
#define HARRIER_MBAR_ENA		(1<<16)	/* enable XMPI reg visibility */



/* HARRIER_MPIC_CONTROL_STATUS_REG    (MCSR) XCSR+$0E4 */

#define HARRIER_MCSR_OPI	       (1<<6)  /* OPIC interrupt */



/* HARRIER_SDRAM_GENERAL_CONTROL_REG  (SDGC) XCSR+$100 */

#define HARRIER_SDGC_MXRR               (3<<12) /* Multiply Refresh Rate */
#define HARRIER_SDGC_DREF		(1<<26)	/* disable refresh */
#define HARRIER_SDGC_DERC		(1<<25)	/* disable error correction */
#define HARRIER_SDGC_RWCB		(1<<24)	/* read/write chk bits enable */
#define HARRIER_SDGC_ENRV		(1<<23)	/* enable reset vector */
#define HARRIER_SDGC_SWVT		(1<<19)	/* swap vector table */


/* HARRIER_SDRAM_TIMING_CONTROL_REG   (SDTC) XCSR+$104 */

#define HARRIER_SDTC_CL3		(1<<28)	/* SDRAM CAS latency */
#define HARRIER_SDTC_CL3_SHIFT	28	    /* SDRAM CAS latency shift */
#define HARRIER_SDTC_CL3_MASK	0x01	    /* SDRAM CAS latency mask  */

#define HARRIER_SDTC_TRC8		(0<<24)	/* SDRAM tRC 8 CLKS */
#define HARRIER_SDTC_TRC9		(1<<24)	/* SDRAM tRC 9 CLKS */
#define HARRIER_SDTC_TRC10		(2<<24)	/* SDRAM tRC 10 CLKS */
#define HARRIER_SDTC_TRC11		(3<<24)	/* SDRAM tRC 11 CLKS */
#define HARRIER_SDTC_TRC6		(6<<24)	/* SDRAM tRC 6 CLKS */
#define HARRIER_SDTC_TRC7		(7<<24)	/* SDRAM tRC 7 CLKS */

#define HARRIER_SDTC_TRC_SHIFT  24      /* TRC bit position */
#define HARRIER_SDTC_TRC_MASK   0x07    /* TRC bits position mask */

#define HARRIER_SDTC_TRAS4		(0<<20)	/* SDRAM tRAS 4CLKS */
#define HARRIER_SDTC_TRAS5		(1<<20)	/* SDRAM tRAS 5CLKS */
#define HARRIER_SDTC_TRAS6		(2<<20)	/* SDRAM tRAS 6CLKS */
#define HARRIER_SDTC_TRAS7		(3<<20)	/* SDRAM tRAS 7CLKS */

#define HARRIER_SDTC_TRAS_SHIFT 20      /* TRAS bit position */
#define HARRIER_SDTC_TRAS_MASK  0x03    /* TRAS bit position mask */

#define HARRIER_SDTC_WDPL		(1<<17)	/* wait on tDPL*/
#define HARRIER_SDTC_WDPL_SHIFT 17      /* wdpl bit position */

#define HARRIER_SDTC_TDP1		(0<<16)	/* SDRAM tDP 1 CLKS */
#define HARRIER_SDTC_TDP2		(1<<16)	/* SDRAM tDP 2 CLKS */

#define HARRIER_SDTC_TDP_SHIFT_MASK  0x01    /* tdp bit position mask */
#define HARRIER_SDTC_TDP_SHIFT  16      /* tdp bit position */

#define HARRIER_SDTC_TRP2		(0<<12)	/* SDRAM tRP 2 CLKS */
#define HARRIER_SDTC_TRP3		(1<<12)	/* SDRAM tRP 3 CLKS */

#define HARRIER_SDTC_TRP_SHIFT  12           /* trp bit position */
#define HARRIER_SDTC_TRP_SHIFT_MASK  0x01    /* trp bit position mask */

#define HARRIER_SDTC_TRCD2		(0<<8)	/* SDRAM tRCD 2 CLKS */
#define HARRIER_SDTC_TRCD3		(1<<8)	/* SDRAM tRCD 3 CLKS */

#define HARRIER_SDTC_TRCD_SHIFT	8	      /* tRCD bit position */
#define HARRIER_SDTC_TRCD_SHIFT_MASK  0x01    /* trcd bit position mask */

#define HARRIER_SDTC_SDER		(1<<0)	/* swap vector table */


/*
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_A (SDBAA) XCSR+($110)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_B (SDBAB) XCSR+($114)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_C (SDBAC) XCSR+($118)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_D (SDBAD) XCSR+($11C)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_E (SDBAE) XCSR+($120)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_F (SDBAF) XCSR+($124)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_G (SDBAG) XCSR+($128)
 * HARRIER_REG_SDRAM_BLOCK_ADDRESSING_H (SDBAH) XCSR+($12C)
 */

#define HARRIER_SDBA_BASE_MASK		0xFF000000 /* SDRAM base address mask */
#define HARRIER_SDBA_BASE_SHIFT		24         /* SDRAM base address shift */

#define HARRIER_SDBA_SIZE_MASK		(15<<16)   /* SDRAM block size mask */
#define HARRIER_SDBA_SIZE_SHIFT		16         /* SDRAM block size shift */

#define HARRIER_SDBA_SIZE_0           (0<<16)  /* bank size 0MB            */
#define HARRIER_SDBA_SIZE_32_4MX16    (1<<16)  /* bank size 32MB(4Mx16)    */
#define HARRIER_SDBA_SIZE_64_8MX8     (2<<16)  /* bank size 64MB(8Mx8)     */
#define HARRIER_SDBA_SIZE_64_8MX16    (3<<16)  /* bank size 64MB(8Mx16)    */
#define HARRIER_SDBA_SIZE_128_16MX4   (4<<16)  /* bank size 128MB(16Mx4)   */
#define HARRIER_SDBA_SIZE_128_16MX8   (5<<16)  /* bank size 128MB(16Mx8)   */
#define HARRIER_SDBA_SIZE_128_16MX16  (6<<16)  /* bank size 128MB(16Mx16)  */
#define HARRIER_SDBA_SIZE_256_32MX4   (7<<16)  /* bank size 256MB(32Mx4)   */
#define HARRIER_SDBA_SIZE_256_32MX8   (8<<16)  /* bank size 256MB(32Mx8)   */
#define HARRIER_SDBA_SIZE_256_32MX16  (9<<16)  /* bank size 256MB(32Mx8)   */
#define HARRIER_SDBA_SIZE_512_64MX4   (10<<16) /* bank size 512MB(64Mx4)   */
#define HARRIER_SDBA_SIZE_512_64MX8   (11<<16) /* bank size 512MB(64Mx8)   */
#define HARRIER_SDBA_SIZE_512_64MX16  (12<<16) /* bank size 512MB(64MX16)  */
#define HARRIER_SDBA_SIZE_1024_128MX4 (13<<16) /* bank size 1024MB(128MX4) */
#define HARRIER_SDBA_SIZE_1024_128MX8 (14<<16) /* bank size 1024MB(128MX8) */
#define HARRIER_SDBA_SIZE_2048_256MX4 (15<<16) /* bank size 2048MB(256MX4) */

#define HARRIER_SDBA_ENB		(1<<8)	/* SDRAM block enable */



/* HARRIER_SDRAM_SCRUB_CONTROL_REG     (SDSC) XCSR+$130 */

#define HARRIER_SDSC_SCWE		(1<<15)	/* scrub write enable */
#define HARRIER_SDSC_SCCNT_MASK		(3<<8)	/* scrub counter */
#define HARRIER_SDSC_SCPA_MASK		(255<<0) /* scrub prescalar adjust */


/*
 * HARRIER_SDRAM_SBE_SYNDROME_REG    (ESYN)  $140
 * HARRIER_SDRAM_SBE_SCRUBBLOCK_REG  (ESB)   $141
 * HARRIER_SDRAM_SBE_COUNT_REG	     (SECNT) $142
 */

#define HARRIER_SDSES_EOS		(1<<3)	/* error on scrub */
#define HARRIER_SDSES_ESB_MASK		(7<<0)	/* error scrub block mask */
#define HARRIER_SDSES_ESB_A		(0<<0)	/* error scrub block A */
#define HARRIER_SDSES_ESB_B		(1<<0)	/* error scrub block B */
#define HARRIER_SDSES_ESB_C		(2<<0)	/* error scrub block C */
#define HARRIER_SDSES_ESB_D		(3<<0)	/* error scrub block D */
#define HARRIER_SDSES_ESB_E		(4<<0)	/* error scrub block E */
#define HARRIER_SDSES_ESB_F		(5<<0)	/* error scrub block F */
#define HARRIER_SDSES_ESB_G		(6<<0)	/* error scrub block G */
#define HARRIER_SDSES_ESB_H		(7<<0)	/* error scrub block H */



/* HARRIER_SDRAM_MBE_SCRUBBLOCK_REG  (ESB) $149 */

#define HARRIER_SDMES_ESB_MASK		(7<<0)	/* error scrub block mask */
#define HARRIER_SDMES_ESB_A		(0<<0)	/* error scrub block A */
#define HARRIER_SDMES_ESB_B		(1<<0)	/* error scrub block B */
#define HARRIER_SDMES_ESB_C		(2<<0)	/* error scrub block C */
#define HARRIER_SDMES_ESB_D		(3<<0)	/* error scrub block D */
#define HARRIER_SDMES_ESB_E		(4<<0)	/* error scrub block E */
#define HARRIER_SDMES_ESB_F		(5<<0)	/* error scrub block F */
#define HARRIER_SDMES_ESB_G		(6<<0)	/* error scrub block G */
#define HARRIER_SDMES_ESB_H		(7<<0)	/* error scrub block H */


/*
 * HARRIER_XPORT0_ATTR_REG	   (XPAT0) XCSR+$154
 * HARRIER_XPORT1_ATTR_REG	   (XPAT1) XCSR+$15C
 * HARRIER_XPORT2_ATTR_REG	   (XPAT2) XCSR+$164
 * HARRIER_XPORT3_ATTR_REG	   (XPAT3) XCSR+$16C
 */

#define HARRIER_XPAT_REN		(1<<31)	/* read enable */
#define HARRIER_XPAT_WEN		(1<<30)	/* write enable */
#define HARRIER_XPAT_BAM		(1<<29)	/* basic mode */
#define HARRIER_XPAT_RVEN		(1<<27)	/* reset vector enable */

#define HARRIER_XPAT_DW_MASK		(3<<24)	/* data width mask */
#define HARRIER_XPAT_DW_8BITS		(0<<24)	/* data width 8 bits */
#define HARRIER_XPAT_DW_16BITS		(1<<24)	/* data width 16 bits */
#define HARRIER_XPAT_DW_32BITS		(2<<24)	/* data width 32 bits */
#define HARRIER_XPAT_DW_16HAWK		(3<<24)	/* 16 bits Hawk compatible */
#define HARRIER_XPAT_AD_MASK		(15<<20) /* access delay */
#define HARRIER_XPAT_BLE_MASK		(3<<16)	/* burst length mask */
#define HARRIER_XPAT_BLE_4BYTES		(0<<16)	/* burst length 4 bytes */
#define HARRIER_XPAT_BLE_8BYTES		(1<<16)	/* burst length 8 bytes */
#define HARRIER_XPAT_BLE_16BYTES	(2<<16)	/* burst length 16 bytes */
#define HARRIER_XPAT_BLE_32BYTES	(3<<16)	/* burst length 32 bytes */

#define HARRIER_XPAT_BREN		(1<<12)	/* burst read enable */
#define HARRIER_XPAT_BRD_MASK		(7<<8)	/* burst read delay mask */

#define HARRIER_XPAT_BWEN		(1<<4)	/* burst write enable */
#define HARRIER_XPAT_BWD_MASK		(7<<0)	/* burst write delay mask */

#define HARRIER_I2CO_STA		(1<<3)	/* i2c mstr controller start */
#define HARRIER_I2CO_STP		(1<<2)	/* i2c mstr controller stop */
#define HARRIER_I2CO_ACKO		(1<<1)	/* i2c mstr controller ack */
#define HARRIER_I2CO_ENA		(1<<0)	/* i2c mstr interface enable */


#define HARRIER_I2ST_DIN		(1<<3)	/* data in from i2c slave */
#define HARRIER_I2ST_ERR		(1<<2)	/* error: STA & STP both set */
#define HARRIER_I2ST_ACKI		(1<<1)	/* acknowledge in slave */
#define HARRIER_I2ST_CMP		(1<<0)	/* i2c operation complete */



/* HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG   (BPCS) XCSR+$200 */

#define HARRIER_BPCS_OFBR		(1<<15)	/* outbound flush before read */
#define HARRIER_BPCS_DLR		(1<<14)	/* ignore latency requirement */
#define HARRIER_BPCS_HIL		(1<<13)	/* host brdg initial latency */

#define HARRIER_BPCS_PIM_MASK		(3<<8)	/* PCI interrupt mappin mask */
#define HARRIER_BPCS_PIM_INTA_		(0<<8)	/* map PCI interrupt to INTA_ */
#define HARRIER_BPCS_PIM_INTB_		(1<<8)	/* map PCI interrupt to INTB_ */
#define HARRIER_BPCS_PIM_INTC_		(2<<8)	/* map PCI interrupt to INTC_ */
#define HARRIER_BPCS_PIM_INTD_		(3<<8)	/* map PCI interrupt to INTD_ */

#define HARRIER_BPCS_CSM		(1<<5)	/* config space mask */
#define HARRIER_BPCS_CSH		(1<<4)	/* config space hold off */
#define HARRIER_BPCS_P64		(1<<0)	/* PCI 64-bit bus */

#define HARRIER_BPCS_XCSR_CSH	(1<<20)

/* HARRIER_BRIDGE_PPC_CONTROLSTATUS_REG   (BXCS) XCSR+$204 */

#define HARRIER_BXCS_IFBR		(1<<15)	/* inbound flush before read */
#define HARRIER_BXCS_BHG		(1<<14)	/* Harrier bus hog mode */
#define HARRIER_BXCS_RSF		(1<<13)	/* read-ahead sync flush */
#define HARRIER_BXCS_SSF		(1<<12)	/* store-gathter sync flush */

#define HARRIER_BXCS_RBT_MASK		(3<<10)	/* read-ahead backup tmr msk */
#define HARRIER_BXCS_RBT_32CLKS		(0<<10)	/* time out: 32 clocks */
#define HARRIER_BXCS_RBT_64CLKS		(1<<10)	/* time out: 64 clocks */
#define HARRIER_BXCS_RBT_256CLKS	(2<<10)	/* time out: 256 clocks */
#define HARRIER_BXCS_RBT_DISABLED	(3<<10)	/* time out: disabled */

#define HARRIER_BXCS_SBT_MASK		(3<<8)	/* store-gthr backup tmr msk */
#define HARRIER_BXCS_SBT_32CLKS		(0<<8)	/* time out: 32 clocks */
#define HARRIER_BXCS_SBT_64CLKS		(1<<8)	/* time out: 64 clocks */
#define HARRIER_BXCS_SBT_256CLKS	(2<<8)	/* time out: 256 clocks */
#define HARRIER_BXCS_SBT_DISABLED	(3<<8)	/* time out: disabled */

#define HARRIER_BXCS_P1H		(1<<5)	/* processor 1 holdoff */
#define HARRIER_BXCS_P0H		(1<<4)	/* processor 0 holdoff */
#define HARRIER_BXCS_CSE		(1<<3)	/* enable copy-back snarfing */

#define HARRIER_OTOF_RXT		(1<<10)	/* read any threshold */

#define HARRIER_OTOF_RXS_MASK		(3<<8)	/* read any size mask */
#define HARRIER_OTOF_RXS_64BYTES	(0<<8)	/* read size 64 bytes */
#define HARRIER_OTOF_RXS_128BYTES	(1<<8)	/* read size 128 bytes */
#define HARRIER_OTOF_RXS_256BYTES	(2<<8)	/* read size 256 bytes */

#define HARRIER_OTOF_ENA		(1<<7)	/* enable oubound translation */
#define HARRIER_OTOF_WPE		(1<<4)	/* enable write posting */
#define HARRIER_OTOF_SGE		(1<<3)	/* enable store-gather */
#define HARRIER_OTOF_RAE		(1<<2)	/* enable read-ahead */
#define HARRIER_OTOF_MEM		(1<<1)	/* memory I/O */
#define HARRIER_OTOF_IOM		(1<<0)	/* I/O mode */



/* HARRIER_PASSIVE_SLAVEATTRIBUTE_REG  (PSAT) XCSR+$24F */

#define HARRIER_PSAT_ENA		(1<<7)	/* enable passive translation */
#define HARRIER_PSAT_SGE		(1<<3)	/* enable store-gather */



/* HARRIER_DMA_CONTROL_REG    (DCTL) XCSR+$250 */

#define HARRIER_DCTL_ABT		(1<<27)	/* DMA transaction abort */
#define HARRIER_DCTL_PAU		(1<<26)	/* DMA transaction pause */
#define HARRIER_DCTL_DGO		(1<<25)	/* DMA transaction start */

#define HARRIER_DCTL_MOD		(1<<23)	/* DMA transaction mode */

#define HARRIER_DCTL_XTH_MASK		(3<<20)	/* PowerPC throttle mask */
#define HARRIER_DCTL_XTH_256BYTES	(0<<20)	/* transfer size: 256 bytes */
#define HARRIER_DCTL_XTH_512BYTES	(1<<20)	/* transfer size: 512 bytes */
#define HARRIER_DCTL_XTH_1024BYTES	(2<<20)	/* transfer size: 1024 bytes */
#define HARRIER_DCTL_XTH_CONTINUOUS	(3<<20)	/* continous transfer */

#define HARRIER_DCTL_PBT_MASK		(7<<16)	/* PCI back-off timer mask */
#define HARRIER_DCTL_PBT_0_CLKS		(0<<16)	/* PCI back-off 0 clocks */
#define HARRIER_DCTL_PBT_32_CLKS	(1<<16)	/* PCI back-off 32 clocks */
#define HARRIER_DCTL_PBT_64_CLKS	(2<<16)	/* PCI back-off 64 clocks */
#define HARRIER_DCTL_PBT_128_CLKS	(3<<16)	/* PCI back-off 128 clocks */
#define HARRIER_DCTL_PBT_256_CLKS	(4<<16)	/* PCI back-off 256 clocks */
#define HARRIER_DCTL_PBT_512_CLKS	(5<<16)	/* PCI back-off 512 clocks */
#define HARRIER_DCTL_PBT_1024_CLKS	(6<<16)	/* PCI back-off 1024 clocks */
#define HARRIER_DCTL_PBT_2048_CLKS	(7<<16)	/* PCI back-off 2048 clocks */

#define HARRIER_DCTL_CSE		(1<<11)	/* copy-back snarfing enable */
#define HARRIER_DCTL_CRI		(1<<10)	/* C-line invalidate - LLD RD */

#define HARRIER_DCTL_GBL		(1<<8)	/* GBL_ pin for PPC LLD reads */


/* HARRIER_REGISTER_DMA_STATUS (DSTA) XCSR+$254 */

#define HARRIER_DSTA_SMA		(1<<15)	/* DMA Signalled Master Abort */
#define HARRIER_DSTA_RTA		(1<<14)	/* DMA Received Target Abort */
#define HARRIER_DSTA_MRC		(1<<13)	/* DMA Max Retry Count error */
#define HARRIER_DSTA_XBT		(1<<12)	/* DMA PPC Bus timeout */
#define HARRIER_DSTA_ABT		(1<<11)	/* DMA abort */
#define HARRIER_DSTA_PAU		(1<<10)	/* DMA pause */
#define HARRIER_DSTA_DON		(1<<9)	/* DMA done */
#define HARRIER_DSTA_BSY		(1<<8)	/* DMA busy */



/* HARRIER_DMA_SOURCEATTR_REG	 (DSAT) XCSR+$264 */

#define HARRIER_DSAT_TYP_MASK		(3<<28)	/* DMA source type mask */
#define HARRIER_DSAT_TYP_PPCBUS		(0<<28)	/* DMA source is PPC bus */
#define HARRIER_DSAT_TYP_PCIBUS		(1<<28)	/* DMA source is PCI bus */
#define HARRIER_DSAT_TYP_DATAPATTERN	(2<<28)	/* DMA source is data pattern */

#define HARRIER_DSAT_NIN		(1<<24)	/* disable source increment */
#define HARRIER_DSAT_PSZ		(1<<16)	/* data pattern transfer size */
#define HARRIER_DSAT_PRC_MASK		(0xf<<8) /* PCI Read Cmd Mask */ 
#define HARRIER_DSAT_PRC_IO		(2<<8)	/* PCI IO Read Cmd */ 
#define HARRIER_DSAT_PRC_MEM		(6<<8)	/* PCI Memory Read Cmd */ 
#define HARRIER_DSAT_PRC_MEM_MULTI	(0xc<<8) /* PCI Memory Read Multi Cmd */
#define HARRIER_DSAT_PRC_MEM_LINE	(0xe<<8) /* PCI Mem Read Line Cmd */ 
#define HARRIER_DSAT_CRI		(1<<2)	/* cache line read invalidate */
#define HARRIER_DSAT_GBL		(1<<0)	/* assert GBL_ for PPC reads */



/* HARRIER_DMA_DESTINATIONATTR_REG     (DDAT) XCSR+$26C */

#define HARRIER_DDAT_TYP_MASK		(1<<28)  /* DMA destination type */
#define HARRIER_DDAT_TYP_PPCBUS 	(0<<28)  /* DMA destination type */
#define HARRIER_DDAT_TYP_PCIBUS 	(1<<28)  /* DMA destination type */
#define HARRIER_DDAT_NIN		(1<<24)  /* disable dest increment */
#define HARRIER_DDAT_PWC_MASK		(0xf<<8) /* PCI Write Cmd Mask */ 
#define HARRIER_DDAT_PWC_IO		(3<<8)	 /* PCI IO Write Cmd */
#define HARRIER_DDAT_PWC_MEM		(7<<8)	 /* PCI Memory Write Cmd */
#define HARRIER_DDAT_PWC_MEM_INV	(0xf<<8) /* PCI Mem Write and Inv Cmd */
#define HARRIER_DDAT_CWF		(1<<1)	 /* cache line write flush */
#define HARRIER_DDAT_GBL		(1<<0)	 /* assert GBL_ for PPC writes */



/* HARRIER_DMA_NEXTLINKADDR_REG    (DNLA) XCSR+$270 */

#define HARRIER_DNLA_NLA_MASK		0xFFFFFFE0 /* next link address mask */
#define HARRIER_DNLA_LLA		(1<<0)	   /* last link address flag */

/* HARRIER_MP_GENERIC_OUTBOUNDDOORBELL_REG  (MGOD)   XCSR+$298 */

#define	HARRIER_MGOD_ODBI31		(0x1<<31)	/* MGOD DB 31 */
#define	HARRIER_MGOD_ODBI30		(0x1<<30)	/* MGOD DB 30 */
#define	HARRIER_MGOD_ODBI29		(0x1<<29)	/* MGOD DB 29 */
#define	HARRIER_MGOD_ODBI28		(0x1<<28)	/* MGOD DB 28 */
#define	HARRIER_MGOD_ODBI27		(0x1<<27)	/* MGOD DB 27 */
#define	HARRIER_MGOD_ODBI26		(0x1<<26)	/* MGOD DB 26 */
#define	HARRIER_MGOD_ODBI25		(0x1<<25)	/* MGOD DB 25 */
#define	HARRIER_MGOD_ODBI24		(0x1<<24)	/* MGOD DB 24 */
#define	HARRIER_MGOD_ODBI23		(0x1<<23)	/* MGOD DB 23 */
#define	HARRIER_MGOD_ODBI22		(0x1<<22)	/* MGOD DB 22 */
#define	HARRIER_MGOD_ODBI21		(0x1<<21)	/* MGOD DB 21 */
#define	HARRIER_MGOD_ODBI20		(0x1<<20)	/* MGOD DB 20 */
#define	HARRIER_MGOD_ODBI19		(0x1<<19)	/* MGOD DB 19 */
#define	HARRIER_MGOD_ODBI18		(0x1<<18)	/* MGOD DB 18 */
#define	HARRIER_MGOD_ODBI17		(0x1<<17)	/* MGOD DB 17 */
#define	HARRIER_MGOD_ODBI16		(0x1<<16)	/* MGOD DB 16 */
#define	HARRIER_MGOD_ODBI15		(0x1<<15)	/* MGOD DB 15 */
#define	HARRIER_MGOD_ODBI14		(0x1<<14)	/* MGOD DB 14 */
#define	HARRIER_MGOD_ODBI13		(0x1<<13)	/* MGOD DB 13 */
#define	HARRIER_MGOD_ODBI12		(0x1<<12)	/* MGOD DB 12 */
#define	HARRIER_MGOD_ODBI11		(0x1<<11)	/* MGOD DB 11 */
#define	HARRIER_MGOD_ODBI10		(0x1<<10)	/* MGOD DB 10 */
#define	HARRIER_MGOD_ODBI9		(0x1<<9)	/* MGOD DB 9 */
#define	HARRIER_MGOD_ODBI8		(0x1<<8)	/* MGOD DB 8 */
#define	HARRIER_MGOD_ODBI7		(0x1<<7)	/* MGOD DB 7 */
#define	HARRIER_MGOD_ODBI6		(0x1<<6)	/* MGOD DB 6 */
#define	HARRIER_MGOD_ODBI5		(0x1<<5)	/* MGOD DB 5 */
#define	HARRIER_MGOD_ODBI4		(0x1<<4)	/* MGOD DB 4 */
#define	HARRIER_MGOD_ODBI3		(0x1<<3)	/* MGOD DB 3 */
#define	HARRIER_MGOD_ODBI2		(0x1<<2)	/* MGOD DB 2 */
#define	HARRIER_MGOD_ODBI1		(0x1<<1)	/* MGOD DB 1 */
#define	HARRIER_MGOD_ODBI0		(0x1<<0)	/* MGOD DB 0 */


/*  HARRIER_MP_GENERIC_INBOUNDDOORBELL_REG   (MGID)   XCSR+$2A8 */

#define	HARRIER_MGID_IDBI7		(0x1<<31)	/* MGID DB 7 */
#define	HARRIER_MGID_IDBI6		(0x1<<30)	/* MGID DB 6 */
#define	HARRIER_MGID_IDBI5		(0x1<<29)	/* MGID DB 5 */
#define	HARRIER_MGID_IDBI4		(0x1<<28)	/* MGID DB 4 */
#define	HARRIER_MGID_IDBI3		(0x1<<27)	/* MGID DB 3 */
#define	HARRIER_MGID_IDBI2		(0x1<<26)	/* MGID DB 2 */
#define	HARRIER_MGID_IDBI1		(0x1<<25)	/* MGID DB 1 */
#define	HARRIER_MGID_IDBI0		(0x1<<24)	/* MGID DB 0 */

#define	HARRIER_MGID_IDBI15		(0x1<<23)	/* MGID DB 15 */
#define	HARRIER_MGID_IDBI14		(0x1<<22)	/* MGID DB 14 */
#define	HARRIER_MGID_IDBI13		(0x1<<21)	/* MGID DB 13 */
#define	HARRIER_MGID_IDBI12		(0x1<<20)	/* MGID DB 12 */
#define	HARRIER_MGID_IDBI11		(0x1<<19)	/* MGID DB 11 */
#define	HARRIER_MGID_IDBI10		(0x1<<18)	/* MGID DB 10 */
#define	HARRIER_MGID_IDBI9		(0x1<<17)	/* MGID DB 9 */
#define	HARRIER_MGID_IDBI8		(0x1<<16)	/* MGID DB 8 */

#define	HARRIER_MGID_IDBI23		(0x1<<15)	/* MGID DB 23 */
#define	HARRIER_MGID_IDBI22		(0x1<<14)	/* MGID DB 22 */
#define	HARRIER_MGID_IDBI21		(0x1<<13)	/* MGID DB 21 */
#define	HARRIER_MGID_IDBI20		(0x1<<12)	/* MGID DB 20 */
#define	HARRIER_MGID_IDBI19		(0x1<<11)	/* MGID DB 19 */
#define	HARRIER_MGID_IDBI18		(0x1<<10)	/* MGID DB 18 */
#define	HARRIER_MGID_IDBI17		(0x1<<9)	/* MGID DB 17 */
#define	HARRIER_MGID_IDBI16		(0x1<<8)	/* MGID DB 16 */

#define	HARRIER_MGID_IDBI31		(0x1<<7)	/* MGID DB 31 */
#define	HARRIER_MGID_IDBI30		(0x1<<6)	/* MGID DB 30 */
#define	HARRIER_MGID_IDBI29		(0x1<<5)	/* MGID DB 29 */
#define	HARRIER_MGID_IDBI28		(0x1<<4)	/* MGID DB 28 */
#define	HARRIER_MGID_IDBI27		(0x1<<3)	/* MGID DB 27 */
#define	HARRIER_MGID_IDBI26		(0x1<<2)	/* MGID DB 26 */
#define	HARRIER_MGID_IDBI25		(0x1<<1)	/* MGID DB 25 */
#define	HARRIER_MGID_IDBI24		(0x1<<0)	/* MGID DB 24 */


#define HARRIER_MP_GENERIC_INBOUNDDOORBELLMASK_REG (HARRIER_XCSR_BASE + 0x2B0)
#define HARRIER_XCSR_MGIDM HARRIER_MP_GENERIC_INBOUNDDOORBELLMASK_REG

#define	HARRIER_MGIDM_IDBM7_MASK	(0x1<<31)	/* MGIDM DB 7 Mask */
#define	HARRIER_MGIDM_IDBM6_MASK	(0x1<<30)	/* MGIDM DB 6 Mask */
#define	HARRIER_MGIDM_IDBM5_MASK	(0x1<<29)	/* MGIDM DB 5 Mask */
#define	HARRIER_MGIDM_IDBM4_MASK	(0x1<<28)	/* MGIDM DB 4 Mask */
#define	HARRIER_MGIDM_IDBM3_MASK	(0x1<<27)	/* MGIDM DB 3 Mask */
#define	HARRIER_MGIDM_IDBM2_MASK	(0x1<<26)	/* MGIDM DB 2 Mask */
#define	HARRIER_MGIDM_IDBM1_MASK	(0x1<<25)	/* MGIDM DB 1 Mask */
#define	HARRIER_MGIDM_IDBM0_MASK	(0x1<<24)	/* MGIDM DB 0 Mask */

#define	HARRIER_MGIDM_IDBM15_MASK	(0x1<<23)	/* MGIDM DB 15 Mask */
#define	HARRIER_MGIDM_IDBM14_MASK	(0x1<<22)	/* MGIDM DB 14 Mask */
#define	HARRIER_MGIDM_IDBM13_MASK	(0x1<<21)	/* MGIDM DB 13 Mask */
#define	HARRIER_MGIDM_IDBM12_MASK	(0x1<<20)	/* MGIDM DB 12 Mask */
#define	HARRIER_MGIDM_IDBM11_MASK	(0x1<<19)	/* MGIDM DB 11 Mask */
#define	HARRIER_MGIDM_IDBM10_MASK	(0x1<<18)	/* MGIDM DB 10 Mask */
#define	HARRIER_MGIDM_IDBM9_MASK	(0x1<<17)	/* MGIDM DB 9 Mask */
#define	HARRIER_MGIDM_IDBM8_MASK	(0x1<<16)	/* MGIDM DB 8 Mask */

#define	HARRIER_MGIDM_IDBM23_MASK	(0x1<<15)	/* MGIDM DB 23 Mask */
#define	HARRIER_MGIDM_IDBM22_MASK	(0x1<<14)	/* MGIDM DB 22 Mask */
#define	HARRIER_MGIDM_IDBM21_MASK	(0x1<<13)	/* MGIDM DB 21 Mask */
#define	HARRIER_MGIDM_IDBM20_MASK	(0x1<<12)	/* MGIDM DB 20 Mask */
#define	HARRIER_MGIDM_IDBM19_MASK	(0x1<<11)	/* MGIDM DB 19 Mask */
#define	HARRIER_MGIDM_IDBM18_MASK	(0x1<<10)	/* MGIDM DB 18 Mask */
#define	HARRIER_MGIDM_IDBM17_MASK	(0x1<<9)	/* MGIDM DB 17 Mask */
#define	HARRIER_MGIDM_IDBM16_MASK	(0x1<<8)	/* MGIDM DB 16 Mask */

#define	HARRIER_MGIDM_IDBM31_MASK	(0x1<<7)	/* MGIDM DB 31 Mask */
#define	HARRIER_MGIDM_IDBM30_MASK	(0x1<<6)	/* MGIDM DB 30 Mask */
#define	HARRIER_MGIDM_IDBM29_MASK	(0x1<<5)	/* MGIDM DB 29 Mask */
#define	HARRIER_MGIDM_IDBM28_MASK	(0x1<<4)	/* MGIDM DB 28 Mask */
#define	HARRIER_MGIDM_IDBM27_MASK	(0x1<<3)	/* MGIDM DB 27 Mask */
#define	HARRIER_MGIDM_IDBM26_MASK	(0x1<<2)	/* MGIDM DB 26 Mask */
#define	HARRIER_MGIDM_IDBM25_MASK	(0x1<<1)	/* MGIDM DB 25 Mask */
#define	HARRIER_MGIDM_IDBM24_MASK	(0x1<<0)	/* MGIDM DB 24 Mask */

#define HARRIER_MI2OHT_PTR_MASK		(16<<2)


/* HARRIER_MP_I2O_CONTROL_REG	(MICT) XCSR+$2E0 */

#define HARRIER_MICT_ENA		(1<<8) /* Enable I2O MP */
#define HARRIER_MICT_QSZ_MASK		(7<<0) /* Queue Size Mask */
#define HARRIER_MICT_QSZ_2K		(0<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_4K		(1<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_8K		(2<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_16K		(3<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_32K		(4<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_64K		(5<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_128K		(6<<0) /* Queue Size */
#define HARRIER_MICT_QSZ_256K		(7<<0) /* Queue Size */


/* HARRIER_MP_I2O_QUEUEBASE_REG   (MIQB) XCSR+$2E4 */

#define HARRIER_MIQB_QBA_MASK		(0xfff<<20) /* Queue Base Addr Mask */


/*
 * HARRIER_PHB_VENDORID_REG    (VENI) XCSR+$300
 * HARRIER_PHB_DEVICEID_REG    (DEVI) XCSR+$302
 */

#define HARRIER_VENI_MASK		(0xffff<<16)	/* vendor ID */
#define HARRIER_DEVI_MASK		(0xffff<<0)	/* device ID */


/* HARRIER_PHB_COMMAND_REG     (CMMD) XCSR+$304 */

#define HARRIER_CMMD_MASK		(0xffff<<16)	/* CMMD MASK */

#define HARRIER_CMMD_PERR		(1<<14)	/* PCI parity error check */
#define HARRIER_CMMD_MTSR		(1<<10)	/* PCI bus master enable */
#define HARRIER_CMMD_MEMSP		(1<<9)	/* PCI memory space enable */
#define HARRIER_CMMD_IOSP		(1<<8)	/* I/O Space Enable */
#define HARRIER_CMMD_SERR		(1<<0)	/* enable SERR_ */



/* HARRIER_PHB_STATUS_REG    (STAT) XCSR+$306 */

#define HARRIER_STAT_MASK		(0xffff<<0)	/* STAT MASK */

#define HARRIER_STAT_FAST		(1<<15)	/* fast back-to-back capable */
#define HARRIER_STAT_P66M		(1<<13)	/* PCI bus 66Mhz capable */
#define HARRIER_STAT_RCVPE		(1<<7)	/* parity error detected */
#define HARRIER_STAT_SIGSE		(1<<6)	/* signaled system error */
#define HARRIER_STAT_RCVMA		(1<<5)	/* received master abort */
#define HARRIER_STAT_RCVTA		(1<<4)	/* received target abort */
#define HARRIER_STAT_SIGTA		(1<<3)	/* signalled target abort */
#define HARRIER_STAT_SELTIM1		(1<<2)	/* DEVSEL timing field */
#define HARRIER_STAT_SELTIM0		(1<<1)	/* DEVSEL timing field */
#define HARRIER_STAT_DPAR		(1<<0)	/* data parity error detected */


/* HARRIER_PHB_REVISIONANDCLASS_REG (REVI)	XCSR+$308 */

#define HARRIER_REVI_MASK		(0xff<<24)	/* REVI Mask */
#define HARRIER_PRGIF_MASK		(0xff<<16)	/* PRGIF Mask */
#define HARRIER_SUBCL_MASK		(0xff<<8)	/* SUBCL Mask */
#define HARRIER_CLASS_MASK		(0xff<<0)	/* CLASS Mask */


/*
 * HARRIER_MSG_PASSING_REGGROUPBASEADDR_REG (MPBAR) XCSR+$310
 *
 *	NOTE:	Use the PCI Config Space Header Bit Definitions (PCFS)
 *		because the PowerPC Bus point of view fragments the
 *		"BASE" field into two separate fields.  However, issuing
 *		PCI config cycles is not desireable.  Therefore, it is
 *		recommended that the corresponding XCSR register be
 *		accessed, but the data needs to be BYTE SWAPPED after
 *		reads and before writes so that the PCFS definitions can
 *		be applied.
 */

/*
 * HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0 (ITBAR0) XCSR+$314
 * HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_1 (ITBAR1) XCSR+$318
 * HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_2 (ITBAR2) XCSR+$31c
 * HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_3 (ITBAR3) XCSR+$320
 *
 *	NOTE:	Use the PCI Config Space Header Bit Definitions (PCFS)
 *		because the PowerPC Bus point of view fragments the
 *		"BASE" field into two separate fields.  However, issuing
 *		PCI config cycles is not desireable.  Therefore, it is
 *		recommended that the corresponding XCSR register be
 *		accessed, but the data needs to be BYTE SWAPPED after
 *		reads and before writes so that the PCFS definitions can
 *		be applied.
 */


/*
 * HARRIER_SUBSYSTEM_ID_REG	  (SUBV)  XCSR+$32C
 * HARRIER_SUBSYSTEM_VENDORID_REG (SUBI)  XCSR+$32E
 */

#define HARRIER_SUBV_MASK		(0xffff<<16)	/* SUBV Mask */
#define HARRIER_SUBI_MASK		(0xffff<<0)	/* SUBI Mask */


/*
 * HARRIER_INTERRUPT_LINE_REG  (INTL)	   XCSR+$33C
 * HARRIER_INTERRUPT_PIN_REG   (INTP)	   XCSR+$33D
 * HARRIER_MINIMUM_GRANT_REG   (MNGN)	   XCSR+$33E
 * HARRIER_MAXIMUM_LATENCY_REG (MXLA)	   XCSR+$33F
 */

#define HARRIER_INTL_MASK		(0xff<<24)	/* INTL Mask */
#define HARRIER_INTP_MASK		(0xff<<16)	/* INTP Mask */
#define HARRIER_MNGN_MASK		(0xff<<8)	/* MNGN Mask */
#define HARRIER_MXLA_MASK		(0xff<<0)	/* MXLA Mask */

#define HARRIER_INTP_1                  (1 << 16)	/* INTP = 1 */


/* HARRIER_MSG_PASSINGATTR_REG	(MPAT) XCSR+$344 */

#define HARRIER_MPAT_ENA		(1<<31)	/* enable MP */
#define HARRIER_MPAT_MEM		(1<<30)	/* located in PCI memory */
#define HARRIER_MPAT_WPE		(1<<29)	/* write-post enable */
#define HARRIER_MPAT_RAE		(1<<28)	/* read ahead enable */
#define HARRIER_MPAT_GBL		(1<<8)	/* assert GBL_ */


/*
 * HARRIER_INBOUND_TRANSLATION_SIZE_0_REG  (ITAT0) XCSR+$348
 * HARRIER_INBOUND_TRANSLATION_SIZE_1_REG  (ITAT1) XCSR+$350
 * HARRIER_INBOUND_TRANSLATION_SIZE_2_REG  (ITAT2) XCSR+$358
 * HARRIER_INBOUND_TRANSLATION_SIZE_3_REG  (ITAT3) XCSR+$360
 */

#define HARRIER_ITSZX_MASK		(0xff<<24) /* ITSZ Mask */
#define HARRIER_ITOFX_MASK		(0xffff<<0) /* ITOF Mask */

#define HARRIER_ITAT_ENA		(1<<31)	/* read & write enable */
#define HARRIER_ITAT_MEM		(1<<30)	/* map to PCI memory space */
#define HARRIER_ITAT_WPE		(1<<29)	/* write post enable */
#define HARRIER_ITAT_RAE		(1<<28)	/* read-ahead enable */
#define HARRIER_ITAT_PRE		(1<<27)	/* prefetch enable */

#define HARRIER_ITAT_RMT		(1<<22)	/* read multiple threshold */
#define HARRIER_ITAT_RMS_MASK		(3<<20)	/* read multiple size mask */
#define HARRIER_ITAT_RMS_64BYTES	(0<<20)	/* FIFO size 64 bytes */
#define HARRIER_ITAT_RMS_128BYTES	(1<<20)	/* FIFO size 128 bytes */
#define HARRIER_ITAT_RMS_256BYTES	(2<<20)	/* FIFO size 256 bytes */
#define HARRIER_ITAT_RXT		(1<<18)	/* read any threshold */
#define HARRIER_ITAT_RXS_MASK		(3<<16)	/* read any size mask */
#define HARRIER_ITAT_RXS_64BYTES	(0<<16)	/* FIFO size 64 bytes */
#define HARRIER_ITAT_RXS_128BYTES	(1<<16)	/* FIFO size 128 bytes */
#define HARRIER_ITAT_RXS_256BYTES	(2<<16)	/* FIFO size 256 bytes */

#define HARRIER_ITAT_AWL		(1<<11)	/* atomic with lock */
#define HARRIER_ITAT_CRI		(1<<10)	/* cache line read-invalidate */
#define HARRIER_ITAT_CWF		(1<<9)	/* cache line write flush */
#define HARRIER_ITAT_GBL		(1<<8)	/* assert GBL_ */



/* HARRIER_PCI_STATUS_REG	(PSTA) XCSR+$383 */

#define HARRIER_PSTA_MASK		(0xff<<24)	/* PSTA Mask */
#define HARRIER_PSTA_LBA		(1<<7)		/* Loop Back Access */


/*  The following are Bit Definitions for the PCI Config Space Header (PCFS) */

/*
 * HARRIER_VENDOR_ID_REG   (VENI) PCFS+$00
 * HARRIER_DEVICE_ID_REG   (DEVI) PCFS+$02
 */

#define HARRIER_PCFS_VENI_MASK		(0xffff<<0)	/* vendor ID */
#define HARRIER_PCFS_DEVI_MASK		(0xffff<<16)	/* device ID */



/* HARRIER_REGISTER_COMMAND (CMMD) PCFS+$04 */

#define HARRIER_PCFS_CMMD_MASK		(0xffff<<0)	/* Command Mask */

#define HARRIER_PCFS_CMMD_SERR		(1<<8)	/* enable SERR_ */
#define HARRIER_PCFS_CMMD_MEMSP		(1<<6)	/* PCI memory space enable */
#define HARRIER_PCFS_CMMD_MTSR		(1<<2)	/* PCI bus master enable */
#define HARRIER_PCFS_CMMD_PERR		(1<<1)	/* PCI parity error check */



/* HARRIER_REGISTER_STATUS (STAT) PCFS+$06 */

#define HARRIER_PCFS_STAT_MASK		(0xffff<<16)	/* Status Mask */

#define HARRIER_PCFS_STAT_RCVPE		(1<<15)	/* parity error detected */
#define HARRIER_PCFS_STAT_SIGSE		(1<<14)	/* signaled system error */
#define HARRIER_PCFS_STAT_RCVMA		(1<<13)	/* received master abort */
#define HARRIER_PCFS_STAT_RCVTA		(1<<12)	/* received target abort */
#define HARRIER_PCFS_STAT_SIGTA		(1<<11)	/* signalled target abort */
#define HARRIER_PCFS_STAT_SELTIM1	(1<<10)	/* DEVSEL timing field */
#define HARRIER_PCFS_STAT_SELTIM0	(1<<9)	/* DEVSEL timing field */
#define HARRIER_PCFS_STAT_DPAR		(1<<8)	/* data parity error detected */

#define HARRIER_PCFS_STAT_FAST		(1<<7)	/* fast back-to-back capable */
#define HARRIER_PCFS_STAT_P66M		(1<<5)	/* PCI bus 66Mhz capable */


/*
 * HARRIER_REGISTER_REVISION_ID (REVI)		PCFS+$08
 * HARRIER_REGISTER_PROGRAM_INTERFACE (PRGIF)	PCFS+$09
 * HARRIER_REGISTER_SUB_CLASS_CODE (SUBCL)	PCFS+$0A
 * HARRIER_REGISTER_CLASS_CODE (CLASS)		PCFS+$0B
 */

#define HARRIER_PCFS_REVI_MASK		(0xff<<0)	/* REVI Mask */
#define HARRIER_PCFS_PRGIF_MASK		(0xff<<8)	/* PRGIF Mask */
#define HARRIER_PCFS_SUBCL_MASK		(0xff<<16)	/* SUBCL Mask */
#define HARRIER_PCFS_CLASS_MASK		(0xff<<24)	/* CLASS Mask */


/*
 * HARRIER_REGISTER_CACHE_LINE_SIZE (CLSZ) PCFS+$0C
 * HARRIER_REGISTER_MASTER_LATENCY_TIMER (MLAT) PCFS+$0D
 * HARRIER_REGISTER_HEADER_TYPE (HEAD) PCFS+$0E
 */

#define HARRIER_PCFS_CLSZ_MASK		(0xff<<0)	/* CLSZ Mask */
#define HARRIER_PCFS_MLAT_MASK		(0xff<<8)	/* MLAT Mask */
#define HARRIER_PCFS_HEAD_MASK		(0xff<<16)	/* HEAD Mask */



/* HARRIER_REGISTER_MESSAGE_PASSING_GROUP_BASE_ADDRESS (MPBAR) PCFS+$10 */

#define HARRIER_PCFS_MPBAR_BASE_MASK	0xFFFFF000 /* mem space base addr msk */
#define HARRIER_PCFS_MPBAR_PRE		(1<<3)	/* hardwired: no prefetching */
#define HARRIER_PCFS_MPBAR_MTYP1	(1<<2)	/* memory type field */
#define HARRIER_PCFS_MPBAR_MTYP0	(1<<1)	/* memory type field */
#define HARRIER_PCFS_MPBAR_IOMEM	(1<<0)	/* hardwired: PCI mem space */


/*
 * HARRIER_REGISTER_INBOUND_TRANSLATION_BASE_ADDRESS_0 (ITBAR0) PCFS+$14
 * HARRIER_REGISTER_INBOUND_TRANSLATION_BASE_ADDRESS_1 (ITBAR1) PCFS+$18
 * HARRIER_REGISTER_INBOUND_TRANSLATION_BASE_ADDRESS_2 (ITBAR2) PCFS+$1c
 * HARRIER_REGISTER_INBOUND_TRANSLATION_BASE_ADDRESS_3 (ITBAR3) PCFS+$20
 */

#define HARRIER_PCFS_ITBAR_BASE_MASK	0xFFFFF000 /* mem space base addr msk */
#define HARRIER_PCFS_ITBAR_PRE		(1<<3)	/* prefetch supported */
#define HARRIER_PCFS_ITBAR_MTYP1	(1<<2)	/* memory type field */
#define HARRIER_PCFS_ITBAR_MTYP0	(1<<1)	/* memory type field */
#define HARRIER_PCFS_ITBAR_IOMEM	(1<<0)	/* I/O space flag */


/*
 * HARRIER_REGISTER_SUBSYSTEM_ID (SUBI)		PFCS+$2C
 * HARRIER_REGISTER_SUBSYSTEM_VENDOR_ID (SUBV)	PFCS+$2E
 */
#define HARRIER_PCFS_SUBI_MASK		(0xffff<<0)   /* SUBI Mask */
#define HARRIER_PCFS_SUBV_MASK		(0xffff<<16)  /* SUBV Maks */


/*
 * HARRIER_REGISTER_INTERRUPT_LINE (INTL)	PCFS+$3C
 * HARRIER_REGISTER_INTERRUPT_PIN (INTP)	PCFS+$3D
 * HARRIER_REGISTER_MINIMUM_GRANT (MNGN)	PCFS+$3E
 * HARRIER_REGISTER_MAXIMUM_LATENCY (MXLA)	PCFS+$3F
 */

#define HARRIER_PCFS_INTL_MASK		(0xff<<0)   /* Interrupt Line Mask */
#define HARRIER_PCFS_INTP_MASK		(0xff<<8)   /* Interrupt Pin Mask */
#define HARRIER_PCFS_MNGN_MASK		(0xff<<16)   /* Minimum Grant Mask */
#define HARRIER_PCFS_MXLA_MASK		(0xff<<24)   /* Maximum Latency Mask */



/* HARRIER_REGISTER_MESSAGE_PASSING_ATTRIBUTE (MPAT) PCFS+$44 */

#define HARRIER_PCFS_MPAT_MASK		(0xffffff<<0)	/* MPAT Mask */
#define HARRIER_PCFS_MPAT_GBL		(1<<16)	/* assert GBL_ */
#define HARRIER_PCFS_MPAT_ENA		(1<<7)	/* enable MP */
#define HARRIER_PCFS_MPAT_MEM		(1<<6)	/* located in PCI memory */
#define HARRIER_PCFS_MPAT_WPE		(1<<5)	/* write-post enable */
#define HARRIER_PCFS_MPAT_RAE		(1<<4)	/* read ahead enable */


/*
 * HARRIER_REGISTER_INBOUND_TRANSLATION_SIZE_0 (ITAT0) PCFS+$48
 * HARRIER_REGISTER_INBOUND_TRANSLATION_SIZE_1 (ITAT1) PCFS+$50
 * HARRIER_REGISTER_INBOUND_TRANSLATION_SIZE_2 (ITAT2) PCFS+$58
 * HARRIER_REGISTER_INBOUND_TRANSLATION_SIZE_3 (ITAT3) PCFS+$60
 */

#define HARRIER_PCFS_ITSZX_MASK		(0xff<<0)  /* ITSZ Mask */


/*
 * HARRIER_REGISTER_INBOUND_TRANSLATION_OFFSET_0 (ITOF0) PCFS+$4a
 * HARRIER_REGISTER_INBOUND_TRANSLATION_OFFSET_1 (ITOF1) PCFS+$52
 * HARRIER_REGISTER_INBOUND_TRANSLATION_OFFSET_2 (ITOF2) PCFS+$58
 * HARRIER_REGISTER_INBOUND_TRANSLATION_OFFSET_3 (ITOF3) PCFS+$62
 */

#define HARRIER_PCFS_ITOFX_MASK		(0xffff<<16) /* ITOF Mask */


/*
 * HARRIER_REGISTER_INBOUND_TRANSLATION_ATTRIBUTE_0 (ITAT0) PCFS+$4C
 * HARRIER_REGISTER_INBOUND_TRANSLATION_ATTRIBUTE_1 (ITAT1) PCFS+$54
 * HARRIER_REGISTER_INBOUND_TRANSLATION_ATTRIBUTE_2 (ITAT2) PCFS+$5C
 * HARRIER_REGISTER_INBOUND_TRANSLATION_ATTRIBUTE_3 (ITAT3) PCFS+$64
 */

#define HARRIER_PCFS_ITAT_MASK		(0xffffff<<0)	/* ITAT Mask */
#define HARRIER_PCFS_ITAT_AWL		(1<<19)	/* atomic with lock */
#define HARRIER_PCFS_ITAT_CRI		(1<<18)	/* cache line read-invalidate */
#define HARRIER_PCFS_ITAT_CWF		(1<<17)	/* cache line write flush */
#define HARRIER_PCFS_ITAT_GBL		(1<<16)	/* assert GBL_ */

#define HARRIER_PCFS_ITAT_RMT		(1<<14)	/* read multiple threshold */
#define HARRIER_PCFS_ITAT_RMS_MASK	(3<<12)	/* read multiple size mask */
#define HARRIER_PCFS_ITAT_RMS_64BYTES	(0<<12)	/* FIFO size 64 bytes */
#define HARRIER_PCFS_ITAT_RMS_128BYTES	(1<<12)	/* FIFO size 128 bytes */
#define HARRIER_PCFS_ITAT_RMS_256BYTES	(2<<12)	/* FIFO size 256 bytes */

#define HARRIER_PCFS_ITAT_RXT		(1<<10)	/* read any threshold */
#define HARRIER_PCFS_ITAT_RXS_MASK	(3<<8)	/* read any size mask */
#define HARRIER_PCFS_ITAT_RXS_64BYTES	(0<<8)	/* FIFO size 64 bytes */
#define HARRIER_PCFS_ITAT_RXS_128BYTES	(1<<8)	/* FIFO size 128 bytes */
#define HARRIER_PCFS_ITAT_RXS_256BYTES	(2<<8)	/* FIFO size 256 bytes */

#define HARRIER_PCFS_ITAT_ENA		(1<<7)	/* read & write enable */
#define HARRIER_PCFS_ITAT_MEM		(1<<6)	/* map to PCI memory space */
#define HARRIER_PCFS_ITAT_WPE		(1<<5)	/* write post enable */
#define HARRIER_PCFS_ITAT_RAE		(1<<4)	/* read-ahead enable */
#define HARRIER_PCFS_ITAT_PRE		(1<<3)	/* prefetch enable */

#define HARRIER_PCFS_OTAT_ENA		(1<<7)	/* read & write enable */
#define HARRIER_PCFS_OTAT_WPE		(1<<4)	/* write post enable */
#define HARRIER_PCFS_OTAT_RAE		(1<<2)	/* read-ahead enable */
#define HARRIER_PCFS_OTAT_MEM		(1<<1)	/* map to PCI memory space */
#define HARRIER_PCFS_OTAT_IO		(1<<0)	/* map to PCI I/O space */
#define HARRIER_PCFS_OTAT_ENA_MASK      0x00ff  /* read & write enable byte mask */


/* HARRIER_REGISTER_PCI_STATUS (PSTA) PCFS+$83 */

#define HARRIER_PCFS_PSTA_MASK		(0xff<<24)	/* PCI Status Mask */
#define HARRIER_PCFS_PSTA_LBA		(1<<7)		/* Loop Back Access */



/*  The following are Bit Definitions for the PCI Memory Space (PMEP) */

/*  MP_I2O_INTERRUPT_STATUS		MIST	(PMEP + $030) */

#define	HARRIER_PMEP_MIST_OPI		(1<<3)	/* OB Post_list Intr */


/*  MP_I2O_INTERRUPT_MASK		MIMS	(PMEP + $034) */

#define	HARRIER_PMEP_MIMS_OPIM		(1<<3)	/* OB Post_list Intr Mask */


/*  MP_GENERIC_OUTBOUND_DOORBELL	MGOD		(PMEP + $108) */

#define	HARRIER_PMEP_MGOD_ODBM24	(0x1<<0)	/* MGOD DB 24 */
#define	HARRIER_PMEP_MGOD_ODBM25	(0x1<<1)	/* MGOD DB 25 */
#define	HARRIER_PMEP_MGOD_ODBM26	(0x1<<2)	/* MGOD DB 26 */
#define	HARRIER_PMEP_MGOD_ODBM27	(0x1<<3)	/* MGOD DB 27 */
#define	HARRIER_PMEP_MGOD_ODBM28	(0x1<<4)	/* MGOD DB 28 */
#define	HARRIER_PMEP_MGOD_ODBM29	(0x1<<5)	/* MGOD DB 29 */
#define	HARRIER_PMEP_MGOD_ODBM30	(0x1<<6)	/* MGOD DB 30 */
#define	HARRIER_PMEP_MGOD_ODBM31	(0x1<<7)	/* MGOD DB 31 */

#define	HARRIER_PMEP_MGOD_ODBM16	(0x1<<8)	/* MGOD DB 16 */
#define	HARRIER_PMEP_MGOD_ODBM17	(0x1<<9)	/* MGOD DB 17 */
#define	HARRIER_PMEP_MGOD_ODBM18	(0x1<<10)	/* MGOD DB 18 */
#define	HARRIER_PMEP_MGOD_ODBM19	(0x1<<11)	/* MGOD DB 19 */
#define	HARRIER_PMEP_MGOD_ODBM20	(0x1<<12)	/* MGOD DB 20 */
#define	HARRIER_PMEP_MGOD_ODBM21	(0x1<<13)	/* MGOD DB 21 */
#define	HARRIER_PMEP_MGOD_ODBM22	(0x1<<14)	/* MGOD DB 22 */
#define	HARRIER_PMEP_MGOD_ODBM23	(0x1<<15)	/* MGOD DB 23 */

#define	HARRIER_PMEP_MGOD_ODBM8		(0x1<<16)	/* MGOD DB 8 */
#define	HARRIER_PMEP_MGOD_ODBM9		(0x1<<17)	/* MGOD DB 9 */
#define	HARRIER_PMEP_MGOD_ODBM10	(0x1<<18)	/* MGOD DB 10 */
#define	HARRIER_PMEP_MGOD_ODBM11	(0x1<<19)	/* MGOD DB 11 */
#define	HARRIER_PMEP_MGOD_ODBM12	(0x1<<20)	/* MGOD DB 12 */
#define	HARRIER_PMEP_MGOD_ODBM13	(0x1<<21)	/* MGOD DB 13 */
#define	HARRIER_PMEP_MGOD_ODBM14	(0x1<<22)	/* MGOD DB 14 */
#define	HARRIER_PMEP_MGOD_ODBM15	(0x1<<23)	/* MGOD DB 15 */

#define	HARRIER_PMEP_MGOD_ODBM0		(0x1<<24)	/* MGOD DB 0 */
#define	HARRIER_PMEP_MGOD_ODBM1		(0x1<<25)	/* MGOD DB 1 */
#define	HARRIER_PMEP_MGOD_ODBM2		(0x1<<26)	/* MGOD DB 2 */
#define	HARRIER_PMEP_MGOD_ODBM3		(0x1<<27)	/* MGOD DB 3 */
#define	HARRIER_PMEP_MGOD_ODBM4		(0x1<<28)	/* MGOD DB 4 */
#define	HARRIER_PMEP_MGOD_ODBM5		(0x1<<29)	/* MGOD DB 5 */
#define	HARRIER_PMEP_MGOD_ODBM6		(0x1<<30)	/* MGOD DB 6 */
#define	HARRIER_PMEP_MGOD_ODBM7		(0x1<<31)	/* MGOD DB 7 */


#define HARRIER_PMEP_MGID_OFFSET (0x118) /* MP_GENERIC_INBOUND_DOORBELL */

#define	HARRIER_PMEP_MGID_IDBI0		(0x1<<0)	/* MGID Doorbell 0 */
#define	HARRIER_PMEP_MGID_IDBI1		(0x1<<1)	/* MGID Doorbell 1 */
#define	HARRIER_PMEP_MGID_IDBI2		(0x1<<2)	/* MGID Doorbell 2 */
#define	HARRIER_PMEP_MGID_IDBI3		(0x1<<3)	/* MGID Doorbell 3 */
#define	HARRIER_PMEP_MGID_IDBI4		(0x1<<4)	/* MGID Doorbell 4 */
#define	HARRIER_PMEP_MGID_IDBI5		(0x1<<5)	/* MGID Doorbell 5 */
#define	HARRIER_PMEP_MGID_IDBI6		(0x1<<6)	/* MGID Doorbell 6 */
#define	HARRIER_PMEP_MGID_IDBI7		(0x1<<7)	/* MGID Doorbell 7 */
#define	HARRIER_PMEP_MGID_IDBI8		(0x1<<8)	/* MGID Doorbell 8 */
#define	HARRIER_PMEP_MGID_IDBI9		(0x1<<9)	/* MGID Doorbell 9 */
#define	HARRIER_PMEP_MGID_IDBI10	(0x1<<10)	/* MGID Doorbell 10 */
#define	HARRIER_PMEP_MGID_IDBI11	(0x1<<11)	/* MGID Doorbell 11 */
#define	HARRIER_PMEP_MGID_IDBI12	(0x1<<12)	/* MGID Doorbell 12 */
#define	HARRIER_PMEP_MGID_IDBI13	(0x1<<13)	/* MGID Doorbell 13 */
#define	HARRIER_PMEP_MGID_IDBI14	(0x1<<14)	/* MGID Doorbell 14 */
#define	HARRIER_PMEP_MGID_IDBI15	(0x1<<15)	/* MGID Doorbell 15 */
#define	HARRIER_PMEP_MGID_IDBI16	(0x1<<16)	/* MGID Doorbell 16 */
#define	HARRIER_PMEP_MGID_IDBI17	(0x1<<17)	/* MGID Doorbell 17 */
#define	HARRIER_PMEP_MGID_IDBI18	(0x1<<18)	/* MGID Doorbell 18 */
#define	HARRIER_PMEP_MGID_IDBI19	(0x1<<19)	/* MGID Doorbell 19 */
#define	HARRIER_PMEP_MGID_IDBI20	(0x1<<20)	/* MGID Doorbell 20 */
#define	HARRIER_PMEP_MGID_IDBI21	(0x1<<21)	/* MGID Doorbell 21 */
#define	HARRIER_PMEP_MGID_IDBI22	(0x1<<22)	/* MGID Doorbell 22 */
#define	HARRIER_PMEP_MGID_IDBI23	(0x1<<23)	/* MGID Doorbell 23 */
#define	HARRIER_PMEP_MGID_IDBI24	(0x1<<24)	/* MGID Doorbell 24 */
#define	HARRIER_PMEP_MGID_IDBI25	(0x1<<25)	/* MGID Doorbell 25 */
#define	HARRIER_PMEP_MGID_IDBI26	(0x1<<26)	/* MGID Doorbell 26 */
#define	HARRIER_PMEP_MGID_IDBI27	(0x1<<27)	/* MGID Doorbell 27 */
#define	HARRIER_PMEP_MGID_IDBI28	(0x1<<28)	/* MGID Doorbell 28 */
#define	HARRIER_PMEP_MGID_IDBI29	(0x1<<29)	/* MGID Doorbell 29 */
#define	HARRIER_PMEP_MGID_IDBI30	(0x1<<30)	/* MGID Doorbell 30 */
#define	HARRIER_PMEP_MGID_IDBI31	(0x1<<31)	/* MGID Doorbell 31 */


/*  MP_GENERIC_INTERRUPT_STATUS 	MGST	(PMEP + $120) */

#define	HARRIER_PMEP_MGST_OMI0		(0x1<<4)   /* OB Msg Interrupt 0 */
#define	HARRIER_PMEP_MGST_OMI1		(0x1<<5)   /* OB Msg Interrupt 1 */


/*  MP_GENERIC_INTERRUPT_MASK		MGMS	(PMEP + $124) */

#define	HARRIER_PMEP_MGMS_OMIM0		(0x1<<4)   /* OB Msg Interrupt 0 Mask */
#define	HARRIER_PMEP_MGMS_OMIM1		(0x1<<5)   /* OB Msg Interrupt 1 Mask */


/*  MP_GENERIC_OUTBOUND_DOORBELL_MASK	MGODM		(PMEP + $128) */

#define	HARRIER_PMEP_MGODM_ODBM24_MASK	(0x1<<0)	/* MGODM DB 24 */
#define	HARRIER_PMEP_MGODM_ODBM25_MASK	(0x1<<1)	/* MGODM DB 25 */
#define	HARRIER_PMEP_MGODM_ODBM26_MASK	(0x1<<2)	/* MGODM DB 26 */
#define	HARRIER_PMEP_MGODM_ODBM27_MASK	(0x1<<3)	/* MGODM DB 27 */
#define	HARRIER_PMEP_MGODM_ODBM28_MASK	(0x1<<4)	/* MGODM DB 28 */
#define	HARRIER_PMEP_MGODM_ODBM29_MASK	(0x1<<5)	/* MGODM DB 29 */
#define	HARRIER_PMEP_MGODM_ODBM30_MASK	(0x1<<6)	/* MGODM DB 30 */
#define	HARRIER_PMEP_MGODM_ODBM31_MASK	(0x1<<7)	/* MGODM DB 31 */

#define	HARRIER_PMEP_MGODM_ODBM16_MASK	(0x1<<8)	/* MGODM DB 16 */
#define	HARRIER_PMEP_MGODM_ODBM17_MASK	(0x1<<9)	/* MGODM DB 17 */
#define	HARRIER_PMEP_MGODM_ODBM18_MASK	(0x1<<10)	/* MGODM DB 18 */
#define	HARRIER_PMEP_MGODM_ODBM19_MASK	(0x1<<11)	/* MGODM DB 19 */
#define	HARRIER_PMEP_MGODM_ODBM20_MASK	(0x1<<12)	/* MGODM DB 20 */
#define	HARRIER_PMEP_MGODM_ODBM21_MASK	(0x1<<13)	/* MGODM DB 21 */
#define	HARRIER_PMEP_MGODM_ODBM22_MASK	(0x1<<14)	/* MGODM DB 22 */
#define	HARRIER_PMEP_MGODM_ODBM23_MASK	(0x1<<15)	/* MGODM DB 23 */

#define	HARRIER_PMEP_MGODM_ODBM8_MASK	(0x1<<16)	/* MGODM DB 8 */
#define	HARRIER_PMEP_MGODM_ODBM9_MASK	(0x1<<17)	/* MGODM DB 9 */
#define	HARRIER_PMEP_MGODM_ODBM10_MASK	(0x1<<18)	/* MGODM DB 10 */
#define	HARRIER_PMEP_MGODM_ODBM11_MASK	(0x1<<19)	/* MGODM DB 11 */
#define	HARRIER_PMEP_MGODM_ODBM12_MASK	(0x1<<20)	/* MGODM DB 12 */
#define	HARRIER_PMEP_MGODM_ODBM13_MASK	(0x1<<21)	/* MGODM DB 13 */
#define	HARRIER_PMEP_MGODM_ODBM14_MASK	(0x1<<22)	/* MGODM DB 14 */
#define	HARRIER_PMEP_MGODM_ODBM15_MASK	(0x1<<23)	/* MGODM DB 15 */

#define	HARRIER_PMEP_MGODM_ODBM0_MASK	(0x1<<24)	/* MGODM DB 0 */
#define	HARRIER_PMEP_MGODM_ODBM1_MASK	(0x1<<25)	/* MGODM DB 1 */
#define	HARRIER_PMEP_MGODM_ODBM2_MASK	(0x1<<26)	/* MGODM DB 2 */
#define	HARRIER_PMEP_MGODM_ODBM3_MASK	(0x1<<27)	/* MGODM DB 3 */
#define	HARRIER_PMEP_MGODM_ODBM4_MASK	(0x1<<28)	/* MGODM DB 4 */
#define	HARRIER_PMEP_MGODM_ODBM5_MASK	(0x1<<29)	/* MGODM DB 5 */
#define	HARRIER_PMEP_MGODM_ODBM6_MASK	(0x1<<30)	/* MGODM DB 6 */
#define	HARRIER_PMEP_MGODM_ODBM7_MASK	(0x1<<31)	/* MGODM DB 7 */

#ifdef __cplusplus
    }
#endif

#endif /* INCharrierh */
