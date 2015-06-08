h31567
s 00011/00000/00231
d D 1.3 08/12/02 20:39:55 boiarino 4 3
c *** empty log message ***
e
s 00201/00284/00030
d D 1.2 03/07/21 14:16:14 sergpozd 3 1
c Completely new revision
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/init_checks.c
e
s 00314/00000/00000
d D 1.1 03/03/03 14:04:19 boiarino 1 0
c date and time created 03/03/03 14:04:19 by boiarino
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
 * init_checks.c - Prgoram reads a file an returns the values for
 * varibles in the init_checks.h.
 * If the file is not present and error return
 * will cause the default values to be used.
E 3
I 3
/*
 * init_checks.c - reads configuration.txt and reset values of
 *                 variables assigned in the init_checks.h
E 3
 *
D 3
 * FILE FORMAT   FILE FORMAT
E 3
I 3
 *   return( 0) if all variables reset correctly,
 *   return(-1) if configuration.txt file can not be written,
 *   return(-2) if BEGIN_INIT not found,
 *   return(-3) if END_INIT   not found,
 *   return(-4) if BEGIN_PROCEDURE not found,
 *   return(-5) if END_PROCEDURE   not found.
E 3
 *
D 3
 * The program will expect a specific format that will make it easier
 * to read and create the file.
E 3
I 3
 * configuration.txt file should have the following format:
E 3
 *
D 3
    
   HEADER COMMENT any lenght and is completely ignored
          :
          :
          :
E 3
I 3
 * ANY COMMENTS with "#" at the beginning of the line and
 * TWO BLOCKS for INIT and PROCEDURE:
E 3

D 3
BEGIN_INIT  string signifying first read portion 
value [tab or space]   comment describing the variable
value [tab or space]   comment
E 3
I 3

BEGIN_INIT
value  [tab or space]  meaning  [tab or space]   comments
value  [tab or space]  meaning  [tab or space]   comments
E 3
          :
          :
D 3
          :
E 3
END_INIT

I 3

E 3
BEGIN_PROCEDURE
I 3
value  [ ]  meaning  [ ]  meaning_index   [ ]   comments
value  [ ]  meaning  [ ]  meaning_index   [ ]   comments
E 3
          :
          :
D 3
          :
E 3
END_PROCEDURE

D 3
* once the BEGIN_INIT has been found each line must contain 
* the value for a specific predifined variable however if 
* the list is ended by the END_INT the program continues and
* uses the defaults for the remaining values
E 3
I 3
 *
 *
 *    modified by:   SP, 30-May-2003
 *
 */
E 3


D 3
 **********************************/

E 3
#include <stdio.h>
#include "init_checks.h"
#include "laser.h"

extern char prog_msg[1024];

int
init_checks(int debug)
{
D 3
  FILE *file_handle ;
  int counter=0;
  int next_char;
  int flag_search=0;
  char *input_filename;
  char word[150];
  int flag_init_start=0;
  int flag_procedure_start=0;
  int flag_init_end=0;
  int flag_procedure_end=0;
  int flag_eol=0;
  int flag_eof=0;
  int line_count;
  int save_line;

  int numberOfsettings=0;
  int setting_counter=0;
  int setting[500];
  int wait_setting[500];

  int processid= -10000;
  int errorid=0;
  int returnv=1;

  float real_no;
  char *tmpstr[50];
 
 /************** read configuration file to set initial values    ********/


 input_filename = "/usr/local/clas/parms/TOF_config/configuration.txt";
          /*  input_filename = "/home/giovanet/TOF/c_cntl/config/configuration.txt";  */
 sprintf(prog_msg," debug value passed %d",debug);
 ss_prog(prog_msg);
 
 
 /* returns !Null if can't open file ********************************/	  
 if ( !(file_handle = fopen(input_filename,"r"))) 
   {
     sprintf(prog_msg,"ERROR: Could not open %s",input_filename);
     ss_prog(prog_msg);
     return(-10010);
   }
 sprintf(prog_msg,"searching for BEGIN_INIT \n");
 if(debug) ss_prog(prog_msg);
 flag_search=0;
 line_count=1;
 flag_init_start=0;
 while (!flag_search && !flag_eof && !flag_init_end) 
   { 
     counter=0;
     flag_eol=0; 
     while(!flag_eol && !flag_eof )
       {
	 if(  (next_char=fgetc(file_handle)) < 0){flag_eof=1;}
	 word[counter]=next_char;
	  /* putchar(next_char);  */
	 if(word[counter] == SPACE || word[counter] == HTAB){ word[counter]=NULL;}
	 if(word[counter] == CR || word[counter]== LF ) 
	   {
	     flag_eol=1; 
	     word[counter]=NULL;
	   } 
	 counter++;     
       }  
     if (strstr(word,"BEGIN_INIT") != NULL) 
       {
	 sprintf(prog_msg,"found BEGIN_INIT line count= %d\n",line_count);
	 if(debug) ss_prog(prog_msg);
	 flag_init_start=1;
       }	 	
     if (strstr(word,"END_INIT") != NULL) 
       {
	 sprintf(prog_msg,"found END_INIT\n");
	 if(debug) ss_prog(prog_msg);
	 flag_init_end=1;
       }
     if(flag_init_start && !flag_init_end)
       {
	 sprintf(prog_msg,"switch line count= %d, word= %s\n",line_count,word);
	 if(debug) ss_prog(prog_msg);
	 switch(line_count++)
	   {
	   case 1: 
	     sprintf(prog_msg," case 1: BEGIN_INIT");
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 2: sscanf(word,"%d", &debug_value);
	     sprintf(prog_msg," setting the debug status (1=use,0=off) %d \n", debug_value);
	     debug=debug_value;
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 3:  sprintf(configuration,"%s",word);
	     sprintf(prog_msg," setting config file = %s\n",configuration);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 4: sscanf(word,"%d", &bit_number);
	     sprintf(prog_msg," setting the trigger bit requirement to %d \n", bit_number);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 5: sprintf(coda_state,"%s",word);
	     sprintf(prog_msg," expected coda state = %s \n", coda_state);
	     if(debug) ss_prog(prog_msg);
	     break;
	   case 6: sprintf(camac3_state,"%s",word);
	     sprintf(prog_msg," expected camac3 state = %s \n", camac3_state);
	     if(debug) ss_prog(prog_msg);
	     break;
	   case 7: sprintf(sc_laser1_state,"%s",word);
	     sprintf(prog_msg," expected sc1_laser1 state = %s \n", sc_laser1_state);
	     if(debug) ss_prog(prog_msg);
	     break;
	   case 8: sscanf(word,"%f", &real_no);
	     freq_hz=(int)(real_no); 
	     sprintf(prog_msg," freq and period(10usec) = %f %d \n", real_no, freq_hz);
	     if(debug) ss_prog(prog_msg);
	     break;
	   case 9: sscanf(word,"%d", &numberOfPulses);
	     sprintf(prog_msg," setting number of pulses to %d \n", numberOfPulses);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 10: sscanf(word,"%d", &interim);
	     sprintf(prog_msg," wait time between intialization attempts %d \n",interim);
	     if(debug) ss_prog(prog_msg);
	     break; 	            	  
	   case 11: sscanf(word,"%d", &use_laser[0]);
	     sprintf(prog_msg," Forward carriage laser (1=use,0=don't use) %d \n", use_laser[0]);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 12: sscanf(word,"%d", &use_laser[1]);
	     sprintf(prog_msg," North Clam laser (1=use,0=don't use) %d \n", use_laser[1]);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 13: sscanf(word,"%d", &use_laser[2]);
	     sprintf(prog_msg," South clam shell laser (1=use,0=don't use) %d \n", use_laser[2]);
	     if(debug) ss_prog(prog_msg);
	     break; 
	   case 14: sscanf(word,"%d", &use_laser[3]);
	     sprintf(prog_msg," Space frame laser (1=use,0=don't use) %d \n", use_laser[3]);
	     if(debug) ss_prog(prog_msg);
	     break;
	   case 15: sscanf(word,"%d", &use_camac3);
	     sprintf(prog_msg," VME create camac3 (in use=1 not in use =0) = %d \n",use_camac3);
	     if(debug) ss_prog(prog_msg);
	     break; 	            	  
	   case 16: sscanf(word,"%d", &use_sc_laser1);
	     sprintf(prog_msg," VME create sc_laser1 (in use=1 not in use =0)= %d \n",use_sc_laser1);
	     if(debug) ss_prog(prog_msg);
	     break; 	            	  
	     
	     
	   default: sprintf(prog_msg,"def  \n");
	     if(debug) ss_prog(prog_msg);
	     errorid=-111;
	     break;
	   }                  
       }
     
   }	
 
 



  /******* beginning of sequence read  ***/

 /************** read configuration file to set execute procedure    ********/
  line_count=0;
  flag_search=0;

  while (!flag_search && !flag_eof && !flag_procedure_end) 
    { 
      counter=0;
      flag_eol=0; 
      if(debug)printf(" reading next line line count=%d  \n",line_count);
      while(!flag_eol && !flag_eof )
	{
	  if(  (next_char=fgetc(file_handle)) < 0){flag_eof=1;}
	  word[counter]=next_char;
	  /* putchar(next_char);  */
	  if((word[counter] == POUND) && (counter == 0))
	    {
	      if(debug)printf("found empty line symbol so break to next line  \n");
	      save_line=line_count;
	      line_count=5;
	    }
	  if(word[counter] == SPACE || word[counter] == HTAB){ word[counter]=NULL;}
	  if(word[counter] == CR || word[counter]== LF ) 
	    {
	      flag_eol=1; 
	      word[counter]=NULL;
	    } 
	  counter++;     
	}  
      if (strstr(word,"BEGIN_PROCEDURE") != NULL) 
	{
	  sprintf(prog_msg,"found BEGIN_PROCEDURE\n");
E 3
I 3
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
E 3
	  ss_prog(prog_msg);
D 3
	  flag_procedure_start=1;
	}	 	
      if (strstr(word,"END_PROCEDURE") != NULL) 
	{
	  sprintf(prog_msg,"found END_PROCEDURE\n");
E 3
I 3
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
E 3
	  ss_prog(prog_msg);
D 3
	  flag_procedure_end=1;
E 3
	}
D 3
      if(flag_procedure_start && !flag_procedure_end)
	{
	  switch(line_count)
	    {
	    case 0: sprintf(prog_msg," processing procedure steps\n");
	      if(debug) ss_prog(prog_msg);
	      line_count=1;
	      seq_no=0;
	      break;
	    case 1: sscanf(word,"%d", &seq_laser[seq_no]);
	      sprintf(prog_msg," getting laser for %d sequence = %d\n",seq_no+1,seq_laser[seq_no] );
	      if(debug) ss_prog(prog_msg);
	      line_count++;
	      break;
	 	  
	    case 2: sscanf(word,"%d", &seq_filter[seq_no]);
	      sprintf(prog_msg," getting filter for %d sequence= %d\n",seq_no+1,seq_filter[seq_no] );
	      if(debug) ss_prog(prog_msg);
	      line_count++;
	      break;
	    case 3: sscanf(word,"%d", &seq_speed[seq_no]);
	      sprintf(prog_msg," getting speed for %d sequence= %d\n",seq_no+1,seq_speed[seq_no] );
	      if(debug) ss_prog(prog_msg);
	      line_count++;
	      break;
	    case 4: sscanf(word,"%d", &seq_loops[seq_no]);
	      sprintf(prog_msg," getting loops for %d sequence= %d\n",seq_no+1,seq_loops[seq_no] );
	      if(debug) ss_prog(prog_msg);
	      line_count=1;
	      seq_no++;
	      break;
	    case 5: /* set the line count to 5 to skip processing return saved value and cont. */
	      line_count=save_line;
	      break;

	    default: sprintf(prog_msg,"bad setting on seq number =%d\n", seq_no);
	      if(debug) ss_prog(prog_msg);
	      break;
	    }                  
E 3
I 3
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
E 3
	}
D 3
	 		 	 	
    }	






  /********************************************
  *tmpstr="config_file_read" ;
  add_status(tmpstr,1);
  ss_status(); 	
*************************************************/

E 3
I 3
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
E 3
  
D 3

  if(errorid != 0) {returnv= processid+errorid;}
  return(returnv);
E 3
I 3
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
E 3
}
I 3

I 4
#else

void
init_checks_dummy()
{
  return;
}

#endif
E 4
E 3
E 1
