h51306
s 00001/00000/00136
d D 1.4 10/03/18 14:31:13 sergpozd 5 4
c *** empty log message ***
e
s 00011/00000/00125
d D 1.3 08/12/02 20:39:28 boiarino 4 3
c *** empty log message ***
e
s 00106/00438/00019
d D 1.2 03/07/21 14:17:09 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:22 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/system_setup.c
e
s 00457/00000/00000
d D 1.1 03/03/03 14:04:21 boiarino 1 0
c date and time created 03/03/03 14:04:21 by boiarino
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
E 3
I 3
/*
 * system_setup.c - setup lasers, masks and filters.
E 3
 *
D 3
 * system_setup.c - Most of the flags that reflect status will be set based on system check.
 *  you request an enable but you don't set the flag. youmust check to see that it
 *  is aenabled. (same with power and gas)
 *  gets a little tricky for the motors. I will let the done and filter flags 
 *  be set only in the check. Despite a move request I will not set these flags.
 *  i will set the motor_moving and the CCW limit flags. CCW limits are important 
 *  because you can' test for them randomly. You need to mve to see if your at limit
 *  therfore they must be set when you request the move
E 3
 *
D 3
 *   A CHECK SHOUL PROCEED EVERY SETUP !!!! except the first one.
E 3
I 3
 * 1. Turn ON all lasers selected in configuration.txt file
 *    to the point from where they can be easily enabled,
 *    that means power and gas should be ON for these lasers:
 *      - turn gas ON;
 *      - turn power ON.
 * 2. Disable all lasers, only 1 laser will be enabled and used
 *    at an appropriate moment.
 * 3. Initialize masks and filters:
 *      - set limit switches ON;
 *      - put masks and filters to the CCW limits;
 *      - nullify mask's and filter's motors.
 * 4. Set masks and filters speeds.
E 3
 *
D 3
 * setup consists of
E 3
I 3
 *  return( 0) if setup done correctly,
 *  return(-1) if get unexpected response from ROC.
E 3
 *
D 3
 * intializing the DP connection. This is done in several places so that the routines
 *     can work independently.
E 3
 *
D 3
 * powering all lasers up to the point that they can be enabled
 *      power and gas only based on the single gas/power flag  (Also use_laser must be set)
 *       
 * disable any laser that is enable (emable flage) is not part of the current seq
 *      
 * enable since only 1 laser is used at a time we will enable based on the seq number
 *      check enable flag, seq #(not 0), and the the laser for seq is set only.
E 3
I 3
 *    modified by:   SP, June 2003
E 3
 *
D 3
 * if the ports for the laser are not open open them
 *
 * putting all the intensity filters to the CCW limit and nulling (setting to zero)
 *        the motor position 
 *        check the done flag and the filter flag. If the motor is moving done should not be set.
 *
 * to keep the system simple idea is to make the setup respond to the current executing sequence
 * therefore based on the current seq the 
 *       laser will be enabled if not already enabled
 *       laser no longer current will be disabled
 *
 *
 **********************************/
/***** system includes **********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
E 3
I 3
 */
E 3

D 3

/******* local includes  *******/
#include "laser.h"
E 3
I 3
#include <stdio.h>
E 3
#include "init_checks.h"
D 3
#include "flags.h"
E 3
I 3
#include "laser.h"
E 3

extern char prog_msg[1024];
I 3
extern char *rocname[];
extern int  chnlN[];
E 3

I 3

E 3
int
system_setup(int debug)
{
D 3
  int i,j,k,l,m,n;
E 3
I 3
  int lsri;
  
  
  /* 1. Turn ON all lasers selected in configuration.txt file */
  /* 1.a. gas ON */
  ss_prog("  turn lasers gas ON");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(gas_ON(lsri) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    gas_ON(laser#%d) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
E 3

D 3
  /* tp is the integer number of seconds since 1/1/1970 
  via program Thu Jun  4 10:57:38 1998 numerical time= 896972258 */
  time_t tp1;
  time_t tp2;
E 3
I 3
  /* 1.b. power ON */
  ss_prog("  turn lasers power ON");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(pwr_ON(lsri) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    pwr_ON(laser#%d) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
  /* 2. Disable all lasers */
  ss_prog("  disable all lasers");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(TOF_laser_disable(lsri) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    TOF_laser_disable(laser#%d) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
  /* 3. Initialize masks and filters */
  ss_prog("  initialize masks and filters");
I 5
  ss_prog("    -----------------------------------------");
E 5
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(init_ctrl(chnlN[lsri]) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    init_ctrl(chnlN[laser#%d]) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
  /* 4. Set masks and filters speeds */
  ss_prog("  set masks and filters speeds");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
E 3

D 3
  int time_out = 0;
E 3
I 3
      if(set_mask_speed(chnlN[lsri],seq_mspeed[lsri]) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    set_mask_speed   (%d,%d)  return = 0",
		  chnlN[lsri], seq_mspeed[lsri]);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
E 3

D 3
  /* varialbes for the dp_cmd  command, returned buffer */
  char exename[111];
  char result[BUFFER_LENGTH];
  int success = 1;

  /********* string stuff******/
  size_t msgSize=0;


  /**  1    *******   initialize DP commands  ********/
  if(flag_clon10_init == 0)
   {
     sprintf(prog_msg,"initializing the connection with clon10 ");
     ss_prog(prog_msg);
     success=DP_cmd_init("clon10");
 
     if ( success == 0)
       {
	 sprintf(prog_msg,"initializing done     success= %d \n\n",success);
	 ss_prog(prog_msg);
	 flag_clon10_init = 1; 
       }
     else
       {
	 sprintf(prog_msg,"initializing failure      success= %d \n\n",success);
	 ss_prog(prog_msg);
	 flag_clon10_init = 0;  
       } 
   }
 /************* end of 1 DP init*********** */
 

 
 /***************** turn 4 lasers on  ********************/
 
 /******** laser gas on ****************************   */ 
 j=0;
 while(j <= 3)
   { 
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(use_laser[j] == 0) 	 j++;  /* skip this laser */
     if(j > 3) break;
     
     if(flag_laser_gas[j] == 0)
       {
	 sprintf(exename,"exec gas(%d,1)",j+1);
	 sprintf(prog_msg,"%s ",exename);
	 ss_prog(prog_msg);
	 if(j == 0)
	   {
	     success = DP_cmd("camac3",exename,result,111);
	   }
	 else
	   {
	     success = DP_cmd("sc-laser1",exename,result,111);
	   }
	 
	 
	 if ( success == 0)
	   {
	     sprintf(prog_msg," sucessful excecution  buffer= %s ",result); 
	     if(debug)ss_prog(prog_msg);
	   }
	 else
	   {
	     sprintf(prog_msg,"unable to reach controller "); 
	     ss_prog(prog_msg);
	     
	     /**********  execute a systems command    *********/
	     if(j == 0)
	       {
		 reboot("camac3",0);
		 flag_camac3_ok=0;
	       }
	     else
	       {
		 reboot("sc-laser1",0);
		 flag_sc_laser1_ok=0;
	       }	   
	     
	   }
       }
     
     if(flag_laser_power[j] == 0 )
       {
	 sprintf(exename,"exec pwr(%d,1)",j+1);
	 sprintf(prog_msg,"%s ",exename);
	 ss_prog(prog_msg);
	 if(j == 0)
	   {
	     success = DP_cmd("camac3",exename,result,111);
	   }
	 else
	   {
	     success = DP_cmd("sc-laser1",exename,result,111);
	   }
	 
	 
	 if ( success == 0)
	   {
	     sprintf(prog_msg," sucessful excecution  buffer= %s \n",result); 
	     if(debug)ss_prog(prog_msg);
	   }
	 else
	   {
	     sprintf(prog_msg,"unable to reach controller "); 
	     ss_prog(prog_msg);
	     
	     /**********  execute a systems command    *********/
	     if(j == 0)
	       {
		 reboot("camac3",0);
		 flag_camac3_ok=0;
	       }
	     else
	       {
		 reboot("sc-laser1",0);
		 flag_sc_laser1_ok=0;
	       }	   
	     
	   }
       }


    /******* go ahead an disable the laser if you are doing a different seq ************/
     if( ((flag_laser_enable[j] == 1)  && (j != seq_laser[current_seq]) ) || (current_seq ==0)) 
       {
	 sprintf(exename,"exec ena(%d,0)",j+1);
	 sprintf(prog_msg,"%s ",exename);
	 ss_prog(prog_msg);
	 if(j == 0)
	   {
	     success = DP_cmd("camac3",exename,result,111);
	   }
	 else
	   {
	     success = DP_cmd("sc-laser1",exename,result,111);
	   }
	 
	 
	 if ( success == 0)
	   {
	     sprintf(prog_msg," sucessful excecution  buffer= %s \n",result); 
	     if(debug)ss_prog(prog_msg);
	   }
	 else
	   {
	     sprintf(prog_msg,"unable to reach controller "); 
	     ss_prog(prog_msg);
	     
	     /**********  execute a systems command    *********/
	     if(j == 0)
	       {
		 reboot("camac3",0);
		 flag_camac3_ok=0;
	       }
	     else
	       {
		 reboot("sc-laser1",0);
		 flag_sc_laser1_ok=0;
	       }	   
	     
	   }
       }


     j++;
   }
 
   /*********************************end of lasers on *****************************/


 /********** motor set all motors to CW limit and NUll the postion pointer *****/


     k=1;
     j=0;
     while(j < 4)
       {
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(j > 3) break;


	 /* command  to open RS232 port for each laser ****************************   */  
	 if(!flag_port_open[j])
	   {
	     sprintf(exename,"exec open_port(%d)",j+1);
	     if(debug){ sprintf(prog_msg,"%s ",exename); ss_prog(prog_msg);}
	     if(j+1 == 1)
	       {
		 DP_cmd("camac3",exename,result,111); 
		 sprintf(prog_msg," camac3 laser port open %d",j+1);
		 ss_prog(prog_msg);
	       }
	     else
	       {
		 DP_cmd("sc-laser1",exename,result,111);
		 sprintf(prog_msg," sc-laser1 laser port open %d",j+1);
		 if(debug)ss_prog(prog_msg);
	       }
	     if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}
	     /***************************  command end of open port **************************/
	   }

	 if(current_seq ==0)     /***** only do this one time when the seq #=0 * can be repeated */
	   {
	     /*******************************   request limit notification    ***********/
	     k=30;
	     m=4;
	     sprintf(exename,"exec velmex_command(\"O1\",%d)",j+1);
	     if(debug)sprintf(prog_msg,"%s ",exename);
	     if(j+1 == 1)
	       {
		 DP_cmd("camac3",exename,result,111); 
		 sprintf(prog_msg," camac3 limit notify  req %d",j+1);
		 ss_prog(prog_msg);
	       }
	     else
	       {
		 DP_cmd("sc-laser1",exename,result,111);
		 sprintf(prog_msg," sc-laser1 limit notification req %d",j+1);
		 ss_prog(prog_msg);
	       }
	     if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}
	     
	     /***************************  command end limit note end **************************/
	   }


	 /*******************************   set speed to 30    ***********/
	 if(current_seq == 0)
	   {
	     k=30;
	   }
	 else
	   {
	     k=seq_speed[current_seq];
	   }
	 if(flag_filter[j] && flag_done[j])
	   {
	     m=4;
	     sprintf(exename,"exec send_command(%d,%d,%d)",j+1,m,k);
	     sprintf(prog_msg,"%s ",exename);
	      if(debug)ss_prog(prog_msg);
	     if(j+1 == 1)
	       {
		 DP_cmd("camac3",exename,result,111); 
		 sprintf(prog_msg," camac3 speed set to %d for  %d",k,j+1);
		 ss_prog(prog_msg);
	       }
	     else
	       {
		 DP_cmd("sc-laser1",exename,result,111);
		 sprintf(prog_msg," sc-laser1 speed set to %d  for %d",k,j+1);
		 ss_prog(prog_msg);
	       }
	     if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}
	     
	     /***************************  command set speed end **************************/
	   }
	 else
	   {
	     sprintf(prog_msg," filter busy or not ready can't set speed for laser %d",j+1);
	     ss_prog(prog_msg);
	   }

	 if(flag_filter[j] && flag_done[j])
	   {
	     sprintf(prog_msg," CCW limit flag  %d j=%d",flag_CCW_limit[j],j);
	     if(debug)ss_prog(prog_msg);
	     

	     if(flag_CCW_limit[j] == 0 )   /* not 0 then skip already at limit */
	       {
		 /*******************************   move CCW    ***********/
		 k=-1000;
		 flag_motor_moving[j]=-1;
		 m=2;
		 sprintf(exename,"exec send_command(%d,%d,%d)",j+1,m,k);
		 if(j+1 == 1)
		   {
		     DP_cmd("camac3",exename,result,111); 
		     sprintf(prog_msg," camac3 CCW limit step -1000 %d",j+1);
		     ss_prog(prog_msg);
		   }
		 else
		   {
		     DP_cmd("sc-laser1",exename,result,111);
		     sprintf(prog_msg," sc-laser1 CCW limit step -1000 %d",j+1);
		     ss_prog(prog_msg);
		   }
		 if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}
		 
		 /*************************** end mov CCW **************************/
	       }
	     else
	       {
		 sprintf(prog_msg," filter busy or not ready or already done; so can't go to CCW limit %d",j+1);
		 ss_prog(prog_msg);
		 sprintf(prog_msg," ready= %d,    done= %d",flag_filter[j],flag_done[j]);
		 ss_prog(prog_msg);
		 
	       }
	   }
	  j++;
	   }

     /* motor set all motors to CCW limit and NUll the postion pointer *****/
   /***************  check laser ******/
     k=1;
     j=0;
     while(j < 4)
       {
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(use_laser[j] == 0) 	 j++;  /* skip this laser */
	 if(j > 3) break;

	 if(flag_filter[j] && flag_done[j])
	   {
	     k=-1000;
	     m=2;
	     /**************************get time *****************/
	     tp1=time(NULL);
	     time_out=0;
	     while( flag_motor_moving[j]==-1 && flag_CCW_limit[j]==0  && !time_out)
	       {
		 sprintf(exename,"exec read_buffer(%d)",j+1);
		 if(j+1 == 1)
		   {
		     DP_cmd("camac3",exename,result,111); 
		     sprintf(prog_msg," camac3 read buffer %d",j+1);
		     if(debug)ss_prog(prog_msg);
		   }
		 else
		   {
		     DP_cmd("sc-laser1",exename,result,111);
		     sprintf(prog_msg," sc-laser1 read buffer %d",j+1);
		     if(debug) ss_prog(prog_msg);
		   }
		 if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}	    
		 l=0;
		 msgSize = strlen(result);
		 sprintf(prog_msg," mes size= %d ",msgSize);
		 if(debug)ss_prog(prog_msg);
		 if( strstr(result,"O") != NULL  ) 
		   {
		     flag_CCW_limit[j]=1;
		     flag_motor_moving[j]=0;
		     k=-1000;
		     m=2;
		     sprintf(exename,"exec null(%d)",j+1);
		     sprintf(prog_msg,"%s ",exename);
		     ss_prog(prog_msg);
		     if(j+1 == 1)
		       {
			 DP_cmd("camac3",exename,result,111); 
			 sprintf(prog_msg," camac3 zero position %d",j+1);
			 if(debug)ss_prog(prog_msg);
		       }
		     else
		       {
			 DP_cmd("sc-laser1",exename,result,111);
			 sprintf(prog_msg," sc-laser1 zero postion %d",j+1);
			 if(debug)ss_prog(prog_msg);
		       }
		     if(debug){sprintf(prog_msg,"%s ",result);ss_prog(prog_msg);}
		   }
		 tp2=time(NULL);
		 if( tp2 > (tp1+20) ) time_out=1;
		 if(time_out)
		   {
		     sprintf(prog_msg," CCW limit timeout for laser= %d",j+1);
		     ss_prog(prog_msg);
		   }
	       }
	   }
	     j++;
       }


E 3
I 3
      if(set_filter_speed(chnlN[lsri],seq_fspeed[lsri]) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    set_filter_speed (%d,%d)   return = 0",
		  chnlN[lsri], seq_fspeed[lsri]);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
E 3
  return(0);
}
I 3

I 4
#else

void
system_setup_dummy()
{
  return;
}

#endif
E 4
E 3
E 1
