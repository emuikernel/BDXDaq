h35303
s 00002/00002/00024
d D 1.3 04/08/10 16:44:32 sergpozd 4 3
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00008/00008/00018
d D 1.2 04/08/10 12:50:12 sergpozd 3 1
c "camac3" --> char *roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/getstatus.c
e
s 00026/00000/00000
d D 1.1 00/09/21 14:11:06 wolin 1 0
c CodeManager Uniquification: laser/ec/src/c_cntl/getstatus.c
c date and time created 00/09/21 14:11:06 by wolin
e
u
U
f e 0
t
T
I 1
D 3
/* camac3 laser control written by Kevin giovanetti JMU jan 99
 *  
E 3
I 3
/*   getstatus.c - written by Kevin Giovanetti JMU jan 99
E 3
 */
D 3
 
E 3
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
 
     sprintf(exename,"exec getstatus");
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
