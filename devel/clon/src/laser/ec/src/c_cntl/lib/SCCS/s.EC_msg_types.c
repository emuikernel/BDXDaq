h10901
s 00000/00000/00000
d R 1.2 00/09/21 14:12:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/EC_msg_types.c
e
s 00038/00000/00000
d D 1.1 00/09/21 14:12:39 wolin 1 0
c date and time created 00/09/21 14:12:39 by wolin
e
u
U
f e 0
t
T
I 1
/* 
 *    init_msg_types
 *
 *  initializes clas standard message types
 *
 *  ejw, 25-mar-97
 *
 */


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <clas_ipc_mt.h>
#include <rtworks/ipc.h>


/*---------------------------------------------------------------------*/


int init_msg_types(void){

  int i;
  T_IPC_MT mt;

  /* create clas standard message types and register in status or data catagory */
  for(i=0; i<sizeof(ipc_msg_type)/sizeof(struct clas_standard_msg_type); i++){
 
    mt=TipcMtCreate(ipc_msg_type[i].mtname,ipc_msg_type[i].mtnum,"verbose");
    if((mt==NULL)&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))return(1);
 
  }

  return;
}

/*---------------------------------------------------------------------*/
E 1
