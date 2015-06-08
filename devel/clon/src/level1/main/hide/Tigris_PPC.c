
#ifndef VXWORKS

/*
-----------------------------------------------------------
-
-   File:     main.c
-
-
-								
-  Revision history:						
-                     						
-  Date       Programmer     Comments				
----------------------------------------------------------------
-  6/28/94	DPH		Initial version
---------------------------------------------------------------
*/

#include <stdio.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

/* ----- main program -------*/

static void
usage()
{
  printf ("usage: tigrisLite [-c] <target host> <trigger filename>\n");
  printf ("\n\twhere:\n\t\t-c = optional: answer the last downloaded trigger file\n");
  printf ("\t\ttarget host = the host name to download to\n");
  printf ("\t\ttrigger filename = name of the trigger to download\n");
  exit (-1);
}

void
main(unsigned int argc, char *argv[])
{
  if (argc > 1) 
    if (*argv[1] == '-') {
      if ((strncmp (argv[1], "-c", 2)) == 0) {
	system ("more Current.Config");
	exit (0);
      } else
	usage ();
    }

  Hv_VaInitialize(argc, argv,
		  Hv_USEVIRTUALVIEW, True, 
		  Hv_VIRTUALHEIGHT, 2500,
		  Hv_VIRTUALVIEWFBDATACOLS, 25,
		  Hv_USEEXTENDEDCOLORS, True,
		  NULL);
  simpleInit();
  Hv_Go();

  exit(0);
}

#else

void
Tigris_PPC_dummy()
{
  return;
}

#endif













