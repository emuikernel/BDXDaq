h45860
s 00002/00002/00246
d D 1.6 10/03/18 14:12:36 sergpozd 7 6
c *** empty log message ***
e
s 00003/00009/00245
d D 1.5 10/03/18 14:00:05 sergpozd 6 5
c *** empty log message ***
e
s 00009/00000/00245
d D 1.4 10/03/16 14:25:04 sergpozd 5 4
c *** empty log message ***
e
s 00026/00004/00219
d D 1.3 08/12/02 20:39:43 boiarino 4 3
c *** empty log message ***
e
s 00192/00863/00031
d D 1.2 03/07/21 14:16:53 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/system_check.c
e
s 00894/00000/00000
d D 1.1 03/03/03 14:04:20 boiarino 1 0
c date and time created 03/03/03 14:04:20 by boiarino
e
u
U
f e 0
t
T
I 4
#ifndef VXWORKS

E 4
I 1
D 3
/********************************
 * 
 * system_check.c - check the status of system.  For some of the checks I will assume that the flags
 * reflect the correct state.  It may be important to reset a flag to zero if there is
 * problem so that the system check really verifies the status.
E 3
I 3
/*
 *  system_check.c - check the status of system.
E 3
 *
D 3
 *   DP init          flag_clon10_init     assume flag refects correc state
 *   check camac3     flag_camac3_ok
 *   check sc_laser1  flag_sc_laser1_ok
 *                      both flags are reset so the routine checks the crates.
 *                      if the crates cannot be reached they are rebooted
 *                      flags are set to 1    flag_rebooting_c3, flag_rebooting_sc 
 *                      if the crates are reached but not in the correct states the flags refect this( nothing done)
 *                 flag_rebooting_c3 (note the program will not reboot within 120 sec of previous reboot)
 *                 flag_rebooting_sc (note rebooting seems to reset all lasers)
E 3
 *
D 3
 * while the system is rebooting there are some errors printed but this program will return with all values
 * set to zero. The laser will all be reset so this is the appropriate status.
E 3
I 3
 * 1. Get and check statuses for all the lasers:
 *      a) get input registers (show statuses)
 *          - check lasers enabling status (should be OFF),
 *          - check lasers power status (should be ON),
 *          - get lasers ready status (expecting READY).
 *      b) get output registers (show control lines states)
 *          - check gas control line level (should be ON).
E 3
 *
D 3
 * even if the crate is hung and must be rebooted the status of the lasers is not
 * necesarily affected. I tested this an to my suprise the I?O registers seem to be reset
 * uppon a reboot ?. (For EC system output registers remain set as well as the input resgister
 * they cannot be read or written.)
 *   check laser status     lasers are only partially setup on init
 *                    flag_laser_gas[]
 *                    flag_laser_power[]
 *                    flag_laser_enable[]  
 *                    flag_laser_ok[]
 *                    flag_laser_all_ok   
E 3
I 3
 * 2. Get and check trigger bit setting
E 3
 *
D 3
 *                    flag_laser_enable[]   not done at init
E 3
I 3
 * 3. Get and check coda configuration (request status from "clastrig2")
E 3
 *
D 3
 *   check filter status (read postion should be 0 and should get ^, and O)
 *      request motor status "V"        since closing and opening RS232 port puts motors on/off-line 
 *                                      first I try to contact the motor. No response I assume port closed.
 *                                      check statsu of filter   R  ready, B busy, else problem
 *                      flag_filter   1 for okay   0 for problem
 *                      flag_done     1 for R 0  B  0   
E 3
I 3
 * 4. Get coda state (request status from EB)
E 3
 *
D 3
 *      open up the port 
 *                     flag_port_open[]   signfies the port has been opened
 *                                        if the ready returns nothing this is set to zero
 *                                        possible reason  power off, crate is havng problems
 *      check statsu "V"  of filter   R  ready, B busy, else problem
 *                      flag_filter   1 for R  0 problem
 *                      flag_done   1 for R  0  B
E 3
I 3
 * 5. Check lasers ready status and coda state
E 3
 *
D 3
 * on any move request the filter_moving flag will be set
 *                            -1 CCW move
 *                             1 CW request
 *                             0 cleared when system not busy
 *                             2 for loop request
 *                            
E 3
I 3
 * Note: 2,3,4 are determined in configuration.txt
E 3
 *
D 3
 *      request limit notification "O"  only responds with an O if it reaches the limit on a move
 *                                      subsequent postion requests do not seem to return O.
 *   check codat state (request status from EB)
 *                    flag_coda_go   (normaly set to 1 if active)
 *   check the configuration file (ask trigger module)
 *                    flag_config  (set to on if the config downloeaded is correct)
 *   check trigger bit set
 *                    flag_trigbit_ok
E 3
I 3
 *  return( 0) if check done correctly and all systems are ready,
 *  return( 1) or return( 2) if some elements not yet ready
 *             (some laser not yet ready, or coda not yet active),
 *  return(-1) if get unexpected response from ROC,
 *  return(-2) if some laser still enabled after system_setup(),
 *  return(-3) if not all requested lasers powered after system_setup(),
 *  return(-4) if not for all requested lasers gas=ON after system_setup(),
 *  return(-5) if expected trigger bit not set,
 *  return(-6) if coda has unexpected configuration,
E 3
 *
D 3
 **********************************/
E 3
I 3
 *
 *    modified by:   SP, 9-June-2003
 *
 */
E 3

#include <stdio.h>
#include <stdlib.h>
I 4

E 4
D 3
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>


/******* local includes  *******/
#include "laser.h"
E 3
#include "init_checks.h"
D 3
#include "flags.h"
E 3
I 3
#include "laser.h"
E 3

I 4
static char	*mysql_database = "clasrun";
static char	*session = "clasprod";

E 4
extern char prog_msg[1024];
I 3
extern char *rocname[];
E 3

I 3

E 3
int
system_check(int debug)
{
I 4
  int runnum;
  char *config;
E 4
D 3
  int i, j, k, l, m, n, jjj;
  int position = 0;
E 3
I 3
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
E 3

D 3
  /***** for trigger ***/
  int trigger_bits;
E 3

D 3
  char *plus, *minus;
  char strtmp[50];
E 3
I 3
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
E 3

D 3
  /****** variables for reading registers *****/
  int iregister[4]={0,0,0,0};
  int oregister[4]={0,0,0,0};
  int bits[16]={0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};

  /*   varialbes for the dp_cmd  command, returned buffer */
  char exename[111];
  char result[BUFFER_LENGTH];
  int success=1;                           

  /********************  execution ******************************/
  sprintf(prog_msg,"\n\n*****************starting system check********");
  ss_prog(prog_msg);

  /**  1    *******   initialize DP commands  ********/
  if(flag_clon10_init == 0)
  {
    sprintf(prog_msg,"initializing the connection with clon10 \n");
    if(debug)ss_prog(prog_msg);
    success=DP_cmd_init("clon10");
 
    if(success == 0)
    {
      sprintf(prog_msg,"initializing clon10 connection  success= %d \n\n",success);
      ss_prog(prog_msg);
      flag_clon10_init = 1; 
E 3
I 3
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
E 3
    }
D 3
    else
    {
      sprintf(prog_msg,"initializing failure      success= %d \n\n",success);
E 3
I 3
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
D 4
  command_execute("clastrig2","exec ts_control");
E 4
I 4
  command_execute("clastrig2","ts_control");
I 5
D 6

  printf("SP0: command_get_result=%s;;\n", command_get_result());

E 6
I 6
/* printf("SP0: command_get_result=%s;;\n", command_get_result()); */
E 6
E 5
E 4
  trigger_bit = strtoul(command_get_result(),NULL,16);
I 5
D 6

  printf("SP1: trigger_bit=%d;;\n", trigger_bit);

E 6
I 6
/* printf("SP1: trigger_bit=%d;;\n", trigger_bit); */
E 6
E 5
  trigger_bit = (trigger_bit >> bit_number) & 0x1;
I 5
D 6

  printf("SP2: trigger_bit=%d;;\n", trigger_bit);

E 6
I 6
/* printf("SP2: trigger_bit=%d;;\n", trigger_bit); */
E 6
E 5
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
D 4
  command_execute("clastrig2","clastrig2 cget -config");
  sprintf(str_tmp,command_get_result());
E 4
I 4


  get_run_config(mysql_database, session, &runnum, &config);
  sprintf(str_tmp,config);
D 7
  printf("Sergey: coda config -->%s<--\n",str_tmp);
E 7
I 7
/* SB_debug: printf("Sergey: coda config -->%s<--\n",str_tmp); */
E 7

E 4
  if(strstr(str_tmp,configuration) != NULL) {
    if(debug) {
      sprintf(prog_msg,"    coda configuration is %s", str_tmp);
E 3
      ss_prog(prog_msg);
D 3
      flag_clon10_init = 0;  
    } 
E 3
I 3
    }
E 3
  }
D 3
  /************* end of 1 DP init*********** */

  /******** check camac 3 *******************  */
  flag_camac3_ok=0;  /* reset the flag so that the crate is checked */
  if(use_camac3 == 0) 
  {
    sprintf(prog_msg,"bypass the camac3 stuff VME crate set to not use\n");
    if(debug) ss_prog(prog_msg);
    flag_camac3_ok=1;
E 3
I 3
  else {
    sprintf(prog_msg,"\n Error: Wrong configuration \"%s\", should be \"%s\" \n",
	    str_tmp, configuration);
    ss_prog(prog_msg);
    return(-6);
E 3
  }
D 3
  if(flag_camac3_ok == 0)
  {
    sprintf(exename,"status");
    sprintf(prog_msg,"\n\n%s \n",exename);
    if(debug) ss_prog(prog_msg);
    success = DP_cmd("camac3",exename,result,111);
     
    if(success == 0)
    {
      sprintf(prog_msg," successful execution \n buffer= %s \n",result);
	 if(debug) ss_prog(prog_msg);
	 if(strstr(result,camac3_state) != NULL) 
	   {
	     flag_camac3_ok=1;
	     flag_rebooting_c3=0;
	     sprintf(prog_msg," camac 3 in requested state crate OK \n\n");
	     ss_prog(prog_msg);
	   }
	 else 
	   {
	     sprintf(prog_msg," camac 3 reached but not in requested state \n\n");
	     ss_prog(prog_msg);
	     flag_camac3_ok=0;
	   }     
       }
     else
       {
	 sprintf(prog_msg,"unable to reach controller ");
	 ss_prog(prog_msg);
	 
	 /**********  execute a systems command    *********/
	 reboot("camac3",0);
	 flag_camac3_ok=0;
	 /************end of system request to reboot****************************/
	 
       }     
   }
 /******************* end of camac 3 check   *******************/
 /* check 3 sc_laser1  check   note sc required for any laser */
 /* all flags have format  sc_laser1  some commands are to sc-laser1 */

flag_sc_laser1_ok=0;  /* reset the flag so that the system is checked */  
if(use_sc_laser1 == 0) 
   {
     sprintf(prog_msg,"bypass the sc_laser stuff VME crate set to not use\n");
     if(debug) ss_prog(prog_msg);
     flag_sc_laser1_ok=1;
   }     
 if(flag_sc_laser1_ok == 0)
   {
     sprintf(exename,"status");
     sprintf(prog_msg,"\n\n%s \n",exename);
     if(debug) ss_prog(prog_msg);
     success = DP_cmd("sc-laser1",exename,result,111);
     
     if ( success == 0)
       {
	 sprintf(prog_msg," sucessful excecution \n buffer= %s \n",result);
	 if(debug) ss_prog(prog_msg);
	 if(strstr(result,sc_laser1_state) != NULL) 
	   {
	     flag_sc_laser1_ok=1;
	     flag_rebooting_sc=0;
	     sprintf(prog_msg," sc-laser1 3 in requested state crate OK \n\n");
	     ss_prog(prog_msg);
	   }
	 else 
	   {
	     sprintf(prog_msg," camac 3 reached but not in requested state \n\n");
	     ss_prog(prog_msg);
	     flag_sc_laser1_ok=0;
	   }     
       }
     else
       {
	 sprintf(prog_msg,"unable to reach controller ");
	 ss_prog(prog_msg);
	 
	 /**********  execute a systems command    *********/
	 reboot("sc-laser1",0);
	 flag_sc_laser1_ok=0;
	 /************end of system request to reboot****************************/
	 
       }     
   }
 /******************* end of sc_laser1 check   *******************/
 
/*************** check status of all the lasers  **************************/

/********get registers ****************************  
 * input registers give the status
 * output registers show the lines that are set for laser control  
 * if  the camac crate is unreachable the registers all return 0
 ********************/
 
 k=0;
 j=0;
 while(j < 4)
   {
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(j > 3) break;
     
     sprintf(exename,"exec input(%d)",j+1);
     sprintf(prog_msg,"%s \n",exename);
     if(debug) ss_prog(prog_msg);
     if(j+1 == 1)
       {
	 DP_cmd("camac3",exename,result,111); 
	 sprintf(prog_msg," camac3 laser %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 laser %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     iregister[j]=sbin_to_int(result,0);
     if(debug)
       {
	 sprintf(prog_msg," register =%d\n",iregister[j]);
	 ss_prog(prog_msg);
       }
     
     j++;
   }
 
 j=0;
 while(j < 4)
   {
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(j > 3) break;
     
     sprintf(exename,"exec output(%d)",j+1);
     sprintf(prog_msg,"%s \n",exename);
     if(debug) ss_prog(prog_msg);
     
     if(j+1 == 1)
       {
	 DP_cmd("camac3",exename,result,111); 
	 sprintf(prog_msg," camac3 laser %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 laser %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     sprintf(prog_msg,"%s \n",result);
     if(debug) ss_prog(prog_msg);
     
     oregister[j]=sbin_to_int(result,0);
     if (debug)
       {
	 sprintf(prog_msg,"register= %d\n",oregister[j]);
	 if(debug) ss_prog(prog_msg);
       }
     
     j++;
   }
 
 
 j=0;
 while(j < 4)
   {
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(j > 3) break;
     
     sprintf(prog_msg,"laser status for laser number = %d \n",j+1);
     if(debug) ss_prog(prog_msg);
     
     sprintf(prog_msg,"input register \n");
     if(debug) ss_prog(prog_msg);
     
     if(  bit_TST(status_pwr_bit[j],iregister[j],debug)  )
       {
	 flag_laser_power[j]=1; 
	 sprintf(prog_msg,"status indicates laser %d  power on ",j+1);
	 ss_prog(prog_msg);
       }
     else
       {	    
	 flag_laser_power[j]=0; 
	 sprintf(prog_msg,"status indicates laser %d power off ",j+1);
	 ss_prog(prog_msg);
       }
     
     
     if(  bit_TST(status_rdy_bit[j],iregister[j],debug)  )
       {
	 flag_laser_ready[j]=1;
	 sprintf(prog_msg,"status indicates laser %d ready ",j+1);
	 ss_prog(prog_msg);
       }
     else
       {
	 flag_laser_ready[j]=0;
	 sprintf(prog_msg,"status indicates laser %d not yet ready ",j+1);
	 ss_prog(prog_msg);
       }
     
     if(  bit_TST(status_ena_bit[j],iregister[j],debug)  )
       {
	 flag_laser_enable[j]=1;
	 sprintf(prog_msg,"status indicates laser %d  enabled ",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 flag_laser_enable[j]=0;
	 sprintf(prog_msg,"status indicates laser %d  disabled ",j+1);
	 ss_prog(prog_msg);
       }
     
     sprintf(prog_msg,"output register \n");
     if(debug) ss_prog(prog_msg);
     
     /**   for gas check that the hardware has the gas flow bit on no check of gas itself **/
     if(  bit_TST(contrl_gas_bit[j],oregister[j],debug)  )
       {
	 flag_laser_gas[j]=1;
	 sprintf(prog_msg,"gas control line %d  level on ",j+1);
	 ss_prog(prog_msg);
       }
     else
       {
	 flag_laser_gas[j]=0;
	 sprintf(prog_msg,"gas control line %d level off ",j+1);
	  ss_prog(prog_msg);
       }
     /**********  not using this to verify the status of the hardware   *****/
     if(  bit_TST(contrl_pwr_bit[j],oregister[j],debug)  )
       {
	 sprintf(prog_msg,"pwr control line %d level on ",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 sprintf(prog_msg,"pwr control line %d level off",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     sprintf(prog_msg," enable and disable are pulses so are not available as status\n\n\n");
     if(debug) ss_prog(prog_msg);
     
     j++;
     
   }
 /*******************/
 
 /***************  check filters  ******/
 j=0;
 while(j < 4)
   {
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(j > 3) break;
     
     /*******************************    command request status  *********************/
     /*** first check all filters assume they are ready if tehy are no need to open  ***/
     k=0;
     sprintf(exename,"exec velmex_command(\"V\",%d)",j+1);
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111); 
	 sprintf(prog_msg," camac3 send V %d command= %s \n",j+1,exename);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 send V %d command= %s  \n",j+1,exename);
	 if(debug) ss_prog(prog_msg);
       }	
     sprintf(prog_msg,"result of send V %s \n",result);
     if(debug) ss_prog(prog_msg); 
     /******************/
     
     /*******************************   command start   *********************/
     k=2;
     sprintf(exename,"exec read_buffer(%d)",j+1);
     sprintf(prog_msg," command= %s \n",exename);
     if(debug) ss_prog(prog_msg);  
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111);
	 sprintf(prog_msg," camac3 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     
     sprintf(prog_msg," finished read results= %s\n",result);
     if(debug) ss_prog(prog_msg);
     
     /***************************  command end **************************/
     /******************** process thereults of a read postion after V *******/
     
     /***  results
      *                R  ready response from V request
      *               B  busy  response from V request
      *               Oxxx  limit  response if O notifiaction set
      *               ^xxx  done  response 
      *		 +xxxx/-xxxx   location returned
      *  The results of a V statsu are simple one character responses
      *    therfore less than 5 long,  numeric values are longer than 5
      *
      *******************************************************************  */
     
     jjj=j;
     
   
     /***********  V response ****************/
     
     if( strstr(result,"R") != NULL  )
       {
	 flag_filter[jjj]=1;
	 flag_port_open[jjj]=1;
	 flag_motor_moving[jjj]=0;
	 flag_done[jjj]=1;
	 sprintf(prog_msg," motor ready %d ", j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     else if( strstr(result,"B") != NULL  )
       {
	 flag_motor_moving[jjj]=1;
	 flag_filter[jjj]=1;
	 flag_port_open[jjj]=1;
	 flag_done[jjj]=0;
	 sprintf(prog_msg," motor%d  busy ",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       } 
     else
       {
	 sprintf(prog_msg," motor %d not repsonding",j+1);
	 if(debug) ss_prog(prog_msg);
	 flag_port_open[jjj]=0;
	 flag_filter[jjj]=0;
       }
 

     /************* end or read response processing **********************/
     
     
     
     /* knowing the ports that are open we can now open othres  ***/
     
     if(!flag_port_open[j])
       {
	 /*******************************   command start   *********************/
	 sprintf(exename,"exec open_port(%d)",j+1);
	 sprintf(prog_msg, "%s \n",exename);
	 if(debug) ss_prog(prog_msg);
	 if(j == 0)
	   {
	     DP_cmd("camac3",exename,result,111); 
	     sprintf(prog_msg," camac3 open port %d\n",j+1);
	     if(debug) ss_prog(prog_msg);
	   }
	 else
	   {
	     DP_cmd("sc-laser1",exename,result,111);
	     sprintf(prog_msg," sc-laser1 laser open port %d\n",j+1);
	     if(debug) ss_prog(prog_msg);
	   }
	 flag_port_open[j]=1;
	 sprintf(prog_msg,"result of open= %s \n",result);
	 if(debug) ss_prog(prog_msg);
	 /***************************  command end **************************/
       }
     
     /****** check to make sure they are all open   ******/
     /*******************************    command request status  *********************/
     k=0;
     sprintf(exename,"exec velmex_command(\"V\",%d)",j+1);
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111); 
	 sprintf(prog_msg," camac3 send V %d command= %s \n",j+1,exename);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 send V %d command= %s  \n",j+1,exename);
	 if(debug) ss_prog(prog_msg);
       }	
     sprintf(prog_msg,"result of send V %s \n",result);
     if(debug) ss_prog(prog_msg); 
     /******************/
     
     /*******************************   command start   *********************/
     k=2;
     sprintf(exename,"exec read_buffer(%d)",j+1);
     sprintf(prog_msg," command= %s \n",exename);
     if(debug) ss_prog(prog_msg);  
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111);
	 sprintf(prog_msg," camac3 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     
     sprintf(prog_msg," finished read results= %s\n",result);
     if(debug) ss_prog(prog_msg);
     
     /***************************  command end **************************/
       /******************** process thereults of a read postion *******/
     
     /***  results
      *                R  ready response from V request
      *               B  busy  response from V request
      *               Oxxx  limit  response if O notifiaction set
      *               ^xxx  done  response 
      *		 +xxxx/-xxxx   location returned
      *  The results of a V statsu are simple one character responses
      *    therfore less than 5 long,  numeric values are longer than 5
      *
      *******************************************************************  */
     
     jjj=j;
     
   
     /***********  V response ****************/
     
     if( strstr(result,"R") != NULL  )
       {
	 flag_filter[jjj]=1;
	 flag_port_open[jjj]=1;
	 flag_motor_moving[jjj]=0;
	 flag_done[jjj]=1;
	 sprintf(prog_msg," motor %d  ready",j+1);
	 ss_prog(prog_msg);
	 
       }
     else if( strstr(result,"B") != NULL  )
       {
	 flag_motor_moving[jjj]=1;
	 flag_filter[jjj]=1;
	 flag_port_open[jjj]=1;
	 flag_done[jjj]=0;
	 sprintf(prog_msg," motor %d  busy ",j+1);
	 ss_prog(prog_msg);
	 
       } 
     else
       {
	 sprintf(prog_msg," motor %d  not repsonding",j+1);
	 ss_prog(prog_msg);
	 flag_port_open[jjj]=0;
	 flag_filter[jjj]=0;
       }
 

     /************* end or read response processing **********************/
     
       
     
     /*******************************    command request location  *********************/
     k=0;
     sprintf(exename,"exec velmex_command(\"Y\",%d)",j+1);
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111); 
	 sprintf(prog_msg," camac3 get postionlaser %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 get postion  %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
       }	
     sprintf(prog_msg,"%s \n",result);
     if(debug) ss_prog(prog_msg); 
     /******************/

     /*******************************   command start   *********************/
     k=2;
     sprintf(exename,"exec read_buffer(%d)",j+1);
     sprintf(prog_msg," command= %s \n",exename);
     if(debug) ss_prog(prog_msg);  
     if(j == 0)
       {
	 DP_cmd("camac3",exename,result,111);
	 sprintf(prog_msg," camac3 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     else
       {
	 DP_cmd("sc-laser1",exename,result,111);
	 sprintf(prog_msg," sc-laser1 read buff %d\n",j+1);
	 if(debug) ss_prog(prog_msg);
	 
       }
     
     sprintf(prog_msg," finished read results= %s\n",result);
     if(debug) ss_prog(prog_msg);
     
     /***************************  command end **************************/
     
     /******************** process thereults of a read postion *******/
     
     /***  results
      *                R  ready response from V request
      *               B  busy  response from V request
      *               Oxxx  limit  response if O notifiaction set
      *               ^xxx  done  response 
      *		 +xxxx/-xxxx   location returned
      *  The results of a V statsu are simple one character responses
      *    therfore less than 5 long,  numeric values are longer than 5
      *
      *******************************************************************  */
     
     jjj=j;
     
     /*      jjj=laser_no[j]-1;  */
     
     k = strlen(result);
     if(k > 5)      /* there must be a position */
       {
	 if( (minus=strstr(result,"-")) != NULL)
	   {
	     strcpy(strtmp,minus);
	     sscanf(strtmp,"%d", &position);
	   }
	 if( (plus=strstr(result,"+")) != NULL)
	   {
	     strcpy(strtmp,plus); 
	     sscanf(strtmp,"%d", &position);
	   }	 
	 if( (!(plus == NULL) ^ (minus == NULL)) )
	   {
	     sprintf(prog_msg," bad positon found for laser   \n",jjj+1);
	     if(debug) ss_prog(prog_msg);
	   }
	 if(flag_motor_moving[jjj] !=0)
	   {
	     flag_CCW_limit[jjj]=0;
	     flag_CW_limit[jjj]=0;
	     flag_motor_moving[jjj]=1;
	   }	
	 current_filter_position[jjj]=position;
	 sprintf(prog_msg," location %d =%d \n ",j+1,position);
	 ss_prog(prog_msg);
       }  /** postion processed **/
     
     if( strstr(result,"O") != NULL  )
       {
	 sprintf(prog_msg," found  limit when requsting status or postion ?? \n");
	 if(debug) ss_prog(prog_msg);
	 if(flag_motor_moving[jjj] == 1 )
	   {
	     flag_CCW_limit[jjj]=0;
	     flag_CW_limit[jjj]=1;
	     flag_port_open[jjj]=1;
	     flag_done[jjj]=1;
	     flag_filter[jjj]=1;  /*  filter ok */
	     flag_motor_moving[jjj]=0;
	   }
	 else if(flag_motor_moving[jjj] == -1 )
	   {
	     flag_CCW_limit[jjj]=1;
	     flag_CW_limit[jjj]=0;
	     flag_port_open[jjj]=1;
	     flag_done[jjj]=1;
	     flag_filter[jjj]=1;  /*  filter ok */
	     flag_motor_moving[jjj]=0;
	   }
	 /*  the motor could be looping 2, or stopped 0  don't know which limit */  
	 
       }	 
     if( strstr(result,"^") != NULL  ) 
       {
	 sprintf(prog_msg," found done on position read\n");
	 if(debug) ss_prog(prog_msg);
	 flag_port_open[jjj]=1;
	 flag_done[jjj]=1;
	 flag_filter[jjj]=1;  /*  filter ok */
	 flag_motor_moving[jjj]=0;
       }
 
  sprintf(prog_msg," laser %d flag_filter=%d ",jjj+1,flag_filter[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d flag_CCW_limit=%d ",jjj+1,flag_CCW_limit[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d flag_CW_limit=%d ",jjj+1,flag_CW_limit[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d flag_port_open=%d ",jjj+1,flag_port_open[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d flag_motor_moving=%d ",jjj+1,flag_motor_moving[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d flag_done=%d ",jjj+1,flag_done[jjj]);
  if(debug)ss_prog(prog_msg);
  sprintf(prog_msg," laser %d current_filter_position=%d ",jjj+1,current_filter_position[jjj]);
  if(debug)ss_prog(prog_msg);
 


E 3
  
D 3
     /************* end or read response processing **********************/     
     
     
     
     j++;
   }
 
 /******************  get coda state *********************/
 if(flag_coda_go == 0)
   {  
     sprintf(prog_msg," test EB coda should be in the state= %s \n",coda_state) ;
     if(debug) ss_prog(prog_msg);
     DP_cmd("EB1","EB1 status",result,111);
     sprintf(prog_msg,"  event builder is %s \n",result);
     if(debug) ss_prog(prog_msg);
     if(strstr(result,coda_state) != NULL) 
       {
	 flag_coda_go=1; 
	 sprintf(prog_msg," coda found in the go state OK \n\n");
	 if(debug) ss_prog(prog_msg);
       }
     else 
       {
	 sprintf(prog_msg," coda in another transistion current state= %s ",result);
	 ss_prog(prog_msg);
	 sprintf(prog_msg," coda should be in the state= %s ",coda_state);
	 ss_prog(prog_msg);
	 flag_coda_go=0;
       } 
   }
   /*****************  EB state returned ******/
  if(flag_config ==0)
  {
     DP_cmd("clastrig2","clastrig2 cget -config",result,111);
     sprintf(prog_msg,"%s \n",result);
     if(debug) ss_prog(prog_msg);
     if(strstr(result,configuration) != NULL) 
       {flag_config=1; 
       sprintf(prog_msg," trigger indicate the correct configuration OK ");
       ss_prog(prog_msg);
       }
     else 
       {
	 sprintf(prog_msg," trigger indicates wrong configuration not %s ", configuration);
	 ss_prog(prog_msg);
	 flag_config=0;
       } 
   }
  /******************  get trigger bit  **************/
  if(flag_trigbit_ok == 0)
  {
     DP_cmd("clastrig2","exec ts_control",result,111);	
     trigger_bits=strtoul(result,NULL, 16);  
     sprintf(prog_msg,"%x \n", trigger_bits);
     if(debug) ss_prog(prog_msg);
     flag_trigbit_ok = trigger_bits >> bit_number & 0x1;
     /* shift right bit_num times and compare with 1*/
     sprintf(prog_msg,"flag_trigbit_ok = %d \n",flag_trigbit_ok);
     if(debug)ss_prog(prog_msg);
     sprintf(prog_msg," check for triger bit %d flag_trigbit_ok = %d (1=set/0=not set) \n",bit_number,flag_trigbit_ok);
     ss_prog(prog_msg);
E 3
I 3
  /* 4. Get coda state (request status from EB) */
  if(debug) ss_prog("  get coda state");
D 4
  command_execute("EB1","EB1 status");
E 4
I 4
  command_execute("clastrig2","rocStatus");
E 4
  sprintf(str_tmp,command_get_result());
I 4
D 7
  printf("Sergey: coda status -->%s<--\n",str_tmp);
E 7
I 7
/* SB_debug: printf("Sergey: coda status -->%s<--\n",str_tmp); */
E 7
E 4
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
E 3

I 3
  flag_all_checked = !flag_set_rdy + !flag_coda_active;
E 3

D 3
   }
 /************************** report flags ************/
 
 if(debug)
   {
     sprintf(prog_msg,"REPORT state ofa all flags  ");
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_clon10_init = %d (DP init status) ",flag_clon10_init);
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_config = %d   (downloaded coda config match",flag_config);
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_coda_go = %d  (EB found usually in the active state) ",flag_coda_go);
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_camac3_ok = %d ",flag_camac3_ok);
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_sc_laser1_ok = %d ",flag_sc_laser1_ok);
     ss_prog(prog_msg);
     
     sprintf(prog_msg,"flag_trigbit_ok = %d requested bit =%d ",flag_trigbit_ok,bit_number);
     ss_prog(prog_msg);
     
     for(j=0;j<4;j++)
       {
	 sprintf(prog_msg,"flag_laser_gas %d = %d ",j+1,flag_laser_gas[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_laser_power %d = %d ",j+1,flag_laser_power[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_laser_ready %d = %d ",j+1,flag_laser_ready[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_laser_enable %d = %d ",j+1,flag_laser_enable[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_laser_ok %d = %d ",j+1,flag_laser_ok[j]);
	 ss_prog(prog_msg);
       }
     
     sprintf(prog_msg,"flag_laser_all_ok = %d ",flag_laser_all_ok);
     ss_prog(prog_msg);
     
     for(j=0;j<4;j++)
       {
	 sprintf(prog_msg,"flag_port_open %d = %d ",j+1,flag_port_open[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_filter %d = %d ",j+1,flag_filter[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_CW_limit %d = %d ",j+1,flag_CW_limit[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_CCW_limit %d = %d ",j+1,flag_CCW_limit[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_motor_moving %d = %d ",j+1,flag_motor_moving[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"flag_done %d = %d ",j+1,flag_done[j]);
	 ss_prog(prog_msg);
	 sprintf(prog_msg,"postion %d = %d ",j+1,current_filter_position[j]);
	 ss_prog(prog_msg);
       }
     
     sprintf(prog_msg,"flag_filter_all_ok = %d ",flag_filter_all_ok);
     ss_prog(prog_msg);
   }
 return(0);
E 3
I 3
  return(flag_all_checked);
E 3
}

I 4
#else

void
system_check_dummy()
{
  return;
}

#endif
E 4
E 1
