/* dpS execution of the laser commands giovanetti, Larson
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "EC_laser_ROC.h"

int
main()
{
 char  exename[111];
 char  result[512];
 int   data;
 float freq;
 long  period_10usec;
 int   numberOfPulses;

/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");

 printf("debug_pulser (1=on 0=off)=?");
 scanf("%d,\n",&data);
 
 printf(" there is a limited amount of data to be passed sometimes debug crashes program");
 sprintf(exename,"exec debug_pulser=%d",data);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);

 printf(" frequency of pulser=?");
 scanf("%f,\n",&freq);
 printf(" using freq= %f \n",freq);
 period_10usec= (long) 100000./freq;
 printf(" the period in 10 usec steps is %ld \n",period_10usec);
 
 sprintf(exename,"exec period_10usec=%ld",period_10usec);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);

 printf(" number of pulses=?");
 scanf("%d,\n",&numberOfPulses);
 printf(" using number of pulses = %d \n",numberOfPulses);

 sprintf(exename,"exec numberOfPulses=%d",numberOfPulses);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);

/* 1   command ****************************   */   
  
 sprintf(exename,"exec make_pulser_data");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);


}

