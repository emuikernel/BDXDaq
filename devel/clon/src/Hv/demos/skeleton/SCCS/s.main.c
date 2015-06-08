h53763
s 00000/00000/00000
d R 1.2 02/08/26 22:43:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/skeleton/main.c
e
s 00045/00000/00000
d D 1.1 02/08/26 22:42:59 boiarino 1 0
c date and time created 02/08/26 22:42:59 by boiarino
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















E 1
