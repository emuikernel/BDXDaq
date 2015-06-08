/* frcBIB.h - MOTOROLA Board Information Block Version 2.1 */

/* Copyright 1997-2004 MOTOROLA  GmbH */
/* Copyright 2004-2006 MOTOROLA GmbH. */

/*
modification history
--------------------
03b,22may06,rgr	  Added data type BIB_NUM32 to handle unsigned 32bit values.
03a,16may06,rgr	  Started implementing BIB 2.1 which merges VPD and BIB.
		  Increased BIB_MAX_SIZE to 2048 bytes.
		  Added BIB20_XXX defines for BIB 2.0 backward compatibility.
		  Added element #21 "FinalPartNumber".
02z,03may06,rgr	  Added I2C-bus devices MAX6900 and MAX664X.
02y,17nov05,rgr	  Added element #20 "ATCA_eKeying".
02x,04jul05,rgr	  Added I2C-bus device LM87 (BIB_ENUM_I2C_LM87).
02w,14apr05,mbr	  Added BIB_ENUM_I2C_ICS932S208 and BIB_ENUM_I2C_ICS9DB108.
02v,29nov04,mbr	  Added BIB_ENUM_I2C_CY28409, BIB_ENUM_I2C_CY28329, 
		  BIB_ENUM_I2C_W83791D type.
02u,23nov04,mbr	  Added BIB_ENUM_I2C_ICS9248_77 type.
02t,10nov04,rgr	  Added BIB_ENUM_I2C_IPMB to distinguish between private
		  I2C-buses and IPMBs which do only allow intelligent devices.
		  Removed support for BIB V1.0 which is no more used since 1998.
02s,22jun04,rgr	  Added field "FT_DateTime" to BIB element #4, "RevInfo".
		  Added BIB element #19 "LongProductName".
02r,16feb04,rgr	  Added I2C-bus multiplexer PCA9544.
02q,05feb04,fxs	  Removed comma behind last element in BIB_I2cDev
02p,27jan04,fxs	  Added I2C-bus device BIB_ENUM_I2C_ICS932S203
02o,13jan03,rgr   Added I2C-bus device LM80.
02n,27aug02,rgr   Added flag "SysEnLowActive" for element #17, "IBMU_Host".
02m,14aug02,rgr   Added field "FinalRevision" to BIB element #4, "RevInfo".
02l,11jun02,rgr   Added I2C-bus device PCF8574.
02k,14may02,rgr   Added I2C-bus devices ADM1022 and PC8741x.
02j,08may02,rgr   Added BIB element #18 "FinalSerNum".
02i,18apr02,rgr   Increased BIB_MAX_SIZE from 256 to 512 bytes.
		  Added new data types BIB_ENUM and BIB_ARRAY.
		  Added BIB elements "FuncTest", "ExtdBoardInfo", "FileId",
		  "PartNumber", "I2cBus", "IPMI", "IBMU", and "IBMU_Host".
		  Added "OnbrdDevs" flags "FloppyDisk", "HardDisk_1..4",
		  and "I2cBusCtrl_1..4".
		  Added field "MfgDateTime" to BIB element #4, "RevInfo".
02h,24jul01,rgr   Corrected size of <BoardSpecInfo> area in BIB_V10 structure.
02g,15mar01,rgr   Added type casts to BIB_DATE_GET() and BIB_DATE_MDY_TO_INT()
		  to let the Microsoft compiler generate correct code.
02f,13dec00,rgr   Set BIB_STRUCT_DEF of BIB_ELEM_ProductSpecInfo in comments
		  again.  This is required by the "mk_bib" tool.
02e,15feb00,fxs   Commented out BIB_ELEM_ProductSpecInfo
02d,23dec99,fxs	  Added conditional define for CHAR
02c,05mar99,rgr   Added some type casts to satisfy the Microsoft compiler.
02b,22feb99,rgr   Changed BIB element "AddOnbrdMemory" to "OnbrdMemory".
02a,23nov98,rgr   BIB 2.0 initial version.
*/

/*
This header file describes the structure of the Board Information Block,
version 2.1.  The Board Information Block (BIB) is a data structure that
allows to store hardware related information in a unique and compact way.
The BIB is stored in a non-volatile memory, usually a serial I2C-bus EEPROM,
called the ID-ROM, which is mounted on the board.

IMPORTANT:
This file must be held unique accross all platforms!  It is maintained by the
administrator of the BIB Specification, for more information see
http://compass.mot.com/doc/197464052/

		*** Please do NOT make any changes here! ***
*/

#ifndef	__INCfrcBIBh
#define	__INCfrcBIBh


/* generic defines - do never change !! */

    /* BIB 2.0 backward compatibility ==== DO NO MORE USE ==== */

#define	BIB20_VERSION	0x20	/* old version is 2.0 */
#define	BIB20_FORCE_ID	"FORCE COMPUTERS"
#define	BIB20_DIR1_CNT	 12	/* 1st BIB directory has 12 entries */
#define	BIB20_DIR_CNT	  8	/* any further directory has 8 entries */

    /* common BIB defines */

#define	BIB_HEADER_SIZE	  32	/* minimum size to load */
#define	BIB_VERSION	0x21	/* current supported version is 2.1 */
#define	BIB_ID_STR	"MOTOROLA"

#define	BIB_STD_SIZE	 128	/* the BIB standard size is 128 bytes */
#define	BIB_MAX_SIZE	2048	/* the BIB maximum  size is 512 bytes */
#define	BIB_LENGTH_UNIT	  16	/* LengthCode tells 16byte portions */
#define	BIB_LENGTH_CODE(size)	(((size)+BIB_LENGTH_UNIT-1)/BIB_LENGTH_UNIT)
#define	SIZE_UNKNOWN	255	/* use maximum if size is unknown */


/* basic data types - compiler/architecture dependent */

#ifndef CHAR
#define	CHAR	char
#endif

#if FALSE	/* examples - data types are platform specific!! */
#define	UINT8	unsigned char
#define	UINT16	unsigned short
#define	UINT32	unsigned long
#endif

/* BIB 2.0 directory structure ==== DO NO MORE USE ==== */

typedef	struct
    {
	UINT8	dirEntryCnt;		/* number of directory entries      */
	UINT8	offsNextDir;		/* byte offset to next directory    */
	UINT8	offsDirEntry[BIB20_DIR1_CNT];   /* array of directory entries */
    } BIB_DIR;


/* BIB data types */

    /* pure binary data */
#define	BIB_DATA(name,length)	UINT8 name [(length)]

    /* ASCIIZ string (zero terminated) */
#define	BIB_STR(name,length)	CHAR  name [(length)]

    /* 8 bit integer number 0..255 (0xFF) */
#define	BIB_NUM(name)		UINT8 name

    /* 16 bit integer number 0..65535 (0xFFFF) */
#define	BIB_NUM16(name)		UINT8 name [2]

    /* 32 bit integer number 0..4294967295 (0xFFFFFFFF) */
#define	BIB_NUM32(name)		UINT8 name [4]

    /* date structure (3 bytes large) */
#define	BIB_DATE(name)		UINT8 name [3]

    /* memory size codes */
#define	BIB_MEMSIZE(name,count)	UINT8 name [(count)]

    /* an array of flags (bit field) */
#define	BIB_FLAGS(name,count)	UINT8 name [((count) + 7) / 8]

    /* enum type (restricted to 256 elements) */
#define	BIB_ENUM(type,name)	UINT8 name

    /* array of a user-defined data type */
#define	BIB_ARRAY(type,name,count)	type name [(count)]

/* BIB structure and index definitions */

    /* flag (array index) definition */
#define	BIB_FLAGS_DEF	typedef enum

    /* element (directory index) definition */
#define	BIB_ELEM_DEF	typedef enum

    /* BIB enum definition (restricted to 256 elements) */
#define	BIB_ENUM_DEF	typedef enum

    /* structure definition */
#define	BIB_STRUCT_DEF	typedef struct

    /* array definition */
#define	BIB_ARRAY_DEF	typedef struct


/* macros to access the BIB data types */

/*
 * Macros to access a BIB_NUM16 entry
 *
 * The following macros allow to get/set a BIB_NUM16 integer number
 * (16 bits).  The coding schema stores first the MSB and then the
 * LSB.
 *
 * Parameters:
 *	<adrs>		Address where to get or set <value>
 *	<value>		16 bit value to store.
 */
#define	BIB_NUM16_GET(adrs)					\
		(((adrs)[0] << 8) + ((adrs)[1]))

#define	BIB_NUM16_SET(adrs,value)				\
		{ (adrs)[0]=((UINT8)((value) >>  8) & 0xff);	\
		  (adrs)[1]=((UINT8)((value))       & 0xff); }

/*
 * Macros to access a BIB_NUM32 entry
 *
 * The following macros allow to get/set a BIB_NUM32 integer number
 * (32 bits).  The coding schema stores first the MSB.
 *
 * Parameters:
 *	<adrs>		Address where to get or set <value>
 *	<value>		32 bit value to store.
 */
#define	BIB_NUM32_GET(adrs)					\
		 ((((UINT32)(adrs)[0]) << 24)			\
		+ (((UINT32)(adrs)[1]) << 16)			\
		+ (((UINT32)(adrs)[2]) <<  8)			\
		+  ((UINT32)(adrs)[3]))

#define	BIB_NUM32_SET(adrs,value)				\
		{ (adrs)[0]=((UINT8)((value) >> 24) & 0xff);	\
		  (adrs)[1]=((UINT8)((value) >> 16) & 0xff);	\
		  (adrs)[2]=((UINT8)((value) >>  8) & 0xff);	\
		  (adrs)[3]=((UINT8)((value))       & 0xff); }

/*
 * Macros to access a BIB_DATE entry
 *
 * The following macros allow to get/set the BIB_DATE as an integer
 * date code.  This code has the following format:
 *
 *  Bit [31.....24|23......12|11.......8|7.5|4......0]
 *      [unused, 0|year(11:0)|month(3:0)|000|day(4:0)]
 *
 * Date code values can directly be compared to another date, because
 * the most significant part of the value is the year, then the month
 * and at least the day.
 *
 * Parameters:
 *	<adrs>		Address where to get or set <dateCode>
 *	<dateCode>	Date code as integer value.  This format
 *			is described above.
 */
#define	BIB_DATE_GET(adrs)					\
		 ((((UINT32)(adrs)[0]) << 16)			\
		+ (((UINT32)(adrs)[1]) <<  8)			\
		+  ((UINT32)(adrs)[2]))

#define	BIB_DATE_SET(adrs,dateCode)				\
		{ (adrs)[0]=((UINT8)((dateCode) >> 16) & 0xff);	\
		  (adrs)[1]=((UINT8)((dateCode) >>  8) & 0xff);	\
		  (adrs)[2]=((UINT8)((dateCode))       & 0xff); }

/*
 * BIB_DATE conversion macros
 *
 * The BIB_DATE_MDY_TO_INT macro converts a date, given as month,
 * year and day values (MDY) into an integer date code.  The format
 * of this date code is descibed above.
 * All other macros allow to extract the month, or day, or year
 * value from a specified date code.
 *
 * Parameters:
 *	<month>		Month value, 1..12
 *	<day>		Day value, 1..31
 *	<year>		Year value, 1996..4095
 *	<dateCode>	Date code as integer value.  This format
 *			is described above.
 */
#define	BIB_DATE_MDY_TO_INT(month,day,year)		\
		  ( (((UINT32)(year)) << 12)		\
		  + (((UINT32)(month)) << 8)		\
		  +  ((UINT32)(day)) )

#define	BIB_DATE_MONTH(dateCode)			\
		  (((dateCode) >> 8) & 0x0f)

#define	BIB_DATE_DAY(dateCode)				\
		  ((dateCode) & 0x1f)

#define	BIB_DATE_YEAR(dateCode)				\
		  (((dateCode) >> 12) & 0xfff)

/*
 * Macros to access the "MfgDateTime" entry
 *
 * Field "MfgDateTime" was added to BIB element #4 - "RevInfo" to store
 * the manufacturing date and time in an IPMI FRU compliant format, i.e.
 * in minutes since JAN/01/1996, 00:00.  Nevertheless, like all multi-byte
 * fields within the BIB, this information is stored in big-endian manner
 * here!  Use the following macros to get/set this 24 bit value.
 *
 * Parameters:
 *	<adrs>		Address where to get or set <value>
 *	<value>		24 bit value (in a 32 bit variable) to store.
 */
#define	BIB_MFG_DT_GET(adrs)					\
		 ((((UINT32)(adrs)[0]) << 16)			\
		+ (((UINT32)(adrs)[1]) <<  8)			\
		+  ((UINT32)(adrs)[2]))

#define	BIB_MFG_DT_SET(adrs,value)				\
		{ (adrs)[0]=((UINT8)((value) >> 16) & 0xff);	\
		  (adrs)[1]=((UINT8)((value) >>  8) & 0xff);	\
		  (adrs)[2]=((UINT8)((value))       & 0xff); }

/*
 * Macros to access and manipulate BIB_FLAGS
 *
 * The following macros allow to get, set and clear a specific
 * flag within a BIB_FLAGS field.  The internal organization of
 * such a field is the following:
 * 
 *  Flag [0|1|2....7][8|9|10...15][16........
 *  Adrs [    + 0   ][    + 1    ][    + 2...
 *
 * Parameters:
 *	<adrs>		Base address of the BIB_FLAGS field.
 *	<flag>		Number of the flag to get, set or clear.
 */
#define	BIB_FLAG_GET(adrs,flag)				\
		  (((adrs)[(flag) / 8] >> (7 - ((flag) & 7))) & 0x01)

#define	BIB_FLAG_SET(adrs,flag)				\
		  ((adrs)[(flag) / 8] |=  (0x80 >> ((flag) & 7)))

#define	BIB_FLAG_CLR(adrs,flag)				\
		  ((adrs)[(flag) / 8] &= ~(0x80 >> ((flag) & 7)))

/*
 * Macros to get and set a BIB_MEMSIZE size code
 *
 * The following macros allow to get the <Multiplier> and
 * <SizeUnit> of a BIB_MEMSIZE element, or build the size code
 * from the <Multiplier> and <SizeUnit>.
 * 
 * For memory sizes less than 64 kilobyte, the <Multiplier> must be
 * set to 0 and the <SizeUnit> contains the power-of-2 index for
 * the memory size, e.g. a 5 means 2^5=32 bytes.  The memory size
 * must be calculated 2 ^ <SizeUnit>.
 *
 * For memory sizes greater or equal 64 kilobytes, the size is
 * divided into a <SizeUnit> and a <Multiplier> value.  The memory
 * size must be calculated <Multiplier> * 2 ^ (16 + <SizeUnit>).
 *
 * The internal organization of a BIB_MEMSIZE byte is the following:
 * 
 *  Bit [7........4|3......0]
 *      [Multiplier|SizeUnit]
 *
 * Parameters:
 *	<sizeCode>	Size code as described above.
 *	<mult>		Multiplier value.
 *	<unit>		SizeUnit value.
 */
#define	BIB_MEMSIZE_MULT_GET(sizeCode)	((sizeCode) >> 4)
#define	BIB_MEMSIZE_UNIT_GET(sizeCode)	((sizeCode) & 0x0f)
#define	BIB_MEMSIZE_CODE(mult,unit)	(((mult) << 4) | (unit))


/* typedefs */

/* BIB Structure Type Definitions */

/*
 * List of all BIB element names (directory entries)
 *
 * NOTE: Before you can define a BIB structure, the corresponding
 *       BIB element name must be defined here!
 */
BIB_ELEM_DEF
    {
	BIB_ProductName,	/* # 0: product name (ASCIIZ string)	*/
	BIB_SerialNumber,	/* # 1: initial serial number (ASCIIZ string) */
	BIB_HostId,		/* # 2: host Id (pure binary data)	*/
	BIB_EthernetAdrs,	/* # 3: all ethernet addresses		*/
	BIB_RevInfo,		/* # 4: revision information section	*/
	BIB_Processor,		/* # 5: processor information		*/
	BIB_OnbrdDevs,		/* # 6: flags for on-board devices	*/
	BIB_OnbrdMemory,	/* # 7: on-board memory (soldered)	*/
	BIB_FlashMemory,	/* # 8: all flash memory banks		*/
	BIB_ProductSpecInfo,	/* # 9: product specific information	*/
	BIB_FuncTest,		/* #10: functional test (internal use)	*/
	BIB_ExtdBoardInfo,	/* #11: extended board information	*/
	BIB_FileId,		/* #12: BIB source file, Oracle item#	*/
	BIB_PartNumber,		/* #13: product part number if not SAP#	*/
	BIB_I2cBus,		/* #14: I2C-bus related information	*/
	BIB_IPMI,		/* #15: IPMI related information	*/
	BIB_IBMU,		/* #16: Intelligent Board Management Unit */
	BIB_IBMU_Host,		/* #17: IBMU host (base board) information */
	BIB_FinalSerNum,	/* #18: final serial number (ASCIIZ string) */
	BIB_LongProductName,	/* #19: long product name (ASCIIZ string) */
	BIB_ATCA_eKeying,	/* #20: ATCA e-keying board connectivity */
	BIB_FinalPartNumber	/* #21: final part number (ASCIIZ string) */
    } BIB_ELEM_NAME;

/*
 * BIB Flags Definition for the processor attributes - "Processor"
 *
 * IMPORTANT:	Do not insert new flags into this table, always
 *		append at the end of the table!
 */
BIB_FLAGS_DEF
    {
	BIB_FLG_MMU,		/* Memory Management Unit */
	BIB_FLG_FPU		/* Floating Point Unit	*/
    } BIB_FLAGS_Processor;

/*
 * BIB Flags Definition for Onboard Devices - "OnbrdDevs"
 *
 * IMPORTANT:	Do not insert new flags into this table, always
 *		append at the end of the table!
 */
BIB_FLAGS_DEF
    {
	BIB_FLG_Serial_1,	/* serial channels	*/
	BIB_FLG_Serial_2,
	BIB_FLG_Serial_3,
	BIB_FLG_Serial_4,
	BIB_FLG_Serial_5,
	BIB_FLG_Serial_6,
	BIB_FLG_Serial_7,
	BIB_FLG_Serial_8,
	BIB_FLG_Serial_9,
	BIB_FLG_Serial_10,
	BIB_FLG_Serial_11,
	BIB_FLG_Serial_12,
	BIB_FLG_Serial_13,
	BIB_FLG_Serial_14,
	BIB_FLG_Serial_15,
	BIB_FLG_Serial_16,
	BIB_FLG_Parallel_1,	/* parallel ports	*/
	BIB_FLG_Parallel_2,
	BIB_FLG_Parallel_3,
	BIB_FLG_Parallel_4,
	BIB_FLG_Parallel_5,
	BIB_FLG_Parallel_6,
	BIB_FLG_Parallel_7,
	BIB_FLG_Parallel_8,
	BIB_FLG_SCSI_1,		/* SCSI controllers	*/
	BIB_FLG_SCSI_2,
	BIB_FLG_SCSI_3,
	BIB_FLG_SCSI_4,
	BIB_FLG_IDE_1,		/* IDE or SATA controllers	*/
	BIB_FLG_IDE_2,
	BIB_FLG_IDE_3,
	BIB_FLG_IDE_4,
	BIB_FLG_Floppy_1,	/* floppy controllers	*/
	BIB_FLG_Floppy_2,
	BIB_FLG_Floppy_3,
	BIB_FLG_Floppy_4,
	BIB_FLG_USB_1,		/* Universal Serial Bus	*/
	BIB_FLG_USB_2,
	BIB_FLG_USB_3,
	BIB_FLG_USB_4,
	BIB_FLG_Bridge_1,	/* bus bridges		*/
	BIB_FLG_Bridge_2,
	BIB_FLG_Bridge_3,
	BIB_FLG_Bridge_4,
	BIB_FLG_Bridge_5,
	BIB_FLG_Bridge_6,
	BIB_FLG_Bridge_7,
	BIB_FLG_Bridge_8,
	BIB_FLG_NVRAM_1,	/* non volatile RAM	*/
	BIB_FLG_NVRAM_2,
	BIB_FLG_RTC,		/* real time clock	*/
	BIB_FLG_Keyboard,	/* keyboard connector	*/
	BIB_FLG_Mouse,		/* mouse connector	*/
	BIB_FLG_Graphics,	/* graphics adapter	*/
	BIB_FLG_Audio,		/* audio device		*/
	BIB_FLG_FloppyDisk,	/* floppy disk drive	*/
	BIB_FLG_SpecDevice_1,	/* special devices	*/
	BIB_FLG_SpecDevice_2,
	BIB_FLG_SpecDevice_3,
	BIB_FLG_SpecDevice_4,
	BIB_FLG_HardDisk_1,	/* hard/flash disks	*/
	BIB_FLG_HardDisk_2,
	BIB_FLG_HardDisk_3,
	BIB_FLG_HardDisk_4,
	BIB_FLG_I2cBusCtrl_1,	/* I2C-bus controllers	*/
	BIB_FLG_I2cBusCtrl_2,
	BIB_FLG_I2cBusCtrl_3,
	BIB_FLG_I2cBusCtrl_4,
	BIB_FLG_I2cBusCtrl_5,
	BIB_FLG_I2cBusCtrl_6,
	BIB_FLG_I2cBusCtrl_7,
	BIB_FLG_I2cBusCtrl_8
    } BIB_FLAGS_OnbrdDevs;

/*
 * BIB Flags Definition for the IBMU Host (base board) - "IBMU_Host"
 *
 * IMPORTANT:	Do not insert new flags into this table, always
 *		append at the end of the table!
 */
BIB_FLAGS_DEF
    {
	BIB_FLG_ICMB_Intf,	/* board provides an ICMB interface	*/
	BIB_FLG_PostPort,	/* 3rd KCS interface receives POST codes */
	BIB_FLG_Reserved_1,	/* reserved - do not use !! */
	BIB_FLG_SysEnLowActive,	/* TRUE if SYS_EN is low active		*/
	BIB_FLG_NmiEdgeTrig,	/* TRUE if NMI is edge triggered	*/
	BIB_FLG_NmiHighActive,	/* TRUE if NMI is high active		*/
	BIB_FLG_SmiEdgeTrig,	/* TRUE if SMI is edge triggered	*/
	BIB_FLG_SmiHighActive,	/* TRUE if SMI is high active		*/
	BIB_FLG_TrigNmi,	/* IBMU can trigger an NMI		*/
	BIB_FLG_TrigSmi,	/* IBMU can trigger an SMI		*/
	BIB_FLG_TrigMsgIrq,	/* IBMU can trigger a message interrupt	*/
	BIB_FLG_TrigHardReset,	/* IBMU can trigger a hard reset	*/
	BIB_FLG_TrigPowerDown,	/* IBMU can perform a power down	*/
	BIB_FLG_TrigPowerCycle	/* IBMU can perform a power cycle	*/
    } BIB_FLAGS_IBMU_Host;


/*
 * BIB Enum Definition for Entity ID Codes - element "IPMI"
 *
 * These codes were taken from the IPMI Specification V1.5,
 * section 37.13, Entity IDs.
 */
BIB_ENUM_DEF
    {
/*00*/	BIB_ENUM_EID_Unspecified,	/* unspecified			*/
/*01*/	BIB_ENUM_EID_Other,		/* other			*/
/*02*/	BIB_ENUM_EID_Unknown,		/* unknown			*/
/*03*/	BIB_ENUM_EID_Processor,		/* processor			*/
/*04*/	BIB_ENUM_EID_Disk,		/* disk or disk bay		*/
/*05*/	BIB_ENUM_EID_Peripheral,	/* peripheral bay		*/
/*06*/	BIB_ENUM_EID_SysMgmtModule,	/* system management module	*/
/*07*/	BIB_ENUM_EID_SysBoard,		/* main system board		*/
/*08*/	BIB_ENUM_EID_MemoryModule,	/* memory module		*/
/*09*/	BIB_ENUM_EID_ProcessorModule,	/* processor module		*/
/*0A*/	BIB_ENUM_EID_PowerSupply,	/* power supply			*/
/*0B*/	BIB_ENUM_EID_AddInCard,		/* add-in card			*/
/*0C*/	BIB_ENUM_EID_FrontPanelBoard,	/* front panel board		*/
/*0D*/	BIB_ENUM_EID_BackPanelBoard,	/* back panel board		*/
/*0E*/	BIB_ENUM_EID_PowerSysBoard,	/* power system board		*/
/*0F*/	BIB_ENUM_EID_DriveBackplane,	/* drive backplane		*/
/*10*/	BIB_ENUM_EID_SysIntExpBoard,	/* system internal expansion board */
/*11*/	BIB_ENUM_EID_OthSysBoard,	/* other system board		*/
/*12*/	BIB_ENUM_EID_ProcessorBoard,	/* processor board		*/
/*13*/	BIB_ENUM_EID_PowerUnit,		/* power unit / power domain	*/
/*14*/	BIB_ENUM_EID_PowerModule,	/* power module / DC-DC converter */
/*15*/	BIB_ENUM_EID_PowerMgmtBoard,	/* power management board	*/
/*16*/	BIB_ENUM_EID_ChassisBpBoard,	/* chassis back panel board	*/
/*17*/	BIB_ENUM_EID_SysChassis,	/* system chassis		*/
/*18*/	BIB_ENUM_EID_SubChassis,	/* sub-chassis			*/
/*19*/	BIB_ENUM_EID_OthChassisBoard,	/* other chassis board		*/
/*1A*/	BIB_ENUM_EID_DiskDriveBay,	/* disk drive bay		*/
/*1B*/	BIB_ENUM_EID_PeripheralBay,	/* peripheral bay (see also 05)	*/
/*1C*/	BIB_ENUM_EID_DeviceBay,		/* device bay			*/
/*1D*/	BIB_ENUM_EID_CoolingDevice,	/* cooling device, e.g. fan	*/
/*1E*/	BIB_ENUM_EID_CoolingUnit,	/* cooling unit (log. entity)	*/
/*1F*/	BIB_ENUM_EID_Cable,		/* cable / interconnect		*/
/*20*/	BIB_ENUM_EID_MemoryDevice,	/* memory device (replaceable)	*/
/*21*/	BIB_ENUM_EID_SysMgmtSoftware,	/* system management software	*/
/*22*/	BIB_ENUM_EID_BIOS,		/* BIOS				*/
/*23*/	BIB_ENUM_EID_OS,		/* operating system		*/
/*24*/	BIB_ENUM_EID_SysBus,		/* system bus			*/
/*25*/	BIB_ENUM_EID_Group,		/* log. entity for association rec. */
/*26*/	BIB_ENUM_EID_RemMgmtComDev	/* remote management commun. device */
    } BIB_EntityId;

/*
 * BIB Enum Definition for I2C-bus Devices - "I2cDev"
 *
 * IMPORTANT:	Do not insert new devices into this table, always
 *		append at the end of the table, or use reserved
 *		entries within the respective numeric range.
 */
BIB_ENUM_DEF
    {
	/* 0~7: identifiers */
	BIB_ENUM_I2C_BUS_ID,	/* begin of a new I2C bus	*/
	BIB_ENUM_I2C_UNKNOWN,	/* no/unknown device on a bus	*/
	BIB_ENUM_I2C_IPMB,	/* this is an IPMB		*/
	BIB_ENUM_I2C_RsvdId3,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdId4,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdId5,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdId6,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdId7,	/* reserved - not used yet !!	*/

	/* 8~15: multiplexers */
	BIB_ENUM_I2C_PCA9540,	/* 2 channel multiplexer	*/
	BIB_ENUM_I2C_PCA9542,	/* 2 chan. mux with interrupts	*/
	BIB_ENUM_I2C_PCA9545,	/* 4 chan. mux with interrupts	*/
	BIB_ENUM_I2C_PCA9544,	/* 4 chan. mux with interrupts	*/
	BIB_ENUM_I2C_RsvdMux4,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdMux5,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdMux6,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdMux7,	/* reserved - not used yet !!	*/

	/* 16~31: EEPROMs/memory */
	BIB_ENUM_I2C_EEP24C01,	/* EEPROM 128B  -  8bit adrs	*/
	BIB_ENUM_I2C_EEP24C02,	/* EEPROM 256B  -  8bit adrs	*/
	BIB_ENUM_I2C_EEP24C04,	/* EEPROM 512B  -  8bit adrs	*/
	BIB_ENUM_I2C_EEP24C08,	/* EEPROM  1KB  -  8bit adrs	*/
	BIB_ENUM_I2C_EEP24C16,	/* EEPROM  2KB  -  8bit adrs	*/
	BIB_ENUM_I2C_EEP24C17,	/* EEPROM  2KB  -  8bit adrs WP	*/
	BIB_ENUM_I2C_EEP24C32,	/* EEPROM  4KB  - 16bit adrs	*/
	BIB_ENUM_I2C_EEP24C64,	/* EEPROM  8KB  - 16bit adrs	*/
	BIB_ENUM_I2C_EEP24C128,	/* EEPROM 16KB  - 16bit adrs	*/
	BIB_ENUM_I2C_EEP24C256,	/* EEPROM 32KB  - 16bit adrs	*/
	BIB_ENUM_I2C_EEP24C512,	/* EEPROM 64KB  - 16bit adrs	*/
	BIB_ENUM_I2C_RsvdMem11,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdMem12,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdMem13,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_EEP8BIT,	/* generic  8bit adrs device	*/
	BIB_ENUM_I2C_EEP16BIT,	/* generic 16bit adrs device	*/

	/* 32~63: sensors/controllers */
	BIB_ENUM_I2C_MAX1617,	/* 2 channel temperature sensor	*/
	BIB_ENUM_I2C_LM75,	/* 1 channel temperature sensor	*/
	BIB_ENUM_I2C_LM81,	/* temp./voltage/fan sensor	*/
	BIB_ENUM_I2C_LM80,	/* temp./voltage/fan sensor	*/
	BIB_ENUM_I2C_SMT4004,	/* SUMMIT 4004 HotSwap controller */
	BIB_ENUM_I2C_SMT4042,	/* SUMMIT 4042 HotSwap controller */
	BIB_ENUM_I2C_LM87,	/* temp./voltage/fan sensor/ctrl */
	BIB_ENUM_I2C_MAX664X,	/* MAX6646/6647/6649 temp. sensor */
	BIB_ENUM_I2C_ADM1022,	/* temp. sensor, fan speed ctrl	*/
	BIB_ENUM_I2C_PCF8574,	/* remote 8-bit I/O expander	*/
	BIB_ENUM_I2C_RsvdSen10,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdSen11,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_PC8741x,	/* LPC Server I/O w/ ACCESS.bus	*/
	BIB_ENUM_I2C_RsvdSen13,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdSen14,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_RsvdSen15,	/* reserved - not used yet !!	*/
	BIB_ENUM_I2C_ICS932S203,/* ICS 932S203 frequency generator	*/
	BIB_ENUM_I2C_ICS9248_77,/* ICS 9248-77 frequency gen.	*/
	BIB_ENUM_I2C_CY28409,	/* CY28409 frequency generator	*/
	BIB_ENUM_I2C_CY28329,	/* CY28329 frequency generator	*/
	BIB_ENUM_I2C_W83791D,	/* W83791D HW monitor (Winbond)	*/
	BIB_ENUM_I2C_ICS932S208,/* ICS 932S208 frequency generator	*/
	BIB_ENUM_I2C_ICS9DB108,	/* ICS 9DB108 differential buffer	*/
	BIB_ENUM_I2C_MAX6900	/* Real Time Clock (RTC)	*/
    } BIB_I2cDev;


/*
 * BIB Array Data Type Definition for I2C-bus Topology - "BUS_TOPO"
 *
 * Each entry of the array consists of an I2C-bus device address and a
 * device type as specified by "BIB_ENUM_DEF BIB_I2cDev" above.  If the
 * device type is BIB_ENUM_I2C_BUS_ID, the <DevAdrs> field contains an
 * I2C bus number instead of a device address.  This signals the begin
 * of a new bus, and all the following devices belong to this bus.
 * If an I2C-bus multiplexer device is used, e.g. a PCA9542, two or more
 * "bus segments" will follow, regarding to the channel count of the
 * multiplexer.
 * The end of a bus - or bus segment, is signaled by setting bit 0 of the
 * last device address to 1.  This bit is the R/W bit and not part of the
 * address, so it can be used for that purpose.
 */
BIB_ARRAY_DEF
    {
	BIB_NUM   (DevAdrs);		/* I2C device address	*/
	BIB_ENUM  (BIB_I2cDev,DevType);	/* I2C device type	*/
    } BIB_I2C_BUS_TOPO;


/*
 * BIB Structure Definitions
 */

typedef	struct		/* BIB 2.0 structure ==== DO NO MORE USE ==== */
    {
/*000*/	UINT8	 LengthCode;		/* BIB block length (coded)	*/
/*001*/	UINT8	 BlockId;		/* BIB block Id is 0x20		*/
/*002*/	BIB_DIR	 Directory;		/* first (!) directory		*/
/*010*/	char	 ForceIdentifier[16];	/* "FORCE COMPUTERS\0"		*/
/*020*/	UINT8	 dataArea[BIB_MAX_SIZE-0x20];		/* data....	*/

/***	.......
	UINT8	CheckSumMSB;		The checkum is always stored
	UINT8	CheckSumLSB;		at the end of the BIB image!
***/
    } BIB_V20;

typedef	struct		/* BIB 2.1 structure */
    {
/*000*/	UINT8	 LengthCode;		/* BIB block length (coded)	*/
/*001*/	UINT8	 BlockId;		/* BIB block Id is 0x21		*/
/*002*/	char	 Motorola[9];		/* "MOTOROLA\0"			*/
/*00B*/	UINT8	 dataArea[BIB_MAX_SIZE-0x0A];		/* data....	*/

/***	.......
	UINT8	CheckSumMSB;		The checkum is always stored
	UINT8	CheckSumLSB;		at the end of the BIB image!
***/
    } BIB_V21;

/* Definition of BIB element structures */

/*
 * BIB element #0 - "ProductName" (short name)
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,SIZE_UNKNOWN); /* product name (short name)	*/
    } BIB_ELEM_ProductName;

/*
 * BIB element #1 - "SerialNumber" (PCA)
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,16);		/* max. 16 bytes for serial number */
    } BIB_ELEM_SerialNumber;

/*
 * BIB element #2 - "HostId"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_DATA  (Data,4);		/* exactly one host id */
    } BIB_ELEM_HostId;

/*
 * BIB element #3 - "EthernetAdrs"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_DATA  (Data,SIZE_UNKNOWN);	/* ethernet address(es) */
    } BIB_ELEM_EthernetAdrs;

/*
 * BIB element #4 - "RevInfo"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_DATE  (BIB_DefinitionDate);	/* date of BIB definition	*/
	BIB_DATE  (TestDate);		/* board test date, init to 0	*/
	BIB_DATE  (LastUpdate);		/* last update of board, init 0	*/
	BIB_NUM   (PCB_Revision);	/* PCB revision (1.3 = 0x13)	*/
	BIB_STR   (BoardRevision,3);	/* board rev. as defined by SAP	*/
	BIB_DATA  (MfgDateTime,3);	/* Manufacturing Date/Time (ICT)*/
	BIB_STR   (FinalRevision,3);	/* revision of the final good	*/
	BIB_DATA  (FT_DateTime,3);	/* Date/Time (Functional Test)	*/
	BIB_STR   (PCB_RevStr,6);	/* PCB revision string (ASCIIZ)	*/
    } BIB_ELEM_RevInfo;

/*
 * BIB element #5 - "Processor"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_NUM16 (Speed);		/* CPU speed in MHz (MSB/LSB)	*/
	BIB_FLAGS (Attr,8);		/* 8 flags for CPU attributes	*/
	BIB_MEMSIZE (CacheSize,3);	/* L1, L2, L3 cache size	*/
	BIB_STR   (Type,SIZE_UNKNOWN);	/* ASCIIZ name of the processor	*/
    } BIB_ELEM_Processor;

/*
 * BIB element #6 - "OnbrdDevs"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_FLAGS (Flag,SIZE_UNKNOWN);	/* flags for existing devices	*/
    } BIB_ELEM_OnbrdDevs;

/*
 * BIB element #7 - "OnbrdMemory"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM     (Size);		/* size of the element in bytes	*/
	BIB_MEMSIZE (MemSize,SIZE_UNKNOWN); /* size code, eg. for NVRAM	*/
    } BIB_ELEM_OnbrdMemory;

/*
 * BIB element #8 - "FlashMemory"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM     (Size);		/* size of the element in bytes	*/
	BIB_MEMSIZE (MemSize,SIZE_UNKNOWN); /* size code of flash memory */
    } BIB_ELEM_FlashMemory;

#if FALSE
/*
 * BIB element #9 - "ProductSpecInfo"
 *
 * If the following structure is needed, it must be defined in the product
 * specific header file, e.g. "frc60.h".  The first element of the structure
 * must always contain the size (automatically set by the "mk_bib" tool).
 *
 * NOTE: The following is only an example.  Define your own product specific
 *       BIB structure in the product specific header file, e.g. "frc60.h".
 */
/*BIB_STRUCT_DEF*/
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/

	/* product specific information (example) */

	BIB_DATA  (Data,2);		/* example data */
    } BIB_ELEM_ProductSpecInfo;
#endif


/*
 * BIB element #10 - "FuncTest"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_DATA  (Data,24);		/* functional test data		*/
    } BIB_ELEM_FuncTest;

/*
 * BIB element #11 - "ExtdBoardInfo"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM     (Size);		/* size of the element in bytes	*/
	BIB_NUM16   (BusSpeed);		/* bus speed in MHz (MSB/LSB)	*/
	BIB_MEMSIZE (L1InstrCache,1);	/* Lvl 1 instruction cache size	*/
	BIB_MEMSIZE (L1DataCache,1);	/* Lvl 1 data cache size	*/
	BIB_MEMSIZE (L2InstrCache,1);	/* Lvl 2 instruction cache size	*/
	BIB_MEMSIZE (L2DataCache,1);	/* Lvl 2 data cache size	*/
	BIB_MEMSIZE (ExtraCache,1);	/* extra cache, e.g. branch cache */
    } BIB_ELEM_ExtdBoardInfo;

/*
 * BIB element #12 - "FileId"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,SIZE_UNKNOWN); /* file Id, Oracle item number	*/
    } BIB_ELEM_FileId;

/*
 * BIB element #13 - "PartNumber" (PCA)
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,SIZE_UNKNOWN); /* product part number		*/
    } BIB_ELEM_PartNumber;

/*
 * BIB element #14 - "I2cBus"
 *
 * IMPORTANT: Do not forget to terminate an I2C-bus segment by setting
 *            bit 0 of the last device address in the list to 1!
 *            See also structure BIB_I2C_BUS_TOPO.
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_DATA  (Reserved,8);		/* reserved - do not use !!	*/
	BIB_ARRAY (BIB_I2C_BUS_TOPO,BusTopo,SIZE_UNKNOWN); /* bus topology */
    } BIB_ELEM_I2cBus;

/*
 * BIB element #15 - "IPMI"
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_ENUM  (BIB_EntityId,EntityId);	/* entity ID	*/
    } BIB_ELEM_IPMI;

/*
 * BIB element #16 - "IBMU" - only used for the IBMU BIB
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_NUM   (DeviceRevision);	/* hardware revision (bit 3..0)	*/
	BIB_DATA  (GUID,16);		/* Globally Unique Identifier	*/
    } BIB_ELEM_IBMU;

/*
 * BIB element #17 - "IBMU_Host" - only used for the base board BIB
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_NUM16 (BusAdrsLPC_0);	/* adrs of KCS #0 (0 if no LPC)	*/
	BIB_NUM16 (BusAdrsLPC_1);	/* adrs of KCS #1 (0 if no LPC)	*/
	BIB_NUM16 (BusAdrsLPC_2);	/* adrs of KCS #2 (0 if no LPC)	*/
	BIB_NUM16 (ProductId);		/* IPMI product ID (5 + 11 bit)	*/
	BIB_FLAGS (Flag,32);		/* flags tell what is supported	*/
    } BIB_ELEM_IBMU_Host;

/*
 * BIB element #18 - "FinalSerNum" (TLA)
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,16);		/* max. 16 bytes for serial number */
    } BIB_ELEM_FinalSerNum;

/*
 * BIB element #19 - "LongProductName" (TLA)
 *
 * Make string as large as you really need it by filling in the appropriate
 * number of dots when including it in your "*.bib" file!
 * For example, "......." reserves space for max. 7 characters.
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,SIZE_UNKNOWN); /* product name (long name)	*/
    } BIB_ELEM_LongProductName;

/*
 * BIB element #20 - "ATCA_eKeying"
 *
 * This element is used by ATCA blades for e-keying.  It contains data for
 * the "Board Point-to-Point Connectivity Record" beginning at offset 10
 * (decimal) within the record, i.e. the <OEM_GUID_Count> field.  The next
 * field is <GUID_LinkDesc>, a data array which contains the OEM GUIDs
 * (n * 16 bytes, it exists only if <OEM_GUID_Count> is not 0) followed by
 * the list of link descriptors.  The number of link descriptors can be
 * calculated by: (<Size> - 1 - (<OEM_GUID_Count> * 16)) / 4
 *
 * IMPORTANT:	All data within the BIB is BIG ENDIAN!  This significantly
 *		improves the readability of BIB definition files.  Within this
 *		element it is recommended to specify the <OEM_GUID_Count> as
 *		decimal value, the OEM GUIDs as a list of 16 hex bytes,
 *		followed by the link descriptors, consisting of 4 binary
 *		or hex bytes each.
 *
 * Example 1:	Standard Channels, no OEM GUIDs
 *	ATCA_eKeying.OEM_GUID_Count = 0
 *	ATCA_eKeying.GUID_LinkDesc  =
 *					# Link Descriptors
 *			# |   Designator  | Link Type |Ext|.Group ID
 *			0b11110100, 0b00010000, 0b00110000, 0x00,  # PICMG 3.2
 *			0b00010100, 0b00010000, 0b00100000, 0x00,  # PICMG 3.1
 *			0b00100100, 0b00010000, 0b00100000, 0x00,  # PICMG 3.1
 *			0b10100100, 0b00010000, 0b00100000, 0x00   # PICMG 3.1
 *
 * Example 2:	Two OEM GUIDs
 *	ATCA_eKeying.OEM_GUID_Count = 2
 *	ATCA_eKeying.GUID_LinkDesc  =
 *					# 2 GUIDs
 *			0xE4,0x75,0x59,0x80,0x03,0x6F,0x11,0xDA,  # GUID 0
 *			0x80,0x01,0x00,0x80,0x42,0x19,0x34,0xC4,
 *			0xE8,0x5B,0x8E,0xB2,0xEC,0xC4,0x11,0xD6,  # GUID 1
 *			0x80,0x01,0x12,0x34,0x56,0x78,0x9A,0xFF,
 *					# Link Descriptors
 *			# |   Designator  | Link Type |Ext|.Group ID
 *			0b11110100, 0b00010000, 0b00110000, 0x00,  # PICMG 3.2
 *			0b00010100, 0b00010000, 0b00100000, 0x00,  # PICMG 3.1
 *			0b00100100, 0b00011111, 0b00000000, 0x00,  # OEM 0
 *			0b10100100, 0b00011111, 0b00010000, 0x00   # OEM 1
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_NUM   (OEM_GUID_Count);	/* offs 10: OEM GUID Count <n>	*/
	BIB_DATA  (GUID_LinkDesc,SIZE_UNKNOWN);	/* offs 11: GUIDs, followed */
						/* by a list of Link Descr. */
    } BIB_ELEM_ATCA_eKeying;

/*
 * BIB element #21 - "FinalPartNumber" (TLA)
 */
BIB_STRUCT_DEF
    {
	BIB_NUM   (Size);		/* size of the element in bytes	*/
	BIB_STR   (String,SIZE_UNKNOWN); /* fnal product part number	*/
    } BIB_ELEM_FinalPartNumber;

#endif	/* __INCfrcBIBh */

