
/* libtcp.h - header used by both TCP server and client */

/* defines */ 
#define SERVER_PORT_NUM 5001      /* server's port number for bind() */ 
#define SERVER_WORK_PRIORITY 250  /* priority of server's work task (<ROC !)*/ 
#define SERVER_STACK_SIZE 100000  /* stack size of server's work task */ 
#define SERVER_MAX_CONNECTIONS 4  /* max clients connected at a time */ 
#define REQUEST_MSG_SIZE 1024     /* max size of request message */ 
#define REPLY_MSG_SIZE 4096        /* max size of reply message */ 

/* structure for requests from clients to server */ 
typedef struct trequest { 
  int reply;                      /* TRUE = request reply from server */ 
  int msgLen;                     /* length of message text */ 
  char message[REQUEST_MSG_SIZE]; /* message buffer */ 
} TREQUEST;

typedef struct twork
{
  int newFd;       /* socket descriptor from accept */
  char *address;
  unsigned short port;
} TWORK;


/* function prototypes */

#ifdef	__cplusplus
extern "C" {
#endif

int tcpClientCmd(char *target, char *command, char *result);
int tcpClientDP(char *target, char *command, char *result);

#ifdef	__cplusplus
}
#endif
