/* dpS execution of the sparsification thresholds 
 * readback vxWorks program. Opening the configurations
 * 03/19/98 by vhg 
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "libtcp.h"

int
main(int argc, char **argv)
{
  char result1[512];
  char result[512];
  char program_name1[111];
  char program_name2[111];
  int time_out;

  if ( argc < 2 || argc > 3 ) {
    printf ("Usage: pedman_write [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
    return 0;
  }

  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;
  
  
  sprintf(program_name1,"spar_hardwrite");
    
  /*printf("%s %s\n",program_name1,program_name2);   */
  
  
  if (!strcmp(argv[1],"ec1")) {

    tcpClientCmd("ec1",program_name1,result);
    printf("%s \n",result);
  }

  else  if (!strcmp(argv[1],"ec2")) {
    tcpClientCmd("ec2",program_name1,result1);
    printf("%s \n",result1);
    
  }
  
  else if (!strcmp(argv[1],"sc1")) {
    tcpClientCmd("sc1",program_name1,result);
    printf("%s \n",result);
  }
  
  else if (!strcmp(argv[1],"cc1")) {
    tcpClientCmd("cc1",program_name1,result);
    printf("%s \n",result);
  }
  
  else if (!strcmp(argv[1],"lac1")) {
    tcpClientCmd("lac1",program_name1,result);
    printf("%s \n",result);
  }
  else {
    printf ("Usage: pedman_write [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
    return 0;
  }
return 0;
}



