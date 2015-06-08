/* usrWdbTaskDsp.c - optional DSP support for the task WDB agent */

/* Copyright 1998-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16nov00,zl	 written based on usrWdbTaskFpp.c
*/

/*
DESCRIPTION

This library configures the task mode WDB agent's DSP support.
*/

/* externals */

extern FUNCPTR	wdbTaskDspRegsSet;
extern FUNCPTR	wdbTaskDspRegsGet;
extern BOOL     wdbTgtHasDsp (void);

/* forward static declarations */

static STATUS	_wdbTaskDspRegsSet (WDB_CTX * pContext, char * pRegSet);
static STATUS	_wdbTaskDspRegsGet (WDB_CTX * pContext, char ** ppRegSet);

/******************************************************************************
*
* _wdbTaskDspRegsSet - set a tasks DSP regs
*/

static STATUS _wdbTaskDspRegsSet
    (
    WDB_CTX *    pContext,
    char *       pRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (pTcb->pDspContext == NULL))
        return (ERROR);

    bcopy (pRegSet, (char *)(pTcb->pDspContext), sizeof (DSP_CONTEXT));
    return (OK);
    }

/******************************************************************************
*
* _wdbTaskDspRegsGet - Get a tasks DSP regs
*/

static STATUS _wdbTaskDspRegsGet
    (
    WDB_CTX *           pContext,
    char **             ppRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (pTcb->pDspContext == NULL))
        return (ERROR);

    *ppRegSet = (char *) pTcb->pDspContext;
    return (OK);
    }

/******************************************************************************
*
* wdbTaskDspInit - initialize DSP support
*
* This routine initializes DSP support for the task mode agent.
*
* NOMANUAL
*/

void wdbTaskDspInit (void)
    {
    /* install task mode DSP get/set routines */

    if (wdbTgtHasDsp())
	{
	wdbTaskDspRegsSet = _wdbTaskDspRegsSet;
	wdbTaskDspRegsGet = _wdbTaskDspRegsGet;
	}
    }

