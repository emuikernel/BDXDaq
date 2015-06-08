/* usrStartup.c - first routine called from usrInit */

/* Copyright 1984-1999 Wind River Systems, Inc. */ 

/*
modification history
--------------------
01f,04apr02,pch  SPR 74348: Enable PPC Machine Check exception ASAP
01e,20jan99,jpd  moved FREE_RAM_START_ADRS definition to usrKernel.c.
01d,19jan99,jgn  add sysHwInit0() facility
01c,29oct98,ms   added I960 hack.
01b,14jul98,ms   moved "IMPORT char end[]" to generated sysConfig.c.
01a,20apr98,ms   written from usrConfig.c
*/ 

/*
DESCRIPTION
This routine performs critical early system initialization.
*/  

/******************************************************************************
*
* sysStart - first c-code executed from usrInit
*/ 

void sysStart (startType)
    {
#if     (CPU_FAMILY == SPARC)
    excWindowInit ();                           /* SPARC window management */
#endif

#ifdef INCLUDE_SYS_HW_INIT_0
    /*
     * Perform any BSP-specific initialisation that must be done before
     * cacheLibInit() is called and/or BSS is cleared.
     */

    SYS_HW_INIT_0 ();
#endif /* INCLUDE_SYS_HW_INIT_0 */


#ifdef	CLEAR_BSS
#if	(CPU_FAMILY==I960)
    /* All 960 BSPs set the initialSP to "supervisorStack", - a BSS variable.
     * This is a problem, since clearing the BSS will trash the stack
     * frame, and so things might crash when this routine returns.
     * So we avoid clearing the variable.
     * The right solution is to put supervisorStack in the .data segment,
     * but then all BSPs (including 3rd party) would have to be updated.
     */

    /* perform hack iff supervisorStack variable is in BSS segment */

    extern UINT32 supervisorStack[];
    if ((UINT)supervisorStack > (UINT)edata)
	{
	UINT stackOffset = (UINT)supervisorStack + 400;
	bzero (edata, (UINT)supervisorStack - (UINT)edata);
	bzero ((char *)stackOffset, (UINT)end - (UINT)stackOffset);
	}
    else
#endif	/* CPU_FAMILY==I960 */
	bzero (edata, end - edata);
#endif	/* CLEAR_BSS */

#if (CPU_FAMILY == PPC)
    /*
     * Immediately after clearing the bss, ensure global stdin
     * etc. are ERROR until set to real values.  This is used in
     * target/src/arch/ppc/excArchLib.c to improve diagnosis of
     * exceptions which occur before I/O is set up.
     */
    ioGlobalStdSet (STD_IN,  ERROR);
    ioGlobalStdSet (STD_OUT, ERROR);
    ioGlobalStdSet (STD_ERR, ERROR);
#endif  /* CPU_FAMILY == PPC */

    sysStartType = startType;
    intVecBaseSet ((FUNCPTR *) VEC_BASE_ADRS);

#if (CPU_FAMILY == PPC) && defined(INCLUDE_EXC_SHOW)
    /*
     * Do this ahead of excVecInit() to set up _func_excPanicHook, in case
     * the enabling of Machine Check there allows a pending one to occur.
     * excShowInit() will be called again later, harmlessly.
     */
    excShowInit ();
#endif  /* CPU_FAMILY == PPC && defined(INCLUDE_EXC_SHOW) */
    }

