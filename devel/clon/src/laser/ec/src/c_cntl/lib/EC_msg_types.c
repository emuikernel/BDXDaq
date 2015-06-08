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
