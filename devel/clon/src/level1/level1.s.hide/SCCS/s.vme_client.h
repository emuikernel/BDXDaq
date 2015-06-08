h63750
s 00002/00002/00011
d D 1.3 06/09/01 00:37:38 boiarino 4 3
c nothing
c 
e
s 00002/00000/00011
d D 1.2 02/06/29 18:06:29 boiarino 3 1
c cosmetic
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 13:00:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_client.h
e
s 00011/00000/00000
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
/* vme_client.h - TCP client header file */
I 3

E 3
D 4
//#define SERVER_INET_ADDR "129.57.35.205"
#define SERVER_INET_ADDR "129.57.36.104"   //TEST
E 4
I 4
/*#define SERVER_INET_ADDR "129.57.35.205"*/
#define SERVER_INET_ADDR "129.57.36.104"   /*TEST*/
E 4
#define SOCK_ADDR_SIZE (sizeof (struct sockaddr_in))
#define REPLY_MSG_SIZE (sizeof (struct request))

/* selected definitions from vxWorks.h */
#define ERROR (-1)
#define TRUE  (1)
#define FALSE (0)

E 1
