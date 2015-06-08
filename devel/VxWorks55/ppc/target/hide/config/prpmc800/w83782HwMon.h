/* w83782HwMon.h - Winbond Hardware Monitoring IC driver header */

/* Copyright 2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,03aug01,srr  Written.
*/

/*
This file contains the configuration parameters for the
Winbond W83782D Hardware Monitoring IC.
*/

#ifndef	INCw83782HwMonh
#define	INCw83782HwMonh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */


#define W83782_ADDR			0x5	/* Address Register */
#define W83782_DATA			0x6	/* Data Register */

#define W83782_POST_RAM			0x0	/* Start of POST RAM */

/* Value RAM */

#define W83782_VCOREA			0x20	/* VCOREA Reading */
#define W83782_VINR0			0x21	/* VINR0 Reading */
#define W83782_PLUS_3_3_VIN		0x22	/* +3.3 VIN Reading */
#define W83782_PLUS_5_VIN		0x23	/* +5 VIN Reading */
#define W83782_PLUS_12_VIN		0x24	/* +12 VIN Reading */
#define W83782_NEG_5_VIN		0x25	/* -5 VIN Reading */
#define W83782_NEG_12_VIN		0x26	/* -12 VIN Reading */
#define W83782_TEMP			0x27	/* Temperature Reading */
#define W83782_FAN1			0x28	/* Fan 1 Reading */
#define W83782_FAN2			0x29	/* Fan 2 Reading */
#define W83782_FAN3			0x2a	/* Fan 3 Reading */
#define W83782_VCOREA_HIGH		0x2b	/* VCOREA High Limit */
#define W83782_VCOREA_LOW		0x2c	/* VCOREA Low Limit */
#define W83782_VINR0_HIGH		0x2d	/* VINR0 High Limit */
#define W83782_VINR0_LOW		0x2e	/* VINR0 Low Limit */
#define W83782_PLUS_3_3_VIN_HIGH	0x2f	/* +3.3 VIN High Limit */
#define W83782_PLUS_3_3_VIN_LOW		0x30	/* +3.3 VIN Low Limit */
#define W83782_PLUS_5_VIN_HIGH		0x31	/* +5 VIN High Limit */
#define W83782_PLUS_5_VIN_LOW		0x32	/* +5 VIN Low Limit */
#define W83782_PLUS_12_VIN_HIGH		0x33	/* +12 VIN High Limit */
#define W83782_PLUS_12_VIN_LOW		0x34	/* +12 VIN Low Limit */
#define W83782_NEG_5_VIN_HIGH		0x35	/* -5 VIN High Limit */
#define W83782_NEG_5_VIN_LOW		0x36	/* -5 VIN Low Limit */
#define W83782_NEG_12_VIN_HIGH		0x37	/* -12 VIN High Limit */
#define W83782_NEG_12_VIN_LOW		0x38	/* -12 VIN Low Limit */
#define W83782_TEMP_HIGH		0x39	/* Temperature High Limit */
#define W83782_TEMP_LOW			0x3a	/* Temperature Low Limit */
#define W83782_FAN1_LOW			0x3b	/* Fan 1 Low Limit Count */
#define W83782_FAN2_LOW			0x3c	/* Fan 2 Low Limit Count */
#define W83782_FAN3_LOW			0x3d	/* Fan 3 Low Limit Count */

/* Auto-Increment Value RAM */

#define W83782_VCOREA_AUTO		0x60	/* VCOREA Reading */
#define W83782_VINR0_AUTO		0x61	/* VINR0 Reading */
#define W83782_PLUS_3_3_VIN_AUTO	0x62	/* +3.3 VIN Reading */
#define W83782_PLUS_5_VIN_AUTO		0x63	/* +5 VIN Reading */
#define W83782_PLUS_12_VIN_AUTO		0x64	/* +12 VIN Reading */
#define W83782_NEG_5_VIN_AUTO		0x65	/* -5 VIN Reading */
#define W83782_NEG_12_VIN_AUTO		0x66	/* -12 VIN Reading */
#define W83782_TEMP_AUTO		0x67	/* Temperature Reading */
#define W83782_FAN1_AUTO		0x68	/* Fan 1 Reading */
#define W83782_FAN2_AUTO		0x69	/* Fan 2 Reading */
#define W83782_FAN3_AUTO		0x6a	/* Fan 3 Reading */
#define W83782_VCOREA_HIGH_AUTO		0x6b	/* VCOREA High Limit */
#define W83782_VCOREA_LOW_AUTO		0x6c	/* VCOREA Low Limit */
#define W83782_VINR0_HIGH_AUTO		0x6d	/* VINR0 High Limit */
#define W83782_VINR0_LOW_AUTO		0x6e	/* VINR0 Low Limit */
#define W83782_PLUS_3_3_VIN_HIGH_AUTO	0x6f	/* +3.3 VIN High Limit */
#define W83782_PLUS_3_3_VIN_LOW_AUTO	0x70	/* +3.3 VIN Low Limit */
#define W83782_PLUS_5_VIN_HIGH_AUTO	0x71	/* +5 VIN High Limit */
#define W83782_PLUS_5_VIN_LOW_AUTO	0x72	/* +5 VIN Low Limit */
#define W83782_PLUS_12_VIN_HIGH_AUTO	0x73	/* +12 VIN High Limit */
#define W83782_PLUS_12_VIN_LOW_AUTO	0x74	/* +12 VIN Low Limit */
#define W83782_NEG_5_VIN_HIGH_AUTO	0x75	/* -5 VIN High Limit */
#define W83782_NEG_5_VIN_LOW_AUTO	0x76	/* -5 VIN Low Limit */
#define W83782_NEG_12_VIN_HIGH_AUTO	0x77	/* -12 VIN High Limit */
#define W83782_NEG_12_VIN_LOW_AUTO	0x78	/* -12 VIN Low Limit */
#define W83782_TEMP_HIGH_AUTO		0x79	/* Temperature High Limit */
#define W83782_TEMP_LOW_AUTO		0x7a	/* Temperature Low Limit */
#define W83782_FAN1_LOW_AUTO		0x7b	/* Fan 1 Low Limit Count */
#define W83782_FAN2_LOW_AUTO		0x7c	/* Fan 2 Low Limit Count */
#define W83782_FAN3_LOW_AUTO		0x7d	/* Fan 3 Low Limit Count */

/* Registers */

#define W83782_CFG			0x40	/* Configuration Register */
#define W83782_INT_STAT_1		0x41	/* Interrupt Status 1 Reg */
#define W83782_INT_STAT_2		0x42	/* Interrupt Status 2 Reg */
#define W83782_SMI_MASK_1		0x43	/* SMI Mask Register 1 */
#define W83782_SMI_MASK_2		0x44	/* SMI Mask Register 2 */
#define W83782_NMI_MASK_1		0x45	/* NMI Mask Register 1 */
#define W83782_NMI_MASK_2		0x46	/* NMI Mask Register 2 */

#define W83782_FAN_DIVISOR		0x47	/* VID/Fan Divisior Register */
#define W83782_BUS_ADDR			0x48	/* Serial Bus Address Reg */

#define W83782_VID4			0x49	/* Voltage ID & Device ID Reg */
#define W83782_TEMP2_TEMP3		0x4a	/* Temp 2 & Temp 3 Reg */
#define W83782_PIN_CTRL			0x4b	/* Pin Control Register */
#define W83782_IRQ_OVT_SEL		0x4c	/* IRQ/OVT Prop Select Reg */
#define W83782_FAN_BEEP_CTRL		0x4d	/* Fan & Beep/GPIO Control Reg*/
#define W83782_BANK_SEL			0x4e	/* Bank Select Register */
#define W83782_VENDOR_ID		0x4f	/* Winbond Vendor ID */

/* Bank 0 Registers */

#define W83782_TEST			0x50	/* Test Register (0x50-0x55) */

#define W83782_BEEP_CTRL_1		0x56	/* Beep Control Register 1 */
#define W83782_BEEP_CTRL_2		0x57	/* Beep Control Register 2 */

#define W83782_CHIP_ID			0x58	/* Chip ID Register */
#define W83782_DIODE_SEL		0x59	/* Diode Selection Register */
#define W83782_PWM_OUT2_CTRL		0x5a	/* PWMOUT2 Control Register */
#define W83782_PWM_OUT1_CTRL		0x5b	/* PWMOUT1 Control Register */
#define W83782_PWM_OUT12_CLOCK_SEL	0x5c	/* PWMOUT1/2 Clock Select Reg */
#define W83782_VBAT_MON_CTRL		0x5d	/* VBAT Monitor Control Reg */
#define W83782_PWM_OUT3_CTRL		0x5e	/* PWMOUT3 Control Register */
#define W83782_PWM_OUT4_CTRL		0x5f	/* PWMOUT4 Control Register */

/* Bank 1 Registers */

#define W83782_TEMP2_HIGH		0x50	/* Temp Sensor 2 High Byte */
#define W83782_TEMP2_LOW		0x51	/* Temp Sensor 2 Low Byte */
#define W83782_TEMP2_CFG		0x52	/* Temp Sensor 2 Config Reg */
#define W83782_TEMP2_HYST_HIGH		0x53	/* Temp Sensor 2 Hysteresis H */
#define W83782_TEMP2_HYST_LOW		0x54	/* Temp Sensor 2 Hysteresis L */
#define W83782_TEMP2_OVER_HIGH		0x55	/* Temp Sensor 2 Over Temp H */
#define W83782_TEMP2_OVER_LOW		0x56	/* Temp Sensor 2 Over Temp L */

/* Bank 2 Registers */

#define W83782_TEMP3_HIGH		0x50	/* Temp Sensor 3 High Byte */
#define W83782_TEMP3_LOW		0x51	/* Temp Sensor 3 Low Byte */
#define W83782_TEMP3_CFG		0x52	/* Temp Sensor 3 Config Reg */
#define W83782_TEMP3_HYST_HIGH		0x53	/* Temp Sensor 3 Hysteresis H */
#define W83782_TEMP3_HYST_LOW		0x54	/* Temp Sensor 3 Hysteresis L */
#define W83782_TEMP3_OVER_HIGH		0x55	/* Temp Sensor 3 Over Temp H */
#define W83782_TEMP3_OVER_LOW		0x56	/* Temp Sensor 3 Over Temp L */

/* Bank 4 Registers */

#define W83782_INT_STAT_3		0x50	/* Interrupt Status 3 Reg */
#define W83782_SMI_MASK_3		0x51	/* SMI Mask Register 3 */

#define W83782_BEEP_CTRL_3		0x53	/* Beep Control Register 3 */

#define W83782_RT_HW_STAT_I		0x59	/* Real Time HW Status Reg I */
#define W83782_RT_HW_STAT_II		0x5a	/* Real Time HW Status Reg II */
#define W83782_RT_HW_STAT_III		0x5b	/* Real Time HW Status Reg III*/

#define W83782_PWM_OUT34_CLOCK_SEL	0x5c	/* PWMOUT3/4 Clock Select Reg */

/* Auto-Increment Value RAM 2 (Bank 5) */

#define W83782_5VSB_AUTO		0x50	/* 5VSB Reading */
#define W83782_VBAT_AUTO		0x51	/* VBAT Reading */
#define W83782_5VSB_HIGH_AUTO		0x54	/* 5VSB High Limit */
#define W83782_5VSB_LOW_AUTO		0x55	/* 5VSB Low Limit */
#define W83782_VBAT_HIGH_AUTO		0x56	/* VBAT High Limit */
#define W83782_VBAT_LOW_AUTO		0x57	/* VBAT Low Limit */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCw83782HwMonh */
