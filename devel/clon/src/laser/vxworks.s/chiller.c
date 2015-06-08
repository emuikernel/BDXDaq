
#ifdef VXWORKS

/*
 * chiller.c - VxWorks library to control temperature of chiller device
 *
 *   written by:   Kyungmo Kim <kmkim@jlab.org> (for motor.c)
 *                 Graduate Student of Kyungpook National University, Korea.
 *                 (Supervisor:       Elton Smith)
 *                 (Academic Adviser: Wooyoung Kim)
 *
 *   modified by:   SP, June 2003
 *   modified by: Nerses Gevorgyan for chiller, Dec 1, 2008
 *
 */

#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <taskLib.h>
#include "laservx.h"
#include "drv6016.h"
#include "params.h"

/* global variable */
static char cmd[20];
static char enable_OnLine_mode = 'F';         /* 'E' echo ON, 'F' echo OFF  */
static char *motor_kind[] = {"","X","Y"};     /* "X" is Mask, "Y" is Filter */
static int  fd_num[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int  limit_switch[] = {0,-1,-1,-1,-1}; /* status: 1=ON , -1=OFF */


int
chiller_open_port(int channel)
{
  CONFIG_BUFFER       bufferConfigInfo;  /* Holds buffer config */
  CONFIG_CHANNEL      channelConfigInfo; /* Channel config info */
  SET_CHANNEL_ENABLE  channelEnableInfo; /* Channel enable info */
  char                fileName[20];      /* Filename of channel	*/
  
  /* Set channel name */
  sprintf(fileName, "%s%d", DEV_NAME, channel);
  
  /* Check if it opened already */
  if(fd_num[channel] != ERROR) {
    logMsg("Device '%s' is already opened.\n\n",fileName,0,0,0,0,0);
    return(OK);
  }
  
  /* Open channel */
  fd_num[channel] = open(fileName, O_RDWR, 0644);
  if(fd_num[channel] == ERROR) {
    logMsg("Cannot open device '%s'\n\n",fileName,0,0,0,0,0);
    return(ERROR);
  }
  
  /* Initialize all channels */
  
  /* Configure the buffer parameters */
  bufferConfigInfo.BufferAddress   = SLAVE_ADDR;
  bufferConfigInfo.ReleaseMode     = RELROR;
  bufferConfigInfo.FairnessTimeout = 0xf;
  bufferConfigInfo.UseA32          = FALSE;
  bufferConfigInfo.UseSupervisor   = FALSE;
  if(ioctl(fd_num[channel], CONFIG_6016_BUFFER,
           (int)&bufferConfigInfo) == ERROR) {
    logMsg("Configure buffer failed on channel %d\n\n",channel,0,0,0,0,0);
    return(ERROR);
  }

  /* Configure the channel */
  channelConfigInfo.FlowControl  = NO_FLOW;
  channelConfigInfo.BaudRate     = BAUDS_9600;
  channelConfigInfo.InputTimeout = THREE_CHAR_TIMEOUT;
  channelConfigInfo.StopBits     = TWO_STOP_BITS;
  channelConfigInfo.ParityType   = NO_PARITY;
  channelConfigInfo.ParitySense  = EVEN_PARITY;
  channelConfigInfo.NumBits      = SEVEN_BITS;
  if(ioctl(fd_num[channel], CONFIG_6016_CHANNEL,
           (int)&channelConfigInfo) == ERROR ) {
    logMsg("Configure channel failed for channel %d\n\n",channel,0,0,0,0,0);
    return(ERROR);
  }
  
  /* Enable Channel */
  channelEnableInfo.Enable = TRUE;
  if(ioctl(fd_num[channel], SET_6016_CHANNEL_ENABLE,
           (int)&channelEnableInfo) == ERROR) {
    logMsg("Enable channel failed for channel %d\n\n",channel,0,0,0,0,0);
    return(ERROR);
  }
  
  /* Enable On-Line mode */
  write(fd_num[channel], &enable_OnLine_mode, 1);
  
  return(OK);
}

int
chiller_close_port(int channel)
{
  int close_result=1;
  
  /* Check if it closed already */
  if(fd_num[channel] == ERROR) {
    logMsg("Device '%d' is already closed.\n\n",channel,0,0,0,0,0);
    return(close_result);
  }
  
  /* Quit On-Line mode */
  write(fd_num[channel],"Q",1);
  
  /* Close channel */
  close_result=close(fd_num[channel]);
  if(close_result == ERROR)
    logMsg("Error closing channel %d\n\n",channel,0,0,0,0,0);

  fd_num[channel] = ERROR;
  return(close_result);
}


/**********************************************************************
 * Send string of char pointed to by "buff" to Velmex motor controller
 * connected to the serial port associated with "channel":
 *   return write status, normally, number of bytes actually written,
 *   return(-1) if any write errors occurred, or channel has problems.
 */
int
chiller_writeBuffer(int channel, char *buff)
{
  /* Check channel status and Open it if necessary */
  if(fd_num[channel] == -1)
    if(open_port(channel) == -1) return(-1);

  /* Send command to Velmex */
  return(write(fd_num[channel], buff, strlen(buff)));
}

/*********************************************************************
 * Read "nbytes" from Velmex (serial port associated with "channel")
 * into the buffer pointed to by "buff":
 *   return number of bytes actually read,
 *   return(-1) if any read errors occurred, or channel has problems.
 */
int
chiller_readBuffer(int channel, char *buff, int nbytes)
{
  char str[3], buff_tmp[201];
  int  i, k, str_len, tmp=-1, ii=0, nbytes_tmp=200;
  
  /* Check rationality of read */
  if(nbytes <= 0) return(-1);
  
  /* Check channel status and Open it if necessary */
  if(fd_num[channel] == -1)
    if(open_port(channel) == -1) return(-1);

  /* Read nbytes of buffer */
  while(nbytes > ii && tmp != 0) {
    if(nbytes-ii > nbytes_tmp)
      tmp = read(fd_num[channel], buff_tmp, nbytes_tmp);
    else
      tmp = read(fd_num[channel], buff_tmp, nbytes-ii);

    if(tmp < 0) return(-1);

    for(i=0; i<tmp; i++) {
      str_len = decodBufferChar(str, buff_tmp[i]);
      for(k=0; k<str_len; k++) {
	buff[ii] = str[k];
	ii++;
      }
    }
  }
  if(ii != 0) buff[ii] = '\0';
  
  return(ii);
}

/*************************************************
 * Decode raw char "ch" received from Velmex and 
 * put it into the string pointed to by "str":
 *   return length of decoded string "str".
 */
int
chiller_decodBufferChar(char *str, int ch)
{
  int i=0;
  
  /* printf("\n raw_char=%c; 0x%x", ch, ch); */
  if(ch == 0x0 || ch == 0xff) return(0);
  
  ch = ch & 0x7f;    /* hex_char to dec_char */
  switch (ch) {
  case 0x5e:         /* "^" */
    str[i] = ch;
    /* printf("       decod_char_step#%d=%c; 0x%x", i, str[i], str[i]); */
    i++;
    str[i] = ';';
    break;
  case 0xd:          /* "<cr>" - carriage return */
    str[i] = ';';
    break;
  default:
    str[i] = ch;
    break;
  }
  /* printf("       decod_char_step#%d=%c; 0x%x", i, str[i], str[i]); */
  i++;
  str[i] = '\0';
  
  return(i);
}

/********************************************************
 * Read and print the whole buffer from Velmex connected
 * to the serial port associated with "channel".
 */
void
chiller_read_buffer(int channel)
{
  char buffer[129];
  int  k=0, ii=-1, nbytes=128;
  
  while(ii != 0) {
    ii=readBuffer(channel, buffer, nbytes);
    if(ii > 0) { printf("%s", buffer); k++; }
  }
  if(k != 0) printf("\n");
}

/******************************************************************************
 * Get status of Velmex connected to the serial port associated with "channel":
 *   return('R') - controller is ready to accept any user commands,
 *   return('B') - controller is busy (for example: motor is moving) and
 *                 unable to accept commands with comma, only mode-commands,
 *   return('J') - controller is in Jog/slew mode,
 *   return('E') - if any errors occurred.
 */
char
chiller_get_status(int channel)
{
  char bb, buffer[129];
  int  nbytes=128;
  
  while(readBuffer(channel, buffer, nbytes) > 0) {}
  if(writeBuffer(channel,"V")  != 1) return('E');
  if(readBuffer(channel,&bb,1) != 1) return('E');

  return(bb);
}


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


void
chiller1()
{
  return;
}

#else

void
chiller_dummy()
{
 return;
}

#endif
