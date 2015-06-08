
/*  vme_download.h */


/* function prototypes */

/* vme_download.c */
void zero_routers(char *hostname);
void evt_proc_init();
void router_init(int router_id, char *hostname);
int  vme_download(unsigned int addr, unsigned short *data, char *hostname);
int  bigdata_download(unsigned short *data, char *hostname);
int  bigdata_upload(unsigned short *data, char *hostname);
int  vme_upload(unsigned int addr, unsigned short *data, char *hostname);
int  ep_creg1_download(unsigned short data, char *hostname);
int  ep_creg2_download(unsigned short data, char *hostname);
int  router_creg_download(unsigned int, unsigned short, char *); /* obsolete ? */

/* vmememget.c */
void get_bigdata(char *hostname, char *filename);

/* vme_client.c */
int  vme_send(struct request *clientRequest);

/* tigris/fileio.c */
void bigdata_cleanup();
