h22051
s 00001/00001/00025
d D 1.3 08/04/01 14:49:16 boiarino 4 3
c *** empty log message ***
e
s 00019/00072/00007
d D 1.2 02/07/06 23:45:53 boiarino 3 1
c add some prototypes
c 
e
s 00000/00000/00000
d R 1.2 01/10/22 13:00:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/vme_download.h
e
s 00079/00000/00000
d D 1.1 01/10/22 13:00:29 boiarino 1 0
c date and time created 01/10/22 13:00:29 by boiarino
e
u
U
f e 0
t
T
I 1
D 3
/*-----------------------------------------------
 * filename:      vme_download.h 
 * author:        Del Putnam
 * last modified: 2/26/96
 *-----------------------------------------------
 */
E 3

I 3
/*  vme_download.h */
E 3


D 3
/*-----------------------------------------------
 *     here are the parameters for vme_download
 *-----------------------------------------------
 *
 * int vme_download(addr, data, hostname)
 *     unsigned int addr;
 *     unsigned short *data;
 *     char *hostname;
 */
E 3
I 3
/* function prototypes */
E 3

I 3
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
E 3

I 3
/* vmememget.c */
D 4
void get_data(char *hostname, unsigned int addr, char *filename);
E 4
I 4
void get_bigdata(char *hostname, char *filename);
E 4
E 3

I 3
/* vme_client.c */
int  vme_send(struct request *clientRequest);
E 3

D 3
/*-----------------------------------------------
 * here is the function prototype 
 *-----------------------------------------------
 */

int vme_download();



/*-----------------------------------------------
 *     here are the parameters for router_init
 *-----------------------------------------------
 *
 * int router_init(router_id, hostname)
 *     int router_id;
 *     char *hostname;
 */


/*-----------------------------------------------
 * here is the function prototype 
 *-----------------------------------------------
 */

int router_init();



/*-----------------------------------------------
 * router id definitions
 *-----------------------------------------------
 */

#define SECTOR_1 1
#define SECTOR_2 2
#define SECTOR_3 3
#define SECTOR_4 4
#define SECTOR_5 5
#define SECTOR_6 6



/*-----------------------------------------------
 *     here are the parameters for evt_proc__init
 *-----------------------------------------------
 *
 * int router_init(hostname)
 *     char *hostname;
 */


/*-----------------------------------------------
 * here is the function prototype 
 *-----------------------------------------------
 */

int evt_proc_init();
E 3
I 3
/* tigris/fileio.c */
void bigdata_cleanup();
E 3
E 1
