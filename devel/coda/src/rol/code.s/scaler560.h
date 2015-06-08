
/* scaler560.h */


/* Address Modifier */

#define AM24N      0x39    /* A24 (standard) non privilegeddata access */
#define AM24S      0x3D    /* A24 (standard) supervisory data access */
#define AM32N      0x09    /* A32 (extended) non privileged data access */
#define AM32S      0x0D    /* A32 (extended) supervisory data access */


/* masks for status register */

#define S0 0x00000008
#define S1 0x00000004
#define S2 0x00000002
#define S3 0x00000001
#define S4 0x00000080
#define S5 0x00000040
#define S6 0x00000020
#define S7 0x00000010


/* functions */

int scaler560getVersion(unsigned int addr, unsigned short *value);
int scaler560getBoardID(unsigned int addr, unsigned short *value);
int scaler560getGEOAddress(unsigned int addr, unsigned short *value);
int scaler560getStatus(unsigned int addr, unsigned short *value);
int scaler560increment(unsigned int addr);
int scaler560resetVETO(unsigned int addr);
int scaler560setVETO(unsigned int addr);
int scaler560getVETO(unsigned int addr, unsigned short *value);
int scaler560clear(unsigned int addr);
int scaler560read(unsigned int addr, unsigned int *array);
int scaler560setInterruptLevel(unsigned int addr, unsigned short value);
int scaler560getInterruptLevel(unsigned int addr, unsigned short *value);
int scaler560setInterruptVector(unsigned int addr, unsigned short value);
int scaler560getInterruptVector(unsigned int addr, unsigned short *value);
int scaler560setInterruptSource(unsigned int addr, unsigned short value);
int scaler560getInterruptSource(unsigned int addr, unsigned short *value);
int scaler560intenable(int addr, unsigned int source, int level, int vector);
int scaler560intdisable(int addr);
int scaler560intClear(int addr);
int scaler560intSave(int addr, unsigned int *mask0);
int scaler560intRestore(int addr, unsigned int mask0);
