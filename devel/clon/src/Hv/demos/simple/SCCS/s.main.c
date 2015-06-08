h65509
s 00000/00000/00000
d R 1.2 02/08/26 22:35:27 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/simple/main.c
e
s 00052/00000/00000
d D 1.1 02/08/26 22:35:26 boiarino 1 0
c date and time created 02/08/26 22:35:26 by boiarino
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















E 1
