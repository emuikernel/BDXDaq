/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 *	SCCS/s.drv6016.h 1.2 11/13/97 10:46:12
 *--------------------------------------------------------------------------
 *	Device Driver Definitions
 *--------------------------------------------------------------------------
 */

#ifndef	DRV6016_H
#  define DRV6016_H
  
#  ifndef	lint
static char drv6016_h_sccs_id[] = "@(#)drv6016.h 1.2 98/07/24 VMIC";
#  endif /* !DRV6016_H AND !lint */
  
#  include <sys/ioctl.h>
#  include <vxWorks.h>
#  include <semLib.h>
  
/* Error codes returned by the driver interface routines */
typedef enum V6016Status
{
  NO_ERR,				/* No error detected		 */
  INVALID_BOARD_ID_ERR,		/* Invalid board ID		 */
  DEVICE_BUSY_ERR,		/* Device currently in use	 */
  CARD_SELFTEST_ERR,		/* Board failed selftest	 */
  INVALID_PARAMETER_ERR,		/* Invalid parameter		 */
  RESOURCE_ERR,			/* Cannot obtain sys resource	 */
  BOARD_ACCESS_ERR,		/* Cannot get access board	 */
  MEMORY_ALLOCATION_ERROR,	/* Memory allocation failed	 */
  DEVICE_ADD_ERROR,		/* Failed device add		 */
  ALREADY_OPEN_ERROR,		/* Device already open		 */
  NOT_OPEN_ERROR			/* Device is not open error	 */
} V6016_STATUS;
  
typedef ULONG		CARD_ID;	/* Type of the card id		 */
  
#define NULL_CARD_ID 0L
  
/* Bus Arbitration Modes used in Config6016Controller() */
typedef enum ArbMode
{
  ROUND_ROBIN = 0x00,		/* Round Robin VME Bus		 */
  PRIORITY = 0x08			/* Priority VME Bus		 */
} ARB_MODE;
  
/* Codes to be ORed together and used in Request6016IntNotify()	 */
  
#define NO_NOTIFY		0x00	/* Stop notification		 */
#define COS_MSK		0x80	/* CTS or DCD Changed State	 */
#define ERR_MSK		0x40	/* FE, PE OVERRUN or ILL CODE	 */
#define BREAK_MSK		0x20	/* Break Received		 */
#define RCV_MSK		0x10	/* User Receive Buffer Has Data	 */
#define TX_MSK		0x08	/* User Xmit Can Take Data	 */
#define ERR_INT		0x04	/* Board Error Interrupt	 */
  
/* Flow Control Types used in Config6016Channel() */
typedef enum FlowCtrl
{
  NO_FLOW = 0x00,			/* No flow control		 */
  XON_XOFF_FLOW = 0x20,		/* XON/XOFF flow control	 */
  ANY_XOFF_FLOW = 0x40,		/* Any/XOFF flow control	 */
  RTS_CTS_FLOW = 0x60		/* RTS/CTS flow control		 */
} FLOW_CTRL;
  
/* Baud Rates used in Config6016Channel() */
typedef enum BaudRate
{
  BAUDS_50 = 0x00,		/* 50 Bits per Second		 */
  BAUDS_110 = 0x01,		/* 110 Bits per Second		 */
  BAUDS_134p5 = 0x02,		/* 134.5 Bits per Second	 */
  BAUDS_200 = 0x03,		/* 200 Bits per Second		 */
  BAUDS_300 = 0x04,		/* 300 Bits per Second		 */
  BAUDS_600 = 0x05,		/* 600 Bits per Second		 */
  BAUDS_1200 = 0x06,		/* 1200 Bits per Second		 */
  BAUDS_1050 = 0x07,		/* 1050 Bits per Second		 */
  BAUDS_2400 = 0x08,		/* 2400 Bits per Second		 */
  BAUDS_4800 = 0x09,		/* 4800 Bits per Second		 */
  BAUDS_7200 = 0x0A,		/* 7200 Bits per Second		 */
  BAUDS_9600 = 0x0B,		/* 9600 Bits per Second		 */
  BAUDS_38400 = 0x0C,		/* 38400 Bits per Second	 */
  BAUDS_75 = 0x10,		/* 75 Bits per Second		 */
  BAUDS_150 = 0x13,		/* 150 Bits per Second		 */
  BAUDS_2000 = 0x17,		/* 2000 Bits per Second		 */
  BAUDS_1800 = 0x1A,		/* 1800 Bits per Second		 */
  BAUDS_19200 = 0x1C		/* 19200 Bits per Second	 */
} BAUD_RATE;
  
/* Number of Stop Bits values used in Config6016Channel() */
typedef enum StopBits
{
  ONE_STOP_BIT = 0x00,		/* One bit-time stop duration	 */
  TWO_STOP_BITS = 0x20,		/* Two bit-times stop duration	 */
} STOP_BITS;
  
/* Parity Type values used in Config6016Channel() */
typedef enum ParityType
{
  ODD_OR_EVEN_PARITY = 0x00,	/* Parity Sense is valid	 */
  FORCE_PARITY = 0x08,		/* Force to Parity Sense state	 */
  NO_PARITY = 0x10		/* No parity bit		 */
} PARITY_TYPE;
  
/* Parity Sense values used in Config6016Channel() */
typedef enum ParitySense
{
  EVEN_PARITY = 0x00,  /* Even Parity */
  ODD_PARITY = 0x04    /* Odd Parity */
} PARITY_SENSE;

/* Number of Data Bits used in Config6016Channel() */
typedef enum NumDataBits
{
	FIVE_BITS = 0x00,		/* 5 Data Bits			 */
	SIX_BITS = 0x01,		/* 6 Data Bits			 */
	SEVEN_BITS = 0x02,		/* 7 Data Bits			 */
	EIGHT_BITS = 0x03		/* 8 Data Bits			 */
} NUM_DATA_BITS;
  
/* Input Timeout values used in Config6016Channel() */
typedef enum Timeouts
{
  NO_TIMEOUT = 0x00,          /* No Timeout			 */
  THREE_CHAR_TIMEOUT = 0x40,  /* max(3 chars, break duration)	 */
  HALF_SEC_TIMEOUT = 0x80,    /* 1/2 Second Timeout		 */
  ONE_SEC_TIMEOUT = 0xC0      /* 1 Second Timeout		 */
} TIMEOUTS;

/* Global register offsets used in Read6016GlobalRegister()
   and Write6016GlobalRegister() */
typedef enum GlobalReg
{
  BRD_ID_REG = 0x00,   /* Board Identification */
  STFLAG_REG = 0x01,   /* Selftest Flag */
  ROM_VER_REG = 0x02,  /* ROM Version */
  CTR0_REG = 0x04,     /* Control Register 0 */
  CTR1_REG = 0x05,     /* Control Register 1 */
  GO_REG = 0x06,       /* Start/Stop Serial Channel */
  TX_REG = 0x08,       /* Channel Transmit On/Off */
  RX_REG = 0x0A,       /* Channel Receive On/Off */
  BREAK_REG = 0x0C,    /* Send Break Register */
  CR2_REG = 0x0E,      /* Control Register 2 */
  RES_REG = 0x0F,      /* Reserved Register */
  SZ_AM_REG = 0x10,    /* Size and Address Modifier */
  ST_PROC_REG = 0x11,  /* Selftest Procedure */
  ER_MSK_REG = 0x12,   /* Error Mask */
  ER_VEC_REG = 0x13,   /* Error Vector */
  BUFBASE_REG = 0x14,  /* Buffer Base Address */
  GST_REG = 0x18,      /* Global Status */
  MAS_GRN_REG = 0x1A   /* Master Granularity */
} GLOBAL_REG;
  
/* Channel register offsets used in Read6016ChannelRegister() and
 Write6016ChannelRegister() */
typedef enum ChannelReg
{
  CST_REG = 0x00,      /* Channel Selftest */
  CH_MSK_REG = 0x02,   /* Channel Interrupt Mask */
  CH_VEC_REG = 0x03,   /* Channel Interrupt Vector */
  EOB_REG = 0x04,      /* End of Buffer Character */
  XOFF_REG = 0x05,     /* XOFF Character Register */
  XON_REG = 0x06,      /* XON Character Register */
  BRK_DUR_REG = 0x07,  /* Break Duration Register */
  SZ_RING_REG = 0x08,  /* Ring Buffer Size Register */
  LO_RING_REG = 0x09,  /* Ring Buffer Low-Water Mark */
  HI_RING_REG = 0x0A,  /* Ring Buffer High-Water Mark */
  CH_CON1_REG = 0x0B,  /* Channel Control Register 1 */
  CH_CON2_REG = 0x0C,  /* Channel Control Register 2 */
  SZ_UBUF_REG = 0x0D   /* User Buffer Size Register */
} CHANNEL_REG;
  
/* Bit flags (which may be OR'ed together) for Run6016Selftest() */
typedef enum Selftests
{
	TEST_DMA_R = 0x20,		/* VME Bus DMA Read Test	 */
	TEST_DMA_W = 0x10,		/* VME Bus DMA Write Test	 */
	TEST_BERR = 0x08,		/* Bus Error Test		 */
	TEST_UART = 0x04,		/* UART Local Loopback Test	 */
	TEST_TIMER = 0x02,		/* Timer Test			 */
	TEST_VINT = 0x01		/* VME Bus Interrupt Test	 */
} SELFTESTS;

/* The following may be ORed together and is sent
   as a parameter to Config6016Errors() */
typedef enum Errors
{
  REGBUSY_MSK = 0x80,     /* Register Busy */
  RFNZ_MSK = 0x40,        /* Reserved Field is Non-Zero */
  STFAIL_MASK = 0x20,     /* Self-Test Failed */
  HDWE_ERR_MSK = 0x10,    /* Hardware Error */
  BERR_MASK = 0x08,       /* Bus Error */
  ER_LEVEL = 0x07         /* VME Bus Interrupt Level */
} ERRORS;

/* Bus Release Modes */
typedef enum ReleaseMode
{
  RELROR = 0x00,            /* Release on Request */
  RELRWD = 0x10,            /* Release When Done */
  RELROC = 0x20,            /* Release on BLCR* */
  RELBCAP = 0x30            /* Bus Capture and Hold */
} RELEASE_MODE;

/* Shape of the generic register object */
typedef union RegisterValueType
{
  unsigned char  ByteValue;  /* As 8-bit value */
  unsigned short WordValue;  /* As 16-bit value */
  unsigned long	 LongValue;  /* As 32-bit value */
} REGISTER_VALUE_TYPE;

/* Buffer classes */
typedef enum BufferType
{
  TX_BUFFER,  /* Transmit Buffer */
  RX_BUFFER	  /* Receive Buffer */
} BUFFER_TYPE;
  
/*
 *------------------------------------------------------------------------
 * Ioctl(2) command definitions
 *------------------------------------------------------------------------
 */
  
#define NO_COMMAND		      _IO(  'S',  0)
  
/* Configure buffer */
#define CONFIG_6016_BUFFER	      _IOWR('S',  3, CONFIG_BUFFER)
typedef struct ConfigBuffer
{
  ULONG         BufferAddress;
  RELEASE_MODE  ReleaseMode;
  UINT          FairnessTimeout;
  BOOL          UseA32;
  BOOL          UseSupervisor;
} CONFIG_BUFFER;
  
/* Send break signal on channel */
#define SEND_6016_BREAK	      _IOWR('S',  4, UINT)

/* Run 6016 selftests */
#define RUN_6016_SELFTEST	      _IOWR('S',  5, SELFTESTS)

/* Reset the 6016 device */
#define RESET_6016_BOARD	      _IO(  'S',  6)

/* Configure 6016 as VMEbus system controller (slot #1 only) */
#define CONFIG_6016_CONTROLLER      _IOWR('S',  7, CONFIG_CONTROLLER)  
typedef struct ConfigController
{
  ARB_MODE  ArbitrationMode;
  UINT      BusErrorTimeout;
} CONFIG_CONTROLLER;
  
/* Enable or disable a 6016 channel					 */
  
#define SET_6016_CHANNEL_ENABLE     _IOWR('S',  8, SET_CHANNEL_ENABLE)
typedef struct SetChannelEnable
{
  BOOL  Enable;
} SET_CHANNEL_ENABLE;

/* Define 6016 hardware device error conditions	*/
#define CONFIG_6016_ERRORS	      _IOWR('S',  9, UINT)
  
/* Read or write 6016 global registers */
#define READ_6016_GLOBAL_REGISTER   _IOWR('S', 10, GLOBAL_REGISTER)
#define WRITE_6016_GLOBAL_REGISTER  _IOWR('S', 11, GLOBAL_REGISTER)
typedef struct GlobalRegister
{
  GLOBAL_REG          GlobalReg;
  REGISTER_VALUE_TYPE Value;
} GLOBAL_REGISTER;

/* Set 6016 conditions for which the semaphore will be given */
#define REQUEST_6016_INT_NOTIFY     _IOWR('S', 12, REQUEST_INT_NOTIFY)
typedef struct RequestIntNotify
{
  ULONG   NotifyConditions;
  SEM_ID  SemID;
} REQUEST_INT_NOTIFY;

/* Configure characteristics of one 6016 channel */
#define CONFIG_6016_CHANNEL	      _IOWR('S', 13, CONFIG_CHANNEL)
typedef struct ConfigChannel
{
  FLOW_CTRL      FlowControl;
  BAUD_RATE      BaudRate;
  TIMEOUTS       InputTimeout;
  STOP_BITS      StopBits;
  PARITY_TYPE    ParityType;
  PARITY_SENSE   ParitySense;
  NUM_DATA_BITS  NumBits;
} CONFIG_CHANNEL;

/* Return last 6016 device error code (if any) */
#define GET_6016_DEVICE_ERROR	    _IOWR('S', 14, V6016_STATUS)

/* Read and write one 6016 channel register */
#define READ_6016_CHANNEL_REGISTER  _IOWR('S', 15, CHANNEL_REGISTER)
#define WRITE_6016_CHANNEL_REGISTER _IOWR('S', 16, CHANNEL_REGISTER)
typedef struct ChannelRegister
{
  CHANNEL_REG          ChannelReg;
  REGISTER_VALUE_TYPE  Value;
} CHANNEL_REGISTER;

/* Read and write a 6016 data buffer */
#define READ_6016_BUFFER	       _IOWR('S', 17, BUFFER_STRUCT)
#define WRITE_6016_BUFFER	       _IOWR('S', 18, BUFFER_STRUCT)
typedef struct BufferStruct
{
  BUFFER_TYPE  WhichBuffer;
  ULONG        Offset;
  UINT         ByteCount;
  UCHAR       *pBuffer;
} BUFFER_STRUCT;

/*
 *------------------------------------------------------------------------
 * Templates for the 6016 Driver interface routines
 *------------------------------------------------------------------------
 * In addition to these driver-specific routines, the normal VxWORKS I/O
 * calls of "open(2)", "close(2)", and "ioctl(2)" may be used.
 *------------------------------------------------------------------------
 */
  
    
extern V6016_STATUS v6016DevCreate(char *pszName,
                                   ULONG CardAddress,
                                   ULONG BusRequestLevel,
                                   UINT InterruptLevel1,
                                   UINT InterruptVector1,
                                   UINT InterruptLevel2,
                                   UINT InterruptVector2,
                                   BOOL *pbTTYDeviceArray);
extern STATUS       v6016DrvRemove(void);
    
  
#endif /* !DRV6016_H */
