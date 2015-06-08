h52088
s 00000/00000/00000
d R 1.2 02/08/26 22:05:08 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/demos/hvdraw/main.c
e
s 00043/00000/00000
d D 1.1 02/08/26 22:05:07 boiarino 1 0
c date and time created 02/08/26 22:05:07 by boiarino
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
#include "draw.h"

/* ----- main program -------*/

void main(unsigned int argc,
	  char **argv)

{
  Hv_VaInitialize(argc, argv, NULL);
  Init();
  Hv_Go();
}















E 1
