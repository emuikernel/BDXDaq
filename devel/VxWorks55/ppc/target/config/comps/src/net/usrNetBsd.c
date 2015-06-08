/* usrNetBsd.c - Support for BSD network devices */

/* Copyright 1992 - 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,08oct01,mas  changed smNetAttach to smNetAttach2 (SPR 4547)
01e,10mar99,yh   changed component name INCLUDE_USR_ENTRIES (25331)
01d,08mar99,spm  added missing prototype for feiattach routine (SPR #23818)
01c,10feb99,dat  added FEI driver, made usrNetIfTbl global (SPR 23818)
01b,08oct98,ms   added prototypes for pcmciaattach and usrSmAttach
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is used to include support for network devices using the BSD
interface. The file contents are included in the project configuration 
file when INCLUDE_BSD is defined. This file creates all necessary data
structures so that any BSD-style network device can be attached by the 
INCLUDE_NET_INIT component or manually by the user.

NOMANUAL
*/

/* Network interface table. */
IMPORT int dcattach ();
IMPORT int feiattach ();
IMPORT int eglattach ();
IMPORT int eiattach ();
IMPORT int exattach ();
IMPORT int enpattach ();
IMPORT int ieattach ();
IMPORT int ilacattach ();
IMPORT int lnattach ();
IMPORT int lnsgiattach ();
IMPORT int nicattach ();
IMPORT int nicEvbattach ();
IMPORT int medattach ();
IMPORT int loattach ();
IMPORT int snattach ();
IMPORT int fnattach ();
IMPORT int elcattach ();
IMPORT int ultraattach ();
IMPORT int eexattach ();
IMPORT int eltattach ();
IMPORT int eneattach ();
IMPORT int esmcattach ();
IMPORT int quattach ();
IMPORT int slattach ();
IMPORT int pppattach ();
IMPORT STATUS pcmciaattach ();
IMPORT STATUS smNetAttach2 ();

#ifdef INCLUDE_USR_ENTRIES
    NETIF_USR_DECL
#endif

#ifdef	INCLUDE_IF_USR
IMPORT int IF_USR_ATTACH ();
#endif	/* INCLUDE_IF_USR */

/* local variables */

NETIF usrNetIfTbl [] =	/* network interfaces */
    {
#ifdef INCLUDE_USR_ENTRIES	/* Additional entries, from BSP */
	NETIF_USR_ENTRIES
#endif

#ifdef	INCLUDE_IF_USR	/* obsolete, do not use. use NETIF_USR_ENTRIES */
	{ IF_USR_NAME, IF_USR_ATTACH, IF_USR_ARG1, IF_USR_ARG2, IF_USR_ARG3,
	  IF_USR_ARG4, IF_USR_ARG5, IF_USR_ARG6, IF_USR_ARG7, IF_USR_ARG8 },
#endif	/* INCLUDE_IF_USR */

#ifdef	INCLUDE_DC
	{ "dc", dcattach, (char*)IO_ADRS_DC, INT_VEC_DC, INT_LVL_DC,
	  DC_POOL_ADRS, DC_POOL_SIZE, DC_DATA_WIDTH, DC_RAM_PCI_ADRS,
	  DC_MODE },
#endif	 /* INCLUDE_DC */
#ifdef  INCLUDE_EGL
	{ "egl", eglattach, (char*)IO_ADRS_EGL, INT_VEC_EGL, INT_LVL_EGL },
#endif	 /* INCLUDE_EGL */
#ifdef	INCLUDE_EI
	{ "ei", eiattach, (char*)INT_VEC_EI, EI_SYSBUS, EI_POOL_ADRS, 0, 0},
#endif	/* INCLUDE_EI */
#ifdef	INCLUDE_FEI
	{ "fei", feiattach, (char *)FEI_POOL_ADRS, 0, 0, 0, 0},
#endif	/* INCLUDE_FEI */
#ifdef	INCLUDE_EX
	{ "ex", exattach, (char*)IO_ADRS_EX, INT_VEC_EX, INT_LVL_EX,
	  IO_AM_EX_MASTER, IO_AM_EX },
#endif	/* INCLUDE_EX */
#ifdef	INCLUDE_ENP
	{ "enp", enpattach, (char*)IO_ADRS_ENP, INT_VEC_ENP, INT_LVL_ENP,
	  IO_AM_ENP },
#endif	/* INCLUDE_ENP */
#ifdef	INCLUDE_IE
	{ "ie", ieattach, (char*)IO_ADRS_IE, INT_VEC_IE, INT_LVL_IE },
#endif	/* INCLUDE_IE */
#ifdef	INCLUDE_ILAC
	{ "ilac", ilacattach, (char*)IO_ADRS_ILAC, INT_VEC_ILAC},
#endif	/* INCLUDE_ILAC */
#ifdef	INCLUDE_LN
	{ "ln", lnattach, (char*)IO_ADRS_LN, INT_VEC_LN, INT_LVL_LN,
	  LN_POOL_ADRS, LN_POOL_SIZE, LN_DATA_WIDTH, LN_PADDING,
	  LN_RING_BUF_SIZE },
#endif	/* INCLUDE_LN */
#ifdef  INCLUDE_LNSGI
        { "lnsgi", lnsgiattach, (char*)IO_ADRS_LNSGI, INT_VEC_LNSGI,
          INT_LVL_LNSGI, LNSGI_POOL_ADRS, LNSGI_POOL_SIZE, LNSGI_DATA_WIDTH,
          LNSGI_PADDING, LNSGI_RING_BUF_SIZE },
#endif  /* INCLUDE_LNSGI */
#ifdef  INCLUDE_NIC
        { "nic", nicattach, (char*)IO_ADRS_NIC, INT_VEC_NIC, INT_LVL_NIC },
#endif	/* INCLUDE_NIC */
#ifdef  INCLUDE_NIC_EVB
        { "nicEvb", nicEvbattach, (char*)IO_ADRS_NIC,INT_VEC_NIC,INT_LVL_NIC },
#endif  /* INCLUDE_NIC_EVB */
#ifdef  INCLUDE_MED
        { "med", medattach, (char*)IO_ADRS_DBETH, INT_VEC_DBETH, INT_LVL_DBETH},
#endif	/* INCLUDE_MED */
#ifdef  INCLUDE_ELC
	{ "elc", elcattach, (char*)IO_ADRS_ELC, INT_VEC_ELC, INT_LVL_ELC,
	  MEM_ADRS_ELC, MEM_SIZE_ELC, CONFIG_ELC},
#endif  /* INCLUDE_ELC */
#ifdef  INCLUDE_ULTRA
	{ "ultra", ultraattach, (char*)IO_ADRS_ULTRA, INT_VEC_ULTRA,
	  INT_LVL_ULTRA, MEM_ADRS_ULTRA, MEM_SIZE_ULTRA, CONFIG_ULTRA},
#endif  /* INCLUDE_ULTRA */
#ifdef  INCLUDE_EEX
	{ "eex", eexattach, (char*)IO_ADRS_EEX, INT_VEC_EEX, INT_LVL_EEX,
	  NTFDS_EEX, CONFIG_EEX},
#endif  /* INCLUDE_EEX */
#ifdef  INCLUDE_ELT
	{ "elt", eltattach, (char*)IO_ADRS_ELT, INT_VEC_ELT, INT_LVL_ELT,
	  NRF_ELT, CONFIG_ELT},
#endif  /* INCLUDE_ELT */
#ifdef  INCLUDE_QU
	{ "qu", quattach, (char*)IO_ADRS_QU_EN, INT_VEC_QU_EN, QU_EN_SCC,
	  QU_EN_TX_BD, QU_EN_RX_BD, QU_EN_TX_OFF, QU_EN_RX_OFF, QU_EN_MEM},
#endif  /* INCLUDE_QU */
#ifdef  INCLUDE_ENE
	{ "ene", eneattach, (char*)IO_ADRS_ENE, INT_VEC_ENE, INT_LVL_ENE},
#endif  /* INCLUDE_ENE */
#ifdef  INCLUDE_ESMC
	{ "esmc", esmcattach, (char*)IO_ADRS_ESMC, INT_VEC_ESMC, INT_LVL_ESMC,
	  CONFIG_ESMC, RX_MODE_ESMC},
#endif  /* INCLUDE_ESMC */
#ifdef  INCLUDE_SN
        { "sn", snattach, (char*)IO_ADRS_SN, INT_VEC_SN },
#endif	/* INCLUDE_SN */
#ifdef  INCLUDE_FN
        { "fn", fnattach },
#endif	/* INCLUDE_FN */

#ifdef  INCLUDE_SM_NET
        { "sm", smNetAttach2, 0, 0, 0, 0, 0, 0, 0, 0 },
#endif  /* INCLUDE_SM_NET */

#ifdef	INCLUDE_PCMCIA
	{ "pcmcia", pcmciaattach, 0, 0, 0, 0, 0, 0 },
#endif	/* INCLUDE_PCMCIA */

#ifdef	INCLUDE_PPP
	{"ppp", 0, 0, 0, 0, 0},
#endif	/* INCLUDE_PPP */

#ifdef	INCLUDE_SLIP
	{"sl", 0, 0, 0, 0, 0},
#endif	/* INCLUDE_SLIP */

	{ "lo", loattach  },
	{ 0, 0, 0, 0, 0, 0 },
    };
