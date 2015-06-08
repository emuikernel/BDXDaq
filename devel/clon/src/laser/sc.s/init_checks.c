#ifndef VXWORKS

/*
 * init_checks.c - reads configuration.txt and reset values of
 *                 variables assigned in the init_checks.h
 *
 *   return( 0) if all variables reset correctly,
 *   return(-1) if configuration.txt file can not be written,
 *   return(-2) if BEGIN_INIT not found,
 *   return(-3) if END_INIT   not found,
 *   return(-4) if BEGIN_PROCEDURE not found,
 *   return(-5) if END_PROCEDURE   not found.
 *
 * configuration.txt file should have the following format:
 *
 * ANY COMMENTS with "#" at the beginning of the line and
 * TWO BLOCKS for INIT and PROCEDURE:


BEGIN_INIT
value  [tab or space]  meaning  [tab or space]   comments
value  [tab or space]  meaning  [tab or space]   comments
          :
          :
END_INIT


BEGIN_PROCEDURE
value  [ ]  meaning  [ ]  meaning_index   [ ]   comments
value  [ ]  meaning  [ ]  meaning_index   [ ]   comments
          :
          :
END_PROCEDURE

 *
 *
 *    modified by:   SP, 30-May-2003
 *
 */


#include <stdio.h>
#include "init_checks.h"
#include "laser.h"

extern char prog_msg[1024];

int
init_checks(int debug)
{
  FILE *fp ;
  char fname[200];
  char str_tmp[200];
  char field[20];
  int  str_len=200;
  int  ch, lsrn, mposn;
  int  flag_init=0;
  int  flag_procedure=0;
  
  
  /* Read configuration file to set initial values */
  
  /* Get name of configuration file */
  sprintf(fname,"%s/TOF_config/configuration.txt", getenv("CLON_PARMS"));
  /*
    sprintf(fname,"/usr/local/clas/devel/source/laser/sc/s/configuration.txt");
    sprintf(fname,"%s/laser/sc/s/configuration.txt", getenv("CLON_SOURCE"));
    sprintf(fname,"/home/giovanet/TOF/c_cntl/config/configuration.txt");
  */
  if(debug) {
    sprintf(prog_msg,"  passed value debug = %d", debug);
    ss_prog(prog_msg);
  }
  
  /* Open configuration file */
  if((fp = fopen(fname,"r")) == NULL) {
    sprintf(prog_msg,"\n Error: Could not open %s \n\n", fname);
    ss_prog(prog_msg);
    return(-1);
  }
  
  /* Searching for BEGIN_INIT */
  while (!flag_init && ((ch = getc(fp)) != EOF)) {
    if(ch == '#' || ch == ' ' || ch == '\t') fgets(str_tmp,str_len,fp);
    else if(ch == '\n') {}
    else {
      ungetc(ch,fp);
      fgets(str_tmp,str_len,fp);
      if(strstr(str_tmp,"BEGIN_INIT") != NULL) {
	if(debug) {
	  sprintf(prog_msg,"  found BEGIN_INIT");
	  ss_prog(prog_msg);
	}
	flag_init=1;
      }
    }
  }
  if(flag_init == 0) return(-2);
  
  /* Read of INIT block */
  while (flag_init && ((ch = getc(fp)) != EOF)) {
    if(ch == '#') fgets(str_tmp,str_len,fp);
    else if(ch == '\n') {}
    else {
      ungetc(ch,fp);
      fgets(str_tmp,str_len,fp);
      if(strstr(str_tmp,"END_INIT") != NULL) {
	if(debug) {
	  sprintf(prog_msg,"  found END_INIT");
	  ss_prog(prog_msg);
	}
	flag_init=0;
      }
      else {
	sscanf(str_tmp,"%*s %s", field);
	if(strstr(field,"debug") != NULL) {
	  sscanf(str_tmp, "%d", &debug_value);
	  debug=debug_value;
	  if(debug) {
	    sprintf(prog_msg,"    init for debug = %d", debug);
	    ss_prog(prog_msg); }
	}
	else if(strstr(field,"configuration") != NULL) {
	  sscanf(str_tmp, "%s", configuration);
	  if(debug) {
	    sprintf(prog_msg,"    init for configuration = %s", configuration);
	    ss_prog(prog_msg); }
	}
	else if(strstr(field,"bit_number") != NULL) {
	  sscanf(str_tmp, "%d", &bit_number);
	  if(debug) {
	    sprintf(prog_msg,"    init for bit_number    = %d", bit_number);
	    ss_prog(prog_msg); }
	}
	else if(strstr(field,"coda_state") != NULL) {
	  sscanf(str_tmp, "%s", coda_state);
	  if(debug) {
	    sprintf(prog_msg,"    init for coda_state    = %s", coda_state);
	    ss_prog(prog_msg); }
	}
	else if(strstr(field,"laser#") != NULL) {
	  sscanf(field, "%*c%*c%*c%*c%*c%*c %d", &lsrn);
	  sscanf(str_tmp, "%d", &use_laser[lsrn]);
	  if(debug) {
	    sprintf(prog_msg,"    init for usage of laser#%d = %d",
		    lsrn, use_laser[lsrn]);
	    ss_prog(prog_msg); }
	}
      }
    }
  }
  if(flag_init == 1) return(-3);
  
  /* Searching for BEGIN_PROCEDURE */
  while (!flag_procedure && ((ch = getc(fp)) != EOF)) {
    if(ch == '#' || ch == ' ' || ch == '\t') fgets(str_tmp,str_len,fp);
    else if(ch == '\n') {}
    else {
      ungetc(ch,fp);
      fgets(str_tmp,str_len,fp);
      if(strstr(str_tmp,"BEGIN_PROCEDURE") != NULL) {
	if(debug) {
	  sprintf(prog_msg,"  found BEGIN_PROCEDURE");
	  ss_prog(prog_msg);
	}
	flag_procedure=1;
      }
    }
  }
  if(flag_procedure == 0) return(-4);
  
  /* Read of PROCEDURE block */
  while (flag_procedure && ((ch = getc(fp)) != EOF)) {
    if(ch == '#') fgets(str_tmp,str_len,fp);
    else if(ch == '\n') {}
    else {
      ungetc(ch,fp);
      fgets(str_tmp,str_len,fp);
      if(strstr(str_tmp,"END_PROCEDURE") != NULL) {
	if(debug) {
	  sprintf(prog_msg,"  found END_PROCEDURE");
	  ss_prog(prog_msg);
	}
	flag_procedure=0;
      }
      else {
	sscanf(str_tmp,"%*s %s %d %d", field, &lsrn, &mposn);
	if(use_laser[lsrn] == 1) {
	  if(strstr(field,"mask") != NULL) {
	    sscanf(str_tmp, "%d", &seq_mask[lsrn]);
	    if(debug) {
	      ss_prog("    --------------------------------");
	      sprintf(prog_msg,"    procedure for seq_mask[%d]    = %d",
		      lsrn, seq_mask[lsrn]);
	      ss_prog(prog_msg); }
	  }
	  else if(strstr(field,"mpos") != NULL) {
	    sscanf(str_tmp, "%d", &seq_mpos[lsrn][mposn]);
	    if(debug) {
	      sprintf(prog_msg,"    procedure for seq_mpos[%d][%d] = %d",
		      lsrn, mposn, seq_mpos[lsrn][mposn]);
	      ss_prog(prog_msg); }
	  }
	  else if(strstr(field,"mspeed") != NULL) {
	    sscanf(str_tmp, "%d", &seq_mspeed[lsrn]);
	    if(debug) {
	      sprintf(prog_msg,"    procedure for seq_mspeed[%d]  = %d",
		      lsrn, seq_mspeed[lsrn]);
	      ss_prog(prog_msg); }
	  }
	  else if(strstr(field,"fspeed") != NULL) {
	    sscanf(str_tmp, "%d", &seq_fspeed[lsrn]);
	    if(debug) {
	      sprintf(prog_msg,"    procedure for seq_fspeed[%d]  = %d",
		      lsrn, seq_fspeed[lsrn]);
	      ss_prog(prog_msg); }
	  }
	  else if(strstr(field,"floops") != NULL) {
	    sscanf(str_tmp, "%d", &seq_floops[lsrn]);
	    if(debug) {
	      sprintf(prog_msg,"    procedure for seq_floops[%d]  = %d",
		      lsrn, seq_floops[lsrn]);
	      ss_prog(prog_msg); }
	  }
	}
      }
    }
  }
  if(flag_procedure == 1) return(-5);
  
  return(0);
}

#else

void
init_checks_dummy()
{
  return;
}

#endif
