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
#include "draw.h"

/* ----- main program -------*/

void main(unsigned int argc,
	  char **argv)

{
  Hv_VaInitialize(argc, argv, NULL);
  Init();
  Hv_Go();
}















