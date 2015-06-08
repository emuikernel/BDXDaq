#ifndef VXWORKS

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
char *rocname[]={"","camac1","sc-laser1","sc-laser1","sc-laser1"};

/* laser_number vs serial port channel number */
int  lsrN[]={0,1,2,3,4};

/* serial port channel number vs laser_number */
int  chnlN[]={0,1,2,3,4};

/* current mask position */
int  current_mpos[]={0,0,0,0,0};

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
  return(0/*DP_cmd_init("clon10")*/);
}

int
command_execute(char *roc, char *cmd)
{
  if(clon10_init != 1)  command_init();
  /*return(tcpClientCmd(roc,cmd,result));*/
  return(tcpClientDP(roc,cmd,result));
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
  sprintf(exename,"gas_ON(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

int
gas_OFF(int laser_number)
{
  res = -1;
  WHAT_laser_number;
  sprintf(exename,"gas_OFF(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

int
pwr_ON(int laser_number)
{
  res = -1;
  WHAT_laser_number;
  sprintf(exename,"pwr_ON(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

int
pwr_OFF(int laser_number)
{
  res = -1;
  WHAT_laser_number;
  sprintf(exename,"pwr_OFF(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

int
TOF_laser_enable(int laser_number)
{
  res = -1;
  WHAT_laser_number;
  sprintf(exename,"TOF_laser_enable(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

int
TOF_laser_disable(int laser_number)
{
  res = -1;
  WHAT_laser_number;
  sprintf(exename,"TOF_laser_disable(%d)",laser_number);
  return(command_execute(rocname[laser_number],exename));
}

char *
output(int laser_number)
{
  char *res;
  
  res = error_str;
  WHAT_laser_number;
  sprintf(exename,"output(%d)",laser_number);
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
  sprintf(exename,"input(%d)",laser_number);
/* SB_debug: printf("input>%s<\n",exename); */
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
  sprintf(exename,"open_port(%d)",channel);
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
close_port(int channel)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"close_port(%d)",channel);
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
nullify_motors(int channel)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"nullify_motors(%d)",channel);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
set_mask_speed(int channel, int speed)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"set_mask_speed(%d,%d)",channel,speed);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
set_filter_speed(int channel, int speed)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"set_filter_speed(%d,%d)",channel,speed);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
get_mask_position(int channel)
{
  res = -12345678;
  WHAT_channel_number;
  sprintf(exename,"get_mask_position(%d)",channel);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%d", &res);
  if(res != -12345678)  current_mpos[lsrN[channel]] = res;
  return(res);
}

int
get_filter_position(int channel)
{
  res = -12345678;
  WHAT_channel_number;
  sprintf(exename,"get_filter_position(%d)",channel);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%d", &res);
  return(res);
}

int
move_mask(int channel, int step)
{
  res = -1;
  WHAT_channel_number;
/* SB_debug: printf("-> %d %d\n",channel,step); */
  sprintf(exename,"move_mask(%d,%d)",channel,step);
/* SB_debug: printf("strs: >%s< >%s<\n",rocname[lsrN[channel]],exename); */
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
/* SB_debug: printf("-> %d %d\n",channel,step); */
/* SB_debug: printf("result: >%s<\n",result); */
  sscanf(result, "%*s %*s %d", &res);
/* SB_debug: printf(">%s< %d", result,res); */
  return(res);
}

int
move_filter(int channel, int step)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"move_filter(%d,%d)",channel,step);
  if(command_execute(rocname[lsrN[channel]],exename) != 0) return(res);
  sscanf(result, "%*s %*s %d", &res);
  return(res);
}

int
loop_filter(int channel)
{
  res = -1;
  WHAT_channel_number;
  sprintf(exename,"loop_filter(%d)",channel);
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
  if((p0-4 <= p2) && (p2 <= p0+4)) {
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
  sprintf(exename,"init_set(%d)",channel);
  return(command_execute(rocname[lsrN[channel]],exename));
}

int
init_ctrl(int channel)
{
  int m1=0, m2=0;
  int f1=0, f2=0;
  
  /* Init start */
  /*  printf("     -----------------------------------------  \n"); */
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
  printf("\n     stop mask and filter moving! \n");
  
  /* Check final positions */
  if(move_mask(channel,-1)   != 'O') return(-1);
/* SB_debug: printf("1\n"); */
  if(move_filter(channel,-1) != 'O') return(-2);
/* SB_debug: printf("2\n"); */

  /* Nullify indexes for mask and filter motors */
  printf("     nullify indexes for mask and filter motors \n");
  if(nullify_motors(channel) != 0)   return(-3);
  current_mpos[lsrN[channel]] = 0;

  printf("     -----------------------------------------  \n");
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
  return(command_execute("clastrig2","pause_run"));
}

int
coda_resume()
{
  return(command_execute("clastrig2","resume_run"));
}


#else /* dummy vxworks */

void
TOF_laser_library_dummy()
{
  return;
}

#endif
