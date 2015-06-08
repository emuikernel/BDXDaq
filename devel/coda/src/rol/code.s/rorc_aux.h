#ifndef _RORC_AUX_
#define _RORC_AUX_

/********************************************************
*   rorc_aux.h                                          *
*   last update:        24/05/06                        *
*   written by: Peter Csato and Ervin Denes             *
********************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

#include <ctype.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <limits.h>

#include "rorc_version.h"

/************************************************/

#define KILO 1000
#define MEGA 1000000
#define GIGA 1000000000

/************************************************/

#define mask(a,b) ((a) & (b))
#define incr15(a) (((a) + 1) & 0xf)
#define decr15(a) (((a) - 1) & 0xf)

#ifdef DEBUG
  #define debug(x...) (printf (x), fflush (stdout))
/* #define debug(x...) fprintf (logFile, x) */
#else
  #define debug(x...)
#endif
#define debugg(x...)

#define MAX_FILE_NAME 255
typedef struct
{
  char          fn[MAX_FILE_NAME];
  time_t        mtime;
  key_t         key;
  int           offset;
  int           wordlen;
} ism_stat_t;

#define MAX_INTELHEX_RECORD 522
//MAX_INTELHEX_RECORD = ':'+len(2)+addr(4)+type(2)+2*len(255)+checksum(2)+'\0'
#define MIN_INTELHEX_RECORD 11
//MIN_INTELHEX_RECORD = ':'+len(2)+addr(4)+type(2)+checksum(2)


/************************************************/

void elapsed(struct timeval *tv2, struct timeval *tv1,
		             int *dsec, int *dusec);
void print_time(int l,
                struct timeval *tv2, struct timeval *tv1, time_t *start_time, 
                int sum_Gbyte, int sum_byte, char *stat_file_name);
char *th(int num);
char *percent(double in_val, double in_base, int prec);
char *order(double input_value, int precision);
char *time_order(double in_val, int prec);
char *border(double input_value, int precision);
void upper(char *string);
int trim(char *string);
int xdgtoi(char c);
int n_key_word(char *key_words[], char *string);
int browse(char *string, char *token[], int max_token, char *separator);
int getToken(char **val, int maxVal, FILE* fp);
int getVal(unsigned long *val, int maxVal, FILE* fp, char *format);
int getIntelHexRecord(FILE *fp, 
                      int *len, int *addr, int *type, int *val, char *err);
int putIntelHexRecord(FILE *fp,
                      int len, int addr, int type, int *val, char *err);
int JTAG2Memory(FILE *i_fp, volatile unsigned long *data_addr, int length);
void Memory2JTAG(FILE *o_fp, volatile unsigned long *data_addr,
                             volatile unsigned long *return_addr, int n_word);
int read_mif(FILE *file, unsigned long *memory, int max_word, int *read_word);
int logi2(unsigned long number);
int roundPowerOf2(int number);
int lock_file(int fd);
int unlock_file(int fd);
int lock_test(int fd);

#endif
