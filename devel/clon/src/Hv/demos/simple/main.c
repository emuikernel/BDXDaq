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
#include "simple.h"

/* ----- main program -------*/

void main(unsigned int argc,
	  char **argv)


{
  Hv_VaInitialize(argc, argv,
		  Hv_WELCOMETEXT,       "version 1.0",
		  Hv_USEVIRTUALVIEW,    True,
		  Hv_LEFT,              150,
		  Hv_LEFT,              100,
		  Hv_WIDTH,             850,
		  Hv_HEIGHT,            875,
		  Hv_USEEXTENDEDCOLORS, True,
		  NULL);
  simpleInit();
  Hv_Go();
}















