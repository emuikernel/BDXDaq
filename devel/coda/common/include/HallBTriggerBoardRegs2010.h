#ifndef HallBTriggerBoardRegs_H
#define HallBTriggerBoardRegs_H

typedef struct
{
	volatile unsigned int TOFP1_Delay;
	volatile unsigned int TOFP2_Delay;
	volatile unsigned int TOFP3_Delay;
	volatile unsigned int ST1_Delay;
	volatile unsigned int ST2_Delay;
	volatile unsigned int ST3_Delay;
	volatile unsigned int ST4_Delay;
	volatile unsigned int CC_Delay;
	volatile unsigned int ECinP_Delay;
	volatile unsigned int ECtotP_Delay;
	volatile unsigned int ECinE_Delay;
	volatile unsigned int ECtotE_Delay;

	volatile unsigned int ST1_Scaler;
	volatile unsigned int ST2_Scaler;
	volatile unsigned int ST3_Scaler;
	volatile unsigned int ST4_Scaler;
	volatile unsigned int CC_Scaler;
	volatile unsigned int ECinP_Scaler;
	volatile unsigned int ECinE_Scaler;
	volatile unsigned int ECtotP_Scaler;
	volatile unsigned int ECtotE_Scaler;
	volatile unsigned int TOFP1_Scaler;
	volatile unsigned int TOFP2_Scaler;
	volatile unsigned int TOFP3_Scaler;
	volatile unsigned int STOF_Scaler;
	volatile unsigned int ECP_Scaler;
	volatile unsigned int ECE_Scaler;
	volatile unsigned int ECPCC_Scaler;
	volatile unsigned int ECECC_Scaler;
	volatile unsigned int STS_Scaler;

	volatile unsigned int CFGSector;

	volatile unsigned int Reserved1[33];
} SectorPeripheral;


typedef struct
{
	volatile unsigned int CFGTrigger;

	volatile unsigned int LUT_Scaler;
    volatile unsigned int LUTMOR_Scaler;
	volatile unsigned int Prescale_Scaler;
	volatile unsigned int Trig_Prescale;

	volatile unsigned int Trig_LUT;
	volatile unsigned int TrigA_LUT[6];
	volatile unsigned int TrigB_LUT[6];

	volatile unsigned int Reserved1[46];
} TriggerbitPeripheral;


typedef struct
{
	volatile unsigned int TrigStatus;
	volatile unsigned int TrigValues[4];
	volatile unsigned int TrigerIgnore[4];
	volatile unsigned int TrigBuffer;
	
	volatile unsigned int Reserved1[54];
} ScopePeripheral;


typedef struct
{
	volatile unsigned int Trig_LUT;
	volatile unsigned int Sector_LUT;
	volatile unsigned int L2_Delay;
	volatile unsigned int L2_Width;

	volatile unsigned int Reserved1[60];
} L2LogicPeripheral;


typedef struct
{
	volatile unsigned int STMult_Scaler;
	volatile unsigned int MORA_Scaler;
	volatile unsigned int MORB_Scaler;
	volatile unsigned int MORAB_Scaler;
	volatile unsigned int CFGSTMult;
	volatile unsigned int REF_Scaler;
	volatile unsigned int EnableScalers;
	volatile unsigned int Revision;
	volatile unsigned int MORA_Delay;
	volatile unsigned int MORB_Delay;
	volatile unsigned int EventData;
	volatile unsigned int CFGEventData;

	volatile unsigned int Reserved1[52];
} GlobalLogicPeripheral;

typedef struct
{
  /* 0x0000-0x0FFF */	unsigned int			BLT32Space[1024];
  /* 0x1000-0x15FF */	SectorPeripheral		Sector[6];
  /* 0x1600-0x1FFF */ unsigned int			Reserved1[640];
  /* 0x2000-0x2BFF */	TriggerbitPeripheral	TriggerBit[12];
  /* 0x2C00-0x2FFF */ unsigned int			Reserved2[256];
  /* 0x3000-0x30FF */	GlobalLogicPeripheral	GlobalLogic;    /*must NOT be compared except 10,20,24*/
  /* 0x3100-0x31FF */	L2LogicPeripheral		L2Logic;
  /* 0x3200-0x32FF */	ScopePeripheral			Scope;
} HallBTriggerStruct;

/* Board Supports VME A32/A24 D32 Accesses (BLT32 only in address range 0x0000-0x0FFC */

#define CLOCK_PERIOD_NS					5
#define	MAX_PRESCALE					1023
#define MAX_DELAY_LONG					1023
#define MAX_DELAY						31
#define MAX_STMULT						24
#define MAX_PERSIST_LONG				255
#define MAX_PERSIST						7

#define TS_BOARD_ADDRESS				0x08510000

#define A_BOARDIDS						0x1300

#define BOARDID_A395A 					0x00	// 32CH IN LVDS/ECL INTERFACE
#define BOARDID_A395B 					0x01	// 32CH OUT LVDS INTERFACE
#define BOARDID_A395C 					0x02	// 32CH OUT ECL INTERFACE
#define BOARDID_A395D					0x03	// 8CH I/O SELECT NIM/TTL INTER

#define TS_REVISION						0x301C

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
#define TS_ENABLE_SCALERS				0x3018

#define TS_MORA_SCALER					0x3004
#define TS_MORB_SCALER					0x3008

#define TS_TOFP1_1_SCALER				0x1054
#define TS_TOFP2_1_SCALER				0x1058
#define TS_TOFP3_1_SCALER				0x105C
#define TS_TOFP1_2_SCALER				0x1154
#define TS_TOFP2_2_SCALER				0x1158
#define TS_TOFP3_2_SCALER				0x115C
#define TS_TOFP1_3_SCALER				0x1254
#define TS_TOFP2_3_SCALER				0x1258
#define TS_TOFP3_3_SCALER				0x125C
#define TS_TOFP1_4_SCALER				0x1354
#define TS_TOFP2_4_SCALER				0x1358
#define TS_TOFP3_4_SCALER				0x135C
#define TS_TOFP1_5_SCALER				0x1454
#define TS_TOFP2_5_SCALER				0x1458
#define TS_TOFP3_5_SCALER				0x145C
#define TS_TOFP1_6_SCALER				0x1554
#define TS_TOFP2_6_SCALER				0x1558
#define TS_TOFP3_6_SCALER				0x155C

#define TS_ECin1P_SCALER				0x1044
#define TS_ECin2P_SCALER				0x1144
#define TS_ECin3P_SCALER				0x1244
#define TS_ECin4P_SCALER				0x1344
#define TS_ECin5P_SCALER				0x1444
#define TS_ECin6P_SCALER				0x1544

#define TS_ECin1E_SCALER				0x1048
#define TS_ECin2E_SCALER				0x1148
#define TS_ECin3E_SCALER				0x1248
#define TS_ECin4E_SCALER				0x1348
#define TS_ECin5E_SCALER				0x1448
#define TS_ECin6E_SCALER				0x1548

#define TS_ECtot1P_SCALER				0x104C
#define TS_ECtot2P_SCALER				0x114C
#define TS_ECtot3P_SCALER				0x124C
#define TS_ECtot4P_SCALER				0x134C
#define TS_ECtot5P_SCALER				0x144C
#define TS_ECtot6P_SCALER				0x154C

#define TS_ECtot1E_SCALER				0x1050
#define TS_ECtot2E_SCALER				0x1150
#define TS_ECtot3E_SCALER				0x1250
#define TS_ECtot4E_SCALER				0x1350
#define TS_ECtot5E_SCALER				0x1450
#define TS_ECtot6E_SCALER				0x1550

#define TS_CC1_SCALER					0x1040
#define TS_CC2_SCALER					0x1140
#define TS_CC3_SCALER					0x1240
#define TS_CC4_SCALER					0x1340
#define TS_CC5_SCALER					0x1440
#define TS_CC6_SCALER					0x1540

#define TS_ST1_SCALER					0x1030
#define TS_ST2_SCALER					0x1034
#define TS_ST3_SCALER					0x1038
#define TS_ST4_SCALER					0x103C
#define TS_ST5_SCALER					0x1130
#define TS_ST6_SCALER					0x1134
#define TS_ST7_SCALER					0x1138
#define TS_ST8_SCALER					0x113C
#define TS_ST9_SCALER					0x1230
#define TS_ST10_SCALER					0x1234
#define TS_ST11_SCALER					0x1238
#define TS_ST12_SCALER					0x123C
#define TS_ST13_SCALER					0x1330
#define TS_ST14_SCALER					0x1334
#define TS_ST15_SCALER					0x1338
#define TS_ST16_SCALER					0x133C
#define TS_ST17_SCALER					0x1430
#define TS_ST18_SCALER					0x1434
#define TS_ST19_SCALER					0x1438
#define TS_ST20_SCALER					0x143C
#define TS_ST21_SCALER					0x1530
#define TS_ST22_SCALER					0x1534
#define TS_ST23_SCALER					0x1538
#define TS_ST24_SCALER					0x153C

#define TS_LUT_SCALER_TRIG1				0x2004
#define TS_LUT_SCALER_TRIG2				0x2104
#define TS_LUT_SCALER_TRIG3				0x2204
#define TS_LUT_SCALER_TRIG4				0x2304
#define TS_LUT_SCALER_TRIG5				0x2404
#define TS_LUT_SCALER_TRIG6				0x2504
#define TS_LUT_SCALER_TRIG7				0x2604
#define TS_LUT_SCALER_TRIG8				0x2704
#define TS_LUT_SCALER_TRIG9				0x2804
#define TS_LUT_SCALER_TRIG10			0x2904
#define TS_LUT_SCALER_TRIG11			0x2A04
#define TS_LUT_SCALER_TRIG12			0x2B04

#define TS_LUT_MOR_SCALER_TRIG1			0x2008
#define TS_LUT_MOR_SCALER_TRIG2			0x2108
#define TS_LUT_MOR_SCALER_TRIG3			0x2208
#define TS_LUT_MOR_SCALER_TRIG4			0x2308
#define TS_LUT_MOR_SCALER_TRIG5			0x2408
#define TS_LUT_MOR_SCALER_TRIG6			0x2508
#define TS_LUT_MOR_SCALER_TRIG7			0x2608
#define TS_LUT_MOR_SCALER_TRIG8			0x2708
#define TS_LUT_MOR_SCALER_TRIG9			0x2808
#define TS_LUT_MOR_SCALER_TRIG10		0x2908
#define TS_LUT_MOR_SCALER_TRIG11		0x2A08
#define TS_LUT_MOR_SCALER_TRIG12		0x2B08

#define TS_TRIG_PRESCALE_SCALER_TRIG1	0x200C
#define TS_TRIG_PRESCALE_SCALER_TRIG2	0x210C
#define TS_TRIG_PRESCALE_SCALER_TRIG3	0x220C
#define TS_TRIG_PRESCALE_SCALER_TRIG4	0x230C
#define TS_TRIG_PRESCALE_SCALER_TRIG5	0x240C
#define TS_TRIG_PRESCALE_SCALER_TRIG6	0x250C
#define TS_TRIG_PRESCALE_SCALER_TRIG7	0x260C
#define TS_TRIG_PRESCALE_SCALER_TRIG8	0x270C
#define TS_TRIG_PRESCALE_SCALER_TRIG9	0x280C
#define TS_TRIG_PRESCALE_SCALER_TRIG10	0x290C
#define TS_TRIG_PRESCALE_SCALER_TRIG11	0x2A0C
#define TS_TRIG_PRESCALE_SCALER_TRIG12	0x2B0C

#define TS_STS1_SCALER					0x1074
#define TS_STS2_SCALER					0x1174
#define TS_STS3_SCALER					0x1274
#define TS_STS4_SCALER					0x1374
#define TS_STS5_SCALER					0x1474
#define TS_STS6_SCALER					0x1574

#define TS_STOF1_SCALER					0x1060
#define TS_STOF2_SCALER					0x1160
#define TS_STOF3_SCALER					0x1260
#define TS_STOF4_SCALER					0x1360
#define TS_STOF5_SCALER					0x1460
#define TS_STOF6_SCALER					0x1560

#define TS_EC1P_SCALER					0x1064
#define TS_EC2P_SCALER					0x1164
#define TS_EC3P_SCALER					0x1264
#define TS_EC4P_SCALER					0x1364
#define TS_EC5P_SCALER					0x1464
#define TS_EC6P_SCALER					0x1564

#define TS_EC1E_SCALER					0x1068
#define TS_EC2E_SCALER					0x1168
#define TS_EC3E_SCALER					0x1268
#define TS_EC4E_SCALER					0x1368
#define TS_EC5E_SCALER					0x1468
#define TS_EC6E_SCALER					0x1568

#define TS_ECECC1_SCALER				0x1070
#define TS_ECECC2_SCALER				0x1170
#define TS_ECECC3_SCALER				0x1270
#define TS_ECECC4_SCALER				0x1370
#define TS_ECECC5_SCALER				0x1470
#define TS_ECECC6_SCALER				0x1570

#define TS_ECPCC1_SCALER				0x106C
#define TS_ECPCC2_SCALER				0x116C
#define TS_ECPCC3_SCALER				0x126C
#define TS_ECPCC4_SCALER				0x136C
#define TS_ECPCC5_SCALER				0x146C
#define TS_ECPCC6_SCALER				0x156C

#define TS_REF_SCALER					0x3014
#define TS_MORAB_SCALER					0x300C
#define TS_STMULT_SCALER				0x3000
/************************************************/
/************** END SCALER REGISTERS ************/
/************************************************/

/************************************************/
/************** BEGIN PRESCALER REGISTERS *******/
/************************************************/
/* Notes:
   1) TS_PRESCALE_TRIGx sets the prescale value for trigger bit x
   2) Range is from 0 to 1023, where 0 disables output.
*/
#define TS_PRESCALE_TRIG1				0x2010
#define TS_PRESCALE_TRIG2				0x2110
#define TS_PRESCALE_TRIG3				0x2210
#define TS_PRESCALE_TRIG4				0x2310
#define TS_PRESCALE_TRIG5				0x2410
#define TS_PRESCALE_TRIG6				0x2510
#define TS_PRESCALE_TRIG7				0x2610
#define TS_PRESCALE_TRIG8				0x2710
#define TS_PRESCALE_TRIG9				0x2810
#define TS_PRESCALE_TRIG10				0x2910
#define TS_PRESCALE_TRIG11				0x2A10
#define TS_PRESCALE_TRIG12				0x2B10
/************************************************/
/************** END PRESCALER REGISTERS *********/
/************************************************/

/************************************************/
/************** BEGIN DELAY REGISTERS **********/
/************************************************/
/* Note: Delays occupy bits(4:0) of the register.
         Each count is a 5ns delay.
		 Range is 0ns (reg = 0) to 155ns (reg = 31)
*/
#define TS_MORA_DELAY					0x3020
#define TS_MORB_DELAY					0x3024

#define TS_TOFP1_1_DELAY				0x1000
#define TS_TOFP2_1_DELAY				0x1004
#define TS_TOFP3_1_DELAY				0x1008
#define TS_TOFP1_2_DELAY				0x1100
#define TS_TOFP2_2_DELAY				0x1104
#define TS_TOFP3_2_DELAY				0x1108
#define TS_TOFP1_3_DELAY				0x1200
#define TS_TOFP2_3_DELAY				0x1204
#define TS_TOFP3_3_DELAY				0x1208
#define TS_TOFP1_4_DELAY				0x1300
#define TS_TOFP2_4_DELAY				0x1304
#define TS_TOFP3_4_DELAY				0x1308
#define TS_TOFP1_5_DELAY				0x1400
#define TS_TOFP2_5_DELAY				0x1404
#define TS_TOFP3_5_DELAY				0x1408
#define TS_TOFP1_6_DELAY				0x1500
#define TS_TOFP2_6_DELAY				0x1504
#define TS_TOFP3_6_DELAY				0x1508

#define TS_ECin1P_DELAY					0x1020
#define TS_ECin2P_DELAY					0x1120
#define TS_ECin3P_DELAY					0x1220
#define TS_ECin4P_DELAY					0x1320
#define TS_ECin5P_DELAY					0x1420
#define TS_ECin6P_DELAY					0x1520

#define TS_ECin1E_DELAY					0x1028
#define TS_ECin2E_DELAY					0x1128
#define TS_ECin3E_DELAY					0x1228
#define TS_ECin4E_DELAY					0x1328
#define TS_ECin5E_DELAY					0x1428
#define TS_ECin6E_DELAY					0x1528

#define TS_ECtot1P_DELAY				0x1024
#define TS_ECtot2P_DELAY				0x1124
#define TS_ECtot3P_DELAY				0x1224
#define TS_ECtot4P_DELAY				0x1324
#define TS_ECtot5P_DELAY				0x1424
#define TS_ECtot6P_DELAY				0x1524

#define TS_ECtot1E_DELAY				0x102C
#define TS_ECtot2E_DELAY				0x112C
#define TS_ECtot3E_DELAY				0x122C
#define TS_ECtot4E_DELAY				0x132C
#define TS_ECtot5E_DELAY				0x142C
#define TS_ECtot6E_DELAY				0x152C

#define TS_CC1_DELAY					0x101C
#define TS_CC2_DELAY					0x111C
#define TS_CC3_DELAY					0x121C
#define TS_CC4_DELAY					0x131C
#define TS_CC5_DELAY					0x141C
#define TS_CC6_DELAY					0x151C

#define TS_ST1_DELAY					0x100C
#define TS_ST2_DELAY					0x1010
#define TS_ST3_DELAY					0x1014
#define TS_ST4_DELAY					0x1018
#define TS_ST5_DELAY					0x110C
#define TS_ST6_DELAY					0x1110
#define TS_ST7_DELAY					0x1114
#define TS_ST8_DELAY					0x1118
#define TS_ST9_DELAY					0x120C
#define TS_ST10_DELAY					0x1210
#define TS_ST11_DELAY					0x1214
#define TS_ST12_DELAY					0x1218
#define TS_ST13_DELAY					0x130C
#define TS_ST14_DELAY					0x1310
#define TS_ST15_DELAY					0x1314
#define TS_ST16_DELAY					0x1318
#define TS_ST17_DELAY					0x140C
#define TS_ST18_DELAY					0x1410
#define TS_ST19_DELAY					0x1414
#define TS_ST20_DELAY					0x1418
#define TS_ST21_DELAY					0x150C
#define TS_ST22_DELAY					0x1510
#define TS_ST23_DELAY					0x1514
#define TS_ST24_DELAY					0x1518
/************************************************/
/************** END DELAY REGISTERS *************/
/************************************************/

/************************************************/
/******** BEGIN SECTOR TRIGGER REGISTERS ********/
/************************************************/
/* Notes:
	bits (17:16) ECPCC config
		"00" => ECP and CC
		"01" => CC
		"10" => ECP
		"11" => 0

	bits (13:12) ECECC config
		"00" => ECE and CC
		"01" => CC
		"10" => ECE
		"11" => 0

	bits (9:8)   STOFP1 config
		"00" => ST and TOFP1
		"01" => TOFP1
		"10" => ST
		"11" => 0

	bits (5:4)   STOFP2 config
		"00" => ST and TOFP2
		"01" => TOFP2
		"10" => ST
		"11" => 0

	bits (1:0)   STOFP3 config
		"00" => ST and TOFP3
		"01" => TOFP3
		"10" => ST
		"11" => 0
*/
#define TS_SECTOR1_CFG					0x1078
#define TS_SECTOR2_CFG					0x1178
#define TS_SECTOR3_CFG					0x1278
#define TS_SECTOR4_CFG					0x1378
#define TS_SECTOR5_CFG					0x1478
#define TS_SECTOR6_CFG					0x1578

/************************************************/
/********* END SECTOR TRIGGER REGISTERS *********/
/************************************************/


/************************************************/
/********* BEGIN MOR TRIGGER REGISTERS **********/
/************************************************/
/* Notes:
	bits (5:4)   MORAB config
		"00" => TRIG and (MORA or MORB)
		"01" => TRIG and MORA
		"10" => TRIG and MORB
		"11" => TRIG

	bits (1:0)   ST Multiplicity config
		"00" => TRIG and STMULT
		"01" => STMULT
		"10" => STMULT
		"11" => TRIG
*/

#define TS_TRIG1_CFG					0x2000
#define TS_TRIG2_CFG					0x2100
#define TS_TRIG3_CFG					0x2200
#define TS_TRIG4_CFG					0x2300
#define TS_TRIG5_CFG					0x2400
#define TS_TRIG6_CFG					0x2500
#define TS_TRIG7_CFG					0x2600
#define TS_TRIG8_CFG					0x2700
#define TS_TRIG9_CFG					0x2800
#define TS_TRIG10_CFG					0x2900
#define TS_TRIG11_CFG					0x2A00
#define TS_TRIG12_CFG					0x2B00

/************************************************/
/********** END MOR TRIGGER REGISTERS ***********/
/************************************************/


/************************************************/
/**** BEGIN ST MULTIPLICITY TRIGGER REGISTERS ***/
/************************************************/
/* Notes:
	1) bits 5:9 => ST Multiplicity Max
	2) bits 0:4 => ST Multiplicity Min
*/
#define TS_STMULT_THRESHOLD				0x3010

/************************************************/
/**** END ST MULTIPLICITY TRIGGER REGISTERS *****/
/************************************************/

/************************************************/
/******* BEGIN L2 SECTOR TRIGGER REGISTERS ******/
/************************************************/
/* Notes:
	1) L2_SECLOGIC is a 5:1 LUT with sector inputs:
	    Address Input 0: STOFP1
	    Address Input 1: STOFP2
	    Address Input 2: STOFP3
		Address Input 3: ECPCC
		Address Input 4: ECECC
	2) L2OUTPUTDELAY delays L2 10b Latch signals in 5ns
	   steps. Values: min. 2*5ns to max. 1023*5ns
	3) L2_OUTPUTWIDTH reshapes width from 1*5ns to 255*5ns
*/
#define TS_L2_SECLOGIC					0x3104
#define TS_L2_OUTPUTDELAY				0x3108
#define TS_L2_OUTPUTWIDTH				0x310C
/************************************************/
/******** END L2 SECTOR TRIGGER REGISTERS *******/
/************************************************/

/************************************************/
/********* BEGIN EVENT READOUT REGISTERS ********/
/************************************************/
/* Notes:
	1) TS_EVENT_CFG:
	   bit 31     => Write Only: '1' to clear event FIFO and event counter
	   bits 15:8  => Trigger bit capture stretch width (in 5ns steps)
	   bits 23:16 => Trigger bit delay setting (in 5ns steps)
	   bits 7:0   => Read Only: Number of words in event FIFO

	2) TS_EVENT_DATA:
	   bit 31     => fifo full flag
	   bits 30:24 => number of words in fifo
	   bits 23:12 => 12bit event counter (L1ACCEPT counter)
	   bits 11:0  => captured 12bit trigger pattern
*/
#define TS_EVENT_DATA				0x3028
#define TS_EVENT_CFG				0x302C

/************************************************/
/**** END ST MULTIPLICITY TRIGGER REGISTERS *****/
/************************************************/

/************************************************/
/************** BEGIN LUT CONTROL REGISTERS *****/
/************************************************/
/* Notes:
   1) See document "HallBTriggerMarch2010_Description.pdf" on how these LUTs are wired for triggers.
	  
        LUT:   LUT Size:


LUT A[1-6]:
		TS_CFG_LUTAs_t          4:1 LUT: 16 x 1bit LUTs
		   s => Sector Number [1:6]
		   t => Trigger Number [1:12]

		LUT Address Mapping (16 x 1bit):
		Address Bit		Signal Mapping
		-----------     --------------
		0				ECPCC_s
		1				STOFP1_s
		2				STOFP2_s
		3				STOFP3_s


LUT B[1-6]:
    	TS_CFG_LUTBs_t          4:1 LUT: 16 x 1bit LUTs
		   s => Sector Number [1:6]
		   t => Trigger Number [1:12]

		LUT Address Mapping (16 x 1bit):
		Address Bit		Signal Mapping
		-----------     --------------
		0				ECECC_s
		1				STOFP1_s
		2				STOFP2_s
		3				STOFP3_s

LUT[1-12]:
		TS_CFG_LUT[1-12]        128Word x 32bit -> 4096 x 1bit LUTs
        
		LUT Address Mapping (4096 x 1bit):
		Address Bit		Signal Mapping
		-----------     --------------
		0				LUT A1
		1				LUT A2
		2				LUT A3
		3				LUT A4
		4				LUT A5
		5				LUT A6
		6				LUT B1
		7				LUT B2
		8				LUT B3
		9				LUT B4
		10				LUT B5
		11				LUT B6

  
L2_LUT
		TS_CFG_LUT_L2			512Word x 32bit -> 4096 x 4bit LUT

		L2 LUT Address Mapping (4096 x 4bit):
		Address Bit		Signal Mapping
		-----------     --------------
		0				Trigger 1
		1				Trigger 2
		2				Trigger 3
		3				Trigger 4
		4				Trigger 5
		5				Trigger 6
		6				Trigger 7
		7				Trigger 8
		8				Trigger 9
		9				Trigger 10
		10				Trigger 11
		11				Trigger 12
		
 */
#define TS_CFG_LUT_1					0x2014
#define TS_CFG_LUTA1_1					0x2018
#define TS_CFG_LUTA2_1					0x201C
#define TS_CFG_LUTA3_1					0x2020
#define TS_CFG_LUTA4_1					0x2024
#define TS_CFG_LUTA5_1					0x2028
#define TS_CFG_LUTA6_1					0x202C
#define TS_CFG_LUTB1_1					0x2030
#define TS_CFG_LUTB2_1					0x2034
#define TS_CFG_LUTB3_1					0x2038
#define TS_CFG_LUTB4_1					0x203C
#define TS_CFG_LUTB5_1					0x2040
#define TS_CFG_LUTB6_1					0x2044

#define TS_CFG_LUT_2					0x2114
#define TS_CFG_LUTA1_2					0x2118
#define TS_CFG_LUTA2_2					0x211C
#define TS_CFG_LUTA3_2					0x2120
#define TS_CFG_LUTA4_2					0x2124
#define TS_CFG_LUTA5_2					0x2128
#define TS_CFG_LUTA6_2					0x212C
#define TS_CFG_LUTB1_2					0x2130
#define TS_CFG_LUTB2_2					0x2134
#define TS_CFG_LUTB3_2					0x2138
#define TS_CFG_LUTB4_2					0x213C
#define TS_CFG_LUTB5_2					0x2140
#define TS_CFG_LUTB6_2					0x2144

#define TS_CFG_LUT_3					0x2214
#define TS_CFG_LUTA1_3					0x2218
#define TS_CFG_LUTA2_3					0x221C
#define TS_CFG_LUTA3_3					0x2220
#define TS_CFG_LUTA4_3					0x2224
#define TS_CFG_LUTA5_3					0x2228
#define TS_CFG_LUTA6_3					0x222C
#define TS_CFG_LUTB1_3					0x2230
#define TS_CFG_LUTB2_3					0x2234
#define TS_CFG_LUTB3_3					0x2238
#define TS_CFG_LUTB4_3					0x223C
#define TS_CFG_LUTB5_3					0x2240
#define TS_CFG_LUTB6_3					0x2244

#define TS_CFG_LUT_4					0x2314
#define TS_CFG_LUTA1_4					0x2318
#define TS_CFG_LUTA2_4					0x231C
#define TS_CFG_LUTA3_4					0x2320
#define TS_CFG_LUTA4_4					0x2324
#define TS_CFG_LUTA5_4					0x2328
#define TS_CFG_LUTA6_4					0x232C
#define TS_CFG_LUTB1_4					0x2330
#define TS_CFG_LUTB2_4					0x2334
#define TS_CFG_LUTB3_4					0x2338
#define TS_CFG_LUTB4_4					0x233C
#define TS_CFG_LUTB5_4					0x2340
#define TS_CFG_LUTB6_4					0x2344

#define TS_CFG_LUT_5					0x2414
#define TS_CFG_LUTA1_5					0x2418
#define TS_CFG_LUTA2_5					0x241C
#define TS_CFG_LUTA3_5					0x2420
#define TS_CFG_LUTA4_5					0x2424
#define TS_CFG_LUTA5_5					0x2428
#define TS_CFG_LUTA6_5					0x242C
#define TS_CFG_LUTB1_5					0x2430
#define TS_CFG_LUTB2_5					0x2434
#define TS_CFG_LUTB3_5					0x2438
#define TS_CFG_LUTB4_5					0x243C
#define TS_CFG_LUTB5_5					0x2440
#define TS_CFG_LUTB6_5					0x2444

#define TS_CFG_LUT_6					0x2514
#define TS_CFG_LUTA1_6					0x2518
#define TS_CFG_LUTA2_6					0x251C
#define TS_CFG_LUTA3_6					0x2520
#define TS_CFG_LUTA4_6					0x2524
#define TS_CFG_LUTA5_6					0x2528
#define TS_CFG_LUTA6_6					0x252C
#define TS_CFG_LUTB1_6					0x2530
#define TS_CFG_LUTB2_6					0x2534
#define TS_CFG_LUTB3_6					0x2538
#define TS_CFG_LUTB4_6					0x253C
#define TS_CFG_LUTB5_6					0x2540
#define TS_CFG_LUTB6_6					0x2544

#define TS_CFG_LUT_7					0x2614
#define TS_CFG_LUTA1_7					0x2618
#define TS_CFG_LUTA2_7					0x261C
#define TS_CFG_LUTA3_7					0x2620
#define TS_CFG_LUTA4_7					0x2624
#define TS_CFG_LUTA5_7					0x2628
#define TS_CFG_LUTA6_7					0x262C
#define TS_CFG_LUTB1_7					0x2630
#define TS_CFG_LUTB2_7					0x2634
#define TS_CFG_LUTB3_7					0x2638
#define TS_CFG_LUTB4_7					0x263C
#define TS_CFG_LUTB5_7					0x2640
#define TS_CFG_LUTB6_7					0x2644

#define TS_CFG_LUT_8					0x2714
#define TS_CFG_LUTA1_8					0x2718
#define TS_CFG_LUTA2_8					0x271C
#define TS_CFG_LUTA3_8					0x2720
#define TS_CFG_LUTA4_8					0x2724
#define TS_CFG_LUTA5_8					0x2728
#define TS_CFG_LUTA6_8					0x272C
#define TS_CFG_LUTB1_8					0x2730
#define TS_CFG_LUTB2_8					0x2734
#define TS_CFG_LUTB3_8					0x2738
#define TS_CFG_LUTB4_8					0x273C
#define TS_CFG_LUTB5_8					0x2740
#define TS_CFG_LUTB6_8					0x2744

#define TS_CFG_LUT_9					0x2814
#define TS_CFG_LUTA1_9					0x2818
#define TS_CFG_LUTA2_9					0x281C
#define TS_CFG_LUTA3_9					0x2820
#define TS_CFG_LUTA4_9					0x2824
#define TS_CFG_LUTA5_9					0x2828
#define TS_CFG_LUTA6_9					0x282C
#define TS_CFG_LUTB1_9					0x2830
#define TS_CFG_LUTB2_9					0x2834
#define TS_CFG_LUTB3_9					0x2838
#define TS_CFG_LUTB4_9					0x283C
#define TS_CFG_LUTB5_9					0x2840
#define TS_CFG_LUTB6_9					0x2844

#define TS_CFG_LUT_10					0x2914
#define TS_CFG_LUTA1_10					0x2918
#define TS_CFG_LUTA2_10					0x291C
#define TS_CFG_LUTA3_10					0x2920
#define TS_CFG_LUTA4_10					0x2924
#define TS_CFG_LUTA5_10					0x2928
#define TS_CFG_LUTA6_10					0x292C
#define TS_CFG_LUTB1_10					0x2930
#define TS_CFG_LUTB2_10					0x2934
#define TS_CFG_LUTB3_10					0x2938
#define TS_CFG_LUTB4_10					0x293C
#define TS_CFG_LUTB5_10					0x2940
#define TS_CFG_LUTB6_10					0x2944

#define TS_CFG_LUT_11					0x2A14
#define TS_CFG_LUTA1_11					0x2A18
#define TS_CFG_LUTA2_11					0x2A1C
#define TS_CFG_LUTA3_11					0x2A20
#define TS_CFG_LUTA4_11					0x2A24
#define TS_CFG_LUTA5_11					0x2A28
#define TS_CFG_LUTA6_11					0x2A2C
#define TS_CFG_LUTB1_11					0x2A30
#define TS_CFG_LUTB2_11					0x2A34
#define TS_CFG_LUTB3_11					0x2A38
#define TS_CFG_LUTB4_11					0x2A3C
#define TS_CFG_LUTB5_11					0x2A40
#define TS_CFG_LUTB6_11					0x2A44

#define TS_CFG_LUT_12					0x2B14
#define TS_CFG_LUTA1_12					0x2B18
#define TS_CFG_LUTA2_12					0x2B1C
#define TS_CFG_LUTA3_12					0x2B20
#define TS_CFG_LUTA4_12					0x2B24
#define TS_CFG_LUTA5_12					0x2B28
#define TS_CFG_LUTA6_12					0x2B2C
#define TS_CFG_LUTB1_12					0x2B30
#define TS_CFG_LUTB2_12					0x2B34
#define TS_CFG_LUTB3_12					0x2B38
#define TS_CFG_LUTB4_12					0x2B3C
#define TS_CFG_LUTB5_12					0x2B40
#define TS_CFG_LUTB6_12					0x2B44


#define TS_CFG_LUT_L2					0x3100
/************************************************/
/*************** END LUT CONTROL REGISTERS ******/
/************************************************/

#endif
