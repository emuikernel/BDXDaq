/* flagutil.h - flag parsing utility library */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,10jul98,c_c  Doc fix.
01d,15jun98,c_c  Changed the flag_desc definition to be more friendly.
01c,09jun98,c_c  Doc update.
01b,22mar95,c_s  changed interfaces for new behavior.
01a,05jan95,jcf  written.
*/

#ifndef __INCflagutilh
#define __INCflagutilh

#ifdef __cplusplus
extern "C" {
#endif


/* types */

/* flag processing routine definition */

typedef int (*PARSE_RTN) (int argc, char **argv, void * outputPtr);

typedef struct flag_desc
    {
    char *		flagName;	/* verbose flag name */
    char *		flagTerseName;	/* abbreviated name of flag (or NULL) */
    PARSE_RTN		parseRoutine;	/* flag processing routine */
    void *  		outputPtr;	/* where to store the output result */
    char * 		flagHelp;	/* flag help string */
    } FLAG_DESC;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	flagParse (FLAG_DESC flagTbl[], int nFlags, int *pArgc,
			   char *argv []);
extern void	flagHelp (FLAG_DESC flagTbl[], int nFlags);
extern int	flagBool (int argc, char *argv[], BOOL *pBool);
extern int	flagName (int argc, char *argv[], char **pName);
extern int	flagInt (int argc, char *argv[], int *pInt);

#else	/* __STDC__ */

extern void	flagParse ();
extern void	flagHelp ();
extern int	flagBool ();
extern int	flagName ();
extern int	flagInt ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCflagutilh */
