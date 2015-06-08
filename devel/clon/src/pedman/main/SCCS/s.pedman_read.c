h39122
s 00033/00060/00049
d D 1.2 02/08/28 20:08:11 boiarino 3 1
c cc->cc1, sc->sc1, lac->lac1
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/spardb/unix/pedman_read.c
e
s 00109/00000/00000
d D 1.1 00/07/31 15:15:09 gurjyan 1 0
c date and time created 00/07/31 15:15:09 by gurjyan
e
u
U
f e 0
t
T
I 1
D 3
/* dpS execution of the sparsification thresholds 
E 3
I 3
/*
 * pedman_read.c
 *
 * dpS execution of the sparsification thresholds 
E 3
 * readback vxWorks program. Opening the configurations
 * 03/19/98 by vhg 
 */
I 3

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

D 3

int main (int argc, char **argv )
E 3
I 3
int
main(int argc, char **argv)
E 3
{
  char result1[512];
  char result[512];
  char program_name1[111];
  char program_name2[111];
  int time_out;

D 3
  if ( argc < 2 || argc > 3 ) {
    printf ("Usage: pedman_read [detector component:ec1,ec2,sc,cc,lac] time_out\n");
    return 0;
E 3
I 3
  if(argc < 2 || argc > 3)
  {
    printf("Usage: pedman_read [detector component:ec1,ec2,sc1,cc1,lac1] time_out\n");
    return(0);
E 3
  }
D 3
  
  if ( argc == 3 ) time_out = atoi(argv[2]);
  else time_out =111;
  
E 3
I 3

  if(argc == 3) time_out = atoi(argv[2]);
  else time_out = 111;

E 3
  sprintf(program_name1,"exec spar_hardread");
  sprintf(program_name2,"exec ped_hardread");
D 3
    
printf("%s %s\n",program_name1,program_name2);   
  
E 3
I 3

  printf("%s %s\n",program_name1,program_name2);

E 3
  DP_cmd_init("clon10");
D 3
  
  
  if (!strcmp(argv[1],"ec1")) {
E 3

I 3
  if(!strcmp(argv[1],"ec1"))
  {
E 3
    DP_cmd("ec1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("ec1",program_name2,result,time_out);
    printf("%s \n",result);
  }
D 3

  else  if (!strcmp(argv[1],"ec2")) {
E 3
I 3
  else if(!strcmp(argv[1],"ec2"))
  {
E 3
    DP_cmd("ec2",program_name1,result1,time_out);
    printf("%s \n",result1);
    DP_cmd("ec2",program_name2,result1,time_out);
    printf("%s \n",result1);
D 3
    
E 3
  }
D 3
  
  else if (!strcmp(argv[1],"sc")) {
E 3
I 3
  else if(!strcmp(argv[1],"sc1"))
  {
E 3
    DP_cmd("sc1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("sc1",program_name2,result,time_out);
    printf("%s \n",result);
  }
D 3
  
  else if (!strcmp(argv[1],"cc")) {
E 3
I 3
  else if(!strcmp(argv[1],"cc1"))
  {
E 3
    DP_cmd("cc1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("cc1",program_name2,result,time_out);
    printf("%s \n",result);
  }
D 3
  
  else if (!strcmp(argv[1],"lac")) {
E 3
I 3
  else if(!strcmp(argv[1],"lac1"))
  {
E 3
    DP_cmd("lac1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("lac1",program_name2,result,time_out);
    printf("%s \n",result);
  }
D 3
  /*  
  else if (!strcmp(argv[1],"all")) {
    DP_cmd("ec1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("ec1",program_name2,result,time_out);
    printf("%s \n",result);


    DP_cmd("ec2",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("ec2",program_name2,result,time_out);
    printf("%s \n",result);

    DP_cmd("sc1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("sc1",program_name2,result,time_out);
    printf("%s \n",result);
    
    
    DP_cmd("cc1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("cc1",program_name2,result,time_out);
    printf("%s \n",result);    


    DP_cmd("lac1",program_name1,result,time_out);
    printf("%s \n",result);
    DP_cmd("lac1",program_name2,result,time_out);
    printf("%s \n",result);
    
  } */
  else {
    printf ("Usage: pedman_read [detector component:ec,sc,cc,lac] time_out \n");
    return 0;
E 3
I 3
  else
  {
    printf ("Usage: pedman_read [detector component:ec1,ec2,sc1,cc1,lac1] time_out \n");
E 3
  }
D 3
return 0;
E 3
I 3

  return(0);
E 3
}



E 1
