/* coldfireTimer.h - ColdFire Timer header */

/*
modification history
--------------------
01b,03apr01,dh  support for 5272 added (allows external definition of TER type)
01b,19oct01,dee  Merge from T2.1.0 release
01a,13apr98,mem written
*/

#ifndef	__INCcoldfireTimerh
#define __INCcoldfireTimerh	

/*
This file contains constants and typedefs for the timer contained in several
of the Motorola ColdFire ports.
*/

#ifdef __cplusplus
extern "C" {
#endif

/* To change the size of the TER register, define the DEFINED_cfTer_t macro
   and supply a typedef for cfTer_t.
*/
#ifndef DEFINED_cfTer_t
#define DEFINED_cfTer_t
typedef UINT8 cfTer_t;
#endif

/* defines */

#define COLDFIRE_TMR_CE_DIS	(0 << 6)	/* disable int on capture */
#define COLDFIRE_TMR_CE_RISE	(1 << 6)	/* capture on rising edge */
#define COLDFIRE_TMR_CE_FALL	(2 << 6)	/* capture on falling edge */
#define COLDFIRE_TMR_CE_ANY	(3 << 6)	/* capture on any edge */
#define COLDFIRE_TMR_OM		(1 << 5)	/* output mode */
#define COLDFIRE_TMR_ORI	(1 << 4)	/* output ref int enable */
#define COLDFIRE_TMR_FRR	(1 << 3)	/* free run/restart */
#define COLDFIRE_TMR_CLK_STOP	(0 << 1)	/* stop count */
#define COLDFIRE_TMR_CLK_BUS	(1 << 1)	/* system bus clock */
#define COLDFIRE_TMR_CLK_BUS16	(2 << 1)	/* system bus clock / 16 */
#define COLDFIRE_TMR_CLK_TIN	(3 << 1)	/* TIN pin */
#define COLDFIRE_TMR_EN		(1 << 0)	/* enable (reset) timer */

#define COLDFIRE_TER_REF	(1 << 1)	/* output reference event */
#define COLDFIRE_TER_CAP	(1 << 0)	/* capture event */

/* typedefs */

typedef struct
    {
    volatile UINT16	*tmr;		/* timer mode register */
    volatile cfTer_t	*ter;		/* timer event register */
    volatile UINT16	*tcn;		/* timer counter register */
    volatile UINT16	*tcr;		/* timer capture register */
    volatile UINT16	*trr;		/* timer reference register */
    volatile void	*imr;		/* interrupt mask register */

    int			imrSize;	/* size of IMR in bytes */
    ULONG		imrMask;	/* mask bit in IMR register */
    } COLDFIRE_TIMER;

#ifdef __cplusplus
}
#endif

#endif	/* __INCcoldfireTimerh */
