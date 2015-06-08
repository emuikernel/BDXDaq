/* dpS execution of the laser commands giovanetti, Larson jan 99
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "EC_laser_ROC.h"

int
main()
{
 char exename[111];
 char result[512];


/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 


/* 1   command ****************************   */   
  
     sprintf(exename,"exec laser_enable");
     printf("%s \n",exename);
     DP_cmd(EC_laser_roc,exename,result,111);
     printf("%s \n",result);
 
    
}

