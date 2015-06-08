h16535
s 00000/00000/00000
d R 1.2 00/09/21 14:12:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/ss_prog.c
e
s 00042/00000/00000
d D 1.1 00/09/21 14:12:41 wolin 1 0
c CodeManager Uniquification: laser/ec/src/c_cntl/lib/ss_prog.c
c date and time created 00/09/21 14:12:41 by wolin
e
u
U
f e 0
t
T
I 1
/*  sends data to RTserever */


 /* main include file variable intialization, subroutines have 0 */
 
 

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

/* for smartsockets */
#include <ssinclude.h>


 int ss_prog (char* msg_prog, int broadcastflg)
{
 int jj = 1;
 int rtncode;
 
 printf( " %s\n ",msg_prog  );

 if(broadcastflg){
   TipcSrvMsgWrite("/EClaser/progress",TipcMtLookupByNum(T_MT_INFO),    
        TRUE, T_IPC_FT_STR,
   	msg_prog,
  	 NULL);

  TipcSrvFlush(); 
 }

return (1);
 	
}
E 1
