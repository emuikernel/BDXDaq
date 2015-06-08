
/* vme_client.h - TCP client header file */

/*#define SERVER_INET_ADDR "129.57.35.205"*/
#define SERVER_INET_ADDR "129.57.36.104"   /*TEST*/
#define SOCK_ADDR_SIZE (sizeof (struct sockaddr_in))
#define REPLY_MSG_SIZE (sizeof (struct request))

/* selected definitions from vxWorks.h */
#define ERROR (-1)
#define TRUE  (1)
#define FALSE (0)

