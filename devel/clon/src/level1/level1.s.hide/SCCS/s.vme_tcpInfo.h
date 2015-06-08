h05129
s 00004/00000/00019
d D 1.2 02/07/08 10:45:53 boiarino 3 1
c add some #define
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 13:00:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_tcpInfo.h
e
s 00019/00000/00000
d D 1.1 01/10/22 13:00:30 boiarino 1 0
c date and time created 01/10/22 13:00:30 by boiarino
e
u
U
f e 0
t
T
I 3

E 3
I 1
/* vme_tcpInfo.h - generic info used by both TCP server and client */ 
I 3

E 3
#define SERVER_PORT_NUM (5261) 
#define MSG_SIZE (1024)
#define VME_DOWNLOAD 1 
#define VME_UPLOAD 2 
#define SERVER_READING 3
#define SERVER_WRITING 4
I 3
#define BIGDATA_DOWNLOAD 5 
#define BIGDATA_UPLOAD 6
E 3

/* structure for requests from clients to server */ 
struct request { 
    unsigned int command; 
    unsigned int addr; 
    unsigned int size; 
    unsigned int length;
    char *data;
    char *hostname; 
    unsigned short checksum;
    unsigned short junk;
};
E 1
