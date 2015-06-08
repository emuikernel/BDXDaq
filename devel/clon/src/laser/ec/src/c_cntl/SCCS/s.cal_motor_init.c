h27459
s 00002/00002/00031
d D 1.3 04/08/10 16:37:33 sergpozd 4 3
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00007/00005/00026
d D 1.2 04/08/10 10:53:26 sergpozd 3 1
c "camac3" --> char *roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/cal_motor_init.c
e
s 00031/00000/00000
d D 1.1 00/09/21 14:11:04 wolin 1 0
c date and time created 00/09/21 14:11:04 by wolin
e
u
U
f e 0
t
T
I 1
/* dpS execution of the laser commands giovanetti, Larson jan 99
 */ 
I 3

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
I 4
#include "EC_laser_ROC.h"
E 4

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
E 3
I 3
D 4
 char *roc="camac1";
E 4
 char exename[111];
 char result[512];
E 3


/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 


/* 1   command ****************************   */   
  
     sprintf(exename,"exec cal_motor_init");
     printf("%s \n",exename);
D 3
     DP_cmd("camac3",exename,result,111);
E 3
I 3
D 4
     DP_cmd(roc,exename,result,111);
E 4
I 4
     DP_cmd(EC_laser_roc,exename,result,111);
E 4
E 3
     printf("%s \n",result);
 
    
}
D 3

E 3

E 1
