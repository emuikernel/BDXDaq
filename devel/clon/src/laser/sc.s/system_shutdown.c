
#ifndef VXWORKS

/*
 * system_shutdown.c - close motor ports and turn OFF lasers.
 *
 *
 * 1. Close motor ports.
 * 2. Turn OFF all used lasers:
 *      - turn power OFF;
 *      - turn gas OFF.
 *
 *  return( 0) if shutdown done correctly,
 *  return(-1) if get unexpected response from ROC.
 *
 *    SP, June 2003
 *
 */

#include <stdio.h>
#include "init_checks.h"
#include "laser.h"

extern char prog_msg[1024];
extern char *rocname[];
extern int  chnlN[];


int
system_shutdown(int debug)
{
  int lsri;
  

  /* 1. Close motor ports */
  ss_prog("  close motor ports");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(close_port(chnlN[lsri]) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    close_port(chnlN[laser#%d]) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }

  /* 2. Turn OFF all used lasers */
  /* 2.a. power OFF */
  ss_prog("  turn lasers power OFF");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(pwr_OFF(lsri) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    pwr_OFF(laser#%d) return = 0", lsri);
	  ss_prog(prog_msg);
	}
      }
      ELSE_Error(rocname[lsri]);
    }
  }
  
  /* 2.b. gas OFF */
  ss_prog("  turn lasers gas OFF");
  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      if(gas_OFF(lsri) == 0) {
	if(debug) {
	  sprintf(prog_msg,"    gas_OFF(laser#%d) return = 0", lsri);
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
system_shutdown_dummy()
{
  return;
}

#endif
