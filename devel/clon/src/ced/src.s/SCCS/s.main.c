h04181
s 00000/00000/00051
d D 1.2 07/11/12 16:41:06 heddle 3 1
c new start counter
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:11 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/main.c
e
s 00051/00000/00000
d D 1.1 02/09/09 16:27:10 boiarino 1 0
c date and time created 02/09/09 16:27:10 by boiarino
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
