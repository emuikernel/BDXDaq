#ifndef HallBTriggerBoardRegs_H
#define HallBTriggerBoardRegs_H

/* Board Supports VME A32/A24 D16 Accesses */

#define	MAX_PRESCALE						1023
#define MAX_DELAY							31
#define CLOCK_PERIOD_NS						5

#define TS_BOARD_ADDRESS					0x08510000

#define TS_REVISION							0x0002

/************************************************/
/************** BEGIN SECTOR CONFIG REGISTERS ***/
/************************************************/
/* Notes:
   1) TS_SECTORx_CFG sets the configuration for sector x,
      which is common to all trigger bits. This configures
	  the ECC output bit only.
   2) Register description:
      
      reg value      description
	      1          ECC outputs ECE
		  2          ECC outputs CC
		  3          ECE outputs (ECE and CC)
		  4          ECE outputs '1'
		  8          ECE outputs '0'
*/
#define TS_SECTOR1_CFG						0x0100	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
#define TS_SECTOR2_CFG						0x0102	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
#define TS_SECTOR3_CFG						0x0104	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
#define TS_SECTOR4_CFG						0x0106	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
#define TS_SECTOR5_CFG						0x0108	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
#define TS_SECTOR6_CFG						0x010A	/* b0: ECE; b1: CC; b2: FORCE1; b3: FORCE0 */
/************************************************/
/************** END SECTOR CONFIG REGISTERS *****/
/************************************************/

/************************************************/
/************** BEGIN PRESCALER REGISTERS *******/
/************************************************/
/* Notes:
   1) TS_PRESCALE_TRIGx sets the prescale value for trigger bit x
   2) Range is from 0 to 1023, where 0 disables output.
*/
#define TS_PRESCALE_TRIG1					0x010C
#define TS_PRESCALE_TRIG2					0x010E
#define TS_PRESCALE_TRIG3					0x0110
#define TS_PRESCALE_TRIG4					0x0112
#define TS_PRESCALE_TRIG5					0x0114
#define TS_PRESCALE_TRIG6					0x0116
#define TS_PRESCALE_TRIG7					0x0118
#define TS_PRESCALE_TRIG8					0x011A
#define TS_PRESCALE_TRIG9					0x011C
#define TS_PRESCALE_TRIG10					0x011E
#define TS_PRESCALE_TRIG11					0x0120
#define TS_PRESCALE_TRIG12					0x0122
/************************************************/
/************** END PRESCALER REGISTERS *********/
/************************************************/

/************************************************/
/************** BEGIN LUT CONTROL REGISTERS *****/
/************************************************/
/* Notes:
   1) TS_TRIG_SEL selects which trigger bit LUTs to program. bit0 => "TRIG BIT 1", ... , bit10 => "TRIG BIT 11"
   2) Once TS_TRIG_SEL has been written to select the trigger bit you can program the LUTs by writing to the
      appropriate addresses:
   3) See document "HallBFlexTriggerIC_TrigBitLUTAddressBitMap.pdf" on how these LUTs are wired for triggers.
	  
        Starting Address:   LUT Size:

        TS_TRIG_LUT_TRIG    256 Word(16bit) LUT
        TS_TRIG_LUT_ECP     256 Word(16bit) LUT
        TS_TRIG_LUT_ECE     256 Word(16bit) LUT
        TS_TRIG_LUT_ECC     4 Word(16bit) LUT
 */
#define TS_TRIG_SEL							0x0010
#define TS_TRIG_LUT_TRIG					0x4000
#define TS_TRIG_LUT_ECP						0x4200
#define TS_TRIG_LUT_ECE						0x4400
#define TS_TRIG_LUT_ECC						0x4600
/************************************************/
/************** END LUT CONTROL REGISTERS *****/
/************************************************/

/************************************************/
/************** BEGIN SCALER REGISTERS **********/
/************************************************/
/* Notes:
   1) Scalers are all 32bits, BIG-ENDIAN.
   2) TS_REFCLK_SCALER is a reference scaler which contains gate time of all scalers (in 25ns ticks)
   3) Set TS_ENABLE_SCALERS to '1' to enable scalers. Set to '0' to stop scalers for readout.
      Setting back to '1' will clear all scalers and allow them to count again.
   4) Scalers are capable of counting at 100MHz, which is about 43sec before overflowing at this high rate
 */
#define TS_ENABLE_SCALERS					0x0800

#define TS_FIRST_SCALER                     TS_REFCLK_SCALER_H
#define TS_REFCLK_SCALER_H					0x1000
#define TS_REFCLK_SCALER_L					0x1002
#define TS_TOF1_SCALER_H					0x1004
#define TS_TOF1_SCALER_L					0x1006
#define TS_TOF2_SCALER_H					0x1008
#define TS_TOF2_SCALER_L					0x100A
#define TS_TOF3_SCALER_H					0x100C
#define TS_TOF3_SCALER_L					0x100E
#define TS_TOF4_SCALER_H					0x1010
#define TS_TOF4_SCALER_L					0x1012
#define TS_TOF5_SCALER_H					0x1014
#define TS_TOF5_SCALER_L					0x1016
#define TS_TOF6_SCALER_H					0x1018
#define TS_TOF6_SCALER_L					0x101A
#define TS_ECin1P_SCALER_H					0x101C
#define TS_ECin1P_SCALER_L					0x101E
#define TS_ECin2P_SCALER_H					0x1020
#define TS_ECin2P_SCALER_L					0x1022
#define TS_ECin3P_SCALER_H					0x1024
#define TS_ECin3P_SCALER_L					0x1026
#define TS_ECin4P_SCALER_H					0x1028
#define TS_ECin4P_SCALER_L					0x102A
#define TS_ECin5P_SCALER_H					0x102C
#define TS_ECin5P_SCALER_L					0x102E
#define TS_ECin6P_SCALER_H					0x1030
#define TS_ECin6P_SCALER_L					0x1032
#define TS_ECin1E_SCALER_H					0x1034
#define TS_ECin1E_SCALER_L					0x1036
#define TS_ECin2E_SCALER_H					0x1038
#define TS_ECin2E_SCALER_L					0x103A
#define TS_ECin3E_SCALER_H					0x103C
#define TS_ECin3E_SCALER_L					0x103E
#define TS_ECin4E_SCALER_H					0x1040
#define TS_ECin4E_SCALER_L					0x1042
#define TS_ECin5E_SCALER_H					0x1044
#define TS_ECin5E_SCALER_L					0x1046
#define TS_ECin6E_SCALER_H					0x1048
#define TS_ECin6E_SCALER_L					0x104A
#define TS_ECtot1P_SCALER_H					0x104C
#define TS_ECtot1P_SCALER_L					0x104E
#define TS_ECtot2P_SCALER_H					0x1050
#define TS_ECtot2P_SCALER_L					0x1052
#define TS_ECtot3P_SCALER_H					0x1054
#define TS_ECtot3P_SCALER_L					0x1056
#define TS_ECtot4P_SCALER_H					0x1058
#define TS_ECtot4P_SCALER_L					0x105A
#define TS_ECtot5P_SCALER_H					0x105C
#define TS_ECtot5P_SCALER_L					0x105E
#define TS_ECtot6P_SCALER_H					0x1060
#define TS_ECtot6P_SCALER_L					0x1062
#define TS_ECtot1E_SCALER_H					0x1064
#define TS_ECtot1E_SCALER_L					0x1066
#define TS_ECtot2E_SCALER_H					0x1068
#define TS_ECtot2E_SCALER_L					0x106A
#define TS_ECtot3E_SCALER_H					0x106C
#define TS_ECtot3E_SCALER_L					0x106E
#define TS_ECtot4E_SCALER_H					0x1070
#define TS_ECtot4E_SCALER_L					0x1072
#define TS_ECtot5E_SCALER_H					0x1074
#define TS_ECtot5E_SCALER_L					0x1076
#define TS_ECtot6E_SCALER_H					0x1078
#define TS_ECtot6E_SCALER_L					0x107A
#define TS_CC1_SCALER_H						0x107C
#define TS_CC1_SCALER_L						0x107E
#define TS_CC2_SCALER_H						0x1080
#define TS_CC2_SCALER_L						0x1082
#define TS_CC3_SCALER_H						0x1084
#define TS_CC3_SCALER_L						0x1086
#define TS_CC4_SCALER_H						0x1088
#define TS_CC4_SCALER_L						0x108A
#define TS_CC5_SCALER_H						0x108C
#define TS_CC5_SCALER_L						0x108E
#define TS_CC6_SCALER_H						0x1090
#define TS_CC6_SCALER_L						0x1092
#define TS_SCALER_TOTAL0_H					0x1094
#define TS_SCALER_TOTAL0_L					0x1096
#define TS_SCALER_TOTAL1_H					0x1098
#define TS_SCALER_TOTAL1_L					0x109A
#define TS_SCALER_TOTAL2_H					0x109C
#define TS_SCALER_TOTAL2_L					0x109E
#define TS_SCALER_ELECTRON0_H				0x10A0
#define TS_SCALER_ELECTRON0_L				0x10A2
#define TS_SCALER_ELECTRON1_H				0x10A4
#define TS_SCALER_ELECTRON1_L				0x10A6
#define TS_SCALER_ELECTRON2_H				0x10A8
#define TS_SCALER_ELECTRON2_L				0x10AA
#define TS_SCALER_VETO0_H					0x10AC
#define TS_SCALER_VETO0_L					0x10AE
#define TS_SCALER_VETO1_H					0x10B0
#define TS_SCALER_VETO1_L					0x10B2
#define TS_SCALER_VETO2_H					0x10B4
#define TS_SCALER_VETO2_L					0x10B6
#define TS_TRIG1_TRIGLUT_SCALER_H			0x10B8
#define TS_TRIG1_TRIGLUT_SCALER_L			0x10BA
#define TS_TRIG2_TRIGLUT_SCALER_H			0x10BC
#define TS_TRIG2_TRIGLUT_SCALER_L			0x10BE
#define TS_TRIG3_TRIGLUT_SCALER_H			0x10C0
#define TS_TRIG3_TRIGLUT_SCALER_L			0x10C2
#define TS_TRIG4_TRIGLUT_SCALER_H			0x10C4
#define TS_TRIG4_TRIGLUT_SCALER_L			0x10C6
#define TS_TRIG5_TRIGLUT_SCALER_H			0x10C8
#define TS_TRIG5_TRIGLUT_SCALER_L			0x10CA
#define TS_TRIG6_TRIGLUT_SCALER_H			0x10CC
#define TS_TRIG6_TRIGLUT_SCALER_L			0x10CE
#define TS_TRIG7_TRIGLUT_SCALER_H			0x10D0
#define TS_TRIG7_TRIGLUT_SCALER_L			0x10D2
#define TS_TRIG8_TRIGLUT_SCALER_H			0x10D4
#define TS_TRIG8_TRIGLUT_SCALER_L			0x10D6
#define TS_TRIG9_TRIGLUT_SCALER_H			0x10D8
#define TS_TRIG9_TRIGLUT_SCALER_L			0x10DA
#define TS_TRIG10_TRIGLUT_SCALER_H			0x10DC
#define TS_TRIG10_TRIGLUT_SCALER_L			0x10DE
#define TS_TRIG11_TRIGLUT_SCALER_H			0x10F0
#define TS_TRIG11_TRIGLUT_SCALER_L			0x10F2
#define TS_TRIG1_ECPLUT_SCALER_H			0x10F4
#define TS_TRIG1_ECPLUT_SCALER_L			0x10F6
#define TS_TRIG2_ECPLUT_SCALER_H			0x10F8
#define TS_TRIG2_ECPLUT_SCALER_L			0x10FA
#define TS_TRIG3_ECPLUT_SCALER_H			0x10FC
#define TS_TRIG3_ECPLUT_SCALER_L			0x10FE
#define TS_TRIG4_ECPLUT_SCALER_H			0x1100
#define TS_TRIG4_ECPLUT_SCALER_L			0x1102
#define TS_TRIG5_ECPLUT_SCALER_H			0x1104
#define TS_TRIG5_ECPLUT_SCALER_L			0x1106
#define TS_TRIG6_ECPLUT_SCALER_H			0x1108
#define TS_TRIG6_ECPLUT_SCALER_L			0x110A
#define TS_TRIG7_ECPLUT_SCALER_H			0x110C
#define TS_TRIG7_ECPLUT_SCALER_L			0x110E
#define TS_TRIG8_ECPLUT_SCALER_H			0x1110
#define TS_TRIG8_ECPLUT_SCALER_L			0x1112
#define TS_TRIG9_ECPLUT_SCALER_H			0x1114
#define TS_TRIG9_ECPLUT_SCALER_L			0x1116
#define TS_TRIG10_ECPLUT_SCALER_H			0x1118
#define TS_TRIG10_ECPLUT_SCALER_L			0x111A
#define TS_TRIG11_ECPLUT_SCALER_H			0x111C
#define TS_TRIG11_ECPLUT_SCALER_L			0x111E
#define TS_TRIG1_ECELUT_SCALER_H			0x1120
#define TS_TRIG1_ECELUT_SCALER_L			0x1122
#define TS_TRIG2_ECELUT_SCALER_H			0x1124
#define TS_TRIG2_ECELUT_SCALER_L			0x1126
#define TS_TRIG3_ECELUT_SCALER_H			0x1128
#define TS_TRIG3_ECELUT_SCALER_L			0x112A
#define TS_TRIG4_ECELUT_SCALER_H			0x112C
#define TS_TRIG4_ECELUT_SCALER_L			0x112E
#define TS_TRIG5_ECELUT_SCALER_H			0x1130
#define TS_TRIG5_ECELUT_SCALER_L			0x1132
#define TS_TRIG6_ECELUT_SCALER_H			0x1134
#define TS_TRIG6_ECELUT_SCALER_L			0x1136
#define TS_TRIG7_ECELUT_SCALER_H			0x1138
#define TS_TRIG7_ECELUT_SCALER_L			0x113A
#define TS_TRIG8_ECELUT_SCALER_H			0x113C
#define TS_TRIG8_ECELUT_SCALER_L			0x113E
#define TS_TRIG9_ECELUT_SCALER_H			0x1140
#define TS_TRIG9_ECELUT_SCALER_L			0x1142
#define TS_TRIG10_ECELUT_SCALER_H			0x1144
#define TS_TRIG10_ECELUT_SCALER_L			0x1146
#define TS_TRIG11_ECELUT_SCALER_H			0x1148
#define TS_TRIG11_ECELUT_SCALER_L			0x114A
#define TS_TRIG1_ECCLUT_SCALER_H			0x114C
#define TS_TRIG1_ECCLUT_SCALER_L			0x114E
#define TS_TRIG2_ECCLUT_SCALER_H			0x1150
#define TS_TRIG2_ECCLUT_SCALER_L			0x1152
#define TS_TRIG3_ECCLUT_SCALER_H			0x1154
#define TS_TRIG3_ECCLUT_SCALER_L			0x1156
#define TS_TRIG4_ECCLUT_SCALER_H			0x1158
#define TS_TRIG4_ECCLUT_SCALER_L			0x115A
#define TS_TRIG5_ECCLUT_SCALER_H			0x115C
#define TS_TRIG5_ECCLUT_SCALER_L			0x115E
#define TS_TRIG6_ECCLUT_SCALER_H			0x1160
#define TS_TRIG6_ECCLUT_SCALER_L			0x1162
#define TS_TRIG7_ECCLUT_SCALER_H			0x1164
#define TS_TRIG7_ECCLUT_SCALER_L			0x1166
#define TS_TRIG8_ECCLUT_SCALER_H			0x1168
#define TS_TRIG8_ECCLUT_SCALER_L			0x116A
#define TS_TRIG9_ECCLUT_SCALER_H			0x116C
#define TS_TRIG9_ECCLUT_SCALER_L			0x116E
#define TS_TRIG10_ECCLUT_SCALER_H			0x1170
#define TS_TRIG10_ECCLUT_SCALER_L			0x1172
#define TS_TRIG11_ECCLUT_SCALER_H			0x1174
#define TS_TRIG11_ECCLUT_SCALER_L			0x1176
#define TS_ECP1_SCALER_H					0x1178
#define TS_ECP1_SCALER_L					0x117A
#define TS_ECP2_SCALER_H					0x117C
#define TS_ECP2_SCALER_L					0x117E
#define TS_ECP3_SCALER_H					0x1180
#define TS_ECP3_SCALER_L					0x1182
#define TS_ECP4_SCALER_H					0x1184
#define TS_ECP4_SCALER_L					0x1186
#define TS_ECP5_SCALER_H					0x1188
#define TS_ECP5_SCALER_L					0x118A
#define TS_ECP6_SCALER_H					0x118C
#define TS_ECP6_SCALER_L					0x118E
#define TS_ECE1_SCALER_H					0x1190
#define TS_ECE1_SCALER_L					0x1192
#define TS_ECE2_SCALER_H					0x1194
#define TS_ECE2_SCALER_L					0x1196
#define TS_ECE3_SCALER_H					0x1198
#define TS_ECE3_SCALER_L					0x119A
#define TS_ECE4_SCALER_H					0x119C
#define TS_ECE4_SCALER_L					0x119E
#define TS_ECE5_SCALER_H					0x11A0
#define TS_ECE5_SCALER_L					0x11A2
#define TS_ECE6_SCALER_H					0x11A4
#define TS_ECE6_SCALER_L					0x11A6
#define TS_ECC1_SCALER_H					0x11A8
#define TS_ECC1_SCALER_L					0x11AA
#define TS_ECC2_SCALER_H					0x11AC
#define TS_ECC2_SCALER_L					0x11AE
#define TS_ECC3_SCALER_H					0x11B0
#define TS_ECC3_SCALER_L					0x11B2
#define TS_ECC4_SCALER_H					0x11B4
#define TS_ECC4_SCALER_L					0x11B6
#define TS_ECC5_SCALER_H					0x11B8
#define TS_ECC5_SCALER_L					0x11BA
#define TS_ECC6_SCALER_H					0x11BC
#define TS_ECC6_SCALER_L					0x11BE
#define TS_E_SECTOR1_SCALER_H				0x11C0
#define TS_E_SECTOR1_SCALER_L				0x11C2
#define TS_E_SECTOR2_SCALER_H				0x11C4
#define TS_E_SECTOR2_SCALER_L				0x11C6
#define TS_E_SECTOR3_SCALER_H				0x11C8
#define TS_E_SECTOR3_SCALER_L				0x11CA
#define TS_E_SECTOR4_SCALER_H				0x11CC
#define TS_E_SECTOR4_SCALER_L				0x11CE
#define TS_E_SECTOR5_SCALER_H				0x11D0
#define TS_E_SECTOR5_SCALER_L				0x11D2
#define TS_E_SECTOR6_SCALER_H				0x11D4
#define TS_E_SECTOR6_SCALER_L				0x11D6
#define TS_E_TRIG_SCALER_H					0x11D8
#define TS_E_TRIG_SCALER_L					0x11DA
#define TS_LAST_SCALER                      TS_E_TRIG_SCALER_L
/************************************************/
/************** END SCALER REGISTERS ************/
/************************************************/


/************************************************/
/************** BEGIN DELAY REGISTERS **********/
/************************************************/
/* Note: Delays occupy bits(4:0) of the register.
         Each count is a 5ns delay.
		 Range is 0ns (reg = 0) to 155ns (reg = 31)
*/
#define TS_FIRST_DELAY                      TS_TOF1_DELAY

#define TS_TOF1_DELAY						0x1400
#define TS_TOF2_DELAY						0x1402
#define TS_TOF3_DELAY						0x1404
#define TS_TOF4_DELAY						0x1406
#define TS_TOF5_DELAY						0x1408
#define TS_TOF6_DELAY						0x140A

#define TS_ECin1P_DELAY						0x140C
#define TS_ECin2P_DELAY						0x140E
#define TS_ECin3P_DELAY						0x1410
#define TS_ECin4P_DELAY						0x1412
#define TS_ECin5P_DELAY						0x1414
#define TS_ECin6P_DELAY						0x1416

#define TS_ECin1E_DELAY						0x1418
#define TS_ECin2E_DELAY						0x141A
#define TS_ECin3E_DELAY						0x141C
#define TS_ECin4E_DELAY						0x141E
#define TS_ECin5E_DELAY						0x1420
#define TS_ECin6E_DELAY						0x1422

#define TS_ECtot1P_DELAY					0x1424
#define TS_ECtot2P_DELAY					0x1426
#define TS_ECtot3P_DELAY					0x1428
#define TS_ECtot4P_DELAY					0x142A
#define TS_ECtot5P_DELAY					0x142C
#define TS_ECtot6P_DELAY					0x142E

#define TS_ECtot1E_DELAY					0x1430
#define TS_ECtot2E_DELAY					0x1432
#define TS_ECtot3E_DELAY					0x1434
#define TS_ECtot4E_DELAY					0x1436
#define TS_ECtot5E_DELAY					0x1438
#define TS_ECtot6E_DELAY					0x143A

#define TS_CC1_DELAY						0x143C
#define TS_CC2_DELAY						0x143E
#define TS_CC3_DELAY						0x1440
#define TS_CC4_DELAY						0x1442
#define TS_CC5_DELAY						0x1444
#define TS_CC6_DELAY						0x1446

#define TS_E_ECin1E_DELAY					0x1448
#define TS_E_ECin2E_DELAY					0x144A
#define TS_E_ECin3E_DELAY					0x144C
#define TS_E_ECin4E_DELAY					0x144E
#define TS_E_ECin5E_DELAY					0x1450
#define TS_E_ECin6E_DELAY					0x1452

#define TS_E_ECtot1E_DELAY					0x1454
#define TS_E_ECtot2E_DELAY					0x1456
#define TS_E_ECtot3E_DELAY					0x1458
#define TS_E_ECtot4E_DELAY					0x145A
#define TS_E_ECtot5E_DELAY					0x145C
#define TS_E_ECtot6E_DELAY					0x145E

#define TS_E_CC1_DELAY						0x1460
#define TS_E_CC2_DELAY						0x1462
#define TS_E_CC3_DELAY						0x1464
#define TS_E_CC4_DELAY						0x1466
#define TS_E_CC5_DELAY						0x1468
#define TS_E_CC6_DELAY						0x146A

#define TS_LAST_DELAY                       TS_E_CC6_DELAY
/************************************************/
/************** END DELAY REGISTERS *************/
/************************************************/

/************************************************/
/************** BEGIN SCOPE REGISTERS ***********/
/************************************************/
#define TS_TRIG_STATUS						0x3000
#define TS_TRIG_VALUE3_H					0x3002
#define TS_TRIG_VALUE3_L					0x3004
#define TS_TRIG_VALUE2_H					0x3006
#define TS_TRIG_VALUE2_L					0x3008
#define TS_TRIG_VALUE1_H					0x300A
#define TS_TRIG_VALUE1_L					0x300C
#define TS_TRIG_VALUE0_H					0x300E
#define TS_TRIG_VALUE0_L					0x3010
#define TS_TRIG_INGORE3_H					0x3012
#define TS_TRIG_INGORE3_L					0x3014
#define TS_TRIG_INGORE2_H					0x3016
#define TS_TRIG_INGORE2_L					0x3018
#define TS_TRIG_INGORE1_H					0x301A
#define TS_TRIG_INGORE1_L					0x301C
#define TS_TRIG_INGORE0_H					0x301E
#define TS_TRIG_INGORE0_L					0x3020
#define TS_TRIG_BUFFER3_H					0x3800
#define TS_TRIG_BUFFER3_L					0x3802
#define TS_TRIG_BUFFER2_H					0x3804
#define TS_TRIG_BUFFER2_L					0x3806
#define TS_TRIG_BUFFER1_H					0x3808
#define TS_TRIG_BUFFER1_L					0x380A
#define TS_TRIG_BUFFER0_H					0x380C
#define TS_TRIG_BUFFER0_L					0x380E
/************************************************/
/************** END SCOPE REGISTERS *************/
/************************************************/

#endif
