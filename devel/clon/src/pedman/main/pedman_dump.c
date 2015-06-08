/* dpS execution of the sparsification thresholds 
 * readback vxWorks program. Opening the configurations
 * 03/19/98 by vhg 
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static char	*mysql_database = "clasrun";
char *getenv();

int
main(int argc, char **argv)
{
  char result[512];
  static  char *path, *session;
  int runnum;
  char *config;
  char *clonparms;
  char program_name[111];
  int time_out;

  if ( argc < 2 || argc > 3 ) {
    printf ("Usage: pedman_dump [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
    return 0;
  }
  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;


  if (session == NULL) session = "clasprod";

  get_run_config(mysql_database, session, &runnum, &config);
  path = getenv("CLON_PARMS"); 
  
  sprintf(program_name,"spar_close(\"%s\",\"%s\",%d,\"%s\")",path,session,runnum,argv[1]); 
  
  printf("%s \n",program_name);   


  if (!strcmp(argv[1],"ec1")) {
    tcpClientCmd("ec1",program_name,result);
    printf("%s \n",result);
  }
  else if (!strcmp(argv[1],"ec2")) {
    tcpClientCmd("ec2",program_name,result);
    printf("%s \n",result);
  }
  
  else if (!strcmp(argv[1],"sc1")) {
    tcpClientCmd("sc1",program_name,result);
    printf("%s \n",result);
  }
  
  else if (!strcmp(argv[1],"cc1")) {
    tcpClientCmd("cc1",program_name,result);
    printf("%s \n",result);
  }
  
  else if (!strcmp(argv[1],"lac1")) {
    tcpClientCmd("lac1",program_name,result);
    printf("%s \n",result);
  }
  else
  {
    printf ("Usage: pedman_read [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
    return 0;
  }
  return 0;
}



