/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 *	SCCS/s.menu6016.c 1.2 11/13/97 10:46:13
 *--------------------------------------------------------------------------
 *	Menu Handler
 *--------------------------------------------------------------------------
 */

#ifndef	lint
static char menu6016_c_sccs_id[] = "@(#)menu6016.c 1.2 98/07/24 VMIC";
#endif	/* lint */

#include <vxWorks.h>
#include <semLib.h>
#include <ioLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dev6016.h"
#include "drv6016.h"
#include "params.h"

/* Configuration constants and other declarations			 */

#define DEBUG
#define MAX_LINE_SIZE      80
#define CHARACTER          0
#define LINE               1
#define TX_BUFFER_SIZE     4096
#define RX_BUFFER_SIZE     4096
#define SERIAL_BUFFER_SIZE (2*TX_BUFFER_SIZE)
#define TEN_SECONDS        (sysClkRateGet()*10)
#define BUFFER_SIZE        333

/*
 *------------------------------------------------------------------------
 * Menu function definitions
 *------------------------------------------------------------------------
 * Each selectable item on the menu gets a definition here.
 *------------------------------------------------------------------------
 */

#define INIT                  0
#define DEINIT                1
#define READ6016              2
#define WRITE6016             3
#define CONFIG                4
#define DUMP_BOARD_REG        5
#define DUMP_CHANNEL_REG      6
#define DUMP_CHANNEL_BUFFER   7
#define READ_WRITE_WRAP       8
#define SELECT_CHANNEL        9
#define EXIT                 10
#define CFG_BUFFER           11
#define SEND_BREAK           12
#define RUN_SELFTEST         13
#define RESET_BOARD          14
#define CFG_CONTROLLER       15
#define SET_CHAN_ENABLE      16
#define CONFIG_ERRORS        17
#define READ_BOARD_REG       18
#define WRITE_BOARD_REG      19
#define RQST_INT_NOTIFY      20
#define WAIT_FOR_NOTIFY      21
#define CFG_CHANNEL          22
#define GET_DEVICE_ERROR     23
#define READ_CHANNEL_REG     24
#define WRITE_CHANNEL_REG    25
#define RD_CHANNEL_RX_BUFFER 26
#define RD_CHANNEL_TX_BUFFER 27
#define WR_CHANNEL_RX_BUFFER 28
#define WR_CHANNEL_TX_BUFFER 29

/* Interface to driver-level						 */

extern V6016_STATUS     v6016DevCreate(
		char *pszName,
		ULONG CardAddress,
		ULONG BusRequestLevel,
		UINT InterruptLevel1,
		UINT InterruptVector1,
		UINT InterruptLevel2,
		UINT InterruptVector2,
        BOOL *pbTTYEnableArray
	);
extern STATUS           v6016DrvRemove(void);

/* Device file descriptors for each channel				 */

static int              theFileDesc[16] = {ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR};

/* Keeps track of the current serial channel in use			 */

static UINT             CurrentChannel = 0;
static SEM_ID           SemID = 0;
static BOOL             bInit = FALSE;

/* Function prototypes							 */

static void             Init(void);
static void             DeInit(void);
static void             SelectChannel(void);
static void             DumpBoardReg(CARD_ID FileDesc);
static void             DumpChannelReg(CARD_ID FileDesc);
static void             DumpChannelBuffer(CARD_ID FileDesc);
static void             ConfigBuffer(CARD_ID FileDesc);
static void             ReadWriteWrap(void);
static void             Read(CARD_ID FileDesc);
static void             Write(CARD_ID FileDesc);
static void             SendBreak(CARD_ID FileDesc);
static void             RunSelftest(CARD_ID FileDesc);
static void             ResetBoard(CARD_ID FileDesc);
static void             ConfigController(CARD_ID FileDesc);
static void             SetChannelEnable(CARD_ID FileDesc);
static void             ConfigErrors(CARD_ID FileDesc);
static GLOBAL_REG       GetBoardReg(void);
static CHANNEL_REG      GetChannelReg(void);
static void             ReadBoardReg(CARD_ID FileDesc);
static void             PrintBoardReg(GLOBAL_REG Reg,
				      REGISTER_VALUE_TYPE * pValue);
static void             PrintChannelReg(CHANNEL_REG Reg,
					REGISTER_VALUE_TYPE * pValue);
static void             WriteBoardReg(CARD_ID FileDesc);
static void             StoreBoardReg(GLOBAL_REG Reg,
				      REGISTER_VALUE_TYPE * pValue);
static void             StoreChannelReg(CHANNEL_REG Reg,
					REGISTER_VALUE_TYPE * pValue);
static void             RequestIntNotify(CARD_ID FileDesc);
static void             WaitForIntNotify(void);
static void             ConfigChannel(CARD_ID FileDesc);
static void             GetDeviceError(CARD_ID FileDesc);
static void             ReadChannelReg(CARD_ID FileDesc);
static void             WriteChannelReg(CARD_ID FileDesc);
static void             RdChannelBuffer(CARD_ID FileDesc,
				BUFFER_TYPE WhichBuffer);
static void             WrChannelBuffer(CARD_ID FileDesc,
				BUFFER_TYPE WhichBuffer);
static int              GetInt(char *Prompt, int Min, int Max);
static ULONG            GetHexLong(char *Prompt);
static USHORT           GetHexShort(char *Prompt);
static int              GetMenuItem(void);
static void             GetString(char *Prompt, char *theString,
				unsigned int size);
static void             DisplayError(CARD_ID FileDesc);

/*
 *------------------------------------------------------------------------
 * Init: perform the 6016 INIT driver call
 *------------------------------------------------------------------------
 */

static void
Init(void)
{
	ULONG                   CardAddress;
	ULONG                   BusRequestLevel;
	UINT                    InterruptLevel1;
	UINT                    InterruptVector1;
	UINT                    InterruptLevel2;
	UINT                    InterruptVector2;
	char                    DevName[80];
	char                    name[80];
	V6016_STATUS            Status;
	int                     iChannel;

  if(bInit)
  {
    logMsg("ERROR: Device Init Already called.\n",0,0,0,0,0,0);
    logMsg("       Must call Deinit before Init.\n",0,0,0,0,0,0);
  }
  else
  {
    /* Get the parameters 
    GetString("Enter Base Device Name:    ", name, sizeof(name));
    CardAddress = GetHexLong("Enter Card Address (hex): ");
    BusRequestLevel = GetInt("Enter Bus Request Level (0-3): ",0,3);
    InterruptLevel1 = GetInt("Enter Board Interrupt Level (1-7): ",1,7);
    InterruptVector1 =
      (UINT) GetHexShort("Enter Board Interrupt Vector (0-0xFF hex): ");
    InterruptLevel2 = GetInt("Enter Channel Interrupt Level (1-7): ",1,7);
    InterruptVector2 =
      (UINT) GetHexShort("Enter Channel Interrupt Vector (0-0xFF hex): ");
	*/
    strcpy(name,DEV_NAME);
    CardAddress = CARD_ADDR;
    BusRequestLevel  = 3;
    InterruptLevel1  = INTLEVEL1;
    InterruptVector1 = INTVECTOR1;
    InterruptLevel2  = INTLEVEL2;
    InterruptVector2 = INTVECTOR2;

    /* Make the init call on the device */
    Status = v6016DevCreate(
                name,
				CardAddress,
				BusRequestLevel,
				InterruptLevel1,
				InterruptVector1,
				InterruptLevel2,
				InterruptVector2,
				(BOOL *) NULL
                );

    if(Status != NO_ERR)
    {
      logMsg("v6016DevCreate FAILED\n\n",0,0,0,0,0,0);
      DisplayError(theFileDesc[0]);
      return;
    }

    for(iChannel=0; iChannel<16; iChannel++)
    {
      sprintf(DevName, "%s%d", name, iChannel);
      theFileDesc[iChannel] = open(DevName, O_RDWR, 0644);
      if(theFileDesc[iChannel] == ERROR)
      {
        logMsg("Cannot Open Device Error %s\n\n",DevName,0,0,0,0,0);
      }
    }
  }
  bInit = TRUE;
}

/*
 *------------------------------------------------------------------------
 * DeInit: perform the DeInit driver call
 *------------------------------------------------------------------------
 */

static void
DeInit(void)
{
  int iChannel;

  for(iChannel=0; iChannel<16; iChannel++)
  {
    /* Close the driver */
    if(close(theFileDesc[iChannel]) == ERROR)
    {
      logMsg("Close Error\n\n", 0, 0, 0, 0, 0, 0);
      DisplayError(theFileDesc[iChannel]);
    }
    theFileDesc[iChannel] = ERROR;
  }

  /* Remove the Driver */
  if(v6016DrvRemove() != NO_ERR)
  {
    logMsg("UnInit Error\n\n", 0, 0, 0, 0, 0, 0);
    DisplayError(theFileDesc[0]);
  }
  bInit = FALSE;
}

/*
 *------------------------------------------------------------------------
 * SelectChannel: allow user to select the serial channel of interest
 *------------------------------------------------------------------------
 */

static void 
SelectChannel(
	void
)
{

	/* Get the user's desired channel				 */
	CurrentChannel = GetInt("Enter Channel Number (0-15): ", 0, 15);
}

/*
 *------------------------------------------------------------------------
 * DumpBoardReg: display all global registers on the board
 *------------------------------------------------------------------------
 */

static void 
DumpBoardReg(CARD_ID FileDesc)
{
	GLOBAL_REGISTER         theReg;

	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
	/* Read and display the BRD_ID Register				 */
	theReg.GlobalReg = BRD_ID_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the STFLAG Register				 */
	theReg.GlobalReg = STFLAG_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the ROM_VER Register			 */
	theReg.GlobalReg = ROM_VER_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CTR0 Register				 */
	theReg.GlobalReg = CTR0_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n");
	/* Read and display the CTR1 Register				 */
	theReg.GlobalReg = CTR1_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the GO Register				 */
	theReg.GlobalReg = GO_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the TX Register				 */
	theReg.GlobalReg = TX_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the RX Register				 */
	theReg.GlobalReg = RX_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n");
	/* Read and display the BREAK Register				 */
	theReg.GlobalReg = BREAK_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CR2 Register				 */
	theReg.GlobalReg = CR2_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the SZ_AM Register				 */
	theReg.GlobalReg = SZ_AM_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the ST_PROC Register. */
	theReg.GlobalReg = ST_PROC_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n");
	/* Read and display the ER_MSK Register				 */
	theReg.GlobalReg = ER_MSK_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the ER_VEC Register				 */
	theReg.GlobalReg = ER_VEC_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the BUFBASE Register			 */
	theReg.GlobalReg = BUFBASE_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08lx ", theReg.Value.LongValue, 0, 0, 0, 0, 0);
	}
	/* Read and display the GST Register				 */
	theReg.GlobalReg = GST_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n");
	/* Read and display the MAS_GRN Register			 */
	theReg.GlobalReg = MAS_GRN_REG;
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
}

/*
 *------------------------------------------------------------------------
 * DumpChannelReg: display all channel control registers of selected ch
 *------------------------------------------------------------------------
 */

static void
DumpChannelReg(
	CARD_ID			FileDesc
)
{
	CHANNEL_REGISTER        theReg;

	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
	/* Read and display the CST Register				 */
	theReg.ChannelReg = CST_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.WordValue) & 0xFFFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CH_MSK Register				 */
	theReg.ChannelReg = CH_MSK_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CH_VEC Register				 */
	theReg.ChannelReg = CH_VEC_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the EOB Register				 */
	theReg.ChannelReg = EOB_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n", 0, 0, 0, 0, 0, 0);
	/* Read and display the XOFF Register				 */
	theReg.ChannelReg = XOFF_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the XON Register				 */
	theReg.ChannelReg = XON_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the BRK_DUR Register			 */
	theReg.ChannelReg = BRK_DUR_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the SZ_RING Register			 */
	theReg.ChannelReg = SZ_RING_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n", 0, 0, 0, 0, 0, 0);
	/* Read and display the LO_RING Register			 */
	theReg.ChannelReg = LO_RING_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR)	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the HI_RING Register			 */
	theReg.ChannelReg = HI_RING_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR)	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CH_CON1 Register			 */
	theReg.ChannelReg = CH_CON1_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR)	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	/* Read and display the CH_CON2 Register			 */
	theReg.ChannelReg = CH_CON2_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR)	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n", 0, 0, 0, 0, 0, 0);
	/* Read and display the SZ_UBUF Register			 */
	theReg.ChannelReg = SZ_UBUF_REG;
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR)	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		logMsg("%08x ", ((UINT) theReg.Value.ByteValue) & 0xFF,
			0, 0, 0, 0, 0);
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
}

/*
 *------------------------------------------------------------------------
 * DumpChannelBuffer: display channel buffer contents
 *------------------------------------------------------------------------
 */

static void 
DumpChannelBuffer(CARD_ID FileDesc)
{
	static int              BufferTypes[2] = {
		TX_BUFFER, RX_BUFFER
	};
	BUFFER_STRUCT           theBuffer;
	UINT                    i;
	UINT                    j;

	/* Ask the user which buffer					 */
	theBuffer.WhichBuffer = BufferTypes[GetInt(
	      "0 = Transmit Buffer\n1 = Receive Buffer\nEnter Buffer: ",
							  0, 1)];
	/* Ask the user the number of bytes				 */
	theBuffer.ByteCount = GetInt("Enter Number of Bytes: ", 1, 0x7FFF);
	/* Allocate the buffer space					 */
	theBuffer.pBuffer = (UCHAR *) malloc(theBuffer.ByteCount);
	if( theBuffer.pBuffer == ((UCHAR *) NULL) )	{
		logMsg("Cannot Malloc Memory.\n", 0, 0, 0, 0, 0, 0);
		return;
	}
	/* Ask the user for the buffer offset				 */
	theBuffer.Offset = (ULONG) GetHexShort(
		"Enter Buffer Offset (in hex): ");
	/* Send the Read Channel Receive Buffer Command			 */
	if( ioctl(FileDesc, READ_6016_BUFFER, (int) &theBuffer) == ERROR ) {
		logMsg("Read Buffer Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
		return;
	}
	/* Display the buffer contents to the user			 */
	for( i = 0; i < (theBuffer.ByteCount / 16) + 1; i++ )	{
		logMsg("\n", 0, 0, 0, 0, 0, 0);
		for( j = 0; j < 16; j++ )	{
			if( ((i * 16) + j) < theBuffer.ByteCount )	{
				logMsg("%02x ",
			       ((int) theBuffer.pBuffer[(i * 16) + j]) & 0xFF,
				       0, 0, 0, 0, 0);
			}
		}
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
	free(theBuffer.pBuffer);	/* Free the buffer memory	 */
}

/*
 *------------------------------------------------------------------------
 * ConfigBuffer: configure the data buffers
 *------------------------------------------------------------------------
 */

static void 
ConfigBuffer(CARD_ID FileDesc)
{
  static int RelModes[4] = {RELROR, RELRWD, RELROC, RELBCAP};
  CONFIG_BUFFER theConfigBuffer;

	/* Ask the user for the Buffer Address				 */
	theConfigBuffer.BufferAddress =
		GetHexLong("Enter Buffer Address (hex): ");
	/* Ask the user for the Bus Release Mode			 */
	theConfigBuffer.ReleaseMode = RelModes[ GetInt(
	"0 = RELROR\n1 = RELRWD\n2 = RELROC\n3 = RELBCAP\nEnter Mode: ",
								 0, 3) ];
	/* Ask the user for the Fairness Timeout Value			 */
	theConfigBuffer.FairnessTimeout =
		GetInt("Enter Fairness Timeout (0-15): ", 0, 15);
	/* Ask the user whether the Slave is an A32			 */
	theConfigBuffer.UseA32 =
		GetInt("0 = Slave is A24\n1 = Slave is A32\nEnter Slave Mode: ",
		       0, 1);
	/* Ask the user whether the slave is a supervisor only		 */
	theConfigBuffer.UseSupervisor = GetInt(
	 "0 = Slave Not Sup Only\n1 = Slave Sup Only\nEnter Sup Mode: ",
						      0, 1);
	/* Perform Buffer Configuration					 */
	if( ioctl(FileDesc, CONFIG_6016_BUFFER, (int) &theConfigBuffer) ==
	ERROR )	{
		logMsg("Configure Buffer Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * ReadWriteWrap: perform read/write wraparound test (master or slave)
 *------------------------------------------------------------------------
 */

static void
ReadWriteWrap()
{
  UINT                NumLines;
  UINT                Line;
  UINT                index;
  UINT                TransmitChannel;
  UINT                ReceiveChannel;
  UINT		       	  BytesWritten;
  UINT			      BytesRead;
  UCHAR               TransmitBuffer[BUFFER_SIZE];
  UCHAR               ReceiveBuffer[BUFFER_SIZE];
  UINT                LastValue;
  UINT                NextValue;
  BOOL                Pass = TRUE;
  CONFIG_BUFFER       theConfigBuffer;
  CONFIG_CHANNEL      theTChannel;
  CONFIG_CHANNEL      theRChannel;
  CHANNEL_REGISTER    theReg;
  REQUEST_INT_NOTIFY  theTNotify;
  REQUEST_INT_NOTIFY  theRNotify;
  SET_CHANNEL_ENABLE  theTEnable;
  SET_CHANNEL_ENABLE  theREnable;

  /* Get the user's desired channel */
  TransmitChannel = GetInt("Enter Transmit Channel Number (0-15): ",0,15);
  ReceiveChannel = GetInt("Enter Receive  Channel Number (0-15): ",0,15);

  /* Ask the user for the number of lines */
  NumLines =
    (UINT) GetInt("Enter Number of Lines (dec 1-32767): ", 1, 0x7FFF);

  /* Reset Board */
  ioctl(theFileDesc[0], RESET_6016_BOARD, 0);

  /* Configure the buffers */
  theConfigBuffer.BufferAddress = SLAVE_ADDR;
  theConfigBuffer.ReleaseMode = RELROR;
  theConfigBuffer.FairnessTimeout = 0xF;
  theConfigBuffer.UseA32 = FALSE;
  theConfigBuffer.UseSupervisor = FALSE;
  if(ioctl(theFileDesc[0], CONFIG_6016_BUFFER, (int) &theConfigBuffer) ==
     ERROR)
  {
    logMsg("Configure Buffer Failed\n\n",0,0,0,0,0,0);
    DisplayError(theFileDesc[0]);
    return;
  }

  /* Transmit side of channel */
  theTChannel.FlowControl = NO_FLOW;
  theTChannel.BaudRate = BAUDS_1200;
  theTChannel.InputTimeout = NO_TIMEOUT;
  theTChannel.ParityType = NO_PARITY;
  theTChannel.ParitySense = EVEN_PARITY;
  theTChannel.StopBits = ONE_STOP_BIT;
  theTChannel.NumBits = EIGHT_BITS;

  /* Receive side of channel */
  theRChannel.FlowControl = NO_FLOW;
  theRChannel.BaudRate = BAUDS_1200;
  theRChannel.InputTimeout = NO_TIMEOUT;
  theRChannel.ParityType = NO_PARITY;
  theRChannel.ParitySense = EVEN_PARITY;
  theRChannel.StopBits = ONE_STOP_BIT;
  theRChannel.NumBits = EIGHT_BITS;

  /* Send Channel Configuration command */
  if((ioctl(theFileDesc[TransmitChannel], CONFIG_6016_CHANNEL,
           (int) &theTChannel) == ERROR) ||
     (ioctl(theFileDesc[ReceiveChannel], CONFIG_6016_CHANNEL,
           (int) &theRChannel) == ERROR) )
  {
    logMsg("Configure Channel Failed\n\n",0,0,0,0,0,0);
    DisplayError(theFileDesc[TransmitChannel]);
    return;
  }

  /* Indicate end of buffer on carriage return */
  theReg.ChannelReg = EOB_REG;
  theReg.Value.ByteValue = 0x0A;

  /* Send the read board register command */
  if(ioctl(theFileDesc[ReceiveChannel], WRITE_6016_CHANNEL_REGISTER,
           (int) &theReg) == ERROR)
  {
    logMsg("Write EOB Channel Register Failed\n\n",0,0,0,0,0,0);
    DisplayError(theFileDesc[ReceiveChannel]);
    return;
  }

  /* Request Interrupt Notification */
  theTNotify.SemID = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  theTNotify.NotifyConditions = TX_MSK;
  theRNotify.SemID = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  theRNotify.NotifyConditions = RCV_MSK;
  if((theTNotify.SemID == ((SEM_ID) NULL)) ||
     (theRNotify.SemID == ((SEM_ID) NULL)) ||
     (ioctl(theFileDesc[TransmitChannel], REQUEST_6016_INT_NOTIFY,
          (int)&theTNotify) == ERROR) ||
     (ioctl(theFileDesc[ReceiveChannel], REQUEST_6016_INT_NOTIFY,
          (int)&theRNotify) == ERROR))
  {
    logMsg("Request Interrupt Notification Failed\n\n",0,0,0,0,0,0);
    if(theTNotify.SemID != 0) semDelete(theTNotify.SemID);
    if(theRNotify.SemID != 0) semDelete(theRNotify.SemID);
    return;
  }
  else
  {
    logMsg("will be notified: 0x%08x 0x%08x\n",
(int)&theTNotify,(int)&theRNotify,0,0,0,0);
  }

  /* Enable the channels */
  theTEnable.Enable = TRUE;
  theREnable.Enable = TRUE;
  if((ioctl(theFileDesc[TransmitChannel], SET_6016_CHANNEL_ENABLE,
          (int) &theREnable) == ERROR)||
     (ioctl(theFileDesc[ReceiveChannel], SET_6016_CHANNEL_ENABLE,
           (int)&theTEnable) == ERROR))
  {
    logMsg("Set Channel Enable Failed\n\n",0,0,0,0,0,0);
    DisplayError(theFileDesc[ReceiveChannel]);
    Pass = FALSE;
    NumLines = 0;
  }
  NextValue = 0;
  for(Line=0; Line<NumLines; Line++)
  {
    LastValue = NextValue; /* Go to the next value */

    /* Store the data patterns */
    for(index=0; index<(BUFFER_SIZE-1); index++)
    {
      TransmitBuffer[index] = (UCHAR) NextValue;
      if(NextValue >= 0xFF)
      {
        NextValue = 0;
      }
      else
      {
        NextValue++;
        if(NextValue == 0xA)
        {
          NextValue++;
        }
      }
    }

    /* Put in the end of buffer character */
    TransmitBuffer[BUFFER_SIZE - 1] = 0xA;

    /* Write out the next string */
    BytesWritten = write(theFileDesc[TransmitChannel],
                         TransmitBuffer, (int) BUFFER_SIZE);
    if(BytesWritten != BUFFER_SIZE)
    {
      logMsg("Write Data Failed. Wrote %d Bytes\n\n",BytesWritten,0,0,0,0,0);
      DisplayError(theFileDesc[TransmitChannel]);
      Pass = FALSE;
      break;
    }

    /* Wait for the data to be received */
    if(semTake(theRNotify.SemID, TEN_SECONDS) == ERROR)
    {
      logMsg("Data was not received\n\n",0,0,0,0,0,0);
      DisplayError(theFileDesc[ReceiveChannel]);
      Pass = FALSE;
      break;
    }

    /* Read the data written */
    BytesRead = read(theFileDesc[ReceiveChannel], ReceiveBuffer, BUFFER_SIZE);
    if(BytesRead != BUFFER_SIZE)
    {
      logMsg("Data cannot be read. Read %d bytes\n\n",BytesRead,0,0,0,0,0);
      DisplayError(theFileDesc[ReceiveChannel]);
      Pass = FALSE;
      break;
    }

    /* Verify the data read */
    for(index=0; index<(BUFFER_SIZE-1); index++)
    {
			if( ReceiveBuffer[index] != ((UCHAR) LastValue) ) {
				logMsg("Data read invalid at index %d\n",
				       index, 0, 0, 0, 0, 0, 0);
				logMsg("Expected = %x, Actual = %x\n\n",
					LastValue,
					(UINT) ReceiveBuffer[index], 0, 0, 0,
					0);
				Pass = FALSE;
				break;
			}
			if( LastValue >= 0xFF )	{
				LastValue = 0;
			} else	{
				LastValue++;
				if( LastValue == 0xA )	{
					LastValue++;
				}
			}
		}
		/* If this failed, get out of town			 */
		if( !Pass )	{
			break;
		}
		/* Verify the end of buffer character			 */
		if( ReceiveBuffer[BUFFER_SIZE - 1] != 0xA )	{
			logMsg("Data read invalid, expected 0xA, actual %x\n\n",
				ReceiveBuffer[BUFFER_SIZE - 1], 0, 0, 0, 0, 0);
			Pass = FALSE;
			break;
		}
		/* Wait for the data to be received			 */
		if( semTake(theTNotify.SemID, TEN_SECONDS) == ERROR )	{
			logMsg("Data was not transmitted\n\n", 0, 0, 0, 0,
				0, 0);
			DisplayError(theFileDesc[TransmitChannel]);
			Pass = FALSE;
			break;
		}
		/* STUB							 */
		logMsg("Pass %d PASSED\n", Line + 1, 0, 0, 0, 0, 0);
	}
	/* Unrequest interrupt notification				 */
	if( (ioctl(theFileDesc[TransmitChannel], REQUEST_6016_INT_NOTIFY,
	(int) &theTNotify) == ERROR) ||
	(ioctl(theFileDesc[ReceiveChannel], REQUEST_6016_INT_NOTIFY,
	(int) &theRNotify) == ERROR) )	{
		logMsg( "Cannot unrequest Interrupt Notification\n\n", 0, 0,
			0, 0, 0, 0 );
		DisplayError(theFileDesc[ReceiveChannel]);
		Pass = FALSE;
	}
	/* Disable the channels						 */
	theTEnable.Enable = FALSE;
	theREnable.Enable = FALSE;
	if( (ioctl(theFileDesc[TransmitChannel], SET_6016_CHANNEL_ENABLE,
	(int) &theREnable) == ERROR) ||
	(ioctl(theFileDesc[ReceiveChannel], SET_6016_CHANNEL_ENABLE,
	(int) &theTEnable) == ERROR) )	{
		logMsg("Set Channel Disable Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(theFileDesc[ReceiveChannel]);
		Pass = FALSE;
	}
	/* Free the semaphore						 */
	if( theTNotify.SemID != 0 )	{
		semDelete(theTNotify.SemID);
	}
	if( theRNotify.SemID != 0 )	{
		semDelete(theRNotify.SemID);
	}
	/* Report Results						 */
	if( Pass )	{
		logMsg("Read/Write Test PASSED\n\n", 0, 0, 0, 0, 0, 0);
	} else	{
		logMsg("Read/Write Test FAILED\n\n", 0, 0, 0, 0, 0, 0);
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
}

/*
 *------------------------------------------------------------------------
 * Read: read from the device
 *------------------------------------------------------------------------
 */

static void 
Read(CARD_ID FileDesc)
{
	UINT                    i;
	UINT                    j;
	UINT			BytesRead;
	UINT			BytesToRead;
	UCHAR			*puchBuffer;

	/* Ask the user the number of bytes to be read			 */
	BytesToRead = GetInt("Enter Number of Bytes: ", 1, 0x7FFF);
	/* Allocate memory for the buffer				 */
	puchBuffer = malloc(BytesToRead);
	if( puchBuffer == ((UCHAR *) NULL) )	{
		logMsg("Cannot Malloc Memory.\n", 0, 0, 0, 0, 0, 0);
		return;
	}
	/* Perform the read operation					 */
	BytesRead = read(FileDesc, puchBuffer, BytesToRead);
	if( BytesRead == ERROR ) {
		free(puchBuffer);
		logMsg("Read Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
		return;
	}
	/* Make sure we got all the bytes				 */
	if( BytesRead != BytesToRead )	{
		logMsg( "Only Read %d Bytes\n", BytesRead, 0, 0,
			0, 0, 0);
	}
	/* Loop through and display all the bytes			 */
	for( i = 0; i < (BytesRead / 16) + 1; i++ )	{
		logMsg("\n", 0, 0, 0, 0, 0, 0);
		for( j = 0; j < 16; j++ )	{
			if( ((i * 16) + j) < BytesRead )	{
				logMsg("%02x ",
		       ((int) puchBuffer[(i * 16) + j]) & 0xFF,
				       0, 0, 0, 0, 0);
			}
		}
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
	free(puchBuffer);	/* Free the allocated buffer	 */
}

/*
 *------------------------------------------------------------------------
 * Write: write to the device
 *------------------------------------------------------------------------
 */

static void 
Write(CARD_ID FileDesc)
{
	UINT                    i;
	UINT			BytesToWrite;
	UINT			BytesWritten;
	UCHAR                   Patt;
	BOOL                    IncPatt;
	char                    Prompt[20];
	UCHAR			*puchBuffer;

	/* Ask the user the number of bytes to be written		 */
	BytesToWrite = GetInt("Enter Number of Bytes: ", 1, 0x7FFF);
	/* Allocate memory for the transmit buffer			 */
	puchBuffer = malloc(BytesToWrite);
	if( puchBuffer == ((UCHAR *) NULL) )	{
		logMsg("Cannot Malloc Memory.\n", 0, 0, 0, 0, 0, 0);
		return;
	}
	/* Ask the user what type of pattern is to be written		 */
	logMsg("0 = Specific Data\n", 0, 0, 0, 0, 0, 0);
	logMsg("1 = Incrementing Pattern\n", 0, 0, 0, 0, 0, 0);
	IncPatt = (BOOL) GetInt("Enter Pattern Type: ", 0, 1);
	if( IncPatt )	{
		Patt = 0;
		for( i = 0; i < BytesToWrite; i++ )	{
			puchBuffer[i] = Patt;
			if( Patt >= 0xFF )	{
				Patt = 0;
			} else	{
				Patt++;
			}
		}
	} else	{
		/* Get the Bytes from the user				 */
		for( i = 0; i < BytesToWrite; i++ )	{
			sprintf(Prompt, "Byte %08d: ", i);
			puchBuffer[i] = (UCHAR) GetHexShort(Prompt);
		}
	}
	/* Write the data to the serial card				 */
	BytesWritten = write(FileDesc, puchBuffer, BytesToWrite);
	if( BytesWritten == ERROR ) {
		logMsg("Write Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	} else	{
		/* Make sure all the bytes were written			 */
		if( BytesWritten != BytesToWrite ) {
			logMsg("Only Wrote %d Bytes\n",
			       BytesWritten, 0, 0, 0, 0, 0);
		}
	}
	free(puchBuffer);	/* Free the allocated buffer	 */
}

/*
 *------------------------------------------------------------------------
 * SendBreak: transmit a BREAK sequence on selected channel
 *------------------------------------------------------------------------
 */

static void 
SendBreak(CARD_ID FileDesc)
{

	/* Send the Transmit Break Command on this channel		 */
	if( ioctl(FileDesc, SEND_6016_BREAK, (int) CurrentChannel) == ERROR ) {
		logMsg("Send Break Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * RunSelftest: cause board to perform a selftest and report results
 *------------------------------------------------------------------------
 */

static void 
RunSelftest(CARD_ID FileDesc)
{
	SELFTESTS               Selftests;

	/*
	 * Ask the user for which selftests to run. Make sure we don't ask
	 * the card to reload its firmware.
	 */
	Selftests = ((int) GetHexShort("Enter Selftests (int hex): ")) & 0x3F;
	/* Command the card to perform its selftest			 */
	if( ioctl(FileDesc, RUN_6016_SELFTEST, (int) Selftests) == ERROR ) {
		logMsg("Run Selftest Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * ResetBoard: perform a board reset
 *------------------------------------------------------------------------
 */

static void 
ResetBoard(CARD_ID FileDesc)
{

	/* Perform the Reset Board Command				 */
	if( ioctl(FileDesc, RESET_6016_BOARD, 0) == ERROR )	{
		logMsg("Reset Board Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * ConfigController: change board's VMEbus system controller settings
 *------------------------------------------------------------------------
 */

static void
ConfigController(CARD_ID FileDesc)
{
	static ARB_MODE         ArbMode[2] =	{
		ROUND_ROBIN, PRIORITY
	};
	CONFIG_CONTROLLER       theConfig;

	/* Ask the user the arbitration mode				 */
	theConfig.ArbitrationMode = ArbMode[GetInt(
	      "0 = ROUND_ROBIN\n1 = PRIORITY\nEnter Arbitration Mode: ", 0, 1)];
	/* Ask the user the Bus Error Timeout				 */
	theConfig.BusErrorTimeout = GetInt("Enter Bus Error Timeout (0-7): ",
		0, 7);
	/* Write the Controller Configuration Command			 */
	if( ioctl(FileDesc, CONFIG_6016_CONTROLLER, (int) &theConfig) ==
	ERROR )	{
		logMsg("Controller Configuration Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * SetChannelEnable: enable or disable a serial channel
 *------------------------------------------------------------------------
 */

static void 
SetChannelEnable(CARD_ID FileDesc)
{
	SET_CHANNEL_ENABLE      theEnable;

	/* Ask the user whether to send enable or disable command	 */
	theEnable.Enable = GetInt(
		"0 = CHANNEL_DISABLE\n1 = CHANNEL_ENABLE\nEnter Enable Mode: ",
		0, 1);
	if( ioctl(FileDesc, SET_6016_CHANNEL_ENABLE, (int) &theEnable) ==
	ERROR )	{
		logMsg("Channel Enable Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * ConfigErrors: configure 6016 device error conditions
 *------------------------------------------------------------------------
 */

static void 
ConfigErrors(CARD_ID FileDesc)
{
	UINT			Errors; /* Errors to interrupt on	 */

	/* Ask user which errors to set up				 */
	Errors = (UINT) GetHexShort("Enter Error Value (in hex): ");
	/* Send the Error Configuration Command				 */
	if( ioctl(FileDesc, CONFIG_6016_ERRORS, (int) Errors) == ERROR ) {
		logMsg("Error Configuration Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * GetBoardReg: prompt user for a board register
 *------------------------------------------------------------------------
 */

static GLOBAL_REG 
GetBoardReg(void)
{
	/* The board register addresses */
	static ULONG            BrdAddresses[17] =	{
		BRD_ID_REG,	STFLAG_REG,	ROM_VER_REG,	CTR0_REG,
		CTR1_REG,	GO_REG,		TX_REG,		RX_REG,
		BREAK_REG,	CR2_REG,	SZ_AM_REG,	ST_PROC_REG,
		ER_MSK_REG,	ER_VEC_REG,	BUFBASE_REG,	GST_REG,
		MAS_GRN_REG
	};

	/* Ask the user for the register's address */
	return( BrdAddresses[ GetInt(
				" 0 = BRD_ID\n"
				" 1 = STFLAG\n"
				" 2 = ROM_VER\n"
				" 3 = CTR0\n"
				" 4 = CTR1\n"
				" 5 = GO\n"
				" 6 = TX\n"
				" 7 = RX\n"
				" 8 = BREAK\n"
				" 9 = CR2\n"
				"10 = SZ_AM\n"
				"11 = ST_PROC\n"
				"12 = ER_MSK\n"
				"13 = ER_VEC\n"
				"14 = BUFBASE\n"
				"15 = GST\n"
				"16 = MAS_GRN\n"
				"Enter Register: ",
				0, 16 )
		]
	);
}

/*
 *------------------------------------------------------------------------
 * GetChannelReg: prompt user for a channel register (by name)
 *------------------------------------------------------------------------
 */

static CHANNEL_REG 
GetChannelReg(void)
{
	/* The Channel Register addresses */
	static ULONG            ChnlAddresses[17] =	{
		CST_REG,	CH_MSK_REG,	CH_VEC_REG,	EOB_REG,
		XOFF_REG,	XON_REG,	BRK_DUR_REG,	SZ_RING_REG,
		LO_RING_REG,	HI_RING_REG,	CH_CON1_REG,	CH_CON2_REG,
		SZ_UBUF_REG
	};


	/* Ask the user for the register's address			 */
	return( ChnlAddresses[ GetInt(
				" 0 = CST\n"
				" 1 = CH_MSK\n"
				" 2 = CH_VEC\n"
				" 3 = EOB\n"
				" 4 = XOFF\n"
				" 5 = XON\n"
				" 6 = BRK_DUR\n"
				" 7 = SZ_RING\n"
				" 8 = LO_RING\n"
				" 9 = HI_RING\n"
				"10 = CH_CON1\n"
				"11 = CH_CON2\n"
				"12 = SZ_UBUF\n"
				"Enter Register: ", 0, 12 )
		]
	);
}

/*
 *------------------------------------------------------------------------
 * ReadBoardReg: read a board register
 *------------------------------------------------------------------------
 */

static void 
ReadBoardReg(CARD_ID FileDesc)
{
	GLOBAL_REGISTER         theReg;

	/* Ask the user for the register's address			 */
	theReg.GlobalReg = GetBoardReg();
	/* Send the read board register command				 */
	if( ioctl(FileDesc, READ_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
	PrintBoardReg(theReg.GlobalReg, &theReg.Value);
}

/*
 *------------------------------------------------------------------------
 * PrintBoardReg: display a board register value
 *------------------------------------------------------------------------
 */

static void 
PrintBoardReg(GLOBAL_REG Reg, REGISTER_VALUE_TYPE *value)
{
	switch( Reg )	{
	default:
		logMsg("Invalid Register.\n", 0, 0, 0, 0, 0, 0);
		break;
	case BRD_ID_REG:
		/* FALLTHROUGH */
	case STFLAG_REG:
		/* FALLTHROUGH */
	case CTR0_REG:
		/* FALLTHROUGH */
	case CTR1_REG:
		/* FALLTHROUGH */
	case CR2_REG:
		/* FALLTHROUGH */
	case SZ_AM_REG:
		/* FALLTHROUGH */
	case ST_PROC_REG:
		/* FALLTHROUGH */
	case ER_MSK_REG:
		/* FALLTHROUGH */
	case ER_VEC_REG:
		/* FALLTHROUGH */
	case MAS_GRN_REG:
		logMsg("Register Value = %02x\n", (UINT) value->ByteValue,
		       0, 0, 0, 0, 0);
		break;
	case ROM_VER_REG:
		/* FALLTHROUGH */
	case GO_REG:
		/* FALLTHROUGH */
	case TX_REG:
		/* FALLTHROUGH */
	case RX_REG:
		/* FALLTHROUGH */
	case BREAK_REG:
		/* FALLTHROUGH */
	case GST_REG:
		logMsg("Register Value = %04x\n", (UINT) value->WordValue,
		       0, 0, 0, 0, 0);
		break;
	case BUFBASE_REG:
		logMsg( "Register Value = %08lx\n", value->LongValue, 0, 0,
			0, 0, 0);
		break;
	}
}

/*
 *------------------------------------------------------------------------
 * PrintChannelReg: display a channel register value
 *------------------------------------------------------------------------
 */

static void 
PrintChannelReg(CHANNEL_REG Reg, REGISTER_VALUE_TYPE *value)
{
	switch( Reg )	{
	default:
		logMsg("Invalid Register.\n", 0, 0, 0, 0, 0, 0);
		break;
	case CH_MSK_REG:
		/*FALLTHROUGH*/
	case CH_VEC_REG:
		/*FALLTHROUGH*/
	case EOB_REG:
		/*FALLTHROUGH*/
	case XOFF_REG:
		/*FALLTHROUGH*/
	case XON_REG:
		/*FALLTHROUGH*/
	case BRK_DUR_REG:
		/*FALLTHROUGH*/
	case SZ_RING_REG:
		/*FALLTHROUGH*/
	case LO_RING_REG:
		/*FALLTHROUGH*/
	case HI_RING_REG:
		/*FALLTHROUGH*/
	case CH_CON1_REG:
		/*FALLTHROUGH*/
	case CH_CON2_REG:
		/*FALLTHROUGH*/
	case SZ_UBUF_REG:
		logMsg("Register Value = %02x\n", (UINT) value->ByteValue,
		       0, 0, 0, 0, 0);
		break;
	case CST_REG:
		logMsg("Register Value = %04x\n", (UINT) value->WordValue,
		       0, 0, 0, 0, 0);
		break;
	}
}

/*
 *------------------------------------------------------------------------
 * WriteBoardReg: write one board register
 *------------------------------------------------------------------------
 */

static void 
WriteBoardReg(
	CARD_ID			FileDesc
)
{
	GLOBAL_REGISTER         theReg;

	/* Ask the user for the register's address			 */
	theReg.GlobalReg = GetBoardReg();
	/* Ask the user for the register value to write			 */
	StoreBoardReg(theReg.GlobalReg, &(theReg.Value));
	/* Send the read board register command				 */
	if( ioctl(FileDesc, WRITE_6016_GLOBAL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Write Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * StoreBoardReg: prompt user for register value and then store it
 *------------------------------------------------------------------------
 */

static void
StoreBoardReg(
	GLOBAL_REG		Reg,
	REGISTER_VALUE_TYPE	*value
)
{

	switch( Reg )	{
	default:
		logMsg("Invalid Register.\n", 0, 0, 0, 0, 0, 0);
		break;
	case CTR0_REG:
		/*FALLTHROUGH*/
	case CTR1_REG:
		/*FALLTHROUGH*/
	case CR2_REG:
		/*FALLTHROUGH*/
	case SZ_AM_REG:
		/*FALLTHROUGH*/
	case ST_PROC_REG:
		/*FALLTHROUGH*/
	case ER_MSK_REG:
		/*FALLTHROUGH*/
	case ER_VEC_REG:
		/*FALLTHROUGH*/
	case MAS_GRN_REG:
		value->ByteValue = (UCHAR) GetHexLong(
				     "Enter Register Value (in hex): ");
		break;
	case GO_REG:
		/*FALLTHROUGH*/
	case TX_REG:
		/*FALLTHROUGH*/
	case RX_REG:
		/*FALLTHROUGH*/
	case BREAK_REG:
		/*FALLTHROUGH*/
	case GST_REG:
		value->WordValue = (USHORT) GetHexLong(
				     "Enter Register Value (in hex): ");
		break;
	case BUFBASE_REG:
		value->LongValue = GetHexLong(
			"Enter Register Value (in hex): " );
		break;
	}
}

/*
 *------------------------------------------------------------------------
 * StoreChannelReg: prompt user for a register value and store it
 *------------------------------------------------------------------------
 */

static void 
StoreChannelReg(
	CHANNEL_REG		Reg,
	REGISTER_VALUE_TYPE	*value
)
{

	switch( Reg )	{
	default:
		logMsg("Invalid Register.\n", 0, 0, 0, 0, 0, 0);
		break;
	case CH_MSK_REG:
		/* FALLTHROUGH*/
	case CH_VEC_REG:
		/* FALLTHROUGH*/
	case EOB_REG:
		/* FALLTHROUGH*/
	case XOFF_REG:
		/* FALLTHROUGH*/
	case XON_REG:
		/* FALLTHROUGH*/
	case BRK_DUR_REG:
		/* FALLTHROUGH*/
	case SZ_RING_REG:
		/* FALLTHROUGH*/
	case LO_RING_REG:
		/* FALLTHROUGH*/
	case HI_RING_REG:
		/* FALLTHROUGH*/
	case CH_CON1_REG:
		/* FALLTHROUGH*/
	case CH_CON2_REG:
		/* FALLTHROUGH*/
	case SZ_UBUF_REG:
		value->ByteValue = (UCHAR) GetHexLong(
				     "Enter Register Value (in hex): ");
		break;
	case CST_REG:
		value->WordValue = (USHORT) GetHexLong(
				     "Enter Register Value (in hex): ");
		break;
	}
}

/*
 *------------------------------------------------------------------------
 * RequestIntNotify: request interrupt notification
 *------------------------------------------------------------------------
 */

static void 
RequestIntNotify(
	CARD_ID			FileDesc
)
{
	REQUEST_INT_NOTIFY      theNotify;

	/* Get the semaphore						 */
	SemID = semBCreate(SEM_Q_FIFO, SEM_FULL);
	theNotify.SemID = SemID;
	/* Ask the user the notify conditions				 */
	theNotify.NotifyConditions =
		GetHexLong("Enter Notify Conditions (in hex): ");
	/* Request notification on the user selected conditions		 */
	if( ioctl(FileDesc, REQUEST_6016_INT_NOTIFY, (int) &theNotify) ==
	ERROR )	{
		logMsg("Request Interrupt Notification Failed\n\n", 0, 0, 0,
			0, 0, 0 );
		DisplayError(FileDesc);
	}
	/* If we are "un-notifying", delete the semaphore		 */
	if( theNotify.NotifyConditions == 0 )	{
		semDelete(SemID);
		SemID = 0;
	}
}

/*
 *------------------------------------------------------------------------
 * WaitForIntNotify: await interrupt event notification
 *------------------------------------------------------------------------
 */

static void 
WaitForIntNotify(
	void
)
{

	if( SemID == 0 )	{
		logMsg("No Request for Notification has been performed\n\n",
		       0, 0, 0, 0, 0, 0);
	} else	{
		if( semTake(SemID, TEN_SECONDS) )	{
			logMsg("Notification Occurred\n\n", 0, 0, 0, 0, 0, 0);
		} else	{
			logMsg(
			"Notification Never Occurred within 10 Seconds\n\n",
			       0, 0, 0, 0, 0, 0);
		}
	}
}

/*
 *------------------------------------------------------------------------
 * ConfigChannel: configure a channel
 *------------------------------------------------------------------------
 */

static void 
ConfigChannel(
	CARD_ID			FileDesc
)
{
	static UINT             Flow[4] =	{
		NO_FLOW, XON_XOFF_FLOW, ANY_XOFF_FLOW, RTS_CTS_FLOW
	};
	static UINT             Baud[18] =	{
		BAUDS_50,	BAUDS_110,	BAUDS_134p5,	BAUDS_200,
		BAUDS_300,	BAUDS_600,	BAUDS_1200,	BAUDS_1050,
		BAUDS_2400,	BAUDS_4800,	BAUDS_7200,	BAUDS_9600,
		BAUDS_38400,	BAUDS_75,	BAUDS_150,	BAUDS_2000,
		BAUDS_1800,	BAUDS_19200
	};
	static UINT             Timeout[4] =	{
		NO_TIMEOUT,	THREE_CHAR_TIMEOUT,	HALF_SEC_TIMEOUT,
		ONE_SEC_TIMEOUT
	};
	static UINT             Stop[2] =	{
		ONE_STOP_BIT,	TWO_STOP_BITS
	};
	static UINT             Type[3] =	{
		ODD_OR_EVEN_PARITY,	FORCE_PARITY,	NO_PARITY
	};
	static UINT             Parity[2] =	{
		EVEN_PARITY,	ODD_PARITY
	};
	static UINT             Bits[4] =	{
		FIVE_BITS,	SIX_BITS,	SEVEN_BITS,	EIGHT_BITS
	};
	CONFIG_CHANNEL          theChannel;

	/* Ask the user the flow control. */
	theChannel.FlowControl = Flow[ GetInt(
					"0 = NO_FLOW\n"
					"1 = XON_XOFF_FLOW\n"
					"2 = ANY_XOFF_FLOW\n"
					"3 = RTS_CTS_FLOW\n"
					"Enter Flow Control: ",
					0, 3
					)
				];
	/* Ask the user for the baud rate				 */
	theChannel.BaudRate = Baud[ GetInt(
						" 0 = BAUDS_50\n"
						" 1 = BAUDS_110\n"
						" 2 = BAUDS_134p5\n"
						" 3 = BAUDS_200\n"
						" 4 = BAUDS_300\n"
						" 5 = BAUDS_600\n"
						" 6 = BAUDS_1200\n"
						" 7 = BAUDS_1050\n"
						" 8 = BAUDS_2400\n"
						" 9 = BAUDS_4800\n"
						"10 = BAUDS_7200\n"
						"11 = BAUDS_9600\n"
						"12 = BAUDS_38400\n"
						"13 = BAUDS_75\n"
						"14 = BAUDS_150\n"
						"15 = BAUDS_2000\n"
						"16 = BAUDS_1800\n"
						"17 = BAUDS_19200\n"
						"Enter Baud Rate: ",
						0,
						17
					)
			];
	/* Ask the user for the input timeout value			 */
	theChannel.InputTimeout = Timeout[ GetInt(
						"0 = NO_TIMEOUT\n"
						"1 = THREE_CHAR_TIMEOUT\n"
						"2 = HALF_SEC_TIMEOUT\n"
						"3 = ONE_SEC_TIMEOUT\n"
						"Enter Timeout: ",
						0,
						3
					)
				];
	/* Ask the user for the number of stop bits			 */
	theChannel.StopBits = Stop[ GetInt(
						"0 = ONE_STOP_BIT\n"
						"1 = TWO_STOP_BITS\n"
						"Enter Stop Bits: ",
						0,
						1
					)
				];
	/* Ask the user for the parity type				 */
	theChannel.ParityType = Type[ GetInt(
						"0 = ODD_OR_EVEN_PARITY\n"
						"1 = FORCE_PARITY\n"
						"2 = NO_PARITY\n"
						"Enter Parity Type: ",
						0,
						2
					)
				];
	/* Ask the user for the parity sense				 */
	theChannel.ParitySense = Parity[ GetInt(
						"0 = EVEN_PARITY\n"
						"1 = ODD_PARITY"
						"\nEnter Parity Sense: ",
						0,
						1
					)
			      ];
	/* Ask the user for the number of bits				 */
	theChannel.NumBits = Bits[ GetInt(
						"0 = FIVE_BITS\n"
						"1 = SIX_BITS\n"
						"2 = SEVEN_BITS\n"
						"3 = EIGHT_BITS\n"
						"Enter Number of Bits: ",
						0,
						3
					)
				];
	/* Send Channel Configuration command				 */
	if( ioctl(FileDesc, CONFIG_6016_CHANNEL, (int) &theChannel) ==
	ERROR )	{
		logMsg("Channel Configuration Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * GetDeviceError: show last 6016 device error
 *------------------------------------------------------------------------
 */

static void 
GetDeviceError(
	CARD_ID			FileDesc
)
{

	DisplayError(FileDesc);
}

/*
 *------------------------------------------------------------------------
 * DisplayError: obtain 6016 error from driver, decode and display it
 *------------------------------------------------------------------------
 */

static void
DisplayError(
	CARD_ID			FileDesc
)
{
	V6016_STATUS            Status;

	/* Send the Get Device Error Code Command for this channel	 */
	if( ioctl(FileDesc, GET_6016_DEVICE_ERROR, (int) &Status) == ERROR ) {
		logMsg("Get Device Error Code Failed\n\n", 0, 0, 0, 0, 0, 0);
	} else	{
		/* Display the error code				 */
		switch( Status )	{
		default:
			logMsg("\nInvalid Error Code\n", 0, 0, 0, 0, 0, 0);
			break;
		case NO_ERR:
			logMsg("\nNo Errors\n\n", 0, 0, 0, 0, 0, 0);
			break;
		case INVALID_BOARD_ID_ERR:
			logMsg("\nInvalid Board ID Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case DEVICE_BUSY_ERR:
			logMsg("\nDevice Busy Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case CARD_SELFTEST_ERR:
			logMsg("\nCard Selftest Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case INVALID_PARAMETER_ERR:
			logMsg("\nInvalid Parameter Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case RESOURCE_ERR:
			logMsg("\nResource Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case BOARD_ACCESS_ERR:
			logMsg("\nBoard Access Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case MEMORY_ALLOCATION_ERROR:
			logMsg("\n Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case DEVICE_ADD_ERROR:
			logMsg("\n Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case ALREADY_OPEN_ERROR:
			logMsg("\n Error\n", 0, 0, 0, 0, 0, 0);
			break;
		case NOT_OPEN_ERROR:
			logMsg("\n Error\n", 0, 0, 0, 0, 0, 0);
			break;
		}
	}
}

/*
 *------------------------------------------------------------------------
 * ReadChannelReg: obtain value of a channel register
 *------------------------------------------------------------------------
 */

static void
ReadChannelReg(
	CARD_ID			FileDesc
)
{
	CHANNEL_REGISTER        theReg;

	/* Ask the user for the register's address			 */
	theReg.ChannelReg = GetChannelReg();
	/* Send the read board register command				 */
	if( ioctl(FileDesc, READ_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Read Channel Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
	PrintChannelReg(theReg.ChannelReg, &theReg.Value);
}

/*
 *------------------------------------------------------------------------
 * WriteChannelReg: write a channel register
 *------------------------------------------------------------------------
 */

static void 
WriteChannelReg(
	CARD_ID			FileDesc
)
{
	CHANNEL_REGISTER        theReg;

	/* Ask the user for the register's address			 */
	theReg.ChannelReg = GetChannelReg();
	/* Ask the user for the register value to write			 */
	StoreChannelReg(theReg.ChannelReg, &theReg.Value);
	/* Send the read board register command				 */
	if( ioctl(FileDesc, WRITE_6016_CHANNEL_REGISTER, (int) &theReg) ==
	ERROR )	{
		logMsg("Write Channel Register Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
}

/*
 *------------------------------------------------------------------------
 * RdChannelBuffer: read channel receive buffer
 *------------------------------------------------------------------------
 */

static void 
RdChannelBuffer(
	CARD_ID			FileDesc,
	BUFFER_TYPE		WhichBuffer
)
{
	BUFFER_STRUCT           theBuffer;
	UINT                    i;
	UINT                    j;

	/* Ask the user for the number of bytes to read			 */
	theBuffer.ByteCount = GetInt("Enter Number of Bytes: ", 1, 0x7FFF);
	/* Allocate buffer for the data					 */
	theBuffer.pBuffer = malloc(theBuffer.ByteCount);
	if( theBuffer.pBuffer == ((UCHAR *) NULL) )	{
		logMsg("Cannot Malloc Memory.\n", 0, 0, 0, 0, 0, 0);
		return;
	}
	/* Ask the user for the buffer offset				 */
	theBuffer.Offset = (ULONG) GetHexShort(
		"Enter Buffer Offset (in hex): " );
	/* Store the buffer						 */
	theBuffer.WhichBuffer = WhichBuffer;
	/* Send the Read Channel Receive Buffer Command			 */
	if( ioctl(FileDesc, READ_6016_BUFFER, (int) &theBuffer) ==
	ERROR )	{
		logMsg("Read Channel RX Buffer Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
		free(theBuffer.pBuffer);
		return;
	}
	/* Display the data read to the user				 */
	for( i = 0; i < (theBuffer.ByteCount / 16) + 1; i++ )	{
		logMsg("\n", 0, 0, 0, 0, 0, 0);
		for( j = 0; j < 16; j++ )	{
			if( ((i * 16) + j) < theBuffer.ByteCount )	{
				logMsg( "%02x ",
					theBuffer.pBuffer[(i * 16) + j] & 0xFF,
					0, 0, 0, 0, 0 );
			}
		}
	}
	logMsg("\n\n", 0, 0, 0, 0, 0, 0);
	free(theBuffer.pBuffer);	/* Free the allocated buffer	 */
}

/*
 *------------------------------------------------------------------------
 * WrChannelBuffer: write the channel receive buffer
 *------------------------------------------------------------------------
 */

static void 
WrChannelBuffer(
	CARD_ID			FileDesc,
	BUFFER_TYPE		WhichBuffer
)
{
	BUFFER_STRUCT           theBuffer;
	UINT                    i;
	char                    Prompt[20];

	/* Ask the user for the number of bytes to write to the buffer	 */
	theBuffer.ByteCount = GetInt("Enter Number of Bytes: ", 1, 0x7FFF);
	/* Ask the user for the buffer offset				 */
	theBuffer.Offset = (ULONG) GetHexShort(
		"Enter Buffer Offset (in hex): " );
	/* Allocate the memory for the buffer				 */
	theBuffer.pBuffer = malloc(theBuffer.ByteCount);
	if( theBuffer.pBuffer == ((UCHAR *) NULL) )	{
		logMsg("Cannot Malloc Memory.\n", 0, 0, 0, 0, 0, 0);
		return;
	}
	/* Prompt the user for the data to write to the buffer		 */
	for( i = 0; i < theBuffer.ByteCount; i++ )	{
		sprintf(Prompt, "Byte %08d: ", i);
		theBuffer.pBuffer[i] = (UCHAR) GetHexShort(Prompt);
	}
	/* Store which buffer						 */
	theBuffer.WhichBuffer = WhichBuffer;
	/* Perform the Write Channel Receive Buffer Command		 */
	if( ioctl(FileDesc, WRITE_6016_BUFFER, (int) &theBuffer) ==
	ERROR )	{
		logMsg("Write Failed\n\n", 0, 0, 0, 0, 0, 0);
		DisplayError(FileDesc);
	}
	free(theBuffer.pBuffer);	/* Free allocated buffer memory	 */
}

/*
 *------------------------------------------------------------------------
 * GetInt: prompt user for an integer value (with limit checking)
 *------------------------------------------------------------------------
 */

static int 
GetInt(
	char			*Prompt,
	int			Min,
	int			Max
)
{
	int                     Value;

	/*
	 * Loop through asking the user for an integer until the user
	 * enters a value within the given range.
	 */
	do	{
		/* Prompt for an integer				 */
		logMsg(Prompt, 0, 0, 0, 0, 0, 0);
		/* Read the integer					 */
		scanf("%d", &Value);
		logMsg("\n", 0, 0, 0, 0, 0, 0);
		getchar();
	} while( (Value < Min) || (Value > Max) );
	return( Value );		/* Return the value entered	 */
}

/*
 *------------------------------------------------------------------------
 * GetHexLong: prompt user for a long hex value
 *------------------------------------------------------------------------
 */

static ULONG 
GetHexLong(char *Prompt)
{
  ULONG  Value;

  /* Prompt user for a hex long value */
  logMsg(Prompt,0,0,0,0,0,0);
  /* Read the value */
  scanf("%lx",&Value);
  logMsg("\n",0,0,0,0,0,0);
  getchar();
  return(Value); /* Return the value */
}

/*
 *------------------------------------------------------------------------
 * GetHexShort: prompt user fo a short hex value and return it
 *------------------------------------------------------------------------
 */

static USHORT 
GetHexShort(char *Prompt)
{
  USHORT  Value;

  /* Prompt the user for a hex short value			 */
  logMsg(Prompt, 0, 0, 0, 0, 0, 0);
  /* Read the value from the user					 */
  scanf("%hx", &Value);
  logMsg("\n", 0, 0, 0, 0, 0, 0);
  getchar();
  return( Value );		/* Return the value		 */
}

/*
 *------------------------------------------------------------------------
 * GetMenuItem: display main menu and return validated selection
 *------------------------------------------------------------------------
 */

static int 
GetMenuItem(
	void
)
{
	int                     Selection;
	BOOL                    First = TRUE;

	/* Loop through until a valid menu option is entered		 */
	do	{
		logMsg("\n\n", 0, 0, 0, 0, 0, 0);
		/*
		 * If this is not the first time through, the user must
		 * have entered an invalid value. Tell him about it.
		 */
		if( !First )	{
			logMsg("Invalid Selection!\n\n", 0, 0, 0, 0, 0, 0);
		}
		First = FALSE;
		/* Display the Menu					 */
		logMsg("*** CHANNEL %d ***\n", CurrentChannel, 0, 0, 0, 0, 0);
		logMsg("A = INIT\n", 0, 0, 0, 0, 0, 0);
		logMsg("B = DEINIT\n", 0, 0, 0, 0, 0, 0);
		logMsg("C = READ\n", 0, 0, 0, 0, 0, 0);
		logMsg("D = WRITE\n", 0, 0, 0, 0, 0, 0);
		logMsg("E = IOCTL\n", 0, 0, 0, 0, 0, 0);
		logMsg("F = DUMP_BOARD_REG\n", 0, 0, 0, 0, 0, 0);
		logMsg("G = DUMP_CHANNEL_REG\n", 0, 0, 0, 0, 0, 0);
		logMsg("H = DUMP_CHANNEL_BUFFER\n", 0, 0, 0, 0, 0, 0);
		logMsg("I = READ_WRITE_WRAP\n", 0, 0, 0, 0, 0, 0);
		logMsg("J = SELECT_CHANNEL\n", 0, 0, 0, 0, 0, 0);
		logMsg("K = EXIT\n\n", 0, 0, 0, 0, 0, 0);
		/* Get the user's selection				 */
		logMsg("Enter Selection: ", 0, 0, 0, 0, 0, 0);
		Selection = getchar();
		getchar();
		logMsg("\n", 0, 0, 0, 0, 0, 0);
		/* We will accept either upper or lower case		 */
		Selection = toupper(Selection) - 'A';
		/*
		 * If this is the config command, prompt the user for
		 * which function by displaying the submenu.
		 */
		if( Selection == CONFIG )	{
			/* Display the submenu				 */
			logMsg("\n\n", 0, 0, 0, 0, 0, 0);
			logMsg("*** CHANNEL %d ***\n", CurrentChannel,
				0, 0, 0, 0, 0);
			logMsg("A = CONFIG_BUFFER\n", 0, 0, 0, 0, 0, 0);
			logMsg("B = SEND_BREAK\n", 0, 0, 0, 0, 0, 0);
			logMsg("C = RUN_SELFTEST\n", 0, 0, 0, 0, 0, 0);
			logMsg("D = RESET_BOARD\n", 0, 0, 0, 0, 0, 0);
			logMsg("E = CONFIG_CONTROLLER\n", 0, 0, 0, 0, 0, 0);
			logMsg("F = SET_CHANNEL_ENABLE\n", 0, 0, 0, 0, 0, 0);
			logMsg("G = CONFIG_ERRORS\n", 0, 0, 0, 0, 0, 0);
			logMsg("H = READ_BOARD_REG\n", 0, 0, 0, 0, 0, 0);
			logMsg("I = WRITE_BOARD_REG\n", 0, 0, 0, 0, 0, 0);
			logMsg("J = REQUEST_INT_NOTIFY\n", 0, 0, 0, 0, 0, 0);
			logMsg("K = WAIT_FOR_NOTIFY\n", 0, 0, 0, 0, 0, 0);
			logMsg("L = CONFIG_CHANNEL\n", 0, 0, 0, 0, 0, 0);
			logMsg("M = GET_DEVICE_ERROR\n", 0, 0, 0, 0, 0, 0);
			logMsg("N = READ_CHANNEL_REG\n", 0, 0, 0, 0, 0, 0);
			logMsg("O = WRITE_CHANNEL_REG\n", 0, 0, 0, 0, 0, 0);
			logMsg("P = READ_CHANNEL_RX_BUFFER\n", 0, 0, 0, 0,
				0, 0);
			logMsg("Q = READ_CHANNEL_TX_BUFFER\n", 0, 0, 0, 0,
				0, 0);
			logMsg("R = WRITE_CHANNEL_RX_BUFFER\n", 0, 0, 0, 0,
				0, 0);
			logMsg("S = WRITE_CHANNEL_TX_BUFFER\n\n", 0, 0, 0,
				0, 0, 0);
			/* Get the user's selection			 */
			logMsg("Enter Selection: ", 0, 0, 0, 0, 0, 0);
			Selection = getchar();
			getchar();
			logMsg("\n", 0, 0, 0, 0, 0, 0);
			/* We will accept upper or lower case letters	 */
			Selection = toupper(Selection) - 'A' + CFG_BUFFER;
		}
	} while( (Selection < INIT) || (Selection > WR_CHANNEL_TX_BUFFER) );
	return( Selection );		/* Return only valid selection	 */
}

/*
 *------------------------------------------------------------------------
 * GetString: prompt user for a string and return it
 *------------------------------------------------------------------------
 */

static void 
GetString(
	char			*Prompt,
	char			*theString,
	unsigned int		size
)
{
	char                    Format[80];

	logMsg(Prompt, 0, 0, 0, 0, 0, 0);
	sprintf(Format, "%%%ds", size - 1);
	scanf(Format, theString);
	getchar();
}

/*
 *------------------------------------------------------------------------
 * menu: main 6016 driver access menu routine
 *------------------------------------------------------------------------
 * This routine calls other routines to display the menu and get the
 * user's selection and then calls the appropriate action routine to
 * perform the function.
 *------------------------------------------------------------------------
 */

void 
menu(void)
{
  int Selection;

  /* Loop through until the user selects exit */
  do
  {
    /* Call routine to display the menu and get a valid menu selection. */
    Selection = GetMenuItem();
    /* Determine the menu selection */
    switch( Selection )
    {
      default:
        /* Display an error to the user.  This should
         * never happen since the menu selection routine
         * already does the error checking. */
        logMsg("INTERNAL MENU ERROR!\n\n", 0, 0, 0, 0, 0, 0);
        break;
	  case DUMP_BOARD_REG:
			DumpBoardReg(theFileDesc[CurrentChannel]);
			break;
	  case DUMP_CHANNEL_REG:
			DumpChannelReg(theFileDesc[CurrentChannel]);
			break;
	  case DUMP_CHANNEL_BUFFER:
			DumpChannelBuffer(theFileDesc[CurrentChannel]);
			break;
	  case INIT:
			Init();
			break;
	  case DEINIT:
			DeInit();
			break;
	  case READ6016:
			Read(theFileDesc[CurrentChannel]);
			break;
	  case WRITE6016:
			Write(theFileDesc[CurrentChannel]);
			break;
	  case CFG_BUFFER:
			ConfigBuffer(theFileDesc[CurrentChannel]);
			break;
	  case SEND_BREAK:
			SendBreak(theFileDesc[CurrentChannel]);
			break;
	  case RUN_SELFTEST:
			RunSelftest(theFileDesc[CurrentChannel]);
			break;
	  case RESET_BOARD:
			ResetBoard(theFileDesc[CurrentChannel]);
			break;
	  case CFG_CONTROLLER:
			ConfigController(theFileDesc[CurrentChannel]);
			break;
	  case SET_CHAN_ENABLE:
			SetChannelEnable(theFileDesc[CurrentChannel]);
			break;
	  case CONFIG_ERRORS:
			ConfigErrors(theFileDesc[CurrentChannel]);
			break;
	  case READ_BOARD_REG:
			ReadBoardReg(theFileDesc[CurrentChannel]);
			break;
	  case WRITE_BOARD_REG:
			WriteBoardReg(theFileDesc[CurrentChannel]);
			break;
	  case RQST_INT_NOTIFY:
			RequestIntNotify(theFileDesc[CurrentChannel]);
			break;
	  case WAIT_FOR_NOTIFY:
			WaitForIntNotify();
			break;
	  case CFG_CHANNEL:
			ConfigChannel(theFileDesc[CurrentChannel]);
			break;
	  case GET_DEVICE_ERROR:
			GetDeviceError(theFileDesc[CurrentChannel]);
			break;
	  case READ_CHANNEL_REG:
			ReadChannelReg(theFileDesc[CurrentChannel]);
			break;
	  case WRITE_CHANNEL_REG:
			WriteChannelReg(theFileDesc[CurrentChannel]);
			break;
	  case RD_CHANNEL_RX_BUFFER:
			RdChannelBuffer(theFileDesc[CurrentChannel], RX_BUFFER);
			break;
	  case RD_CHANNEL_TX_BUFFER:
			RdChannelBuffer(theFileDesc[CurrentChannel], TX_BUFFER);
			break;
	  case WR_CHANNEL_RX_BUFFER:
			WrChannelBuffer(theFileDesc[CurrentChannel], RX_BUFFER);
			break;
	  case WR_CHANNEL_TX_BUFFER:
			WrChannelBuffer(theFileDesc[CurrentChannel], TX_BUFFER);
			break;
	  case READ_WRITE_WRAP:
			ReadWriteWrap();
			break;
	  case SELECT_CHANNEL:
			SelectChannel();
			break;
      case EXIT:
        break;
    }
  } while( Selection != EXIT );

  /* If we have a semaphore, delete it */
  if( SemID != ((SEM_ID) NULL) )
  {
    semDelete(SemID);
    SemID = 0;
  }

  /* Make sure user de-init's driver */
  if(bInit) DeInit();

}
