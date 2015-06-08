/* usrWdbTaskAltivec.c - optional altivec support for the task WDB agent */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,15apr,kab  created
*/

/*
DESCRIPTION

This library configures the task mode WDB agent's altivec support.
*/

/* externals */

extern FUNCPTR	wdbTaskAvRegsSet;
extern FUNCPTR	wdbTaskAvRegsGet;
extern BOOL     wdbTgtHasAltivec (void);

/* forward static declarations */

static STATUS	_wdbTaskAvRegsSet (WDB_CTX * pContext, char * pRegSet);
static STATUS	_wdbTaskAvRegsGet (WDB_CTX * pContext, char ** ppRegSet);

/******************************************************************************
*
* _wdbTaskAvRegsSet - set a tasks altivec regs
*/

static STATUS _wdbTaskAvRegsSet
    (
    WDB_CTX *    pContext,
    char *       pRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (ALTIVEC_CONTEXT_GET(pTcb) == NULL))
        return (ERROR);

    bcopy (pRegSet, (char *)ALTIVEC_CONTEXT_GET(pTcb), 
			     sizeof (ALTIVEC_CONTEXT));
    return (OK);
    }

/******************************************************************************
*
* _wdbTaskAvRegsGet - Get a tasks altivec regs
*/

static STATUS _wdbTaskAvRegsGet
    (
    WDB_CTX *           pContext,
    char **             ppRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (ALTIVEC_CONTEXT_GET(pTcb) == NULL))
        return (ERROR);

    *ppRegSet = (char *)ALTIVEC_CONTEXT_GET(pTcb);
    return (OK);
    }

/******************************************************************************
*
* wdbTaskAltivecInit - initialize altivec support
*
* This routine initializes altivec support for the task
* mode agent.
*
* NOMANUAL
*/

void wdbTaskAltivecInit (void)
    {
    /* install task mode altivec get/set routines */

    if (wdbTgtHasAltivec())
	{
	wdbTaskAvRegsSet = _wdbTaskAvRegsSet;
	wdbTaskAvRegsGet = _wdbTaskAvRegsGet;
	}
    }

