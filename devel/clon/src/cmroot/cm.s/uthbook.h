#ifndef _UTHBOOK_


#define NHIST 2000
#define MAXI2      65536
#define NORMAL_I2  0x01
#define PACKED_I2  0x11
#define NORMAL_I4  0x02
#define PACKED_I4  0x12
#define NORMAL_F   0x03
#define PACKED_F   0x13

#ifdef VXWORKS
typedef int hvar; /* use integers only; data in BOS bank can be I*2 or I*4 */
#define HZERO 0
#else
typedef float hvar;
#define HZERO 0.001
#endif

typedef struct uthisti
{
  int entries;    /* total number of entries */

  int ifi2_book; /* set to 1 in booking if I2 permitted */
  int ifi2_fill; /* set to 1 in filling if I2 permitted */

  int nbinx;
  int xmin;
  int xmax;
  int xunderflow;
  int xoverflow;

  int nbiny;
  int ymin;
  int ymax;
  int yunderflow;
  int yoverflow;

  int nwtitle;
  char *title;

  int dx;
  int dy;

  int *buf;
  int **buf2;

} UThisti;

typedef struct uthistf
{
  int entries;    /* total number of entries */

  int ifi2_book; /* set to 1 in booking if I2 permitted */
  int ifi2_fill; /* set to 1 in filling if I2 permitted */

  int nbinx;
  float xmin;
  float xmax;
  float xunderflow;
  float xoverflow;

  int nbiny;
  float ymin;
  float ymax;
  float yunderflow;
  float yoverflow;

  int nwtitle;
  char *title;

  float dx;
  float dy;

  float *buf;
  float **buf2;

} UThistf;

#ifdef VXWORKS
typedef UThisti hstr;
#else
typedef UThistf hstr;
#endif


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
double all prototypes because of unknown problem in ROOT *.h, namely
construction

#ifdef	__cplusplus
extern "C" {
#endif

..........
..........
..........

#ifdef	__cplusplus
}
#endif


does not work !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifdef	__cplusplus

extern "C" {
#include "uthbook_fun.h"
}

#else

#include "uthbook_fun.h"

#endif


#define _UTHBOOK_
#endif
