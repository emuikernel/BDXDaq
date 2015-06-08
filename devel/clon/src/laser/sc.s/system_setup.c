#ifndef VXWORKS

/*
 * system_setup.c - setup lasers, masks and filters.
 *
 *
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
 *
 *  return( 0) if setup done correctly,
 *  return(-1) if get unexpected response from ROC.
 *
 *
 *    modified by:   SP, June 2003
 *
 */

#include <stdio.h>
#include "init_checks.h"
#include "laser.h"

extern char prog_msg[1024];
extern char *rocname[];
extern int  chnlN[];


int
system_setup(int debug)
{
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
  ss_prog("    -----------------------------------------");
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

      if(set_mask_speed(chnlN[lsri],seq_mspeed[lsri]) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    set_mask_speed   (%d,%d)  return = 0",
		  chnlN[lsri], seq_mspeed[lsri]);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);

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
  
  return(0);
}

#else

void
system_setup_dummy()
{
  return;
}

#endif
