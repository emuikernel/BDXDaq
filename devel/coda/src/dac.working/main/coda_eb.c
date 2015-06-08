/* coda_eb.c - BOS format */

/* following flag (if defined) will force BOS format functions to be linked
   directly rather then loaded dynamicaly; need it to check function
   prototypes etc; CODA format will not work !!! */

#include "tcpServer.c"

#define LINK_BOS_FORMAT

#include "cinclude/coda_eb_inc.c"
