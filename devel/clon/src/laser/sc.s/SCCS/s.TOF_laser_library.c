h51712
s 00002/00001/00477
d D 1.8 10/03/18 14:31:31 sergpozd 9 8
c *** empty log message ***
e
s 00009/00010/00469
d D 1.7 10/03/18 14:13:14 sergpozd 8 7
c *** empty log message ***
e
s 00046/00024/00433
d D 1.6 08/12/02 20:40:17 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/00456
d D 1.5 04/08/12 12:12:02 sergpozd 6 5
c camac3 --> camac1
e
s 00001/00000/00456
d D 1.4 03/08/13 15:52:12 sergpozd 5 4
c added "current_mpos[lsrN[channel]] = 0" in init_ctrl()
e
s 00001/00001/00455
d D 1.3 03/08/12 17:09:44 sergpozd 4 3
c changed final filter position check to +/- 4
e
s 00004/00000/00452
d D 1.2 03/07/21 17:14:43 sergpozd 3 1
c added current_mpos[]
e
s 00000/00000/00000
d R 1.2 03/07/21 14:13:19 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/TOF_laser_library.c
e
s 00452/00000/00000
d D 1.1 03/07/21 14:13:18 sergpozd 1 0
c 
e
u
U
f b 
f e 0
t
T
I 7
#ifndef VXWORKS

E 7
I 1
/*  TOF_laser_library.c
 *
 *   UNIX library for TOF Laser
 *
 *   SP, June 2003
 */

#include <stdio.h>
#include "laser.h"

#define WHAT_laser_number \
  if(laser_number<1 || 4<laser_number) { \
    printf("\n Error: Wrong laser_number=%d \n\n", laser_number); \
    return(res); \
  }

#define WHAT_channel_number \
  if(channel!=1 && channel!=2 && channel!=3 && channel!=4) { \
    printf("\n Error: Wrong channel_number=%d \n\n", channel); \
    return(res); \
  }


/* rocname vs laser_number */
D 6
char *rocname[]={"","camac3","sc-laser1","sc-laser1","sc-laser1"};
E 6
I 6
char *rocname[]={"","camac1","sc-laser1","sc-laser1","sc-laser1"};
E 6

/* laser_number vs serial port channel number */
int  lsrN[]={0,1,2,3,4};

/* serial port channel number vs laser_number */
int  chnlN[]={0,1,2,3,4};

I 3
/* current mask position */
int  current_mpos[]={0,0,0,0,0};

E 3
static char error_str[]="ERROR";
static char result[BUFFER_LENGTH];
static char exename[200];
static int  clon10_init=0;
static int  timeout=5;      /* usually was 111 */
static int  res=-1;


int
command_init()
{
  clon10_init=1;
D 7
  return(DP_cmd_init("clon10"));
E 7
I 7
  return(0/*DP_cmd_init("clon10")*/);
E 7
}

int
command_execute(char *roc, char *cmd)
{
  if(clon10_init != 1)  command_init();
D 7
  return(DP_cmd(roc,cmd,result,timeout));
E 7
I 7
  /*return(tcpClientCmd(roc,cmd,result));*/
  return(tcpClientDP(roc,cmd,result));
E 7
}

char *
command_get_result()
{
  return(result);
}

void
command_print_result()
{
  printf("%s\n",result);
}

/********************************************************************/
/*                                                                  */
/*   UNIX functions equivalent to VxWorks functions in laser.c      */
/*                                                                  */
/********************************************************************/

int
gas_ON(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec gas_ON(%d)",laser_number);
E 7
I 7
  sprintf(exename,"gas_ON(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

int
gas_OFF(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec gas_OFF(%d)",laser_number);
E 7
I 7
  sprintf(exename,"gas_OFF(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

int
pwr_ON(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec pwr_ON(%d)",laser_number);
E 7
I 7
  sprintf(exename,"pwr_ON(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

int
pwr_OFF(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec pwr_OFF(%d)",laser_number);
E 7
I 7
  sprintf(exename,"pwr_OFF(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

int
TOF_laser_enable(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec TOF_laser_enable(%d)",laser_number);
E 7
I 7
  sprintf(exename,"TOF_laser_enable(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

int
TOF_laser_disable(int laser_number)
{
  res = -1;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec TOF_laser_disable(%d)",laser_number);
E 7
I 7
  sprintf(exename,"TOF_laser_disable(%d)",laser_number);
E 7
  return(command_execute(rocname[laser_number],exename));
}

char *
output(int laser_number)
{
  char *res;
  
  res = error_str;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec output(%d)",laser_number);
E 7
I 7
  sprintf(exename,"output(%d)",laser_number);
E 7
  if(command_execute(rocname[laser_number],exename) == 0)
    return(result);
  else
    return(res);
}

char *
input(int laser_number)
{
  char *res;
  
  res = error_str;
  WHAT_laser_number;
D 7
  sprintf(exename,"exec input(%d)",laser_number);
E 7
I 7
  sprintf(exename,"input(%d)",laser_number);
D 8
  printf("input>%s<\n",exename);
E 8
I 8
/* SB_debug: printf("input>%s<\n",exename); */
E 8
E 7
  if(command_execute(rocname[laser_number],exename) == 0)
    return(result);
  else
    return(res);
}

/********************************************************************/
/*                                                                  */
/*   UNIX functions equivalent to VxWorks functions in motor.c      */
/*                                                                  */
/********************************************************************/

int
open_port(int channel)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec open_port(%d)",channel);
E 7
I 7
  sprintf(exename,"open_port(%d)",channel);
E 7
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
close_port(int channel)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec close_port(%d)",channel);
E 7
I 7
  sprintf(exename,"close_port(%d)",channel);
E 7
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
nullify_motors(int channel)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec nullify_motors(%d)",channel);
E 7
I 7
  sprintf(exename,"nullify_motors(%d)",channel);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
set_mask_speed(int channel, int speed)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec set_mask_speed(%d,%d)",channel,speed);
E 7
I 7
  sprintf(exename,"set_mask_speed(%d,%d)",channel,speed);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
set_filter_speed(int channel, int speed)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec set_filter_speed(%d,%d)",channel,speed);
E 7
I 7
  sprintf(exename,"set_filter_speed(%d,%d)",channel,speed);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
get_mask_position(int channel)
{
  res = -12345678;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec get_mask_position(%d)",channel);
E 7
I 7
  sprintf(exename,"get_mask_position(%d)",channel);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%d", &res);
I 3
  if(res != -12345678)  current_mpos[lsrN[channel]] = res;
E 3
  return(res);
}

int
get_filter_position(int channel)
{
  res = -12345678;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec get_filter_position(%d)",channel);
E 7
I 7
  sprintf(exename,"get_filter_position(%d)",channel);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%d", &res);
  return(res);
}

int
move_mask(int channel, int step)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec move_mask(%d,%d)",channel,step);
E 7
I 7
D 8
printf("-> %d %d\n",channel,step);
E 8
I 8
/* SB_debug: printf("-> %d %d\n",channel,step); */
E 8
  sprintf(exename,"move_mask(%d,%d)",channel,step);
D 8
printf("strs: >%s< >%s<\n",rocname[lsrN[channel]],exename);
E 8
I 8
/* SB_debug: printf("strs: >%s< >%s<\n",rocname[lsrN[channel]],exename); */
E 8
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
I 7
D 8
printf("-> %d %d\n",channel,step);
printf("result: >%s<\n",result);
E 8
I 8
/* SB_debug: printf("-> %d %d\n",channel,step); */
/* SB_debug: printf("result: >%s<\n",result); */
E 8
E 7
  sscanf(result, "%*s %*s %d", &res);
I 7
D 8
printf(">%s< %d", result,res);
E 8
I 8
/* SB_debug: printf(">%s< %d", result,res); */
E 8
E 7
  return(res);
}

int
move_filter(int channel, int step)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec move_filter(%d,%d)",channel,step);
E 7
I 7
  sprintf(exename,"move_filter(%d,%d)",channel,step);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
loop_filter(int channel)
{
  res = -1;
  WHAT_channel_number;
D 7
  sprintf(exename,"exec loop_filter(%d)",channel);
E 7
I 7
  sprintf(exename,"loop_filter(%d)",channel);
E 7
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
move_mask_ctrl(int channel, int step)
{
  int  p0, p1, p2, ii=0;
  
  /* Get current mask position */
  p0 = get_mask_position(channel);
  if(p0 == -12345678) {
    printf("\n Error: Can't get mask position for laser#%d \n\n",
	   lsrN[channel]);
    return(-1);
  }
  printf("     current laser#%d mask position = %d \n", lsrN[channel], p0);
  
  /* Start mask move */
  p1 = move_mask(channel,step);
  if(p1 != 0) {
    printf("\n Error: Can't move mask. \n\n");
    if(p1 == 'O')
      printf("        Limit switch reached. \n\n");
    return(-1);
  }
  printf("     start mask move \n");
  
  /* Looking for mask moving */
  printf("       moving .");
  fflush(stdout);
  p2 = p0;
  while(ii < 2) {
    sleep(1);
    printf(".");
    fflush(stdout);
    p1 = p2;
    p2 = get_mask_position(channel);
    if(p1 == p2) ii++;
    else ii=0;
  }
  printf("\n     stop mask move \n");
  
  /* Check final mask position */
  printf("     new laser#%d mask position = ", lsrN[channel]);
  if(((p2-p0) == step) || ((step == 0) && (p2 == 0))) {
    printf("%d, set correctly \n", p2);
    return(0);
  }
  else {
    if(step > 0) p1 =  1;
    else         p1 = -1;
    if(move_mask(channel,p1) == 'O') {
      printf("%d, limit switch was encountered \n", p2);
      return(1);
    }
    else {
      printf("%d, set unexpectedly \n", p2);
      return(-1);
    }
  }
}

int
loop_filter_ctrl(int channel)
{
  int  p0, p1, p2, ii=0;
  
  /* Get current filter position */
  p0 = get_filter_position(channel);
  if(p0 == -12345678) {
    printf("\n Error: Can't get filter position for laser#%d \n\n",
	   lsrN[channel]);
    return(-1);
  }
  printf("     current laser#%d filter position = %d \n", lsrN[channel], p0);
  
  /* Start filter loop */
  if(loop_filter(channel) != 0) {
    printf("\n Error: Can't loop filter. \n\n");
    return(-1);
  }
  printf("     start filter loop \n");
  
  /* Looking for filter moving */
  printf("       moving ... ");
  fflush(stdout);
  p2 = p0;
  while(ii < 2) {
    sleep(1);
    p1 = p2;
    p2 = get_filter_position(channel);
    if(p1 == p2) ii++;
    else ii=0;
    if(p1 < p2) { printf("\b. ");  fflush(stdout); }
    else        { printf("\b\b "); fflush(stdout); }
  }
  printf("\n     stop filter loop \n");
  
  /* Check final filter position */
  printf("     final laser#%d filter position = ", lsrN[channel]);
D 4
  if((p0-2 <= p2) && (p2 <= p0+2)) {
E 4
I 4
  if((p0-4 <= p2) && (p2 <= p0+4)) {
E 4
    printf("%d, loop done correctly \n", p2);
    return(0);
  }
  printf("%d, loop ended unexpectedly \n", p2);
  return(-1);
}

int
init_set(int channel)
{
  WHAT_channel_number;
D 7
  sprintf(exename,"exec init_set(%d)",channel);
E 7
I 7
  sprintf(exename,"init_set(%d)",channel);
E 7
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
init_ctrl(int channel)
{
  int m1=0, m2=0;
  int f1=0, f2=0;
  
  /* Init start */
D 9
  printf("     -----------------------------------------  \n");
E 9
I 9
  /*  printf("     -----------------------------------------  \n"); */
E 9
  printf("     init start for mask and filter of laser#%d \n", lsrN[channel]);
  if(init_set(channel) != 0) return(-111);
  printf("     start moving mask and filter to CCW limit switch \n");
  
  m2 = get_mask_position(channel);
  f2 = get_filter_position(channel);

  /* Looking for mask and filter moving */
  printf("     moving ...");
  while(!((m1 == m2) && (f1 == f2))) {
    m1 = m2;
    m2 = get_mask_position(channel);
    f1 = f2;
    f2 = get_filter_position(channel);
    printf(".");
    fflush(stdout);
  }
D 7
  printf("\n     stop mask and filter moving \n");
E 7
I 7
  printf("\n     stop mask and filter moving! \n");
E 7
  
  /* Check final positions */
  if(move_mask(channel,-1)   != 'O') return(-1);
I 7
D 8
printf("1\n");
E 8
I 8
/* SB_debug: printf("1\n"); */
E 8
E 7
  if(move_filter(channel,-1) != 'O') return(-2);
I 7
D 8
printf("2\n");
E 8
I 8
/* SB_debug: printf("2\n"); */
E 8
E 7

  /* Nullify indexes for mask and filter motors */
  printf("     nullify indexes for mask and filter motors \n");
  if(nullify_motors(channel) != 0)   return(-3);
I 5
  current_mpos[lsrN[channel]] = 0;
E 5
D 8
  
I 7
  printf("\n done \n");
E 8
I 8

I 9
  printf("     -----------------------------------------  \n");
E 9
E 8
E 7
  return(0);
}


/********************************************************************/
/*                                                                  */
/*   Functions came from separate UNIX files (now eliminated):      */
/*      ss_prog.c                                                   */
/*                                                                  */
/*                                                                  */
/********************************************************************/

/* was used to sends data to RTserever, now just prints */
int
ss_prog(char *prog_msg)
{
  printf(" %s\n",prog_msg);
  return(1);
}

/********************************************************************/
/*                                                                  */
/*   UNIX functions to operate coda                                 */
/*                                                                  */
/********************************************************************/

int
coda_pause()
{
D 7
  return(command_execute("clastrig2","exec pause_run"));
E 7
I 7
  return(command_execute("clastrig2","pause_run"));
E 7
}

int
coda_resume()
{
D 7
  return(command_execute("clastrig2","exec resume_run"));
E 7
I 7
  return(command_execute("clastrig2","resume_run"));
E 7
}

I 7

#else /* dummy vxworks */

void
TOF_laser_library_dummy()
{
  return;
}

#endif
E 7
E 1
