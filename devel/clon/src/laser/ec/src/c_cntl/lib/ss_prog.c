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
