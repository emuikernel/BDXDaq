
/* vxCompat.c */

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <vxWorks.h>
#include <bootLib.h>
#include <sysLib.h>
#include <tickLib.h>

/*
 * compatability routines for Unix code
 */

/* sergey: copy from UNIX's unistd.h (???) */
/* Symbolic constants for the "access" routine: */
#define	R_OK	4	/* Test for Read permission */
#define	W_OK	2	/* Test for Write permission */
#define	X_OK	1	/* Test for eXecute permission */
#define	F_OK	0	/* Test for existence of File */

/*
 * a very limited implementation of the Unix system call,
 * sufficient for use by TCL
 */
int
access(char *name, int mode)
{
  int fd;

  logMsg("access reached !!!\n",1,2,3,4,5,6);
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

  return(string);
}


int
strncasecmp(const char *s1, const char *s2, size_t n)
{
  int c1, c2, l=0;

  while (*s1 && *s2 && l < n)
  {
	c1 = tolower(*s1);
	c2 = tolower(*s2);
	if (c1 != c2) return(c1 - c2);
	s1++;
	s2++;
	l++;
  }

  return(0);
}


#else

void
vxCompat_dummy()
{
  return;
}

#endif /* VXWORKS */

