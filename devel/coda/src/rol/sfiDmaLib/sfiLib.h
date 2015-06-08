/******************************************************************************************
 *
 *  Header File for SFI Async Readout lists    
 *
 *
 *
 */



/* Local Headers */
#include "sfi.h"
#include "sfi_fb_macros.h"


extern struct sfiStruct sfi;

extern  int sysBusToLocalAdrs(int, char *, char **);
extern  int intDisconnect();
extern  int sysIntEnable(int);

extern int sfiIntCount;
extern int sfiNeedAck;

/*Macros */
#define SFI_ENABLE_SEQUENCER      *sfi.sequencerEnable = 1

/* prototypes */
void sfiUserTrigger(int arg);

