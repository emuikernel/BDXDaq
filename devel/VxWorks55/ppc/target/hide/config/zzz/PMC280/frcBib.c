/* frcBib.c - include file for BIB show routine */

/*
This file was automatically generated via the following command:

  mk_bib -show frcBib.c ../../h/eeprom/frcBib.h 

It contains all BIB data types, element names and field names.
The file can be included by another module, e.g. a generic
BIB show routine, to display the contents of a BIB structure.

NOTE: All definitions below assume a version 2.0 BIB.
*/


/* BIB data type definitions */

typedef enum
{
    TYPE_BIB_DATA,
    TYPE_BIB_STR,
    TYPE_BIB_NUM,
    TYPE_BIB_NUM16,
    TYPE_BIB_NUM32,
    TYPE_BIB_DATE,
    TYPE_BIB_FLAGS,
    TYPE_BIB_MEMSIZE,
    TYPE_BIB_ENUM,
    TYPE_BIB_ARRAY
} BIB_DATA_TYPE;


static char * const dataTypeStr[] =
{
    "BIB_DATA",
    "BIB_STR",
    "BIB_NUM",
    "BIB_NUM16",
    "BIB_NUM32",
    "BIB_DATE",
    "BIB_FLAGS",
    "BIB_MEMSIZE",
    "BIB_ENUM",
    "BIB_ARRAY",
     NULL		/* end of list */
};


/* typedefs to hold the BIB structure */

typedef struct		/* field description structure	*/
{
    char	 *field;	/* field name		*/
    BIB_DATA_TYPE type;		/* field type		*/
    int		  size;		/* field size in bytes	*/
    int		  elemCnt;	/* number of elements	*/
    void	 *pElemDesc;	/* element description	*/
} BIB_FIELD;

typedef struct		/* BIB element structure	*/
{
    char	 *element;	/* BIB element name	*/
    BIB_FIELD	 *pFields;	/* field description	*/
} BIB_ELEM;


/* BIB flag names of each element */

static char *bibFlags_Processor[] =
{
    "MMU",
    "FPU",
     NULL
};

static char *bibFlags_OnbrdDevs[] =
{
    "Serial_1",
    "Serial_2",
    "Serial_3",
    "Serial_4",
    "Serial_5",
    "Serial_6",
    "Serial_7",
    "Serial_8",
    "Serial_9",
    "Serial_10",
    "Serial_11",
    "Serial_12",
    "Serial_13",
    "Serial_14",
    "Serial_15",
    "Serial_16",
    "Parallel_1",
    "Parallel_2",
    "Parallel_3",
    "Parallel_4",
    "Parallel_5",
    "Parallel_6",
    "Parallel_7",
    "Parallel_8",
    "SCSI_1",
    "SCSI_2",
    "SCSI_3",
    "SCSI_4",
    "IDE_1",
    "IDE_2",
    "IDE_3",
    "IDE_4",
    "Floppy_1",
    "Floppy_2",
    "Floppy_3",
    "Floppy_4",
    "USB_1",
    "USB_2",
    "USB_3",
    "USB_4",
    "Bridge_1",
    "Bridge_2",
    "Bridge_3",
    "Bridge_4",
    "Bridge_5",
    "Bridge_6",
    "Bridge_7",
    "Bridge_8",
    "NVRAM_1",
    "NVRAM_2",
    "RTC",
    "Keyboard",
    "Mouse",
    "Graphics",
    "Audio",
    "FloppyDisk",
    "SpecDevice_1",
    "SpecDevice_2",
    "SpecDevice_3",
    "SpecDevice_4",
    "HardDisk_1",
    "HardDisk_2",
    "HardDisk_3",
    "HardDisk_4",
    "I2cBusCtrl_1",
    "I2cBusCtrl_2",
    "I2cBusCtrl_3",
    "I2cBusCtrl_4",
    "I2cBusCtrl_5",
    "I2cBusCtrl_6",
    "I2cBusCtrl_7",
    "I2cBusCtrl_8",
     NULL
};

static char *bibFlags_IBMU_Host[] =
{
    "ICMB_Intf",
    "PostPort",
    "Reserved_1",
    "SysEnLowActive",
    "NmiEdgeTrig",
    "NmiHighActive",
    "SmiEdgeTrig",
    "SmiHighActive",
    "TrigNmi",
    "TrigSmi",
    "TrigMsgIrq",
    "TrigHardReset",
    "TrigPowerDown",
    "TrigPowerCycle",
     NULL
};


/* BIB ENUM types */

static char *bibEnums_EntityId[] =
{
    "EID_Unspecified",
    "EID_Other",
    "EID_Unknown",
    "EID_Processor",
    "EID_Disk",
    "EID_Peripheral",
    "EID_SysMgmtModule",
    "EID_SysBoard",
    "EID_MemoryModule",
    "EID_ProcessorModule",
    "EID_PowerSupply",
    "EID_AddInCard",
    "EID_FrontPanelBoard",
    "EID_BackPanelBoard",
    "EID_PowerSysBoard",
    "EID_DriveBackplane",
    "EID_SysIntExpBoard",
    "EID_OthSysBoard",
    "EID_ProcessorBoard",
    "EID_PowerUnit",
    "EID_PowerModule",
    "EID_PowerMgmtBoard",
    "EID_ChassisBpBoard",
    "EID_SysChassis",
    "EID_SubChassis",
    "EID_OthChassisBoard",
    "EID_DiskDriveBay",
    "EID_PeripheralBay",
    "EID_DeviceBay",
    "EID_CoolingDevice",
    "EID_CoolingUnit",
    "EID_Cable",
    "EID_MemoryDevice",
    "EID_SysMgmtSoftware",
    "EID_BIOS",
    "EID_OS",
    "EID_SysBus",
    "EID_Group",
    "EID_RemMgmtComDev",
     NULL
};

static char *bibEnums_I2cDev[] =
{
    "I2C_BUS_ID",
    "I2C_UNKNOWN",
    "I2C_IPMB",
    "I2C_RsvdId3",
    "I2C_RsvdId4",
    "I2C_RsvdId5",
    "I2C_RsvdId6",
    "I2C_RsvdId7",
    "I2C_PCA9540",
    "I2C_PCA9542",
    "I2C_PCA9545",
    "I2C_PCA9544",
    "I2C_RsvdMux4",
    "I2C_RsvdMux5",
    "I2C_RsvdMux6",
    "I2C_RsvdMux7",
    "I2C_EEP24C01",
    "I2C_EEP24C02",
    "I2C_EEP24C04",
    "I2C_EEP24C08",
    "I2C_EEP24C16",
    "I2C_EEP24C17",
    "I2C_EEP24C32",
    "I2C_EEP24C64",
    "I2C_EEP24C128",
    "I2C_EEP24C256",
    "I2C_EEP24C512",
    "I2C_RsvdMem11",
    "I2C_RsvdMem12",
    "I2C_RsvdMem13",
    "I2C_EEP8BIT",
    "I2C_EEP16BIT",
    "I2C_MAX1617",
    "I2C_LM75",
    "I2C_LM81",
    "I2C_LM80",
    "I2C_SMT4004",
    "I2C_SMT4042",
    "I2C_LM87",
    "I2C_MAX664X",
    "I2C_ADM1022",
    "I2C_PCF8574",
    "I2C_RsvdSen10",
    "I2C_RsvdSen11",
    "I2C_PC8741x",
    "I2C_RsvdSen13",
    "I2C_RsvdSen14",
    "I2C_RsvdSen15",
    "I2C_ICS932S203",
    "I2C_ICS9248_77",
    "I2C_CY28409",
    "I2C_CY28329",
    "I2C_W83791D",
    "I2C_ICS932S208",
    "I2C_ICS9DB108",
    "I2C_MAX6900",
     NULL
};


/* BIB ARRAY types */

static BIB_FIELD bibArray_I2C_BUS_TOPO[] =
{
    { "DevAdrs",             TYPE_BIB_NUM,       1,   0, NULL                 },
    { "DevType",             TYPE_BIB_ENUM,      1,  56, bibEnums_I2cDev      },
    {  NULL,                 0,                  0,   0, NULL                 }
};


/* BIB field names of each element */

static BIB_FIELD bibFields_ProductName[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_SerialNumber[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,      16,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_HostId[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Data",                TYPE_BIB_DATA,      4,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_EthernetAdrs[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Data",                TYPE_BIB_DATA,    255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_RevInfo[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "BIB_DefinitionDate",  TYPE_BIB_DATE,      3,   0, NULL                 },
    { "TestDate",            TYPE_BIB_DATE,      3,   0, NULL                 },
    { "LastUpdate",          TYPE_BIB_DATE,      3,   0, NULL                 },
    { "PCB_Revision",        TYPE_BIB_NUM,       1,   0, NULL                 },
    { "BoardRevision",       TYPE_BIB_STR,       3,   0, NULL                 },
    { "MfgDateTime",         TYPE_BIB_DATA,      3,   0, NULL                 },
    { "FinalRevision",       TYPE_BIB_STR,       3,   0, NULL                 },
    { "FT_DateTime",         TYPE_BIB_DATA,      3,   0, NULL                 },
    { "PCB_RevStr",          TYPE_BIB_STR,       6,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_Processor[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Speed",               TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "Attr",                TYPE_BIB_FLAGS,     1,   2, bibFlags_Processor   },
    { "CacheSize",           TYPE_BIB_MEMSIZE,   3,   0, NULL                 },
    { "Type",                TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_OnbrdDevs[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Flag",                TYPE_BIB_FLAGS,     9,  72, bibFlags_OnbrdDevs   },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_OnbrdMemory[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "MemSize",             TYPE_BIB_MEMSIZE, 255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_FlashMemory[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "MemSize",             TYPE_BIB_MEMSIZE, 255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_FuncTest[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Data",                TYPE_BIB_DATA,     24,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_ExtdBoardInfo[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "BusSpeed",            TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "L1InstrCache",        TYPE_BIB_MEMSIZE,   1,   0, NULL                 },
    { "L1DataCache",         TYPE_BIB_MEMSIZE,   1,   0, NULL                 },
    { "L2InstrCache",        TYPE_BIB_MEMSIZE,   1,   0, NULL                 },
    { "L2DataCache",         TYPE_BIB_MEMSIZE,   1,   0, NULL                 },
    { "ExtraCache",          TYPE_BIB_MEMSIZE,   1,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_FileId[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_PartNumber[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_I2cBus[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "Reserved",            TYPE_BIB_DATA,      8,   0, NULL                 },
    { "BusTopo",             TYPE_BIB_ARRAY,   255,   0, bibArray_I2C_BUS_TOPO},
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_IPMI[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "EntityId",            TYPE_BIB_ENUM,      1,  39, bibEnums_EntityId    },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_IBMU[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "DeviceRevision",      TYPE_BIB_NUM,       1,   0, NULL                 },
    { "GUID",                TYPE_BIB_DATA,     16,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_IBMU_Host[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "BusAdrsLPC_0",        TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "BusAdrsLPC_1",        TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "BusAdrsLPC_2",        TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "ProductId",           TYPE_BIB_NUM16,     2,   0, NULL                 },
    { "Flag",                TYPE_BIB_FLAGS,     4,  14, bibFlags_IBMU_Host   },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_FinalSerNum[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,      16,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_LongProductName[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_ATCA_eKeying[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "OEM_GUID_Count",      TYPE_BIB_NUM,       1,   0, NULL                 },
    { "GUID_LinkDesc",       TYPE_BIB_DATA,    255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};

static BIB_FIELD bibFields_FinalPartNumber[] =
{
    { "Size",                TYPE_BIB_NUM,       1,   0, NULL                 },
    { "String",              TYPE_BIB_STR,     255,   0, NULL                 },
    {  NULL,                 0,                  0,   0, NULL                 }
};


/* BIB element names */

static BIB_ELEM bibElements[] =
{
    { "ProductName",          bibFields_ProductName          },
    { "SerialNumber",         bibFields_SerialNumber         },
    { "HostId",               bibFields_HostId               },
    { "EthernetAdrs",         bibFields_EthernetAdrs         },
    { "RevInfo",              bibFields_RevInfo              },
    { "Processor",            bibFields_Processor            },
    { "OnbrdDevs",            bibFields_OnbrdDevs            },
    { "OnbrdMemory",          bibFields_OnbrdMemory          },
    { "FlashMemory",          bibFields_FlashMemory          },
    { "ProductSpecInfo",      NULL                           },
    { "FuncTest",             bibFields_FuncTest             },
    { "ExtdBoardInfo",        bibFields_ExtdBoardInfo        },
    { "FileId",               bibFields_FileId               },
    { "PartNumber",           bibFields_PartNumber           },
    { "I2cBus",               bibFields_I2cBus               },
    { "IPMI",                 bibFields_IPMI                 },
    { "IBMU",                 bibFields_IBMU                 },
    { "IBMU_Host",            bibFields_IBMU_Host            },
    { "FinalSerNum",          bibFields_FinalSerNum          },
    { "LongProductName",      bibFields_LongProductName      },
    { "ATCA_eKeying",         bibFields_ATCA_eKeying         },
    { "FinalPartNumber",      bibFields_FinalPartNumber      },
    {  NULL,                  NULL                           }
};

