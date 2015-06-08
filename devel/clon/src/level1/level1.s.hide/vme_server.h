
/* vme_server.h - TCP server header file */

#define SERVER_WORK_PRIORITY (100)
#define SERVER_STACK_SIZE (10000)
#define MAX_QUEUED_CONNECTIONS (1)

void   get_tigris_status();
void   init_tigris_status();
STATUS vme_server(void);
STATUS trig_sem_init(void);

