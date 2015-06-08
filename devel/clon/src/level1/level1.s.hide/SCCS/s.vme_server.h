h04335
s 00000/00001/00012
d D 1.4 08/04/01 14:53:31 boiarino 5 4
c *** empty log message ***
e
s 00000/00003/00013
d D 1.3 08/04/01 14:08:00 boiarino 4 3
c remove obsoleted prototypes
c 
e
s 00010/00000/00006
d D 1.2 02/07/08 10:44:50 boiarino 3 1
c add function prototypes
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 13:00:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_server.h
e
s 00006/00000/00000
d D 1.1 01/10/22 13:00:29 boiarino 1 0
c date and time created 01/10/22 13:00:29 by boiarino
e
u
U
f e 0
t
T
I 3

E 3
I 1
/* vme_server.h - TCP server header file */

#define SERVER_WORK_PRIORITY (100)
#define SERVER_STACK_SIZE (10000)
#define MAX_QUEUED_CONNECTIONS (1)
I 3

void   get_tigris_status();
void   init_tigris_status();
D 4
STATUS mem_set(void);
STATUS mem_zero(void);
E 4
D 5
STATUS sreg_d(void);
E 5
STATUS vme_server(void);
STATUS trig_sem_init(void);
D 4
STATUS trig_init(void);
E 4
E 3

E 1
