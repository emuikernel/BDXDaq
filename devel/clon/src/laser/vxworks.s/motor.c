
#ifdef VXWORKS

/*
 * motor.c - VxWorks library to control TOF_laser motors
 *
 *   written by:   Kyungmo Kim <kmkim@jlab.org>
 *                 Graduate Student of Kyungpook National University, Korea.
 *                 (Supervisor:       Elton Smith)
 *                 (Academic Adviser: Wooyoung Kim)
 *
 *   modified by:   SP, June 2003
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


#ifdef GREEN_SPRING

int
open_port(int id)
{
  /* in 6016 board :    "/tty6016/1","/tty6016/2","/tty6016/3","/tty6016/4" */
  char *SerialPortName[] = {"","/tyIP/8","/tyIP/0","/tyIP/1","/tyIP/2"};
  
  fd_num[id] = open(SerialPortName[id], O_RDWR, 0755);     /* Open serial port */
  if(fd_num[id] == ERROR) return(ERROR);
  if(ioctl(fd_num[id], FIOBAUDRATE, 9600) == ERROR) {      /* Set baud rate */ 
    close(fd_num[id]);
    return(ERROR);
  }
  if(ioctl(fd_num[id], FIOSETOPTIONS, OPT_RAW) == ERROR) { /* Set raw mode */
    close(fd_num[id]);
    return(ERROR);
  }
  if(ioctl(fd_num[id], FIOFLUSH, 0) == ERROR) {    /* Set flush I/O buffer */
    close(fd_num[id]);
    return(ERROR);
  }
  write(fd_num[id], "F", 1);      /* Set enable On-Line mode with echo OFF */
  return(OK);
}

int
close_port(int id)
{
  write(fd_num[id],"Q",1);      /* Quit On-Line mode */
  return(close(fd_num[id]));
}

#else

int
open_port(int channel)
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
close_port(int channel)
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

#endif


void
close_all()
{
  int i;
  for(i=0;i<16;i++)
    if(fd_num[i] != ERROR) close_port(i);
}


/**********************************************************************
 * Send string of char pointed to by "buff" to Velmex motor controller
 * connected to the serial port associated with "channel":
 *   return write status, normally, number of bytes actually written,
 *   return(-1) if any write errors occurred, or channel has problems.
 */
int
writeBuffer(int channel, char *buff)
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
readBuffer(int channel, char *buff, int nbytes)
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
decodBufferChar(char *str, int ch)
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
read_buffer(int channel)
{
  char buffer[129];
  int  k=0, ii=-1, nbytes=128;
  
#ifdef GREEN_SPRING
  ioctl(fd_num[channel], FIONREAD, (int)&nbytes);
  if(nbytes <= 0) return;
#endif
  
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
get_Velmex_status(int channel)
{
  char bb, buffer[129];
  int  nbytes=128;
  
  while(readBuffer(channel, buffer, nbytes) > 0) {}
  if(writeBuffer(channel,"V")  != 1) return('E');
  if(readBuffer(channel,&bb,1) != 1) return('E');

  return(bb);
}

/***********************************************
 * Kill operation in progress, stop all motors.
 */
int
kill_operation(int channel)
{
  if(writeBuffer(channel,"K") != 1) return(-1);
  return(0);
}

/********************************************************
 * Null (zero) motors 1,2,3 absolute position registers.
 */
int
nullify_motors(int channel)
{
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"N") != 1)     return(-2);
  return(0);
}

/**************************************************************************
 * Set/Unset Velmex connected to the serial port associated with "channel"
 * enable to send "O" respose when limit switch is encountered.
 */
int
set_limit_switch(int channel)
{
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"O1,") != 3)   return(-2);
  limit_switch[channel] = 1;
  return(0);
}
int
unset_limit_switch(int channel)
{
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"O0,") != 3)   return(-2);
  limit_switch[channel] = -1;
  return(0);
}

/**********************************************************************
 * Set motor "speed" for Velmex connected to the serial port
 * associated with "channel", motor kind is defined by "motor_number".
 */
int
set_motor_speed(int channel, int motor_number, int speed)
{
  if(motor_number<1 || 2<motor_number)  return(-102);
  if(speed<1 || 6000<speed)             return(-103);
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C S%dM%d,R",motor_number,speed);
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  return(0);
}

/****************************************************************
 * Set mask/filter "speed" for Velmex associated with "channel":
 *   return( 0)   - Velmex command was sent correctly,
 *   return(-1)   - Velmex is not ready to accept command,
 *   return(-2)   - any errors occurred,
 *   return(-10n) - wrong n-th input for set_motor_speed().
 */
int
set_mask_speed(int channel, int speed)
{
  return(set_motor_speed(channel,1,speed));
}
int
set_filter_speed(int channel, int speed)
{
  return(set_motor_speed(channel,2,speed));
}

/**********************************************************************
 * Get motor position for Velmex connected to the serial port
 * associated with "channel", motor kind is defined by "motor_number":
 *   return motor position, or
 *   return(-12345678) if any errors occurred.
 */
int
get_motor_position(int channel, int motor_number)
{
  char *sign_pos, buffer[129];
  int  position, nbytes=128;
  
  while(readBuffer(channel, buffer, nbytes) > 0) {}
  writeBuffer(channel, motor_kind[motor_number]);
  readBuffer(channel, buffer, nbytes);

  if((sign_pos=strstr(buffer,"-")) == NULL)
    if((sign_pos=strstr(buffer,"+")) == NULL) return(-12345678);
  
  sscanf(sign_pos, "%d", &position);
  
  return(position);
}

/*******************************************************************
 * Get mask/filter position for Velmex connected to the serial port
 * associated with "channel":
 *   return position of motor responsible for mask/filter, or
 *   return(-12345678) if any errors occurred.
 */
int
get_mask_position(int channel)
{
  int  position;
  
  position=get_motor_position(channel,1);
  printf("%d \n", position);
  return(position);
}
int
get_filter_position(int channel)
{
  int  position;
  
  position=get_motor_position(channel,2);
  printf("%d \n", position);
  return(position);
}

/*************************************************************
 * Move mask on "step" steps from current position by Velmex 
 * connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
move_mask(int channel, int step)
{
  char bb;
  
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C I1M%d,R",step);
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  if(limit_switch[channel] == 1) {
    readBuffer(channel,&bb,1);
    if(bb == 'O') return(79);
  }  
  return(0);
}

/*********************************************************************
 * Control mask move:
 *   - get current mask position,
 *   - start mask move at new_position = current_position + "step",
 *       Note: If "step"=0 mask will be set at position = 0 !!!
 *   - looking for mask moving and waiting end of moving,
 *   - check final mask position.
 * Mask operating by Velmex connected to the serial port
 * associated with "channel":
 *   return( 0) if set correctly,
 *   return( 1) if set unreachable because of limit switch encounter,
 *   return(-1) if set unexpectedly, or any errors occurred.
 */
int
move_mask_ctrl(int channel, int step)
{
  char dots[200];
  int  p0, p1, p2, ii=0;
  
  /* Get current mask position */
  p0 = get_motor_position(channel,1);
  if(p0 == -12345678) {
    logMsg(" Error: Can't get mask position. \n\n",0,0,0,0,0,0);
    return(-1);
  }
  logMsg(" Current mask position = %d \n", p0,0,0,0,0,0);
  
  /* Start mask move */
  p1 = move_mask(channel,step);
  if(p1 != 0) {
    logMsg(" Error: Can't move mask. \n",0,0,0,0,0,0);
    if(p1 == 'O')
      logMsg("        Limit switch reached. \n",0,0,0,0,0,0);
    return(-1);
  }
  logMsg(" Start mask move \n",0,0,0,0,0,0);
  
  /* Looking for mask moving */
  sprintf(dots, " ");
  p2 = p0;
  while(ii < 2) {
    taskDelay(50);
    logMsg("   moving%s. \n", dots,0,0,0,0,0);
    strcat(dots," ");
    p1 = p2;
    p2 = get_motor_position(channel,1);
    if(p1 == p2) ii++;
    else ii=0;
  }
  logMsg(" Stop mask move \n",0,0,0,0,0,0);
  
  /* Check final mask position */
  if(((p2-p0) == step) || ((step == 0) && (p2 == 0))) {
    logMsg(" New mask position = %d, set correctly \n", p2,0,0,0,0,0);
    return(0);
  }
  else {
    if(limit_switch[channel] == 1) {
      if(step > 0) p1 =  1;
      else         p1 = -1;
      if(move_mask(channel,p1) == 'O') {
	logMsg(" New mask position = %d, limit switch was encountered \n", \
	       p2,0,0,0,0,0);
	return(1);
      }
    }
    else {
      logMsg(" New mask position = %d, set unexpectedly \n", p2,0,0,0,0,0);
      return(-1);
    }
  }
}

/**************************************************************
 * Move filter on "step" steps from current position by Velmex 
 * connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
move_filter(int channel, int step)
{
  char bb;
  
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C I2M%d,R",step);
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  if(limit_switch[channel] == 1) {
    readBuffer(channel,&bb,1);
    if(bb == 'O') return(79);
  }
  return(0);
}

/**********************************************************************
 * Loop filter, index it 800 steps from current position and back to 0
 * by Velmex connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
loop_filter(int channel)
{
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C I2M800,I2M0,R");
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  return(0);
}

/*********************************************************************
 * Control filter loop:
 *   - get current filter position,
 *   - start filter loop,
 *   - looking for filter moving and waiting the end of the loop,
 *   - check final filter position.
 * Filter operating by Velmex connected to the serial port
 * associated with "channel":
 *   return( 0) if loop was done properly,
 *   return(-1) if loop ended unexpectedly, or any errors occurred.
 */
int
loop_filter_ctrl(int channel)
{
  char *dots_tmp, dots[200];
  int  p0, p1, p2, ii=0;
  
  /* Get current filter position */
  p0 = get_motor_position(channel,2);
  if(p0 == -12345678) {
    logMsg(" Error: Can't get filter position. \n\n",0,0,0,0,0,0);
    return(-1);
  }
  logMsg(" Current filter position = %d \n", p0,0,0,0,0,0);
  
  /* Start filter loop */
  if(loop_filter(channel) != 0) {
    logMsg(" Error: Can't loop filter. \n",0,0,0,0,0,0);
    return(-1);
  }
  logMsg(" Start filter loop \n",0,0,0,0,0,0);
  
  /* Looking for filter moving */
  sprintf(dots, " ");
  p2 = p0;
  while(ii < 2) {
    taskDelay(50);
    logMsg("   moving%s. \n", dots,0,0,0,0,0);
    p1 = p2;
    p2 = get_motor_position(channel,2);
    if(p1 == p2) ii++;
    else ii=0;
    if(p1 < p2) strcat(dots," ");
    else {
      dots_tmp = strchr(dots,' ');
      if(strlen(dots_tmp) > 1)  sprintf(dots, ++dots_tmp);
    }
  }
  logMsg(" Stop filter loop \n",0,0,0,0,0,0);
  
  /* Check final filter position */
  if((p0-2 <= p2) && (p2 <= p0+2)) {
    logMsg(" Final filter position = %d, loop done correctly \n", \
	   p2,0,0,0,0,0);
    return(0);
  }
  logMsg(" Final filter position = %d, loop ended unexpectedly \n", \
	 p2,0,0,0,0,0);
  return(-1);
}

/*********************************************************
 * Init start: stop all possibly rest activities,
 *             set limit switch ON,
 *             set reasonable speeds for mask and filter,
 *             start to move them to the CCW limit switch.
 */
int
init_set(int channel)
{
  kill_operation(channel);
  set_limit_switch(channel);
  set_mask_speed(channel,200);
  set_filter_speed(channel,30);
  writeBuffer(channel,"C I1M-20000,I2M-1000,R");
  return(0);
}

/**************************************************************
 * Control motors init:
 *   - wait till mask and filter reach their CCW limit switch,
 *   - nullify indexes for both motors.
 * Init is doing for Velmex motors which connected to the
 * serial port associated with "channel":
 *   return( 0) if init done correctly,
 *   return(-1) if mask did not encounter its limit switch,
 *   return(-2) if filter did not encounter its limit switch,
 *   return(-3) if motors did not nullify.
 */
int
init_ctrl(int channel)
{
  int m1=0, m2=0;
  int f1=0, f2=0;
  
  init_set(channel);
  
  m2 = get_motor_position(channel,1);
  f2 = get_motor_position(channel,2);
  while(!((m1 == m2) && (f1 == f2))) {
    m1 = m2;
    m2 = get_motor_position(channel,1);
    f1 = f2;
    f2 = get_motor_position(channel,2);
  }

  if(move_mask(channel,-1)   != 'O') return(-1);
  if(move_filter(channel,-1) != 'O') return(-2);
  if(nullify_motors(channel) != 0)   return(-3);
  
  return(0);
}




/*********************************************************************/
/*********************************************************************/
/*********************************************************************/


void
pzz()
{
  int p1, p2, ii=0;
  
  while(ii < 3) {
    p1=get_motor_position(4,1);
    if(p1 == p2) ii++;
    else ii=0;
    printf("X_pos=%d \n", p1);
    taskDelay(100);
    p2=get_motor_position(4,1);
  }
}

void
ttt(int inp)
{
  char bb[400];
  
  sprintf(bb,"%d", inp);
  printf("\n inp=%d; bb=%s;; bb_len=%d;;; \n\n", inp, bb, strlen(bb));

  printf(" ERROR=%d(d),0x%x,%c(c),%s(s);   \n",ERROR,ERROR,ERROR,ERROR);
  printf("    OK=%d(d),0x%x,%c(c),%s(s);   \n",OK,OK,OK,OK);
}




/* this command is initialize the space frame Mask */
void
ini4()
{
  open_port(4);
 
  taskDelay(100);
  move_mask(4,-20000); 
 
  taskDelay(200);
  printf("Starting Mask move.. \n");
  move_mask(4,6000);
  taskDelay(200);
}

/* this command is control the space frame Mask */
void
Mask4()
{
  int i;
  
  open_port(4);
  printf("Moving Mask other position.. \n");

  taskDelay(100);

  move_mask(4,1);

  taskDelay(200);

  for(i=1; i<=4000; ++i)
  {
    move_mask(4,1);    
    printf("step %d has been moved \n",i);
    taskDelay(200);
  }

  printf("Stopping Mask move.. \n");
}

/* this command is initialize the Forward Carriage Mask */
void
ini1()
{
  open_port(1);
 
  taskDelay(100);
  move_mask(1,-20000); 
 
  taskDelay(200);
  printf("Starting Mask move.. \n");
  move_mask(1,170);
  taskDelay(200);
}

/* this command is control the forward carriage Mask */
void
Mask1()
{
  int i;
  
  open_port(1);
  printf("Moving Mask other position.. \n");

  taskDelay(100);

  move_mask(1,1);

  taskDelay(400);

  for(i=1; i<=750; i++)
  {
    move_mask(1,1);
    taskDelay(400);
    printf("step %d has been moved \n",i);
  }
  printf("Stopping Mask move.. \n");
}

/* this command is initialize back Mask */
void
ini(int id)
{
  open_port(id);
 
  taskDelay(100);
  move_mask(id,-20000); 
 
  taskDelay(200);
  printf("Moving Back position of Mask.. \n");
}

#else

void
motor_dummy()
{
 return;
}

#endif
