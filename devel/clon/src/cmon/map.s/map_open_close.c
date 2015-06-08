/* 
        map_open_close.c
         Created           :   4-Mar-1994 by Thom Sulanke
	                      contains map_open_ro, map_open_ro_lock, map_open_rw, and 
			      map_close_map for the sharing of saved_handler.
*/
 
#include <stdio.h>                   /* I/O definitions                      */

#if defined(vaxc)
#include <file.h>
#else
#include <fcntl.h>

#if !defined(readonly)
#include <signal.h>
    static void (*saved_handler)(int);
#endif

#endif

#include "map_manager.h"
#include "map_internal.h"
#include <unistd.h>
#include <stdlib.h>
 

/***********************************************************************/
/*                                                                     */
/*   MAP_OPEN_RO                                                       */
/*   -----------                                                       */
/*                                                                     */
/*         Created     :  17-DEC-1992    Author : Thom Sulanke         */
/*         Purpose     :  open the map for read only (unlocked)        */
/*                                                                     */
/***********************************************************************/
 
int map_open_ro( const char filename[])
{
  int fd ;
  
  int icntl;
  
  /* open file for read only */
 
  fd = open(filename,O_RDONLY);

  if ( fd == -1 ) 
    return MAP_SYSTEM_ERROR_OPEN;

  return fd;
}
 

/***********************************************************************/
/*                                                                     */
/*   MAP_OPEN_RO_LOCK                                                  */
/*   ----------------                                                  */
/*                                                                     */
/*         Created     :  17-DEC-1992    Author : Thom Sulanke         */
/*         Purpose     :  open the map for read only (locked)          */
/*                                                                     */
/***********************************************************************/
 
int map_open_ro_lock( const char filename[])
{
    int fd ;

#if !defined(vaxc) && !defined(readonly)
    struct flock lock;
#endif

    int icntl;
 
    /* open file for read only */
    
    fd = open(filename,O_RDONLY);
    
    if ( fd == -1 ) 
      return MAP_SYSTEM_ERROR_OPEN;
    
#if !defined(vaxc) && !defined(readonly)
    /* put read lock on file */
    
    saved_handler = signal(SIGUSR1,SIG_DFL);
    /*
       if ( saved_handler != SIG_DFL )
       {
       fprintf(stderr, "Handler for SIGUSR1 was not SIG_DFL.\n");
       fprintf(stderr, "Handler for SIGUSR1 has been set to SIG_DFL by map_open_ro.\n");
       }
       */
    
    lock.l_type = F_RDLCK;
    lock.l_whence = 0;
    lock.l_start = 0;
    lock.l_len = 0;
    icntl = fcntl(fd,F_SETLKW,&lock);
    if ( icntl == -1 )
      {
	map_syserror(MAP_SYSTEM_ERROR_IO,
		     "Can't put read lock on TheMap.");
	close(fd);
	return MAP_SYSTEM_ERROR_IO;
      }
#endif
    
    return fd;
}
 

#if !defined(readonly)
/***********************************************************************/
/*                                                                     */
/*   MAP_OPEN_RW                                                       */
/*   -----------                                                       */
/*                                                                     */
/*         Created     :  17-DEC-1992    Author : Thom Sulanke         */
/*         Purpose     :  open the map for read and write              */
/*                                                                     */
/***********************************************************************/
 
int map_open_rw( const char filename[])
{
    int fd ;

#if !defined(vaxc) && !defined(readonly)
    struct flock lock;
#endif

    int icntl;
  
/* open file for read and write */
 
    fd = open(filename,O_RDWR,0);

    if ( fd == -1 ) 
      return MAP_SYSTEM_ERROR_OPEN;

#if !defined(vaxc) && !defined(readonly)
/* put write lock on file */

    saved_handler = signal(SIGUSR1,SIG_DFL);
/*
    if ( saved_handler != SIG_DFL )
      {
	fprintf(stderr, "Handler for SIGUSR1 was not SIG_DFL.\n");
	fprintf(stderr, "Handler for SIGUSR1 has been set to SIG_DFL by map_open_rw.\n");
      }
*/

    lock.l_type = F_WRLCK;
    lock.l_whence = 0;
    lock.l_start = 0;
    lock.l_len = 0;
    icntl = fcntl(fd,F_SETLKW,&lock);
    if ( icntl == -1 )
      {
	map_syserror(MAP_SYSTEM_ERROR_IO,
		     "Can't put write lock on TheMap.");
	close(fd);
	return MAP_SYSTEM_ERROR_IO;
      }
#endif

    return fd;
}
#endif

/***********************************************************************/
/*                                                                     */
/*   MAP_CLOSE_MAP                                                  */
/*   ----------------                                                  */
/*                                                                     */
/*         Created     :  1-OCT-1992    Author : Thom Sulanke          */
/*         Purpose     : close the map file                            */
/*                                                                     */
/***********************************************************************/
 
int map_close_map( int fd )
 
{
    if ( close(fd) == -1 )
    {
        map_syserror(MAP_SYSTEM_ERROR_IO,"can't close TheMap");
        return MAP_SYSTEM_ERROR_IO;
    }
#if !defined(vaxc) && !defined(readonly)
    if ( saved_handler != SIG_DFL )
      saved_handler = signal(SIGUSR1,saved_handler);
#endif
    return MAP_OK;
}



