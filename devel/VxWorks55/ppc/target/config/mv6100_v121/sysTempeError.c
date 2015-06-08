/* sysTempeError.c - Catch and log Tempe (Tsi148) asserted errors */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,03aug04,scb Initial writing.
*/

/*
DESCRIPTION:

This module implements error handling for the Tempe (Tsi148) PCI-VME
bridge chip.  The errors addressed in this module are those involving
the VEAT (VMEbus Exception Attributes) register and the EDPAT (Error
Diagnostic PCI Attributes) register.  Interaction with the generic
error handling module in "sysErrorHandler.c" is performed here.
Error initialization for the Tsi148 error domain is done,  several
Tsi148 errors are registered and the scanning, clearing and error
message formatting functions are supplied.  Functions which connect
interrupt handlers for these two classes (VEAT and EDPAT) are 
provided so the errors in these two registers can be monitored on an
interrupt basis.

*/

/* includes */

#include "vxWorks.h"
#include "errorHandler.h"
#include "tempe.h"

/* defines */

#define VEAT_STR  " veat(@268)=%08x, veal(@264) = %08x"
#define EDPAT_STR "\n\r               edpat(@280)=%08x, edpal(@274)=%08x\n\r"\
		      "               edpxa(@278)=%08x, edpxs(@27c)=%08x"

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

void sysTempeErrorInit (void);
STATUS sysTempeErrIntInit (void);

LOCAL void sysTempeErrorRegister (void);
LOCAL char * tempeErrorFormat (ERROR_ID id);
LOCAL BOOL tempeErrorScan (ERROR_ID id);
LOCAL BOOL tempeErrorClear (ERROR_ID id);
LOCAL void sysTempeVeatInt (int param);
LOCAL void sysTempeEdpatInt (int param);

/* externals */

/******************************************************************************
*
* sysTempeErrorInit - Initialize error monitoring for Tempe (Tsi148)
*
* This routine will initialize the error domain for the Tsi148 (Tempe)
* PCI-VME bridge chip.  
*
* RETURNS: NA
*/

void sysTempeErrorInit (void)
    {
    ERROR_USR_FUNCS myErrorFuncs = { tempeErrorFormat,
                                     tempeErrorScan,
                                     tempeErrorClear
                                   };

    if (tempeErrorUtils.errorLog != NULL)
 	 return;		/* Already been through here */
    
    /* Initialize Tempe (Tsi148) domain */

    if ( _func_errorHandlerInit == NULL)
	return;			/* No error handler module is linked */

    tempeErrorUtils = (* _func_errorHandlerInit)
				(TEMPE_ERROR_DOMAIN_CODE,
				 TEMPE_ERROR_ASCII_DOMAIN,
				 &myErrorFuncs);
				 
    if (tempeErrorUtils.errorLog == NULL)
 	 return;		/* Error handler module is dead */

    /*
     * "Tempe (Tsi148) error domain is now initialized.  We can now
     *	register the errors that we will be monitoring
     */

    sysTempeErrorRegister ();
    }

/******************************************************************************
*
* sysTempeErrIntInit - Enable Tempe (Tsi148) error interrupts
*
* This routine deals with the VMEbus Exception Attributes Register and
* the Error Diagnostic PCI Attributes Register.
* It connects an error interrupt service routine (if one has not already
* been connected) and enables the VERR (VME Bus) and PERR (PCI Bus)
* interrupts.  This function is safe to call multiple times.  In fact,
* if the Tempe (Tsi148) asserts either a VMEbus error or PCI bus error, 
* with associated interrupt, that interrupt will be disabled.  Calling this 
* function will reenable the interrupts.
*
* RETURNS: ERROR if intConnect or intEnable returned ERROR, OK otherwise.
*/

STATUS sysTempeErrIntInit (void)
    {
    static BOOL tempeVerrIntInitDone = FALSE;
    static BOOL tempePerrIntInitDone = FALSE;

    /* Connect an interrupt handler if none has already been connected */

    if (!tempeVerrIntInitDone)
	if (intConnect (INUM_TO_IVEC (TEMPE_INT_NUM_VERR),
		       sysTempeVeatInt, 0) == ERROR)
            {
            return (ERROR);
            }

    tempeVerrIntInitDone = TRUE;

    /* Enable the interrupt */

    if (intEnable (TEMPE_INT_NUM_VERR) != OK)
	return (ERROR);

    if (!tempePerrIntInitDone)
	if (intConnect (INUM_TO_IVEC (TEMPE_INT_NUM_PERR),
		       sysTempeEdpatInt, 0) == ERROR)
            {
            return (ERROR);
            }

    tempePerrIntInitDone = TRUE;

    /* Enable the interrupt */

    if (intEnable (TEMPE_INT_NUM_PERR) != OK)
	return (ERROR);

    return (OK);
    }

/******************************************************************************
*
* sysTempeErrorRegister - Register errors for Tempe (Tsi48)
*
* This routine will register errors associated with the Tempe (Tsi148)
* VEAT "VME Exception Attributes" register (offset 0x268) and those 
* associated with the EDPAT "Error Diagnostic PCI Attributes" register
* (offset 0x280).
*
* RETURNS: NA
*/

LOCAL void sysTempeErrorRegister (void)
    {

    /*
     * Register the VME exception attributes register errors.  
     * Start with VME bus error.
     */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_VEAT_BERR, ERROR_DISP_DEFAULT, 0, 0);

    /* Register VME 2e odd termination error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_VEAT_2EOT, ERROR_DISP_DEFAULT, 0, 0);

    /* Register VME 2e slave termination error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_VEAT_2EST, ERROR_DISP_DEFAULT, 0, 0);

    /* Register unknown VEAT VERR error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_VEAT_VERR_HUH, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Split completion discarded error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_SCD, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Unexpected split completion error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_USC, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Split Response Time-out error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_SRT, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Split completion error message error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_SCEM, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Data Parity Error Detected error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_DPED, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Detected parity error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_DPE, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Maximum retry count error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_MRC, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Received master abort error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_RMA, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Received target abort error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_RTA, ERROR_DISP_DEFAULT, 0, 0);

    /* Register EDPAT Delayed transaction timeout error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_DTT, ERROR_DISP_DEFAULT, 0, 0);

    /* Register unknown EDPAT error */

    TEMPE_ERROR_REGISTER (TEMPE_ERR_EDPAT_PERR_HUH, ERROR_DISP_DEFAULT, 0, 0);

    }

/******************************************************************************
*
* tempeErrorFormat - Format a message for error handling module
*
* This is the standard "error formatting" helper function for the error
* handling module.  When called from the error handling module, it will 
* return an error format string (or NULL if one does not exist) associated
* with the requested error ID.
*
* RETURNS: Error formatting string or NULL if one does not exit.
*/

LOCAL char * tempeErrorFormat
    (
    ERROR_ID id
    )
    {
    switch (id)
	{
	case TEMPE_ERR_VEAT_BERR:
	    return ("VME Bus Error:" VEAT_STR);

	case TEMPE_ERR_VEAT_2EOT:
	    return ("VME 2e Odd Term:" VEAT_STR);

	case TEMPE_ERR_VEAT_2EST:
	    return ("VME 2e Slave Term:" VEAT_STR);

	case TEMPE_ERR_VEAT_VERR_HUH:
	    return ("VME Unknown VEAT VERR: veatReg(@268) = 0x%08x");

        case TEMPE_ERR_EDPAT_SCD:
	    return ("PCI split completion discard:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_USC:
	    return ("PCI unexpected split completion:" EDPAT_STR); 

        case TEMPE_ERR_EDPAT_SRT:
	    return ("PCI Split response timeout:" EDPAT_STR); 

        case TEMPE_ERR_EDPAT_SCEM:
	    return ("PCI Split completion error message:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_DPED:
	    return ("PCI Data parity error detected:" EDPAT_STR); 

        case TEMPE_ERR_EDPAT_DPE:
	    return ("PCI Data parity error:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_MRC:
	    return ("PCI Maximum retry count exceeded:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_RMA:
	    return ("PCI Received master abort:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_RTA:
	    return ("PCI Received target abort:" EDPAT_STR);

        case TEMPE_ERR_EDPAT_DTT:
	    return ("PCI Delayed transaction timeout:" EDPAT_STR);

	case TEMPE_ERR_EDPAT_PERR_HUH:
	    return ("VME Unknown EDPAT PERR: edpat(@280) = 0x%08x");

	default:
	    return (NULL);
	}
    }

/******************************************************************************
*
* tempeErrorScan - Scan for an error being asserted by Tempe (Tsi148)
*
* This routine will scan for an error associated with the Tempe (Tsi148)
* VEAT "VME Exception Attributes" register (offset 0x268) or the  EDPAT 
* "Error Diagnostic PCI Attributes" register (offset 0x280).  The particular 
* error for which the scan is done is identified by an error ID parameter.
* If the error being scanned is indeed asserted, the generic error handling
* error logging fuction is called.
*
* RETURNS: TRUE if error is asserted, FALSE otherwise
*/

LOCAL BOOL tempeErrorScan
    (
    ERROR_ID id		/* ID of error for which to scan */
    )
    {
    BOOL ret = FALSE;
    UINT32 mask;
    ERROR_MSG myErrorMsg;

    myErrorMsg.id = id;

    if ((id & 0x00000fff) == TEMPE_VEAT)
	{
	myErrorMsg.params[0] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VEAT);
	myErrorMsg.params[1] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VEAL);
	}
    else if ((id & 0x00000fff) == TEMPE_EDPAT) 
	{
	myErrorMsg.params[0] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_EDPAT);
	myErrorMsg.params[1] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_EDPAL);
	myErrorMsg.params[2] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_EDPXA);
	myErrorMsg.params[3] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_EDPXS);
	}
    else
	return (FALSE);		/* Bad error ID */

    switch (id)
	{

	/* VME VEAT Error */

	case TEMPE_ERR_VEAT_BERR:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_BERR_MASK);
	    break;

	case TEMPE_ERR_VEAT_2EOT:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_2EOT_MASK); 
	    break;

	case TEMPE_ERR_VEAT_2EST:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_2EST_MASK); 
	    break;

	/* PCI EPDAT Error */

	case TEMPE_ERR_EDPAT_SCD:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SCD_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_USC:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_USC_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_SRT:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SRT_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_SCEM:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SCEM_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_DPED:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DPED_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_DPE:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DPE_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_MRC:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_MRC_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_RMA:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_RMA_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_RTA:
	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_RTA_MASK); 
	    break;

	case TEMPE_ERR_EDPAT_DTT:

	    mask = (TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DTT_MASK); 
	    break;

	default:
	    return (FALSE);
	}

        if ((myErrorMsg.params[0] & mask) == mask)
	    {
	    ret = TRUE;
	    TEMPE_ERROR_LOG (&myErrorMsg);
	    }

    return (ret);
    }

/******************************************************************************
*
* tempeErrorClear - Clear an error being asserted by Tempe (Tsi148)
*
* This routine will clear an error associated with the Tempe (Tsi148)
* VEAT "VME Exception Attributes" register (offset 0x268) or the  EDPAT 
* "Error Diagnostic PCI Attributes" register (offset 0x280).  The particular 
* error for which is cleared is identified by an error ID parameter.
*
* RETURNS: TRUE if error state is changed from "asserted" to "not asserted",
* FALSE otherwise.
*/

LOCAL BOOL tempeErrorClear
    (
    ERROR_ID id		/* ID of error to clear */
    )
    {
    BOOL ret = FALSE;
    UINT32 reg;
    UINT32 regOffset;
    UINT32 regClearMask;
    UINT32 mask;

    if ((id & 0x00000fff) == TEMPE_VEAT)
	{
	regOffset = TEMPE_VEAT;
	regClearMask = TEMPE_VEAT_VESCL_MASK;
	}
    else if ((id & 0x00000fff) == TEMPE_EDPAT) 
	{
	regOffset = TEMPE_EDPAT;
	regClearMask = TEMPE_EDPAT_EDPCL_MASK;
	}
    else
	return (FALSE);		/* Bad error ID */

    switch (id)
	{

	/* VME VEAT Error */

	case TEMPE_ERR_VEAT_BERR:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_BERR_MASK);
	    break;

	case TEMPE_ERR_VEAT_2EOT:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_2EOT_MASK); 
	    break;

	case TEMPE_ERR_VEAT_2EST:
	    mask = (TEMPE_VEAT_VES_MASK | TEMPE_VEAT_2EST_MASK); 
	    break;

	/* PCI EPDAT Error */

	case TEMPE_ERR_EDPAT_SCD:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SCD_MASK;
	    break;

	case TEMPE_ERR_EDPAT_USC:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_USC_MASK;
	    break;

	case TEMPE_ERR_EDPAT_SRT:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SRT_MASK;
	    break;

	case TEMPE_ERR_EDPAT_SCEM:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_SCEM_MASK;
	    break;

	case TEMPE_ERR_EDPAT_DPED:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DPED_MASK;
	    break;

	case TEMPE_ERR_EDPAT_DPE:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DPE_MASK;
	    break;

	case TEMPE_ERR_EDPAT_MRC:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_MRC_MASK;
	    break;

	case TEMPE_ERR_EDPAT_RMA:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_RMA_MASK;
	    break;

	case TEMPE_ERR_EDPAT_RTA:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_RTA_MASK;
	    break;

	case TEMPE_ERR_EDPAT_DTT:
	    mask = TEMPE_EDPAT_EDPST_MASK | TEMPE_EDPAT_DTT_MASK;
	    break;

	default:
	    return (FALSE);
	}

        reg = TEMPE_READ32 (TEMPE_REG_BASE, regOffset);

        if ((reg & mask) == mask)
	    {
	    ret = TRUE;
	    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, regOffset, regClearMask);
	    }

    return (ret);
    }

/******************************************************************************
*
* sysTempeVeatInt - Interrupt handling routine for Tempe VEAT errors
*
* This function is invoked when VEAT "VME Exception Attributes" register
* related errors are enabled and an actual VEAT error occurs.  It will 
* call the error scanning function to do a scan of the Tempe VEAT register 
* to verify that an associated error actually occurred.  If the error indeed 
* occurred, the scanning function will log the error.
*
* RETURNS: NA
*/

LOCAL void sysTempeVeatInt
    (
    int	param ARG_UNUSED
    )
    {
    BOOL errorAsserted = FALSE;
    ERROR_MSG myErrorMsg;

    if (tempeErrorScan (TEMPE_ERR_VEAT_BERR) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_VEAT_2EOT) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_VEAT_2EOT) == TRUE)
	errorAsserted = TRUE;

    if (errorAsserted == FALSE)
	{

	/*
	 * Error type for which we were called does not now appear to
	 * be asserted - Why are we here then?  We'll log a huh? error
	 * to commemorate this event.
	 */

	myErrorMsg.id = TEMPE_ERR_VEAT_VERR_HUH;
	myErrorMsg.params[0] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VEAT);

        TEMPE_ERROR_LOG (&myErrorMsg);
	}

    /*
     * The scanning function should have logged the error so now
     * we can disable the associated interrupt to avoid getting
     * flooded with like interrupts.
     */

    (void) intDisable (TEMPE_INT_NUM_VERR);

    }

/******************************************************************************
*
* sysTempeEdpatInt - Interrupt handling routine for Tempe EDPAT errors
*
* This function is invoked when EDPAT "Error Diagnostic PCI Attributes"
* register related errors are enabled and an actual EDPAT error occurs.  It 
* will call the error scanning function to do a scan of the Tempe EDPAT 
* register to verify that an associated error actually occurred.  If the 
* error indeed occurred, the scanning function will log the error.
*
* RETURNS: NA
*/

LOCAL void sysTempeEdpatInt
    (
    int	param ARG_UNUSED
    )
    {
    BOOL errorAsserted = FALSE;
    ERROR_MSG myErrorMsg;

    if (tempeErrorScan (TEMPE_ERR_EDPAT_SCD) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_USC) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_SRT) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_SCEM) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_DPED) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_DPE) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_MRC) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_RMA) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_RTA) == TRUE)
	errorAsserted = TRUE;
    if (tempeErrorScan (TEMPE_ERR_EDPAT_DTT) == TRUE)
	errorAsserted = TRUE;

    if (errorAsserted == FALSE)
	{

	/*
	 * Error type for which we were called does not now appear to
	 * be asserted - Why are we here then?  We'll log a huh? error
	 * to commemorate this event.
	 */

	myErrorMsg.id = TEMPE_ERR_EDPAT_PERR_HUH;
	myErrorMsg.params[0] = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_EDPAT);

        TEMPE_ERROR_LOG (&myErrorMsg);
	}

    /*
     * The scanning function should have logged the error so now
     * we can disable the associated interrupt to avoid getting
     * flooded with like interrupts.
     */

    (void) intDisable (TEMPE_INT_NUM_PERR);

    }
