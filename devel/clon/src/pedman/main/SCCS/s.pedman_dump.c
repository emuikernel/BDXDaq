h30385
s 00005/00005/00090
d D 1.2 02/08/28 20:04:39 boiarino 3 1
c cc->cc1, sc->sc1, lac->lac1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/spardb/unix/pedman_dump.c
e
s 00095/00000/00000
d D 1.1 00/07/31 15:15:09 gurjyan 1 0
c date and time created 00/07/31 15:15:09 by gurjyan
e
u
U
f e 0
t
T
I 1
/* dpS execution of the sparsification thresholds 
 * readback vxWorks program. Opening the configurations
 * 03/19/98 by vhg 
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static char	*msql_database = "clasrun";
char *getenv();

int	get_run_number(char *msql_database, char *session);

int main (int argc, char **argv)
{
  char result[512];
  static  char *path, *session;
  int *runnum;
  char	*config;
  char *clonparms;
  char program_name[111];
  int time_out;

  if ( argc < 2 || argc > 3 ) {
D 3
    printf ("Usage: pedman_dump [detector component:ec,sc,cc,lac] time_out \n");
E 3
I 3
    printf ("Usage: pedman_dump [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
E 3
    return 0;
  }
  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;


  if (session == NULL) session = "clasprod";

  get_run_config(msql_database, session, &runnum, &config);
  path = getenv("CLON_PARMS"); 
  
  sprintf(program_name,"exec spar_close(\"%s\",\"%s\",%d,\"%s\")",path,session,runnum,argv[1]); 
  
  printf("%s \n",program_name);   
  
  DP_cmd_init("clon10");
  
  
  if (!strcmp(argv[1],"ec1")) {
    DP_cmd("ec1",program_name,result,time_out);
    printf("%s \n",result);
  }
  else if (!strcmp(argv[1],"ec2")) {
    DP_cmd("ec2",program_name,result,time_out);
    printf("%s \n",result);
  }
  
D 3
  else if (!strcmp(argv[1],"sc")) {
E 3
I 3
  else if (!strcmp(argv[1],"sc1")) {
E 3
    DP_cmd("sc1",program_name,result,time_out);
    printf("%s \n",result);
  }
  
D 3
  else if (!strcmp(argv[1],"cc")) {
E 3
I 3
  else if (!strcmp(argv[1],"cc1")) {
E 3
    DP_cmd("cc1",program_name,result,time_out);
    printf("%s \n",result);
  }
  
D 3
  else if (!strcmp(argv[1],"lac")) {
E 3
I 3
  else if (!strcmp(argv[1],"lac1")) {
E 3
    DP_cmd("lac1",program_name,result,time_out);
    printf("%s \n",result);
  }
  
  /*
    else if (!strcmp(argv[1],"all")) {
    DP_cmd("ec1",program_name,result,time_out);
    printf("%s \n",result);

    DP_cmd("ec2",program_name,result,time_out);
    printf("%s \n",result);
    
    DP_cmd("sc1",program_name,result,time_out);
    printf("%s \n",result);
    
    
    DP_cmd("cc1",program_name,result,time_out);
    printf("%s \n",result);

    DP_cmd("lac1",program_name,result,time_out);
    printf("%s \n",result);
    
    } */
  else {
D 3
    printf ("Usage: pedman_read [detector component:ec1,ec2,sc,cc,lac] time_out \n");
E 3
I 3
    printf ("Usage: pedman_read [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
E 3
    return 0;
  }
  return 0;
}



E 1
