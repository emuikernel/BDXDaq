
/* vme_tcpInfo.h - generic info used by both TCP server and client */ 

#define SERVER_PORT_NUM (5261) 
#define MSG_SIZE (1024)
#define VME_DOWNLOAD 1 
#define VME_UPLOAD 2 
#define SERVER_READING 3
#define SERVER_WRITING 4
#define BIGDATA_DOWNLOAD 5 
#define BIGDATA_UPLOAD 6

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
