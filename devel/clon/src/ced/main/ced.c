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
-  10/19/94	DPH		Initial version
-  11/15/96     DPH             Added "Hv_TRAPCOMMANDARGS" fo
-                               Arne's request
---------------------------------------------------------------
*/

#include "ced.h"

/* ----- main program -------*/

#ifndef MLIB

void main(unsigned int argc,
           char    *argv[])

{
  Hv_VaInitialize(argc, argv,
		  Hv_USEEXTENDEDCOLORS,       True,
		  Hv_USEVIRTUALVIEW,          True,
		  Hv_TRAPCOMMANDARGS,         True,
		  NULL);
  Init();
  Hv_Go();
}

#endif














