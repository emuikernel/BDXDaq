/* ivPpc.h - PowerPC interrupt vectors */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,09dec96,tpr  changed _func_intEnableRtn() & _func_intDisableRtn() return
		 type from void to int.
01d,17jun96,tpr  added _func_intXXX prototype.
01c,17jun96,tam  used "INUM_TO_IVEC" instead of "IVEC_TO_INUM" for the PPC403
		 interrupt vector definitions (spr #6017).
01b,02feb95,yao  added vector definition for PPC403.
01a,29may87,yao  written.
*/

#ifndef __INCivPpch
#define __INCivPpch

#ifdef __cplusplus
extern "C" {
#endif

/* macros to convert exception vectors <-> exception numbers */

#define EVEC_TO_ENUM(excVec)	((int)(excVec))
#define ENUM_TO_EVEC(excNum)	((VOIDFUNCPTR *) (excNum))

/* macros to convert interrupt vectors <-> interrupt numbers */

#define IVEC_TO_INUM(intVec)	((int) (intVec))
#define INUM_TO_IVEC(intNum)	((VOIDFUNCPTR *) (intNum))

/* interrupt vector definitions */

#if	(CPU==PPC403)
#define	_IV_403_CI	INUM_TO_IVEC(0)		/* critical interrupt */
#define	_IV_403_SRI	INUM_TO_IVEC(4)		/* serial port receiver int. */
#define	_IV_403_STI	INUM_TO_IVEC(5)		/* serial port transmiter int.*/
#define	_IV_403_JRI	INUM_TO_IVEC(6)		/* jtag receiver interrupt */
#define	_IV_403_JTI	INUM_TO_IVEC(7)		/* jtag transmiter interrupt */
#define	_IV_403_D0I	INUM_TO_IVEC(8)		/* dma channel 0 interrupt */
#define	_IV_403_D1I	INUM_TO_IVEC(9)		/* dma channel 1 interrupt */
#define	_IV_403_D2I	INUM_TO_IVEC(10)	/* dma channel 2 interrupt */
#define	_IV_403_D3I	INUM_TO_IVEC(11)	/* dma channel 3 interrupt */
#define	_IV_403_E0I	INUM_TO_IVEC(27)	/* external interrupt 0 */
#define	_IV_403_E1I	INUM_TO_IVEC(28)	/* external interrupt 1 */
#define	_IV_403_E2I	INUM_TO_IVEC(29)	/* external interrupt 2 */
#define	_IV_403_E3I	INUM_TO_IVEC(30)	/* external interrupt 3 */
#define	_IV_403_E4I	INUM_TO_IVEC(31)	/* external interrupt 4 */
#endif	/* CPU==PPC403 */

#ifndef _ASMLANGUAGE

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS	(* _func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT void	(* _func_intVecSetRtn) (FUNCPTR *, FUNCPTR );
IMPORT FUNCPTR	(* _func_intVecGetRtn) (FUNCPTR *);
IMPORT void	(* _func_intVecBaseSetRtn) (FUNCPTR *);
IMPORT FUNCPTR *(* _func_intVecBaseGetRtn) (void);
IMPORT int	(* _func_intLevelSetRtn) (int);
IMPORT int	(* _func_intEnableRtn) (int);
IMPORT int	(* _func_intDisableRtn) (int);

#else   /* __STDC__ */

IMPORT STATUS	(* _func_intConnectRtn) ();
IMPORT void	(* _func_intVecSetRtn) ();
IMPORT FUNCPTR	(* _func_intVecGetRtn) ();
IMPORT void	(* _func_intVecBaseSetRtn) ();
IMPORT FUNCPTR *(* _func_intVecBaseGetRtn) ();
IMPORT int	(* _func_intLevelSetRtn) ();
IMPORT int	(* _func_intEnableRtn) ();
IMPORT int	(* _func_intDisableRtn) ();

#endif  /* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCivPpch */
