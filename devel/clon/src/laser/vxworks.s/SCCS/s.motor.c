h22271
s 00572/00603/00228
d D 1.10 03/07/21 16:44:37 sergpozd 11 10
c Completely new revision
e
s 00083/00019/00748
d D 1.9 03/04/22 10:09:49 sergpozd 10 9
c changes still in process
e
s 00008/00002/00759
d D 1.8 03/03/06 10:45:11 boiarino 9 8
c fix serial port parameters
e
s 00038/00036/00723
d D 1.7 03/03/04 18:33:13 boiarino 8 7
c in progress
e
s 00529/00011/00230
d D 1.6 03/03/03 14:01:52 boiarino 7 6
c first version for 2306 CPU
e
s 00001/00001/00240
d D 1.5 03/02/14 13:02:32 boiarino 6 5
c #include "laser.h"
e
s 00066/00080/00175
d D 1.4 03/02/14 12:44:49 boiarino 5 4
c some cleanup
e
s 00001/00001/00254
d D 1.3 03/01/08 17:06:08 boiarino 4 3
c *** empty log message ***
e
s 00074/00071/00181
d D 1.2 03/01/08 17:05:21 boiarino 3 1
c cosmetic
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/laser/motor.c
e
s 00252/00000/00000
d D 1.1 00/08/03 10:53:14 gurjyan 1 0
c date and time created 00/08/03 10:53:14 by gurjyan
e
u
U
f e 0
t
T
I 1
D 11
/*##############################################################################
E 11
I 11
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
E 11
I 5

D 11
  motor.c
E 5
                                                                           
    This C code is control program for motors of TOF Laser.
                
    Author     : Kyungmo Kim <kmkim@jlab.org>
                 Graduate Student
                 Kyungpook National University, Korea.
                 Academic Adviser : Wooyoung Kim.
               
    Supervisor : Elton Smith, TOF in CEBAF.

Jan 2000  added the command velmex_command at end
          added string .h  via #include
          added some defienitions via #define

################################################################################

  integer_to_string() : converts integer to string.
        copy_string() : copise a string.
        read_buffer() : .
      call_position() : .
       send_command() : send to Velmex command to moving motor .
          port_open() : open the Serial Port to communicate with the Velmex
         port_close() : close the Serial Port to communicate with the Velmex
                         
##############################################################################*/

E 11
D 7
#include <ioLib.h>
#include <taskLib.h>
E 7
#include <stdio.h>
#include <string.h>
I 7
#include <ioLib.h>
#include <taskLib.h>
E 7
I 6
#include "laser.h"
E 6
D 11

D 5
/*
 *  Defines 
 */
E 5
D 6

E 6
D 5
    #define CR	        13
    #define GTsym       '>'
    #define ETX	         3
    #define EOS	       '\0'
E 5
I 5
#define CR	        13
#define GTsym       '>'
#define ETX	         3
#define EOS	       '\0'
E 5
 

E 11
I 7
#include "drv6016.h"
#include "params.h"
E 7

I 10
/* global variable */
D 11
char enable_OnLine_mode = 'F';   /* E-with echo ON, F-with echo OFF */

E 10
D 5


int  fd_num[] = {-1, -1, -1, -1, -1};     /* save fd_number with velmex port */  
char *motor_kind[] = {"","X","Y"};              /* "X" is Mask, "Y" is Filter */

E 5
I 5
D 7
int  fd_num[] = {-1, -1, -1, -1, -1};  /* save fd_number with velmex port */  
char *motor_kind[] = {"","X","Y"};     /* "X" is Mask, "Y" is Filter */
E 5
int readDelay = 5;
E 7
I 7
static int  fd_num[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
E 11
I 11
static char cmd[20];
static char enable_OnLine_mode = 'F';         /* 'E' echo ON, 'F' echo OFF  */
E 11
static char *motor_kind[] = {"","X","Y"};     /* "X" is Mask, "Y" is Filter */
D 10
static int readDelay = 5;
E 10
I 10
D 11
static int  readDelay = 5;
E 11
I 11
static int  fd_num[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int  limit_switch[] = {0,-1,-1,-1,-1}; /* status: 1=ON , -1=OFF */
E 11
E 10
E 7

D 3
void  integer_to_string (int  integer, char  *string)
E 3
I 3
D 11
void
D 5
integer_to_string (int  integer, char  *string)
E 5
I 5
integer_to_string(int integer, char *string)
E 11
I 11

#ifdef GREEN_SPRING

int
open_port(int id)
E 11
E 5
E 3
{
D 5
    int temp_buffer[5];
    int *temp = &temp_buffer[0];
    int counter = 0;
E 5
I 5
D 11
  int temp_buffer[5];
  int *temp = &temp_buffer[0];
  int counter = 0;
E 11
I 11
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
E 11
E 5

D 5
    if  ( integer < 0 ) {
        integer = -integer;
        *string++ = '-';
    }
E 5
I 5
D 11
  if(integer < 0)
  {
    integer = -integer;
    *string++ = '-';
E 11
I 11
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
E 11
  }
I 11
  
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
E 11
E 5

D 5
    do {
        *temp++ = integer % 10;
        integer /= 10;
        counter++;
    }
    while( integer );
E 5
I 5
D 11
  do
  {
    *temp++ = integer % 10;
    integer /= 10;
    counter++;
  } while(integer);
E 11
I 11
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
E 11
E 5

D 5
    do {
        *string++ = '0' + *--temp;
        counter--;
    }
    while( counter );
E 5
I 5
D 11
  do
  {
    *string++ = '0' + *--temp;
    counter--;
  } while(counter);
E 5
   
D 5
    *string = '\0';
E 5
I 5
  *string = '\0';
E 11
I 11
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
E 11
E 5
}

I 11
#endif


E 11
D 5

E 5
D 3
void  copy_string (char  *from, char  *to)
  {
    while ( *from )
      *to++ = *from++;
E 3
I 3
void
D 5
copy_string (char  *from, char  *to)
E 5
I 5
D 11
copy_string(char *from, char *to)
E 11
I 11
close_all()
E 11
E 5
{
D 11
  while(*from) *to++ = *from++;
  *to = '\0';
E 11
I 11
  int i;
  for(i=0;i<16;i++)
    if(fd_num[i] != ERROR) close_port(i);
E 11
}
E 3

D 3
    *to = '\0';
  }
E 3
D 5

E 5
D 3

void  read_buffer(int  id)
E 3
I 3
D 11
void
D 5
read_buffer(int  id)
E 3
{ 
E 5
I 5
read_buffer(int id)
E 11
I 11

/**********************************************************************
 * Send string of char pointed to by "buff" to Velmex motor controller
 * connected to the serial port associated with "channel":
 *   return write status, normally, number of bytes actually written,
 *   return(-1) if any write errors occurred, or channel has problems.
 */
int
writeBuffer(int channel, char *buff)
E 11
{
E 5
D 7
  char buffer[16];
D 5
  int  i;
  int  nbytes = -1;
E 5
I 5
  int i;
E 7
I 7
D 10
  char buffer[17];
  int i, tmp;
E 7
  int nbytes = -1;
E 5

E 10
I 10
D 11
  /*  char buffer[17]; */
  char buffer[129];
  int  ii=0, nbytes = -1;
  
E 10
I 7
#ifdef GREEN_SPRING
E 7
  ioctl(fd_num[id], FIONREAD, (int)&nbytes);
D 7
  
E 7
I 7
#else
D 10
  nbytes = 16;
E 10
I 10
  /*  nbytes = 16; */
  nbytes = 128;
E 10
#endif
I 10
  
  if(nbytes != 0) ii=readBuffer(id, buffer, nbytes);
  if(ii > 0) printf("%s\n", buffer);
E 11
I 11
  /* Check channel status and Open it if necessary */
  if(fd_num[channel] == -1)
    if(open_port(channel) == -1) return(-1);

  /* Send command to Velmex */
  return(write(fd_num[channel], buff, strlen(buff)));
E 11
}
E 10

I 11
/*********************************************************************
 * Read "nbytes" from Velmex (serial port associated with "channel")
 * into the buffer pointed to by "buff":
 *   return number of bytes actually read,
 *   return(-1) if any read errors occurred, or channel has problems.
 */
E 11
E 7
D 5
  if ( nbytes != 0)
    {
      read(fd_num[id], buffer, nbytes);
E 5
I 5
D 10
  if(nbytes != 0)
  {
D 7
    read(fd_num[id], buffer, nbytes);
E 5

E 7
I 7
    tmp = read(fd_num[id], buffer, nbytes);
    printf("read %d bytes\n",tmp);
#ifndef GREEN_SPRING
    nbytes = tmp;
#endif
E 7
D 5
      for (i=0; i<nbytes ; i++) 
	buffer[i] = buffer[i] & 0x7f;                    /* hex_char to dec_char */
     
      buffer[nbytes] = '\0' ;
      
      printf("%s\n", buffer);
E 5
I 5
    for(i=0; i<nbytes; i++)
    {
      buffer[i] = buffer[i] & 0x7f; /* hex_char to dec_char */
E 10
I 10
int
D 11
readBuffer(int id, char *buff, int nbytes)
E 11
I 11
readBuffer(int channel, char *buff, int nbytes)
E 11
{
  char str[3], buff_tmp[201];
  int  i, k, str_len, tmp=-1, ii=0, nbytes_tmp=200;
  
I 11
  /* Check rationality of read */
E 11
  if(nbytes <= 0) return(-1);
  
I 11
  /* Check channel status and Open it if necessary */
  if(fd_num[channel] == -1)
    if(open_port(channel) == -1) return(-1);

  /* Read nbytes of buffer */
E 11
  while(nbytes > ii && tmp != 0) {
    if(nbytes-ii > nbytes_tmp)
D 11
      tmp = read(fd_num[id], buff_tmp, nbytes_tmp);
E 11
I 11
      tmp = read(fd_num[channel], buff_tmp, nbytes_tmp);
E 11
    else
D 11
      tmp = read(fd_num[id], buff_tmp, nbytes-ii);
    /* printf("read %d bytes, ii=%d\n", tmp, ii); */
    if(tmp <  0) return(-1);
E 11
I 11
      tmp = read(fd_num[channel], buff_tmp, nbytes-ii);

    if(tmp < 0) return(-1);

E 11
    for(i=0; i<tmp; i++) {
      str_len = decodBufferChar(str, buff_tmp[i]);
      for(k=0; k<str_len; k++) {
	buff[ii] = str[k];
	ii++;
      }
E 10
E 5
    }
I 5
D 10
    buffer[nbytes] = '\0';
    printf("%s\n", buffer);
E 10
  }
I 10
  if(ii != 0) buff[ii] = '\0';
  
  return(ii);
E 10
E 5
}

I 11
/*************************************************
 * Decode raw char "ch" received from Velmex and 
 * put it into the string pointed to by "str":
 *   return length of decoded string "str".
 */
E 11
I 10
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

I 11
/********************************************************
 * Read and print the whole buffer from Velmex connected
 * to the serial port associated with "channel".
 */
E 11
E 10
D 3
void call_position(int id1, int id2)
E 3
I 3
void
I 10
D 11
writeBuffer(int id, char *buff)
E 11
I 11
read_buffer(int channel)
E 11
{
D 11
  /*
  size_t ll;
E 11
I 11
  char buffer[129];
  int  k=0, ii=-1, nbytes=128;
E 11
  
D 11
  ll = strlen(buff);
E 11
I 11
#ifdef GREEN_SPRING
  ioctl(fd_num[channel], FIONREAD, (int)&nbytes);
  if(nbytes <= 0) return;
#endif
E 11
  
D 11
  printf("\n ll=%d; \n", ll);
  printf("\n fd_num=%d; \n\n", fd_num[id]);
  */
  write(fd_num[id], buff, strlen(buff));   /* Send command to Velmex */
E 11
I 11
  while(ii != 0) {
    ii=readBuffer(channel, buffer, nbytes);
    if(ii > 0) { printf("%s", buffer); k++; }
  }
  if(k != 0) printf("\n");
E 11
}

D 11
void
E 10
call_position(int id1, int id2)
E 11
I 11
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
E 11
E 3
{
D 11
  write(fd_num[id1], motor_kind[id2], 1);
  taskDelay(readDelay);
E 11
I 11
  char bb, buffer[129];
  int  nbytes=128;
  
  while(readBuffer(channel, buffer, nbytes) > 0) {}
  if(writeBuffer(channel,"V")  != 1) return('E');
  if(readBuffer(channel,&bb,1) != 1) return('E');

  return(bb);
E 11
}

D 3
void  send_command( int id1, int id2, int step ) 
E 3
I 3
D 11
void
I 7
position(int id1, int id2)
E 11
I 11
/***********************************************
 * Kill operation in progress, stop all motors.
 */
int
kill_operation(int channel)
E 11
{
D 8
  char space[20];
  char *position = &space[0];
E 8
I 8
D 11
  char space[20], *position;
E 8
  int present_position=0, sign_var=0, integer_value;
D 8
  int a=0;
  int b=0;
E 8
I 8
  int tmp, a=0, b=0;
E 8

  write(fd_num[id1], motor_kind[id2], 1);
  taskDelay(100);

D 8
  read(fd_num[id1],position,10);
E 8
I 8
  position = space;
  tmp = read(fd_num[id1], position, 10);
E 8

D 8
  printf("\n\n=================================================\n");
  printf("Now processing  ");
E 8
I 8
  printf("\n\n==== %d: ===== %20.20s =======================\n",tmp,position);
  printf("Search for sign ..  ");
  tmp = 0;
E 8
  while(*position !='-' && *position !='+')
  {
    if(a>50) a=0;
    a++;
    if(a%50==0)
	{
D 8
          b++;
          printf("\b");
          if(b%4==0) printf("-");
          if(b%4==1) printf("/");
          if(b%4==2) printf("|");
          if(b%4==3) printf("\\");
E 8
I 8
      b++;
      printf("\b");
      if(b%4==0) printf("-");
      if(b%4==1) printf("/");
      if(b%4==2) printf("|");
      if(b%4==3) printf("\\");
E 8

D 8
          taskDelay(8);
E 8
I 8
      taskDelay(8);
E 8
	}

    position++;
I 8
    if(tmp++ > 10) {printf("tmp=%d-break\n",tmp);break;}
E 8
  }
  printf("\b");

  if(*position=='-') sign_var=1;
  position++;

D 8
   taskDelay(100);
   b=0;
E 8
I 8
  taskDelay(100);
  b=0;
E 8

D 8
   printf("\nNext Processing  ");
   while(*position !='\0' && '0'-*position !=163)
     {
      *position = *position & 0x7f;
      integer_value = *position-'0';
      present_position = present_position * 10 + integer_value;
E 8
I 8
  printf("\nSearch for value ..  ");
  tmp = 0;
  while(*position !='\0' && '0'-*position !=163)
  {
    *position = *position & 0x7f;
    integer_value = *position - '0';
    present_position = present_position * 10 + integer_value;
E 8

D 8
          b++;
          printf("\b");
          if(b%4==0) printf("-");
          if(b%4==1) printf("/");
          if(b%4==2) printf("|");
          if(b%4==3) printf("\\");
          taskDelay(8);
E 8
I 8
    b++;
    printf("\b");
    if(b%4==0) printf("-");
    if(b%4==1) printf("/");
    if(b%4==2) printf("|");
    if(b%4==3) printf("\\");
    taskDelay(8);
E 8

D 8
      ++position;
     }
E 8
I 8
    ++position;
    if(tmp++ > 10) {printf("tmp=%d-break\n",tmp);break;}
  }
E 8

D 8
   if (sign_var == 1)
      present_position= -present_position;
E 8
I 8
  if(sign_var == 1) present_position= -present_position;
E 8

D 8
   taskDelay(100);
     printf("\n\npresent_position is %d \n\n", present_position);
E 8
I 8
  taskDelay(100);
  printf("\n\npresent_position is %d \n\n", present_position);
E 8

D 8
     printf("\n================================================================\n");
E 8
I 8
  printf("\n=============================================================\n");
E 11
I 11
  if(writeBuffer(channel,"K") != 1) return(-1);
  return(0);
E 11
E 8
}

D 11
void
positionx(int id)
E 11
I 11
/********************************************************
 * Null (zero) motors 1,2,3 absolute position registers.
 */
int
nullify_motors(int channel)
E 11
{
D 11
  char space[20];
  char *position = &space[0];
  int present_position=0, sign_var=0, integer_value;
  int a=0;
  int b=0;

  write(fd_num[id],"X", 1);
  taskDelay(100);

  read(fd_num[id],position,10);

  printf("\n\n=================================================\n");
  printf("Now processing  ");
  while(*position !='-' && *position !='+')
  {
    if(a>50) a=0;
    a++;
    if(a%50==0)
    {
          b++;
          printf("\b");
          if(b%4==0) printf("-");
          if(b%4==1) printf("/");
          if(b%4==2) printf("|");
          if(b%4==3) printf("\\");

          taskDelay(8);
	}

    position++;
  }
     printf("\b");
   if (*position=='-')
      sign_var=1;

   position++;

   taskDelay(100);
   b=0;

   printf("\nNext Processing  ");
   while(*position !='\0' && '0'-*position !=163)
     {
      *position = *position & 0x7f;
      integer_value = *position-'0';
      present_position = present_position * 10 + integer_value;

          b++;
          printf("\b");
          if(b%4==0) printf("-");
          if(b%4==1) printf("/");
          if(b%4==2) printf("|");
          if(b%4==3) printf("\\");
          taskDelay(8);

      ++position;
     }

   if (sign_var == 1)
      present_position= -present_position;

   taskDelay(100);
     printf("\n\npresent_position is %d \n\n", present_position);

     printf("\n================================================================\n");
E 11
I 11
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"N") != 1)     return(-2);
  return(0);
E 11
}

D 11
void
E 7
D 5
send_command( int id1, int id2, int step ) 
E 5
I 5
send_command(int id1, int id2, int step) 
E 11
I 11
/**************************************************************************
 * Set/Unset Velmex connected to the serial port associated with "channel"
 * enable to send "O" respose when limit switch is encountered.
 */
int
set_limit_switch(int channel)
E 11
E 5
E 3
{
D 5
    char  command_buffer[20];
    char  *command = &command_buffer[0];
    int   string = 8;
     
    switch (id2) {                                    /* select a kind of motor */
    case 1 :    
E 5
I 5
D 11
  char command_buffer[20];
  char *command = &command_buffer[0];
  int string = 8;

  switch(id2)  /* select a kind of motor */
  {
    case 1: 
E 5
      copy_string("C I1M", command);
      break;
D 5
    case 2 :    
E 5
I 5
    case 2:    
E 5
      copy_string("C I2M", command);
      break;
D 5
    case 3 :    
E 5
I 5
    case 3:    
E 5
      copy_string("C S1M", command);
      break;
D 5
    case 4 :    
E 5
I 5
    case 4:    
E 5
      copy_string("C S2M", command);
      break;
    default :
      return;
D 5
    }
E 5
I 5
  }
E 5

D 5

    command += 5;   
     
    integer_to_string(step, command);  

    while ( *command ) {      
        command++;
        string++;
    }
        
    copy_string(", R", command);

    write(fd_num[id1], &command_buffer[0], string);   /* Send command to Velmex */
E 5
I 5
  command += 5;
  integer_to_string(step, command);
  while(*command)
  {      
    command++;
    string++;
  }
  copy_string(", R", command);
  write(fd_num[id1], &command_buffer[0], string);  /* Send command to Velmex */
E 11
I 11
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"O1,") != 3)   return(-2);
  limit_switch[channel] = 1;
  return(0);
E 11
E 5
}
D 11

D 5

E 5
D 3
void loop_filter(int id)
E 3
I 3
void
loop_filter(int id)
E 11
I 11
int
unset_limit_switch(int channel)
E 11
E 3
{
I 7
D 11
  printf("Starting loop.. \n");
  taskDelay(100);
E 7
D 3
    write(fd_num[id],"C I2M800,I2M-800,R",18); 
E 3
I 3
  write(fd_num[id],"C I2M800,I2M-800,R",18); 
I 7
  taskDelay(100);
  printf("Loop Ending.. \n");
E 11
I 11
  if(get_Velmex_status(channel) != 'R') return(-1);
  if(writeBuffer(channel,"O0,") != 3)   return(-2);
  limit_switch[channel] = -1;
  return(0);
E 11
E 7
E 3
}

D 5

E 5
D 3
void pre_move(int id)
E 3
I 3
D 11
void
I 7
move_mask(int id, int motorid, int step)
E 11
I 11
/**********************************************************************
 * Set motor "speed" for Velmex connected to the serial port
 * associated with "channel", motor kind is defined by "motor_number".
 */
int
set_motor_speed(int channel, int motor_number, int speed)
E 11
{
D 11
  char command_buffer[20];
  char *command = &command_buffer[0];
  int string = 8;

  switch(motorid)
  {
    case 1 : 
      copy_string("C I1M", command);
      break;
    case 2 :
      copy_string("C S1M", command);
      break;
    default:
      return;
  }

  command += 5;
  integer_to_string(step, command);
  while(*command)
  {
    command++;
    string++;
  }
  copy_string(", R", command);
  write(fd_num[id], &command_buffer[0], string); /* Send command to Velmex*/

  return;
E 11
I 11
  if(motor_number<1 || 2<motor_number)  return(-102);
  if(speed<1 || 6000<speed)             return(-103);
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C S%dM%d,R",motor_number,speed);
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  return(0);
E 11
}

D 11

void
A(int id, int motorid, int step)
E 11
I 11
/****************************************************************
 * Set mask/filter "speed" for Velmex associated with "channel":
 *   return( 0)   - Velmex command was sent correctly,
 *   return(-1)   - Velmex is not ready to accept command,
 *   return(-2)   - any errors occurred,
 *   return(-10n) - wrong n-th input for set_motor_speed().
 */
int
set_mask_speed(int channel, int speed)
E 11
{
D 11
  char command_buffer[20];
  char *command = &command_buffer[0];
  int string = 8;

  switch(motorid)
  {
    case 1 : 
      copy_string("C I1M", command);
      break;
    case 2 :
      copy_string("C S1M", command);
      break;
    default:
      return;
  }

  command += 5;

  integer_to_string(step, command);

  while(*command)
  {
    command++;
    string++;
  }

  copy_string(", R", command);
  write(fd_num[id], &command_buffer[0], string); /* Send command to Velmex */
E 11
I 11
  return(set_motor_speed(channel,1,speed));
E 11
}
I 11
int
set_filter_speed(int channel, int speed)
{
  return(set_motor_speed(channel,2,speed));
}
E 11

D 11

void
B(int id, int filterid, int step)
E 11
I 11
/**********************************************************************
 * Get motor position for Velmex connected to the serial port
 * associated with "channel", motor kind is defined by "motor_number":
 *   return motor position, or
 *   return(-12345678) if any errors occurred.
 */
int
get_motor_position(int channel, int motor_number)
E 11
{
D 11
  char command_buffer[20];
  char *command = &command_buffer[0];
  int string = 8;
E 11
I 11
  char *sign_pos, buffer[129];
  int  position, nbytes=128;
  
  while(readBuffer(channel, buffer, nbytes) > 0) {}
  writeBuffer(channel, motor_kind[motor_number]);
  readBuffer(channel, buffer, nbytes);
E 11

D 11
  switch(filterid)
  {
    case 1 : 
      copy_string("C I2M", command);
      break;
    case 2 :
      copy_string("C S2M", command);
      break;
    default:
      return;
  }
E 11
I 11
  if((sign_pos=strstr(buffer,"-")) == NULL)
    if((sign_pos=strstr(buffer,"+")) == NULL) return(-12345678);
  
  sscanf(sign_pos, "%d", &position);
  
  return(position);
}
E 11

D 11
  command += 5;
  integer_to_string(step, command);
  while (*command)
  {
    command++;
    string++;
  }

  copy_string(", R", command);
  write(fd_num[id], &command_buffer[0], string); /* Send command to Velmex */
E 11
I 11
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
E 11
}
I 11
int
get_filter_position(int channel)
{
  int  position;
  
  position=get_motor_position(channel,2);
  printf("%d \n", position);
  return(position);
}
E 11

D 11
void
Test(int id, int motorid, int step)
E 11
I 11
/*************************************************************
 * Move mask on "step" steps from current position by Velmex 
 * connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
move_mask(int channel, int step)
E 11
{
D 11
  int i;
  printf("Starting Mask move.. \n");
E 11
I 11
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
E 11

D 11
  {
    char command_buffer[20];
    char *command = &command_buffer[0];
    int string = 8;

    switch(motorid)
    {
      case 1 : 
        copy_string("C I1M", command);
        break;
      case 2 :
        copy_string("C S1M", command);
        break;
      default:
        return;
E 11
I 11
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
E 11
      }
D 11

    command += 5;
    integer_to_string(step, command);

    while(*command)
    {
      command++;
      string++;
E 11
    }
D 11
    
    copy_string(", R", command);
    write(fd_num[id], &command_buffer[0], string); /* Send command to Velmex*/ 
E 11
I 11
    else {
      logMsg(" New mask position = %d, set unexpectedly \n", p2,0,0,0,0,0);
      return(-1);
    }
E 11
  }
D 11

  taskDelay(100); 
  printf("Mask Stop.. \n");
  taskDelay(100);
 
  printf("Mask Position Processing.. \n");
  positionx(id);

  taskDelay(100);
  loop_filter(id);

  for(i=1; i<=24; ++i)
  {
    printf("Starting Mask move.. \n");
    taskDelay(100);
    move_mask(id, motorid, step);
    taskDelay(100); 
    printf("Mask Stop.. \n");
    taskDelay(100);

    printf("Mask Position Processing.. \n");
    positionx(id);

    taskDelay(100);
    loop_filter(id);
   }
E 11
}

D 11
void
E 7
pre_move(int id)
E 11
I 11
/**************************************************************
 * Move filter on "step" steps from current position by Velmex 
 * connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
move_filter(int channel, int step)
E 11
E 3
{
D 3
    write(fd_num[id],"C I2M100,I2M-1000,R",19);
E 3
I 3
D 11
  write(fd_num[id],"C I2M100,I2M-1000,R",19);
E 11
I 11
  char bb;
  
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C I2M%d,R",step);
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  if(limit_switch[channel] == 1) {
    readBuffer(channel,&bb,1);
    if(bb == 'O') return(79);
  }
  return(0);
E 11
E 3
}

D 5

E 5
D 3
void null(int id)
E 3
I 3
D 11
void
null(int id)
E 11
I 11
/**********************************************************************
 * Loop filter, index it 800 steps from current position and back to 0
 * by Velmex connected to the serial port associated with "channel":
 *   return( 0) if Velmex command was sent correctly,
 *   return(-1) if Velmex is not ready to accept command,
 *   return(-2) if any errors occurred.
 */
int
loop_filter(int channel)
E 11
E 3
{
D 3
    write(fd_num[id],"N",1);
E 3
I 3
D 11
  write(fd_num[id],"N",1);
E 11
I 11
  if(get_Velmex_status(channel) != 'R') return(-1);
  sprintf(cmd,"C I2M800,I2M0,R");
  if(writeBuffer(channel,cmd) != strlen(cmd)) return(-2);
  return(0);
E 11
E 3
}

I 7
D 11



#ifdef GREEN_SPRING

E 11
I 11
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
E 11
E 7
D 5

E 5
D 3
int open_port(int id)
E 3
I 3
int
D 11
open_port(int id)
E 3
{ 
D 3
  char *SerialPortName[] = { "",
                             "/tyIP/8",  
			     "/tyIP/0",	              
			     "/tyIP/1",
			     "/tyIP/2"  };
  
  fd_num[id] = open(SerialPortName[id], O_RDWR, 0755);  /* Open the serial port */
  
  if  ( fd_num[id]  ==  ERROR )  
E 3
I 3
D 7
  char *SerialPortName[] = { "","/tyIP/8","/tyIP/0","/tyIP/1","/tyIP/2"};
E 7
I 7
  /* in 6016 board :    "/tty6016/1","/tty6016/2","/tty6016/3","/tty6016/4" */
  char *SerialPortName[] = {"","/tyIP/8","/tyIP/0","/tyIP/1","/tyIP/2"};
E 7

  fd_num[id] = open(SerialPortName[id], O_RDWR, 0755); /* Open serial port */
  if(fd_num[id] == ERROR) return(ERROR);
  if(ioctl(fd_num[id], FIOBAUDRATE, 9600) == ERROR)    /* Set baud rate */ 
  {
    close(fd_num[id]);
E 3
    return(ERROR);
E 11
I 11
loop_filter_ctrl(int channel)
{
  char *dots_tmp, dots[200];
  int  p0, p1, p2, ii=0;
  
  /* Get current filter position */
  p0 = get_motor_position(channel,2);
  if(p0 == -12345678) {
    logMsg(" Error: Can't get filter position. \n\n",0,0,0,0,0,0);
    return(-1);
E 11
D 3
  if  ( ioctl( fd_num[id], FIOBAUDRATE, 9600)  ==  ERROR )    /* Set baud rate */ 
    {
      close(fd_num[id]);
      return(ERROR);
    }
  if  ( ioctl(fd_num[id], FIOSETOPTIONS, OPT_RAW)  ==  ERROR ) /* Set raw mode */
    {
      close(fd_num[id]);
      return(ERROR);
    } 
  if  ( ioctl(fd_num[id], FIOFLUSH, 0)  ==  ERROR )    /* Set flush I/O buffer */
    {
      close(fd_num[id]);
      return(ERROR);
    }
E 3
I 3
  }
D 11
  if(ioctl(fd_num[id], FIOSETOPTIONS, OPT_RAW) == ERROR) /* Set raw mode */
  {
    close(fd_num[id]);
    return(ERROR);
  } 
  if(ioctl(fd_num[id], FIOFLUSH, 0) == ERROR)    /* Set flush I/O buffer */
  {
    close(fd_num[id]);
    return(ERROR);
E 11
I 11
  logMsg(" Current filter position = %d \n", p0,0,0,0,0,0);
  
  /* Start filter loop */
  if(loop_filter(channel) != 0) {
    logMsg(" Error: Can't loop filter. \n",0,0,0,0,0,0);
    return(-1);
E 11
  }
I 11
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
E 11
E 3

D 3
  write(fd_num[id], "F", 1);          /* set enable On-Line mode with echo OFF */
E 3
I 3
D 11
  write(fd_num[id], "F", 1);      /* set enable On-Line mode with echo OFF */
E 3
  return(OK);
} 

D 3
void close_port(int id) 
E 3
I 3
void
close_port(int id)
E 11
I 11
/*********************************************************
 * Init start: stop all possibly rest activities,
 *             set limit switch ON,
 *             set reasonable speeds for mask and filter,
 *             start to move them to the CCW limit switch.
 */
int
init_set(int channel)
E 11
E 3
{
D 3
  write(fd_num[id],"Q",1);                                /* Quit On-Line mode */ 
E 3
I 3
D 11
  write(fd_num[id],"Q",1);  /* Quit On-Line mode */ 
E 3
  close(fd_num[id]);
E 11
I 11
  kill_operation(channel);
  set_limit_switch(channel);
  set_mask_speed(channel,200);
  set_filter_speed(channel,30);
  writeBuffer(channel,"C I1M-20000,I2M-1000,R");
  return(0);
E 11
}

I 7
D 11
#else
E 7

E 11
I 11
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
E 11
I 7
int
D 11
open_port(int channel)
E 11
I 11
init_ctrl(int channel)
E 11
{
D 11
  CONFIG_BUFFER       bufferConfigInfo;  /* Holds buffer config */
  CONFIG_CHANNEL      channelConfigInfo; /* Channel config info */
  SET_CHANNEL_ENABLE  channelEnableInfo; /* Channel enable info */
  char                fileName[20];      /* Filename of channel	*/

  /* set channel name */
  sprintf(fileName, "%s%d", DEV_NAME, channel);

  /* check if it opened already */
  if(fd_num[channel] != ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Device '%s' is already opened.\n\n",fileName);
    logMsg(msg,0,0,0,0,0,0);
    return(OK);
E 11
I 11
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
E 11
  }

D 11
  /* open channel */
  fd_num[channel] = open(fileName, O_RDWR, 0644);
  if(fd_num[channel] == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Cannot open device '%s'\n\n",fileName);
    logMsg(msg,0,0,0,0,0,0);
    return(ERROR);
  }
E 11
I 11
  if(move_mask(channel,-1)   != 'O') return(-1);
  if(move_filter(channel,-1) != 'O') return(-2);
  if(nullify_motors(channel) != 0)   return(-3);
  
  return(0);
}
E 11

D 11
  /* Initialize all channels */
E 11

D 11
  /* Configure the buffer parameters */
  bufferConfigInfo.BufferAddress   = SLAVE_ADDR;
  bufferConfigInfo.ReleaseMode     = RELROR;
  bufferConfigInfo.FairnessTimeout = 0xf;
  bufferConfigInfo.UseA32          = FALSE;
  bufferConfigInfo.UseSupervisor   = FALSE;
  if(ioctl(fd_num[channel], CONFIG_6016_BUFFER,
           (int)&bufferConfigInfo) == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Configure buffer failed on channel %d\n\n",channel);
    logMsg(msg,0,0,0,0,0,0);
    return(ERROR);
  }
E 11

D 11
  /* Configure the channel */
  channelConfigInfo.FlowControl  = NO_FLOW;
  channelConfigInfo.BaudRate     = BAUDS_9600;
  channelConfigInfo.InputTimeout = THREE_CHAR_TIMEOUT;
D 9
  channelConfigInfo.StopBits     = ONE_STOP_BIT;
E 9
I 9
  channelConfigInfo.StopBits     = TWO_STOP_BITS;
E 9
  channelConfigInfo.ParityType   = NO_PARITY;
  channelConfigInfo.ParitySense  = EVEN_PARITY;
D 9
  channelConfigInfo.NumBits      = EIGHT_BITS;
E 9
I 9
  channelConfigInfo.NumBits      = SEVEN_BITS;
E 9
  if(ioctl(fd_num[channel], CONFIG_6016_CHANNEL,
           (int)&channelConfigInfo) == ERROR )
  {
    char msg[80];
    (void) sprintf(msg,"Configure channel failed for channel %d\n\n",channel);
    logMsg(msg,0,0,0,0,0,0);
    return(ERROR);
  }
E 11

D 11
  /* Enable Channel */
  channelEnableInfo.Enable = TRUE;
  if(ioctl(fd_num[channel], SET_6016_CHANNEL_ENABLE,
           (int)&channelEnableInfo) == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Enable channel failed for channel %d\n\n",channel);
    logMsg( msg,0,0,0,0,0,0);
    return(ERROR);
  }
E 11
I 11
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
E 11

I 9
D 11
  /* enable On-Line mode */
D 10
  write(fd_num[channel], "E", 1); /* E-echo ON, F-echo OFF */
E 10
I 10
  write(fd_num[channel], &enable_OnLine_mode, 1);
E 11
E 10

E 9
D 11
  return(OK);
E 11
I 11
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
E 11
}

void
D 11
close_port(int channel)
E 11
I 11
ttt(int inp)
E 11
{
D 11
  /* check if it closed already */
  if(fd_num[channel] == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Device '%d' is already closed.\n\n",channel);
    logMsg(msg,0,0,0,0,0,0);
    return;
  }
E 11
I 11
  char bb[400];
  
  sprintf(bb,"%d", inp);
  printf("\n inp=%d; bb=%s;; bb_len=%d;;; \n\n", inp, bb, strlen(bb));
E 11

I 9
D 11
  /* Quit On-Line mode */
  write(fd_num[channel],"Q",1);

E 9
  /* close channel */
  if(close(fd_num[channel]) == ERROR)
  {
    char msg[80];
    (void) sprintf(msg,"Error closing channel %d\n\n",channel);
    logMsg(msg,0,0,0,0,0,0);
  }
  fd_num[channel] = ERROR;
E 11
I 11
  printf(" ERROR=%d(d),0x%x,%c(c),%s(s);   \n",ERROR,ERROR,ERROR,ERROR);
  printf("    OK=%d(d),0x%x,%c(c),%s(s);   \n",OK,OK,OK,OK);
E 11
}

D 11
#endif
E 11



D 11
/******* this command is initialize the space frame Mask *******************/
E 11
I 11
/* this command is initialize the space frame Mask */
E 11
void
ini4()
{
  open_port(4);
 
  taskDelay(100);
D 11
  move_mask(4,1,-20000); 
E 11
I 11
  move_mask(4,-20000); 
E 11
 
  taskDelay(200);
  printf("Starting Mask move.. \n");
D 11
  move_mask(4,1,6000);
E 11
I 11
  move_mask(4,6000);
E 11
  taskDelay(200);
}

D 11
/******* this command is control the space frame Mask **********************/
E 11
I 11
/* this command is control the space frame Mask */
E 11
void
Mask4()
{
  int i;
  
  open_port(4);
  printf("Moving Mask other position.. \n");

  taskDelay(100);

D 11
  move_mask(4,1,1);
E 11
I 11
  move_mask(4,1);
E 11

  taskDelay(200);

  for(i=1; i<=4000; ++i)
  {
D 11
    move_mask(4,1,1);    
E 11
I 11
    move_mask(4,1);    
E 11
    printf("step %d has been moved \n",i);
    taskDelay(200);
  }

  printf("Stopping Mask move.. \n");
}

D 11
/******* this command is initialize the Forward Carriage Mask *******************/
E 11
I 11
/* this command is initialize the Forward Carriage Mask */
E 11
void
ini1()
{
  open_port(1);
 
  taskDelay(100);
D 11
  move_mask(1,1,-20000); 
E 11
I 11
  move_mask(1,-20000); 
E 11
 
  taskDelay(200);
  printf("Starting Mask move.. \n");
D 11
  move_mask(1,1,170);
E 11
I 11
  move_mask(1,170);
E 11
  taskDelay(200);
}

D 11
/****** this command is control the forward carriage Mask ********************/
E 11
I 11
/* this command is control the forward carriage Mask */
E 11
void
Mask1()
{
  int i;
  
  open_port(1);
  printf("Moving Mask other position.. \n");

  taskDelay(100);

D 11
  move_mask(1,1,1);
E 11
I 11
  move_mask(1,1);
E 11

  taskDelay(400);

  for(i=1; i<=750; i++)
  {
D 11
    move_mask(1,1,1);
E 11
I 11
    move_mask(1,1);
E 11
    taskDelay(400);
    printf("step %d has been moved \n",i);
  }
  printf("Stopping Mask move.. \n");
}

D 11
/******* this command is initialize back Mask *******************/
E 11
I 11
/* this command is initialize back Mask */
E 11
void
ini(int id)
{
  open_port(id);
 
  taskDelay(100);
D 11
  move_mask(id,1,-20000); 
E 11
I 11
  move_mask(id,-20000); 
E 11
 
  taskDelay(200);
  printf("Moving Back position of Mask.. \n");
}
D 11



E 7
/*----------------------------------------------------------------------------*/

D 3
    int velmex_command(char *raw, int whichPort)

E 3
/*  sends argument to the RS232 Port
 *  returns error status (defined in SUCCESS and WRITE_ERROR)
 */
I 3
int
velmex_command(char *raw, int whichPort)
{           
  char output[140];
  int msgSize = 0;    /* Number of bytes to write */
  int bytesSent = 0;  /* Number of bytes written */
D 4
  nt SerialPort;          
E 4
I 4
  int SerialPort;          
E 4
  int iii = 0;
E 3

D 3
{
           
	char output[140];
	int msgSize = 0;		/* Number of bytes to write */
	int bytesSent = 0;		/* Number of bytes written */
        int SerialPort;          
        int iii=0;
        
E 3
I 3
D 5
 /*
  *  Write Message 
  */
E 3
   
E 5
I 5
  /* Write Message */

E 5
D 3
    /*
     *  Write Message 
     */
   
	SerialPort=fd_num[whichPort];
        iii=0;
        do {output[iii]= raw[iii]; ++iii;} while ( raw[iii]!=EOS );
        output[iii]= '\r'; output[++iii]= EOS;   
   
 	msgSize = strlen(output);
  
        printf(" send= %s \n", output);
            
	bytesSent = write(SerialPort, output, msgSize); 
E 3
I 3
  SerialPort = fd_num[whichPort];
E 3

D 3
	
	return(bytesSent);
E 3
I 3
  iii = 0;
  do
  {
    output[iii] = raw[iii];
    ++iii;
  } while (raw[iii] != EOS);

  output[iii] = '\r';
  output[++iii] = EOS;   

  msgSize = strlen(output);
  printf(" send= %s \n", output);
  bytesSent = write(SerialPort, output, msgSize);

  return(bytesSent);
E 3
}






E 11

E 1
