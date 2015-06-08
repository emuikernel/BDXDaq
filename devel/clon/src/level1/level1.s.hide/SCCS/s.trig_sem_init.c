h14402
s 00011/00000/00052
d D 1.3 07/03/20 16:40:13 boiarino 4 3
c *** empty log message ***
e
s 00008/00004/00044
d D 1.2 04/02/03 20:56:20 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/10/22 12:58:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/trig_sem_init.c
e
s 00048/00000/00000
d D 1.1 01/10/22 12:58:16 boiarino 1 0
c date and time created 01/10/22 12:58:16 by boiarino
e
u
U
f e 0
t
T
I 4

#ifdef VXWORKS

E 4
I 1
/* trig_sem_init.c - TCP server code that runs on VxWorks */

/* use VxWorks header files */ 
I 3
#include <stdio.h>
#include <string.h>

E 3
#include <vxWorks.h> 
#include <sys/socket.h> 
#include <in.h> 
#include <inetLib.h> 
D 3
#include <string.h>
E 3
#include <vxLib.h>
#include <semLib.h>

static SEM_ID trigSem;
D 3
//change 18
void trig_sem_init () {
    trigSem = semBCreate (SEM_FULL,SEM_Q_FIFO);
E 3
I 3

void
trig_sem_init()
{
  trigSem = semBCreate(SEM_FULL,SEM_Q_FIFO);
E 3
}

I 4
#else
E 4

I 4
void
trig_sem_init_dummy()
{
  return;
}
E 4

I 4
#endif
E 4

I 4

E 4




























E 1
