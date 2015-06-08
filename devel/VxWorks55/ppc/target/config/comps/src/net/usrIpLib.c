IP_CFG_PARAMS ipCfgParams =	/* ip configuration parameters */
    {
    IP_FLAGS_DFLT,		/* default ip flags */
    IP_TTL_DFLT,		/* ip default time to live */
    IP_QLEN_DFLT,		/* default ip intr queue len */
    IP_FRAG_TTL_DFLT		/* default ip fragment time to live */
    }; 

STATUS usrIpLibInit ()
    {

    if (ipLibInit (&ipCfgParams) == ERROR)  /* has to included by default */
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


