h29782
s 00000/00000/00000
d R 1.2 03/05/23 12:19:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.inc/map_internal.h
e
s 00161/00000/00000
d D 1.1 03/05/23 12:19:48 boiarino 1 0
c date and time created 03/05/23 12:19:48 by boiarino
e
u
U
f e 0
t
T
I 1

#include "map_manager.h"

/* 
   map_internal.h        declarations for internal map routines
   Created           : 19-OCT-1992 by Thom Sulanke
*/
 
#define VERSION 2      /* data base format version */
#define ROOT  0        /* offset in file of root of tree */

#ifdef UNDEF

/* The IBM XL (eXtra Large) C compiler does not
   understand this
*/

#ifndef _SIZE_T_
#define _SIZE_T_
typedef unsigned int size_t;
#endif /* !_SIZE_T_ */

#endif

#ifndef vaxc
#include <sys/types.h> /* to get the correct size_t */
#else
#include <types.h> /* to get the correct size_t */
#endif

#include <ntypes.h>

typedef int32 pointer_t;

#define NULL_LOC 0    /* null pointer within file */

typedef struct subtableheader {
  int negversion;      /* cause version 1 software to fail */
  int nullname;
  int table_length;
  int table_used;
  pointer_t table;
} subtableheader_t;

typedef struct nametable {
  char name[MAP_NAMELENGTH];
  pointer_t loc;
} nametable_t;

#define INIT_NAMETABLE_SIZE 10

typedef struct subsystem {
  pointer_t next;
  char name[MAP_NAMELENGTH];
  pointer_t list;
  pointer_t table;
  int table_length;
  int table_used;
} subsystem_t;

#define SUBSYSTEM_LIST_OFFSET 20

typedef struct item {
  pointer_t next;
  char name[MAP_NAMELENGTH];
  pointer_t list;
  int length;
  int type;
  pointer_t table;
  int table_length;
  int table_used;
  int32 spare[1];
} item_t;

#define ITEM_LIST_OFFSET 20
#define INIT_TABLE_SIZE 50
#define TABLE_FACTOR 5

typedef struct arrayheader {
  pointer_t next;
  int time;
  int length;
  int32 spare[1];
} arrayheader_t;

#define ARRAY_VALUES_OFFSET 16

typedef struct name {
  pointer_t next;
  char name[MAP_NAMELENGTH];
} name_t;

typedef struct table {
  int time;
  pointer_t loc;
} table_t;

typedef enum map_conversion
{
  swaperror = 1,      /* generate an error */
  swaprootpointer,    /* swap pointer_t */
  swappointer,        /* swap pointer_t */
  swapsubtableheader, /* swap subtableheader_t */
  swapsubsystem,      /* swap subsystem_t    */
  swapitem,           /* swap item_t         */
  swapnametablearray, /* swap array of nametable_t */
  swapintarray,       /* swap array of int   */
  swapfloatarray,   /* swap array of float   */
  swapchararray,    /* swap array of char    */
  swaptablearray,   /* swap array of table_t */
  swapname,         /* swap name_t           */
  swaparrayheader  /* swap arrayheader_t    */

} map_conversion_t;

extern int map_needEndianConversion;
/* function prototypes */
int map_endianConvert(void *chunck, size_t nbytes, map_conversion_t conversionType);
int 	    map_add_item1(const char filename[], const char subsystemname[], 
			  const char itemname[], int arraylength, 
			  int arraytype, int table_size);
int         map_add_sub(int fd, const char subsystemname[], int nitems);
int         map_close_map( int fd );
int         map_dump( const char filename[] );
void        map_error(int error_code, char *fmt, ... );
void        map_fillname(char to[], const char from[]); 
int   map_find_item(int fd, const char subsystemname[], 
			  const char itemname[],pointer_t *adr);
int   map_find_pred(int fd, pointer_t start, const char name[], 
			  int table_length, int table_used, 
			  pointer_t table_loc, int *tindex, 
			  pointer_t *succ_loc,pointer_t *adr);
int   map_find_same(int fd, pointer_t start, const char name[], 
			  int table_length, int table_used, 
			  pointer_t table_loc, int *tindex, 
			  pointer_t *succ_loc, pointer_t *adr);
int   map_find_sub(int fd, const char subsystemname[],pointer_t *adr);
pointer_t   map_find_time_pred(int fd, pointer_t start, int atime);
int 	    map_fix_head(const char filename[], int nsubsystems); 
int 	    map_fix_item(const char filename[], const char subsystemname[], 
			 const char itemname[], int nvals); 
int 	    map_fix_sub(const char filename[], const char subsystemname[], 
			int nitems); 
int         map_open_ro(const char filename[]);
int         map_open_ro_lock(const char filename[]);
int         map_open_rw(const char filename[]);
int         map_overwrite(const void *chunck, size_t nbytes, int fd, 
			  pointer_t offset,
			  map_conversion_t conversionType);

int         map_read(void *chunck, size_t nbytes, int fd, pointer_t offset,map_conversion_t conversionType);
int 	    map_stat_head(const char filename[], int *nsubsystems); 
void        map_syserror(int error_code, char *fmt, ... );
int   map_time_pred(int fd, pointer_t pointer, item_t item, int atime,
			  int *tindex, pointer_t *suc_loc, int *suc_time,
		    pointer_t *adr);
void        map_warn(int error_code, char *fmt, ... );
int   map_write(const void *chunck, size_t nbytes, int fd, pointer_t *adr, map_conversion_t conversionType);
int map_conv_Fstring(char *C_string, char *F_string, int F_strlen);

/* end file */
E 1
