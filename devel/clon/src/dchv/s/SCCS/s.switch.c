h26120
s 00007/00008/00020
d D 1.3 03/12/16 22:28:50 boiarino 4 3
c .
e
s 00013/00005/00015
d D 1.2 03/09/24 15:05:27 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dchv/vme/s/switch.c
e
s 00020/00000/00000
d D 1.1 00/08/03 10:40:41 gurjyan 1 0
c date and time created 00/08/03 10:40:41 by gurjyan
e
u
U
f e 0
t
T
I 3

/* switch.c */

E 3
I 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

D 4
#ifndef ushort
#define ushort unsigned short
#endif
E 4

I 3
#ifdef VXWORKSPPC
D 4
ushort *base = (ushort *)0xfbff0DA0;
E 4
I 4
volatile UINT16 *base = (ushort *)0xfbff0DA0;
E 4
#else
E 3
D 4
ushort *base = (ushort *)0xffff0DA0;
E 4
I 4
UINT16 *base = (ushort *)0xffff0DA0;
E 4
I 3
#endif
E 3

/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
D 3
int sw(ushort vmedat)
E 3
I 3
int
D 4
sw(ushort vmedat)
E 4
I 4
sw(UINT16 vmedat)
E 4
E 3
{
D 3
        (ushort)*base = (ushort)vmedat;

return 0;
      }
E 3
I 3
D 4
printf("sw reached\n");
  (ushort)*base = (ushort)vmedat;
E 4
I 4
  /*printf("sw reached\n");*/

  *base = vmedat;

E 4
  return(0);
}
E 3
E 1
