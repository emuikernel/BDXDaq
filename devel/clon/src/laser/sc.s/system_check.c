#ifndef VXWORKS

/*
 *  system_check.c - check the status of system.
 *
 *
 * 1. Get and check statuses for all the lasers:
 *      a) get input registers (show statuses)
 *          - check lasers enabling status (should be OFF),
 *          - check lasers power status (should be ON),
 *          - get lasers ready status (expecting READY).
 *      b) get output registers (show control lines states)
 *          - check gas control line level (should be ON).
 *
 * 2. Get and check trigger bit setting
 *
 * 3. Get and check coda configuration (request status from "clastrig2")
 *
 * 4. Get coda state (request status from EB)
 *
 * 5. Check lasers ready status and coda state
 *
 * Note: 2,3,4 are determined in configuration.txt
 *
 *  return( 0) if check done correctly and all systems are ready,
 *  return( 1) or return( 2) if some elements not yet ready
 *             (some laser not yet ready, or coda not yet active),
 *  return(-1) if get unexpected response from ROC,
 *  return(-2) if some laser still enabled after system_setup(),
 *  return(-3) if not all requested lasers powered after system_setup(),
 *  return(-4) if not for all requested lasers gas=ON after system_setup(),
 *  return(-5) if expected trigger bit not set,
 *  return(-6) if coda has unexpected configuration,
 *
 *
 *    modified by:   SP, 9-June-2003
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "init_checks.h"
#include "laser.h"

static char	*mysql_database = "clasrun";
static char	*session = "clasprod";

extern char prog_msg[1024];
extern char *rocname[];


int
system_check(int debug)
{
  int runnum;
  char *config;
  int  lsri;
  char str_tmp[200];
  int  flag_all_checked = 0;
  int  flag_coda_active = 0;
  int  trigger_bit;
  int  flag_set_dis  = 1;
  int  flag_set_pwr  = 1;
  int  flag_set_rdy  = 1;
  int  flag_ctrl_gas = 1;
  int  iregister[MAX_LSR_NO] = {0,0,0,0,0};
  int  oregister[MAX_LSR_NO] = {0,0,0,0,0};
  int  st_set_pwr[MAX_LSR_NO]  = {0,0,0,0,0};
  int  st_set_rdy[MAX_LSR_NO]  = {0,0,0,0,0};
  int  st_set_dis[MAX_LSR_NO]  = {0,0,0,0,0};
  int  st_ctrl_gas[MAX_LSR_NO] = {0,0,0,0,0};


  /* 1. Check status of all the lasers */
  /* 1.a. get input registers */
  if(debug) ss_prog("  get input registers");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      sprintf(str_tmp,input(lsri));
      if(strstr(str_tmp,"ERROR") == NULL) {
	if(debug) {
	  sprintf(prog_msg,"    input(laser#%d)  return = %s", lsri, str_tmp);
	  ss_prog(prog_msg);
	}
	iregister[lsri] = sbin_to_int(str_tmp);
      }
      ELSE_Error(rocname[lsri]);
    }
  }

  /* 1.b. get output registers */
  if(debug) ss_prog("  get output registers");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      sprintf(str_tmp,output(lsri));
      if(strstr(str_tmp,"ERROR") == NULL) {
	if(debug) {
	  sprintf(prog_msg,"    output(laser#%d) return = %s", lsri, str_tmp);
	  ss_prog(prog_msg);
	}
	oregister[lsri] = sbin_to_int(str_tmp);
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
  /* determine statuses and control lines states */
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      st_set_dis[lsri]  = !bit_TST(status_ena_bit[lsri],iregister[lsri]);
      st_set_pwr[lsri]  = bit_TST(status_pwr_bit[lsri],iregister[lsri]);
      st_set_rdy[lsri]  = bit_TST(status_rdy_bit[lsri],iregister[lsri]);
      st_ctrl_gas[lsri] = bit_TST(contrl_gas_bit[lsri],oregister[lsri]);
      flag_set_dis  = flag_set_dis  & st_set_dis[lsri];
      flag_set_pwr  = flag_set_pwr  & st_set_pwr[lsri];
      flag_set_rdy  = flag_set_rdy  & st_set_rdy[lsri];
      flag_ctrl_gas = flag_ctrl_gas & st_ctrl_gas[lsri];
    }
  }
  
  /* 1.a.1. check lasers enabling status (should be OFF) */
  if(debug) ss_prog("  check lasers enabling status");
  if(flag_set_dis != 1) {
    ss_prog("\n Error: All lasers should be disabled \n");
    for(lsri=1; lsri<=4; lsri++) {
      if(use_laser[lsri] == 1) {
	if(st_set_dis[lsri])
	  sprintf(prog_msg,"    status indicates laser#%d disabled", lsri);
	else
	  sprintf(prog_msg,"    status indicates laser#%d enabled", lsri);
	ss_prog(prog_msg);
      }
    }
    return(-2);
  }
  
  /* 1.a.2. check lasers power status (should be ON) */
  if(debug) ss_prog("  check lasers power status");
  if(flag_set_pwr != 1) {
    ss_prog("\n Error: Not all requested lasers are powered \n");
    for(lsri=1; lsri<=4; lsri++) {
      if(use_laser[lsri] == 1) {
	if(st_set_pwr[lsri])
	  sprintf(prog_msg,"    status indicates laser#%d power ON", lsri);
	else
	  sprintf(prog_msg,"    status indicates laser#%d power OFF", lsri);
	ss_prog(prog_msg);
      }
    }
    return(-3);
  }
  
  /* 1.b.1. check gas control line level (should be ON) */
  if(debug) ss_prog("  check gas control line level");
  if(flag_ctrl_gas != 1) {
    ss_prog("\n Error: Not for all requested lasers gas is ON \n");
    for(lsri=1; lsri<=4; lsri++) {
      if(use_laser[lsri] == 1) {
	if(st_ctrl_gas[lsri])
	  sprintf(prog_msg,"    laser#%d gas control line level ON", lsri);
	else
	  sprintf(prog_msg,"    laser#%d gas control line level OFF", lsri);
	ss_prog(prog_msg);
      }
    }
    return(-4);
  }
  
  /* 2. Get and check trigger bit setting */
  if(debug) ss_prog("  get and check trigger bit setting");
  command_execute("clastrig2","ts_control");
/* printf("SP0: command_get_result=%s;;\n", command_get_result()); */
  trigger_bit = strtoul(command_get_result(),NULL,16);
/* printf("SP1: trigger_bit=%d;;\n", trigger_bit); */
  trigger_bit = (trigger_bit >> bit_number) & 0x1;
/* printf("SP2: trigger_bit=%d;;\n", trigger_bit); */
  if(trigger_bit == 1) {
    if(debug) ss_prog("    trigger bit set correctly");
  }
  else {
    sprintf(prog_msg,"\n Error: Trigger bit %d not set \n", bit_number);
    ss_prog(prog_msg);
  return(-5);
  }
  
  /* 3. Get and check coda configuration (request status from "clastrig2") */
  if(debug) ss_prog("  get and check coda configuration");


  get_run_config(mysql_database, session, &runnum, &config);
  sprintf(str_tmp,config);
/* SB_debug: printf("Sergey: coda config -->%s<--\n",str_tmp); */

  if(strstr(str_tmp,configuration) != NULL) {
    if(debug) {
      sprintf(prog_msg,"    coda configuration is %s", str_tmp);
      ss_prog(prog_msg);
    }
  }
  else {
    sprintf(prog_msg,"\n Error: Wrong configuration \"%s\", should be \"%s\" \n",
	    str_tmp, configuration);
    ss_prog(prog_msg);
    return(-6);
  }
  
  /* 4. Get coda state (request status from EB) */
  if(debug) ss_prog("  get coda state");
  command_execute("clastrig2","rocStatus");
  sprintf(str_tmp,command_get_result());
/* SB_debug: printf("Sergey: coda status -->%s<--\n",str_tmp); */
  if(strstr(str_tmp,coda_state) != NULL) flag_coda_active = 1;
  else                                   flag_coda_active = 0;
  if(debug) {
    sprintf(prog_msg,"    coda state is %s", str_tmp);
    ss_prog(prog_msg);
  }
  
  /* 5. Check lasers ready status and coda state */
  ss_prog("  --------------------");
  if(flag_set_rdy == 1) ss_prog("  all lasers are ready");
  else {
    for(lsri=1; lsri<=4; lsri++) {
      if(use_laser[lsri] == 1) {
	if(st_set_rdy[lsri]) sprintf(prog_msg,"  laser#%d ready", lsri);
	else                 sprintf(prog_msg,"  laser#%d not yet ready", lsri);
	ss_prog(prog_msg);
      }
    }
  }
  if(flag_coda_active == 1) ss_prog("  coda state active");
  else                      ss_prog("  coda state not yet active");

  flag_all_checked = !flag_set_rdy + !flag_coda_active;

  return(flag_all_checked);
}

#else

void
system_check_dummy()
{
  return;
}

#endif
