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


/* ----- main program -------*/

void main(int argc,
	  char **argv)

{
  Hv_VaInitialize(argc, argv,
		  Hv_WELCOMETEXT, "version 1.0",
		  Hv_USEWELCOMEVIEW, True,
		  NULL);
  Hv_Go();
}















