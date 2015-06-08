h28242
s 00000/00000/00000
d R 1.2 00/09/21 14:12:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/ss_init.c
e
s 00042/00000/00000
d D 1.1 00/09/21 14:12:41 wolin 1 0
c CodeManager Uniquification: laser/ec/src/c_cntl/lib/ss_init.c
c date and time created 00/09/21 14:12:41 by wolin
e
u
U
f e 0
t
T
I 1
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
 
 
E 1
