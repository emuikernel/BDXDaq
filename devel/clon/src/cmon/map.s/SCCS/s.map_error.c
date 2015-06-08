h42854
s 00000/00000/00000
d R 1.2 03/05/23 12:18:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_error.c
e
s 00045/00000/00000
d D 1.1 03/05/23 12:17:59 boiarino 1 0
c date and time created 03/05/23 12:17:59 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_error.c
         Created           :  1-OCT-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdarg.h>
#include "map_manager.h"
#include "map_internal.h"
#include "map_error_log.h"


/***********************************************************************/
/*                                                                     */
/*   MAP_ERROR                                                         */
/*   -----                                                             */
/*                                                                     */
/*         Created     :  1-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     :  print error message for non-system error     */
/*                                                                     */
/***********************************************************************/
 
void map_error(int error_code, char *fmt, ... )
 
{
	va_list args;

	switch ( error_code ) {
	case MAP_USER_ERROR_ARGUMENT:
	  if ( map_user_error_log[MAP_USER_ERROR_ARGUMENT-error_code] )
	    fprintf(stderr, "Error code = MAP_USER_ERROR_ARGUMENT\n");
	  else
	    return;
	  break;
        default:
	  fprintf(stderr, "Unexpected Error code = %d.\n",error_code);
	  break;
	}

	va_start(args,fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}
E 1
