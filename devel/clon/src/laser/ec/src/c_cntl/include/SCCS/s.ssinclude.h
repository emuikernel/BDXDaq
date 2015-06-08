h14608
s 00000/00000/00000
d R 1.2 00/09/21 14:12:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/ssinclude.h
e
s 00059/00000/00000
d D 1.1 00/09/21 14:12:20 wolin 1 0
c date and time created 00/09/21 14:12:20 by wolin
e
u
U
f e 0
t
T
I 1
/* RT server include and the CLAS ipc */
#include <rtworks/ipc.h>
#include <clas_ipc_prototypes.h> 





/* *********************
      variables for RTserver smart sockets

   for include files you need to initialize variables once
     main routines will have INIT=1
     subroutines will have INIT=0
   variales will not be given values in the subroutine compile. 

   #if defined(INIT) 
		int variable=100;
   #elif !defined(INIT)  		
		extern  int variable;
   #endif

A variable must be defined external if other source files refer to
it. It can be declared extern any number of times but only needs 1
declaration per source file.  The key is to be sure to define it
ie give it a value only once. (Same for procedures. You
must list the procedure in one place only  xxx{.....} but
you can declare it extern any number of times. ).

*****************   */
#if defined(INIT) 
T_OPTION opt;
T_IPC_MSG msg;
T_STR status_id[100][50];
T_INT4 status_val[100];
T_INT4 nopairs=0;

char ECevent_subject[]="evt_bosbank/clasprod";

#elif !defined(INIT)

extern T_OPTION opt;
extern T_IPC_MSG msg;
extern T_STR status_id[100][50];
extern T_INT4 status_val[100];
extern T_INT4 nopairs;

extern char ECevent_subject[]; 
    
       
#endif 



extern  int   boswrite(T_INT4 buf[], T_INT4 ec_nrow);
extern  int   ss_init(char* project, int debug);
extern  int   ss_prog(char* msg_prog, int broadcastflg);
extern  int   ss_status( void);
extern  int   add_status(T_STR stringvar[], T_INT4 intval);
E 1
