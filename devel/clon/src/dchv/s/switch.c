
/* switch.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#ifdef VXWORKSPPC
volatile UINT16 *base = (ushort *)0xfbff0DA0;
#else
UINT16 *base = (ushort *)0xffff0DA0;
#endif

/*----------------------------------------------------------*/
/*---------- Send commands to caen function ----------------*/
/*----------------------------------------------------------*/
int
sw(UINT16 vmedat)
{
  /*printf("sw reached\n");*/

  *base = vmedat;

  return(0);
}
