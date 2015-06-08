#ifndef V1495VMEFLEXREMOTE_H
#define V1495VMEFLEXREMOTE_H

#define V1495_VMESERVER_PORT	6001    /* server's port number for bind() */
#define REQUEST_MSG_SIZE		2048
#define V1495_BLK_MAX_LEN		512

#define V1495_CMD_SETBASE		0x00
#define V1495_CMD_READ16		0x01
#define V1495_CMD_WRITE16		0x02
#define V1495_CMD_BLKREAD16		0x03
#define V1495_CMD_BLKWRITE16	0x04
#define V1495_CMD_GETSTATUS		0x05
#define V1495_CMD_GETSCOPEBUF	0x06

#define V1495_VERIFY_OK			1
#define V1495_VERIFY_BAD		0

#define V1495_CMD_RSP(cmd)			(cmd | 0x40)
#define V1495_CMD_ERR(cmd)			(cmd & 0x80)

#ifdef WIN32
	#pragma pack(push)
	#pragma pack(1)
#else
/*#error "Set structure packing to bytes here"*/
#endif

typedef struct
{
	unsigned char Msgs[REQUEST_MSG_SIZE];
} V1495VMERemote;

typedef struct
{
	unsigned short Offset;
	unsigned short Count;
} CMD_GetScopeBuf;

typedef struct
{
	unsigned short Values[768];
} CMD_GetScopeBuf_Rsp;

typedef struct
{
	unsigned int Address;
} CMD_SetBase;

typedef struct
{
	unsigned short Offset;
} CMD_Read16;

typedef struct
{
	unsigned short Value;
} CMD_Read16_Rsp;

typedef struct
{
	unsigned short Offset;
	unsigned short Value;
} CMD_Write16;

typedef struct
{
	unsigned short Offset;
	unsigned short Count;
} CMD_BlkRead16;

typedef struct
{
	unsigned short Values[V1495_BLK_MAX_LEN];
} CMD_BlkRead16_Rsp;

typedef struct
{
	unsigned short Offset;
	unsigned short Count;
	unsigned short Verify;
	unsigned short Values[V1495_BLK_MAX_LEN];
} CMD_BlkWrite16;

typedef struct
{
	unsigned char Verified;
} CMD_BlkWrite16_Rsp;

#define TS_REVISION										0x0002	/* Firmware Revision => 0x0102 = V1.02 */
#define TS_ENABLE_SCALERS								0x0800	/* 0->1 resets all scalers. 1 to enable scalers.
                                                                   0 to disable and allow readout */

/* Scalers with _H and _L indicates a 32bit scaler; otherwise scaler is 16bit */
/* See Pawel's document for more detail regarding scaler descriptions         */
/* TS_REF_SCALER_H,TS_REF_SCALER_L is a 32bit reference scaler that indicates */
/* the total counts (in 5ns increments) that all scalers have accumulated over*/
#define TS_MORA_SCALER_H								0x1000
#define TS_MORA_SCALER_L								0x1002
#define TS_MORB_SCALER_H								0x1004
#define TS_MORB_SCALER_L								0x1006
#define TS_TOF1_SCALER_H								0x1008
#define TS_TOF1_SCALER_L								0x100A
#define TS_TOF2_SCALER_H								0x100C
#define TS_TOF2_SCALER_L								0x100E
#define TS_TOF3_SCALER_H								0x1010
#define TS_TOF3_SCALER_L								0x1012
#define TS_TOF4_SCALER_H								0x1014
#define TS_TOF4_SCALER_L								0x1016
#define TS_TOF5_SCALER_H								0x1018
#define TS_TOF5_SCALER_L								0x101A
#define TS_TOF6_SCALER_H								0x101C
#define TS_TOF6_SCALER_L								0x101E
#define TS_ECin1P_SCALER_H								0x1020
#define TS_ECin1P_SCALER_L								0x1022
#define TS_ECin2P_SCALER_H								0x1024
#define TS_ECin2P_SCALER_L								0x1026
#define TS_ECin3P_SCALER_H								0x1028
#define TS_ECin3P_SCALER_L								0x102A
#define TS_ECin4P_SCALER_H								0x102C
#define TS_ECin4P_SCALER_L								0x102E
#define TS_ECin5P_SCALER_H								0x1030
#define TS_ECin5P_SCALER_L								0x1032
#define TS_ECin6P_SCALER_H								0x1034
#define TS_ECin6P_SCALER_L								0x1036
#define TS_ECin1E_SCALER_H								0x1038
#define TS_ECin1E_SCALER_L								0x103A
#define TS_ECin2E_SCALER_H								0x103C
#define TS_ECin2E_SCALER_L								0x103E
#define TS_ECin3E_SCALER_H								0x1040
#define TS_ECin3E_SCALER_L								0x1042
#define TS_ECin4E_SCALER_H								0x1044
#define TS_ECin4E_SCALER_L								0x1046
#define TS_ECin5E_SCALER_H								0x1048
#define TS_ECin5E_SCALER_L								0x104A
#define TS_ECin6E_SCALER_H								0x104C
#define TS_ECin6E_SCALER_L								0x104E
#define TS_ECtot1P_SCALER_H							0x1050
#define TS_ECtot1P_SCALER_L							0x1052
#define TS_ECtot2P_SCALER_H							0x1054
#define TS_ECtot2P_SCALER_L							0x1056
#define TS_ECtot3P_SCALER_H							0x1058
#define TS_ECtot3P_SCALER_L							0x105A
#define TS_ECtot4P_SCALER_H							0x105C
#define TS_ECtot4P_SCALER_L							0x105E
#define TS_ECtot5P_SCALER_H							0x1060
#define TS_ECtot5P_SCALER_L							0x1062
#define TS_ECtot6P_SCALER_H							0x1064
#define TS_ECtot6P_SCALER_L							0x1066
#define TS_ECtot1E_SCALER_H							0x1068
#define TS_ECtot1E_SCALER_L							0x106A
#define TS_ECtot2E_SCALER_H							0x106C
#define TS_ECtot2E_SCALER_L							0x106E
#define TS_ECtot3E_SCALER_H							0x1070
#define TS_ECtot3E_SCALER_L							0x1072
#define TS_ECtot4E_SCALER_H							0x1074
#define TS_ECtot4E_SCALER_L							0x1076
#define TS_ECtot5E_SCALER_H							0x1078
#define TS_ECtot5E_SCALER_L							0x107A
#define TS_ECtot6E_SCALER_H							0x107C
#define TS_ECtot6E_SCALER_L							0x107E
#define TS_CC1_SCALER_H								0x1080
#define TS_CC1_SCALER_L								0x1082
#define TS_CC2_SCALER_H								0x1084
#define TS_CC2_SCALER_L								0x1086
#define TS_CC3_SCALER_H								0x1088
#define TS_CC3_SCALER_L								0x108A
#define TS_CC4_SCALER_H								0x108C
#define TS_CC4_SCALER_L								0x108E
#define TS_CC5_SCALER_H								0x1090
#define TS_CC5_SCALER_L								0x1092
#define TS_CC6_SCALER_H								0x1094
#define TS_CC6_SCALER_L								0x1096
#define TS_ST1_SCALER_H								0x1098
#define TS_ST1_SCALER_L								0x109A
#define TS_ST2_SCALER_H								0x109C
#define TS_ST2_SCALER_L								0x109E
#define TS_ST3_SCALER_H								0x10A0
#define TS_ST3_SCALER_L								0x10A2
#define TS_ST4_SCALER_H								0x10A4
#define TS_ST4_SCALER_L								0x10A6
#define TS_ST5_SCALER_H								0x10A8
#define TS_ST5_SCALER_L								0x10AA
#define TS_ST6_SCALER_H								0x10AC
#define TS_ST6_SCALER_L								0x10AE
#define TS_ST7_SCALER_H								0x10B0
#define TS_ST7_SCALER_L								0x10B2
#define TS_ST8_SCALER_H								0x10B4
#define TS_ST8_SCALER_L								0x10B6
#define TS_ST9_SCALER_H								0x10B8
#define TS_ST9_SCALER_L								0x10BA
#define TS_ST10_SCALER_H								0x10BC
#define TS_ST10_SCALER_L								0x10BE
#define TS_ST11_SCALER_H								0x10C0
#define TS_ST11_SCALER_L								0x10C2
#define TS_ST12_SCALER_H								0x10C4
#define TS_ST12_SCALER_L								0x10C6
#define TS_ST13_SCALER_H								0x10C8
#define TS_ST13_SCALER_L								0x10CA
#define TS_ST14_SCALER_H								0x10CC
#define TS_ST14_SCALER_L								0x10CE
#define TS_ST15_SCALER_H								0x10D0
#define TS_ST15_SCALER_L								0x10D2
#define TS_ST16_SCALER_H								0x10D4
#define TS_ST16_SCALER_L								0x10D6
#define TS_ST17_SCALER_H								0x10D8
#define TS_ST17_SCALER_L								0x10DA
#define TS_ST18_SCALER_H								0x10DC
#define TS_ST18_SCALER_L								0x10DE
#define TS_ST19_SCALER_H								0x10E0
#define TS_ST19_SCALER_L								0x10E2
#define TS_ST20_SCALER_H								0x10E4
#define TS_ST20_SCALER_L								0x10E6
#define TS_ST21_SCALER_H								0x10E8
#define TS_ST21_SCALER_L								0x10EA
#define TS_ST22_SCALER_H								0x10EC
#define TS_ST22_SCALER_L								0x10EE
#define TS_ST23_SCALER_H								0x10F0
#define TS_ST23_SCALER_L								0x10F2
#define TS_ST24_SCALER_H								0x10F4
#define TS_ST24_SCALER_L								0x10F6
/*															0x10F8*/
/*															0x10FA*/
/*															0x10FC*/
/*															0x10FE*/
/*															0x1100*/
/*															0x1102*/
/*															0x1104*/
/*															0x1106*/
/*															0x1108*/
/*															0x110A*/
/*															0x110C*/
/*															0x110E*/
/*															0x1110*/
/*															0x1112*/
/*															0x1114*/
/*															0x1116*/
/*															0x1118*/
/*															0x111A*/
/*															0x111C*/
/*															0x111E*/
/*															0x1120*/
/*															0x1122*/
/*															0x1124*/
/*															0x1126*/
/*	#define TS_LUT_SCALER_TRIG1_H							0x1128*/
#define TS_LUT_SCALER_TRIG1							0x112A
/*	#define TS_LUT_SCALER_TRIG2_H							0x112C*/
#define TS_LUT_SCALER_TRIG2							0x112E
/*	#define TS_LUT_SCALER_TRIG3_H							0x1130*/
#define TS_LUT_SCALER_TRIG3							0x1132
/*	#define TS_LUT_SCALER_TRIG4_H							0x1134*/
#define TS_LUT_SCALER_TRIG4							0x1136
/*	#define TS_LUT_SCALER_TRIG5_H							0x1138*/
#define TS_LUT_SCALER_TRIG5							0x113A
/*	#define TS_LUT_SCALER_TRIG6_H							0x113C*/
#define TS_LUT_SCALER_TRIG6							0x113E
/*	#define TS_LUT_SCALER_TRIG7_H							0x1140*/
#define TS_LUT_SCALER_TRIG7							0x1142
/*	#define TS_LUT_SCALER_TRIG8_H							0x1144*/
#define TS_LUT_SCALER_TRIG8							0x1146
/*	#define TS_LUT_SCALER_TRIG9_H							0x1148*/
#define TS_LUT_SCALER_TRIG9							0x114A
/*	#define TS_LUT_SCALER_TRIG10_H							0x114C*/
#define TS_LUT_SCALER_TRIG10						0x114E
/*	#define TS_LUT_SCALER_TRIG11_H							0x1150*/
#define TS_LUT_SCALER_TRIG11						0x1152
/*	#define TS_LUT_SCALER_TRIG12_H							0x1154*/
#define TS_LUT_SCALER_TRIG12						0x1156
/*	#define TS_LUT_MOR_SCALER_TRIG1_H						0x1158*/
#define TS_LUT_MOR_SCALER_TRIG1						0x115A
/*	#define TS_LUT_MOR_SCALER_TRIG2_H						0x115C*/
#define TS_LUT_MOR_SCALER_TRIG2						0x115E
/*	#define TS_LUT_MOR_SCALER_TRIG3_H						0x1160*/
#define TS_LUT_MOR_SCALER_TRIG3						0x1162
/*	#define TS_LUT_MOR_SCALER_TRIG4_H						0x1164*/
#define TS_LUT_MOR_SCALER_TRIG4						0x1166
/*	#define TS_LUT_MOR_SCALER_TRIG5_H						0x1168*/
#define TS_LUT_MOR_SCALER_TRIG5						0x116A
/*	#define TS_LUT_MOR_SCALER_TRIG6_H						0x116C*/
#define TS_LUT_MOR_SCALER_TRIG6						0x116E
/*	#define TS_LUT_MOR_SCALER_TRIG7_H						0x1170*/
#define TS_LUT_MOR_SCALER_TRIG7						0x1172
/*	#define TS_LUT_MOR_SCALER_TRIG8_H						0x1174*/
#define TS_LUT_MOR_SCALER_TRIG8						0x1176
/*	#define TS_LUT_MOR_SCALER_TRIG9_H						0x1178*/
#define TS_LUT_MOR_SCALER_TRIG9						0x117A
/*	#define TS_LUT_MOR_SCALER_TRIG10_H						0x117C*/
#define TS_LUT_MOR_SCALER_TRIG10						0x117E
/*	#define TS_LUT_MOR_SCALER_TRIG11_H						0x1180*/
#define TS_LUT_MOR_SCALER_TRIG11						0x1182
/*	#define TS_LUT_MOR_SCALER_TRIG12_H						0x1184*/
#define TS_LUT_MOR_SCALER_TRIG12						0x1186
/*                      								0x1188*/
#define TS_TRIG_PERSIST_SCALER_TRIG1					0x118A
/*                      								0x118C*/
#define TS_TRIG_PERSIST_SCALER_TRIG2					0x118E
/*                      								0x1190*/
#define TS_TRIG_PERSIST_SCALER_TRIG3					0x1192
/*                      								0x1194*/
#define TS_TRIG_PERSIST_SCALER_TRIG4					0x1196
/*                      								0x1198*/
#define TS_TRIG_PERSIST_SCALER_TRIG5					0x119A
/*                      								0x119C*/
#define TS_TRIG_PERSIST_SCALER_TRIG6					0x119E
/*                      								0x11A0*/
#define TS_TRIG_PERSIST_SCALER_TRIG7					0x11A2
/*                      								0x11A4*/
#define TS_TRIG_PERSIST_SCALER_TRIG8					0x11A6
/*                      								0x11A8*/
#define TS_TRIG_PERSIST_SCALER_TRIG9					0x11AA
/*                      								0x11AC*/
#define TS_TRIG_PERSIST_SCALER_TRIG10					0x11AE
/*                      								0x11B0*/
#define TS_TRIG_PERSIST_SCALER_TRIG11					0x11B2
/*                      								0x11B4*/
#define TS_TRIG_PERSIST_SCALER_TRIG12					0x11B6
/*                      								0x11B8*/
#define TS_TRIG_PRESCALE_SCALER_TRIG1					0x11BA
/*                      								0x11BC*/
#define TS_TRIG_PRESCALE_SCALER_TRIG2					0x11BE
/*                      								0x11C0*/
#define TS_TRIG_PRESCALE_SCALER_TRIG3					0x11C2
/*                      								0x11C2*/
#define TS_TRIG_PRESCALE_SCALER_TRIG4					0x11C6
/*                      								0x11C8*/
#define TS_TRIG_PRESCALE_SCALER_TRIG5					0x11CA
/*                      								0x11CC*/
#define TS_TRIG_PRESCALE_SCALER_TRIG6					0x11CE
/*                      								0x11D0*/
#define TS_TRIG_PRESCALE_SCALER_TRIG7					0x11D2
/*                      								0x11D4*/
#define TS_TRIG_PRESCALE_SCALER_TRIG8					0x11D6
/*                      								0x11D8*/
#define TS_TRIG_PRESCALE_SCALER_TRIG9					0x11DA
/*                      								0x11DC*/
#define TS_TRIG_PRESCALE_SCALER_TRIG10					0x11DE
/*                      								0x11E0*/
#define TS_TRIG_PRESCALE_SCALER_TRIG11					0x11E2
/*                      								0x11E4*/
#define TS_TRIG_PRESCALE_SCALER_TRIG12					0x11E6
#define TS_STS1_SCALER_H								0x11E8
#define TS_STS1_SCALER_L								0x11EA
#define TS_STS2_SCALER_H								0x11EC
#define TS_STS2_SCALER_L								0x11EE
#define TS_STS3_SCALER_H								0x11F0
#define TS_STS3_SCALER_L								0x11F2
#define TS_STS4_SCALER_H								0x11F4
#define TS_STS4_SCALER_L								0x11F6
#define TS_STS5_SCALER_H								0x11F8
#define TS_STS5_SCALER_L								0x11FA
#define TS_STS6_SCALER_H								0x11FC
#define TS_STS6_SCALER_L								0x11FE
/*                      								0x1200*/
#define TS_STOF1_SCALER									0x1202
/*                      								0x1204*/
#define TS_STOF2_SCALER									0x1206
/*                      								0x1208*/
#define TS_STOF3_SCALER									0x120A
/*                      								0x120C*/
#define TS_STOF4_SCALER									0x120E
/*                      								0x1210*/
#define TS_STOF5_SCALER									0x1212
/*                      								0x1214*/
#define TS_STOF6_SCALER									0x1216
#define TS_EC1P_SCALER_H								0x1218
#define TS_EC1P_SCALER_L								0x121A
#define TS_EC2P_SCALER_H								0x121C
#define TS_EC2P_SCALER_L								0x121E
#define TS_EC3P_SCALER_H								0x1220
#define TS_EC3P_SCALER_L								0x1222
#define TS_EC4P_SCALER_H								0x1224
#define TS_EC4P_SCALER_L								0x1226
#define TS_EC5P_SCALER_H								0x1228
#define TS_EC5P_SCALER_L								0x122A
#define TS_EC6P_SCALER_H								0x122C
#define TS_EC6P_SCALER_L								0x122E
#define TS_EC1E_SCALER_H								0x1230
#define TS_EC1E_SCALER_L								0x1232
#define TS_EC2E_SCALER_H								0x1234
#define TS_EC2E_SCALER_L								0x1236
#define TS_EC3E_SCALER_H								0x1238
#define TS_EC3E_SCALER_L								0x123A
#define TS_EC4E_SCALER_H								0x123C
#define TS_EC4E_SCALER_L								0x123E
#define TS_EC5E_SCALER_H								0x1240
#define TS_EC5E_SCALER_L								0x1242
#define TS_EC6E_SCALER_H								0x1244
#define TS_EC6E_SCALER_L								0x1246
/*                      								0x1248*/
#define TS_ECC1_SCALER									0x124A
/*                      								0x1248*/
#define TS_ECC2_SCALER									0x124E
/*                      								0x1250*/
#define TS_ECC3_SCALER									0x1252
/*                      								0x1254*/
#define TS_ECC4_SCALER									0x1256
/*                      								0x1258*/
#define TS_ECC5_SCALER									0x125A
/*                      								0x125C*/
#define TS_ECC6_SCALER									0x125E
#define TS_REF_SCALER_H									0x1260
#define TS_REF_SCALER_L									0x1262
#define TS_MORAB_SCALER_H								0x1264
#define TS_MORAB_SCALER_L								0x1266

#define TS_STMULT_SCALER_H								0x1268
#define TS_STMULT_SCALER_L								0x126A

/* end of scalers */
/******************/






/*************/
/* registers */

/* Delays all range from 0 to 31. 0->0ns delay, 31->155ns delay */
#define TS_MORA_DELAY									0x2000
#define TS_MORB_DELAY									0x2002
#define TS_TOF1_DELAY									0x2004
#define TS_TOF2_DELAY									0x2006
#define TS_TOF3_DELAY									0x2008
#define TS_TOF4_DELAY									0x200A
#define TS_TOF5_DELAY									0x200C
#define TS_TOF6_DELAY									0x200E



#define TS_ECin1P_DELAY									0x2010
#define TS_ECin2P_DELAY									0x2012
#define TS_ECin3P_DELAY									0x2014
#define TS_ECin4P_DELAY									0x2016
#define TS_ECin5P_DELAY									0x2018
#define TS_ECin6P_DELAY									0x201A

#define TS_ECin1E_DELAY									0x201C
#define TS_ECin2E_DELAY									0x201E
#define TS_ECin3E_DELAY									0x2020
#define TS_ECin4E_DELAY									0x2022
#define TS_ECin5E_DELAY									0x2024
#define TS_ECin6E_DELAY									0x2026
#define TS_ECtot1P_DELAY								0x2028
#define TS_ECtot2P_DELAY								0x202A
#define TS_ECtot3P_DELAY								0x202C
#define TS_ECtot4P_DELAY								0x202E
#define TS_ECtot5P_DELAY								0x2030
#define TS_ECtot6P_DELAY								0x2032
#define TS_ECtot1E_DELAY								0x2034
#define TS_ECtot2E_DELAY								0x2036
#define TS_ECtot3E_DELAY								0x2038
#define TS_ECtot4E_DELAY								0x203A
#define TS_ECtot5E_DELAY								0x203C
#define TS_ECtot6E_DELAY								0x203E

#define TS_CC1_DELAY									0x2040
#define TS_CC2_DELAY									0x2042
#define TS_CC3_DELAY									0x2044
#define TS_CC4_DELAY									0x2046
#define TS_CC5_DELAY									0x2048
#define TS_CC6_DELAY									0x204A

#define TS_ST1_DELAY									0x204C
#define TS_ST2_DELAY									0x204E
#define TS_ST3_DELAY									0x2050
#define TS_ST4_DELAY									0x2052
#define TS_ST5_DELAY									0x2054
#define TS_ST6_DELAY									0x2056
#define TS_ST7_DELAY									0x2058
#define TS_ST8_DELAY									0x205A
#define TS_ST9_DELAY									0x205C
#define TS_ST10_DELAY									0x205E
#define TS_ST11_DELAY									0x2060
#define TS_ST12_DELAY									0x2062
#define TS_ST13_DELAY									0x2064
#define TS_ST14_DELAY									0x2066
#define TS_ST15_DELAY									0x2068
#define TS_ST16_DELAY									0x206A
#define TS_ST17_DELAY									0x206C
#define TS_ST18_DELAY									0x206E
#define TS_ST19_DELAY									0x2070
#define TS_ST20_DELAY									0x2072
#define TS_ST21_DELAY									0x2074
#define TS_ST22_DELAY									0x2076
#define TS_ST23_DELAY									0x2078
#define TS_ST24_DELAY									0x207A



/* 0-7 trigger bit persistence. 0->0ns stretch, 7->35ns stretch */
#define TS_PERSIST_TRIG1								0x2100
#define TS_PERSIST_TRIG2								0x2102
#define TS_PERSIST_TRIG3								0x2104
#define TS_PERSIST_TRIG4								0x2106
#define TS_PERSIST_TRIG5								0x2108
#define TS_PERSIST_TRIG6								0x210A
#define TS_PERSIST_TRIG7								0x210C
#define TS_PERSIST_TRIG8								0x210E
#define TS_PERSIST_TRIG9								0x2110
#define TS_PERSIST_TRIG10								0x2112
#define TS_PERSIST_TRIG11								0x2114
#define TS_PERSIST_TRIG12								0x2116
/* 0-1023 prescale trigger bit. 0->disabled trigger, 1->no prescale, 2->prescale by 2, etc... */
#define TS_PRESCALE_TRIG1								0x2200
#define TS_PRESCALE_TRIG2								0x2202
#define TS_PRESCALE_TRIG3								0x2204
#define TS_PRESCALE_TRIG4								0x2206
#define TS_PRESCALE_TRIG5								0x2208
#define TS_PRESCALE_TRIG6								0x220A
#define TS_PRESCALE_TRIG7								0x220C
#define TS_PRESCALE_TRIG8								0x220E
#define TS_PRESCALE_TRIG9								0x2210
#define TS_PRESCALE_TRIG10								0x2212
#define TS_PRESCALE_TRIG11								0x2214
#define TS_PRESCALE_TRIG12								0x2216














/* Sector Trigger Signals:                                     */
/* bit 0->sector 1, bit 1->sector 2, etc...                    */
/* STS <= ST0 or ST1 or ST2 or ST3                             */
/* STOF <= (STS or STS_DIS) and (TOF or TOF_DIS) and STOF_EN   */
/* ECP <= ECinP and ECtotP                                     */
/* ECE <= ECinE and ECtotE                                     */
/* ECC <= (ECE or ECE_DIS) and (CC or CC_DIS) and ECC_EN       */
#define TS_STOF_EN										0x2300
#define TS_TOF_DIS										0x2302
#define TS_STS_DIS										0x2304
#define TS_ECE_DIS										0x2306
#define TS_CC_DIS										0x2308
#define TS_ECC_EN										0x230A

/* MOR signals:
/* bit 0->trigger 1, bit 1->trigger 2, etc...                  */
/* MOR <= (MORA_EN and MORA) or (MORB_EN and MORB) or MOR_DIS  */
/* MOR is used in trigger signal                               */
#define TS_MORA_EN										0x2404
#define TS_MORB_EN										0x2406
#define TS_MOR_DIS										0x2408





/* bits 5:9 => ST Multiplicity Max, bits 0:4 > ST Multiplicity Min */
#define TS_STMULT_THRESHOLD								0x240A

#define TS_STMULT_DIS									0x240C

/** end of registers **/
/**********************/
/**********************/







/* Scope triggering configuration and FIFOs                    */
#define TS_TRIG_STATUS									0x3000
#define TS_TRIG_VALUE2_H								0x3002
#define TS_TRIG_VALUE2_L								0x3004
#define TS_TRIG_VALUE1_H								0x3006
#define TS_TRIG_VALUE1_L								0x3008
#define TS_TRIG_VALUE0_H								0x300A
#define TS_TRIG_VALUE0_L								0x300C
#define TS_TRIG_INGORE2_H								0x300E
#define TS_TRIG_INGORE2_L								0x3010
#define TS_TRIG_INGORE1_H								0x3012
#define TS_TRIG_INGORE1_L								0x3014
#define TS_TRIG_INGORE0_H								0x3016
#define TS_TRIG_INGORE0_L								0x3018


/* scope readout */
#define TS_TRIG_BUFFER2_H								0x3800
#define TS_TRIG_BUFFER2_L								0x3802
#define TS_TRIG_BUFFER1_H								0x3804
#define TS_TRIG_BUFFER1_L								0x3806
#define TS_TRIG_BUFFER0_H								0x3808
#define TS_TRIG_BUFFER0_L								0x380A




/* Trigger Bit LUT base address                               */
#define TS_CFG_LUT_BASE									0x4000

/* counting bits from 1:  address bit 9 not in use            */
/* address bits 14:10 defines trigger bits:                   */
/*        first 12 addresses are ECC/STOF tables,             */
/*        following 12 addresses are ECP/STOF tables          */
/* LUT tables are 12->1 => 4096 bits                          */
/* LUT tables are written 16bits at a time.                   */
/* Address mapping:                                           */
/*   ECC/STOF Table:                                          */
/*      ECC(1) => LUT_ADDR_BIT(0)                             */
/*      ECC(2) => LUT_ADDR_BIT(1)                             */
/*      ECC(3) => LUT_ADDR_BIT(2)					          */
/*      ECC(4) => LUT_ADDR_BIT(3)					          */
/*      ECC(5) => LUT_ADDR_BIT(4)					          */
/*      ECC(6) => LUT_ADDR_BIT(5)					          */
/*      STOF(1) => LUT_ADDR_BIT(6)					          */
/*      STOF(2) => LUT_ADDR_BIT(7)					          */
/*      STOF(3) => LUT_ADDR_BIT(8)					          */
/*      STOF(4) => LUT_ADDR_BIT(9)					          */
/*      STOF(5) => LUT_ADDR_BIT(10)					          */
/*      STOF(6) => LUT_ADDR_BIT(11)					          */
/*   ECP/STOF Table:                                          */
/*      ECP(1) => LUT_ADDR_BIT(0)					          */
/*      ECP(2) => LUT_ADDR_BIT(1)					          */
/*      ECP(3) => LUT_ADDR_BIT(2)					          */
/*      ECP(4) => LUT_ADDR_BIT(3)					          */
/*      ECP(5) => LUT_ADDR_BIT(4)					          */
/*      ECP(6) => LUT_ADDR_BIT(5)					          */
/*      STOF(1) => LUT_ADDR_BIT(6)					          */
/*      STOF(2) => LUT_ADDR_BIT(7)					          */
/*      STOF(3) => LUT_ADDR_BIT(8)					          */
/*      STOF(4) => LUT_ADDR_BIT(9)					          */
/*      STOF(5) => LUT_ADDR_BIT(10)					          */
/*      STOF(6) => LUT_ADDR_BIT(11)					          */

#define V1495_VMEADDRESS	0x08510000

#define CLOCK_PERIOD_NS		5

#define MAX_DELAY			31
#define MAX_STMULT			24
#define MAX_PERSIST			7
#define MAX_PRESCALE		1023

#ifdef WIN32
	#pragma pack(pop)
#else
/*#error "Restore structure packing here"*/
#endif

#endif
