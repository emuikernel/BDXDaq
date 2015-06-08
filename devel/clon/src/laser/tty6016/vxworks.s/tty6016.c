/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 * tty6016.c - test for TTY-Mode Support
 *--------------------------------------------------------------------------
 */

#include <vxWorks.h>
#include <semLib.h>
#include <ioLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dev6016.h"
#include "drv6016.h"
#include "params.h"

#define ESC 0x1B


/* The menu items */

#define SELECT_CHANNEL                0
#define SELECT_TERMINAL_MODE          1
#define SELECT_RAW_MODE               2
#define TOGGLE_LINE_MODE              3
#define TOGGLE_ECHO_MODE              4
#define TOGGLE_CR_TRANSLATION_ENABLE  5
#define TOGGLE_XON_XOFF_ENABLE        6
#define TOGGLE_BITS                   7
#define TOGGLE_TRAP_ENABLE            8
#define TOGGLE_ABORT_ENABLE           9
#define SET_ABORT_CHARACTER          10
#define SET_BACKSPACE_CHARACTER      11
#define SET_DELETE_LINE_CHARACTER    12
#define SET_EOF_CHARACTER            13
#define SET_TRAP_CHARACTER           14
#define TTY_READ                     15
#define TTY_WRITE                    16
#define SET_BAUD                     17
#define EXIT                         18

/* Local Data */

LOCAL int    FileDesc[16] = {ERROR, ERROR, ERROR, ERROR,
                             ERROR, ERROR, ERROR, ERROR,
                             ERROR, ERROR, ERROR, ERROR,
                             ERROR, ERROR, ERROR, ERROR};
LOCAL ULONG  slave_addr = SLAVE_ADDR;
LOCAL UINT   CurrentChannel = 0;

/* Local Function Prototypes */

LOCAL int     PerformTTY(void);
LOCAL int     GetMenuItem(void);
LOCAL void    SelectChannel(void);
LOCAL void    SelectTerminalMode(void);
LOCAL void    SelectRawMode(void);
LOCAL void    ToggleLineMode(void);
LOCAL void    ToggleEchoMode(void);
LOCAL void    ToggleCRTransEnable(void);
LOCAL void    Toggle_XON_XOFF_Enable(void);
LOCAL void    ToggleBits(void);
LOCAL void    ToggleTrapEnable(void);
LOCAL void    ToggleAbortEnable(void);
LOCAL void    SetAbortChar(void);
LOCAL void    SetBSChar(void);
LOCAL void    SetDelLineChar(void);
LOCAL void    SetEOFChar(void);
LOCAL void    SetTrapChar(void);
LOCAL void    TTYRead(void);
LOCAL void    TTYWrite(void);
LOCAL void    SetBaudRate(void);
LOCAL int     GetInt(char *Prompt, int Min, int Max);
LOCAL ULONG   GetHexLong(char *Prompt);
LOCAL USHORT  GetHexShort(char *Prompt);
LOCAL int     GetMenuItem(void);
LOCAL void    GetString(char *Prompt, char *theString, unsigned int size);
LOCAL void    AbortFunc(void);

/*
 *------------------------------------------------------------------------
 * GetMenuItem: display main menu and return validated selection
 *------------------------------------------------------------------------
 */

LOCAL int 
GetMenuItem(void)
{
  int   Selection;
  BOOL  First = TRUE;
  int   options;

  /* Loop through until a valid menu option is entered  */
  do
  {
    logMsg("\n\n",0,0,0,0,0,0);

    /* If this is not the first time through, the user must
    have entered an invalid value. Tell him about it */
    if(!First) logMsg("Invalid Selection!\n\n",0,0,0,0,0,0);
    First = FALSE;

    /* Read current settings */
    options = ioctl( FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
    if(options == ERROR) logMsg("ioctl Failure\n",0,0,0,0,0,0);

    /* Display the Menu */
    logMsg( "** CHANNEL %2d     **\n",CurrentChannel,0,0,0,0,0);
    if(options == OPT_TERMINAL)
    {
      logMsg("** Terminal Mode  **\n\n",0,0,0,0,0,0);
    }
    else
    {
      if(options == OPT_RAW)
      {
        logMsg("** Raw Mode       **\n\n",0,0,0,0,0,0);
      }
      else
      {
        logMsg("** Line     %s - Echo  %s - C/R Mod %s **\n",
               (options & OPT_LINE)   ? "ENABLED " : "DISABLED",
               (options & OPT_ECHO)   ? "ENABLED " : "DISABLED",
               (options & OPT_CRMOD)  ? "ENABLED " : "DISABLED",
               0,0,0);
        logMsg("** XON/XOFF %s - Abort %s - Trap    %s **\n",
               (options & OPT_TANDEM)   ? "ENABLED " : "DISABLED",
               (options & OPT_ABORT)    ? "ENABLED " : "DISABLED",
               (options & OPT_MON_TRAP) ? "ENABLED " : "DISABLED",
               0,0,0);
        logMsg("** %d Bits **\n\n",
               (options & OPT_7_BIT)    ? 7 : 8,
               0,0,0,0,0);
      }
    }
    logMsg( "A = SELECT CHANNEL\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "B = SELECT TERMINAL MODE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "C = SELECT RAW MODE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "D = TOGGLE LINE MODE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "E = TOGGLE ECHO MODE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "F = TOGGLE C/R TRANSLATION\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "G = TOGGLE XON/XOFF ENABLE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "H = TOGGLE 7/8 BITS\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "I = TOGGLE TRAP ENABLE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "J = TOGGLE ABORT ENABLE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "K = SET ABORT CHARACTER\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "L = SET BACKSPACE CHARACTER\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "M = SET DELETE LINE CHARACTER\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "N = SET EOF CHARACTER\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "O = SET TRAP CHARACTER\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "P = READ LINE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "Q = WRITE LINE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "R = SET BAUD RATE\n", 0, 0, 0, 0, 0, 0 );
    logMsg( "S = EXIT\n\n", 0, 0, 0, 0, 0, 0 );

    /* Get the user's selection */
    logMsg("Enter Selection: ",0,0,0,0,0,0);
    Selection = getchar();
    getchar();
    logMsg("\n",0,0,0,0,0,0);

    /* We will accept either upper or lower case */
    Selection = toupper(Selection) - 'A';
  } while( (Selection < SELECT_CHANNEL) || (Selection > EXIT) );

  return(Selection); /* Return only valid selection */
}

LOCAL void
SelectChannel(void)
{
  /* Get the user's desired channel */
  CurrentChannel = GetInt("Enter Channel Number (0-15): ", 0, 15);
}

LOCAL void
SelectTerminalMode(void)
{
  /* Setup the port */
  if(ioctl( FileDesc[CurrentChannel], FIOSETOPTIONS, OPT_TERMINAL) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
SelectRawMode(void)
{
  /* Setup the port */
  if(ioctl( FileDesc[CurrentChannel], FIOSETOPTIONS, OPT_RAW ) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleLineMode(void)
{
  int options;

  /* Read current settings */
  options = ioctl( FileDesc[CurrentChannel], FIOGETOPTIONS, 0 );
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl( FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_LINE) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleEchoMode()
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl( FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_ECHO) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleCRTransEnable(void)
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl( FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_CRMOD) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
Toggle_XON_XOFF_Enable(void)
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl(FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_TANDEM) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleBits(void)
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl(FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_7_BIT) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleTrapEnable(void)
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl(FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_MON_TRAP) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
ToggleAbortEnable(void)
{
  int options;

  /* Read current settings */
  options = ioctl(FileDesc[CurrentChannel], FIOGETOPTIONS, 0);
  if(options == ERROR)
  {
    logMsg("ioctl Failure\n",0,0,0,0,0,0);
  }
  else if(ioctl(FileDesc[CurrentChannel], FIOSETOPTIONS,
          options ^ OPT_ABORT) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

LOCAL void
SetAbortChar(void)
{
  int iCh;

  iCh = GetInt( "Enter New Abort Character (0-255): ", 0, 255 );
  tyAbortSet( (char) iCh );
}

LOCAL void
SetBSChar(void)
{
  int iCh;

  iCh = GetInt( "Enter New Backspace Character (0-255): ", 0, 255 );
  tyBackspaceSet( (char) iCh );
}

LOCAL void
SetDelLineChar(void)
{
  int iCh;

  iCh = GetInt( "Enter New Delete Line Character (0-255): ", 0, 255 );
  tyDeleteLineSet( (char) iCh );
}

LOCAL void
SetEOFChar(void)
{
  int iCh;

  iCh = GetInt( "Enter New End Of File Character (0-255): ", 0, 255 );
  tyEOFSet( (char) iCh );
}

LOCAL void
SetTrapChar(void)
{
  int iCh;

  iCh = GetInt( "Enter New Trap Character (0-255): ", 0, 255 );
  tyMonitorTrapSet( (char) iCh );
}

LOCAL void
TTYRead(void)
{
  int count;
  char buf[256];

  count = read(FileDesc[CurrentChannel], buf, sizeof(buf));
  if(count > 0)
  {
    char *bp;
    bp = strchr(buf,'\n');
    if(bp) *bp = '\0';
    logMsg("Read <%s>\n",buf,0,0,0,0,0);
  }
  else
  {
    logMsg("Error Reading\n");
  }
}


LOCAL void
TTYWrite(void)
{
  int count;
  char buf[256];

  logMsg("Enter string to write: ");
  gets(buf);
  strcat(buf,"\n");
  count = write(FileDesc[CurrentChannel], buf, strlen(buf));
  if(count <= 0) logMsg("Error Writing\n");
}

LOCAL void
SetBaudRate(void)
{
  int baudRate;

  /* Prompt for baud rate */
  baudRate = GetInt("Enter New Baud Rate (50-38400): ",50,38400);

  /* Setup the port */
  if(ioctl(FileDesc[CurrentChannel], FIOBAUDRATE, baudRate) == ERROR)
  {
    logMsg("ioctl Failure.\n",0,0,0,0,0,0);
  }
}

/*
 *------------------------------------------------------------------------
 * GetInt: prompt user for an integer value (with limit checking)
 *------------------------------------------------------------------------
 */
LOCAL int 
GetInt(char *Prompt, int Min, int Max)
{
  int Value;

  /*
   * Loop through asking the user for an integer until the user
   * enters a value within the given range.
   */
  do
  {
    /* Prompt for an integer */
    logMsg(Prompt,0,0,0,0,0,0);

    /* Read the integer */
    scanf("%d", &Value);
    logMsg("\n",0,0,0,0,0,0);
    getchar();
  } while( (Value < Min) || (Value > Max) );

  return(Value); /* Return the value entered */
}

/*
 *------------------------------------------------------------------------
 * GetHexLong: prompt user for a long hex value
 *------------------------------------------------------------------------
 */

LOCAL ULONG 
GetHexLong(char *Prompt)
{
  ULONG Value;

  /* Prompt user for a hex long value */
  logMsg(Prompt,0,0,0,0,0,0);

  /* Read the value */
  scanf("%lx", &Value);
  logMsg("\n",0,0,0,0,0,0);
  getchar();

  return(Value); /* Return the value */
}

/*
 *------------------------------------------------------------------------
 * GetHexShort: prompt user fo a short hex value and return it
 *------------------------------------------------------------------------
 */

LOCAL USHORT 
GetHexShort(char *Prompt)
{
  USHORT Value;

  /* Prompt the user for a hex short value */
  logMsg(Prompt,0,0,0,0,0,0);

  /* Read the value from the user */
  scanf("%hx", &Value);
  logMsg("\n",0,0,0,0,0,0);
  getchar();

  return(Value); /* Return the value */
}

/*
 *------------------------------------------------------------------------
 * GetString: prompt user for a string and return it
 *------------------------------------------------------------------------
 */

LOCAL void 
GetString(char *Prompt, char *theString, unsigned int size)
{
  char Format[80];

  logMsg(Prompt,0,0,0,0,0,0);
  sprintf(Format, "%%%ds", size-1);
  scanf(Format, theString);
  getchar();
}

LOCAL void
AbortFunc(void)
{
  logMsg("Abort Function Called\n");
}

/*
 *------------------------------------------------------------------------
 * PerformTTY: Perform TTY
 *------------------------------------------------------------------------
 */

LOCAL int
PerformTTY(void)
{
  CONFIG_BUFFER       configBufferInfo;
  CONFIG_CHANNEL      configChannelInfo;
  SET_CHANNEL_ENABLE  channelEnable;
  int                 channel;
  int                 choice;

  /* Reset the card to start it in a known state */
  if(ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Initialize all channels */
  for(channel=0; channel<16; channel++)
  {
    /* Configure the buffer parameters */
    configBufferInfo.BufferAddress   = SLAVE_ADDR;
    configBufferInfo.ReleaseMode     = RELROR;
    configBufferInfo.FairnessTimeout = 0xf;
    configBufferInfo.UseA32          = FALSE;
    configBufferInfo.UseSupervisor   = FALSE;
    if(ioctl(FileDesc[channel], CONFIG_6016_BUFFER, (int) &configBufferInfo)
       == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Configure buffer failed on channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return(ERROR);
    }

    /* Configure the channel */
    configChannelInfo.FlowControl  = NO_FLOW;
    configChannelInfo.BaudRate     = BAUDS_9600;
    configChannelInfo.InputTimeout = THREE_CHAR_TIMEOUT;
    configChannelInfo.StopBits     = ONE_STOP_BIT;
    configChannelInfo.ParityType   = NO_PARITY;
    configChannelInfo.ParitySense  = EVEN_PARITY;
    configChannelInfo.NumBits      = EIGHT_BITS;
    if(ioctl(FileDesc[channel], CONFIG_6016_CHANNEL, (int) &configChannelInfo)
       == ERROR)
    {
      char msg[80];
      (void)sprintf(msg,"Configure channel failed for channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return(ERROR);
    }

    /* Enable Channel */
    channelEnable.Enable = TRUE;
    if(ioctl(FileDesc[channel], SET_6016_CHANNEL_ENABLE, (int) &channelEnable)
       == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Enable channel failed for channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return(ERROR);
    }

    /* Setup the port */
    if(ioctl(FileDesc[channel], FIOSETOPTIONS, OPT_TERMINAL) == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Port setup failed for channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return(ERROR);
    }
  }

  tyAbortFuncSet(AbortFunc); /* Set an abort function */

  /* Wait for a character */
  do
  {
    /* Get the menu selection */
    choice = GetMenuItem();

    /* Figure out what the user wants to do */
    switch(choice)
    {
      default:
        break;
      case SELECT_CHANNEL:
        SelectChannel();
        break;
      case SELECT_TERMINAL_MODE:
        SelectTerminalMode();
        break;
      case SELECT_RAW_MODE:
        SelectRawMode();
        break;
      case TOGGLE_LINE_MODE:
        ToggleLineMode();
        break;
      case TOGGLE_ECHO_MODE:
        ToggleEchoMode();
        break;
      case TOGGLE_CR_TRANSLATION_ENABLE:
        ToggleCRTransEnable();
        break;
      case TOGGLE_XON_XOFF_ENABLE:
        Toggle_XON_XOFF_Enable();
        break;
      case TOGGLE_BITS:
        ToggleBits();
        break;
      case TOGGLE_TRAP_ENABLE:
        ToggleTrapEnable();
        break;
      case TOGGLE_ABORT_ENABLE:
        ToggleAbortEnable();
        break;
      case SET_ABORT_CHARACTER:
        SetAbortChar();
        break;
      case SET_BACKSPACE_CHARACTER:
        SetBSChar();
        break;
      case SET_DELETE_LINE_CHARACTER:
        SetDelLineChar();
        break;
      case SET_EOF_CHARACTER:
        SetEOFChar();
        break;
      case SET_TRAP_CHARACTER:
        SetTrapChar();
        break;
      case TTY_READ:
        TTYRead();
        break;
      case TTY_WRITE:
        TTYWrite();
        break;
      case SET_BAUD:
        SetBaudRate();
        break;
    }
  } while(choice != EXIT);

  logMsg("TTY Exited Normally.\n",0,0,0,0,0,0);

  return(OK);
}

/*
 *------------------------------------------------------------------------
 * tty: iasc command-line interface
 *------------------------------------------------------------------------
 */

int
tty(void)
{
  int   status;
  char  deviceName[20];
  int   channel;
  BOOL  bTTYArray[16] = {TRUE, TRUE, TRUE, TRUE,
                         TRUE, TRUE, TRUE, TRUE,
                         TRUE, TRUE, TRUE, TRUE,
                         TRUE, TRUE, TRUE, TRUE};

  /* Print out welcome message */
  logMsg("\nVMIVME/SW-6016-ABC-030 TTY Program\n\n",0,0,0,0,0,0);

  /* Tell user the device and address */
  logMsg("Slave Buffer Address = %x\n",slave_addr,0,0,0,0,0);
  status = v6016DevCreate(DEV_NAME, CARD_ADDR, 3, INTLEVEL1, INTVECTOR1,
                          INTLEVEL2, INTVECTOR2, bTTYArray);
  if(status != NO_ERR)
  {
    logMsg("v6016DevCreate FAILED\n\n",0,0,0,0,0,0);
    return(ERROR);
  }
  for(channel=0; channel<16; channel++)
  {
    sprintf(deviceName, "%s%d", DEV_NAME, channel);
    FileDesc[channel] = open(deviceName, O_RDWR, 0644);
  }
  if((FileDesc[0]  == ERROR) ||
     (FileDesc[1]  == ERROR) ||
     (FileDesc[2]  == ERROR) ||
     (FileDesc[3]  == ERROR) ||
     (FileDesc[4]  == ERROR) ||
     (FileDesc[5]  == ERROR) ||
     (FileDesc[6]  == ERROR) ||
     (FileDesc[7]  == ERROR) ||
     (FileDesc[8]  == ERROR) ||
     (FileDesc[9]  == ERROR) ||
     (FileDesc[10] == ERROR) ||
     (FileDesc[11] == ERROR) ||
     (FileDesc[12] == ERROR) ||
     (FileDesc[13] == ERROR) ||
     (FileDesc[14] == ERROR) ||
     (FileDesc[15] == ERROR))
  {
    logMsg("Cannot Open Device Error\n\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("\n",0,0,0,0,0,0);
    /* Call routine to run tty */
    PerformTTY();
    logMsg("\n",0,0,0,0,0,0);
  }
  for(channel=0; channel<16; channel++)
  {
    if(close(FileDesc[channel]) == ERROR)
    {
      logMsg("Close Error\n\n",0,0,0,0,0,0);
    }
    FileDesc[channel] = ERROR;
  }
  v6016DrvRemove();

  return(OK);
}
