h41179
s 00000/00000/00061
d D 1.3 07/03/13 10:21:17 sergpozd 3 2
c added libtcp.h
c 
e
s 00001/00000/00060
d D 1.2 07/03/13 10:20:34 sergpozd 2 1
c *** empty log message ***
e
s 00060/00000/00000
d D 1.1 06/08/24 08:22:31 boiarino 1 0
c date and time created 06/08/24 08:22:31 by boiarino
e
u
U
f e 0
t
T
I 1
/*     fb_map.c
 *
 * Function is looking for ADC's and TDC's in any FASTBUS crate.
 * Output of this function is an output file roc_***.hardmap, which 
 * placed in directory $CLON_PARMS/fb_boards/, where *** in output
 * file means "rocname" and is the input parameter of this function.
 *
 * input of this function:
 *     roc = "name of necessary ROC".
 *
 *
 *          04/09/99 by Serguei P.
 */

#ifndef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
I 2
#include <libtcp.h>
E 2

int
main(int argc, char *argv[])
{
  static char     roc[21];
  char            result[1000];
  char            function[130];

  char help[1000] = " \n"
   "Usage: fb_map roc \n\n"
   "      ---------------------- \n"
   "Where \"roc\" is a ROC name. \n"
   "      ---------------------- \n";

   
   if ( argc != 2 ) {
     printf("%s \n", help);
     return (1); }
   else
     strcpy(roc,argv[1]);
   

   sprintf(function,"fb_map(\"%s\")", roc);
   /*
   printf(" roc = %s \n", roc);
   printf(" function =%s \n", function);
   */
   tcpClientCmd(roc,function,result);
   
   printf("%s \n\n",result);
}

#else /* no VXWORKS version */

void
fb_map_dummy()
{
  return;
}

#endif
E 1
