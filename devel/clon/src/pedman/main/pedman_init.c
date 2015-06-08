/* dpS execution of the sparsification thresholds 
 * readback vxWorks program. Opening the configurations
 * 03/19/98 by vhg 
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "libtcp.h"

int
main(int argc, char **argv )
{
  char result1[512];
  char result[512];
  char exename[111];
  int time_out;
  
  if ( argc < 2 || argc >3 ) {
    printf ("Usage: pedman_init [detector component:ec1,ec2,sc1,cc1,lac1] time_out\n");
    return 0;
  }
  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;
  
  if (!strcmp(argv[1],"ec1")) {
    sprintf(exename,"spar_init(\"%s\")","ec1.ped");
    tcpClientCmd("ec1",exename,result);
    printf("%s \n",result);
  }
  else  if (!strcmp(argv[1],"ec2")) {
    sprintf(exename,"spar_init(\"%s\")","ec2.ped");
    tcpClientCmd("ec2",exename,result); 
  }
  else if (!strcmp(argv[1],"sc1")) {
    sprintf(exename,"spar_init(\"%s\")","sc1.ped");
    tcpClientCmd("sc1",exename,result);
    printf("%s \n",result);
  }
  else if (!strcmp(argv[1],"cc1")) {
    sprintf(exename,"spar_init(\"%s\")","cc1.ped");
    tcpClientCmd("cc1",exename,result);
    printf("%s \n",result); 
    
  }
  else if (!strcmp(argv[1],"lac1")) {
    sprintf(exename,"spar_init(\"%s\")","lac1.ped");
    tcpClientCmd("lac1",exename,result);
    printf("lac1 \n",result);
    
  }
  else
  {
    printf ("Usage: pedman_init [detector component:ec1,ec2,sc1,cc1,lac1] time_out\n");
    return 0;
  }
  return 0;
}



