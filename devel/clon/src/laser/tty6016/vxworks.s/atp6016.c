/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 *	SCCS/s.atp6016.c 1.2 11/13/97 10:46:11
 *--------------------------------------------------------------------------
 *	Acceptance Test Program
 *--------------------------------------------------------------------------
 */

#ifndef	lint
static char atp6016_c_sccs_id[] = "@(#)atp6016.c 1.2 98/07/24 VMIC";
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

/* Definitions */

#define OK                0
#define BUFFER_TEST_SIZE  128
#define TERM_LINE_SIZE    1000
#define TX_BUFFER_SIZE    4096
#define RX_BUFFER_SIZE    4096
#define BUFFER_SIZE       (RX_BUFFER_SIZE*2)
#define RW_BUFFER_SIZE    333
#define NUM_PATTERNS      5
#define RO                1
#define RW                0
#define TRANSMIT          1
#define RECEIVE           0
#define TIMEOUT_COUNT     30
#define NUM_WRAP_PATTERNS 2000
#define ROM_VER_MASK      0xc000
#define ROM_VER_SHIFT     8
#define ONE_SECOND        (sysClkRateGet())
#define TEN_SECONDS       (10*ONE_SECOND)

/* Local Data */

LOCAL int               FileDesc[16] = {ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR,
                                        ERROR, ERROR, ERROR, ERROR};
LOCAL ULONG             slave_addr = SLAVE_ADDR;
LOCAL int               wrap = 1;
LOCAL int               slave = 1;
LOCAL int               loop_cnt = 1;
LOCAL int               debug = 1;
LOCAL int               verbose = 1;
LOCAL ULONG             first_time;
LOCAL int               WrapStatus = OK;
LOCAL int               WrapIndex;
LOCAL char             *pWrapStrings[NUM_PATTERNS] =	{
	"Wrap String1aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	"Wrap String1aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n",
	"Wrap String2bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
	"Wrap String2bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n",
	"Wrap String3cccccccccccccccccccccccccccccccccccccccccccccccccccc"
	"Wrap String3cccccccccccccccccccccccccccccccccccccccccccccccccccc\n",
	"Wrap String4dddddddddddddddddddddddddddddddddddddddddddddddddddd"
	"Wrap String4dddddddddddddddddddddddddddddddddddddddddddddddddddd\n",
	"Wrap String5eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
	"Wrap String5eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n"
};

/* Local Function Prototypes */

LOCAL int	    PerformATP(void);
LOCAL int       TestBoardReg(GLOBAL_REG Reg, UINT ByteCount, BOOL ReadOnly,
				     ULONG Expected, char *pszName);
LOCAL int       TestChanReg(CHANNEL_REG Reg, UINT ByteCount, BOOL ReadOnly,
				    ULONG Expected, char *Name);
LOCAL int       VerifyBuffer(BUFFER_TYPE WhichBuffer, ULONG BufferSize);
LOCAL int       BoardRegTest(void);
LOCAL int       ResetTest(void);
LOCAL int       SelftestTest(void);
LOCAL int       ChannelRegTest(void);
LOCAL int       BufferConfigTest(void);
LOCAL int       CtrlConfigTest(void);
LOCAL int       ErrorConfigTest(void);
LOCAL int       TXBuffTest(void);
LOCAL int       RXBuffTest(void);
LOCAL int       IntNotifyTest(void);
LOCAL int       ReadWriteWrapTest(void);
LOCAL void      StartTime(void);
LOCAL void      StopTime(void);


/*
 *------------------------------------------------------------------------
 * TestBoardReg: This routine will perform a test on a register.
 *------------------------------------------------------------------------
 */

LOCAL int 
TestBoardReg(GLOBAL_REG Reg, UINT ByteCount, BOOL ReadOnly, ULONG Expected,
             char *pszName)
{
  ULONG           Actual;
  ULONG           Mask;
  GLOBAL_REGISTER theReg;

  /* Set the register to test */
  theReg.GlobalReg = Reg;

  /* Determine the register mask */
  switch(ByteCount)
  {
    case 1:
      Mask = UCHAR_MAX;
      theReg.Value.ByteValue = (UCHAR) Expected;
      break;
    case 2:
      Mask = USHRT_MAX;
      theReg.Value.WordValue = (USHORT) Expected;
      break;
    case 4:
      Mask = ULONG_MAX;
      theReg.Value.LongValue = Expected;
      break;
  }

  /* See if this register is writeable. */
  if(!ReadOnly)
  {
    /* Write the Register */
    if(ioctl(FileDesc[0], WRITE_6016_GLOBAL_REGISTER,
        (int) &theReg) == ERROR)
    {
      logMsg("Cannot Write %s Register.\n",pszName,0,0,0,0,0);
      return(ERROR);
    }
  }

  /* Read the Register */
  theReg.Value.LongValue = 0L;
  if(ioctl(FileDesc[0], READ_6016_GLOBAL_REGISTER, (int) &theReg) == ERROR)
  {
    logMsg("Cannot Read %s Register.\n", pszName, 0, 0, 0, 0, 0);
    return(ERROR);
  }

  /* Determine value read */
  switch(ByteCount)
  {
    case 1:
      Actual = (ULONG) theReg.Value.ByteValue;
      break;
    case 2:
      Actual = (ULONG) theReg.Value.WordValue;
      if(Reg == ROM_VER_REG)
      {
        Actual = Actual & ROM_VER_MASK;
      }
      break;
    case 4:
      Actual = theReg.Value.LongValue;
      break;
  }

  /* Verify the Register */
  if((Actual & Mask) != Expected)
  {
    logMsg("%s Register Expected = %x, Actual = %x\n", pszName,
           (int) Expected, (int) (Actual & Mask), 0, 0, 0);
    return(ERROR);
  }

  return(OK); /* Return success */
}


/*
 *------------------------------------------------------------------------
 * TestChanReg: This routine will perform a test on a register.
 *------------------------------------------------------------------------
 */

LOCAL int 
TestChanReg(CHANNEL_REG Reg, UINT ByteCount, BOOL ReadOnly, ULONG Expected,
            char *Name)
{
  CHANNEL_REGISTER theReg;
  ULONG Actual;
  ULONG Mask;

  /* Set which register and channel to test */
  theReg.ChannelReg = Reg;

  /* Determine the register mask */
  switch(ByteCount)
  {
    case 1:
      Mask = UCHAR_MAX;
      theReg.Value.ByteValue = (UCHAR) Expected;
      break;
    case 2:
      Mask = USHRT_MAX;
      theReg.Value.WordValue = (USHORT) Expected;
      break;
    case 4:
      Mask = ULONG_MAX;
      theReg.Value.LongValue = Expected;
      break;
  }

  /* See if this register is writeable */
  if(!ReadOnly)
  {
    /* Write the Register */
    if(ioctl(FileDesc[0],WRITE_6016_CHANNEL_REGISTER,(int)&theReg) == ERROR)
    {
      logMsg("TestChanReg: Cannot Write %s Register.\n",Name,0,0,0,0,0);
      return(ERROR);
    }
  }

  /* Read the Register */
  theReg.Value.LongValue = 0L;
  if(ioctl(FileDesc[0], READ_6016_CHANNEL_REGISTER, (int) &theReg) == ERROR)
  {
    logMsg("TestChanReg: Cannot Read %s Register.\n",Name,0,0,0,0,0);
    return(ERROR);
  }

  /* Determine the register mask */
  switch(ByteCount)
  {
    case 1:
      Actual = (ULONG) theReg.Value.ByteValue;
      break;
    case 2:
      Actual = (ULONG) theReg.Value.WordValue;
      break;
    case 4:
      Actual = theReg.Value.LongValue;
      break;
  }

  /* Verify the register */
  if( (Actual & Mask) != Expected )
  {
    logMsg("TestChanReg: ERROR: %s Register Expected = %lx, Actual = %lx\n",
           Name,(ULONG)Expected,(ULONG)(Actual & Mask),0,0,0);
    return(ERROR);
  }
  else
  {
    logMsg("TestChanReg: INFO: %s Register Expected = %lx, Actual = %lx\n",
           Name,(ULONG)Expected,(ULONG)(Actual & Mask),0,0,0);
  }

  return(OK);
}

/*
 *------------------------------------------------------------------------
 * VerifyBuffer: This routine will verify a buffer.
 *------------------------------------------------------------------------
 */

LOCAL int 
VerifyBuffer(BUFFER_TYPE WhichBuffer, ULONG BufferSize)
{
  BUFFER_STRUCT theBuffer;
  UCHAR         pReadBuffer[BUFFER_TEST_SIZE];
  UCHAR         pWriteBuffer[BUFFER_TEST_SIZE];
  UINT          ByteIndex;
  UINT          BufferIndex;
  ULONG         Pattern = 0;
  ULONG         Offset;

  /* Initialize the buffer */
  for( ByteIndex = 0; ByteIndex < BUFFER_TEST_SIZE; ByteIndex++ )
  {
    pWriteBuffer[ByteIndex] = (UCHAR) Pattern;
    Pattern++;
  }

  /* Set up loop to go through each test buffer */
  Pattern = 0;
  for( BufferIndex = 0; BufferIndex < (BufferSize / BUFFER_TEST_SIZE);
    BufferIndex++ )
  {
    /* Initialize the offset into the buffer for this pass */
    Offset = BufferIndex * BUFFER_TEST_SIZE;
    /* Write the buffer */
    theBuffer.ByteCount = BUFFER_TEST_SIZE;
    theBuffer.Offset = Offset;
    theBuffer.pBuffer = pWriteBuffer;
    theBuffer.WhichBuffer = WhichBuffer;
    if( ioctl(FileDesc[0], WRITE_6016_BUFFER, (int)&theBuffer) == ERROR )
    {
      logMsg( "write buffer failed.\n", 0, 0, 0, 0, 0, 0 );
      return( ERROR );
    }
    /* Initialize the buffer to dummy values */
    memset(pReadBuffer, 0, BUFFER_TEST_SIZE);
    /* Read the buffer */
    theBuffer.pBuffer = pReadBuffer;
    if( ioctl(FileDesc[0], READ_6016_BUFFER, (int)&theBuffer) == ERROR )
    {
      logMsg("read buffer failed.\n", 0, 0, 0, 0, 0, 0 );
      return( ERROR );
    }
    /* Set up loop to verify the buffer */
    for( ByteIndex = 0; ByteIndex < BUFFER_TEST_SIZE; ByteIndex++ )
    {
      if( pReadBuffer[ByteIndex] != pWriteBuffer[ByteIndex] )
      {
        logMsg("Buffer Read/Write Fail at offset %x\n",
               BufferIndex * BUFFER_TEST_SIZE,0,0,0,0,0);
        return(ERROR);
      }
    }
  }
  return(OK); /* Return success */
}

/*
 *------------------------------------------------------------------------
 * BoardRegTest: verify READ_BOARD_REGISTER / WRITE_BOARD_REGISTER ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
BoardRegTest(void)
{
  int Status;

	/* Test each register until one fails. */
	Status = OK;
	if(
	(TestBoardReg(BRD_ID_REG, 1, RO, 0x0000003a, "BRD_ID") == OK)	&&
	(TestBoardReg(STFLAG_REG, 1, RO, 0x00000000, "STFLAG") == OK)	&&
	(TestBoardReg(ROM_VER_REG, 2, RO, 0x00000000, "ROM_VER") == OK) &&
	(TestBoardReg(CTR0_REG, 1, RW, 0x00000051, "CTR0") == OK)	&&
	(TestBoardReg(CTR1_REG, 1, RW, 0x0000000a, "CTR1") == OK)	&&
	(TestBoardReg(GO_REG, 2, RW, 0x0000a5a5, "GO") == OK)		&&
	(TestBoardReg(GO_REG, 2, RW, 0x00000000, "GO") == OK) 		&&
	(TestBoardReg(TX_REG, 2, RW, 0x0000a5a5, "TX") == OK) 		&&
	(TestBoardReg(RX_REG, 2, RW, 0x0000a5a5, "RX") == OK) 		&&
	(TestBoardReg(BREAK_REG, 2, RW, 0x0000a5a5, "BREAK") == OK) 	&&
	(TestBoardReg(CR2_REG, 1, RW, 0x000000a5, "CR2") == OK) 	&&
	(TestBoardReg(SZ_AM_REG, 1, RW, 0x000000ba, "SZ_AM") == OK) 	&&
	(TestBoardReg(ST_PROC_REG, 1, RW, 0x0000002a, "ST_PROC") == OK)	&&
	(TestBoardReg(ER_MSK_REG, 1, RW, 0x00000005, "ER_MSK") == OK) 	&&
	(TestBoardReg(ER_VEC_REG, 1, RW, 0x000000f4, "ER_VEC") == OK) 	&&
	(TestBoardReg(BUFBASE_REG, 4, RW, 0xa5a5a5a5L, "BUFBASE") == OK) &&
	(TestBoardReg(GST_REG, 2, RW, 0x0000aa80, "GST") == OK) 	&&
	(TestBoardReg(MAS_GRN_REG, 1, RW, 0x000000a5, "MAS_GRN") == OK) ) {
		logMsg("Board Register Test     PASS\n", 0, 0, 0, 0, 0, 0);
	} else {
		logMsg("Board Register Test     FAIL\n", 0, 0, 0, 0, 0, 0);
		Status = ERROR;
	}
	/* Reset the card so as to start it in a known state		 */
	if( ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR )	{
		logMsg("Board Reset Failure.\n", 0, 0, 0, 0, 0, 0);
		return( ERROR );
	}
	return (Status);
}

/*
 *------------------------------------------------------------------------
 * ResetTest: verifies RESET_BOARD_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
ResetTest(void)
{
  int Status;

	Status = OK;
	/* Reset the card						 */
	if( ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR )	{
		logMsg("Board Reset Failure.\n", 0, 0, 0, 0, 0, 0);
		return( ERROR );
	}
	/* Read and verify all the board registers			 */
	if( (TestBoardReg(BRD_ID_REG, 1, RO, 0x0000003a, "BRD_ID") == OK) &&
	(TestBoardReg(STFLAG_REG, 1, RO, 0x00000000, "STFLAG") == OK) 	&&
	(TestBoardReg(ROM_VER_REG, 2, RO, 0x00000000, "ROM_VER") == OK)	&&
	(TestBoardReg(CTR0_REG, 1, RO, 0x00000001, "CTR0") == OK) 	&&
	(TestBoardReg(CTR1_REG, 1, RO, 0x00000007, "CTR1") == OK) 	&&
	(TestBoardReg(GO_REG, 2, RO, 0x00000000, "GO") == OK) 		&&
	(TestBoardReg(TX_REG, 2, RO, 0x00000000, "TX") == OK) 		&&
	(TestBoardReg(RX_REG, 2, RO, 0x00000000, "RX") == OK) 		&&
	(TestBoardReg(BREAK_REG, 2, RO, 0x00000000, "BREAK") == OK) 	&&
	(TestBoardReg(CR2_REG, 1, RO, 0x000000c0, "CR2") == OK) 	&&
	(TestBoardReg(SZ_AM_REG, 1, RO, 0x000000b9, "SZ_AM") == OK) 	&&
	(TestBoardReg(ST_PROC_REG, 1, RO, 0x00000000, "ST_PROC") == OK)	&&
	(TestBoardReg(BUFBASE_REG, 4, RO, 0x00000000, "BUFBASE") == OK)	&&
	(TestBoardReg(GST_REG, 2, RO, 0x00000000, "GST") == OK) 	&&
	(TestBoardReg(MAS_GRN_REG, 1, RO, 0x00000000, "MAS_GRN") == OK) ) {
		logMsg("Board Reset Test        PASS\n", 0, 0, 0, 0, 0, 0);
	} else {
		logMsg("Board Reset Test        FAIL\n", 0, 0, 0, 0, 0, 0);
		Status = ERROR;
	}
	return( Status );
}

/*
 *------------------------------------------------------------------------
 * SelftestTest: verifies RUN_SELFTEST_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
SelftestTest(void)
{
  int Selftests;
  int Status = OK;

  /* Setup which tests to run */
  Selftests = TEST_UART | TEST_TIMER;
  /* Command the card to perform its selftest */
	if( ioctl(FileDesc[0], RUN_6016_SELFTEST, (int)Selftests) == ERROR ) {
		logMsg("Selftest Failure.\n", 0, 0, 0, 0, 0, 0);
		Status = ERROR;
	} else {
		/* See if all the tests passed				 */
		if( TestBoardReg(GST_REG, 2, RO, 0x00000000, "GST") != OK ) {
			Status = ERROR;
		}
	}
	if( Status == OK )	{
		logMsg("Board Selftest Test     PASS\n", 0, 0, 0, 0, 0, 0);
	} else {
		logMsg("Board Selftest Test     FAIL\n", 0, 0, 0, 0, 0, 0);
	}
	return( Status );
}

/*
 *------------------------------------------------------------------------
 * ChannelRegTest: verify READ_CHANNEL_REGISTER & WRITE_CHANNEL_REGISTER
 *------------------------------------------------------------------------
 */

LOCAL int
ChannelRegTest(void)
{
  int Status = OK;

  /* Read and Verify the Channel registers until one fails	 */
  if((TestChanReg(CST_REG,     2, RW, 0x0000aaaa, "CST"    ) == OK) &&
     (TestChanReg(CH_MSK_REG,  1, RW, 0x000000aa, "CH_MSK" ) == OK) &&
     (TestChanReg(CH_VEC_REG,  1, RW, 0x000000aa, "CH_VEC" ) == OK) &&
     (TestChanReg(EOB_REG,     1, RW, 0x000000aa, "EOB"    ) == OK) &&
     (TestChanReg(XOFF_REG,    1, RW, 0x000000aa, "XOFF"   ) == OK) &&
     (TestChanReg(XON_REG,     1, RW, 0x000000aa, "XON"    ) == OK) &&
     (TestChanReg(BRK_DUR_REG, 1, RW, 0x000000aa, "BRK_DUR") == OK) &&
     (TestChanReg(SZ_RING_REG, 1, RW, 0x00000055, "SZ_RING") == OK) &&
     (TestChanReg(LO_RING_REG, 1, RW, 0x000000aa, "LO_RING") == OK) &&
     (TestChanReg(HI_RING_REG, 1, RW, 0x000000aa, "HI_RING") == OK) &&
     (TestChanReg(CH_CON1_REG, 1, RW, 0x000000aa, "CH_CON1") == OK) &&
     (TestChanReg(CH_CON2_REG, 1, RW, 0x000000aa, "CH_CON2") == OK) &&
     (TestChanReg(SZ_UBUF_REG, 1, RW, 0x00000015, "SZ_UBUF") == OK))
  {
    logMsg("Channel Register Test   PASS\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("Channel Register Test   FAIL\n",0,0,0,0,0,0);
    Status = ERROR;
  }

  /* Reset the card so as to start it in a known state */
  if(ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    return(ERROR);
  }

  return(Status); /* Return status */
}

/*
 *------------------------------------------------------------------------
 * BufferConfigTest: verify BUFFER_CONFIG_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
BufferConfigTest(void)
{
  CONFIG_BUFFER  theConfigBuffer;
  int            Status = OK;

  /* Send Buffer Config Command */
  theConfigBuffer.BufferAddress = slave_addr;
  theConfigBuffer.ReleaseMode = RELROR;
  theConfigBuffer.FairnessTimeout = 10;
  theConfigBuffer.UseA32 = FALSE;
  theConfigBuffer.UseSupervisor = TRUE;
  if(ioctl(FileDesc[0], CONFIG_6016_BUFFER, (int)&theConfigBuffer) == ERROR)
  {
    logMsg("Buffer Config Failure.\n", 0, 0, 0, 0, 0, 0);
    Status = ERROR;
  }
  else
  {
    /* Read and verify the CTR0 Register */
    if(TestBoardReg(CTR0_REG, 1, RO, 0x00000051, "CTR0") != OK)
    {
      Status = ERROR;
    }
    else
    {
      /* Read and verify the CR2 Register */
      if(TestBoardReg(CR2_REG, 1, RO, 0x000000ca, "CR2") != OK)
      {
        Status = ERROR;
      }
      else
      {
        /* Read and verify the SZ_AM Register	 */
        if( TestBoardReg(SZ_AM_REG, 1, RO, 0x000000b9,"SZ_AM") != OK )
        {
          Status = ERROR;
        }
        else
        {
          /* Read and verify the BUFBASE Register */
          if( TestBoardReg(BUFBASE_REG, 4, RO, slave_addr, "BUFBASE") != OK )
          {
            Status = ERROR;
          }
          else
          {
            /* Read and verify the MAS_GRN Register */
            if( TestBoardReg(MAS_GRN_REG, 1, RO, 0, "MAS_GRN") != OK)
            {
              Status = ERROR;
            }
          }
        }
      }
    }
  }

  if(Status == OK)
  {
    logMsg("Buffer Config Test      PASS\n", 0, 0, 0, 0, 0, 0);
  }
  else
  {
    logMsg("Buffer Config Test      FAIL\n", 0, 0, 0, 0, 0, 0);
  }
  return(Status); /* Return the status */
}

/*
 *------------------------------------------------------------------------
 * CtrlConfigTest: verify CONTROLLER_CONFIG_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
CtrlConfigTest(void)
{
  CONFIG_CONTROLLER  theConfig;
  int                Status = OK;

  /* Send Buffer Config Command */
  theConfig.ArbitrationMode = PRIORITY;
  theConfig.BusErrorTimeout = 5;
  if(ioctl(FileDesc[0], CONFIG_6016_CONTROLLER, (int)&theConfig) == ERROR)
  {
    logMsg("Controller Config Failure.\n", 0, 0, 0, 0, 0, 0);
    Status = ERROR;
  }
  else
  {
    /* Read and verify the CTR1 Register */
    if(TestBoardReg(CTR1_REG, 1, RO, 0x0000000d, "CTR1") != OK)
    {
      Status = ERROR;
    }
  }
  if(Status == OK)
  {
    logMsg("Controller Config Test  PASS\n", 0, 0, 0, 0, 0, 0);
  }
  else
  {
    logMsg("Controller Config Test  FAIL\n", 0, 0, 0, 0, 0, 0);
  }

  return(Status);
}

/*
 *------------------------------------------------------------------------
 * ErrorConfigTest: verify ERROR_CONFIG_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
ErrorConfigTest(void)
{
  UINT  Errors;
  int   Status = OK;

  /* Send Error Config Command */
  Errors = (REGBUSY_MSK | RFNZ_MSK | STFAIL_MASK | HDWE_ERR_MSK | BERR_MASK);
  if(ioctl(FileDesc[0], CONFIG_6016_ERRORS, (int)Errors) == ERROR)
  {
    logMsg("Error Config Failure.\n", 0, 0, 0, 0, 0, 0);
    Status = ERROR;
  }
  else
  {
    /* Read and verify the ER_MSK Register */
    /* 0x000000fe - must correspond to actual err int level, set in atp() */
    if(TestBoardReg(ER_MSK_REG, 1, RO, 0x000000fd, "ER_MSK") != OK)
    {
      Status = ERROR;
    }
  }
  if(Status == OK)
  {
    logMsg("Error Config Test       PASS\n", 0, 0, 0, 0, 0, 0);
  }
  else
  {
    logMsg("Error Config Test       FAIL\n", 0, 0, 0, 0, 0, 0);
  }

  return(Status);
}

/*
 *------------------------------------------------------------------------
 * TXBuffTest: verify READ_CHANNEL_TX_BUFFER & WRITE_CHANNEL_TX_BUFFER ioctl
 *------------------------------------------------------------------------
 */

LOCAL int
TXBuffTest(void)
{
  UINT              MemOpt;
  CONFIG_BUFFER     theConfigBuffer;
  CHANNEL_REGISTER  theReg;
  GLOBAL_REGISTER   theGReg;
  int               Status = OK;

  /* Send Buffer Config Command */
  theConfigBuffer.BufferAddress = slave_addr;
  theConfigBuffer.ReleaseMode = RELROR;
  theConfigBuffer.FairnessTimeout = 2;
  theConfigBuffer.UseA32 = FALSE;
  theConfigBuffer.UseSupervisor = FALSE;
  if(ioctl(FileDesc[0], CONFIG_6016_BUFFER, (int)&theConfigBuffer) == ERROR)
  {
    logMsg("Buffer Config Failure.\n",0,0,0,0,0,0);
    Status = ERROR;
  }
  else
  {
    /* Call routines to set buffer sizes */
    theReg.ChannelReg = SZ_UBUF_REG;
    theReg.Value.ByteValue = 0x1f;
    if(ioctl(FileDesc[0], WRITE_6016_CHANNEL_REGISTER,
        (int) &theReg) == ERROR)
    {
      logMsg("Cannot Write SZ_UBUF Register.\n",0,0,0,0,0,0);
      Status = ERROR;
    }
    else
    {
      /* Read the memory option register to determine buffer size */
      theGReg.GlobalReg = ROM_VER_REG;
      if(ioctl(FileDesc[0], READ_6016_GLOBAL_REGISTER,
           (int) &theGReg) == ERROR)
      {
        logMsg("Cannot Read ROM_VER Register.\n",0,0,0,0,0,0);
        Status = ERROR;
      }
      else
      {
        MemOpt = 1 << ((theGReg.Value.WordValue & MEM_OPT) >> ROM_VER_SHIFT);
        /* Call routine to verify the buffer */
        Status = VerifyBuffer(TX_BUFFER, TX_BUFFER_SIZE * MemOpt);
      }
    }
  }

  if(Status == OK)
  {
    logMsg("Transmit Buffer Test    PASS\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("Transmit Buffer Test    FAIL\n",0,0,0,0,0,0);
  }

  return(Status); /* Return buffer test status */
}

/*
 *--------------------------------------------------------------------------
 * RXBuffTest: verify READ_CHANNEL_RX_BUFFER & WRITE_CHANNEL_RX_BUFFER ioctl
 *--------------------------------------------------------------------------
 */

LOCAL int 
RXBuffTest(void)
{

	int                     MemOpt;
	CONFIG_BUFFER           theConfigBuffer;
	CHANNEL_REGISTER        theReg;
	GLOBAL_REGISTER         theGReg;
	int                     Status;

	Status = OK;
	/* Send Buffer Config Command					 */
	theConfigBuffer.BufferAddress = slave_addr;
	theConfigBuffer.ReleaseMode = RELROR;
	theConfigBuffer.FairnessTimeout = 2;
	theConfigBuffer.UseA32 = FALSE;
	theConfigBuffer.UseSupervisor = FALSE;
	if( ioctl(FileDesc[0], CONFIG_6016_BUFFER, (int)&theConfigBuffer) ==
	ERROR )	{
		logMsg("Buffer Config Failure.\n", 0, 0, 0, 0, 0, 0);
		Status = ERROR;
	} else	{
		/* Call routines to set buffer sizes			 */
		theReg.ChannelReg = SZ_UBUF_REG;
		theReg.Value.ByteValue = 0x1f;
		if( ioctl(FileDesc[0], WRITE_6016_CHANNEL_REGISTER,
		(int) &theReg) == ERROR )	{
			logMsg( "Cannot Write SZ_UBUF Register.\n", 0, 0, 0,
				0, 0, 0 );
			Status = ERROR;
		} else	{
			/*
			 * Read the memory option register to determine
			 * buffer size.
			 */
			theGReg.GlobalReg = ROM_VER_REG;
			if( ioctl(FileDesc[0], READ_6016_GLOBAL_REGISTER,
			(int) &theGReg) == ERROR )	{
				logMsg( "Cannot Read ROM_VER Register.\n", 0,
					0, 0, 0, 0, 0 );
				Status = ERROR;
			} else	{
				MemOpt = 1 << (
					(theGReg.Value.WordValue & MEM_OPT) >>
					       ROM_VER_SHIFT );
				/* Call routine to verify the buffer	 */
				Status = VerifyBuffer(RX_BUFFER,
					RX_BUFFER_SIZE * MemOpt);
			}
		}
	}
	if( Status == OK )	{
		logMsg("Receive Buffer Test     PASS\n", 0, 0, 0, 0, 0, 0);
	} else	{
		logMsg("Receive Buffer Test     FAIL\n", 0, 0, 0, 0, 0, 0);
	}
	return( Status );
}

/*
 *------------------------------------------------------------------------
 * IntNotifyTest: verity INT_NOTIFY_FUNC ioctl
 *------------------------------------------------------------------------
 */

LOCAL int 
IntNotifyTest(void)
{
  GLOBAL_REGISTER    theGReg;
  REQUEST_INT_NOTIFY theNotify;
  int                Status = OK;

  /* Reset the card so as to start it in a known state */
  if(ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    Status = ERROR;
  }

  /* Send notification on error */
  theNotify.SemID = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  theNotify.NotifyConditions = ERR_INT;
  if(ioctl(FileDesc[0], REQUEST_6016_INT_NOTIFY, (int)&theNotify) == ERROR)
  {
    logMsg("Ioctl Failed\n",0,0,0,0,0,0);
    Status = ERROR;
  }
  else
  {
    if(ioctl(FileDesc[0], CONFIG_6016_ERRORS, (int)RFNZ_MSK) == ERROR)
    {
      logMsg("Error Config Failure.\n",0,0,0,0,0,0);
      Status = ERROR;
    }
    else
    {
      /* Make signal occur by writing a 1 to a reserved 0 location
      in a register and then starting the channel */
      theGReg.GlobalReg = CTR1_REG;
      theGReg.Value.ByteValue = 0x87; /* 0x17 does NOT force error !!! */
      if(ioctl(FileDesc[0],WRITE_6016_GLOBAL_REGISTER,(int)&theGReg) == ERROR)
      {
        logMsg("Cannot Write CTR1 Register.\n",0,0,0,0,0,0);
        Status = ERROR;
      }
      else
      {
        /* Wait for the interrupt to occur */
        taskDelay(ONE_SECOND);
        logMsg("Delay ..\n",0,0,0,0,0,0);
      }
    }
  }

  if( (Status == OK) && (semTake(theNotify.SemID, NO_WAIT) != ERROR) )
  {
    logMsg("Interrupt Notify Test   PASS\n",0,0,0,0,0,0);
  }
  else
  {
    Status = ERROR;
    logMsg( "Interrupt Notify Test   FAIL\n",0,0,0,0,0,0);
  }

  /* Delete notification */
  theNotify.NotifyConditions = NO_NOTIFY;
  if(ioctl(FileDesc[0], REQUEST_6016_INT_NOTIFY, (int)&theNotify) == ERROR)
  {
    logMsg("Unrequest Interrupt Notify Failed\n",0,0,0,0,0,0);
    Status = ERROR;
  }

  semDelete(theNotify.SemID); /* Free the semaphore */
  return(Status);             /* Return status */
}


/*
 *------------------------------------------------------------------------
 * ReadWriteWrapTest: verify read(2)/write(2)/ioctl(2)
 *------------------------------------------------------------------------
 * IOCTL's Tested: BREAK_FUNC, BUFFER_CONFIG_FUNC, CHANNEL_CONFIG_FUNC,
 * and CHANNEL_ENABLE_FUNC
 *------------------------------------------------------------------------
 */

#define  CH1  0
#define  CH2  0

LOCAL int 
ReadWriteWrapTest(void)
{
  UINT               NumLines = 25;
  SEM_ID             XmtSemID[16] =	{ 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0 };
  SEM_ID             RcvSemID[16] =	{ 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0 };
  UINT               Line;
  UINT               index, itmp;
  UCHAR              TransmitBuffer[RW_BUFFER_SIZE];
  UCHAR              ReceiveBuffer[RW_BUFFER_SIZE];
  UINT               LastValue;
  UINT               NextValue;
  UINT               SaveValue;
  BOOL               Pass = TRUE;
  UINT               Channel;
  CONFIG_BUFFER      theConfigBuffer;
  CONFIG_CHANNEL     theChannel;
  CHANNEL_REGISTER   theReg;
  REQUEST_INT_NOTIFY theTNotify;
  REQUEST_INT_NOTIFY theRNotify;
  SET_CHANNEL_ENABLE theEnable;

  /* Reset Board */
  ioctl(FileDesc[0], RESET_6016_BOARD, 0);

  /* Configure the buffer parameters */
  theConfigBuffer.BufferAddress = SLAVE_ADDR;
  theConfigBuffer.ReleaseMode = RELROR;
  theConfigBuffer.FairnessTimeout = 0xF;
  theConfigBuffer.UseA32 = FALSE;
  theConfigBuffer.UseSupervisor = FALSE;
  if(ioctl(FileDesc[0], CONFIG_6016_BUFFER, (int)&theConfigBuffer) == ERROR)
  {
    logMsg("Configure Buffer Failed\n\n",0,0,0,0,0,0);
    Pass = FALSE;
  }

  /* Configure the channels */
  for(Channel=CH1; (Channel<=CH2) && (Pass == TRUE); Channel++)
  {
    /* general parameters */
    theChannel.FlowControl = NO_FLOW;
    theChannel.BaudRate = BAUDS_38400;
    theChannel.InputTimeout = NO_TIMEOUT;
    theChannel.StopBits = ONE_STOP_BIT;
    theChannel.ParityType = NO_PARITY;
    theChannel.ParitySense = EVEN_PARITY;
    theChannel.NumBits = EIGHT_BITS;
    if(ioctl(FileDesc[Channel],CONFIG_6016_CHANNEL,(int)&theChannel) == ERROR)
    {
      logMsg( "Configure Channel Failed\n\n",0,0,0,0,0,0);
      Pass = FALSE;
      break;
    }

    /* Indicate end of buffer on carriage return (code 0x0A) */
    theReg.ChannelReg = EOB_REG;
    theReg.Value.ByteValue = 0x0A;
    if(ioctl(FileDesc[Channel],WRITE_6016_CHANNEL_REGISTER,(int)&theReg)
        == ERROR)
    {
      logMsg("Write EOB Channel Register Failed\n\n",0,0,0,0,0,0);
      Pass = FALSE;
      break;
    }

    /* Request Interrupt Notification */
    XmtSemID[Channel] = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if(XmtSemID[Channel] == 0) Pass = FALSE;
    RcvSemID[Channel] = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if(RcvSemID[Channel] == 0) Pass = FALSE;
    theTNotify.SemID = XmtSemID[Channel];
    theTNotify.NotifyConditions = TX_MSK;
    theRNotify.SemID = RcvSemID[Channel];
    theRNotify.NotifyConditions = RCV_MSK;
    if((ioctl(FileDesc[Channel], REQUEST_6016_INT_NOTIFY, (int)&theTNotify)
         == ERROR) ||
       (ioctl(FileDesc[Channel], REQUEST_6016_INT_NOTIFY, (int)&theRNotify)
         == ERROR))
    {
      logMsg("Request Interrupt Notification Failed\n\n",0,0,0,0,0,0);
      Pass = FALSE;
    }
    /*else
    {
logMsg("Interrupt Notification registered: ch=%2d, Xmt=0x%08x, Rcv=0x%08x\n",
Channel,(int)theTNotify.SemID,(int)theRNotify.SemID,0,0,0);
    }*/
  }

  /* Enable the channels (calls StartChannel) */
  for(Channel=CH1; Channel<=CH2; Channel++)
  {
    theEnable.Enable = TRUE;
    if(ioctl(FileDesc[Channel], SET_6016_CHANNEL_ENABLE, (int) &theEnable)
         == ERROR)
    {
      logMsg("Set Channel Enable Failed\n\n",0,0,0,0,0,0);
      Pass = FALSE;
    }
  }

  /* Read/Write Wrap Test */
  logMsg("Read/Write Wrap Test\n",0,0,0,0,0,0);
  NextValue = 0;
  for(Line=0; (Line<NumLines) && (Pass); Line++)
  {
    logMsg("    Pass %d\n",Line+1,0,0,0,0,0);

    /* save starting value */
    LastValue = NextValue;

    /* Store the data patterns into buffer */
    for(index=0; index<(RW_BUFFER_SIZE-1); index++)
    {
      TransmitBuffer[index] = (UCHAR) NextValue;
      if(NextValue >= 0xFF)
      {
        NextValue = 0;
      }
      else
      {
        NextValue++;
        if(NextValue == 0xA) NextValue++;
      }
    }

    /* Put in the end of buffer character */
    TransmitBuffer[RW_BUFFER_SIZE-1] = 0x0A;

    /* Write out the buffer */
    for(Channel=CH1; Channel<=CH2; Channel++)
    {
      if(itmp = write(FileDesc[Channel], TransmitBuffer, RW_BUFFER_SIZE) !=
           RW_BUFFER_SIZE)
      {
        logMsg("\n1: Pass #%d Channel #%d Failed, return=%d != %d\n",
               Line+1,Channel,itmp,RW_BUFFER_SIZE,0,0);
        Pass = FALSE;
        break;
      }
    }

    /* receive data */
    for(Channel=CH1; Channel<=CH2; Channel++)
    {
      /* Wait for the data to be received */
      if(semTake(RcvSemID[Channel], TEN_SECONDS) == ERROR)
      {
        logMsg("\n1: Data was not received, semTake(0x%08x,..)\n\n",
               (int)RcvSemID[Channel],0,0,0,0,0);
        Pass = FALSE;
        break;
      }
    }

    /* Read the data written */
    SaveValue = LastValue;
    for(Channel=CH1; Channel<=CH2; Channel++)
    {
      LastValue = SaveValue;
ReceiveBuffer[0]=0;
ReceiveBuffer[1]=1;
ReceiveBuffer[2]=2;
ReceiveBuffer[3]=3;
ReceiveBuffer[4]=4;
ReceiveBuffer[5]=5;
logMsg("befor: %x %x %x %x %x %x\n",ReceiveBuffer[0],ReceiveBuffer[1],
ReceiveBuffer[2],ReceiveBuffer[3],ReceiveBuffer[4],ReceiveBuffer[5]);
      if(itmp=read(FileDesc[Channel], ReceiveBuffer, RW_BUFFER_SIZE)
         != RW_BUFFER_SIZE)
      {
        logMsg("\n2: Pass #%d Channel #%d Failed, return=%d != %d\n",
               Line+1,Channel,itmp,RW_BUFFER_SIZE,0,0);
        Pass = FALSE;
        break;
      }
logMsg("after: %x %x %x %x %x %x\n",ReceiveBuffer[0],ReceiveBuffer[1],
ReceiveBuffer[2],ReceiveBuffer[3],ReceiveBuffer[4],ReceiveBuffer[5]);

      /* Verify the data read  */
      for(index=0; index<(RW_BUFFER_SIZE-1); index++)
      {
        if(ReceiveBuffer[index] != ((UCHAR) LastValue))
        {
          logMsg("\n3: Pass #%d Channel #%d Failed\n",Line+1,Channel,0,0,0,0);
          logMsg("Data read invalid at index %d\n",index,0,0,0,0,0);
          logMsg("Expected = %x, Actual = %x\n\n",LastValue,
                 (UINT) ReceiveBuffer[index],0,0,0,0);
          Pass = FALSE;
          break;
        }
        if(LastValue >= 0xFF)
        {
          LastValue = 0;
        }
        else
        {
          LastValue++;
          if(LastValue == 0xA)
          {
            LastValue++;
          }
        }
      }
      if(!Pass) break;

      /* Verify the end of buffer character */
      if(ReceiveBuffer[RW_BUFFER_SIZE - 1] != 0xA)
      {
        logMsg("\n4: Pass #%d Channel #%d Failed\n",Line+1,Channel,0,0,0,0);
        logMsg("Data read invalid, expected 0xa, actual %x\n\n",
                ReceiveBuffer[RW_BUFFER_SIZE-1],0,0,0,0,0);
        Pass = FALSE;
        break;
      }
    }

    for(Channel=CH1; Channel<=CH2; Channel++)
    {
      /* Wait for the transmit buffer to empty */
      if(semTake(XmtSemID[Channel], TEN_SECONDS) == ERROR)
      {
        logMsg("\n2: Data was not received\n\n",0,0,0,0,0,0);
        Pass = FALSE;
        break;
      }
    }
  }

 /*
  *----------------------------------------------------------------
  * Unrequest interrupt notification.
  *----------------------------------------------------------------
  */
  for(Channel=CH1; Channel<=CH2; Channel++)
  {
    /* Free the semaphores */
    if((XmtSemID[Channel] != ((SEM_ID) NULL)) ||
       (RcvSemID[Channel] != ((SEM_ID) NULL)))
    {
      if(XmtSemID[Channel] != ((SEM_ID) NULL))
      {
        semDelete(XmtSemID[Channel]);
        theTNotify.SemID = XmtSemID[Channel];
        theTNotify.NotifyConditions = NO_NOTIFY;
        if(ioctl(FileDesc[Channel], REQUEST_6016_INT_NOTIFY,
             (int) &theTNotify) == ERROR)
        {
          logMsg("\nCannot unrequest Interrupt Notification\n\n",0,0,0,0,0,0);
          Pass = FALSE;
        }
      }
      if(RcvSemID[Channel] != ((SEM_ID) NULL))
      {
        semDelete(RcvSemID[Channel]);
        theRNotify.SemID = RcvSemID[Channel];
        theRNotify.NotifyConditions = NO_NOTIFY;
        if(ioctl(FileDesc[Channel], REQUEST_6016_INT_NOTIFY,
             (int) &theRNotify) == ERROR)
        {
          logMsg("\nCannot unrequest Interrupt Notification\n\n",0,0,0,0,0,0);
          Pass = FALSE;
        }
      }
    }
  }

  /* See if test passed */
  if(Pass)
  {
    logMsg("Read/Write Wrap Test    PASS\n",0,0,0,0,0,0);
    return(OK);
  }
  logMsg("Read/Write Wrap Test    FAIL\n",0,0,0,0,0,0);
  return(ERROR);
}


/*
 *------------------------------------------------------------------------
 * StartTime: record current system time for test throughput calcs later
 *------------------------------------------------------------------------
 */

LOCAL void 
StartTime(void)
{
  /* Save the starting time for calculating throughput */
  first_time = tickGet();
}


/*
 *------------------------------------------------------------------------
 * StopTime: handle end of a test
 *------------------------------------------------------------------------
 * This routine reads the current system time and calculates the elapsed
 * time since the StartTime routine was called and then calculates and
 * displays the throughput for a wrap routine in bits per second.
 *------------------------------------------------------------------------
 */

LOCAL void 
StopTime(void)
{
  ULONG			last_time;/* end of test time		 */
  ULONG			time_diff;/* elapsed test time		 */
  float			fTime;	/* floating elapsed time	 */

  last_time = tickGet();		/* Get the current time.	 */
  /* Calculate the time difference				 */
  time_diff = last_time - first_time;
  fTime = ( (float)time_diff) / ((float) sysClkRateGet() );
  /* Display throughput in bits per second			 */
  if(verbose)
  {
    logMsg("Throughput = %f bits per second\n",
        ((float) (strlen(pWrapStrings[0]) *
        8 * NUM_WRAP_PATTERNS)) / fTime, 0, 0, 0, 0, 0);
  }
}


/*
 *------------------------------------------------------------------------
 * PerformATP: Perform Acceptance Test
 *------------------------------------------------------------------------
 */

LOCAL int
PerformATP(void)
{
  /* Reset the card so as to start it in a known state */
  if(ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Run each of the tests until one fails */
  if((BoardRegTest()      == OK) &&
     (ResetTest()         == OK) &&
     (SelftestTest()      == OK) &&
     (ChannelRegTest()    == OK) &&
     (BufferConfigTest()  == OK) &&
     (CtrlConfigTest()    == OK) &&
     (ErrorConfigTest()   == OK) &&
     (TXBuffTest()        == OK) &&
     (RXBuffTest()        == OK) &&
     (IntNotifyTest()     == OK) &&
     (ReadWriteWrapTest() == OK) )
  {
    return(OK);
  }
  return(ERROR);
}


/*
 *------------------------------------------------------------------------
 * main: iasc command-line interface
 *------------------------------------------------------------------------
 * This routine also verifies the open(2) and close(2) functions. The
 * booting of the HP-RT Operating system verifies the install(2) function.
 *------------------------------------------------------------------------
 */

int
atp(void)
{
  int  iIndex;
  int  Status;
  char pszName[20];
  int  i;

  /* TTY mode is enabled individually, on a per-channel basis */
  BOOL ttyEnables[16] = { TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE };

  /* Print out welcome message					 */
  logMsg("\nVMIVME/SW-6016-ABC-030 Acceptance Test Program\n\n",
         0,0,0,0,0,0);

  /* Tell user the device and address				 */
  if(verbose)
  {
    logMsg("Slave Buffer Address = %x\n",slave_addr,0,0,0,0,0);
  }

  Status = v6016DevCreate(DEV_NAME, CARD_ADDR, 3, INTLEVEL1, INTVECTOR1,
                          INTLEVEL2, INTVECTOR2, (BOOL *)NULL);
                                                 /*ttyEnables*/
  if(Status != NO_ERR)
  {
    logMsg("v6016DevCreate FAILED\n\n",0,0,0,0,0,0);
    return;
  }

  for(i=0; i<16; i++)
  {
    sprintf(pszName, "%s%d", DEV_NAME, i);
    FileDesc[i] = open(pszName, O_RDWR, 0644);
  }

  if(( FileDesc[0]  == ERROR ) ||
     ( FileDesc[1]  == ERROR ) ||
     ( FileDesc[2]  == ERROR ) ||
     ( FileDesc[3]  == ERROR ) ||
     ( FileDesc[4]  == ERROR ) ||
     ( FileDesc[5]  == ERROR ) ||
     ( FileDesc[6]  == ERROR ) ||
     ( FileDesc[7]  == ERROR ) ||
     ( FileDesc[8]  == ERROR ) ||
     ( FileDesc[9]  == ERROR ) ||
     ( FileDesc[10] == ERROR ) ||
     ( FileDesc[11] == ERROR ) ||
     ( FileDesc[12] == ERROR ) ||
     ( FileDesc[13] == ERROR ) ||
     ( FileDesc[14] == ERROR ) ||
     ( FileDesc[15] == ERROR ))
  {
    logMsg("Cannot Open Device Error\n\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("\n",0,0,0,0,0,0);

    /* Call routine to run tests loop_cnt times */
    Status = OK;
    for(iIndex = 0; (iIndex < loop_cnt) && (Status == OK); iIndex++)
    {
      logMsg("*** PASS %d ***\n",iIndex+1,0,0,0,0,0);
      Status = PerformATP();
      logMsg("\n",0,0,0,0,0,0);
    }

    for(i=0; i<16; i++)
    {
      if(close(FileDesc[i]) == ERROR)
      {
        logMsg("Close Error\n\n",0,0,0,0,0,0);
      }
      FileDesc[i] = ERROR;
    }
    v6016DrvRemove();
  }

  /* Report overall status */
  if(Status == OK)
  {
    logMsg("All ATP Tests           PASS\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("ATP Test                FAIL\n",0,0,0,0,0,0);
  }

  /* Allow the user to see the results */
  logMsg("Hit Any Key to Continue: ",0,0,0,0,0,0);
  getchar();
  logMsg("\n",0,0,0,0,0,0);
  return (OK);
}
