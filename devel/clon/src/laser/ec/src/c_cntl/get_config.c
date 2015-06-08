 /* dpS execution of the laser commands giovanetti, Larson jan 99
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int
main()
{
 char result[512];


/* intialize DP stuff */

 printf("\n initializing the connection \n");
 DP_cmd_init("clon10");
 printf(" initializing done \n\n");
 
 DP_cmd("clastrig2","clastrig2 cget -config",result,111);
 printf(" trigger indicates  configuration  is %s \n", result);
 
 
 DP_cmd("EB1","EB1 status",result,111);
 printf(" event builder is in the %s state \n\n",result);

}

