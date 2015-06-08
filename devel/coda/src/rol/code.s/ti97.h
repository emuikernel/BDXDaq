/*
 *  ti97.h -  TI (Trigger Interface) board header file
 */


typedef struct ti97
{
  unsigned short csr;
  unsigned short vec;
  unsigned short dat;
  unsigned short oport;
  unsigned short iport;
} TI97;


/* functions */

int ti97reset(int addr);
int ti97setExternalMode(int addr);
int ti97setTSMode(int addr);
int ti97getInterruptLevel(int addr, unsigned short *level);
int ti97getInterruptVector(int addr, unsigned short *vector);
int ti97getTrigger(int addr, unsigned short *data);
int ti97intEnable(int addr, int vector);
int ti97intDisable(int addr);
int ti97intSave(int addr);
int ti97intRestore(int addr);
