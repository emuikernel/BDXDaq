/* EPICS names are in [] brackets */

#ifndef HALLBTRIGGERREGISTERS_H
#define HALLBTRIGGERREGISTERS_H


	#define A_BOARDIDS			0x0000
	#define A_REVISION			0x0002
	#define A_ERRORS			0x0004
	
    #define A_LATCHSCALAR		0x1000	/* R/W */
	#define A_MORA_SCALAR_L		0x1004
	#define A_MORA_SCALAR_H		0x1006
	#define A_MORB_SCALAR_L		0x1008
	#define A_MORB_SCALAR_H		0x100A
	#define A_TOF0_SCALAR_L		0x100C
	#define A_TOF0_SCALAR_H		0x100E
	#define A_TOF1_SCALAR_L		0x1010
	#define A_TOF1_SCALAR_H		0x1012
	#define A_TOF2_SCALAR_L		0x1014
	#define A_TOF2_SCALAR_H		0x1016
	#define A_TOF3_SCALAR_L		0x1018
	#define A_TOF3_SCALAR_H		0x101A
	#define A_TOF4_SCALAR_L		0x101C
	#define A_TOF4_SCALAR_H		0x101E
	#define A_TOF5_SCALAR_L		0x1020
	#define A_TOF5_SCALAR_H		0x1022
	#define A_ST_SCALAR_L		0x1024
	#define A_ST_SCALAR_H		0x1026
	#define A_STS0_SCALAR_L		0x1028
	#define A_STS0_SCALAR_H		0x102A
	#define A_STS1_SCALAR_L		0x102C
	#define A_STS1_SCALAR_H		0x102E
	#define A_STS2_SCALAR_L		0x1030
	#define A_STS2_SCALAR_H		0x1032
	#define A_STS3_SCALAR_L		0x1034
	#define A_STS3_SCALAR_H		0x1036
	#define A_STS4_SCALAR_L		0x1038
	#define A_STS4_SCALAR_H		0x103A
	#define A_STS5_SCALAR_L		0x103C
	#define A_STS5_SCALAR_H		0x103E
	#define A_S0A_SCALAR_L		0x1040
	#define A_S0A_SCALAR_H		0x1042
	#define A_S1A_SCALAR_L		0x1044
	#define A_S1A_SCALAR_H		0x1046
	#define A_S2A_SCALAR_L		0x1048
	#define A_S2A_SCALAR_H		0x104A
	#define A_S3A_SCALAR_L		0x104C
	#define A_S3A_SCALAR_H		0x104E
	#define A_S4A_SCALAR_L		0x1050
	#define A_S4A_SCALAR_H		0x1052
	#define A_S5A_SCALAR_L		0x1054
	#define A_S5A_SCALAR_H		0x1056
	#define A_TRIG0_SCALAR_L	0x1058
	#define A_TRIG0_SCALAR_H	0x105A
	#define A_TRIG1_SCALAR_L	0x105C
	#define A_TRIG1_SCALAR_H	0x105E
	#define A_TRIG2_SCALAR_L	0x1060
	#define A_TRIG2_SCALAR_H	0x1062
	#define A_TRIG3_SCALAR_L	0x1064
	#define A_TRIG3_SCALAR_H	0x1066
	#define A_TRIG4_SCALAR_L	0x1068
	#define A_TRIG4_SCALAR_H	0x106A
	#define A_TRIG5_SCALAR_L	0x106C
	#define A_TRIG5_SCALAR_H	0x106E
	#define A_TRIG6_SCALAR_L	0x1070
	#define A_TRIG6_SCALAR_H	0x1072
	#define A_TRIG7_SCALAR_L	0x1074
	#define A_TRIG7_SCALAR_H	0x1076
	#define A_TRIG8_SCALAR_L	0x1078
	#define A_TRIG8_SCALAR_H	0x107A
	#define A_TRIG9_SCALAR_L	0x107C
	#define A_TRIG9_SCALAR_H	0x107E

	#define A_S0B_SCALAR_L		0x1080
	#define A_S0B_SCALAR_H		0x1082
	#define A_S1B_SCALAR_L		0x1084
	#define A_S1B_SCALAR_H		0x1086
	#define A_S2B_SCALAR_L		0x1088
	#define A_S2B_SCALAR_H		0x108A
	#define A_S3B_SCALAR_L		0x108C
	#define A_S3B_SCALAR_H		0x108E
	#define A_S4B_SCALAR_L		0x1090
	#define A_S4B_SCALAR_H		0x1092
	#define A_S5B_SCALAR_L		0x1094
	#define A_S5B_SCALAR_H		0x1096

	#define A_ST0_SCALAR_L		0x1098
	#define A_ST0_SCALAR_H		0x109A
	#define A_ST1_SCALAR_L		0x109C
	#define A_ST1_SCALAR_H		0x109E
	#define A_ST2_SCALAR_L		0x10A0
	#define A_ST2_SCALAR_H		0x10A2
	#define A_ST3_SCALAR_L		0x10A4
	#define A_ST3_SCALAR_H		0x10A6
	#define A_ST4_SCALAR_L		0x10A8
	#define A_ST4_SCALAR_H		0x10AA
	#define A_ST5_SCALAR_L		0x10AC
	#define A_ST5_SCALAR_H		0x10AE
	#define A_ST6_SCALAR_L		0x10B0
	#define A_ST6_SCALAR_H		0x10B2
	#define A_ST7_SCALAR_L		0x10B4
	#define A_ST7_SCALAR_H		0x10B6
	#define A_ST8_SCALAR_L		0x10B8
	#define A_ST8_SCALAR_H		0x10BA
	#define A_ST9_SCALAR_L		0x10BC
	#define A_ST9_SCALAR_H		0x10BE
	#define A_ST10_SCALAR_L		0x10C0
	#define A_ST10_SCALAR_H		0x10C2
	#define A_ST11_SCALAR_L		0x10C4
	#define A_ST11_SCALAR_H		0x10C6
	#define A_ST12_SCALAR_L		0x10C8
	#define A_ST12_SCALAR_H		0x10CA
	#define A_ST13_SCALAR_L		0x10CC
	#define A_ST13_SCALAR_H		0x10CE
	#define A_ST14_SCALAR_L		0x10D0
	#define A_ST14_SCALAR_H		0x10D2
	#define A_ST15_SCALAR_L		0x10D4
	#define A_ST15_SCALAR_H		0x10D6
	#define A_ST16_SCALAR_L		0x10D8
	#define A_ST16_SCALAR_H		0x10DA
	#define A_ST17_SCALAR_L		0x10DC
	#define A_ST17_SCALAR_H		0x10DE
	#define A_ST18_SCALAR_L		0x10F0
	#define A_ST18_SCALAR_H		0x10F2
	#define A_ST19_SCALAR_L		0x10F4
	#define A_ST19_SCALAR_H		0x10F6
	#define A_ST20_SCALAR_L		0x10F8
	#define A_ST20_SCALAR_H		0x10FA
	#define A_ST21_SCALAR_L		0x10FC
	#define A_ST21_SCALAR_H		0x10FE
	#define A_ST22_SCALAR_L		0x1100
	#define A_ST22_SCALAR_H		0x1102
	#define A_ST23_SCALAR_L		0x1104
	#define A_ST23_SCALAR_H		0x1106
	
    #define A_MORA_DLY			0x2000	/* bits 15:5 => don't care, bits 4:0 => delay setting (0=0ns, 1=5ns,..., 31=155ns) [TS_MORA_DELAY] */
    #define A_MORB_DLY			0x2002 /* [TS_MORB_DELAY] */
	#define A_TOF0_DLY			0x2004 /* [TS_TOF1_DELAY] */
	#define A_TOF1_DLY			0x2006 /* [TS_TOF2_DELAY] */
	#define A_TOF2_DLY			0x2008 /* [TS_TOF3_DELAY] */
	#define A_TOF3_DLY			0x200A /* [TS_TOF4_DELAY] */
	#define A_TOF4_DLY			0x200C /* [TS_TOF5_DELAY] */
	#define A_TOF5_DLY			0x200E /* [TS_TOF6_DELAY] */
	#define A_STS0_DLY			0x2010 /* [TS_STS1_DELAY] */
	#define A_STS1_DLY			0x2012 /* [TS_STS2_DELAY] */
	#define A_STS2_DLY			0x2014 /* [TS_STS3_DELAY] */
	#define A_STS3_DLY			0x2016 /* [TS_STS4_DELAY] */
	#define A_STS4_DLY			0x2018 /* [TS_STS5_DELAY] */
	#define A_STS5_DLY			0x201A /* [TS_STS6_DELAY] */
	#define A_ST_DLY			0x201C /* [TS_ST_MULT_DELAY] */



	#define A_LATCHPRESCALE		0x201E	/* don't use */
	#define A_TRIG0_PRESCALE	0x2020	/* bits 15:13 => don't care,
                                           bits 12:10 => pulse stretch (0=0ns, 1=5ns,...,7=35ns) [TS_TRIG1_STRETCH]
		                                   bits 9:0 => prescale [TS_TRIG1_PRESCALE]
                                             (0=output disabled, 1=no prescale, 2=every other event, ..., 1023=...)
										*/
    #define A_TRIG1_PRESCALE	0x2022  /* [TS_TRIG2_STRETCH] [TS_TRIG2_PRESCALE] */
	#define A_TRIG2_PRESCALE	0x2024  /* [TS_TRIG3_STRETCH] [TS_TRIG3_PRESCALE] */
	#define A_TRIG3_PRESCALE	0x2026  /* [TS_TRIG4_STRETCH] [TS_TRIG4_PRESCALE] */
	#define A_TRIG4_PRESCALE	0x2028  /* [TS_TRIG5_STRETCH] [TS_TRIG5_PRESCALE] */
	#define A_TRIG5_PRESCALE	0x202A  /* [TS_TRIG6_STRETCH] [TS_TRIG6_PRESCALE] */
	#define A_TRIG6_PRESCALE	0x202C  /* [TS_TRIG7_STRETCH] [TS_TRIG7_PRESCALE] */
	#define A_TRIG7_PRESCALE	0x202E  /* [TS_TRIG8_STRETCH] [TS_TRIG8_PRESCALE] */
	#define A_TRIG8_PRESCALE	0x2030  /* [TS_TRIG11_STRETCH] [TS_TRIG11_PRESCALE] */
	#define A_TRIG9_PRESCALE	0x2032  /* [TS_TRIG12_STRETCH] [TS_TRIG12_PRESCALE] */

	#define A_STMULT_THRESH		0x2034  /* bits 15:10 => don't care,
                                           bits 9:5 => maximum ST multiplicity [TS_ST_MULT_MAX]
                                           bits 4:0 => minimum ST multiplicity (thresholds are included in range) [TS_ST_MULT_MIN] */

	#define A_TRIGMASKS1		0x2036	/* bits 15:14 => don't care
										   bits 13:8 => sector ST disable bits, i.e. bit 13=>sector 6, bits 12=>sector 5, etc...
                                             "Disable" causes permanant assertion of trigger from this module
                                             [TS_ST_SEC1_DISABLE] [TS_ST_SEC2_DISABLE] [TS_ST_SEC3_DISABLE]
                                             [TS_ST_SEC4_DISABLE] [TS_ST_SEC5_DISABLE] [TS_ST_SEC6_DISABLE]
										   bits 7:2 => sector TOF disable bits, i.e. bit 13=>sector 6, bits 12=>sector 5, etc...
                                             [TS_TOF_SEC1_DISABLE] [TS_TOF_SEC2_DISABLE] [TS_TOF_SEC3_DISABLE]
                                             [TS_TOF_SEC4_DISABLE] [TS_TOF_SEC5_DISABLE] [TS_TOF_SEC6_DISABLE]
                                             "Disable" causes permanant assertion of trigger from this module
		                                */
	#define A_TRIGMASKS2		0x2038	/* bits 15:12 => don't care
										   bits 11:6 => sector B logic enable bits, i.e. bit 11=>sector 6, bits 10=>sector 5, etc...
                                             "if Enable=0" causes sector trigger output to stay low
                                             [TS_LOGICB_SEC1_ENABLE] [TS_LOGICB_SEC2_ENABLE] [TS_LOGICB_SEC3_ENABLE]
                                             [TS_LOGICB_SEC4_ENABLE] [TS_LOGICB_SEC5_ENABLE] [TS_LOGICB_SEC6_ENABLE]
										   bits 5:0 => sector A logic enable bits, i.e. bit 5=>sector 6, bits 4=>sector 5, etc...
                                             "if Enable=0" causes sector trigger output to stay low
                                             [TS_LOGICA_SEC1_ENABLE] [TS_LOGICA_SEC2_ENABLE] [TS_LOGICA_SEC3_ENABLE]
                                             [TS_LOGICA_SEC4_ENABLE] [TS_LOGICA_SEC5_ENABLE] [TS_LOGICA_SEC6_ENABLE]
		                                */
	#define A_TRIGMASKS3		0x203A	/* bits 15:10 => single sector trigger enable bits,
                                             i.e. bit 15=>sector 6 causes TRIG6 to trigger without coincidence with another sector....
                                             [TS_SINGLE_SEC1_ENABLE] [TS_SINGLE_SEC2_ENABLE] [TS_SINGLE_SEC3_ENABLE]
                                             [TS_SINGLE_SEC4_ENABLE] [TS_SINGLE_SEC5_ENABLE] [TS_SINGLE_SEC6_ENABLE]
										   bits 9:0 => trigger bit ST multiplicity enable,
                                             i.e. TRIG bit is asserted only with STMULT coincidence when bit='1',
                                             else TRIG output doesn't depend on ST multiplicity.
		                                       bit 9=>TRIG12, bit 8=>TRIG11, bit 7=>TRIG8, bit 6=>TRIG7, bit 5=>TRIG6,
                                               bit 4=>TRIG5, bit 3=>TRIG4, bit 2=>TRIG3, bit 1=>TRIG2, bit 0=>TRIG1
                                             [TS_ST_MULT_BIT1_ENABLE] [TS_ST_MULT_BIT2_ENABLE] [TS_ST_MULT_BIT3_ENABLE]
                                             [TS_ST_MULT_BIT4_ENABLE] [TS_ST_MULT_BIT5_ENABLE] [TS_ST_MULT_BIT6_ENABLE]
                                             [TS_ST_MULT_BIT7_ENABLE] [TS_ST_MULT_BIT8_ENABLE] [TS_ST_MULT_BIT11_ENABLE]
                                             [TS_ST_MULT_BIT12_ENABLE]
		                                */
	#define A_TRIGMASKS4		0x203C	/* bits 15:10 => don't care
										   bits 9:0 => trigger bit MOR enable,
                                             i.e. TRIG bit is asserted only with MOR coincidence when bit='1',
                                             else TRIG output doesn't depend on MOR.
		                                       bit 9=>TRIG12, bit 8=>TRIG11, bit 7=>TRIG8, bit 6=>TRIG7, bit 5=>TRIG6, bit 4=>TRIG5,
                                               bit 3=>TRIG4, bit 2=>TRIG3, bit 1=>TRIG2, bit 0=>TRIG1
											   bits 7:0 use MORA, bits 9:8 use MORB
                                             [TS_MORA_BIT1_ENABLE] [TS_MORA_BIT2_ENABLE] [TS_MORA_BIT3_ENABLE]
                                             [TS_MORA_BIT4_ENABLE] [TS_MORA_BIT5_ENABLE] [TS_MORA_BIT6_ENABLE]
                                             [TS_MORA_BIT7_ENABLE] [TS_MORA_BIT8_ENABLE]
                                             [TS_MORB_BIT11_ENABLE] [TS_MORB_BIT12_ENABLE]
		                                */




	/* SCOPE TRIGGERING REGISTERS - do not need it in EPICS */
	#define A_TRIG_STATUS				0x3000
	#define A_TRIG_CFG					0x3002
	#define A_TRIG_VAL_INPUTS_L			0x3004
	#define A_TRIG_VAL_INPUTS_H			0x3006
	#define A_TRIG_IGNORE_INPUTS_L		0x3008
	#define A_TRIG_IGNORE_INPUTS_H		0x300A
	#define A_TRIG_VAL_TRIGLOGIC_L		0x300C
	#define A_TRIG_VAL_TRIGLOGIC_H		0x300E
	#define A_TRIG_IGNORE_TRIGLOGIC_L	0x3010
	#define A_TRIG_IGNORE_TRIGLOGIC_H	0x3012
	#define A_TRIG_INPUTS_L				0x3200
	#define A_TRIG_INPUTS_H				0x3202
/*          A_TRIG_INPUTS reserves up ->0x33FF */
	#define A_TRIG_TRIGLOGIC_L			0x3400
	#define A_TRIG_TRIGLOGIC_H			0x3402
/*          A_TRIG_LOGIC  reserves up ->0x35FF */

	#define V1495_VMEADDRESS	0x08510000
/*	#define V1495_VMEADDRESS	0x10000000 */
	#define V895_VMEADDRESS1	0x60000000
	#define V895_VMEADDRESS2	0x70000000

	#define PRESCALE_MAX			1023
	#define PERSIST_MAX				7
	#define DELAY_MAX				31
	#define MULT_MAX				24
	#define V1495_CLKPERIOD_NS		5
	#define TRIGGER_CAP_LEN			128

#endif
