/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 *	install6016.c - TTY-Mode install/uninstall
 *--------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <vxWorks.h>
#include <ioLib.h>
#include "dev6016.h"
#include "drv6016.h"
#include "params.h"


void
tty6016install()
{
  int fd, status, channel;
  char fileName[20];      /* Filename of channel	*/

  /* TTY mode is disabled individually, on a per-channel basis */
  BOOL ttyEnables[16] = { FALSE, FALSE, FALSE, FALSE,
                          FALSE, FALSE, FALSE, FALSE,
                          FALSE, FALSE, FALSE, FALSE,
                          FALSE, FALSE, FALSE, FALSE };
  /*BOOL ttyEnables[16] = { TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE };*/

  /* Create the board devices */
  status = v6016DevCreate(DEV_NAME, CARD_ADDR, 3, INTLEVEL1, INTVECTOR1,
                          INTLEVEL2, INTVECTOR2, ttyEnables);
  if(status != NO_ERR)
  {
    char msg[80];
    (void) sprintf(msg,"Create device \"%s\" at 0x%08X failed !!!\n\n",
                   DEV_NAME, CARD_ADDR);
    logMsg(msg,0,0,0,0,0,0);
    return;
  }

  /* open channel, reset the card to start it in a known state
  and close channel */
  channel = 0;
  sprintf(fileName, "%s%d", DEV_NAME, channel);
  fd = open(fileName, O_RDWR, 0644);
  if(fd == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Cannot open device '%s'\n\n",fileName);
    logMsg(msg,0,0,0,0,0,0);
  }
  if(ioctl(fd, RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    return;
  }
  if(close(fd) == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Error closing channel %d\n\n",channel);
    logMsg(msg,0,0,0,0,0,0);
  }

  return;
}

void
tty6016uninstall()
{
  v6016DrvRemove();
}





/*********************************/
/* following is just example !!! */

/*
 *------------------------------------------------------------------------
 * Each 6016 channel is opened on a separate file descriptor. The table
 * below records the file descriptor for each channel we've opened.
 *------------------------------------------------------------------------
 */

LOCAL int FileDesc[16] = { ERROR, ERROR, ERROR, ERROR,
                           ERROR, ERROR, ERROR, ERROR,
                           ERROR, ERROR, ERROR, ERROR,
                           ERROR, ERROR, ERROR, ERROR };

/*
 *------------------------------------------------------------------------
 * tty6016install: Initialize VMIVME-6016 Board for TTY Operations.
 *------------------------------------------------------------------------
 */
void
tty6016install_open(void)
{
  CONFIG_BUFFER       bufferConfigInfo;  /* Holds buffer config */
  CONFIG_CHANNEL      channelConfigInfo; /* Channel config info */
  SET_CHANNEL_ENABLE  channelEnableInfo; /* Channel enable info */
  int                 channel;           /* Counts channels as we init */
  int                 status;            /* Results of last operation */
  char                fileName[20];      /* Filename of channel	*/

  /* TTY mode is enabled individually, on a per-channel basis */
  BOOL ttyEnables[16] = { TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE,
                          TRUE, TRUE, TRUE, TRUE };

  /* Create the board devices */
  status = v6016DevCreate(DEV_NAME, CARD_ADDR, 3, INTLEVEL1, INTVECTOR1,
                          INTLEVEL2, INTVECTOR2, ttyEnables);
  if(status != NO_ERR)
  {
    char msg[80];
    (void) sprintf(msg,"Create device \"%s\" at 0x%08X failed !!!\n\n",
                   DEV_NAME, CARD_ADDR);
    logMsg(msg,0,0,0,0,0,0);
    return;
  }

  /* Open each of the 16 channels */
  for(channel=0; channel<16; channel++)
  {
    sprintf(fileName, "%s%d", DEV_NAME, channel);
    FileDesc[channel] = open(fileName, O_RDWR, 0644);
    if(FileDesc[channel] == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Cannot open device '%s'\n\n",fileName);
      logMsg(msg,0,0,0,0,0,0);
    }
  }

  /* Reset the card to start it in a known state */
  if(ioctl(FileDesc[0], RESET_6016_BOARD, 0) == ERROR)
  {
    logMsg("Board Reset Failure.\n",0,0,0,0,0,0);
    return;
  }

  /* Initialize all channels */
  for(channel=0; channel<16; channel++)
  {
    /* Configure the buffer parameters */
    bufferConfigInfo.BufferAddress   = SLAVE_ADDR;
    bufferConfigInfo.ReleaseMode     = RELROR;
    bufferConfigInfo.FairnessTimeout = 0xf;
    bufferConfigInfo.UseA32          = FALSE;
    bufferConfigInfo.UseSupervisor   = FALSE;
    if(ioctl(FileDesc[channel], CONFIG_6016_BUFFER,
             (int)&bufferConfigInfo) == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Configure buffer failed on channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return;
    }

    /* Configure the channel */
    channelConfigInfo.FlowControl  = NO_FLOW;
    channelConfigInfo.BaudRate     = BAUDS_9600;
    channelConfigInfo.InputTimeout = THREE_CHAR_TIMEOUT;
    channelConfigInfo.StopBits     = ONE_STOP_BIT;
    channelConfigInfo.ParityType   = NO_PARITY;
    channelConfigInfo.ParitySense  = EVEN_PARITY;
    channelConfigInfo.NumBits      = EIGHT_BITS;
    if(ioctl(FileDesc[channel], CONFIG_6016_CHANNEL,
             (int)&channelConfigInfo) == ERROR )
    {
      char msg[80];
      (void) sprintf(msg,
                     "Configure channel failed for channel %d\n\n",
                     channel);
      logMsg(msg, 0, 0, 0, 0, 0, 0);
      return;
    }

    /* Enable Channel */
    channelEnableInfo.Enable = TRUE;
    if(ioctl(FileDesc[channel], SET_6016_CHANNEL_ENABLE,
             (int)&channelEnableInfo) == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Enable channel failed for channel %d\n\n",channel);
      logMsg( msg,0,0,0,0,0,0);
      return;
    }

    /* Setup the port */
    if(ioctl(FileDesc[channel], FIOSETOPTIONS, OPT_TERMINAL) == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Port setup failed for channel %d\n",channel);
      logMsg(msg,0,0,0,0,0,0);
      return;
    }
  }


  /*
   *================================================================
   * INSERT YOUR TTY APPLICATION HERE
   *----------------------------------------------------------------
   * At this point, the 6016 device has been created and each of the
   * channels has been initialized. The 6016 is now operational.
   *================================================================
   */

}



void
tty6016close_uninstall(void)
{
  int channel; /* Counts channels as we init */

  /* Close all the devices */
  for(channel=0; channel<16; channel++)
  {
    if(close(FileDesc[channel]) == ERROR)
    {
      char msg[80];
      (void) sprintf(msg,"Error closing channel %d\n\n",channel);
      logMsg(msg,0,0,0,0,0,0);
    }
    FileDesc[channel] = ERROR;
  }

  /* Clean up the driver */
  v6016DrvRemove();
}




