/* tclserial.h - declarations for tclserial package */

/* Copyright 1997, Wind River Systems, Inc. */

/* 
modification history
--------------------
01b,08sep97,dat  reformatted, added copyright
01a,20jun95,wmd  Created.
*/

#ifdef WIN32
typedef struct { /* SCB */
    HANDLE	fd;
    char	nae[20];
    } SCB, *serial_t; 
		
typedef DCB *serial_ttystate;

#ifndef NOT_OVERLAPPED
extern 	OVERLAPPED ovrlapped_s;
#endif

extern int win32_select (HANDLE fd, int timeout);
extern int win32_read (HANDLE fd, char * buf, int len);
extern int win32_write (HANDLE fd, const char *str, int len);
extern int win32_flush_output (HANDLE hcom);
extern int win32_flush_input (HANDLE hcom);

/* externs */
extern HANDLE tclSerialOpen (char * devName, int baudRate,
			    int dataBits, int stopBits, char * parity);
extern int tclSerialClose (HANDLE hcom);
extern int tclSerialSend (HANDLE hcom, char * buf, int nBytes);
extern int tclSerialReceive (HANDLE hcom, char* buf, int nBytes,
			    int timeout, char* pattern);
extern int tclSerialFlush (HANDLE hcom);

#else

typedef int HANDLE;

/* externs */
extern int tclSerialOpen (char * devName, int baudRate, int dataBits,
			int stopBits, char * parity);
extern int tclSerialClose (int fd);
extern int tclSerialSend (int fd, char * buf, int nBytes);
extern int tclSerialReceive (int fd, char* buf, int nBytes, int timeout, 
			  char* pattern);
extern int tclSerialFlush (int fd);
#endif  /* WIN32 */


/* timeout mulitplier value for serial communications device */

#define MULTIPLIER(time, nbytes)	time/nbytes * 1000    
#define TIMEOUT_VALUE  45
#define SERIAL_TIMEOUT_FAILURE -1

#define dbprintf(sg,err) if(dbg_on) printf ("%s %d\n", sg, err)
#define DBG_LINE_CNT	16

#ifdef DEBUG
#   define DEBUG_PRINT(fmt, param) printf(fmt, param)
#else
#   define DEBUG_PRINT(fmt, param)
#endif

#define PRINT_ARGS(cnt, arg) \
    { \
    int ix; \
    if (dbg_on) \
	{ \
	for (ix = 0; ix < cnt; ix++) \
	    printf ("arg[%d] = %s ", ix, arg[ix]); \
	printf ("\n"); \
	} \
    }


extern  int dbg_on;
extern  int dbg_ch_printed;
