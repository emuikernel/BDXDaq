/*
---------------------------------------------------------------------------
-
-   	File:     macro.h
-
-	main macro defs for ced version 2.0
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  10/19/94	DPH		Initial Version					
-------------------------------------------------------------------------------
*/

#ifndef __CEDMACROH
#define __CEDMACROH

/* for event dumps */

#define EV_MAX_TEXTSIZE  32768
#define EV_LINE_SIZE     256

/* for DC1 display */

#define JUSTTRACKS    1
#define ALLDC1BANKS   2


/* bad format codes */

#define GENERICBADFORMAT        0
#define BADDCLAYER              1
#define BADDCWIRE               2
#define BADSCINTID              3
#define BADUVALUE               4
#define BADVVALUE               5
#define BADWVALUE               6


#define SINGLEEVENT             1
#define ACCUMULATEDEVENTS       2 /* i.e. counts */
#define AVERAGETDC              3

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
#define NUM_SUPERLAYER          6        /* number of superlayers in a sector */
#define NUM_STARTSEG            2        /* number of start counter segments in a sector */ 

/* it is useful in many cases to define "twice" the number of things.
   make sure this section is consistent with the one above */

#define NUM_SHOWER2              4
#define NUM_CERENK2              36
#define NUM_SCINTPLANE2          8
#define NUM_SUPERLAYER2          12 


#define NUM_T_COUNTERS          48       /* number of t counters in the photon tagger */


/* bits used for field wire determination */

#define POINT1       01
#define POINT2       02
#define POINT3       04
#define POINT4      010
#define POINT5      020
#define POINT6      040
#define ALLPOINTS   POINT1+POINT2+POINT3+POINT4+POINT5+POINT6

/*------ types of messages ----*/


/* for caps log messages */

#define CEDMESSAGE    1
#define CEDWARNING    2
#define CEDERROR      3


/* for sizing all dc view */

#define ALLDCWWIDTH2   16.0 
#define ALLDCWHEIGHT2  13.0 

/* Item Tags for ced specific items*/

#define     BEAMLINE          1 
#define     FIELDMAP          2
#define     SCINTITEM         3
#define     SECTORHEXITEM     4
#define     SECTORSHOWERITEM  5
#define     FANNEDSHOWERITEM  6
#define     CERENKOVITEM      7
#define     DCITEM            8
#define     SECTORGRIDITEM    9
#define     SECTOREC1ITEM    10
#define     EC1VIEWEC1ITEM   11
#define     TAGGERITEM       12
#define     STARTITEM        13

/* feedback tags */

#define     ANCHORDISTANCE   1
#define     ZPOSITION        2
#define     XPOSITION        3
#define     YPOSITION        4
#define     DISTANCE         5
#define     ANGLE            6
#define     PHI              7
#define     WHATSECTOR       8
#define     EVENT            9
#define     BMAG            10
#define     BVECT           11
#define     WHATSCINT       12
#define     WHATSUPERLAYER  13
#define     WHATLAYER       14
#define     WHATWIRE        15
#define     OCCUPANCY       16
#define     WHATCERENK      17
#define     WHATSHOWER      18
#define     WHATUVW         19
#define     TDCL            20
#define     ADCL            21
#define     TDCR            22
#define     ADCR            23
#define     PIXENERGY       24
#define     PIXID           25
#define     UTDC            26
#define     UADC            27
#define     VTDC            28
#define     VADC            29
#define     WTDC            30
#define     WADC            31
#define     MIDPLANECOORD   32
#define     XADCL           33
#define     XTDCL           34
#define     YADCL           35
#define     YTDCL           36
#define     XADCR           37
#define     XTDCR           38
#define     YADCR           39
#define     YTDCR           40
#define     WHATXY          41
#define     TRACKID         42
#define     CHI2            43
#define     MOMENTUM        44
#define     VERTEX          45
#define     DIRCOSINE       46
#define     CHARGE          47

#define     IJCOORD         48

#define     ACCUMCNT        49
#define     AVGTDC          50
#define     EVSOURCE        51

#define     HOTWIRE         52
#define     TRIGGERFILTER   53

#define     RUNNUMBER       54
#define     WHATSTARTCNT    55
#define     TDC             56
#define     ADC             57

/* View Types */

#define     SECTOR       1
#define     CALORIMETER  2
#define     ALLDC        3
#define     SCINT        4
#define     TAGGER       5

/* numerical constant */

#define     ROOT3OVER2     0.8660254

/* visible sectors for sector views */

#define     SECTORS14       1
#define     SECTORS25       2
#define     SECTORS36       3
#define     MOSTDATASECTOR  4
#define     MOSTECSUMSECTOR 5

/* shower planes */

#define     ECINNER        0
#define     ECOUTER        1
#define     ECBOTH         2


/* strip directions */

#define     USTRIP         0
#define     VSTRIP         1
#define     WSTRIP         2

#define     XSTRIP         0
#define     YSTRIP         1

#endif

