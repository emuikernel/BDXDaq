
/* vxCompat.c */

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <bootLib.h>
#include <sysLib.h>
#include "tclInt.h"
#include "tclPort.h"

/*
 * compatability routines for Unix code
 */


#ifdef NEED_ACCESS	/* now found in softclock/stubs.c */
/*
 * a very limited implementation of the Unix system call,
 * sufficient for use by TCL
 */
int
access(char *name, int mode)
{
  int fd;
    
  switch (mode)
  {
	case R_OK:
	    if ((fd = open(name, O_RDONLY, 0)) < 0) 
	    {
		return(-1);
	    }
	    (void) close(fd);
	    break;
	case W_OK:
	    if ((fd = open(name, O_WRONLY, 0)) < 0) 
	    {
		return(-1);
	    }
	    (void) close(fd);
	    break;
	default:
	    return(-1);
  }
  return(0);
}
#endif

/*****************************************************
 * hostname
 *
 * Provided to implement the unix "hostname" command.
 * In this case, host refers to the vxWorks target on
 * which this command is implemented.
 *****************************************************/


void
hostname()
{
  fprintf(stdout, "%s\n", sysBootParams.targetName);
  return;
}

/*
 * provided time in ticks for timing purposes
 */
 
#include <tickLib.h>

int
times(struct tms *dummy)
{
  return(tickGet());
}

#ifdef NOTDEF
abort()
{
  logMsg ("abort called - task suspended\n");
  taskSuspend(0);
}
#endif
    

char *
mystrdup(const char *s)
{
  char *string = NULL;
    
  if(s == 0) return NULL;

  if ((string = (char *) malloc(strlen(s) + 1)) == NULL)
  {
	printf("strdup: failed malloc in string duplication\n");
	return NULL;
  }
    
  strcpy(string, s);

  return string;
}

#else

void
vxCompat_dummy()
{
  return;
}

#endif /* VXWORKS */

