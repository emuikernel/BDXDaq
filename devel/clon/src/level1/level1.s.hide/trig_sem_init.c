
#ifdef VXWORKS

/* trig_sem_init.c - TCP server code that runs on VxWorks */

/* use VxWorks header files */ 
#include <stdio.h>
#include <string.h>

#include <vxWorks.h> 
#include <sys/socket.h> 
#include <in.h> 
#include <inetLib.h> 
#include <vxLib.h>
#include <semLib.h>

static SEM_ID trigSem;

void
trig_sem_init()
{
  trigSem = semBCreate(SEM_FULL,SEM_Q_FIFO);
}

#else

void
trig_sem_init_dummy()
{
  return;
}

#endif






























