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

#include "Hv.h"
#include "plot.h"

/* ----- main program -------*/

void main(int argc,
	  char **argv)
{
  Hv_VaInitialize(argc, argv,
		  Hv_TRAPCOMMANDARGS, True,
		  Hv_HEIGHT,          1040,
		  NULL);
  init();
  Hv_Go();
}















