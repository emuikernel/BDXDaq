/* auLib.h - Au support header */

/* Copyright 2001 Wind River Systems, Inc. */

/*
 * This file has been developed or significantly modified by the
 * MIPS Center of Excellence Dedicated Engineering Staff.
 * This notice is as per the MIPS Center of Excellence Master Partner
 * Agreement, do not remove this notice without checking first with
 * WR/Platforms MIPS Center of Excellence engineering management.
 */

/*
modification history
--------------------
01m,14may02,zmm  Global au1000 name changes. SPR 77333.
01l,30apr02,zmm  Add  AU1000_GPIO_PINFUNC definition.
01k,03dec01,zmm  Add CofE comment.
01j,27nov01,zmm  Add bits of counter control register, fix __cpluscplus bug.
01i,18oct01,pes  Merge changes
01h,03oct01,pes  Remove references to PHYS_TO_K1 macro so that this file
                 becomes assembler safe.
01g,26sep01,tlc  Correct AC'97 macros.
01f,22aug01,pes  Add USB HOST register information
01e,13aug01,tlc  Add AC'97 Controller Information.
01d,13aug01,pes  Add defines needed to support PCI. Adjust addresses to make
                 their KSEG1 locations more obvious.
01c,23jul01,zmm  Added Static Bus, SDRAM, UART, and clock Controllers.
01b,19jun01,zmm  Fix AU1000_INTERRUPT_CONTROLLER1_BASE
01a,18may01,mem	 written.
*/

/*
This file contains I/O addresses and related constants for the
Alchemy Semiconductor Au1000.
*/

#ifndef __INCauh
#define __INCauh

#ifdef __cplusplus
extern "C" {
#endif	 /* __cplusplus */

#define MSK(n,m) (((1<<(n+1)) - 1) & ~((1<<(m)) - 1))

/* Interrupt controllers */

#define AU_INTERRUPT_CONTROLLER0_BASE	(0xb0400000)
#define AU_INTERRUPT_CONTROLLER1_BASE	(0xb1800000)

#define AU_INT_CTRL_REG(ctrl, reg)	\
	(*(volatile UINT32 *)(((ctrl) ? AU_INTERRUPT_CONTROLLER1_BASE \
		: AU_INTERRUPT_CONTROLLER0_BASE) + (reg)))

#define AU_INTC_CONFIG0_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x40)
#define AU_INTC_CONFIG0_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x40)
#define AU_INTC_CONFIG0_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x44)
#define AU_INTC_CONFIG1_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x48)
#define AU_INTC_CONFIG1_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x48)
#define AU_INTC_CONFIG1_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x4c)
#define AU_INTC_CONFIG2_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x50)
#define AU_INTC_CONFIG2_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x50)
#define AU_INTC_CONFIG2_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x54)
#define AU_INTC_REQUEST0_INT(ctrl)	AU_INT_CTRL_REG(ctrl, 0x54)
#define AU_INTC_SOURCE_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x58)
#define AU_INTC_SOURCE_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x58)
#define AU_INTC_SOURCE_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x5c)
#define AU_INTC_REQUEST1_INT(ctrl)	AU_INT_CTRL_REG(ctrl, 0x5c)
#define AU_INTC_ASSIGN_REQUEST_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x60)
#define AU_INTC_ASSIGN_REQUEST_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x60)
#define AU_INTC_ASSIGN_REQUEST_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x64)
#define AU_INTC_WAKEUP_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x68)
#define AU_INTC_WAKEUP_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x68)
#define AU_INTC_WAKEUP_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x6c)
#define AU_INTC_MASK_READ(ctrl)	AU_INT_CTRL_REG(ctrl, 0x70)
#define AU_INTC_MASK_SET(ctrl)	AU_INT_CTRL_REG(ctrl, 0x70)
#define AU_INTC_MASK_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x74)
#define AU_INTC_RISING_EDGE_DETECT(ctrl)	AU_INT_CTRL_REG(ctrl, 0x78)
#define AU_INTC_RISING_EDGE_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x78)
#define AU_INTC_FALLING_EDGE_DETECT(ctrl)	AU_INT_CTRL_REG(ctrl, 0x7c)
#define AU_INTC_FALLING_EDGE_CLEAR(ctrl)	AU_INT_CTRL_REG(ctrl, 0x7c)
#define AU_INTC_TEST_BIT(ctrl)	AU_INT_CTRL_REG(ctrl, 0x80)

/* SDRAM controller */

#define AU_SDRAM_CONTROLLER_BASE   (0xb4000000)
#define AU_SDRAM_REG(reg) (AU_SDRAM_CONTROLLER_BASE + (reg))

#define AU_SDRAM_CS_MODE_0          AU_SDRAM_REG(0x00)
#define AU_SDRAM_CS_MODE_1          AU_SDRAM_REG(0x04)
#define AU_SDRAM_CS_MODE_2          AU_SDRAM_REG(0x08)
#define AU_SDRAM_CS_CONFIG_0        AU_SDRAM_REG(0x0c)
#define AU_SDRAM_CS_CONFIG_1        AU_SDRAM_REG(0x10)
#define AU_SDRAM_CS_CONFIG_2        AU_SDRAM_REG(0x14)
#define AU_SDRAM_REFRESH_CONFIG     AU_SDRAM_REG(0x18)
#define AU_SDRAM_PRECHARGE_CMD      AU_SDRAM_REG(0x1c)
#define AU_SDRAM_AUTO_REFRESH_CMD   AU_SDRAM_REG(0x20)
#define AU_SDRAM_WRITE_EXTERN_0     AU_SDRAM_REG(0x24)
#define AU_SDRAM_WRITE_EXTERN_1     AU_SDRAM_REG(0x28)
#define AU_SDRAM_WRITE_EXTERN_2     AU_SDRAM_REG(0x2c)
#define AU_SDRAM_SLEEP              AU_SDRAM_REG(0x30)
#define AU_SDRAM_TOGGLE_CKE         AU_SDRAM_REG(0x34)

#define AU_SDRAM_CS_MODE_F		(1<<22)
#define AU_SDRAM_CS_MODE_SR		(1<<21)
#define AU_SDRAM_CS_MODE_BS		(1<<20)
#define AU_SDRAM_CS_MODE_RS		MSK(19,18)
#define AU_SDRAM_CS_MODE_CS		MSK(17,15)
#define AU_SDRAM_CS_MODE_TRAS	MSK(14,11)
#define AU_SDRAM_CS_MODE_TMRD	MSK(10,9)
#define AU_SDRAM_CS_MODE_TWR	MSK(8,7)
#define AU_SDRAM_CS_MODE_TRP	MSK(6,5)
#define AU_SDRAM_CS_MODE_TRCD	MSK(4,3)
#define AU_SDRAM_CS_MODE_TCL	MSK(2,0)
#define SET_SDRAM_CS_MODE_RS(x)		((x)<<18)
#define SET_SDRAM_CS_MODE_CS(x)		((x)<<15)
#define SET_SDRAM_CS_MODE_TRAS(x)	((x)<<11)
#define SET_SDRAM_CS_MODE_TMRD(x)	((x)<<9)
#define SET_SDRAM_CS_MODE_TWR(x)	((x)<<7)
#define SET_SDRAM_CS_MODE_TRP(x)	((x)<<5)
#define SET_SDRAM_CS_MODE_TRCD(x)	((x)<<3)
#define SET_SDRAM_CS_MODE_TCL(x)	(x)

#define AU_SDRAM_CS_CONFIG_E	(1<<20)
#define AU_SDRAM_CS_CONFIG_CSBA	MSK(19,10)
#define AU_SDRAM_CS_CONFIG_CSMASK	MSK(9,0)
#define SET_SDRAM_CS_CONFIG_CSBA(x)	((x)<<10)
#define SET_SDRAM_CS_CONFIG_CSMASK(x)	(x)

#define AU_SDRAM_REFRESH_CONFIG_TRC		MSK(31,28)
#define AU_SDRAM_REFRESH_CONFIG_TRPM	MSK(27,26)
#define AU_SDRAM_REFRESH_CONFIG_E		(1<<25)
#define AU_SDRAM_REFRESH_CONFIG_REF_INTER	MSK(24,0)
#define SET_SDRAM_REFRESH_CONFIG_TRC(x)		((x)<<28)
#define SET_SDRAM_REFRESH_CONFIG_TRPR(x)	((x)<<26)
#define SET_SDRAM_REFRESH_CONFIG_REF_INTER(x)	(x)

/* Static BUS controllers */

#define AU_STATIC_CONTROLLER_BASE   (0xb4001000)
#define AU_STATIC_REG(chipsel, reg) (AU_STATIC_CONTROLLER_BASE + 0x10*(chipsel) + reg)

#define AU_STATIC_CONFIG(chipsel)   AU_STATIC_REG(chipsel, 0x0)
#define AU_STATIC_TIMING(chipsel)   AU_STATIC_REG(chipsel, 0x4)
#define AU_STATIC_ADDRESS(chipsel)  AU_STATIC_REG(chipsel, 0x8)

#define AU_STATIC_CONFIG_BV 	(1 << 12)
#define AU_STATIC_CONFIG_D5 	(1 << 11)
#define AU_STATIC_CONFIG_AV 	(1 << 10)
#define AU_STATIC_CONFIG_LE         (1 << 9)
#define AU_STATIC_CONFIG_TS 	(1 << 8)
#define AU_STATIC_CONFIG_EW 	(1 << 7)
#define AU_STATIC_CONFIG_H  	(1 << 6)
#define AU_STATIC_CONFIG_BS 	(1 << 5)
#define AU_STATIC_CONFIG_PM 	(1 << 4)
#define AU_STATIC_CONFIG_RO 	(1 << 3)
#define AU_STATIC_CONFIG_DTY 	MSK(2,0)

#define AU_STATIC_TIMING_TWCS	MSK(30,28)
#define AU_STATIC_TIMING_TCSH	MSK(27,24)
#define AU_STATIC_TIMING_TWP	MSK(19,14)
#define AU_STATIC_TIMING_TCSW	MSK(13,10)
#define AU_STATIC_TIMING_TPM	MSK(9,6)
#define AU_STATIC_TIMING_TA		MSK(5,0)

#define AU_STATIC_TIMING_TMST	MSK(31,24)
#define AU_STATIC_TIMING_TMSU	MSK(23,17)
#define AU_STATIC_TIMING_TMIH	MSK(16,11)
#define AU_STATIC_TIMING_TIST	MSK(10,5)
#define AU_STATIC_TIMING_TISU	MSK(4,0)

#define AU_STATIC_ADDRESS_E         (1<<28)
#define AU_STATIC_ADDRESS_CSADDR	MSK(27,14)
#define AU_STATIC_ADDRESS_AMASK	MSK(13,0)
#define SET_STATIC_ADDRESS_CSADDR(x)  	((x)<<14)
#define SET_STATIC_ADDRESS_AMASK(x)	(x)

/* Serial devices, UART 1 - UART 4 */
#define UART0_BASE_ADR		AU_UART_BASE
#define AU_UART_BASE 		(0xb1100000)
#define AU_UART_REG(uart, reg)      (AU_UART_BASE + 0x100000*(uart) + (reg))

#define AU_UART_RXDATA(uart)		AU_UART_REG(uart,0x000)
#define AU_UART_TXDATA(uart) 		AU_UART_REG(uart,0x004)
#define AU_UART_INTERRUPT_ENABLE(uart) 	AU_UART_REG(uart,0x008)
#define AU_UART_INTERRUPT_CAUSE(uart) 	AU_UART_REG(uart,0x00C)
#define AU_UART_FIFO_CONTROL(uart) 		AU_UART_REG(uart,0x010)
#define AU_UART_LINE_CONTROL(uart) 		AU_UART_REG(uart,0x014)
#define AU_UART_MODEM_CONTROL(uart) 	AU_UART_REG(uart,0x018)
#define AU_UART_LINE_STATUS(uart) 		AU_UART_REG(uart,0x01C)
#define AU_UART_MODEM_STATUS(uart) 		AU_UART_REG(uart,0x020)
#define AU_UART_CLOCK_DIVIDER(uart) 	AU_UART_REG(uart,0x028)
#define AU_UART_MODULE_CONTROL(uart)	AU_UART_REG(uart,0x100)

/* Clock controller */

#define AU_CLOCK_BASE 			(0xb1900000)
#define AU_CLOCK_REG(reg) 			(AU_CLOCK_BASE + (reg))

#define AU_CLOCK_FREQUENCY_CONTROL0 	AU_CLOCK_REG(0x20)
#define AU_CLOCK_FREQUENCY_CONTROL1 	AU_CLOCK_REG(0x24)
#define AU_CLOCK_CLOCK_SOURCE_CONTROL 	AU_CLOCK_REG(0x28)
#define AU_CLOCK_CPU_PLL_CONTROL 		AU_CLOCK_REG(0x60)
#define AU_CLOCK_AUX_PLL_CONTROL 		AU_CLOCK_REG(0x64)

/* for use with AU_CLOCK_FREQUENCY_CONTROL0 register */
#define AU_CLOCK_FREQUENCY_CONTROL_FS0      (1<<0)
#define AU_CLOCK_FREQUENCY_CONTROL_FE0      (1<<1)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV0   MSK(9,2)

#define AU_CLOCK_FREQUENCY_CONTROL_FS1      (1<<10)  
#define AU_CLOCK_FREQUENCY_CONTROL_FE1      (1<<11)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV1   MSK(19,12)
 
#define AU_CLOCK_FREQUENCY_CONTROL_FS2      (1<<20)
#define AU_CLOCK_FREQUENCY_CONTROL_FE2      (1<<21)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV2   MSK(29,22)
 
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV0_SH 2
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV1_SH 12
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV2_SH 22
 
/* for use with AU_CLOCK_FREQUENCY_CONTROL1 register */
#define AU_CLOCK_FREQUENCY_CONTROL_FS3      (1<<0)
#define AU_CLOCK_FREQUENCY_CONTROL_FE3      (1<<1)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV3   MSK(9,2)
 
#define AU_CLOCK_FREQUENCY_CONTROL_FS4      (1<<10)
#define AU_CLOCK_FREQUENCY_CONTROL_FE4      (1<<11)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV4   MSK(19,12)
 
#define AU_CLOCK_FREQUENCY_CONTROL_FS5      (1<<20)
#define AU_CLOCK_FREQUENCY_CONTROL_FE5      (1<<21)
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV5   MSK(29,22)
 
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV3_SH 2
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV4_SH 12
#define AU_CLOCK_FREQUENCY_CONTROL_FRDIV5_SH 22

#define AU_CLOCK_SOURCE_CONTROL_CIR         (1<<0)      
#define AU_CLOCK_SOURCE_CONTROL_DIR         (1<<1)      
#define AU_CLOCK_SOURCE_CONTROL_MIR         MSK(4,2)   
#define AU_CLOCK_SOURCE_CONTROL_CUD         (1<<5)      
#define AU_CLOCK_SOURCE_CONTROL_DUD         (1<<6)     
#define AU_CLOCK_SOURCE_CONTROL_MUD         MSK(9,7)   
#define AU_CLOCK_SOURCE_CONTROL_CUH         (1<<10)   
#define AU_CLOCK_SOURCE_CONTROL_DUH         (1<<11)    
#define AU_CLOCK_SOURCE_CONTROL_MUH         MSK(14,12) 
#define AU_CLOCK_SOURCE_CONTROL_CI2         (1<<15)   
#define AU_CLOCK_SOURCE_CONTROL_DI2         (1<<16)     
#define AU_CLOCK_SOURCE_CONTROL_MI2         MSK(19,17)
#define AU_CLOCK_SOURCE_CONTROL_CE0         (1<<20)    
#define AU_CLOCK_SOURCE_CONTROL_DE0         (1<<21)   
#define AU_CLOCK_SOURCE_CONTROL_ME0         MSK(24,22)  
#define AU_CLOCK_SOURCE_CONTROL_CE1         (1<<25)   
#define AU_CLOCK_SOURCE_CONTROL_DE1         (1<<26)     
#define AU_CLOCK_SOURCE_CONTROL_ME1         MSK(29,27)  

#define AU_CLOCK_SOURCE_CONTROL_IR_SH       2
#define AU_CLOCK_SOURCE_CONTROL_UD_SH       7
#define AU_CLOCK_SOURCE_CONTROL_UH_SH       12
#define AU_CLOCK_SOURCE_CONTROL_I2_SH       17
#define AU_CLOCK_SOURCE_CONTROL_E0_SH       22
#define AU_CLOCK_SOURCE_CONTROL_E1_SH       27

#define AU_CLOCK_SOURCE_CONTROL_AUX         0x1
#define AU_CLOCK_SOURCE_CONTROL_FREQ0       0x2
#define AU_CLOCK_SOURCE_CONTROL_FREQ1       0x3
#define AU_CLOCK_SOURCE_CONTROL_FREQ2       0x4
#define AU_CLOCK_SOURCE_CONTROL_FREQ3       0x5
#define AU_CLOCK_SOURCE_CONTROL_FREQ4       0x6
#define AU_CLOCK_SOURCE_CONTROL_FREQ5       0x7

/* GPIO */
#define AU_GPIO_BASE			(0xb1900000)
#define AU_GPIO_REG(reg)			(AU_GPIO_BASE + (reg))

#define AU_GPIO_PINFUNC			AU_GPIO_REG(0x02c)
#define AU_GPIO_TRISTATE_STATE_READ		AU_GPIO_REG(0x100)
#define AU_GPIO_TRISTATE_STATE_SET		AU_GPIO_REG(0x100)
#define AU_GPIO_OUTPUT_STATE_READ		AU_GPIO_REG(0x108)
#define AU_GPIO_OUTPUT_STATE_SET		AU_GPIO_REG(0x108)
#define AU_GPIO_OUTPUT_STATE_CLEAR		AU_GPIO_REG(0x10c)
#define AU_GPIO_PIN_STATE_READ     		AU_GPIO_REG(0x110)
#define AU_GPIO_PIN_INPUT_ENABLE     	AU_GPIO_REG(0x110)
#define AU_SYS_PIN_INPUT_EN		        AU_GPIO_REG(0x110)

/* AC'97 Controller */

#define AU_AC97_BASE                        (0xb0000000)
#define AU_AC97_REG(reg)                    (AU_AC97_BASE + (reg))

#define AU_AC97_CONFIG                      AU_AC97_REG(0x00)
#define AU_AC97_STATUS                      AU_AC97_REG(0x04)
#define AU_AC97_DATA                        AU_AC97_REG(0x08)
#define AU_AC97_CMMD                        AU_AC97_REG(0x0C)
#define AU_AC97_CMMDRESP                    AU_AC97_REG(0x0C)
#define AU_AC97_ENABLE                      AU_AC97_REG(0x10)

#define AU_AC97_CONFIG_RSLOTS               MSK(22,13)
#define AU_AC97_CONFIG_RSLOT3               (1 << 13)
#define AU_AC97_CONFIG_RSLOT4               (1 << 14)
#define AU_AC97_CONFIG_RSLOT5               (1 << 15)
#define AU_AC97_CONFIG_RSLOT6               (1 << 16)
#define AU_AC97_CONFIG_RSLOT7               (1 << 17)
#define AU_AC97_CONFIG_RSLOT8               (1 << 18)
#define AU_AC97_CONFIG_RSLOT9               (1 << 19)
#define AU_AC97_CONFIG_RSLOT10              (1 << 20)
#define AU_AC97_CONFIG_RSLOT11              (1 << 21)
#define AU_AC97_CONFIG_RSLOT12              (1 << 22)

#define AU_AC97_CONFIG_TSLOTS               MSK(12,3)
#define AU_AC97_CONFIG_TSLOT3               (1 << 3)
#define AU_AC97_CONFIG_TSLOT4               (1 << 4)
#define AU_AC97_CONFIG_TSLOT5               (1 << 5)
#define AU_AC97_CONFIG_TSLOT6               (1 << 6)
#define AU_AC97_CONFIG_TSLOT7               (1 << 7)
#define AU_AC97_CONFIG_TSLOT8               (1 << 8)
#define AU_AC97_CONFIG_TSLOT9               (1 << 9)
#define AU_AC97_CONFIG_TSLOT10              (1 << 10)
#define AU_AC97_CONFIG_TSLOT11              (1 << 11)
#define AU_AC97_CONFIG_TSLOT12              (1 << 12)

#define AU_AC97_CONFIG_ACLINK_RESET         (1 << 0)

#define AU_AC97_STATUS_READY                (1 << 7)
#define AU_AC97_STATUS_CP                   (1 << 6)

#define AU_AC97_CMMD_READ                   (1 << 7)
#define AU_AC97_CMMD_INDEX                  MSK(6,0)

#define AU_AC97_ENABLE_CTRL                 (1 << 4)
#define AU_AC97_ENABLE_CLOCK                (1 << 0)

#define SET_AC97_COMMAND(reg, value)            ((value) << 16 | reg )
#define GET_AC97_COMMAND(reg)                   (reg >> 16)

/* USB Controller */
#define AU_USB_HOST_BASE			(0xb0100000)
#define AU_USB_HOST_REG(reg)		(AU_USB_HOST_BASE + (reg))

#define AU_USB_HOST_ENABLE			AU_USB_HOST_REG(0x7fffc)
/* bits in host config register */
#define AU_USB_HOST_ENABLE_RD		(1 << 4) /* Reset Done */
#define AU_USB_HOST_ENABLE_CE		(1 << 3) /* Clock Enable */
#define AU_USB_HOST_ENABLE_E		(1 << 2) /* Enable */
#define AU_USB_HOST_ENABLE_C		(1 << 1) /* Cacheable */
#define AU_USB_HOST_ENABLE_BE		(1 << 0) /* Big Endian */


#define AU_SYS_BASE			(0xb1900000)
#define AU_SYS_REG(reg)		(AU_SYS_BASE + (reg))

#define AU_SYS_TOY_TRIM		AU_SYS_REG(0x00)
#define AU_SYS_TOY_WRITE		AU_SYS_REG(0x04)
#define AU_SYS_TOY_MATCH_0		AU_SYS_REG(0x08)
#define AU_SYS_TOY_MATCH_1		AU_SYS_REG(0x0c)
#define AU_SYS_TOY_MATCH_2		AU_SYS_REG(0x10)
#define AU_SYS_COUNTER_CONTROL	AU_SYS_REG(0x14)
#define AU_SYS_SCRATCH_0		AU_SYS_REG(0x18)
#define AU_SYS_SCRATCH_1		AU_SYS_REG(0x1c)
#define AU_SYS_FREQ_CONTROL_0	AU_SYS_REG(0x20)
#define AU_SYS_FREQ_CONTROL_1	AU_SYS_REG(0x24)
#define AU_SYS_CLOCK_SOURCE		AU_SYS_REG(0x28)
#define AU_SYS_PIN_FUNCTION		AU_SYS_REG(0x2c)

#define AU_SYS_WAKE_MASK		AU_SYS_REG(0x34)
#define AU_SYS_ENDIAN		AU_SYS_REG(0x38)
#define AU_SYS_POWER_CONTROL	AU_SYS_REG(0x3c)
#define AU_SYS_TOY_READ		AU_SYS_REG(0x40)
#define AU_SYS_RTC_TRIM		AU_SYS_REG(0x44)
#define AU_SYS_RTC_WRITE		AU_SYS_REG(0x48)
#define AU_SYS_RTC_MATCH_0		AU_SYS_REG(0x4c)
#define AU_SYS_RTC_MATCH_1		AU_SYS_REG(0x50)
#define AU_SYS_RTC_MATCH_2		AU_SYS_REG(0x54)
#define AU_SYS_RTC_READ		AU_SYS_REG(0x58)
#define AU_SYS_WAKE_SOURCE		AU_SYS_REG(0x5c)
#define AU_SYS_CPU_PLL		AU_SYS_REG(0x60)
#define AU_SYS_AUX_PLL		AU_SYS_REG(0x64)

#define AU_SYS_SLEEP_POWER		AU_SYS_REG(0x78)
#define AU_SYS_SLEEP		AU_SYS_REG(0x7c)

/* bits in RTC and TOY control register */
#define AU_SYS_COUNTER_CONTROL_ERS  (1 << 23)
#define AU_SYS_COUNTER_CONTROL_RTS  (1 << 20)
#define AU_SYS_COUNTER_CONTROL_RM2  (1 << 19)
#define AU_SYS_COUNTER_CONTROL_RM1  (1 << 18)
#define AU_SYS_COUNTER_CONTROL_RM0  (1 << 17)
#define AU_SYS_COUNTER_CONTROL_RS   (1 << 16)
#define AU_SYS_COUNTER_CONTROL_BP   (1 << 14)
#define AU_SYS_COUNTER_CONTROL_REN  (1 << 13)
#define AU_SYS_COUNTER_CONTROL_BRT  (1 << 12)
#define AU_SYS_COUNTER_CONTROL_TEN  (1 << 11)
#define AU_SYS_COUNTER_CONTROL_BTT  (1 << 10)
#define AU_SYS_COUNTER_CONTROL_EO   (1 << 8)
#define AU_SYS_COUNTER_CONTROL_ETS  (1 << 7)
#define AU_SYS_COUNTER_CONTROL_32S  (1 << 5)
#define AU_SYS_COUNTER_CONTROL_TTS  (1 << 4)
#define AU_SYS_COUNTER_CONTROL_TM2  (1 << 3)
#define AU_SYS_COUNTER_CONTROL_TM1  (1 << 2)
#define AU_SYS_COUNTER_CONTROL_TM0  (1 << 1)
#define AU_SYS_COUNTER_CONTROL_TS   (1 << 0)

#ifdef __cplusplus
}
#endif	/* __cplusplus */
#endif	/* __INCauah */



