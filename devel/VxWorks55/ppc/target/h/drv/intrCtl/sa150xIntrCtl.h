/* sa150xIntrCtl.h - ARM 1500/1501 interrupt controller */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,23sep98,cdp  written.
*/

#ifndef	__INCsa150xIntrCtlh
#define	__INCsa150xIntrCtlh

#ifdef __cplusplus
extern "C" {
#endif


#define SA150X_INT_REG_ADDR_INTERVAL 16
#ifdef _ASMLANGUAGE
#define SA150X_INT_ADRS(base,reg)	((base) + \
					 (reg)*SA150X_INT_REG_ADDR_INTERVAL)
#else
#define SA150X_INT_ADRS(base,reg)	((UINT32)(base) + \
					 (reg)*SA150X_INT_REG_ADDR_INTERVAL)
#endif

/* interrupt controller registers */

#define SA150X_INT_DESTINATION(base)	SA150X_INT_ADRS(base,0)
#define SA150X_INT_SPECIAL(base)	SA150X_INT_ADRS(base,1)
#define SA150X_INT_ENABLE(base)		SA150X_INT_ADRS(base,2)
#define SA150X_INT_POLARITY(base)	SA150X_INT_ADRS(base,3)
#define SA150X_INT_IRQ_REQUEST(base)	SA150X_INT_ADRS(base,4)
#define SA150X_INT_FIQ_REQUEST(base)	SA150X_INT_ADRS(base,5)
#define SA150X_INT_STATUS(base)		SA150X_INT_ADRS(base,6)
#define SA150X_INT_CLEAR(base)		SA150X_INT_STATUS(base)
#define SA150X_INT_SOURCE(base)		SA150X_INT_ADRS(base,7)
#define SA150X_INT_SET(base)		SA150X_INT_SOURCE(base)


#ifndef _ASMLANGUAGE

/* hardware access methods */

#ifndef SA150X_INT_REG_READ
#define SA150X_INT_REG_READ(a,val) ((val) = *(volatile UINT32 *)(a))
#endif

#ifndef SA150X_INT_REG_WRITE
#define SA150X_INT_REG_WRITE(a,val) (*(volatile UINT32 *)(a) = (val))
#endif

#if defined(SA150X_INT_RETRY_WRITES) && \
   !defined(SA150X_INT_REG_WRITE_RETRY)
#define SA150X_INT_REG_WRITE_RETRY(reg,val)	\
    {						\
    UINT32 _rVal, _wVal;			\
    _wVal = (val);				\
    do						\
	{					\
	SA150X_INT_REG_WRITE ((reg), _wVal);	\
	SA150X_INT_REG_READ ((reg), _rVal);	\
	}					\
    while (_rVal != _wVal);			\
    }
#endif

#ifndef	SA150X_INT_REQ_REG_READ
#ifdef	SA150X_INT_RETRY_READS
#define SA150X_INT_REQ_REG_READ(reg,val,mask,tries,ok)	\
    {						\
    int _i;					\
    for (_i = 0; _i < (tries); ++_i)		\
	{					\
	SA150X_INT_REG_READ ((reg), (val));	\
	if (((val) & (mask)) == 0)		\
	    break;				\
	}					\
    (ok) = _i < tries;				\
    }
#else
#define	SA150X_INT_REQ_REG_READ(reg,val,mask,tries,ok)	\
	SA150X_INT_REG_READ ((reg), (val));
#endif
#endif


/* structure describing an interrupt controller */

typedef struct
    {
    /*
     * The following values should be initialised in the BSP before
     * the interrupt driver is initialised.
     */

    volatile UINT32 *	base;			/* base of registers */
#ifdef SA150X_INT_HANDLE_1501_DESTINATION
    UINT32		destModifier;		/* EOR with destination reg */
#endif

    /*
     * The following value should be initialised in the BSP to the FIQ
     * bits enabled prior to the call to sa150xIntDevInit()  (e.g. by
     * sysInit) or 0 if no FIQ sources are enabled until after that
     * call.  Calls to sa150xIntFiqEnable/Disable will change bits in
     * this word.  It should not be changed by any other means.
     */

    UINT32		enabledFiqs;		/* FIQs currently enabled */

    /*
     * The following values are reserved for internal use by the
     * interrupt driver and should not be changed by anything else.
     * They do not require initialisation by the BSP.
     */

    UINT32		enabledIrqs;		/* IRQs enabled by intEnable */
#ifdef SA150X_INT_CACHE_IRQ_REQUEST
    UINT32		irqRequests;		/* cached current requests */
    int			requestsValid;		/* is irqRequests valid */
#endif
    } sa150xIntrCtlDetails;


/* interrupt feature bits for configuration */

#define SA150X_INT_POLARITY_NEGATIVE	(0 << 0)
#define	SA150X_INT_POLARITY_POSITIVE	(1 << 0)
#define SA150X_INT_TYPE_EDGE		(0 << 1)
#define SA150X_INT_TYPE_LEVEL		(1 << 1)
#define	SA150X_INT_DESTINATION_IRQ	(0 << 2)
#define	SA150X_INT_DESTINATION_FIQ	(1 << 2)


IMPORT STATUS sa150xIntLvlConfigure (int level, int features);


/* macro to convert level number to vector number */

#ifndef SA150X_INT_LVL_VEC_MAP
#   define SA150X_INT_LVL_VEC_MAP(level, vector) \
	    ((vector) = ((level) + SA150X_INT_VEC_BASE))
#endif


#endif	/* _ASMLANGUAGE */


/* debug macro */

#ifndef SA150X_INT_DEBUG
#define	SA150X_INT_DEBUG(a,b)
#endif

#ifdef __cplusplus
}
#endif

#endif	/* __INCsa150xIntrCtlh */
