h45309
s 00008/00008/00085
d D 1.2 02/08/28 20:06:57 boiarino 3 1
c cc->cc1, sc->sc1, lac->lac1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/spardb/unix/pedman_init.c
e
s 00093/00000/00000
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


int main (int argc, char **argv )
{
  char result1[512];
  char result[512];
  char exename[111];
  int time_out;
  
  if ( argc < 2 || argc >3 ) {
D 3
    printf ("Usage: pedman_init [detector component:ec1,ec2,sc,cc,lac] time_out\n");
E 3
I 3
    printf ("Usage: pedman_init [detector component:ec1,ec2,sc1,cc1,lac1] time_out\n");
E 3
    return 0;
  }
  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;
  
  
  
  DP_cmd_init("clon10");
  
  
  if (!strcmp(argv[1],"ec1")) {
    sprintf(exename,"exec spar_init(\"%s\")","ec1.ped");
    DP_cmd("ec1",exename,result,time_out);
    printf("%s \n",result);
  }
  
  else  if (!strcmp(argv[1],"ec2")) {
    sprintf(exename,"exec spar_init(\"%s\")","ec2.ped");
    DP_cmd("ec2",exename,result,time_out);
    
    
  }
  
D 3
  else if (!strcmp(argv[1],"sc")) {
    sprintf(exename,"exec spar_init(\"%s\")","sc.ped");
E 3
I 3
  else if (!strcmp(argv[1],"sc1")) {
    sprintf(exename,"exec spar_init(\"%s\")","sc1.ped");
E 3
    DP_cmd("sc1",exename,result,time_out);
    printf("%s \n",result);
  }
  
D 3
  else if (!strcmp(argv[1],"cc")) {
    sprintf(exename,"exec spar_init(\"%s\")","cc.ped");
E 3
I 3
  else if (!strcmp(argv[1],"cc1")) {
    sprintf(exename,"exec spar_init(\"%s\")","cc1.ped");
E 3
    DP_cmd("cc1",exename,result,time_out);
    printf("%s \n",result); 
    
  }
  
D 3
  else if (!strcmp(argv[1],"lac")) {
    sprintf(exename,"exec spar_init(\"%s\")","lac.ped");
E 3
I 3
  else if (!strcmp(argv[1],"lac1")) {
    sprintf(exename,"exec spar_init(\"%s\")","lac1.ped");
E 3
    DP_cmd("lac1",exename,result,time_out);
    printf("lac1 \n",result);
    
  }
  /*
    else if (!strcmp(argv[1],"all")) {
    sprintf(exename,"exec spar_init(\"%s\")","ec1.ped");
    DP_cmd("ec1",exename,result,time_out);
    printf("%s \n",result);
    
    
    sprintf(exename,"exec spar_init(\"%s\")","ec2.ped");
    DP_cmd("ec2",exename,result,time_out);
    printf("%s \n",result);
    
    sprintf(exename,"exec spar_init(\"%s\")","sc.ped");
    DP_cmd("sc1",exename,result,time_out);
    printf("%s \n",result);
    
    sprintf(exename,"exec spar_init(\"%s\")","cc.ped");
    DP_cmd("cc1",exename,result,time_out);
    printf("%s \n",result); 
    
    sprintf(exename,"exec spar_init(\"%s\")","lac.ped");
    DP_cmd("lac1",exename,result,time_out);
    printf("lac1 \n",result);
    
    } */
  else {
D 3
    printf ("Usage: pedman_init [detector component:ec,sc,cc,lac] time_out\n");
E 3
I 3
    printf ("Usage: pedman_init [detector component:ec1,ec2,sc1,cc1,lac1] time_out\n");
E 3
    return 0;
  }
  return 0;
}



E 1
