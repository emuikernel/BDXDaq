h39407
s 00010/00000/00071
d D 1.2 07/03/20 16:37:31 boiarino 2 1
c *** empty log message ***
e
s 00071/00000/00000
d D 1.1 06/07/02 09:08:31 boiarino 1 0
c date and time created 06/07/02 09:08:31 by boiarino
e
u
U
f e 0
t
T
I 1

I 2
#ifndef VXWORKS

E 2
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

I 2
#else
E 2

I 2
void
Tigris_PPC_dummy()
{
  return;
}
E 2

I 2
#endif
E 2

I 2

E 2











E 1
