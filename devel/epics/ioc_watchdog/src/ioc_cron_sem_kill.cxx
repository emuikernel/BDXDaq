
/* ioc_cron_sem_kill.cxx */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <limits.h>

#include "string"
#include <semaphore.h>

int
main(int argc, char *argv[])
{
  int ret;

  if(argc!=2)
  {
    printf("Usage: ioc_cron_sem_kill <semaphore name>\n");
    exit(0);
  }

  printf("trying to kill semaphore %s\n",argv[1]);

  ret = sem_unlink(argv[1]);
  if(ret<0)
  {
    perror("unable to kill semaphore");
  }
  else
  {
    printf("semaphore >%s< has been killed\n",argv[1]);
  }

  exit(0);
}
