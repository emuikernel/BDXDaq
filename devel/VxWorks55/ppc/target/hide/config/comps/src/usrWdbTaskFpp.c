/* usrWdbTaskFpp.c - optional floating point support for the task WDB agent */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,22may98,dbt	 taken from 01a of usrWdbFpp.c
*/

/*
DESCRIPTION

This library configures the task mode WDB agent's hardware floating point
support.
*/

/* externals */

extern FUNCPTR	wdbTaskFpRegsSet;
extern FUNCPTR	wdbTaskFpRegsGet;
extern BOOL     wdbTgtHasFpp (void);

/* forward static declarations */

static STATUS	_wdbTaskFpRegsSet (WDB_CTX * pContext, char * pRegSet);
static STATUS	_wdbTaskFpRegsGet (WDB_CTX * pContext, char ** ppRegSet);

/******************************************************************************
*
* _wdbTaskFpRegsSet - set a tasks fpp regs
*/

static STATUS _wdbTaskFpRegsSet
    (
    WDB_CTX *    pContext,
    char *       pRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (pTcb->pFpContext == NULL))
        return (ERROR);

    bcopy (pRegSet, (char *)(pTcb->pFpContext), sizeof (FP_CONTEXT));
    return (OK);
    }

/******************************************************************************
*
* _wdbTaskFpRegsGet - Get a tasks fpp regs
*/

static STATUS _wdbTaskFpRegsGet
    (
    WDB_CTX *           pContext,
    char **             ppRegSet
    )
    {
    WIND_TCB * pTcb = taskTcb (pContext->contextId);

    if ((pTcb == NULL) || (pTcb->pFpContext == NULL))
        return (ERROR);

    *ppRegSet = (char *) pTcb->pFpContext;
    return (OK);
    }

/******************************************************************************
*
* wdbTaskFppInit - initialize hardware floating point support
*
* This routine initializes hardware floating point support for the task
* mode agent.
*
* NOMANUAL
*/

void wdbTaskFppInit (void)
    {
    /* install task mode fpp get/set routines */

    if (wdbTgtHasFpp())
	{
	wdbTaskFpRegsSet = _wdbTaskFpRegsSet;
	wdbTaskFpRegsGet = _wdbTaskFpRegsGet;
	}
    }

