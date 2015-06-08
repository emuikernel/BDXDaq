#ifndef _BOS_ 
/* header for BOS system */
 
/* parameters */
 
#define	MAGIC	123456	    /* checking word */
#define	NHW     6	    /* the number of the bank header words */
#define ICOL	5	    /* position the number of columns */
#define IROW	4	    /* position the number of rows */
#define INAM	3	    /* position the name of bank */
#define INR     2	    /* position the number of bank */
#define INXT	1	    /* position the index of next bank */
#define IDAT	0	    /* position the number of data words */
 
/* INR for work bank contains the total number of words in bank NT */

#define NT      INR

/* BOS system structure */
 
typedef struct bos *SYSptr;
typedef struct bos {
        int jw1;	/* ADDITIONAL (INPUT) ARGUMENT */
        int nsyst;	/* NUMBER OF SYSTEM WORDS IN ARRAY */
        int names;	/* MAX. NUMBER OF NAMES */
        int nprim;	/* PRIME NUMBER FOR HASH SEARCH */
        int nresr;	/* NUMBER OF RESERVED WORDS (WBANK) */
        int nlplm;	/* NUMBER OF BANKS FOR GIVEN NAME WITHOUT LINK BANK */
        int narr;	/* NUMBER OF INITIALIZED ARRAYS */
        int iarr[100];	/* ARRAY ADRESSES */
        int tleft;	/* TIME LEFT AT START OF BOS */
        int lub;	/* THE NUMBER OF BANKS TO BE PRINTED */
      } SYS;
 
/* bos array structure */
 
typedef struct bcs *BOSptr;
typedef struct bcs {
        int njw;	/* LENGTH OF ARRAY */
        int ick;	/* CHECK WORD (=MAGIC) */
        int idfmt;	/* INDEX OF WORK BANK WITH FORMAT INDICES */
        int idptr;	/* INDEX OF WORK BANK WITH POINTERS */
        int idnam;	/* INDEX OF WORK BANK WITH NAMES */
        int inm;	/* FIRST INDEX OF NAMED BANK AREA */
        int igp;	/* FIRST INDEX OF GAP */
        int iwk;	/* FIRST INDEX OF WORK BANK AREA */
        int ndn;	/* NUMBER OF DROPPED WORDS OF NAMED BANKS */
        int ndw;	/* NUMBER OF DROPPED WORDS OF WORK BANKS */
        int ign;	/* INDEX OF LOWEST DELETED NAMED BANK */
        int igw;	/* INDEX OF HIGHEST DELETED WORK BANK */
        int idl;	/* INDEX OF LINK BANK */
        int ilt[5];	/* ilt[0] - INDEX OF WORK BANK FOR LIST 'C'
    	        	   ilt[1] - INDEX OF WORK BANK FOR LIST 'E'
    		           ilt[2] - INDEX OF WORK BANK FOR LIST 'R'
    		           ilt[3] - INDEX OF WORK BANK FOR LIST 'S'
    		           ilt[4] - INDEX OF WORK BANK FOR LIST 'T' */
        SYSptr s;	/* pointer on system structure */
      } BOS;
 
/* backward compatibility */


 
#include "bosfun.h" 
#include "bosfor.h"
/* errors */
 
#define ARRAY_NOT_INITIALIZED			-1	/* bbnot */
#define W_INSUFFICIENT_SPACE_FOR_NEW_BANK	-2	/* bbret */
#define INSUFFICIENT_SPACE			-3	/* bbspc */
#define WRONG_ARGUMENT				-4
#define W_INDEX_INCORRECT			-5
#define W_INSUFFICIENT_SPACE_TO_INCREASE_LENGTH_OF_EXISTING_BANK	-6
#define TOO_MANY_DIFFERENT_NAMES_USED		-7
#define MBANK_WRONG_ARGUMENT			-8
#define MBANK_INSUFFICIENT_SPACE_FOR_NEW_BANK	-9
#define MBANK_INSUFFICIENT_SPACE_TO_INCREASE_LENGTH_OR_MOVE		-10
#define BGARB_ERROR_AT_INDEX			-11
#define BLIST_WRONG_ARGUMENT			-12
#define MKFMT_BAD_FORMAT			-13
#define BOS_INITIALIZATION_ERROR		-14
#define BOS_ARRAY_LENGTH_TOO_SMALL		-15

 
#define _BOS_
#endif
