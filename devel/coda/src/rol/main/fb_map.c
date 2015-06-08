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
#include <libtcp.h>

int
main(int argc, char *argv[])
{
  static char     roc[21];
  char            result[10000];
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
