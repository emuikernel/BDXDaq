/* usrPPPCfg.c - Configlette for configuring PPP */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the init routine for the INCLUDE_PPP 
component. The usrPPPInit routine is actually called from
usrNetPPPBoot.c

NOMANUAL
*/


#include "pppLib.h"
#include "netinet/ppp/options.h"

#ifndef PPP_OPTIONS_FLAGS
#define PPP_OPTIONS_FLAGS ((PPP_OPT_NO_ALL << PPP_S_NO_ALL)  | \
    (PPP_OPT_PASSIVE_MODE << PPP_S_PASSIVE_MODE)             | \
    (PPP_OPT_SILENT_MODE << PPP_S_SILENT_MODE)               | \
    (PPP_OPT_DEFAULTROUTE << PPP_S_DEFAULTROUTE)             | \
    (PPP_OPT_PROXYARP << PPP_S_PROXYARP)                     | \
    (PPP_OPT_IPCP_ACCEPT_LOCAL << PPP_S_IPCP_ACCEPT_LOCAL)   | \
    (PPP_OPT_IPCP_ACCEPT_REMOTE << PPP_S_IPCP_ACCEPT_REMOTE) | \
    (PPP_OPT_NO_IP << PPP_S_NO_IP)                           | \
    (PPP_OPT_NO_ACC << PPP_S_NO_ACC)                         | \
    (PPP_OPT_NO_PC << PPP_S_NO_PC)                           | \
    (PPP_OPT_NO_VJ << PPP_S_NO_VJ)                           | \
    (PPP_OPT_NO_VJCCOMP << PPP_S_NO_VJCCOMP)                 | \
    (PPP_OPT_NO_ASYNCMAP << PPP_S_NO_ASYNCMAP)               | \
    (PPP_OPT_NO_MN << PPP_S_NO_MN)                           | \
    (PPP_OPT_NO_MRU << PPP_S_NO_MRU)                         | \
    (PPP_OPT_NO_PAP << PPP_S_NO_PAP)                         | \
    (PPP_OPT_NO_CHAP << PPP_S_NO_CHAP)                       | \
    (PPP_OPT_REQUIRE_PAP << PPP_S_REQUIRE_PAP)               | \
    (PPP_OPT_REQUIRE_CHAP << PPP_S_REQUIRE_CHAP)             | \
    (PPP_OPT_LOGIN << PPP_S_LOGIN)                           | \
    (PPP_OPT_DEBUG << PPP_S_DEBUG)                           | \
    (PPP_OPT_DRIVER_DEBUG << PPP_S_DRIVER_DEBUG))
#endif  /* PPP_OPTIONS_FLAGS */
 
PPP_OPTIONS pppOptions = 
    {
    PPP_OPTIONS_FLAGS,              /* flags field */
    PPP_STR_ASYNCMAP,               /* Set the desired async map */
    PPP_STR_ESCAPE_CHARS,           /* Set chars to escape on transmission */
    PPP_STR_VJ_MAX_SLOTS,           /* Set max VJ compression header slots */
    PPP_STR_NETMASK,                /* Set netmask value for negotiation */
    PPP_STR_MRU,                    /* Set MRU value for negotiation */
    PPP_STR_MTU,                    /* Set MTU value for negotiation */
    PPP_STR_LCP_ECHO_FAILURE,       /* Set max consecutive LCP echo failures */
    PPP_STR_LCP_ECHO_INTERVAL,      /* Set time for LCP echo requests */
    PPP_STR_LCP_RESTART,            /* Set timeout for LCP */
    PPP_STR_LCP_MAX_TERMINATE,      /* Set max # xmits for LCP term-reqs */
    PPP_STR_LCP_MAX_CONFIGURE,      /* Set max # xmits for LCP conf-reqs */
    PPP_STR_LCP_MAX_FAILURE,        /* Set max # conf-naks for LCP */
    PPP_STR_IPCP_RESTART,           /* Set timeout for IPCP */
    PPP_STR_IPCP_MAX_TERMINATE,     /* Set max # xmits for IPCP term-reqs */
    PPP_STR_IPCP_MAX_CONFIGURE,     /* Set max # xmits for IPCP conf-reqs */
    PPP_STR_IPCP_MAX_FAILURE,       /* Set max # conf-naks for IPCP */
    PPP_STR_LOCAL_AUTH_NAME,        /* Set local name for authentication */
    PPP_STR_REMOTE_AUTH_NAME,       /* Set remote name for authentication */
    PPP_STR_PAP_FILE,               /* Set the PAP secrets file */
    PPP_STR_PAP_USER_NAME,          /* Set username for PAP auth with peer */
    PPP_STR_PAP_PASSWD,             /* Set password for PAP auth with peer */
    PPP_STR_PAP_RESTART,            /* Set timeout for PAP */
    PPP_STR_PAP_MAX_AUTHREQ,        /* Set max # xmits for PAP auth-reqs */
    PPP_STR_CHAP_FILE,              /* Set the CHAP secrets file */
    PPP_STR_CHAP_RESTART,           /* Set timeout for CHAP */
    PPP_STR_CHAP_INTERVAL,          /* Set interval for CHAP rechallenge */
    PPP_STR_CHAP_MAX_CHALLENGE      /* Set max # xmits for CHAP challenge */
    };

/********************************************************************************
* usrPPPInit - initialize a ppp channel
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/
 
STATUS usrPPPInit 
    (
    char *      pBootDev,		/* boot device */
    int 	unitNum, 		/* unit number */
    char *      localAddr,		/* local address */
    char *      peerAddr		/* peer address */
    )
    {
#ifdef INCLUDE_PPP
    PPP_INFO    pppInfo;
    PPP_OPTIONS *pOptions = NULL;
    char        pppTyDev [20];          	/* ppp device */
#ifdef PPP_BAUDRATE
    int		pppBaudRate = PPP_BAUDRATE;	/* ppp baud rate */
#else
    int		pppBaudRate = 0;		/* ppp baud rate */
#endif	/* PPP_BAUDRATE */
    char *	pBaudStr;			/* ppp boot string */
    int         netmask;                	/* netmask */
    int         sysRate = sysClkRateGet();	/* system clock rate */
    int         ix;

#ifdef	PPP_OPTIONS_STRUCT
    pOptions = &pppOptions;
#endif	/* PPP_OPTIONS_STRUCT */

    if ((pBaudStr = strpbrk (pBootDev, ",")) != NULL)
        {
	*pBaudStr++ = '\0';
	pppBaudRate = atoi (pBaudStr);
	}

    if (pBootDev [3] == '=')
        {
        /* get the tty device used for PPP interface e.g. "ppp=/tyCo/1" */
        strcpy (pppTyDev, &pBootDev [4]);
        pBootDev [3] = '\0';
        }
    else 
        {
        /* construct the default PPP tty device */
        sprintf (pppTyDev, "%s%d", "/tyCo/", PPP_TTY);
        }
 
    printf ("Attaching network interface ppp%d...\n", unitNum);
 
    bootNetmaskExtract (localAddr, &netmask); /* remove and ignore mask */
 
    if (pppInit (unitNum, pppTyDev, localAddr, peerAddr, pppBaudRate,
		 pOptions, PPP_OPTIONS_FILE) == ERROR)
        {
        printf ("\npppInit failed 0x%x\n", errno);
        return (ERROR);
        }
 
    /* wait for PPP link to be established */

    for (ix = 0; ix < PPP_CONNECT_DELAY; ix++)
        {
        taskDelay (sysRate);

        if ((pppInfoGet (unitNum, &pppInfo) == OK) &&
            ((pppInfo.ipcp_fsm.state == OPENED) ||
	     (pppInfo.lcp_wantoptions.silent)	||
	     (pppInfo.lcp_wantoptions.passive)))
           break;
        }

    if (ix == PPP_CONNECT_DELAY)
        {
 	pppDelete (unitNum);			/* kill the interface */
        printf ("ppp0: timeout: could not establish link with peer.\n");
        return (ERROR);
 	}
 
#if CPU==SIMSPARCSOLARIS
     /* Add default route thru ppp gateway */
    routeAdd ("0.0.0.0", peerAddr);
    routeAdd (localAddr, "127.0.0.1"); /* loopback for local addresses */
#endif

    printf ("done.\n");
    return (OK);
     
#else /* INCLUDE_PPP */
    printf ("\nError: ppp not included.\n");
    return (ERROR);
#endif  /* INCLUDE_PPP */
    }
