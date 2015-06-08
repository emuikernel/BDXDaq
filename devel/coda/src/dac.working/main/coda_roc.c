
#include "cinclude/ROC_class.c"
#include "cinclude/CODA_class.c"
/*#include "cinclude/LINK_class.c"*/
#include "cinclude/roc_component.c"

/****************/
/* main program */
/****************/

#ifdef VXWORKS

/*
#include <cacheLib.h>
*/

void
coda_roc (char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6, char *arg7)
{
	int            argc = 1;
	char           *argv[8];

	/* Copy arguments into argv array */

	argv[0] = "coda_roc";
	if(arg1) {
	  argv[1] = arg1;
	  argc++;
	}
	if(arg2) {
	  argv[2] = arg2;
	  argc++;
	}
	if(arg3) {
	  argv[3] = arg3;
	  argc++;
	}
	if(arg4) {
	  argv[4] = arg4;
	  argc++;
	}
	if(arg5) {
	  argv[5] = arg5;
	  argc++;
	}
	if(arg6) {
	  argv[6] = arg6;
	  argc++;
	}
	if(arg7) {
	  argv[7] = arg7;
	  argc++;
	}
	
	if (argc < 5) {
	  printf (" Wrong number of arguments (must be >= 5) argc = %d\n",argc);
	}else if (argc == 5) { 
	  printf (" Args = %s %s %s %s \n", argv[1], argv[2], argv[3], argv[4]);
	} else if (argc == 6) {
	  printf (" Args = %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
	} else if (argc == 7){
	  printf (" Args = %s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	} else {
	  printf (" Args = %s %s %s %s %s %s %s\n", 
		  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
	}


	/*
printf("Disable L1 cache\n");
cacheDisable(DATA_CACHE);
	*/

/**********/


#else

#include "tcpServer.c"

void
main (int argc, char **argv)
{
#endif


	CODA_Init (argc, argv);
	/* CODA_Service ("ROC"); */
printf("main 1\n");
	CODA_Execute ();
printf("main 2\n");
}









