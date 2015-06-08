h45459
s 00015/00022/00125
d D 1.2 04/08/11 15:49:21 sergpozd 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 00/09/21 14:11:11 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/ss_send.c
e
s 00147/00000/00000
d D 1.1 00/09/21 14:11:10 wolin 1 0
c date and time created 00/09/21 14:11:10 by wolin
e
u
U
f e 0
t
T
I 1
/*  sends data to RTserever */

D 3


E 3
 /* main include file variable intialization */
D 3
#define INIT=1 
 
E 3
I 3
#define INIT 1
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

/* for smartsockets */
#include <ssinclude.h>


 int main (int argc, char *argv[])
{
 int jj = 1;
 int m,n;
 int l;
 int ii=0;
 int rtncode;
 T_STR  tmpstr[50];
 T_INT4 ecrows;
 T_INT4 ECbuf[25][4], buf[100];

 
 rtncode=ss_init("clasprod",0);

 printf(" status returned from init= %d \n",rtncode );

 ecrows=2;
 ii=0;
while(ii < ecrows)
  {
    ECbuf[ii][0]= 10*ii+0;
    ECbuf[ii][1]= 10*ii+1;
    ECbuf[ii][2]= 10*ii+2;
    ECbuf[ii][3]= 10*ii+3;
    ++ii;
  }
 

  l=0;
  m=0;
  while(m<ecrows)
    {
      l=0;
      while(l<4)
	{
	  buf[4*m+l]=  ECbuf[m][l];
	  ++l;
	}
    m++;
    }

  /****************************
   boswrite(buf,ecrows); 
 ************************/
 nopairs=0;
 *tmpstr="kevin" ;
 add_status(tmpstr,1);
 *tmpstr="arnie" ;
 add_status(tmpstr,2);
 *tmpstr="adam" ;
 add_status(tmpstr,3);
 *tmpstr="jason" ;
 add_status(tmpstr,4);
 *tmpstr="giovanetti" ;
 add_status(tmpstr,5);
 *tmpstr="larson" ;
 add_status(tmpstr,6);

*tmpstr="done" ;
 add_status(tmpstr,1);

 printf("nopairs=%d\n",nopairs);
 n=0;
 while(n<nopairs){printf(" status buffer = %s, %d \n", *status_id[n],status_val[n]);++n;}

 ss_status(); /************************************/
 
 
 
  nopairs=0;
 *tmpstr="kevin" ;
 add_status(tmpstr,10);
 *tmpstr="arnie" ;
 add_status(tmpstr,20);
 *tmpstr="adam" ;
 add_status(tmpstr,30);
 *tmpstr="jason" ;
 add_status(tmpstr,40);
 *tmpstr="giovanetti" ;
 add_status(tmpstr,50);
 *tmpstr="larson" ;
 add_status(tmpstr,60);
 *tmpstr="done" ;
 add_status(tmpstr,1);


 printf("nopairs=%d\n",nopairs);
 n=0;
 while(n<nopairs){printf(" status buffer = %s, %d \n", *status_id[n],status_val[n]);++n;}

 ss_status(); /************************************/
D 3
 
 
 
 
 
E 3
I 3


E 3
 while (jj < 10) {
 
 	char kmessage[50];
	char prog_msg[100];
 
	TutOut(" TutOut  load message  jj = %d  \n", jj );
	sprintf(kmessage,"message = %d",jj);
	
D 3
	TipcSrvMsgWrite("/EClaser/progress",TipcMtLookupByNum(T_MT_INFO),    
        TRUE, T_IPC_FT_STR,
   	&kmessage,
  	 NULL);
	   sprintf(prog_msg," prog message = %d",jj);
	 ss_prog(prog_msg,1);
	 TipcSrvFlush();  
E 3
I 3
	TipcSrvMsgWrite("/EClaser/progress",TipcMtLookupByNum(T_MT_INFO),
			TRUE, T_IPC_FT_STR,
			&kmessage,
			NULL);
	sprintf(prog_msg," prog message = %d",jj);
	ss_prog(prog_msg,1);
	TipcSrvFlush();
E 3
 	sleep(5);
	ss_prog(prog_msg,1);
 	jj++;
D 3
 
E 3
     }
D 3
 TipcSrvDestroy(T_IPC_SRV_CONN_NONE);  
   
 
   
E 3
I 3

 TipcSrvDestroy(T_IPC_SRV_CONN_NONE);

E 3
 exit;
D 3
	
E 3
I 3

E 3
}
I 3

E 3
E 1
