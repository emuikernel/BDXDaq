h20120
s 00007/00005/00026
d D 1.2 04/08/11 09:48:49 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lpoweron.c
e
s 00031/00000/00000
d D 1.1 00/09/21 14:11:08 wolin 1 0
c CodeManager Uniquification: laser/ec/src/c_cntl/lpoweron.c
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
E 3
I 3
 char exename[111];
 char result[512];
E 3


/* intialize DP stuff */

 printf("initializing the connection \n");
 DP_cmd_init("clon10");
 printf("initializing done \n");
 


/* 1   command ****************************   */   
  
     sprintf(exename,"exec lpoweron");
     printf("%s \n",exename);
D 3
     DP_cmd("camac3",exename,result,111);
E 3
I 3
     DP_cmd(EC_laser_roc,exename,result,111);
E 3
     printf("%s \n",result);
 
    
}
D 3

E 3

E 1
