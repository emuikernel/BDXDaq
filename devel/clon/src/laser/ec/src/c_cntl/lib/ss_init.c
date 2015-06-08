 /*  init connection  to RTserever */


 
/* for smartsockets */
#include <ssinclude.h>


 int   ss_init(char* project, int debug)
{
  int j1;
  float kl;

	/* read Smartsockets license file */

  	printf("initializing the socket license \n");
  	TutCommandParseStdLicense();


  	/* set project */
  	printf("set project = %s \n", project);
  	opt=TutOptionLookup("project");
  	TutOptionSetEnum(opt,project);

 	/* try to connect with RT server */
 
 	if(!TipcSrvCreate(T_IPC_SRV_CONN_FULL)) {
     		TutOut( "cannot  connect to RT server. \n");
     		printf("cannot connect to RT server. \n");
     	 	return(801);		
     	}

/* init standard message types this routine is inclded with clas_ipc_prototypes.h */
	j1=init_msg_types(); 

	printf("done with ss_init returning \n");


 return (1); /* return 1 if success */
} 
 
 
