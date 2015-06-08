 
/* bcs.h */

/* #define NBCS 700000 - have to be outside */
 
/* define a BOScommon data type */
 
typedef struct boscommon {
      int junk[5];
      int iw[NBCS];
	} BOScommon;
 
/* declare a global variable */
 
BOScommon	bcs_;
 
 
