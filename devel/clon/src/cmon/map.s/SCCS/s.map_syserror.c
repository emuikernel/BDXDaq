h51804
s 00000/00000/00000
d R 1.2 03/05/23 12:18:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_syserror.c
e
s 00084/00000/00000
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
        map_syserror.c
         Created           :  23-SEP-1993 by Thom Sulanke
*/
 
#include <stdio.h>                  /* I/O definitions                       */
#include <string.h>                 /* I/O definitions                       */
#include <stdarg.h>
#include <errno.h>
#ifdef vaxc
#include <perror.h>
#endif

#include "map_manager.h"
#include "map_internal.h"
#include "map_error_log.h"

/***********************************************************************/
/*                                                                     */
/*   MAP_SYSERROR                                                      */
/*   ------------                                                      */
/*                                                                     */
/*         Created     :  1-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     :  print error message for system error         */
/*                                                                     */
/***********************************************************************/

/* Sergey: Solaris:
 * The symbols _sys_errlist and _sys_nerr are not visible in the
 * LP64 libc.  Use strerror(3C) instead.
 */
 
void map_syserror(int error_code, char *fmt, ... )
 
{
	va_list args;

	extern int errno;
    /*extern int sys_nerr;*/
#ifndef Linux
#ifndef RedHat5
#ifndef EGCS
	/*extern char *sys_errlist[];*/
#endif
#endif
#endif
	
	switch ( error_code ) {
	case MAP_SYSTEM_ERROR_OPEN:
	  if ( map_system_error_log[MAP_SYSTEM_ERROR_OPEN-error_code] )
	    fprintf(stderr, "Error code = MAP_SYSTEM_ERROR_OPEN\n");
	  else
	    return;
	  break;
	case MAP_SYSTEM_ERROR_IO:
	  if ( map_system_error_log[MAP_SYSTEM_ERROR_OPEN-error_code] )
	    fprintf(stderr, "Error code = MAP_SYSTEM_ERROR_IO\n");
	  else
	    return;
	  break;
	case MAP_SYSTEM_ERROR_MEMORY:
	  if ( map_system_error_log[MAP_SYSTEM_ERROR_OPEN-error_code] )
	    fprintf(stderr, "Error code = MAP_SYSTEM_ERROR_MEMORY\n");
	  else
	    return;
	  break;
        default:
	  fprintf(stderr, "Unexpected Error code = %d.\n",error_code);
	  break;
	}

	va_start(args,fmt);
	fprintf(stderr, "system error: ");
	vfprintf(stderr, fmt, args);

    fprintf(stderr, " (%s)", strerror(errno));
	/*
	if ( errno > 0 && errno < sys_nerr )
		fprintf(stderr, " (%s)", sys_errlist[errno]);
	*/

	fprintf(stderr, "\n");
	va_end(args);
}
E 1
