/*
 * TOF_laser.c - operate lasers for TOF calibration.
 *
 *    written by:    Kevin Giovanetti, 1999-2000
 *
 *    modified by:   SP, June 2003
 *
 *
 * Code will be started by the DAQ for calibration runs.
 * DAQ will operate independently of this code and will
 * collect data generated by these lasers. If the code
 * crashes or functions improperly DAQ should not be affected.
 *
 * Logic of this code:
 *
 * 1. Get initial values for the checks and procedure:
 *      - read configuration file configuration.txt,
 *        get and set initial values;
 *      - check rationality of continuation.
 * 2. System setup:
 *      - turn ON (gas ON and power ON) all selected lasers
 *        to the point from where they can be easily enabled;
 *      - disable all lasers, only 1 laser will be enabled
 *        and used at an appropriate moment;
 *      - initialize masks and filters (set them to the CCW
 *        limits and nullify mask's and filter's motors);
 *      - set masks and filters speeds.
 * 3. System check:
 *      - get and check statuses for all the lasers;
 *      - get and check trigger bit setting;
 *      - get and check coda configuration;
 *      - get coda state;
 *      - check and return lasers ready status and coda state.
 * 4. Main execution:
 *      - main loop over all selected lasers;
 *      - inside laser_loop loop over all choosen mask
 *        positions selected in configuration.txt;
 *      - inside mask_loop for each mask positions activate
 *        n-times filter_loop, n get form configuration.txt;
 *      - for each step in mask_loop:
 *         . pause the run,
 *         . move mask at new position,
 *         . resume the run,
 *         . enable appropriate laser,
 *         . activate filter_loop n-times,
 *         . disable current laser.
 * 5. Shutdown:
 *      - close motor ports;
 *      - turn OFF all lasers.
 *
 */


#define LASER_CODA_ENABLE 1

#define INIT 1

#define TIMEOUT 600

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <strings.h>
#include "laser.h"
#include "init_checks.h"

extern int  chnlN[];
extern char *rocname[];
extern int  current_mpos[];

char prog_msg[1024]; /* message buffer; extern in subroutines */


int
main(int argc, char *argv[])
{
  time_t tp;
  int  tmpval;
  int  lsri, mski, fltri, i;
  int  step;
  
  
  /***********!!!!!!!!   Init, Setup and Check   !!!!!!!!***********/
  
  /* Get time */
  tp=time(NULL);
  sprintf(prog_msg,"\n TOF_laser \n\n Start time = %s \n", ctime(&tp));
  ss_prog(prog_msg);
  
  /* Get initial values for the checks and procedure:  */
  /* read configuration.txt                            */
  ss_prog("Read configuration.txt");
  tmpval = init_checks(debug_value);
  sprintf(prog_msg,"init_checks() return status = %d \n", tmpval);
  ss_prog(prog_msg);
  
  /* Check rationality of continuation */
  if(!use_laser[1] && !use_laser[2] && !use_laser[3] && !use_laser[4]) {
    ss_prog("All use_laser[i]=0,  Program stopped !!! \n");
    return(-1);
  }
  
  /* Begin system setup */
  ss_prog("Begin system setup");
  tmpval = system_setup(debug_value);
  sprintf(prog_msg,"system_setup() return status = %d \n", tmpval);
  ss_prog(prog_msg);
  if(tmpval != 0) {
    ss_prog("Some error occurred. Fix problem and restart program. \n");
    return(-1);
  }
  
  /* Begin system check */
  ss_prog("Begin system check");
  tmpval=10;
  while(tmpval != 0) {
    printf(" wait a few second ");
    for(i=0; i<10; i++) { printf("."); fflush(stdout); sleep(1); }
    printf("\n");
    tmpval = system_check(debug_value);
    sprintf(prog_msg,"system_check() return status = %d \n", tmpval);
    ss_prog(prog_msg);
    if(tmpval < 0) {
      ss_prog("Some error occurred. Fix problem and restart program. \n");
      return(-1);
    }
    if((time(NULL) - tp) > TIMEOUT) {
      sprintf(prog_msg,"SYSTEM_CHECK_TIMEOUT of %d seconds passed.\n",TIMEOUT);
      ss_prog(prog_msg);
      ss_prog("TIMEOUT EXIT: System still not ready for Main Execution. \n");
      return(-1);
    }
  }
  
  
  /***********!!!!!!!!   Execution   !!!!!!!!***********/
  ss_prog("Main Execution");

  for(lsri=1; lsri<=4; lsri++) {
    if(use_laser[lsri] == 1) {
      
      for(mski=0; mski<=seq_mask[lsri]; mski++) {

	ss_prog("  ---------------------------------------------");
	sprintf(prog_msg,"  main loop for laser#%d  mask's position = %d",
		lsri, seq_mpos[lsri][mski]);
	ss_prog(prog_msg);
	
#ifdef LASER_CODA_ENABLE
	/* pause the run */
	if(coda_pause() == 0) {
	  if(debug_value) ss_prog("  run paused");
	}
	ELSE_Error("clastrig2");
#endif

	/* move mask at new position */
	step = seq_mpos[lsri][mski] - current_mpos[lsri];
	/*
	printf("seq_mpos[lsri][mski] = %d;; \n", seq_mpos[lsri][mski]);
	printf("current_mpos[lsri]   = %d;; \n", current_mpos[lsri]);
	printf("step                 = %d;; \n", step);
	*/
	if(step != 0) {
	  ss_prog("  move mask at new position \n");
	  if(move_mask_ctrl(chnlN[lsri], step) < 0) {
	    ss_prog("\n Error: Can not set mask properly \n");
	    return(-1);
	  }
	}
	
#ifdef LASER_CODA_ENABLE
	/* resume the run */
	if(coda_resume() == 0) {
	  if(debug_value) ss_prog("  run resumed");
	}
	ELSE_Error("clastrig2");
	
	/* enable laser */
	if(TOF_laser_enable(lsri) == 0) {
	  if(debug_value) {
	    sprintf(prog_msg,"  laser#%d enabled", lsri);
	    ss_prog(prog_msg);
	  }
	}
	ELSE_Error(rocname[lsri]);
#endif
	
	/* activate filter loop */
	for(fltri=1; fltri<=seq_floops[lsri]; fltri++) {
	  ss_prog("  =============");
	  sprintf(prog_msg,"  filter loop#%d \n", fltri);
	  ss_prog(prog_msg);
	  if(loop_filter_ctrl(chnlN[lsri]) != 0) {
	    ss_prog("\n Error: Can not loop filter properly \n");
	    return(-1);
	  }
	}

	/* disable laser */
	if(TOF_laser_disable(lsri) == 0) {
	  if(debug_value) {
	    sprintf(prog_msg,"  laser#%d disabled", lsri);
	    ss_prog(prog_msg);
	  }
	}
	ELSE_Error(rocname[lsri]);
	
      } /* end of "for(mski=1; mski<=seq_mask[lsri]; mski++)" */

    } /* end of "if(use_laser[lsri] == 1)" */
  } /* end of "for(lsri=1; lsri<=4; lsri++)" */
  ss_prog("main execution done \n");
  
  
  /***********!!!!!!!!   Shutdown   !!!!!!!!***********/
  ss_prog("Begin system shutdown");
  
  tmpval = system_shutdown(debug_value);
  sprintf(prog_msg,"system_shutdown() return status = %d \n", tmpval);
  ss_prog(prog_msg);
  if(tmpval != 0) {
    ss_prog("Some error occurred. Do shutdown manually. \n");
    return(-1);
  }
  
  
  return(0);
}
