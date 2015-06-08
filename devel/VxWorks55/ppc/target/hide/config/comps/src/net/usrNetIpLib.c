/* usrNetIpLib.c - Initialization routine for the IP library */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,23jan03,wap  restore compatibility with old ipLib API
01b,13jan03,vvv  merged from branch wrn_velocecp, ver01b
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_IP component. It calls the initialization routines for the
IP libraries.

NOMANUAL
*/


IP_CFG_PARAMS ipCfgParams =	/* ip configuration parameters */
    {
    IP_FLAGS_DFLT,		/* default ip flags */
    IP_TTL_DFLT,		/* ip default time to live */
    IP_QLEN_DFLT,		/* default ip intr queue len */
    IP_FRAG_TTL_DFLT		/* default ip fragment time to live */
    }; 

STATUS usrIpLibInit ()
    {

#ifdef INCLUDE_VIRTUAL_STACK
    if (ipLibMultiInit (&ipCfgParams, IP_MAX_UNITS) == ERROR)
                                                          /* has to included */
							  /* by default */
#else
    if (ipLibInit (&ipCfgParams) == ERROR)  /* has to included */
					    /* by default */
#endif /* INCLUDE_VIRTUAL_STACK */
        {
        printf ("ipLibInit failed.\n");
        return (ERROR);
        }
    
    if (rawIpLibInit () == ERROR)           /* has to included by default */
        {
        printf ("rawIpLibInit failed.\n");
        return (ERROR);
        }
    
    if (rawLibInit () == ERROR)
        {
        printf ("rawLibInit failed.\n");
        return (ERROR);
        }

    return (OK);

    }


