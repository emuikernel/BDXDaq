h53608
s 00051/00000/00000
d D 1.1 07/10/16 23:57:40 boiarino 1 0
c date and time created 07/10/16 23:57:40 by boiarino
e
u
U
f e 0
t
T
I 1
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














E 1
