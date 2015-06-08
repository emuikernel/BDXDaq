h24761
s 00000/00000/00000
d R 1.2 03/05/23 12:18:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/map.s/map_log_mess.c
e
s 00066/00000/00000
d D 1.1 03/05/23 12:18:01 boiarino 1 0
c date and time created 03/05/23 12:18:01 by boiarino
e
u
U
f e 0
t
T
I 1
/* 
        map_log_mess.c
         Created           : 11-FEB-1994 by Thom Sulanke
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdarg.h>
#include "map_manager.h"
#include "map_internal.h"
#include "map_error_log.h"

int map_user_error_log[MAP_USER_ERROR_ARGUMENT - MAP_USER_ERROR_ARGUMENT+1] = {1};
int map_user_warn_log[MAP_USER_WARN_NOREPEAT - MAP_USER_WARN_NOREPLACE+1] = {1,1,1,1};
int map_system_error_log[MAP_SYSTEM_ERROR_OPEN - MAP_SYSTEM_ERROR_MEMORY+1] = {1,1,1};

/***********************************************************************/
/*                                                                     */
/*   MAP_LOG_MESS                                                      */
/*   -----                                                             */
/*                                                                     */
/*         Created     : 11-FEB-1994    Author : Thom Sulanke          */
/*         Purpose     :  Controls whether error message are printed.  */
/*                        error_code is the code of the error whose    */ 
/*                        message is to be turned on or off.           */
/*                        error_code = 0 affects all messages.  The    */
/*                        logical value of log determines whether or   */
/*                        not the message is to be printed.            */
/*                                                                     */
/***********************************************************************/
 
int map_log_mess(const int error_code, const int log )
 
{

	switch ( error_code ) {
	case 0:
	  map_user_error_log[MAP_USER_ERROR_ARGUMENT - MAP_USER_ERROR_ARGUMENT] = log;
	  map_user_warn_log[MAP_USER_WARN_NOREPEAT - MAP_USER_WARN_NOREPEAT] = log;
	  map_user_warn_log[MAP_USER_WARN_NOREPEAT - MAP_USER_WARN_NOFIND] = log;
	  map_user_warn_log[MAP_USER_WARN_NOREPEAT - MAP_USER_WARN_NOMATCH] = log;
	  map_user_warn_log[MAP_USER_WARN_NOREPEAT - MAP_USER_WARN_NOREPLACE] = log;
	  map_system_error_log[MAP_SYSTEM_ERROR_OPEN - MAP_SYSTEM_ERROR_OPEN] = log;
	  map_system_error_log[MAP_SYSTEM_ERROR_OPEN - MAP_SYSTEM_ERROR_IO] = log;
	  map_system_error_log[MAP_SYSTEM_ERROR_OPEN - MAP_SYSTEM_ERROR_MEMORY] = log;
	  break;
	case MAP_USER_ERROR_ARGUMENT:
	  map_user_error_log[MAP_USER_ERROR_ARGUMENT-error_code] = log;
	  break;
	case MAP_USER_WARN_NOREPEAT:
	case MAP_USER_WARN_NOFIND:
	case MAP_USER_WARN_NOMATCH:
	case MAP_USER_WARN_NOREPLACE:
	  map_user_warn_log[MAP_USER_WARN_NOREPEAT-error_code] = log;
	  break;
	case MAP_SYSTEM_ERROR_OPEN:
	case MAP_SYSTEM_ERROR_IO:
	case MAP_SYSTEM_ERROR_MEMORY:
	  map_system_error_log[MAP_SYSTEM_ERROR_OPEN-error_code] = log;
	  break;
        default:
	  fprintf(stderr, "map_log_mess: Unexpected Error code = %d.\n",error_code);
	  return MAP_USER_ERROR_ARGUMENT;
	}

	return MAP_OK;
}
E 1
