/*
---------------------------------------------------------------------------
-
-   	File:     constants.h
-
-	Header file for the numbers of various
-       things in ced and other numerical constants
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
*/


#ifndef __CEDCONSTH
#define __CEDCONSTH

#define MAX_REGIONS             3        /* Total number of regions in sector */

/*********************************************************
Note: on the number of cerenkovs: ced uses one item for
the hi and lo phi scints, thus as far as ced is concerned,
there are 18 (not 36) of them.
***********************************************************/

#define NUM_SECT                6        /* the six CLAS sectors */
#define NUM_CERENK              18       /* number of cerenkov counters per sector */
#define NUM_SHOWER              2        /* number of shower counter planes per sector */
#define NUM_STRIP               36       /* number of shower counter strips per direction */
#define NUM_SCINT               57       /* number of scints per sector */
#define NUM_SCINTPLANE          4        /* number of geometrically distinct planes */

/* it is useful in many cases to define "twice" the number of things.
   make sure this section is consistent with the one above */

#define NUM_CERENK2              36
#define NUM_SCINTPLANE2          8


#define NUM_T_COUNTERS          48       /* number of t counters in the photon tagger */


/*------ types of messages ----*/


/* for caps log messages */

#define CEDMESSAGE    1
#define CEDWARNING    2
#define CEDERROR      3


#endif



