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
-  08/24/95	DPH		Initial version
---------------------------------------------------------------
*/

#include "Hv.h"
#include "maps.h"


/* ----- main program -------*/

void main(argc, argv)

unsigned int argc;
char        *argv[];

{
  Hv_VaInitialize(argc, argv,
		  Hv_NORAINBOW,      True,
		  Hv_USEWELCOMEVIEW, True,
		  Hv_VERSIONNUMBER,  101,
		  NULL);
  Init();
  Hv_Go();
}















