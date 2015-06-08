/*
---------------------------------------------------------------------------
-
-   	File:     ced.h
-
-	main header file for ced version 2.0
-
-										
-  Revision history:								
-                     							        
-  Date       Programmer     Comments						
--------------------------------------------------------------------------------
-  10/19/94	DPH		Initial Version					
-------------------------------------------------------------------------------
*/

#ifndef __CEDH
#define __CEDH

#include  "Hv.h"
#include  "macro.h"
#include  "data.h"
#include  "geometry.h"
#include  "bosced.h"
#include  "proto.h"
#include  "map_manager.h"

/* event source macros */

#define   CED_FROMFILE        0
#define   CED_FROMET          1

/* evvent "demand" macros */

#define   CED_ONDEMAND        0
#define   CED_ONTIMER         1

/*=====================================
      Global Variables 
=======================================*/

extern  int             runnumber;      /* current run number */

extern   FieldMap	fmap;	        /* current field map */
extern   float          cedversion;     /* current version */

extern   Boolean        etisok;         /* true if et initialized */
 
extern   short          eventsource;    /* where the events are coming from*/

extern   int            eventwhen;      /* on demand or from timer */
extern   int            eventtime;      /* seconds to view when "on timer" */    
extern   Hv_IntervalId  eventinterval;  /* for the timer */
extern   Boolean        precountEvents; /* if True, scan the event file when opened */
extern   char          *eventFileFilter; /* filter used when looking for event files */

/* related to events improperly formatted */

extern   Boolean        eventBadFormat;
extern   int            eventBadCode;
extern   int            eventBadFlag;

/* tagger data */

extern int             tagemin;
extern int             tagemax;
extern int             tagtmin;
extern int             tagtmax;
extern short           taglowcolor;
extern short           taghigcolor;
extern short           tagtjust1color;
extern short           tagtbothcolor;
extern short           tageinanytrangecolor;
extern short           tageinnotrangecolor;

/* startc counter */

extern int             starttdcmin;
extern int             starttdcmax;
extern int             startadcmin;
extern int             startadcmax;
extern short           starthitcolor;
extern short           starttaglowcolor;
extern short           starttaghighcolor;

/* dc data */
    
extern short           dcframecolor;
extern short           dcfillcolor;
extern short           dchitcolor;
extern short           dcdcacolor;
extern short           dctrigsegcolor;
extern short           dcaddedbytrigcolor;
extern short           dcnoisecolor;
extern short           dcbadtimecolor;
extern short           dcdeadwirecolor;
extern short           dcfromdc1color;  /* for hits from dc1 in the track */

extern int             dcmintime[3];
extern int             dcmaxtime[3];

/* other detector colors */

extern short           ecframecolor[3]; /* array for inner, outer */
extern short           ecfillcolor[3];  /* array for inner, outer */
extern short           echitcolor;

extern short           scintframecolor;
extern short           scintfillcolor;
extern short           scinthitcolor;
extern short           scintdeadcolor;

extern short           ec1framecolor; /* array for inner, outer */
extern short           ec1fillcolor[2];  /* array for inner, outer */
extern short           ec1hitcolor;
    
/* pixel densities control when various dc features are drawn */
    
extern short           dcbighitwires[MAX_REGIONS];
extern short           dcnormalwire[MAX_REGIONS];
extern short           dchitcircle[MAX_REGIONS];
extern short           dchexagon[MAX_REGIONS];
    
/* drift chamber noise reduction parameters */
    
extern Boolean         reducenoise;
extern int             missinglayers[3][2];  /* rgn, supl */
extern int             layershifts[6];

/* some global menu items */

extern   Hv_Widget      evfromfile;
extern   Hv_Widget      evfromet;

extern   Hv_Widget      evondemand;
extern   Hv_Widget      evontimer;
extern   Hv_Widget      evaccumulate;

#endif












