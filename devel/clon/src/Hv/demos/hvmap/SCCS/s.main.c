h58827
s 00000/00000/00000
d R 1.2 02/08/26 22:12:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvmap/main.c
e
s 00050/00000/00000
d D 1.1 02/08/26 22:12:16 boiarino 1 0
c date and time created 02/08/26 22:12:16 by boiarino
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















E 1
