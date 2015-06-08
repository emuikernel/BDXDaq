h18700
s 00000/00000/00000
d R 1.2 03/05/23 12:18:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_warn.c
e
s 00062/00000/00000
d D 1.1 03/05/23 12:18:02 boiarino 1 0
c date and time created 03/05/23 12:18:02 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_warn.c
         Created           : 17-DEC-1992 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdarg.h>
#include "map_manager.h"
#include "map_internal.h"
#include "map_error_log.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_WARN                                                          */
/*   --------                                                          */
/*                                                                     */
/*         Created     : 17-DEC-1992    Author : Thom Sulanke          */
/*         Purpose     :  print warning message                        */
/*                                                                     */
/***********************************************************************/
 
void map_warn(int error_code, char *fmt, ... )
 
{
	va_list args;

	switch ( error_code ) {
	case MAP_USER_WARN_NOREPEAT:
	  if ( map_user_warn_log[MAP_USER_WARN_NOREPEAT-error_code] )
	    fprintf(stderr, "Error code = MAP_USER_WARN_NOREPEAT\n");
	  else
	    return;
	  break;
	case MAP_USER_WARN_NOFIND:
	  if ( map_user_warn_log[MAP_USER_WARN_NOREPEAT-error_code] )
	    fprintf(stderr, "Error code = MAP_USER_WARN_NOFIND\n");
	  else
	    return;
	  break;
	case MAP_USER_WARN_NOMATCH:
	  if ( map_user_warn_log[MAP_USER_WARN_NOREPEAT-error_code] )
	    fprintf(stderr, "Error code = MAP_USER_WARN_NOMATCH\n");
	  else
	    return;
	  break;
	case MAP_USER_WARN_NOREPLACE:
	  if ( map_user_warn_log[MAP_USER_WARN_NOREPEAT-error_code] )
	    fprintf(stderr, "Error code = MAP_USER_WARN_NOREPLACE\n");
	  else
	    return;
	  break;
        default:
	  fprintf(stderr, "Unexpected Error code = %d.\n",error_code);
	  break;
	}

	va_start(args,fmt);
	fprintf(stderr, "warning: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}
E 1
