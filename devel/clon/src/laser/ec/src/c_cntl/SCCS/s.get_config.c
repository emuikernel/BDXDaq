h50672
s 00015/00018/00013
d D 1.2 04/08/11 12:27:01 sergpozd 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/get_config.c
e
s 00031/00000/00000
d D 1.1 00/09/21 14:11:05 wolin 1 0
c CodeManager Uniquification: laser/ec/src/c_cntl/get_config.c
c date and time created 00/09/21 14:11:05 by wolin
e
u
U
f e 0
t
T
I 1
 /* dpS execution of the laser commands giovanetti, Larson jan 99
D 3
 */ 
E 3
I 3
 */

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

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
char camac_state[50]="booted";
int success =1 ;
E 3
I 3
 char result[512];
E 3

I 3

E 3
/* intialize DP stuff */

D 3
 printf("initializing the connection \n");
E 3
I 3
 printf("\n initializing the connection \n");
E 3
 DP_cmd_init("clon10");
D 3
 printf("initializing done \n");
E 3
I 3
 printf(" initializing done \n\n");
E 3
 
D 3
        DP_cmd("clastrig2","clastrig2 cget -config",result,111);
E 3
I 3
 DP_cmd("clastrig2","clastrig2 cget -config",result,111);
 printf(" trigger indicates  configuration  is %s \n", result);
 
 
 DP_cmd("EB1","EB1 status",result,111);
 printf(" event builder is in the %s state \n\n",result);
E 3

D 3

              printf(" trigger indicates  configuration  is %s \n\n", result);
              

    DP_cmd("EB1","EB1 status",result,111);
      	printf("  event builder is in the %s state \n",result);

            

E 3
}
I 3

E 3
E 1
