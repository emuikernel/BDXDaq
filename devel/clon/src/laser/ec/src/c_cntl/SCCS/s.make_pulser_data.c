h31593
s 00030/00035/00033
d D 1.2 04/08/11 10:00:29 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/make_pulser_data.c
e
s 00068/00000/00000
d D 1.1 00/09/21 14:11:08 wolin 1 0
c date and time created 00/09/21 14:11:08 by wolin
e
u
U
f e 0
t
T
I 1
/* dpS execution of the laser commands giovanetti, Larson
 */
I 3

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
I 3
#include "EC_laser_ROC.h"
E 3

D 3
int main()
E 3
I 3
int
main()
E 3
{
D 3
char exename[111];
char result[512];
int data;
float freq;
long period_10usec;
int numberOfPulses;
E 3
I 3
 char  exename[111];
 char  result[512];
 int   data;
 float freq;
 long  period_10usec;
 int   numberOfPulses;
E 3

/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
D 3
 
E 3

 printf("debug_pulser (1=on 0=off)=?");
 scanf("%d,\n",&data);
D 3

E 3
 
D 3
     printf(" there is a limited amount of data to be passed sometimes debug crashes program");
     sprintf(exename,"exec debug_pulser=%d",data);
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
E 3
I 3
 printf(" there is a limited amount of data to be passed sometimes debug crashes program");
 sprintf(exename,"exec debug_pulser=%d",data);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
E 3

D 3

E 3
 printf(" frequency of pulser=?");
 scanf("%f,\n",&freq);
 printf(" using freq= %f \n",freq);
 period_10usec= (long) 100000./freq;
 printf(" the period in 10 usec steps is %ld \n",period_10usec);
D 3

E 3
 
D 3
 
     sprintf(exename,"exec period_10usec=%ld",period_10usec);
     printf("%s \n",exename);
      DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
E 3
I 3
 sprintf(exename,"exec period_10usec=%ld",period_10usec);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
E 3

 printf(" number of pulses=?");
 scanf("%d,\n",&numberOfPulses);
 printf(" using number of pulses = %d \n",numberOfPulses);
D 3
 
 
 
     sprintf(exename,"exec numberOfPulses=%d",numberOfPulses);
     printf("%s \n",exename);
      DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
E 3

I 3
 sprintf(exename,"exec numberOfPulses=%d",numberOfPulses);
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);

E 3
/* 1   command ****************************   */   
  
D 3
     sprintf(exename,"exec make_pulser_data");
     printf("%s \n",exename);
     DP_cmd("camac3",exename,result,111);
     printf("%s \n",result);
 
    
}
E 3
I 3
 sprintf(exename,"exec make_pulser_data");
 printf("%s \n",exename);
 DP_cmd(EC_laser_roc,exename,result,111);
 printf("%s \n",result);
E 3

I 3

}
E 3

E 1
