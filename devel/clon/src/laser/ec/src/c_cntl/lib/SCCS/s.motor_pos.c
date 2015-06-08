h08699
s 00017/00018/00034
d D 1.2 04/08/11 17:42:33 sergpozd 3 1
c "camac3" --> char *EC_laser_roc="camac1"
e
s 00000/00000/00000
d R 1.2 00/09/21 14:12:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/motor_pos.c
e
s 00052/00000/00000
d D 1.1 00/09/21 14:12:41 wolin 1 0
c date and time created 00/09/21 14:12:41 by wolin
e
u
U
f e 0
t
T
I 3
/****************************************************
   $1R resets the PLR
*****************************************************/
E 3
I 1

D 3
/*********   $1R resets the PLR   ***********/

E 3
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <laser.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <laser.h>
#include <bit_defs.h>
#include <parse.h>
I 3
#include "EC_laser_ROC.h"
E 3

D 3
 
int motor_pos(int *position, int debug)
E 3
I 3

int
motor_pos(int *position, int debug)
E 3
{
I 3
 int success=0;
 int flag_loop=0;
 int mstat=0;
 int j;
E 3


D 3
int success=0;
int flag_loop=0;


int mstat=0;


int j;



E 3
/* *************status blank start ****************************   */   
  
     sprintf(exename,"exec {laser_motor(\"$16\")}");
     if(debug)printf("\n %s ",exename);
D 3
     success = DP_cmd("camac3",exename,result,111);
E 3
I 3
     success = DP_cmd(EC_laser_roc,exename,result,111);
E 3
     if ( success == 0)
       {
       if(debug)printf("%s \n",result);
       ret_value=parse(result, int_data, real_data, string_data,n1,n2,n3);
       if(debug)printf(" value of string %s \n",&string_data[0][3]);
       sscanf(&string_data[0][3],"%d", &mstat);
       if(debug)printf("   position = %d ", mstat);
        }
    else
        {
         printf("unable to reach controller in motor_pos ");
	 return(-3);
D 3
        } 
E 3
I 3
        }
E 3
  *position=mstat;
D 3
  return(1); /*  return 1 if idle*/            
E 3
I 3

  return(1); /*  return 1 if idle */

E 3
}
I 3

E 3
E 1
